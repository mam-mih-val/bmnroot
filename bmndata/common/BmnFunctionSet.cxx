#include "BmnFunctionSet.h"

#include "FairLogger.h"

#include "TSystem.h"
#include "TFile.h"

#include <iostream>
using namespace std;

bool BmnFunctionSet::CheckFileExist(TString fileName)
{
    gSystem->ExpandPathName(fileName);
    if (gSystem->AccessPathName(fileName.Data()) == true)
    {
        LOG(FATAL)<<"ERROR: no specified file: "<<fileName;
        return false;
    }

    return true;
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
