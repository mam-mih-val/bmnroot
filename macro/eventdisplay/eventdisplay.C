#include "../run/bmnloadlibs.C"
// common EVENT DISPLAY macro for simulated and experimental data
//
// data source: 0 - event display for simulatated data (it shows both MC and reconstructed simulated data):
//      sim_run_info - path to the ROOT file with MC data and detector geometry
//      reco_file - ROOT file with reconstructed data for simulated events
// data source: 1 - offline event display for reconstructed experimental data (it can work in online continious view):
//      sim_run_info - run number in 'runN-NN' format, e.g. "run3-642" to obtain BM@N geometry from the Unified Database
//      reco_file - ROOT file with reconstructed data for experimental events
// data source: 2 - online event display for raw experimental data from directory (it can work in offline mode for last modified file in the directory):
//                  it uses ATLAS TDAQ components, so TDAQ should be present on your machine (FindTDAQ.cmake script)
//      sim_run_info - run number in 'runN-NN' format, e.g. "run6-1" to obtain BM@N geometry from the Unified Database
//      reco_file - path to the directory with raw '*.data' files, e.g. "/tdaq/data/" (last slash is required)
// is_online: false (default) - use Offline Mode (manual switching of events); true - use Online Mode (continious view events)
//void eventdisplay(char* sim_run_info = "run6-1220", char* reco_file = "$VMCWORKDIR/macro/run/bmn_run1220.root", int data_source = 1, bool is_online = false)
//void eventdisplay(char* sim_run_info = "run6-1220", char* reco_file = "/tdaq/data/", int data_source = 2, bool is_online = true)
void eventdisplay(char* sim_run_info = "$VMCWORKDIR/macro/run/evetest.root", char* reco_file = "$VMCWORKDIR/macro/run/bmndst.root", int data_source = 0, bool is_online = false)
{
    gDebug = 0;

    // load main and detectors libraries
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs();

    // load Event Display libraries
    gSystem->Load("libEve");
    gSystem->Load("libEventDisplay");

    // CREATE FairRunAna
    FairRunAna* fRunAna = new FairRunAna();

    int run_period = -1, run_number = -1;
    bool isTarget = false, isField = true;
    FairSource* fFileSource = NULL;

    // FOR SIMULATION : set source of events to display and addtiional parameters
    if (data_source == 0)
    {
        // check file existence with MC data and detector geometry
        if (!CheckFileExist(sim_run_info))
        {
            cout<<endl<<"ERROR: Simulation file with detector geometry wasn't found!"<<endl;
            return;
        }

        fFileSource = new FairFileSource(sim_run_info);

        // set parameter file with MC data and detector geometry
        FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
        FairParRootFileIo* parIo1 = new FairParRootFileIo();
        parIo1->open(sim_run_info);
        rtdb->setFirstInput(parIo1);
        rtdb->setOutput(parIo1);
        rtdb->saveOutput();

        // add file with reconstructed data as a friend
        if (CheckFileExist(reco_file))
            ((FairFileSource*)fFileSource)->AddFriend(reco_file);
        else
            cout<<endl<<"Warning: File with reconstructed data wasn't found!"<<endl;
    }
    // FOR EXPERIMENTAL DATA
    // FROM RECONSTRUCTED ROOT FILE (data_source == 1), FROM DIRECTORY WITH RAW .DATA FILES (data_source == 2)
    else
    {
        TString strRunInfo(sim_run_info);
        Ssiz_t indDash = strRunInfo.First('-');
        if ((indDash > 0) && (strRunInfo.BeginsWith("run")))
        {
            // get run period
            run_period = TString(strRunInfo(3, indDash - 3)).Atoi();
            // get run number
            run_number = TString(strRunInfo(indDash+1, strRunInfo.Length() - indDash-1)).Atoi();

            // get geometry for run
            TString root_file_path = "current_geo_file.root";
            Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, (char*)root_file_path.Data());
            if (res_code != 0)
            {
                cout << "\nGeometry couldn't' be read from the database" << endl;
                return;
            }

            // get gGeoManager from ROOT file (if required)
            TFile* geoFile = new TFile(root_file_path, "READ");
            if (!geoFile->IsOpen())
            {
                cout<<"Error: could not open ROOT file with geometry!"<<endl;
                return;
            }
            TList* keyList = geoFile->GetListOfKeys();
            TIter next(keyList);
            TKey* key = (TKey*) next();
            TString className(key->GetClassName());
            if (className.BeginsWith("TGeoManager"))
                key->ReadObj();
            else
            {
                cout<<"Error: TGeoManager isn't top element in geometry file "<<root_file_path<<endl;
                return;
            }

            // set magnet field with factor corresponding the given run (for GEANE)
            UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
            if (pCurrentRun == 0)
                return;

            Double_t fieldScale = 0.0;
            Double_t map_current  = 55.87;
            Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
            if ((field_voltage == NULL) || (*field_voltage < 10))
            {
                fieldScale = 0;
                isField = kFALSE;
            }
            else
                fieldScale = (*field_voltage) / map_current;
            BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.dat");
            magField->SetScale(fieldScale);
            magField->Init();
            fRunAna->SetField(magField);
            TString targ = "-", beam = pCurrentRun->GetBeamParticle();
            if (pCurrentRun->GetTargetParticle() != NULL)
            {
                isTarget = kTRUE;
                targ = *(pCurrentRun->GetTargetParticle());
            }

            cout << "\n\n|||||||||||||||| EXPERIMENTAL RUN SUMMARY ||||||||||||||||" << endl;
            cout << "||\t\tPeriod:\t\t" << run_period << "\t\t\t||" << endl;
            cout << "||\t\tNumber:\t\t" << run_number << "\t\t\t||" << endl;
            cout << "||\t\tBeam:\t\t" << beam << "\t\t\t||" << endl;
            cout << "||\t\tTarget:\t\t" << targ << "\t\t\t||" << endl;
            cout << "||\t\tField scale:\t" << setprecision(4) << fieldScale << "\t\t\t||" << endl;
            cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n" << endl;
        }
        else
        {
            cout<<"Error: run info wasn't found!"<<endl;
            return;
        }

        if (!CheckFileExist(reco_file)) return;

        // set source as raw data file
        if (data_source == 1)
            fFileSource = new BmnFileSource(reco_file);
        // set source as TDAQ Event Monitor
        if (data_source == 2)
        {
            // load TDAQ
            gSystem->Load("libemon");
            gSystem->Load("libemon-dal");
            gSystem->Load("libcmdline");
            gSystem->Load("libipc");
            gSystem->Load("libowl");
            gSystem->Load("libomniORB4");
            gSystem->Load("libomnithread");
            gSystem->Load("libers");

            fFileSource = new BmnTdaqSource("bmn", "raw", "file", 2);
        }
    }

    if (fFileSource != NULL) fRunAna->SetSource(fFileSource);

    // Create Event Manager
    FairEventManager* fMan = new FairEventManager();
    fMan->isOnline = is_online;
    fMan->iDataSource = data_source;

    // set output file
    fRunAna->SetOutputFile("ed_out.root");

    // set tasks to draw
    SetTasks(fMan, data_source, run_period, run_number, isField, isTarget);

    // light background color by default
    fMan->background_color = 17;
    fMan->isDarkColor = false;

    // FairEventManager::Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000)
    fMan->Init();
}

