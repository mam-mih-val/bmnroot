
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

#include "FairVolume.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdv1::CbmPsdv1()  : CbmPsd() {
  f = new TFile("PSD_bmn.root","RECREATE","PSD hists");
  fhist1 = new TH1F("psdhist1","Energy loss in one plate",100,0.,0.01);
  fhist2 = new TH1F("psdhist2","Total energy loss",   1000,0,100);
  fhist3 = new TH1F("psdhist3","Tower number",200,0.,200.);
  fhist4 = new TH1F("psdhist4","Plate number",100,0.,100.);
  fhist5 = new TH2F("psdhist5","Y vs X",125,-50.,200.,60,-60,60 );
  fhist6 = new TH1F("psdhist6","DE conribution vs charge",100,0.,100.);
  fTotal = 0;
  fEloss = 0;
  fXshift = 45.;
  fZposition = 1000.;
  fWithHole = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmPsdv1::CbmPsdv1(const char* name, Bool_t active)
  : CbmPsd(name, active) {
  f = new TFile("PSD_bmn.root","RECREATE","PSD hists");
  fhist1 = new TH1F("psdhist1","Energy loss in one plate",100,0.,0.01);
  fhist2 = new TH1F("psdhist2","Total energy loss",   1000,0,100);
  fhist3 = new TH1F("psdhist3","Tower number",200,0.,200.);
  fhist4 = new TH1F("psdhist4","Plate number",100,0.,100.);
  fhist5 = new TH2F("psdhist5","Y vs X",125,-50.,200.,60,-60,60 );
  fhist6 = new TH1F("psdhist6","DE conribution vs charge",100,0.,100.);
  fTotal = 0;
  fEloss = 0;
  fXshift = 45.;
  fZposition = 1000.;
  fWithHole = 1;
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdv1::~CbmPsdv1() {
 if (f)
 {
  f->cd();
  f->Write();
  f->Close();
  f = NULL;
 }
 if (fPsdCollection) {
   fPsdCollection->Delete(); 
   delete fPsdCollection;
   fPsdCollection = 0;
 }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmPsdv1::ProcessHits(FairVolume* vol)
{
  //  cout<<" CbmPsdv1::ProcessHits in "<<vol->GetName()<<endl;
   if (gMC->TrackCharge() == 0) return kFALSE;
//   if (gMC->GetStack()->GetCurrentTrackNumber()<0) return kFALSE;
//   if (strcmp(gMC->CurrentVolName(),"VSCL") && strcmp(gMC->CurrentVolName(),"USCL")) return kFALSE;
   if (strcmp(vol->GetName(),"VSCL") && strcmp(vol->GetName(),"USCL")) return kFALSE;

  Int_t pdg;
  // Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    ResetParameters();
    fEloss   = 0.;
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fTime    = gMC->TrackTime() * 1.0e09;
    fLength  = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    if (fhist5) fhist5->Fill(fPos.X(),fPos.Y());
    gMC->TrackMomentum(fMom);
    Int_t copyNo;
    gMC->CurrentVolID(copyNo);
    //fVolumeID=copyNo;
    Int_t iCell, iNumm ;
    gMC->CurrentVolOffID(1, iCell);
    fModuleID=iCell-1;
    gMC->CurrentVolOffID(2, iNumm);
//    fVolumeID=iNumm;
    if (copyNo==66) fVolumeID=65;
    else            fVolumeID=copyNo%66-1;
    pdg = gMC->TrackPid();
    if (fPos.X()<12. && 0)
    {
	Info("ProcessHits side track ","track %d pdg %d volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), time %f fLength %f dE=%.3e",
	     fTrackID, pdg, fVolumeID, gMC->CurrentVolName(),
	     fPos.X(),fPos.Y(),fPos.Z(),fMom.Px(),fMom.Py(),fMom.Pz(),fTime, fLength, fEloss);
    }
//    if (!strcmp(gMC->CurrentVolName(),"USCL"))
    if (!strcmp(vol->GetName(),"USCL"))
    {
	fModuleID += 68;
    }
//    Info("ProcessHits track inside","track %d pdg %d volumeid %d moduleid %d copyno %d volname %s",
//    fTrackID, pdg, fVolumeID, fModuleID, copyNo%66, gMC->CurrentVolName());
     f->cd();
     fhist3->Fill(fModuleID);
     fhist4->Fill(fVolumeID);
  }
  if (gMC->IsTrackInside() ) {
    // Sum energy loss for all steps in the active volume
    fEloss += gMC->Edep();
    fTotal += gMC->Edep();      
    if( gMC->IsTrackStop() ||
	gMC->IsTrackDisappeared()   ) {
//      fEloss += gMC->Edep();
//      fTotal += gMC->Edep();      
     if (pdg == 1000791970)
     // if (strstr(fDebug,"hit"))
	Info("ProcessHits track inside","track %d pdg %d volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), time %f fLength %f dE=%.3e",
	     fTrackID, pdg, fVolumeID, gMC->CurrentVolName(),
	     fPos.X(),fPos.Y(),fPos.Z(),fMom.Px(),fMom.Py(),fMom.Pz(),fTime, fLength, fEloss);
      
      // Create CbmPsdPoint
      CbmPsdPoint *fPoint =  AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
				    TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),fTime, fLength,fEloss);
//      cout << " ****************** DE for track stop " << fTrackID << " " << fEloss << " " << fVolumeID << " " << fModuleID << endl;
      fhist6->Fill(gMC->TrackCharge(),fEloss);
      fEloss = 0;
      fPoint->SetModuleID(fModuleID);
      fPoint->SetTrackID(fTrackID);
      ((CbmStack*)gMC->GetStack())->AddPoint(kZDC, fTrackID);
      return kTRUE;
    }
  }
  
  if ( gMC->IsTrackExiting() ) { //   ||
//       gMC->IsTrackStop()       ||
//       gMC->IsTrackDisappeared()  ) {
     // Sum energy loss for all steps in the active volume
    fEloss += gMC->Edep();
    fTotal += gMC->Edep();

    if (fhist1)
    {
	f->cd();
	fhist1->Fill(fEloss);
    }
   
     if (pdg == 1000791970)
      //    if (strstr(fDebug,"hit"))
      Info("ProcessHits","track %d pdg %d volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), time %f fLength %f dE=%.3e",
	   fTrackID, pdg, fVolumeID, gMC->CurrentVolName(),
	   fPos.X(),fPos.Y(),fPos.Z(),fMom.Px(),fMom.Py(),fMom.Pz(),fTime, fLength, fEloss);
    
    // Create CbmPsdPoint
    CbmPsdPoint *fPoint =  AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
				  TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),fTime, fLength,fEloss);
