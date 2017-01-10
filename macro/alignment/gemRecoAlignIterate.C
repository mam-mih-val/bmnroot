// -----------------------------------------------------------------------------
// Macro for finding alignment corrections iteratively (only for GEMs so far)
//
// Reconstruct limited number of events -> run alignment -> run new
// reconstruction of the same events using corrections obtained at the previous
// iteration and run the alignment again, and so on, until either corrections
// become small or the maximal number of iterations is reached. Namely, if
// maxNumOfIterations==1, then the subset reconstruction run and alignment are
// run only once i.e. iterativity is degenerate. The obtained corrections then
// simply used for the full reconstruction run. But starting from
// maxNumOfIterations==2 the subset reconstruction run and alignment may be run
// up to maxNumOfIterations times.
//
// After each iteration, starting from the second, the newly obtained
// corrections are added to the existing cumulative corrections that will
// eventually be used for the full reconstruction run.
//
// Now, when to stop? There are several possibilities:
//
// 1. a trivial one is to stop at maxNumOfIterations which does not make much
// sense, of course;
//
// 2. relatively cheap, but evidently not perfect, is to stop when the newly
// obtained corrections are <= their errors;
//
// 3. when track reconstruction precision reaches its plateau at a given set of
// all other conditions; this is perfect, but not so easy to formulate and
// implement; note, that this approach will generally stop earlier!
//
// digiFileName - input file with digis, like file_digi.root.root. To process
// experimental data, you can use 'runN-NNN:' prefix, e.g.
// "run5-458:../digits_run5/bmn_run0458_digi.root"
// then the geometry is obtained from the Unified Database.
//
// However, if the file name is like
// "bmn_run005_Glob_000812_digi.root", then nothing to do: the geometry will be
// obtained from the Unified Database automatically without any prefix, although
// if prefix is still used, it also works correctly.
//
// nStartEvent - number (start with zero) of first event to process, default: 0
// nEvents - number of events to process
// -----------------------------------------------------------------------------
#include  <fstream>
#include  <vector>
#include  <TString.h>
#include  <TStopwatch.h>
//#include"alignment/BmnGemAlignmentCorrections.h"
//#include"BmnGemStripStationSet_RunWinter2016.h"
//#include"BmnGemStripHitMaker.h"

using      namespace std;

