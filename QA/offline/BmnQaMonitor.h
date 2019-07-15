#include <TFile.h>
#include <TNamed.h>
#include <TGraph.h>
#include <TObjString.h>
#include <THttpServer.h>

#include <BmnCoordinateDetQa.h>
#include <BmnTimeDetQa.h>
#include <BmnCalorimeterDetQa.h>
#include <BmnTrigDetQa.h>
#include <BmnDstQa.h>

#ifndef BMNQAMONITOR_H
#define BMNQAMONITOR_H 1

class PadInfo : public TNamed {
public:
    PadInfo() {
        current = nullptr;
        ref = nullptr;
    }

    ~PadInfo() {
        if (current)
            delete current;
        if (ref)
            delete ref;
        current = nullptr;
        ref = nullptr;
    }

    TH1F* GetCurrentHisto() {
        return current;
    }

    TH1F* GetRefHisto() {
        return ref;
    }
    
    TString GetOption() {
        return opt;
    } 

    void SetCurrentHisto(TH1F* h) {
        current = h;
    }

    void SetRefHisto(TH1F* h) {
        ref = h;
    }
    
    void SetOption(Option_t option) {
        opt = option;
    } 

private:
    TH1F* current;
    TH1F* ref;
    TString opt;
private:
    ClassDef(PadInfo, 1)
};

class BmnQaMonitor : public TNamed {
public:

    BmnQaMonitor();
    virtual ~BmnQaMonitor();

    void SetDebug(Bool_t opt) {
        fDebug = opt;
    }

    vector <TH1F*> GetRun(UInt_t);
    vector <TH1F*> GetCurrentRun(UInt_t run) {return GetRun(run);} 
    vector <TH1F*> GetReferenceRun(UInt_t run) {return GetRun(run);}
    
    void DrawRef(TCanvas*, vector <PadInfo*>*);
    void ShowCurrentHistos(Int_t);
    void ShowReferenceHistos(Int_t);

private:
    Bool_t fDebug;
    THttpServer* fServer;

    void InitServer(Int_t cgi = 9000, Int_t http = 8080);
    void RegisterCanvases();
    void DivideCanvases(Int_t);
    void FillCanvasesWithHistos(Int_t);
    void RegisterUserCommands();
    
    void MakeNormalization(vector <TH1F*>, vector <TH1F*>);

    template <class T> void FillCanvasesWithHistos(T* histos, TString name, Int_t iCanvas, Int_t padCounter = 1) {
        BmnQaHistoManager* man = histos->GetManager();
        for (auto it : fHistoNames) {
            if (!it.Contains(name.Data()) || !man->Exists(it))
                continue;
            fCanvases[iCanvas]->cd(padCounter);
            TH1F* h = (TH1F*) man->H1(it);
            h->Draw();

            padCounter++;
        }
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

    template <class T> void GetHistosToBeRegistered(T* man, Int_t canv) {
        BmnQaHistoManager* histoMan = man->GetManager();

        for (auto it : fHistoNames) {
            if (!histoMan->Exists(it))
                continue;

            fHisto.push_back(histoMan->H1(it));
        }
        // cout << fHisto.size() << " " << canv << endl;
    }

    BmnCoordinateDetQa* gem;
    BmnCoordinateDetQa* silicon;
    BmnCoordinateDetQa* csc;

    BmnTimeDetQa* tof400;
    BmnTimeDetQa* tof700;
    BmnTimeDetQa* dch;
    BmnTimeDetQa* mwpc;

    BmnCalorimeterDetQa* ecal;
    BmnCalorimeterDetQa* zdc;

    BmnDstQa* dst;
    
    BmnTrigDetQa* triggers;

    vector <TString> fHistoNames; // Histo names to get by corresponding getter
    vector <TH1*> fHisto; // Histos to be registered via server
    
    Int_t fCurrentRun;

    TCanvas** fCanvases;

    ClassDef(BmnQaMonitor, 1)
};

#endif