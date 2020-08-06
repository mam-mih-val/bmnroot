////////////////////////////////////////////
//    BmnEventPull.h
//    BM@N Event Pull class implementation
//    Creates the Pull Sampler for BM@N experimental data (MPD raw format):
//    read raw dara from file, convert raw data to digits and send it to the event channel
//    Konstantin Gertsenberger
//    Created: Apr. 10 2017
////////////////////////////////////////////

#include "BmnEventPull.h"
#include "BmnRawDataDecoder.h"

#include "TBufferFile.h"

#include <signal.h>
#include <dirent.h>
#include <regex>

#include "sys/timeb.h"

using namespace emon;

timeb tStart, tEnd;

// keyboard Ctrl-C capture to exit pull sampler
int signalSamplerReceived;
EventSampler* sampler;
void signal_handler_sampler(int sig)
{
    if (sampler)
    {
        signalSamplerReceived = sig;
        sampler->stop();
    }
}

/*! BM@N PullSampling class
 * Apart from the constructor, you will have to override the method sampleEvent.
 * The startSampling method of the PullSamplingFactory shall return instances of this class. For every emon::EventChannel
 * one instance of this class will be created.*/
class BmnPullSampling : public PullSampling
{
 private:
    TString strRawData;
    Int_t iRunPeriod;
    Int_t iMaxEvent;
    Int_t iWaitFileMs;

    BmnRawDataDecoder* rawDataDecoder;
    UInt_t uiCurrentEvent;
    TString strCurrentRaw;
    bool isNewFile;

 public:
    //! Constructor of the BmnPullSampling.
    /*! This method usually initializes hardware of the data flow system for event sampling
     * \param sc the selection criteria sampled events shall match
     * \param event BM@N event to push to the event channel
     * \param size size of the BM@N event */
    BmnPullSampling(const SelectionCriteria&, string raw_data, int run_period, int wait_file_sec)
    {
        rawDataDecoder = NULL;

        strRawData = raw_data.c_str();
        iRunPeriod = run_period;
        iWaitFileMs = wait_file_sec * 1000;

        uiCurrentEvent = 0;
        strCurrentRaw = "";
        isNewFile = true;

        ftime(&tStart);

        ERS_DEBUG(1, "Initialization of BmnPullSampling completed");
    }

