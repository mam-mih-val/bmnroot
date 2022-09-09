/*************************************************************************************
 *
 *         Class BmnNdetDigiScheme
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015   
 *  Modified by M.Golubeva July 2022
 *
 ************************************************************************************/

#ifndef BMNNDETDIGISCHEME_H
#define BMNNDETDIGISCHEME_H

#include "FairGeoNode.h"
#include "BmnNdetGeoPar.h"
#include "BmnNdetDigiPar.h"

#include "TObject.h"
#include <map>
#include <vector>

using std::vector;
using std::pair;

  typedef std::vector<Int_t>          BmnNdetVolId_t;    // now - { VolumeId, MotherMotherCopyNo}
  typedef std::vector<Int_t>          BmnNdetDigiId_t;   // now - {GroupID, ModuleID, ChannelID}
  typedef std::vector<Double_t>       BmnNdetVolInfo_t;  // now - Center X,Y,Z; and Dx,Dy,Dz from center to the corner

class BmnNdetDigiScheme: public TObject
{
 public:

  BmnNdetDigiScheme();
  virtual ~BmnNdetDigiScheme();

  static BmnNdetDigiScheme* Instance();
  Bool_t Init    (BmnNdetGeoPar*    geoPar, BmnNdetDigiPar*    digiPar, Int_t pVerbose=0);

  Bool_t AddNodes (TObjArray* sensNodes, Int_t pVerbose=0);
  Bool_t CalcDimensions (Int_t pGlobalDetectorNumber, Int_t &nx, Int_t &ny, Int_t &nz);

  BmnNdetVolId_t*   CreateVolElement (FairGeoNode* nod, Int_t nodeNumber,
				     BmnNdetDigiId_t* right, Int_t pVerbose);
  Bool_t           CreateVolCopyElements  (BmnNdetVolId_t* left, BmnNdetDigiId_t* right);
  BmnNdetVolInfo_t* CreateVolInfoElement (FairGeoNode* nod, Int_t pVerbose);
  Bool_t           CreateVolInfoCopyElements  (BmnNdetDigiId_t* right, BmnNdetVolInfo_t *volInfo );

  Bool_t          IsVolumeExist (BmnNdetVolId_t* pVolId);
  BmnNdetDigiId_t  GetDigiId     (BmnNdetVolId_t* pVolId);
  Int_t           GetGroupID (BmnNdetVolId_t* pVolId);
  Int_t           GetChannelID  (BmnNdetVolId_t* pVolId);
  Bool_t          GetGroupIdModIdChanId (Int_t pMcVolumeNumber, Int_t pMcCopyNumber, Int_t pMotherCopyNumber, 
				  Int_t pMotherMotherCopyNumber, Int_t &pGroupId, Int_t &pChanId, Int_t &pModId);  

  Bool_t GetVolCenterXYZ        (BmnNdetDigiId_t* pDigiId, Double_t &x, Double_t &y,Double_t &z);
  Bool_t GetVolDxDyDz           (BmnNdetDigiId_t* pDigiId, Double_t &Dx, Double_t &Dy, Double_t &Dz);
  BmnNdetVolInfo_t* GetVolInfo (BmnNdetVolId_t* pVolId);

  void Print();
  void PrintVolume (Int_t volID, Int_t copyNoMotherMother=1);

  void GetNdetDimensions (Int_t &nx, Int_t &ny, Int_t &nz);
 
  BmnNdetDigiId_t  GetDigiIdFromCoords  (Double_t x, Double_t y, Double_t z);
  BmnNdetDigiId_t  GetDigiIdFromVolumeData  (Int_t pMcVolumeNumber, Int_t pMotherMotherCopyNumber);
  void SplitDigiID (BmnNdetDigiId_t digiID, Int_t &module_groupID, Int_t &modID, Int_t &chanID);

  inline BmnNdetDigiPar*     GetNdetDigiPar()      {return fNdetDigiPar;};

  protected:

  static BmnNdetDigiScheme* fInstance;       // Instance of singleton object
  static Int_t             fRefcount;       // Counter of references on this 
  static Bool_t            fInitialized;    // Defines whether was initialized

 private:

  std::map<BmnNdetVolId_t,BmnNdetDigiId_t> fVolToDigiIdMap;      //! correspondence for all active volumes (Ndet)
  std::map<BmnNdetDigiId_t,BmnNdetVolInfo_t*> fDigiToVolInfoMap; //! correspondence for active volumes 

  Int_t Nx;
  Int_t Ny;
  Int_t Nz;

  BmnNdetDigiPar*    fNdetDigiPar;       //! 
  TObjArray*        fPasNodes;         //!

  ClassDef(BmnNdetDigiScheme,1);

};

#endif // BMNNDETDIGISCHEME_H
