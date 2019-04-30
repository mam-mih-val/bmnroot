/********************************************************************************
 *    BmnFillDstTask.h                                                          *
 *    BM@N Fill DST Task class implementation                                   *
 *    Fill Event Headers in the DST file after reconstruction                   *
 *    Konstantin Gertsenberger                                                  *
 *    Created: Apr. 25 2019                                                     *
 *******************************************************************************/

#define ANSI_COLOR_RED   "\x1b[91m"
#define ANSI_COLOR_BLUE  "\x1b[94m"
#define ANSI_COLOR_RESET "\x1b[0m"

#include "BmnFillDstTask.h"
#include "function_set.h"

#include "FairLogger.h"

using namespace std;

// ---- Default constructor -------------------------------------------
BmnFillDstTask::BmnFillDstTask() :
 FairTask("BmnFillDstTask"),
 fInputEventHeaderName("BmnEventHeader."),
 fOutputEventHeaderName("DstEventHeader."),
 fEventHead(NULL),
 fDstHead(NULL),
 fNEvents(-1),
 fIEvent(0),
 isSimulationInput(false)
{
    LOG(DEBUG)<<"Defaul Constructor of BmnFillDstTask"<<FairLogger::endl;
}

// ---- Constructor with the given event number to be processed -------
BmnFillDstTask::BmnFillDstTask(Long64_t nEvents) :
 FairTask("BmnFillDstTask"),
 fInputEventHeaderName("BmnEventHeader."),
 fOutputEventHeaderName("DstEventHeader."),
 fEventHead(NULL),
 fDstHead(NULL),
 fNEvents(nEvents),
 fIEvent(0),
 isSimulationInput(false)
{
    LOG(DEBUG)<<"Constructor of BmnFillDstTask"<<FairLogger::endl;
}

// Constructor with input Event Header Name and event number to be processed
BmnFillDstTask::BmnFillDstTask(TString input_event_header_name, Long64_t nEvents) :
 FairTask("BmnFillDstTask"),
 fInputEventHeaderName(input_event_header_name),
 fOutputEventHeaderName("DstEventHeader."),
 fEventHead(NULL),
 fDstHead(NULL),
 fNEvents(nEvents),
 fIEvent(0),
 isSimulationInput(false)
{
    LOG(DEBUG)<<"Constructor of BmnFillDstTask"<<FairLogger::endl;
}

// Constructor with input and output Event Header Name, and event number to be processed
BmnFillDstTask::BmnFillDstTask(TString input_event_header_name, TString output_event_header_name, Long64_t nEvents) :
 FairTask("BmnFillDstTask"),
 fInputEventHeaderName(input_event_header_name),
 fOutputEventHeaderName(output_event_header_name),
 fEventHead(NULL),
 fDstHead(NULL),
 fNEvents(nEvents),
 fIEvent(0),
 isSimulationInput(false)
{
    LOG(DEBUG)<<"Constructor of BmnFillDstTask"<<FairLogger::endl;
}

// ---- Destructor ----------------------------------------------------
BmnFillDstTask::~BmnFillDstTask()
{
    LOG(DEBUG)<<"Destructor of BmnFillDstTask"<<FairLogger::endl;
}

// ----  Initialisation  ----------------------------------------------
void BmnFillDstTask::SetParContainers()
{
    LOG(DEBUG)<<"SetParContainers of BmnFillDstTask"<<FairLogger::endl;
    // Load all necessary parameter containers from the runtime data base
    /*
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    <BmnFillDstTaskDataMember> = (<ClassPointer>*)
    (rtdb->getContainer("<ContainerName>"));
    */
}

