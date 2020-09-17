/**
 
  Digital implementation of the low-pass filter for Arduino
   
  Changelog:
  v1.0.0:
    2015-02-24 - Initial release 
    
  v1.0.1:
    2016-01-05 - Switched to arduino std constrain() and abs() methods
    
  v1.0.2:
    2020-09-14 - added initialize() method

============================================
Digital implementation of the low-pass filter code is based on the following
article: ADAPTIVE IIR FILTER USING FIXED POINT INTEGER ARITHMETIC
by Hari Nair published on March 23, 2013 

Original text of the article is located here:
http://www.dsprelated.com/showcode/326.php

Copyright (c) 2015 Anatoli Arkhipenko

*/

#include <Arduino.h>

#ifndef _DLPFILTER_H
#define _DLPFILTER_H

class dlpFilter {

    public: 
        dlpFilter(long aInitValue=0, byte aK=192);
        
        long        value(long aSample);
        void        initialize(long aInitValue=0);
        inline long currentValue() { return (iShiftedFilter>>8); };

    public:
        byte    iK;
        long    iInitValue;
        
    private:
        long    iShiftedFilter;
};


class adlpFilter {

    public: 
        adlpFilter(long aInitValue, int aK, int aKMin, int aKMax, long aAccelMin, long aAccelMax, bool aPredictive=true);
        long        value(long aSample);
        inline long currentValue() { return (iShiftedFilter>>8); };
        inline byte K(void) { return iK; };
        void        initialize(long aInitValue=0);
        
    private:
        long    iInitValue;
        long    iShiftedFilter;
        long    iLastSample;
        int     iK;
        int     iKInitValue;
        int     iKMin;
        int     iKMax;
        long    iAccelMin;
        long    iAccelMax;
        long    iAccel;
        bool    iPredictive;
};


dlpFilter::dlpFilter(long aInitValue, byte aK) {
    iInitValue = aInitValue;
    iShiftedFilter = aInitValue << 8;
    iK = aK;
}

void dlpFilter::initialize(long aInitValue) {
    iInitValue = aInitValue;
    iShiftedFilter = aInitValue << 8;
}

long dlpFilter::value(long aSample) {
    long tmp;
    
    // tmp = gnCpsx256*gnKLpf + (gnCps<<8)*(256L-gnKLpf);
    tmp = iShiftedFilter*iK + (aSample << 8)*(256L - iK);
    iShiftedFilter = (tmp >= 0 ? ((tmp + 128L)>>8) : ((tmp - 128L)>>8));
    return (iShiftedFilter>>8);
}

adlpFilter::adlpFilter(long aInitValue, int aK, int aKMin, int aKMax, long aAccelMin, long aAccelMax, bool aPredictive) {
    iInitValue = aInitValue;
    iShiftedFilter = aInitValue << 8;
    iLastSample = aInitValue;
    iKInitValue = aK;
    iK = aK;
    iKMin = aKMin;
    iKMax = aKMax;
    iAccelMin = aAccelMin;
    iAccelMax = aAccelMax;
    iAccel = 0;
    iPredictive = aPredictive;
}

void adlpFilter::initialize(long aInitValue) {
    iK = iKInitValue;
    iInitValue = aInitValue;
    iShiftedFilter = aInitValue << 8;
    iAccel = 0;
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

#endif

