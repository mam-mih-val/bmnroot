#include <TFile.h>
#include <TNamed.h>
#include <TGraph.h>
#include <TObjString.h>
#include <THttpServer.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TObjString.h>

#include <BmnCoordinateDetQa.h>
#include <BmnTimeDetQa.h>
#include <BmnCalorimeterDetQa.h>
#include <BmnTrigDetQa.h>
#include <BmnDstQa.h>

#include <BmnOfflineQaSteering.h>
#include <BmnRunInfo.h>
#include <UniRun.h>
#include <TLatex.h>

#ifndef BMNQAMONITOR_H
#define BMNQAMONITOR_H 1

class AllHistos : public TNamed {
public:

    AllHistos() {

    }

    virtual ~AllHistos() {

    }

    vector <TH1F*> Get1D() {
        return _h1d;
    }

    vector <TH2F*> Get2D() {
        return _h2d;
    }

    void Set1D(TH1F* h) {
        _h1d.push_back(h);
    }

    void Set2D(TH2F* h) {
        _h2d.push_back(h);
    }

private:
    vector <TH1F*> _h1d;
    vector <TH2F*> _h2d;

    ClassDef(AllHistos, 1)
};

class BmnQaMonitor : public TNamed {
public:

    BmnQaMonitor() {}
    BmnQaMonitor(TString);
    virtual ~BmnQaMonitor();

    void ShowCurrentHistos(Int_t);
    void ShowReferenceHistos(Int_t);

    void SetRelease(Int_t iRel) {
        fRelease = iRel;
    }

    void SetPeriod(Int_t period) {
        fPeriod = period;
    }

    void SetSetup(Int_t iSetup) {
        fSetup = iSetup;
    }

    void ClearCanvases();

    void SetHistoDir(TString path) {
        fHistoDir = path;
    }

private:
    THttpServer* fServer;

    void CreateInfoLists();
    void InitServer();
    void RegisterCanvases();
    void DivideCanvases();
    void RegisterUserCommands();
    void DrawInfoC();

    AllHistos* GetRun(UInt_t);

    AllHistos* GetCurrentRun(UInt_t run) {
        return GetRun(run);
    }

    AllHistos* GetReferenceRun(UInt_t run) {
        return GetRun(run);
    }

    template <class T> void GetHistoNames(T* man) {
        TObjArray* tx = man->GetManager()->GetListOfHistos().Tokenize("\n");
        for (Int_t i = 0; i < tx->GetEntries(); i++) {
            TString name = ((TObjString*) (tx->UncheckedAt(i)))->String();

            if (name.Contains("BmnHistManager"))
                continue;

            fHistoNames.push_back(name);
        }
    }

    template <class T> void GetHistosToBeRegistered(T* man) {
        BmnQaHistoManager* histoMan = man->GetManager();

        for (auto it : fHistoNames) {
            if (!histoMan->Exists(it))
                continue;

            fHisto.push_back(histoMan->H1(it));
        }
    }

    BmnCoordinateDetQa*** gem;
    BmnCoordinateDetQa*** silicon;
    BmnCoordinateDetQa*** csc;

    BmnTimeDetQa*** tof400;
    BmnTimeDetQa*** tof700;
    BmnTimeDetQa*** dch;
    BmnTimeDetQa*** mwpc;

    BmnCalorimeterDetQa*** ecal;
    BmnCalorimeterDetQa*** zdc;

    BmnDstQa*** dst;

    BmnTrigDetQa*** triggers;

    vector <TString> fHistoNames; // Histo names to get by corresponding getter
    vector <TH1*> fHisto; // Histos to be registered via server
    
    Int_t fCurrentRun;

    Int_t nReleases;
    Int_t* runs;
    Int_t nSetups;
    TString* setups;
    Int_t nCanvases;
    Int_t nDims;
    
    Int_t fPeriod;
    Int_t fRelease;
    Int_t fSetup;

    vector <TString>* fRefHistosNames;
    Bool_t isOneRefDrawn;

    TCanvas*** fCanvases;

    BmnOfflineQaSteering* fSteering;

    TString fHistoDir;
    AllHistos* fHistos;

//    TObjArray**** fRefList;
    TList* fRefList;

    TCanvas *infoCanvas;

    BmnRunInfo* fCurRunInfo;
    BmnRunInfo* fRefRunInfo;

    ClassDef(BmnQaMonitor, 1)
};

#endif
