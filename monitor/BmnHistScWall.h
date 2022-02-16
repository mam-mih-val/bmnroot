#ifndef BMNHISTScWall_H
#define BMNHISTScWall_H 1

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
#include "BmnScWallDigi.h"
#include "BmnScWallRaw2Digit.h"
#define ScWall_ROWS    1
#define ScWall_COLS    2


class BmnHistScWall : public BmnHist {
public:
    BmnHistScWall(TString title = "ScWall", TString path = "");
    virtual ~BmnHistScWall();
    void CreateHistos();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile, TTree *recoTree);
    void DrawBoth();
    void ClearRefRun();
    void FillFromDigi(DigiArrays *fDigiArrays);
    BmnStatus  SetRefRun(Int_t id);
private:
    long int fEventCounter = 0;
    vector<float> fUniqueXpos;
    vector<float> fUniqueYpos;
    TH2D* h2d_grid;
    TH2D* h2d_profile;
    TCanvas *canAmps;
    vector<PadInfo*> canAmpsPads;
    vector<TString> NamesAmps;

    ClassDef(BmnHistScWall, 1)
};

#endif /* BMNHISTScWall_H */

