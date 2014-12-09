#ifndef CBMSTSPARROOTFILEIO_H
#define CBMSTSPARROOTFILEIO_H

#include "FairDetParRootFileIo.h"

#include "TFile.h"
#include "TArrayI.h"

class FairParRootFile;
class FairParSet;
class CbmStsDigiPar;

class CbmStsParRootFileIo : public FairDetParRootFileIo {
public:
  CbmStsParRootFileIo(FairParRootFile* f);
  ~CbmStsParRootFileIo();
  Bool_t init(FairParSet*,Int_t*);
  Bool_t read(CbmStsDigiPar*,Int_t*);
  ClassDef(CbmStsParRootFileIo,0) // Class for STS parameter I/O from ROOT file
};

#endif  /* !CBMSTSPARROOTFILEIO_H */










