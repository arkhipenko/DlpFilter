//
// Digital implementation of the low-pass filter
//

#include <Arduino.h>

#ifndef _DLPFILTER_H
#define _DLPFILTER_H

#define ADLP_ABS(x)                 ((x) < 0 ? -(x) : (x))
#define ADLP_CLAMP(x,min,max)       {if ((x) <= (min)) (x) = (min); else if ((x) >= (max)) (x) = (max);}


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

#endif

