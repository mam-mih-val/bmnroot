// -------------------------------------------------------------------------
// -----                 BmnNdetHitproducer header file                 -----
// -----                 Created 14/08/06  by S.Spataro                -----
// *  Modified by M.Golubeva July 2022
// -------------------------------------------------------------------------

#ifndef BMNNDETDIGIPRODUCER_H
#define BMNNDETDIGIPRODUCER_H 1


#include <map>
#include "FairTask.h"
#include "TClonesArray.h"
#include "BmnNdetDigi.h"
#include "BmnNdetGeoPar.h"
#include "BmnNdetDigiScheme.h"

#include "TParameter.h"
#include "TH2F.h"

#include "TRandom3.h"

class BmnNdetDigiProducer : public FairTask
{

 public:

  /** Default constructor **/  
  BmnNdetDigiProducer(const char* name="BmnNdet Digi Producer");


  /** Destructor **/
  ~BmnNdetDigiProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  //BmnNdetDigi* AddHit(Int_t module_groupID, Int_t modID, Int_t chanID,Float_t energy);
  BmnNdetDigi* AddHit(Double_t time, Int_t modID, Int_t chanID,Float_t energy);

  void CreateHistograms ( BmnNdetDigiId_t *pDigiID);

   inline void SetPix2Mip(Double_t setValue) { fPix2Mip = setValue; }
   inline void SetMIPEnergy(Double_t setValue) { fMIPEnergy = setValue; }
   inline void SetMIPNoise(Double_t setValue) { fMIPNoise = setValue; }
   inline void SetMIP2GeV(Double_t setValue) { fMIP2GeV = setValue; }

   inline void SetMappingFile_modules(TString mappingFile_modules) { fMappingFile_modules = mappingFile_modules;}
   inline void SetMappingFile_sections(TString mappingFile_sections) { fMappingFile_sections = mappingFile_sections;}

 private: 
   
  virtual void SetParContainers(); 
 

 private: 
   
    TRandom3 *fRandom3;

   Double_t fPix2Mip;   // MPPC pixels per MIP
   Double_t fMIPEnergy; // MIP energy (5 MeV)
   Double_t fMIPNoise;  // MIP noise level
   Double_t fMIP2GeV;   // MIP to GeV

   TString fMappingFile_modules;  // mapping file for nDet modules (X,Y)
   TString fMappingFile_sections;  // mapping file for nDet sections 
   Double_t fSectionZ[70]; // section Z coordinates
   Double_t fModuleX[1000]; // module X coordinates
   Double_t fModuleY[1000]; // module Y coordinates

   Double_t RecoEnergy(Double_t pfELoss);

 /** Input array of BmnNdetPoints **/
  TClonesArray* fPointArray;

  /** Output array of BmnNdetDigi **/
  TClonesArray* fDigiArray; 


  /** Input geometry parameters container**/
  BmnNdetGeoPar* fGeoPar;
  
  ClassDef(BmnNdetDigiProducer,2);
  
};

#endif
