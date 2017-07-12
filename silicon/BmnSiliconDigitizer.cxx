#include "BmnSiliconDigitizer.h"

#include "TSystem.h"

#include "CbmMCTrack.h"


static Float_t workTime = 0.0;
static int entrys = 0;

BmnSiliconDigitizer::BmnSiliconDigitizer()
: fOnlyPrimary(kFALSE) {

    fInputBranchName = "SiliconPoint";
    fOutputDigitsBranchName = "BmnSiliconDigit";

    fVerbose = 1;

    StationSet = NULL;
}

BmnSiliconDigitizer::~BmnSiliconDigitizer() {

}

InitStatus BmnSiliconDigitizer::Init() {

    if (fVerbose) cout << "\nBmnSiliconDigitizer::Init()\n ";

    if (fVerbose && fOnlyPrimary) cout << "  Only primary particles are processed!!! " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnSiliconPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnSiliconDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "SILICON", fBmnSiliconDigitsArray, kTRUE);

    TString gPathSiliconConfig = gSystem->Getenv("VMCWORKDIR");
        gPathSiliconConfig += "/silicon/XMLConfigs/";

    //Create Silicon detector ------------------------------------------------------

    //StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSpring2017.xml");
    cout << "   Current Configuration : RunSpring2017" << "\n";

    //--------------------------------------------------------------------------

    if (fVerbose) cout << "BmnSiliconDigitizer::Init() finished\n\n";
    return kSUCCESS;
}

void BmnSiliconDigitizer::Exec(Option_t* opt) {
    clock_t tStart = clock();
    fBmnSiliconDigitsArray->Delete();

    if (!fBmnSiliconPointsArray) {
        Error("BmnGemStripDigitizer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) {
        cout << "\n BmnSiliconDigitizer::Exec(), event = " << entrys << "\n";
        cout << " BmnSiliconDigitizer::Exec(), Number of BmnSiliconPoints = " << fBmnSiliconPointsArray->GetEntriesFast() << "\n";
    }

    ProcessMCPoints();

    if (fVerbose) cout << " BmnSiliconDigitizer::Exec() finished\n\n";
    entrys++;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnSiliconDigitizer::ProcessMCPoints() {

    FairMCPoint* SiliconPoint;
    Int_t NNotPrimaries = 0;

    for (UInt_t ipoint = 0; ipoint < fBmnSiliconPointsArray->GetEntriesFast(); ipoint++) {
        SiliconPoint = (FairMCPoint*) fBmnSiliconPointsArray->At(ipoint);

        if (fOnlyPrimary) {
            CbmMCTrack *track = (CbmMCTrack*) fMCTracksArray->UncheckedAt(SiliconPoint->GetTrackID());
            if (!track) continue;
            if (track->GetMotherId() != -1) {
                NNotPrimaries++;
                continue;
            }
        }

        Double_t x = -SiliconPoint->GetX(); // invert because in current geometry +x is left, -x is right
        Double_t y = SiliconPoint->GetY();
        Double_t z = SiliconPoint->GetZ();

        Double_t px = -SiliconPoint->GetPx(); // invert because in current geometry +x is left, -x is right
        Double_t py = SiliconPoint->GetPy();
        Double_t pz = SiliconPoint->GetPz();

        Double_t dEloss = SiliconPoint->GetEnergyLoss()*1e6; // in keV
        Int_t refId = ipoint;

        StationSet->AddPointToDetector(x, y, z, px, py, pz, dEloss, refId);
    }

    Int_t NAddedPoints = StationSet->CountNAddedToDetectorPoints();
    if (fVerbose && fOnlyPrimary) cout << "   Number of not primaries points : " << NNotPrimaries << "\n";
    if (fVerbose) cout << "   Processed MC points  : " << NAddedPoints << "\n";

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnSiliconStation *station = StationSet->GetSiliconStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnSiliconModule *module = station->GetModule(iModule);

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnSiliconLayer layer = module->GetStripLayer(iLayer);

                for (Int_t iStrip = 0; iStrip < layer.GetNStrips(); ++iStrip) {
                    new ((*fBmnSiliconDigitsArray)[fBmnSiliconDigitsArray->GetEntriesFast()])
                            BmnSiliconDigit(iStation, iModule, iLayer, iStrip, layer.GetStripSignal(iStrip));
                }
            }
        }
    }
    StationSet->Reset();
}

void BmnSiliconDigitizer::Finish() {
    if (StationSet) {
        delete StationSet;
        StationSet = NULL;
    }

    cout << "Work time of the Silicon digitizer: " << workTime << endl;
}

ClassImp(BmnSiliconDigitizer)


