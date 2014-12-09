// -------------------------------------------------------------------------
// -----                 CbmPsdHitProducerIdel header file             -----
// -----                  Created 15/05/12  by Alla                   -----
// -------------------------------------------------------------------------


/** CbmPsdIdealDigitize.h
 *@author Alla Maevskaya <alla@inr.ru>
 **
 ** The ideal digitizer produces digits  of type CbmPsdDigi as sum of Edep
 **for 6 scintilators .
 **/


#ifndef CBMPSDIdealDigitizer_H
#define CBMPSDIdealDigitizer_H 1


#include "FairTask.h"

class TClonesArray;



class CbmPsdIdealDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  CbmPsdIdealDigitizer();


  /** Destructor **/
  ~CbmPsdIdealDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 private:
  Int_t             fNDigis;
   
  /** Input array of CbmPsdPoints **/
  TClonesArray* fPointArray;

  /** Output array of CbmPsdDigits **/
  TClonesArray* fDigiArray; 

  CbmPsdIdealDigitizer(const CbmPsdIdealDigitizer&);
  CbmPsdIdealDigitizer operator=(const CbmPsdIdealDigitizer&);
  /** Reset eventwise counters **/
  void Reset();

  ClassDef(CbmPsdIdealDigitizer,1);

};

#endif
