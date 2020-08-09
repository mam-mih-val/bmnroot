////////////////////////////////////////////
//    BmnEventMonitor.cxx
//    BM@N Event Monitoring Task implementation
//    Konstantin Gertsenberger
//    Created: Sep. 28 2016
////////////////////////////////////////////

#include "BmnEventMonitor.h"
#include "BmnRawDataDecoder.h"
#include "FairRootManager.h"

#include "TBufferFile.h"

#include <signal.h>
#include <iostream>

using namespace std;

int ReceivedSignal = 0;
void signal_handler(int sig) { ReceivedSignal = sig; }

unique_ptr<emon::EventIterator> it;
OWLTimer timer;

BmnEventMonitor::BmnEventMonitor()
{
    strPartitionName = "mpd";
    strSamplingType = "raw";
    strSamplingName = "file";

    isDispersion = false;
    iWait = 10000;
    iMaxEvents = 100;
    iRepetitions = 1;
    iBufferSize = 100;
    iAsync = 0;
    iTimeout = 10000;

    iVerbose = 0;

    iEventCount = 0;
}

BmnEventMonitor::BmnEventMonitor(TString partition_name, TString sampling_type, TString sampling_name, int max_events, Int_t verbose)
{
    strPartitionName = partition_name;
    strSamplingType = sampling_type;
    strSamplingName = sampling_name;

    iMaxEvents = max_events;

    isDispersion = false;
    iWait = 10000;
    iRepetitions = 1;
    iBufferSize = 100;
    iAsync = 0;
    iTimeout = 10000;

    iVerbose = verbose;

    iEventCount = 0;
}

BmnEventMonitor::~BmnEventMonitor()
{
    if (it.get())
        ERS_LOG("End of run, monitor dropped "<<it->eventsDropped()<<" events.");

    //stop the timer
    timer.stop();
    ERS_LOG("Total number of events received : "<<iEventCount);
    ERS_LOG("Total CPU Time : "<<timer.userTime() + timer.systemTime());
    ERS_LOG("Total Time : "<<timer.totalTime());
    ERS_LOG("Events/second : "<<iEventCount/timer.totalTime());
}

InitStatus BmnEventMonitor::Init()
{
    cout<<"HERE!!!!!"<<endl;
    if (iVerbose > 1)
        cout<<"BmnEventMonitor::Init()"<<endl;

    fGemDigits = new TClonesArray("BmnGemDigit");
    FairRootManager::Instance()->Register("BmnGemDigit", "GEM", fGemDigits, kFALSE);

    // initialize IPC
    try
    {
        list< pair < string, string > > list_arg;
        IPCCore::init(list_arg);
    }
    catch(daq::ipc::Exception& ex)
    {
        //SetActive(kFALSE);
        ers::fatal(ex);
        return kERROR;
    }

    if (iVerbose > 1)
        cout<<"BmnEventMonitor::Init() IPC was initialized"<<endl;

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

    emon::SamplingAddress address((const char *)strSamplingType.Data(), address_names);

    IPCPartition partition(strPartitionName);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while (!ReceivedSignal)
    {
        try
        {
            // Connect to the samplers
            it.reset(new emon::EventIterator(partition, address, criteria, iBufferSize, isDispersion));
            break;
        }
        catch (emon::Exception& ex)
        {
            ers::error(ex);
            if (iWait == 0) return kERROR;
        }

        usleep(iWait * 1000);
    }
    ERS_LOG("Monitoring Task started");

    // Start the timer
    timer.start( );

    return kSUCCESS;
}

void BmnEventMonitor::Exec(Option_t* option)
{
    //if (!IsActive())
    //    return;

    if (iEventCount >= iMaxEvents)
        return;

    if (ReceivedSignal)
    {
        cout<<"Received signal "<<ReceivedSignal<<", exiting ... "<<endl;
        return;
    }

    emon::Event event;
    // wait some time to simulate event processing in our sample application
    try
    {
        // retrieve the event from the buffer, this either blocks (when iAsync == 0)
        // or it will throw NoMoreEvents (when iAsync == 1)
        // you can pass a timeout in milliseconds when in synchronous mode
        // after the timeout, NoMoreEvents will be thrown
        if (iVerbose > 1)
        {
            if (iAsync)
                clog<<"Trying next event...";
            else
                clog<<"Waiting for next event (timeout: "<<iTimeout/1000<<" sec)... ";
        }

        if (iAsync)
            event = it->tryNextEvent();
        else
            event = it->nextEvent(iTimeout);

        if (iVerbose > 1)
            cout<<"done"<<endl;
    }
    catch (emon::NoMoreEvents& ex)
    {
        // output only when in synchronous case, because in asynchronous mode this will happen way too often!
        if (!iAsync)
        {
            ers::warning(ex);
            return; // just keep on trying
        }
        else
            return; // We do nothing here, we just keep on trying
    }
    catch (emon::SamplerStopped& ex)
    {
        // the sampler crashed or exitted...
        ers::error(ex);
        return;
    }
    catch (emon::Exception& ex)
    {
        // we actually have to exit here, or an uncatched NotInitialized
        // exception will be thrown on deletion of the iterator
        ers::fatal(ex);
        return;
    }

    unsigned int* data = (unsigned int*) event.data();
    unsigned int event_id = data[0];
    unsigned int event_length = data[1];
    iEventCount++;

    // get digit array
    TBufferFile t(TBuffer::kRead);
    t.Reset();
    t.SetWriteMode();
    t.SetBuffer((char*)&data[2], event_length);
    t.SetReadMode();
 /* DigiArrays* fDigiArrays = (DigiArrays*) (t.ReadObject(DigiArrays::Class()));

    if (iVerbose > 0)
        cout<<"Event count = "<<iEventCount<<" Buffer occupancy = ["<<it->eventsAvailable()<<"/"<<iBufferSize<<"]"<<endl;
    if (iVerbose > 1)
    {
        cout<<"Event id = "<<event_id<<"\tEvent length = "<<event_length<<"\tFull size = "<<event.size()<<" DWORD"<<endl;
        //fDigiArrays->Dump();
        //usleep(3*1000000);
    }

    if (fDigiArrays->header->GetEntriesFast() == 0)
    {
        ers::warning(emon::Exception(ERS_HERE, "No Header was found for Digit Array."));
        fDigiArrays->Clear();
        delete fDigiArrays;
        t.DetachBuffer();
        return;
    }

    // get event header
    BmnEventHeader* head = (BmnEventHeader*) fDigiArrays->header->At(0);
    Int_t runID = head->GetRunId();
    usleep(1*1000000);

    fDigiArrays->Clear();
    delete fDigiArrays;
    t.DetachBuffer();
*/
    return;
}

/** Action after each event**/
void BmnEventMonitor::Finish()
{
}

ClassImp(BmnEventMonitor)
