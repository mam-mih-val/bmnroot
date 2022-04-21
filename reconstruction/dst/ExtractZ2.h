#ifndef EXTRACTZ2_H
#define EXTRACTZ2_H 1

#include "BmnTrigWaveDigit.h"
#include "TClonesArray.h"

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

// Define constants for PID
const double meanBC1_Cin = 1095;
const double meanBC2_Cin = 795;
const double sigmaBC1_Cin = 130;
const double sigmaBC2_Cin = 130;
const double sigmaMultiBC1_Cin = 2;
const double sigmaMultiBC2_Cin = 2;

void findIdx(TClonesArray *data, int &index, double refT) {
    // Function to find the waveform that is closest in time to T0 waveform
    // and returns the index where the closest waveform is in the data array.
    double minT = 1e4;
    for (int m = 0; m < data->GetEntriesFast(); m++) {
        BmnTrigWaveDigit *signal = (BmnTrigWaveDigit *)data->At(m);
        double time = fabs(signal->GetTime() - refT);
        if (time < minT) {
            minT = time;
            index = m;
        }
    }
}

void grabZ2(TClonesArray *TQDC_BC1, TClonesArray *bc2Data, double t0Time, double &z2, double &x, double BC1calib, double BC2calib, short &Z, bool isIncoming) {
    double adcBC1, adcBC2;
    int bc1Idx, bc2Idx;

    // Require that BC1 and T0 had TQDC digits
    if (TQDC_BC1->GetEntriesFast() && bc2Data->GetEntriesFast()) {
        // Find the waveform closest in time to T0 time
        findIdx(TQDC_BC1, bc1Idx, t0Time);
        BmnTrigWaveDigit *signal1 = (BmnTrigWaveDigit *)TQDC_BC1->At(bc1Idx);
        if (isIncoming)
            adcBC1 = signal1->GetPeak() - pedBC1 + BC1calib;
        else {
            adcBC1 = signal1->GetPeak() - pedBC3;
        }

        // Find the waveform closest in time to T0 time
        findIdx(bc2Data, bc2Idx, t0Time);
        BmnTrigWaveDigit *signal2 = (BmnTrigWaveDigit *)bc2Data->At(bc2Idx);
        if (isIncoming)
            adcBC2 = signal2->GetPeak() - pedBC2 + BC2calib;
        else {
            adcBC2 = signal2->GetPeak() - pedBC4;
        }

        // Take geometric mean adc
        x = sqrt(adcBC1 * adcBC2);

        // Convert to Z2:
        // Ask if this is for before target or after target
        // because the calibration constants are different for the
        // different PMTs
        if (isIncoming) {
            z2 = a_in + b_in * x + c_in * x * x;
            // Ellipse sigmaMulti cut for Carbon on adcBC2(adcBC1) plot
            if (adcBC2 > meanBC2_Cin - sigmaMultiBC2_Cin * sigmaBC2_Cin * sqrt(1 - (adcBC1 - meanBC1_Cin) * (adcBC1 - meanBC1_Cin) / (sigmaMultiBC1_Cin * sigmaBC1_Cin * sigmaMultiBC1_Cin * sigmaBC1_Cin)) && adcBC2 < meanBC2_Cin + sigmaMultiBC2_Cin * sigmaBC2_Cin * sqrt(1 - (adcBC1 - meanBC1_Cin) * (adcBC1 - meanBC1_Cin) / (sigmaMultiBC1_Cin * sigmaBC1_Cin * sigmaMultiBC1_Cin * sigmaBC1_Cin)))
                Z = 6;
        } else
            z2 = a_out + b_out * x + c_out * x * x;
    }
}

void grabZ2OR(TClonesArray *TQDC_BC1, TClonesArray *bc2Data, double t0Time, double &z2A, double &z2B, double &x, double &y, bool isIncoming) {
    double adcBC1, adcBC2;
    int bc1Idx, bc2Idx;

    // Require that BC1 and T0 had TQDC digits
    x = -100.0;
    y = -100.0;
    z2A = -100.0;
    z2B = -100.0;
    if (TQDC_BC1->GetEntriesFast()) {
        // Find the waveform closest in time to T0 time
        findIdx(TQDC_BC1, bc1Idx, t0Time);
        BmnTrigWaveDigit *signal1 = (BmnTrigWaveDigit *)TQDC_BC1->At(bc1Idx);
        if (isIncoming)
            adcBC1 = signal1->GetPeak() - pedBC1;
        else {
            adcBC1 = signal1->GetPeak() - pedBC3;
        }
        x = adcBC1;
        if (isIncoming)
            z2A = a_in + b_in * x + c_in * x * x;
        else
            z2A = a_out + b_out * x + c_out * x * x;
    }
    if (bc2Data->GetEntriesFast()) {
        findIdx(bc2Data, bc2Idx, t0Time);
        BmnTrigWaveDigit *signal2 = (BmnTrigWaveDigit *)bc2Data->At(bc2Idx);
        if (isIncoming)
            adcBC2 = signal2->GetPeak() - pedBC2;
        else {
            adcBC2 = signal2->GetPeak() - pedBC4;
        }
        y = adcBC2;
        if (isIncoming)
            z2B = a_in + b_in * y + c_in * y * y;
        else
            z2B = a_out + b_out * y + c_out * y * y;
    }
}

#endif