// set FairRunAna drawing tasks depending from data source and on/offline mode
class FairEventManager;
void SetTasks(FairEventManager* fMan, int data_source, int run_period, int run_number, bool isField, bool isTarget)
{
    Style_t pointMarker = kFullDotSmall;
    Color_t mcPointColor = kRed, recoPointColor = kBlack, expPointColor = kRed;

    // FOR SIMULATION
    if (data_source == 0)
    {
        // draw MC Points
        //FairMCPointDraw* RecoilPoint = new FairMCPointDraw("RecoilPoint", mcPointColor, pointMarker);
        //fMan->AddTask(RecoilPoint);
        FairMCPointDraw* MWPCPoint = new FairMCPointDraw("MWPCPoint", mcPointColor, pointMarker);
        fMan->AddTask(MWPCPoint);
        FairMCPointDraw* TOF1Point = new FairMCPointDraw("TOF1Point", mcPointColor, pointMarker);
        fMan->AddTask(TOF1Point);
        FairMCPointDraw* DCHPoint = new FairMCPointDraw("DCHPoint", mcPointColor, pointMarker);
        fMan->AddTask(DCHPoint);
        FairMCPointDraw* TofPoint = new FairMCPointDraw("TOFPoint", mcPointColor, pointMarker);
        fMan->AddTask(TofPoint);
        FairMCModuleDraw* PsdPoint = new FairMCModuleDraw("PsdPoint", mcPointColor, pointMarker);
        fMan->AddTask(PsdPoint);
        FairMCPointDraw* StsPoint = new FairMCPointDraw("StsPoint", mcPointColor, pointMarker);
        fMan->AddTask(StsPoint);
        FairMCPointDraw* SiliconPoint = new FairMCPointDraw("SiliconPoint", mcPointColor, pointMarker);
        fMan->AddTask(SiliconPoint);

        // draw MC Geometry Tracks
        FairMCTracks* GeoTrack = new FairMCTracks("GeoTracks");
        fMan->AddTask(GeoTrack);
        // OR draw MC tracks by Geane - not implemented yet
        //FairMCStack* MCTrack = new FairMCStack("MCTrack");
        //fMan->AddTask(MCTrack);

        // draw Reconstructed Detector Hits
        FairHitPointSetDraw* BmnGemHit = new FairHitPointSetDraw("BmnGemStripHit", recoPointColor, pointMarker); // new FairHitDraw("BmnGemStripHit", 1); //in box view
        fMan->AddTask(BmnGemHit);
        FairHitPointSetDraw* BmnTof1Hit = new FairHitPointSetDraw("BmnTof1Hit", recoPointColor, pointMarker);
        fMan->AddTask(BmnTof1Hit);
        FairHitPointSetDraw* BmnDchHit = new FairHitPointSetDraw("BmnDchHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnDchHit);
        FairHitPointSetDraw* BmnTof2Hit = new FairHitPointSetDraw("BmnTofHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnTof2Hit);
        FairHitPointSetDraw* BmnSiliconHit = new FairHitPointSetDraw("BmnSiliconHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnSiliconHit);

        // draw Reconstructed Global Tracks
        BmnGlobalTrackDraw* BmnGlobalTrack = new BmnGlobalTrackDraw("BmnGlobalTrack");
        fMan->AddTask(BmnGlobalTrack);

        // save EventDisplay Screenshot
        //FairWebScreenshots* WebScreenshots = new FairWebScreenshots("WebScreenshots", "/var/www/html/events"); // for WEB-page
        //FairWebScreenshots* WebScreenshots = new FairWebScreenshots("WebScreenshots","screenshots"); // folder to save the screenshots
        //WebScreenshots->SetFormatFiles(0); // 0 -.png, 1 -.jpg, 2 -.jpg and .png
        //WebScreenshots->SetMultiFiles(0); //0 - the same file (event.png), 1 - multiple files (event_nnn.png)
        //WebScreenshots->SetPort(8016); // 8016 by default
        //fMan->AddTask(WebScreenshots);

        return;
    }

    // FOR EXPERIMENTAL DATA FROM RECONSTRUCTED ROOT FILE
    if (data_source == 1)
    {
        // draw MWPC Hits
        FairHitPointSetDraw* MwpcHit = new FairHitPointSetDraw("BmnMwpcHit", expPointColor, pointMarker);
        fMan->AddTask(MwpcHit);
        // draw Silicon Hits
        FairHitPointSetDraw* SiliconHit = new FairHitPointSetDraw("BmnSiliconHit", expPointColor, pointMarker);
        fMan->AddTask(SiliconHit);
        // draw GEM Hits
        FairHitPointSetDraw* GemHit = new FairHitPointSetDraw("BmnGemStripHit", expPointColor, pointMarker);
        fMan->AddTask(GemHit);
        // draw DCH Hits
        FairHitPointSetDraw* DchHit = new FairHitPointSetDraw("BmnDchHit", expPointColor, pointMarker);
        fMan->AddTask(DchHit);
        // draw TOF1 Hits
        FairHitPointSetDraw* Tof1Hit = new FairHitPointSetDraw("BmnTof1Hit", expPointColor, pointMarker);
        fMan->AddTask(Tof1Hit);
        // draw TOF2 Hits
        FairHitPointSetDraw* Tof2Hit = new FairHitPointSetDraw("BmnTofHit", expPointColor, pointMarker);
        fMan->AddTask(Tof2Hit);

        // draw MWPC Tracks
        BmnTrackDrawH* MwpcTrack = new BmnTrackDrawH("BmnMwpcTrack", "BmnMwpcHit");
        fMan->AddTask(MwpcTrack);
        // draw GEM Tracks
        BmnTrackDrawH* GemTrack = new BmnTrackDrawH("BmnGemTrack", "BmnGemStripHit");
        fMan->AddTask(GemTrack);
        // draw DCH Tracks
        //BmnTrackDrawH* DchTrack = new BmnTrackDrawH("BmnDchTrack", "BmnDchHit");
        //fMan->AddTask(DchTrack);

        // draw Reconstructed Global Tracks
        BmnGlobalTrackDraw* BmnGlobalTrack = new BmnGlobalTrackDraw("BmnGlobalTrack");
        fMan->AddTask(BmnGlobalTrack);

        //FairGeane* Geane = new FairGeane();
        //fMan->AddTask(Geane);
        //CbmTrackDraw* MwpcTrack = new CbmTrackDraw("MwpcMatchedTracks");
        //fMan->AddTask(MwpcTrack);
    }

    // FOR EXPERIMENTAL DATA FROM DIRECTORY WITH .DATA FILES
    if (data_source == 2)
    {
        // GEM hit finder
        BmnGemStripConfiguration::GEM_CONFIG gem_config;
        if (run_period == 6)
            gem_config = BmnGemStripConfiguration::RunSpring2017;
        else if (run_period == 5)
            gem_config = BmnGemStripConfiguration::RunWinter2016;
        BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(true);
        gemHM->SetCurrentConfig(gem_config);
        gemHM->SetAlignmentCorrectionsFileName(run_period, run_number);
        gemHM->SetHitMatching(kTRUE);
        gemHM->SetVerbose(0);
        fMan->AddTask(gemHM);

        // Tracking GEM
        BmnGemTracking* gemTF = new BmnGemTracking();
        gemTF->SetTarget(isTarget);
        gemTF->SetField(isField);
        TVector3 vAppr = TVector3(0.0, -3.5, -21.7);
        gemTF->SetRoughVertex(vAppr);
        gemTF->SetVerbose(1);
        fMan->AddTask(gemTF);

        // TOF-400 hit finder
        BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", false, 0/*iVerbose*/, kTRUE);
        fMan->AddTask(tof1HP);

        // draw GEM hits
        FairHitPointSetDraw* GemHit = new FairHitPointSetDraw("BmnGemStripHit", expPointColor, pointMarker);
        GemHit->SetVerbose(1);
        fMan->AddTask(GemHit);

        // draw TOF-400 hits
        FairHitPointSetDraw* Tof1Hit = new FairHitPointSetDraw("BmnTof1Hit", expPointColor, pointMarker);
        Tof1Hit->SetVerbose(1);
        fMan->AddTask(Tof1Hit);

        // draw GEM seeds or tracks
        BmnTrackDrawH* GemTrack = new BmnTrackDrawH("BmnGemTrack", "BmnGemStripHit");
        GemTrack->SetVerbose(1);
        fMan->AddTask(GemTrack);
    }
}
