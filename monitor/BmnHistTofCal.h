#ifndef BMNHISTTOFCAL_H
#define BMNHISTTOFCAL_H
// FairSoft
#include <TNamed.h>
#include "TChain.h"
#include "TClonesArray.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TFolder.h"
#include "THttpServer.h"
// BmnRoot
#include "BmnHist.h"
#include "BmnTofCalDigit.h"
#define BAR_COUNT   120
#define TOF_MIN    -200
#define TOF_MAX     200
#define Q_MAX      4000
#define T_MAX      4000
#define CANVAS_ROWS   3
#define CANVAS_COLS   2

class BmnHistTofCal : public BmnHist {
public:
    BmnHistTofCal(TString title, TString path = "");
    virtual ~BmnHistTofCal();
    
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL);
    void Reset();
    void DrawBoth();
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);
        
    void FillFromDigi(DigiArrays *fDigiArrays);
private:
    TH2F *Q0vsBar;
    TH2F *Q1vsBar;
    TH2F *T0vsBar;
    TH2F *T1vsBar;
    TH2F *TDiffvsBar;
    TH2F *QvsToF;
    vector<TString> Names;
    TCanvas *can;
    vector<PadInfo*> canPads;

    ClassDef(BmnHistTofCal, 1)
};

#endif /* BMNHISTTOFCAL_H */

