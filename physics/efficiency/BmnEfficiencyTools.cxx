#include "BmnEfficiencyTools.h"
#include "BmnDataTriggerInfo.h"
#include "BmnEfficiency.h"

BmnEfficiencyTools::BmnEfficiencyTools(TString dst) :
fPeriod(7),
fInnTracker(nullptr),
fAna(nullptr),
fRunTrigInfo(nullptr), 
fDstName(dst),
isDoNormalization(kTRUE), isMc(kTRUE) {

    // Inner tracker config ...
    fInnTracker = new BmnInnerTrackerGeometryDraw();

    fAna = new FairRunAna();

    Char_t* geoFileName = Form("current_geo_file.root");
    Int_t res_code = UniDbRun::ReadGeometryFile(fPeriod, 4649, geoFileName);
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-1);
    }

    TGeoManager::Import(geoFileName);
}

BmnEfficiencyTools::BmnEfficiencyTools() :
fInnTracker(nullptr),
fAna(nullptr),
fRunTrigInfo(nullptr),
fPeriod(7),
startRun(3589),
finishRun(5186),
fDataPath(""), fDstName(""), isDoNormalization(kTRUE), isMc(kFALSE) {
    fTargets.resize(0);
    fTriggers.resize(0);

    // Initializing trigg. info ...
    fRunTrigInfo = new BmnDataTriggerInfo();

    // Inner tracker config ...
    fInnTracker = new BmnInnerTrackerGeometryDraw();

    fAna = new FairRunAna();

    Char_t* geoFileName = Form("current_geo_file.root");
    Int_t res_code = UniDbRun::ReadGeometryFile(fPeriod, 4649, geoFileName);
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-1);
    }
    //cout << "1" << endl;
    TGeoManager::Import(geoFileName);
    //cout << "2" << endl;
}

void BmnEfficiencyTools::Process() {
    // Creating a list of dst files corresponding to the criteria established ...

    vector <Int_t> selectedRuns;
    vector <TString> selectedDst;

    if (!isMc) {
        // Loop over all dst files recorded ...
        for (Int_t iDst = startRun; iDst < finishRun; iDst++) {

            // Getting curr. trigger ...
            TString trigger = fRunTrigInfo->GetTrigger(iDst);
            if (trigger.IsNull())
                continue;

            // Getting curr. target ...
            UniDbRun* pCurrentRun = UniDbRun::GetRun(fPeriod, iDst);
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
            TString dstFile = fDataPath + "/" + TString::Format("bmndst_%d.root", run);
            // cout << dstFile << endl;
            selectedDst.push_back(dstFile);
        }
    }
    else 
        selectedDst.push_back(fDstName);

    const Int_t nStatsGem = fInnTracker->GetGemGeometry()->GetNStations();
    const Int_t nStatsSil = fInnTracker->GetSiliconGeometry()->GetNStations();

    const Int_t nModsGem = fInnTracker->GetGemGeometry()->GetStation(0)->GetNModules();
    const Int_t nModsSil = fInnTracker->GetSiliconGeometry()->GetStation(2)->GetNModules();

    const Int_t nZones = 2; // hot or big in GEM
    TH1F * gEffs[nStatsGem][nModsGem][nZones]; // GEM: stat, module, zone
    TH1F * sEffs[nStatsSil][nModsSil]; // SILICON: stat, module

    for (Int_t iGemStat = 0; iGemStat < nStatsGem; iGemStat++)
        for (Int_t iGemMod = 0; iGemMod < nModsGem; iGemMod++)
            for (Int_t iGemZone = 0; iGemZone < nZones; iGemZone++)
                gEffs[iGemStat][iGemMod][iGemZone] = new TH1F(Form("Detector# GEM, Station# %d, Module# %d, Zone# %d", iGemStat, iGemMod, iGemZone),
                    Form("Detector# GEM, Station# %d, Module# %d, Zone# %d", iGemStat, iGemMod, iGemZone), 1100, 0., 1.1);

    for (Int_t iSilStat = 0; iSilStat < nStatsSil; iSilStat++)
        for (Int_t iSilMod = 0; iSilMod < nModsSil; iSilMod++)
            sEffs[iSilStat][iSilMod] = new TH1F(Form("Detector# SILICON, Station# %d, Module# %d", iSilStat, iSilMod),
                Form("Detector# SILICON, Station# %d, Module# %d", iSilStat, iSilMod), 1100, 0., 1.1);

    // Trying to start eff. calculations for the Central Tracker elements ...
    TH1F* tmp = new TH1F("errDist", "errDist", 200, 0., 100.);

    for (TString dst : selectedDst) {
        BmnEfficiency* eff = new BmnEfficiency(fAna, fInnTracker, dst);

        UInt_t nEffEvs = 0;
        eff->Efficiency(nEffEvs, tmp);

        if (nEffEvs == 0) {
            delete eff;
            continue;
        }

        TClonesArray* silicons = eff->GetSiliconEfficiency();
        TClonesArray* gems = eff->GetGemEfficiency();

        for (Int_t iEff = 0; iEff < silicons->GetEntriesFast(); iEff++) {
            EffStore* epsilon = (EffStore*) silicons->UncheckedAt(iEff);

            Int_t stat = epsilon->Station();
            Int_t mod = epsilon->Module();

            // cout << " SILICON# " << stat << " " << mod << " --> " << epsilon->Efficiency() << endl;
            sEffs[stat][mod]->Fill(epsilon->Efficiency(), nEffEvs);
        }

        for (Int_t iEff = 0; iEff < gems->GetEntriesFast(); iEff++) {
            EffStore* epsilon = (EffStore*) gems->UncheckedAt(iEff);

            Int_t stat = epsilon->Station();
            Int_t mod = epsilon->Module();
            TString zone = epsilon->Zone();

            // cout << " GEM# " << stat << " " << mod << " " << zone << " --> " << epsilon->Efficiency() << endl;
            gEffs[stat][mod][((zone == "hot") ? 0 : 1)]->Fill(epsilon->Efficiency(), nEffEvs);
        }

        // cout << "nEff# " << nEffEvs << endl;
        delete eff;
    }

    if (isDoNormalization) {
        for (Int_t iSilStat = 0; iSilStat < nStatsSil; iSilStat++)
            for (Int_t iSilMod = 0; iSilMod < nModsSil; iSilMod++)
                DoNormalization(sEffs[iSilStat][iSilMod]);

        for (Int_t iGemStat = 0; iGemStat < nStatsGem; iGemStat++)
            for (Int_t iGemMod = 0; iGemMod < nModsGem; iGemMod++)
                for (Int_t iGemZone = 0; iGemZone < nZones; iGemZone++)
                    DoNormalization(gEffs[iGemStat][iGemMod][iGemZone]);
    }

    TFile* fOut = new TFile("outFile.root", "recreate");

    // Saving results to histos ..
    for (Int_t iSilStat = 0; iSilStat < nStatsSil; iSilStat++)
        for (Int_t iSilMod = 0; iSilMod < nModsSil; iSilMod++)
            sEffs[iSilStat][iSilMod]->Write();

    for (Int_t iGemStat = 0; iGemStat < nStatsGem; iGemStat++)
        for (Int_t iGemMod = 0; iGemMod < nModsGem; iGemMod++)
            for (Int_t iGemZone = 0; iGemZone < nZones; iGemZone++)
                gEffs[iGemStat][iGemMod][iGemZone]->Write();

    tmp->Write();
    delete fOut;
}

void BmnEfficiencyTools::DoNormalization(TH1F* h) {
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
