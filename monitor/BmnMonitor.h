#ifndef BMNMONITOR_H
#define BMNMONITOR_H 1

#include <iostream>

#include <TNamed.h>
#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TFolder.h"
#include "THttpServer.h"

#include <BmnTrigDigit.h>
#include <BmnTof1Digit.h>
#include <BmnGemStripDigit.h>
#include <BmnDchDigit.h>
#include "BmnDataReceiver.h"
#include "BmnHistGem.h"
#include <BmnRawDataDecoder.h>

#define BD_CHANNELS        40

using namespace std;
using namespace TMath;

class BmnMonitor : public TNamed {
public:
    
    BmnMonitor();
    virtual ~BmnMonitor();
    void ProcessRun(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root");
    void ProcessStreamRun(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root");
    void SaveHist(TH1 *hist, TString imgSavePath);
    // Getters

    // Setters
   
private:
    Int_t fTest;
    Int_t runIndex = 84;
    TString imgSavePath = "~/Documents/BmnMonJS/public_html/img/";
    Int_t itersToUpdate = 10;
    // GEM config
    Int_t hitBins = 100;
  
     
    ClassDef(BmnMonitor, 1)
};

#endif