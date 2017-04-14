// EVENT DISPLAY macro for simulated and experimental data
//
// data source: 0 - event display for simulatated data:
//      sim_run_info - path to the file with MC data and detector geometry
// data source: 1 - event display for experimental data:
//      sim_run_info - run number in 'runN-NN' format, e.g. "run3-642" (the geometry is obtained from the Unified Database)
// reco_file - file with reconstructed data for simulated or experimental events
// is_online: false - use Offline Mode (manual switching of events), default; true - use Online Mode (continious view events)
//void eventdisplay(char* sim_run_info = "run3-642", char* reco_file = "$VMCWORKDIR/macro/run/bmndst.root", int data_source = 1, bool is_online = false)
void eventdisplay(char* sim_run_info = "$VMCWORKDIR/macro/run/evetest.root", char* reco_file = "$VMCWORKDIR/macro/run/bmndst.root", int data_source = 0, bool is_online = false)
{
    gDebug = 0;

    // load main and detectors libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();

    // load Event Display libraries
    gSystem->Load("libEve");
    gSystem->Load("libEventDisplay");

    // CREATE FairRunAna
    FairRunAna* fRun = new FairRunAna();

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
        FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
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
    // FOR EXPERIMENTAL DATA : set source of events to display and addtiional parameters
    else
    {
        TString strRunInfo(sim_run_info);
        Ssiz_t indDash = strRunInfo.First('-');
        if ((indDash > 0) && (strRunInfo.BeginsWith("run")))
        {
            // get run period
            TString number_string(strRunInfo(3, indDash - 3));
            Int_t run_period = number_string.Atoi();
            // get run number
            number_string = strRunInfo(indDash+1, strRunInfo.Length() - indDash-1);
            Int_t run_number = number_string.Atoi();

            // get geometry for run
            TString root_file_path = "current_geo_file.root";
            Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, root_file_path.Data());
            if (res_code != 0)
            {
                cout << "\nGeometry couldn't' be read from the database" << endl;
                return;
            }

            // get gGeoManager from ROOT file (if required)
            TFile* geoFile = new TFile(root_file_path, "READ");
            if (!geoFile->IsOpen())
            {
                cout << "Error: could not open ROOT file with geometry!" << endl;
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
                cout << "Error: TGeoManager isn't top element in geometry file " << root_file_path << endl;
                return;
            }

            // set magnet field with factor corresponding the given run (for GEANE)
            UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
            if (pCurrentRun == 0) return;
            Double_t fieldScale = 0;
            double map_current = 55.87;
            double* field_voltage = pCurrentRun->GetFieldVoltage();
            if (field_voltage == NULL)
                fieldScale = 0;
            else
                fieldScale = (*field_voltage) / map_current;
            BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v3_ascii_Extrap.dat");
            magField->SetScale(fieldScale);
            magField->Init();
            fRun->SetField(magField);
        }
        else
        {
            cout << "Error: run info wasn't found!" << endl;
            return;
        }

        // set source as raw data file
        if (!CheckFileExist(reco_file)) return;
        fFileSource = new BmnFileSource(reco_file);
    }

    fRun->SetSource(fFileSource);

    // Create Event Manager
    FairEventManager* fMan = new FairEventManager();
    fMan->isOnline = is_online;
    fMan->iDataSource = data_source;

    // set output file
    fRun->SetOutputFile("ed_out.root");

    // set tasks to draw
    SetTasks(fMan, data_source);

    // light background color by default
    fMan->background_color = 17;
    fMan->isDarkColor = false;

    // FairEventManager::Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000)
    fMan->Init();
}

