////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// copyEvents.C                                                               //
//                                                                            //
// An example macro how to copy only a part of events from a source file      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int copyEvents(TString inputFilePath, TString outputFilePath, Int_t nStartEvent = 10000, Int_t nEvents = 100)
{
    if (inputFilePath == "")
    {
        cout<<"ERROR: Input file path was not specified"<<endl;
        return -1;
    }
    gSystem->ExpandPathName(inputFilePath);

    if (outputFilePath == "")
    {
        cout<<"ERROR: Output file path was not specified"<<endl;
        return -2;
    }
    gSystem->ExpandPathName(outputFilePath);

    TFile* inputFile = new TFile(inputFilePath.Data());
    if (!inputFile)
    {
        cout<<"ERROR: Input file was not opened: "<<inputFilePath<<endl;
        return -3;
    }

    TFile* outputFile = new TFile(outputFilePath.Data(), "RECREATE");
    if (!outputFile)
    {
        cout<<"ERROR: Output file was not opened: "<<outputFilePath<<endl;
        return -4;
    }

    // loop on all entries to find all trees (or FairRootManager::GetTreeName() can be used)
    TTree* inputTree = (TTree*) inputFile->Get(FairRootManager::GetTreeName());
    if (!inputTree)
    {
        cout<<"ERROR: Input tree was not obtained: "<<FairRootManager::GetTreeName()<<endl;
        return -5;
    }
    TTree* outputTree = inputTree->CloneTree(0);
    inputTree->CopyAddresses(outputTree);

    const auto nentries = inputTree->GetEntries();
    if (nEvents == 0)
        nEvents = nentries - nStartEvent;
    else
        if ((nStartEvent+nEvents) > nentries) nEvents = nentries - nStartEvent;
    cout<<"Tree Name: "<<FairRootManager::GetTreeName()<<", event count = "<<nEvents<<endl;

    for (int i = nStartEvent; i < nStartEvent+nEvents; i++)
    {
        inputTree->GetEntry(i);
        outputTree->Fill();
    }
    outputTree->Write();

    delete inputFile;
    delete outputFile;
    return 0;
}