    int AttachRawFile(TString directory_name, TString& current_file)
    {
        /** Find last modified file **/
        struct dirent** namelist;
        regex re("\\w+\\.data");
        TString ret = "";
        while (1)
        {
            Int_t n = scandir(directory_name.Data(), &namelist, 0, versionsort);
            if (n < 0)
            {
                perror("scandir");
                return -1;
            }

            for (Int_t i = n-1; i >= 0; i--)
            {
                if (regex_match(namelist[i]->d_name, re))
                {
                    ret = namelist[i]->d_name;

                    for (Int_t j = i; i >= 0; i--)
                        free(namelist[i]);
                    break;
                }

                free(namelist[i]);
            }
            free(namelist);

            if (current_file.CompareTo(ret) != 0)
            {
                current_file = ret;
                break;
            }

            gSystem->ProcessEvents();
            usleep(iWaitFileMs);
        }

        if (rawDataDecoder != NULL)
        {
            rawDataDecoder->DisposeDecoder();
            delete rawDataDecoder;
            uiCurrentEvent = 0;
        }

        /** create BmnRawDataDecoder instance **/
        rawDataDecoder = new BmnRawDataDecoder(directory_name+current_file, 0, iRunPeriod);
        cout<<"Raw file "<<directory_name+current_file<<" is used now."<<endl;
        // set mapping for decoder
        Bool_t setup[11]; //array of flags to determine BM@N setup
        //Just put "0" to exclude detector from decoding
        setup[0] = 1; // TRIGGERS
        setup[1] = 1; // MWPC
        setup[2] = 1; // SILICON
        setup[3] = 1; // GEM
        setup[4] = 1; // TOF-400
        setup[5] = 1; // TOF-700
        setup[6] = 1; // DCH
        setup[7] = 1; // ZDC
        setup[8] = 0; // ECAL
        setup[9] = 0; // LAND
        setup[10] = 0; // CSC
        rawDataDecoder->SetDetectorSetup(setup);
        BmnSetup stp = kBMNSETUP;
        TString PeriodSetupExt = Form("%d%s.txt", iRunPeriod, ((stp == kBMNSETUP) ? "" : "_SRC"));
        rawDataDecoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt); 
        rawDataDecoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt);
        rawDataDecoder->SetSiliconMapping("SILICON_map_run6.txt");
        // in case comment out the line decoder->SetTof400Mapping("...")
        // the maps of TOF400 will be readed from DB (only for JINR network)
        rawDataDecoder->SetTof400Mapping("TOF400_PlaceMap_RUN6.txt", "TOF400_StripMap_RUN6.txt");
        rawDataDecoder->SetTof700Mapping("TOF700_map_period_6.txt");
        rawDataDecoder->SetZDCMapping("ZDC_map_period_5.txt");
        rawDataDecoder->SetZDCCalibration("zdc_muon_calibration.txt");
        //rawDataDecoder->SetECALMapping("ECAL_map_period_5.txt");
        //rawDataDecoder->SetECALCalibration("");
        rawDataDecoder->SetMwpcMapping("MWPC_mapping_period_5.txt");

        rawDataDecoder->SetEvForPedestals(150);
        rawDataDecoder->InitConverter(directory_name+current_file);
        rawDataDecoder->InitDecoder();

        return 0;
    }

    //! Destructor of the BmnPullSampling.
    /*! This method usually deallocates memory and prepares data flow for shutdown. Whenever an event channel
     * will be closed, the appropriate destructor will be called. */
    ~BmnPullSampling()
    {
        if (rawDataDecoder)
        {
            rawDataDecoder->DisposeDecoder();
            delete rawDataDecoder;
        }

        ERS_DEBUG(1, "Finalization of BmnPullSampling object");
    }

    //! Implementation of the sampling process.
    /*! This method just pushes one BM@N event to the Monitoring Tasks. When pushEvent of EventChannel
     * has finished, it is guaranteed, that the event buffer passed to EventChannel may be freed. */
    void sampleEvent(EventChannel& cc)
    {
        ftime(&tEnd);
        int dif = 1000*((int)tEnd.time - (int)tStart.time) + (tEnd.millitm - tStart.millitm);
        cout<<"\nNew Cycle time: "<<dif<<" ms\n";

        DigiArrays iterDigi;
        TBufferFile t(TBuffer::kWrite);
        bool isFoundEvent = false;
        while (!isFoundEvent)
        {
            ftime(&tStart);

            if (isNewFile)
            {
                if (AttachRawFile(strRawData, strCurrentRaw) != 0)
                    return;
                isNewFile = false;
            }

            // read one raw event and convert to ROOT format
            BmnStatus convertResult = rawDataDecoder->ConvertRawToRootIterateFile();
            if (convertResult == kBMNFINISH)    //if finished
            {
                isNewFile = true;
                continue;
            }

            ftime(&tEnd);
            dif = 1000*((int)tEnd.time - (int)tStart.time) + (tEnd.millitm - tStart.millitm);
            cout<<"ConvertRawToRootIterateFile time: "<<dif<<" ms\n";

            ftime(&tStart);
            uiCurrentEvent = rawDataDecoder->GetEventId();
            // convert one raw ROOT event to digits
            rawDataDecoder->DecodeDataToDigiIterate();

            ftime(&tEnd);
            dif = 1000*((int)tEnd.time - (int)tStart.time) + (tEnd.millitm - tStart.millitm);
            cout<<"DecodeDataToDigiIterate time: "<<dif<<" ms\n";


            ftime(&tStart);
            // get array with digits
            iterDigi = rawDataDecoder->GetDigiArraysObject();
            cout<<"GEM digits: "<<iterDigi.gem->GetEntries()<<endl;
            if ((iterDigi.header->GetEntriesFast() == 0) || (((BmnEventHeader*)iterDigi.header->At(0))->GetType() == kBMNPEDESTAL))
                continue;

            isFoundEvent = true;
        }

        // write digit array to the buffer in order to send to EventChannel
        t.Reset();
        t.WriteObject(&iterDigi);

        // Create and fill the actual buffer of the current event
        UInt_t event_length = t.Length();
        iovec* event = new iovec[0];
        int add_length = (event_length % 4), head_length = 2*sizeof(int);
        UInt_t full_length = event_length + add_length;
        event[0].iov_len = full_length + head_length; // + 1 int for event number + 1 int for real size
        event[0].iov_base = new caddr_t[event[0].iov_len];

        //ERS_INFO("Event "<<(uiCurrentEvent+1)<<" size is "<<full_length<<" ("<<event_length<<") bytes"<<" + event id + real length");

        // copy the buffer with digit array to event structure
        unsigned int* pUInt = (unsigned int*) event[0].iov_base;
        pUInt[0] = uiCurrentEvent;
        pUInt[1] = event_length;
        memcpy((void*)&pUInt[2], t.Buffer(), event_length);
        if (add_length > 0) memset(((char*)(event[0].iov_base)) + event_length + head_length, 0, add_length);

        ftime(&tEnd);
        dif = 1000*((int)tEnd.time - (int)tStart.time) + (tEnd.millitm - tStart.millitm);
        cout<<"write and memcpy time: "<<dif<<" ms\n";

        ftime(&tStart);
        ERS_DEBUG(3, "Sending event to the consumer");
        cc.pushEvent(event, 1);
        ERS_DEBUG(3,"BM@N event was successfully sent");
        delete [] ((caddr_t*)event[0].iov_base);
        delete [] event;

        ftime(&tEnd);
        dif = 1000*((int)tEnd.time - (int)tStart.time) + (tEnd.millitm - tStart.millitm);
        cout<<"pushEvent time: "<<dif<<" ms\n";

        ftime(&tStart);
    }
};