//    cout << " ****************** DE for track exit " << fTrackID << " " << fEloss << " " << fVolumeID << " " << fModuleID<< endl;
    fhist6->Fill(gMC->TrackCharge(),fEloss);
    fEloss = 0;
    fPoint->SetModuleID(fModuleID);
    fPoint->SetTrackID(fTrackID);
    ((CbmStack*)gMC->GetStack())->AddPoint(kZDC, fTrackID);
  }
  
 
//  cout << " No of PSD points (size)  " << (*fPsdCollection).GetEntries() << endl;
//  cout << " No of PSD points (index) " << fPosIndex << endl;
  //  ResetParameters();
    
  return kTRUE;
}
// -------------------------------------------------------------------------
// -----   Public method EndOfEvent   --------------------------------------
void CbmPsdv1::EndOfEvent() {
//  cout << " No of PSD points (size)  " << (*fPsdCollection).GetEntries() << endl;
//  cout << " No of PSD points (index) " << fPosIndex << endl;
//  if (fVerboseLevel) Print();
//  fPsdCollection->Clear();
//  fPosIndex = 0;
  CbmPsd::EndOfEvent();
  fhist2->Fill(fTotal);
//  cout << " ****************** DE in event " << fTotal << endl;
  fTotal = 0;
  fEloss = 0;
}
// -----   Public method FinishRun   --------------------------------------
void CbmPsdv1::FinishRun() {
 if (f)
 {
  f->cd();
  f->Write();
  f->Close();
  f = NULL;
 }
}
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
  
  TGeoVolume *VETO = gGeoManager->MakeBox("VETO",med1,83.5,53.5,52.5);
  
  //Large modules
  TGeoVolume *VMDL = gGeoManager->MakeBox("VMDL",med9,7.5,7.5,52.0);
  TGeoVolume *VFEL = gGeoManager->MakeBox("VFEL",med9,7.3,7.3,1.0);
  TGeoVolume *VPBL = gGeoManager->MakeBox("VPBL",med24,7.3,7.3,0.5);
  TGeoVolume *VSCL = gGeoManager->MakeBox("VSCL",med11,7.3,7.3,0.25);
  TGeoVolume *VRFL = gGeoManager->MakeBox("VRFL",med27,0.2,7.3,1.0);
  TGeoVolume *VRPL = gGeoManager->MakeBox("VRPL",med27,0.2,7.3,0.5);
  TGeoVolume *VRSL = gGeoManager->MakeBox("VRSL",med27,0.2,7.3,0.25);

  //Small modules
  TGeoVolume *UMDL = gGeoManager->MakeBox("UMDL",med9,3.75,3.75,52.0);
  TGeoVolume *USCL = gGeoManager->MakeBox("USCL",med11,3.65,3.65,0.25);
  TGeoVolume *URSL = gGeoManager->MakeBox("URSL",med27,0.1,3.65,0.25);
  TGeoVolume *UPBL = gGeoManager->MakeBox("UPBL",med24,3.65,3.65,0.5);
  TGeoVolume *UFEL = gGeoManager->MakeBox("UFEL",med9,3.65,3.65,1.0);
  TGeoVolume *URFL = gGeoManager->MakeBox("URFL",med27,0.1,3.65,1.0);
 
  
  Float_t xPSD = fXshift;
  Float_t zPSD = fZposition + 52.5;
  gGeoManager->Node("VETO",1,"cave", xPSD, 0, zPSD, 0, kTRUE, buf, 0);
  gGeoManager->Node("VRFL",1,"VFEL", 7.2, 0.00,   0.0, 0,kTRUE, buf, 0);
  gGeoManager->Node("URFL",1,"UFEL", 3.65, 0.00,   0.0, 0,kTRUE, buf, 0);
  gGeoManager->Node("VFEL",1,"VMDL", 0.0, 0.00, -50.5, 0, kTRUE, buf, 0 );
  gGeoManager->Node("VFEL",2,"VMDL", 0, 0, +50.5, 0, kTRUE, buf, 0 );
  gGeoManager->Node("UFEL",1,"UMDL", 0.0, 0.00, -50.5, 0, kTRUE, buf, 0 );
  gGeoManager->Node("UFEL",2,"UMDL", 0, 0, +50.5, 0, kTRUE, buf, 0 );

  gGeoManager->Node("VSCL", 1, "VMDL", 0.0, 0.0, -49.25,0,kTRUE, buf, 0);
  AddSensitiveVolume(VSCL);
  fNbOfSensitiveVol++;
  gGeoManager->Node("VRSL", 1, "VSCL", 7.2, 0.0, 0.0,   0,kTRUE, buf, 0); 
  gGeoManager->Node("VPBL", 1, "VMDL", 0.0, 0.0, -48.5, 0,kTRUE, buf, 0); 
  gGeoManager->Node("VRPL", 1, "VPBL", 7.2, 0.0, 0,     0,kTRUE, buf, 0);
  gGeoManager->Node("USCL", 1, "UMDL", 0.0, 0.0, -49.25,0,kTRUE, buf, 0);
  AddSensitiveVolume(USCL);
  fNbOfSensitiveVol++;
  gGeoManager->Node("URSL", 1, "USCL", +3.55, 0.0, 0.0, 0,kTRUE, buf, 0); 
  gGeoManager->Node("UPBL", 1, "UMDL", 0.0, 0.0, -48.5, 0,kTRUE, buf, 0); 
  Int_t ivol = 1;
  for (Int_t iv=2; iv<67; iv++)
    {
	Float_t zvtyl = -49.25 + 1.5*(iv-1); 
	Float_t zvpbl = -48.50 + 1.5*(iv-1); 
	ivol++;
	gGeoManager->Node("VSCL",ivol, "VMDL", 0, 0, zvtyl , 0,kTRUE, buf, 0);
	gGeoManager->Node("VPBL",ivol, "VMDL", 0, 0, zvpbl , 0, kTRUE, buf, 0); 
	fNbOfSensitiveVol++;
	gGeoManager->Node("USCL", ivol, "UMDL", 0, 0, zvtyl, 0, kTRUE, buf, 0);
	gGeoManager->Node("UPBL", ivol, "UMDL", 0, 0, zvpbl, 0, kTRUE, buf, 0); 
	fNbOfSensitiveVol++;
    }
  //XY positions written to the file to be read in reconstruction
  Float_t xi[104], yi[104];
  Float_t xCur=-75., yCur=-45.;
  Int_t iMod=0, iModNoHole=0, iMod1=0;
  xPSD = 0.;
  for(Int_t iy=0; iy<7; iy++) {
    for(Int_t ix=0; ix<11; ix++) {
      iModNoHole++;
      if(ix>=4 && ix<=6 && iy>=2 && iy<=4)
	{
    	  iMod1++;
	  if ((!(ix == 6 && iy == 3)) || (fWithHole == 0)) gGeoManager->Node("UMDL", iMod1, "VETO", xCur + xPSD - 3.75,yCur - 3.75,0, 0, kTRUE, buf, 0); 
//	  cout <<"SMALL MODULE :::::iMod,xxxx,yyyy " <<iMod1 <<" " <<xCur - 3.75 <<" " <<yCur - 3.75 <<endl;
	  xi[iMod1+67] = xCur + fXshift - 3.75;
	  yi[iMod1+67] = yCur - 3.75;
    	  iMod1++;
	  if ((!(ix == 5 && iy == 3)) || (fWithHole == 0)) gGeoManager->Node("UMDL", iMod1, "VETO", xCur + xPSD + 3.75,yCur - 3.75,0, 0, kTRUE, buf, 0); 
//	  cout <<"SMALL MODULE :::::iMod,xxxx,yyyy " <<iMod1 <<" " <<xCur + 3.75 <<" " <<yCur - 3.75 <<endl;
	  xi[iMod1+67] = xCur + fXshift + 3.75;
	  yi[iMod1+67] = yCur - 3.75;
    	  iMod1++;
	  if ((!(ix == 6 && iy == 3)) || (fWithHole == 0)) gGeoManager->Node("UMDL", iMod1, "VETO", xCur + xPSD - 3.75,yCur + 3.75,0, 0, kTRUE, buf, 0); 
//	  cout <<"SMALL MODULE :::::iMod,xxxx,yyyy " <<iMod1 <<" " <<xCur - 3.75 <<" " <<yCur + 3.75 <<endl;
	  xi[iMod1+67] = xCur + fXshift - 3.75;
	  yi[iMod1+67] = yCur + 3.75;
    	  iMod1++;
	  if ((!(ix == 5 && iy == 3)) || (fWithHole == 0)) gGeoManager->Node("UMDL", iMod1, "VETO", xCur + xPSD + 3.75,yCur + 3.75,0, 0, kTRUE, buf, 0); 
//	  cout <<"SMALL MODULE :::::iMod,xxxx,yyyy " <<iMod1 <<" " <<xCur + 3.75 <<" " <<yCur + 3.75 <<endl;
	  xi[iMod1+67] = xCur + fXshift + 3.75;
	  yi[iMod1+67] = yCur + 3.75;
	}
      else
	{
    	  iMod++;
	  gGeoManager->Node("VMDL", iMod, "VETO", xCur+xPSD,yCur,0, 0, kTRUE, buf, 0); 
//	  cout <<"BIG MODULE :::::iMod,xxxx,yyyy " <<iMod <<" " <<xCur <<" " <<yCur <<endl;
	  xi[iMod-1] = xCur + fXshift;
	  yi[iMod-1] = yCur;
	}
	xCur=xCur + 15.;      
    }//for(Int_t ix==0; ix<9; ix++)
    xCur=-75.;
    yCur=yCur + 15.;
  }//for(Int_t iy==0; iy<7; iy++)

  ofstream fxypos("psd_geo_xy.txt");
  for (Int_t ii=0; ii<104; ii++) {
    fxypos<<xi[ii]<<" "<<yi[ii]<<endl;
//    cout<<xi[ii]<<" "<<yi[ii]<<endl;
  }
  fxypos.close();
  

}
// -------------------------------------------------------------------------

ClassImp(CbmPsdv1)
