/********************************************************************************
 *    BmnFillDstTask.h                                                          *
 *    BM@N Fill DST Task class implementation                                   *
 *    Fill Event Headers in the DST file after reconstruction                   *
 *    Konstantin Gertsenberger                                                  *
 *    Created: Apr. 25 2019                                                     *
 *******************************************************************************/

#define ANSI_COLOR_RED "\x1b[91m"
#define ANSI_COLOR_BLUE "\x1b[94m"
#define ANSI_COLOR_RESET "\x1b[0m"

#include "BmnFillDstTask.h"
#include "UniDbRun.h"
#include "function_set.h"

#include "ExtractZ2.cxx"

#include "FairLogger.h"

#include "TDirectory.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"

using namespace std;

// ---- Default constructor -------------------------------------------
BmnFillDstTask::BmnFillDstTask() : FairTask("BmnFillDstTask"),
                                   fInputEventHeaderName("BmnEventHeader."),
                                   fOutputEventHeaderName("DstEventHeader."),
                                   fEventHead(NULL),
                                   fDstHead(NULL),
                                   fNEvents(-1),
                                   fIEvent(0),
                                   fPeriodNumber(-1),
                                   fRunNumber(-1),
				   fZCalib1(1),
				   fZCalib2(0),
                                   isSimulationInput(false) {
    LOG(DEBUG) << "Defaul Constructor of BmnFillDstTask";
}

// ---- Constructor with the given event number to be processed -------
BmnFillDstTask::BmnFillDstTask(Long64_t nEvents) : FairTask("BmnFillDstTask"),
                                                   fInputEventHeaderName("BmnEventHeader."),
                                                   fOutputEventHeaderName("DstEventHeader."),
                                                   fEventHead(NULL),
                                                   fDstHead(NULL),
                                                   fNEvents(nEvents),
                                                   fIEvent(0),
                                                   fPeriodNumber(-1),
                                                   fRunNumber(-1),
						   fZCalib1(1),
						   fZCalib2(0),
                                                   isSimulationInput(false) {
    fRunHead = new DstRunHeader();
    LOG(DEBUG) << "Constructor of BmnFillDstTask";
}

// Constructor with input Event Header Name and event number to be processed
BmnFillDstTask::BmnFillDstTask(TString input_event_header_name, Long64_t nEvents) : FairTask("BmnFillDstTask"),
                                                                                    fInputEventHeaderName(input_event_header_name),
                                                                                    fOutputEventHeaderName("DstEventHeader."),
                                                                                    fEventHead(NULL),
                                                                                    fDstHead(NULL),
                                                                                    fNEvents(nEvents),
                                                                                    fIEvent(0),
                                                                                    fPeriodNumber(-1),
                                                                                    fRunNumber(-1),
										    fZCalib1(1),
										    fZCalib2(0),
                                                                                    isSimulationInput(false) {
    fRunHead = new DstRunHeader();
    LOG(DEBUG) << "Constructor of BmnFillDstTask";
}

// Constructor with input and output Event Header Name, and event number to be processed
BmnFillDstTask::BmnFillDstTask(TString input_event_header_name, TString output_event_header_name, Long64_t nEvents) : FairTask("BmnFillDstTask"),
                                                                                                                      fInputEventHeaderName(input_event_header_name),
                                                                                                                      fOutputEventHeaderName(output_event_header_name),
                                                                                                                      fEventHead(NULL),
                                                                                                                      fDstHead(NULL),
                                                                                                                      fNEvents(nEvents),
                                                                                                                      fIEvent(0),
                                                                                                                      fPeriodNumber(-1),
                                                                                                                      fRunNumber(-1),
														      fZCalib1(1),
														      fZCalib2(0),
                                                                                                                      isSimulationInput(false) {
    fRunHead = new DstRunHeader();
    LOG(DEBUG) << "Constructor of BmnFillDstTask";
}

// ---- Destructor ----------------------------------------------------
BmnFillDstTask::~BmnFillDstTask() {
    LOG(DEBUG) << "Destructor of BmnFillDstTask";
}

