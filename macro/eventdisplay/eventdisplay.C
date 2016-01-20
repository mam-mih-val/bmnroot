// sim_geo_file - file with MC data and/or detector geometry
// reco_file - file with reconstructed data for simulation or experimental events
// data source: 0 - event display for simulatated data, event display for experimental data
// is_online: true - use Online Mode (continious view events), false - use Offline Mode (manual switching of events)
void eventdisplay(char* sim_geo_file = "$VMCWORKDIR/macro/run/evetest.root", char* reco_file = "$VMCWORKDIR/macro/run/bmndst.root", int data_source = 0, bool is_online = false)
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    // load main and detectors libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();

    // load TEve libraries
    gSystem->Load("libEve");
    gSystem->Load("libEventDisplay");

    // CREATE FairRunAna
    FairRunAna* fRun = new FairRunAna();

    // Create event manager
    FairEventManager* fMan= new FairEventManager();
    fMan->isOnline = is_online;
    fMan->iDataSource = data_source;


    // FOR SIMULATION
    if (data_source == 0)
    {
        if (!CheckFileExist(sim_geo_file))
        {
            cout<<endl<<"ERROR: Simulation file with detector geometry wasn't found!"<<endl;
            return;
        }

        fRun->SetInputFile(sim_geo_file);

        // set parameter file with simulation data and detector geometry
        FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
        FairParRootFileIo* parIo1 = new FairParRootFileIo();
        parIo1->open(sim_geo_file);
        rtdb->setFirstInput(parIo1);
        rtdb->setOutput(parIo1);
        rtdb->saveOutput();

        // add file with reconstructed data as friend
        if (CheckFileExist(reco_file))
            fRun->AddFriend(reco_file);
        else
            cout<<endl<<"Warning: File with reconstructed data wasn't found!"<<endl;
    }
    // for experimental data
    else
    {
        // check file existence with detector geometry
        if (!CheckFileExist(sim_geo_file))
        {
            cout<<endl<<"ERROR: File with detector geometry wasn't found!"<<endl;
            return;
        }

        fRun->SetInputFile(sim_geo_file);

        // set parameter file with simulation data and detector geometry
        FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
        FairParRootFileIo* parIo1 = new FairParRootFileIo();
        parIo1->open(sim_geo_file);
        rtdb->setFirstInput(parIo1);
        rtdb->setOutput(parIo1);
        rtdb->saveOutput();

        // get gGeoManager from geometry file
        if (!gGeoManager)
        {
            TFile* geoFile = new TFile(sim_geo_file, "READ");
            if (!geoFile->IsOpen())
            {
                cout<<"Error: could not open ROOT file with geometry!"<<endl;
                return;
            }

            TObject* pObj = geoFile->Get("FairBaseParSet");
            if (pObj == NULL)
            {
                TList* keyList = geoFile->GetListOfKeys();
                TIter next(keyList);
                TKey* key = (TKey*)next();
                TString className(key->GetClassName());
                if (className.BeginsWith("TGeoManager"))
                    key->ReadObj();
                else
                {
                    cout<<"Error: TGeoManager isn't top element in given file "<<root_file_path<<endl;
                    return;
                }
            }
        }

        fMan->strExperimentFile = reco_file;

        // set field for Geane (e.g. constant inside the magnet)
        BmnFieldConst* magField = new BmnFieldConst();
        magField->SetFieldRegion(-300., 300., -300., 300., -300., 300);
        magField->SetField(0., -9. * 0.44, 0.);
        fRun->SetField(magField);
    }

    // set output file
    fRun->SetOutputFile("ed_out.root");

    // set tasks to draw
    SetTasks(fMan, data_source);

    fMan->background_color = 17;
    fMan->isDarkColor = false;

    // FairEventManager::Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000)
    fMan->Init();
}


// set FairRunAna tasks depending from data source and on/offline mode
class FairEventManager;
void SetTasks(FairEventManager* fMan, int data_source)
{
    // root files with simulation and reconstructed data
    if (data_source == 0)
    {
        Style_t pointMarker = kFullDotSmall;
        Color_t mcPointColor = kRed, recoPointColor = kBlack;

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
        FairHitPointSetDraw* BmnGemHit = new FairHitPointSetDraw("BmnGemStripHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnGemHit);
        FairHitPointSetDraw* TOF1Hit = new FairHitPointSetDraw("TOF1Hit", recoPointColor, pointMarker);
        fMan->AddTask(TOF1Hit);
        FairHitPointSetDraw* BmnDch1Hit = new FairHitPointSetDraw("BmnDch1Hit0", recoPointColor, pointMarker);
        fMan->AddTask(BmnDch1Hit);
        FairHitPointSetDraw* BmnDch2Hit = new FairHitPointSetDraw("BmnDch2Hit0", recoPointColor, pointMarker);
        fMan->AddTask(BmnDch2Hit);
        FairHitPointSetDraw* BmnTof2Hit = new FairHitPointSetDraw("BmnTof2Hit", recoPointColor, pointMarker);
        fMan->AddTask(BmnTof2Hit);

        // or DST hits in box view
        //FairHitDraw* TpcHit = new FairHitDraw("TpcHit", 1);
        //fMan->AddTask(TpcHit);

        // DST tracks from simulation data
        BmnTrackDraw* BmnGlobalTrack = new BmnTrackDraw("GlobalTrack");
        fMan->AddTask(BmnGlobalTrack);

        // save EventDisplay Screenshot
        //FairWebScreenshots* WebScreenshots = new FairWebScreenshots("WebScreenshots","output32434535");
        //fMan->AddTask(WebScreenshots);

        return;
    }

    // root files with experimental hits and tracks
    if (data_source == 1)
    {
        Style_t pointMarker = kFullDotSmall;
        Color_t pointColor = kRed;

        /** raw data in ROOT file
        // draw MWPC digits
        BmnDigitDraw* MwpcDigit = new BmnDigitDraw("bmn_mwpc_digit", 1, pointColor, pointMarker);
        fMan->AddTask(MwpcDigit);

        // draw DCH digits
        BmnDigitDraw* DchDigit = new BmnDigitDraw("bmn_dch_digit", 2, pointColor, pointMarker);
        fMan->AddTask(DchDigit);**/

        /*  tracks with points in ROOT files
        // draw MWPC hits
        BmnHitDraw* MwpcHit = new BmnHitDraw("BmnMwpcHit", pointColor, pointMarker);
        fMan->AddTask(MwpcHit);

        // draw DCH hits
        BmnHitDraw* DchHit = new BmnHitDraw("BmnDchHit", pointColor, pointMarker);
        fMan->AddTask(DchHit);

        // draw MWPC tracks
        BmnExpTrackDraw* MwpcTrack = new BmnExpTrackDraw("MwpcMatchedTracks", "BmnMwpcHit");
        fMan->AddTask(MwpcTrack);
        
        // draw DCH tracks
        BmnExpTrackDraw* DchTrack = new BmnExpTrackDraw("DchTracks", "BmnDchHit");
        fMan->AddTask(DchTrack);*/

        FairGeane* Geane = new FairGeane();
        fMan->AddTask(Geane);

        CbmTrackDraw* MwpcTrack = new CbmTrackDraw("MwpcMatchedTracks");
        fMan->AddTask(MwpcTrack);
    }
}
