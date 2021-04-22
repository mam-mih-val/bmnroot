#include "BmnRealisticMc.h"

BmnRealisticMc::BmnRealisticMc() :
fSilHitsArray(nullptr),
fGemHitsArray(nullptr),
fGemHitsArrayFiltered(nullptr),
fSilHitsArrayFiltered(nullptr),
fEff(nullptr) {
    fEff = new BmnEfficiency(); //test writing. Speed obtained looks suitable for working:)
}

InitStatus BmnRealisticMc::Init() {

    if (fVerbose > 1)
        cout << "======================== RealisticMc init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    fGemHitsArray = (TClonesArray*) ioman->GetObject("BmnGemStripHit"); //in
    fSilHitsArray = (TClonesArray*) ioman->GetObject("BmnSiliconHit"); //in 

    fGemHitsArrayFiltered = new TClonesArray("BmnGemStripHit");
    ioman->Register("BmnGemStripHitFiltered", "BmnGemStripHitFiltered_", fGemHitsArrayFiltered, kFALSE);

    fSilHitsArrayFiltered = new TClonesArray("BmnSiliconHit");
    ioman->Register("BmnSiliconHitFiltered", "BmnSiliconHitFiltered_", fSilHitsArrayFiltered, kFALSE);

    return kSUCCESS;
}

void BmnRealisticMc::Exec(Option_t* option) {
    fGemHitsArrayFiltered->Delete();
    fSilHitsArrayFiltered->Delete();

    gRandom->SetSeed(0);

    doHitSelection(fGemHitsArray, fGemHitsArrayFiltered);
    doHitSelection(fSilHitsArray, fSilHitsArrayFiltered);
}

void BmnRealisticMc::Finish() {

}

void BmnRealisticMc::doHitSelection(TClonesArray* hitsArray, TClonesArray* hitsArrayFiltered) {
    // Define detector we are processing (GEM or SILICON) ... 
    Bool_t isGem = kFALSE, isSilicon = kFALSE;

    TString className = TString(hitsArray->GetClass()->GetName());
    TString detector = (className.Contains("BmnGemStripHit")) ? "GEM" :
            className.Contains("BmnSiliconHit") ? "SILICON" : "";

    if (detector.IsNull())
        return;

    if (detector.Contains("GEM"))
        isGem = kTRUE;
    else
        isSilicon = kTRUE;

    // Collecting unique X'-digit indices ...
    set <Int_t> digiIndices;

    for (Int_t iHit = 0; iHit < hitsArray->GetEntriesFast(); iHit++) {
        BmnHit* hit = (BmnHit*) hitsArray->UncheckedAt(iHit);
        Int_t idx = hit->GetDigitNumberMatch().GetLink(0).GetIndex(); // Link to the corresponding index of X'-digit ...
        digiIndices.insert(idx);
    }

    // Trying to sort hits according to the indices of X'-digits ...       
    map <Int_t, vector < BmnHit>> hitMap;

    for (auto idx : digiIndices) {
        vector <BmnHit> hitVector;
        hitMap[idx] = hitVector;
    }

    // Loop over GEM hits once more ...
    for (Int_t iHit = 0; iHit < hitsArray->GetEntriesFast(); iHit++) {
        BmnHit* hit = (BmnHit*) hitsArray->UncheckedAt(iHit);

        // Getting link to X'-digit of the hit ...       
        BmnMatch digiMatch = hit->GetDigitNumberMatch();
        Int_t idx = digiMatch.GetLink(0).GetIndex(); // Link to the corresponding index of X'-digit ...

        hitMap.find(idx)->second.push_back(*(BmnHit*) hit);
    }

    // Looking for hits with the known X'-digit index ...
    for (auto it : hitMap) {

        // Generating prob. value for the digit ...
        Double_t probDigiValue = gRandom->Rndm();

        TString zone = "";
        BmnHit* hit = nullptr;

        Int_t counter = 0;

        Bool_t isSkipped = kFALSE;

        if (isGem) {
            do {
                // Getting hit and GEM zone ...
                Int_t hitIdx = it.second.at(Int_t(it.second.size() * gRandom->Rndm())).GetIndex();
                hit = (BmnHit*) hitsArray->UncheckedAt(hitIdx);
                zone = fEff->GetGemZone(hit);

                counter++;

                if (counter > 2. * it.second.size()) {
                    isSkipped = kTRUE;
                    break;
                }
            } while (zone == "");
        } else if (isSilicon) {
            Int_t hitIdx = it.second.at(Int_t(it.second.size() * gRandom->Rndm())).GetIndex();
            hit = (BmnHit*) hitsArray->UncheckedAt(hitIdx);
        }

        if (isSkipped)
            continue;

        // Generating prob. value for the hit ...
        BmnEfficiencyProbability* probs = new BmnEfficiencyProbability("");

        probs->SetDetector(detector);
        probs->SetStation(hit->GetStation());
        probs->SetModule(hit->GetModule());

        if (isGem)
            probs->SetZone(zone);

        Double_t probHitValue = probs->GetProbability();

        delete probs;

        Bool_t isExcluded = (probDigiValue > probHitValue) ? kTRUE : kFALSE;
        // Mark hits to be excluded ...
        if (isExcluded) {
            for (BmnHit hit0 : it.second) {
                BmnHit* hitWithSetType = (BmnHit*) hitsArray->UncheckedAt(hit0.GetIndex());
                hitWithSetType->SetType(1000);
            }
        }            // Write survived hits ...
        else {
            for (BmnHit hit0 : it.second) {
                if (isGem) {
                    BmnGemStripHit* hitWithSetType = (BmnGemStripHit*) hitsArray->UncheckedAt(hit0.GetIndex());
                    new ((*hitsArrayFiltered)[hitsArrayFiltered->GetEntriesFast()]) BmnGemStripHit(*hitWithSetType);
                }
                else {
                    BmnSiliconHit* hitWithSetType = (BmnSiliconHit*) hitsArray->UncheckedAt(hit0.GetIndex());
                    new ((*hitsArrayFiltered)[hitsArrayFiltered->GetEntriesFast()]) BmnSiliconHit(*hitWithSetType);
                }
            }
        }
    }
}



