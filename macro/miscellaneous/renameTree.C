////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// renameTree.C                                                               //
//                                                                            //
// An example macro how to rename Tree in the ROOT file                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int renameTree(TString filePath = "", TString oldName="cbmsim", TString newName = "bmndata", TString newTitle = "digit")
{
    if (filePath == "")
    {
        cout<<"ERROR: File path was not specified"<<endl;
        return -1;
    }

    gSystem->ExpandPathName(filePath);

    TFile* rootFile = new TFile(filePath.Data(), "UPDATE");
    if (!rootFile)
    {
        cout<<"ERROR: File was not found: "<<filePath<<endl;
        return -2;
    }

    TTree* oldTree = (TTree*) rootFile->Get(oldName.Data());
    if (!oldTree)
    {
        cout<<"ERROR: Tree was not found: "<<oldName<<endl;
        return -3;
    }

    //oldTree->SetNameTitle(newName.Data(), newName.Data());
    oldTree->SetObject(newName.Data(), newTitle.Data());
    gDirectory->Delete(TString::Format("%s;*", oldName.Data()));

    rootFile->Write();
    rootFile->Close();

    delete rootFile;
    return 0;
}
