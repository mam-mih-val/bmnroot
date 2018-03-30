/* 
 * File:   BmnHist.h
 * Author: ilnur
 *
 * Created on February 2, 2017, 2:10 PM
 * 
 * Base class for histogram sets
 */

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

#define PAD_WIDTH   256
#define PAD_HEIGHT  192

using namespace std;

//template <class HH>
class PadInfo : public TObject {
public:
    PadInfo() {
        current = NULL;
        ref = NULL;
        ref2 = NULL;
    }

    ~PadInfo() {
        if (current) delete current;
        if (ref) delete ref;
        current = NULL;
        ref = NULL;
        ref2 = NULL;
    }
    TH1* current;
    TH1* ref;
    TH1* ref2;
    TString opt;
private:
    ClassDef(PadInfo, 1)
};
ClassImp(PadInfo)

class BmnHist : public TNamed {
public:

    BmnHist(Int_t periodID = 7);
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
    static BmnStatus LoadRefRun(Int_t refID, TString FullName, TString fTitle, vector<PadInfo*> canPads, vector<TString> Names);

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

    void SetperiodID(Int_t v) {
        this->fPeriodID = v;
    }

    Int_t GetperiodID() const {
        return fPeriodID;
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

    ClassDef(BmnHist, 1)

};

#endif /* BMNHIST_H */

