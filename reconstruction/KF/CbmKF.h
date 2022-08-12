/** The CbmKF Kalman Filter algorithm
 *
 * @author  S.Gorbunov
 * @version 0.0
 * @since   02.12.05
 * 
 * Description will be here
 *
 * To be used by high-level utilites like StsTrackFitter
 *
 */

#ifndef CBMKF_H
#define CBMKF_H

#include "CbmKFMaterial.h"
#include "CbmStsDigiScheme.h"
#include "CbmKFTrackInterface.h"
#include "BmnNewFieldMap.h"

#include "FairField.h"
#include "FairTask.h"
#include "FairGeoNode.h"

#include <map>
#include <vector>

class CbmKF : public FairTask {

 public:

  CbmKF(const char *name="KF", Int_t iVerbose = 1 );
  ~CbmKF();

  static CbmKF *Instance(){ return fInstance; }

  /// * FairTask methods
  
  void SetParContainers();
  InitStatus ReInit();
  InitStatus Init();
 
  /// * Utilites  
  
  Int_t GetMaterialIndex( Int_t uid );  
  
  /**  Propagation of (T, C) to z_out without material, using linearisation at qp0
   */
  
  Int_t Propagate( Double_t *T, Double_t *C, Double_t z_out, Double_t QP0, Bool_t line=false );

  Int_t PassMaterial( CbmKFTrackInterface &track, Double_t &QP0, Int_t ifst, Int_t ilst );
  Int_t PassMaterialBetween( CbmKFTrackInterface &track, Double_t &QP0, Int_t ifst, Int_t ilst );
  Int_t PassMaterialBetween( CbmKFTrackInterface &track, Double_t &QP0, CbmKFHit *fst, CbmKFHit *lst );
  
  /// * Stored materials
  
  std::vector<CbmKFMaterial*> vMaterial; 
  
  std::vector<CbmKFTube> vMvdMaterial;
  std::vector<CbmKFTube> vStsMaterial;
  std::vector<CbmKFWall> vSttMaterial;
  std::vector<CbmKFTube> vTargets;
  std::vector<CbmKFCone> vPipe;

  std::vector<CbmKFTube> vPassiveTube;
  std::vector<CbmKFWall> vPassiveWall;
  std::vector<CbmKFBox> vPassiveBox;
 
  /// * Usefull information 
  
  //FairField *GetMagneticField(){ return fMagneticField; }
  BmnNewFieldMap *GetMagneticField(){ return fMagneticField; }

  std::map<Int_t,Int_t> MvdStationIDMap;
  std::map<Int_t,Int_t> StsStationIDMap;
  std::map<Int_t,Int_t> SttStationIDMap;

  Int_t GetMethod(){ return fMethod; }

  void SetMethod(Int_t fm){ fMethod=fm; }

  //AZ CbmStsDigiScheme StsDigi;
  CbmStsDigiScheme *StsDigi;    //!

  int GetNMvdStations() const { return CbmKF::Instance()->vMvdMaterial.size(); }
  //AZ int GetNStsStations() const { return const_cast<CbmStsDigiScheme*>(&StsDigi)->GetNStations();}
  int GetNStsStations() const { return CbmStsDigiScheme::Instance()->GetNStations(); }

 private:
  
  static CbmKF *fInstance;
  
 // FairField *fMagneticField;

 BmnNewFieldMap* fMagneticField; //!

  
  Int_t fMethod; /* 0 = straight line,
		    1 = AnalyticLight
		    2 = Runge-Kutta 4 order, 
		 */
 
  std::map<Int_t,Int_t> fMaterialID2IndexMap;

  Int_t ReadTube( CbmKFTube &tube, FairGeoNode *node);
  CbmKFMaterial *ReadPassive( FairGeoNode *node);

 private:
  CbmKF(const CbmKF&);
  void operator=(const CbmKF&);
 public: 
  
  ClassDef( CbmKF, 1 );

};

#endif /* !CBMKF_H */
