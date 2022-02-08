#ifndef WfmProcessor_H
#define	WfmProcessor_H

#include <iostream>
#include <numeric>

#include "FairLogger.h"
#include "BmnDigiContainerTemplate.h"
#include "PronyFitter.h"

class WfmProcessor{

public:

    WfmProcessor() {};
    virtual ~WfmProcessor() { if(fAZik) delete[] fAZik; };

    void ProcessWfm(std::vector<float> wfm, BmnDigiContainerTemplate* digi);

    struct digiPars {
      bool isWriteWfm;
      int gateBegin;
      int gateEnd;
      float threshold;
      int signalType;
      bool doInvert;

      bool isfit;
      std::vector<std::complex<float>> harmonics;
    } fdigiPars;

    int fSignalLength;
    std::complex<float> **fAZik = nullptr; // Inverse Harmo matrix for fit

private:
    void MeanRMScalc(std::vector<float> wfm, float* Mean, float* RMS, int begin, int end, int step = 1);
    ClassDef(WfmProcessor, 1);
};
#endif	/* WfmProcessor_H */


