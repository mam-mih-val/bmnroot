// -------------------------------------------------------------------------
// -----                 BmnSttHitproducer header file                 -----
// -------------------------------------------------------------------------

#ifndef CBMHYPHITPRODUCER_H
#define CBMHYPHITPRODUCER_H 1


#include "FairTask.h"
#include "TH1F.h"
#include "TFile.h"
#include "BmnSttHit.h"
#include "TVector3.h"

class TClonesArray;
class TObjectArray;

class BmnSttHitProducer : public FairTask
{

 public:

  /** Default constructor **/  
  BmnSttHitProducer(const char* fileGeo);


  /** Destructor **/
  ~BmnSttHitProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  BmnSttHit* AddHit(Int_t trackID, Int_t detID, Float_t energy);
  void CreateStructure();
  

 private: 
   
  /** Input array of BmnSttPoints **/
  TClonesArray* fPointArray;

  /** Output array of BmnSttHit **/
  TClonesArray* fDigiArray;  

  TObjArray *fVolumeArray;
 
  /** Geo file to use **/
  TString fFileGeo; 
  Float_t eneThr;
  
 // map<Int_t, Float_t> emcX;
 // map<Int_t, Float_t> emcY;
 // map<Int_t, Float_t> emcZ;
 // map<Int_t, Float_t> emcTheta;
 // map<Int_t, Float_t> emcPhi;
 // map<Int_t, Float_t> emcTau;
  
  ClassDef(BmnSttHitProducer,1);
  
};

#endif
