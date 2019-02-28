//------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <assert.h>
#include <sstream>
#include <cmath>
#include <limits>

#include "TROOT.h"
//#include "TEveTrackPropagator.h"
//#include "TDatabasePDG.h"
//#include "BmnTrackDraw.h"
#include "BmnTrack.h"
#include "CbmStack.h"
#include <TClonesArray.h>
//#include <TGeoManager.h>
//#include <TGeoVolume.h>
//#include <TGeoShape.h>
//#include <TGeoBBox.h>
//#include <TGeoNode.h>

//#include <TGeoTranslation.h>
//#include <TGeoPgon.h>
//#include <TGeoTube.h>
//#include <TGeoCompositeShape.h>
//#include <FairRunSim.h>
#include <FairModule.h>
#include <FairCave.h>
//#include <FairBoxGenerator.h>
//#include <FairPrimaryGenerator.h>


//#include <TGeoMatrix.h>
#include <TApplication.h>
#include <TMath.h>
#include <TRandom.h>
#include <TRandom2.h>
#include <TVector3.h>
#include <TVector2.h>
#include <TDecompLU.h>
#include <TKey.h>
//#include "TRandom.h"
#include <TRandom2.h>
#include <TSpectrum.h>
#include <TObjectTable.h>
#include "TEveElement.h"
#include "TEveManager.h"
#include "TDatabasePDG.h"
//#include "TGeant3.h"                    // for TGeant3
#include "TGeant3TGeo.h"
#include "TVirtualMC.h"                 // for gMC3

#include "BmnDch.h"
#include "BmnDchHit.h"
#include "BmnDchPoint.h"
#include "FairRun.h"
/*#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairGeoMedia.h"
#include "FairGeoBuilder.h"
#include "FairDetector.h"
#include "FairMCPoint.h"*/
#include "CbmMCTrack.h"
#include "CbmStack.h"
#include "BmnDchHitProducer_exp.h"
#include "BmnDchDigit.h"
//#include "FairGeane.h"
//#include "FairGeanePro.h"

//#include "FairMCStack.h"
//#include "FairRunAna.h"

//#include "FairEventManager.h"
//#include "FairEventManagerEditor.h"
//#include "FairMCTracksEditor.h"
//#include "RawMWPCDigitDraw.h"
#include "UniDbRun.h"

#include "TVectorD.h"
//#include "TMatrixD.h"

#include "Math/Point3D.h"
#include "Math/Point2D.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "TDecompQRH.h"

#include "Math/GenVector/Rotation3D.h"
#include "Math/GenVector/EulerAngles.h"
//#include "Math/GenVector/AxisAngle.h"
//#include "Math/GenVector/Quaternion.h"
//#include "Math/GenVector/RotationX.h"
//#include "Math/GenVector/RotationY.h"
//#include "Math/GenVector/RotationZ.h"
//#include "Math/GenVector/RotationZYX.h"

#include "Math/GenVector/Transform3D.h"
#include "Math/GenVector/Plane3D.h"
#include "Math/GenVector/VectorUtil.h"

//#include <TObjectTable.h>

//#include "testfunction.h"

using namespace ROOT::Math;

// constants definition
        const Double_t BmnDchHitProducer_exp::cosPhi_45 = TMath::Cos(-45.*TMath::DegToRad()); 
        const Double_t BmnDchHitProducer_exp::sinPhi_45 = TMath::Sin(-45.*TMath::DegToRad());
        const Double_t BmnDchHitProducer_exp::cosPhi45 = TMath::Cos(45.*TMath::DegToRad()); 
        const Double_t BmnDchHitProducer_exp::sinPhi45 = TMath::Sin(45.*TMath::DegToRad());
	const Double_t BmnDchHitProducer_exp::Phi_45 = -45.*TMath::DegToRad(); 
        const Double_t BmnDchHitProducer_exp::Phi45 = 45.*TMath::DegToRad(); 
        const Double_t BmnDchHitProducer_exp::Phi90 = 90.*TMath::DegToRad();
	const Double_t BmnDchHitProducer_exp::sqrt2=sqrt(2.); // 
        const Double_t BmnDchHitProducer_exp::tg3=tan(angleLayerRad[4]);
        const Double_t BmnDchHitProducer_exp::tg4=tan(angleLayerRad[6]);
	const Double_t BmnDchHitProducer_exp::resolution =  15e-3; // 150 mkm  ==  0.015 cm
	const Double_t BmnDchHitProducer_exp::hitErr[3] = {resolution,resolution,0.};
	const Double_t BmnDchHitProducer_exp::WheelR_2 = pow(MaxRadiusOfActiveVolume,2.); // cm x cm
        //const Float_t BmnDchHitProducer_exp::hmintime=200.;
        //const Float_t BmnDchHitProducer_exp::hmaxtime=1200.;
        //const Int_t BmnDchHitProducer_exp::hnbins=500;
        const Float_t BmnDchHitProducer_exp::hmintime[numRunPeriods]={200.,200.,200.,0.,0.,-400.,0.};
        const Float_t BmnDchHitProducer_exp::hmaxtime[numRunPeriods]={1200.,1200.,1200.,0.,0.,1200.,0.};
        const Int_t BmnDchHitProducer_exp::hnbins[numRunPeriods]={500,500,500,0,0,800,0};
        const UInt_t BmnDchHitProducer_exp::nintervals=40;
        const TVector2 BmnDchHitProducer_exp::unitU(0.,1.);
        const TVector2 BmnDchHitProducer_exp::unitV(0.,-1.);
        //const Float_t BmnDchHitProducer_exp::Dmomentum=sqrt(pow(Dmass+2.*DT0,2.)-pow(Dmass,2.));
        //const Double_t BmnDchHitProducer_exp::Dmomentum=sqrt(pow(1.875612928+2.*3.5,2.)-pow(1.875612928,2.));
        //const Int_t BmnDchHitProducer_exp::GeantBeamCode=45;

//------------------------------------------------------------------------------------------------------------------------
struct __ltstr
{
	bool operator()(Double_t s1, Double_t s2) const
  	{
    		return s1 > s2;
  	}
};

//------------------------------------------------------------------------------------------------------------------------
BmnDchHitProducer_exp::BmnDchHitProducer_exp(Bool_t checkDCH, Bool_t check, UShort_t runperiod, UShort_t runnumber, UShort_t iter, UShort_t filenumber) : checkDch(checkDCH),fDoCheck(check)
{
	pRandom = new TRandom2;
        //fDchNum = num;
        //checkDch=false;
        runPeriod = runperiod;
        runNumber = runnumber;
        fileNumber = filenumber;
        Iter=iter;
        cout<<"Iter = "<<iter<<endl;
        //Iter=0;
}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHitProducer_exp::~BmnDchHitProducer_exp() 
{
	delete pRandom;	
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::MagFieldInit(){
   
  if(Iter==0&&fDoCheck){
     grBy = new TGraph2D();
     grBx = new TGraph2D();
     grBy->SetNameTitle("grBy","By as function of x,z");
     grBx->SetNameTitle("grBx","Bx as function of y,z");
     grBy->SetName("Byvsxz");
     grBx->SetName("Bxvsyz");
     grBy->SetDirectory(0); hList.Add(grBy);
     grBx->SetDirectory(0); hList.Add(grBx);
  }
 
  //if(Iter==0){
  //magField = new BmnNewFieldMap("field_sp41v3_ascii_Extrap.dat");
  //magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.dat");
  magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap_xzswapped.dat");
  // magField->SetScale(fieldScale);
  //magField = new BmnFieldConst();
  //magField->SetFieldRegion(-300., 300., -300., 300., -300., 300);
  //magField->SetField(0., 9., 0.);  
  //magFieldScale();
  magField->Init();
  //magField->Print();

  if(fDoCheck&&Iter==0){
   Float_t zval; 
   /*for (UShort_t j = 0; j <= 100; j++) {
    zval=-500.+Float_t(j)*10.;
    //cout<<"z = "<<zval<<endl;
    cout<<"bx = "<<magField->GetBx(0.,0.,zval)<<", by = "<<magField->GetBy(0.,0.,zval)<<", bz = "<<magField->GetBz(0.,0.,zval)<<", z = "<<zval<<endl;
    //cout<<"bx = "<<magField->GetBx(5.,0.,zval)<<", by = "<<magField->GetBy(5.,0.,zval)<<", bz = "<<magField->GetBz(5.,0.,zval)<<", z = "<<zval<<endl;
    //cout<<"bx = "<<magField->GetBx(-5.,0.,zval)<<", by = "<<magField->GetBy(-5.,0.,zval)<<", bz = "<<magField->GetBz(-5.,0.,zval)<<", z = "<<zval<<endl;
  } */ 
   cout<<"-----------------------------------------------------------------"<<endl; 
   Float_t xval; 
   /*for (UShort_t j = 0; j <= 150; j++) {
    xval=-500.+Float_t(j)*10.;
    cout<<"bx = "<<magField->GetBx(xval,0.,0.)<<", by = "<<magField->GetBy(xval,0.,0.)<<", bz = "<<magField->GetBz(xval,0.,0.)<<", x = "<<xval<<endl;
   }*/
   cout<<"-----------------------------------------------------------------"<<endl; 
   Float_t yval; 
   /*for (UShort_t j = 0; j <= 100; j++) {
    yval=-500.+Float_t(j)*10.;
    cout<<"bx = "<<magField->GetBx(0.,yval,0.)<<", by = "<<magField->GetBy(0.,yval,0.)<<", bz = "<<magField->GetBz(0.,yval,0.)<<", y = "<<yval<<endl;
   }*/
   Int_t ngr=0,ngr2=0; 
   for (UShort_t j = 0; j <= 150; j++) {
    zval=-250.+Float_t(j)*5.;
    for (UShort_t i = 0; i <= 60; i++) {
     xval=-150.+Float_t(i)*5.;
     //cout<<xval<<" "<<zval<<" "<<magField->GetBy(xval,0.,zval)<<endl;
     grBy->SetPoint(ngr,zval,xval,magField->GetBy(zval,0.,xval));
     ngr++;
    }
    for (UShort_t i = 0; i <= 120; i++) {
     yval=-60.+Float_t(i)*1.;
     grBx->SetPoint(ngr2,zval,yval,magField->GetBx(zval,yval,0.));
     ngr2++;
    }
    //ngr=0;
    /*for (UShort_t i = 0; i <= 50; i++) {
     yval=-250.+Float_t(i)*10.;
     //cout<<xval<<" "<<yval<<" "<<magField->GetBx(xval,yval,0.)<<endl;
     grBx->SetPoint(ngr,zval,yval,magField->GetBx(zval,yval,0.));
     ngr++;
    }*/
   }
  }//fDoCheck
  //}//if Iter

}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::InitDchParameters(){
  //checkDch=kFALSE;
  //checkDch=kTRUE;
  //checkDchHist=kFALSE;
  checkDchHist=kTRUE;
  if(!fDoCheck)checkDchHist=kFALSE;
  checkGraphs=kFALSE;
  //checkGraphs=kTRUE;
  //checkMatrices=kTRUE;
  checkMatrices=kFALSE;
  if(!fDoCheck)checkGraphs=kFALSE;
  //fAngleCorrectionFill=kFALSE;
  //fAngleCorrectionFill=kTRUE;
  //calibMethod=1;// maximum interval
  calibMethod=2; //inflex points
  eventNum1track=0; 
  sumDeltaPhi=0.; 
  trCand=0; 
  if(Iter==0){
   C.ResizeTo(4,4);
  }
  //Double_t locAngleX,locAngleY;
  //UShort_t itertmp;

  //C0.ResizeTo(4,4);
  //D0.ResizeTo(4,4);
  /*ifstream myfile;
  myfile.open("DCHpar");
  myfile >> locAngleX;
  myfile >> locAngleY;
  myfile >> itertmp;
  myfile >> ranmin[0];
  myfile >> ranmax[0];
  myfile >> ranmin[1];
  myfile >> ranmax[1];
  for (UShort_t j = 0; j < numChambers; j++) {
   cout<<"RANMIN = "<<ranmin[j]<<endl;
   cout<<"RANMAX = "<<ranmax[j]<<endl;
  }
  for (UShort_t j = 0; j < 4; j++) {
    for (UShort_t k = 0; k < 4; k++) {
        myfile >> C0(j,k);
    }
  }
  myfile.close();*/
  if(Iter==0){
   XYZshift1.SetX(0.);
   XYZshift1.SetY(0.);
   XYZshift1.SetZ(0.);
   XYZshift2.SetX(0.);
   XYZshift2.SetY(0.);
   XYZshift2.SetZ(0.);
  //if Iter==0
  //if(fDchNum==1){dchtrackcands=new TFile("dchtrackcands1.root", "RECREATE");}
  //else if(fDchNum==2){dchtrackcands=new TFile("dchtrackcands2.root", "RECREATE");}
 
  detXshift[0]=DCH1_Xpos[runPeriod-1];
  detXshift[1]=DCH2_Xpos[runPeriod-1];
  // DCH1 and DCH2 x shifts, cm
  detYshift[0]=DCH1_Ypos[runPeriod-1];
  detYshift[1]=DCH2_Ypos[runPeriod-1];
  // DCH1 and DCH2 y shifts, cm
  }
  detZshift[0]=DCH1_Zpos[runPeriod-1];
  detZshift[1]=DCH2_Zpos[runPeriod-1];
  // DCH1 and DCH2 z shifts, cm

  //XYZ1(detXshift[0],detYshift[0],detZshift[0]);
  //XYZ2(detXshift[1],detYshift[1],detZshift[1]);
  XYZ1.SetX(detXshift[0]);
  XYZ1.SetY(detYshift[0]);
  XYZ1.SetZ(detZshift[0]);
  XYZ2.SetX(detXshift[1]);
  XYZ2.SetY(detYshift[1]);
  XYZ2.SetZ(detZshift[1]);
  XYZ0beam.SetX(beamPos[0]);
  XYZ0beam.SetY(beamPos[1]);
  XYZ0beam.SetZ(beamPos[2]);
  XYZ0.SetX(0.);
  XYZ0.SetY(0.);
  XYZ0.SetZ(0.);

 
    //if(fDchNum==1){detXshift[0]=DCH1_Xpos[runPeriod-1];}
    //else if(fDchNum==2){detXshift[1]=DCH2_Xpos[runPeriod-1];}
    // DCH1 and DCH2 x shifts, cm
 
  for (UShort_t i = 0; i < numLayers; i++) {
    DCH1_ZlayerPos_global[i]=DCH_ZlayerPos_local[i]+DCH1_Zpos[runPeriod-1];
    DCH2_ZlayerPos_global[i]=DCH_ZlayerPos_local[i]+DCH2_Zpos[runPeriod-1];
  }

 
  /*if(fDchNum==1){
        z3121=(DCH1_ZlayerPos_global[4]-DCH1_ZlayerPos_global[0])/(DCH1_ZlayerPos_global[2]-DCH1_ZlayerPos_global[0]);
        z4121=(DCH1_ZlayerPos_global[6]-DCH1_ZlayerPos_global[0])/(DCH1_ZlayerPos_global[2]-DCH1_ZlayerPos_global[0]);
        //z4131=(DCH1_ZlayerPos_global[6]-DCH1_ZlayerPos_global[0])/(DCH1_ZlayerPos_global[4]-DCH1_ZlayerPos_global[0]);
      }else if(fDchNum==2){
        z3121=(DCH2_ZlayerPos_global[4]-DCH2_ZlayerPos_global[0])/(DCH2_ZlayerPos_global[2]-DCH2_ZlayerPos_global[0]);
        z4121=(DCH2_ZlayerPos_global[6]-DCH2_ZlayerPos_global[0])/(DCH2_ZlayerPos_global[2]-DCH2_ZlayerPos_global[0]);
        //z4131=(DCH2_ZlayerPos_global[6]-DCH2_ZlayerPos_global[0])/(DCH2_ZlayerPos_global[4]-DCH2_ZlayerPos_global[0]);
  }*/

  
  for (UShort_t i = 2; i < 4; i++) {
   for (UShort_t j = 0; j < 2; j++) {
    zDif21[GetGasGap(i)][GetGasGap(j)]=DCH_ZlayerPos_local[i]-DCH_ZlayerPos_local[j];
   if(checkDch)cout<<"zDif21: "<<zDif21[GetGasGap(i)][GetGasGap(j)]<<" "<<GetGasGap(i)<<" "<<i<<" "<<GetGasGap(j)<<" "<<j<<endl;
   }
  }
  for (UShort_t i = 4; i < 6; i++) {
   for (UShort_t j = 0; j < 2; j++) {
    zDif31[GetGasGap(i)][GetGasGap(j)]=DCH_ZlayerPos_local[i]-DCH_ZlayerPos_local[j];
   if(checkDch)cout<<"zDif31: "<<zDif31[GetGasGap(i)][GetGasGap(j)]<<" "<<GetGasGap(i)<<" "<<i<<" "<<GetGasGap(j)<<" "<<j<<endl;
   }
  }
  for (UShort_t i = 6; i < 8; i++) {
   for (UShort_t j = 0; j < 2; j++) {
    zDif41[GetGasGap(i)][GetGasGap(j)]=DCH_ZlayerPos_local[i]-DCH_ZlayerPos_local[j];
   if(checkDch)cout<<"zDif41: "<<zDif41[GetGasGap(i)][GetGasGap(j)]<<" "<<GetGasGap(i)<<" "<<i<<" "<<GetGasGap(j)<<" "<<j<<endl;
   }
  }
  for (UShort_t i = 4; i < 6; i++) {
   for (UShort_t j = 2; j < 4; j++) {
    zDif32[GetGasGap(i)][GetGasGap(j)]=DCH_ZlayerPos_local[i]-DCH_ZlayerPos_local[j];
   if(checkDch)cout<<"zDif32: "<<zDif32[GetGasGap(i)][GetGasGap(j)]<<" "<<GetGasGap(i)<<" "<<i<<" "<<GetGasGap(j)<<" "<<j<<endl;
   }
  }
  for (UShort_t i = 6; i < 8; i++) {
   for (UShort_t j = 2; j < 4; j++) {
    zDif42[GetGasGap(i)][GetGasGap(j)]=DCH_ZlayerPos_local[i]-DCH_ZlayerPos_local[j];
   if(checkDch)cout<<"zDif42: "<<zDif42[GetGasGap(i)][GetGasGap(j)]<<" "<<GetGasGap(i)<<" "<<i<<" "<<GetGasGap(j)<<" "<<j<<endl;
   }
  }
  for (UShort_t i = 6; i < 8; i++) {
   for (UShort_t j = 4; j < 6; j++) {
    zDif43[GetGasGap(i)][GetGasGap(j)]=DCH_ZlayerPos_local[i]-DCH_ZlayerPos_local[j];
   if(checkDch)cout<<"zDif43: "<<zDif43[GetGasGap(i)][GetGasGap(j)]<<" "<<GetGasGap(i)<<" "<<i<<" "<<GetGasGap(j)<<" "<<j<<endl;
   }
  }
  for (UShort_t k = 2; k < 4; k++) {
   for (UShort_t l = 0; l < 2; l++) {
    zMult12[GetGasGap(k)][GetGasGap(l)]=2.*DCH_ZlayerPos_local[k]*DCH_ZlayerPos_local[l];
    if(checkDch)cout<<"zMult12: "<<zMult12[GetGasGap(k)][GetGasGap(l)]<<" "<<GetGasGap(k)<<" "<<k<<" "<<GetGasGap(l)<<" "<<l<<endl;
   }
  }
  for (UShort_t i = 6; i < 8; i++) {
   for (UShort_t j = 4; j < 6; j++) {
    zMult34[GetGasGap(i)][GetGasGap(j)]=2.*DCH_ZlayerPos_local[i]*DCH_ZlayerPos_local[j];
    if(checkDch)cout<<"zMult34: "<<zMult34[GetGasGap(i)][GetGasGap(j)]<<" "<<GetGasGap(i)<<" "<<i<<" "<<GetGasGap(j)<<" "<<j<<endl;
   }
  }
  for (UShort_t i = 6; i < 8; i++) {
   for (UShort_t j = 4; j < 6; j++) {
    for (UShort_t k = 2; k < 4; k++) {
     for (UShort_t l = 0; l < 2; l++) {
      zMult1234[GetGasGap(i)][GetGasGap(j)][GetGasGap(k)][GetGasGap(l)]=(DCH_ZlayerPos_local[i]+DCH_ZlayerPos_local[j])*(DCH_ZlayerPos_local[k]+DCH_ZlayerPos_local[l]);
      if(checkDch)cout<<"zMult1234: "<<zMult1234[GetGasGap(i)][GetGasGap(j)][GetGasGap(k)][GetGasGap(l)]<<" "<<GetGasGap(i)<<" "<<i<<" "<<GetGasGap(j)<<" "<<j<<" "<<GetGasGap(k)<<" "<<k<<" "<<GetGasGap(l)<<" "<<l<<endl;
     }
    }
   }
  }



   /*cout<<"1zDif21: "<<zDif21[0][0]<<endl; 
   cout<<"1zDif21: "<<zDif21[1][0]<<endl; 
   cout<<"1zDif21: "<<zDif21[0][1]<<endl; 
   cout<<"1zDif21: "<<zDif21[1][1]<<endl;*/ 
  /*
  //for (UShort_t i = 0; i < 2; i++) {
  //for (UShort_t i = 2; i < 4; i++) {
  for (UShort_t i = 0; i < 4; i++) {
   //for (UShort_t j = 2; j < 4; j++) {
   //for (UShort_t j = 0; j < 2; j++) {
   for (UShort_t j = 0; j < 4; j++) {
    for (UShort_t k = 4; k < 6; k++) {
       zCoeff1[i][GetGasGap(j)][GetGasGap(k)]=(DCH_ZlayerPos_local[k]-DCH_ZlayerPos_local[i])/(DCH_ZlayerPos_local[j]-DCH_ZlayerPos_local[i]);
       if(checkDch)cout<<"zCoeff1: "<<zCoeff1[i][GetGasGap(j)][GetGasGap(k)]<<" "<<i<<" "<<GetGasGap(j)<<" "<<GetGasGap(k)<<endl;
    }
    for (UShort_t l = 6; l < numLayers; l++) {
       zCoeff2[i][GetGasGap(j)][GetGasGap(l)]=(DCH_ZlayerPos_local[l]-DCH_ZlayerPos_local[i])/(DCH_ZlayerPos_local[j]-DCH_ZlayerPos_local[i]);
       if(checkDch)cout<<"zCoeff2: "<<zCoeff2[i][GetGasGap(j)][GetGasGap(l)]<<" "<<i<<" "<<GetGasGap(j)<<" "<<GetGasGap(l)<<endl;
    }
   }
  }*/
   /*cout<<"2zDif21: "<<zDif21[0][0]<<endl; 
   cout<<"2zDif21: "<<zDif21[1][0]<<endl; 
   cout<<"2zDif21: "<<zDif21[0][1]<<endl; 
   cout<<"2zDif21: "<<zDif21[1][1]<<endl;*/ 

  for (UInt_t i = 0; i < numChambers; i++) {
   zCenter[i] = (i==0) ? DCH1_Zpos[runPeriod-1]:DCH2_Zpos[runPeriod-1];
   for (UShort_t k = 0; k < numLayers; k++) {
      if(i==0){
        zLayer[i][k]=DCH1_ZlayerPos_global[k]; 
        zLayerExtrap[i][k]=DCH2_ZlayerPos_global[k];
        /*if(k==0||k==1){
         zLayer[i][k]=DCH1_ZlayerPos_global[k+2];
         zLayerExtrap[i][k]=DCH2_ZlayerPos_global[k+2];
        }else if(k==2||k==3){
         zLayer[i][k]=DCH1_ZlayerPos_global[k-2];
         zLayerExtrap[i][k]=DCH2_ZlayerPos_global[k-2];
        }else{
         zLayer[i][k]=DCH1_ZlayerPos_global[k]; 
         zLayerExtrap[i][k]=DCH2_ZlayerPos_global[k];
        }*/
        //zgap[fDchNum-1][k/2]=(DCH1_ZlayerPos_global[k]+DCH1_ZlayerPos_global[k+2])/2.;
      }else if(i==1){
        /*if(k==0||k==1){
         zLayer[i][k]=DCH2_ZlayerPos_global[k+2];
         zLayerExtrap[i][k]=DCH1_ZlayerPos_global[k+2];
        }else if(k==2||k==3){
         zLayer[i][k]=DCH2_ZlayerPos_global[k-2];
         zLayerExtrap[i][k]=DCH1_ZlayerPos_global[k-2];
        }else{
         zLayer[i][k]=DCH2_ZlayerPos_global[k]; 
         zLayerExtrap[i][k]=DCH1_ZlayerPos_global[k];
        }*/
        zLayer[i][k]=DCH2_ZlayerPos_global[k]; 
        zLayerExtrap[i][k]=DCH1_ZlayerPos_global[k];
        //zgap[fDchNum-1][k/2]=(DCH2_ZlayerPos_global[k]+DCH2_ZlayerPos_global[k+2])/2.;
      }
   }
  }

  //if(checkDch){
   cout<<"Z of DCH layer (lab coordinate frame): "<<endl;
   for (UInt_t i = 0; i < numChambers; i++) {
    cout<<"DCH X shift = "<<detXshift[i]<<endl;
    cout<<"DCH Y shift = "<<detYshift[i]<<endl;
    cout<<"DCH = "<<i+1<<endl;
    for (UShort_t k = 0; k < numLayers; k++) {
     if(i==0){
      cout<<DCH1_ZlayerPos_global[k]<<" ";
     }else if(i==1){
      cout<<DCH2_ZlayerPos_global[k]<<" ";
     }
    }
    cout<<endl;
   }
  xyTolerance=3.*resolution; 
  //radialRange=2.; //cm (AuAu)
  //radialRange=3.; //cm (CC)
  for (UShort_t k = 0; k < 3; k++) {
   radialRange[k]=1.; //cm (d beam)
  }
  radialRange[5]=2.; //cm (C beam)

  uOrt=unitU.Rotate(angleLayerRad[4]);
  vOrt=unitV.Rotate(angleLayerRad[6]);

}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::BookAnalysisHistograms(){

             hExtrapMeasDiffX = new TH1D("hExtrapMeasDiffX", "difference of extrapolated and measured track X coordinate in DCH2 (AlignmentMatrix)",100,-20.,20.); 
             hExtrapMeasDiffY = new TH1D("hExtrapMeasDiffY", "difference of extrapolated and measured track Y coordinate in DCH2 (AlignmentMatrix)",100,-20.,20.); 

}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::BookCheckHistograms(){

//adding analysis histograms to hList, first BookAnalysisHistograms must be called
             hExtrapMeasDiffX->SetDirectory(0); hList.Add(hExtrapMeasDiffX);
             hExtrapMeasDiffY->SetDirectory(0); hList.Add(hExtrapMeasDiffY);

             if(Iter==0){
              hDigits = new TH1I("hDigits", "number of hits per event",500,0,500);
	      hDigits->SetDirectory(0); hList.Add(hDigits);
              //TString minDriftTimeName="minDriftTime";
              //minDriftTime = new TH1D(minDriftTimeName,"minimal drift times",900, 300.,1200.); 
              //minDriftTimeAll = new TH1D(minDriftTimeName+TString("0")+TString("1"),"minimal drift times", hnbins, hmintime, hmaxtime); 
	      minDriftTimeAll->SetDirectory(0); hList.Add(minDriftTimeAll);
              //minDriftTime[0] = new TH1D(minDriftTimeName+TString("0"),"minimal drift times", hnbins, hmintime, hmaxtime); 
	      minDriftTime[0]->SetDirectory(0); hList.Add(minDriftTime[0]);
              //minDriftTime[1] = new TH1D(minDriftTimeName+TString("1"),"minimal drift times", hnbins, hmintime, hmaxtime); 
	      minDriftTime[1]->SetDirectory(0); hList.Add(minDriftTime[1]);
	      hDch1WireHitsVsDch2WireHitsPerEvent = new TH2I("hDch1WireHitsVsDch2WireHitsPerEvent", "number of wire hits in DCH2 vs number of wire hits in DCH1 per event",30,0,30,30,0,30); 
	      hDch1WireHitsVsDch2WireHitsPerEvent->SetDirectory(0); hList.Add(hDch1WireHitsVsDch2WireHitsPerEvent);
             }
	     calibList.Add(calib_cham[0]);
	     calibList.Add(calib_cham[1]);
	     hDch1TracksVsDch2TracksPerEvent = new TH2I("hDch1TracksVsDch2TracksPerEvent", "number of tracks in DCH2 vs number of tracks in DCH1 per event",500,0,500,500,0,500); 
	     hDch1TracksVsDch2TracksPerEvent->SetDirectory(0); hList.Add(hDch1TracksVsDch2TracksPerEvent);
	     hExtrapMeasDiffXY = new TH2D("hExtrapMeasDiffXY", "difference of extrapolated and measured track Y vs X coordinate in DCH2 (AlignmentMatrix)",100,-10.,10.,100,-10.,10.); 
	     hExtrapMeasDiffXY->SetDirectory(0); hList.Add(hExtrapMeasDiffXY);


             hAliMisaliX = new TH1D("hAliMisaliX", "difference of aligned and misaligned track X coordinate in DCH2",100,-10.,10.);
             hAliMisaliX->SetDirectory(0); hList.Add(hAliMisaliX);
             hAliMisaliY = new TH1D("hAliMisaliY", "difference of aligned and misaligned track Y coordinate in DCH2",100,-10.,10.);
	     hAliMisaliY->SetDirectory(0); hList.Add(hAliMisaliY);
	     hAliMisaliXYdist = new TH2D("hAliMisaliXYdist", "difference of aligned and misaligned track Y vs X coordinate in DCH2",100,-10.,10.,100,-10.,10.); 
	     hAliMisaliXYdist->SetDirectory(0); hList.Add(hAliMisaliXYdist);
             hAliX = new TH1D("hAliX", "aligned track X coordinate in DCH2",100,-10.,10.);
             hAliX->SetDirectory(0); hList.Add(hAliX);
             hAliY = new TH1D("hAliY", "aligned track Y coordinate in DCH2",100,-10.,10.);
	     hAliY->SetDirectory(0); hList.Add(hAliY);
	     hAliXYdist = new TH2D("hAliXYdist", "aligned track Y vs X coordinate in DCH2",100,-10.,10.,100,-10.,10.); 
	     hAliXYdist->SetDirectory(0); hList.Add(hAliXYdist);
             
             hMisaliX = new TH1D("hMisaliX", "misaligned track X coordinate in DCH2",100,-10.,10.);
             hMisaliX->SetDirectory(0); hList.Add(hMisaliX);
             hMisaliY = new TH1D("hMisaliY", "misaligned track Y coordinate in DCH2",100,-10.,10.);
	     hMisaliY->SetDirectory(0); hList.Add(hMisaliY);
	     hMisaliXYdist = new TH2D("hMisaliXYdist", "misaligned track Y vs X coordinate in DCH2",100,-10.,10.,100,-10.,10.); 
	     hMisaliXYdist->SetDirectory(0); hList.Add(hMisaliXYdist);

             for (UShort_t fDchNum = 0;  fDchNum < numChambers; fDchNum++) {
              TString str;
              str.Form("%u",fDchNum);
              TString str3;
              str3.Form("%u",fDchNum+1);
              if(checkDchHist){
               TString difxuname=TString("difxu")+str;
               TString difuvname=TString("difuv")+str;
               TString difxvname=TString("difxv")+str;
               difxu[fDchNum] = new TH1D(difxuname, "difference of hit x and x component of u coordinates",200,-10.,10.);
               difxu[fDchNum]->SetDirectory(0); hList.Add(difxu[fDchNum]); 
               difuv[fDchNum] = new TH1D(difuvname, "difference of hit x components of u and v coordinates",200,-10.,10.);
               difuv[fDchNum]->SetDirectory(0); hList.Add(difuv[fDchNum]); 
               difxv[fDchNum] = new TH1D(difxvname, "difference of hit x and x component of v coordinates",200,-10.,10.);
               difxv[fDchNum]->SetDirectory(0); hList.Add(difxv[fDchNum]); 
               TString difxuname2=TString("difxu_2")+str;
               TString difuvname2=TString("difuv_2")+str;
               TString difxvname2=TString("difxv_2")+str;
               difxu_2[fDchNum] = new TH1D(difxuname2, "difference of hit x and x component of u coordinates",200,-10.,10.);
               difxu_2[fDchNum]->SetDirectory(0); hList.Add(difxu_2[fDchNum]); 
               difuv_2[fDchNum] = new TH1D(difuvname2, "difference of hit x components of u and v coordinates",200,-10.,10.);
               difuv_2[fDchNum]->SetDirectory(0); hList.Add(difuv_2[fDchNum]); 
               difxv_2[fDchNum] = new TH1D(difxvname2, "difference of hit x and x component of v coordinates",200,-10.,10.);
               difxv_2[fDchNum]->SetDirectory(0); hList.Add(difxv_2[fDchNum]); 
               TString xdtname=TString("xdt")+str;
               TString ydtname=TString("ydt")+str;
               TString udtname=TString("udt")+str;
               TString vdtname=TString("vdt")+str;
               xdt[fDchNum] = new TH1D(xdtname, "drift time in X planes",100,0.,200.);
               xdt[fDchNum]->SetDirectory(0); hList.Add(xdt[fDchNum]); 
               ydt[fDchNum] = new TH1D(ydtname, "drift time in Y planes",100,0.,200.);
               ydt[fDchNum]->SetDirectory(0); hList.Add(ydt[fDchNum]); 
               udt[fDchNum] = new TH1D(udtname, "drift time in U planes",100,0.,200.);
               udt[fDchNum]->SetDirectory(0); hList.Add(udt[fDchNum]); 
               vdt[fDchNum] = new TH1D(vdtname, "drift time in V planes",100,0.,200.);
               vdt[fDchNum]->SetDirectory(0); hList.Add(vdt[fDchNum]); 
               TString xdtcorrelname=TString("xdtcorrel")+str;
               TString ydtcorrelname=TString("ydtcorrel")+str;
               TString udtcorrelname=TString("udtcorrel")+str;
               TString vdtcorrelname=TString("vdtcorrel")+str;
               xdtcorrel[fDchNum] = new TH2D(xdtcorrelname, "drift time correlation in X planes",100,0.,200.,100,0.,200.);
               xdtcorrel[fDchNum]->SetDirectory(0); hList.Add(xdtcorrel[fDchNum]); 
               ydtcorrel[fDchNum] = new TH2D(ydtcorrelname, "drift time correlation in Y planes",100,0.,200.,100,0.,200.);
               ydtcorrel[fDchNum]->SetDirectory(0); hList.Add(ydtcorrel[fDchNum]); 
               udtcorrel[fDchNum] = new TH2D(udtcorrelname, "drift time correlation in U planes",100,0.,200.,100,0.,200.);
               udtcorrel[fDchNum]->SetDirectory(0); hList.Add(udtcorrel[fDchNum]); 
               vdtcorrel[fDchNum] = new TH2D(vdtcorrelname, "drift time correlation in V planes",100,0.,200.,100,0.,200.);
               vdtcorrel[fDchNum]->SetDirectory(0); hList.Add(vdtcorrel[fDchNum]); 
               TString uvcorrelname=TString("uvcorrel")+str;
               uvcorrel[fDchNum] = new TH2D(uvcorrelname, "V vs U correlation",600,0.,60.,600,-60.,0.);
               uvcorrel[fDchNum]->SetDirectory(0); hList.Add(uvcorrel[fDchNum]); 
               TString uxvxcorrelname=TString("uxvxcorrel")+str;
               uxvxcorrel[fDchNum] = new TH2D(uxvxcorrelname, "VX vs UX correlation",600,0.,60.,600,0.,60.);
               uxvxcorrel[fDchNum]->SetDirectory(0); hList.Add(uxvxcorrel[fDchNum]); 
               TString hXplanesHitDistName = TString("hXplanesHitDist") + str;
               hXplanesHitDist[fDchNum] = new TH1D(hXplanesHitDistName, "x distance of hits between two neighbouring DCH planes",100,-1.,1.);
               hXplanesHitDist[fDchNum]->SetDirectory(0); hList.Add(hXplanesHitDist[fDchNum]); 
               TString hYplanesHitDistName = TString("hYplanesHitDist") + str;
               hYplanesHitDist[fDchNum] = new TH1D(hYplanesHitDistName, "y distance of hits between two neighbouring DCH planes",100,-1.,1.);
               hYplanesHitDist[fDchNum]->SetDirectory(0); hList.Add(hYplanesHitDist[fDchNum]); 
               TString hUplanesHitDistName = TString("hUplanesHitDist") + str;
               hUplanesHitDist[fDchNum] = new TH1D(hUplanesHitDistName, "u distance of hits between two neighbouring DCH planes",100,-1.,1.);
               hUplanesHitDist[fDchNum]->SetDirectory(0); hList.Add(hUplanesHitDist[fDchNum]); 
               TString hVplanesHitDistName = TString("hVplanesHitDist") + str;
               hVplanesHitDist[fDchNum] = new TH1D(hVplanesHitDistName, "v distance of hits between two neighbouring DCH planes",100,-1.,1.);
               hVplanesHitDist[fDchNum]->SetDirectory(0); hList.Add(hVplanesHitDist[fDchNum]); 
              }
              if(Iter==0){
               TString hYwiresName = TString("hYwires") + str;
	       hYwires[fDchNum] = new TH1I(hYwiresName, "y wires",240,0,240); 
	       hYwires[fDchNum]->SetDirectory(0); hList.Add(hYwires[fDchNum]);
               TString hRadialRangeName = TString("hRadialRange") + str;
	       hRadialRange[fDchNum] = new TH1D(hRadialRangeName, "distance of hits in two consecutive DCH planes", 500, 0, 100); 
	       hRadialRange[fDchNum]->SetDirectory(0); hList.Add(hRadialRange[fDchNum]);
               TString hRadialRangeAllName = TString("hRadialRangeAll") + str;
	       hRadialRangeAll[fDchNum] = new TH1D(hRadialRangeAllName, "distance of hits in two consecutive DCH planes", 500, 0, 100); 
	       hRadialRangeAll[fDchNum]->SetDirectory(0); hList.Add(hRadialRangeAll[fDchNum]);
              }
              TString hTracksPerEventName = TString("hTracksPerEvent") + str;
	      hTracksPerEvent[fDchNum] = new TH1I(hTracksPerEventName, "number of tracks per event in DCH"+str3, 500, 0, 500); 
	      hTracksPerEvent[fDchNum]->SetDirectory(0); hList.Add(hTracksPerEvent[fDchNum]);
              if(Iter==0){
               TString hWireHitsVsPlanePerEventName = TString("hWireHitsVsPlanePerEvent") + str;
	       hWireHitsVsPlanePerEvent[fDchNum] = new TH2I(hWireHitsVsPlanePerEventName, "number of wire hits vs plane number per event in DCH"+str3,numLayers,0,numLayers,30,0,30); 
	       hWireHitsVsPlanePerEvent[fDchNum]->SetDirectory(0); hList.Add(hWireHitsVsPlanePerEvent[fDchNum]);
              }
              //TString hHitsVsPlanePerEventName = TString("hHitsVsPlanePerEvent") + str;
	      //hHitsVsPlanePerEvent[fDchNum] = new TH2I(hHitsVsPlanePerEventName, "number of hits vs plane number per event in DCH"+str3,numLayers,0,numLayers,50,0,500); 
	      //hHitsVsPlanePerEvent[fDchNum]->SetDirectory(0); hList.Add(hHitsVsPlanePerEvent[fDchNum]);
              TString hWireHitsVsTracksPerEventName = TString("hWireHitsVsTracksPerEvent") + str;
	      hWireHitsVsTracksPerEvent[fDchNum] = new TH2I(hWireHitsVsTracksPerEventName, "number of tracks vs number of wire hits per event in DCH"+str3,30,0,30,500,0,500); 
	      hWireHitsVsTracksPerEvent[fDchNum]->SetDirectory(0); hList.Add(hWireHitsVsTracksPerEvent[fDchNum]);
              for (UShort_t j = 0;  j < numLayers_half; j++) {
                if(Iter==0){
		 minDriftTimePlane[fDchNum][j]->SetDirectory(0); hList.Add(minDriftTimePlane[fDchNum][j]);			
                }
	        calibList.Add(calib[fDchNum][j]);
              }
              for (UShort_t j = 0;  j < numLayers_half; j++) {
                TString hXYhitName = TString("hXYhit") + str;
		hXYhit[fDchNum][j] = new TH2D(hXYhitName, "xy hit distribution", 1000, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 1000, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume); 
		hXYhit[fDchNum][j]->SetDirectory(0); hList.Add(hXYhit[fDchNum][j]);			
                TString hXname = TString("hX") + str;
		hXhit[fDchNum][j] = new TH1D(hXname, "x coordinate", 1000, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume); 
		hXhit[fDchNum][j]->SetDirectory(0); hList.Add(hXhit[fDchNum][j]);
                TString hYname = TString("hY") + str;
		hYhit[fDchNum][j] = new TH1D(hYname, "y coordinate", 1000, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume); 
		hYhit[fDchNum][j]->SetDirectory(0); hList.Add(hYhit[fDchNum][j]);
                TString hZname = TString("hZ") + str;
		hZhit[fDchNum][j] = new TH1D(hZname, "z coordinate", 1200, 540., 660.); 
		hZhit[fDchNum][j]->SetDirectory(0); hList.Add(hZhit[fDchNum][j]);
              }		
              for (UShort_t j = 0;  j < numLayers; j++) {
                TString str2;
                str2.Form("%u",j);
                TString hWNPName = TString("hWireNumberInPlane") + str + str2;
                hWireNumberInPlane[fDchNum][j]=new TH1I(hWNPName, "wire number in plane " + str2, numWiresPerLayer, -Int_t(halfNumWiresPerLayer), Int_t(halfNumWiresPerLayer));
		hWireNumberInPlane[fDchNum][j]->SetDirectory(0); hList.Add(hWireNumberInPlane[fDchNum][j]);
                TString hWCPName = TString("hWireCoordinateInPlane") + str + str2;
                hWireCoordinateInPlane[fDchNum][j]=new TH1F(hWCPName, "wire coordinate in plane " + str2, numWiresPerLayer, -Float_t(halfNumWiresPerLayer), Float_t(halfNumWiresPerLayer));
		hWireCoordinateInPlane[fDchNum][j]->SetDirectory(0); hList.Add(hWireCoordinateInPlane[fDchNum][j]);
                TString hHCPName = TString("hHitCoordinateInPlane") + str + str2;
                hHitCoordinateInPlane[fDchNum][j]=new TH1F(hHCPName, "hit coordinate in plane " + str2, numWiresPerLayer*10, -Float_t(halfNumWiresPerLayer), Float_t(halfNumWiresPerLayer));
		hHitCoordinateInPlane[fDchNum][j]->SetDirectory(0); hList.Add(hHitCoordinateInPlane[fDchNum][j]);
		//hResolX = new TH1D("hResolX", "hit X coordinate resolution", 500, -5., 5.); 
		//hResolX->SetDirectory(0); hList.Add(hResolX);		
		//hResolY = new TH1D("hResolY", "hit Y coordinate resolution", 500, -5., 5.); 
		//hResolY->SetDirectory(0); hList.Add(hResolY);		
              }		
             }//fDchNum cycle		
             trackAngleXdch1 = new TH1D("trackAngleX_DCH1","track angle along x in DCH1",400,-20.,20.);
             trackAngleXdch1->SetDirectory(0); hList.Add(trackAngleXdch1);
             trackAngleYdch1 = new TH1D("trackAngleY_DCH1","track angle along y in DCH1",400,-20.,20.);
             trackAngleYdch1->SetDirectory(0); hList.Add(trackAngleYdch1);
             trackAngleYvsXdch1 = new TH2D("trackAngleYvsX_DCH1","track angle along yvs x in DCH1",400,-20.,20.,400,-20.,20.);
             trackAngleYvsXdch1->SetDirectory(0); hList.Add(trackAngleYvsXdch1);
             trackAngleXdch2 = new TH1D("trackAngleX_DCH2","track angle along x in DCH2",400,-20.,20.);
             trackAngleXdch2->SetDirectory(0); hList.Add(trackAngleXdch2);
             trackAngleYdch2 = new TH1D("trackAngleY_DCH2","track angle along y in DCH2",400,-20.,20.);
             trackAngleYdch2->SetDirectory(0); hList.Add(trackAngleYdch2);
             trackAngleYvsXdch2 = new TH2D("trackAngleYvsX_DCH2","track angle along yvs x in DCH2",400,-20.,20.,400,-20.,20.);
             trackAngleYvsXdch2->SetDirectory(0); hList.Add(trackAngleYvsXdch2);
             if(geantCheck){
              deflectPointX = new TH1D("deflectionPointX","x coordinate of deflection point",80,-20.,20.);
              deflectPointX->SetDirectory(0); hList.Add(deflectPointX);
              deflectPointY = new TH1D("deflectionPointY","y coordinate of deflection point",80,-20.,20.);
              deflectPointY->SetDirectory(0); hList.Add(deflectPointY);
              deflectPointPx = new TH1D("deflectionPointPx","x component of beam momentum at deflection point",100,-1.,1.);
              deflectPointPx->SetDirectory(0); hList.Add(deflectPointPx);
              deflectPointPy = new TH1D("deflectionPointPy","y component of beam momentum at deflection point",100,-1.,1.);
              deflectPointPy->SetDirectory(0); hList.Add(deflectPointPy);
              deflectPointPz = new TH1D("deflectionPointPz","z component of beam momentum at deflection point",30,8.5,8.8);
              deflectPointPz->SetDirectory(0); hList.Add(deflectPointPz);
              magfieldinteg = new TH1D("magneticfieldintegral","integral of By component of mag. field",100,0.95*abs(magFieldIntegSimul),1.05*abs(magFieldIntegSimul));
              magfieldinteg->SetDirectory(0); hList.Add(magfieldinteg);
             }
              //TString hRadiusRangeName = "hRadiusRange";
	      //hRadiusRange = new TH1D(hRadiusRangeName, "track radius range in next plane", 100, 0., 10.); 
	      //hRadiusRange->SetDirectory(0); hList.Add(hRadiusRange);	
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::BookHistsAngleCorr(){

                TString str; 
                for (UShort_t i = 0; i < numLayers; i+=2) {
                 str.Form("%u",i);
                 TString name   = TString("hAngleVsWirepos") + str;
		 hAngleVsWirepos[i]= new TH2D(name, "angle vs wire position", numWiresPerLayer, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 180, -90., 90.); 		
		 hAngleVsWirepos[i]->SetDirectory(0); hList.Add(hAngleVsWirepos[i]);	
                }
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::FitHistsAngleCorr(){

     Double_t hMax,minval;
     Int_t nbinsX,nbinsY;
     
     TFile *corrfile = new TFile("testtmp.BmnDchHitProducer_exp.root","read");

     corrfile->cd(); 

             TString str; 
             for (UShort_t i = 0; i < numLayers; i+=2) {
              str.Form("%u",i);
              TString name   = TString("hAngleVsWirepos") + str;
              hAngleVsWirepos[i] = (TH2D*)corrfile->Get(name);
              //hAngleVsWirepos[i] = (TH2D*)hAngleVsWirepos[i]->Clone(name);
              hMax=hAngleVsWirepos[i]->GetMaximum();
              minval=0.05*hMax;
              nbinsX=hAngleVsWirepos[i]->GetNbinsX();
              nbinsY=hAngleVsWirepos[i]->GetNbinsY();
              for (Int_t j = 0; j < nbinsX; j++) {
               for (Int_t k = 0; k < nbinsY; k++) {
	        if(hAngleVsWirepos[i]->GetBinContent(j,k)<minval)hAngleVsWirepos[i]->SetBinContent(j,k,0.);
               }
              }
              hAngleVsWirepos[i]->Fit("pol1");
              TF1 *myfit = (TF1*) hAngleVsWirepos[i]->GetFunction("pol1");
              anglepar[i+1][0]=myfit->GetParameter(0);
              anglepar[i+1][1]=myfit->GetParameter(1);
              cout<<" angle lin. fit params: "<<anglepar[i+1][0]<<", "<<anglepar[i+1][1]<<endl;
             }

} 
//------------------------------------------------------------------------------------------------------------------------
InitStatus 		BmnDchHitProducer_exp::Init(TChain *bmnTree, TClonesArray *dchDigits) 
{
//cout<<"Init beginning"<<endl;
//gObjectTable->Print(); 

        cout << " BmnDchHitProducer_exp::Init() " << endl;

        if(strcmp(opt,"reconstruction")==0){
         fOutputHitsBranchName = "BmnDchHit";
         TString str;
         for (UShort_t i = 0; i < 3; i++) {
          str.Form("%d",i);
          if(i>0){
           fOutputTracksBranchName[i] = "BmnDchTrack"+str;
          }else{
           fOutputTracksBranchName[i] = "BmnDchTrack";
          } 
         }
        }

        //isGeant=true;
        isGeant=false;
      
        if(strcmp(opt,"alignment")==0){
         UniDbRun* pCurrentRun = UniDbRun::GetRun(runPeriod, runNumber);
         if(pCurrentRun == 0)cout<<"Error! Run not defined in the database!"<<endl;
         assert(pCurrentRun!=0);
         double* field_voltage = 0;
         field_voltage = pCurrentRun->GetFieldVoltage();
         if(field_voltage == NULL)cout<<"Error! Field voltage not defined in the database!"<<endl;
         assert(field_voltage != NULL);
         cout<<"field voltage = "<<*field_voltage<<endl; 
         zeroField=(fabs(*field_voltage)>1.)?false:true;
        }
        cout<<"zeroField = "<<zeroField<<endl;

        geantCheck=true;
	hitGlobID = 0;
        for (UShort_t j = 0; j < 3; j++) {DCHtrackIDglob[j] = 0;} 
        if(strcmp(opt,"reconstruction")==0)isGeant=false;
        if(!isGeant)geantCheck=false;
        cout<<"file number = "<<fileNumber<<endl;
        //if(Iter==0)magFieldScale();//temporary only!!!
        //if(Iter==0)GeantInit();
        if(isGeant){ 
         if(Iter==0&&fileNumber==0)MagFieldInit();
         if(Iter==0)magFieldScale();
         if(Iter==0&&fileNumber==0)GeantInit();
         if(Iter==0&&fileNumber!=0){
          //(fRun->GetField())->Reset();
          magField->Reset();
          fRun->SetField(magField);
          (fRun->GetField())->Init();
         }
         cout<<"gMC = "<<gMC<<endl; 
         //GeantInit();
        }
        FileManagement();
        InitDchParameters();
        if(isGeant){ 
         if(Iter==0){
          GeantBeamTracker();
          Bool_t backward=false;
          Double_t parFitL[4];
          magFieldIntegSimul=MagFieldIntegral(backward,parFitL);
         }
        }
        TString str1,str3;
        if(strcmp(opt,"reconstruction")==0){
         str1="";str3="";
        }else{ 
           str1.Form("%u",Iter);str3="_";
        } 
        runPeriodStr.Form("%u",runPeriod);
        TString datatype="exp";
        TString analysisType;
        /*if(strcmp(opt,"reconstruction")==0){
          analysisType="_reconstruction";
        } else { 
          //analysisType="_calibration";
          analysisType="";
        }*/                    
        UInt_t runNumberCalib;
        TString *targetParticle;
        isTarget=false;
        if(strcmp(opt,"reconstruction")==0){
         UniDbRun* pCurrentRun = UniDbRun::GetRun(runPeriod, runNumber);
         targetParticle=pCurrentRun->GetTargetParticle();
        cout<<"target particle = "<<*targetParticle<<endl; 
         //if(strcmp(targetParticle,"")!=0)isTarget=false;
         //if(targetParticle->CompareTo("")!=0)isTarget=false; 
         if(targetParticle!=0)isTarget=true; 
        cout<<isTarget<<endl; 
        }
        if(strcmp(opt,"reconstruction")==0&&isTarget){
         runNumberCalib=runSelection();
         cout<<"runNumberCalib = "<<runNumberCalib<<endl;
        }
        TString fileName,fileNameTmp;          
        TString strrun;
        //strrun.Form("%u",runNumber);
        strrun.Form("%u",runNumber);
        TString strrunReco;
        if(strcmp(opt,"reconstruction")==0&&isTarget)strrunReco.Form("%u",runNumberCalib);
        fileName0=datatype + TString(".") + "runPeriod" + runPeriodStr + TString(".");
        //fileName =  datatype + TString(".") + "runPeriod" + TString(".") + strrun;
        fileName =  fileName0 + strrun;
        if(strcmp(opt,"reconstruction")==0&&isTarget){
         fileNameTmp=fileName0+strrunReco;
         }else{
         fileNameTmp=fileName;
        }
        DCHpar=TString("DCHpar") + TString(".") + fileNameTmp;
        if(Iter==0){
         cout<<"datatype = "<<datatype<<endl;
         cout<<"opt = "<<opt<<endl;
         TString minDriftTimeName="minDriftTime";
         minDriftTimeAll = new TH1D(minDriftTimeName+TString("0")+TString("1"),"minimal drift times", hnbins[runPeriod-1], hmintime[runPeriod-1], hmaxtime[runPeriod-1]);
         minDriftTime[0] = new TH1D(minDriftTimeName+TString("0"),"minimal drift times", hnbins[runPeriod-1], hmintime[runPeriod-1], hmaxtime[runPeriod-1]);
         minDriftTime[1] = new TH1D(minDriftTimeName+TString("1"),"minimal drift times", hnbins[runPeriod-1], hmintime[runPeriod-1], hmaxtime[runPeriod-1]);
        }else{
          TString str2;
          str2.Form("%u",Iter-1);
          residualsFileName_old = TString("dchresid") + fileName + TString("_") + str2 + TString(".root");
          //cout<<"here0 residualsFileName_old = "<<residualsFileName_old<<endl;
        }//if Iter
        residualsFileName = TString("dchresid") + fileName + str3 + str1 + TString(".root");
        if(strcmp(opt,"reconstruction")==0&&isTarget)residualsFileNameReco = TString("dchresid") + fileNameTmp + str3 + str1 + TString(".root");
        //cout<<"here0 residualsFileName = "<<residualsFileName<<endl;
        for (UShort_t fDchNum = 0;  fDchNum < numChambers; fDchNum++) {
         for (UShort_t j = 0;  j < numLayers_half; j++) {
          if(Iter==0){
           TString str,str4;
           str.Form("%u",fDchNum);
           str4.Form("%u",j);
           TString minDriftTimePlaneName="minDriftTimePlane"+str+str4;
           minDriftTimePlane[fDchNum][j] = new TH1D(minDriftTimePlaneName,"minimal drift times", hnbins[runPeriod-1], hmintime[runPeriod-1], hmaxtime[runPeriod-1]);
          }
         }
        }
        if(strcmp(opt,"reconstruction")==0){
         fOutputFileNameReco=TString("DCH.") + "runPeriod" + runPeriodStr + TString(".") + strrun + TString(".root");
         TString outPut=outDirectory+fOutputFileNameReco;
         fReco = new TFile(outPut,"RECREATE");
         tReco = new TTree("cbmexp", "test_bmn");
        }
//gObjectTable->Print(); 
        //minDriftTimeName=mindrifttimename+str; 

        //for (UShort_t fDchNum = 1; fDchNum <= numChambers; fDchNum++){
         /*TString hOccuptitle="number of hits per plane in DCH",hOccupname="hoccup";
         TString str; 
         str.Form("%u",fDchNum);
         hOccupname=hOccupname+str;
         hOccuptitle=hOccuptitle+str; 
         hOccup[fDchNum-1]  = new TH1D(hOccuptitle,hOccupname,20,0,20);*/
        //TString str; 
        //str.Form("%u",fDchNum);
        /*TString nameTrCand=TString("DchTrackCandidates")+str;
        //if(Iter==0)InitDchParameters();
        trackcand = new DchTrackCand();  
        trackcand->SetName(nameTrCand);
        trackcand->SetNameTitle(nameTrCand,"DCH track candidates, event numbers");*/
        //tree = new TTree("T","An example of a ROOT tree");
        //tree->Branch("trackcand", "Event", &trackcand, 64000,0);
         //xzAngle[fDchNum-1]=new TH1D("xzAngle","track angle in xz plane",180,-90.,90.);  
         //yzAngle[fDchNum-1]=new TH1D("yzAngle","track angle in yz plane",180,-90.,90.);
        //}
        //if(fAngleCorrectionFill)BookHistsAngleCorr(); 
        //if(fAngleCorrectionFill==false)FitHistsAngleCorr();
        //if(useCalib)rtCalibration();
        fBmnDchDigitsArray = dchDigits;
        fBmnTree = bmnTree;
        if(Iter==0){
          //rtCalibration(bmnTree);
          //gObjectTable->Print(); 
          if(strcmp(opt,"reconstruction")==0){
           rtCalibrationReader();
          }else{
           rtCalibration();
          }    
          //rtCalibration(bmnTree);
          //gObjectTable->Print(); 
          cout << " RT calibration done. " << endl;
          /*TFile f2("tmp.root","RECREATE");
          minDriftTimeAll->Write();
          for (UShort_t j = 0; j < numChambers ; j++) {
           if(calibMethod==2)splinederivsmooth_cham[j]->Write();
           minDriftTime[j]->Write();
           calib_cham[j]->Write();
           for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
            minDriftTimePlane[j][jkk]->Write();
            calib[j][jkk]->Write();
            if(calibMethod==2)splinederivsmooth[j][jkk]->Write();
           }
          }
          f2.Close();*/
        }else{
          if(!(strcmp(opt,"reconstruction")==0)){
           rtCalibCorrection();
           cout << " RT calibration correction done. " << endl;
          }
        }
      
        BookAnalysisHistograms(); 
        if(fDoCheck)BookCheckHistograms(); 

        //scaleSimExTDC=(ranmax-ranmin)/driftTimeMax;
 

    //Get ROOT Manager
  
 
        //gObjectTable->Print(); 
  	// Create and register output array

        //gObjectTable->Print(); 
        if(strcmp(opt,"reconstruction")==0){
         pHitCollection = new TClonesArray(fOutputHitsBranchName);  
         for (UShort_t i = 0;  i < 3; i++) {
          pTrackCollection[i] = new TClonesArray("BmnDchTrack");  
         }
        cout<<"after pTrackCollection: "<<endl;  
        //gObjectTable->Print(); 

         //TString OutputHitsBranchName = "BmnDch"+str+"Hit";
         //TString OutputHitsBranchName = TString("BmnDch")+TString("Hit");
         //tReco->Branch("BmnDchHit", &pHitCollection); 
         //tReco->Branch(OutputHitsBranchName, &pHitCollection); 
         hitBranch = tReco->Branch(fOutputHitsBranchName, &pHitCollection); 
         //tReco->Branch(fOutputHitsBranchName, &pHitCollection); 
         for (UShort_t i = 0;  i < 3; i++) {
          trackBranch[i] = tReco->Branch(fOutputTracksBranchName[i], &pTrackCollection[i]); 
         }
        }//if reconstruction
        //TString str0; 
        //str0.Form("%u",0);
        //TString name   = TString("BmnDch") + str + TString("Hit");
        //TString folder = TString("DCH") + str;

        cout << " Initialization finished succesfully. " << endl;
        //gObjectTable->Print(); 
        fDchTrCand1Align = new TClonesArray("TMatrix");
        fDchTrCand2Align = new TClonesArray("TMatrix");
        //gObjectTable->Print(); 
        dchTracks = new DchTrackManager(runPeriod);
        //gObjectTable->Print(); 
        dchTracks->InitDch(Iter,itermax,ranmin,ranmax,ranmin_cham,ranmax_cham,resolution,nintervals, hList, fDoCheck, outDirectory, residualsFileName_old,opt);
        if(strcmp(opt,"alignment")==0&&Iter>0)dchTracks->SetMeanDeltaPhi(meanDeltaPhi);
        if(strcmp(opt,"alignment")==0&&Iter>1)cout<<"meanDeltaPhi1 = "<<meanDeltaPhi<<endl;

        cout << " Initialization finished succesfully. " << endl;
	
//cout<<"Init end"<<endl;
        //gObjectTable->Print(); 
return kSUCCESS;

}
//------------------------------------------------------------------------------------------------------------------------
void			BmnDchHitProducer_exp::Rotate(UShort_t proj, Double_t x, Double_t y, Double_t& xRot, Double_t& yRot, Bool_t back)
{
    	// Transform to the rotated coordinate system
	// [0-3]==[x,y,u,v]  0, 90, -45, 45
	//
	assert(proj<4);
	const Int_t map[]={0, 4, 3, 2};// 0<-->0, 1<-->4, 2<-->3, 3<-->2
	if(back) proj = map[proj];
	
	//Double_t u = -h->GetX() * cosSin[1] + h->GetY() * cosSin[0];
	//Double_t v = h->GetX() * cosSin[0] + h->GetY() * cosSin[1];
	switch(proj)
	{
		case 0:		// 0 degree
		  //xRot = x;
		  //yRot = y;		
	    		xRot = y;
    			yRot = x;		
			return;		
		case 1:		// 90 degree
		  //xRot = y;
		  //yRot = -x;	
			xRot = -x;
    			yRot = y;	
			return;
		case 2:		// -45 degree
		  //xRot =  x * cosPhi_45 + y * sinPhi_45;
		  //yRot = -x * sinPhi_45 + y * cosPhi_45;
    			xRot =  -x * sinPhi_45 + y * cosPhi_45;
    			yRot = x * cosPhi_45 + y * sinPhi_45;
			return;
		case 3:		// 45 degree
		  //xRot =  x * cosPhi45 + y * sinPhi45;
		  //yRot = -x * sinPhi45 + y * cosPhi45;
    			xRot =  -x * sinPhi45 + y * cosPhi45;
    			yRot = x * cosPhi45 + y * sinPhi45;
			return;	
		case 4:		// -90 degree
		  //xRot = -y;
		  //yRot = x;
			xRot = x;
    			yRot = -y;
			return;	
											
		default: assert(false);
	}
}
//------------------------------------------------------------------------------------------------------------------------
inline Double_t			BmnDchHitProducer_exp::GetPhi(UShort_t proj)
{

	switch(proj)
	{
		case 0:	return 0.;	// 0 degree				
		case 1:	return Phi90;	// 90 degree	
		case 2:	return Phi_45;	// -45 degree
		case 3:	return Phi45;	// 45 degree						
		default: assert(false);
	}
}
//------------------------------------------------------------------------------------------------------------------------
Double_t		BmnDchHitProducer_exp::GetDriftLength(UShort_t proj, UShort_t gasgap, Double_t x, Double_t& wirePos)
{
	//   ... -1	0	1	...  - first(0) gap wire  position (X) [cm]
	//   ...    -0.5   0.5     1.5  ...  - second(1) gap wire  position (X) [cm]
	
	wirePos = (gasgap == 0) ? TMath::Nint(x) : TMath::Nint(x + 0.5) - 0.5;
		
	//AZ return TMath::Abs(x - wirePos);	// [cm]
return x - wirePos;	// [cm]
}
//------------------------------------------------------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::wirePosition(UShort_t gasgap, UInt_t wirenum, UShort_t uidLocalproj, const UShort_t idch)
{

Double_t wirePos;
//cout<<"uidLocalproj = "<<uidLocalproj<<endl;

//wire position in local frame
/*
        if(uidLocalproj==0){
          //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+1):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
          wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
          //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer));
        }else if(uidLocalproj==1){
          //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+1):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
          wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
          //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer));
        }else if(uidLocalproj==2){
          //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+1):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
          wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
          //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer));
        }else if(uidLocalproj==3){
          //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+1):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
          wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
          //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer));
        }
*/      
       if(runPeriod<=3){
        if(uidLocalproj==2){
         wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
        }else{
         wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)-stepXYhalf):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer));
        }
       }else if(runPeriod==6){
       //if(idch==0){ 
        if(uidLocalproj==2){
         wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
        }else{
         wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)-2.*stepXYhalf):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)-stepXYhalf);
        }
       //}else{
         //wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
        /*if(uidLocalproj==2){
         wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
        }else{
         wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)-stepXYhalf):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer));
        }*/
       //}
       }
        /*
        if(uidLocalproj==2){
         wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+2.*stepXYhalf);
        }else{
         wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
        }*/ 
         ////wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
         ////wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
        //wirePos=-wirePos;
        //wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
        //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer));
        //wirePos = wirePos-0.1;
        //if(uidLocalproj==1)wirePos = wirePos+0.1;
        /*if(uidLocalproj==1){
         wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+0.5):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf+0.5);
         //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)-stepXYhalf+0.5);
        }else{
         wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
         //wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)-stepXYhalf);
        }*/
        //if(uidLocalproj==0)cout<<"wires: "<<wirenum<<" "<<wirePos<<endl;
        ////wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)-stepXYhalf);
        //wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
		
//return wirePos+0.15;	// [cm]
return wirePos;	// [cm]

}

//------------------------------------------------------------------------------------------------------------------------
/*Double_t BmnDchHitProducer_exp::GetDriftDist(FairMCPoint *dchPoint, UShort_t idch, UShort_t uidLocal, Double_t x, Double_t &wirePos){

Double_t xRot,yRot;
Double_t xdchwmin,ydchwmin,zdchwmin,length,mu0,lambda0,dca,x2,y2,z2,lambdanew,dist,xnew,ynew,znew,dircosx1,dircosy1,dircosz1,mindist;
wirePos = (uidLocal%2==0) ? TMath::Nint(x) : TMath::Nint(x + 0.5) - 0.5;
Double_t mom=sqrt(pow(dchPoint->GetPx(),2.)+pow(dchPoint->GetPy(),2.)+pow(dchPoint->GetPz(),2.));
TVector2 xy(0.,wirePos),xy2;
if(uidLocal==2||uidLocal==3){
 xy2=xy.Rotate(-angleLayerRad[uidLocal]);
}else{
 xy2=xy.Rotate(angleLayerRad[uidLocal]);
}
//xRot=xy2.X();
//yRot=xy2.Y();

dircosx1=dchPoint->GetPx()/mom;dircosy1=dchPoint->GetPy()/mom;dircosz1=dchPoint->GetPz()/mom;

dca=LineLineDCA(dchPoint->GetX()-detXshift[idch], dchPoint->GetY(), dchPoint->GetZ(), dircosx1, dircosy1, dircosz1, xy2.X(), xy2.Y(), zLayer[idch][uidLocal], dirCosLayerX[uidLocal], dirCosLayerY[uidLocal], 0., x2, y2, z2, xdchwmin, ydchwmin, zdchwmin, length, mu0, lambda0); 
//cout<<dchPoint->GetX()<<" "<<dchPoint->GetY()<<" "<<dchPoint->GetZ()<<" "<<dchPoint->GetPx()/mom<<" "<<dchPoint->GetPy()/mom<<" "<<dchPoint->GetPz()/mom<<" "<<xy2.X()<<" "<<xy2.Y()<<" "<<zLayer[uidLocal]<<" "<<dirCosLayerX[uidLocal]<<" "<<dirCosLayerY[uidLocal]<<" "<<uidLocal<<endl;
//cout<<"dca = "<<dca<<", length = "<<length<<endl;
Double_t clusmean2=clusmean*length;
UInt_t nclusters=UInt_t(gRandom->Poisson(clusmean2));
Double_t stepvector[nclusters];
mindist=100000.;
for(UInt_t i = 0; i<nclusters; i++) {
   Double_t rand=gRandom->Rndm();
   stepvector[i]=length*rand;
   lambdanew=stepvector[i]/sqrt(pow(dircosx1,2.)+pow(dircosy1,2.)+pow(dircosz1,2.));
   xnew=x2-lambdanew*dircosx1;
   ynew=y2-lambdanew*dircosy1;
   znew=z2-lambdanew*dircosz1;
   dist=PointLineDistance(xnew,ynew,znew,xdchwmin,ydchwmin,zdchwmin,dirCosLayerX[uidLocal], dirCosLayerY[uidLocal], 0.); 
   if(dist<mindist)mindist=dist;
   //cout<<"dist to wire = "<<dist<<endl;  
}
//cout<<"min dist to wire = "<<mindist<<endl; 
if(fDoCheck){
  hDCA->Fill(dca); 
  hLength->Fill(length); 
  hMinDriftTime->Fill(mindist/gasDriftSpeed); 
}

return dca;

}*/
/*
//---------------------------------------------------------------------------

void BmnDchHitProducer_exp::rtCalibration(){

//Double_t drifttimemax=50.;
//TFile *fdstread = new TFile("/home/fedorisin/trunk/mpdroot/macro/strawendcapoutput/dstread_strawect_calibhist.root","read");
//TFile *fdstread = new TFile("/home/fedorisin/trunk/mpdroot/macro/strawendcapoutput/dstread_strawect_calibhist3.root","read");
TFile *fdstread; 
if(fDchNum==1){
     fdstread = new TFile("/home/fedorisin/trunk/bmnroot/macro/run/test001.BmnDchHitProducer_exp_calib.root","read");
}else if(fDchNum==2){
     fdstread = new TFile("/home/fedorisin/trunk/bmnroot/macro/run/test002.BmnDchHitProducer_exp_calib.root","read");
}

hMinDriftTime_read = (TH1D*)fdstread->Get("hMinDriftTime");
//hMinDriftTime_read->Draw();
//hMinDriftTime_read->Smooth(2);
hMinDriftTime_read->Rebin();
hMinDriftTime_read->Smooth();
TH1D* hMinDriftTimesmoothed = (TH1D*)hMinDriftTime_read->Clone("hMinDriftTimesmoothed");
hMinDriftTimesmoothed->Smooth(20);
Int_t nbinsdth2=hMinDriftTime_read->GetXaxis()->GetNbins();
for (Int_t j = 1; j <= nbinsdth2; j++) {
 hMinDriftTime_read->SetBinError(j,fabs(hMinDriftTime_read->GetBinContent(j)-hMinDriftTimesmoothed->GetBinContent(j)));
}
//Int_t binmax = hMinDriftTime_read->GetMaximumBin();
Double_t hmax = hMinDriftTime_read->GetMaximum();
//cout<<"hmax = "<<hmax<<endl;
//Double_t hmaxtol=hmax*0.01;
Double_t hmaxtol=hmax*0.05;
Int_t ranminbin,ranmaxbin;
Double_t ranmin,ranmax;
Double_t bincon;
Int_t i=1;
do{
 bincon=hMinDriftTime_read->GetBinContent(i);
 ranminbin=i;
 i++; 
}while(bincon<hmaxtol);
i=hMinDriftTime_read->GetXaxis()->GetNbins();
do{
 bincon=hMinDriftTime_read->GetBinContent(i);
 ranmaxbin=i;
 cout<<"ranmax "<<ranmaxbin<<" "<<hmaxtol<<" "<<i<<endl;
 i--; 
}while(bincon<hmaxtol);
hMinDriftTime_read->GetXaxis()->SetRange(ranminbin,ranmaxbin);
cout<<"dt fit range = "<<ranminbin<<" - "<<ranmaxbin<<endl;
hMinDriftTime_read->Fit("pol8");
fitdt = hMinDriftTime_read->GetFunction("pol8");
Double_t chi2 = fitdt->GetChisquare(); 
Int_t ndf = fitdt->GetNDF(); 
if(chi2/Double_t(ndf)>2.)cout<<"Warning, chi_squared/ndf of drift time distribution is to high! "<<"chi2/ndf = "<<chi2/Double_t(ndf)<<endl;
cout<<"chi2/ndf = "<<chi2/Double_t(ndf)<<endl;
ranmin=hMinDriftTime_read->GetBinCenter(ranminbin)-0.5*(hMinDriftTime_read->GetBinWidth(ranminbin)); 
ranmax=hMinDriftTime_read->GetBinCenter(ranmaxbin)+0.5*(hMinDriftTime_read->GetBinWidth(ranmaxbin)); 
cout<<ranminbin<<" "<<ranmaxbin<<endl;
cout<<ranmin<<" "<<ranmax<<endl;
const UInt_t nintervals=20,npoints=nintervals+1;
Double_t deldt=(ranmax-ranmin)/Double_t(npoints);
Double_t dt[npoints],r[npoints];
//Double_t norm=diagonal/(fitdt->Integral(0.,ranmax));
Double_t norm=stepXYhalf/(fitdt->Integral(0.,ranmax));
for (Int_t j = 0; j <= nintervals; j++) {
 dt[j]=deldt*Double_t(j);
 r[j]=norm*(fitdt->Integral(dt[0],dt[j]));
 cout<<"calibration curve "<<dt[j]<<" "<<r[j]<<endl;
}
calib = new TGraph(npoints,dt,r);
calib->Fit("pol3");
calib->SetTitle("r-t calibration curve");
calib->SetName("r-t calibration curve");
fitrt = calib->GetFunction("pol3");
//calib->Draw("APL");
//calib->SetMarkerSize(1);
//calib->SetMarkerStyle(21);
//const UInt_t poldeg=9; 
//Double_t pardt[poldeg]; 
//for (Int_t i = 0; i < poldeg; i++) {
  //pardt[i]=pol8->GetParameter(i);
//}
fdstread->Close();

}
*/

//---------------------------------------------------------------------------

//void BmnDchHitProducer_exp::rtCalibration(TChain *bmnTree){
void BmnDchHitProducer_exp::rtCalibration(){

     /*TFile *fdstread = new TFile("/home/fedorisin/trunk/bmnroot/dch_exp/testCC.BmnDchHitProducer_exp.root","read");
     minDriftTime = (TH1D*)fdstread->Get("minDriftTime1");
     */

    BmnDchDigit* digit = NULL;
//gObjectTable->Print();
    UInt_t nevents = fBmnTree->GetEntries();
    cout << "nevents = " << nevents << endl;
    //fBmnTree->SetAutoDelete(kTRUE);
    for (Int_t iev = 0; iev < nevents; iev++) {
//gObjectTable->Print();
        fBmnTree->GetEntry(iev);
        //bmnTree->GetEntry(iev);
//gObjectTable->Print();
        //if(iev%1000==0)cout << "event number = " << iev << endl;
        Int_t nhits=fBmnDchDigitsArray->GetEntriesFast();
        //cout<<"nhits in Dch = "<<nhits<<endl;
      for (Int_t ihit = 0; ihit < nhits; ihit++) {
        digit = (BmnDchDigit*) fBmnDchDigitsArray->At(ihit);
        //cout<<"time = "<<digit->GetTime()<<endl;
        //minDriftTime[fDchNum-1]->Fill(digit->GetTime());
        UShort_t uid=UShort_t(digit->GetPlane());
        //if(!(uid==4||uid==5||uid==13||uid==15)){ 
         if(uid<8){
           minDriftTime[0]->Fill(digit->GetTime());
           minDriftTimePlane[0][mapPlaneID(uid)/2]->Fill(digit->GetTime());
         }else{
           minDriftTime[1]->Fill(digit->GetTime());
           //minDriftTimePlane[1][(mapPlaneID(uid-numLayers))/2]->Fill(digit->GetTime());
           minDriftTimePlane[1][(mapPlaneID(uid)-numLayers)/2]->Fill(digit->GetTime());
         }
         minDriftTimeAll->Fill(digit->GetTime());
        //}
      }
//gObjectTable->Print();
    } // event loop
cout<<"before backgroundRemoval"<<endl;

//gObjectTable->Print();
    for (UShort_t i = 0; i < numChambers; i++) {
     backgroundRemoval(minDriftTime[i]);
     for (UShort_t j = 0; j < numLayers_half; j++) { 
      backgroundRemoval(minDriftTimePlane[i][j]);
     }
    }

//Double_t hist_entries=minDriftTime[1]->Integral();
//Int_t nsmooth=Int_t(2e+06/hist_entries)+1;

for (UShort_t i = 0; i < numChambers; i++) {
 Double_t hist_entries_cham=minDriftTime[i]->Integral();
 Int_t nsmooth_cham=Int_t(2e+06/hist_entries_cham)+1;
 Int_t nbins_cham=minDriftTime[i]->GetXaxis()->GetNbins();
 minDriftTime[i]->Smooth(nsmooth_cham);
 spline5_cham[i] = new TSpline5(minDriftTime[i], "", 0.,0.,0.,0.);
 //if(fDoCheck)hList.Add(spline5_cham[i]);
 spline5_cham[i]->SetTitle("TDC");
 spline5_cham[i]->SetName("tdc");
 Int_t nbinsUsed_cham;
 Bool_t cham=false;
 for (UShort_t k = 0; k < numLayers_half; k++) {

     Double_t hist_entries;
     Int_t nsmooth;
     Int_t nbins;
     Int_t nbinsUsed;
     hist_entries=minDriftTimePlane[i][k]->Integral();
     nsmooth=Int_t(2e+06/hist_entries)+1;
     TString str;
     str.Form("%u",i+1);
     cout<<TString("DCH")+str<<endl;
     //if(i==0)minDriftTime=minDriftTime1;
     //if(i==1)minDriftTime=minDriftTime2;

     //Int_t nbins=minDriftTime[i]->GetXaxis()->GetNbins();
     nbins=minDriftTimePlane[i][k]->GetXaxis()->GetNbins();

    /*if(checkDch){
     for (Int_t ibin = 1; ibin <= nbins; ibin++) {
        cout<<"hist. time = "<<minDriftTime[fDchNum-1]->GetBinCenter(ibin)<<" "<<minDriftTime[fDchNum-1]->GetBinContent(ibin)<<endl;
     }
    }*/
//minDriftTime[fDchNum-1] = (TH1D*)fdstread->Get("hMinDriftTime");
//minDriftTime[fDchNum-1]->Draw();
//minDriftTime[fDchNum-1]->Smooth(2);
//minDriftTime[fDchNum-1]->Rebin();
//minDriftTime[i]->Smooth();
//minDriftTime[i]->Smooth(nsmooth);
minDriftTimePlane[i][k]->Smooth(nsmooth);
/*TH1D* hMinDriftTimesmoothed = (TH1D*)minDriftTime[fDchNum-1]->Clone("hMinDriftTimesmoothed");
hMinDriftTimesmoothed->Smooth(20);
Int_t nbinsdth2=minDriftTime[fDchNum-1]->GetXaxis()->GetNbins();
for (Int_t j = 1; j <= nbinsdth2; j++) {
 minDriftTime[fDchNum-1]->SetBinError(j,fabs(minDriftTime[fDchNum-1]->GetBinContent(j)-hMinDriftTimesmoothed->GetBinContent(j)));
}
*/
//Int_t binmax = minDriftTime[fDchNum-1]->GetMaximumBin();
//Int_t nbinsUsed[numChambers];
//spline3 = new TSpline3(minDriftTime[fDchNum-1], "", 0., 0.);
TString str2,minDriftTimeName="minDriftTime";
str2.Form("%u",i);
//spline5 = new TSpline5(minDriftTimeName+str2, "", 0.,0.,0.,0.);
//if(i==0){
 //spline5[i] = new TSpline5(minDriftTime[i], "", 0.,0.,0.,0.);
 //spline5[i]->SetTitle("TDC");
 //spline5[i]->SetName("tdc");
 //for (UShort_t j = 0; j < numLayers_half; j++) { 
  spline5[i][k] = new TSpline5(minDriftTimePlane[i][k], "", 0.,0.,0.,0.);
  //if(fDoCheck)hList.Add(spline5[i][k]);
  spline5[i][k]->SetTitle("TDC");
  spline5[i][k]->SetName("tdc");
 //}
//}
if(calibMethod==1&&k==0){
//if(calibMethod==1){
Double_t hmax = minDriftTime[i]->GetMaximum();
Int_t hmaxbin = minDriftTime[i]->GetMaximumBin();
Double_t hmin = minDriftTime[i]->GetMinimum();
minDriftTime[i]->GetXaxis()->SetRange(1,hmaxbin);
Int_t hminbin1 = minDriftTime[i]->GetMinimumBin();
minDriftTime[i]->GetXaxis()->SetRange(hmaxbin,nbins_cham);
Int_t hminbin2 = minDriftTime[i]->GetMinimumBin();
minDriftTime[i]->GetXaxis()->SetRange(1,nbins_cham);
Double_t hminx1 = minDriftTime[i]->GetXaxis()->GetBinCenter(hminbin1);
Double_t hminx2 = minDriftTime[i]->GetXaxis()->GetBinCenter(hminbin2);
cout<<"hmaxbin = "<<hmaxbin<<endl;
cout<<"hminx1 = "<<hminx1<<", hminbin1 = "<<hminbin1<<endl;
cout<<"hminx2 = "<<hminx2<<", hminbin2 = "<<hminbin2<<endl;
cout<<"hmin = "<<hmin<<", hmax = "<<hmax<<endl;
//Int_t ranminbin,ranmaxbin;
//Double_t ranmin,ranmax;
/*
Double_t hmaxtol=hmax*0.01;
Double_t bincon;
Int_t i=1;
do{
 bincon=minDriftTime[fDchNum-1]->GetBinContent(i);
 ranminbin=i;
 i++; 
}while(bincon<hmaxtol);
i=minDriftTime[fDchNum-1]->GetXaxis()->GetNbins();
do{
 bincon=minDriftTime[fDchNum-1]->GetBinContent(i);
 ranmaxbin=i;
 //cout<<"ranmax "<<ranmaxbin<<" "<<hmaxtol<<" "<<i<<endl;
 i--; 
}while(bincon<hmaxtol);
minDriftTime[fDchNum-1]->GetXaxis()->SetRange(ranminbin,ranmaxbin);
cout<<"dt fit range = "<<ranminbin<<" - "<<ranmaxbin<<endl;

minDriftTime[fDchNum-1]->Fit("pol4");
minDriftTime[fDchNum-1]->Fit("pol5");
minDriftTime[fDchNum-1]->Fit("pol6");
minDriftTime[fDchNum-1]->Fit("pol7");
//minDriftTime[fDchNum-1]->Fit("pol8");
//fitdt = minDriftTime[fDchNum-1]->GetFunction("pol6");
fitdt = minDriftTime[fDchNum-1]->GetFunction("pol7");
//fitdt = minDriftTime[fDchNum-1]->GetFunction("pol8");
Double_t chi2 = fitdt->GetChisquare(); 
//if(chi2==0) Fix it!
Int_t ndf = fitdt->GetNDF(); 
if(chi2/Double_t(ndf)>2.)cout<<"Warning, chi_squared/ndf of drift time distribution is to high! "<<"chi2/ndf = "<<chi2/Double_t(ndf)<<endl;
cout<<"chi2/ndf = "<<chi2/Double_t(ndf)<<endl;
ranmin=minDriftTime[fDchNum-1]->GetBinCenter(ranminbin)-0.5*(minDriftTime[fDchNum-1]->GetBinWidth(ranminbin)); 
ranmax=minDriftTime[fDchNum-1]->GetBinCenter(ranmaxbin)+0.5*(minDriftTime[fDchNum-1]->GetBinWidth(ranmaxbin)); 
*/
Int_t jmin=hminbin1-5,jmax=hminbin1+5;
UInt_t jj=0;
Double_t sum=0.,binc=0.;
for (Int_t j = jmin; j <= jmax; j++) {
 if(j>0){
   binc=minDriftTime[i]->GetBinContent(j);
   sum=sum+binc;
   jj++;
 }
}
Double_t averbincon=sum/Double_t(jj),upperlim=averbincon+5.*sqrt(averbincon);
Double_t bincon0,bincon1;
Int_t ranminbin;
Int_t ii=hmaxbin-1;
do{
 bincon0=minDriftTime[i]->GetBinContent(ii+1);
 bincon1=minDriftTime[i]->GetBinContent(ii);
 //cout<<"ranmax "<<ranmaxbin<<" "<<hmaxtol<<" "<<i<<endl;
 ii--; 
}while(!((bincon1>bincon0&&bincon1<upperlim)||UInt_t(bincon1)==0));
ranminbin=ii+2;
//cout<<ranmaxbin<<" "<<ranmaxbin<<endl;
cout<<"ranminbin = "<<ranminbin<<endl;
//nbinsUsed[i]=hminbin2-ranminbin+1;
nbinsUsed_cham=hminbin2-ranminbin+1;
//Int_t nbinsUsed=hminbin2-hminbin1+1;
//ranmin=minDriftTime[fDchNum-1]->GetBinCenter(hminbin1)-0.5*(minDriftTime[fDchNum-1]->GetBinWidth(hminbin1)); 
ranmin_cham[i]=minDriftTime[i]->GetBinCenter(ranminbin)-0.5*(minDriftTime[i]->GetBinWidth(ranminbin)); 
ranmax_cham[i]=minDriftTime[i]->GetBinCenter(hminbin2)+0.5*(minDriftTime[i]->GetBinWidth(hminbin2)); 
//cout<<"time interval: "<<ranmin[i]<<" "<<ranmax[i]<<endl;
}else if(calibMethod==2){
 Double_t inflX1,inflX2;
 tdcInflexPoints(inflX1,inflX2,i,k,cham);
 cout<<"inflex points (left,right) in chamber " <<i<<" of plane "<<k<<" = "<<inflX1<<", "<<inflX2<<endl;
 if(runPeriod==6){
  Int_t hmaxbin = minDriftTimePlane[i][k]->GetMaximumBin();
  Double_t hmaxpos = minDriftTimePlane[i][k]->GetXaxis()->GetBinCenter(hmaxbin);
  ranmin[i][k]=inflX1-(hmaxpos-inflX1);
 }else{
  ranmin[i][k]=inflX1;
 }
 ranmax[i][k]=inflX2;
 Double_t binwidth=minDriftTimePlane[i][k]->GetBinWidth(1);
 nbinsUsed=Int_t(ranmax[i][k]/binwidth)-Int_t(ranmin[i][k]/binwidth)+1;
 if(k==0){
  cham=true; 
  tdcInflexPoints(inflX1,inflX2,i,k,cham);
  cout<<"inflex points (left,right) in chamber " <<i<<" = "<<inflX1<<", "<<inflX2<<endl;
  if(runPeriod==6){
   Int_t hmaxbin = minDriftTime[i]->GetMaximumBin();
   Double_t hmaxpos = minDriftTime[i]->GetXaxis()->GetBinCenter(hmaxbin);
   ranmin_cham[i]=inflX1-(hmaxpos-inflX1);
  }else{
   ranmin_cham[i]=inflX1;
  }
  ranmax_cham[i]=inflX2;
  binwidth=minDriftTime[i]->GetBinWidth(1);
  nbinsUsed_cham=Int_t(ranmax_cham[i]/binwidth)-Int_t(ranmin_cham[i]/binwidth)+1;
  cout<<"nbinsUsed_cham = "<<nbinsUsed_cham<<endl;
  cham=false; 
 }
}

Double_t timeRange=ranmax[i][k]-ranmin[i][k];
cout<<"time interval: "<<ranmin[i][k]<<" "<<ranmax[i][k]<<endl;
UInt_t npoints=nintervals+1;
Double_t deldt=(ranmax[i][k]-ranmin[i][k])/Double_t(nintervals);
//Double_t dt[npoints],r[npoints];
Double_t dt[nintervals],r[nintervals];
Double_t rr[nintervals];
//Double_t norm=diagonal/(fitdt->Integral(0.,ranmax));
//Double_t norm=stepXYhalf/(fitdt->Integral(0.,ranmax));
cout<<"nbinsUsed = "<<nbinsUsed<<endl;
//Double_t norm=stepXYhalf/SplineIntegral(ranmin[i][k],ranmax[i][k],nbinsUsed*5,spline5[i][k]);
Double_t normPlane;
normPlane=stepXYhalf/SplineIntegral(ranmin[i][k],ranmax[i][k],nbinsUsed*5,spline5[i][k]);
for (Int_t j = 0; j < nintervals; j++) {
 dt[j]=deldt*Double_t(j)+deldt/2.;
 //dt[k][j]=deldt*Double_t(j)+deldt/2.;
 //r[j]=norm*(fitdt->Integral(dt[0],dt[j]));
 //r[j]=norm*SplineIntegral(dt[0]+ranmin[i][k],dt[j]+ranmin[i][k],5*TMath::Nint(Double_t(nbinsUsed)*(dt[j]-dt[0])/timeRange),spline5[i][k]);
  rr[j]=normPlane*SplineIntegral(dt[0]+ranmin[i][k],dt[j]+ranmin[i][k],5*TMath::Nint(Double_t(nbinsUsed)*(dt[j]-dt[0])/timeRange),spline5[i][k]);
 if(checkDch)cout<<"calibration curve "<<dt[j]<<" "<<rr[j]<<" "<<SplineIntegral(dt[0]+ranmin[i][k],dt[j]+ranmin[i][k],5*TMath::Nint(Double_t(nbinsUsed)*(dt[j]-dt[0])/timeRange),spline5[i][k])<<endl;
}
 //calib[i][k] = new TGraph(nintervals,dt,r);
 //spline5rt[i][k] = new TSpline5("r-t calibration curve",calib[i][k], "", 0.,0.,0.,0.);
 //if(fDoCheck)hList.Add(spline5rt[i][k]); 
 //calib[i][k]->SetTitle("r-t calibration curve");
 //calib[i][k]->SetName("rtCalibCurve"+str2);
 //spline5rt[i][k]->SetName("rtCalibSpline"+str2);
 TString str3;
 str3.Form("%u",k);
 calib[i][k] = new TGraph(nintervals,dt,rr);
 spline5rt[i][k] = new TSpline5("r-t calibration curve",calib[i][k], "", 0.,0.,0.,0.);
 if(fDoCheck)hList.Add(spline5rt[i][k]); 
 calib[i][k]->SetTitle("r-t calibration curve");
 calib[i][k]->SetName("rtCalibCurve"+str2+str3);
 spline5rt[i][k]->SetName("rtCalibSpline"+str2+str3);
 if(k==0){
  cham=true; 
  timeRange=ranmax_cham[i]-ranmin_cham[i];
  cout<<"time interval: "<<ranmin_cham[i]<<" "<<ranmax_cham[i]<<endl;
  npoints=nintervals+1;
  deldt=(ranmax_cham[i]-ranmin_cham[i])/Double_t(nintervals);
  //Double_t dt[npoints],r[npoints];
  dt[nintervals],r[nintervals];
  Double_t rr_cham[nintervals];
  Double_t norm;
  //Double_t norm=diagonal/(fitdt->Integral(0.,ranmax));
  //Double_t norm=stepXYhalf/(fitdt->Integral(0.,ranmax));
  cout<<"nbinsUsed_cham = "<<nbinsUsed_cham<<endl;
  norm=stepXYhalf/SplineIntegral(ranmin_cham[i],ranmax_cham[i],nbinsUsed_cham*5,spline5_cham[i]);
  //normPlane=stepXYhalf/SplineIntegral(ranmin_cham[i],ranmax_cham[i],nbinsUsed_cham*5,spline5_cham[i]);
  for (Int_t j = 0; j < nintervals; j++) {
   dt[j]=deldt*Double_t(j)+deldt/2.;
   //dt[k][j]=deldt*Double_t(j)+deldt/2.;
   //r[j]=norm*(fitdt->Integral(dt[0],dt[j]));
   //r[j]=norm*SplineIntegral(dt[0]+ranmin_cham[i],dt[j]+ranmin_cham[i],5*TMath::Nint(Double_t(nbinsUsed_cham)*(dt[j]-dt[0])/timeRange),spline5_cham[i]);
    rr_cham[j]=norm*SplineIntegral(dt[0]+ranmin_cham[i],dt[j]+ranmin_cham[i],5*TMath::Nint(Double_t(nbinsUsed_cham)*(dt[j]-dt[0])/timeRange),spline5_cham[i]);
   if(checkDch)cout<<"calibration curve "<<dt[j]<<" "<<rr_cham[j]<<" "<<SplineIntegral(dt[0]+ranmin_cham[i],dt[j]+ranmin_cham[i],5*TMath::Nint(Double_t(nbinsUsed_cham)*(dt[j]-dt[0])/timeRange),spline5_cham[i])<<endl;
  }
   //calib_cham[i] = new TGraph(nintervals,dt,r);
   //spline5rt_cham[i] = new TSpline5("r-t calibration curve",calib_cham[i], "", 0.,0.,0.,0.);
   //calib->Fit("pol5");
   //calib_cham[i]->SetTitle("r-t calibration curve");
   //calib_cham[i]->SetName("rtCalibCurve"+str2);
   //spline5rt_cham[i]->SetName("rtCalibSpline"+str2);
   calib_cham[i] = new TGraph(nintervals,dt,rr_cham);
   spline5rt_cham[i] = new TSpline5("r-t calibration curve",calib_cham[i], "", 0.,0.,0.,0.);
   //if(fDoCheck)hList.Add(spline5rt_cham[i]); 
   hList.Add(spline5rt_cham[i]); 
   calib_cham[i]->SetTitle("r-t calibration curve");
   calib_cham[i]->SetName("rtCalibCurve"+str2);
   spline5rt_cham[i]->SetName("rtCalibSpline"+str2);
   cham=false;
  }
 } // plane cycle
} // chambers cycle
for (UShort_t idch = 0; idch < numChambers; idch++) {
   delete spline5_cham[idch]; 
   for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
     delete spline5[idch][jkk]; 
   }
}
//fitrt = calib->GetFunction("pol5");
//cout <<"drift time spectrum maximum = "<<fitdt->GetMaximumX()<<" ns"<<endl;
//cout <<"drift time spectrum maximum = "<<minDriftTime[fDchNum-1]->GetMaximumX()<<" ns"<<endl;
//calib->Draw("APL");
//calib->SetMarkerSize(1);
//calib->SetMarkerStyle(21);
//const UInt_t poldeg=9; 
//Double_t pardt[poldeg]; 
//for (Int_t i = 0; i < poldeg; i++) {
  //pardt[i]=pol8->GetParameter(i);
//}

//spline3 = new TSpline3("spline3",splinederiv->GetX(),splinederiv->GetY(),splinederiv->GetN());
//fdstread->Close();
//NumericalRootFinder();

}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::tdcInflexPoints(Double_t &inflX1, Double_t &inflX2, const UShort_t idch, const UShort_t iplane, const Bool_t cham){

TGraph *splinederiv;
TH1D *hist_new;
 
//startsmooth:if(cham){
if(cham){
 Int_t ngrpoints=minDriftTime[idch]->GetNbinsX();
 Double_t binwidth=minDriftTime[idch]->GetBinWidth(1);
 Double_t halfbinwidth=0.5*binwidth;
 Int_t nbinsOffset;Double_t xstart;
 if(runPeriod<=3){
  nbinsOffset=Int_t(200./binwidth);
 }else if(runPeriod==6){
 // nbinsOffset=Int_t(300./binwidth);
  nbinsOffset=Int_t(200./binwidth);
 }
 //Double_t xstart=minDriftTime[idch]->GetBinCenter(1);
 xstart=minDriftTime[idch]->GetBinCenter(nbinsOffset);//x starts at 400 for runPeriod<=3, at -100 for runPeriod=6
 cout<<"xstart = "<<xstart<<endl;
 ngrpoints=ngrpoints-nbinsOffset;
 Double_t xspl[ngrpoints],yspl[ngrpoints];
 for (Int_t ival = 0; ival < ngrpoints; ival++) {
  xspl[ival]=xstart+Double_t(ival)*binwidth;
  yspl[ival]=spline5_cham[idch]->Derivative(xspl[ival]);
 }
 splinederiv = new TGraph(ngrpoints,xspl,yspl);
 splinederiv->SetTitle("derivative of r-t calibration curve");
 splinederiv->SetName("rtCalibCurve derivative");
 TString spldersm="splinederivsmooth";
 TString str;
 str.Form("%u",idch+1);
 TString splineDerivSmooth = spldersm+str;
 splinederivsmooth_cham[idch] = new TH1D(splineDerivSmooth,"smoothed hist of spline derivatives",ngrpoints,xspl[0]-halfbinwidth,xspl[ngrpoints-1]+halfbinwidth);
 splinederivsmooth_cham[idch]->SetDirectory(0); hList.Add(splinederivsmooth_cham[idch]);
 for (Int_t ival = 0; ival < ngrpoints; ival++) {
  splinederivsmooth_cham[idch]->SetBinContent(ival+1,yspl[ival]);
 }
 //splinederivsmooth_cham[idch]->Rebin();
 splinederivsmooth_cham[idch]->Rebin();
 splinederivsmooth_cham[idch]->Smooth(1000);
 Int_t hmaxbin_splinederiv = splinederivsmooth_cham[idch]->GetMaximumBin();
 inflX1 = splinederivsmooth_cham[idch]->GetBinCenter(hmaxbin_splinederiv); //left inflex point
//Int_t hminbin_splinederiv = splinederivsmooth[fDchNum-1]->GetMinimumBin();
 hist_new=(TH1D*)splinederivsmooth_cham[idch]->Clone();
}else{
 Int_t ngrpoints=minDriftTimePlane[idch][iplane]->GetNbinsX();
 Double_t binwidth=minDriftTimePlane[idch][iplane]->GetBinWidth(1);
 Double_t halfbinwidth=0.5*binwidth;
 Int_t nbinsOffset;Double_t xstart;
 if(runPeriod<=3){
  nbinsOffset=Int_t(200./binwidth);
 }else if(runPeriod==6){
  //nbinsOffset=Int_t(300./binwidth);
  nbinsOffset=Int_t(200./binwidth);
 }
 //Double_t xstart=minDriftTime[idch]->GetBinCenter(1);
 xstart=minDriftTimePlane[idch][iplane]->GetBinCenter(nbinsOffset);//x starts at 400 for runPeriod<=3, at -100 for runPeriod=6
 ngrpoints=ngrpoints-nbinsOffset;
 Double_t xspl[ngrpoints],yspl[ngrpoints];
 for (Int_t ival = 0; ival < ngrpoints; ival++) {
  xspl[ival]=xstart+Double_t(ival)*binwidth;
  yspl[ival]=spline5[idch][iplane]->Derivative(xspl[ival]);
 }
 splinederiv = new TGraph(ngrpoints,xspl,yspl);
 splinederiv->SetTitle("derivative of r-t calibration curve");
 splinederiv->SetName("rtCalibCurve derivative");
 TString spldersm="splinederivsmooth";
 TString str,str2;
 str.Form("%u",idch+1);
 str2.Form("%u",iplane+1);
 TString splineDerivSmooth = spldersm+str+str2;
 splinederivsmooth[idch][iplane] = new TH1D(splineDerivSmooth,"smoothed hist of spline derivatives",ngrpoints,xspl[0]-halfbinwidth,xspl[ngrpoints-1]+halfbinwidth);
 splinederivsmooth[idch][iplane]->SetDirectory(0); hList.Add(splinederivsmooth[idch][iplane]);
 for (Int_t ival = 0; ival < ngrpoints; ival++) {
 splinederivsmooth[idch][iplane]->SetBinContent(ival+1,yspl[ival]);
 }
 splinederivsmooth[idch][iplane]->Rebin();
 splinederivsmooth[idch][iplane]->Smooth(1000);
 Int_t hmaxbin_splinederiv = splinederivsmooth[idch][iplane]->GetMaximumBin();
 inflX1 = splinederivsmooth[idch][iplane]->GetBinCenter(hmaxbin_splinederiv); //left inflex point
//Int_t hminbin_splinederiv = splinederivsmooth[fDchNum-1]->GetMinimumBin();
 hist_new=(TH1D*)splinederivsmooth[idch][iplane]->Clone();
}
hist_new->Scale(-1.);
TSpectrum *s = new TSpectrum(10,1.);
//Int_t nfound = s->Search(hist_new,2,"nobackground",0.025);
Int_t nfound = s->Search(hist_new,2,"nobackground",0.05);
/*Int_t nfound=0;
Double_t threshold=0.05;
Double_t threshdif=0.005;
while(nfound<=2){
 nfound = s->Search(hist_new,2,"nobackground",threshold);
 cout<<"number of inflection points = "<<nfound<<endl;
 if(areSame(threshold,threshdif)){
  threshold=std::numeric_limits<double>::epsilon();
 }else{ 
  threshold-=threshdif;
 }
}*/
Double_t *xpeaks = s->GetPositionX();
//Float_t ypeaks[nfound];
/*
if(nfound==2){
 inflX2 = Double_t(xpeaks[nfound-1]); //right inflection point
}else if(nfound>2){
 inflX2 = Double_t(0.5*(xpeaks[nfound-1]+xpeaks[nfound-2]));
}
*/
TAxis *xaxis = hist_new->GetXaxis();
cout<<"inflection points:"<<endl;
//for (Int_t ival = 0; ival < nfound; ival++) {
 //cout<<xpeaks[ival]<<endl;
 //Int_t binx = xaxis->FindBin(xpeaks[ival]);
 //ypeaks[ival]=hist_new->GetBinContent(binx);
 //cout<<ypeaks[ival]<<endl;
//}
//Float_t ymax=(Float_t)*std::max_element(ypeaks,ypeaks+nfound);
//Int_t index_ymax=(Int_t)std::distance(ypeaks,std::max_element(ypeaks,ypeaks+nfound));
//cout<<"maximum is "<<ymax<<endl;
//cout<<"index of maximum is "<<index_ymax<<endl;
//Double_t hist_new_max = hist_new->GetMaximum();
int index[nfound];
int newindexmax;
TMath::Sort(nfound,xpeaks,index,kTRUE);
for (Int_t ival = 0; ival < nfound; ival++) {
 cout<<xpeaks[index[ival]]<<endl;
 //cout<<ypeaks[index[ival]]<<endl;
 if(runPeriod<=3)if(index[ival]==0)newindexmax=ival;
 //if(index[ival]==0)newindexmax=ival;
}
Int_t hist_new_maxbin = hist_new->GetMaximumBin();
Double_t hist_new_max_position=hist_new->GetXaxis()->GetBinCenter(hist_new_maxbin);
cout<<"Maximum position is "<<hist_new_max_position<<endl;
cout<<nfound-1<<" "<<index[0]<<endl;
if(runPeriod<=3){
 if((nfound-1)>newindexmax)nfound=newindexmax+1;
}
cout<<"Corrected number of peaks = "<<nfound<<endl;
//assert(nfound>=2);
if(nfound>=4)cout<<"Number of peaks is too high!"<<endl;
//if(nfound>4)goto startsmooth;
if(runPeriod<=3)assert(nfound>=1&&nfound<=4);
//if(runPeriod==6)assert(nfound>=1&&nfound<=5);
if(runPeriod==6)assert(nfound>=1);
//assert(nfound>=1);
/*if(nfound==2){
 inflX2=xpeaks[index[0]];
}else if(nfound>2){
 cout<<xpeaks[index[0]]<<" "<<xpeaks[index[1]]<<" "<<hist_new_max_position<<" "<<abs(xpeaks[index[0]]-xpeaks[index[1]])<<" "<<abs(xpeaks[index[1]]-hist_new_max_position)<<endl;
 if(abs(xpeaks[index[0]]-xpeaks[index[1]])<abs(xpeaks[index[1]]-hist_new_max_position)){
  inflX2=0.5*(xpeaks[index[0]]+xpeaks[index[1]]);
 }else{
  inflX2=xpeaks[index[0]];
 }
}*/
if(runPeriod<=3){
 if(nfound==1){
  //inflX2=xpeaks[index[0]];
  inflX2=inflX1+(xpeaks[index[0]]-inflX1)*1.5;
 }else if(nfound==2){
  inflX2=0.5*(xpeaks[index[0]]+hist_new_max_position);
 }else if(nfound==3){
  cout<<xpeaks[index[0]]<<" "<<xpeaks[index[1]]<<" "<<hist_new_max_position<<" "<<abs(xpeaks[index[0]]-xpeaks[index[1]])<<" "<<abs(xpeaks[index[1]]-hist_new_max_position)<<endl;
  if(abs(xpeaks[index[0]]-xpeaks[index[1]])<abs(xpeaks[index[1]]-hist_new_max_position)){
   inflX2=0.5*(xpeaks[index[0]]+xpeaks[index[1]]);
   inflX2=0.5*(hist_new_max_position+inflX2);
  }else{
   inflX2=0.5*(xpeaks[index[0]]+hist_new_max_position);
  }
 }else if(nfound==4){
   inflX2=0.5*(xpeaks[index[0]]+xpeaks[index[1]]);
   inflX2=0.5*(hist_new_max_position+inflX2);
 }
}else if(runPeriod==6){
 /*Int_t nPositPeaks=0;//peaks on the positive x xaxis
 for (Int_t i = 0; i < nfound; i++) {
  if(xpeaks[index[i]]>0.)nPositPeaks++;
 }
 Int_t difInd=nfound-nPositPeaks;
 cout<<"difInd: "<<difInd<<", nPositPeaks: "<<nPositPeaks <<endl;
 Float_t xpeaks2[nPositPeaks];
 for (Int_t i = 0; i < nfound-difInd; i++) {
  xpeaks2[i]=xpeaks[index[i]];
  //cout<<"xpeaks2: "<<xpeaks2[i]<<" "<<i<<endl;
 }*/
 if(nfound==1){
  //inflX2=xpeaks[index[0]];
  inflX2=inflX1+350.;
 }else if(nfound==2){
  if(xpeaks[index[0]]>200&&xpeaks[index[0]]<500){ 
   inflX2=xpeaks[index[0]];
  }else{
   inflX2=inflX1+350.;
  }
 //}else if(nfound==3||nfound==4){
 }else if(nfound>=3||nfound<=5){
  if(xpeaks[index[0]]>500.){
   inflX2=xpeaks[index[1]];
  }else{
   inflX2=xpeaks[index[0]];
  }
 }else{
 cout<<"Warning: Too many peaks!"<<endl;
 inflX2=inflX1+350.;
  //inflX2=xpeaks2[nPositPeaks-2];
  //inflX2=xpeaks[index[1]];
 }
}
 //cout<<xpeaks[index[1]]<<" "<<hist_new_max_position<<" "<<abs(xpeaks[index[1]]-hist_new_max_position)<<endl;
/*if(abs(xpeaks[index[1]]-hist_new_max_position)<10.){
 inflX2=xpeaks[index[0]];
}else{
 inflX2=0.5*(xpeaks[index[0]]+xpeaks[index[1]]);
}*/
cout << "The upper limit is "<<inflX2<<" and the interval width is "<<inflX2-inflX1<<"."<<'\n';
/*
if(nfound==2){
inflX2=xpeaks[index[0]];
}else if(nfound>2){
inflX2=(0.5*(xpeaks[index[0]]+xpeaks[index[1]])+xpeaks[index[2]])*0.5;
}*/
//inflX2=(Double_t)*std::max_element(xpeaks,xpeaks+nfound);
//inflX2=(inflX2+hist_new_max_position)/2.;
//inflX2=(Double_t)*std::max_element(xpeaks,xpeaks+nfound)-50.;
//inflX2=inflX1+150.;
//inflX2=inflX1+2.*(hist_new_max_position-inflX1);
//inflX2=inflX1+0.75*(inflX2-inflX1);
//cout << "The upper corrected limit is "<<inflX2<<" and the corrected interval width is "<<inflX2-inflX1<<"."<<'\n';
delete hist_new;
delete splinederiv;
delete s;

}
 
//------------------------------------------------------------------------------------------------------------------------
Bool_t 			BmnDchHitProducer_exp::HitExist(Double_t delta) // [ns] 
{
  //const static Double_t _Time[4]	= {0.,	20.,	20.01,	20.02}; 	// 0.,	2.,	10.,	40.
 	const static Double_t _Time[4]	= {0.,	2.,	2.01,	2.02}; 	// 0.,	2.,	10.,	40.
 	const static Double_t _Eff[4]	= {0.,	0.01,	0.99,	1.0};	// 0.,	0.2,	0.8,	1.

// 	Efficiency 	
//-------------------------------------
// 1.                          x
// 0.9      x
// 
//        
// 0.6   x     
//       
// 0 x
//   0   5  10                  50 ns
//-------------------------------------
		
  	const static Double_t slope1 = (_Eff[1] - _Eff[0]) / (_Time[1] - _Time[0]);
  	const static Double_t slope2 = (_Eff[2] - _Eff[1]) / (_Time[2] - _Time[1]);
  	const static Double_t slope3 = (_Eff[3] - _Eff[2]) / (_Time[3] - _Time[2]);		
		
  	Double_t efficiency;
  	if(	delta > _Time[3]) return true;
  	else if(delta > _Time[2] && 	delta < _Time[3]) 	efficiency = _Eff[2] + (delta - _Time[2]) * slope3;
  	else if(delta > _Time[1] && 	delta < _Time[2]) 	efficiency = _Eff[1] + (delta - _Time[1]) * slope2;
  	else if(			delta < _Time[1]) 	efficiency = delta * slope1;		  

///cout<<"\n   eff="<<efficiency<<" "<<delta;
	
    	if(pRandom->Rndm() < efficiency) return true;
	
return false;	
}
//------------------------------------------------------------------------------------------------------------------------
/*Double_t		BmnDchHitProducer_exp::GetTShift(Double_t driftLength, Double_t wirePos, Double_t R, Double_t& L)
{

	driftLength = driftLength > 0 ? driftLength : -1.*driftLength;
	L = sqrt(WheelR_2 -  wirePos*wirePos); 	 // half wire length
	if(wirePos > -MinRadiusOfActiveVolume && wirePos < MinRadiusOfActiveVolume)	L = L - TMath::Abs(R);// two wires 
	else					L = L + R; // one wire 
        //cout<<"L = "<<L<<" "<<R<<endl;
	
//cout<<"\n t1="<<driftLength / gasDriftSpeed<<"  ("<<driftLength<<") t2="<<(wireLength + R) / wireDriftSpeed<<" L="<<wireLength<<" Y="<<R;
//cout<<"drift time (gas+wire) "<<driftLength/gasDriftSpeed<<" "<<L/wireDriftSpeed<<" "<<driftLength<<" "<<L<<endl;	
return driftLength / gasDriftSpeed + L / wireDriftSpeed;
}*/
//------------------------------------------------------------------------------------------------------------------------
Int_t		BmnDchHitProducer_exp::WireID(UInt_t uid, Double_t wirePos, Double_t R)
{
	//uid--; // uid [0,15]
	uid++; // uid [1,16]
	// wirePos: [-MaxRadiusOfActiveVolume,MaxRadiusOfActiveVolume]cm
	// tube R: MinRadiusOfActiveVolume

	if(wirePos > -MinRadiusOfActiveVolume && wirePos < MinRadiusOfActiveVolume) if(R > 0)return (int)(wirePos + 1000.*uid + 500); // two wires 

return (int)(wirePos + 1000.*uid);  // one wire 
}
//------------------------------------------------------------------------------------------------------------------------
void 			BmnDchHitProducer_exp::Exec(Int_t eventnum) 
{
      Int_t nDchHit0=fBmnDchDigitsArray->GetEntriesFast();
      if(nDchHit0==0)return;
      if(runPeriod==6&&!(strcmp(opt,"reconstruction")==0)){
        //BmnDchDigit* digit = NULL;
        //Int_t nDchHit=fBmnDchDigitsArray->GetEntriesFast();
        //cout<<"nDchHit = "<<nDchHit0<<endl;
        if(Iter==0&&fDoCheck)hDigits->Fill(nDchHit0);
        if(nDchHit0>65)return;
        //if(nDchHit0>30)return;
        //if(nDchHit0>200)return;
      } 
      eventNum=eventnum; 
      Bool_t oneTrackInTwoDch=false;
      //gObjectTable->Print();
      for (UShort_t idch = 0; idch < numChambers; idch++) {
        //if(eventNum%5000==0)cout<<"number of Dch = "<<idch+1<<endl;
  	//pHitCollection->Delete();
  	//pHitCollection->Clear();
        UShort_t planeUsedFast[twoNumLayers],allPlanesUsed=1;
        UShort_t        uid, uidLocal, wheel, gasgap, proj;

	for(UShort_t i = 0; i < numLayers; i++ ){ //loop over the DCH planes
         fMapOccup[i].clear();
        };

        //gObjectTable->Print();
        if(idch==0){ 
	 for(Int_t i = 0; i < twoNumLayers; i++ ) //loop over the DCH planes
         {
            planeUsedFast[i]=0;
         } 
         BmnDchDigit* digit = NULL;
         Int_t nDchHit=fBmnDchDigitsArray->GetEntriesFast();
	 for(Int_t i = 0; i < nDchHit; i++ )  // <---Loop over the DCH hits
	 {
             digit = (BmnDchDigit*) fBmnDchDigitsArray->At(i);
             uid=UShort_t(digit->GetPlane());
             uid=mapPlaneID(uid);
             if(planeUsedFast[uid]==0)planeUsedFast[uid]++;
         }
	 for(Int_t i = 0; i < twoNumLayers; i++ ) //loop over the DCH planes
         {
            allPlanesUsed*=planeUsedFast[i];
            if(checkDch)cout<<"allPlanesUsed1 = "<<allPlanesUsed<<" "<<planeUsedFast[i]<<" "<<i<<endl;
         } 
         if(checkDch)cout<<"allPlanesUsed = "<<allPlanesUsed<<endl;
         //if(nDchPoint==twoNumLayers){
         if(allPlanesUsed==1){
          oneTrackInTwoDch=true;}else{break;}
          if(checkDch)cout << " BmnDchHitProducer_exp::Exec(), Number of BmnDchDigits = " << nDchHit <<", event number = "<<eventNum<<", chamber = "<<idch<<endl;
        }
 
        if(strcmp(opt,"autocalibration")==0&&checkGraphs){	
         hXYZcombhits = new TGraph2D(); 
         hXYZcombhits->SetNameTitle("hXYZcombhits","Hits in DCH");
         TString str;
         str.Form("%u",eventNum);
         TString str2;
         str2.Form("%u",idch);
         TString hXYZcombhitsName = TString("hXYZcombhits") + str2 + TString("_") + str;
         hXYZcombhits->SetName(hXYZcombhitsName);
        }
 
        UInt_t           ijkl[numLayers];
	//fMapOccup.clear();
        //for (UShort_t j = 0; j < numChambers; j++) {
          for (UShort_t k = 0; k < numLayers; k++) {
             ijkl[k]=0;
             dchhitplane[k] = new DchHitPlane();
             planeUsed[k]=false;
             for (UInt_t j = 0; j < numWiresPerLayer; j++) {
              wireUsed[k][j]=false;
             }
          }
        //}
          //for (UShort_t k = numLayers; k < numLayers; k++) {
           //  dchhitplane[k] = new DchHitPlane();
          //}
          
        //TString str; 
        //str.Form("%u",eventNum);
        //TString nameTrCand=TString("DchTrackCandidates")+str;
       //gObjectTable->Print();
        if(idch==0){
           dchTrCand1 = new DchTrackCand();  
        }else if(idch==1){
           dchTrCand2 = new DchTrackCand();  
        }
       //gObjectTable->Print();
        //trackcand->SetName(nameTrCand);
        //trackcand->SetNameTitle(nameTrCand,"DCH track candidates, event number");
        
        if(eventNum%5000==0)cout<<"event number = "<<eventNum<<", number of Dch = "<<idch+1<<endl;

        Double_t        drifttimemin;
        Int_t           detID;
        UInt_t          hitWire;
        Int_t jjgr=0;
        cout.precision(5);



	//BmnDchHit 	*dchHit=NULL;
        BmnDchDigit* digit = NULL;

        Int_t nDchHit=fBmnDchDigitsArray->GetEntriesFast();
        if(checkDch)cout<<"Number of digits = "<<nDchHit<<endl;

        //if(checkDch){ 
           Bool_t xPlane2,yPlane0;
           Int_t xHitInd,yHitInd;
           Double_t xPlane2wirePos,xPlane2DCA,yPlane0wirePos,yPlane0DCA;
        //} 
        //if(checkDch)cout<<"Number of DCH corrected hits = "<<nDchHit<<endl;

	for(Int_t i = 0; i < nDchHit; i++ )  // <---Loop over the DCH hits
	{
                digit = (BmnDchDigit*) fBmnDchDigitsArray->At(i);
                uid=UShort_t(digit->GetPlane());
                //cout<<"idch = "<<idch<<endl;
                //cout<<"uid1 = "<<uid<<endl;
                uid=mapPlaneID(uid);
                //cout<<"uid2 = "<<uid<<endl;
                if(idch==0&&uid>=numLayers)continue;
                if(idch==1&&uid<numLayers)continue;
                uid<numLayers?uidLocal=uid:uidLocal=uid-numLayers;
                //cout<<"uidLocal = "<<uidLocal<<endl;
                //uidLocal%2==0?(hitWire=UInt_t(Int_t(dchHit->GetWirePosition())+Int_t(halfNumWiresPerLayer))):(hitWire=UInt_t(Int_t(dchHit->GetWirePosition()-0.5)+Int_t(halfNumWiresPerLayer)));
                hitWire=UInt_t(digit->GetWireNumber());
                //if(uidLocal==1||uidLocal==3||uidLocal==5||uidLocal==7)hitWire=hitWire+1;
                //if(Iter==0&&fDoCheck){
                 //if(uidLocal==0||uidLocal==1)hYwires[idch]->Fill(hitWire);
                //} 
                //if((uidLocal==2||uidLocal==3)&&hitWire>115)continue;  
                //if((uidLocal==2||uidLocal==3)&&idch==0&&hitWire>115)continue;  
                if(runPeriod<=3&&(uidLocal==0||uidLocal==1)&&idch==0&&hitWire>115)continue;  
                //if((uid==11)&&idch==1&&hitWire>104&&(runNumber>=358&&runNumber<=386))continue;  
                if(runPeriod<=3&&(uid==9)&&idch==1&&hitWire>104&&(runNumber>=358&&runNumber<=386))continue;  
                if(runPeriod==6&&hitWire>=240)continue;  
                //if(uidLocal==0||uidLocal==1)hitWire=240-hitWire;   
                //if(uidLocal==2||uidLocal==3)hitWire=240-hitWire;   
                //if(uidLocal==4||uidLocal==5)hitWire=240-hitWire;   
                //if(uidLocal==6||uidLocal==7)hitWire=240-hitWire;   
                gasgap = GetGasGap(uid); 
                if(checkDch)cout<<"GetWireNumber = "<<digit->GetWireNumber()<<", event number = "<<eventNum<<", uid = "<<uid<<", hit number = "<<i<<", DCH = "<<idch<<endl;
                //if(fDoCheck&&uidLocal%2==0)hWirePlaneChamber[idch][uidLocal/2]->Fill(Int_t(hitWire)-Int_t(halfNumWiresPerLayer));
                if(fDoCheck)hWireNumberInPlane[idch][uidLocal]->Fill(Int_t(hitWire)-Int_t(halfNumWiresPerLayer));
                if(fDoCheck)hWireCoordinateInPlane[idch][uidLocal]->Fill(wirePosition(gasgap,hitWire,GetProj(uidLocal),idch));
                if(checkDch)cout<<"hitWire = "<<hitWire<<" "<<wirePosition(gasgap,hitWire,GetProj(uidLocal),idch)<<", uidLocal = "<<uidLocal<<", idch = "<<idch<<endl;
                //cout<<" angle wire position correction = "<<dchHit->GetAngleCorrWirePos()<<endl;
                //cout<<"hitWire = "<<hitWire<<" "<<dchHit->GetAngleCorrWirePos()<<", uidLocal = "<<uidLocal<<", trackId = "<<dchHit->GetTrackID()<<endl; 
                uid<numLayers?detID=detIdDch1:detID=detIdDch2;
                //dchhitplane[uidLocal]->SetDchPlaneHit(ijkl[uidLocal]++, dchHit->GetWirePosition(),0.,dchHit->GetTrackID(),dchHit->GetDetectorID(),hitWire,fabs(dchHit->GetDrift()),dchHit->GetDriftTime(),dchHit->GetRefIndex());
                //if(rtCurve(digit->GetTime(),idch,GetProj(uidLocal))>1000.)continue;
                //dchhitplane[uidLocal]->SetDchPlaneHit(ijkl[uidLocal]++, wirePosition(gasgap,hitWire,GetProj(uidLocal)),0.,0,detID,hitWire,rtCurve(digit->GetTime(),idch),digit->GetTime()-ranmin[idch],i);
                //fMapOccup[uidLocal][hitWire]=ijkl[uidLocal];
                //if(checkDch)cout<<"fMapOccup: "<<uidLocal<<" "<<hitWire<<" "<<ijkl[uidLocal]<<" "<<eventNum<<endl;
                //cout<<"fMapOccup: "<<uidLocal<<" "<<hitWire<<" "<<endl;
                //if(rtCurve(digit->GetTime(),idch,GetProj(uidLocal))>9999.)continue; 
		if(wireUsed[uidLocal][hitWire]==false)wireUsed[uidLocal][hitWire]=true; 
                Double_t raddist=rtCurve(digit->GetTime(),idch,GetProj(uidLocal));
                if(raddist>9999.)continue;
                fMapOccup[uidLocal][hitWire]=ijkl[uidLocal];
                if(checkDch)cout<<"fMapOccup: "<<uidLocal<<" "<<hitWire<<" "<<ijkl[uidLocal]<<" "<<eventNum<<endl;
                dchhitplane[uidLocal]->SetDchPlaneHit(ijkl[uidLocal], wirePosition(gasgap,hitWire,GetProj(uidLocal),idch),0.,0,detID,hitWire,raddist,digit->GetTime()-ranmin[idch][GetProj(uidLocal)],i);
                //if(checkDch) cout<<"hity: "<<ijkl[uidLocal]<<" "<<wirePosition(gasgap,hitWire,GetProj(uidLocal))<<" "<<hitWire<<" "<<rtCurve(digit->GetTime(),idch,GetProj(uidLocal))<<" "<<idch<<" "<<GetProj(uidLocal)<<" "<<digit->GetTime()-ranmin[idch][GetProj(uidLocal)]<<" "<<uidLocal<<" "<<i<<" "<<wireUsed[uidLocal][hitWire]<<endl; 
                ijkl[uidLocal]++; 
                //cout<<dchHit->GetDriftTime()<<endl;
                //cout<<" hits "<<ijkl[uidLocal]<<" "<<idch<<" "<<uidLocal<<endl;
                //if(checkDch)cout<<" hit position (global)(x,y,z): "<<dchHit->GetX()<<", "<<dchHit->GetY()<<", "<<dchHit->GetZ()<<", "<<dchHit->GetTrackID()<<", "<<dchHit->GetRefIndex()<<endl;
        }
        //for (UShort_t j = 0; j < numChambers; j++) {
          for (UShort_t k = 0; k < numLayers; k++) {
             dchhitplane[k]->SetDchPlaneHitsNumber(ijkl[k],k);
             if(ijkl[k]>0)planeUsed[k]=true;
             if(checkDch)cout<<"number of hits in plane "<<k<<" of drift chamber "<<idch+1<<" = "<<ijkl[k]<<endl;
             //ijkl[j][k] = 0;
             if(fDoCheck&&Iter==0)hWireHitsVsPlanePerEvent[idch]->Fill(k,ijkl[k]);
          }
        //}

        topol[idch]=PlanesTopology(idch);
        if(checkDch)cout<<"topol = "<<topol[idch]<<" "<<idch<<endl;
        //if(topol<2){
	if(topol[idch]==0){
	//if(topol[idch]==0&&oneTrackInTwoDch){
        //if(topol==1){
          //dchTrCand = new DchTrackCand();
          //HitFinder(dchTrCand);
          HitFinder(idch,opt);
          //hitsFound=true;
          //dchTrCand->fDchTrackCand->Print();
        }else{
          if(checkDch)cout<<"Not enough hits in DCH"<<idch+1<<"! Hits not produced!"<<endl;
        }  

        
        if(strcmp(opt,"autocalibration")==0&&checkGraphs){	
         //hXYZcombhits->Write(); 
         gList.Add(hXYZcombhits); 
        }
	
 	//cout<<" "<<pHitCollection0->GetEntriesFast()<<"("<<hitID<<") hits created.\n";

        //for (UShort_t j = 0; j < numChambers; j++) {
        for (UShort_t k = 0; k < numLayers; k++) {
           delete dchhitplane[k];
           dchhitplane[k] = 0;
        }
        //}
        
       //gObjectTable->Print();
      }//cycle over chambers

      if(fDoCheck&&topol[0]==0&&topol[1]==0){
        if(Iter==0)hDch1WireHitsVsDch2WireHitsPerEvent->Fill(DCHwireHits[0],DCHwireHits[1]);
        hDch1TracksVsDch2TracksPerEvent->Fill(DCHtracks[0],DCHtracks[1]);
      } 

      if(oneTrackInTwoDch){
       //gObjectTable->Print();
       //if(strcmp(opt,"alignment")==0)dchTracks->DchAlignmentData(dchTrCand1,dchTrCand2,fDchTrCand1Align,fDchTrCand2Align,eventNum1track,C0,Iter,XYZ1,sumDeltaPhi);
       //gObjectTable->Print();
       if(strcmp(opt,"alignment")==0)dchTracks->DchAlignmentData(dchTrCand1,dchTrCand2,fDchTrCand1Align,fDchTrCand2Align,eventNum1track,C,XYZ1,sumDeltaPhi);
       //gObjectTable->Print();
       Double_t parFitL[4];
       Int_t fitErrorcode=10;
       //if(strcmp(opt,"autocalibration")==0)dchTracks->DchTrackMerger(dchTrCand1,dchTrCand2,C0,parFitL,fitErrorcode);
       //if(strcmp(opt,"autocalibration")==0)dchTracks->DchTrackMerger(dchTrCand1,dchTrCand2,C,parFitL,fitErrorcode);
       //cout<<"before DchTrackMerger "<<endl;
      
       if(!(strcmp(opt,"alignment")==0))dchTracks->DchTrackMerger(dchTrCand1,dchTrCand2,C,DCHtrackIDglob,eventNum,opt,pTrackCollection,covMat,parFitL,fitErrorcode);
       //cout<<"after DchTrackMerger "<<endl;
       //gObjectTable->Print();
       //if(strcmp(opt,"autocalibration")==0)dchTracks->DchGlobalAlignement(dchTrCand1,dchTrCand2,C0);
       if(strcmp(opt,"autocalibration")==0&&fitErrorcode==0&&geantCheck){
        GeantTracker(parFitL);
        Bool_t backward=true;
        if(fDoCheck)magfieldinteg->Fill(MagFieldIntegral(backward,parFitL));
       }
       delete dchTrCand1;
       delete dchTrCand2;
       //gObjectTable->Print();
      } 
}

//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::HitFinder(const UShort_t idch, const Option_t *opt_){ 

  Double_t x[numLayers_half],y[numLayers_half];
  Double_t xerror[numLayers_half],yerror[numLayers_half];
  UInt_t numPlaneHits[numLayers];
  UInt_t numPlaneHits2[numLayers];
  //UShort_t ijk=0;
  UShort_t ijk[numLayers_half]; //index of main plane in pair 
  UShort_t ijk2[numLayers_half]; //index of secondary plane in pair 
  UInt_t planeHits[numLayers_half]={0,0,0,0}; //
  UInt_t trcand=0;
  Int_t hitID=0;
  UInt_t counter=0;
  Double_t hittmp[2],hitx1[2],hity2[2],hitq3[2],hitq4[2];
  Double_t hitx1_2[2],hity2_2[2],hitq3_2[2],hitq4_2[2];
  Double_t x1,y2,q3,q4; 
  Double_t mindist,mindiff;
  TVector3 pos, dpos;	
  BmnDchHit *dchCombHit;
  UShort_t nhits[numLayers];
  UShort_t nhits2[numLayers];
  Int_t jjgr2=0;
  UInt_t sumHits=0;
  UShort_t nwires_nextplane; 
  UInt_t hw_nextplane[2];
  UShort_t nwires_nextplane2; 
  UInt_t hw_nextplane2[2];
  ScalarD* hitX[numLayers];ScalarI* trackId[numLayers];ScalarI* hitId[numLayers];ScalarI* detId[numLayers];ScalarUI* hitwire[numLayers];ScalarD* driftlen[numLayers];ScalarD* drifttim[numLayers];ScalarI* pointind[numLayers];
  ScalarD* hitX2[numLayers];ScalarI* trackId2[numLayers];ScalarI* hitId2[numLayers];ScalarI* detId2[numLayers];ScalarUI* hitwire2[numLayers];ScalarD* driftlen2[numLayers];ScalarD* drifttim_2[numLayers];ScalarI* pointind2[numLayers];
                                
  TVector2 &uOrtRef=uOrt;
  TVector2 &vOrtRef=vOrt;
  //TMatrixD C(4,4);
  const Double_t shiftOnWire=10.;
  
        //if((eventNum%100)==0)cout<<"eventNum0 = "<<eventNum<<endl; 
 
          for(UShort_t i = 0; i < numLayers; i+=2) {
             if(planeUsed[i]){
               ijk[i/2]=i;
               ijk2[i/2]=i+1;
               //cout<<"1ijk "<<ijk[i/2]<<" "<<ijk2[i/2]<<" "<<i<<endl;
             }else if ((!planeUsed[i])&&planeUsed[i+1]){
               ijk[i/2]=i+1;
               ijk2[i/2]=i;
               //cout<<"2ijk "<<ijk[i/2]<<" "<<ijk2[i/2]<<" "<<i<<endl;    
             }
          } 

          for(UShort_t i = 0; i < numLayers; i++) {
            nhits[i]=0;nhits2[i]=0;
          }
          numPlaneHits[ijk[0]]=dchhitplane[ijk[0]]->GetDchPlaneHitsNumber(ijk[0]); //hits in plane 0 
          numPlaneHits[ijk[1]]=dchhitplane[ijk[1]]->GetDchPlaneHitsNumber(ijk[1]); //hits in plane 2
          numPlaneHits[ijk[2]]=dchhitplane[ijk[2]]->GetDchPlaneHitsNumber(ijk[2]); //hits in plane 4 
          numPlaneHits[ijk[3]]=dchhitplane[ijk[3]]->GetDchPlaneHitsNumber(ijk[3]); //hits in plane 6 
          numPlaneHits2[ijk2[0]]=dchhitplane[ijk2[0]]->GetDchPlaneHitsNumber(ijk2[0]); //hits in plane 1 
          numPlaneHits2[ijk2[1]]=dchhitplane[ijk2[1]]->GetDchPlaneHitsNumber(ijk2[1]); //hits in plane 3
          numPlaneHits2[ijk2[2]]=dchhitplane[ijk2[2]]->GetDchPlaneHitsNumber(ijk2[2]); //hits in plane 5 
          numPlaneHits2[ijk2[3]]=dchhitplane[ijk2[3]]->GetDchPlaneHitsNumber(ijk2[3]); //hits in plane 7 
          for (UShort_t i = 0; i < numLayers_half; i++) {
           //if(checkDch)cout<<"numPlaneHits = "<<numPlaneHits[ijk[i]]<<" "<<numPlaneHits2[ijk2[i]]<<" "<<ijk[i]<<" "<<ijk2[i]<<endl;
           sumHits=sumHits+numPlaneHits[ijk[i]];
          }  
          //cout<<" numPlaneHits0 = "<<numPlaneHits0<<", numLayer = "<<k<<", drift chamber = "<<fDchNum<<endl;
          for (UInt_t i = 0; i < numPlaneHits[ijk[0]]; i++) {
              ReturnPointers(ijk[0],i,hitX[ijk[0]],trackId[ijk[0]],detId[ijk[0]],hitwire[ijk[0]],driftlen[ijk[0]],drifttim[ijk[0]],pointind[ijk[0]]);
              UInt_t hw0=hitwire[ijk[0]]->GetSV();
              Double_t driftdist0=driftlen[ijk[0]]->GetSV();
              Double_t drifttim0=drifttim[ijk[0]]->GetSV();
              if(checkDchHist)xdt[idch]->Fill(drifttim0);   
              //LRambiguity(ijk[0],hittmp,hitX[ijk[0]]->GetSV(),driftdist0,hw0,nhits[ijk[0]],drifttim0);
              if(checkDch)cout<<"hw0 = "<<hw0<<endl;
              LRambiguity(ijk[0],hitX[ijk[0]]->GetSV(),driftdist0,hw0,drifttim0,nhits[ijk[0]],hittmp,nwires_nextplane,hw_nextplane);
              for (UInt_t ii = 0; ii < nhits[ijk[0]]; ii++) {
                //hittmp[ii]=hittmp[ii]-0.5;//abcdef
                hitx1[ii]=hittmp[ii];
                if(checkDch)cout<<"hittmp0: "<<hittmp[ii]<<" "<<ii<<", hitX = "<<hitX[ijk[0]]->GetSV()<<", driftdist = "<<driftdist0<<endl;
                if(fDoCheck)hHitCoordinateInPlane[idch][0]->Fill(hitx1[ii]);
              }
              if(checkDchHist){
               for (UInt_t ii = 0; ii < nwires_nextplane; ii++) {
                if(checkDch)cout<<"wire_nextplane: "<<hw_nextplane[ii]<<endl;
               } 
               for (UInt_t ii = 0; ii <nwires_nextplane ; ii++) {
                UInt_t hitnum=fMapOccup[ijk2[0]][hw_nextplane[ii]]; 
                if(checkDch)cout<<"number of hits = "<<numPlaneHits2[ijk2[0]]<<", hitnum = "<<hitnum<<endl;
                ReturnPointers(ijk2[0],hitnum,hitX2[ijk2[0]],trackId2[ijk2[0]],detId2[ijk2[0]],hitwire2[ijk2[0]],driftlen2[ijk2[0]],drifttim_2[ijk2[0]],pointind2[ijk2[0]]);
                UInt_t hw1=hitwire2[ijk2[0]]->GetSV();
                Double_t driftdist1=driftlen2[ijk2[0]]->GetSV();
                Double_t drifttim1=drifttim_2[ijk2[0]]->GetSV();
                xdt[idch]->Fill(drifttim1);   
                xdtcorrel[idch]->Fill(drifttim0,drifttim1);   
                if(checkDch)cout<<"supplementary plane: "<<"plane index = "<<ijk2[0]<<", wire number = "<<hw1<<", hitX = "<<hitX2[ijk2[0]]->GetSV()<<", drift distance = "<<driftdist1<<endl;
                LRambiguity(ijk2[0],hitX2[ijk2[0]]->GetSV(),driftdist1,hw1,drifttim1,nhits2[ijk2[0]],hittmp,nwires_nextplane2,hw_nextplane2);
                Double_t mindiffX[nhits2[ijk2[0]]],mindistX[nhits2[ijk2[0]]];
                for (UInt_t iii = 0; iii < nhits2[ijk2[0]]; iii++) {
                  hitx1_2[iii]=hittmp[iii];
                  if(checkDch)cout<<"hittmp0_2: "<<hittmp[iii]<<" "<<iii<<", hitX = "<<hitX2[ijk2[0]]->GetSV()<<", driftdist = "<<driftdist1<<endl;
                  if(fDoCheck)hHitCoordinateInPlane[idch][1]->Fill(hitx1_2[iii]);
                  mindist=0.;mindiff=0.;
                  //UInt_t jjtmp;
                  for (UInt_t jj = 0; jj < nhits[ijk[0]]; jj++) {
                   if(jj==0){
                    //mindist=abs(hitx1[jj]-hitx1_2[iii]);
                    mindiff=hitx1[jj]-hitx1_2[iii];
                    mindist=abs(mindiff);
                   }else{
                    //mindist=min(mindist,abs(hity1[jj]-hity1_2[iii]));
                    mindiff = (abs(hitx1[jj]-hitx1_2[iii]) < mindist) ? (hitx1[jj]-hitx1_2[iii]) : mindiff;
                   }
                   //jjtmp==jj;
                  }
                  //cout<<"mindist_x = "<<mindist<<endl;
                  //cout<<"mindiff_x = "<<mindiff<<endl;
                  //hYplanesHitDist[idch]->Fill(mindist);
                  //if(nhits[ijk[0]]>0&&nhits2[ijk2[0]]>0){ 
                  //if(mindiff>0.)cout<<"positive "<<mindiff<<" hittmp0: "<<hitx1[ii]<<" "<<ii<<", hitX = "<<hitX[ijk[0]]->GetSV()<<", driftdist = "<<driftdist0<<", hittmp0_2: "<<hitx1_2[iii]<<" "<<iii<<", hitX = "<<hitX2[ijk2[0]]->GetSV()<<", driftdist = "<<driftdist1<<endl;
                  //if(mindiff<0.)cout<<"negative "<<mindiff<<" hittmp0: "<<hitx1[ii]<<" "<<ii<<", hitX = "<<hitX[ijk[0]]->GetSV()<<", driftdist = "<<driftdist0<<", hittmp0_2: "<<hitx1_2[iii]<<" "<<iii<<", hitX = "<<hitX2[ijk2[0]]->GetSV()<<", driftdist = "<<driftdist1<<endl;
                  mindiffX[iii]=mindiff; 
                  mindistX[iii]=abs(mindiff);
                }
                Int_t locmin=Int_t(TMath::LocMin(nhits2[ijk2[0]],mindistX));
                Double_t mindiffx=mindiffX[locmin];
                hXplanesHitDist[idch]->Fill(mindiffx);
                if(checkDch)cout<<"mindiffX = "<<mindiffx<<endl;
               }
              }
              //numPlaneHits[ijk[1]]=dchhitplane[ijk[1]]->GetDchPlaneHitsNumber(ijk[1]); //hits in plane 2
              for (UInt_t j = 0; j < numPlaneHits[ijk[1]]; j++) {
               ReturnPointers(ijk[1],j,hitX[ijk[1]],trackId[ijk[1]],detId[ijk[1]],hitwire[ijk[1]],driftlen[ijk[1]],drifttim[ijk[1]],pointind[ijk[1]]);
               UInt_t hw2=hitwire[ijk[1]]->GetSV();
               Double_t driftdist2=driftlen[ijk[1]]->GetSV();
               Double_t drifttim2=drifttim[ijk[1]]->GetSV();
               if(checkDchHist)ydt[idch]->Fill(drifttim2);   
               //LRambiguity(ijk[1],hittmp,hitX[ijk[1]]->GetSV(),driftdist2,hw2,nhits[ijk[1]],drifttim2);
               if(checkDch)cout<<"hw2 = "<<hw2<<endl;
               LRambiguity(ijk[1],hitX[ijk[1]]->GetSV(),driftdist2,hw2,drifttim2,nhits[ijk[1]],hittmp,nwires_nextplane,hw_nextplane);
               for (UInt_t ii = 0; ii < nhits[ijk[1]]; ii++) {
                 //hittmp[ii]=hittmp[ii]-0.5;//abcdef
                 hity2[ii]=hittmp[ii];
                 if(checkDch)cout<<"hittmp1: "<<hittmp[ii]<<" "<<ii<<", hitX = "<<hitX[ijk[1]]->GetSV()<<", driftdist = "<<driftdist2<<endl;
                 if(fDoCheck)hHitCoordinateInPlane[idch][2]->Fill(hity2[ii]);
               }
                if(checkDchHist){
                 for (UInt_t ii = 0; ii < nwires_nextplane; ii++) {
                  if(checkDch)cout<<"wire_nextplane: "<<hw_nextplane[ii]<<endl;
                 } 
                 for (UInt_t ii = 0; ii <nwires_nextplane ; ii++) {
                  UInt_t hitnum=fMapOccup[ijk2[1]][hw_nextplane[ii]]; 
                  if(checkDch)cout<<"number of hits = "<<numPlaneHits2[ijk2[1]]<<", hitnum = "<<hitnum<<endl;
                  //cout<<ijk2[1]<<endl;
                  ReturnPointers(ijk2[1],hitnum,hitX2[ijk2[1]],trackId2[ijk2[1]],detId2[ijk2[1]],hitwire2[ijk2[1]],driftlen2[ijk2[1]],drifttim_2[ijk2[1]],pointind2[ijk2[1]]);
                  UInt_t hw3=hitwire2[ijk2[1]]->GetSV();
                  Double_t driftdist3=driftlen2[ijk2[1]]->GetSV();
                  Double_t drifttim3=drifttim_2[ijk2[1]]->GetSV();
                  ydt[idch]->Fill(drifttim3);   
                  ydtcorrel[idch]->Fill(drifttim2,drifttim3);   
                  if(checkDch)cout<<"supplementary plane: "<<"plane index = "<<ijk2[1]<<", wire number = "<<hw3<<", hitX = "<<hitX2[ijk2[1]]->GetSV()<<", drift distance = "<<driftdist3<<endl;
                  LRambiguity(ijk2[1],hitX2[ijk2[1]]->GetSV(),driftdist3,hw3,drifttim3,nhits2[ijk2[1]],hittmp,nwires_nextplane2,hw_nextplane2);
                  Double_t mindiffY[nhits2[ijk2[1]]],mindistY[nhits2[ijk2[1]]];
                  for (UInt_t iii = 0; iii < nhits2[ijk2[1]]; iii++) {
                    hity2_2[iii]=hittmp[iii];
                    if(checkDch)cout<<"hittmp1_2: "<<hittmp[iii]<<" "<<iii<<", hitX = "<<hitX2[ijk2[1]]->GetSV()<<", driftdist = "<<driftdist3<<endl;
                    if(fDoCheck)hHitCoordinateInPlane[idch][3]->Fill(hity2_2[iii]);
                    mindist=0.;mindiff=0.;
                    for (UInt_t jj = 0; jj < nhits[ijk[1]]; jj++) {
                     if(jj==0){
                      //mindist=abs(hity2[jj]-hity2_2[iii]);
                      mindiff=hity2[jj]-hity2_2[iii];
                      mindist=abs(mindiff);
                     }else{
                      //mindist=min(mindist,abs(hity2[jj]-hity2_2[iii]));
                      mindiff = (abs(hity2[jj]-hity2_2[iii]) < mindist) ? (hity2[jj]-hity2_2[iii]) : mindiff; 
                     }
                    }
                  //cout<<"mindist_y = "<<mindist<<endl;
                  //cout<<"mindiff_y = "<<mindiff<<endl;
                  //hXplanesHitDist[idch]->Fill(mindist);
                  mindiffY[iii]=mindiff; 
                  mindistY[iii]=abs(mindiff);
                  }
                  Int_t locmin=Int_t(TMath::LocMin(nhits2[ijk2[1]],mindistY));
                  Double_t mindiffy=mindiffY[locmin];
                  hYplanesHitDist[idch]->Fill(mindiffy);
                  if(checkDch)cout<<"mindiffY = "<<mindiffy<<endl;
                 }
                }
               //cout<<"kDch = "<<ijk[2]<<endl;
               //numPlaneHits[ijk[2]]=dchhitplane[ijk[2]]->GetDchPlaneHitsNumber(ijk[2]); //hits in plane 4 
               for (UInt_t k = 0; k < numPlaneHits[ijk[2]]; k++) {
                ReturnPointers(ijk[2],k,hitX[ijk[2]],trackId[ijk[2]],detId[ijk[2]],hitwire[ijk[2]],driftlen[ijk[2]],drifttim[ijk[2]],pointind[ijk[2]]);
                UInt_t hw4=hitwire[ijk[2]]->GetSV();
                Double_t driftdist4=driftlen[ijk[2]]->GetSV();
                Double_t drifttim4=drifttim[ijk[2]]->GetSV();
                if(checkDchHist)udt[idch]->Fill(drifttim4);   
                //LRambiguity(ijk[2],hittmp,hitX[ijk[2]]->GetSV(),driftdist4,hw4,nhits[ijk[2]],drifttim4);
               if(checkDch)cout<<"hw4 = "<<hw4<<endl;
               LRambiguity(ijk[2],hitX[ijk[2]]->GetSV(),driftdist4,hw4,drifttim4,nhits[ijk[2]],hittmp,nwires_nextplane,hw_nextplane);
                for (UInt_t ii = 0; ii < nhits[ijk[2]]; ii++) {
                  hitq3[ii]=hittmp[ii]*sqrt2;
                  //if(checkDch)cout<<"hittmp2: "<<hittmp[ii]<<" "<<ii<<endl;
                  if(checkDch)cout<<"hittmp2: "<<hittmp[ii]<<" "<<ii<<", hitX = "<<hitX[ijk[2]]->GetSV()<<", driftdist = "<<driftdist4<<endl;
                  if(fDoCheck)hHitCoordinateInPlane[idch][4]->Fill(hittmp[ii]);
                }
                 if(checkDchHist){
                  for (UInt_t ii = 0; ii < nwires_nextplane; ii++) {
                   if(checkDch)cout<<"wire_nextplane: "<<hw_nextplane[ii]<<endl;
                  } 
                  for (UInt_t ii = 0; ii <nwires_nextplane ; ii++) {
                   UInt_t hitnum=fMapOccup[ijk2[2]][hw_nextplane[ii]]; 
                   //if(checkDch)cout<<"number of hits = "<<numPlaneHits2[ijk2[2]]<<", hitnum = "<<hitnum<<endl;
                   ReturnPointers(ijk2[2],hitnum,hitX2[ijk2[2]],trackId2[ijk2[2]],detId2[ijk2[2]],hitwire2[ijk2[2]],driftlen2[ijk2[2]],drifttim_2[ijk2[2]],pointind2[ijk2[2]]);
                   UInt_t hw5=hitwire2[ijk2[2]]->GetSV();
                   Double_t driftdist5=driftlen2[ijk2[2]]->GetSV();
                   Double_t drifttim5=drifttim_2[ijk2[2]]->GetSV();
                   udt[idch]->Fill(drifttim5);   
                   udtcorrel[idch]->Fill(drifttim4,drifttim5);   
                   if(checkDch)cout<<"supplementary plane: "<<"plane index = "<<ijk2[2]<<", wire number = "<<hw5<<", hitX = "<<hitX2[ijk2[2]]->GetSV()<<", drift distance = "<<driftdist5<<endl;
                   LRambiguity(ijk2[2],hitX2[ijk2[2]]->GetSV(),driftdist5,hw5,drifttim5,nhits2[ijk2[2]],hittmp,nwires_nextplane2,hw_nextplane2);
                   Double_t mindiffU[nhits2[ijk2[2]]],mindistU[nhits2[ijk2[2]]]; 
                   for (UInt_t iii = 0; iii < nhits2[ijk2[2]]; iii++) {
                     hitq3_2[iii]=hittmp[iii];
                     if(checkDch)cout<<"hittmp2_2: "<<hittmp[iii]<<" "<<iii<<", hitX = "<<hitX2[ijk2[2]]->GetSV()<<", driftdist = "<<driftdist5<<endl;
                     if(fDoCheck)hHitCoordinateInPlane[idch][5]->Fill(hitq3_2[iii]);
                     mindist=0.;mindiff=0.;
                     for (UInt_t jj = 0; jj < nhits[ijk[2]]; jj++) {
                      if(jj==0){
                       //mindist=abs(hitq3[jj]/sqrt2-hitq3_2[iii]);
                       //cout<<"mindist1 "<<mindist<<" "<<hitq3[jj]/sqrt2<<" "<<hitq3_2[iii]<<endl;
                       mindiff=hitq3[jj]/sqrt2-hitq3_2[iii];
                       mindist=abs(mindiff);
                      }else{
                       //mindist=min(mindist,abs(hitq3[jj]/sqrt2-hitq3_2[iii]));
                       //cout<<"mindist2 "<<mindist<<" "<<abs(hitq3[jj]/sqrt2-hitq3_2[iii])<<endl;
                       mindiff = (abs(hitq3[jj]/sqrt2-hitq3_2[iii]) < mindist) ? (hitq3[jj]/sqrt2-hitq3_2[iii]) : mindiff; 
                      }
                     }
                     //cout<<"mindist_u = "<<mindist<<endl;
                     //cout<<"mindiff_u = "<<mindiff<<endl;
                     //hUplanesHitDist[idch]->Fill(mindist);
                     mindiffU[iii]=mindiff; 
                     mindistU[iii]=abs(mindiff);
                   }
                   Int_t locmin=Int_t(TMath::LocMin(nhits2[ijk2[2]],mindistU));
                   Double_t mindiffu=mindiffU[locmin];
                   hUplanesHitDist[idch]->Fill(mindiffu);
                   if(checkDch)cout<<"mindiffU = "<<mindiffu<<endl;
                  }
                 }
                //numPlaneHits[ijk[3]]=dchhitplane[ijk[3]]->GetDchPlaneHitsNumber(ijk[3]); //hits in plane 6 
                 for (UInt_t l = 0; l < numPlaneHits[ijk[3]]; l++) {
                  ReturnPointers(ijk[3],l,hitX[ijk[3]],trackId[ijk[3]],detId[ijk[3]],hitwire[ijk[3]],driftlen[ijk[3]],drifttim[ijk[3]],pointind[ijk[3]]);
                  UInt_t hw6=hitwire[ijk[3]]->GetSV();
                  Double_t driftdist6=driftlen[ijk[3]]->GetSV();
                  Double_t drifttim6=drifttim[ijk[3]]->GetSV();
                  if(checkDchHist)vdt[idch]->Fill(drifttim6);   
                  //LRambiguity(ijk[3],hittmp,hitX[ijk[3]]->GetSV(),driftdist6,hw6,nhits[ijk[3]],drifttim6);
                  if(checkDch)cout<<"hw6 = "<<hw6<<endl;
                  LRambiguity(ijk[3],hitX[ijk[3]]->GetSV(),driftdist6,hw6,drifttim6,nhits[ijk[3]],hittmp,nwires_nextplane,hw_nextplane);
                  for (UInt_t ii = 0; ii < nhits[ijk[3]]; ii++) {
                    //hittmp[ii]=hittmp[ii]-0.5;//abcdef
                    hitq4[ii]=hittmp[ii]*sqrt2;
                    if(checkDch)cout<<"hittmp3: "<<hittmp[ii]<<" "<<ii<<", hitX = "<<hitX[ijk[3]]->GetSV()<<", driftdist = "<<driftdist6<<endl;
                    if(fDoCheck)hHitCoordinateInPlane[idch][6]->Fill(hittmp[ii]);
                  }
                  if(checkDchHist){
                   for (UInt_t ii = 0; ii < nwires_nextplane; ii++) {
                    if(checkDch)cout<<"wire_nextplane: "<<hw_nextplane[ii]<<endl;
                   } 
                   for (UInt_t ii = 0; ii <nwires_nextplane ; ii++) {
                    UInt_t hitnum=fMapOccup[ijk2[3]][hw_nextplane[ii]]; 
                    if(checkDch)cout<<"number of hits = "<<numPlaneHits2[ijk2[3]]<<", hitnum = "<<hitnum<<endl;
                    ReturnPointers(ijk2[3],hitnum,hitX2[ijk2[3]],trackId2[ijk2[3]],detId2[ijk2[3]],hitwire2[ijk2[3]],driftlen2[ijk2[3]],drifttim_2[ijk2[3]],pointind2[ijk2[3]]);
                    UInt_t hw7=hitwire2[ijk2[3]]->GetSV();
                    Double_t driftdist7=driftlen2[ijk2[3]]->GetSV();
                    Double_t drifttim7=drifttim_2[ijk2[3]]->GetSV();
                    vdt[idch]->Fill(drifttim7);   
                    vdtcorrel[idch]->Fill(drifttim6,drifttim7);   
                    if(checkDch)cout<<"supplementary plane: "<<"plane index = "<<ijk2[3]<<", wire number = "<<hw7<<", hitX = "<<hitX2[ijk2[3]]->GetSV()<<", drift distance = "<<driftdist7<<endl;
                    LRambiguity(ijk2[3],hitX2[ijk2[3]]->GetSV(),driftdist7,hw7,drifttim7,nhits2[ijk2[3]],hittmp,nwires_nextplane2,hw_nextplane2);
                    Double_t mindiffV[nhits2[ijk2[3]]],mindistV[nhits2[ijk2[3]]];
                    for (UInt_t iii = 0; iii < nhits2[ijk2[3]]; iii++) {
                      hitq4_2[iii]=hittmp[iii];
                      if(checkDch)cout<<"hittmp3_2: "<<hittmp[iii]<<" "<<iii<<", hitX = "<<hitX2[ijk2[3]]->GetSV()<<", driftdist = "<<driftdist7<<endl;
                      if(fDoCheck)hHitCoordinateInPlane[idch][7]->Fill(hitq4_2[iii]);
                      mindist=0.;mindiff=0.;
                      for (UInt_t jj = 0; jj < nhits[ijk[3]]; jj++) {
                       if(jj==0){
                        //mindist=abs(hitq4[jj]/sqrt2-hitq4_2[iii]);
                        //cout<<"mindist1 "<<mindist<<" "<<hitq4[jj]/sqrt2<<" "<<hitq4_2[iii]<<endl;
                        mindiff=hitq4[jj]/sqrt2-hitq4_2[iii];
                        mindist=abs(mindiff);
                       }else{
                        //mindist=min(mindist,abs(hitq4[jj]/sqrt2-hitq4_2[iii]));
                        //cout<<"mindist2 "<<mindist<<" "<<abs(hitq4[jj]/sqrt2-hitq4_2[iii])<<endl;
                        mindiff = (abs(hitq4[jj]/sqrt2-hitq4_2[iii]) < mindist) ? (hitq4[jj]/sqrt2-hitq4_2[iii]) : mindiff; 
                       }
                      }
                      //cout<<"mindist_v = "<<mindist<<endl;
                      //cout<<"mindiff_v = "<<mindiff<<endl;
                      //hVplanesHitDist[idch]->Fill(mindist);
                      mindiffV[iii]=mindiff; 
                      mindistV[iii]=abs(mindiff);
                    }
                    Int_t locmin=Int_t(TMath::LocMin(nhits2[ijk2[3]],mindistV));
                    Double_t mindiffv=mindiffV[locmin];
                    hVplanesHitDist[idch]->Fill(mindiffv);
                    if(checkDch)cout<<"mindiffV = "<<mindiffv<<endl;
                   }
                  }
                            //planeHits[jk]++; 
                  for (UInt_t m = 0; m < nhits[ijk[0]]; m++) {
                   x1=hitx1[m];
                   for (UInt_t n = 0; n < nhits[ijk[1]]; n++) {
                    y2=hity2[n];
                    for (UInt_t mm = 0; mm < nhits[ijk[2]]; mm++) {
                     q3=hitq3[mm];
                     for (UInt_t nn = 0; nn < nhits[ijk[3]]; nn++) {
                      q4=hitq4[nn];
                      //if(checkDch){
                       if(checkDch)cout<<"x1,y2,q3,q4 = "<<x1<<", "<<y2<<", "<<q3<<", "<<q4<<endl;
                       if(checkDch)cout<<-x1-q3+y2<<" "<<q3+q4-2.*y2<<" "<<-x1+q4-y2<<endl;
                       //xcoor[idch]->Fill(x1); 
                       //ycoor[idch]->Fill(y2); 
                       //ucoor[idch]->Fill(q3/sqrt2); 
                       //vcoor[idch]->Fill(q4/sqrt2); 
                       /*x1=x1-0.5; 
                       y2=y2-0.5; 
                       q3=q3+0.5*sqrt(2.); 
                       q4=q4-0.5*sqrt(2.);*/ 
                       if(checkDchHist){ 
                        difxu[idch]->Fill(-x1-q3+y2);
                        //difxu[idch]->Fill(-y2-q3+x1);
                        difuv[idch]->Fill(q3+q4-2.*y2);
                        //difuv[idch]->Fill(q3+q4-2.*x1);
                        difxv[idch]->Fill(-x1+q4-y2);
                        //difxv[idch]->Fill(-y2+q4-x1);
                        uxvxcorrel[idch]->Fill(q3-y2,y2-q4);   
                        //uxvxcorrel[idch]->Fill(q3-x1,x1-q4);   
                        uvcorrel[idch]->Fill(q3/sqrt2,q4/sqrt2);   
                        //uxvxycorrel[idch]->Fill(q3,-q4,y2);  
                        //uxvxycorrel[idch]->Fill(q3,-q4,x1);  
                       } 
                      //}
                      //CoordinateFinder(x2,y1,tg3,q3,tg4,q4,ijk,x,y);
                      //CoordinateFinder2(x1,y2,tg3,q3,tg4,q4,ijk,x,y);
                      CoordinateFinder3(x1,y2,tg3,q3,tg4,q4,ijk,x,y);
                      //CoordinateCorrection(x,y) 
                      /*for (UShort_t ik = 0; ik < numLayers_half; ik++) {
                       x[ik]=-x[ik]; 
                       y[ik]=-y[ik]; 
                      }*/
                      //cout<<"------------------------------------------------------"<<endl; 
                       Bool_t radcond=true;
                       Double_t radDist=sqrt(pow(x[1]-x[0],2.)+pow(y[1]-y[0],2.));
                       if(radDist>radialRange[runPeriod-1])radcond=false;
                       if(checkDch)cout<<"radcond = "<<radcond<<" "<<sqrt(pow(x[1]-x[0],2.)+pow(y[1]-y[0],2.))<<endl;
                       if(fDoCheck&&Iter==0){
                        hRadialRangeAll[idch]->Fill(radDist);
                        if(numPlaneHits[ijk[0]]==1&&numPlaneHits[ijk[1]]==1&&numPlaneHits[ijk[2]]==1&&numPlaneHits[ijk[3]]==1)hRadialRange[idch]->Fill(radDist);
                       }
                       if(radcond){
                        for (UShort_t jk = 0; jk < numLayers_half; jk++) {
                         if(sqrt(pow(x[jk],2.)+pow(y[jk],2.))>MinRadiusOfActiveVolume&&sqrt(pow(x[jk],2.)+pow(y[jk],2.))<MaxRadiusOfActiveVolume){

                         /*if(checkDchHist){
                          if(checkDch)cout<<"x1,y2,q3,q4 2 = "<<x1<<", "<<y2<<", "<<q3<<", "<<q4<<endl;
                          difxu[idch]->Fill(-y2-q3+x1);
                          difuv[idch]->Fill(q3+q4-2.*x1);
                          difxv[idch]->Fill(-y2+q4-x1);
                          uxvxcorrel[idch]->Fill(q3-x1,x1-q4);   
                          uvcorrel[idch]->Fill(q3/sqrt2,q4/sqrt2);   
                         }*/

	                  if(fDoCheck){
                            hXYhit[idch][jk]->Fill(x[jk],y[jk]);
                            hXhit[idch][jk]->Fill(x[jk]);
                            hYhit[idch][jk]->Fill(y[jk]);
                            hZhit[idch][jk]->Fill(zLayer[idch][ijk[jk]]);
                          }
                          /*if(checkGraphs){
                            hXYZcombhits->SetPoint(jjgr2++,zLayer[idch][ijk[jk]],x[jk]+detXshift[idch],y[jk]+detYshift[idch]); 
                            if(jk==3)ExtrapToDch(x,y,zLayer,idch,ijk,jjgr2);
                          }*/ 
                          if(checkDch){
                           cout<<"hits(x,y,z): "<<x[jk]<<", "<<y[jk]<<", "<<zLayer[idch][ijk[jk]]<<", plane number = "<<jk<<", chamber = "<<idch+1<<", hit number = "<<hitID<<", track cand. = "<<trcand<<", event = "<<eventNum<<endl;
                           /*if(jk==3){
                            cout<<"nhits0 = "<<nhits[0]<<", nhits2 = "<<nhits[2]<<", nhits4 = "<<nhits[4]<<", nhits6 = "<<nhits[6]<<endl; 
                            cout<<"------------------------------------------------------"<<endl; 
                           }*/
                          }
                          if(!(strcmp(opt,"reconstruction")==0))hitID++;
                          if(jk==3){
                           //cout<<"trcand: "<<trcand<<" "<<trCand<<" "<<eventNum<<endl;
                           //cout<<"angles before rotation, xz,yz: "<<TMath::RadToDeg()*atan((x[1]-x[0])/(zLayer[idch][ijk[1]]-zLayer[idch][ijk[0]]))<<", "<<TMath::RadToDeg()*atan((y[1]-y[0])/(zLayer[idch][ijk[1]]-zLayer[idch][ijk[0]]))<<", "<<idch+1<<endl;
                           TVector2 uvec(0.,hitX[ijk[2]]->GetSV()),uvecRot; 
                           uvecRot=uvec.Rotate(angleLayerRad[4]);
                           TVector2 vvec(0.,hitX[ijk[3]]->GetSV()),vvecRot; 
                           vvecRot=vvec.Rotate(angleLayerRad[6]);
                           /*cout<<"uvec components(x,y): "<<uvec.X()<<" "<<uvec.Y()<<endl;
                           cout<<"uvecRot components(x,y): "<<uvecRot.X()<<" "<<uvecRot.Y()<<endl;
                           cout<<"vvec components(x,y): "<<vvec.X()<<" "<<vvec.Y()<<endl;
                           cout<<"vvecRot components(x,y): "<<vvecRot.X()<<" "<<vvecRot.Y()<<endl;*/
                           TVector2 uvec2(shiftOnWire,hitX[ijk[2]]->GetSV()),uvec2Rot; 
                           uvec2Rot=uvec2.Rotate(angleLayerRad[4]);
                           TVector2 vvec2(shiftOnWire,hitX[ijk[3]]->GetSV()),vvec2Rot; 
                           vvec2Rot=vvec2.Rotate(angleLayerRad[6]);
                           /*cout<<"uvec2 components(x,y): "<<uvec2.X()<<" "<<uvec2.Y()<<endl;
                           cout<<"uvec2Rot components(x,y): "<<uvec2Rot.X()<<" "<<uvec2Rot.Y()<<endl;
                           cout<<"vvec2 components(x,y): "<<vvec2.X()<<" "<<vvec2.Y()<<endl;
                           cout<<"vvec2Rot components(x,y): "<<vvec2Rot.X()<<" "<<vvec2Rot.Y()<<endl;*/
                           TVector2 xzCenterLocal1(DCH_ZlayerPos_local[ijk[0]],0.),xzCenterLocal1rot; 
                           xzCenterLocal1rot=xzCenterLocal1.Rotate(-angleXZ);
                           //cout<<"rotated center, plane 1 (x):"<<xzCenterLocal1rot.X()<<" "<<xzCenterLocal1rot.Y()<<endl;
                           TVector2 xzHit1(x[0],0.),xzHit1rot;
                           xzHit1rot=xzHit1.Rotate(angleXZ);
                           //XYZPoint xyzHit1(x[0]+detXshift[idch],y[0]+detYshift[idch],zLayer[idch][ijk[0]]);
                           //XYZPoint xyzHit1(xzHit1rot.X()+detXshift[idch],y[0]+detYshift[idch],zLayer[idch][ijk[0]]+xzHit1rot.Y());
                           XYZPoint xyzHit1(xzHit1rot.X()+xzCenterLocal1rot.Y()+detXshift[idch],y[0]+detYshift[idch],zCenter[idch]+xzCenterLocal1rot.X()+xzHit1rot.Y());
                           TVector2 xzCenterLocal2(DCH_ZlayerPos_local[ijk[1]],0.),xzCenterLocal2rot; 
                           xzCenterLocal2rot=xzCenterLocal2.Rotate(-angleXZ);
                           //cout<<"rotated center, plane 2 (y):"<<xzCenterLocal2rot.X()<<" "<<xzCenterLocal2rot.Y()<<endl;
                           TVector2 xzHit2(x[1],0.),xzHit2rot;
                           xzHit2rot=xzHit2.Rotate(angleXZ);
                           //XYZPoint xyzHit2(x[1]+detXshift[idch],y[1]+detYshift[idch],zLayer[idch][ijk[1]]);
                           //XYZPoint xyzHit2(xzHit2rot.X()+detXshift[idch],y[1]+detYshift[idch],zLayer[idch][ijk[1]]+xzHit2rot.Y());
                           XYZPoint xyzHit2(xzHit2rot.X()+xzCenterLocal2rot.Y()+detXshift[idch],y[1]+detYshift[idch],zCenter[idch]+xzCenterLocal2rot.X()+xzHit2rot.Y());
                           //cout<<"original point u: "<<x[2]<<" "<<y[2]<<" "<<zLayer[idch][ijk[2]]<<endl;
                           TVector2 xzCenterLocal3(DCH_ZlayerPos_local[ijk[2]],0.),xzCenterLocal3rot; 
                           xzCenterLocal3rot=xzCenterLocal3.Rotate(-angleXZ);
                           //cout<<"rotated center, plane 3 (u):"<<xzCenterLocal3rot.X()<<" "<<xzCenterLocal3rot.Y()<<endl;
                           TVector2 xzHit3(x[2],0.),xzHit3rot;
                           xzHit3rot=xzHit3.Rotate(angleXZ);
                           //XYZPoint xyzHit3(x[2]+detXshift[idch],y[2]+detYshift[idch],zLayer[idch][ijk[2]]);
                           //XYZPoint xyzHit3(xzHit3rot.X()+detXshift[idch],y[2]+detYshift[idch],zLayer[idch][ijk[2]]+xzHit3rot.Y());
                           XYZPoint xyzHit3(xzHit3rot.X()+xzCenterLocal3rot.Y()+detXshift[idch],y[2]+detYshift[idch],zCenter[idch]+xzCenterLocal3rot.X()+xzHit3rot.Y());
                           /*cout<<"rotated point u: "<<xyzHit3.X()<<" "<<xyzHit3.Y()<<" "<<xyzHit3.Z()<<endl;
                           cout<<"original point v: "<<x[3]<<" "<<y[3]<<" "<<zLayer[idch][ijk[3]]<<endl;*/
                           TVector2 xzCenterLocal4(DCH_ZlayerPos_local[ijk[3]],0.),xzCenterLocal4rot; 
                           xzCenterLocal4rot=xzCenterLocal4.Rotate(-angleXZ);
                           //cout<<"rotated center, plane 4 (v):"<<xzCenterLocal4rot.X()<<" "<<xzCenterLocal4rot.Y()<<endl;
                           TVector2 xzHit4(x[3],0.),xzHit4rot;
                           xzHit4rot=xzHit4.Rotate(angleXZ);
                           //XYZPoint xyzHit4(x[3]+detXshift[idch],y[3]+detYshift[idch],zLayer[idch][ijk[3]]);
                           //XYZPoint xyzHit4(xzHit4rot.X()+detXshift[idch],y[3]+detYshift[idch],zLayer[idch][ijk[3]]+xzHit4rot.Y());
                           XYZPoint xyzHit4(xzHit4rot.X()+xzCenterLocal4rot.Y()+detXshift[idch],y[3]+detYshift[idch],zCenter[idch]+xzCenterLocal4rot.X()+xzHit4rot.Y());
                           //cout<<"rotated point v: "<<xyzHit4.X()<<" "<<xyzHit4.Y()<<" "<<xyzHit4.Z()<<endl;
                           TVector2 xzWire1(hitX[ijk[0]]->GetSV(),0.),xzWire1rot;
                           xzWire1rot=xzWire1.Rotate(angleXZ);
                           //XYZPoint xyzWire1(hitX[ijk[0]]->GetSV()+detXshift[idch],detYshift[idch],zLayer[idch][ijk[0]]);
                           //XYZPoint xyzWire1(xzWire1rot.X()+detXshift[idch],detYshift[idch],zLayer[idch][ijk[0]]+xzWire1rot.Y());
                           XYZPoint xyzWire1(xzWire1rot.X()+xzCenterLocal1rot.Y()+detXshift[idch],detYshift[idch],zCenter[idch]+xzCenterLocal1rot.X()+xzWire1rot.Y());
                           //cout<<"wire1 point1: "<<xyzWire1.X()<<" "<<xyzWire1.Y()<<" "<<xyzWire1.Z()<<endl;
                           TVector2 xzWire2(0.,0.),xzWire2rot;
                           xzWire2rot=xzWire2.Rotate(angleXZ);
                           //XYZPoint xyzWire2(detXshift[idch],hitX[ijk[1]]->GetSV()+detYshift[idch],zLayer[idch][ijk[1]]);
                           //XYZPoint xyzWire2(xzWire2rot.X()+detXshift[idch],hitX[ijk[1]]->GetSV()+detYshift[idch],zLayer[idch][ijk[1]]+xzWire2rot.Y());
                           XYZPoint xyzWire2(xzWire2rot.X()+xzCenterLocal2rot.Y()+detXshift[idch],hitX[ijk[1]]->GetSV()+detYshift[idch],zCenter[idch]+xzCenterLocal2rot.X()+xzWire2rot.Y());
                           //cout<<"wire2 point1: "<<xyzWire2.X()<<" "<<xyzWire2.Y()<<" "<<xyzWire2.Z()<<endl;
                           //cout<<"original wire point1 u: "<<uvecRot.X()<<" "<<uvecRot.Y()<<" "<<zLayer[idch][ijk[2]]<<endl;
                           TVector2 xzWire3(uvecRot.X(),0.),xzWire3rot;
                           xzWire3rot=xzWire3.Rotate(angleXZ);
                           //XYZPoint xyzWire3(uvecRot.X()+detXshift[idch],uvecRot.Y()+detYshift[idch],zLayer[idch][ijk[2]]);
                           //XYZPoint xyzWire3(xzWire3rot.X()+detXshift[idch],uvecRot.Y()+detYshift[idch],zLayer[idch][ijk[2]]+xzWire3rot.Y());
                           XYZPoint xyzWire3(xzWire3rot.X()+xzCenterLocal3rot.Y()+detXshift[idch],uvecRot.Y()+detYshift[idch],zCenter[idch]+xzCenterLocal3rot.X()+xzWire3rot.Y());
                           /*cout<<"rotated wire point1 u: "<<xyzWire3.X()<<" "<<xyzWire3.Y()<<" "<<xyzWire3.Z()<<endl;
                           cout<<"original wire point1 v: "<<vvecRot.X()<<" "<<vvecRot.Y()<<" "<<zLayer[idch][ijk[3]]<<endl;*/
                           TVector2 xzWire4(vvecRot.X(),0.),xzWire4rot;
                           xzWire4rot=xzWire4.Rotate(angleXZ);
                           //XYZPoint xyzWire4(vvecRot.X()+detXshift[idch],vvecRot.Y()+detYshift[idch],zLayer[idch][ijk[3]]);
                           //XYZPoint xyzWire4(xzWire4rot.X()+detXshift[idch],vvecRot.Y()+detYshift[idch],zLayer[idch][ijk[3]]+xzWire4rot.Y());
                           XYZPoint xyzWire4(xzWire4rot.X()+xzCenterLocal4rot.Y()+detXshift[idch],vvecRot.Y()+detYshift[idch],zCenter[idch]+xzCenterLocal4rot.X()+xzWire4rot.Y());
                           //cout<<"rotated wire point1 v: "<<xyzWire4.X()<<" "<<xyzWire4.Y()<<" "<<xyzWire4.Z()<<endl;
                           //if(fabs(x[0]-hitX[ijk[0]]->GetSV())>0.51)cout<<"driftdist check1: "<<x[0]<<" "<<y[0]<<" "<<0<<" "<<hitX[ijk[0]]->GetSV()<<endl;
                           //if(fabs(y[1]-hitX[ijk[1]]->GetSV())>0.51)cout<<"driftdist check2: "<<x[1]<<" "<<y[1]<<" "<<hitX[ijk[1]]->GetSV()<<" "<<0<<endl;
                           TVector2 xzWire1_2(hitX[ijk[0]]->GetSV(),0.),xzWire1_2rot;
                           xzWire1_2rot=xzWire1_2.Rotate(angleXZ);
                           //XYZPoint xyzWire1_2(detXshift[idch]+shiftOnWire,hitX[ijk[0]]->GetSV()+detYshift[idch],zLayer[idch][ijk[0]]);
                           //XYZPoint xyzWire1_2(xzWire1_2rot.X()+detXshift[idch],detYshift[idch]+shiftOnWire,zLayer[idch][ijk[0]]+xzWire1_2rot.Y());
                           XYZPoint xyzWire1_2(xzWire1_2rot.X()+xzCenterLocal1rot.Y()+detXshift[idch],detYshift[idch]+shiftOnWire,zCenter[idch]+xzCenterLocal1rot.X()+xzWire1_2rot.Y());
                           //cout<<"wire1 point2: "<<xyzWire1_2.X()<<" "<<xyzWire1_2.Y()<<" "<<xyzWire1_2.Z()<<endl;
                           //XYZPoint xyzWire1_2(hitX[ijk[0]]->GetSV()+detXshift[idch],detYshift[idch]+shiftOnWire,zLayer[idch][ijk[0]]);
                           TVector2 xzWire2_2(shiftOnWire,0.),xzWire2_2rot;
                           xzWire2_2rot=xzWire2_2.Rotate(angleXZ);
                           //XYZPoint xyzWire2_2(hitX[ijk[1]]->GetSV()+detXshift[idch],detYshift[idch]+shiftOnWire,zLayer[idch][ijk[1]]);
                           //XYZPoint xyzWire2_2(xzWire2_2rot.X()+detXshift[idch],hitX[ijk[1]]->GetSV()+detYshift[idch],zLayer[idch][ijk[1]]+xzWire2_2rot.Y());
                           XYZPoint xyzWire2_2(xzWire2_2rot.X()+xzCenterLocal2rot.Y()+detXshift[idch],hitX[ijk[1]]->GetSV()+detYshift[idch],zCenter[idch]+xzCenterLocal2rot.X()+xzWire2_2rot.Y());
                           //cout<<"wire2 point2: "<<xyzWire2_2.X()<<" "<<xyzWire2_2.Y()<<" "<<xyzWire2_2.Z()<<endl;
                           //XYZPoint xyzWire2_2(detXshift[idch]+shiftOnWire,hitX[ijk[1]]->GetSV()+detYshift[idch],zLayer[idch][ijk[1]]);
                           //cout<<"original wire point2 u: "<<uvec2Rot.X()<<" "<<uvec2Rot.Y()<<" "<<zLayer[idch][ijk[2]]<<endl;
                           TVector2 xzWire3_2(uvec2Rot.X(),0.),xzWire3_2rot;
                           xzWire3_2rot=xzWire3_2.Rotate(angleXZ);
                           //XYZPoint xyzWire3_2(xzWire3_2rot.X()+detXshift[idch],uvec2Rot.Y()+detYshift[idch],zLayer[idch][ijk[2]]+xzWire3_2rot.Y());
                           XYZPoint xyzWire3_2(xzWire3_2rot.X()+xzCenterLocal3rot.Y()+detXshift[idch],uvec2Rot.Y()+detYshift[idch],zCenter[idch]+xzCenterLocal3rot.X()+xzWire3_2rot.Y());
                           //cout<<"rotated wire point2 u: "<<xyzWire3_2.X()<<" "<<xyzWire3_2.Y()<<" "<<xyzWire3_2.Z()<<endl;
                           //XYZPoint xyzWire3_2(uvec2Rot.X()+detXshift[idch],uvec2Rot.Y()+detYshift[idch],zLayer[idch][ijk[2]]);
                           //cout<<"original wire point2 v: "<<vvec2Rot.X()<<" "<<vvec2Rot.Y()<<" "<<zLayer[idch][ijk[3]]<<endl;
                           TVector2 xzWire4_2(vvec2Rot.X(),0.),xzWire4_2rot;
                           xzWire4_2rot=xzWire4_2.Rotate(angleXZ);
                           //XYZPoint xyzWire4_2(vvec2Rot.X()+detXshift[idch],vvec2Rot.Y()+detYshift[idch],zLayer[idch][ijk[3]]);
                           //XYZPoint xyzWire4_2(xzWire4_2rot.X()+detXshift[idch],vvec2Rot.Y()+detYshift[idch],zLayer[idch][ijk[3]]+xzWire4_2rot.Y());
                           XYZPoint xyzWire4_2(xzWire4_2rot.X()+xzCenterLocal4rot.Y()+detXshift[idch],vvec2Rot.Y()+detYshift[idch],zCenter[idch]+xzCenterLocal4rot.X()+xzWire4_2rot.Y());
                           //cout<<"rotated wire point2 v: "<<xyzWire4_2.X()<<" "<<xyzWire4_2.Y()<<" "<<xyzWire4_2.Z()<<endl;
                           /*Double_t vecmag01=shiftOnWire;
                           Double_t vecmag02=shiftOnWire;
                           Double_t vecmag03=sqrt(pow(uvec2Rot.X()-uvecRot.X(),2)+pow(uvec2Rot.Y()-uvecRot.Y(),2));
                           Double_t vecmag04=sqrt(pow(uvec2Rot.X()-uvecRot.X(),2)+pow(vvec2Rot.Y()-vvecRot.Y(),2));
                           cout<<"distance in first plane = "<<PointLineDistance(x[0],y[0],zLayer[idch][ijk[0]],hitX[ijk[0]]->GetSV(),0.,zLayer[idch][ijk[0]],0.,1.,0.)<<endl; 
                           cout<<"distance in second plane = "<<PointLineDistance(x[1],y[1],zLayer[idch][ijk[1]],0.,hitX[ijk[1]]->GetSV(),zLayer[idch][ijk[1]],1.,0.,0.)<<endl; 
                           cout<<"distance in third plane = "<<PointLineDistance(x[2],y[2],zLayer[idch][ijk[2]],uvecRot.X(),uvecRot.Y(),zLayer[idch][ijk[2]],(uvec2Rot.X()-uvecRot.X())/vecmag03,(uvec2Rot.Y()-uvecRot.Y())/vecmag03,0.)<<endl; 
                           cout<<"distance in fourth plane = "<<PointLineDistance(x[3],y[3],zLayer[idch][ijk[3]],vvecRot.X(),vvecRot.Y(),zLayer[idch][ijk[3]],(vvec2Rot.X()-vvecRot.X())/vecmag04,(vvec2Rot.Y()-vvecRot.Y())/vecmag04,0.)<<endl; 
                           cout<<"original vector magnitudes (1,2,3,4): "<<vecmag01<<" "<<vecmag02<<" "<<vecmag03<<" "<<vecmag04<<" "<<endl;  
                           Double_t vecmag1=sqrt(pow(xyzWire1_2.X()-xyzWire1.X(),2)+pow(xyzWire1_2.Y()-xyzWire1.Y(),2)+pow(xyzWire1_2.Z()-xyzWire1.Z(),2));
                           Double_t vecmag2=sqrt(pow(xyzWire2_2.X()-xyzWire2.X(),2)+pow(xyzWire2_2.Y()-xyzWire2.Y(),2)+pow(xyzWire2_2.Z()-xyzWire2.Z(),2));
                           Double_t vecmag3=sqrt(pow(xyzWire3_2.X()-xyzWire3.X(),2)+pow(xyzWire3_2.Y()-xyzWire3.Y(),2)+pow(xyzWire3_2.Z()-xyzWire3.Z(),2));
                           Double_t vecmag4=sqrt(pow(xyzWire4_2.X()-xyzWire4.X(),2)+pow(xyzWire4_2.Y()-xyzWire4.Y(),2)+pow(xyzWire4_2.Z()-xyzWire4.Z(),2));
                           cout<<"rotated vector magnitudes (1,2,3,4): "<<vecmag1<<" "<<vecmag2<<" "<<vecmag3<<" "<<vecmag4<<" "<<endl;  
                           cout<<"distance in first rotated plane = "<<PointLineDistance(xyzHit1.X(),xyzHit1.Y(),xyzHit1.Z(),xyzWire1.X(),xyzWire1.Y(),xyzWire1.Z(),(xyzWire1_2.X()-xyzWire1.X())/vecmag1,(xyzWire1_2.Y()-xyzWire1.Y())/vecmag1,(xyzWire1_2.Z()-xyzWire1.Z())/vecmag1)<<endl; 
                           cout<<"direction cosines in first rotated plane (x,y,z): "<< (xyzWire1_2.X()-xyzWire1.X())/vecmag1<<" "<<(xyzWire1_2.Y()-xyzWire1.Y())/vecmag1<<" "<<(xyzWire1_2.Z()-xyzWire1.Z())/vecmag1<<endl;
                           cout<<"distance in second rotated plane = "<<PointLineDistance(xyzHit2.X(),xyzHit2.Y(),xyzHit2.Z(),xyzWire2.X(),xyzWire2.Y(),xyzWire2.Z(),(xyzWire2_2.X()-xyzWire2.X())/vecmag2,(xyzWire2_2.Y()-xyzWire2.Y())/vecmag2,(xyzWire2_2.Z()-xyzWire2.Z())/vecmag2)<<endl; 
                           cout<<"direction cosines in second rotated plane (x,y,z): "<< (xyzWire2_2.X()-xyzWire2.X())/vecmag2<<" "<<(xyzWire2_2.Y()-xyzWire2.Y())/vecmag2<<" "<<(xyzWire2_2.Z()-xyzWire2.Z())/vecmag2<<endl;
                           cout<<"distance in third rotated plane = "<<PointLineDistance(xyzHit3.X(),xyzHit3.Y(),xyzHit3.Z(),xyzWire3.X(),xyzWire3.Y(),xyzWire3.Z(),(xyzWire3_2.X()-xyzWire3.X())/vecmag3,(xyzWire3_2.Y()-xyzWire3.Y())/vecmag3,(xyzWire3_2.Z()-xyzWire3.Z())/vecmag3)<<endl; 
                           cout<<"direction cosines in third rotated plane (x,y,z): "<< (xyzWire3_2.X()-xyzWire3.X())/vecmag3<<" "<<(xyzWire3_2.Y()-xyzWire3.Y())/vecmag3<<" "<<(xyzWire3_2.Z()-xyzWire3.Z())/vecmag3<<endl;
                           cout<<"distance in fourth rotated plane = "<<PointLineDistance(xyzHit4.X(),xyzHit4.Y(),xyzHit4.Z(),xyzWire4.X(),xyzWire4.Y(),xyzWire4.Z(),(xyzWire4_2.X()-xyzWire4.X())/vecmag4,(xyzWire4_2.Y()-xyzWire4.Y())/vecmag4,(xyzWire4_2.Z()-xyzWire4.Z())/vecmag4)<<endl; 
                           cout<<"direction cosines in fourth rotated plane (x,y,z): "<< (xyzWire4_2.X()-xyzWire4.X())/vecmag4<<" "<<(xyzWire4_2.Y()-xyzWire4.Y())/vecmag4<<" "<<(xyzWire4_2.Z()-xyzWire4.Z())/vecmag4<<endl;*/
                           //if(idch==0)dchTrCand1->SetDchTrackHits(trcand,xyzHit1.X(),xyzHit1.Y(),xyzHit1.Z(),drifttim[ijk[0]]->GetSV(),0.,xyzWire1.Y(),0.,0.,ijk[0],xyzHit2.X(),xyzHit2.Y(),xyzHit2.Z(),drifttim[ijk[1]]->GetSV(),xyzWire2.X(),0.,0.,0.,ijk[1],xyzHit3.X(),xyzHit3.Y(),xyzHit3.Z(),drifttim[ijk[2]]->GetSV(),xyzWire3.X(),xyzWire3.Y(),0.,0.,ijk[2],xyzHit4.X(),xyzHit4.Y(),xyzHit4.Z(),drifttim[ijk[3]]->GetSV(),xyzWire4.X(),xyzWire4.Y(),0.,0.,ijk[3]);
                           //if(idch==0)dchTrCand1->SetDchTrackHits(trcand,xyzHit1.X(),xyzHit1.Y(),xyzHit1.Z(),drifttim[ijk[0]]->GetSV(),0.,xyzWire1.Y(),0.,0.,0.,0.,ijk[0],xyzHit2.X(),xyzHit2.Y(),xyzHit2.Z(),drifttim[ijk[1]]->GetSV(),xyzWire2.X(),0.,0.,0.,0.,0.,ijk[1],xyzHit3.X(),xyzHit3.Y(),xyzHit3.Z(),drifttim[ijk[2]]->GetSV(),xyzWire3.X(),xyzWire3.Y(),0.,0.,0.,0.,ijk[2],xyzHit4.X(),xyzHit4.Y(),xyzHit4.Z(),drifttim[ijk[3]]->GetSV(),xyzWire4.X(),xyzWire4.Y(),0.,0.,0.,0.,ijk[3]);
                           //if(idch==0)dchTrCand1->SetDchTrackHits(trcand,xyzHit1,drifttim[ijk[0]]->GetSV(),xyzWire1,xyzWire1_2,ijk[0],xyzHit2,drifttim[ijk[1]]->GetSV(),xyzWire2,xyzWire2_2,ijk[1],xyzHit3,drifttim[ijk[2]]->GetSV(),xyzWire3,xyzWire3_2,ijk[2],xyzHit4,drifttim[ijk[3]]->GetSV(),xyzWire4,xyzWire4_2,ijk[3]);
                           if(fDoCheck){
                            if(idch==0){
                              trackAngleXdch1->Fill(TMath::RadToDeg()*atan((xyzHit2.X()-xyzHit1.X())/(xyzHit2.Z()-xyzHit1.Z())));
                              trackAngleYdch1->Fill(TMath::RadToDeg()*atan((xyzHit2.Y()-xyzHit1.Y())/(xyzHit2.Z()-xyzHit1.Z())));
                              trackAngleYvsXdch1->Fill(TMath::RadToDeg()*atan((xyzHit2.X()-xyzHit1.X())/(xyzHit2.Z()-xyzHit1.Z())),TMath::RadToDeg()*atan((xyzHit2.Y()-xyzHit1.Y())/(xyzHit2.Z()-xyzHit1.Z())));
                             //cout<<"angleX = "<<atan((xyzHit2.X()-xyzHit1.X())/(xyzHit2.Z()-xyzHit1.Z()))<<", angleY = "<<atan((xyzHit2.Y()-xyzHit1.Y())/(xyzHit2.Z()-xyzHit1.Z()))<<endl;
                            }else if(idch==1){
                              trackAngleXdch2->Fill(TMath::RadToDeg()*atan((xyzHit2.X()-xyzHit1.X())/(xyzHit2.Z()-xyzHit1.Z())));
                              trackAngleYdch2->Fill(TMath::RadToDeg()*atan((xyzHit2.Y()-xyzHit1.Y())/(xyzHit2.Z()-xyzHit1.Z())));
                              trackAngleYvsXdch2->Fill(TMath::RadToDeg()*atan((xyzHit2.X()-xyzHit1.X())/(xyzHit2.Z()-xyzHit1.Z())),TMath::RadToDeg()*atan((xyzHit2.Y()-xyzHit1.Y())/(xyzHit2.Z()-xyzHit1.Z())));
                            }   
                           }
                           //if(idch==1)MisAlignment(xyzHit1,xyzHit2,xyzHit3,xyzHit4,xyzWire1,xyzWire2,xyzWire3,xyzWire4); 
                           //if(idch==1&&strcmp(opt,"autocalibration")==0)DchAlignment(xyzHit1,xyzHit2,xyzHit3,xyzHit4,xyzWire1,xyzWire2,xyzWire3,xyzWire4); 
                           if(idch==0&&(strcmp(opt,"autocalibration")==0||strcmp(opt,"reconstruction")==0))DchAlignment(xyzHit1,xyzHit2,xyzHit3,xyzHit4,xyzWire1,xyzWire2,xyzWire3,xyzWire4,xyzWire1_2,xyzWire2_2,xyzWire3_2,xyzWire4_2,idch); 
                           if(idch==1&&(strcmp(opt,"autocalibration")==0||strcmp(opt,"reconstruction")==0))DchAlignment(xyzHit1,xyzHit2,xyzHit3,xyzHit4,xyzWire1,xyzWire2,xyzWire3,xyzWire4,xyzWire1_2,xyzWire2_2,xyzWire3_2,xyzWire4_2,idch); 
                           //if(idch==1&&strcmp(opt,"autocalibration")==0&&Iter>0)DchAlignment(xyzHit1,xyzHit2,xyzHit3,xyzHit4,xyzWire1,xyzWire2,xyzWire3,xyzWire4,xyzWire1_2,xyzWire2_2,xyzWire3_2,xyzWire4_2); 
                           //if(idch==1)dchTrCand2->SetDchTrackHits(trcand,xyzHit1.X(),xyzHit1.Y(),xyzHit1.Z(),drifttim[ijk[0]]->GetSV(),xyzWire1.X(),xyzWire1.Y(),xyzWire1_2.X(),xyzWire1_2.Y(),ijk[0],xyzHit2.X(),xyzHit2.Y(),xyzHit2.Z(),drifttim[ijk[1]]->GetSV(),xyzWire2.X(),xyzWire2.Y(),xyzWire2_2.X(),xyzWire2_2.Y(),ijk[1],xyzHit3.X(),xyzHit3.Y(),xyzHit3.Z(),drifttim[ijk[2]]->GetSV(),xyzWire3.X(),xyzWire3.Y(),xyzWire3_2.X(),xyzWire3_2.Y(),ijk[2],xyzHit4.X(),xyzHit4.Y(),xyzHit4.Z(),drifttim[ijk[3]]->GetSV(),xyzWire4.X(),xyzWire4.Y(),xyzWire4_2.X(),xyzWire4_2.Y(),ijk[3]);
                           //if(idch==1)dchTrCand2->SetDchTrackHits(trcand,xyzHit1.X(),xyzHit1.Y(),xyzHit1.Z(),drifttim[ijk[0]]->GetSV(),xyzWire1.X(),xyzWire1.Y(),xyzWire1.Z(),xyzWire1_2.X(),xyzWire1_2.Y(),xyzWire1_2.Z(),ijk[0],xyzHit2.X(),xyzHit2.Y(),xyzHit2.Z(),drifttim[ijk[1]]->GetSV(),xyzWire2.X(),xyzWire2.Y(),xyzWire2.Z(),xyzWire2_2.X(),xyzWire2_2.Y(),xyzWire2_2.Z(),ijk[1],xyzHit3.X(),xyzHit3.Y(),xyzHit3.Z(),drifttim[ijk[2]]->GetSV(),xyzWire3.X(),xyzWire3.Y(),xyzWire3.Z(),,xyzWire3_2.X(),xyzWire3_2.Y(),xyzWire3_2.Z(),ijk[2],xyzHit4.X(),xyzHit4.Y(),xyzHit4.Z(),drifttim[ijk[3]]->GetSV(),xyzWire4.X(),xyzWire4.Y(),xyzWire4.Z(),xyzWire4_2.X(),xyzWire4_2.Y(),xyzWire4_2.Z(),ijk[3]);
                           //if(idch==1)dchTrCand2->SetDchTrackHits(trcand,xyzHit1,drifttim[ijk[0]]->GetSV(),xyzWire1,xyzWire1_2,ijk[0],xyzHit2,drifttim[ijk[1]]->GetSV(),xyzWire2,xyzWire2_2,ijk[1],xyzHit3,drifttim[ijk[2]]->GetSV(),xyzWire3,xyzWire3_2,ijk[2],xyzHit4,drifttim[ijk[3]]->GetSV(),xyzWire4,xyzWire4_2,ijk[3]);
                           //cout<<"angles after rotation, xz, yz: "<<TMath::RadToDeg()*atan((xyzHit2.X()-xyzHit1.X())/(xyzHit2.Z()-xyzHit1.Z()))<<", "<<TMath::RadToDeg()*atan((xyzHit2.Y()-xyzHit1.Y())/(xyzHit2.Z()-xyzHit1.Z()))<<", "<<idch+1<<endl;  
                           if(idch==0)dchTrCand1->SetDchTrackHits(trcand,xyzHit1,xyzWire1,xyzWire1_2,xyzHit2,xyzWire2,xyzWire2_2,xyzHit3,xyzWire3,xyzWire3_2,xyzHit4,xyzWire4,xyzWire4_2,drifttim,ijk);
                           if(idch==1)dchTrCand2->SetDchTrackHits(trcand,xyzHit1,xyzWire1,xyzWire1_2,xyzHit2,xyzWire2,xyzWire2_2,xyzHit3,xyzWire3,xyzWire3_2,xyzHit4,xyzWire4,xyzWire4_2,drifttim,ijk);
                           //cout<<"angles after rotation (and alignment), xz, yz: "<<TMath::RadToDeg()*atan((xyzHit2.X()-xyzHit1.X())/(xyzHit2.Z()-xyzHit1.Z()))<<", "<<TMath::RadToDeg()*atan((xyzHit2.Y()-xyzHit1.Y())/(xyzHit2.Z()-xyzHit1.Z()))<<", "<<idch+1<<endl;  
                           //cout<<"trackhits: "<<trcand<<" "<<idch+1<<" "<<xyzHit1.X()<<" "<<xyzHit1.Y()<<" "<<xyzHit1.Z()<<" "<<xyzHit2.X()<<" "<<xyzHit2.Y()<<" "<<xyzHit2.Z()<<" "<<xyzHit3.X()<<" "<<xyzHit3.Y()<<" "<<xyzHit3.Z()<<" "<<xyzHit4.X()<<" "<<xyzHit4.Y()<<" "<<xyzHit4.Z()<<" "<<xyzWire1.X()<<" "<<xyzWire1.Y()<<" "<<xyzWire1.Z()<<" "<<xyzWire2.X()<<" "<<xyzWire2.Y()<<" "<<xyzWire2.Z()<<" "<<xyzWire3.X()<<" "<<xyzWire3.Y()<<" "<<xyzWire3.Z()<<" "<<xyzWire4.X()<<" "<<xyzWire4.Y()<<" "<<xyzWire4.Z()<<" "<<xyzWire1_2.X()<<" "<<xyzWire1_2.Y()<<" "<<xyzWire1_2.Z()<<" "<<xyzWire2_2.X()<<" "<<xyzWire2_2.Y()<<" "<<xyzWire2_2.Z()<<" "<<xyzWire3_2.X()<<" "<<xyzWire3_2.Y()<<" "<<xyzWire3_2.Z()<<" "<<xyzWire4_2.X()<<" "<<xyzWire4_2.Y()<<" "<<xyzWire4_2.Z()<<" "<<drifttim<<" "<<ijk<<endl;
                           if(checkGraphs&&strcmp(opt,"autocalibration")==0){
                            //hXYZcombhits->SetPoint(jjgr2++,zLayer[idch][ijk[jk]],x[jk]+detXshift[idch],y[jk]+detYshift[idch]); 
                            hXYZcombhits->SetPoint(jjgr2++,xyzHit1.Z(),xyzHit1.X(),xyzHit1.Y()); 
                            hXYZcombhits->SetPoint(jjgr2++,xyzHit2.Z(),xyzHit2.X(),xyzHit2.Y()); 
                            hXYZcombhits->SetPoint(jjgr2++,xyzHit3.Z(),xyzHit3.X(),xyzHit3.Y()); 
                            hXYZcombhits->SetPoint(jjgr2++,xyzHit4.Z(),xyzHit4.X(),xyzHit4.Y()); 
                            ExtrapToDch(xyzHit1.X(),xyzHit2.X(),xyzHit1.Y(),xyzHit2.Y(),zLayer,idch,ijk,jjgr2);
                           }
                           //trackcand->SetDchTrackCandNumber(trCand,trcand);
                           //trcand++;
                           //trackcand->SetDchTrackCandEventNumber(trCand,eventNum);
                           //trCand++;
                            //trackCand = AddTrack(hitID++, detId[ijk[jk]]->GetSV(), pos, dpos, trackId[ijk[jk]]->GetSV(), pointind[ijk[jk]]->GetSV(), 0, ijk[jk]); 
                            /*if(checkDch)cout<<"trId0 = "<<trackId[ijk[0]]->GetSV()<<", trId2 = "<<trackId[ijk[1]]->GetSV()<<", trId4 = "<<trackId[ijk[2]]->GetSV()<<", trId6 = "<<trackId[ijk[3]]->GetSV()<<", chamber = "<<idch+1<<endl; 
                            if(trackId[ijk[0]]->GetSV()==trackId[ijk[1]]->GetSV()&&trackId[ijk[1]]->GetSV()==trackId[ijk[2]]->GetSV()&&trackId[ijk[2]]->GetSV()==trackId[ijk[3]]->GetSV()){
                             for (UShort_t jkk = 0; jkk < numLayers/2; jkk++) {
                              if(checkDch)cout<<" event = "<<eventNum<<", track cand. = "<<trackId[ijk[jkk]]->GetSV()<<", point ID = "<<pointind[ijk[jkk]]->GetSV()<<", chamber = "<<idch+1<<endl;
                              //if(fDoCheck){ 
                               //if(numPlaneHits[ijk[0]]==1&&numPlaneHits[ijk[1]]==1&&numPlaneHits[ijk[2]]==1&&numPlaneHits[ijk[3]]==1){} 
                              //}
                             }
                           }*/
	                   if(strcmp(opt,"reconstruction")==0){
                            //if(hitID>0)hitID++;
                            dchCombHit = AddHit(hitGlobID, detId[ijk[0]]->GetSV(), TVector3(xyzHit1.X(),xyzHit1.Y(),xyzHit1.Z()),TVector3(dx[idch],dy_x[idch],0.), trcand, 0, 0, ijk[0], idch, driftlen[ijk[0]]->GetSV(), hitX[ijk[0]]->GetSV(), hitID);
                            cout<<"hit characteristics0: "<<hitGlobID<<" "<<detId[ijk[0]]->GetSV()<<" "<<xyzHit1.X()<<" "<<xyzHit1.Y()<<" "<<xyzHit1.Z()<<" "<<trcand<<" "<<0<<" "<<ijk[0]<<" "<<driftlen[ijk[0]]->GetSV()<<" "<<hitX[ijk[0]]->GetSV()<<" "<<hitID<<" "<<trcand<<" "<<eventNum<<endl;
                            //tReco->Fill();
                            cout<<"hitid = "<<hitID<<endl;
                            hitID++;hitGlobID++;
                            //delete dchCombHit;
                            dchCombHit = AddHit(hitGlobID, detId[ijk[1]]->GetSV(), TVector3(xyzHit2.X(),xyzHit2.Y(),xyzHit2.Z()),TVector3(dx_y[idch],dy[idch],0.), trcand, 0, 0, ijk[1], idch, driftlen[ijk[1]]->GetSV(), hitX[ijk[1]]->GetSV(), hitID);
                            cout<<"hit characteristics1: "<<hitGlobID<<" "<<detId[ijk[1]]->GetSV()<<" "<<xyzHit2.X()<<" "<<xyzHit2.Y()<<" "<<xyzHit2.Z()<<" "<<trcand<<" "<<0<<" "<<ijk[1]<<" "<<driftlen[ijk[1]]->GetSV()<<" "<<hitX[ijk[1]]->GetSV()<<" "<<hitID<<" "<<trcand<<" "<<eventNum<<endl;
                            //tReco->Fill();
                            cout<<"hitid = "<<hitID<<endl;
                            hitID++;hitGlobID++;
                            //delete dchCombHit;
                            dchCombHit = AddHit(hitGlobID, detId[ijk[2]]->GetSV(), TVector3(xyzHit3.X(),xyzHit3.Y(),xyzHit3.Z()),TVector3(dx_u[idch],dy_u[idch],0.), trcand, 0, 0, ijk[2], idch, driftlen[ijk[2]]->GetSV(), hitX[ijk[2]]->GetSV(), hitID);
                            cout<<"hit characteristics2: "<<hitGlobID<<" "<<detId[ijk[2]]->GetSV()<<" "<<xyzHit3.X()<<" "<<xyzHit3.Y()<<" "<<xyzHit3.Z()<<" "<<trcand<<" "<<0<<" "<<ijk[2]<<" "<<driftlen[ijk[2]]->GetSV()<<" "<<hitX[ijk[2]]->GetSV()<<" "<<hitID<<" "<<trcand<<" "<<eventNum<<endl;
                            //tReco->Fill();
                            //cout<<"hitid = "<<hitID<<endl;
                            hitID++;hitGlobID++;
                            //delete dchCombHit;
                            dchCombHit = AddHit(hitGlobID, detId[ijk[3]]->GetSV(), TVector3(xyzHit4.X(),xyzHit4.Y(),xyzHit4.Z()),TVector3(dx_v[idch],dy_v[idch],0.), trcand, 0, 0, ijk[3], idch, driftlen[ijk[3]]->GetSV(), hitX[ijk[3]]->GetSV(), hitID);
                            cout<<"hit characteristics3: "<<hitGlobID<<" "<<detId[ijk[3]]->GetSV()<<" "<<xyzHit4.X()<<" "<<xyzHit4.Y()<<" "<<xyzHit4.Z()<<" "<<trcand<<" "<<0<<" "<<ijk[3]<<" "<<driftlen[ijk[3]]->GetSV()<<" "<<hitX[ijk[3]]->GetSV()<<" "<<hitID<<" "<<trcand<<" "<<eventNum<<endl;
                            //tReco->Fill();
                            //cout<<"hitid = "<<hitID<<endl;
                            hitID++;hitGlobID++;
                            //delete dchCombHit;
                            }
                           trcand++;
                          }// if(jk==3)
	                  //if(strcmp(opt,"reconstruction")==0)dchCombHit = AddHit(hitID, detId[ijk[jk]]->GetSV(), pos, dpos, trackId[ijk[jk]]->GetSV(), pointind[ijk[jk]]->GetSV(), 0, ijk[jk]);
                          /*if (fDchNum == 1) {
                           dchCombHit->SetDetId(kDCH1);
                           dchCombHit->SetStation(13);
                          } else if (fDchNum == 2) { 
                           dchCombHit->SetDetId(kDCH2);
                           dchCombHit->SetStation(14);
                          }*/
                          //if(jk==3)++trcand;
                         }//radii test
                        }// layers
                       }// radcond
                        
                       //cout<<"trackId = "<<dchCombHit->GetTrackID()<<", hitId = "<<dchCombHit->GetRefIndex()<<endl;
                       //cout<<"hitID = "<<hitID<<" "<<k<<" "<<i<<" "<<j<<endl;
                     } // nn loop 
                    } // mm loop 
                   } // n loop 
                  } // m loop 
                 if((nhits2[ijk2[0]]!=0)&&(nhits2[ijk2[1]]!=0)&&(nhits2[ijk2[2]]!=0)&&(nhits2[ijk2[3]]!=0)){
                  for (UInt_t m = 0; m < nhits2[ijk2[0]]; m++) {
                   x1=hitx1_2[m];
                   for (UInt_t n = 0; n < nhits2[ijk2[1]]; n++) {
                    y2=hity2_2[n];
                    for (UInt_t mm = 0; mm < nhits2[ijk2[2]]; mm++) {
                     q3=hitq3_2[mm];q3=q3*sqrt2;
                     for (UInt_t nn = 0; nn < nhits2[ijk2[3]]; nn++) {
                      q4=hitq4_2[nn];q4=q4*sqrt2;
                      if(checkDch){
                       cout<<"x1_2,y2_2,q3_2,q4_2 = "<<x1<<", "<<y2<<", "<<q3<<", "<<q4<<endl;
                       cout<<-x1-q3+y2<<" "<<q3+q4-2.*y2<<" "<<-x1+q4-y2<<endl;
                      }
                      if(checkDchHist){
                       difxu_2[idch]->Fill(-x1-q3+y2);
                       difuv_2[idch]->Fill(q3+q4-2.*y2);
                       difxv_2[idch]->Fill(-x1+q4-y2);
                      }
                     }
                    }
                   }
                  }
                 }
                 } // l loop 
               } // k loop 
              } // j loop 
          } // i loop 

       
       if(fDoCheck){
        hTracksPerEvent[idch]->Fill(trcand);
        hWireHitsVsTracksPerEvent[idch]->Fill(sumHits,trcand);
        DCHtracks[idch]=trcand;
        DCHwireHits[idch]=sumHits;
       }
       //trackcand->SetDchNumberOfTracksInEvent(trCand,trcand);
       if(checkDch)cout<<"Number of DCH hit candidates = "<<hitID<<" in chamber "<<idch+1<<" of event "<<eventNum<<endl;
       if(checkDch)cout<<"Number of DCH track candidates = "<<trcand<<" in chamber "<<idch+1<<" of event "<<eventNum<<endl;
}
//---------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz){

// x0,y0,z0 - point coordinates
// x,y,z - point on line coordinates

    TVector3 pos0(x0,y0,z0);
    TVector3 pos(x,y,z);
    TVector3 dir(dircosx,dircosy,dircosz);
    TVector3 dif=pos-pos0;
    
    Double_t distance = ((dif.Cross(dir)).Mag())/dir.Mag();
    
    return distance;

}
//------------------------------------------------------------------------------------------------------------------------
//void		BmnDchHitProducer_exp::Finish(TH1D* &hResidRMSvsIter)
void		BmnDchHitProducer_exp::Finish(Bool_t &noError)
{

       //hitBranch->Dump();
       //trackBranch->Dump();
       //tReco->SetEntries(hitGlobID+trackGlobID);
       //cout<<hitBranch->GetEntries()<<" "<<trackBranch->GetEntries()<<" "<<tReco->GetEntries()<<endl;
       //if(strcmp(opt,"reconstruction")==0)hitBranch->Fill();
       //if(strcmp(opt,"reconstruction")==0)trackBranch->Fill();
       if(strcmp(opt,"reconstruction")==0)tReco->Fill();
       TString str1;
       str1.Form("%u",Iter);
       TString strrun;
       strrun.Form("%u",runNumber);
       //if(!(strcmp(opt,"reconstruction")==0)){
        TGraph *dchtr[numChambers];
        dchtr[0]=calib_cham[0];
        dchtr[1]=calib_cham[1];
        dchtr[0]->SetName("calibr0");
        dchtr[0]->SetTitle("rt calibration curve for DCH1");
        dchtr[1]->SetName("calibr1");
        dchtr[1]->SetTitle("rt calibration curve for DCH2");
        TGraph *dchtrPlane[numChambers][numLayers_half];
        //if(Iter>0){
        for (UShort_t idch = 0; idch < numChambers; idch++) {
         for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
          TString str2,str3;
          str2.Form("%u",idch);
          str3.Form("%u",jkk);
          dchtrPlane[idch][jkk]=calib[idch][jkk];
          dchtrPlane[idch][jkk]->SetName("calibr"+str2+str3);
          dchtrPlane[idch][jkk]->SetTitle("rt calibration curve for plane "+str3+" of DCH"+str2);
         }
        }   
        dchTracks->FinishDch(dchtr,dchtrPlane,runNumber,outDirectory,residualsFileName,locAngleX,locAngleY,noError);
       //}
       if(fDoCheck){
         TString analysisType;
         if(strcmp(opt,"autocalibration")==0){ 
          analysisType="calibration";
         }else if(strcmp(opt,"reconstruction")==0){
          analysisType="reconstruction";
         }				
         fOutputFileName = "runPeriod" + runPeriodStr + TString(".") + strrun + TString(".BmnDch_exp") + TString(".") + analysisType + str1 + TString(".root");
         TString outPut=outDirectory+fOutputFileName;
         //TFile file(fOutputFileName, "RECREATE");
         TFile file(outPut, "RECREATE");
         hList.Write(); 	
         calibList.Write(); 	
         hList.Delete();
         calibList.Delete();
         /*if(Iter==0){
          if(calibMethod==2){
           for (UShort_t idch = 0; idch < numChambers; idch++) {
            splinederivsmooth_cham[idch]->Write();
            spline5rt_cham[idch]->Write();
            minDriftTime[idch]->Write();
            for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
             delete minDriftTimePlane[idch][jkk];
             splinederivsmooth[idch][jkk]->Write();
            }
           } 
          }
          //spline5rt->Write();
          //minDriftTimeAll->Write();
          //grBy->Write();
          //grBx->Write();
         }*/
        /*trackAngleXdch1->Write();
        trackAngleYdch1->Write(); 
        trackAngleYvsXdch1->Write(); 
        trackAngleXdch2->Write();
        trackAngleYdch2->Write(); 
        trackAngleYvsXdch2->Write();*/ 
        /*deflectPointX->Write();
        deflectPointY->Write();
        deflectPointPx->Write();
        deflectPointPy->Write();
        deflectPointPz->Write();
        magfieldinteg->Write();*/
        /*for (UShort_t idch = 0; idch < numChambers; idch++) {
         for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
          //spline5rtPlane[idch][jkk]->Write();
          calib[idch][jkk]->Write();
         }
        }
        for (UShort_t j = 0; j < numChambers; j++) {
         calib_cham[j]->Write();
        }*/
        /*for (UShort_t idch = 0; idch < numChambers; idch++) {
         //for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
         for (UShort_t jkk = 0; jkk < numLayers; jkk++) {
          hWireNumberInPlane[idch][jkk]->Write();
          hWireCoordinateInPlane[idch][jkk]->Write();
          hHitCoordinateInPlane[idch][jkk]->Write();
         }
        }*/
        //calib->Write();
        if(checkGraphs)gList.Write();
        file.Close();
        if(checkGraphs)gList.Delete();
       }else{
        if(Iter==0&&!(strcmp(opt,"reconstruction")==0)){
         for (UShort_t idch = 0; idch < numChambers; idch++) {
          delete splinederivsmooth_cham[idch];
          for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
           delete splinederivsmooth[idch][jkk];
          }
         }
        }
       } // fDoCheck
       /*for (UShort_t idch = 0; idch < numChambers; idch++) {
        //delete spline5rt_cham[idch];
        //if(Iter==0)delete spline5_cham[idch]; 
       cout<<"I am here1"<<endl;
        for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
         //delete spline5rt[idch][jkk]; 
         //if(Iter==0)delete spline5[idch][jkk]; 
       cout<<"I am here2"<<endl;
        }
       }*/
        /*for (UShort_t idch = 0; idch < numChambers; idch++) {
         //for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
         for (UShort_t jkk = 0; jkk < numLayers; jkk++) {
          //delete hWireNumberInPlane[idch][jkk];
          //delete hWireCoordinateInPlane[idch][jkk];
          //delete hHitCoordinateInPlane[idch][jkk];
          //if(Iter>0)delete spline5rtPlane[idch][jkk]; 
          //if(Iter>0&&jkk<numLayers_half)delete spline5rtPlane[idch][jkk]; 
          //if(jkk<numLayers_half)delete spline5rt[idch][jkk]; 
          //if(Iter==0&&jkk<numLayers_half)delete spline5[idch][jkk]; 
         }
        }*/
        /*if(Iter==0){
         delete minDriftTimeAll; 
         for (UShort_t idch = 0; idch < numChambers; idch++) {
          delete minDriftTime[idch];
          delete spline5_cham[idch];
         }
         if (calibMethod==2){ 
          for (UShort_t idch = 0; idch < numChambers; idch++) {
           delete splinederivsmooth_cham[idch];
           for (UShort_t jkk = 0; jkk < numLayers_half; jkk++) {
            delete splinederivsmooth[idch][jkk];
           }
          }
         }
         //delete spline5rt[0]; delete spline5rt[1];
        }*/
        //delete spline5rt_cham[0]; delete spline5rt_cham[1];
        //delete trackAngleXdch1; delete trackAngleYdch1;
        //delete trackAngleXdch2; delete trackAngleYdch2;
        //delete trackAngleYvsXdch1; delete trackAngleYvsXdch2;
        //delete deflectPointX; delete deflectPointY; 
        //delete deflectPointPx; delete deflectPointPy; delete deflectPointPz;
        //delete magfieldinteg;

        // TIter nextkey(fdstread1->GetListOfKeys());
        // TKey *key;
        // while (key = (TKey*)nextkey()) {
        // DchTrackCand *trackcand1 = (DchTrackCand*)key->ReadObj();
        // Int_t trackEntries=trackcand1->fDchTrackCand->GetEntriesFast();
        // cout<<"trackEntries1 = "<<trackEntries<<endl;
        //TGraph *dchtr=calib;
        //}
        //dchTracks->FinishDch(dchtr,hResidRMSvsIter,Iter,datatype);
        //Double_t locAngleX,locAngleY;
        //if(Iter==0){delete calib[0];delete calib[1];}
        //TFile f2("tmp.root","RECREATE");
        if(strcmp(opt,"reconstruction")==0){
         fReco->cd();
         //cout<<"CurrentFile: "<<tReco->GetCurrentFile()->GetName()<<endl;; 
         //cout<<"CurrentDirectory: "<<tReco->GetDirectory()->GetName()<<endl;; 
         //tReco->ResetBranchAddresses();
         tReco->Write();
         delete tReco; 
         fReco->Close();
         fReco->Delete();
         //pHitCollection->Dump();
         pHitCollection->Delete();
         for (UShort_t jkk = 0; jkk < 3; jkk++) {
          pTrackCollection[jkk]->Delete();
         }
        }
        delete dchTracks;
        //tReco->Delete();
        //delete magField;
        gDirectory->GetList()->Delete();
        //gObjectTable->Print(); 
        //if(strcmp(opt,"autocalibration")==0&&!(noError)){
        if(strcmp(opt,"autocalibration")==0&&noError){
         ofstream DCHparFile;
         DCHpar=outDirectory+DCHpar;
         DCHparFile.open (DCHpar);
         //DCHpar << locAngleX <<endl;
         //DCHpar << locAngleY <<endl;
         //DCHpar << Iter <<endl;
         DCHparFile << XYZshift1.X() <<endl;
         DCHparFile << XYZshift1.Y() <<endl;
         DCHparFile << XYZshift1.Z() <<endl;
         DCHparFile << XYZshift2.X() <<endl;
         DCHparFile << XYZshift2.Y() <<endl;
         DCHparFile << XYZshift2.Z() <<endl;
         DCHparFile << ranmin_cham[0] <<endl;
         DCHparFile << ranmax_cham[0] <<endl;
         DCHparFile << ranmin_cham[1] <<endl;
         DCHparFile << ranmax_cham[1] <<endl;
         for (UShort_t j = 0; j < numChambers; j++) {
           for (UShort_t k = 0; k < numLayers_half; k++) {
             DCHparFile << ranmin[j][k] <<endl;
           }
         }
         for (UShort_t j = 0; j < numChambers; j++) {
           for (UShort_t k = 0; k < numLayers_half; k++) {
             DCHparFile << ranmax[j][k] <<endl;
           }
         }
         for (UShort_t j = 0; j < 4; j++) {
           for (UShort_t k = 0; k < 4; k++) {
             DCHparFile << C(j,k) <<endl;
           }
         }
         DCHparFile.close();
        }
//gObjectTable->Print(); 
}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHit* BmnDchHitProducer_exp::AddHit0(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer)
{
	BmnDchHit *pHit	=  new ((*pHitCollection)[index]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	pHit->AddLink(FairLink(1, pointIndex)); 
	pHit->AddLink(FairLink(2, trackIndex)); 
	
return pHit;
}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHit* BmnDchHitProducer_exp::AddHit(const Int_t hitGlobId, const Int_t detID, const TVector3& posHit, const TVector3& posHitErr, const Int_t trackIndex, const Int_t pointIndex, const Int_t flag, const UShort_t dchlayerNumber, const UShort_t idch, const Double_t driftlen, const Double_t hitx, const Int_t hitID)
{
	//BmnDchHit *pHit	=  new ((*pHitCollection)[index]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	BmnDchHit *pHit	=  new ((*pHitCollection)[hitGlobId]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, ' ');
	//TClonesArray& clref = *pHitCollection;
        //Int_t size = clref.GetEntriesFast();
        //BmnDchHit *pHit = new(clref[size]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
        //cout<<"size = "<<size<<endl;
        pHit->SetDchIdNumber(idch+1);
        pHit->SetPhi(posHit.Phi());
        pHit->SetLength(sqrt(pow(posHit.X()-XYZ0beam.X(),2.)+pow(posHit.Y()-XYZ0beam.Y(),2.)+pow(posHit.Z()-XYZ0beam.Z(),2.)));
        pHit->SetDrift(driftlen);
        pHit->SetWirePosition(hitx);
        pHit->SetDchLayerNumber(dchlayerNumber);
        //cout<<"hitID = "<<hitID<<endl;
        pHit->SetHitId(hitID);
        //cout<<"hitID = "<<pHit->GetHitId()<<endl;
        pHit->SetEventID(eventNum);
	//pHit->AddLink(FairLink(1, pointIndex)); 
	//pHit->AddLink(FairLink(2, trackIndex)); 
	//pHit->Dump();
	
return pHit;
}
//-------------------------------------------------------------------------------------------------------------------------
/*DchTrackCand* BmnDchHitProducer_exp::AddTrack(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer)
{
	DchTrackCand *pTrack	=  new ((*pTrackCollection)[index]) DchTrackCand(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	//pTrack->AddLink(FairLink(1, pointIndex)); 
	//pTrack->AddLink(FairLink(2, trackIndex)); 
	
return pTrack;
}*/
//------------------------------------------------------------------------------------------------------------------------
/*void BmnDchHitProducer_exp::RadialRange(FairMCPoint* dchPoint){

Double_t x0,y0,z0;
Double_t x,y,z;
Double_t px,py,pz,p;
Double_t dircosx,dircosy,dircosz;

    x=dchPoint->GetX();y=dchPoint->GetY();z=dchPoint->GetZ();
    px=dchPoint->GetPx();py=dchPoint->GetPy();pz=dchPoint->GetPz();
    p=sqrt(px*px+py*py+pz*pz);
    dircosx=px/p;dircosy=py/p;dircosz=pz/p;
    x0=x;y0=y;z0=z+4.8;
    Double_t r=PointLineDistance(x0, y0, z0, x, y, z, dircosx, dircosy, dircosz);
    //cout<<"RadialRange "<<x0<<" "<<y0<<" "<<z0<<" "<<x<<" "<<y<<" "<<z<<" "<<dircosx<<" "<<dircosy<<" "<<dircosz<<endl;
    hRadiusRange->Fill(r);
    //cout<<"radius in next plane = "<<r<<endl;

}*/
//-------------------------------------------------------------------------------------------------------------------------
//void BmnDchHitProducer_exp::LRambiguity(UInt_t k, Double_t hittmp[2], Double_t hitx, Double_t driftdist, UInt_t hw, UShort_t &nhits){
void BmnDchHitProducer_exp::LRambiguity(const UInt_t k, const Double_t hitx, const Double_t driftdist, const UInt_t hw, Double_t drifttim, UShort_t &nhits, Double_t hittmp[2], UShort_t &nwires_nextplane, UInt_t hw_nextplane[2]){


//cout<<"plane number, "<<"anode wire coordinate, "<<"drift distance, "<<"anode wire number, "<<"drift time ,"<<"left wire in the next plane (hit yes or not), "<<"right wire in the next plane (hit yes or not)"<<endl;

//cout<<k<<" "<<hitx<<" "<<driftdist<<" "<<hw<<" "<<drifttim<<" "<<wireUsed[k+1][hw-1]<<" "<<wireUsed[k+1][hw]<<endl;

              UInt_t kcompl; //index of second plane in pair
              UInt_t hw1,hw2;
              if(k%2==0){
                kcompl=k+1;
                //hw1=hw;hw2=hw-1;
                hw1=hw+1;hw2=hw;
              }else{
                kcompl=k-1;
                //hw1=hw+1;hw2=hw;
                hw1=hw;hw2=hw-1;
              }

              //hw1=hw;hw2=hw-1;
              //hw1=hw+1;hw2=hw;

              if(hw!=0&&hw!=numWiresPerLayer){
               //cout<<"wireUsed: "<<wireUsed[kcompl][hw]<<" "<<wireUsed[kcompl][hw-1]<<" "<<hw<<endl;
               if(wireUsed[kcompl][hw1]&&wireUsed[kcompl][hw2]){
                 nhits=2; 
                 hittmp[0]=hitx-driftdist;
                 hittmp[1]=hitx+driftdist;
                 hw_nextplane[0]=hw1;
                 hw_nextplane[1]=hw2;
                 nwires_nextplane=2;
                 if(checkDch)cout<<"LRambig1"<<endl;
               }else if(!wireUsed[kcompl][hw1]&&!wireUsed[kcompl][hw2]){
                 nhits=2; 
                 hittmp[0]=hitx-driftdist;
                 hittmp[1]=hitx+driftdist;
                 nwires_nextplane=0;
                 if(checkDch)cout<<"LRambig2"<<endl;
               }else if(!wireUsed[kcompl][hw1]&&wireUsed[kcompl][hw2]){
                 hittmp[0]=hitx-driftdist;
                 nhits=1; 
                 hw_nextplane[0]=hw2;
                 nwires_nextplane=1;
                 //if(checkDch)cout<<wireUsed[kcompl][hw1]<<" "<<wireUsed[kcompl][hw2]<<" "<<hw1<<" "<<hw2<<" "<<kcompl<<endl;
                 if(checkDch)cout<<"LRambig3"<<endl;
               }else if(wireUsed[kcompl][hw1]&&!wireUsed[kcompl][hw2]){
                 hittmp[0]=hitx+driftdist;
                 nhits=1; 
                 hw_nextplane[0]=hw1;
                 nwires_nextplane=1;
                 if(checkDch)cout<<"LRambig4"<<endl;
                 //if(checkDch)cout<<wireUsed[kcompl][hw1]<<" "<<wireUsed[kcompl][hw2]<<" "<<hw1<<" "<<hw2<<" "<<kcompl<<endl;
               }
              }else if(hw==0){
               nhits=1; 
               hittmp[0]=hitx+driftdist;
               if(wireUsed[kcompl][hw1]){
                 hw_nextplane[0]=hw1;
                 nwires_nextplane=1;
                 if(checkDch)cout<<"LRambig5"<<endl;
               }else{
                 nwires_nextplane=0;
                 if(checkDch)cout<<"LRambig6"<<endl;
               }
              }else if(hw==numWiresPerLayer){ 
               nhits=1; 
               hittmp[0]=hitx-driftdist;
               if(wireUsed[kcompl][hw2]){
                 hw_nextplane[0]=hw2;
                 nwires_nextplane=1;
                 if(checkDch)cout<<"LRambig7"<<endl;
               }else{
                 nwires_nextplane=0;
                 if(checkDch)cout<<"LRambig8"<<endl;
               }
              }

}
//-------------------------------------------------------------------------------------------------------------------------
/*void BmnDchHitProducer_exp::CoordinateFinder(const Double_t x2,const Double_t y1,const Double_t k3,const Double_t q3,const Double_t k4,const Double_t q4,const UShort_t *ijk,Double_t x[],Double_t y[]){ 

   Double_t x1,x3,x4,y2,y3,y4;

   x[1]=x2;y[0]=y1;
             /*
             x[0]=(x[1]*(k4-k3)+(q4-y[0])/z4121-(q3-y[0])/z3121)/(k4-k3+k3/z3121-k4/z4121);
             x[2]=(x[1]-x[0])*z3121+x[0];
             x[3]=(x[1]-x[0])*z4121+x[0];
             y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
             */
            
             /*
             x[0]=(x[1]*(k4-k3)+(q4-y[0])/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]-(q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])])/(k4-k3+k3/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]-k4/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]);
             //x[2]=(x[1]-x[0])*z3121+x[0];
             x[2]=(x[1]-x[0])*zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]+x[0];
             //x[3]=(x[1]-x[0])*z4121+x[0];
             x[3]=(x[1]-x[0])*zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]+x[0];
             //y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]+y[0];
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
             */ 
 
/*                      cout<<"2------------------------------------------------------"<<endl; 
                      for (UShort_t jk = 0; jk < numLayers/2; jk++) {
                       cout<<"points: "<<x[jk]<<" "<<y[jk]<<endl;
                      }
                      cout<<"2------------------------------------------------------"<<endl; 
*/  /*
}*/ 
//-------------------------------------------------------------------------------------------------------------------------
/*void BmnDchHitProducer_exp::CoordinateFinder2(const Double_t x1,const Double_t y2,const Double_t k3,const Double_t q3,const Double_t k4,const Double_t q4,const UShort_t *ijk,Double_t x[],Double_t y[]){ 

   Double_t x1,x3,x4,y2,y3,y4;

   //x[1]=x2;y[0]=y1;
   x[0]=x1;y[1]=y2;
   
   
             /*
             x[0]=(x[1]*(k4-k3)+(q4-y[0])/z4121-(q3-y[0])/z3121)/(k4-k3+k3/z3121-k4/z4121);
             x[2]=(x[1]-x[0])*z3121+x[0];
             x[3]=(x[1]-x[0])*z4121+x[0];
             y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
             */
            
             /* 
             //x[0]=(x[1]*(k4-k3)+(q4-y[0])/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]-(q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])])/(k4-k3+k3/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]-k4/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]);
             x[1]=(-x[0]*(k4-k3)-(q4-y[1])/zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]+(q3-y[1])/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])])/(-(k4-k3)-k3/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]+k4/zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]);
             //cout<<"x1 test1 "<<-x[0]*(k4-k3)<<" "<<-(q4-y[1])/zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<" "<<(q3-y[1])/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]<<endl;
             //cout<<"x1 test1 "<<-x[0]*(k4-k3)<<" "<<-(q4-y[1])<<" "<<zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<" "<<(q3-y[1])<<" "<<zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]<<endl;
             //cout<<"x1 test2 "<<-(k4-k3)<<" "<<-k3/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]<<" "<<k4/zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<endl;
             //x[2]=(x[1]-x[0])*z3121+x[0];
             x[2]=-(x[1]-x[0])*zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]+x[1];
             //x[3]=(x[1]-x[0])*z4121+x[0];
             x[3]=-(x[1]-x[0])*zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]+x[1];
             //y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             //y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]+y[0];
             y[0]=-k3*(x[1]-x[0])+(k3*x[1]+q3-y[1])/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]+y[1];
             //cout<<"y1 test1 "<<-k3*(x[1]-x[0])<<" "<<k3*x[1]/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]<<" "<<+q3/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<" "<<-y[1]/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<endl;
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
             */  
             /* 
                      cout<<"2------------------------------------------------------"<<endl; 
                      for (UShort_t jk = 0; jk < numLayers/2; jk++) {
                       cout<<"points2: "<<x[jk]<<" "<<y[jk]<<endl;
                      }
                      cout<<"2------------------------------------------------------"<<endl; 
             
}*/ 
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::CoordinateFinder3(const Double_t x1,const Double_t y2,const Double_t k3,const Double_t q3,const Double_t k4,const Double_t q4,const UShort_t *ijk,Double_t x[],Double_t y[]){ 

   //Double_t x1,x3,x4,y2,y3,y4;

   //x[1]=x2;y[0]=y1;
   x[0]=x1;y[1]=y2;
   
             /*
             x[0]=(x[1]*(k4-k3)+(q4-y[0])/z4121-(q3-y[0])/z3121)/(k4-k3+k3/z3121-k4/z4121);
             x[2]=(x[1]-x[0])*z3121+x[0];
             x[3]=(x[1]-x[0])*z4121+x[0];
             y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
             */
            

             //x[0]=(x[1]*(k4-k3)+(q4-y[0])/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]-(q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])])/(k4-k3+k3/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]-k4/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]);
             //x[1]=(-x[0]*(k4-k3)-(q4-y[1])/zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]+(q3-y[1])/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])])/(-(k4-k3)-k3/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]+k4/zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]);
             //x[1]=zDif21[GetGasGap(ijk[1])][GetGasGap(ijk[0])]*(((q4-y[1])/zDif42[GetGasGap(ijk[3])][GetGasGap(ijk[1])]-(q3-y[1])/zDif32[GetGasGap(ijk[2])][GetGasGap(ijk[1])])/k3+x[0]*(1./zDif21[GetGasGap(ijk[1])][GetGasGap(ijk[0])]-1./zDif42[GetGasGap(ijk[3])][GetGasGap(ijk[1])]/tmpfac-1./zDif32[GetGasGap(ijk[2])][GetGasGap(ijk[1])]/tmpfac));
             Double_t denom=(-zMult1234[GetGasGap(ijk[0])][GetGasGap(ijk[1])][GetGasGap(ijk[2])][GetGasGap(ijk[3])]+
zMult12[GetGasGap(ijk[0])][GetGasGap(ijk[1])]+zMult34[GetGasGap(ijk[2])][GetGasGap(ijk[3])]);
             x[1]=(2.*x[0]*zDif32[GetGasGap(ijk[2])][GetGasGap(ijk[1])]*zDif42[GetGasGap(ijk[3])][GetGasGap(ijk[1])]+
(zDif21[GetGasGap(ijk[1])][GetGasGap(ijk[0])])*
(y[1]*zDif43[GetGasGap(ijk[3])][GetGasGap(ijk[2])]/tg3-
q3*zDif42[GetGasGap(ijk[3])][GetGasGap(ijk[1])]+
q4*zDif32[GetGasGap(ijk[2])][GetGasGap(ijk[1])]))/denom;
             //cout<<"x1 test1 "<<-x[0]*(k4-k3)<<" "<<-(q4-y[1])/zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<" "<<(q3-y[1])/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]<<endl;
             //cout<<"x1 test1 "<<-x[0]*(k4-k3)<<" "<<-(q4-y[1])<<" "<<zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<" "<<(q3-y[1])<<" "<<zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]<<endl;
             //cout<<"x1 test2 "<<-(k4-k3)<<" "<<-k3/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]<<" "<<k4/zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<endl;
             //x[2]=(x[1]-x[0])*z3121+x[0];
             //x[2]=-(x[1]-x[0])*zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]+x[1];
             x[2]=(x[1]-x[0])*zDif31[GetGasGap(ijk[2])][GetGasGap(ijk[0])]/zDif21[GetGasGap(ijk[1])][GetGasGap(ijk[0])]+x[0];
             //x[3]=(x[1]-x[0])*z4121+x[0];
             //x[3]=-(x[1]-x[0])*zCoeff2[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]+x[1];
             x[3]=(x[1]-x[0])*zDif41[GetGasGap(ijk[3])][GetGasGap(ijk[0])]/zDif21[GetGasGap(ijk[1])][GetGasGap(ijk[0])]+x[0];
             //y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             //y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]+y[0];
             //y[0]=-k3*(x[1]-x[0])+(k3*x[1]+q3-y[1])/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]+y[1];
             //y[0]=-zDif21[GetGasGap(ijk[1])][GetGasGap(ijk[0])]/zDif32[GetGasGap(ijk[2])][GetGasGap(ijk[1])]*(q3+k3*((x[1]-x[0])*zDif31[GetGasGap(ijk[2])][GetGasGap(ijk[0])]/zDif21[GetGasGap(ijk[1])][GetGasGap(ijk[0])]+x[0])-y[1])+y[1]; 
             //cout<<"y1 test1 "<<-k3*(x[1]-x[0])<<" "<<k3*x[1]/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[2])]<<" "<<+q3/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<" "<<-y[1]/zCoeff1[ijk[1]][GetGasGap(ijk[0])][GetGasGap(ijk[3])]<<endl;
             y[0]=(2.*y[1]*zDif31[GetGasGap(ijk[2])][GetGasGap(ijk[0])]*zDif41[GetGasGap(ijk[3])][GetGasGap(ijk[0])]-
zDif21[GetGasGap(ijk[1])][GetGasGap(ijk[0])]*
(x[0]*tg3*zDif43[GetGasGap(ijk[3])][GetGasGap(ijk[2])]+
q3*zDif41[GetGasGap(ijk[3])][GetGasGap(ijk[0])]+
q4*zDif31[GetGasGap(ijk[2])][GetGasGap(ijk[0])]))/denom;
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
           
                    if(checkDch){ 
                      cout<<"2------------------------------------------------------"<<endl; 
                      for (UShort_t jk = 0; jk < numLayers/2; jk++) {
                       cout<<"points3: "<<x[jk]<<" "<<y[jk]<<endl;
                      }
                      cout<<"2------------------------------------------------------"<<endl; 
                    }
} 
//-------------------------------------------------------------------------------------------------------------------------
/*void BmnDchHitProducer_exp::CoordinateCorrection(Double_t x[],Double_t y){

  Double_t x_mean=0.,y_mean=0.;

       for (UShort_t jk = 0; jk < numLayers_half; jk++) {
        //cout<<"points: "<<x[jk]<<" "<<y[jk]<<endl;
        x_mean+=x[jk]; 
        y_mean+=y[jk]; 
       }

  x_mean/=numLayers_half;
  y_mean/=numLayers_half;

  Double_t z_origin=0.5*(zLayer[idch][3]+zLayer[idch][5]);

  


}*/ 
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::ReturnPointers(UInt_t ijk,UInt_t i,ScalarD* &hitX,ScalarI* &trackId,ScalarI* &detId,ScalarUI* &hitwire,ScalarD* &driftlen,ScalarD* &drifttim,ScalarI* &pointind){
              hitX = (ScalarD*) dchhitplane[ijk]->fDchHitPlane1->UncheckedAt(i); 
              trackId = (ScalarI*) dchhitplane[ijk]->fDchHitPlane2->UncheckedAt(i); 
              detId = (ScalarI*) dchhitplane[ijk]->fDchHitPlane4->UncheckedAt(i); 
              hitwire = (ScalarUI*) dchhitplane[ijk]->fDchHitPlane5->UncheckedAt(i); 
              driftlen = (ScalarD*) dchhitplane[ijk]->fDchHitPlane6->UncheckedAt(i); 
              drifttim = (ScalarD*) dchhitplane[ijk]->fDchHitPlane7->UncheckedAt(i); 
              pointind = (ScalarI*) dchhitplane[ijk]->fDchHitPlane8->UncheckedAt(i); 
}
//---------------------------------------------------------------------------

Double_t BmnDchHitProducer_exp::LineLineDCA(const Double_t x1, const Double_t y1, const Double_t z1, const Double_t dircosx1, const Double_t dircosy1, const Double_t dircosz1, const Double_t stx, const Double_t sty, const Double_t stz, const Double_t dircosstx, const Double_t dircossty, const Double_t dircosstz, Double_t &x2, Double_t &y2, Double_t &z2, Double_t &xstmin, Double_t  &ystmin, Double_t &zstmin, Double_t &length, Double_t &mu0, Double_t &lambda0){

    Double_t lambda2,x0,y0,z0;
    TVector3 pos1(x1,y1,z1);
    TVector3 posst(stx,sty,stz);
    TVector3 dir1(dircosx1,dircosy1,dircosz1);
    TVector3 dirst(dircosstx,dircossty,dircosstz);
    //TVector3 dif1st(posst(0)-pos1(0),posst(1)-pos1(1),posst(2)-pos1(2));
    TVector3 dif1st=posst-pos1;

   
    Double_t denom=1.-pow(dir1*dirst,2);

    lambda0=(dif1st*dir1-(dif1st*dirst)*(dir1*dirst))/denom;
    mu0=-(dif1st*dirst-(dif1st*dir1)*(dir1*dirst))/denom;
 
    TVector3 closdis(dif1st(0)+mu0*dirst(0)-lambda0*dir1(0),dif1st(1)+mu0*dirst(1)-lambda0*dir1(1),dif1st(2)+mu0*dirst(2)-lambda0*dir1(2));

    //TVector3 trackvec(x1+lambda0*dircosx1,y1+lambda0*dircosy1,z1+lambda0*dircosz1);
    //TVector3 strawtubevec(stx+mu0*dircosstx,sty+mu0*dircossty,stz+mu0*dircosstz);
    lambda2=(ZhalfLength_DCHActiveVolume)/dircosz1;
    //if(checkDch)cout<<"middle track point in DCH volume "<<x1<<" "<<y1<<" "<<z1<<endl;
    x0=x1-lambda2*dircosx1;y0=y1-lambda2*dircosy1;z0=z1-ZhalfLength_DCHActiveVolume;
    if(checkDch)cout<<"first track point in DCH volume "<<x0<<" "<<y0<<" "<<z0<<endl;
    if(checkDch)cout<<"dca point on track line "<<x1+lambda0*dircosx1<<" "<<y1+lambda0*dircosy1<<" "<<z1+lambda0*dircosz1<<endl;// dca point on track line
    x2=x1+lambda2*dircosx1;y2=y1+lambda2*dircosy1;z2=z1+lambda2*dircosz1; 
    if(checkDch)cout<<"second track point in DCH volume "<<x2<<" "<<y2<<" "<<z2<<endl;
    xstmin=stx+mu0*dircosstx;ystmin=sty+mu0*dircossty;zstmin=stz+mu0*dircosstz; 
    if(checkDch)cout<<"dca point on anode wire "<<xstmin<<" "<<ystmin<<" "<<zstmin<<endl;
    //length=2.*lambda0*sqrt(pow(dircosx1,2.)+pow(dircosy1,2.)+pow(dircosz1,2.)) ;
    length=LineLength(x0,y0,z0,x2,y2,z2);
    /*TVector3 test1(x1,y1,z1),test2(x2,y2,z2);
    Double_t rad1=test1.Perp(),rad2=test2.Perp();
    Double_t delrad=fabs(rad2-rad1);
    Double_t length0=sqrt(length*length-delrad*delrad);*/ 
  /*if(checktrtube){
    delradtube->Fill(delrad);
    length0hist->Fill(length0);
    delradvslength0->Fill(length0,delrad); 
    dcavslength0->Fill(length0,closdis.Mag()); 
  }*/
    //if(length0>0.4)cout<<"track in tube components "<<length0<<" "<<delrad<<" "<<length<<endl;
    //if(length0>0.4)cout<<"track in tube components "<<dircosx1<<" "<<dircosy1<<" "<<dircosz1<<endl;
    //cout<<"closdis = "<<closdis.Mag()<<" mu0 = "<<mu0<<" "<<endl;
    return closdis.Mag();   // return the closest distance
}

//---------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::LineLength(const Double_t x1, const Double_t y1, const Double_t z1, const Double_t x2, const Double_t y2, const Double_t z2){

    TVector3 pos1(x1,y1,z1);
    TVector3 pos2(x2,y2,z2);
    TVector3 dif=pos2-pos1;
    
    Double_t distance = dif.Mag();
    
    return distance;
}

//-------------------------------------------------------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::rtCurve(const Double_t time,const UShort_t idch, const UShort_t iplane)
{

Double_t radDist;
//Double_t timemin;
//if(checkDch)cout<<"time = "<<time<<" "<<ranmin[idch]<<" "<<ranmax[idch]<<endl;

if(time<ranmin[idch][iplane]){
 //radDist=0.;
 radDist=10000.;
 if(checkDch)cout<<"drift time underflow"<<endl;
//}else if(time>=ranmin[idch]&&time<=ranmax[idch]){
}else if(time>=ranmin[idch][iplane]&&time<=ranmax[idch][iplane]){
 //radDist=spline5rt->Eval(time-ranmin[idch]);
 /*if(Iter==0){
  radDist=spline5rt[idch]->Eval(time-ranmin[idch]);
 }else{
  radDist=spline5rtPlane[idch][iplane]->Eval(time-ranmin[idch]);
 }*/ 
 //radDist=spline5rtPlane[idch][iplane]->Eval(time-ranmin[idch]);
 radDist=spline5rt[idch][iplane]->Eval(time-ranmin[idch][iplane]);
}else{
 //if(checkDch)cout<<"spline5rt "<<time<<" "<<ranmin[idch]<<" "<<ranmax[idch]<<endl;
 if(checkDch)cout<<"spline5rt "<<time<<" "<<ranmin[idch][iplane]<<" "<<ranmax[idch][iplane]<<endl;
 //radDist=spline5rt->Eval(ranmax[idch]-ranmin[idch]);
 //radDist=spline5rt[idch]->Eval(ranmax[idch]-ranmin[idch]);
radDist=10000.;
//radDist=0.5;
if(checkDch)cout<<"drift time overflow"<<endl;
}
if(radDist<0.)radDist=0.;
//if(radDist>0.45&&idch==1)cout<<"radius045 = "<<radDist<<endl;
return radDist;
 
}
//-------------------------------------------------------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::SplineIntegral(const Double_t a, const Double_t b, const Int_t n, const TSpline5 *Spline5)
{
  Double_t integral=0.,delta=(b-a)/Double_t(n);
  Double_t x0,x1;

         x0=a;    
 
         for(Int_t i = 0; i < n; i++) {
            x1=x0+delta; 
            //integral=integral+delta*spline5[idch]->Eval(x0)+0.5*delta*(spline5[idch]->Eval(x1)-spline5[idch]->Eval(x0));   
            integral=integral+delta*Spline5->Eval(x0)+0.5*delta*(Spline5->Eval(x1)-Spline5->Eval(x0));   
            //cout<<"splineintegral params.: "<<n<<" "<<delta<<" "<<x0<<" "<<x1<<" "<<spline5->Eval(x0)<<" "<<spline5->Eval(x1)<<" "<<spline5->Eval(x1)-spline5->Eval(x0)<<endl;
            //cout<<"splineintegral = "<<integral<<" "<<a<<" "<<b<<endl;
            x0=x1;
         }
    
return integral;

}
//-------------------------------------------------------------------------------------------------------------------------
UShort_t BmnDchHitProducer_exp::PlanesTopology(const UShort_t idch){

//Bool_t planesUsed=true;
//UShort_t i=0;

/*
//all planes used

     while(planesUsed&&i<numLayers){
      planesUsed=planesUsed*planeUsed[i++];
     }   
     if(planesUsed)topol=0;
     break;    
*/
/*
Bool_t evenPlanesUsed=true,oddPlanesUsed=true;
//either all odd or all even planes used

i=0;
 
     while(evenPlanesUsed&&i<numLayers){
      evenPlanesUsed=evenPlanesUsed*planeUsed[i];
      i+=2;
     }   

i=1;
 
     while(oddPlanesUsed&&i<numLayers){
      oddPlanesUsed=oddPlanesUsed*planeUsed[i];
      i+=2;
     }   

     if((evenPlanesUsed+oddPlanesUsed)==2){
      topol=0;
     }else if((evenPlanesUsed+oddPlanesUsed)==1){
      topol=1;
     }else{
      topol=2;
     } 
     break;    
*/

UShort_t pairPlanesUsed=0,planesUsed=0;
Short_t topol_=-1;
//UShort_t imin,imax;

     /*if(idch==0){
      imin=0;imax=numLayers;
     }else if(idch==1){
      imin=numLayers;imax=twoNumLayers;
     }*/ 

     for(UShort_t i = 0; i < numLayers; i+=2) {
     //for(UShort_t i = imin; i < imax; i+=2) {
       if(planeUsed[i]||planeUsed[i+1])pairPlanesUsed=++pairPlanesUsed;
       planesUsed=planesUsed+UShort_t(planeUsed[i])+UShort_t(planeUsed[i+1]);
     }

     if(pairPlanesUsed==4){
      topol_=1;
     }else{
      topol_=2;
     }
     if(planesUsed==8)topol_=0;

if(checkDch)cout<<"planes topology in DCH "<<idch+1<<" = "<<topol_<<", main planes (0 to 4) = "<<pairPlanesUsed<<", planes (0 to 8) = "<<planesUsed<<endl; 

return topol_;

}
//-------------------------------------------------------------------------------------------------------------------------
/*UShort_t BmnDchHitProducer_exp::RunPeriodToNumber(TString runPeriod_){

UShort_t j;

           if(runPeriod_=="run1"){j=0;}
           else if(runPeriod_=="run2"){j=1;}
           else if(runPeriod_=="run3"){j=2;}
           else{cout<<"run type not in the list!"<<endl; 
                cout<<" run type = "<<runPeriod_<<endl;}
return j;

}*/
//-------------------------------------------------------------------------------------------------------------------------
//void BmnDchHitProducer_exp::ExtrapToDch(const Double_t x[],const Double_t y[],const Double_t zLayer[][numLayers], const UShort_t idch, const UShort_t ijk[], Int_t &jjgr2){
void BmnDchHitProducer_exp::ExtrapToDch(const Double_t x0, const Double_t x1, const Double_t y0, const Double_t y1, const Double_t zLayer_[][numLayers], const UShort_t idch, const UShort_t ijk[], Int_t &jjgr2){

Double_t delZ=zLayer_[idch][ijk[1]]-zLayer_[idch][ijk[0]],delZ2;
//Double_t delXshift=detXshift[1]-detXshift[0];
//Double_t tgx=(x[1]-x[0])/delZ;
//Double_t tgy=(y[1]-y[0])/delZ;
Double_t tgx=(x1-x0)/delZ;
Double_t tgy=(y1-y0)/delZ;
//if(checkDch)cout<<"tgx = "<<tgx<<", tgy = "<<tgy<<", delZ = "<<delZ<<", x[1] = "<<x[1]<<", x[0] = "<<x[0]<<", y[1] = "<<y[1]<<", y[0] = "<<y[0]<<endl;
if(checkDch)cout<<"tgx = "<<tgx<<", tgy = "<<tgy<<", delZ = "<<delZ<<", x[1] = "<<x1<<", x[0] = "<<x0<<", y[1] = "<<y1<<", y[0] = "<<y0<<endl;
/*if(idch==1){
tgx=-tgx;
tgy=-tgy;
}*/
Double_t xExtrap[numLayers],yExtrap[numLayers];

     for(UShort_t i = 0; i < numLayers; i++) {
       delZ2=(zLayerExtrap[idch][i]-zLayer_[idch][ijk[0]]);
       //xExtrap[i]=x[0]+tgx*delZ2+delXshift;
       xExtrap[i]=x0+tgx*delZ2;
       //xExtrap[i]=x[0]+detXshift[idch]+tgx*delZ2;
       //if(checkDch)cout<<"xExtrap[i] = "<<xExtrap[i]<<", x[i] = "<<x[i/2]<<", detXshift[idch] = "<<detXshift[idch]<<", x[i] + detXshift[idch] = "<<x[i/2] + detXshift[idch]<<", tgx*delZ2 = "<<tgx*delZ2<<endl;
       //yExtrap[i]=y[0]+tgy*delZ2;
       yExtrap[i]=y0+tgy*delZ2;
       //yExtrap[i]=y[0]+detYshift[idch]+tgy*delZ2;
       //if(checkDch)cout<<"yExtrap[i] = "<<yExtrap[i]<<", y[i] = "<<y[i/2]<<", detYshift[idch] = "<<detYshift[idch]<<", y[i] + detYshift[idch] = "<<y[i/2] + detYshift[idch]<<", tgy*delZ2 = "<<tgy*delZ2<<endl;
       hXYZcombhits->SetPoint(jjgr2++,zLayerExtrap[idch][i],xExtrap[i],yExtrap[i]);
       //cout<<"zLayerExtrap: "<<zLayerExtrap[idch][i]<<endl;
     }

}

//------------------------------------------------------------------------------------------------------------------------
/*void	BmnDchHitProducer_exp::rtCalibCorrection(){

TString str;
str.Form("%u",Iter-1);
TString residfile = TString("dchresidexp")+str+TString(".root");
TFile resid(residfile,"read");
UShort_t itertmp;

//ScalarD* Ranmin = (ScalarD*)resid.Get("Ranmin");
//ranmin=Ranmin->GetSV();

C0.ResizeTo(4,4);
ifstream myfile;
myfile.open("DCHpar");
myfile >> itertmp;
myfile >> ranmin;
myfile >> ranmax;
cout<<"RANMIN = "<<ranmin<<endl;
cout<<"RANMAX = "<<ranmax<<endl;
for (UShort_t j = 0; j < 4; j++) {
  for (UShort_t k = 0; k < 4; k++) {
      myfile >> C0(j,k);
  }
}
myfile.close();

//TH2D* residVsTDCy0 = (TH2D*)resid.Get("residVsTDCy0");
//TH2D* residVsTDCx1 = (TH2D*)resid.Get("residVsTDCx1");
TH2D* residVsTDC = (TH2D*)resid.Get("residVsTDC");

//TH1D* profResidVsTDCy0 = (TH1D*)residVsTDCy0->ProfileX("profResidVsTDCy0");
//TH1D* profResidVsTDCx1 = (TH1D*)residVsTDCx1->ProfileX("profResidVsTDCx1");
TH1D* profResidVsTDC = (TH1D*)residVsTDC->ProfileX("profResidVsTDC");
     
//TGraph *profResidY0=new TGraph(profResidVsTDCy0);
//TGraph *profResidX1=new TGraph(profResidVsTDCx1);
TGraph *profResid=new TGraph(profResidVsTDC);

calib=(TGraph*)resid.Get("calibr");
//Int_t npoints1=profResidY0->GetN();
//Int_t npoints2=profResidX1->GetN();
Int_t npoints=profResid->GetN();
Int_t npointsCalib=calib->GetN();

if(checkDch){
  //cout<<"npoints (res. Y0) = "<<npoints1<<"npoints (res. X1) = "<<npoints2<<"npoints (calib. gr.) = "<<npointsCalib<<endl;
  cout<<"npoints (residuals) = "<<npoints<<", npoints (calib. gr.) = "<<npointsCalib<<endl;
}

Double_t par[5];
profResid->Fit("pol4");
TF1 *myfit = (TF1*) profResid->GetFunction("pol4");
par[0]=myfit->GetParameter(0);
par[1]=myfit->GetParameter(1);
par[2]=myfit->GetParameter(2);
par[3]=myfit->GetParameter(3);
par[4]=myfit->GetParameter(4);

//Double_t* residY0=profResidY0->GetX();
//Double_t* residX1=profResidX1->GetX();
Double_t* residX=profResid->GetX();
//Double_t* calibGrX=calib[fDchNum-1]->GetX();
//Double_t* residY=profResid->GetY();
Double_t* calibGrY=calib->GetY();

for (Int_t ipoint = 0; ipoint < npoints; ipoint++) {

 //if(checkDch){
   //cout<<"rt graph (x):"<<*(calibGrX+ipoint)<<" "<<*(residX+ipoint)<<endl;
   //cout<<"rt graph (y):"<<*(calibGrY+ipoint)<<" "<<*(residY+ipoint)<<endl;
 //} 
   //*(calibGrY+ipoint)=*(calibGrY+ipoint)-*(residY+ipoint); 
   Double_t x=*(residX+ipoint); 
   *(calibGrY+ipoint)=*(calibGrY+ipoint)+(par[0]+par[1]*x+par[2]*pow(x,2.)+par[3]*pow(x,3.)+par[4]*pow(x,4.)); 
   if(*(calibGrY+ipoint)<0.)*(calibGrY+ipoint)=0.;
   if(*(calibGrY+ipoint)>0.5)*(calibGrY+ipoint)=0.5;

}

//calib[fDchNum-1] = new TGraph(npoints,dt,r);
//spline3rt = new TSpline3("r-t calibration curve",calib, "", 0., 0.);
spline5rt = new TSpline5("r-t calibration curve",calib, "", 0.,0.,0.,0.);
////calib->Fit("pol5");
//calib[fDchNum-1]->SetTitle("r-t calibration curve");
//calib[fDchNum-1]->SetName("rtCalibCurve");
spline5rt->SetName("rtCalibSpline");


}*/
//------------------------------------------------------------------------------------------------------------------------
void	BmnDchHitProducer_exp::rtCalibCorrection(){

/*TString str;
str.Form("%u",Iter-1);
TString strrun;
strrun.Form("%u",runNumber);
TString residFile = TString("dchresidexp")+TString(".")+runPeriod+TString(".")+strrun+TString("_")+str+TString(".root");*/
cout<<"residualsFileName_old = "<<residualsFileName_old<<endl;
TString outPut=outDirectory+residualsFileName_old;
//TFile resid(residfile,"read");
TFile resid(outPut,"read");

//ScalarD* Ranmin = (ScalarD*)resid.Get("Ranmin");
//ranmin=Ranmin->GetSV();

TString str2,str3,histname,calibname;
//TString map[4]={"X","Y","U","V"}; 
const UShort_t npar=5; 

for (UShort_t j = 0; j < numChambers; j++) {
 str2.Form("%d",j);
 str3.Form("%d",j+1);
 histname=TString("ResidVsTDC")+str3;
 TH2D* residVsTDC = (TH2D*)resid.Get(histname);
 //histname=TString("profResidVsTDC")+str2;
 //histname=TString("profResidVsTDC");
 //TH1D* profResidVsTDC = (TH1D*)residVsTDC->ProfileX(histname);
 TH1D* profResidVsTDC = (TH1D*)residVsTDC->ProfileX("profResidVsTDC");
 TGraph *profResid=new TGraph(profResidVsTDC);

 calibname=TString("calibr")+str2;
 calib_cham[j]=(TGraph*)resid.Get(calibname);
 Int_t npoints=profResid->GetN();
 Int_t npointsCalib=calib_cham[j]->GetN();
 if(checkDch)profResid->Print();
 if(checkDch){
   cout<<"npoints (residuals) = "<<npoints<<", npoints (calib. gr.) = "<<npointsCalib<<endl;
 }

 Double_t par[5];
 profResid->Fit("pol4");
 TF1 *myfit = (TF1*) profResid->GetFunction("pol4");
 par[0]=myfit->GetParameter(0);
 par[1]=myfit->GetParameter(1);
 par[2]=myfit->GetParameter(2);
 par[3]=myfit->GetParameter(3);
 par[4]=myfit->GetParameter(4);
 delete myfit; 

 Double_t* residX=profResid->GetX();
 //Double_t* calibGrX=calib[fDchNum-1]->GetX();
 //Double_t* residY=profResid->GetY();
 Double_t* calibGrY=calib_cham[j]->GetY();

 //for (Int_t ipoint = 0; ipoint < npoints; ipoint++) {
 for (Int_t ipoint = 0; ipoint < npointsCalib; ipoint++) {

  //if(checkDch){
    //cout<<"rt graph (x):"<<*(calibGrX+ipoint)<<" "<<*(residX+ipoint)<<endl;
    //cout<<"rt graph (y):"<<*(calibGrY+ipoint)<<" "<<*(residY+ipoint)<<endl;
  //} 
    //*(calibGrY+ipoint)=*(calibGrY+ipoint)-*(residY+ipoint); 
    //Double_t x=*(residX+ipoint); 
    Double_t x=*(residX+Int_t((Float_t(npoints)/Float_t(npointsCalib))*Float_t(ipoint))); 
    //cout<<x<<endl;
    *(calibGrY+ipoint)=*(calibGrY+ipoint)+(par[0]+par[1]*x+par[2]*pow(x,2.)+par[3]*pow(x,3.)+par[4]*pow(x,4.)); 
    if(*(calibGrY+ipoint)<0.)*(calibGrY+ipoint)=0.;
    if(*(calibGrY+ipoint)>0.5)*(calibGrY+ipoint)=0.5;

 }

 //calib[fDchNum-1] = new TGraph(npoints,dt,r);
 //spline3rt = new TSpline3("r-t calibration curve",calib, "", 0., 0.);
 spline5rt_cham[j] = new TSpline5("r-t calibration curve",calib_cham[j], "", 0.,0.,0.,0.);
 //if(fDoCheck)hList.Add(spline5rt_cham[j]); 
 hList.Add(spline5rt_cham[j]); 
 ////calib->Fit("pol5");
 //calib[fDchNum-1]->SetTitle("r-t calibration curve");
 //calib[fDchNum-1]->SetName("rtCalibCurve");
 spline5rt_cham[j]->SetName("rtCalibSpline");

//calibcurves for DCH planes

 histname=TString("ResidVsTDCy")+str3;
 TH2D* residVsTDCy = (TH2D*)resid.Get(histname);
 histname=TString("ResidVsTDCx")+str3;
 TH2D* residVsTDCx = (TH2D*)resid.Get(histname);
 histname=TString("ResidVsTDCu")+str3;
 TH2D* residVsTDCu = (TH2D*)resid.Get(histname);
 histname=TString("ResidVsTDCv")+str3;
 TH2D* residVsTDCv = (TH2D*)resid.Get(histname);

 TGraph *profResidPlane[4];
 TH1D* profResidVsTDCy = (TH1D*)residVsTDCy->ProfileX("profResidVsTDCy");
 profResidPlane[0]=new TGraph(profResidVsTDCy);
 TH1D* profResidVsTDCx = (TH1D*)residVsTDCx->ProfileX("profResidVsTDCx");
 profResidPlane[1]=new TGraph(profResidVsTDCx);
 TH1D* profResidVsTDCu = (TH1D*)residVsTDCu->ProfileX("profResidVsTDCu");
 profResidPlane[2]=new TGraph(profResidVsTDCu);
 TH1D* profResidVsTDCv = (TH1D*)residVsTDCv->ProfileX("profResidVsTDCv");
 profResidPlane[3]=new TGraph(profResidVsTDCv);


 for (UShort_t i = 0; i < numLayers_half; i++) {
 
  TString str4;
  str4.Form("%d",i);
  //if(Iter>1)calibname=TString("calibr")+str2+str4;
  calibname=TString("calibr")+str2+str4;
  cout<<calibname<<endl;
  calib[j][i]=(TGraph*)resid.Get(calibname);
  Int_t npointsCalib2=calib[j][i]->GetN();
   
  Double_t parPlane[npar];
  profResidPlane[i]->Fit("pol4");
  TF1 *myfit2 = (TF1*) profResidPlane[i]->GetFunction("pol4");

  for (UShort_t k = 0; k < npar; k++) {
   parPlane[k]=myfit2->GetParameter(k);
  }
  delete myfit2; 

  Double_t* residX2=profResid->GetX();
  Double_t* calibGrY2=calib[j][i]->GetY();
  for (Int_t ipoint = 0; ipoint < npointsCalib2; ipoint++) {

  //if(checkDch){
    //cout<<"rt graph (x):"<<*(calibGrX+ipoint)<<" "<<*(residX+ipoint)<<endl;
    //cout<<"rt graph (y):"<<*(calibGrY2+ipoint)<<" "<<*(residY+ipoint)<<endl;
  //} 
    //*(calibGrY2+ipoint)=*(calibGrY2+ipoint)-*(residY+ipoint); 
    //Double_t x=*(residX2+ipoint); 
    Double_t x=*(residX2+Int_t((Float_t(npoints)/Float_t(npointsCalib2))*Float_t(ipoint))); 
    *(calibGrY2+ipoint)=*(calibGrY2+ipoint)+(parPlane[0]+parPlane[1]*x+parPlane[2]*pow(x,2.)+parPlane[3]*pow(x,3.)+parPlane[4]*pow(x,4.)); 
    //*(calibGrY2+ipoint)=*(calibGrY2+ipoint)+0.1;
    if(*(calibGrY2+ipoint)<0.)*(calibGrY2+ipoint)=0.;
    if(*(calibGrY2+ipoint)>0.5)*(calibGrY2+ipoint)=0.5;

  }
 
 spline5rt[j][i] = new TSpline5("r-t calibration curve",calib[j][i], "", 0.,0.,0.,0.);
 //if(fDoCheck)hList.Add(spline5rt[j][i]); 
 hList.Add(spline5rt[j][i]); 
 spline5rt[j][i]->SetName("rtCalibSpline");


  profResidPlane[i]->Delete();
 }//cycle over DCH planes
 
 profResid->Delete();

}//cycle over chambers

resid.Close();


}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::MisAlignment(XYZPoint &xyzHit1,XYZPoint &xyzHit2,XYZPoint &xyzHit3,XYZPoint &xyzHit4,XYZPoint &xyzWire1,XYZPoint &xyzWire2,XYZPoint &xyzWire3,XYZPoint &xyzWire4){

//first chamber position
//XYZVector XYZ1(DCH1_Xpos[runPeriod-1],DCH1_Ypos[runPeriod-1],DCH1_Zpos[runPeriod-1]);
//XYZVector XYZ1(detXshift[0],detYshift[0],detZshift[0]);
//second chamber position
//XYZVector XYZ2(detXshift[1],detYshift[1],detZshift[1]);
//XYZVector XYZ2(DCH2_Xpos[runPeriod-1],DCH2_Ypos[runPeriod-1],DCH2_Zpos[runPeriod-1]);
XYZVector delXYZ21=XYZ2-XYZ1;

XYZVector v(delXYZ21.X(),delXYZ21.Y(),delXYZ21.Z());
EulerAngles r1(Pi()/10.,Pi()/40.,Pi()/30.);// misalignment rotation
XYZVector v1 = r1 * v;
XYZVector d(1.,-2.,3.);// misalignment translation vector

XYZPoint xyz5loc1,xyz6loc1,xyz7loc1,xyz8loc1;
XYZPoint xyz5loc1new,xyz6loc1new,xyz7loc1new,xyz8loc1new;
XYZPoint xyz5wireLoc1,xyz6wireLoc1,xyz7wireLoc1,xyz8wireLoc1;
XYZPoint xyz5wireLoc1new,xyz6wireLoc1new,xyz7wireLoc1new,xyz8wireLoc1new;

  // four points in the second chamber in the frame of the first chamber
  xyz5loc1=xyzHit1-XYZ1;
  xyz6loc1=xyzHit2-XYZ1;
  xyz7loc1=xyzHit3-XYZ1;
  xyz8loc1=xyzHit4-XYZ1;
  xyz5wireLoc1=xyzWire1-XYZ1;
  xyz6wireLoc1=xyzWire2-XYZ1;
  xyz7wireLoc1=xyzWire3-XYZ1;
  xyz8wireLoc1=xyzWire4-XYZ1;

  xyz5loc1new=r1*xyz5loc1+d;
  xyz6loc1new=r1*xyz6loc1+d;
  xyz7loc1new=r1*xyz7loc1+d;
  xyz8loc1new=r1*xyz8loc1+d;
  xyz5wireLoc1new=r1*xyz5wireLoc1+d;
  xyz6wireLoc1new=r1*xyz6wireLoc1+d;
  xyz7wireLoc1new=r1*xyz7wireLoc1+d;
  xyz8wireLoc1new=r1*xyz8wireLoc1+d;

  //returning to the original (global) frame
  xyzHit1=xyz5loc1new+XYZ1;
  xyzHit2=xyz6loc1new+XYZ1;
  xyzHit3=xyz7loc1new+XYZ1;
  xyzHit4=xyz8loc1new+XYZ1;
  xyzWire1=xyz5wireLoc1new+XYZ1;
  xyzWire2=xyz6wireLoc1new+XYZ1;
  xyzWire3=xyz7wireLoc1new+XYZ1;
  xyzWire4=xyz8wireLoc1new+XYZ1;

} 
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::AlignmentTracksFilter(Double_t &Xmin,Double_t &Xmax,Double_t &Ymin,Double_t &Ymax,UInt_t &filterTracks,Bool_t aliTrIndex[]){

using namespace ROOT::Math;

//first chamber position
//XYZVector XYZ1(DCH1_Xpos[runPeriod-1],DCH1_Ypos[runPeriod-1],DCH1_Zpos[runPeriod-1]);
//XYZVector XYZ1(detXshift[0],detYshift[0],detZshift[0]);
//second chamber position
//XYZVector XYZ2(detXshift[1],detYshift[1],detZshift[1]);
//XYZVector XYZ2(DCH2_Xpos[runPeriod-1],DCH2_Ypos[runPeriod-1],DCH2_Zpos[runPeriod-1]);

//cout<<XYZ1.X()<<" "<<XYZ2.X()<<endl;

XYZVector delXYZ21=XYZ2-XYZ1;
XYZVector v(delXYZ21.X(),delXYZ21.Y(),delXYZ21.Z());
cout<<v.X()<<" "<<v.Y()<<" "<<v.Z()<<" "<<endl;
Int_t ntracks1=fDchTrCand1Align->GetEntriesFast();
Int_t ntracks2=fDchTrCand2Align->GetEntriesFast();

//cout<<"ntracks1 = "<<ntracks1<<", ntracks2 = "<<ntracks2<<endl;
assert(ntracks1==ntracks2);
const Int_t ntracks=ntracks1;
cout<<"number of alignment tracks = "<<ntracks<<endl;

Double_t x1,y1,z1;
Double_t x2,y2,z2;
Double_t tgx,tgy;
Double_t x5,y5,z5;
UShort_t ijk5;
Double_t x5extrap,y5extrap;

XYZPoint xyz1,xyz5,xyz1loc,xyz5extrap,xyz5loc1,xyz5loc1new;

Double_t difX[ntracks],difY[ntracks];

const Int_t dim=16;
for (UInt_t l = 0; l < ntracks; l++) {
  TMatrix dch1Mat  = *((TMatrix*) fDchTrCand1Align->UncheckedAt(l));
  TMatrix dch2Mat  = *((TMatrix*) fDchTrCand2Align->UncheckedAt(l));
  // two points in the first chamber (global frame)
  x1=dch1Mat(0,0);
  y1=dch1Mat(0,1);
  z1=dch1Mat(0,2);
  x2=dch1Mat(1,0);
  y2=dch1Mat(1,1);
  z2=dch1Mat(1,2);
  tgx=(x2-x1)/(z2-z1);
  tgy=(y2-y1)/(z2-z1);
  // point in the second chamber (global frame)
  x5=dch2Mat(0,0);
  y5=dch2Mat(0,1);
  z5=dch2Mat(0,2);
  ijk5=dch2Mat(0,10);
  //x5extrap=x1+tgx*(zLayer[1][ijk5]-z1);
  //y5extrap=y1+tgy*(zLayer[1][ijk5]-z1);
  x5extrap=x1+tgx*(z5-z1);
  y5extrap=y1+tgy*(z5-z1);
  (xyz1).SetX(x1);(xyz1).SetY(y1);(xyz1).SetZ(z1);
  (xyz5).SetX(x5);(xyz5).SetY(y5);(xyz5).SetZ(z5);
  //(xyz5extrap).SetX(x5extrap);(xyz5extrap).SetY(y5extrap);(xyz5extrap).SetZ(zLayer[1][ijk5]);
  (xyz5extrap).SetX(x5extrap);(xyz5extrap).SetY(y5extrap);(xyz5extrap).SetZ(z5);
  // point in the first chamber (local frame)
  xyz1loc=xyz1-XYZ1;
  // point in the second chamber in the frame of the first chamber
  xyz5loc1=xyz5extrap-XYZ1;
  // point in the second chamber in the frame of the first chamber after rotation and translation
  xyz5loc1new=xyz5-XYZ1;
  hExtrapMeasDiffX->Fill(xyz5loc1new.X()-xyz5loc1.X()); 
  hExtrapMeasDiffY->Fill(xyz5loc1new.Y()-xyz5loc1.Y()); 
  difX[l]=xyz5loc1new.X()-xyz5loc1.X();
  difY[l]=xyz5loc1new.Y()-xyz5loc1.Y();
  //cout<<xyz5loc1new.X()<<" "<<xyz5loc1.X()<<endl;
}//track cycle
Int_t bin1X,bin2X,bin1Y,bin2Y;
UShort_t ii=2;
do{
 bin1X = hExtrapMeasDiffX->FindFirstBinAbove(hExtrapMeasDiffX->GetMaximum()/ii);
 bin2X = hExtrapMeasDiffX->FindLastBinAbove(hExtrapMeasDiffX->GetMaximum()/ii);
 ii=ii*2;
}while(bin1X==bin2X);
ii=2;
//cout<<hExtrapMeasDiffX->GetMaximum()/Float_t(ii)<<endl;
do{
 bin1Y = hExtrapMeasDiffY->FindFirstBinAbove(hExtrapMeasDiffY->GetMaximum()/ii);
 bin2Y = hExtrapMeasDiffY->FindLastBinAbove(hExtrapMeasDiffY->GetMaximum()/ii);
 ii=ii*2;
}while(bin1Y==bin2Y);
//cout<<hExtrapMeasDiffY->GetMaximum()/Float_t(ii)<<endl;
Xmin=hExtrapMeasDiffX->GetBinCenter(bin1X),Xmax=hExtrapMeasDiffX->GetBinCenter(bin2X),Ymin=hExtrapMeasDiffY->GetBinCenter(bin1Y),Ymax=hExtrapMeasDiffY->GetBinCenter(bin2Y);  

Double_t difXminmax=Xmax-Xmin,difYminmax=Ymax-Ymin;
cout<<"Xmin = "<<Xmin<<", Xmax = "<<Xmax<<", Ymin = "<<Ymin<<", Ymax = "<<Ymax<<endl;
if(runPeriod<=3){
 Xmin=Xmin+0.05*difXminmax;
 Xmax=Xmax-0.05*difXminmax;
 Ymin=Ymin+0.05*difYminmax;
 Ymax=Ymax-0.05*difYminmax;
}else if(runPeriod==6){
 Xmin=Xmin-difXminmax;
 Xmax=Xmax+difXminmax;
 Ymin=Ymin-difYminmax;
 Ymax=Ymax+difYminmax;
}
/*Xmin=Xmin+0.25*difXminmax;
Xmax=Xmax-0.25*difXminmax;
Ymin=Ymin+0.25*difYminmax;
Ymax=Ymax-0.25*difYminmax;*/
/*Xmin=Xmin-difXminmax;
Xmax=Xmax+difXminmax;
Ymin=Ymin-difYminmax;
Ymax=Ymax+difYminmax;*/
//UInt_t filterTracks=0,aliTrIndex[ntracks];
for (UInt_t l = 0; l < ntracks; l++) {
   aliTrIndex[l]=false; 
   if(!(difX[l]>Xmin&&difX[l]<Xmax))continue;
   if(!(difY[l]>Ymin&&difY[l]<Ymax))continue;
   aliTrIndex[l]=true;
   filterTracks++;
}
cout<<"corrected number of alignment tracks = "<<filterTracks<<endl;

cout<<" alignment tracks filter (xmin,xmax,ymin,ymax): "<<Xmin<<" "<<Xmax<<" "<<" "<<Ymin<<" "<<Ymax<<endl;

if(filterTracks>12000)filterTracks=12000;

}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::AlignmentMatrix(){

using namespace ROOT::Math;

//first chamber position
//XYZVector XYZ1(DCH1_Xpos[runPeriod-1],DCH1_Ypos[runPeriod-1],DCH1_Zpos[runPeriod-1]);
//XYZVector XYZ1(detXshift[0],detYshift[0],detZshift[0]);
//second chamber position
//XYZVector XYZ2(detXshift[1],detYshift[1],detZshift[1]);
//XYZVector XYZ2(DCH2_Xpos[runPeriod-1],DCH2_Ypos[runPeriod-1],DCH2_Zpos[runPeriod-1]);

//cout<<XYZ1.X()<<" "<<XYZ2.X()<<endl;

//XYZVector delXYZ21=XYZ2-XYZ1;
//XYZVector v(delXYZ21.X(),delXYZ21.Y(),delXYZ21.Z());
//cout<<v.X()<<" "<<v.Y()<<" "<<v.Z()<<" "<<endl;;
Int_t ntracks1=fDchTrCand1Align->GetEntriesFast();
Int_t ntracks2=fDchTrCand2Align->GetEntriesFast();

assert(ntracks1==ntracks2);
const Int_t ntracks=ntracks1;
//cout<<"number of alignment tracks = "<<ntracks<<endl;

Double_t Xmin,Xmax,Ymin,Ymax;
//UInt_t filterTracks=ntracks;
UInt_t filterTracks=0;
Bool_t aliTrIndex[ntracks];
AlignmentTracksFilter(Xmin,Xmax,Ymin,Ymax,filterTracks,aliTrIndex);

Double_t x1[filterTracks],y1[filterTracks],z1[filterTracks];
Double_t x2[filterTracks],y2[filterTracks],z2[filterTracks];
//Double_t x3[filterTracks],y3[filterTracks],z3[filterTracks];
//Double_t x4[filterTracks],y4[filterTracks],z4[filterTracks];
Double_t tgx[filterTracks],tgy[filterTracks];
Double_t x5[filterTracks],y5[filterTracks],z5[filterTracks];
UShort_t ijk5[filterTracks],ijk6[filterTracks],ijk7[filterTracks],ijk8[filterTracks];
Double_t x6[filterTracks],y6[filterTracks],z6[filterTracks];
Double_t x7[filterTracks],y7[filterTracks],z7[filterTracks];
Double_t x8[filterTracks],y8[filterTracks],z8[filterTracks];
Double_t x5extrap[filterTracks],y5extrap[filterTracks];
Double_t x6extrap[filterTracks],y6extrap[filterTracks];
Double_t x7extrap[filterTracks],y7extrap[filterTracks];
Double_t x8extrap[filterTracks],y8extrap[filterTracks];

//XYZPoint xyz1[filterTracks],xyz2[filterTracks];
//xyz3[filterTracks],xyz4[filterTracks];
//XYZPoint xyz1loc[filterTracks],xyz2loc[filterTracks];
//xyz3loc[filterTracks],xyz4loc[filterTracks];
XYZPoint xyz5[filterTracks],xyz6[filterTracks],xyz7[filterTracks],xyz8[filterTracks];
XYZPoint xyz5extrap[filterTracks],xyz6extrap[filterTracks],xyz7extrap[filterTracks],xyz8extrap[filterTracks];
XYZPoint xyz5loc[filterTracks],xyz6loc[filterTracks],xyz7loc[filterTracks],xyz8loc[filterTracks];
XYZPoint xyz5loc1[filterTracks],xyz6loc1[filterTracks],xyz7loc1[filterTracks],xyz8loc1[filterTracks];
XYZPoint xyz5loc1new[filterTracks],xyz6loc1new[filterTracks],xyz7loc1new[filterTracks],xyz8loc1new[filterTracks];

std::vector<double> k1;

XYZVector XYZ;
//TVectorD tmpXYZ1(4),tmpDXYZ1(4);

Double_t xShift1,yShift1,xShift2,yShift2;

  XYZ=XYZ1;


const Int_t dim=16;
//Int_t dim=16,fulllldim=dim*ntracks;
/*TVectorD k(fulllldim);  //vector of corrected coordinates
TMatrixD m(fulllldim,dim);
for (UInt_t j = 0; j < fulllldim; j++) {
    k(j)=0.;
    for (UInt_t ll = 0; ll < dim; ll++) {
      m(j,ll)=0.;
    }
}*/
TClonesArray  *fMatrix = new TClonesArray("TMatrix");
TClonesArray &ffMatrix = *fMatrix;
UInt_t ll=0;
xShift1=meanShiftX[0];
yShift1=meanShiftY[0];
xShift2=meanShiftX[1];
yShift2=meanShiftY[1];

//xShift1=0.; yShift1=0.; xShift2=0.; yShift2=0.;
//cout<<"xyShifts: "<<xShift1<<" "<<xShift2<<" "<<yShift1<<" "<<yShift2<<" "<<endl;

for (UInt_t l = 0; l < ntracks; l++) {
  if(!aliTrIndex[l])continue;
  TMatrixD m1(dim,dim);
//for (UInt_t ll = 0; ll < ntracks/2; ll++) {
  TMatrix dch1Mat  = *((TMatrix*) fDchTrCand1Align->UncheckedAt(l));
  TMatrix dch2Mat  = *((TMatrix*) fDchTrCand2Align->UncheckedAt(l));
  // four points in the first chamber (global frame)
  //if(isGlobalAlignment){
   /*x1[ll]=dch1Mat(0,0)+xDifSE[0];
   y1[ll]=dch1Mat(0,1)+yDifSE[0];
   x2[ll]=dch1Mat(1,0)+xDifSE[1];
   y2[ll]=dch1Mat(1,1)+yDifSE[1];*/
   x1[ll]=dch1Mat(0,0)+xShift1;
   y1[ll]=dch1Mat(0,1)+yShift1;
   x2[ll]=dch1Mat(1,0)+xShift1;
   y2[ll]=dch1Mat(1,1)+yShift1;
   /*x1[ll]=dch1Mat(0,0);
   y1[ll]=dch1Mat(0,1);
   x2[ll]=dch1Mat(1,0);
   y2[ll]=dch1Mat(1,1);*/
  /*}else{
   x1[ll]=dch1Mat(0,0);
   y1[ll]=dch1Mat(0,1);
   x2[ll]=dch1Mat(1,0);
   y2[ll]=dch1Mat(1,1);
  }*/
  z1[ll]=dch1Mat(0,2);
  z2[ll]=dch1Mat(1,2);
  tgx[ll]=(x2[ll]-x1[ll])/(z2[ll]-z1[ll]);
  //cout<<"tgx = "<<TMath::RadToDeg()*tgx[ll]<<", "<<x2[ll]<<" "<<x1[ll]<<" "<<x2[ll]-x1[ll]<<endl;
  tgy[ll]=(y2[ll]-y1[ll])/(z2[ll]-z1[ll]);
  //cout<<"tgy = "<<TMath::RadToDeg()*tgy[ll]<<", "<<y2[ll]<<" "<<y1[ll]<<" "<<y2[ll]-y1[ll]<<endl;
  //cout<<"z: "<<z2[ll]<<" "<<z1[ll]<<" "<<z2[ll]-z1[ll]<<endl;
  /*x3[ll]=dch1Mat(2,0);
  y3[ll]=dch1Mat(2,1);
  z3[ll]=dch1Mat(2,2);
  x4[ll]=dch1Mat(3,0);
  y4[ll]=dch1Mat(3,1);
  z4[ll]=dch1Mat(3,2);*/
  //tgx[ll]=(x4[ll]-x1[ll])/(z4[ll]-z1[ll]);
  //tgy[ll]=(y4[ll]-y1[ll])/(z4[ll]-z1[ll]);
  // four points in the second chamber (global frame)
  x5[ll]=dch2Mat(0,0)+xShift2;
  y5[ll]=dch2Mat(0,1)+yShift2;
  z5[ll]=dch2Mat(0,2);
  ijk5[ll]=dch2Mat(0,10);
  x6[ll]=dch2Mat(1,0)+xShift2;
  y6[ll]=dch2Mat(1,1)+yShift2;
  z6[ll]=dch2Mat(1,2);
  ijk6[ll]=dch2Mat(1,10);
  x7[ll]=dch2Mat(2,0)+xShift2;
  y7[ll]=dch2Mat(2,1)+yShift2;
  z7[ll]=dch2Mat(2,2);
  ijk7[ll]=dch2Mat(2,10);
  x8[ll]=dch2Mat(3,0)+xShift2;
  y8[ll]=dch2Mat(3,1)+yShift2;
  z8[ll]=dch2Mat(3,2);
  ijk8[ll]=dch2Mat(3,10);
  //x5extrap[ll]=x1[ll]+tgx[ll]*(zLayer[1][ijk5[ll]]-z1[ll]);
  //y5extrap[ll]=y1[ll]+tgy[ll]*(zLayer[1][ijk5[ll]]-z1[ll]);
  x5extrap[ll]=x1[ll]+tgx[ll]*(z5[ll]-z1[ll]);
  y5extrap[ll]=y1[ll]+tgy[ll]*(z5[ll]-z1[ll]);
  //cout<<"extrapolated (plane 1, global frame): "<<x5extrap[ll]<<" "<<y5extrap[ll]<<" "<<zLayer[1][ijk5[ll]]<<" "<<endl;
  //cout<<x1[ll]<<" "<<tgx[ll]<<" "<<tgx[ll]*(zLayer[1][ijk5[ll]]-z1[ll])<<endl;
  //cout<<y1[ll]<<" "<<tgy[ll]<<" "<<tgy[ll]*(zLayer[1][ijk5[ll]]-z1[ll])<<endl;
  //cout<<zLayer[1][ijk5[ll]]<<" "<<z1[ll]<<" "<<zLayer[1][ijk5[ll]]-z1[ll]<<endl;
  /*x6extrap[ll]=x1[ll]+tgx[ll]*(zLayer[1][ijk6[ll]]-z1[ll]);
  y6extrap[ll]=y1[ll]+tgy[ll]*(zLayer[1][ijk6[ll]]-z1[ll]);
  x7extrap[ll]=x1[ll]+tgx[ll]*(zLayer[1][ijk7[ll]]-z1[ll]);
  y7extrap[ll]=y1[ll]+tgy[ll]*(zLayer[1][ijk7[ll]]-z1[ll]);
  x8extrap[ll]=x1[ll]+tgx[ll]*(zLayer[1][ijk8[ll]]-z1[ll]);
  y8extrap[ll]=y1[ll]+tgy[ll]*(zLayer[1][ijk8[ll]]-z1[ll]);*/
  x6extrap[ll]=x1[ll]+tgx[ll]*(z6[ll]-z1[ll]);
  y6extrap[ll]=y1[ll]+tgy[ll]*(z6[ll]-z1[ll]);
  x7extrap[ll]=x1[ll]+tgx[ll]*(z7[ll]-z1[ll]);
  y7extrap[ll]=y1[ll]+tgy[ll]*(z7[ll]-z1[ll]);
  x8extrap[ll]=x1[ll]+tgx[ll]*(z8[ll]-z1[ll]);
  y8extrap[ll]=y1[ll]+tgy[ll]*(z8[ll]-z1[ll]);
  //(xyz1[ll]).SetX(x1[ll]);(xyz1[ll]).SetY(y1[ll]);(xyz1[ll]).SetZ(z1[ll]);
  //(xyz2[ll]).SetX(x2[ll]);(xyz2[ll]).SetY(y2[ll]);(xyz2[ll]).SetZ(z2[ll]);
  //(xyz3[ll]).SetX(x3[ll]);(xyz3[ll]).SetY(y3[ll]);(xyz3[ll]).SetZ(z3[ll]);
  //(xyz4[ll]).SetX(x4[ll]);(xyz4[ll]).SetY(y4[ll]);(xyz4[ll]).SetZ(z4[ll]);
  (xyz5[ll]).SetX(x5[ll]);(xyz5[ll]).SetY(y5[ll]);(xyz5[ll]).SetZ(z5[ll]);
  (xyz6[ll]).SetX(x6[ll]);(xyz6[ll]).SetY(y6[ll]);(xyz6[ll]).SetZ(z6[ll]);
  (xyz7[ll]).SetX(x7[ll]);(xyz7[ll]).SetY(y7[ll]);(xyz7[ll]).SetZ(z7[ll]);
  (xyz8[ll]).SetX(x8[ll]);(xyz8[ll]).SetY(y8[ll]);(xyz8[ll]).SetZ(z8[ll]);
  /*(xyz5extrap[ll]).SetX(x5extrap[ll]);(xyz5extrap[ll]).SetY(y5extrap[ll]);(xyz5extrap[ll]).SetZ(zLayer[1][ijk5[ll]]);
  (xyz6extrap[ll]).SetX(x6extrap[ll]);(xyz6extrap[ll]).SetY(y6extrap[ll]);(xyz6extrap[ll]).SetZ(zLayer[1][ijk6[ll]]);
  (xyz7extrap[ll]).SetX(x7extrap[ll]);(xyz7extrap[ll]).SetY(y7extrap[ll]);(xyz7extrap[ll]).SetZ(zLayer[1][ijk7[ll]]);
  (xyz8extrap[ll]).SetX(x8extrap[ll]);(xyz8extrap[ll]).SetY(y8extrap[ll]);(xyz8extrap[ll]).SetZ(zLayer[1][ijk8[ll]]);*/
  (xyz5extrap[ll]).SetX(x5extrap[ll]);(xyz5extrap[ll]).SetY(y5extrap[ll]);(xyz5extrap[ll]).SetZ(z5[ll]);
  (xyz6extrap[ll]).SetX(x6extrap[ll]);(xyz6extrap[ll]).SetY(y6extrap[ll]);(xyz6extrap[ll]).SetZ(z6[ll]);
  (xyz7extrap[ll]).SetX(x7extrap[ll]);(xyz7extrap[ll]).SetY(y7extrap[ll]);(xyz7extrap[ll]).SetZ(z7[ll]);
  (xyz8extrap[ll]).SetX(x8extrap[ll]);(xyz8extrap[ll]).SetY(y8extrap[ll]);(xyz8extrap[ll]).SetZ(z8[ll]);
  // four points in the first chamber (local frame)
  //xyz1loc[ll]=xyz1[ll]-XYZ;
  //cout<<" measured in DCH1 (plane 1, DCH1 frame): "<<xyz1loc[ll].X()<<" "<<xyz1loc[ll].Y()<<" "<<xyz1loc[ll].Z()<<" "<<endl;
  //xyz2loc[ll]=xyz2[ll]-XYZ;
  //xyz3loc[ll]=xyz3[ll]-XYZ;
  //xyz4loc[ll]=xyz4[ll]-XYZ;
  // four points in the second chamber (local frame)
  //xyz5loc[ll]=xyz5[ll]-XYZ2;
  //xyz6loc[ll]=xyz6[ll]-XYZ2;
  //xyz7loc[ll]=xyz7[ll]-XYZ2;
  //xyz8loc[ll]=xyz8[ll]-XYZ2;
  // four points in the second chamber in the frame of the first chamber
  xyz5loc1[ll]=xyz5extrap[ll]-XYZ;
  xyz6loc1[ll]=xyz6extrap[ll]-XYZ;
  xyz7loc1[ll]=xyz7extrap[ll]-XYZ;
  xyz8loc1[ll]=xyz8extrap[ll]-XYZ;
  //cout<<"extrapolated (plane 1, DCH1 frame): "<<xyz5loc1[ll].X()<<" "<<xyz5loc1[ll].Y()<<" "<<xyz5loc1[ll].Z()<<" "<<endl;
  // four points in the second chamber in the frame of the first chamber after rotation and transllation
  xyz5loc1new[ll]=xyz5[ll]-XYZ;
  xyz6loc1new[ll]=xyz6[ll]-XYZ;
  xyz7loc1new[ll]=xyz7[ll]-XYZ;
  xyz8loc1new[ll]=xyz8[ll]-XYZ;
  //cout<<"measured in DCH2 (plane 1, DCH1 frame): "<<xyz5loc1new[ll].X()<<" "<<xyz5loc1new[ll].Y()<<" "<<xyz5loc1new[ll].Z()<<" "<<endl;
  //hExtrapMeasDiffX->Fill(xyz5loc1new[ll].X()-xyz5loc1[ll].X()); 
  //hExtrapMeasDiffY->Fill(xyz5loc1new[ll].Y()-xyz5loc1[ll].Y()); 
  //if(!((xyz5loc1new[ll].X()-xyz5loc1[ll].X())>-10.&&(xyz5loc1new[ll].X()-xyz5loc1[ll].X())<-9.))continue;
  //if(!((xyz5loc1new[ll].X()-xyz5loc1[ll].X())>-4.&&(xyz5loc1new[ll].X()-xyz5loc1[ll].X())<-2.))continue;
  //if(!((xyz5loc1new[ll].Y()-xyz5loc1[ll].Y())>2.&&(xyz5loc1new[ll].Y()-xyz5loc1[ll].Y())<3.))continue;
  //if(!((xyz5loc1new[ll].X()-xyz5loc1[ll].X())>Xmin&&(xyz5loc1new[ll].X()-xyz5loc1[ll].X())<Xmax))continue;
  //if(!((xyz5loc1new[ll].Y()-xyz5loc1[ll].Y())>Ymin&&(xyz5loc1new[ll].Y()-xyz5loc1[ll].Y())<Ymax))continue;
  //cout<<xyz5loc1.X()<<" "<<xyz5loc1new.X()<<" "<<xyz5loc1.Y()<<" "<<xyz5loc1new.Y()<<" "<<xyz5loc1.Z()<<" "<<xyz5loc1new.Z()<<endl;
  /*k(0+ll*dim)=xyz5loc1[ll].X();
  k(1+ll*dim)=xyz5loc1[ll].Y();
  k(2+ll*dim)=xyz5loc1[ll].Z();
  k(3+ll*dim)=1.;
  k(4+ll*dim)=xyz6loc1[ll].X();
  k(5+ll*dim)=xyz6loc1[ll].Y();
  k(6+ll*dim)=xyz6loc1[ll].Z();
  k(7+ll*dim)=1.;
  k(8+ll*dim)=xyz7loc1[ll].X();
  k(9+ll*dim)=xyz7loc1[ll].Y();
  k(10+ll*dim)=xyz7loc1[ll].Z();
  k(11+ll*dim)=1.;
  k(12+ll*dim)=xyz8loc1[ll].X();
  k(13+ll*dim)=xyz8loc1[ll].Y();
  k(14+ll*dim)=xyz8loc1[ll].Z();
  k(15+ll*dim)=1.;*/
  k1.push_back(xyz5loc1[ll].X());
  k1.push_back(xyz5loc1[ll].Y());
  k1.push_back(xyz5loc1[ll].Z());
  k1.push_back(1.);
  k1.push_back(xyz6loc1[ll].X());
  k1.push_back(xyz6loc1[ll].Y());
  k1.push_back(xyz6loc1[ll].Z());
  k1.push_back(1.);
  k1.push_back(xyz7loc1[ll].X());
  k1.push_back(xyz7loc1[ll].Y());
  k1.push_back(xyz7loc1[ll].Z());
  k1.push_back(1.);
  k1.push_back(xyz8loc1[ll].X());
  k1.push_back(xyz8loc1[ll].Y());
  k1.push_back(xyz8loc1[ll].Z());
  k1.push_back(1.); 
  /*if(checkMatrices){
   if(ll==(ntracks-1)){
    cout<<"k vector:"<<endl;
    k.Print();
   }
  }*/
  /*UInt_t kk=0;
  for (UInt_t i = 0; i < 4; i++) {
   m(i+ll*dim,0+4*kk)=xyz5loc1new[ll].X();
   m(i+ll*dim,1+4*kk)=xyz5loc1new[ll].Y();
   m(i+ll*dim,2+4*kk)=xyz5loc1new[ll].Z();
   m(i+ll*dim,3+4*kk)=1.;
   kk++;
  }
  kk=0;
  for (UInt_t i = 4; i < 8; i++) {
   m(i+ll*dim,0+4*kk)=xyz6loc1new[ll].X();
   m(i+ll*dim,1+4*kk)=xyz6loc1new[ll].Y();
   m(i+ll*dim,2+4*kk)=xyz6loc1new[ll].Z();
   m(i+ll*dim,3+4*kk)=1.;
   kk++;
  }
  kk=0;
  for (UInt_t i = 8; i < 12; i++) {
   m(i+ll*dim,0+4*kk)=xyz7loc1new[ll].X();
   m(i+ll*dim,1+4*kk)=xyz7loc1new[ll].Y();
   m(i+ll*dim,2+4*kk)=xyz7loc1new[ll].Z();
   m(i+ll*dim,3+4*kk)=1.;
   kk++;
  }
  kk=0;
  for (UInt_t i = 12; i < 16; i++) {
   m(i+ll*dim,0+4*kk)=xyz8loc1new[ll].X();
   m(i+ll*dim,1+4*kk)=xyz8loc1new[ll].Y();
   m(i+ll*dim,2+4*kk)=xyz8loc1new[ll].Z();
   m(i+ll*dim,3+4*kk)=1.;
   kk++;
  }*/
  UInt_t kk=0;
  for (UInt_t i = 0; i < 4; i++) {
   m1(i,0+4*kk)=xyz5loc1new[ll].X();
   m1(i,1+4*kk)=xyz5loc1new[ll].Y();
   m1(i,2+4*kk)=xyz5loc1new[ll].Z();
   m1(i,3+4*kk)=1.;
   kk++;
  }
  kk=0;
  for (UInt_t i = 4; i < 8; i++) {
   m1(i,0+4*kk)=xyz6loc1new[ll].X();
   m1(i,1+4*kk)=xyz6loc1new[ll].Y();
   m1(i,2+4*kk)=xyz6loc1new[ll].Z();
   m1(i,3+4*kk)=1.;
   kk++;
  }
  kk=0;
  for (UInt_t i = 8; i < 12; i++) {
   m1(i,0+4*kk)=xyz7loc1new[ll].X();
   m1(i,1+4*kk)=xyz7loc1new[ll].Y();
   m1(i,2+4*kk)=xyz7loc1new[ll].Z();
   m1(i,3+4*kk)=1.;
   kk++;
  }
  kk=0;
  for (UInt_t i = 12; i < 16; i++) {
   m1(i,0+4*kk)=xyz8loc1new[ll].X();
   m1(i,1+4*kk)=xyz8loc1new[ll].Y();
   m1(i,2+4*kk)=xyz8loc1new[ll].Z();
   m1(i,3+4*kk)=1.;
   kk++;
  }
 new (ffMatrix[ll]) TMatrix(m1);
 ll++;
 //cout<<"ll = "<<ll<<endl;
  /*if(checkMatrices){
   if(l==(ntracks-1)){
    cout<<"m matrix:"<<endl;
    m.Print();
   }
  }*/
}//track cycle
//cout<<"corrected number of alignment tracks = "<<ll<<endl;
Int_t fulldim=dim*ll;
TVectorD k(fulldim);  //vector of corrected coordinates
for (UInt_t l = 0; l < ll; l++) {
 for (Int_t j = 0; j < dim; j++) {
  k(j+l*dim)=k1[j+l*dim];
 }
}
if(checkMatrices){
  cout<<"k vector:"<<endl;
  k.Print();
}
k1.clear();
TMatrixD m(fulldim,dim);
for (UInt_t j = 0; j < fulldim; j++) {
    for (UInt_t l = 0; l < dim; l++) {
      m(j,l)=0.;
    }
}
for (UInt_t l = 0; l < ll; l++) {
 TMatrix tmpmat  = *((TMatrix*) ffMatrix.UncheckedAt(l));
 //cout<<"m1 matrix:"<<endl;
 //tmpmat.Print();
 for (Int_t j = 0; j < dim; j++) {
  for (Int_t kk = 0; kk < dim; kk++) {
   m(j+l*dim,kk)=tmpmat(j,kk);
  }
 }
}
fMatrix->Delete();
delete fMatrix;
fMatrix=0;
if(checkMatrices){
  cout<<"m matrix:"<<endl;
  m.Print();
}
//cout<<"position of 2nd chamber relative to 1st chamber: "<<delX<<" "<<delY<<" "<<delZ<<endl;

/*TDecompLU lu(m);
//lu.SetTol(1.0e-12);
//lu.SetTol(1.0e-15);
lu.SetTol(1.0e-20);
lu.Solve(k);
k.Print();
Int_t dim0=Int_t(sqrt(Double_t(dim)));
TMatrixD C(dim0,dim0);
//C(0,0)=k(0);C(0,1)=k(1);C(0,2)=k(2);
//C(1,0)=k(3);C(1,1)=k(4);C(1,2)=k(5);
//C(2,0)=k(6);C(2,1)=k(7);C(2,2)=k(8);
C(0,0)=k(0);C(0,1)=k(1);C(0,2)=k(2);C(0,3)=k(3);
C(1,0)=k(4);C(1,1)=k(5);C(1,2)=k(6);C(1,3)=k(7);
C(2,0)=k(8);C(2,1)=k(9);C(2,2)=k(10);C(2,3)=k(11);
C(3,0)=k(12);C(3,1)=k(13);C(3,2)=k(14);C(3,3)=k(15);
//TMatrixD Cinv=C.Invert(&det);
cout<<"C matrix:"<<endl;
C.Print();
//Cinv.Print();
//cout<<"determinant of inverted matrix = "<<det<<endl;
TVectorD test(dim0);
test(0)=xyz5loc1new.X();test(1)=xyz5loc1new.Y();test(2)=xyz5loc1new.Z();test(3)=1.;
TVectorD test2(dim0);
test2=C*test;
cout<<"test vector:"<<endl;
test.Print();
cout<<"test2 vector:"<<endl;
test2.Print();
cout<<xyz5loc1new.X()*C(0,0)+xyz5loc1new.Y()*C(0,1)+xyz5loc1new.Z()*C(0,2)+C(0,3)<<" "<<xyz5loc1new.X()*C(1,0)+xyz5loc1new.Y()*C(1,1)+xyz5loc1new.Z()*C(1,2)+C(1,3)<<" "<<xyz5loc1new.X()*C(2,0)+xyz5loc1new.Y()*C(2,1)+xyz5loc1new.Z()*C(2,2)+C(2,3)<<endl;
cout<<xyz5loc1.X()<<" "<<xyz5loc1.Y()<<" "<<xyz5loc1.Z()<<endl;*/
TMatrixD mt(dim,fulldim);
mt.Transpose(m); // transposed C matrix
if(checkMatrices){
 cout<<"m transposed matrix:"<<endl;
 mt.Print();
}
TMatrixD mtm(dim,dim);
mtm=mt*m;
if(checkMatrices){
 cout<<"m transposed matrix * m matrix:"<<endl;
 (mtm).Print();
}
Double_t det;
mtm.Invert(&det);
if(checkMatrices){
 cout<<"inverted (m transposed matrix * m matrix):"<<endl;
 cout<<"determinant = "<<det<<endl;
 mtm.Print();
}
TVectorD mtk(dim);
mtk=mt*k;
if(checkMatrices)mtk.Print();
TVectorD xvec(dim);
xvec=mtm*mtk;
if(checkMatrices)xvec.Print();
//Int_t dim0=Int_t(sqrt(Double_t(dim)));
//TMatrixD C(dim0,dim0);
C(0,0)=xvec(0);C(0,1)=xvec(1);C(0,2)=xvec(2);C(0,3)=xvec(3);
C(1,0)=xvec(4);C(1,1)=xvec(5);C(1,2)=xvec(6);C(1,3)=xvec(7);
C(2,0)=xvec(8);C(2,1)=xvec(9);C(2,2)=xvec(10);C(2,3)=xvec(11);
C(3,0)=xvec(12);C(3,1)=xvec(13);C(3,2)=xvec(14);C(3,3)=xvec(15);
/*if(Iter==0){
 C(0,0)=1.;
 C(1,1)=1.;
 C(2,2)=1.;
}*/ 
/*C[0][0]=xvec(0);C[0][1]=xvec(1);C[0][2]=xvec(2);C[0][3]=xvec(3);
C[1][0]=xvec(4);C[1][1]=xvec(5);C[1][2]=xvec(6);C[1][3]=xvec(7);
C[2][0]=xvec(8);C[2][1]=xvec(9);C[2][2]=xvec(10);C[2][3]=xvec(11);
C[3][0]=xvec(12);C[3][1]=xvec(13);C[3][2]=xvec(14);C[3][3]=xvec(15);*/
cout<<"transformation matrix C:"<<endl;
C.Print();
cout<<"determinant of C matrix = "<<C.Determinant()<<endl;
/*
cout<<"rotation matrix tests:"<<endl;
TMatrixD Crot(3,3);
Crot(0,0)=xvec(0);Crot(0,1)=xvec(1);Crot(0,2)=xvec(2);
Crot(1,0)=xvec(4);Crot(1,1)=xvec(5);Crot(1,2)=xvec(6);
Crot(2,0)=xvec(8);Crot(2,1)=xvec(9);Crot(2,2)=xvec(10);
//cout<<"sum = "<<Crot.Sum()<<endl;
TMatrixD Crott(3,3);
Crott.Transpose(Crot); // transposed C matrix
cout<<"Crot*Crott = "<<endl;
(Crot*Crott).Print();
Crot.Invert(&det);
cout<<"determinant of rotation matrix = "<<det<<endl;
*/
/*
TMatrixD Crot(3,3);
Crot(0,0)=xvec(0);Crot(0,1)=xvec(1);Crot(0,2)=xvec(2);
Crot(1,0)=xvec(4);Crot(1,1)=xvec(5);Crot(1,2)=xvec(6);
Crot(2,0)=xvec(8);Crot(2,1)=xvec(9);Crot(2,2)=xvec(10);
const TMatrixD &c=Crot;
TDecompQRH Cdec(c);
Cdec.Decompose();
TMatrixD R=Cdec.GetR();
TMatrixD Rinv(3,3);
Rinv=R.Invert();
TMatrixD Cnew(3,3); //Cnew=Q
Cnew=Crot*Rinv;
cout<<"determinant of rotation matrix = "<<Cnew.Determinant()<<endl;
//C(0,0)=-Cnew(0,0);C(0,1)=Cnew(0,1);C(0,2)=Cnew(0,2);
//C(1,0)=-Cnew(1,0);C(1,1)=-Cnew(1,1);C(1,2)=Cnew(1,2);
C(0,0)=Cnew(0,0);C(0,1)=Cnew(0,1);C(0,2)=Cnew(0,2);
C(1,0)=Cnew(1,0);C(1,1)=Cnew(1,1);C(1,2)=Cnew(1,2);
C(2,0)=Cnew(2,0);C(2,1)=Cnew(2,1);C(2,2)=Cnew(2,2);
cout<<"determinant of C matrix = "<<C.Determinant()<<endl;
C.Print();
*/
/*
C(0,0)=sqrt(1.-pow(C(1,0),2.));
C(1,1)=sqrt(1.-pow(C(0,1),2.));
C(2,2)=sqrt(1.-pow(C(0,2),2.)-pow(C(1,2),2.));
*/
/*C(0,0)=1.;
C(1,1)=1.;*/
/*
const TMatrixD &c=C;
cout<<"determinant of C matrix = "<<c.Determinant()<<endl;
TDecompQRH Cdec(c);
Cdec.Decompose();
TMatrixD R=Cdec.GetR();
TMatrixD Rinv(4,4);
Rinv=R.Invert();
TMatrixD Cnew(4,4); //Cnew=Q
Cnew=C*Rinv;
//Cnew.Print();
cout<<"determinant of Q (C corrected) matrix = "<<Cnew.Determinant()<<endl;
C=Cnew;
C.Print();
*/
// testing transformation matrix
/*
for (UInt_t l = 0; l < ntracks; l++) {
 TVectorD test(dim0);
 test(0)=xyz5loc1new[l].X();test(1)=xyz5loc1new[l].Y();test(2)=xyz5loc1new[l].Z();test(3)=1.;
 cout<<"misaligned point:"<<endl;
 //test.Print();
 cout<<test(0)<<" "<<test(1)<<" "<<test(2)<<endl;
 TVectorD test2(dim0);
 test2=C*test;
 cout<<"corrected point:"<<endl;
 cout<<test2(0)<<" "<<test2(1)<<" "<<test2(2)<<endl;
 //test2.Print();
 cout<<"extrapolated point:"<<endl;
 cout<<xyz5loc1[l].X()<<" "<<xyz5loc1[l].Y()<<" "<<xyz5loc1[l].Z()<<endl;
 cout<<"-----------------------------------------------------------------"<<endl;
}*/
}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::GlobalAlignmentParameters(Bool_t &noError){

/*if(!isGeant){
  for(UShort_t j=0; j<2; j++) {
   for(Int_t i=0; i<2; i++) {
    x5extrapBeam[i+2*j]=0.;
    y5extrapBeam[i+2*j]=0.;
    x6extrapBeam[i+2*j]=0.;
    y6extrapBeam[i+2*j]=0.;
    x7extrapBeam[i+2*j]=0.;
    y7extrapBeam[i+2*j]=0.;
    x8extrapBeam[i+2*j]=0.;
    y8extrapBeam[i+2*j]=0.;
   }
  }
}*/

Int_t ntracks1=fDchTrCand1Align->GetEntriesFast();
Int_t ntracks2=fDchTrCand2Align->GetEntriesFast();

//cout<<"ntracks1 = "<<ntracks1<<", ntracks2 = "<<ntracks2<<endl;

assert(ntracks1==ntracks2);
const Int_t ntracks=ntracks1;
cout<<"number of alignment tracks = "<<ntracks1<<endl;
//cout<<"number of alignment tracks2 = "<<ntracks2<<endl;

Double_t Xmin,Xmax,Ymin,Ymax;
UInt_t filterTracks=0;
Bool_t aliTrIndex[ntracks];
AlignmentTracksFilter(Xmin,Xmax,Ymin,Ymax,filterTracks,aliTrIndex);
//if(filterTracks<=20)noError=false;
//if(filterTracks<=20)return;
cout<<"filterTracks = "<<filterTracks<<endl;
Double_t xDifSE[numLayers_half],yDifSE[numLayers_half];

Double_t x5[filterTracks],y5[filterTracks],z5[filterTracks];
UShort_t ijk5[filterTracks],ijk6[filterTracks],ijk7[filterTracks],ijk8[filterTracks];
Double_t x6[filterTracks],y6[filterTracks],z6[filterTracks];
Double_t x7[filterTracks],y7[filterTracks],z7[filterTracks];
Double_t x8[filterTracks],y8[filterTracks],z8[filterTracks];
Double_t x5extrap[filterTracks],y5extrap[filterTracks];
Double_t x6extrap[filterTracks],y6extrap[filterTracks];
Double_t x7extrap[filterTracks],y7extrap[filterTracks];
Double_t x8extrap[filterTracks],y8extrap[filterTracks];

XYZPoint xyz5[filterTracks],xyz6[filterTracks],xyz7[filterTracks],xyz8[filterTracks];
XYZPoint xyz5extrap[filterTracks],xyz6extrap[filterTracks],xyz7extrap[filterTracks],xyz8extrap[filterTracks];
//XYZPoint xyz5loc[filterTracks],xyz6loc[filterTracks],xyz7loc[filterTracks],xyz8loc[filterTracks];
//XYZPoint xyz5loc1[filterTracks],xyz6loc1[filterTracks],xyz7loc1[filterTracks],xyz8loc1[filterTracks];
//XYZPoint xyz5loc1new[filterTracks],xyz6loc1new[filterTracks],xyz7loc1new[filterTracks],xyz8loc1new[filterTracks];
//Float_t sumx5=0.,sumy5=0.,sumx6=0.,sumy6=0.,sumx7=0.,sumy7=0.,sumx8=0.,sumy8=0.;
Double_t sumx5,sumy5,sumz5,sumx6,sumy6,sumz6,sumx7,sumy7,sumx8,sumy8;
Double_t alphaxz1,alphaxz2,alphaxz3,alphaxz;
Double_t alphayz1,alphayz2,alphayz3,alphayz;

Double_t sumx,sumy;
//TClonesArray  *fMatrix = new TClonesArray("TMatrix");
//TClonesArray &ffMatrix = *fMatrix;
for (UInt_t j = 0; j < numChambers; j++) {
 sumx5=0.,sumy5=0.,sumz5=0.,sumx6=0.,sumy6=0.,sumz6=0.,sumx7=0.,sumy7=0.,sumx8=0.,sumy8=0.; 
 sumx=0.,sumy=0.;
 memset(xDifSE, 0., sizeof(xDifSE));
 memset(yDifSE, 0., sizeof(yDifSE));
 UInt_t ll=0;
for (UInt_t l = 0; l < ntracks; l++) {
 if(!aliTrIndex[l])continue;
TMatrix dch1Mat = *((TMatrix*) fDchTrCand1Align->UncheckedAt(l));
TMatrix dch2Mat = *((TMatrix*) fDchTrCand2Align->UncheckedAt(l));
//TMatrix dchMat[2] = *((TMatrix*) fDchTrCand2Align->UncheckedAt(l));
 if(j==0){
  x5[ll]=dch1Mat(0,0);
  y5[ll]=dch1Mat(0,1);
  z5[ll]=dch1Mat(0,2);
  ijk5[ll]=dch1Mat(0,10);
  x6[ll]=dch1Mat(1,0);
  y6[ll]=dch1Mat(1,1);
  z6[ll]=dch1Mat(1,2);
  ijk6[ll]=dch1Mat(1,10);
  x7[ll]=dch1Mat(2,0);
  y7[ll]=dch1Mat(2,1);
  z7[ll]=dch1Mat(2,2);
  ijk7[ll]=dch1Mat(2,10);
  x8[ll]=dch1Mat(3,0);
  y8[ll]=dch1Mat(3,1);
  z8[ll]=dch1Mat(3,2);
  ijk8[ll]=dch1Mat(3,10);
 }else{
  x5[ll]=dch2Mat(0,0);
  y5[ll]=dch2Mat(0,1);
  z5[ll]=dch2Mat(0,2);
  ijk5[ll]=dch2Mat(0,10);
  x6[ll]=dch2Mat(1,0);
  y6[ll]=dch2Mat(1,1);
  z6[ll]=dch2Mat(1,2);
  ijk6[ll]=dch2Mat(1,10);
  x7[ll]=dch2Mat(2,0);
  y7[ll]=dch2Mat(2,1);
  z7[ll]=dch2Mat(2,2);
  ijk7[ll]=dch2Mat(2,10);
  x8[ll]=dch2Mat(3,0);
  y8[ll]=dch2Mat(3,1);
  z8[ll]=dch2Mat(3,2);
  ijk8[ll]=dch2Mat(3,10);
 }
  /*x5[ll]=dchMat[j](0,0);
  y5[ll]=dchMat[j](0,1);
  z5[ll]=dchMat[j](0,2);
  ijk5[ll]=dchMat[j](0,10);
  x6[ll]=dchMat[j](1,0);
  y6[ll]=dchMat[j](1,1);
  z6[ll]=dchMat[j](1,2);
  ijk6[ll]=dchMat[j](1,10);
  x7[ll]=dchMat[j](2,0);
  y7[ll]=dchMat[j](2,1);
  z7[ll]=dchMat[j](2,2);
  ijk7[ll]=dchMat[j](2,10);
  x8[ll]=dchMat[j](3,0);
  y8[ll]=dchMat[j](3,1);
  z8[ll]=dchMat[j](3,2);
  ijk8[ll]=dchMat[j](3,10);*/
  if(isGeant){
   x5extrap[ll]=x5extrapBeam[ijk5[ll]%2+2*j];
   y5extrap[ll]=y5extrapBeam[ijk5[ll]%2+2*j];
   x6extrap[ll]=x6extrapBeam[ijk6[ll]%2+2*j];
   y6extrap[ll]=y6extrapBeam[ijk6[ll]%2+2*j];
   x7extrap[ll]=x7extrapBeam[ijk7[ll]%2+2*j];
   y7extrap[ll]=y7extrapBeam[ijk7[ll]%2+2*j];
   x8extrap[ll]=x8extrapBeam[ijk8[ll]%2+2*j];
   y8extrap[ll]=y8extrapBeam[ijk8[ll]%2+2*j];
  }
  /*(xyz5[ll]).SetX(x5[ll]);(xyz5[ll]).SetY(y5[ll]);(xyz5[ll]).SetZ(z5[ll]);
  (xyz6[ll]).SetX(x6[ll]);(xyz6[ll]).SetY(y6[ll]);(xyz6[ll]).SetZ(z6[ll]);
  (xyz7[ll]).SetX(x7[ll]);(xyz7[ll]).SetY(y7[ll]);(xyz7[ll]).SetZ(z7[ll]);
  (xyz8[ll]).SetX(x8[ll]);(xyz8[ll]).SetY(y8[ll]);(xyz8[ll]).SetZ(z8[ll]);
  (xyz5extrap[ll]).SetX(x5extrap[ll]);(xyz5extrap[ll]).SetY(y5extrap[ll]);(xyz5extrap[ll]).SetZ(zLayer[0][ijk5[ll]]);
  (xyz6extrap[ll]).SetX(x6extrap[ll]);(xyz6extrap[ll]).SetY(y6extrap[ll]);(xyz6extrap[ll]).SetZ(zLayer[0][ijk6[ll]]);
  (xyz7extrap[ll]).SetX(x7extrap[ll]);(xyz7extrap[ll]).SetY(y7extrap[ll]);(xyz7extrap[ll]).SetZ(zLayer[0][ijk7[ll]]);
  (xyz8extrap[ll]).SetX(x8extrap[ll]);(xyz8extrap[ll]).SetY(y8extrap[ll]);(xyz8extrap[ll]).SetZ(zLayer[0][ijk8[ll]]);*/ 
  sumx5=sumx5+x5[ll];
  sumy5=sumy5+y5[ll];
  sumz5=sumz5+z5[ll];
  sumx6=sumx6+x6[ll];
  sumy6=sumy6+y6[ll];
  sumz6=sumz6+z6[ll];
  sumx7=sumx7+x7[ll];
  sumy7=sumy7+y7[ll];
  sumx8=sumx8+x8[ll];
  sumy8=sumy8+y8[ll];
ll++;
}//track cycle
cout<<"ll = "<<ll<<endl;
if(ll<=20)noError=false;
if(ll<=20)return;
if(ll>0){
 sumx5=sumx5/Float_t(ll);sumx6=sumx6/Float_t(ll);sumx7=sumx7/Float_t(ll);sumx8=sumx8/Float_t(ll);
 sumy5=sumy5/Float_t(ll);sumy6=sumy6/Float_t(ll);sumy7=sumy7/Float_t(ll);sumy8=sumy8/Float_t(ll);
 sumz5=sumz5/Float_t(ll);sumz6=sumz6/Float_t(ll);
}
cout<<"------------------------------------------------------------------------"<<endl;
cout<<"DCH"<<j+1<<endl;
cout<<"experimental x coordinates in DCH planes"<<endl;
cout<<sumx5<<" "<<sumx6<<" "<<sumx7<<" "<<sumx8<<endl;
cout<<"experimental y coordinates in DCH planes"<<endl;
cout<<sumy5<<" "<<sumy6<<" "<<sumy7<<" "<<sumy8<<endl;
cout<<"experimental z coordinates in DCH planes (1,2)"<<endl;
cout<<sumz5<<" "<<sumz6<<endl;
if(isGeant){
cout<<"simulated x coordinates in DCH planes"<<endl;
 cout<<x5extrapBeam[0+2*j]<<" "<<x6extrapBeam[0+2*j]<<" "<<x7extrapBeam[0+2*j]<<" "<<x8extrapBeam[0+2*j]<<endl;
 cout<<x5extrapBeam[1+2*j]<<" "<<x6extrapBeam[1+2*j]<<" "<<x7extrapBeam[1+2*j]<<" "<<x8extrapBeam[1+2*j]<<endl;
 cout<<"simulated y coordinates in DCH planes"<<endl;
 cout<<y5extrapBeam[0+2*j]<<" "<<y6extrapBeam[0+2*j]<<" "<<y7extrapBeam[0+2*j]<<" "<<y8extrapBeam[0+2*j]<<endl;
 cout<<y5extrapBeam[1+2*j]<<" "<<y6extrapBeam[1+2*j]<<" "<<y7extrapBeam[1+2*j]<<" "<<y8extrapBeam[1+2*j]<<endl;
 cout<<"differences of x_simulated - x_experimental"<<endl;
 cout<<x5extrapBeam[0+2*j]-sumx5<<" "<<x6extrapBeam[0+2*j]-sumx6<<" "<<x7extrapBeam[0+2*j]-sumx7<<" "<<x8extrapBeam[0+2*j]-sumx8<<endl;
 cout<<"differences of y_simulated - y_experimental"<<endl;
 cout<<y5extrapBeam[0+2*j]-sumy5<<" "<<y6extrapBeam[0+2*j]-sumy6<<" "<<y7extrapBeam[0+2*j]-sumy7<<" "<<y8extrapBeam[0+2*j]-sumy8<<endl;
//if(j==0){
  xDifSE[0]=x5extrapBeam[0+2*j]-sumx5;xDifSE[1]=x6extrapBeam[0+2*j]-sumx6;xDifSE[2]=x7extrapBeam[0+2*j]-sumx7;xDifSE[3]=x8extrapBeam[0+2*j]-sumx8; yDifSE[0]=y5extrapBeam[0+2*j]-sumy5;yDifSE[1]=y6extrapBeam[0+2*j]-sumy6;yDifSE[2]=y7extrapBeam[0+2*j]-sumy7;yDifSE[3]=y8extrapBeam[0+2*j]-sumy8;
  for(UShort_t i=0; i<numLayers_half; i++) {
   sumx=sumx+xDifSE[i];
   sumy=sumy+yDifSE[i];
  } 
}else{
 if(!zeroField){
  if(Iter==0){
   TString noFieldRun;
   noFieldRun.Form("%u",1884);
   ifstream DCHparFile;
   DCHpar=TString("DCHpar") + TString(".") + fileName0 + noFieldRun;
   DCHpar=inDirectory+DCHpar;
   DCHparFile.open (DCHpar);
   Double_t tmp;
   // alignment parameters
   DCHparFile >> tmp;
   XYZshift1.SetX(tmp);
   DCHparFile >> tmp;
   XYZshift1.SetY(tmp);
   DCHparFile >> tmp;
   XYZshift1.SetZ(tmp);
   DCHparFile >> tmp;
   XYZshift2.SetX(tmp);
   DCHparFile >> tmp;
   XYZshift2.SetY(tmp);
   DCHparFile >> tmp;
   XYZshift2.SetZ(tmp);  
  } 
 }
 sumx=sumx5+sumx6+sumx7+sumx8;
 sumy=sumy5+sumy6+sumy7+sumy8;
}//isGeant
meanShiftX[j]=sumx/Double_t(numLayers_half);
meanShiftY[j]=sumy/Double_t(numLayers_half);
cout<<"meanShiftX = "<<meanShiftX[j]<<", idch = "<<j<<endl;
cout<<"meanShiftY = "<<meanShiftY[j]<<", idch = "<<j<<endl;
//} 
//if(j==1){xDifSE2[0]=x5extrapBeam[0+2*j]-sumx5;xDifSE2[1]=x6extrapBeam[0+2*j]-sumx6;xDifSE2[2]=x7extrapBeam[0+2*j]-sumx7;xDifSE2[3]=x8extrapBeam[0+2*j]-sumx8; yDifSE2[0]=y5extrapBeam[0+2*j]-sumy5;yDifSE2[1]=y6extrapBeam[0+2*j]-sumy6;yDifSE2[2]=y7extrapBeam[0+2*j]-sumy7;yDifSE2[3]=y8extrapBeam[0+2*j]-sumy8;} 
//alphaxz1=atan((sumx6-sumx5)/(zLayer[0][2]-zLayer[0][0]));
//alphaxz2=atan((sumx7-sumx5)/(zLayer[0][4]-zLayer[0][0]));
//alphaxz3=atan((sumx8-sumx5)/(zLayer[0][6]-zLayer[0][0]));
//alphaxz=(alphaxz1+alphaxz2+alphaxz3)/3.;
//alphaxz=atan((sumx6-sumx5)/(zLayer[j][2]-zLayer[j][0]));
alphaxz=atan((sumx6-sumx5)/(sumz6-sumz5));
//alphayz1=atan((sumy6-sumy5)/(zLayer[0][2]-zLayer[0][0]));
//alphayz2=atan((sumy7-sumy5)/(zLayer[0][4]-zLayer[0][0]));
//alphayz3=atan((sumy8-sumy5)/(zLayer[0][6]-zLayer[0][0]));
//alphayz=(alphayz1+alphayz2+alphayz3)/3.;
//alphayz=atan((sumy6-sumy5)/(zLayer[j][2]-zLayer[j][0]));
alphayz=atan((sumy6-sumy5)/(sumz6-sumz5));
//cout<<alphaxz1<<" "<<alphaxz2<<" "<<alphaxz3<<" "<<endl;
//cout<<alphayz1<<" "<<alphayz2<<" "<<alphayz3<<" "<<endl;
if(isGeant)cout<<"simulated xz angle alphaxzBeam = "<<alphaxzBeam[j]<<" radians"<<" = "<<TMath::RadToDeg()*alphaxzBeam[j]<<" degrees"<<endl;
cout<<"experimental xz angle alphaxz = "<<alphaxz<<" radians"<<" = "<<TMath::RadToDeg()*alphaxz<<" degrees"<<endl;
if(isGeant)cout<<"simulated yz angle alphayzBeam = "<<alphayzBeam[j]<<" radians"<<" = "<<TMath::RadToDeg()*alphayzBeam[j]<<" degrees"<<endl;
cout<<"experimental yz angle alphayz = "<<alphayz<<" radians"<<" = "<<TMath::RadToDeg()*alphayz<<" degrees"<<endl;
//if(isGeant)cout<<"simulated xz angle alphaxzBeam = "<<TMath::RadToDeg()*alphaxzBeam[j]<<" degrees"<<endl;
//cout<<"experimental xz angle alphaxz = "<<TMath::RadToDeg()*alphaxz<<" degrees"<<endl;
//if(isGeant)cout<<"simulated yz angle alphayzBeam = "<<TMath::RadToDeg()*alphayzBeam[j]<<" degrees"<<endl;
//cout<<"experimental yz angle alphayz = "<<TMath::RadToDeg()*alphayz<<" degrees"<<endl;
cout<<"------------------------------------------------------------------------"<<endl;
}//chamber cycle
//finish: ;
}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::FinishAlignment(const Bool_t noError){

      //cout<<"zeroField = "<<zeroField<<endl;
      if(noError&&zeroField){
       XYZshift1.SetX(meanShiftX[0]);
       XYZshift1.SetY(meanShiftY[0]);
       XYZshift1.SetZ(0.);
       XYZshift2.SetX(meanShiftX[1]);
       XYZshift2.SetY(meanShiftY[1]);
       XYZshift2.SetZ(0.);
      }
      //cout<<"XYZshift1: "<<XYZshift1.X()<<" "<<XYZshift1.Y()<<" "<<XYZshift1.Z()<<endl;
      //cout<<"XYZshift2: "<<XYZshift2.X()<<" "<<XYZshift2.Y()<<" "<<XYZshift2.Z()<<endl;

      //gObjectTable->Print();
      fDchTrCand1Align->Delete();
      delete fDchTrCand1Align;
      fDchTrCand2Align->Delete();
      delete fDchTrCand2Align;
      //gObjectTable->Print();
      //cout<<"I am deleting in run "<<runNumber<<", called from "<<fromwhere<<", iter = "<<Iter<<endl; 

}
//-------------------------------------------------------------------------------------------------------------------------
//void BmnDchHitProducer_exp::DchAlignment(XYZPoint &xyzHit1,XYZPoint &xyzHit2,XYZPoint &xyzHit3,XYZPoint &xyzHit4,XYZPoint &xyzWire1,XYZPoint &xyzWire2,XYZPoint &xyzWire3,XYZPoint &xyzWire4){
void BmnDchHitProducer_exp::DchAlignment(XYZPoint &xyzHit1,XYZPoint &xyzHit2,XYZPoint &xyzHit3,XYZPoint &xyzHit4,XYZPoint &xyzWire1,XYZPoint &xyzWire2,XYZPoint &xyzWire3,XYZPoint &xyzWire4,XYZPoint &xyzWire1_2,XYZPoint &xyzWire2_2,XYZPoint &xyzWire3_2,XYZPoint &xyzWire4_2, UShort_t idch){
 
//first chamber position
//XYZVector XYZ1(DCH1_Xpos[runPeriod-1],DCH1_Ypos[runPeriod-1],DCH1_Zpos[runPeriod-1]);
//XYZVector XYZ1(detXshift[0],detYshift[0],detZshift[0]);

TVectorD misaligned(4);
TVectorD misalignedtmp(4);
TVectorD aligned(4);
TVectorD alignedtmp(4);
XYZVector XYZ;
//XYZPoint xyzHit1tmp,xyzHit2tmp,xyzHit3tmp,xyzHit4tmp;

if(idch==0){
  XYZ=XYZ0;
}else{
   XYZ=XYZ1;
}

XYZPoint xyz5loc1,xyz6loc1,xyz7loc1,xyz8loc1;
XYZPoint xyz5loc1new,xyz6loc1new,xyz7loc1new,xyz8loc1new;
//XYZPoint xyz5loc1newtmp,xyz6loc1newtmp,xyz7loc1newtmp,xyz8loc1newtmp;

 /*cout<<"misaligned1(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit1.X()<<", "<<xyzHit1.Y()<<", "<<xyzHit1.Z()<<" "<<endl; 
 cout<<"misaligned2(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit2.X()<<", "<<xyzHit2.Y()<<", "<<xyzHit2.Z()<<" "<<endl; 
 cout<<"misaligned3(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit3.X()<<", "<<xyzHit3.Y()<<", "<<xyzHit3.Z()<<" "<<endl; 
 cout<<"misaligned4(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit4.X()<<", "<<xyzHit4.Y()<<", "<<xyzHit4.Z()<<" "<<endl;*/ 

for (UShort_t i = 0; i < 3; i++) {
 if(i==0){
  if(idch==1){
   xyz5loc1=xyzHit1-XYZ+XYZshift2;
   xyz6loc1=xyzHit2-XYZ+XYZshift2;
   xyz7loc1=xyzHit3-XYZ+XYZshift2;
   xyz8loc1=xyzHit4-XYZ+XYZshift2;
   /*xyz5loc1=xyzHit1-XYZ;
   xyz6loc1=xyzHit2-XYZ;
   xyz7loc1=xyzHit3-XYZ;
   xyz8loc1=xyzHit4-XYZ;*/
  }else{
   xyz5loc1=xyzHit1-XYZ+XYZshift1;
   xyz6loc1=xyzHit2-XYZ+XYZshift1;
   xyz7loc1=xyzHit3-XYZ+XYZshift1;
   xyz8loc1=xyzHit4-XYZ+XYZshift1;
  }
 }else if(i==1){
  if(idch==1){
   xyz5loc1=xyzWire1-XYZ+XYZshift2;
   xyz6loc1=xyzWire2-XYZ+XYZshift2;
   xyz7loc1=xyzWire3-XYZ+XYZshift2;
   xyz8loc1=xyzWire4-XYZ+XYZshift2;
   /*xyz5loc1=xyzWire1-XYZ;
   xyz6loc1=xyzWire2-XYZ;
   xyz7loc1=xyzWire3-XYZ;
   xyz8loc1=xyzWire4-XYZ;*/
  }else{
   xyz5loc1=xyzWire1-XYZ+XYZshift1;
   xyz6loc1=xyzWire2-XYZ+XYZshift1;
   xyz7loc1=xyzWire3-XYZ+XYZshift1;
   xyz8loc1=xyzWire4-XYZ+XYZshift1;
  }
 }else if(i==2){
  if(idch==1){
   xyz5loc1=xyzWire1_2-XYZ+XYZshift2;
   xyz6loc1=xyzWire2_2-XYZ+XYZshift2;
   xyz7loc1=xyzWire3_2-XYZ+XYZshift2;
   xyz8loc1=xyzWire4_2-XYZ+XYZshift2;
   /*xyz5loc1=xyzWire1_2-XYZ;
   xyz6loc1=xyzWire2_2-XYZ;
   xyz7loc1=xyzWire3_2-XYZ;
   xyz8loc1=xyzWire4_2-XYZ;*/
  }else{
   xyz5loc1=xyzWire1_2-XYZ+XYZshift1;
   xyz6loc1=xyzWire2_2-XYZ+XYZshift1;
   xyz7loc1=xyzWire3_2-XYZ+XYZshift1;
   xyz8loc1=xyzWire4_2-XYZ+XYZshift1;
  }
 }

 //if(i==0)cout<<"aligned,misaligned (hits):"<<endl; 
 //if(i==1)cout<<"aligned,misaligned (wires - first point):"<<endl; 
 //if(i==2)cout<<"aligned,misaligned (wires - second point):"<<endl; 
 misaligned(0)=xyz5loc1.X();misaligned(1)=xyz5loc1.Y();misaligned(2)=xyz5loc1.Z();misaligned(3)=1.;
if(idch==1){
 /*misalignedtmp(0)=misaligned(0)+xDifSE[0];
 misalignedtmp(1)=misaligned(1)+yDifSE[0];
 misalignedtmp(2)=misaligned(2);
 misalignedtmp(3)=misaligned(3);
 aligned=C*misalignedtmp;*/
 /*aligned(0)=misaligned(0)+xDifSE2[0];
 aligned(1)=misaligned(1)+yDifSE2[0];
 aligned(2)=misaligned(2);
 aligned(3)=misaligned(3);*/
 aligned=C*misaligned;
 //aligned(0)=aligned(0)+meanShiftX[idch];
 //aligned(1)=aligned(1)+meanShiftY[idch];
 /*aligned(0)=aligned(0)+xDifSE[0];
 aligned(1)=aligned(1)+yDifSE[0];
 aligned(2)=aligned(2);
 aligned(3)=aligned(3);*/
 /*alignedtmp=D*misaligned;
 if(i==0){
  xyz5loc1newtmp.SetX(alignedtmp(0));xyz5loc1newtmp.SetY(alignedtmp(1));xyz5loc1newtmp.SetZ(alignedtmp(2));
  xyzHit1tmp=xyz5loc1newtmp+XYZ;
 }
 aligned=C*alignedtmp;
 //C.Print();*/
}else{
 //aligned=D*misaligned;
 //D.Print();
 //aligned(0)=misaligned(0)+xDifSE[0];
 //aligned(1)=misaligned(1)+yDifSE[0];
 //aligned(0)=misaligned(0)+meanShiftX[idch];
 //aligned(1)=misaligned(1)+meanShiftY[idch];
 aligned(0)=misaligned(0);
 aligned(1)=misaligned(1);
 aligned(2)=misaligned(2);
 aligned(3)=misaligned(3);
}

 //cout<<"idch = "<<idch<<", misaligned1(X): "<<misaligned(0)<<" "<<misaligned(1)<<" "<<misaligned(2)<<" "<<endl; 
 //cout<<"idch = "<<idch<<", aligned1(X): "<<aligned(0)<<" "<<aligned(1)<<" "<<aligned(2)<<" "<<endl; 
 if(checkDch)cout<<"distance aligned-misaligned (coordinates, plane 1): "<<aligned(0)-misaligned(0)<<", "<<aligned(1)-misaligned(1)<<", "<<aligned(2)-misaligned(2)<<endl;
 if(checkDch)cout<<"distance aligned-misaligned: "<<sqrt(pow(aligned(0)-misaligned(0),2)+pow(aligned(1)-misaligned(1),2)+pow(aligned(2)-misaligned(2),2))<<endl;
 xyz5loc1new.SetX(aligned(0));xyz5loc1new.SetY(aligned(1));xyz5loc1new.SetZ(aligned(2));
 misaligned(0)=xyz6loc1.X();misaligned(1)=xyz6loc1.Y();misaligned(2)=xyz6loc1.Z();misaligned(3)=1.;
if(idch==1){
 /*misalignedtmp(0)=misaligned(0)+xDifSE[1];
 misalignedtmp(1)=misaligned(1)+yDifSE[1];
 misalignedtmp(2)=misaligned(2);
 misalignedtmp(3)=misaligned(3);
 aligned=C*misalignedtmp;*/
 /*aligned(0)=misaligned(0)+xDifSE2[1];
 aligned(1)=misaligned(1)+yDifSE2[1];
 aligned(2)=misaligned(2);
 aligned(3)=misaligned(3);*/
 aligned=C*misaligned;
 //aligned(0)=aligned(0)+meanShiftX[idch];
 //aligned(1)=aligned(1)+meanShiftY[idch];
 /*aligned(0)=aligned(0)+xDifSE[1];
 aligned(1)=aligned(1)+yDifSE[1];
 aligned(2)=aligned(2);
 aligned(3)=aligned(3);*/
 /*alignedtmp=D*misaligned;
 if(i==0){
  xyz6loc1newtmp.SetX(alignedtmp(0));xyz6loc1newtmp.SetY(alignedtmp(1));xyz6loc1newtmp.SetZ(alignedtmp(2));
  xyzHit2tmp=xyz6loc1newtmp+XYZ;
 }
 aligned=C*alignedtmp;*/
}else{
 //aligned=D*misaligned;
 //aligned(0)=misaligned(0)+xDifSE[1];
 //aligned(1)=misaligned(1)+yDifSE[1];
 //aligned(0)=misaligned(0)+meanShiftX[idch];
 //aligned(1)=misaligned(1)+meanShiftY[idch];
 aligned(0)=misaligned(0);
 aligned(1)=misaligned(1);
 aligned(2)=misaligned(2);
 aligned(3)=misaligned(3);
}
 if(i==0&&fDoCheck){ //only track points, no wires
  hAliMisaliX->Fill(aligned(0)-misaligned(0));
  hAliMisaliY->Fill(aligned(1)-misaligned(1));
  hAliMisaliXYdist->Fill(aligned(0)-misaligned(0),aligned(1)-misaligned(1));
  hAliX->Fill(aligned(0));
  hAliY->Fill(aligned(1));
  hAliXYdist->Fill(aligned(0),aligned(1));
  hMisaliX->Fill(misaligned(0));
  hMisaliY->Fill(misaligned(1));
  hMisaliXYdist->Fill(misaligned(0),misaligned(1));
 }
 //cout<<"idch = "<<idch<<", misaligned2(X): "<<misaligned(0)<<" "<<misaligned(1)<<" "<<misaligned(2)<<" "<<endl; 
 //cout<<"idch = "<<idch<<", aligned2(Y): "<<aligned(0)<<" "<<aligned(1)<<" "<<aligned(2)<<" "<<endl; 
 //cout<<"distance aligned-misaligned (coordinates, plane 2): "<<aligned(0)-misaligned(0)<<", "<<aligned(1)-misaligned(1)<<", "<<aligned(2)-misaligned(2)<<endl;
 //cout<<"distance aligned-misaligned: "<<sqrt(pow(aligned(0)-misaligned(0),2)+pow(aligned(1)-misaligned(1),2)+pow(aligned(2)-misaligned(2),2))<<endl;
 xyz6loc1new.SetX(aligned(0));xyz6loc1new.SetY(aligned(1));xyz6loc1new.SetZ(aligned(2));
 misaligned(0)=xyz7loc1.X();misaligned(1)=xyz7loc1.Y();misaligned(2)=xyz7loc1.Z();misaligned(3)=1.;
if(idch==1){
 /*misalignedtmp(0)=misaligned(0)+xDifSE[2];
 misalignedtmp(1)=misaligned(1)+yDifSE[2];
 misalignedtmp(2)=misaligned(2);
 misalignedtmp(3)=misaligned(3);
 aligned=C*misalignedtmp;*/
 /*aligned(0)=misaligned(0)+xDifSE2[2];
 aligned(1)=misaligned(1)+yDifSE2[2];
 aligned(2)=misaligned(2);
 aligned(3)=misaligned(3);*/
 aligned=C*misaligned;
 //aligned(0)=aligned(0)+meanShiftX[idch];
 //aligned(1)=aligned(1)+meanShiftY[idch];
 /*aligned(0)=aligned(0)+xDifSE[2];
 aligned(1)=aligned(1)+yDifSE[2];
 aligned(2)=aligned(2);
 aligned(3)=aligned(3);*/
 /*alignedtmp=D*misaligned;
 if(i==0){
  xyz7loc1newtmp.SetX(alignedtmp(0));xyz7loc1newtmp.SetY(alignedtmp(1));xyz7loc1newtmp.SetZ(alignedtmp(2));
  xyzHit3tmp=xyz7loc1newtmp+XYZ;
 }
 aligned=C*alignedtmp;*/
}else{
 //aligned=D*misaligned;
 //aligned(0)=misaligned(0)+xDifSE[2];
 //aligned(1)=misaligned(1)+yDifSE[2];
 //aligned(0)=misaligned(0)+meanShiftX[idch];
 //aligned(1)=misaligned(1)+meanShiftY[idch];
 aligned(0)=misaligned(0);
 aligned(1)=misaligned(1);
 aligned(2)=misaligned(2);
 aligned(3)=misaligned(3);
}
 //cout<<"idch = "<<idch<<", misaligned3(X): "<<misaligned(0)<<" "<<misaligned(1)<<" "<<misaligned(2)<<" "<<endl; 
 //cout<<"idch = "<<idch<<", aligned3(Y): "<<aligned(0)<<" "<<aligned(1)<<" "<<aligned(2)<<" "<<endl; 
 //cout<<"distance aligned-misaligned (coordinates, plane 3): "<<aligned(0)-misaligned(0)<<", "<<aligned(1)-misaligned(1)<<", "<<aligned(2)-misaligned(2)<<endl;
 //cout<<"distance aligned-misaligned: "<<sqrt(pow(aligned(0)-misaligned(0),2)+pow(aligned(1)-misaligned(1),2)+pow(aligned(2)-misaligned(2),2))<<endl;
 xyz7loc1new.SetX(aligned(0));xyz7loc1new.SetY(aligned(1));xyz7loc1new.SetZ(aligned(2));
 misaligned(0)=xyz8loc1.X();misaligned(1)=xyz8loc1.Y();misaligned(2)=xyz8loc1.Z();misaligned(3)=1.;
if(idch==1){
 /*misalignedtmp(0)=misaligned(0)+xDifSE[3];
 misalignedtmp(1)=misaligned(1)+yDifSE[3];
 misalignedtmp(2)=misaligned(2);
 misalignedtmp(3)=misaligned(3);
 aligned=C*misalignedtmp;*/
 /*aligned(0)=misaligned(0)+xDifSE2[3];
 aligned(1)=misaligned(1)+yDifSE2[3];
 aligned(2)=misaligned(2);
 aligned(3)=misaligned(3);*/
 aligned=C*misaligned;
 //aligned(0)=aligned(0)+meanShiftX[idch];
 //aligned(1)=aligned(1)+meanShiftY[idch];
 /*aligned(0)=aligned(0)+xDifSE[3];
 aligned(1)=aligned(1)+yDifSE[3];
 aligned(2)=aligned(2);
 aligned(3)=aligned(3);*/
 /*alignedtmp=D*misaligned;
 if(i==0){
  xyz8loc1newtmp.SetX(alignedtmp(0));xyz8loc1newtmp.SetY(alignedtmp(1));xyz8loc1newtmp.SetZ(alignedtmp(2));
  xyzHit4tmp=xyz8loc1newtmp+XYZ;
 }
 aligned=C*alignedtmp;*/
}else{
 //aligned=D*misaligned;
 //aligned(0)=misaligned(0)+xDifSE[3];
 //aligned(1)=misaligned(1)+yDifSE[3];
 //aligned(0)=misaligned(0)+meanShiftX[idch];
 //aligned(1)=misaligned(1)+meanShiftY[idch];
 aligned(0)=misaligned(0);
 aligned(1)=misaligned(1);
 aligned(2)=misaligned(2);
 aligned(3)=misaligned(3);
}
 //cout<<"idch = "<<idch<<", misaligned4(X): "<<misaligned(0)<<" "<<misaligned(1)<<" "<<misaligned(2)<<" "<<endl; 
 //cout<<"idch = "<<idch<<", aligned4(Y): "<<aligned(0)<<" "<<aligned(1)<<" "<<aligned(2)<<" "<<endl; 
 //cout<<"distance aligned-misaligned (coordinates, plane 4): "<<aligned(0)-misaligned(0)<<", "<<aligned(1)-misaligned(1)<<", "<<aligned(2)-misaligned(2)<<endl;
 //cout<<"distance aligned-misaligned: "<<sqrt(pow(aligned(0)-misaligned(0),2)+pow(aligned(1)-misaligned(1),2)+pow(aligned(2)-misaligned(2),2))<<endl;
 xyz8loc1new.SetX(aligned(0));xyz8loc1new.SetY(aligned(1));xyz8loc1new.SetZ(aligned(2));
 if(i==0){
  xyzHit1=xyz5loc1new+XYZ;
  xyzHit2=xyz6loc1new+XYZ;
  xyzHit3=xyz7loc1new+XYZ;
  xyzHit4=xyz8loc1new+XYZ;
 }else if(i==1){
  xyzWire1=xyz5loc1new+XYZ;
  xyzWire2=xyz6loc1new+XYZ;
  xyzWire3=xyz7loc1new+XYZ;
  xyzWire4=xyz8loc1new+XYZ;
 }else if(i==2){
  xyzWire1_2=xyz5loc1new+XYZ;
  xyzWire2_2=xyz6loc1new+XYZ;
  xyzWire3_2=xyz7loc1new+XYZ;
  xyzWire4_2=xyz8loc1new+XYZ;
 }
 //cout<<"----------------------------------------------------------------"<<endl;
}// i cycle

 /*cout<<"Daligned1(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit1tmp.X()<<", "<<xyzHit1tmp.Y()<<", "<<xyzHit1tmp.Z()<<" "<<endl; 
 cout<<"Daligned2(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit2tmp.X()<<", "<<xyzHit2tmp.Y()<<", "<<xyzHit2tmp.Z()<<" "<<endl; 
 cout<<"Daligned3(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit3tmp.X()<<", "<<xyzHit3tmp.Y()<<", "<<xyzHit3tmp.Z()<<" "<<endl; 
 cout<<"Daligned4(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit4tmp.X()<<", "<<xyzHit4tmp.Y()<<", "<<xyzHit4tmp.Z()<<" "<<endl;*/ 

/*cout<<"aligned1(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit1.X()<<", "<<xyzHit1.Y()<<", "<<xyzHit1.Z()<<" "<<endl; 
 cout<<"aligned2(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit2.X()<<", "<<xyzHit2.Y()<<", "<<xyzHit2.Z()<<" "<<endl; 
 cout<<"aligned3(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit3.X()<<", "<<xyzHit3.Y()<<", "<<xyzHit3.Z()<<" "<<endl; 
 cout<<"aligned4(idch,x,y,z): " <<"idch = "<<idch<<", "<<xyzHit4.X()<<", "<<xyzHit4.Y()<<", "<<xyzHit4.Z()<<" "<<endl;*/ 
}
//-------------------------------------------------------------------------------------------------------------------------
UInt_t BmnDchHitProducer_exp::GetNumberOfAlignmentTracks(){

 return eventNum1track;

};
//-------------------------------------------------------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::GetSumOfDeltaPhi(){

 return sumDeltaPhi;

};
//-------------------------------------------------------------------------------------------------------------------------
/*void BmnDchHitProducer_exp::RunBeamTarget(TString &beam, TString &target){

beam="";target="";

  if (runNumber>=12&&runNumber<=188){
     beam="d"; 
  //}else if (runNumber>=220&&runNumber<=395){
  }else if ((runNumber>=244&&runNumber<=245)||(runNumber>=262&&runNumber<=337)){
     beam="d"; 
     target="Cu";  
  }else if ((runNumber>=246&&runNumber<=258)||(runNumber>=358&&runNumber<=386)){
     beam="d"; 
  //}else if (runNumber>=403&&runNumber<=688){
  }else if ((runNumber>=465&&runNumber<=492)||(runNumber>=589&&runNumber<=597)||(runNumber>=647&&runNumber<=685)){
     beam="C"; 
     target="Cu";  
  }else if ((runNumber>=493&&runNumber<=574)||(runNumber==604)||(runNumber>=645&&runNumber<=646)){
     beam="C"; 
  }else if ((runNumber>=575&&runNumber<=586)||(runNumber>=607&&runNumber<=644)){
     beam="C"; 
     target="C";  
  }

};*/
//-------------------------------------------------------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::minDel(Double_t coor1, Double_t driftdist1,Double_t coor2, Double_t driftdist2){

//using namespace ROOT::Math;

Double_t coords[4],comb[4],abscomb[4];
coords[0]=coor1-driftdist1;//left
coords[1]=coor1+driftdist1;//right
coords[2]=coor2-driftdist2;//left
coords[3]=coor2+driftdist2;//right


comb[0]=coords[0]-coords[2];
comb[1]=coords[0]-coords[3];
comb[2]=coords[1]-coords[2];
comb[3]=coords[1]-coords[3];
for (UShort_t i = 0; i < 4; i++) {
 abscomb[i]=fabs(comb[i]);
}

//Double_t min=TMath::MinElement(4,comb);
Int_t locmin=Int_t(TMath::LocMin(4,abscomb));
Double_t min=comb[locmin];
cout<<coor1<<" "<<coor2<<" "<<driftdist1<<" "<<driftdist2<<endl;
cout<<coords[0]<<" "<<coords[1]<<" "<<coords[2]<<" "<<coords[3]<<endl;
cout<<comb[0]<<" "<<comb[1]<<" "<<comb[2]<<" "<<comb[3]<<endl;
//cout<<" min = "<<min<<endl;
//cout<<"locmin = "<<locmin<<", abs(min) = "<<abscomb[locmin]<<", min = "<<min<<endl;
//cout<<" angle = "<<TMath::RadToDeg()*atan(min/1.2)<<endl;

return min;

}
//-------------------------------------------------------------------------------------------------------------------------
UShort_t BmnDchHitProducer_exp::mapPlaneID(UShort_t Uid){

UShort_t uid;
/*
//DCH1
if(Uid==0)uid=6;
if(Uid==1)uid=7;
if(Uid==2)uid=4;
if(Uid==3)uid=5;
if(Uid==4)uid=2;
if(Uid==5)uid=3;
if(Uid==6)uid=0;
if(Uid==7)uid=1;
//DCH2
if(Uid==8)uid=14;
if(Uid==9)uid=15;
if(Uid==10)uid=12;
if(Uid==11)uid=13;
if(Uid==12)uid=10;
if(Uid==13)uid=11;
if(Uid==14)uid=8;
if(Uid==15)uid=9;
*/
/*
//DCH1
if(Uid==0)uid=14;
if(Uid==1)uid=15;
if(Uid==2)uid=12;
if(Uid==3)uid=13;
if(Uid==4)uid=10;
if(Uid==5)uid=11;
if(Uid==6)uid=8;
if(Uid==7)uid=9;
//DCH2
if(Uid==8)uid=6;
if(Uid==9)uid=7;
if(Uid==10)uid=4;
if(Uid==11)uid=5;
if(Uid==12)uid=2;
if(Uid==13)uid=3;
if(Uid==14)uid=0;
if(Uid==15)uid=1;*/

/*
//DCH1
if(Uid==0)uid=7;
if(Uid==1)uid=6;
if(Uid==2)uid=5;
if(Uid==3)uid=4;
if(Uid==4)uid=3;
if(Uid==5)uid=2;
if(Uid==6)uid=1;
if(Uid==7)uid=0;
//DCH2
if(Uid==8)uid=15;
if(Uid==9)uid=14;
if(Uid==10)uid=13;
if(Uid==11)uid=12;
if(Uid==12)uid=11;
if(Uid==13)uid=10;
if(Uid==14)uid=9;
if(Uid==15)uid=8;*/
if(runPeriod<=3){
 //DCH1
 if(Uid==0)uid=7;
 if(Uid==1)uid=6;
 if(Uid==2)uid=5;
 if(Uid==3)uid=4;
 if(Uid==4)uid=1;
 if(Uid==5)uid=0;
 if(Uid==6)uid=3;
 if(Uid==7)uid=2;
 //DCH2
 if(Uid==8)uid=15;
 if(Uid==9)uid=14;
 if(Uid==10)uid=13;
 if(Uid==11)uid=12;
 if(Uid==12)uid=9;
 if(Uid==13)uid=8;
 if(Uid==14)uid=11;
 if(Uid==15)uid=10;
}else if (runPeriod==6){
 //DCH1
 if(Uid==0)uid=7;
 if(Uid==1)uid=6;
 if(Uid==2)uid=5;
 if(Uid==3)uid=4;
 if(Uid==4)uid=3;
 if(Uid==5)uid=2;
 if(Uid==6)uid=1;
 if(Uid==7)uid=0;
 //DCH2
 if(Uid==8)uid=15;
 if(Uid==9)uid=14;
 if(Uid==10)uid=13;
 if(Uid==11)uid=12;
 if(Uid==12)uid=11;
 if(Uid==13)uid=10;
 if(Uid==14)uid=9;
 if(Uid==15)uid=8;
}

/*
//DCH1
if(Uid==0)uid=6;
if(Uid==1)uid=7;
if(Uid==2)uid=4;
if(Uid==3)uid=5;
if(Uid==4)uid=2;
if(Uid==5)uid=3;
if(Uid==6)uid=0;
if(Uid==7)uid=1;

//DCH2

if(Uid==8)uid=14;
if(Uid==9)uid=15;
if(Uid==10)uid=12;
if(Uid==11)uid=13;
if(Uid==12)uid=10;
if(Uid==13)uid=11;
if(Uid==14)uid=8;
if(Uid==15)uid=9;
*/

return uid;

}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::backgroundRemoval(TH1D *mindrifttime){

//Double_t konst[numChambers];
Double_t konst;

 //for (UShort_t i = 0; i < numChambers; i++) {
   //minDriftTime[i]->Fit("pol0","","",200.,400.);
    if(runPeriod<=3)mindrifttime->Fit("pol0","","",200.,400.);
    if(runPeriod==6)mindrifttime->Fit("pol0","","",-250.,-100.);
    //TF1 *myfit = (TF1*) minDriftTime[i]->GetFunction("pol0");
    TF1 *myfit = (TF1*) mindrifttime->GetFunction("pol0");
    //konst[i]=myfit->GetParameter(0);
    konst=myfit->GetParameter(0);
    for (Int_t j = 0; j < hnbins[runPeriod]; j++) {
      Float_t binCon;
      //binCon=minDriftTime[i]->GetBinContent(j+1)-konst[i];
      binCon=mindrifttime->GetBinContent(j+1)-konst;
      binCon=(binCon>0.)?binCon:0.;
      //minDriftTime[i]->SetBinContent(j+1,binCon);
      mindrifttime->SetBinContent(j+1,binCon);
    }
    delete myfit; 
 //} 


}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::magFieldScale(){

  Bool_t isField = kTRUE; // flag for tracking (to use mag.field or not)

  //UniDbRun* pCurrentRun0;
  //pCurrentRun0->PrintAll();
  // set magnet field with factor corresponding the given run
  //cout<<"run period = "<<runPeriod<<", "<<"run number = "<<runNumber<<endl; 
  UniDbRun* pCurrentRun = UniDbRun::GetRun(runPeriod, runNumber);
  //UniDbRun* pCurrentRun = UniDbRun::GetRun(2,247);
  Double_t fieldScale = 0;
  if (pCurrentRun == 0) {
   //exit(-2);
   //fieldScale=1.;
   //fieldScale=0.665119;
   //fieldScale=0.88683;
   //fieldScale=0.888889;//temporary, only for run 247 
   //fieldScale=1.1085;
   //fieldScale=1.2194;
   cout<<"field scale = "<<fieldScale<<endl;
   magField->SetScale(fieldScale);
   //goto finish;
   cout<<"Database not working! Using fieldScale = 1 for the magnetic field!"<<endl;
   return;
  }
  //double map_current = 900.0;
  double map_current = 56.;
  //int* current_current = pCurrentRun->GetFieldCurrent();
  double* field_voltage = 0;
  field_voltage = pCurrentRun->GetFieldVoltage();
  cout<<*field_voltage<<" "<<pCurrentRun<<endl;
  if (field_voltage == NULL) {
   fieldScale = 0;
   isField = kFALSE;
  } else {
   //fieldScale = (*current_current) / map_current;
   fieldScale = (*field_voltage) / map_current;
  }
  //fieldScale=0.888889;//temporary, only for run 247 
  //fieldScale=5.;//temporary, only for run 247 
  if(isGeant)magField->SetScale(fieldScale);
  //finish:
  cout<<"field scale = "<<fieldScale<<endl;
  //magField->Init();
  //magField->Print();

}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::GeantInit(){
   //gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
   //gROOT->ProcessLine("bmnloadlibs()"); 
   cout<<"running GeantInit"<<endl;
   //cout<<"Iter = "<<Iter<<endl;
   //if(Iter==0){
   //if(Iter==0)fRun = new FairRunSim(); 
   fRun = new FairRunSim(); 
   //}else{
    //fRun= FairRunSim::Instance();
   //}
   fRun->SetName("TGeant3"); 
   fRun->SetField(magField);
   fRun->SetOutputFile("DchGeantOutput.root"); 
   // ----- Create media 
   fRun->SetMaterials("media.geo"); 
   // ----- Create geometry 
   FairModule* cave = new FairCave("CAVE"); 
   cave->SetGeometryFileName("cave.geo"); 
   fRun->AddModule(cave); 

  /* 
  FairBoxGenerator* boxGen = new FairBoxGenerator(211, 1); 

  boxGen->SetThetaRange ( 0., 8.); 

  boxGen->SetPRange (2.,2.5); 

  boxGen->SetPhiRange (0.,360.); 
  boxGen->SetXYZ(0., 0., 0.);

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator(); 

  primGen->AddGenerator(boxGen); 

  fRun->SetGenerator(primGen);*/ 
  //run->SetGenerator(boxGen);  
  //if(Iter==0)fRun->Init();
  fRun->Init();
  //fRun->Reinit();
  //fRun->Run(1);
  cout<<"gMC = "<<gMC<<endl; 
  Float_t a; Float_t z; Float_t density; Float_t radl; Float_t absl;
  gMC->CurrentMaterial(a,z,density,radl,absl);
  cout<<"top volume material characteristics (a,z,density,radl,absl): "<<a<<", "<<z<<", "<<density<<", "<<radl<<", "<<absl<<endl;
  /*
  //cout<<gMC->CurrentMedium()<<endl;
  cout<<gMC->TrackMass()<<endl;
  Float_t a; Float_t z; Float_t density; Float_t radl; Float_t absl;
        cout<<gMC->CurrentMedium()<<endl;
        gMC->CurrentMaterial(a,z,density,radl,absl);
        cout<<a<<" "<<z<<" "<<density<<" "<<radl<<" "<<absl<<endl;
        Double_t xx,yy,zz;
        gMC->TrackPosition(xx,yy,zz);
        cout<<xx<<" "<<yy<<" "<<zz<<endl; 
  */
  cout<<"fRun = "<<fRun<<endl;
}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::GeantBeamTracker(){
  //Int_t particlePDG = 2212;
  
  Float_t x1[3],p1[3],x2[3],p2[3];
  Float_t ein[15];
  Float_t pli[6];Float_t plo[9];
  Int_t nepred=1;

  UniDbRun* pCurrentRun = UniDbRun::GetRun(runPeriod, runNumber);
  TString beamParticle=pCurrentRun->GetBeamParticle();
  cout<<"beam particle = "<<beamParticle<<endl; 
  GeantBeamCode=geantCode(beamParticle); 
 
  double *collEnergy = pCurrentRun->GetEnergy();

  cout<<"beam particle energy = "<<*collEnergy<<endl; 

  beamMomentum=sqrt(pow(geantMass(GeantBeamCode)+2.*(*collEnergy),2.)-pow(geantMass(GeantBeamCode),2.));
  //GeantBeamCode=62;
  cout<<"Geant beam code = "<<GeantBeamCode<<endl;
  cout<<"geant beam momentum = "<<beamMomentum<<endl;
  Double_t tanxz,tanyz;
  //beamMomentum=1.;

  /*Double_t DmomentumX,DmomentumY,DmomentumZ,compY=0.0025,compZ=0.0;
  DmomentumY=compY*Dmomentum;
  DmomentumZ=compZ*Dmomentum;
  DmomentumX=sqrt(Dmomentum*Dmomentum-DmomentumY*DmomentumY-DmomentumZ*DmomentumZ);*/
 
for(UShort_t j=0; j<2; j++) { //cycle over chambers
 if(j==0){
  p1[0] = beamMomentum;
  p1[1] = 0.;
  p1[2] = 0.;
  //p1[0] = DmomentumX;
  //p1[1] = DmomentumY;
  //p1[2] = DmomentumZ;
  x1[0] = XYZ0beam.Z();
  x1[1] = 0.;
  //x1[1] = -0.8;
  x1[2] = 0.;
 }else {
  p1[0] = p2[0];
  p1[1] = p2[1];
  p1[2] = p2[2];
  x1[0] = x2[0];
  x1[1] = x2[1];
  x1[2] = x2[2];
 }
  cout<<"---------------------------------------------------------------------"<<endl;
  //cout<<"Initial position (x,y,z): "<<x1[0]<<", "<<x1[1]<<", "<<x1[2]<<endl;
  //cout<<"Initial impulse (px,py,pz,p): "<<p1[0]<<", "<<p1[1]<<", "<<p1[2]<<", "<<sqrt(p1[0]*p1[0]+p1[1]*p1[1]+p1[2]*p1[2])<<endl;
  cout<<"Simulated track initial point (x,y,z): ("<<x1[0]<<","<<x1[1]<<","<<x1[2]<<") with momentum (px,py,pz,p): ("<<p1[0]<<","<<p1[1]<<","<<p1[2]<<","<<sqrt(p1[0]*p1[0]+p1[1]*p1[1]+p1[2]*p1[2])<<")"<<endl;

  x2[0] = 0.;
  x2[1] = 0.;
  x2[2] = 0.;
  p2[0] = 0.;
  p2[1] = 0.;
  p2[2] = 0.;
  //TDatabasePDG* fdbPDG;
  //fdbPDG= TDatabasePDG::Instance();
  //Int_t GeantBeamCode=fdbPDG->ConvertPdgToGeant3(particlePDG);
  if(j==0)gMC3=static_cast<TGeant3 *> (TVirtualMC::GetMC());
  for(Int_t i=0; i<15; i++) {
      ein[i]=1.;
  }
  //Float_t pli[6];Float_t plo[12];
  pli[0] = 0.;
  pli[1] = 0.;
  pli[2] = 1.;
  pli[3] = 0.;
  pli[4] = 1.;
  pli[5] = 0.;
  plo[0] = 0.;
  plo[1] = 0.;
  plo[2] = 1.;
  plo[3] = 0.;
  plo[4] = 1.;
  plo[5] = 0.;
  //plo[6] = 0.;
  Double_t x1DCHrot,x2DCHrot,z1DCHrot,z2DCHrot; 
  if(j==0){ 
    plo[6] = DCH1_ZlayerPos_global[0];
    /*XYPoint xzDCHrot(endBeamPos[j][0]-XYZshift1.X(), 0.);
    xzDCHrot.Rotate(angleXZ);
    //x1DCHrot=xzDCHrot.X()+XYZshift1.X();
    //x2DCHrot=XYZshift1.X();
    //z1DCHrot=xzDCHrot.Y()+DCH1_ZlayerPos_global[0];
    z1DCHrot=xzDCHrot.Y()+endBeamPos[j][2];
    //z2DCHrot=DCH1_ZlayerPos_global[0]; 
    plo[6] = z1DCHrot;*/
  }else{
   plo[6] = DCH2_ZlayerPos_global[0];
    /*XYPoint xzDCHrot(endBeamPos[j][0]-XYZshift2.X(),0.);
    xzDCHrot.Rotate(angleXZ);
    //x1DCHrot=xzDCHrot.X()+XYZshift2.X();
    //x2DCHrot=XYZshift2.X();
    //z1DCHrot=xzDCHrot.Y()+DCH2_ZlayerPos_global[0];
    z1DCHrot=xzDCHrot.Y()+endBeamPos[j][2];
    //z2DCHrot=DCH2_ZlayerPos_global[0]; 
    plo[6] = z1DCHrot;*/
  }
  //LineLineIntersection2D(x1DCHrot, z1DCHrot, x2DCHrot, z2DCHrot, x3, y3, x4, y4, Double_t &xp, Double_t &yp)
  //plo[6] = DCH1_ZlayerPos_global[7];
  plo[7] = 0.;
  //plo[8] = 100.;
  //plo[8] = DCH1_ZlayerPos_global[0]; 
  plo[8] = 0.;
  gMC3->Eufilp(nepred, ein, pli, plo);
  //gMC3->Ertrak(x1,p1,x2,p2,GeantBeamCode, "BPO"); 
  //gMC3->Ertrak(x1,p1,x2,p2,GeantBeamCode, "PO"); 
  gMC3->Ertrak(x1,p1,x2,p2,GeantBeamCode, "P"); 
  //gMC3->Ertrak(x1,p1,x2,p2,GeantBeamCode, "POB"); 
  tanxz=p2[2]/p2[0];
  tanyz=p2[1]/p2[0];
  alphaxzBeam[j]=atan(tanxz);
  alphayzBeam[j]=atan(tanyz);
  for(UShort_t i=0; i<3; i++) {
   endBeamPos[j][i]=x2[i]; 
  }
  cout<<"Simulated track end point (x,y,z): ("<<x2[0]<<","<<x2[1]<<","<<x2[2]<<") with momentum (px,py,pz,p): ("<<p2[0]<<","<<p2[1]<<","<<p2[2]<<","<<sqrt(p2[0]*p2[0]+p2[1]*p2[1]+p2[2]*p2[2])<<")"<<" and angles (xz,yz):"<<TMath::RadToDeg()*atan(p2[2]/p2[0])<<","<<TMath::RadToDeg()*atan(p2[1]/p2[0])<<endl;
  cout<<"Simulated track end point - initial point (x,y,z): ("<<x2[0]-x1[0]<<","<<x2[1]-x1[1]<<","<<x2[2]-x1[2]<<") with momentum difference (px2-px1,py2-py1,pz2-pz1,p2-p1): ("<<p2[0]-p1[0]<<","<<p2[1]-p1[1]<<","<<p2[2]-p1[2]<<","<<sqrt((p2[0])*(p2[0])+(p2[1])*(p2[1])+(p2[2])*(p2[2]))-beamMomentum<<")"<<endl;

x5extrapBeam[0+2*j]=x2[2];
y5extrapBeam[0+2*j]=x2[1];
x5extrapBeam[1+2*j]=x2[2]+tanxz*(zLayer[j][1]-x2[0]);
y5extrapBeam[1+2*j]=x2[1]+tanyz*(zLayer[j][1]-x2[0]);
//cout<<"extrapBeam (plane 1, global frame): "<<x5extrapBeam[ll]<<" "<<y5extrapBeam[ll]<<" "<<zLayer[1][ijk5[ll]]<<" "<<endl;
//cout<<x1[ll]<<" "<<tgx[ll]<<" "<<tgx[ll]*(zLayer[1][ijk5[ll]]-z1[ll])<<endl;
//cout<<y1[ll]<<" "<<tgy[ll]<<" "<<tgy[ll]*(zLayer[1][ijk5[ll]]-z1[ll])<<endl;
//cout<<zLayer[1][ijk5[ll]]<<" "<<z1[ll]<<" "<<zLayer[1][ijk5[ll]]-z1[ll]<<endl;
for(Int_t i=0; i<2; i++) {
 x6extrapBeam[i+2*j]=x2[2]+tanxz*(zLayer[j][2+i]-x2[0]);
 y6extrapBeam[i+2*j]=x2[1]+tanyz*(zLayer[j][2+i]-x2[0]);
 x7extrapBeam[i+2*j]=x2[2]+tanxz*(zLayer[j][4+i]-x2[0]);
 y7extrapBeam[i+2*j]=x2[1]+tanyz*(zLayer[j][4+i]-x2[0]);
 x8extrapBeam[i+2*j]=x2[2]+tanxz*(zLayer[j][6+i]-x2[0]);
 y8extrapBeam[i+2*j]=x2[1]+tanyz*(zLayer[j][6+i]-x2[0]);
}
if(j==0){
 //for(UShort_t i=0; i<2; i++) { 
  //DbeamMomentumLoss=Dmomentum-sqrt((p2[0])*(p2[0])+(p2[1])*(p2[1])+(p2[2])*(p2[2]));
  beamMomentum2=sqrt((p2[0])*(p2[0])+(p2[1])*(p2[1])+(p2[2])*(p2[2]));
 //}
}
cout<<x5extrapBeam[0+2*j]<<" "<<y5extrapBeam[0+2*j]<<" "<<x6extrapBeam[0+2*j]<<" "<<y6extrapBeam[0+2*j]<<" "<<x7extrapBeam[0+2*j]<<" "<<y7extrapBeam[0+2*j]<<" "<<x8extrapBeam[0+2*j]<<" "<<y8extrapBeam[0+2*j]<<" "<<endl;
cout<<x5extrapBeam[1+2*j]<<" "<<y5extrapBeam[1+2*j]<<" "<<x6extrapBeam[1+2*j]<<" "<<y6extrapBeam[1+2*j]<<" "<<x7extrapBeam[1+2*j]<<" "<<y7extrapBeam[1+2*j]<<" "<<x8extrapBeam[1+2*j]<<" "<<y8extrapBeam[1+2*j]<<" "<<endl;
if(j==0){
cout<<DCH1_ZlayerPos_global[0]<<" "<<DCH1_ZlayerPos_global[0]+(zLayer[j][2]-x2[0])<<" "<<DCH1_ZlayerPos_global[0]+(zLayer[j][4]-x2[0])<<" "<<DCH1_ZlayerPos_global[0]+(zLayer[j][6]-x2[0])<<endl;
}else{
cout<<DCH2_ZlayerPos_global[0]<<" "<<DCH2_ZlayerPos_global[0]+(zLayer[j][2]-x2[0])<<" "<<DCH2_ZlayerPos_global[0]+(zLayer[j][4]-x2[0])<<" "<<DCH2_ZlayerPos_global[0]+(zLayer[j][6]-x2[0])<<endl;
}
//gMC3->Delete();
}
}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::GeantTracker(const Double_t parFitL[]){

  Float_t x1[3],p1[3],x2[3],p2[3];
  Float_t ein[15];
  Float_t pli[6], plo[9];
  Int_t nepred=1;
  //cout<<"Beam Geant code = "<<GeantBeamCode<<endl;

  //gMC3=static_cast<TGeant3 *> (TVirtualMC::GetMC());
  for(Int_t i=0; i<15; i++) {
      ein[i]=1.;
  }
  //cout<<"Track initial point2(x,y,z): ("<<x2[0]<<","<<x2[1]<<","<<x2[2]<<") with momentum(px,py,pz,p): ("<<p2[0]<<","<<p2[1]<<","<<p2[2]<<","<<sqrt(p2[0]*p2[0]+p2[1]*p2[1]+p2[2]*p2[2])<<")"<<endl;

  XYPoint xzDCHrot(parFitL[0]+parFitL[1]*DCH1_ZlayerPos_global[0]-XYZshift1.X(), 0.);
  xzDCHrot.Rotate(angleXZ);
  Double_t z1DCHrot=xzDCHrot.Y()+DCH1_ZlayerPos_global[0];
 
//
  //p1[0] = -p2[0];
  //p1[1] = -p2[1];
  //p1[0] = Dmomentum2;
  //p1[0] = p2[0];
  //p1[1] = 0.;
  //p1[1] = p2[1];
  p1[1] = parFitL[3]*beamMomentum2;
  //x1[0] = x2[0];
  //x1[1] = x2[1];
  //x1[2] = x2[2];
  //p1[2] = -Dmomentum2;
  //p1[2] = 0.;
  //p1[2] = p2[2];
  p1[2] = parFitL[1]*beamMomentum2;
  p1[0] = sqrt(beamMomentum2*beamMomentum2-p1[1]*p1[1]-p1[2]*p1[2]);
  //x1[0] = 18.;
  //x1[0] = DCH1_ZlayerPos_global[0];
  x1[0] = z1DCHrot;
  //x1[0] = 0.;
  //x1[1] = 0.;
  //x1[1] = x2[1];
  //x1[1] = parFitL[2]+parFitL[3]*DCH1_ZlayerPos_global[0];
  x1[1] = parFitL[2]+parFitL[3]*z1DCHrot;
  //x1[2] = DCH1_ZlayerPos_global[0];
  //x1[2] = 0.;
  //x1[2] = x2[2];
  //x1[2] = parFitL[0]+parFitL[1]*DCH1_ZlayerPos_global[0];
  x1[2] = parFitL[0]+parFitL[1]*z1DCHrot;
  x2[0] = 0.;
  x2[1] = 0.;
  x2[2] = 0.;
  p2[0] = 0.;
  p2[1] = 0.;
  p2[2] = 0.;
  pli[0] = 0.;
  pli[1] = 0.;
  pli[2] = 1.;
  pli[3] = 0.;
  pli[4] = 1.;
  pli[5] = 0.;
  plo[0] = 0.;
  plo[1] = 0.;
  plo[2] = 1.;
  plo[3] = 0.;
  plo[4] = 1.;
  plo[5] = 0.;
  //plo[6] = 0.;
  plo[6] = XYZ0beam.Z();
  //plo[6] = -DCH1_ZlayerPos_global[0];
  //plo[6] = DCH1_ZlayerPos_global[0];
  plo[7] = 0.;
  //plo[8] = 100.;
  //plo[8] = DCH1_ZlayerPos_global[0]; 
  plo[8] = 0.;
  if(checkDch)cout<<"Experimental track initial point (x,y,z): ("<<x1[0]<<","<<x1[1]<<","<<x1[2]<<") with momentum (px,py,pz,p): ("<<p1[0]<<","<<p1[1]<<","<<p1[2]<<","<<sqrt(p1[0]*p1[0]+p1[1]*p1[1]+p1[2]*p1[2])<<")"<<" and angles (xz,yz):"<<TMath::RadToDeg()*atan(parFitL[1])<<","<<TMath::RadToDeg()*atan(parFitL[3])<<endl;
  //gMC3->GetIonPdg(6,12,0);
  gMC3->Eufilp(nepred, ein, pli, plo);
  gMC3->Ertrak(x1,p1,x2,p2,GeantBeamCode, "PB"); 
  if(checkDch){
  cout<<"Experimental track end point (x,y,z): ("<<x2[0]<<","<<x2[1]<<","<<x2[2]<<") with momentum (px,py,pz,p): ("<<p2[0]<<","<<p2[1]<<","<<p2[2]<<","<<sqrt(p2[0]*p2[0]+p2[1]*p2[1]+p2[2]*p2[2])<<")"<<" and angles (xz,yz):"<<TMath::RadToDeg()*atan(p2[2]/p2[0])<<","<<TMath::RadToDeg()*atan(p2[1]/p2[0])<<endl;
  cout<<"Experimental track end point - initial point (x,y,z): ("<<x2[0]-x1[0]<<","<<x2[1]-x1[1]<<","<<x2[2]-x1[2]<<") with momentum difference (px2-px1,py2-py1,pz2-pz1,p2-p1): ("<<p2[0]-p1[0]<<","<<p2[1]-p1[1]<<","<<p2[2]-p1[2]<<","<<sqrt((p2[0])*(p2[0])+(p2[1])*(p2[1])+(p2[2])*(p2[2]))-sqrt((p1[0])*(p1[0])+(p1[1])*(p1[1])+(p1[2])*(p1[2]))<<")"<<endl;}
  if(geantCheck&&fDoCheck){
   deflectPointX->Fill(x2[2]); 
   deflectPointY->Fill(x2[1]); 
   deflectPointPx->Fill(p2[2]); 
   deflectPointPy->Fill(p2[1]); 
   deflectPointPz->Fill(p2[0]); 
  } 
  //gMC3->Gdebug();
//gMC3->Delete();
}
//-------------------------------------------------------------------------------------------------------------------------
//void BmnDchHitProducer_exp::GeantStepTracker(Float_t x1[],Float_t p1[],Float_t pli[],Float_t plo[],Int_t nepred,Float_t ein[],Int_t GeantBeamCode, const Bool_t backward, Float_t (&x2)[3], Float_t (&p2)[3]){
void BmnDchHitProducer_exp::GeantStepTracker(Float_t x1[],Float_t p1[],Float_t pli[],Float_t plo[],Int_t nepred,Float_t ein[],Int_t geantBeamCode, const Bool_t backward, Float_t x2[], Float_t p2[]){


  Option_t* chopt;
  if(backward){chopt="PB";}else{chopt="P";}
  gMC3->Eufilp(nepred, ein, pli, plo);
  gMC3->Ertrak(x1,p1,x2,p2,GeantBeamCode,chopt);

}
//-------------------------------------------------------------------------------------------------------------------------
//Double_t BmnDchHitProducer_exp::MagFieldIntegral(const Double_t *parFitL){
Double_t BmnDchHitProducer_exp::MagFieldIntegral(const Bool_t backward, const Double_t parFitL[]){

  Float_t x1[3],p1[3],x2[3],p2[3];
  Float_t ein[15];
  Float_t pli[6];Float_t plo[9];
  Int_t nepred=1;
  //gMC3=static_cast<TGeant3 *> (TVirtualMC::GetMC());
  pli[0] = 0.;
  pli[1] = 0.;
  pli[2] = 1.;
  pli[3] = 0.;
  pli[4] = 1.;
  pli[5] = 0.;
  plo[0] = 0.;
  plo[1] = 0.;
  plo[2] = 1.;
  plo[3] = 0.;
  plo[4] = 1.;
  plo[5] = 0.;
  plo[7] = 0.;
  plo[8] = 0.;

  for(Int_t i=0; i<15; i++) {
      ein[i]=1.;
  }

  Double_t integral_xz=0.,z0,delz,integral_yz;
  Double_t zstep=1.;// 1cm, except for the last step

  if(backward){
   p1[1] = parFitL[3]*beamMomentum2;
   p1[2] = parFitL[1]*beamMomentum2;
   p1[0] = sqrt(beamMomentum2*beamMomentum2-p1[1]*p1[1]-p1[2]*p1[2]);
   x1[0] = DCH1_ZlayerPos_global[0];
   x1[1] = parFitL[2]+parFitL[3]*DCH1_ZlayerPos_global[0];
   x1[2] = parFitL[0]+parFitL[1]*DCH1_ZlayerPos_global[0]; 
   //plo[6] = XYZ0beam.Z();
   plo[6] = DCH1_ZlayerPos_global[0]-zstep;
  }else{
   p1[0] = beamMomentum;
   p1[1] = 0.;
   p1[2] = 0.;
   x1[0] = XYZ0beam.Z();
   x1[1] = 0.;
   x1[2] = 0.;
   //plo[6] = DCH1_ZlayerPos_global[0];
   plo[6] = XYZ0beam.Z()+zstep;
  } 
 
  x2[0] = 0.;
  x2[1] = 0.;
  x2[2] = 0.;
  p2[0] = 0.;
  p2[1] = 0.;
  p2[2] = 0.; 

  const UInt_t nsteps=UInt_t((DCH1_ZlayerPos_global[0]-XYZ0beam.Z())/zstep);
  //cout<<"nsteps = "<<nsteps<<endl;

  for (UShort_t i = 0; i <= nsteps; i++) {
   integral_xz+=zstep*magField->GetBy(x1[0],x1[1],x1[2]);
   integral_yz+=zstep*magField->GetBx(x1[0],x1[1],x1[2]);
   //cout<<"zstep = "<<zstep<<endl;
   //cout<<"x,y,z,istep: "<<x1[0]<<" "<<x1[1]<<" "<<x1[2]<<" "<<i<<endl;
   //cout<<"mag. field (By) = "<<magField->GetBy(x1[0],x1[1],x1[2])<<", preliminary integral(xz) = "<<integral_xz<<endl;
   //cout<<"mag. field (Bx) = "<<magField->GetBx(x1[0],x1[1],x1[2])<<", preliminary integral(yz) = "<<integral_yz<<endl;
   if(i==nsteps)continue;
   GeantStepTracker(x1, p1, pli, plo, nepred, ein, GeantBeamCode, backward, x2, p2);
   //if((i==(nsteps-1))&&!backward){
    //cout<<"bx = "<<magField->GetBx(x2[0],x2[1],x2[2])<<", by = "<<magField->GetBy(x2[0],x2[1],x2[2])<<", bz = "<<magField->GetBz(x2[0],x2[1],x2[2])<<", x = "<<x2[0]<<", y = "<<x2[1]<<", z = "<<x2[2]<<endl;
    //cout<<"geant beam xz deflection angle = "<<TMath::RadToDeg()*atan(x2[2]-x1[2])/(x2[0]-x1[0])<<" "<<p2[2]<<" "<<p2[1]<<" "<<p2[0]<<" "<<x2[2]<<" "<<x2[1]<<" "<<x2[0]<<endl; 
   //} 
   p1[0] = p2[0];
   p1[1] = p2[1];
   p1[2] = p2[2];
   x1[0] = x2[0];
   x1[1] = x2[1];
   x1[2] = x2[2];
   if(i==(nsteps-1)){
    if(backward){
     zstep=plo[6]-XYZ0beam.Z();
     //plo[6]=XYZ0beam.Z();
    }else{
     zstep=DCH1_ZlayerPos_global[0]-plo[6];
     //plo[6]=DCH1_ZlayerPos_global[0];
    }
   }else{
    if(backward){plo[6]=plo[6]-zstep;}else{plo[6]=plo[6]+zstep;}
   } 
  }
 
  integral_xz=integral_xz*0.1*0.01; //kGauss.cm->Tm 
  integral_yz=integral_yz*0.1*0.01; //kGauss.cm->Tm 
  if(backward){
   integral_xz=-integral_xz;
   integral_yz=-integral_yz;
  }
  if(checkDch||!backward){
   cout<<"mag. field integral(xz) = "<<integral_xz<<endl;
   cout<<"mag. field integral(yz) = "<<integral_yz<<endl;
  }

return integral_xz;

}

//---------------------------------------------------------------------------
/*void BmnDchHitProducer_exp::MeanTrackLocalPosition(UShort_t numCham){

Double_t x[numLayers_half],y[numLayers_half];

  for(UShort_t i=0; i<numLayers_half; i++) {
   int binmax = hXhit[numCham][i]->GetMaximumBin(); 
   x[i] = hXhit[numCham][i]->GetXaxis()->GetBinCenter(binmax);
   binmax = hYhit[numCham][i]->GetMaximumBin(); 
   y[i] = hYhit[numCham][i]->GetXaxis()->GetBinCenter(binmax);
   binmax = hZhit[numCham][i]->GetMaximumBin(); 
   z[i] = hZhit[numCham][i]->GetXaxis()->GetBinCenter(binmax);
  }

  TGraph *grx = new TGraph(numLayers_half,z,x);
  TGraph *gry = new TGraph(numLayers_half,z,y);

  grx->Fit("pol1");  
  fitx = grx->GetFunction("pol1");
  gry->Fit("pol1");  
  fity = gry->GetFunction("pol1");

  xTrackPos=fitx->Eval(0.);
  yTrackPos=fity->Eval(0.);
  
  

}*/
//---------------------------------------------------------------------------
void BmnDchHitProducer_exp::LineLineIntersection2D(const Double_t x1, const Double_t y1, const Double_t x2, const Double_t y2, const Double_t x3, const Double_t y3, const Double_t x4, const Double_t y4, Double_t &xp, Double_t &yp){

Double_t k1,q1; //x1,y1,x2,y2 - first line
Double_t k2,q2; //x3,y3,x4,y4 - second line
// xp,yp - intersection point

//x1=0;y1=1;x2=1;y2=0;
//x3=0;y3=-1;x4=2;y4=2;

if(x2==x1&&x4==x3){
 if(x2==x4){
  cout<<"Both lines coincide along z axis!"<<endl;
  gApplication->Terminate();
 }else{
  cout<<"Lines are parallel along z axis!"<<endl;
  gApplication->Terminate();
 } 
}else if(x2==x1&&x4!=x3){
 k2=(y4-y3)/(x4-x3);
 q2=y3-k2*x3;
 cout<<"k2 = "<<k2<<", q2 = "<<q2<<endl;
 xp=x2;
 yp=k2*x2+q2;
}else if(x2!=x1&&x4==x3){
 k1=(y2-y1)/(x2-x1);
 q1=y1-k1*x1;
 cout<<"k1 = "<<k1<<", q1 = "<<q1<<endl;
 xp=x4;
 yp=k1*x4+q1;
}else if(x2!=x1&&x4!=x3){
 k1=(y2-y1)/(x2-x1);
 k2=(y4-y3)/(x4-x3);
 q1=y1-k1*x1;
 q2=y3-k2*x3;
 cout<<"k1 = "<<k1<<", q1 = "<<q1<<endl;
 cout<<"k2 = "<<k2<<", q2 = "<<q2<<endl;
 if(k1!=k2){
  xp=(q1-q2)/(k2-k1);
  yp=(q1*k2-q2*k1)/(k2-k1);
 }else if(k1==k2){
  if(q1==q2){
   if(k1==0.){cout<<"Both lines coincide along x axis!"<<endl;}
   else{cout<<"Both lines coincide!"<<endl;}
   gApplication->Terminate();
  }else{
   if(k1==0.){cout<<"Lines are parallel along x axis!"<<endl;}
   else{cout<<"Lines are parallel along x axis!"<<endl;}
   gApplication->Terminate();
  }
 }
}

cout<<"intersection point (x,y): "<<xp<<", "<<yp<<endl;

}
//---------------------------------------------------------------------------
void BmnDchHitProducer_exp::rtCalibrationReader(){

         ifstream DCHparFile;
         DCHpar=inDirectory+DCHpar;
         DCHparFile.open (DCHpar);
         Double_t tmp;
         //DCHparFile >> ab >>endl;
         // alignment parameters
         DCHparFile >> tmp;
         XYZshift1.SetX(tmp);
         DCHparFile >> tmp;
         XYZshift1.SetY(tmp);
         DCHparFile >> tmp;
         XYZshift1.SetZ(tmp);
         DCHparFile >> tmp;
         XYZshift2.SetX(tmp);
         DCHparFile >> tmp;
         XYZshift2.SetY(tmp);
         DCHparFile >> tmp;
         XYZshift2.SetZ(tmp);
         DCHparFile >> ranmin_cham[0];
         DCHparFile >> ranmax_cham[0];
         DCHparFile >> ranmin_cham[1];
         DCHparFile >> ranmax_cham[1];
         for (UShort_t j = 0; j < numChambers; j++) {
           for (UShort_t k = 0; k < numLayers_half; k++) {
             DCHparFile >> ranmin[j][k];
           }
         }
         for (UShort_t j = 0; j < numChambers; j++) {
           for (UShort_t k = 0; k < numLayers_half; k++) {
             DCHparFile >> ranmax[j][k];
           }
         }
         for (UShort_t j = 0; j < 4; j++) {
           for (UShort_t k = 0; k < 4; k++) {
             DCHparFile >> C(j,k);
           }
         }
         DCHparFile.close();

        // residual spectra
        //TString inPut=inDirectory+residualsFileName;
        TString inPut; 
        if(strcmp(opt,"reconstruction")==0&&isTarget){
         inPut=inDirectory+residualsFileNameReco;
        }else{
         inPut=inDirectory+residualsFileName;
        } 
        TFile resid(inPut,"read");

        cout<<"input calibration file for reconstruction: "<<inPut<<endl;
        //resid.ls();

        TString str3,str,histname;
        for (UShort_t j = 0; j < numChambers; j++) {
         str3.Form("%d",j+1);
         histname=TString("ResidVsTDCx")+str3;
         TH2D* residVsTDCx = (TH2D*)resid.Get(histname);
         histname=TString("ResidVsTDCx_y")+str3;
         TH2D* residVsTDCx_y = (TH2D*)resid.Get(histname);
         histname=TString("ResidVsTDCx_u")+str3;
         TH2D* residVsTDCx_u = (TH2D*)resid.Get(histname);
         histname=TString("ResidVsTDCx_v")+str3;
         TH2D* residVsTDCx_v = (TH2D*)resid.Get(histname);
         histname=TString("ResidVsTDCy")+str3;
         TH2D* residVsTDCy = (TH2D*)resid.Get(histname);
         histname=TString("ResidVsTDCy_x")+str3;
         TH2D* residVsTDCy_x = (TH2D*)resid.Get(histname);
         histname=TString("ResidVsTDCy_u")+str3;
         TH2D* residVsTDCy_u = (TH2D*)resid.Get(histname);
         histname=TString("ResidVsTDCy_v")+str3;
         TH2D* residVsTDCy_v = (TH2D*)resid.Get(histname);
         histname=TString("ResidVsTDCu")+str3;
         TH2D* residVsTDCu = (TH2D*)resid.Get(histname);
         histname=TString("ResidVsTDCv")+str3;
         TH2D* residVsTDCv = (TH2D*)resid.Get(histname);

         TH1D* projResidVsTDCx = (TH1D*)residVsTDCx->ProjectionY("projResidVsTDCx");
         TH1D* projResidVsTDCx_y = (TH1D*)residVsTDCx_y->ProjectionY("projResidVsTDCx_y");
         TH1D* projResidVsTDCx_u = (TH1D*)residVsTDCx_u->ProjectionY("projResidVsTDCx_u");
         TH1D* projResidVsTDCx_v = (TH1D*)residVsTDCx_v->ProjectionY("projResidVsTDCx_v");
         dx[j]=HWHM(projResidVsTDCx);
         dx_y[j]=HWHM(projResidVsTDCx_y);
         dx_u[j]=HWHM(projResidVsTDCx_u);
         dx_v[j]=HWHM(projResidVsTDCx_v);

	 TH1D* projResidVsTDCy = (TH1D*)residVsTDCy->ProjectionY("projResidVsTDCy");
	 TH1D* projResidVsTDCy_x = (TH1D*)residVsTDCy_x->ProjectionY("projResidVsTDCy_x");
	 TH1D* projResidVsTDCy_u = (TH1D*)residVsTDCy_u->ProjectionY("projResidVsTDCy_u");
	 TH1D* projResidVsTDCy_v = (TH1D*)residVsTDCy_v->ProjectionY("projResidVsTDCy_v");
	 dy[j]=HWHM(projResidVsTDCy);
	 dy_x[j]=HWHM(projResidVsTDCy_x);
	 dy_u[j]=HWHM(projResidVsTDCy_u);
	 dy_v[j]=HWHM(projResidVsTDCy_v);
	 TH1D* projResidVsTDCu = (TH1D*)residVsTDCu->ProjectionY("projResidVsTDCu");
	 du[j]=HWHM(projResidVsTDCu);
	 TH1D* projResidVsTDCv = (TH1D*)residVsTDCv->ProjectionY("projResidVsTDCv");
	 dv[j]=HWHM(projResidVsTDCv);
	 cout<<"HWHM V,U,Y,X errors in DCH"<<str3<<" are: "<<dv[j]<<", "<<du[j]<<", "<<dy[j]<<", "<<dx[j]<<endl; 
	 cout<<"HWHM X_V,X_U,X_Y errors in DCH"<<str3<<" are: "<<dx_v[j]<<", "<<dx_u[j]<<", "<<dx_y[j]<<endl; 
	 cout<<"HWHM Y_V,Y_U,Y_X errors in DCH"<<str3<<" are: "<<dy_v[j]<<", "<<dy_u[j]<<", "<<dy_x[j]<<endl; 
	 cout<<"RMS V,U,Y,X errors in DCH"<<str3<<" are: "<<projResidVsTDCv->GetRMS()<<", "<<projResidVsTDCu->GetRMS()<<", "<<projResidVsTDCx->GetRMS()<<", "<<projResidVsTDCy->GetRMS()<<endl; 
	 cout<<"RMS X_V,X_U,X_Y errors in DCH"<<str3<<" are: "<<projResidVsTDCx_v->GetRMS()<<", "<<projResidVsTDCx_u->GetRMS()<<", "<<projResidVsTDCx_y->GetRMS()<<endl; 
	 cout<<"RMS Y_V,Y_U,Y_X errors in DCH"<<str3<<" are: "<<projResidVsTDCy_v->GetRMS()<<", "<<projResidVsTDCy_u->GetRMS()<<", "<<projResidVsTDCy_x->GetRMS()<<endl; 
	 /*dy[j]=projResidVsTDCy->GetRMS();
	   dx[j]=projResidVsTDCx->GetRMS();
	   du[j]=projResidVsTDCu->GetRMS();
	   dv[j]=projResidVsTDCv->GetRMS();*/
	 //str.Form("%u",j);
	 /*if(j==0){
	   delete residVsTDCy,residVsTDCx,residVsTDCu,residVsTDCu;
	   delete projResidVsTDCy,projResidVsTDCx,projResidVsTDCu,projResidVsTDCu;
	   }*/
	}          

        histname=TString("trackAngleXbf1");
        TH1D* trackAngleXbf1 = (TH1D*)resid.Get(histname);
        histname=TString("trackAngleXbf2");
        TH1D* trackAngleXbf2 = (TH1D*)resid.Get(histname);
        histname=TString("trackAngleYbf1");
        TH1D* trackAngleYbf1 = (TH1D*)resid.Get(histname);
        histname=TString("trackAngleYbf2");
        TH1D* trackAngleYbf2 = (TH1D*)resid.Get(histname);
        histname=TString("trackAngleXaf");
        TH1D* trackAngleXaf = (TH1D*)resid.Get(histname);
        histname=TString("trackAngleYaf");
        TH1D* trackAngleYaf = (TH1D*)resid.Get(histname);

        Float_t sigmaTx[numChambers],sigmaTy[numChambers],sigmaTxaf,sigmaTyaf;
        Float_t angleXZ1=TMath::DegToRad()*trackAngleXbf1->GetMean(),angleYZ1=TMath::DegToRad()*trackAngleYbf1->GetMean(),angleXZ2=TMath::DegToRad()*trackAngleXbf2->GetMean(),angleYZ2=TMath::DegToRad()*trackAngleYbf2->GetMean(); 

        Float_t angleXZaf=TMath::DegToRad()*trackAngleXaf->GetMean(); 
        Float_t angleYZaf=TMath::DegToRad()*trackAngleYaf->GetMean(); 

        cout<<angleXZ1<<" "<<angleYZ1<<" "<<angleXZ2<<" "<<angleYZ2<<" "<<endl;
        //cout<<HWHM(trackAngleXbf1)<<" "<<angleXY1<<" "<<<angleXZ2<<" "<<angleXY2<<" "<<end;

        sigmaTx[0]=TMath::DegToRad()*HWHM(trackAngleXbf1)/pow(cos(angleXZ1),2.);
        sigmaTy[0]=TMath::DegToRad()*HWHM(trackAngleYbf1)/pow(cos(angleYZ1),2.);
        sigmaTx[1]=TMath::DegToRad()*HWHM(trackAngleXbf2)/pow(cos(angleXZ2),2.);
        sigmaTy[1]=TMath::DegToRad()*HWHM(trackAngleYbf2)/pow(cos(angleYZ2),2.);
        sigmaTxaf=TMath::DegToRad()*HWHM(trackAngleXaf)/pow(cos(angleXZaf),2.);
        sigmaTyaf=TMath::DegToRad()*HWHM(trackAngleYaf)/pow(cos(angleYZaf),2.);

        cout<<"HWHM error of track tgxz and tgyz in DCH1 = "<<sigmaTx[0]<<" and "<<sigmaTy[0]<<endl;
        cout<<"HWHM error of track tgxz and tgyz in DCH2 = "<<sigmaTx[1]<<" and "<<sigmaTy[1]<<endl;
        cout<<"RMS error of track tgxz and tgyz in DCH1 = "<<TMath::DegToRad()*trackAngleXbf1->GetRMS()/pow(cos(angleXZ1),2.)<<" and "<<TMath::DegToRad()*trackAngleYbf1->GetRMS()/pow(cos(angleYZ1),2.)<<endl;
        cout<<"RMS error of track tgxz and tgyz in DCH2 = "<<TMath::DegToRad()*trackAngleXbf2->GetRMS()/pow(cos(angleXZ2),2.)<<" and "<<TMath::DegToRad()*trackAngleYbf2->GetRMS()/pow(cos(angleYZ2),2.)<<endl;


	//resid.Close(); 

	// tracks covariance matrix for DCH1,DCH2
	

	for (UShort_t k = 0; k < 6; k++) {
	 covMat[k].ResizeTo(6,6);
	 for (UShort_t i = 0; i < 5; i++) {
		for (UShort_t j = 0; j < 5; j++) {
			covMat[k](i,j)=0.;
		}
	 }
	}
        
        //DCH1
	covMat[0](0,0)=dx[0];covMat[0](1,1)=dy_x[0];covMat[0](2,2)=sigmaTx[0];covMat[0](3,3)=sigmaTy[0]; 
	covMat[1](0,0)=dx_v[0];covMat[1](1,1)=dy_v[0];covMat[1](2,2)=sigmaTx[0];covMat[1](3,3)=sigmaTy[0]; 
        //DCH2
	covMat[2](0,0)=dx[1];covMat[2](1,1)=dy_x[1];covMat[2](2,2)=sigmaTx[1];covMat[2](3,3)=sigmaTy[1];
	covMat[3](0,0)=dx_v[1];covMat[3](1,1)=dy_v[1];covMat[3](2,2)=sigmaTx[1];covMat[3](3,3)=sigmaTy[1];
        //DCH1+DCH2
	covMat[4](0,0)=dx[0];covMat[4](1,1)=dy[0];covMat[4](2,2)=sigmaTx[0];covMat[4](3,3)=sigmaTy[0];
	covMat[5](0,0)=dx[1];covMat[5](1,1)=dy[1];covMat[5](2,2)=sigmaTx[1];covMat[5](3,3)=sigmaTy[1];
  
        TString str2,str4,calibname;
          
        // calibration curves
        //TFile resid2(inPut,"read");

        for (UShort_t j = 0; j < numChambers; j++) {
         str2.Form("%d",j);
         calibname=TString("calibr")+str2;       
         calib_cham[j]=(TGraph*)resid.Get(calibname);
         for (UShort_t i = 0; i < numLayers_half; i++) {
         str4.Form("%d",i);
         calibname=TString("calibr")+str2+str4;
         calib[j][i]=(TGraph*)resid.Get(calibname);
         spline5rt[j][i] = new TSpline5("r-t calibration curve",calib[j][i], "", 0.,0.,0.,0.);
         }
        } 
        resid.Close(); 
        //resid2.Close(); 
         
}
//---------------------------------------------------------------------------
void BmnDchHitProducer_exp::FileManagement( ){

        TString baseDirectory="/home2/bmn_expdata/run";
        baseDirectory=baseDirectory+TString::Itoa(runPeriod,10);

        if(strcmp(opt,"reconstruction")==0){
          inDirectory=baseDirectory+"/calibration/";           
          outDirectory=baseDirectory+"/reconstruction/"; 
        }else{
          inDirectory=baseDirectory+"/digit/";           
          outDirectory=baseDirectory+"/calibration/"; 
        }

}
//---------------------------------------------------------------------------
double BmnDchHitProducer_exp::HWHM(const TH1D *h1){

   int bin1 = h1->FindFirstBinAbove(h1->GetMaximum()/2.);
   int bin2 = h1->FindLastBinAbove(h1->GetMaximum()/2.);
   double hwhm = 0.5*(h1->GetBinCenter(bin2) - h1->GetBinCenter(bin1));

return hwhm;

}
//---------------------------------------------------------------------------
UInt_t BmnDchHitProducer_exp::runSelection(){

   UInt_t linecounter=1;
   ifstream file("goodRuns");
   uint i;
   Bool_t runPeriodFound=false;
   UInt_t runvecsize=0;
   std::vector<uint> runvec;
   //while(!file.eof()){
   while(!runPeriodFound){
   string line;
   getline(file, line);
    //cout<<line<<endl;
    if(linecounter==runPeriod){
     istringstream fin(line);
	     while(fin>>i){ //loop till end of line
      //cout<<i<<endl;
      runvec.push_back(i);
     }
     runPeriodFound=true;
     runvecsize=runvec.size();
    }
    linecounter++;
   }
   for (UInt_t j = 0; j < runvecsize; j++) {
        cout << runvec[j] << endl;
   }

   UInt_t randIndex = rand() % runvecsize;
   cout<<"randIndex, runvec:" <<randIndex<<" "<<runvec[randIndex]<<endl;

   return runvec[randIndex];

}
//---------------------------------------------------------------------------
Bool_t BmnDchHitProducer_exp::areSame(double a, double b){

return std::fabs(a - b) < std::numeric_limits<double>::epsilon();

}
//---------------------------------------------------------------------------
Int_t BmnDchHitProducer_exp::geantCode(TString particle){

Int_t geantcode; 
if(strcmp(particle,"d")==0){
 geantcode=45; 
}else if(strcmp(particle,"C")==0){
 geantcode=67; 
}else{
 cout<<"Unknown GEANT particle!"<<endl;	
 gApplication->Terminate();
}

return geantcode;

}
//---------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::geantMass(Int_t geantcode){

Double_t geantmass;
if(geantcode==45){
 geantmass=1.875613; 
}else if(geantcode==67){
 geantmass=11.17793; 
}else{
 cout<<"Unknown GEANT particle mass!"<<endl;	
 gApplication->Terminate();
}

return geantmass;

}
//---------------------------------------------------------------------------
ClassImp(BmnDchHitProducer_exp)
