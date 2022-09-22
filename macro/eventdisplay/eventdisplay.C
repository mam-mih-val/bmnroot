void SetTasks(MpdEventManager* fMan, bool isExp, int run_period, int run_number, bool isField, bool isTarget);

// EVENT DISPLAY macro for experimental and simulated (both MC and reconstructed simulated data) data
//    reco_file - ROOT file with reconstructed data for simulated or experimetal events
//    sim_file  - ROOT file with MC data (and detector geometry) in case of MC visualization
//    is_online - false (default) to use Offline Mode (manual switching of events); true to use Online Mode (continious view events)
//
// void eventdisplay(const char* reco_file = "$VMCWORKDIR/macro/run/bmn_run1220.root", const char* sim_file = "", bool is_online = false)
void eventdisplay(const char* reco_file = "$VMCWORKDIR/macro/run/bmndst.root",
                  const char* sim_file = "$VMCWORKDIR/macro/run/bmnsim.root",
                  bool is_online = false)
{
    gDebug = 0; // Debug option

    // create FairRunAna
    FairRunAna* fRunAna = new FairRunAna();

    // check reconstruction file exists
    TString strRecoFile(reco_file), strSimFile(sim_file);
    if (!BmnFunctionSet::CheckFileExist(strRecoFile, 1)) return;

    Int_t run_period = 7, run_number = -1;
    Bool_t isField = (strRecoFile.Contains("noField")) ? kFALSE : kTRUE;
    Bool_t isTarget = kTRUE;
    Bool_t isExp = (BmnFunctionSet::isSimulationFile(strRecoFile) == 0);
    Double_t fieldScale = 0.;

    FairSource* fFileSource = NULL;
    // FOR SIMULATION : set source of events to display and addtiional parameters
    if (!isExp)
    {
        // check file existence with MC data and detector geometry
        if (!BmnFunctionSet::CheckFileExist(strSimFile))
        {
            cout<<endl<<"ERROR: simulation file with detector geometry was not found: "<<strSimFile<<endl;
            return;
        }

        fFileSource = new FairFileSource(strSimFile);

        // set parameter file with MC data and detector geometry
        FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
        FairParRootFileIo* parIo1 = new FairParRootFileIo();
        parIo1->open(strSimFile.Data());
        rtdb->setFirstInput(parIo1);
        rtdb->setOutput(parIo1);
        rtdb->saveOutput();

        // add file with reconstructed data as a friend
        ((FairFileSource*)fFileSource)->AddFriend(strRecoFile);
    }
    // FOR EXPERIMENTAL DATA
    else
    {
        // set source as root data file (without additional directories)
        fFileSource = new BmnFileSource(strRecoFile, run_period, run_number);

        // get geometry for run
        gRandom->SetSeed(0);
        TString geoFileName = Form("current_geo_file_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));
        Int_t res_code = UniRun::ReadGeometryFile(run_period, run_number, (char*) geoFileName.Data());
        if (res_code != 0)
        {
            cout << "ERROR: could not read geometry file from the database" << endl;
            return;
        }

        // get gGeoManager from ROOT file (if required)
        TFile* geoFile = new TFile(geoFileName, "READ");
        if (!geoFile->IsOpen())
        {
            cout << "ERROR: could not open ROOT file with geometry: " + geoFileName << endl;
            exit(-4);
        }
        TList* keyList = geoFile->GetListOfKeys();
        TIter next(keyList);
        TKey* key = (TKey*) next();
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoManager"))
            key->ReadObj();
        else {
            cout << "ERROR: TGeoManager is not top element in geometry file " + geoFileName << endl;
            return;
        }

        // set magnet field with factor corresponding to the given run
        UniRun* pCurrentRun = UniRun::GetRun(run_period, run_number);
        if (pCurrentRun == 0)
            exit(-6);
        Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
        if (field_voltage == NULL) {
            cout << "ERROR: no field voltage was found for run " << run_period << ":" << run_number << endl;
            exit(-7);
        }
        Double_t map_current = 55.87;
        if (*field_voltage < 10) {
            fieldScale = 0;
            isField = kFALSE;
        } else
            fieldScale = (*field_voltage) / map_current;

        BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v5_ascii_Extrap.root");
        magField->SetScale(fieldScale);
        magField->Init();
        fRunAna->SetField(magField);
        isExp = kTRUE;
        TString targ;
        if (pCurrentRun->GetTargetParticle() == NULL) {
            targ = "-";
            isTarget = kFALSE;
        } else {
            targ = (pCurrentRun->GetTargetParticle())[0];
            isTarget = kTRUE;
        }
        TString beam = pCurrentRun->GetBeamParticle();
        cout << "\n\n|||||||||||||||| EXPERIMENTAL RUN SUMMARY ||||||||||||||||" << endl;
        cout << "||\t\t\t\t\t\t\t||" << endl;
        cout << "||\t\tPeriod:\t\t" << run_period << "\t\t\t||" << endl;
        cout << "||\t\tNumber:\t\t" << run_number << "\t\t\t||" << endl;
        cout << "||\t\tBeam:\t\t" << beam << "\t\t\t||" << endl;
        cout << "||\t\tTarget:\t\t" << targ << "\t\t\t||" << endl;
        cout << "||\t\tField scale:\t" << setprecision(4) << fieldScale << "\t\t\t||" << endl;
        cout << "||\t\t\t\t\t\t\t||" << endl;
        cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n" << endl;
        remove(geoFileName.Data());
    }

    if (fFileSource != NULL) fRunAna->SetSource(fFileSource);
    // set output file
    fRunAna->SetSink(new FairRootFileSink("ed_out.root"));
    fRunAna->SetGenerateRunInfo(false);

    // Create Event Manager
    MpdEventManager* fMan = new MpdEventManager();
    fMan->isOnline = is_online;
    fMan->iDataSource = isExp;

    // set tasks to draw
    SetTasks(fMan, isExp, run_period, run_number, isField, isTarget);

    // light background color by default
    fMan->background_color = 17;
    fMan->isDarkColor = false;

    // MpdEventManager::Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000)
    fMan->Init(3, 3, 10000);
}

// set FairRunAna drawing tasks depending from data source and on/offline mode
void SetTasks(MpdEventManager* fMan, bool isExp, int run_period, int run_number, bool isField, bool isTarget)
{
    Style_t pointMarker = kFullDotSmall;
    Color_t mcPointColor = kRed, recoPointColor = kBlack, expPointColor = kRed;

    // FOR SIMULATION
    if (!isExp)
    {
        // draw MC Points
        MpdMCPointDraw* ArmTrigPoint = new MpdMCPointDraw("ArmTrigPoint", mcPointColor, pointMarker);
        fMan->AddTask(ArmTrigPoint);
        MpdMCPointDraw* BCPoint = new MpdMCPointDraw("BCPoint", mcPointColor, pointMarker);
        fMan->AddTask(BCPoint);
        MpdMCPointDraw* FDPoint = new MpdMCPointDraw("FDPoint", mcPointColor, pointMarker);
        fMan->AddTask(FDPoint);
        MpdMCPointDraw* MWPCPoint = new MpdMCPointDraw("MWPCPoint", mcPointColor, pointMarker);
        fMan->AddTask(MWPCPoint);
        MpdMCPointDraw* BdPoint = new MpdMCPointDraw("BdPoint", mcPointColor, pointMarker);
        fMan->AddTask(BdPoint);
        MpdMCPointDraw* SiliconPoint = new MpdMCPointDraw("SiliconPoint", mcPointColor, pointMarker);
        fMan->AddTask(SiliconPoint);
        MpdMCPointDraw* StsPoint = new MpdMCPointDraw("StsPoint", mcPointColor, pointMarker); // GEM
        fMan->AddTask(StsPoint);
        MpdMCPointDraw* CSCPoint = new MpdMCPointDraw("CSCPoint", mcPointColor, pointMarker);
        fMan->AddTask(CSCPoint);
        MpdMCPointDraw* SiBTPoint = new MpdMCPointDraw("SiBTPoint", mcPointColor, pointMarker);
        fMan->AddTask(SiBTPoint);
        MpdMCPointDraw* TOF1Point = new MpdMCPointDraw("TOF400Point", mcPointColor, pointMarker);
        fMan->AddTask(TOF1Point);
        MpdMCPointDraw* DCHPoint = new MpdMCPointDraw("DCHPoint", mcPointColor, pointMarker);
        fMan->AddTask(DCHPoint);
        MpdMCPointDraw* TofPoint = new MpdMCPointDraw("TOF700Point", mcPointColor, pointMarker);
        fMan->AddTask(TofPoint);
        MpdMCPointDraw* SSDPoint = new MpdMCPointDraw("SsdPoint", mcPointColor, pointMarker);
        fMan->AddTask(SSDPoint);

        // draw MC Geometry Tracks
        MpdMCTracks* GeoTrack = new MpdMCTracks("GeoTracks");
        fMan->AddTask(GeoTrack);
        // OR draw MC tracks by Geane - not implemented yet
        //MpdMCStack* MC      Track = new MpdMCStack("MCTrack");
        //fMan->AddTask(MCTrack);

        BmnCaloTowerDraw* BmnZDCTower = new BmnCaloTowerDraw("ZdcDigit", kZDC);
        fMan->AddTask(BmnZDCTower);
        BmnCaloTowerDraw* BmnECALTower = new BmnCaloTowerDraw("EcalDigit", kECAL);
        fMan->AddTask(BmnECALTower);

        // draw Reconstructed Detector Hits
        MpdHitPointSetDraw* BmnMwpcHit = new MpdHitPointSetDraw("BmnMwpcHit", expPointColor, pointMarker);
        fMan->AddTask(BmnMwpcHit);
        MpdHitPointSetDraw* BmnArmTrigHit = new MpdHitPointSetDraw("BmnArmTrigHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnArmTrigHit);
        MpdHitPointSetDraw* BmnBCHit = new MpdHitPointSetDraw("BmnBCHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnBCHit);
        MpdHitPointSetDraw* BmnGemHit = new MpdHitPointSetDraw("BmnGemStripHit", recoPointColor, pointMarker); // new MpdHitDraw("BmnGemStripHit", 1); //in box view
        fMan->AddTask(BmnGemHit);
        MpdHitPointSetDraw* BmnTof1Hit = new MpdHitPointSetDraw("BmnTof400Hit", recoPointColor, pointMarker);
        fMan->AddTask(BmnTof1Hit);
        MpdHitPointSetDraw* BmnDchHit = new MpdHitPointSetDraw("BmnDchHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnDchHit);
        MpdHitPointSetDraw* BmnTof2Hit = new MpdHitPointSetDraw("BmnTof700Hit", recoPointColor, pointMarker);
        fMan->AddTask(BmnTof2Hit);
        MpdHitPointSetDraw* BmnSiliconHit = new MpdHitPointSetDraw("BmnSiliconHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnSiliconHit);
        MpdHitPointSetDraw* BmnCSCHit = new MpdHitPointSetDraw("BmnCSCHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnCSCHit);
        MpdHitPointSetDraw* BmnUpstreamHit = new MpdHitPointSetDraw("BmnUpstreamHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnUpstreamHit);

        // draw Reconstructed Global Tracks
        BmnGlobalTrackDraw* BmnGlobalTrack = new BmnGlobalTrackDraw("BmnGlobalTrack");
        fMan->AddTask(BmnGlobalTrack);

        // save EventDisplay Screenshot
        //MpdWebScreenshots* WebScreenshots = new MpdWebScreenshots("WebScreenshots", "/var/www/html/events"); // for WEB-page
        //MpdWebScreenshots* WebScreenshots = new MpdWebScreenshots("WebScreenshots","screenshots"); // folder to save the screenshots
        //WebScreenshots->SetFormatFiles(0);    // 0 -.png, 1 -.jpg, 2 -.jpg and .png
        //WebScreenshots->SetMultiFiles(0);     // 0 - the same file (event.png), 1 - multiple files (event_nnn.png)
        //WebScreenshots->SetPort(8016);        // 8016 by default
        //fMan->AddTask(WebScreenshots);

        return;
    }

    // FOR EXPERIMENTAL DATA FROM RECONSTRUCTED ROOT FILE
    if (isExp)
    {
        // draw MWPC Hits
        MpdHitPointSetDraw* BmnMwpcHit = new MpdHitPointSetDraw("BmnMwpcHit", expPointColor, pointMarker);
        fMan->AddTask(BmnMwpcHit);
        // draw Silicon Hits
        MpdHitPointSetDraw* BmnSiliconHit = new MpdHitPointSetDraw("BmnSiliconHit", expPointColor, pointMarker);
        fMan->AddTask(BmnSiliconHit);
        // draw GEM Hits
        MpdHitPointSetDraw* BmnGemHit = new MpdHitPointSetDraw("BmnGemStripHit", expPointColor, pointMarker);
        fMan->AddTask(BmnGemHit);
        // draw DCH Hits
        MpdHitPointSetDraw* BmnDchHit = new MpdHitPointSetDraw("BmnDchHit", expPointColor, pointMarker);
        fMan->AddTask(BmnDchHit);
        // draw TOF1 Hits
        MpdHitPointSetDraw* BmnTof1Hit = new MpdHitPointSetDraw("BmnTof400Hit", expPointColor, pointMarker);
        fMan->AddTask(BmnTof1Hit);
        // draw TOF2 Hits
        MpdHitPointSetDraw* BmnTof2Hit = new MpdHitPointSetDraw("BmnTof700Hit", expPointColor, pointMarker);
        fMan->AddTask(BmnTof2Hit);
        // draw CSC Hits
        MpdHitPointSetDraw* BmnCSCHit = new MpdHitPointSetDraw("BmnCSCHit", expPointColor, pointMarker);
        fMan->AddTask(BmnCSCHit);
        // draw Upstream Hits
        MpdHitPointSetDraw* BmnUpstreamHit = new MpdHitPointSetDraw("BmnUpstreamHit", expPointColor, pointMarker);
        fMan->AddTask(BmnUpstreamHit);

        // draw MWPC Tracks
        BmnTrackDrawH* BmnMwpcTrack = new BmnTrackDrawH("BmnMwpcTrack", "BmnMwpcHit");
        fMan->AddTask(BmnMwpcTrack);
        // draw GEM Tracks
        BmnTrackDrawH* BmnGemTrack = new BmnTrackDrawH("BmnGemTrack", "BmnGemStripHit");
        fMan->AddTask(BmnGemTrack);
        // draw DCH Tracks
        // BmnTrackDrawH* DchTrack = new BmnTrackDrawH("BmnDchTrack", "BmnDchHit");
        // fMan->AddTask(DchTrack);

        // draw Reconstructed Global Tracks
        BmnGlobalTrackDraw* BmnGlobalTrack = new BmnGlobalTrackDraw("BmnGlobalTrack");
        fMan->AddTask(BmnGlobalTrack);

        //FairGeane* Geane = new FairGeane();
        //fMan->AddTask(Geane);
        //CbmTrackDraw* MwpcTrack = new CbmTrackDraw("MwpcMatchedTracks");
        //fMan->AddTask(MwpcTrack);

        return;
    }
}
