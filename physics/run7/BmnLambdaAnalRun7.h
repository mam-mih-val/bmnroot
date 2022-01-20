#ifndef BMNLAMBDAANALRUN7_H
#define BMNLAMBDAANALRUN7_H 1

#include <BmnMassSpectrumAnal.h>

#include "TObjString.h"

#include <set>

using namespace std;

// Class to store signal (and its error, if necessary) for different samples of data and path bins

class SdeltaS {
public:
    TString dSet; // MC or DATA

    Int_t pathBin = -1; // path bin
    Int_t ptBin = -1; // pt bin
    Int_t yBin = -1; // y bin

    Int_t s;
    Int_t ds;
};

class TriggerEfficiency : public TObject {
public:

    TriggerEfficiency() {
        ;
    }

    TriggerEfficiency(TString tr, pair <Int_t, Int_t> mult, pair <Double_t, Double_t> e) {
        trigg = tr;

        minMult = mult.first;
        maxMult = mult.second;

        eff = e.first;
        errEff = e.second;
    }

    virtual ~TriggerEfficiency() {
        ;
    }

    TString trigger() {
        return trigg;
    }

    pair <Int_t, Int_t> multilplicity() {
        return make_pair(minMult, maxMult);
    }

    Double_t efficiency() {
        return eff;
    }

    Double_t efficiencyError() {
        return errEff;
    }

private:

    TString trigg;

    Int_t minMult;
    Int_t maxMult;

    Double_t eff;
    Double_t errEff;


    ClassDef(TriggerEfficiency, 1)
};

class BmnLambdaAnalRun7 : public BmnMassSpectrumAnal {
public:

    BmnLambdaAnalRun7() :
    fTrigEffs(new TClonesArray("TriggerEfficiency")) {

    }

    BmnLambdaAnalRun7(TString dir) : BmnMassSpectrumAnal(dir),
    fTrigEffs(nullptr) {

        fPtBinMap.clear();
        fYBinMap.clear();

        isMc = kFALSE;
    }

    // Constructor with two input dirs (data and MC)

    BmnLambdaAnalRun7(TString dirMc, TString dirData, TString target = "") :
    fTrigEffs(nullptr) {

        fPtBinMap.clear();
        fYBinMap.clear();

        isMc = kTRUE;

        BmnMassSpectrumAnal* mc = new BmnMassSpectrumAnal(dirMc);
        fMcList = mc->GetFileList();
        delete mc;

        BmnMassSpectrumAnal* data = new BmnMassSpectrumAnal(dirData);
        data->SetTarget(target);
        fDataList = data->createFilelist();
        delete data;

        hSpectrumImproved = nullptr;
    }

    virtual ~BmnLambdaAnalRun7() {
        if (fTrigEffs)
            delete fTrigEffs;
    }

    void SetIsMc(Bool_t flag) {
        isMc = flag;
    }

    // Cuts to be used for analysis ...

    void SetCuts(TString target,
            Double_t DCA0, Double_t DCA1, Double_t DCA2, Double_t DCA12, Double_t PATH = -1, Int_t nHitsPos = 2, Int_t nHitsNeg = 2) {

        BmnParticlePairCut* pCut = new BmnParticlePairCut();
        pCut->SetDca0(DCA0);
        pCut->SetDca1(DCA1);
        pCut->SetDca2(DCA2);
        pCut->SetDca12(DCA12);
        pCut->SetPath(PATH);
        pCut->SetNHitsGemPos(nHitsPos);
        pCut->SetNHitsGemNeg(nHitsNeg);

        fTargCutsMap[target] = pCut;
    }

    BmnParticlePairCut* GetCuts(TString target) const {
        return fTargCutsMap.find(target)->second;
    }

    void SetPtBins(vector <pair <Double_t, Double_t>> bins) {
        fPtBinMap.clear();

        nPtBins = bins.size();

        for (Int_t iBin = 0; iBin < bins.size(); iBin++)
            fPtBinMap[iBin] = bins.at(iBin);
    }

    void SetYBins(vector <pair <Double_t, Double_t>> bins) {
        fYBinMap.clear();

        nYBins = bins.size();

        for (Int_t iBin = 0; iBin < bins.size(); iBin++)
            fYBinMap[iBin] = bins.at(iBin);
    }

    void SetPathBins(vector <pair <Double_t, Double_t>> bins) {
        fPathBins.clear();

        nPathBins = bins.size();

        for (Int_t iBin = 0; iBin < bins.size(); iBin++)
            fPathBins[iBin] = bins.at(iBin);
    }

    // Trigger efficiency ...

    void SetTriggerEffData(TString trigger, TString data) {
        fTrigEffData[trigger] = data;
    }

    // Getters ...

    map <TString, BmnParticlePairCut*> GetTargetCutsMap() {
        return fTargCutsMap;
    }

    // For specific use outside the class if needed ...
    TClonesArray* GetFilledArray() {
        return fTrigEffs;
    }

    map <Int_t, pair <Double_t, Double_t>> GetSignalPtBinMap() {
        return fSdeltaS_PtBinMap;
    }

    map <Int_t, pair <Double_t, Double_t>> GetSignalYBinMap() {
        return fSdeltaS_YBinMap;
    }

    void doAllTargetsAnal();
    void doTargetAnal(TString target);
    void doTargetAnal(vector <TString> targets);

private:
    void targetListsByCutsEstablished(set <TString>&, set <TString>&); // Checking whether the established cuts are the same for all targets ... 
    void DrawH(TH1F*, Double_t, Double_t, pair <Double_t, Double_t>, pair <Double_t, Double_t>, Int_t si = 1);

    void doTargetAnal(); // For internal use in the class ...
    void doTargetAnalPath();

public:

    void ReadData() {
        fTrigEffs = new TClonesArray("TriggerEfficiency");

        for (auto trigger : fTrigEffData) {
            TString trig = trigger.first;
            TString data = trigger.second;

            TFile* f = new TFile(data.Data());
            TH1F* h = (TH1F*) f->Get("Eff");

            if (!h)
                continue;

            for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++) {
                vector <Int_t> mults;
                vector <Double_t> effs;

                effs.push_back(h->GetBinContent(iBin));
                effs.push_back(h->GetBinError(iBin));

                TString label = (TString) h->GetXaxis()->GetBinLabel(iBin);
                label.ReplaceAll(")", "").ReplaceAll("[", "").ReplaceAll(",", "");

                TObjArray* tx = label.Tokenize(" ");

                for (Int_t i = 0; i < tx->GetEntries(); i++)
                    mults.push_back(((TObjString*) (tx->At(i)))->String().Atoi());

                new ((*fTrigEffs) [fTrigEffs->GetEntriesFast()])
                        TriggerEfficiency(trig, make_pair(mults.at(0), mults.at(1)), make_pair(effs.at(0), effs.at(1)));
            }
        }
    }

private:
    TClonesArray* fTrigEffs;
    map <TString, TString> fTrigEffData;

    // Maps with output S and deltaS for all bins over Pt and Y ...
    map <Int_t, pair <Double_t, Double_t>> fSdeltaS_PtBinMap;
    map <Int_t, pair <Double_t, Double_t>> fSdeltaS_YBinMap;

    vector <TString> fMcList;
    vector <TString> fDataList;

protected:
    map <TString, BmnParticlePairCut*> fTargCutsMap;

    ClassDef(BmnLambdaAnalRun7, 0)
};

#endif
