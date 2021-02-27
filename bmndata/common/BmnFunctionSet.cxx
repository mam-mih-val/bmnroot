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
        if (iVerbose) LOG(FATAL)<<"No specified file: "<<fileName;
        return 0;
    }
    if ((mode != kFileExists) && (gSystem->AccessPathName(fileName.Data(), mode) == true))
    {
        if (iVerbose) LOG(FATAL)<<"No required permissions to access the file: "<<fileName;
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
        if (iVerbose) LOG(FATAL)<<"No specified directory: "<<dirName;
        return 0;
    }
    if ((mode != kFileExists) && (gSystem->AccessPathName(dirName.Data(), mode) == true))
    {
        if (iVerbose) LOG(FATAL)<<"No required permissions to access the directory: "<<dirName;
        return -1;
    }

    return 1;
}

// create directory tree for the file name if not exists: 1 - exists, 0 - not existed, -1 - exists but cannot access with the mode (default: kWritePermission)
int BmnFunctionSet::CreateDirectoryTree(TString& fileName, int iVerbose, EAccessMode mode)
{
    int status = CheckDirectoryExist(fileName, mode);
    if (status < 0)
    {
        if (iVerbose) LOG(FATAL)<<"The directory of the output file (\""<<fileName<<"\") does not provide required permissions"<<endl;
        return status;
    }
    if (status == 0)
    {
        TString dirName(gSystem->DirName(fileName.Data()));
        if (gSystem->MakeDirectory(dirName) != 0)
        {
            if (iVerbose) LOG(FATAL)<<"The directory of the output file (\""<<fileName<<"\") could not be created"<<endl;
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
        LOG(FATAL)<<"ERROR: no specified file: "<<fileName;
        return false;
    }

    TFile* fRootFile = new TFile(fileName.Data());
    if (fRootFile->IsZombie())
    {
        LOG(FATAL)<<"ERROR: opening the input file";
        return false;
    }

    TObject* list = fRootFile->Get("BranchList");
    bool isSim = false;
    if (list) isSim = true;

    fRootFile->Close();

    return isSim;
}

ClassImp(BmnFunctionSet);
