
#include "BmnGemStripDigitizer.h"
#include "CbmMCTrack.h"
#include "BmnGemStripStationSet_FullConfig.h"
#include "BmnGemStripStationSet_1stConfig.h"
#include "BmnGemStripStationSet_1stConfigShort.h"
#include "BmnGemStripStationSet_2ndConfig.h"

static Float_t workTime = 0.0;
int entrys = 0;

BmnGemStripDigitizer::BmnGemStripDigitizer()
: fOnlyPrimary(kFALSE), fStripMatching(kTRUE) {

    fInputBranchName = "StsPoint";
    fOutputDigitsBranchName = "BmnGemStripDigit";
    fOutputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fVerbose = 1;

    fSmearingSigma = 0.0; //cm

    fCurrentConfig = BmnGemStripConfiguration::None;
}

BmnGemStripDigitizer::~BmnGemStripDigitizer() {

}

InitStatus BmnGemStripDigitizer::Init() {

    if(fVerbose) cout << "\nBmnGemStripDigitizer::Init()\n ";

    //if GEM configuration is not set -> return a fatal error
    if(!fCurrentConfig) Fatal("BmnGemStripDigitizer::Init()", " !!! Current GEM config is not set !!! ");

    if(fVerbose && fOnlyPrimary) cout << "  Only primary particles are processed!!! " << endl;

    if(fVerbose && fStripMatching) cout << "  Strip Matching is activated!!! " << endl;
    else cout << "  Strip Matching is deactivated!!! " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnGemStripDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "GEM", fBmnGemStripDigitsArray, kTRUE);

    if(fStripMatching) {
        fBmnGemStripDigitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputDigitMatchesBranchName, "GEM", fBmnGemStripDigitMatchesArray, kTRUE);
    }

    if(fVerbose) cout << "BmnGemStripDigitizer::Init() finished\n\n";
    return kSUCCESS;
}

void BmnGemStripDigitizer::Exec(Option_t* opt) {
    clock_t tStart = clock();
    fBmnGemStripDigitsArray->Clear();

    if(fStripMatching) {
        fBmnGemStripDigitMatchesArray->Clear();
    }

    if (!fBmnGemStripPointsArray) {
        Error("BmnGemStripDigitizer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if(fVerbose) {
        cout << "\n BmnGemStripDigitizer::Exec(), event = " << entrys << "\n";
        cout << " BmnGemStripDigitizer::Exec(), Number of BmnGemStripPoints = " << fBmnGemStripPointsArray->GetEntriesFast() << "\n";
    }

    ProcessMCPoints();

    if(fVerbose) cout << " BmnGemStripDigitizer::Exec() finished\n\n";
    entrys++;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnGemStripDigitizer::ProcessMCPoints() {

    FairMCPoint* GemStripPoint;
    Int_t NNotPrimaries = 0;

    BmnGemStripStationSet *StationSet = 0;
    switch (fCurrentConfig) {
        case BmnGemStripConfiguration::Full:
            StationSet = new BmnGemStripStationSet_FullConfig();
            if(fVerbose) cout << "   Current Configuration : FullConfig" << "\n";
            break;
        case BmnGemStripConfiguration::First:
            StationSet = new BmnGemStripStationSet_1stConfig();
            if(fVerbose) cout << "   Current Configuration : FirstConfig" << "\n";
            break;
        case BmnGemStripConfiguration::FirstShort:
            StationSet = new BmnGemStripStationSet_1stConfigShort();
            if(fVerbose) cout << "   Current Configuration : FirstConfig (short version)" << "\n";
            break;
        case BmnGemStripConfiguration::Second:
            StationSet = new BmnGemStripStationSet_2ndConfig();
            if(fVerbose) cout << "   Current Configuration : SecondConfig" << "\n";
            break;
        default:
            StationSet = 0;
    }

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

        Double_t px = -GemStripPoint->GetPx(); // invert because in current geometry +x -left, -x - right
        Double_t py = GemStripPoint->GetPy();
        Double_t pz = GemStripPoint->GetPz();

        Double_t dEloss = GemStripPoint->GetEnergyLoss()*1e6; // in keV
        Int_t refId = ipoint;

        //smearing
        if(fSmearingSigma > 0.0) {
            x = gRandom->Gaus(x, fSmearingSigma);
            y = gRandom->Gaus(y, fSmearingSigma);
        }

        StationSet->AddPointToDetector(x, y, z, px, py, pz, dEloss, refId);
    }

    Int_t NAddedPoints = StationSet->CountNAddedToDetectorPoints();
    if(fVerbose && fOnlyPrimary) cout << "   Number of not primaries points : " << NNotPrimaries << "\n";
    if(fVerbose) cout << "   Processed MC points  : " << NAddedPoints << "\n";

    for(Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnGemStripStation *station = StationSet->GetGemStation(iStation);

        for(Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripReadoutModule *module = station->GetReadoutModule(iModule);

            for(Int_t iLowerStrip = 0; iLowerStrip < module->CountLowerStrips(); ++iLowerStrip) {
                new ((*fBmnGemStripDigitsArray)[fBmnGemStripDigitsArray->GetEntriesFast()])
                    BmnGemStripDigit(iStation, iModule, 0, iLowerStrip, module->GetValueOfLowerStrip(iLowerStrip));

                if(fStripMatching) {
                    new ((*fBmnGemStripDigitMatchesArray)[fBmnGemStripDigitMatchesArray->GetEntriesFast()])
                        BmnMatch(module->GetMatchOfLowerStrip(iLowerStrip));
                }
            }
            for(Int_t iUpperStrip = 0; iUpperStrip < module->CountUpperStrips(); ++iUpperStrip) {
                new ((*fBmnGemStripDigitsArray)[fBmnGemStripDigitsArray->GetEntriesFast()])
                    BmnGemStripDigit(iStation, iModule, 1, iUpperStrip, module->GetValueOfUpperStrip(iUpperStrip));

                if(fStripMatching) {
                    new ((*fBmnGemStripDigitMatchesArray)[fBmnGemStripDigitMatchesArray->GetEntriesFast()])
                        BmnMatch(module->GetMatchOfUpperStrip(iUpperStrip));
                }
            }
        }
    }
    if(StationSet) delete StationSet;
}

void BmnGemStripDigitizer::Finish() {
    cout << "Work time of the GEM digitizer: " << workTime << endl;
}

ClassImp(BmnGemStripDigitizer)
