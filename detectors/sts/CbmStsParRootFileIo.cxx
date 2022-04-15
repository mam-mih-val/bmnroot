//*-- AUTHOR : Denis Bertini
//*-- Modified : 06/10/2005 by Denis Bertini

/////////////////////////////////////////////////////////////
//  CbmStsParRootFileIo
//
//  Class for Start parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////

#include "CbmStsParRootFileIo.h"

#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"
#include "CbmStsDigiPar.h"

#include <iostream> 

using std::cout;
using std::cerr;
using std::endl;

ClassImp(CbmStsParRootFileIo)

CbmStsParRootFileIo::CbmStsParRootFileIo(FairParRootFile* f) : FairDetParRootFileIo(f) {
  // constructor sets the name of the detector I/O "CbmStsParIo"
  fName="CbmStsParIo";

}


CbmStsParRootFileIo::~CbmStsParRootFileIo() {
  // destructor
/*
    if (modulesFound) {
    delete modulesFound;
    modulesFound=0;
  }
  if (initModules) {
    delete initModules;
    initModules=0;
    }
*/
}

Bool_t CbmStsParRootFileIo::init(FairParSet* pPar,Int_t* set) {
  // Initializes a container called by name, but only the modules defined
  //   in the array 'set.
  // Calls the special read function for this container.
  // If it is called the first time, it reads the setup found in the file.
  const Text_t* name=pPar->GetName();
  if (pFile) {
    if (!strcmp(name,"StsDigiPar")) return read((CbmStsDigiPar*)pPar,set);
   }
  cerr<<"initialization of "<<name<<" not possible from ROOT file!"<<endl;
  return kFALSE;
}


Bool_t CbmStsParRootFileIo::read(CbmStsDigiPar* pPar,Int_t* set) {
    cout << "-I- CbmStsParRootFileIo : reading " << endl;
 /*
  // Reads and fills the container "StsDigiPar"
  Text_t* name=(char*)pPar->GetName();
  Int_t version=findInputVersion(name);
    if (version<=0)
    {
	pPar->setInputVersion(-1,inputNumber);
	return kFALSE;    // not in ROOT file
    }
  if (pPar->getInputVersion(inputNumber)==version
        && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  CbmStsCalPar* rStart=(CbmStsCalPar*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  initModules->Reset();
  for(Int_t m=0;m<pPar->getSize();m++) {
    if (set[m]) {
      CbmStsCalParMod& r=(*rStart)[m];
      CbmStsCalParMod& p=(*pPar)[m];
      Int_t pModSize=p.getSize();
      if (pModSize && r.getSize()==pModSize) {
        for(Int_t c=0;c<pModSize;c++) p[c].fill(r[c]);
        initModules->AddAt(m+1,m);
      } else allFound=kFALSE;
    }
  }
  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();
  printInfo("StartCalPar: module(s) initialized from Root file ");
  delete rStart;
  return allFound;
*/
    return kTRUE;
}

