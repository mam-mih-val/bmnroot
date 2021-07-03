#if !defined(__CLING__) || defined(__MAKECLING__)
// ROOT includes
#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPRegexp.h"
#include "TKey.h"
#include "TRandom3.h"
#include "TList.h"

// Fair includes
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairTask.h"
#include "FairField.h"
#include "FairTrackParP.h"
#include "FairCave.h"
#include "FairPipe.h"
#include "FairMagnet.h"
#include "FairFileSource.h"
#include "FairParAsciiFileIo.h"
#include "FairRootFileSink.h"

// BM@N includes
#include "BmnFileSource.h"
#include "UniDbRun.h"
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"
#include "BmnCounterTask.h"
#include "BmnTriggersCheck.h"
#include "BmnMwpcHitFinder.h"
#include "BmnSiliconHitMaker.h"
#include "BmnGemStripHitMaker.h"
#include "BmnTof1HitProducer.h"
#include "BmnTofHitProducer.h"
#include "BmnMwpcHitProducer.h"
#include "BmnMwpcTrackFinder.h"
#include "BmnGemTrack.h"
#include "BmnDchTrackFinder.h"
#include "BmnGlobalTracking.h"
#include "BmnGemResiduals.h"
#include "BmnSiResiduals.h"
#include "BmnLANDHitProducer.h"
#include "BmnFunctionSet.h"
#include "DstEventHeader.h"
#include "MpdGetNumEvents.h"    
#include "BmnCSCHitMaker.h"
#include "BmnInnerTrackingRun7.h"
#include "BmnDchHitProducer.h"
#include "BmnZdcAnalyzer.h"
#include "BmnFillDstTask.h"
#include "BmnVertexFinder.h"


#include <iostream>
using namespace std;
#endif

// -----------------------------------------------------------------------------
// Macro for reconstruction of simulated or experimental events.
//
// inputFileName - input file with data.
//
// To process experimental data, you must use 'runN-NNN:'-like prefix
// and then the geometry will be obtained from the Unified Database.
//
// bmndstFileName - output file with reconstructed data.
//
// nStartEvent - number of first event to process (starts with zero), default: 0.
//
// nEvents - number of events to process, 0 - all events of given file will be
// processed, default: 10000.
//
// alignCorrFileName - argument for choosing input file with the alignment
// corrections.
//
// If alignCorrFileName == 'default', (case insensitive) then corrections are
// retrieved from UniDb according to the running period and run number.
//
// If alignCorrFileName == '', then no corrections are applied at all.
//
// If alignCorrFileName == '<path>/<file-name>', then the corrections are taken
// from that file.

//#include "../run/bmnloadlibs.C"

