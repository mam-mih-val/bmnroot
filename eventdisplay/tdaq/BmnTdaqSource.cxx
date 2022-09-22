////////////////////////////////////////////
//    BmnTdaqSource.cxx
//    BM@N TDAQ Source implementation
//    Konstantin Gertsenberger
//    Created: Apr. 18 2017
////////////////////////////////////////////

#include "BmnTdaqSource.h"
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

BmnTdaqSource::BmnTdaqSource()
  : FairOnlineSource()
{
    strPartitionName = "mpd";
    strSamplingType = "raw";
    strSamplingName = "file";

    isDispersion = false;
    iWait = 10000;      //ms
    iRepetitions = 1;
    iBufferSize = 100;  //event count
    iAsync = 0;
    iTimeout = 10000;   //ms

    iVerbose = 0;

    iEventNumber = 0;
    fEventHeader = NULL;
    fGemDigits = NULL;
    fT0Digits = NULL;
    fTof1Digits = NULL;
}

BmnTdaqSource::BmnTdaqSource(TString partition_name, TString sampling_type, TString sampling_name, Int_t verbose)
{
    strPartitionName = partition_name;
    strSamplingType = sampling_type;
    strSamplingName = sampling_name;

    isDispersion = false;
    iWait = 10000;      //ms
    iRepetitions = 1;
    iBufferSize = 100;  //event count
    iAsync = 0;
    iTimeout = 10000;   //ms

    iVerbose = verbose;

    iEventNumber = 0;
    fEventHeader = NULL;
    fGemDigits = NULL;
    fT0Digits = NULL;
    fTof1Digits = NULL;
}

BmnTdaqSource::BmnTdaqSource(const BmnTdaqSource& source)
  : FairOnlineSource(source),
    strPartitionName(source.GetPartitionName()),
    strSamplingType(source.GetSamplingType()),
    strSamplingName(source.GetSamplingName()),
    isDispersion(false),
    iWait(10000),       //ms
    iRepetitions(1),
    iBufferSize(1),     //event count
    iAsync(0),
    iTimeout(10000),    //ms
    iVerbose(source.GetVerbose()),
    iEventNumber(0),
    fEventHeader(source.fEventHeader),
    fGemDigits(source.fGemDigits),
    fT0Digits(source.fT0Digits),
    fTof1Digits(source.fTof1Digits)
{
}


BmnTdaqSource::~BmnTdaqSource()
{
}


Bool_t BmnTdaqSource::Init()
{
    if (iVerbose > 1) cout<<"BmnTdaqSource::Init()"<<endl;

    /*TObject** ppObj = new TObject*[fBranchList->GetEntries()];
    for (int i = 0; i < fBranchList->GetEntries(); i++)
    {
        TBranch* pBranch = (TBranch*) fBranchList->At(i);
        TString ObjName = pBranch->GetName();
        LOG(debug)<<"Branch name "<<ObjName.Data();

        fCheckInputBranches[chainName]->push_back(ObjName.Data());
        FairRootManager::Instance()->AddBranchToList(ObjName.Data());

        ppObj[i] = NULL;
        //ActivateObject(&(ppObj[i]), ObjName);
        fInChain->SetBranchAddress(ObjName, &ppObj[i]);
        FairRootManager::Instance()->RegisterInputObject(ObjName, ppObj[i]);
    }*/

    fEventHeader = new TClonesArray("BmnEventHeader");
    FairRootManager::Instance()->Register("EventHeader", "Event", fEventHeader, kFALSE);

    fGemDigits = new TClonesArray("BmnGemStripDigit");
    FairRootManager::Instance()->Register("GEM", "GEMDIR", fGemDigits, kFALSE);

    fT0Digits = new TClonesArray("BmnTrigDigit");
    FairRootManager::Instance()->Register("T0", "T0DIR", fT0Digits, kFALSE);

    fTof1Digits = new TClonesArray("BmnTof1Digit");
    FairRootManager::Instance()->Register("TOF400", "TOFDIR", fTof1Digits, kFALSE);

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
        return kFALSE;
    }
    if (iVerbose > 1) cout<<"BmnTdaqSource::Init() IPC was initialized"<<endl;

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
    address_names.push_back((const char*)strSamplingName.Data());

    emon::SamplingAddress address((const char*)strSamplingType.Data(), address_names);

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
            if (iWait == 0) return kFALSE;
        }

        ERS_LOG("Waiting for BM@N event sampler.");
        usleep(iWait * 1000);
    }
    ERS_LOG("Monitoring Task started.");

    // Start the timer
    timer.start();

    return kFALSE;
}

