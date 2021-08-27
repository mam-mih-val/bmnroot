//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigitize header file                 -----
// -----                  Created 30/08/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsDigitise
 *@author Volker Friese <v.friese@gsi.de>
 *@since 30.08.06
 *@version 1.0
 **
 ** CBM task class for digitising STS
 ** Task level SIM
 ** Produces objects of type CbmStsDigi out of CbmStsPoint.
 **/


#ifndef CBMSTSIDEALDIGITIZE_H
#define CBMSTSIDEALDIGITIZE_H 1

#include "FairTask.h"

#include "TStopwatch.h"

#include <list>
#include <map>

class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsStation;



class CbmStsIdealDigitize : public FairTask
{

 public:

  /** Default constructor **/
  CbmStsIdealDigitize();


  /** Standard constructor **/
  CbmStsIdealDigitize(Int_t iVerbose);


  /** Constructor with name **/
  CbmStsIdealDigitize(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsIdealDigitize();


  /** Execution **/
  virtual void Exec(Option_t* opt);



 private:

  CbmGeoStsPar*     fGeoPar;       /** Geometry parameter container **/
  CbmStsDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  CbmStsDigiScheme* fDigiScheme;   /** Digitisation scheme **/
  TClonesArray*     fPoints;       /** Input array of CbmStsPoint **/
  TClonesArray*     fDigis;        /** Output array of CbmStsDigi **/
  TClonesArray*     fDigiMatches;  /** Output array of CbmStsDigiMatches**/
  Int_t             fNPoints;     
  Int_t             fNFailed;
  Int_t             fNOutside;
  Int_t             fNMulti;
  Int_t             fNDigis;
  TStopwatch        fTimer;

  /** Map of active channels (pair detectorId, channel number) 
   ** to index of StsDigi **/
  std::map<std::pair<Int_t, Int_t>, Int_t> fChannelMap; //!
 

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Reset eventwise counters **/
  void Reset();

  CbmStsIdealDigitize(const CbmStsIdealDigitize&);
  CbmStsIdealDigitize operator=(const CbmStsIdealDigitize&);


  ClassDef(CbmStsIdealDigitize,1);

};

#endif


