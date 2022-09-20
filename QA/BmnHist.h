
#ifndef BMNHIST_H
#define BMNHIST_H 1

#include <stdlib.h>
#include <vector>

#include <TNamed.h>
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFolder.h"
#include "TString.h"
#include "THttpServer.h"
#include "TFile.h"
#include "TFolder.h"

#include "BmnEventHeader.h"
#include "DigiArrays.h"

#include <PadInfo.h>
#include <BmnPadBranch.h>

#define PAD_WIDTH   256
#define PAD_HEIGHT  192

using namespace std;

/** 
 * Base class for histogram sets
 */
class BmnHist : public TNamed {
public:

    BmnHist(Int_t periodID = 7, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHist();
    virtual void Reset() = 0;
    virtual void Register(THttpServer *serv) = 0;
    virtual void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) = 0;
    virtual void DrawBoth() = 0;
    virtual void FillFromDigi(DigiArrays *fDigiArrays) = 0;
    virtual void ClearRefRun() = 0;
//    virtual BmnStatus LoadRefRun(TString FileName) = 0;
//    template <class HH>
    static void DrawRef(TCanvas *canGemStrip, vector<PadInfo*> *canGemStripPads);
    static void DrawPad(TVirtualPad *pad, PadInfo *info);
    static void FillPad(PadInfo *info, TTree* tree);
    static BmnStatus LoadRefRun(Int_t refID, TString FullName, TString fTitle, vector<PadInfo*> canPads, vector<TString> Names);
    static BmnStatus DrawPadFromTree(BmnPadBranch* br);
    static BmnStatus FillPadFromTree(BmnPadBranch* br, TTree* tree);

    void SetRefRunName(TString v) {
        this->refRunName = v;
    }

    TString GetRefRunName() const {
        return refRunName;
    }

    void SetRefID(Int_t v) {
        this->refID = v;
    }

    Int_t GetRefID() const {
        return refID;
    }

    void SetRefPath(TString v) {
        this->refPath = v;
    }

    TString GetRefPath() const {
        return refPath;
    }

    void SetPeriodID(Int_t v) {
        this->fPeriodID = v;
    }

    Int_t GetPeriodID() const {
        return fPeriodID;
    }
    
    void SetBmnSetup(BmnSetup v) {
        this->fSetup = v;
    }

    BmnSetup GetBmnSetup() const {
        return fSetup;
    }
    
protected:

    Bool_t isShown = kFALSE;
    THttpServer *fServer;
    TTree *frecoTree;
    TDirectory *fDir;
    TString refPath;
    TString refRunName;
    Int_t refID;
    TFile *refFile;
    Int_t fPeriodID;
    BmnSetup fSetup;

    ClassDef(BmnHist, 1)

};

#endif /* BMNHIST_H */

