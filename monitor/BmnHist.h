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

#include <TNamed.h>
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TChain.h"
#include "TFolder.h"
#include "TString.h"
#include "THttpServer.h"

template <class HH>
class PadInfo : public TObject {
public:
    PadInfo() {
        current = NULL;
        ref = NULL;
    }

    ~PadInfo() {
        if (ref) delete ref;
        ref = NULL;
    }
    HH* current;
    HH* ref;
private:
    ClassDef(PadInfo, 1)
};
templateClassImp(PadInfo)

class BmnHist : public TNamed {
public:

    BmnHist();
    virtual ~BmnHist();
    virtual void Reset() = 0;
    virtual void Register(THttpServer *serv) = 0;
    virtual void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) = 0;

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
    //    virtual void FillFromDigi(TClonesArray * digits) = 0;
protected:


    THttpServer *fServer;
    TTree *frecoTree;
    TString refPath;
    TString refRunName;
    Int_t refID;
    TFile *refFile;

    ClassDef(BmnHist, 1)

};

#endif /* BMNHIST_H */

