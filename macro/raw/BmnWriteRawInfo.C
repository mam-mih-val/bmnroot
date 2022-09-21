R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "bmndata/BmnEnums.h"

enum WriteAction { OnlyCreate, OnlyUpdate, CreateAndUpdate };
int append_log(TString log_name, TString message);

// Macro to extract information from obtained raw files to the Unified Database
// based on BmnDataToRoot.C
//
// Parameters:
// file: full path to the raw file
// output_file: full path to save the final (digi) root file, if you don't need it, please, don't set it or use ""
// iAction - action, possible values: OnlyCreate, OnlyUpdate, CreateAndUpdate
// error_log_name: full path to the log file with errors, default: "" - no log will be created
// example: root 'BmnWriteRawInfo.C("~/bmnroot/macro/raw/mpd_run_trigCode_2185.data","", OnlyCreate, "error.log")'
void BmnWriteRawInfo(TString input_file, TString output_file = "", UInt_t period = 7, WriteAction iAction = OnlyCreate, TString error_log_name = "")
{
    Int_t nEvents = 0;
    bool isLog = false;
    if (error_log_name != "")
    {
        isLog = true;
        gSystem->ExpandPathName(error_log_name);
    }

    // check input file exist and get file size
    gSystem->ExpandPathName(input_file);
    if (gSystem->AccessPathName(input_file.Data()) == true)
    {
        TString message = TString::Format("ERROR: no input file was found: %s", input_file.Data());
        cout<<endl<<message<<endl;
        if (isLog) append_log(error_log_name, message);
        return;
    }
    Long_t id, flags, modtime;
    Long64_t l_file_size = -1;
    gSystem->GetPathInfo(input_file.Data(), &id, &l_file_size, &flags, &modtime);
    if (l_file_size <= 0)
    {
        TString message = TString::Format("ERROR: input file has zero size: %s", input_file.Data());
        cout<<endl<<message<<endl;
        //if (isLog) append_log(error_log_name, message);
        return;
    }
    double file_size = l_file_size/1048576.0;   //MB


    BmnSetup stp = kBMNSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(input_file, output_file, nEvents, period);
    decoder->SetBmnSetup(stp);

    bool isRunExist = UniRun::CheckRunExists(decoder->GetPeriodId(), decoder->GetRunId());
    // if run exists and flag 'OnlyCreate' was set then exit
    if ((iAction == OnlyCreate) && (isRunExist))
    {
        cout<<endl<<"Run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" already exists and 'Only Create' flag was set"<<endl;
        delete decoder;
        return;
    }

    Bool_t setup[11]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0]  = 1; // TRIGGERS
    setup[1]  = 1; // MWPC
    setup[2]  = 1; // SILICON
    setup[3]  = 1; // GEM
    setup[4]  = 1; // TOF-400
    setup[5]  = 1; // TOF-700
    setup[6]  = 1; // DCH
    setup[7]  = 1; // ZDC
    setup[8]  = 0; // ECAL
    setup[9]  = 1; // LAND
    setup[10] = 1; // CSC
    decoder->SetDetectorSetup(setup);

    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt);
    decoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt);
    decoder->SetSiliconMapping("SILICON_map_run7.txt");
    decoder->SetGemMapping(TString("GEM_map_run") + PeriodSetupExt);
    decoder->SetCSCMapping(TString("CSC_map_period") + PeriodSetupExt);
    // in case comment out the line decoder->SetTof400Mapping("...")
    // the maps of TOF400 will be read from DB (only for JINR network)
    decoder->SetTOF700ReferenceRun(-1);
    decoder->SetTof700Geom("TOF700_geometry_run7.txt");
    decoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") +PeriodSetupExt, TString("TOF400_StripMap_RUN") +PeriodSetupExt);
    decoder->SetTof700Mapping("TOF700_map_period_7.txt");
    decoder->SetZDCMapping("ZDC_map_period_5.txt");
    decoder->SetZDCCalibration("zdc_muon_calibration.txt");
    //decoder->SetECALMapping("ECAL_map_period_7.txt");
    //decoder->SetECALCalibration("");
    decoder->SetMwpcMapping(TString("MWPC_map_period") + PeriodSetupExt);
    decoder->SetLANDMapping("land_mapping_jinr_triplex.txt");
    decoder->SetLANDPedestal("r0030_land_clock.hh");
    decoder->SetLANDTCal("r0030_land_tcal.hh");
    decoder->SetLANDDiffSync("r352_cosmic1.hh");
    decoder->SetLANDVScint("neuland_sync_2.txt");
    decoder->InitMaps();

    decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format

    // open RAW ROOT file, then print and write to the Unified Database
    TString fRootFileName = decoder->GetRootFileName();
    TChain* tree = new TChain("BMN_RAW");
    tree->Add(fRootFileName);

    // link run header to get information about run
    BmnEventHeader* fEventHeader = NULL;
    tree->SetBranchAddress("BmnEventHeader.", &fEventHeader);

    Int_t records = tree->GetEntries(); // events = records - 1; (-RunHeader)
    //cout<<"Number of events in RAW file = "<<events<<endl;
    if (records > 0)
    {
        tree->GetEntry(0);

        // update start time, end time and event count in the Unified Database
        TTimeStamp startTime = fEventHeader->GetEventTimeTS();

        tree->GetEntry(records-1);

        TTimeStamp endTime = fEventHeader->GetEventTimeTS();
        int event_count = records;

        if (event_count < 1)
        {
            cout<<endl<<"ERROR: Run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" has 0 events"<<endl;
            return;
        }

        cout<<"Start time: "<<startTime.AsString()<<endl;
        cout<<"End time: "<<endTime.AsString()<<endl;
        cout<<"Event count: "<<event_count<<endl;

        if (isRunExist)
        {
            if (iAction == OnlyCreate)
                cout<<"Run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" exists and 'Only Create' flag was set"<<endl;
            else
            {
                UniRun* pRun = UniRun::GetRun(decoder->GetPeriodId(), decoder->GetRunId());

                bool isErrors = false;
                if (pRun == NULL) isErrors = true;
                else
                {
                    TDatime startDate(Int_t(startTime.GetDate(kFALSE)), Int_t(startTime.GetTime(kFALSE)));
                    TDatime endDate(Int_t(endTime.GetDate(kFALSE)), Int_t(endTime.GetTime(kFALSE)));
                    if (pRun->SetFilePath(input_file.Data()) != 0) isErrors = true;
                    if (pRun->SetStartDatetime(startDate) != 0) isErrors = true;
                    if (pRun->SetEndDatetime(&endDate) != 0) isErrors = true;
                    if (pRun->SetEventCount(&event_count) != 0) isErrors = true;
                    if (pRun->SetFileSize(&file_size) != 0) isErrors = true;

                    delete pRun;
                }
                if (isErrors)
                    cout<<"The errors occured during run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" info updated."<<endl;
                else
                    cout<<"Info for run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" was updated."<<endl;
            }
        }
        else // if run doens't exist
        {
            if (iAction == OnlyUpdate)
                cout<<"Run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" does not exist and 'Only Update' flag was set"<<endl;
            else
            {
                TDatime startDate(Int_t(startTime.GetDate(kFALSE)), Int_t(startTime.GetTime(kFALSE)));
                TDatime endDate(Int_t(endTime.GetDate(kFALSE)), Int_t(endTime.GetTime(kFALSE)));
                TString* strMD5 = NULL;
                TMD5* pMD5 = TMD5::FileChecksum(input_file.Data());
                if (pMD5) strMD5 = pMD5->AsString();
                else cout<<"WARNING: a problem occured while file checksum calculation: "<<input_file<<endl;
                UniRun* pRun = UniRun::CreateRun(decoder->GetPeriodId(), decoder->GetRunId(), input_file, "", NULL, NULL, startDate, &endDate, &event_count, NULL, &file_size, NULL, strMD5);
                if (pMD5) delete pMD5;
                if (strMD5) delete strMD5;

                bool isErrors = false;
                if (pRun == NULL)
                    isErrors = true;
                else
                    delete pRun;

                if (isErrors)
                    cout<<"ERROR: the errors occured during run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" info created."<<endl;
                else
                    cout<<"Info for run "<<decoder->GetPeriodId()<<":"<<decoder->GetRunId()<<" was created."<<endl;
            }
        }

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
        cout<<"ERROR: "<<fRootFileName<<" raw root file is empty"<<endl;

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

int append_log(TString log_name, TString message)
{
    // open log file
    ofstream logFile;
    logFile.open(log_name, ofstream::out | ofstream::app);
    if (!logFile.is_open())
    {
        cout<<"ERROR: log file could not be opened: "<<log_name<<endl;
        return -1;
    }

    logFile<<message<<endl;

    logFile.close();
    return 0;
}
