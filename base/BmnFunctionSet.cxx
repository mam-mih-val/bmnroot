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
        if (iVerbose) cout<<"ERROR: no specified file: "<<fileName<<endl;
        return 0;
    }
    if ((mode != kFileExists) && (gSystem->AccessPathName(fileName.Data(), mode) == true))
    {
        if (iVerbose) cout<<"ERROR: no required permissions to access the file: "<<fileName<<endl;
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
        if (iVerbose) cout<<"ERROR: no specified directory: "<<dirName<<endl;
        return 0;
    }
    if ((mode != kFileExists) && (gSystem->AccessPathName(dirName.Data(), mode) == true))
    {
        if (iVerbose) cout<<"ERROR: no required permissions to access the directory: "<<dirName<<endl;
        return -1;
    }

    return 1;
}

// create directory tree for the file name if not exists: 1 - exists, 0 - created
// error codes: -1 - exists but cannot access with the mode (default: kWritePermission), -2 - cannot be created, -3 - empty file name
int BmnFunctionSet::CreateDirectoryTree(TString& fileName, int iVerbose, EAccessMode mode)
{
    if (fileName == "")
    {
        if (iVerbose) cout<<"ERROR: the specified file name is empty"<<endl;
        return -3;
    }

    int status = CheckDirectoryExist(fileName, 0, mode);
    if (status < 0)
    {
        if (iVerbose) cout<<"ERROR: the directory of the specified file (\""<<fileName<<"\") does not provide required permissions"<<endl;
        return status;
    }
    if (status == 0)
    {
        TString dirName(gSystem->DirName(fileName.Data()));
        int status_parent = CreateDirectoryTree(dirName, 0, mode);
        if (status_parent < 0) return status_parent;
        if (gSystem->MakeDirectory(dirName) != 0)
        {
            if (iVerbose) cout<<"ERROR: the directory of the specified file (\""<<fileName<<"\") could not be created"<<endl;
            return -2;
        }
        return 0;
    }

    return 1;
}

int BmnFunctionSet::isSimulationFile(TString fileName)
{
    gSystem->ExpandPathName(fileName);
    if (gSystem->AccessPathName(fileName.Data()) == true)
    {
        cout<<"ERROR: no specified file: "<<fileName<<endl;
        return -1;
    }

    TFile* fRootFile = new TFile(fileName.Data());
    if (fRootFile->IsZombie())
    {
        cout<<"ERROR: opening the input file failed"<<endl;
        return -2;
    }

    TObject* branch_list = fRootFile->Get("BranchList");
    if (branch_list == nullptr)
    {
        fRootFile->Close();
        return 0;
    }

    TObject* mctrack_find = branch_list->FindObject("MCTrack");
    if (mctrack_find == nullptr)
    {
        fRootFile->Close();
        return 0;
    }

    fRootFile->Close();
    return 1;
}

// check whether path is a directory
bool BmnFunctionSet::isDirectory(TString path)
{
    FileStat_t file_stat;
    gSystem->GetPathInfo(path.Data(), file_stat);
    if (R_ISDIR(file_stat.fMode))
        return true;
    else
        return false;
}

// check whether path is a directory
FairRunAnaProof* BmnFunctionSet::EnableProof(Int_t proofWorkers)
{
    cout<<"PROOF-Lite mode is activated"<<endl;
    TString strProofString = "";
    if (proofWorkers > 0)
        strProofString += TString::Format("workers=%d", proofWorkers);

    FairRunAnaProof* proofRun = new FairRunAnaProof(strProofString);
    proofRun->SetProofParName("$VMCWORKDIR/config/libBmnRoot.par");
    proofRun->SetProofOutputStatus("merge");

    TProof* proofSession = proofRun->GetProof();
    proofSession->SetLogLevel(0, TProofDebug::kAll);

    proofSession->SetParameter("PROOF_PacketizerStrategy", (Int_t) 0);
    proofSession->SetParallel(proofWorkers);

    return proofRun;
}

ClassImp(BmnFunctionSet);