void run_reco_bmn(TString inputFileName = "/home/andrey/DataBMN/experimental/bmn_run5142_digi.root",
        TString bmndstFileName = "$VMCWORKDIR/macro/run/bmndst.root",
        Int_t nStartEvent = 0, Int_t nEvents = 10)
{
    gDebug = 0; // Debug option
    // Verbosity level (0 = quiet (progress bar), 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // check input file exists
    if (!BmnFunctionSet::CheckFileExist(inputFileName, 1)) exit(-1);

    // -----   Reconstruction run   --------------------------------------------
    FairRunAna* fRunAna = new FairRunAna();
    fRunAna->SetEventHeader(new DstEventHeader());

    Bool_t isField = (inputFileName.Contains("noField")) ? kFALSE : kTRUE; // flag for tracking (to use mag.field or not)
    Bool_t isTarget = kTRUE; //kTRUE; // flag for tracking (run with target or not)
    Bool_t isExp = !BmnFunctionSet::isSimulationFile(inputFileName); // flag for hit finder (to create digits or take them from data-file)

    // Declare input source as simulation file or experimental data
    FairSource* fFileSource;

    // -1 means use of the BM@N-setup when processing MC-input
    // DO NOT change it manually!
    Int_t run_period = 7, run_number = -1;
    Double_t fieldScale = 0.;
    if (!isExp) // for simulation files
        fFileSource = new FairFileSource(inputFileName);
    else        // for experimental files
    {
        // set source as raw root data file (without additional directories)
        fFileSource = new BmnFileSource(inputFileName, run_period, run_number);

        // get geometry for run
        gRandom->SetSeed(0);
        TString geoFileName = Form("current_geo_file_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));
        Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, (char*) geoFileName.Data());
        if (res_code != 0) {
            cout << "ERROR: could not read geometry file from the database" << endl;
            exit(-3);
        }

        // get gGeoManager from ROOT file (if required)
        TFile* geoFile = new TFile(geoFileName, "READ");
        if (!geoFile->IsOpen()) {
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
            exit(-5);
        }

        // set magnet field with factor corresponding to the given run
        UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
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

    fRunAna->SetSource(fFileSource);
    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(bmndstFileName, 1) < 0) exit(-2);
    fRunAna->SetSink(new FairRootFileSink(bmndstFileName));
    fRunAna->SetGenerateRunInfo(false);

    // if nEvents is equal 0 then all events of the given file starting with "nStartEvent" should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)inputFileName.Data()) - nStartEvent;

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the FairParAsciiFileIo.
    // The FairParAsciiFileIo will create on fly a concatenated input parameter file, which is then used during the reconstruction.
    TList* parFileNameList = new TList();
#ifdef L1
    TObjString stsDigiFile = "$VMCWORKDIR/parameters/sts_v1_BMN_SI_GEM.digi.par";
    parFileNameList->Add(&stsDigiFile);
#endif

    // ====================================================================== //
    // ===                           Check Triggers                       === //
    // ====================================================================== //
    //    BmnTriggersCheck* triggs = new BmnTriggersCheck(isExp, run_period, run_number);
    //    fRunAna->AddTask(triggs);
    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //
    if(!isExp) {
      BmnMwpcHitProducer *mwpcHP = new BmnMwpcHitProducer();
      fRunAna->AddTask(mwpcHP);
    }
    BmnMwpcHitFinder* mwpcHM = new BmnMwpcHitFinder(isExp, run_period, run_number);
    fRunAna->AddTask(mwpcHM);

#ifdef L1
    // ====================================================================== //
    // ===                           STS digitizer                        === //
    // ====================================================================== //
    CbmBmnStsDigitize* stsDigitize = new CbmBmnStsDigitize("STS Digitiser", iVerbose);
    stsDigitize->SetGeoFile("$VMCWORKDIR/geometry/SIGEMS_r7.root");
    fRunAna->AddTask(stsDigitize);

    // ====================================================================== //
    // ===                         STS Cluster Finder                     === //
    // ====================================================================== //
    FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
    fRunAna->AddTask(stsClusterFinder);

    // ====================================================================== //
    // ===                          STS hit finder                        === //
    // ====================================================================== //
    FairTask* stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
    fRunAna->AddTask(stsFindHits);

    // ====================================================================== //
    // ===                         STS track finding                      === //
    // ====================================================================== //
    CbmKF* kalman = new CbmKF();
    fRunAna->AddTask(kalman);

    CbmL1* l1 = new CbmL1();
    TString stsMatBudgetFile = ""; // paramDir + "/sts/sts_matbudget_v12b_12344444.root"; // paramDir + "/sts_matbudget_var_fr.root";
    l1->SetMaterialBudgetFileName(stsMatBudgetFile);
    fRunAna->AddTask(l1);

    CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
    FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder);
    fRunAna->AddTask(stsFindTracks);

    // ====================================================================== //
    // ===                      Primary vertex finding                    === //
    // ====================================================================== //
    CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
    CbmFindPrimaryVertex * findVertex = new CbmFindPrimaryVertex(pvFinder);
    fRunAna->AddTask(findVertex);
#else
    // ====================================================================== //
    // ===                         Silicon hit finder                     === //
    // ====================================================================== //
    BmnSiliconHitMaker* siliconHM = new BmnSiliconHitMaker(run_period, run_number, isExp);
    if (!isExp)
        siliconHM->SetCurrentConfig(BmnSiliconConfiguration::RunSpring2018); //set explicitly
    fRunAna->AddTask(siliconHM);

    // ====================================================================== //
    // ===                          GEM hit finder                        === //
    // ====================================================================== //
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(run_period, run_number, isExp);
    if (!isExp)
        gemHM->SetCurrentConfig(BmnGemStripConfiguration::RunSpring2018); //set explicitly
    gemHM->SetHitMatching(kTRUE);
    fRunAna->AddTask(gemHM);

    // ====================================================================== //
    // ===                          CSC hit finder                        === //
    // ====================================================================== //
    BmnCSCHitMaker* cscHM = new BmnCSCHitMaker(run_period, run_number, isExp);
    if (!isExp)
        cscHM->SetCurrentConfig(BmnCSCConfiguration::RunSpring2018); //set explicitly
    cscHM->SetHitMatching(kTRUE);
    fRunAna->AddTask(cscHM);

    // ====================================================================== //
    // ===                       Tracking (InnerTracker)                  === //
    // ====================================================================== //
    BmnInnerTrackingRun7* innerTF = new BmnInnerTrackingRun7(run_number, isField, isTarget);
    innerTF->SetFiltration(isExp); //we use filtration for experimental data only now
    fRunAna->AddTask(innerTF);

