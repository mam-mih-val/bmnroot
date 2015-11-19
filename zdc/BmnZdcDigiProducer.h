// -------------------------------------------------------------------------
// -----                 BmnZdcHitproducer header file                 -----
// -----                 Created 14/08/06  by S.Spataro                -----
// -------------------------------------------------------------------------

#ifndef BMNZDCDIGIPRODUCER_H
#define BMNZDCDIGIPRODUCER_H 1


#include <map>
#include "FairTask.h"
#include "TClonesArray.h"
#include "BmnZdcDigi.h"
#include "BmnZdcGeoPar.h"
#include "BmnZdcDigiScheme.h"

#include "TParameter.h"
#include "TH2F.h"


class BmnZdcDigiProducer : public FairTask
{

 public:

  /** Default constructor **/  
  BmnZdcDigiProducer(const char* name="BmnZdc Digi Producer");


  /** Destructor **/
  ~BmnZdcDigiProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  BmnZdcDigi* AddHit(Int_t module_groupID, Int_t modID, Int_t chanID,Float_t energy);

  void CreateHistograms ( BmnZdcDigiId_t *pDigiID);

 private: 
   
  virtual void SetParContainers(); 
 

 private: 
   
  /** Input array of BmnZdcPoints **/
  TClonesArray* fPointArray;

  /** Output array of BmnZdcDigi **/
  TClonesArray* fDigiArray; 

  TClonesArray* fELossZdcValue;

  TClonesArray* fELossZdcHisto;

  /** Input geometry parameters container**/
  BmnZdcGeoPar* fGeoPar;

  /** Output Histograms of X-Y energy map **/
  TH2F *fHistZdcEn;

  
  ClassDef(BmnZdcDigiProducer,1);
  
};

#endif
