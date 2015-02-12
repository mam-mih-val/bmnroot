#include "BmnGemStripDigitizer.h"

BmnGemStripDigitizer::BmnGemStripDigitizer() {

    fInputBranchName = "StsPoint";
    fOutputDigitsBranchName = "BmnGemStripDigit";

    fVerbose = 1;
}

BmnGemStripDigitizer::~BmnGemStripDigitizer() {

}

InitStatus BmnGemStripDigitizer::Init() {

    if(fVerbose) cout << " BmnGemStripDigitizer::Init()\n ";

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    //fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnGemStripDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "GEM", fBmnGemStripDigitsArray, kTRUE);

    return kSUCCESS;
}

void BmnGemStripDigitizer::Exec(Option_t* opt) {

    fBmnGemStripDigitsArray->Clear();

    if (!fBmnGemStripPointsArray) {
        Error("BmnGemStripDigitizer::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    if(fVerbose) cout << " BmnGemStripDigitizer::Exec(), Number of BmnGemStripPoints = " << fBmnGemStripPointsArray->GetEntriesFast() << "\n";

    ProcessMCPoints();

    if(fVerbose) cout << " BmnGemStripDigitizer::Exec() finished\n";
}

void BmnGemStripDigitizer::ProcessMCPoints() {

    FairMCPoint* GemStripPoint;

    BmnGemStripStationSet StationSet;

    for(UInt_t ipoint = 0; ipoint < fBmnGemStripPointsArray->GetEntriesFast(); ipoint++) {
        GemStripPoint = (FairMCPoint*) fBmnGemStripPointsArray->At(ipoint);

        Double_t x = -GemStripPoint->GetX(); // invert because in current geometry +x -left, -x - right
        Double_t y = GemStripPoint->GetY();
        Double_t z = GemStripPoint->GetZ();
        Double_t dEloss = GemStripPoint->GetEnergyLoss()*1e6; // in keV

        StationSet.AddPointToDetector(x, y, z, dEloss);
    }

    Int_t NAddedPoints = StationSet.CountNAddedToDetectorPoints();
    if(fVerbose) cout << "   Processed MC points  : " << NAddedPoints << "\n";

    for(Int_t iStation = 0; iStation < StationSet.GetNStations(); ++iStation) {
        BmnGemStripStation *station = StationSet.GetGemStation(iStation);

        for(Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripReadoutModule *module = station->GetReadoutModule(iModule);

            for(Int_t iLowerStrip = 0; iLowerStrip < module->CountLowerStrips(); ++iLowerStrip) {
                new ((*fBmnGemStripDigitsArray)[fBmnGemStripDigitsArray->GetEntriesFast()])
                    BmnGemStripDigit(iStation, iModule, 0, iLowerStrip, module->GetValueOfLowerStrip(iLowerStrip));
            }
            for(Int_t iUpperStrip = 0; iUpperStrip < module->CountUpperStrips(); ++iUpperStrip) {
                new ((*fBmnGemStripDigitsArray)[fBmnGemStripDigitsArray->GetEntriesFast()])
                    BmnGemStripDigit(iStation, iModule, 1, iUpperStrip, module->GetValueOfUpperStrip(iUpperStrip));
            }
        }
    }
}

void BmnGemStripDigitizer::Finish() {

}

ClassImp(BmnGemStripDigitizer)