/*
void gemRecoAlignIterate(TString digiFileName="file_digi.root", Int_t maxNumOfIterations=1, Int_t nEvents=1e3, TString addInfo="", Bool_t isPrimary=kTRUE)
*/
void gemRecoAlignIterate(TString digiFileListFileName="digi_files.txt", Int_t maxNumOfIterations=1, Int_t nEvents=1e3, TString addInfo="", Bool_t isPrimary=kTRUE)
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
  //if ( ! CheckFileExist(digiFileName) ) return;
    if ( ! CheckFileExist(digiFileListFileName) ) return;
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // Here we start iterations, each consisting of these steps:
    //
    // 1. reconstruction, either using only geometry from the database (initial,
    // 0-th iteration), or (starting from the 1st iteration) additionally
    // correcting it with the cumulative misalignment corrections updated after
    // each alignment
    //
    // 2. obtaining the misalignment values
    //
    // 3. updating the cumulative misalignment values
    //
    // 4. checking whether this is the last iteration, according to a certain
    // criterion (TBD)
    //
    // maxNumOfIterations is the brutal-force limit

    // Create GEM detector ------------------------------------------------------
    BmnGemStripConfiguration::GEM_CONFIG fCurrentConfig = BmnGemStripConfiguration::RunWinter2016;
    BmnGemStripStationSet* StationSet; // Entire GEM detector
    StationSet = new BmnGemStripStationSet_RunWinter2016(fCurrentConfig);

    const Int_t nStat = StationSet->GetNStations();
    const Int_t nParams = 3;
    Double_t*** corr; // array to store the corrections
    corr = new Double_t**[nStat];
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
        corr[iStat] = new Double_t*[nModul];
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            corr[iStat][iMod] = new Double_t[nParams];
            for (Int_t iPar = 0; iPar < nParams; iPar++) {
                corr[iStat][iMod][iPar] = 0.;
            }
        }
    }
    // first store the list of fileas in the vector digiFileNames,
    // so that the file with their list is read only once
    //
    // open file with list of input digi files chosen for running the alignment
    ifstream digiFiles(digiFileListFileName);
    vector<TString> digiFileNames;
    TString fname;
    while (digiFiles >> fname) {
        digiFileNames.push_back(fname);
    }
    digiFiles.close();
    TString sumAlignCorrFileName(""); // at zero-th iteration reconstruction does not use
                                      // any corrections anyway, as they do not exist yet
    for (Int_t iterNr=0; iterNr < maxNumOfIterations; iterNr++) {
        TString itNr = TString::Format("0%i", iterNr); // "0" becomes "00" and so on
        // The reconstruction is run file-by-file in the following loop below,
        // while the subsequent alignment corrections are produced using
        // the whole chain of the newly produced bmndst files with the reconstructed events.
        //
        // Therefore, we also prepare a file with the list of these files inside the loop.
        // It will be used to create a chain inside the gemAlignment.C
        //
        // we will also need a vector for storing these bmndstFileName's at the current iteration
        vector<TString> bmndstFileNames;
        TRegexp re("digi"); // this will be used for forming the bmndstFileName's
        // Run reconstruction on limited number of events from each digi file
        // from the list that is defined in the ifstream digiFiles(digiFileListFileName)
        for (vector<TString>::const_iterator digiFileName=digiFileNames.begin(); digiFileName!=digiFileNames.end(); digiFileName++) {
            TString bmndstFileName = *digiFileName;
            // replace digi with bmndst and add addInfo and itNr 
            if (iterNr == 0) { // do not include the addInfo metainformation into the name of the bmndstFileName
                bmndstFileName(re) =          "bmndst_it"+itNr; // e.g. "somename_bmndst_it01.root"
            }
            else { // iterNr > 0        include the addInfo metainformation into the name of the bmndstFileName
                bmndstFileName(re) = addInfo+"_bmndst_it"+itNr;
            }
            // NB! at iterNr==0 sumAlignCorrFileName==""
          //gROOT->ProcessLine(".L $VMCWORKDIR/macro/run/run_reco_bmn.C");
            gROOT->LoadMacro("$VMCWORKDIR/macro/run/run_reco_bmn.C");
            run_reco_bmn(*digiFileName, bmndstFileName, 0, nEvents, kTRUE, sumAlignCorrFileName);
            // and also continue preparing list of the bmndst input files for the alignment:
            bmndstFileNames.push_back(bmndstFileName);
        }
        // Create file with the list of bmndstFileName's
        TString bmndstFileListFileName = digiFileListFileName;
        // replace digi with bmndst and add addInfo and itNr 
        if (iterNr == 0) { // do not include the addInfo metainformation into the name of the bmndstFileListFileName
            bmndstFileListFileName(re) =          "bmndst_it"+itNr; // e.g. "somename_bmndst_it01.root"
        }
        else { // iterNr > 0        include the addInfo metainformation into the name of the bmndstFileListFileName
            bmndstFileListFileName(re) = addInfo+"_bmndst_it"+itNr;
        }
        // create file with the list of new bmndst files:
        ofstream bmndstFiles(bmndstFileListFileName);
        for (vector<TString>::const_iterator bmndstFileName=bmndstFileNames.begin(); bmndstFileName!=bmndstFileNames.end(); bmndstFileName++) {
            bmndstFiles <<*bmndstFileName<< endl;
        }
        bmndstFiles.close();
        // define the name of the newAlignCorrFileName alignment output file:
      //gROOT->ProcessLine(.x $VMCWORKDIR/macro/alignment/gemAlignment.C(nEvents, bmndstFileName, addInfo));
        TString newAlignCorrFileName; // for now, it is semi-hardcoded: TBD!
        if (addInfo == "") {
            newAlignCorrFileName = "bmn_run005_Glob_new_align_corr_it"+itNr+".root"
        }
        else {
            newAlignCorrFileName = "bmn_run005_Glob_"+addInfo+"_new_align_corr_it"+itNr+".root";
        }
        // and  now run the alignment
        gROOT->LoadMacro("$VMCWORKDIR/macro/alignment/gemAlignment.C");
        gemAlignment(nEvents, bmndstFileListFileName, newAlignCorrFileName);

        sumAlignCorrFileName =  newAlignCorrFileName; // as a basis, and then replace "new" with "sum"
        re = "new";
        sumAlignCorrFileName(re) = "sum";
        // if iterNr == 0 nothing to update - in the next (iterNr=1) iteration use the only existing corrections
        if (iterNr == 0) {
            gROOT->ProcessLine(Form(".! cp %s %s", newAlignCorrFileName, sumAlignCorrFileName));
        }
        else { // iterNr >= 0: update and use sumAlignCorrFileName
          //Double_t*** corr;
          //TChain* sumCh = new TChain("cbmsim");
          //sumCh->Add(sumAlignCorrFileName.Data());
          //TClonesArray* sumCorrs = NULL;
          //sumCh->SetBranchAddress("BmnGemAlignmentCorrections", &sumCorrs);
          //
          //TChain* newCh = new TChain("cbmsim");
          //newCh->Add(newAlignCorrFileName.Data());
          //TClonesArray* newCorrs = NULL;
          //newCh->SetBranchAddress("BmnGemAlignmentCorrections", &newCorrs);
          //
          //
          //for (Int_t iEntry=0; iEntry<(sumCh->GetEntries()); iEntry++) {
          //    sumCh->GetEntry(iEntry);
          //    newCh->GetEntry(iEntry);
          //    for (Int_t iCorr=0; iCorr<(sumCorrs->GetEntriesFast()); iCorr++) {
          //        BmnGemAlignmentCorrections* sumAlign = (BmnGemAlignmentCorrections*)sumCorrs->UncheckedAt(iCorr);
          //        BmnGemAlignmentCorrections* newAlign = (BmnGemAlignmentCorrections*)newCorrs->UncheckedAt(iCorr);
          //        sumAlign->SetCorrections(sumAlign->GetCorrections().X() + newAlign->GetCorrections().X()
          //                                ,sumAlign->GetCorrections().Y() + newAlign->GetCorrections().Y()
          //                                ,sumAlign->GetCorrections().Z() + newAlign->GetCorrections().Z()
          //                                )
          //    }
          //}
            TFile* sumf = new TFile(sumAlignCorrFileName.Data());
            TTree* sumT = (TTree*)sumf->Get("cbmsim");
            TClonesArray* sumCorrs = NULL;
            sumT->GetBranch("BmnGemAlignmentCorrections")->SetAutoDelete(kFALSE);
            sumT->SetBranchAddress("BmnGemAlignmentCorrections",&sumCorrs);

            TFile* newf = new TFile(newAlignCorrFileName.Data());
            TTree* newT = (TTree*)newf->Get("cbmsim");
            TClonesArray* newCorrs = NULL;
            newT->GetBranch("BmnGemAlignmentCorrections")->SetAutoDelete(kFALSE);
            newT->SetBranchAddress("BmnGemAlignmentCorrections",&newCorrs);

            for (Int_t iEntry=0; iEntry<(sumT->GetEntries()); iEntry++) {
               sumCorrs->Clear();
               newCorrs->Clear();
               sumT->GetEntry(iEntry);
               newT->GetEntry(iEntry);
               for (Int_t iCorr=0; iCorr<(sumCorrs->GetEntriesFast()); iCorr++) {
                    BmnGemAlignmentCorrections* sumAlign = (BmnGemAlignmentCorrections*)sumCorrs->UncheckedAt(iCorr);
                    BmnGemAlignmentCorrections* newAlign = (BmnGemAlignmentCorrections*)newCorrs->UncheckedAt(iCorr);
                    sumAlign->SetCorrections(sumAlign->GetCorrections().X() + newAlign->GetCorrections().X(),
                                             sumAlign->GetCorrections().Y() + newAlign->GetCorrections().Y(),
                                             sumAlign->GetCorrections().Z() + newAlign->GetCorrections().Z())
               }
            }
        }
    }
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
}
