#include "BmnTrigWaveDigit.h"
#include "BmnTrigDigit.h"
// Define some constants for analysis
const double pedBC1 = 69.2885;
const double pedBC2 = -11.7212;
const double pedBC3 = -25.4808;
const double pedBC4 = 126.067;
const double a_out = 0.00173144;
const double b_out = 0.0384856;
const double c_out = 0.000015362;
const double a_in = 0.020542;
const double b_in = 0.0305108;
const double c_in = 0.0000114953;

void findIdx( TClonesArray* data, int &index , double refT){
	// Function to find the waveform that is closest in time to T0 waveform
	// and returns the index where the closest waveform is in the data array.
	double minT = 1e4;
	for( int m = 0 ; m < data->GetEntriesFast() ; m++){
		BmnTrigWaveDigit * signal = (BmnTrigWaveDigit*)data->At(m);
		double time = fabs(signal->GetTime() - refT);
		if( time < minT){
			minT = time;
			index = m;
		}
	}
}

void grabZ2( TClonesArray *TQDC_BC1, TClonesArray *bc2Data, double t0Time, double &z2, double &x, bool isIncoming, float a_from_calibration, float b_from_calibration ){
	double adcBC1, adcBC2;
	int bc1Idx, bc2Idx;

	// Require that BC1 and T0 had TQDC digits
	if( TQDC_BC1->GetEntriesFast() && bc2Data->GetEntriesFast() ){
		
		// Find the waveform closest in time to T0 time
		findIdx(TQDC_BC1,bc1Idx,t0Time);
		BmnTrigWaveDigit * signal1 = (BmnTrigWaveDigit *) TQDC_BC1->At(bc1Idx);
		if( isIncoming) adcBC1 = signal1->GetPeak() - pedBC1;
		else{ adcBC1 = signal1->GetPeak() - pedBC3; }

		// Find the waveform closest in time to T0 time
		findIdx(bc2Data,bc2Idx,t0Time);
		BmnTrigWaveDigit * signal2 = (BmnTrigWaveDigit *) bc2Data->At(bc2Idx);
		if (isIncoming) adcBC2 = signal2->GetPeak() - pedBC2;
		else{ adcBC2 = signal2->GetPeak() - pedBC4; }
		
		// Take geometric mean adc
		x =  sqrt( adcBC1 * adcBC2 );

		// Convert to Z2:
		// Ask if this is for before target or after target
		// because the calibration constants are different for the
		// different PMTs
		if( isIncoming ){
		  if (a_from_calibration != 0 && b_from_calibration !=0){
		    z2 = a_from_calibration*x + b_from_calibration;
		  }else{
		    z2 = a_in + b_in*x + c_in*x*x;
		  }
		}else{
		  if (a_from_calibration != 0 && b_from_calibration !=0){
		    z2 = a_from_calibration*x + b_from_calibration;
		  }else{
		    z2 = a_out + b_out*x + c_out*x*x;
		  }
		}
	}
}
