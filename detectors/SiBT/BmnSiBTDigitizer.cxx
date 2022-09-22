#include "TSystem.h"

#include "BmnSiBTDigitizer.h"
#include "CbmMCTrack.h"
#include "BmnSiBTPoint.h"

static Float_t workTime = 0.0;
static int entrys = 0;

BmnSiBTDigitizer::BmnSiBTDigitizer()
: fOnlyPrimary(kFALSE), fStripMatching(kTRUE) {

    fInputBranchName = "SiBTPoint";
    fOutputDigitsBranchName = "BmnSiBTDigit";
    fOutputDigitMatchesBranchName = "BmnSiBTDigitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnSiBTConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnSiBTDigitizer::~BmnSiBTDigitizer() {
    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }

}

InitStatus BmnSiBTDigitizer::Init() {

    if (fVerbose) cout << "\nBmnSiBTDigitizer::Init()\n ";

    //if the configuration is not set -> return a fatal error
    if (!fCurrentConfig) Fatal("BmnSiBTDigitizer::Init()", " !!! Current SiBT config is not set !!! ");

    if (fVerbose && fOnlyPrimary) cout << "  Only primary particles are processed!!! " << endl;

    if (fVerbose && fStripMatching) cout << "  Strip Matching is activated!!! " << endl;
    else cout << "  Strip Matching is deactivated!!! " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnSiBTPointsArray = (TClonesArray*)ioman->GetObject(fInputBranchName);
    if (!fBmnSiBTPointsArray) {
        cout << "BmnSiBTDigitizer::Init(): branch " << fInputBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    
    fMCTracksArray = (TClonesArray*)ioman->GetObject("MCTrack");

    fBmnSiBTDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "SiBT_DIGIT", fBmnSiBTDigitsArray, kTRUE);

    if (fStripMatching) {
        fBmnSiBTDigitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputDigitMatchesBranchName, "SiBT_DIGIT", fBmnSiBTDigitMatchesArray, kTRUE);
    }

    TString gPathSiBTConfig = gSystem->Getenv("VMCWORKDIR");
        gPathSiBTConfig += "/parameters/SiBT/XMLConfigs/";

    //Create SiBT detector ------------------------------------------------------
    switch (fCurrentConfig) {

        case BmnSiBTConfiguration::Run8:
            StationSet = new BmnSiBTStationSet(gPathSiBTConfig + "SiBTRun8.xml");
            TransfSet = new BmnSiBTTransform();
            TransfSet->LoadFromXMLFile(gPathSiBTConfig + "SiBTRun8.xml");
            if (fVerbose) cout << "   Current SiBT Configuration : Run8" << "\n";
            break;

        default:
            StationSet = nullptr;
    }
    //--------------------------------------------------------------------------

    if (fVerbose) cout << "BmnSiBTDigitizer::Init() finished\n\n";
    return kSUCCESS;
}

void BmnSiBTDigitizer::Exec(Option_t* opt) {
    
    if (!IsActive())
        return;
    
    clock_t tStart = clock();
    fBmnSiBTDigitsArray->Delete();

    if (fStripMatching) {
        fBmnSiBTDigitMatchesArray->Delete();
    }

    if (!fBmnSiBTPointsArray) {
        Error("BmnSiBTDigitizer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) {
        cout << "\n BmnSiBTDigitizer::Exec(), event = " << entrys << "\n";
        cout << " BmnSiBTDigitizer::Exec(), Number of BmnSiBTPoints = " << fBmnSiBTPointsArray->GetEntriesFast() << "\n";
    }

    ProcessMCPoints();

    if (fVerbose) cout << " BmnSiBTDigitizer::Exec() finished\n\n";
    entrys++;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnSiBTDigitizer::ProcessMCPoints() {

    FairMCPoint* SiBTPoint;
    Int_t NNotPrimaries = 0;

    for (UInt_t ipoint = 0; ipoint < fBmnSiBTPointsArray->GetEntriesFast(); ipoint++) {
        SiBTPoint = (FairMCPoint*) fBmnSiBTPointsArray->At(ipoint);

        if (fOnlyPrimary) {
            CbmMCTrack *track = (CbmMCTrack*) fMCTracksArray->UncheckedAt(SiBTPoint->GetTrackID());
            if (!track) continue;
            if (track->GetMotherId() != -1) {
                NNotPrimaries++;
                continue;
            }
        }

        Double_t x = -SiBTPoint->GetX(); // invert because in the current geometry +x is left, -x is right
        Double_t y = SiBTPoint->GetY();
        Double_t z = SiBTPoint->GetZ();

        Double_t px = -SiBTPoint->GetPx(); // invert because in the current geometry +x is left, -x is right
        Double_t py = SiBTPoint->GetPy();
        Double_t pz = SiBTPoint->GetPz();

        Double_t dEloss = SiBTPoint->GetEnergyLoss()*1e6; // in keV
        Int_t refId = ipoint;

        //Information from MC-points
        Int_t mc_station_num = ((BmnSiBTPoint*)SiBTPoint)->GetStation();
        Int_t mc_module_num = ((BmnSiBTPoint*)SiBTPoint)->GetModule();

        //test output
        //cout << "mc_station_num = " << mc_station_num << "\n";
        //cout << "mc_module_num = " << mc_module_num << "\n";

        //Transform mc-point coordinates to local coordinate system of SiBT-planes
        if(TransfSet && mc_station_num < StationSet->GetNStations()) {
            if(mc_module_num < StationSet->GetStation(mc_station_num)->GetNModules()) {
                Plane3D::Point loc_point = TransfSet->ApplyInverseTransforms(Plane3D::Point(-x, y, z), mc_station_num, mc_module_num);
                Plane3D::Point loc_direct = TransfSet->ApplyInverseTransforms(Plane3D::Point(-(px+x), (py+y), (pz+z)), mc_station_num, mc_module_num);
                x = -loc_point.X();
                y = loc_point.Y();
                z = loc_point.Z();

                px = -(loc_direct.X() - loc_point.X());
                py = loc_direct.Y() - loc_point.Y();
                pz = loc_direct.Z() - loc_point.Z();
            }
        }

        StationSet->AddPointToDetector(x, y, z, px, py, pz, dEloss, refId);
    }

    Int_t NAddedPoints = StationSet->CountNAddedToDetectorPoints();
    if (fVerbose && fOnlyPrimary) cout << "   Number of not primaries points : " << NNotPrimaries << "\n";
    if (fVerbose) cout << "   Processed MC points  : " << NAddedPoints << "\n";

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnSiBTStation *station = StationSet->GetStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnSiBTModule *module = station->GetModule(iModule);

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnSiBTLayer layer = module->GetStripLayer(iLayer);

                for (Int_t iStrip = 0; iStrip < layer.GetNStrips(); ++iStrip) {
                    Double_t signal = layer.GetStripSignal(iStrip);
                    if(signal > 0.0) {
                        new ((*fBmnSiBTDigitsArray)[fBmnSiBTDigitsArray->GetEntriesFast()])
                                BmnSiBTDigit(iStation, iModule, iLayer, iStrip, signal);

                        if (fStripMatching) {
                            new ((*fBmnSiBTDigitMatchesArray)[fBmnSiBTDigitMatchesArray->GetEntriesFast()])
                                    BmnMatch(layer.GetStripMatch(iStrip));
                        }
                    }
                }
            }
        }
    }
    StationSet->Reset();
}

void BmnSiBTDigitizer::Finish() {
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if(TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    cout << "Work time of the SiBT digitizer: " << workTime << endl;
}

ClassImp(BmnSiBTDigitizer)
