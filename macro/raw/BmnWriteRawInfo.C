// Macro to save information of raw files to the Unified Database
// based on BmnDataToRoot.C
// (optionally, you can save the result digi file, if output_file isn't equal "")
//
// Parameters:
//file: full path to raw-file
//output_file: full path to save the final (digi) root file, if you don't need it, please, don't set it or use ""
//isOnlyNew - if true then only new runs will be created in the database (no update for existing runs), otherwise - new run will be created and existing runs will be updated
void BmnWriteRawInfo(TString file, TString output_file = "", bool isOnlyNew = false)
{
    Int_t nEvents = 0;
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    // check input file exist and get file size
    gSystem->ExpandPathName(file);
    if (gSystem->AccessPathName(file.Data()) == true)
    {
        cout<<endl<<"No input file was found: "<<file<<endl;
        return;
    }
    Long_t id, flags, modtime;
    Long64_t l_file_size = -1;
    gSystem->GetPathInfo(file.Data(), &id, &l_file_size, &flags, &modtime);
    if (l_file_size <= 0)
    {
        cout<<endl<<"Input file has zero size. Exiting..."<<endl;
        return;
    }
	double file_size = l_file_size/1048576.0;

    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents, 6); //6 - period

    bool isRunExist = UniDbRun::CheckRunExists(decoder->GetPeriodId(), decoder->GetRunId());
    // if run exists and corresponding flags were set then exit
    if ((output_file == "") && (isOnlyNew) && (isRunExist))
    {
        cout<<endl<<"Run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" already exists and 'only new run' flag was set. Exiting..."<<endl;
        delete decoder;
        return;
    }

    Bool_t setup[9]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0] = 1; // TRIGGERS
    setup[1] = 1; // MWPC
    setup[2] = 1; // SILICON
    setup[3] = 1; // GEM
    setup[4] = 1; // TOF-400
    setup[5] = 1; // TOF-700
    setup[6] = 1; // DCH
    setup[7] = 1; // ZDC
    setup[8] = 1; // ECAL
    decoder->SetDetectorSetup(setup);

    // set mapping
    decoder->SetTrigMapping("Trig_map_Run6.txt");   //e.g. "Trig_map_Run5.txt"
    decoder->SetTrigINLFile("TRIG_INL.txt");
    // in case comment out the line decoder->SetTof400Mapping("...")
    // the maps of TOF400 will be readed from DB (only for JINR network)
    decoder->SetTof400Mapping("TOF400_PlaceMap_RUN6.txt", "TOF400_StripMap_RUN6.txt");  //e.g. "TOF400_PlaceMap_Period5_v3.txt", "TOF400_StripMap_Period5_v3.txt"
    decoder->SetTof700Mapping("TOF700_map_period_6.txt");   //e.g. "TOF700_map_period_5.txt"
    decoder->SetZDCMapping("ZDC_map_period_5.txt");
    decoder->SetZDCCalibration("zdc_muon_calibration.txt");
    decoder->SetECALMapping("ECAL_map_period_5.txt");
    decoder->SetECALCalibration("");
    decoder->SetMwpcMapping("MWPC_mapping_period_5.txt");

    // convert RAW to RAW ROOT format
    decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format

    // open RAW ROOT file, then print and write to the Unified Database
    TString fRootFileName = decoder->GetRootFileName();
    TChain* tree = new TChain("BMN_RAW");
    tree->Add(fRootFileName);

    // link run header to get information about run
    BmnRunHeader* fRunHeader = NULL;
    tree->SetBranchAddress("RunHeader", &fRunHeader);

    Int_t records = tree->GetEntries(); // events = records - 1; (-RunHeader)
    //cout<<"Number of events in RAW file = "<<events<<endl;
    if (records > 0)
    {
        tree->GetEntry(0);

        cout<<"Start time: "<<fRunHeader->GetStartTime().AsString("lc")<<endl;
        cout<<"End time: "<<fRunHeader->GetFinishTime().AsString("lc")<<endl;
        cout<<"Event count: "<<fRunHeader->GetNEvents()<<endl;

        // update start time, end time and event count in the Unified Database
        TDatime startDate((Int_t)fRunHeader->GetStartTime().GetDate(kFALSE), (Int_t)fRunHeader->GetStartTime().GetTime(kFALSE));
        TDatime* endDate = new TDatime((Int_t)fRunHeader->GetFinishTime().GetDate(kFALSE), (Int_t)fRunHeader->GetFinishTime().GetTime(kFALSE));
        int* event_count = new int(fRunHeader->GetNEvents());

        if (isRunExist)
        {
            if (isOnlyNew)
                cout<<"Run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" is exist (it will not updated)."<<endl;
            else
            {
                UniDbRun* pRun = UniDbRun::GetRun(decoder->GetPeriodId(), decoder->GetRunId());

                bool isErrors = false;
                if (pRun == NULL) isErrors = true;
                else
                {
                    if (pRun->SetFilePath(file.Data()) != 0) isErrors = true;
                    if (pRun->SetStartDatetime(startDate) != 0) isErrors = true;
                    if (pRun->SetEndDatetime(endDate) != 0) isErrors = true;
                    if (pRun->SetEventCount(event_count) != 0) isErrors = true;
                    if (pRun->SetFileSize(&file_size) != 0) isErrors = true;

                    delete pRun;
                }
                if (isErrors)
                    cout<<"The errors occured during run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" info updated."<<endl;
                else
                    cout<<"Info for run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" was updated."<<endl;
            }
        }
        else
        {
            UniDbRun* pRun = UniDbRun::CreateRun(decoder->GetPeriodId(), decoder->GetRunId(), file, "", NULL, NULL, startDate, endDate, event_count, NULL, &file_size, NULL);

            bool isErrors = false;
            if (pRun == NULL)
                isErrors = true;
            else
                delete pRun;

            if (isErrors)
                cout<<"The errors occured during run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" info created."<<endl;
            else
                cout<<"Info for run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" was created."<<endl;
        }

        delete event_count;
        delete endDate;

        // write output digi file if required
        if (output_file != "")
        {
            TString fDigiFileName = Form("bmn_run%04d_digi.root", decoder->GetRunId());
            decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.

            gSystem->ExpandPathName(output_file);
            Int_t status = gSystem->CopyFile(fDigiFileName.Data(), output_file.Data(), kTRUE);

            if (status != 0)
            {
                switch (status)
                {
                    case -1: Warning("Copy", "Failed to copy result file to %s (file open failure)", output_file.Data());
                        break;
                    case -2: Warning("Copy", "File %s already exists", output_file.Data());
                        break;
                    case -3: Warning("Copy", "Failed to copy result file to %s (error during copy)", output_file.Data());
                        break;
                    default: Warning("Copy", "Failed to copy result file to %s", output_file.Data());
                        break;
                }
            }

            // delete root digi file
            gSystem->Unlink(fDigiFileName);
        }
    }
    else
        cout<<"Error: "<<fRootFileName<<" raw root file is empty"<<endl;

    // delete root raw file
    gSystem->Unlink(fRootFileName);

    delete tree;
    delete decoder;
}

/*for (Int_t i = 0; i < events; i++)
{
    tree->GetEntry(i);

    if ((i != 0) && (i != (events-1)))
        continue;

    Int_t event_headers = fEventHeader->GetEntriesFast();
    cout<<"Number of event header items = "<<event_headers<<endl;
    for (Int_t j = 0; j < event_headers; j++)
    {
        BmnEventHeader* pHeader = (BmnEventHeader*) fEventHeader->UncheckedAt(j);
        cout<<"Event time: sec - "<<pHeader->GetEventTimeS()<<" nsec - "<<pHeader->GetEventTimeNS()<<endl;
        TTimeStamp time_stamp((time_t)(pHeader->GetEventTimeS()+3*3600), (Int_t)pHeader->GetEventTimeNS());
        cout<<time_stamp.AsString("c")<<endl;
    } // track loop
} // event loop
*/
