
// -------------------------------------------------------------------------
// -----                       CbmPsdv1 source file                   -----
// -----                  Created 04/02/05  by Alla Maevskaya              -----
// -------------------------------------------------------------------------

#include "CbmPsdv1.h"

#include "CbmPsdPoint.h"
#include "CbmStack.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoBBox.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdv1::CbmPsdv1() {
  fPsdCollection = new TClonesArray("CbmPsdPoint");
  fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmPsdv1::CbmPsdv1(const char* name, Bool_t active)
  : CbmPsd(name, active) {
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdv1::~CbmPsdv1() {
 if (fPsdCollection) {
   fPsdCollection->Delete(); 
   delete fPsdCollection;
 }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmPsdv1::ProcessHits(FairVolume* vol)
{
  //  cout<<" CbmPsdv1::ProcessHits in "<<vol->GetName()<<endl;
  //if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;


  // Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    ResetParameters();
    fEloss   = 0.;
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fTime    = gMC->TrackTime() * 1.0e09;
    fLength  = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
    Int_t copyNo;
    gMC->CurrentVolID(copyNo);
    //fVolumeID=copyNo;
    Int_t iCell, iNumm ;
    gMC->CurrentVolOffID(1, iCell);
    fModuleID=iCell;
    gMC->CurrentVolOffID(2, iNumm);
    fVolumeID=iNumm;
    //    cout<<" CbmPsdv1::ProcessHits fVolumeID"<<fVolumeID<<" fModuleID "<<fModuleID<<" copyNo "<<gMC->CurrentVolID(copyNo)<<endl;
    }
  if (gMC->IsTrackInside() ) {
    // Sum energy loss for all steps in the active volume
    fEloss += gMC->Edep();
    if( gMC->IsTrackStop()       ||
	gMC->IsTrackDisappeared()   ) {
      fEloss += gMC->Edep();
      
      Int_t pdg = gMC->TrackPid();
      //  cout<<" pdg "<<pdg<<endl;
      //     if (pdg == 1000791970)
	
      // if (strstr(fDebug,"hit"))
	//    if(pdg == 211 || pdg==-211)
        if(pdg>1000000000)
	//  if(pdg == 2212 )
	Info("ProcessHits track inside","track %d pdg %d volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), time %f fLength %f dE=%.3e",
	     // printf("track %d pdg %i, volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), dE=%.3e,  pdg %i \n",
	     fTrackID, pdg, fVolumeID, gMC->CurrentVolName(),
	     fPos.X(),fPos.Y(),fPos.Z(),fMom.Px(),fMom.Py(),fMom.Pz(),fTime, fLength, fEloss);
      
      // Create CbmPsdPoint
      CbmPsdPoint *fPoint =  AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
				    TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),fTime, fLength,fEloss);
      fPoint->SetModuleID(fModuleID);
      fPoint->SetTrackID(fTrackID);
      //    cout<<" track "<<fPoint->GetTrackID()<<" pdg "<<pdg<<endl;
    }
  }
  
  if ( gMC->IsTrackExiting() /*   ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared() */  ) {
     // Sum energy loss for all steps in the active volume
    fEloss += gMC->Edep();
    
    Int_t pdg = gMC->TrackPid();
    //  cout<<" pdg "<<pdg<<endl;
    //     if (pdg == 1000791970)
    
    //    if (strstr(fDebug,"hit"))
    //    if(pdg == 211 || pdg==-211)
    if(pdg>1000000000 )
    //  if(pdg == 2212 )
     Info("ProcessHits","track %d pdg %d volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), time %f fLength %f dE=%.3e",
      // printf("track %d pdg %i, volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), dE=%.3e,  pdg %i \n",
	 fTrackID, pdg, fVolumeID, gMC->CurrentVolName(),
	 fPos.X(),fPos.Y(),fPos.Z(),fMom.Px(),fMom.Py(),fMom.Pz(),fTime, fLength, fEloss);
    
    // Create CbmPsdPoint
    CbmPsdPoint *fPoint =  AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
				  TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),fTime, fLength,fEloss);
    fPoint->SetModuleID(fModuleID);
    fPoint->SetTrackID(fTrackID);
    //    cout<<" track "<<fPoint->GetTrackID()<<" pdg "<<pdg<<endl;
  }


   // Increment number of ecal points for TParticle

  ((CbmStack*)gMC->GetStack())->AddPoint(kZDC, fTrackID);
    /*
  // Increment number of ecal points for TParticle
  Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
  Int_t nPsdPoints = (points & (15<<16)) >> 16;
  nPsdPoints ++;
  if (nPsdPoints > 15) nPsdPoints = 15;
  points = ( points & ( ~ (15<<16) ) ) | (nPsdPoints << 16);
  gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
   */
  //  ResetParameters();
    
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method CreateMaterial   ----------------------------------
void CbmPsdv1::CreateMaterial() {
}
// -----   Public method ConstructGeometry   -------------------------------
void CbmPsdv1::ConstructGeometry() {
  // Create materials, media and volume

// Create materials, media and volume
  //-----------List of Materials and Mixtures--------------
  
  TGeoMaterial *mat10 = new TGeoMaterial("IRON",55.85,26,7.87);
  mat10->SetUniqueID(  10);
  TGeoMaterial *mat13 = new TGeoMaterial("LEAD",207.19,82,11.35);
  mat13->SetUniqueID(  13);
  TGeoMaterial *mat15 = new TGeoMaterial("AIR",14.61,7.3,0.1205000E-02);
  mat15->SetUniqueID(  15);
  TGeoMaterial *mat16 = new TGeoMaterial("VACUUM",0,0,0);
  mat16->SetUniqueID(  16);
  TGeoMaterial *mat20 = new TGeoMaterial("SILICON",28.09,14,2.33);
  mat20->SetUniqueID(  20);
  TGeoMaterial *mat24 = new TGeoMaterial("HE_GAS",4,2,0.1780000E-03);
  mat24->SetUniqueID(  24);
  TGeoMixture *mat25 = new TGeoMixture("PLASTIC",2,   1.03200    );
  mat25->SetUniqueID(  25);
  mat25->DefineElement(0,12.01,6,0.9225687);
  mat25->DefineElement(1,1.008,1,0.7743125E-01);
  TGeoMixture *mat38 = new TGeoMixture("TYVEC",2,  0.930000    );
  mat38->SetUniqueID(  38);
  mat38->DefineElement(0,12.011,6,0.8562772);
  mat38->DefineElement(1,1.008,1,0.1437228);
  
  //-----------List of Tracking Media--------------
  
  TGeoMedium *med1 = new TGeoMedium("AIR",      1,15,0,1,0.19,0.25,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med2 = new TGeoMedium("VACUUM",   2,16,0,1,0.19,0.25,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med9 = new TGeoMedium("IRON",     9,10,0,1,0.19,1,-1,-1,0.1,-1);
  TGeoMedium *med10 = new TGeoMedium("HE_GAS", 10,24,0,1,0.19,0.25,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med11 = new TGeoMedium("PLASTIC",11,25,1,0,0,1,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med24 = new TGeoMedium("LEAD",   24,13,0,0,0,1,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med27 = new TGeoMedium("PLASTIC",27,25,0,1,0.19,1,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med32 = new TGeoMedium("TYVEC",  32,38,0,0,0,1,-1,-1,0.1000000E-02,-1);

 
  //-----------List of Rotation matrices--------------
  
  TGeoMaterial *material = 0;
  TGeoMedium   *medium   = 0;
  Float_t *buf = 0;
  /* 
  TGeoVolume *VETO = gGeoManager->MakeBox("VETO",med1, 70, 50, 62.4);
  TGeoVolume *VHLL = gGeoManager->MakeBox("VHLL",med1,10,10,62.4);//hole
  
  //Large modules
  TGeoVolume *VMDL = gGeoManager->MakeBox("VMDL",med9,10,10,62.4);
  TGeoVolume *VFEL = gGeoManager->MakeBox("VFEL",med9,9.9,9.85,1);
  TGeoVolume *VPBL = gGeoManager->MakeBox("VPBL",med24,9.9,9.85,0.8);
  TGeoVolume *VTYL = gGeoManager->MakeBox("VTYL",med32,9.9,9.85,0.22);
  TGeoVolume *VSCL = gGeoManager->MakeBox("VSCL",med11,9.88,9.83,0.2);
  TGeoVolume *VRFL = gGeoManager->MakeBox("VRFL",med27,0.1,8.6,1);
  TGeoVolume *VRPL = gGeoManager->MakeBox("VRPL",med27,0.1,8.6,0.8);
  TGeoVolume *VRYL = gGeoManager->MakeBox("VRYL",med27,0.01,8.6,0.22);
  TGeoVolume *VRAL = gGeoManager->MakeBox("VRAL",med27,0.09,8.6,0.01);
  TGeoVolume *VRSL = gGeoManager->MakeBox("VRSL",med27,0.09,8.6,0.2);
 
  
  Float_t zPSD= 1360;
  Float_t xPSD= 7.5;
  gGeoManager->Node("VETO", 1, "cave", xPSD, 0, zPSD, 0, kTRUE, buf, 0);
  gGeoManager->Node("VMDL", 1, "VETO", 60, 40,0., 0, kTRUE, buf, 0);
  gGeoManager->Node("VFEL",1,"VMDL", 0, 0, -61.4, 0, kTRUE, buf, 0 );
  gGeoManager->Node("VRFL",1,"VFEL", 9.8, 0.75, 0, 0,kTRUE, buf, 0);
  gGeoManager->Node("VFEL",2,"VMDL", 0, 0, 61.4, 0, kTRUE, buf, 0 );

  gGeoManager->Node("VTYL",1, "VMDL", 0, 0, -60.18, 0,kTRUE, buf, 0);
  gGeoManager->Node("VRYL",1, "VTYL", 9.89, 0.75, 0,  0,kTRUE, buf, 0);
  gGeoManager->Node("VRAL", 1, "VTYL", 9.79, 0.75, -0.21, 0, kTRUE, buf, 0);
  gGeoManager->Node("VRAL", 2, "VTYL", 9.79, 0.75, 0.21,  0,kTRUE, buf, 0);
  gGeoManager->Node("VSCL", 1, "VTYL", 0, 0, 0,  0,kTRUE, buf, 0);
  AddSensitiveVolume(VSCL);
  gGeoManager->Node("VRSL", 1, "VSCL", 9.79, 0.75, 0, 0,kTRUE, buf, 0); 
  gGeoManager->Node("VPBL",2, "VMDL", 0, 0, -59.16,  0,kTRUE, buf, 0); 
  gGeoManager->Node("VRPL",1, "VPBL", 9.8, 0.75, 0, 0, kTRUE, buf, 0);
  for (Int_t ivol=2; ivol<61; ivol++)
    {
      Float_t zvtyl = -60.18 + 2.04*(ivol-1); 
      gGeoManager->Node("VTYL",ivol, "VMDL", 0, 0, zvtyl , 0,kTRUE, buf, 0);
      Float_t zvpbl = -59.16 + 2.04*(ivol-1); 
      gGeoManager->Node("VPBL",ivol+1, "VMDL", 0, 0, zvpbl , 0, kTRUE, buf, 0); 
      //    cout<<ivol<<"zvtyl "<<zvtyl<<" zvpbl "<<zvpbl<<endl;
      fNbOfSensitiveVol++;
    }
   Float_t xCur=-60., yCur=-40.;
  Int_t iMod=0;
  for(Int_t iy=0; iy<5; iy++) {
    for(Int_t ix=0; ix<7; ix++) {
      iMod++;
 	if(iMod==32) 
	  gGeoManager->Node("VHLL", iMod, "VETO", xCur,yCur,0,  0,kTRUE, buf, 0); 
	else
	  gGeoManager->Node("VMDL", iMod, "VETO", xCur,yCur,0, 0, kTRUE, buf, 0); 
       	cout <<"iMod,xxxx,yyyy " <<iMod <<" " <<xCur <<" " <<yCur <<endl;


	fNbOfSensitiveVol++;
	// }
      // if(ix==6) xCur=80.;
      xCur=xCur + 20.;      
    }//for(Int_t ix==0; ix<9; ix++)
    xCur=-80;
    yCur=yCur + 20.;
  }//for(Int_t iy==0; iy<9; iy++)
*/
   TGeoVolume *HOLE = gGeoManager->MakeBox("HOLE", med1 ,10, 10, 1);
   TGeoVolume *FW = gGeoManager->MakeBox("FW", med11 ,100, 100, 1);
   AddSensitiveVolume(FW);
  Float_t zfw[4] = {700, 1000, 1300, 1900};
  // Float_t xhole [4] = {4., 6., 7.9, 11.7};
  Float_t xhole [4] = {4., 15., 7.9, 11.7};
   gGeoManager->Node("FW", 1, "cave", xhole[0], 0, zfw[0], 0, kTRUE, buf, 0);
    gGeoManager->Node("HOLE", 1, "FW", 0, 0, 0, 0, kTRUE, buf, 0);
   fNbOfSensitiveVol++;
   for(Int_t iz=1; iz<4; iz++) {
   gGeoManager->Node("FW", iz+1, "cave", xhole[iz], 0, zfw[iz], 0, kTRUE, buf, 0);
     fNbOfSensitiveVol++;
  }
   

}
// -------------------------------------------------------------------------

ClassImp(CbmPsdv1)
