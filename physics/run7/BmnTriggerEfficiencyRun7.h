#include <TGraph.h>

#include <BmnLambdaAnalRun7.h>
#include <BmnDataTriggerInfo.h>
#include <BmnTwoParticleDecay.h>

using namespace std;

#ifndef BMNTRIGEFFRUN7_H
#define BMNTRIGEFFRUN7_H 1

class BmnTriggerEfficiencyRun7 : public BmnLambdaAnalRun7 {
public:

    BmnTriggerEfficiencyRun7() {
        ;
    }

    BmnTriggerEfficiencyRun7(TString dir) : BmnLambdaAnalRun7(dir),
    fEoSNode("root://ncm.jinr.ru/"), fSpectraFile(""),
    nMultBins(18),
    nBdCounts(1000),
    nFdCounts(1000), isAddTriggerCondition(kFALSE) {

        // Preparing default multiplicity ranges (nTracks in prim. vertex) ...
        const Int_t minMult = 2;
        const Int_t maxMult = 20;
        Int_t multStep = (maxMult - minMult) / nMultBins;

        for (Int_t iBin = 0; iBin < nMultBins; iBin++) {
            Int_t step = iBin * multStep + minMult;

            fMultMap[iBin] = make_pair(step, step + multStep);
        }
    }

    virtual ~BmnTriggerEfficiencyRun7() {
        ;
    }

    // Setters ...

    void SetTrigger(TString trigg) {
        fTrigger = trigg;
    }

    void SetMultiplicityMap(vector <pair <Int_t, Int_t>> bins) {
        fMultMap.clear();

        nMultBins = bins.size();

        for (Int_t iBin = 0; iBin < bins.size(); iBin++)
            fMultMap[iBin] = bins.at(iBin);
    }

    void SetSpectraFile(TString f) {
        fSpectraFile = f;
    }

    void SetEffFiles(TString f) {
        fEffFiles.push_back(f);
    }

    // Getters ...


    // Anals ...
    void triggerEfficiency();

private:
    TString fEoSNode;

    TString fTrigger;
    Bool_t isAddTriggerCondition;

    TString fSpectraFile;
    vector <TString> fEffFiles; // to be set if needed !!!

    enum TriggConditions {
        base = 0, severe
    };

    Int_t nFdCounts;
    Int_t nBdCounts;
    Int_t nMultBins;

    map <Int_t, pair <Int_t, Int_t>> fMultMap;

    // Spectrum histos ...
    TH1F*** hSpectra; // [triggCond][multCondition]

    // Multiplicity histos ...
    TH1F** hMult; // [triggCond]

    // Redefinition aimed at getting trigger condition when doing file list ...
    vector <TString> createFilelist(vector <TString>);

    // Redefinition aimed at getting trigger condition in anal ...
    void ReadFile(TString, BmnParticlePairCut* cut0);

    Int_t FinMultBin(Int_t multValue) {
        for (auto it : fMultMap) {
            Int_t bin = it.first;
            Int_t min = it.second.first;
            Int_t max = it.second.second;

            if (multValue >= min && multValue < max)
                return bin;
        }
        // If multBin not found ...
        return -1;
    }

private:

    void DoNormalization(TH1F* h) {
        if (h->GetEntries() == 0)
            return;

        // Collecting all bin contents ...
        Double_t contentAll = 0.;

        for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
            contentAll += h->GetBinContent(iBin);

        // Normalizing histo ...
        for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++) {
            h->SetBinContent(iBin, h->GetBinContent(iBin) / contentAll);
            h->SetBinError(iBin, 0.);
        }

        h->GetYaxis()->SetRangeUser(0., 1.1 * h->GetMaximum());
    }
    
    ClassDef(BmnTriggerEfficiencyRun7, 0)
};

#endif