// ----  Initialisation  ----------------------------------------------
void BmnFillDstTask::SetParContainers() {
    LOG(DEBUG) << "SetParContainers of BmnFillDstTask";
    // Load all necessary parameter containers from the runtime data base
    /*
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    <BmnFillDstTaskDataMember> = (<ClassPointer>*)
    (rtdb->getContainer("<ContainerName>"));
    */
}

// ---- Init ----------------------------------------------------------
InitStatus BmnFillDstTask::Init() {
    LOG(DEBUG) << "Initilization of BmnFillDstTask";

    // Get a handle from the IO manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        LOG(ERROR) << "Init: FairRootManager is not instantiated!\n"
                   << "BmnFillDstTask will be inactive";
        return kERROR;
    }

    // Get a pointer to the input Event Header
    TObject* pObj = ioman->GetObject(fInputEventHeaderName);
    if (!pObj) {
        // if no input Event Header was found, searching for "MCEventHeader."
        fMCEventHead = (FairMCEventHeader*)ioman->GetObject("MCEventHeader.");
        if (!fMCEventHead) {
            LOG(ERROR) << "No input Event Header (" << fInputEventHeaderName << " or MCEventHeader.) was found!\n"
                       << "BmnFillDstTask will be inactive!";
            return kERROR;
        }
        isSimulationInput = true;
    } else {
        if (pObj->InheritsFrom(FairMCEventHeader::Class())) {
            fMCEventHead = (FairMCEventHeader*)pObj;
            isSimulationInput = true;
        } else
            fEventHead = (BmnEventHeader*)pObj;
    }

    // Read in the z-calibration file
    TString gPathWorkdir = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathWorkdir + "/input/ZOutCorrections4.txt";

    string line;
    ifstream f(gPathFull.Data(), ios::in);

    vector <Double_t> axisAttr;

    while (!f.eof()) {
      getline(f, line);
      
      TString currString(line);
      int run;
      TString str_run( currString(0,4) );
      run = str_run.Atoi();
      
      if(run == fRunNumber){
	//	cout<<"++++++++++++FILLDSTTASK!!!!"<<endl;
	cout<<currString.Data()<<endl;
	TString ab( currString(5, currString.Length()) );
	TString a ( ab(0, ab.First(" ")) );
	float a_float;
	fZCalib1 = a.Atof();
	TString c ( ab( a.Length()+1, ab.Length() ) );
	//cout<<"c = "<<c.Data()<<endl;
	TString b ( c(0, c.First(" ")) );
	fZCalib2 = b.Atof();
	//cout<<"ab = "<<ab.Data()<<", a = "<<fZCalib1<<", b = "<<fZCalib2<<endl;
      }
    }
      
    
    //Get input branches	
    fT0 = (TClonesArray*) ioman->GetObject("BC2");
    fBC1 = (TClonesArray*) ioman->GetObject("TQDC_BC1");
    fBC2 = (TClonesArray*) ioman->GetObject("TQDC_BC2");
    fBC3 = (TClonesArray*) ioman->GetObject("TQDC_BC3");
    fBC4 = (TClonesArray*) ioman->GetObject("TQDC_BC4");
    // Get a pointer to the output DST Event Header
    fDstHead = (DstEventHeader*)ioman->GetObject(fOutputEventHeaderName);
    if (!fDstHead) {
        LOG(ERROR) << "No Event Header(" << fOutputEventHeaderName << ") prepared for the output DST file!\n"
                   << "BmnFillDstTask will be inactive";
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

    // Fill Run Header from the Database
    if (fRunNumber > 0) {
        InitParticleInfo();

        UniDbRun* pCurrentRun = UniDbRun::GetRun(fPeriodNumber, fRunNumber);
        if (pCurrentRun != 0) {
            fRunHead->SetPeriodRun(fPeriodNumber, fRunNumber);
            TDatime start_date = pCurrentRun->GetStartDatetime();
            TTimeStamp tsStartTime(start_date.GetYear(), start_date.GetMonth(), start_date.GetDay(),
                                   start_date.GetHour(), start_date.GetMinute(), start_date.GetSecond());
            fRunHead->SetStartTime(tsStartTime);
            if (pCurrentRun->GetEndDatetime() != NULL) {
                //TTimeStamp tsEndTime((time_t) pCurrentRun->GetEndDatetime()->Convert(), 0);
                TDatime* end_date = pCurrentRun->GetEndDatetime();
                TTimeStamp tsEndTime(end_date->GetYear(), end_date->GetMonth(), end_date->GetDay(),
                                     end_date->GetHour(), end_date->GetMinute(), end_date->GetSecond());
                fRunHead->SetFinishTime(tsEndTime);
            }
            TString beam = pCurrentRun->GetBeamParticle();
            beam.ToLower();
            map<TString, stParticleInfo>::iterator it = mapParticleInfo.find(beam);
            if (it != mapParticleInfo.end()) {
                fRunHead->SetBeamA(it->second.A);
                fRunHead->SetBeamZ(it->second.Z);
            }
            double* energy = pCurrentRun->GetEnergy();
            if (energy != NULL)
                fRunHead->SetBeamEnergy(*energy);
            TString* target = pCurrentRun->GetTargetParticle();
            if (target != NULL) {
                target->ToLower();
                it = mapParticleInfo.find(*target);
                if (it != mapParticleInfo.end()) {
                    fRunHead->SetTargetA(it->second.A);
                    fRunHead->SetTargetZ(it->second.Z);
                }
            }
            Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
            if (field_voltage != NULL)
                fRunHead->SetMagneticField(*field_voltage);
        }  // if (pCurrentRun != 0)
    }      // FIll Run Header from the Database

    return kSUCCESS;
}

