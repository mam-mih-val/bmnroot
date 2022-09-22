// -------------------------------------------------------------------------
// -----                 BmnNdetHitproducer header file                 -----
// -----                 Created 14/08/06  by S.Spataro                -----
// *  Modified by M.Golubeva July 2022
// -------------------------------------------------------------------------

#ifndef BMNNDETDIGITIZER_H
#define BMNNDETDIGITIZER_H 1


#include <map>
#include "FairTask.h"
#include "TClonesArray.h"
#include "BmnNdetDigit.h"
#include "BmnNdetGeoPar.h"
#include "BmnNdetDigitScheme.h"

#include "TParameter.h"
#include "TH2F.h"

#include "TRandom3.h"

class BmnNdetDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  BmnNdetDigitizer(const char* name="BmnNdet Digi Producer");


  /** Destructor **/
  ~BmnNdetDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  //BmnNdetDigit* AddHit(Int_t module_groupID, Int_t modID, Int_t chanID,Float_t energy);
  BmnNdetDigit* AddHit(Double_t time, Int_t modID, Int_t chanID,Float_t energy);

  void CreateHistograms ( BmnNdetDigitId_t *pDigiID);

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

  /** Output array of BmnNdetDigit **/
  TClonesArray* fDigiArray; 


  /** Input geometry parameters container**/
  BmnNdetGeoPar* fGeoPar;
  
  ClassDef(BmnNdetDigitizer,2);
  
};

#endif