// ---- Init ----------------------------------------------------------
InitStatus BmnFillDstTask::Init()
{
    LOG(DEBUG)<<"Initilization of BmnFillDstTask"<<FairLogger::endl;

    // Get a handle from the IO manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        LOG(ERROR)<<"Init: FairRootManager is not instantiated!"<<FairLogger::endl<<"BmnFillDstTask will be inactive"<<FairLogger::endl;
        return kERROR;
    }

    // Get a pointer to the input Event Header
    TObject* pObj = ioman->GetObject(fInputEventHeaderName);
    if (!fEventHead)
    {
        // if no input Event Header was found, searching for "MCEventHeader."
        fMCEventHead = (FairMCEventHeader*) ioman->GetObject("MCEventHeader.");
        if (!fMCEventHead)
        {
            LOG(ERROR)<<"No input Event Header ("<<fInputEventHeaderName<<") or MCEventHeader was found!"<<FairLogger::endl<<"BmnFillDstTask will be inactive"<<FairLogger::endl;
            return kERROR;
        }
        isSimulationInput = true;
    }
    else
    {
        if (pObj->InheritsFrom(FairMCEventHeader::Class()))
        {
            fMCEventHead = (FairMCEventHeader*) pObj;
            isSimulationInput = true;
        }
        else fEventHead = (BmnEventHeader*) pObj;
    }

    // Get a pointer to the output DST Event Header
    fDstHead = (DstEventHeader*) ioman->GetObject(fOutputEventHeaderName);
    if (!fDstHead)
    {
        LOG(ERROR)<<"No Event Header("<<fOutputEventHeaderName<<") prepared for the output DST file!"<<FairLogger::endl<<"BmnFillDstTask will be inactive"<<FairLogger::endl;
        return kERROR;
    }

    // Create the TClonesArray for the output data and register
    // it in the IO manager
    /*
    <OutputDataLevel> = new TClonesArray("OutputDataLevelName", 100);
    ioman->Register("OutputDataLevelName","OutputDataLevelName",<OutputDataLevel>,kTRUE);
    */

    if (ioman->CheckMaxEventNo(fNEvents) < fNEvents)
        fNEvents = ioman->CheckMaxEventNo(fNEvents);

    return kSUCCESS;
}

// ---- ReInit  -------------------------------------------------------
InitStatus BmnFillDstTask::ReInit()
{
    LOG(DEBUG)<<"Initilization of BmnFillDstTask"<<FairLogger::endl;
    return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void BmnFillDstTask::Exec(Option_t* /*option*/)
{
    LOG(DEBUG)<<"Exec of BmnFillDstTask"<<FairLogger::endl;

    // fill output DST event header
    if (isSimulationInput)
    {
        fDstHead->SetRunId( fMCEventHead->GetRunID() );
        fDstHead->SetEventId( fMCEventHead->GetEventID() );
        fDstHead->SetEventTime( fMCEventHead->GetT() );
        fDstHead->SetEventTimeTS( (TTimeStamp) convert_double_to_timespec(fMCEventHead->GetT()) );
        fDstHead->SetB( fMCEventHead->GetB() );
    }
    else
    {
        fDstHead->SetRunId( fEventHead->GetRunId() );
        fDstHead->SetEventId( fEventHead->GetEventId() );
        fDstHead->SetEventTime( fEventHead->GetEventTime() );
        fDstHead->SetEventTimeTS( fEventHead->GetEventTimeTS() );
        fDstHead->SetTriggerType( fEventHead->GetTrig() );
    }

    // printing progress bar in terminal
    if (fNEvents > 0)
    {
        UInt_t fRunId = (fDstHead) ? fDstHead->GetRunId() : 0;

        printf(ANSI_COLOR_BLUE "RUN-" ANSI_COLOR_RESET);
        printf(ANSI_COLOR_RED "%d" ANSI_COLOR_RESET, fRunId);
        printf(ANSI_COLOR_BLUE ": [");

        Float_t progress = fIEvent * 1.0 / fNEvents;
        Int_t barWidth = 70;

        Int_t pos = barWidth * progress;
        for (Int_t i = 0; i < barWidth; ++i) {
            if (i < pos) printf("=");
            else if (i == pos) printf(">");
            else printf(" ");
        }

        printf("] " ANSI_COLOR_RESET);
        printf(ANSI_COLOR_RED "%d%%\r" ANSI_COLOR_RESET, Int_t(progress * 100.0 + 0.5));
        cout.flush();
        fIEvent++;
    }
}

// ---- Finish --------------------------------------------------------
void BmnFillDstTask::Finish()
{
    LOG(DEBUG)<<"Finish of BmnFillDstTask"<<FairLogger::endl;

    if (fNEvents > 0) printf("\n");
}


ClassImp(BmnFillDstTask)
