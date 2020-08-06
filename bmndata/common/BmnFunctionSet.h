#ifndef BMNFUNCTIONSET_H
#define BMNFUNCTIONSET_H 1

#include "TString.h"

/** Class with static common functions to simplify experiment code **/
class BmnFunctionSet
{
 public:
    // check whether file exists
    static bool CheckFileExist(TString& fileName);
    // define whether simulation file
    static bool isSimulationFile(TString fileName);

 private:
    BmnFunctionSet() {}
    ~BmnFunctionSet() {}

 ClassDef(BmnFunctionSet,1)
};

#endif
