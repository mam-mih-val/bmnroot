#include "BmnSiliconDigitizer.h"

#include "TSystem.h"

#include "CbmMCTrack.h"
#include "BmnSiliconPoint.h"


static Float_t workTime = 0.0;
static int entries = 0;

BmnSiliconDigitizer::BmnSiliconDigitizer()
: fOnlyPrimary(kFALSE), fStripMatching(kTRUE), fUseRealEffects(kFALSE) {

    fInputBranchName = "SiliconPoint";
    fOutputDigitsBranchName = "BmnSiliconDigit";
    fOutputDigitMatchesBranchName = "BmnSiliconDigitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnSiliconConfiguration::None;
    StationSet = nullptr;
    fField = nullptr;
    TransfSet = nullptr;
}

BmnSiliconDigitizer::~BmnSiliconDigitizer() {
    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }
}

InitStatus BmnSiliconDigitizer::Init() {

    if (fVerbose) cout << "\nBmnSiliconDigitizer::Init()\n ";

    if (fVerbose && fOnlyPrimary) cout << "  Only primary particles are processed!!! " << endl;

    if (fVerbose && fStripMatching) cout << "  Strip Matching is activated!!! " << endl;
    else cout << "  Strip matching is deactivated!!! " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnSiliconPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnSiliconDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "SILICON", fBmnSiliconDigitsArray, kTRUE);

    if (fStripMatching) {
        fBmnSiliconDigitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputDigitMatchesBranchName, "SILICON", fBmnSiliconDigitMatchesArray, kTRUE);
    }

    TString gPathSiliconConfig = gSystem->Getenv("VMCWORKDIR");
    gPathSiliconConfig += "/parameters/silicon/XMLConfigs/";

    //Create SILICON detector ------------------------------------------------------
    switch (fCurrentConfig) {
        case BmnSiliconConfiguration::RunSpring2017:
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSpring2017.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : RunSpring2017" << "\n";
            break;

        case BmnSiliconConfiguration::RunSpring2018:
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSpring2018.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : RunSpring2018" << "\n";
            break;

        case BmnSiliconConfiguration::RunSRCSpring2018:
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSRCSpring2018.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : RunSRCSpring2018" << "\n";
            break;

        case BmnSiliconConfiguration::Run8_3stations:
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRun8_3stations.xml");
            TransfSet = new BmnSiliconTransform();
            TransfSet->LoadFromXMLFile(gPathSiliconConfig + "SiliconRun8_3stations.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : SiliconRun8_3stations" << "\n";
            break;

        case BmnSiliconConfiguration::Run8_4stations:
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRun8_4stations.xml");
            TransfSet = new BmnSiliconTransform();
            TransfSet->LoadFromXMLFile(gPathSiliconConfig + "SiliconRun8_4stations.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : SiliconRun8_4stations" << "\n";
            break;

        case BmnSiliconConfiguration::Run8_5stations:
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRun8_5stations.xml");
            TransfSet = new BmnSiliconTransform();
            TransfSet->LoadFromXMLFile(gPathSiliconConfig + "SiliconRun8_5stations.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : SiliconRun8_5stations" << "\n";
            break;

        case BmnSiliconConfiguration::Run8_mods_6_10_14_18:
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRun8_mods_6_10_14_18.xml");
            TransfSet = new BmnSiliconTransform();
            TransfSet->LoadFromXMLFile(gPathSiliconConfig + "SiliconRun8_mods_6_10_14_18.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : SiliconRun8_mods_6_10_14_18" << "\n";
            break;

        case BmnSiliconConfiguration::Run8_mods_10_14rot_18:
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRun8_mods_10_14rot_18.xml");
            TransfSet = new BmnSiliconTransform();
            TransfSet->LoadFromXMLFile(gPathSiliconConfig + "SiliconRun8_mods_10_14rot_18.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : SiliconRun8_mods_10_14rot_18" << "\n";
            break;

        default:
            StationSet = nullptr;
    }
    //--------------------------------------------------------------------------

    if (fVerbose) cout << "BmnSiliconDigitizer::Init() finished\n\n";
    return kSUCCESS;
}

void BmnSiliconDigitizer::Exec(Option_t* opt) {
    clock_t tStart = clock();
    fBmnSiliconDigitsArray->Delete();

    if (fStripMatching) {
        fBmnSiliconDigitMatchesArray->Delete();
    }

    if (!fBmnSiliconPointsArray) {
        Error("BmnSiliconDigitizer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) {
        cout << "\n BmnSiliconDigitizer::Exec(), event = " << entries << "\n";
        cout << " BmnSiliconDigitizer::Exec(), Number of BmnSiliconPoints = " << fBmnSiliconPointsArray->GetEntriesFast() << "\n";
    }

    ProcessMCPoints();

    if (fVerbose) cout << " BmnSiliconDigitizer::Exec() finished\n\n";
    entries++;
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

        //Information from MC-points
        Int_t mc_station_num = ((BmnSiliconPoint*) SiliconPoint)->GetStation();
        Int_t mc_module_num = ((BmnSiliconPoint*) SiliconPoint)->GetModule();

        //test output
        //cout << "mc_station_num = " << mc_station_num << "\n";
        //cout << "mc_module_num = " << mc_module_num << "\n";

        //Transform MC-point coordinates to local coordinate system
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
        BmnSiliconStation *station = StationSet->GetSiliconStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnSiliconModule *module = station->GetModule(iModule);
            vector<Int_t> processed_zones;

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnSiliconLayer layer = module->GetStripLayer(iLayer);

                Int_t zone_id = layer.GetZoneID();

                Bool_t is_processed_zone = false;
                for (Int_t iz = 0; iz < processed_zones.size(); ++iz) {
                    if (zone_id == processed_zones[iz]) is_processed_zone = true;
                }

                if (!is_processed_zone) {
                    Int_t first_strip_in_zone = module->GetFirstStripInZone(zone_id);
                    Int_t last_strip_in_zone = module->GetLastStripInZone(zone_id);

                    for (Int_t iStrip = first_strip_in_zone; iStrip < last_strip_in_zone + 1; ++iStrip) {
                        Double_t signal = module->GetStripSignalInZone(zone_id, iStrip);

                        if (signal > 0.0) {
                            signal = signal / (120. / 500.0); // el to ADC
                            if (signal <= 12) continue;
                            if (signal >= 2048) signal = 2047;
                            new ((*fBmnSiliconDigitsArray)[fBmnSiliconDigitsArray->GetEntriesFast()])
                                    BmnSiliconDigit(iStation, iModule, zone_id, iStrip, signal); // zone_id == layer_num !!!!!!

                            if (fStripMatching) {
                                new ((*fBmnSiliconDigitMatchesArray)[fBmnSiliconDigitMatchesArray->GetEntriesFast()])
                                        BmnMatch(module->GetStripMatchInZone(zone_id, iStrip));
                            }
                        }
                    }

                    processed_zones.push_back(zone_id);
                }
            }
        }
    }

    StationSet->Reset();
}

void BmnSiliconDigitizer::Finish() {
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if(TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    cout << "Work time of the Silicon digitizer: " << workTime << endl;
}

ClassImp(BmnSiliconDigitizer)


