#if !defined(__CLING__) || defined(__MAKECLING__)
// ROOT includes
#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPRegexp.h"
#include "TKey.h"

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
#include "BmnMwpcTrackFinder.h"
#include "BmnGemTracking.h"
#include "BmnDchTrackFinder.h"
#include "BmnGlobalTracking.h"
#include "BmnGemVertexFinder.h"
#include "BmnGemResiduals.h"
#include "BmnSiResiduals.h"
#include "BmnLANDHitProducer.h"

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

#include "../run/bmnloadlibs.C"

void run_reco_bmn(TString inputFileName = "$VMCWORKDIR/macro/run/bmnsim.root",
        TString bmndstFileName = "$VMCWORKDIR/macro/run/bmndst.root",
        Int_t nStartEvent = 0,
        Int_t nEvents = 10000,
        TString alignCorrFileName = "default") { // Verbosity level (0=quiet, 1=event-level, 2=track-level, 3=debug)
    Int_t iVerbose = 0;
    // ----    Debug option   --------------------------------------------------
    gDebug = 0;
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // -----   Timer   ---------------------------------------------------------
    //TStopwatch timer;
    //timer.Start();
    // -------------------------------------------------------------------------
    // -----   Reconstruction run   --------------------------------------------
    FairRunAna* fRunAna = new FairRunAna();

    //Bool_t isField = kTRUE; // flag for tracking (to use mag.field or not)
    //Bool_t isTarget = kFALSE; // flag for tracking (run with target or not)
    //Bool_t isExp = kFALSE; // flag for hit finder (to create digits or take them from data-file)

    // Declare input source as simulation file or experimental data
    FairSource* fFileSource = NULL;
    // for experimental datasource
    //Int_t run_period, run_number;
    //Double_t fieldScale = 0.;
    //TPRegexp run_prefix("^run[0-9]+-[0-9]+:");
    /*if (inputFileName.Contains(run_prefix)) {
        Ssiz_t indDash = inputFileName.First('-'), indColon = inputFileName.First(':');
        // get run period
        run_period = TString(inputFileName(3, indDash - 3)).Atoi();
        // get run number
        run_number = TString(inputFileName(indDash + 1, indColon - indDash - 1)).Atoi();
        inputFileName.Remove(0, indColon + 1);

        if (!CheckFileExist(inputFileName)) {
            cout << "Error: digi file " + inputFileName + " does not exist!" << endl;
            exit(-1);
        }
        // set source as raw data file
        fFileSource = new BmnFileSource(inputFileName);

        // get geometry for run
        TString geoFileName = "current_geo_file.root";
        Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, (char*) geoFileName.Data());
        if (res_code != 0) {
            cout << "Geometry file can't be read from the database" << endl;
            exit(-1);
        }

        // get gGeoManager from ROOT file (if required)
        TFile* geoFile = new TFile(geoFileName, "READ");
        if (!geoFile->IsOpen()) {
            cout << "Error: could not open ROOT file with geometry: " + geoFileName << endl;
            exit(-2);
        }
        TList* keyList = geoFile->GetListOfKeys();
        TIter next(keyList);
        TKey* key = (TKey*) next();
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoManager"))
            key->ReadObj();
        else {
            cout << "Error: TGeoManager isn't top element in geometry file " + geoFileName << endl;
            exit(-3);
        }
        // set magnet field with factor corresponding to the given run
        UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
        if (pCurrentRun == 0) {
            exit(-2);
        }
        Double_t map_current = 55.87;
        Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
        if (*field_voltage < 10) {
            fieldScale = 0;
            isField = kFALSE;
        } else {
            fieldScale = (*field_voltage) / map_current;
        }
        BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
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
    } else { // for simulated files*/
        //if (!CheckFileExist(inputFileName)) return;
        fFileSource = new FairFileSource(inputFileName);
    //}
    fRunAna->SetSource(fFileSource);
    fRunAna->SetOutputFile(bmndstFileName);
    //fRunAna->SetGenerateRunInfo(false);

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the
    // FairParAsciiFileIo.
    // The FairParAsciiFileIo will take care to create on fly a
    // concatenated input parameter file, which is then used during the
    // reconstruction.
    //TList* parFileNameList = new TList();
    //TObjString stsDigiFile = "$VMCWORKDIR/parameters/sts_v15a_gem.digi.par";
    //parFileNameList->Add(&stsDigiFile);

    //TObjString tofDigiFile = "$VMCWORKDIR/parameters/tof_standard.geom.par";
    //parFileNameList->Add(&tofDigiFile);

    //if (iVerbose == 0) { // print only progress bar in terminal in quiet mode
    //    BmnCounter* cntr = new BmnCounter(nEvents);
    //    fRunAna->AddTask(cntr);
    //}
    // ====================================================================== //
    // ===                           Check Triggers                       === //
    // ====================================================================== //
    //BmnTriggersCheck* triggs = new BmnTriggersCheck(isExp);
    // fRunAna->AddTask(triggs);
    // ====================================================================== //
    // ===			     LAND hit finder			  === //
    // ====================================================================== //
    //BmnLANDHitProducer* land = new BmnLANDHitProducer("LAND", !isExp, iVerbose, kTRUE);
    //fRunAna->AddTask(land);
    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //
    //BmnMwpcHitFinder* mwpcHM = new BmnMwpcHitFinder(isExp);
    //mwpcHM->SetUseDigitsInTimeBin(kFALSE);
    //fRunAna->AddTask(mwpcHM);
    // ====================================================================== //
    // ===                         Silicon hit finder                     === //
    // ====================================================================== //
    //BmnSiliconHitMaker* siliconHM = new BmnSiliconHitMaker(isExp);
    //fRunAna->AddTask(siliconHM);
    // ====================================================================== //
    // ===                         GEM hit finder                         === //
    // ====================================================================== //
    /*
    BmnGemStripConfiguration::GEM_CONFIG gem_config;
    if (!isExp || run_period == 6)
        gem_config = BmnGemStripConfiguration::RunSpring2017;
    else if (run_period == 5) {
        gem_config = BmnGemStripConfiguration::RunWinter2016;
    }
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(isExp);
    gemHM->SetCurrentConfig(gem_config);
    // Set name of file with the alignment corrections for GEMs using one of the
    // two variants of the SetAlignmentCorrectionsFileName function defined in
    // BmnGemStripHitMaker.h
    if (isExp) {
        if (alignCorrFileName == "default")
            // retrieve from UniDb (default)
            gemHM->SetAlignmentCorrectionsFileName(run_period, run_number);
        else {
            // set explicitly, for testing purposes and for interactive
            // alignment; in case of determining alignment corrections from
            // scratch, set alignCorrFileName == "" (at first iteration) and it
            // will be properly used in BmnGemStripHitMaker.cxx, i.e. the input
            // alignment corrections will be set to zeros
            gemHM->SetAlignmentCorrectionsFileName(alignCorrFileName);
        }
    }
    gemHM->SetHitMatching(kTRUE);
    fRunAna->AddTask(gemHM);
    */
    // ====================================================================== //
    // ===                           TOF1 hit finder                      === //
    // ====================================================================== //
    //BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", !isExp, iVerbose, kTRUE);
    //tof1HP->SetPeriod(run_period);
    //tof1HP->SetOnlyPrimary(kTRUE);
    //fRunAna->AddTask(tof1HP);
    // ====================================================================== //
    // ===                           TOF2 hit finder                      === //
    // ====================================================================== //
    //BmnTofHitProducer* tof2HP = new BmnTofHitProducer("TOF", "TOF700_geometry_run6.txt", !isExp, iVerbose, kTRUE);
    //fRunAna->AddTask(tof2HP);
    // ====================================================================== //
    // ===                           Tracking (MWPC)                      === //
    // ====================================================================== //
    //BmnMwpcTrackFinder* mwpcTF = new BmnMwpcTrackFinder(isExp);
    //fRunAna->AddTask(mwpcTF);

    // ====================================================================== //
    // ===                           Tracking (GEM)                       === //
    // ====================================================================== //
    //BmnGemTracking* gemTF = new BmnGemTracking();
    //gemTF->SetTarget(isTarget);
    //gemTF->SetField(isField);
    //TVector3 vAppr = (isExp) ? TVector3(0.0, -3.5, -21.7) : TVector3(0.0, 0.0, -21.7);
    //gemTF->SetRoughVertex(vAppr);
    //fRunAna->AddTask(gemTF);

    // ====================================================================== //
    // ===                           Tracking (DCH)                       === //
    // ====================================================================== //
    //BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(isExp);
    //dchTF->SetTransferFunction("pol_coord00813.txt");
    //fRunAna->AddTask(dchTF);
    // ====================================================================== //
    // ===                          Global Tracking                       === //
    // ====================================================================== //
    //BmnGlobalTracking* globalTF = new BmnGlobalTracking();
    //globalTF->SetField(isField);
    //fRunAna->AddTask(globalTF);
/*
    // ====================================================================== //
    // ===                     Primary vertex finding                     === //
    // ====================================================================== //
    BmnGemVertexFinder* gemVF = new BmnGemVertexFinder();
    gemVF->SetField(isField);
    gemVF->SetVertexApproximation(vAppr);
    fRunAna->AddTask(gemVF);

    // Residual analysis
    if (isExp) {
        BmnGemResiduals* residAnalGem = new BmnGemResiduals(run_period, run_number, fieldScale);
        // residAnal->SetPrintResToFile("file.txt");
        // residAnal->SetUseDistance(kTRUE); // Use distance instead of residuals
        fRunAna->AddTask(residAnalGem);
        BmnSiResiduals* residAnalSi = new BmnSiResiduals(run_period, run_number, fieldScale);
        fRunAna->AddTask(residAnalSi);
    }
    */
    // -----   Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    //FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(inputFileName.Data());
    //parIo2->open(parFileNameList, "in");
    rtdb->setFirstInput(parIo1);
    //rtdb->setSecondInput(parIo2);
    //rtdb->setOutput(parIo1);
    //rtdb->saveOutput();
    // -------------------------------------------------------------------------
    // -----   Initialize and run   --------------------------------------------
    //fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
    cout << "Starting run" << endl;
    fRunAna->Run(nStartEvent, nStartEvent + nEvents);
    // -------------------------------------------------------------------------

    delete fFileSource;

    // -----   Finish   --------------------------------------------------------
    //timer.Stop();
    //Double_t rtime = timer.RealTime();
    //Double_t ctime = timer.CpuTime();
    //cout << endl << endl;
    cout << "Macro finished successfully." << endl; // marker of successful execution for CDASH
    cout << "Input  file is " + inputFileName << endl;
    cout << "Output file is " + bmndstFileName << endl;
    //cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}

int main(int argc, char** arg)
{
   run_reco_bmn();
}
