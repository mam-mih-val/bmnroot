#ifndef BMNHISTECAL_H
#define BMNHISTECAL_H 1

#include <TNamed.h>
#include "TChain.h"
#include "TColor.h"
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
#include "BmnECALDigit.h"
#define ECAL_ROWS    1
#define ECAL_COLS    2


class BmnHistECAL : public BmnHist {
public:
    BmnHistECAL(TString title, TString path = "");
    virtual ~BmnHistECAL();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile, TTree *recoTree);
    void DrawBoth();
    void FillFromDigi(DigiArrays *fDigiArrays);
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);
private:
    TClonesArray* ZDCHits;
    TH2D* h2d_grid;
    TH2D* h2d_profile;
    TH1D* hx;
    TH1D* hy;
    TCanvas *canAmps;
    vector<PadInfo*> canAmpsPads;
    vector<TString> NamesAmps;

    ClassDef(BmnHistECAL, 1)
};

#endif /* BMNHISTECAL_H */

