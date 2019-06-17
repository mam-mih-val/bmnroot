#include "BmnFunctionSet.h"

#include "TSystem.h"

#include <iostream>
using namespace std;

bool BmnFunctionSet::CheckFileExist(TString fileName)
{
    gSystem->ExpandPathName(fileName);
    if (gSystem->AccessPathName(fileName.Data()) == true)
    {
        cout<<endl<<"no specified file: "<<fileName<<endl;
        return false;
    }

    return true;
}

ClassImp(BmnFunctionSet);
