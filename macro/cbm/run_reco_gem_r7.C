
// --------------------------------------------------------------------------
//
// Macro for reconstruction of simulated events with standard settings
//
// HitProducers in MVD, RICH, TRD, TOF, ECAL
// Digitizer and HitFinder in STS
// FAST MC for ECAL
// STS track finding and fitting (L1 / KF)
// TRD track finding and fitting (L1 / KF)
// RICH ring finding (ideal) and fitting
// Global track finding (ideal), rich assignment
// Primary vertex finding (ideal)
// Matching of reconstructed and MC tracks in STS, RICH and TRD
//
// V. Friese   24/02/2006
// Version     24/04/2007 (V. Friese)
//
// --------------------------------------------------------------------------
#include <TGDMLParse.h>
// CBM includes
/*#include "CbmKF.h"
#include "CbmKFSecondaryVertexFinder.h"
#include "CbmKFParticle.h"
#include "CbmKFTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsHit.h"
#include "CbmStsPoint.h"
#include "CbmStsStation.h"
#include "CbmVertex.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParam.h"
#include "BmnFileSource.h"
#include "BmnFieldConst.h"
#include "UniRun.h"
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"
#include "CbmStsDigiPar.h"
#include "TKey.h"

// ROOT includes
#include <TBranch.h>
#include <TClonesArray.h>
#include <TDatabasePDG.h>
#include <TFile.h>
#include <TGeoManager.h>
#include <TGeoCone.h>
#include <TGeoVolume.h>
#include <TH1.h>
#include <TH2.h>
#include <TMatrixD.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TTree.h>
#include <TVector3.h>
#include <TVectorD.h>
#include <Riostream.h>
#include <map>
#include <set>
#include <vector>
#include <TError.h>

#include "BmnEventHeader.h"
#include "TStopwatch.h"
#include "CbmBmnStsDigitize.h"
#include "CbmStsClusterFinder.h"
#include "CbmStsFindHits.h"
#include "CbmL1.h"
#include "CbmStsTrackFinder.h"
#include "CbmL1StsTrackFinder.h"
#include "CbmStsFindTracks.h"
#include "CbmPrimaryVertexFinder.h"
#include "CbmPVFinderKF.h"
#include "CbmFindPrimaryVertex.h"
#include "StsTrackExtrapToDCH.h"
*/