#endif

    // ====================================================================== //
    // ===                          TOF1 hit finder                       === //
    // ====================================================================== //
    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", !isExp, iVerbose, kFALSE);
    tof1HP->SetPeriodRun(run_period, run_number);
    fRunAna->AddTask(tof1HP);

    // ====================================================================== //
    // ===                          TOF2 hit finder                       === //
    // ====================================================================== //
    BmnTofHitProducer* tof2HP = new BmnTofHitProducer("TOF", "TOF700_geometry_run7.txt", !isExp, iVerbose, kFALSE);
    tof2HP->SetTimeResolution(0.115);
    tof2HP->SetProtonTimeCorrectionFile("bmn_run9687_digi_calibration.root");
    tof2HP->SetMCTimeFile("TOF700_MC_argon_qgsm_time_run7.txt");
    tof2HP->SetMainStripSelection(0); // 0 - minimal time, 1 - maximal amplitude
    tof2HP->SetSelectXYCalibration(2); // 0 - Petukhov, 1 - Panin SRC, 2 - Petukhov Argon (default)
    tof2HP->SetTimeMin(-2.f); // minimal digit time
    tof2HP->SetTimeMax(+39.f); // Maximal digit time
    tof2HP->SetDiffTimeMaxSmall(1.2f); // Abs maximal difference for small chambers
    tof2HP->SetDiffTimeMaxBig(3.5f); // Abs maximal difference for big chambers
    fRunAna->AddTask(tof2HP);

    // ====================================================================== //
    // ===                          Tracking (MWPC)                       === //
    // ====================================================================== //
    BmnMwpcTrackFinder* mwpcTF = new BmnMwpcTrackFinder(isExp, run_period, run_number);
    fRunAna->AddTask(mwpcTF);


    // ====================================================================== //
    // ===                          Tracking (DCH)                        === //
    // ====================================================================== //

    if(!isExp) {
        BmnDchHitProducer *dchHP = new BmnDchHitProducer();
        fRunAna->AddTask(dchHP);
    }

    BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(run_period, run_number, isExp);
    dchTF->SetTransferFunction("transfer_func.txt");
    fRunAna->AddTask(dchTF);

    // Residual analysis
    BmnResiduals* res = new BmnResiduals(run_period, run_number, isField);
    fRunAna->AddTask(res);

    Bool_t doAlign = kTRUE;
    if (!isExp) doAlign = kFALSE;
    BmnGlobalTracking* glTF = new BmnGlobalTracking(isField, isExp, kFALSE/*doAlign*/);
    fRunAna->AddTask(glTF);

    // ====================================================================== //
    // ===                             ZDC                                === //
    // ====================================================================== //
    BmnZdcAnalyzer * zdcAna = new BmnZdcAnalyzer();
    fRunAna->AddTask(zdcAna);

    // ====================================================================== //
    // ===                      Primary vertex finding                    === //
    // ====================================================================== //
    BmnVertexFinder* gemVF = new BmnVertexFinder(run_period, isField);
    fRunAna->AddTask(gemVF);
    
//    // ====================================================================== //
//    // ===          Alternative Primary vertex finding                    === //
//    // ====================================================================== //
//        CbmKF* kalman = new CbmKF("q", iVerbose);
//        fRunAna->AddTask(kalman);\
//    BmnPVAnalyzer* pv = new BmnPVAnalyzer(run_period, run_number, isField);
//    fRunAna->AddTask(pv);

    // Fill DST Event Header (if iVerbose = 0, then print progress bar)
    BmnFillDstTask* dst_task = new BmnFillDstTask(nEvents);
    dst_task->SetRunNumber(run_period, run_number);
    fRunAna->AddTask(dst_task);

    // -----   Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(inputFileName.Data());
    parIo2->open(parFileNameList, "in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    // -------------------------------------------------------------------------
    // -----   Initialize and run   --------------------------------------------
    fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
    cout << "Starting run" << endl;
    fRunAna->Run(nStartEvent, nStartEvent + nEvents);
    // -------------------------------------------------------------------------
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl; // marker of successful execution for CDASH
    cout << "Input  file is " + inputFileName << endl;
    cout << "Output file is " + bmndstFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}

int main(int argc, char** arg)
{
   run_reco_bmn();
}
