/* Macro reads ROOT file */
const char* pcChainName = "cbmsim";
const char* pcFileName = "$VMCWORKDIR/macro/raw/bmn_run1220_digi.root";
const char* pcBranchName = "GEM";

void readRootFile()
{
    TStopwatch timer;
    timer.Start();

    /* Load libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();

    TChain* rootTree = new TChain(pcChainName);
    rootTree->Add(pcFileName);

    // Activate branches
    TClonesArray* fClass = 0x00;
    rootTree->SetBranchAddress(pcBranchName, &fClass);

    Int_t event_count = rootTree->GetEntries();
    cout<<"Number of events in ROOT file = "<<event_count<<endl;

    for (Int_t i = 0; i < event_count; i++)
    {
        rootTree->GetEntry(i);

        Int_t fNobj = fClass->GetEntriesFast();
        cout<<"Number of "<<pcBranchName<<" = "<<fNobj<<" in event #"<<i<<endl;
    }// event loop
     
    timer.Print();
}