// GEMS_v16_AZ_3mm.root
// GEMS_RunSummer2016.root
// bmn_run991_stripdigitthr15.root GEMS_RunWinter2016_old.root SIGEMS_v1.root
void run_reco_gem_r7(TString inFile = "", TString outFile = "", TString geoFile = "SIGEMS_r7.root",
                       Int_t nEvents = 0, Int_t rnum = 0, Double_t Mscale = 1.366666667)
{
  Mscale =1250/900.0;
   // cout<<inFile<<endl;
   // exit(0);
   // ========================================================================
   //          Adjust this part according to your requirements
/*
   TFile         file(inFile);
   TTree *       tree = (TTree *)file.Get("BMN_DIGIT");
   Int_t         runID;
   TClonesArray *bevhtca;
   tree->SetBranchAddress("EventHeader", &bevhtca);
   tree->GetEntry(0); 
   // cout<<" RUUUNNNN: "<<((BmnEventHeader*)bevhtca->At(0))->GetRunId()<<endl;
   // exit(0);
   //(TClonesArray *) FairRootManager::Instance()->GetObject("StsTrack");
   runID = ((BmnEventHeader *)bevhtca->At(0))->GetRunId();
   file.Close();

   ifstream listOfFieldRuns("listOfFieldRuns.txt");

   string line;
   while (getline(listOfFieldRuns, line)) {
      std::istringstream rfs(line);
      int                run_n;
      double             field, scale;
      if (!(rfs >> run_n >> field >> scale)) {
         break;
      } // error

      if (runID == run_n) {
         Mscale = scale;
         // cout<<run_n<<" "<<field<<" "<<scale<<endl;
         break;
      }
   }

   listOfFieldRuns.close();
   if (Mscale < 0.5) exit(0);
   */
   // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
   Int_t iVerbose    = 0;
   gErrorIgnoreLevel = kFatal;
   // Int_t iVerbose = 3;
   // Input file (MC events)
   //  TString inFile = "data/AuAu_urqmd_3.3p1_4.0_gev__10000_ev__centr__seed_1_fscale88.mc.root";
   // TString inFile = "mc.root";
   // inFile = "bmn_run814_stripdigitthr15_noswap.root";
   // Parameter file
   // TString parFile = "data/AuAu_urqmd_3.3p1_4.0_gev__10000_ev__mbias__seed_0_params.root";
   //  TString dir = ""; // Output directory
   //  TString inFile = dir + mcFile; // MC transport file
   //  TString scriptFile = TString(gSystem->Getenv("MCFILE"));
   //  if (scriptFile != "") inFile = dir + scriptFile;
   TString parFile = inFile;

   // Output file
   // outFile = "814.root";
   //  TString recoFile = TString(gSystem->Getenv("RECOFILE"));
   //  if (recoFile != "") outFile = dir + recoFile;

   //  Digitisation files.
   // Add TObjectString containing the different file names to
   // a TList which is passed as input to the FairParAsciiFileIo.
   // The FairParAsciiFileIo will take care to create on the fly
   // a concatenated input parameter file which is then used during
   // the reconstruction.
   TList *parFileList = new TList();

   TString paramDir = gSystem->Getenv("VMCWORKDIR");
   paramDir += "/parameters";

   // TObjString stsDigiFile = paramDir + "/sts/sts_v12b_12344444_std.digi.par";
   // TObjString stsDigiFile = paramDir + "/sts_v17a_gem.digi.par";
   // TObjString stsDigiFile = paramDir + "/sts_v15a90_gem.digi.par";
   // TObjString stsDigiFile = paramDir + "/sts_1st_30.digi.par";
   // TObjString stsDigiFile = paramDir + "/sts_var.digi.par";
   TString stsMatBudgetFile = ""; // paramDir + "/sts/sts_matbudget_v12b_12344444.root";
   // TString stsMatBudgetFile = paramDir + "/sts_matbudget_var_fr.root";

   // TString parFile="sts_v16s_gem.digi.par";//"GEMS_RunWinter2016.root";

   TObjString stsDigiFile = "sts_v1_BMN_SI_GEM.digi.par"; // sts_v17BMN_gem.digi.par //18 for SI
   // TObjString stsDigiFile2 = "/home/pokat/bmnM/bmnR/AZ_data/sts/GEMS_v16_AZ_3mm.root";
   parFileList->Add(&stsDigiFile);
   //  parFileList->Add(&stsDigiFile2);

   // In general, the following parts need not be touched
   // ========================================================================

   // ----    Debug option   -------------------------------------------------
   gDebug = 0;
   // ------------------------------------------------------------------------

   // -----   Timer   --------------------------------------------------------
   TStopwatch timer;
   timer.Start();
   // ------------------------------------------------------------------------

   // ----  Load libraries   -------------------------------------------------
   //gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
   //bmnloadlibs();
   // ------------------------------------------------------------------------

   // -----   Reconstruction run   -------------------------------------------
   FairRunAna *run = new FairRunAna();
   run->SetSource(new BmnFileSource(inFile));
   run->SetOutputFile(outFile);
   // -----------------------------------------------------------------------

   // -----   Field map   -------------------------------------------
   //  Int_t run_period=1, run_number=065;
   //  UniRun* pCurrentRun = UniRun::GetRun(run_period, run_number);
   //  if (pCurrentRun == 0) {
   //      exit(-2);
   //  }
   //  Double_t fieldScale = 0.01;
   //  double map_current = 0.01;//900.0;
   //  int* current_current = pCurrentRun->GetFieldCurrent();
   //  if (current_current == NULL) {
   //      fieldScale = 0.01;

   //  } else
   //      fieldScale = (*current_current) / map_current;
   //  cout<<" SCALE: "<<fieldScale<<endl;
   //    fieldScale=TMath::Max(fieldScale,0.01);
   // FairRootManager::Instance()->InitSource();

   //  BmnEventHeader* bevh= (BmnEventHeader*)(
   //  ((TClonesArray*)FairRootManager::Instance()->GetObject("EventHeader"))->At(0) );
   // Int_t runn=bevh->GetEventId();

   BmnNewFieldMap *magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.dat");
   // Mscale=1.777777778;
   // gROOT->LoadMacro("/run/media/pokat/SSD250/reco/tango_avg_field.C");
   // Double_t Mscale=tango_avg_field(6,((BmnEventHeader*)bevhtca->At(0))->GetRunId());
   // if(TMath::Abs(Mscale-1.3333)>0.2) exit(0);//Mscale=1.333333333;

   magField->SetScale(Mscale); // 0.666666667);//2.0);//1.777777778);//2.0);//fieldScale);1.333333333
   // BmnFieldConst* magField = new BmnFieldConst();
   // magField->SetFieldRegion(-3000., 3000., -3000., 3000., -3000., 3000);
   // magField->SetField(0, 0.000001 , 0.);

   magField->Init();

   run->SetField(magField);

   cout << "FIELD SCALE: " << magField->GetScale() << endl;
   // exit(0);
   // ------------------------------------------------------------------------
   // exit(0);
   // =========================================================================
   // ===             Detector Response Simulation (Digitiser)              ===
   // ===                          (where available)                        ===
   // =========================================================================

   // -----   STS digitizer   -------------------------------------------------
   /*Double_t threshold  =  4;
   Double_t noiseWidth =  0.01;
   Int_t    nofBits    = 12;
   //Double_t electronsPerAdc    =  10;
   Double_t electronsPerAdc    =  20; //AZ
   Double_t StripDeadTime = 0.1;
   Double_t minStep    =  0.00004; //AZ
   CbmBmnStsDigitize* stsDigitize = new CbmBmnStsDigitize("STS Digitiser", iVerbose);
   stsDigitize->SetRealisticResponse();
   stsDigitize->SetFrontThreshold (threshold);
   stsDigitize->SetBackThreshold  (threshold);
   stsDigitize->SetFrontNoiseWidth(noiseWidth);
   stsDigitize->SetBackNoiseWidth (noiseWidth);
   stsDigitize->SetFrontNofBits   (nofBits);
   stsDigitize->SetBackNofBits    (nofBits);
   stsDigitize->SetFrontNofElPerAdc(electronsPerAdc);
   stsDigitize->SetBackNofElPerAdc(electronsPerAdc);
   stsDigitize->SetStripDeadTime  (StripDeadTime);
   stsDigitize->SetStep   (minStep); //AZ
   */
   CbmBmnStsDigitize *stsDigitize = new CbmBmnStsDigitize("STS Digitiser", iVerbose);
   stsDigitize->SetGeoFile(geoFile); // GP
   run->AddTask(stsDigitize);
   // -------------------------------------------------------------------------

   // =========================================================================
   // ===                      STS local reconstruction                     ===
   // =========================================================================
   // -----   STS Cluster Finder   --------------------------------------------
   FairTask *stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
   run->AddTask(stsClusterFinder);
   // -------------------------------------------------------------------------


  // -----   CSC Cluster Finder   -------------------------------------------- //
  //this task working, but isn't doing anything ! FOR NOW (16.07.18)
  // FairTask *CSCClusterFinder = new CSCClusterFinder();
  // run->AddTask(CSCClusterFinder);
   // -------------------------------------------------------------------------
  

   // -----   STS hit finder   ------------------------------------------------
   FairTask *stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
   run->AddTask(stsFindHits);
   // -------------------------------------------------------------------------

   // -----  STS hit matching   -----------------------------------------------
   //  FairTask* stsMatchHits = new CbmStsMatchHits("STS Hit Matcher", iVerbose);
   //  run->AddTask(stsMatchHits);
   // -------------------------------------------------------------------------

   // ---  STS track finding   ------------------------------------------------
   CbmKF *kalman = new CbmKF();
   run->AddTask(kalman);

   CbmL1 *l1 = new CbmL1();
   l1->SetMaterialBudgetFileName(stsMatBudgetFile);
   run->AddTask(l1);

   CbmStsTrackFinder *stsTrackFinder = new CbmL1StsTrackFinder();
   FairTask *         stsFindTracks  = new CbmStsFindTracks(iVerbose, stsTrackFinder);
   run->AddTask(stsFindTracks);
   
   
     // ---   STS track fitting   -----------------------------------------------
   //  CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
    // FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, iVerbose);
  //   run->AddTask(stsFitTracks);
     // -------------------------------------------------------------------------
   
   // -----   Primary vertex finding   ---------------------------------------

   CbmPrimaryVertexFinder *pvFinder   = new CbmPVFinderKF();
   CbmFindPrimaryVertex *  findVertex = new CbmFindPrimaryVertex(pvFinder); //
   run->AddTask(findVertex);
   
   // ------------------------------------------------------------------------

 //   StsTrackExtrapToDCH* extrapDCH= new StsTrackExtrapToDCH();
   // extrapDCH.fEff=1;
  //  run->AddTask(extrapDCH);

   /*
     // ---   STS track matching   ----------------------------------------------
     FairTask* stsMatchTracks = new CbmStsMatchTracks(iVerbose);
     run->AddTask(stsMatchTracks);
     // -------------------------------------------------------------------------


     // ---   STS track fitting   -----------------------------------------------
   ///  CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
   ///  FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, iVerbose);
   ///  run->AddTask(stsFitTracks);
     // -------------------------------------------------------------------------

     // ===                 End of STS local reconstruction                   ===
     // =========================================================================



     // =========================================================================
     // ===                        Global tracking                            ===
     // =========================================================================

   ///  CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
     // Tracking method to be used
     // "branch" - branching tracking
     // "nn" - nearest neighbor tracking
     // "weight" - weighting tracking
   ///  finder->SetTrackingType("branch");

     // Hit-to-track merger method to be used
     // "nearest_hit" - assigns nearest hit to the track
   ///  finder->SetMergerType("nearest_hit");

   ///  run->AddTask(finder);

     // -----   Primary vertex finding   ---------------------------------------
     CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
     CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder); //
     run->AddTask(findVertex);
     // ------------------------------------------------------------------------

     // ===                      End of global tracking                       ===
     // =========================================================================

     // ----------- TRD track Pid Like ----------------------
     // Since in the newest version of this method depends on the global
     // track the task has to move after the global tracking
   // FU 08.02.12 Switch the task off since the input file needed for the new geometry has to be generated first.
   //  CbmTrdSetTracksPidLike* trdSetTracksPidLikeTask =
   //  		new CbmTrdSetTracksPidLike("Likelihood", "Likelihood");
   //  run->AddTask(trdSetTracksPidLikeTask);
     // ----------------------------------------------------
   */
   // -----  Parameter database   --------------------------------------------
   //  TString stsDigi = gSystem->Getenv("VMCWORKDIR");
   // stsDigi += "/parameters/sts/";
   //  stsDigi += stsDigiFile;
   FairRuntimeDb *     rtdb   = run->GetRuntimeDb();
   FairParRootFileIo * parIo1 = new FairParRootFileIo();
   FairParAsciiFileIo *parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(parFileList, "in");
   rtdb->setFirstInput(parIo1);
   rtdb->setSecondInput(parIo2);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();

   // ------------------------------------------------------------------------

   // -----   Intialise and run   --------------------------------------------
   run->Init();
   cout << "Starting run" << endl;
   run->Run(0, nEvents);
   // ------------------------------------------------------------------------

   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is " << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
   cout << endl;
   // ------------------------------------------------------------------------

   //  delete run;

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
   exit(0);
}