// ---- ReInit  -------------------------------------------------------
InitStatus BmnFillDstTask::ReInit() {
    LOG(DEBUG) << "Re-initilization of BmnFillDstTask";
    return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void BmnFillDstTask::Exec(Option_t* /*option*/) {
    LOG(DEBUG) << "Exec of BmnFillDstTask";

    // fill output DST event header
    if (isSimulationInput) {
        fDstHead->SetRunId(fMCEventHead->GetRunID());
        fDstHead->SetEventId(fMCEventHead->GetEventID());
        fDstHead->SetEventTime(fMCEventHead->GetT());
        fDstHead->SetEventTimeTS((TTimeStamp)convert_double_to_timespec(fMCEventHead->GetT()));
        fDstHead->SetB(fMCEventHead->GetB());
    } else {
        fDstHead->SetRunId(fEventHead->GetRunId());
        fDstHead->SetEventId(fEventHead->GetEventId());
        fDstHead->SetEventTime(fEventHead->GetEventTime());
        fDstHead->SetEventTimeTS(fEventHead->GetEventTimeTS());
    }

    //calculate Z2in and Z2out:
    Double_t Z2in = -100.0, Z2out = -100.0;
    Double_t adcIn = -100.0, adcOut = -100.0;
    if (fT0 && fBC1 && fBC2 && fBC3 && fBC4) {
        BmnTrigDigit* digT0 = NULL;
        Int_t t0Count = 0;
        for (UInt_t i = 0; i < fT0->GetEntriesFast(); i++){
	    digT0 = (BmnTrigDigit*)fT0->At(i);
            if (digT0->GetMod() == 0) t0Count++;
	}
        if (t0Count == 1) {
            Double_t t0Time = digT0->GetTime();
            grabZ2(fBC1, fBC2, t0Time, Z2in, adcIn, true);
            grabZ2(fBC3, fBC4, t0Time, Z2out, adcOut, false);
	}
	fDstHead->SetZ2in(Z2in);
	fDstHead->SetZ2out(Z2out);
	fDstHead->SetADCin(adcIn);
	fDstHead->SetADCout(adcOut);
    }
    
    Double_t Z1 = - 100.0, Z2 = -100.0, Z3 = -100.0, Z4 = -100.0, ADC1 = -100.0, ADC2 = -100.0, ADC3 = -100.0, ADC4 = -100.0;	
    if(fT0) {
      Int_t t0Count = 0;
      BmnTrigDigit* digT0 = NULL;
      Double_t t0Time = -100000;
      for (UInt_t i = 0; i < fT0->GetEntriesFast(); i++) {
	digT0 = (BmnTrigDigit*)fT0->At(i);
	if (digT0->GetMod() == 0) {
	  t0Count++;
	  t0Time = digT0->GetTime();
	}
      }
      if (t0Count == 1) {
	if(fBC1 && fBC2){
	  grabZ2OR(fBC1, fBC2, t0Time, Z1, Z2, ADC1, ADC2, true);
	}
	if(fBC3 && fBC4){
	  grabZ2OR(fBC3, fBC4, t0Time, Z3, Z4, ADC3, ADC4, false);
	}
      }
    
      //no calibration from single adc to charge yet
      //fDstHead->SetZ1(Z1);
      //fDstHead->SetZ2(Z2);
      //fDstHead->SetZ3(Z3);
      //fDstHead->SetZ4(Z4);
    
      fDstHead->SetADC1(ADC1);
      fDstHead->SetADC2(ADC2);
      fDstHead->SetADC3(ADC3);
      fDstHead->SetADC4(ADC4);
    }
    
    // printing progress bar in terminal
    if (fVerbose == 0) {
        if (gROOT->IsBatch()) {
            if (fIEvent % 1000 == 0)
                printf("%lld/%lld\n", fIEvent, fNEvents);
        } else {
            UInt_t fRunId = (fDstHead) ? fDstHead->GetRunId() : 0;

            if (isSimulationInput)
                printf(ANSI_COLOR_RED "SIMULATION RUN" ANSI_COLOR_RESET);
            else {
                printf(ANSI_COLOR_BLUE "RUN-" ANSI_COLOR_RESET);
                printf(ANSI_COLOR_RED "%d" ANSI_COLOR_RESET, fRunId);
            }
            printf(ANSI_COLOR_BLUE ": [");

            Float_t progress = fIEvent * 1.0 / fNEvents;
            Int_t barWidth = 70;

            Int_t pos = barWidth * progress;
            for (Int_t i = 0; i < barWidth; ++i) {
                if (i < pos)
                    printf("=");
                else if (i == pos)
                    printf(">");
                else
                    printf(" ");
            }

            printf("] " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_RED "%d%%\r" ANSI_COLOR_RESET, Int_t(progress * 100.0 + 0.5));
            cout.flush();
        }
    } else {
        cout << "Event #" << fIEvent << endl;
    }
    fIEvent++;
}

// ---- Finish --------------------------------------------------------
void BmnFillDstTask::Finish() {
    LOG(DEBUG) << "Finish of BmnFillDstTask";

    FairRootManager* ioman = FairRootManager::Instance();
    FairSink* fSink = ioman->GetSink();
    fSink->WriteObject(fRunHead, "DstRunHeader", TObject::kSingleKey);

    if (fVerbose == 0) printf("\n");
}

void BmnFillDstTask::InitParticleInfo() {
    stParticleInfo particle_info;
    particle_info.A = 2;
    particle_info.Z = 1;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("d", particle_info));
    particle_info.A = 12;
    particle_info.Z = 6;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("c", particle_info));
    particle_info.A = 40;
    particle_info.Z = 18;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("ar", particle_info));
    particle_info.A = 84;
    particle_info.Z = 36;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("kr", particle_info));
    particle_info.A = 27;
    particle_info.Z = 13;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("al", particle_info));
    particle_info.A = 207;
    particle_info.Z = 82;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("pb", particle_info));
    particle_info.A = 119;
    particle_info.Z = 50;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("sn", particle_info));
    particle_info.A = 64;
    particle_info.Z = 29;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("cu", particle_info));
    particle_info.A = 1;
    particle_info.Z = 1;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("h2", particle_info));
    particle_info.A = 29;
    particle_info.Z = 0;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("c2h4", particle_info));

    return;
}

ClassImp(BmnFillDstTask)
