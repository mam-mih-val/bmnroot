#ifndef BMNSIMDDIGITIZER_H
#define BMNSIMDDIGITIZER_H 1


#include "FairTask.h"

#include <map>

class TClonesArray;

class BmnSiMDDigitizer : public FairTask {

public:

  /** Default constructor **/
  BmnSiMDDigitizer();

  /** Destructor **/
  ~BmnSiMDDigitizer();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

private:

  /** Input array of BmnSiMDPoints **/
  TClonesArray* fPointArray;

  /** Output array of BmnSiMDDigits **/
  TClonesArray* fDigitArray;

  Int_t ModByPoint(Double_t x, Double_t y);
  Double_t* fModsAngles;

  BmnSiMDDigitizer(const BmnSiMDDigitizer&);
  BmnSiMDDigitizer& operator=(const BmnSiMDDigitizer&);

  Int_t fNMod;

  ClassDef(BmnSiMDDigitizer, 0);

};

#endif