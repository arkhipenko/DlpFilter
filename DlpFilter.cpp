#include <DlpFilter.h>


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
	
	accel = ADLP_ABS(accel);
	ADLP_CLAMP(accel, iAccelMin, iAccelMax);
	K = iKMax + ((iKMin-iKMax)*(accel-iAccelMin))/(iAccelMax-iAccelMin);
	ADLP_CLAMP(K, iKMin, iKMax);
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