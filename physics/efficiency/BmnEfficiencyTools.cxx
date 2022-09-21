#include "BmnEfficiencyTools.h"
#include "BmnDataTriggerInfo.h"
#include "BmnEfficiency.h"

#include "TFile.h"

BmnEfficiencyTools::BmnEfficiencyTools(FairRunAna* fRunAna) :
fPeriod(7) {
    fRunAna->Print();
}

BmnEfficiencyTools::BmnEfficiencyTools(TString dst) :
fOutFile(""),
fPeriod(7),
fInnTracker(nullptr),
effGem(nullptr),
effSilicon(nullptr),
fAna(nullptr),
fRunTrigInfo(nullptr),
fDstName(dst), isMc(kTRUE), isL1(kFALSE), fGeomFile("") {

    // Inner tracker config ...
    fInnTracker = new BmnInnerTrackerGeometryDraw();

    fAna = new FairRunAna();
}

BmnEfficiencyTools::BmnEfficiencyTools() :
fOutFile(""),
fInnTracker(nullptr),
effGem(nullptr),
effSilicon(nullptr),
fAna(nullptr),
fRunTrigInfo(nullptr),
fPeriod(7),
startRun(3589),
finishRun(5186),
fDataPath(""), fDstName(""), fGeomFile(""), isMc(kFALSE), isL1(kFALSE) {

    // Setting zero content vectors for beams, targets and triggers ...
    // To be checked when processing a file list created manually ...
    fBeams.resize(0);
    fTargets.resize(0);
    fTriggers.resize(0);

    fManualList.resize(0);

    // Initializing trigg. info ...
    fRunTrigInfo = new BmnDataTriggerInfo();

    // Inner tracker config ...
    fInnTracker = new BmnInnerTrackerGeometryDraw();

    fAna = new FairRunAna();
}

void BmnEfficiencyTools::Process() {

    // Setting inn. tracker geometry ...
    if (fGeomFile.IsNull())
        Fatal("BmnEfficiencyTools::Process()", "No geometry file passed!!!");
    else
        TGeoManager::Import(fGeomFile.Data());

    // Creating a list of dst files corresponding to the criteria established ...

    vector <Int_t> selectedRuns;
    vector <TString> selectedDst;

    if (!isMc && !fManualList.size()) {
        // Loop over all dst files recorded ...
        for (Int_t iDst = startRun; iDst < finishRun; iDst++) {

            // Getting curr. trigger ...
            TString trigger = fRunTrigInfo->GetTrigger(iDst);
            if (trigger.IsNull())
                continue;

            // Getting curr. target ...
            UniRun* pCurrentRun = UniRun::GetRun(fPeriod, iDst);
            if (!pCurrentRun)
                continue;

            TString* targ = pCurrentRun->GetTargetParticle();
            if (!targ)
                continue;

            // Getting beam ...
            TString beam = pCurrentRun->GetBeamParticle();

            auto triggerSearch = find(fTriggers.begin(), fTriggers.end(), trigger);
            auto targetSearch = find(fTargets.begin(), fTargets.end(), *targ);
            auto beamSearch = find(fBeams.begin(), fBeams.end(), beam);

            if (targetSearch != fTargets.end() && triggerSearch != fTriggers.end() && beamSearch != fBeams.end())
                selectedRuns.push_back(iDst);
        }

        for (auto run : selectedRuns) {
            TString dstFile = fDataPath + "/" + ((!isL1) ? TString::Format("bmndst_%d.root", run) : (TString::Format("dst-cbm2bmn-%d.root", run)));
            selectedDst.push_back(dstFile);
        }
    } else if (!isMc && fManualList.size())
        for (auto it : fManualList)
            selectedDst.push_back(fDataPath + "/" + TString::Format("dst-cbm2bmn-%d.root", it));
    else
        selectedDst.push_back(fDstName);

    const Int_t nStatsGem = fInnTracker->GetGemGeometry()->GetNStations();
    const Int_t nStatsSil = fInnTracker->GetSiliconGeometry()->GetNStations();

    // Preparing containers to store 2d-efficiency ...
    effGem = new TClonesArray("EffStore2D");
    for (Int_t iStat = 0; iStat < nStatsGem; iStat++)
        new ((*effGem)[effGem->GetEntriesFast()]) EffStore2D("GEM", iStat, fYRangesGem);

    effSilicon = new TClonesArray("EffStore2D");
    for (Int_t iStat = 0; iStat < nStatsSil; iStat++)
        new ((*effSilicon)[effSilicon->GetEntriesFast()]) EffStore2D("SILICON", iStat, fYRangesSilicon);

    for (auto it = selectedDst.begin(); it != selectedDst.end(); it++) {
        BmnEfficiency* eff = new BmnEfficiency(fAna, fInnTracker, *it);
        eff->Efficiency(effGem, effSilicon, fYRangesGem, fYRangesSilicon);
        delete eff;
    }

    TFile* fOut = nullptr;

    if (fOutFile.IsNull())
        fOut = new TFile("outFile.root", "recreate");
    else
        fOut = new TFile(Form("%s", fOutFile.Data()), "recreate");

    // GEM ...
    const Int_t nQp = 2;

    for (Int_t iEff = 0; iEff < effGem->GetEntriesFast(); iEff++) {
        EffStore2D* epsilon = (EffStore2D*) effGem->UncheckedAt(iEff);

        Int_t stat = epsilon->Station();

        for (Int_t iGemStat = 0; iGemStat < nStatsGem; iGemStat++) {
            if (iGemStat != stat)
                continue;

            for (Int_t iQp = 0; iQp < nQp; iQp++) {
                epsilon->efficiency(iQp)->Write();

                for (Int_t iRange = 0; iRange < fYRangesGem.find(iGemStat)->second.size(); iRange++)
                    epsilon->efficiency4range(iRange, iQp)->Write();
            }
        }
    }

    // SILICON ...
    for (Int_t iEff = 0; iEff < effSilicon->GetEntriesFast(); iEff++) {
        EffStore2D* epsilon = (EffStore2D*) effSilicon->UncheckedAt(iEff);

        Int_t stat = epsilon->Station();

        for (Int_t iSilStat = 0; iSilStat < nStatsSil; iSilStat++) {

            if (iSilStat != stat)
                continue;

            for (Int_t iQp = 0; iQp < nQp; iQp++) {
                epsilon->efficiency(iQp, "", "SILICON")->Write();
                TH2F* hPassed = (TH2F*) epsilon->efficiency(iQp, "xP", "SILICON")->GetPassedHistogram();
                if (!iSilStat) {
                    epsilon->efficiency(iQp, "xP", "SILICON")->Write();
                    hPassed->Write("#varepsilon (x, P)");
                }

                TString tmp = !iQp ? ">" : "<";
                if (!iSilStat) {
                    TH1D* hMomPassed = hPassed->ProjectionY(Form("Momentum (passed), Q_{p} %s 0", tmp.Data()), 1, hPassed->GetNbinsX());
                    DoNormalization(hMomPassed);
                    hMomPassed->Write();
                }

                for (Int_t iRange = 0; iRange < fYRangesSilicon.find(iSilStat)->second.size(); iRange++)
                    epsilon->efficiency4range(iRange, iQp, "SILICON")->Write();
            }
        }
    }

    delete fOut;
}

void BmnEfficiencyTools::DoNormalization(TH1D* h) {
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
