#include <TFile.h>
#include <TNamed.h>
#include <TGraph.h>
#include <TObjString.h>
#include <THttpServer.h>
#include <TCanvas.h>

#include <BmnCoordinateDetQa.h>
#include <BmnTimeDetQa.h>
#include <BmnCalorimeterDetQa.h>
#include <BmnTrigDetQa.h>
#include <BmnDstQa.h>

#ifndef BMNQAMONITOR_H
#define BMNQAMONITOR_H 1

class AllHistos : public TNamed {
public:

    AllHistos() {

    }

    ~AllHistos() {

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

    BmnQaMonitor();
    virtual ~BmnQaMonitor();

    AllHistos* GetRun(UInt_t);

    AllHistos* GetCurrentRun(UInt_t run) {
        return GetRun(run);
    }

    AllHistos* GetReferenceRun(UInt_t run) {
        return GetRun(run);
    }

    void ShowCurrentHistos(Int_t);
    void ShowReferenceHistos(Int_t);

private:
    THttpServer* fServer;

    void InitServer(Int_t cgi = 9000, Int_t http = 8080);
    void RegisterCanvases();
    void DivideCanvases(Int_t);
    void FillCanvasesWithHistos(Int_t);
    void RegisterUserCommands();

    // void MakeNormalization(vector <TH1F*>, vector <TH1F*>);

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
        // cout << fHisto.size() << " " << canv << endl; getchar();
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