Int_t BmnTdaqSource::ReadEvent(UInt_t)
{
    emon::Event event;
    // read next event from the TDAQ buffer
    try
    {
        // retrieve the event from the buffer, this either blocks (when iAsync == 0) or it will throw NoMoreEvents (when iAsync == 1)
        // you can pass a timeout in milliseconds when in synchronous mode after the timeout, NoMoreEvents will be thrown
        if (iVerbose > 1)
        {
            if (iAsync) clog<<"Trying next event...";
            else clog<<"Waiting for next event...";
        }

        if (iAsync)
            event = it->tryNextEvent();
        else
            event = it->nextEvent(iTimeout); // timeout = iTimeout/1000 seconds

        if (iVerbose > 1) cout<<"done"<<endl;
    }
    catch (emon::NoMoreEvents& ex)
    {
        // this will happen very often in asynchronous mode
        if (!iAsync)
        {
            ers::warning(ex);
            return 0; // just keep on trying
        }
        else
            return 0; // We do nothing here, we just keep on trying
    }
    catch (emon::SamplerStopped& ex)
    {
        // the sampler crashed or exitted...
        ers::error(ex);
        return 1;
    }
    catch (emon::Exception& ex)
    {
        // we actually have to exit here, or an uncatched NotInitialized exception will be thrown on deletion of the iterator
        ers::fatal(ex);
        return 2;
    }

    unsigned int* data = (unsigned int*) event.data();
    unsigned int event_id = data[0];
    unsigned int event_length = data[1];
    iEventNumber++;

    // get digit array
    TBufferFile t(TBuffer::kRead);
    t.Reset();
    t.SetWriteMode();
    t.SetBuffer((char*)&data[2], event_length);
    t.SetReadMode();
    DigiArrays* fDigiArrays = (DigiArrays*) (t.ReadObject(DigiArrays::Class()));

    if (iVerbose > 0) cout<<"Event count = "<<iEventNumber<<" Buffer occupancy = ["<<it->eventsAvailable()<<"/"<<iBufferSize<<"]"<<endl;
    if (iVerbose > 1)
    {
        cout<<"Event id = "<<event_id<<"\tEvent length = "<<event_length<<endl;
        //fDigiArrays->Dump();
        //usleep(3*1000000);
    }

    if (fDigiArrays->header->GetEntriesFast() == 0)
    {
        ers::warning(emon::Exception(ERS_HERE, "No Header was found for Digit Array."));
        fDigiArrays->Clear();
        delete fDigiArrays;
        t.DetachBuffer();
        return 3;
    }

    // get event header
    //BmnEventHeader* head = (BmnEventHeader*) fDigiArrays->header->At(0);
    //cout<<"Current Run Id: "<<head->GetRunId()<<endl;
    //cout<<"Count of BmnEventHeader: "<<fDigiArrays->header->GetEntriesFast()<<endl;
    cout<<"Count of GEM digits: "<<fDigiArrays->gem->GetEntriesFast()<<endl;
    cout<<"Count of TOF digits: "<<fDigiArrays->tof400->GetEntriesFast()<<endl;

    // move result TClonesArray to registered TClonesArray
    fEventHeader->Delete();
    fGemDigits->Delete();
    fT0Digits->Delete();
    fTof1Digits->Delete();
    fEventHeader->AbsorbObjects(fDigiArrays->header);
    fGemDigits->AbsorbObjects(fDigiArrays->gem);
    fT0Digits->AbsorbObjects(fDigiArrays->t0);
    fTof1Digits->AbsorbObjects(fDigiArrays->tof400);

    fDigiArrays->Clear();
    delete fDigiArrays;
    t.DetachBuffer();

    return 0;
}


void BmnTdaqSource::Close()
{
    if (fEventHeader)
    {
        fEventHeader->Delete();
        delete fEventHeader;
    }
    if (fGemDigits)
    {
        fGemDigits->Delete();
        delete fGemDigits;
    }
    if (fTof1Digits)
    {
        fTof1Digits->Delete();
        delete fTof1Digits;
    }
    if (fT0Digits)
    {
        fT0Digits->Delete();
        delete fT0Digits;
    }

    if (it.get())
        ERS_LOG("End of run, monitor dropped "<<it->eventsDropped()<<" events.");

    //stop the timer
    timer.stop();
    ERS_LOG("Total number of events received : "<<iEventNumber);
    ERS_LOG("Total CPU Time : "<<timer.userTime() + timer.systemTime());
    ERS_LOG("Total Time : "<<timer.totalTime());
    ERS_LOG("Events/second : "<<iEventNumber/timer.totalTime());
}

ClassImp(BmnTdaqSource)
