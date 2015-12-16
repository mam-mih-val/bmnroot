#include "BmnGemStripDigitizer.h"
#include "CbmMCTrack.h"

int entrys = 0;

BmnGemStripDigitizer::BmnGemStripDigitizer()
: fOnlyPrimary(kFALSE) {

    fInputBranchName = "StsPoint";
    fOutputDigitsBranchName = "BmnGemStripDigit";

    fVerbose = 1;

    fSmearingSigma = 0.0; //cm
}

BmnGemStripDigitizer::~BmnGemStripDigitizer() {

}

InitStatus BmnGemStripDigitizer::Init() {

    if(fVerbose) cout << " BmnGemStripDigitizer::Init()\n ";
    if(fVerbose && fOnlyPrimary) cout << " Only primary particles are processed!!! " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

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

    if(fVerbose) {
        cout << " BmnGemStripDigitizer::Exec(), event = " << entrys << "\n";
        cout << " BmnGemStripDigitizer::Exec(), Number of BmnGemStripPoints = " << fBmnGemStripPointsArray->GetEntriesFast() << "\n";
    }

    ProcessMCPoints();

    if(fVerbose) cout << " BmnGemStripDigitizer::Exec() finished\n\n";
    entrys++;
}

void BmnGemStripDigitizer::ProcessMCPoints() {

    FairMCPoint* GemStripPoint;
    Int_t NNotPrimaries = 0;

    BmnGemStripStationSet StationSet;

    for(UInt_t ipoint = 0; ipoint < fBmnGemStripPointsArray->GetEntriesFast(); ipoint++) {
        GemStripPoint = (FairMCPoint*) fBmnGemStripPointsArray->At(ipoint);

        if(fOnlyPrimary) {
            CbmMCTrack *track = (CbmMCTrack*)fMCTracksArray->UncheckedAt(GemStripPoint->GetTrackID());
            if(!track) continue;
            if(track->GetMotherId() != -1)  {
                NNotPrimaries++;
                continue;
            }
        }

        Double_t x = -GemStripPoint->GetX(); // invert because in current geometry +x -left, -x - right
        Double_t y = GemStripPoint->GetY();
        Double_t z = GemStripPoint->GetZ();
        Double_t dEloss = GemStripPoint->GetEnergyLoss()*1e6; // in keV

        //smearing
        if(fSmearingSigma > 0.0) {
            x = gRandom->Gaus(x, fSmearingSigma);
            y = gRandom->Gaus(y, fSmearingSigma);
        }

        StationSet.AddPointToDetector(x, y, z, dEloss);
    }

    Int_t NAddedPoints = StationSet.CountNAddedToDetectorPoints();
    if(fVerbose && fOnlyPrimary) cout << "   Number of not primaries points : " << NNotPrimaries << "\n";
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