//! Implementation of the BmnPullSamplingFactory.
/*! This class is implementation of the BmnPullSamplingFactory, it's main
 * purpose being the creation of BmnPullSampling objects in the method startSampling */
class BmnPullSamplingFactory : public PullSamplingFactory
{
 private:
    string strRawData;
    int iRunPeriod;
    int iWaitFileSec;

 public:
    //! Constructor of BmnPullSamplingFactory
    /*! This constructor simply allocates some memory for a new BM@N event or uses
     * the event passed in the constructor
     * \param event the BM@N event that shall be pushed to the EventChannel
     * \param size size of the BM@N event */
    BmnPullSamplingFactory(string raw_data, int run_period, int wait_file_sec):
        strRawData(raw_data),
        iRunPeriod(run_period),
        iWaitFileSec(wait_file_sec)
    {
        ERS_DEBUG(1, "Initialization of BmnPullSamplingFactory completed");
    }

    //! This will be called whenever a new EventChannel needs to be created.
    /*! This method will be called whenever a new EventChannel is required and a new
     * sampling thread needs to be created. It's main purpose is to enable users to pass
     * custom parameters to the BmnPullSampling objects upon object creation time.
     * \param criteria the selection criteria, the events being sampled shall satisfy
     * \return a new instance of class BmnPullSampling */
    PullSampling* startSampling(const SelectionCriteria& criteria)
        throw (BadCriteria, NoResources)
    {
        ERS_DEBUG(1, "Factory creating new PullSampling object with criteria: "<<criteria);
        return new BmnPullSampling(criteria, strRawData, iRunPeriod, iWaitFileSec);
    }
};

/******************************/
// BmnEventPull class
/******************************/
BmnEventPull::BmnEventPull()
{
    strPartitionName = "mpd";
    strSamplingType = "raw";
    strSamplingName = "file";

    strRawData = "/tdaq/data/";
    iRunPeriod = 6;
    iMaxChannels = 100;
    iWaitFileSec = 30;

    iVerbose = 0;
}

BmnEventPull::BmnEventPull(TString partition_name, TString sampling_type, TString sampling_name, TString raw_data, int run_period,
                           int max_event, int max_channels, int wait_file_sec, Int_t verbose)
{
    strPartitionName = partition_name;
    strSamplingType = sampling_type;
    strSamplingName = sampling_name;

    strRawData = raw_data;
    iRunPeriod = run_period;
    iMaxChannels = max_channels;
    iWaitFileSec = wait_file_sec;

    iVerbose = verbose;
}

BmnEventPull::~BmnEventPull()
{}

