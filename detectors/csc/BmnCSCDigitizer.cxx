#include "TSystem.h"

#include "BmnCSCDigitizer.h"
#include "CbmMCTrack.h"
#include "BmnCSCPoint.h"

static Float_t workTime = 0.0;
static int entrys = 0;

BmnCSCDigitizer::BmnCSCDigitizer()
: fOnlyPrimary(kFALSE), fStripMatching(kTRUE) {

    fInputBranchName = "CSCPoint";
    fOutputDigitsBranchName = "BmnCSCDigit";
    fOutputDigitMatchesBranchName = "BmnCSCDigitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnCSCConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnCSCDigitizer::~BmnCSCDigitizer() {
    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }
}

InitStatus BmnCSCDigitizer::Init() {

    if (fVerbose) cout << "\nBmnCSCDigitizer::Init()\n ";

    //if the configuration is not set -> return a fatal error
    if (!fCurrentConfig) Fatal("BmnCSCDigitizer::Init()", " !!! Current CSC config is not set !!! ");

    if (fVerbose && fOnlyPrimary) cout << "  Only primary particles are processed!!! " << endl;

    if (fVerbose && fStripMatching) cout << "  Strip Matching is activated!!! " << endl;
    else cout << "  Strip Matching is deactivated!!! " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnCSCPointsArray = (TClonesArray*)ioman->GetObject(fInputBranchName);
    if (!fBmnCSCPointsArray) {
        cout << "BmnCSCDigitizer::Init(): branch " << fInputBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fMCTracksArray = (TClonesArray*)ioman->GetObject("MCTrack");

    fBmnCSCDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "CSC_DIGIT", fBmnCSCDigitsArray, kTRUE);

    if (fStripMatching) {
        fBmnCSCDigitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputDigitMatchesBranchName, "CSC_DIGIT", fBmnCSCDigitMatchesArray, kTRUE);
    }

    TString gPathCSCConfig = gSystem->Getenv("VMCWORKDIR");
        gPathCSCConfig += "/parameters/csc/XMLConfigs/";

    //Create CSC detector ------------------------------------------------------
    switch (fCurrentConfig) {
        case BmnCSCConfiguration::RunSpring2018 :
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRunSpring2018.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRunSpring2018.xml");
            if (fVerbose) cout << "   Current CSC Configuration : RunSpring2018" << "\n";
            break;

        case BmnCSCConfiguration::RunSRCSpring2018:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRunSRCSpring2018.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRunSRCSpring2018.xml");
            if (fVerbose) cout << "   Current CSC Configuration : RunSRCSpring2018" << "\n";
            break;

        case BmnCSCConfiguration::Run8:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRun8.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRun8.xml");
            if (fVerbose) cout << "   Current CSC Configuration : Run8" << "\n";
            break;

        case BmnCSCConfiguration::RunSRC2021:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRunSRC2021.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRunSRC2021.xml");
            if (fVerbose) cout << "   Current CSC Configuration : RunSRC2021" << "\n";
            break;

        case BmnCSCConfiguration::LargeCSC_Run8:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "LargeCSCRun8.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "LargeCSCRun8.xml");
            if (fVerbose) cout << "   Current CSC Configuration : LargeCSCRun8" << "\n";
            break;

        default:
            StationSet = nullptr;
    }
    //--------------------------------------------------------------------------

    if (fVerbose) cout << "BmnCSCDigitizer::Init() finished\n\n";
    return kSUCCESS;
}

void BmnCSCDigitizer::Exec(Option_t* opt) {

    if (!IsActive())
        return;

    clock_t tStart = clock();
    fBmnCSCDigitsArray->Delete();

    if (fStripMatching) {
        fBmnCSCDigitMatchesArray->Delete();
    }

    // if (!fBmnCSCPointsArray) {
    //     Error("BmnCSCDigitizer::Exec()", " !!! Unknown branch name !!! ");
    //     return;
    // }

    if (fVerbose) {
        cout << "\n BmnCSCDigitizer::Exec(), event = " << entrys << "\n";
        cout << " BmnCSCDigitizer::Exec(), Number of BmnCSCPoints = " << fBmnCSCPointsArray->GetEntriesFast() << "\n";
    }

    ProcessMCPoints();

    if (fVerbose) cout << " BmnCSCDigitizer::Exec() finished\n\n";
    entrys++;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnCSCDigitizer::ProcessMCPoints() {

    FairMCPoint* CSCPoint;
    Int_t NNotPrimaries = 0;

    for (UInt_t ipoint = 0; ipoint < fBmnCSCPointsArray->GetEntriesFast(); ipoint++) {
        CSCPoint = (FairMCPoint*) fBmnCSCPointsArray->At(ipoint);

        if (fOnlyPrimary) {
            CbmMCTrack *track = (CbmMCTrack*) fMCTracksArray->UncheckedAt(CSCPoint->GetTrackID());
            if (!track) continue;
            if (track->GetMotherId() != -1) {
                NNotPrimaries++;
                continue;
            }
        }

        Double_t x = -CSCPoint->GetX(); // invert because in the current geometry +x is left, -x is right
        Double_t y = CSCPoint->GetY();
        Double_t z = CSCPoint->GetZ();

        Double_t px = -CSCPoint->GetPx(); // invert because in the current geometry +x is left, -x is right
        Double_t py = CSCPoint->GetPy();
        Double_t pz = CSCPoint->GetPz();

        Double_t dEloss = CSCPoint->GetEnergyLoss()*1e6; // in keV
        Int_t refId = ipoint;

        //Information from MC-points
        Int_t mc_station_num = ((BmnCSCPoint*)CSCPoint)->GetStation();
        Int_t mc_module_num = ((BmnCSCPoint*)CSCPoint)->GetModule();

        //test output
        //cout << "mc_station_num = " << mc_station_num << "\n";
        //cout << "mc_module_num = " << mc_module_num << "\n";

        //Transform mc-point coordinates to local coordinate system of CSC-planes
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
        BmnCSCStation *station = StationSet->GetStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnCSCModule *module = station->GetModule(iModule);

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnCSCLayer layer = module->GetStripLayer(iLayer);

                for (Int_t iStrip = 0; iStrip < layer.GetNStrips(); ++iStrip) {
                    Double_t signal = layer.GetStripSignal(iStrip);
                    if(signal > 0.0) {
                        new ((*fBmnCSCDigitsArray)[fBmnCSCDigitsArray->GetEntriesFast()])
                                BmnCSCDigit(iStation, iModule, iLayer, iStrip, signal);

                        if (fStripMatching) {
                            new ((*fBmnCSCDigitMatchesArray)[fBmnCSCDigitMatchesArray->GetEntriesFast()])
                                    BmnMatch(layer.GetStripMatch(iStrip));
                        }
                    }
                }
            }
        }
    }
    StationSet->Reset();
}

void BmnCSCDigitizer::Finish() {
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if(TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    cout << "Work time of the CSC digitizer: " << workTime << endl;
}

ClassImp(BmnCSCDigitizer)
