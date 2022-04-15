/*************************************************************************************
 *
 *         Class BmnZdcDigiScheme
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015   
 *
 ************************************************************************************/

#ifndef BMNZDCDIGISCHEME_H
#define BMNZDCDIGISCHEME_H

#include "FairGeoNode.h"
#include "BmnZdcGeoPar.h"
#include "BmnZdcDigiPar.h"

#include "TObject.h"
#include <map>
#include <vector>

using std::vector;
using std::pair;

  typedef std::vector<Int_t>          BmnZdcVolId_t;    // now - { VolumeId, MotherMotherCopyNo}
  typedef std::vector<Int_t>          BmnZdcDigiId_t;   // now - {GroupID, ModuleID, ChannelID}
  typedef std::vector<Double_t>       BmnZdcVolInfo_t;  // now - Center X,Y,Z; and Dx,Dy,Dz from center to the corner

class BmnZdcDigiScheme: public TObject
{
 public:

  BmnZdcDigiScheme();
  virtual ~BmnZdcDigiScheme();

  static BmnZdcDigiScheme* Instance();
  Bool_t Init    (BmnZdcGeoPar*    geoPar, BmnZdcDigiPar*    digiPar, Int_t pVerbose=0);

  Bool_t AddNodes (TObjArray* sensNodes, Int_t pVerbose=0);
  Bool_t CalcDimensions (Int_t pGlobalDetectorNumber, Int_t &nx, Int_t &ny, Int_t &nz);

  BmnZdcVolId_t*   CreateVolElement (FairGeoNode* nod, Int_t nodeNumber,
				     BmnZdcDigiId_t* right, Int_t pVerbose);
  Bool_t           CreateVolCopyElements  (BmnZdcVolId_t* left, BmnZdcDigiId_t* right);
  BmnZdcVolInfo_t* CreateVolInfoElement (FairGeoNode* nod, Int_t pVerbose);
  Bool_t           CreateVolInfoCopyElements  (BmnZdcDigiId_t* right, BmnZdcVolInfo_t *volInfo );

  Bool_t          IsVolumeExist (BmnZdcVolId_t* pVolId);
  BmnZdcDigiId_t  GetDigiId     (BmnZdcVolId_t* pVolId);
  Int_t           GetGroupID (BmnZdcVolId_t* pVolId);
  Int_t           GetChannelID  (BmnZdcVolId_t* pVolId);
  Bool_t          GetGroupIdModIdChanId (Int_t pMcVolumeNumber, Int_t pMcCopyNumber, Int_t pMotherCopyNumber, 
				  Int_t pMotherMotherCopyNumber, Int_t &pGroupId, Int_t &pChanId, Int_t &pModId);  

  Bool_t GetVolCenterXYZ        (BmnZdcDigiId_t* pDigiId, Double_t &x, Double_t &y,Double_t &z);
  Bool_t GetVolDxDyDz           (BmnZdcDigiId_t* pDigiId, Double_t &Dx, Double_t &Dy, Double_t &Dz);
  BmnZdcVolInfo_t* GetVolInfo (BmnZdcVolId_t* pVolId);

  void Print();
  void PrintVolume (Int_t volID, Int_t copyNoMotherMother=1);

  void GetZdcDimensions (Int_t &nx, Int_t &ny, Int_t &nz);
 
  BmnZdcDigiId_t  GetDigiIdFromCoords  (Double_t x, Double_t y, Double_t z);
  BmnZdcDigiId_t  GetDigiIdFromVolumeData  (Int_t pMcVolumeNumber, Int_t pMotherMotherCopyNumber);
  void SplitDigiID (BmnZdcDigiId_t digiID, Int_t &module_groupID, Int_t &modID, Int_t &chanID);

  inline BmnZdcDigiPar*     GetZdcDigiPar()      {return fZdcDigiPar;};

  protected:

  static BmnZdcDigiScheme* fInstance;       // Instance of singleton object
  static Int_t             fRefcount;       // Counter of references on this 
  static Bool_t            fInitialized;    // Defines whether was initialized

 private:

  std::map<BmnZdcVolId_t,BmnZdcDigiId_t> fVolToDigiIdMap;      //! correspondence for all active volumes (Zdc+ZdcPsd)
  std::map<BmnZdcDigiId_t,BmnZdcVolInfo_t*> fDigiToVolInfoMap; //! correspondence for active volumes 

  Int_t Nx;
  Int_t Ny;
  Int_t Nz;

  BmnZdcDigiPar*    fZdcDigiPar;       //! 
  TObjArray*        fPasNodes;         //!

  ClassDef(BmnZdcDigiScheme,1);

};

#endif // BMNZDCDIGISCHEME_H