int BmnEventPull::Exec()
{
    // initialize IPC
    try
    {
        list< pair < string, string > > list_arg;
        IPCCore::init(list_arg);
    }
    catch(daq::ipc::Exception& ex)
    {
        ers::fatal(ex);
    }

    signal(SIGINT, signal_handler_sampler);
    signal(SIGTERM, signal_handler_sampler);

    OWLTimer timer;

    // define partition by name
    IPCPartition partition(strPartitionName);

    CmdArgInt		lvl1_type ('L', "lvl1-type", "type", "lvl1_trigger type (default -1)" );
    CmdArgIntList	lvl1_bits ('B', "lvl1-bits", "bits", "non-zero bits positions in the LVL1 bit pattern", CmdArg::isLIST );
    CmdArgBool		lvl1_bits_logic ('l', "lvl1-bits-logic", "logic attribute for the LVL1 bit pattern , AND if given OR otherwise");
    CmdArgInt		lvl1_bits_origin ('o', "lvl1-bits-origin", "origin", "origin of the LVL1 bit pattern, 0 - BEFORE_PRESCALE\n"
                                      " 1 - AFTER_PRESCALE, 2 - AFTER_VETO (default)" );
    CmdArgInt		status_word 	('H', "header-word", "word", "status word (default 0 )");
    CmdArgStr		stream_type 	('T', "stream-type", "stream-type", "stream type name" );
    CmdArgStrList	stream_names	('N', "stream-names", "stream-names", "list of stream names", CmdArg::isLIST );
    CmdArgBool		stream_logic	('S', "stream logic", "logic attribute for the stream tags, AND if given OR otherwise");

    lvl1_type = -1;
    status_word = 0;
    lvl1_bits_origin = 2;
    stream_type = "";

    vector<unsigned short> L1_bits;
    for (size_t i = 0; i < lvl1_bits.count(); i++)
        L1_bits.push_back(lvl1_bits[i]);

    vector<string> STREAM_names;
    for (size_t i = 0; i < stream_names.count(); i++)
        STREAM_names.push_back((const char*)stream_names[i]);

    emon::Logic L1_logic = !(lvl1_bits.flags() && CmdArg::GIVEN) ? emon::logic::IGNORE
                                                           : (lvl1_bits_logic.flags() && CmdArg::GIVEN) ? emon::logic::AND : emon::logic::OR;
    emon::Logic STREAM_logic = !(stream_type.flags() && CmdArg::GIVEN) ? emon::logic::IGNORE
                                                                 : (stream_logic.flags() && CmdArg::GIVEN) ? emon::logic::AND : emon::logic::OR;

    /*emon::SelectionCriteria criteria(emon::L1TriggerType((unsigned char)(int)lvl1_type, !(lvl1_type.flags() && CmdArg::GIVEN)),
                                        emon::SmartBitValue(L1_bits, L1_logic, (emon::Origin)(int)lvl1_bits_origin),
                                        emon::SmartStreamValue((const char*)stream_type, STREAM_names, STREAM_logic),
                                        emon::StatusWord(status_word, !(status_word.flags() && CmdArg::GIVEN)));*/
    emon::SelectionCriteria criteria;
    vector<string> address_names;
    address_names.push_back((const char *)strSamplingName.Data());

    try
    {
        emon::SamplingAddress address((const char *)strSamplingType.Data(), address_names);

        // start sampler
        emon::EventSampler temp(partition, address, new BmnPullSamplingFactory(strRawData.Data(), iRunPeriod, iWaitFileSec), iMaxChannels);
        sampler = &temp;
        ERS_INFO("BM@N Pull Sampler started in partition \"" << partition.name() << "\"");
        // Start the timer
        timer.start();

        sampler->wait();
        sampler = 0;
        if (signalSamplerReceived != 0)
            ERS_LOG("Shutting down BM@N sampler...");

    }
    catch (Exception& ex)
    {
        ers::fatal(ex);
    }

    ERS_LOG("Shutdown of BM@N Event Sampler complete.");
    timer.stop();
    ERS_LOG("Total CPU Time : " << timer.userTime() + timer.systemTime());
    ERS_LOG("Total Time : " << timer.totalTime());

    return 0;
}

ClassImp(BmnEventPull)
