#ifndef BMNHISTSRC_H
#define BMNHISTSRC_H 1
#include <TNamed.h>
#include <exception>
#include <stdlib.h>
#include <vector>
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TFolder.h"
#include "TString.h"
#include "THttpServer.h"

#include "BmnHist.h"
#include "BmnTrigDigit.h"
#include "BmnTrigWaveDigit.h"
#include "BmnTrigRaw2Digit.h"

#include "BmnADCDigit.h"

#define SRC_COLS  2

using namespace std;

class BmnHistSrc : public BmnHist {
public:

    BmnHistSrc(TString title, TString path = "", Int_t periodID = 7, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistSrc();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(DigiArrays *fDigiArrays);
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);
    
private:
    void InitHistsFromArr(vector<TClonesArray*> *trigAr);
    void SetDir(TDirectory *Dir);
    vector<TString> Names;
    vector<vector<TH1F*> > hists;
    vector<vector<TH1F*> > histsAux;
    TH2F *hTDCTimes;
    TH2F *hTDCAmps;
    TH2F *hTQDCTimes;
    TH2F *hTQDCAmps;
    TCanvas *canvas;
    TCanvas *can2d;
    vector<PadInfo*> canPads;
    vector<PadInfo*> can2dPads;
    Int_t fRows = 0;
    Int_t fCols = 0;
    vector<TString> tdcNames;
    vector<TString> tqdcNames;
//    const TString trigNames[13] = 
//    {
//        "BC1", "BC2", "BC3", "BC4", "VC",
//        "X1_Left", "X1_Right", "X2_Left", "X2_Right",
//        "Y1_Left", "Y1_Right", "Y2_Left", "Y2_Right"
//    };
    //BmnTrigRaw2Digit fTrigMapper;

    ClassDef(BmnHistSrc, 1)
};

#endif /* BMNHISTSRC_H */

