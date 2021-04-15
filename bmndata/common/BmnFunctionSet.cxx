#include "BmnFunctionSet.h"

#include "FairLogger.h"

#include "TSystem.h"
#include "TFile.h"

#include <iostream>
using namespace std;

// check whether file exists: 1 - exists, 0 - not exists, -1 - exists but cannot access with the mode (default: kFileExists)
int BmnFunctionSet::CheckFileExist(TString& fileName, int iVerbose, EAccessMode mode)
{
    gSystem->ExpandPathName(fileName);
    if (gSystem->AccessPathName(fileName.Data(), kFileExists) == true)
    {
        if (iVerbose) cout<<"No specified file: "<<fileName<<endl;
        return 0;
    }
    if ((mode != kFileExists) && (gSystem->AccessPathName(fileName.Data(), mode) == true))
    {
        if (iVerbose) cout<<"No required permissions to access the file: "<<fileName<<endl;
        return -1;
    }

    return 1;
}

// check whether directory exists: 1 - exists, 0 - not exists, -1 - exists but cannot access with the mode (default: kFileExists)
int BmnFunctionSet::CheckDirectoryExist(TString& fileName, int iVerbose, EAccessMode mode)
{
    gSystem->ExpandPathName(fileName);
    TString dirName(gSystem->DirName(fileName.Data()));
    if (gSystem->AccessPathName(dirName.Data(), kFileExists) == true)
    {
        if (iVerbose) cout<<"No specified directory: "<<dirName<<endl;
        return 0;
    }
    if ((mode != kFileExists) && (gSystem->AccessPathName(dirName.Data(), mode) == true))
    {
        if (iVerbose) cout<<"No required permissions to access the directory: "<<dirName<<endl;
        return -1;
    }

    return 1;
}

// create directory tree for the file name if not exists: 1 - exists, 0 - created, -1 - cannot be created, no access with the mode (default: kWritePermission)
int BmnFunctionSet::CreateDirectoryTree(TString& fileName, int iVerbose, EAccessMode mode)
{
    int status = CheckDirectoryExist(fileName, 0, mode);
    if (status < 0)
    {
        if (iVerbose) cout<<"The directory of the output file (\""<<fileName<<"\") does not provide required permissions"<<endl;
        return status;
    }
    if (status == 0)
    {
        TString dirName(gSystem->DirName(fileName.Data()));
        int status_parent = CreateDirectoryTree(dirName, 0, mode);
        if (status_parent < 0) return status_parent;
        if (gSystem->MakeDirectory(dirName) != 0)
        {
            if (iVerbose) cout<<"The directory of the output file (\""<<fileName<<"\") could not be created"<<endl;
            return -1;
        }
        return 0;
    }

    return 1;
}

bool BmnFunctionSet::isSimulationFile(TString fileName)
{
    gSystem->ExpandPathName(fileName);
    if (gSystem->AccessPathName(fileName.Data()) == true)
    {
        cout<<"ERROR: no specified file: "<<fileName<<endl;
        return false;
    }

    TFile* fRootFile = new TFile(fileName.Data());
    if (fRootFile->IsZombie())
    {
        cout<<"ERROR: opening the input file"<<endl;
        return false;
    }

    TObject* list = fRootFile->Get("BranchList");
    bool isSim = false;
    if (list) isSim = true;

    fRootFile->Close();
    delete fRootFile;
    delete list;

    return isSim;
}

ClassImp(BmnFunctionSet);
