#include <TClonesArray.h>
#include <TChain.h>

void  fitInvSpectrum(Char_t* fileName = "reco.root") {
 gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/geometry.C");

    TChain* out = new TChain("cbmsim");
    out->Add(fileName);
    cout << "#recorded entries = " << out->GetEntries() << endl;

    TClonesArray* particlePair = NULL;
    TClonesArray* particlePairCuts = NULL;
    out->SetBranchAddress("ParticlePair", &particlePair);
    out->SetBranchAddress("ParticlePairCuts", &particlePairCuts);
    
    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);
        
        for (Int_t iPair = 0; iPair < particlePair->GetEntriesFast(); iPair++) {
            BmnParticlePair* pair = (BmnParticlePair*) particlePair->UncheckedAt(iPair);
            // cout << pair->GetInvMass() << endl;
               
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