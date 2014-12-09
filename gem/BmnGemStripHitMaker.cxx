#include "BmnGemStripHitMaker.h"

BmnGemStripHitMaker::BmnGemStripHitMaker() {

    fInputBranchName = "StsPoint";
    fOutputDigitsBranchName = "BmnGemStripDigit";
    fOutputHitsBranchName = "BmnGemStripHit";
}

BmnGemStripHitMaker::~BmnGemStripHitMaker() {

}

InitStatus BmnGemStripHitMaker::Init() {

    cout << " BmnGemStripHitMaker::Init()\n ";

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    //fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnGemStripDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "GEM", fBmnGemStripDigitsArray, kTRUE);

    fBmnGemStripHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "GEM", fBmnGemStripHitsArray, kTRUE);

    return kSUCCESS;
}

void BmnGemStripHitMaker::Exec(Option_t* opt) {

    fBmnGemStripDigitsArray->Clear();
    fBmnGemStripHitsArray->Clear();

    if (!fBmnGemStripPointsArray) {
        Error("BmnGemStripHitMaker::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    cout << " BmnGemStripHitMaker::Exec(), Number of BmnGemStripPoints = " << fBmnGemStripPointsArray->GetEntriesFast() << "\n";

    ProcessMCPoints();

    cout << " BmnGemStripHitMaker::Exec() finished\n";
}

void BmnGemStripHitMaker::ProcessMCPoints() {

    FairMCPoint* GemStripPoint;

    BmnGemStripStationSet StationSet;

    for(UInt_t ipoint = 0; ipoint < fBmnGemStripPointsArray->GetEntriesFast(); ipoint++) {
        GemStripPoint = (FairMCPoint*) fBmnGemStripPointsArray->At(ipoint);

        Double_t x = -GemStripPoint->GetX(); // invert because in current geometry +x -left, -x - right
        Double_t y = GemStripPoint->GetY();
        Double_t z = GemStripPoint->GetZ();

        //Double_t px = GemStripPoint->GetPx();
        //Double_t py = GemStripPoint->GetPy();
        //Double_t pz = GemStripPoint->GetPz();

        StationSet.AddPointToDetector(x, y, z);
    }

    StationSet.ProcessPointsInDetector();

    Int_t NAddedPoints = StationSet.CountNAddedToDetectorPoints();
    Int_t NCalculatedPoints = StationSet.CountNProcessedPointsInDetector();

    cout << "   Processed MC points  : " << NAddedPoints << "\n";
    cout << "   Calculated points  : " << NCalculatedPoints << "\n";

    Int_t index = 0;

    for(Int_t iStation = 0; iStation < StationSet.GetNStations(); iStation++) {
        BmnGemStripStation *station = StationSet.GetGemStation(iStation);

        for(Int_t iModule = 0; iModule < station->GetNModules(); iModule++) {
            BmnGemStripReadoutModule *module = station->GetReadoutModule(iModule);

            Double_t z = module->GetZPositionReadout();
            Double_t x_err = 0.0;
            Double_t y_err = 0.0;
            Double_t z_err = 0.0;

            for(Int_t iPoint = 0; iPoint < module->GetNIntersectionPoints(); iPoint++) {
                Double_t x = -module->GetIntersectionPointX(iPoint); // invert back
                Double_t y = module->GetIntersectionPointY(iPoint);

                Int_t lower_strip = module->GetIntersectionPointLowerStrip(iPoint);
                Int_t upper_strip = module->GetIntersectionPointUpperStrip(iPoint);

                //Add digit
                new ((*fBmnGemStripDigitsArray)[fBmnGemStripDigitsArray->GetEntriesFast()])
                    BmnGemStripDigit(iStation, iModule, lower_strip, upper_strip, x, y, z);

                //Add hit
                new ((*fBmnGemStripHitsArray)[fBmnGemStripHitsArray->GetEntriesFast()])
                    BmnGemStripHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), index);

                index++;
            }
        }
    }
}

void BmnGemStripHitMaker::Finish() {

}

ClassImp(BmnGemStripHitMaker)