// set FairRunAna tasks depending from data source and on/offline mode
class FairEventManager;
void SetTasks(FairEventManager* fMan, int data_source)
{
    Style_t pointMarker = kFullDotSmall;
    Color_t mcPointColor = kRed, recoPointColor = kBlack, expPointColor = kRed;

    // FOR SIMULATION : set drawing tasks
    if (data_source == 0)
    {
        // draw MC points
        FairMCPointDraw* RecoilPoint = new FairMCPointDraw("RecoilPoint", mcPointColor, pointMarker);
        fMan->AddTask(RecoilPoint);
        //FairMCPointDraw* MWPC1Point = new FairMCPointDraw("MWPC1Point", mcPointColor, pointMarker);
        //fMan->AddTask(MWPC1Point);
        //FairMCPointDraw* MWPC2Point = new FairMCPointDraw("MWPC2Point", mcPointColor, pointMarker);
        //fMan->AddTask(MWPC2Point);
        //FairMCPointDraw* MWPC3Point = new FairMCPointDraw("MWPC3Point", mcPointColor, pointMarker);
        //fMan->AddTask(MWPC3Point);
        FairMCPointDraw* TOF1Point = new FairMCPointDraw("TOF1Point", mcPointColor, pointMarker);
        fMan->AddTask(TOF1Point);
        FairMCPointDraw* DCH1Point = new FairMCPointDraw("DCH1Point", mcPointColor, pointMarker);
        fMan->AddTask(DCH1Point);
        FairMCPointDraw* DCH2Point = new FairMCPointDraw("DCH2Point", mcPointColor, pointMarker);
        fMan->AddTask(DCH2Point);
        FairMCPointDraw* TofPoint = new FairMCPointDraw("TofPoint", mcPointColor, pointMarker);
        fMan->AddTask(TofPoint);
        FairMCModuleDraw* PsdPoint = new FairMCModuleDraw("PsdPoint", mcPointColor, pointMarker);
        fMan->AddTask(PsdPoint);
        FairMCPointDraw* StsPoint = new FairMCPointDraw("StsPoint", mcPointColor, pointMarker);
        fMan->AddTask(StsPoint);

        // draw MC geometry tracks
        FairMCTracks* GeoTrack = new FairMCTracks("GeoTracks");
        fMan->AddTask(GeoTrack);

        // or draw MC tracks by Geane - not implemented yet
        //FairMCStack* MCTrack = new FairMCStack("MCTrack");
        //fMan->AddTask(MCTrack);

        // DST hits
        FairHitPointSetDraw* BmnGemHit = new FairHitPointSetDraw("BmnGemStripHit", recoPointColor, pointMarker); // new FairHitDraw("BmnGemStripHit", 1); //in box view
        fMan->AddTask(BmnGemHit);
        FairHitPointSetDraw* TOF1Hit = new FairHitPointSetDraw("TOF1Hit", recoPointColor, pointMarker);
        fMan->AddTask(TOF1Hit);
        FairHitPointSetDraw* BmnDch1Hit = new FairHitPointSetDraw("BmnDchHit_1", recoPointColor, pointMarker);
        fMan->AddTask(BmnDch1Hit);
        FairHitPointSetDraw* BmnDch2Hit = new FairHitPointSetDraw("BmnDchHit_2", recoPointColor, pointMarker);
        fMan->AddTask(BmnDch2Hit);
        FairHitPointSetDraw* BmnTof2Hit = new FairHitPointSetDraw("BmnTof2Hit", recoPointColor, pointMarker);
        fMan->AddTask(BmnTof2Hit);

        // DST tracks
        BmnGlobalTrackDraw* BmnGlobalTrack = new BmnGlobalTrackDraw("GlobalTrack");
        //fMan->AddTask(BmnGlobalTrack);

        // save EventDisplay Screenshot
        //FairWebScreenshots* WebScreenshots = new FairWebScreenshots("WebScreenshots", "/var/www/html/events"); // for WEB-page
        //FairWebScreenshots* WebScreenshots = new FairWebScreenshots("WebScreenshots","screenshots"); // folder to save the screenshots
        //WebScreenshots->SetFormatFiles(0); // 0 -.png, 1 -.jpg, 2 -.jpg and .png
        //WebScreenshots->SetMultiFiles(0); //0 - the same file (event.png), 1 - multiple files (event_nnn.png)
        //WebScreenshots->SetPort(8016); // 8016 by default
        //fMan->AddTask(WebScreenshots);

        return;
    }

    // FOR EXPERIMENTAL DATA : set drawing tasks
    if (data_source == 1)
    {
        // draw MWPC hits
        FairHitPointSetDraw* MwpcHit = new FairHitPointSetDraw("BmnMwpcHit", expPointColor, pointMarker);
        fMan->AddTask(MwpcHit);
        
        // draw GEM hits
        FairHitPointSetDraw* GemHit = new FairHitPointSetDraw("BmnGemStripHit", expPointColor, pointMarker);
        fMan->AddTask(GemHit);

        // draw DCH hits
        FairHitPointSetDraw* DchHit = new FairHitPointSetDraw("BmnDchHit", expPointColor, pointMarker);
        fMan->AddTask(DchHit);

        // draw MWPC tracks
        BmnTrackDrawH* MwpcTrack = new BmnTrackDrawH("MwpcMatchedTracks", "BmnMwpcHit");
        //fMan->AddTask(MwpcTrack);
        
        // draw GEM tracks
        BmnTrackDrawH* GemTrack = new BmnTrackDrawH("BmnGemSeeds", "BmnGemStripHit");
        fMan->AddTask(GemTrack);
        
        // draw DCH tracks
        BmnTrackDrawH* DchTrack = new BmnTrackDrawH("DchTracks", "BmnDchHit");
        //fMan->AddTask(DchTrack);

        /*FairGeane* Geane = new FairGeane();
        fMan->AddTask(Geane);

        CbmTrackDraw* MwpcTrack = new CbmTrackDraw("MwpcMatchedTracks");
        fMan->AddTask(MwpcTrack);*/
    }
}
