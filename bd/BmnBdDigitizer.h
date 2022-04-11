#ifndef BMNBDDIGITIZER_H
#define BMNBDDIGITIZER_H 1


#include "FairTask.h"

#include <map>

class TClonesArray;

class BmnBdDigitizer : public FairTask {

public:

  /** Default constructor **/
  BmnBdDigitizer();

  /** Destructor **/
  ~BmnBdDigitizer();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

private:

  /** Input array of BmnBdPoints **/
  TClonesArray* fPointArray;

  /** Output array of BmnBdDigits **/
  TClonesArray* fDigitArray;

  BmnBdDigitizer(const BmnBdDigitizer&);
  BmnBdDigitizer& operator=(const BmnBdDigitizer&);

  Int_t fNMod;

  ClassDef(BmnBdDigitizer, 0);

};

#endif