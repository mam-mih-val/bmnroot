#ifndef BMNFUNCTIONSET_H
#define BMNFUNCTIONSET_H 1

#include "FairRunAnaProof.h"

#include "TString.h"
#include "TSystem.h"

/** Class with static common functions to simplify experiment code **/
class BmnFunctionSet
{
 public:
    // check whether file exists: 1 - exists, 0 - not exists, -1 - exists but cannot access with the mode (default: kFileExists)
    static int CheckFileExist(TString& fileName, int iVerbose = 0, EAccessMode mode = kFileExists);
    // check whether directory exists: 1 - exists, 0 - not exists, -1 - exists but cannot access with the mode (default: kFileExists)
    static int CheckDirectoryExist(TString& fileName, int iVerbose = 0, EAccessMode mode = kFileExists);
    // create directory tree for the file name if not exists: 1 - exists, 0 - created
    // error codes: -1 - exists but cannot access with the mode (default: kWritePermission), -2 - cannot be created, -3 - empty file name
    static int CreateDirectoryTree(TString& fileName, int iVerbose = 0, EAccessMode mode = kWritePermission);
    // define whether simulation file: 1 - true, 0 - false, <0 - some errors occured
    static int isSimulationFile(TString fileName);
    // check whether path is a directory
    static bool isDirectory(TString path);
    // configuring PROOF execution for FaiRunAna
    static FairRunAnaProof* EnableProof(Int_t proofWorkers);

 private:
    BmnFunctionSet() {}
    ~BmnFunctionSet() {}

 ClassDef(BmnFunctionSet,1)
};

#endif
