#ifndef BMNCSCHITMAKER_H
#define BMNCSCHITMAKER_H 1

#include <iostream>
#include <fstream>
#include <sstream>

#include "Rtypes.h"
#include "TClonesArray.h"
#include "TRegexp.h"
#include "TString.h"

#include "FairTask.h"

#include "BmnProfRawTools.h"
#include "BmnProfRaw2Digit.h"
#include "BmnADCDigit.h"
#include "BmnSiBTDigit.h"

using namespace std;

class BmnProfAsic2Raw : public FairTask {
public:

    BmnProfAsic2Raw();
    BmnProfAsic2Raw(Int_t, Int_t);

    virtual ~BmnProfAsic2Raw();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();
    
    
private:
    void CopyDataToPedMap(TClonesArray* adcGem, UInt_t ev);
    UInt_t fPedEvCntr;
    UInt_t fNoiseEvCntr;
    Int_t fEvForPedestals;
    Bool_t fPedEnough;
    
    BmnProfRaw2Digit * fMapper;

    TString fInputAdcBranchName;
    TString fWorkAdcBranchName;

    TString fOutputProfBranchName;

    /** Input array of ADC signals **/
    TClonesArray* fInAdcArray;
    /** Intermediate array of ADC signals **/
    TClonesArray* fWorkAdcArray;

    /** Output array of Profilometer digits **/
    TClonesArray* fBmnProfDigitArray;

    ClassDef(BmnProfAsic2Raw, 1);
};


#endif
