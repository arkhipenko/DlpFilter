/**
 
  Digital implementation of the low-pass filter for Arduino
   
  Changelog:
  v1.0.0:
    2015-02-24 - Initial release 
	
  v1.0.1:
    2016-01-05 - Switched to arduino std constrain() and abs() methods


============================================
Digital implementation of the low-pass filter code is based on the following
article: ADAPTIVE IIR FILTER USING FIXED POINT INTEGER ARITHMETIC
by Hari Nair published on March 23, 2013 

Original text of the article is located here:
http://www.dsprelated.com/showcode/326.php

Current implementation is placed under the MIT license
Copyright (c) 2015 Anatoli Arkhipenko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include <Arduino.h>

#ifndef _DLPFILTER_H
#define _DLPFILTER_H

class dlpFilter {

	public: 
		dlpFilter(long aInitValue=0, byte aK=192);
		long 	value(long aSample);

	public:
		byte	iK;
		
	private:
		long	iShiftedFilter;
};


class adlpFilter {

	public: 
		adlpFilter(long aInitValue, int aK, int aKMin, int aKMax, long aAccelMin, long aAccelMax, bool aPredictive=true);
		long 	value(long aSample);
		byte	K(void);
		
	private:
		long	iShiftedFilter;
		long	iLastSample;
		int		iK;
		int     iKMin;
		int 	iKMax;
		long	iAccelMin;
		long	iAccelMax;
		long	iAccel;
		bool iPredictive;
};


dlpFilter::dlpFilter(long aInitValue, byte aK) {
	iShiftedFilter = aInitValue << 8;
	iK = aK;
}

long dlpFilter::value(long aSample) {
	long tmp;
	
	// tmp = gnCpsx256*gnKLpf + (gnCps<<8)*(256L-gnKLpf);
	tmp = iShiftedFilter*iK + (aSample << 8)*(256L - iK);
	iShiftedFilter = (tmp >= 0 ? ((tmp + 128L)>>8) : ((tmp - 128L)>>8));
	return (iShiftedFilter>>8);
}

adlpFilter::adlpFilter(long aInitValue, int aK, int aKMin, int aKMax, long aAccelMin, long aAccelMax, bool aPredictive) {
	iShiftedFilter = aInitValue << 8;
	iLastSample = aInitValue;
	iK = aK;
	iKMin = aKMin;
	iKMax = aKMax;
	iAccelMin = aAccelMin;
	iAccelMax = aAccelMax;
	iAccel = 0;
	iPredictive = aPredictive;
}

long adlpFilter::value(long aSample) {
	long tmp;
	long accel = aSample - iLastSample;
	byte K = iK;
	
	accel = abs(accel);
	accel = constrain(accel, iAccelMin, iAccelMax);
	K = iKMax + ((iKMin-iKMax)*(accel-iAccelMin))/(iAccelMax-iAccelMin);
	K = constrain(K, iKMin, iKMax);
	if (iPredictive) iK = K;
	
	tmp = iShiftedFilter*iK + (aSample << 8)*(256L - iK);
	iShiftedFilter = (tmp >= 0 ? ((tmp + 128L)>>8) : ((tmp - 128L)>>8)); 
	iLastSample = iShiftedFilter>>8;
	iK = K;
	return (iLastSample);
}

byte adlpFilter::K(void) {
	return iK;
}

#endif

