// -------------------------------------------------------------------------
// -----                   CbmStsStripSensor source file               -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------

#include "CbmStsSensor.h"

#include "CbmStsSensorDigiPar.h"

#include "TGeoManager.h" //AZ
#include "TMath.h"
#include "TRandom.h"



#include <iostream>
#include <list>
#include <vector>

using std::cout;
using std::endl;
using std::pair;
using std::vector;




// -----   Default constructor   -------------------------------------------
CbmStsSensor::CbmStsSensor()
  :
  fDetectorId(0),
  fType(0),
  fX0(0.),
  fY0(0.),
  fZ0(0.),
  fRotation(0.),
  fLx(0.),
  fLy(0.),
  fDx(0.),
  fDy(0.),
  fStereoF(0.),
  fStereoB(0.),
  fD(0.),
  fNChannelsFront(0),
  fNChannelsBack(0),
  fBackStripShift(0.),
  fFrontStripShift(0.),
  fSigmaX(0.),
  fSigmaY(0.),
  fSigmaXY(0.),
  fXSmearWidth(0.0005),
  fZSmearSlope(0.04),
  fFrontLorentzShift(0), //0.132 0
  fBackLorentzShift(0), //0.132 0
  fFrontActive(),
  fBackActive(),
  fTrueHits()
{
  flg=0;
  cout << "-W- CbmStsSensor: Do not use this constructor! " << endl;
}
// -------------------------------------------------------------------------

//Double_t xBeam[10][2][2]={ 135.052, 115.921, 0.,0., 393.75, 446.889, 0.,0.,
		//	   391.245, 443.836, 0.,0., 393.75, 446.889, 0.,0., 63.2164, 128.129, 0.,0., -17.2712, 51.3482, 0.,0.,
		//	   3.96189, 69.3356, 0.,0., 1025.26, 1056.42, 0.,0., -10.3345, 55.9771, 0.,0., 1011., 1042.84, 0.,0.  }; //run 5
/*
Double_t xBeam[10][2]= { 135.052, 115.921,  393.75, 446.889,  393.125, 443.27,  393.75, 446.889,
               66.1164, 129.809,  -20.1712, 47.8682,  -0.03811, 64.9356,  1021.16, 1051.42,
               -0.7045, 63.3531,  1020.38, 1050.77}; //run 6



         const Double_t ColdStripBias[4][2]={   0.,0.,   0., 101.,   0., 97.,   519., 623.};
         const Int_t mStrLayerAct[4][2][2]={ 256,256,0,0, 825,930,190,215, 1019,1130,500,506, 1019,1130,500,506 };
         Int_t xDir[10]={ 1, -1,-1,-1,      1,-1,     -1,-1,     1,1}; // right system
         const Double_t pt[10]={ 0.04,0.08,0.08,0.08,0.08, 0.08,0.08,0.08,0.08,0.08};
         const Int_t ModType[10]={ 0, 1,1,1, 1,1, 3,2,2,3}; // тип камеры
         const Double_t ySi[8]={6.67738,6.74426,6.6588,6.69745,      -6.67738,-6.36139,-6.34705,-6.48174};

const Int_t ModTypeS[8]={7,5,4,6,    6,4,5,7 };//7,5,4,6,6,4,5,7 };//7, 6, 7, 6, 4, 4, 5, 5};
const Double_t pRotS[8]={ 0,0,0,0, 180,180,180,180};//0., 0., 180., 180., 0., 180., 180., 0.};
Bool_t flgBoundary=false;
Int_t TypeSi;

Bool_t flgBound=true;
*/

//------------------run7----------------------------- 
//ruf calculated
/*
const int MPlanes=14; // 6*2 GEMS + 1*2 CSC

 const double arZ0[ MPlanes]={42.2, 42.2,  67.77,67.77,  115.484,115.654,  137.815,137.85,  163.55, 163.53, 186.103, 186.103, -100,-100 };
 //{42.4323, 42.1404, 67.7787, 68.0303, 115.582, 115.983, 138.213, 138.024, 163.735, 163.96, 186.595, 186.422,412.5,412.5};
 //{43.9323, 43.6404, 69.2787, 69.5303, 117.082, 117.483,  
			     //139.713, 139.524, 165.235, 165.46, 188.095, 187.922, 414.,414.};
const double arX0[ MPlanes]={-40.716, 40.914, 41.085, -40.598, 42.278, -39.429, -38.45, 43.25,  44.076, -37.596,  -36.8092, 44.8207, -100,-100};
//{-40.7102, 40.9414, 41.1033, -40.5867, 42.3003, -39.4213, -38.4425, 43.2731, 44.1035, -37.5865, -36.7989, 44.8606, 0.,0.}; 
const double arY0[ MPlanes]={22.5211, 22.2759, 22.574, 22.6124,  22.8827,  22.516, 22.5385, 22.9377, 22.5828, 22.3481, 22.7875, 23.007, -100, -100};
//{22.2753, 22.4706, 22.4851, 22.6689, 22.7851, 22.5916,  22.3308, 23.1519, 22.4786, 22.4484, 22.5971, 23.2184, 0.,0.}; 
const double AlphaZmRad[ MPlanes]={-1.31, -1.31, -0.6,-1.13, 2.59,3.59,  5.67,7,  1.52,0.52,  3.31,3.31,  -100,-100};
//{ -2.26131, -3.94131, -2.24914, -2.24914, 1.3594, 3.1594,5.02441, 6.26441, 0.370423, 0.370423, 3.08637, 2.18637, 0.,0.}; 
*/
/* //v2
const double arX0[ 26 ]=
{3.32829,-2.67635,2.83055,-3.16945, //I Si
  2.84975, -3.11505, //II Si
  8.91718,2.91888,-3.09552,-9.09292,  8.91618,2.91618,-3.08382,-9.08382, //III Si
  -40.716, 40.914, 41.085, -40.598, 42.278, -39.429, -38.45, 43.25,  44.076, -37.596,  -36.8092, 44.8207};//GEM
const double arY0[ 26 ]=
  {4.46297,5.07597, -1.55503,-1.05503, //I Si
   1.8115,1.42, //II Si
   7.84856,7.80306,7.92256,7.85506,-4.25594,-4.25594, -4.25594,-4.25594, // III Si  
   22.5211, 22.2759, 22.574, 22.6124,  22.8827,  22.516, 22.5385, 22.9377, 22.5828, 22.3481, 22.7875, 23.007};//GEM
 const double arZ0[ 26 ]=
  {14.05-0.025 ,14.68-0.025, 14.73-0.025, 14.0-0.025, //I Si
   17.36-0.05,18.02-0.05,  //II Si
   26.91,25.31,26.83,25.26,25.65, 27.26,25.79,27.4,  // III Si  
   42.2, 42.2,  67.77,67.77,  115.484,115.654,  137.815,137.85,  163.55, 163.53, 186.103, 186.103};//GEM
const double AlphaZmRad[ 26 ]=
{2.5,8,-1,-1, //I Si
-1.5,10, //II Si
 5.5,5.5,10,10,8,8,8,8, //III Si
-1.31, -1.31, -0.6,-1.13, 2.59,3.59,  5.67,7,  1.52,0.52,  3.31,3.31};//GEM
*/

const double arX0[ 26 ]=
{3.33029,-2.67835,2.82585 ,-3.14615, //I Si
  2.85175, -3.11705, //II Si
  8.91918 ,2.92088, -3.09752,-9.09492,  8.91198 ,2.94528 ,-2.98182,-8.98402, //III Si
  -40.718 , 40.916, 41.087, -40.6, 42.28, -39.431, -38.452, 43.252,  44.078, -37.598,  -36.8112, 44.8227};//GEM

const double arY0[ 26 ]=
  {4.46297,5.07597, -1.48803,-0.910531, //I Si
   1.8115,1.42, //II Si
   7.84856,7.80306,7.92256,7.85506,-4.57154,-4.61864, -4.43934,-4.51954, // III Si  
 //  22.5211-0.17, 22.2759-0.17, 22.574-0.15, 22.6124-0.15,  22.8827-0.1,  22.516-0.1, 22.5385-0.05, 22.9377-0.05, 22.5828+0.025, 22.3481+0.025, 22.7875+0.07, 23.007+0.07};//GEM
   22.5211, 22.2759, 22.574, 22.6124,  22.8827,  22.516, 22.5385, 22.9377, 22.5828, 22.3481, 22.7875, 23.007};//GEM



 const double arZ0[ 26 ]=
  {13.9461  ,14.7834, 14.6265 , 14.0539, //I Si
   17.2581,18.1214,  //II Si
   26.8138,25.2129,26.9261,25.357,25.5531 , 27.034,25.8867,27.3958,  // III Si  
   42.2868, 42.1131,  67.6985,67.8414,  115.441,115.697,  137.844,137.821,  163.536, 163.544, 186.103, 186.103};//GEM
const double AlphaZmRad[ 26 ]=
{2.5,8,-2,2, //I Si
-1.5,10, //II Si
 5.5,5.5,10,10,5,2,8,8, //III Si
-1.31, -1.31, -0.6,-1.13, 2.59,3.59,  5.67,7,  1.52,0.52,  3.31,3.31};//GEM


  const Double_t ColdStripBias[4][2]={   0.,0.,   0., 101.,   0., 97.,   519., 623.};
  const Int_t mStrLayerAct[4][2][2]={ 256,256,0,0, 825,930,190,215, 1019,1130,500,506, 1019,1130,500,506 };

  const Double_t ModSiSize[3][2]={6.08,6.089, 6.08,12.178, 6.08,12.178}; // 6.089 to 6.08
    const Double_t YSiShift[3]={0.24795,0.2256361,0.2256361};//{0.0120414, -0.0102728 ,-0.0102728 }; // 6.089 to 6.08

const Int_t ip_r7[6][2]={0,1, 2,3, 4,5, 6,7, 8,9, 10,11};

const Int_t ModType_r7[6][2]={ 2,3, 2,3, 2,3, 2,3, 2,3, 2,3}; // тип камеры


const Int_t modip[3][8]={ 0,1,2,3,-1,-1,-1,-1, 4,5,-1,-1,-1,-1,-1,-1, 6,7,8,9, 10,11,12,13};

Bool_t flgBoundary=false;
Bool_t flgBound=true;

 //Ruf 
  void CbmStsSensor::SetRotZ( double angleMRad, double &x, double &y) {
    //GP FIX
    double a=angleMRad*0.001;
    double c=TMath::Cos(a);
    double s=TMath::Sin(a);
    double xx=x;//-fX0;
    double yy=y;//-fY0;
    x= xx*c-yy*s;//+fX0;
    y= xx*s+yy*c;//+fY0;
  }


//-------------------run7----------------------------

// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
CbmStsSensor::CbmStsSensor(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0,
			   Double_t rotation, Double_t lx, Double_t ly,
			   Double_t d, Double_t dx, Double_t dy, Double_t stereoF, Double_t stereoB)
  :
  TNamed(tempName.Data(), ""),
  fDetectorId(0),
  fType(0),
  fX0(0.),
  fY0(0.),
  fZ0(0.),
  fRotation(0.),
  fLx(0.),
  fLy(0.),
  fDx(0.),
  fDy(0.),
  fStereoF(0.),
  fStereoB(0.),
  fD(0.),
  fNChannelsFront(0),
  fNChannelsBack(0),
  fBackStripShift(0.),
  fFrontStripShift(0.),
  fSigmaX(0.),
  fSigmaY(0.),
  fSigmaXY(0.),
  fXSmearWidth(0.0005),
  fZSmearSlope(0.04),
  fFrontLorentzShift(0),
  fBackLorentzShift(0),
  fFrontActive(),
  fBackActive(),
  fTrueHits()
{



  fName = tempName.Data();
  fDetectorId = detId;
  fType       = iType;
  fX0         = x0;
  fY0         = y0;
  fZ0         = z0;  // z position of the station
  fRotation   = rotation;
  fLx         = lx;
  fLy         = ly;
  fD          = d;   // thickness of the station
  fDx         = dx;
  fDy         = dy;
  fStereoF    = stereoF;//FIX GP; angle*TMath::Pi()/180 to RAD;
  fStereoB    = stereoB;
  fBackStripShift  = 0.;
  fFrontStripShift = 0.;
  Double_t dbNoX = fLx / fDx;
flg=0;
Double_t PIR=3.1415926536/180.; // Ruf (GP)
  // Calculate number of channels
  if ( fType == 1 ) {             // Pixel sensor
    Double_t dbNoY = fLy / fDy;
    fNChannelsFront = Int_t( TMath::Ceil ( dbNoX )
			    *TMath::Ceil ( dbNoY ));
    fNChannelsBack  = 0;


  }
  else if ( fType == 2 ) {        // strip sensor
  
  TopVolume=gGeoManager->GetTopVolume();
  cout<<"TopVolume is: "<<TopVolume->GetName()<<endl;
  
 if(GetStationNr()<4){
 
Int_t stnNum=GetStationNr();
/*
 Int_t module =-1;
    if(GetSectorNr()==1 || GetSectorNr()==2) module=0;
    if(GetSectorNr()==3 || GetSectorNr()==4) module=1;
	  
    Int_t  ip=-1;
    Int_t  Sect=0;

    if(stnNum>1 && stnNum <= 4) ip= stnNum-1;
    else if(stnNum<=7)  ip= (stnNum-1)*2 - 4 + module;
*/
	  fNChannelsFront=fNChannelsBack=640; //mStrLayerAct[ModType_r7[stnNum-1][module]][0][0];
    if(stnNum==1) {
      fNChannelsFront=640;
      fNChannelsBack=614;
    }
	  //fNChannelsBack= mStrLayerAct[ModType_r7[stnNum-1][module]][0][1];

 //   fDx=0.08;
 // fDy=fDx/TMath::Cos(15.*PIR);
  fLx=ModSiSize[stnNum-1][0];//fDx*fNChannelsFront;
  fLy=ModSiSize[stnNum-1][1];//fDx*(fNChannelsBack-fNChannelsFront*TMath::Cos(15.*PIR))/TMath::Sin(15.*PIR);

//ip= modip[MYSILICON_fStation-1][MYSILICON_fModule];
 
    fX0=arX0[modip[stnNum-1][GetSectorNr()-1]];
    fY0=arY0[modip[stnNum-1][GetSectorNr()-1]];
    fZ0=arZ0[modip[stnNum-1][GetSectorNr()-1]];
 cout<<"\n";
    cout<<"Statiton: "<<stnNum<<" Sector: "<<GetSectorNr()<<" Alignment X,Y:  "<<fX0<<","<<fY0<<","<<fZ0<<"\n";
    cout<< "Current Node: "<<gGeoManager->GetCurrentNode()->GetName() <<" Path: "<< gGeoManager->GetPath() << "\n";
    cout<<" XYZ: "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[0]<<", "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[1]<<", "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[2]<<endl;
    cout<<"\n";
//if(GetSectorNr()%2>0){
    if(TMath::Abs(fX0-gGeoManager->GetCurrentMatrix()->GetTranslation()[0])>0.00001) {
      cout<<"Sens X and Align not equals ! Align X: "<<fX0<<" Geo X: "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[0]<<endl;
      Fatal("", "Illegal sensor XYZ");
      }
      if(TMath::Abs(fY0-gGeoManager->GetCurrentMatrix()->GetTranslation()[1])>0.00001) {
      cout<<"Sens Y and Align not equals ! Align Y: "<<fY0<<" Geo Y: "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[1]<<endl;
      Fatal("", "Illegal sensor XYZ");
      }
       if(TMath::Abs(fZ0-gGeoManager->GetCurrentMatrix()->GetTranslation()[2])>0.00001) {
      cout<<"Sens Z and Align not equals ! Align Z: "<<fZ0<<" Geo Z: "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[2]<<endl;
      Fatal("", "Illegal sensor XYZ");
      }
  //}
    GeoMaskSi[ (GetStationNr())-1 ][GetSectorNr()-1]=gGeoManager->GetPath();//gGeoManager->GetCurrentNode()->GetVolume();

flgBound=true;

  }else{
 //ip_r7[(statNr-1)-3][moduleNr]+14
Int_t stnNum=GetStationNr()-3;

 Int_t module =-1;
    if(GetSectorNr()==1 || GetSectorNr()==2) module=0;
    if(GetSectorNr()==3 || GetSectorNr()==4) module=1;
	  
    Int_t  ip=-1;
    Int_t  Sect=0;

    if(stnNum>1 && stnNum <= 4) ip= stnNum-1;
    else if(stnNum<=7)  ip= (stnNum-1)*2 - 4 + module;

	  fNChannelsFront= mStrLayerAct[ModType_r7[stnNum-1][module]][0][0];
	  fNChannelsBack= mStrLayerAct[ModType_r7[stnNum-1][module]][0][1];

    fDx=0.08;
  fDy=fDx/TMath::Cos(15.*PIR);
  fLx=fDx*fNChannelsFront;
  fLy=fDx*(fNChannelsBack-fNChannelsFront*TMath::Cos(15.*PIR))/TMath::Sin(15.*PIR);




 
    fX0=arX0[ip_r7[stnNum-1][module]+14];
    fY0=arY0[ip_r7[stnNum-1][module]+14];
    fZ0=arZ0[ip_r7[stnNum-1][module]+14];
 cout<<"\n";
    cout<<"Statiton: "<<stnNum<<" Sector: "<<GetSectorNr()<<" Alignment X,Y:  "<<fX0<<","<<fY0<<","<<fZ0<<"\n";
    cout<< "Current Node: "<<gGeoManager->GetCurrentNode()->GetName() <<" Path: "<< gGeoManager->GetPath() << "\n";
    cout<<" XYZ: "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[0]<<", "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[1]<<", "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[2]<<endl;
    cout<<"\n";
if(GetSectorNr()%2>0){
    if(TMath::Abs(fX0-gGeoManager->GetCurrentMatrix()->GetTranslation()[0])>0.00001) {
      cout<<"Sens X and Align not equals ! Align X: "<<fX0<<" Geo X: "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[0]<<endl;
      Fatal("", "Illegal sensor XYZ");
      }
      if(TMath::Abs(fY0-gGeoManager->GetCurrentMatrix()->GetTranslation()[1])>0.00001) {
      cout<<"Sens Y and Align not equals ! Align Y: "<<fY0<<" Geo Y: "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[1]<<endl;
      Fatal("", "Illegal sensor XYZ");
      }
       if(TMath::Abs(fZ0-gGeoManager->GetCurrentMatrix()->GetTranslation()[2])>0.00001) {
      cout<<"Sens Z and Align not equals ! Align Z: "<<fZ0<<" Geo Z: "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[2]<<endl;
      Fatal("", "Illegal sensor XYZ");
      }
  }
   GeoMaskGem[ (GetStationNr()-3)-1 ][GetSectorNr()-1]=gGeoManager->GetPath();//gGeoManager->GetCurrentNode();//->GetVolume();

flgBound=true;

  }
  
  }
  
  else if ( fType == 3 ) {        // strip sensor with double metal layer
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    if (fStereoB*180/TMath::Pi()>80) {
      fNChannelsBack  = Int_t(TMath::Ceil ( fLy / fDy ));
    }
    else {
      fNChannelsBack = fNChannelsFront;
    }
  }
  else {
    cout << "-E- CbmStsSensor: Illegal sensor type " << fType << endl;
    Fatal("", "Illegal sensor type");
  }

  // Calculate errors and covariance
  if ( fType == 1 ) {
    fSigmaX  = fDx / TMath::Sqrt(12);
    fSigmaY  = fDy / TMath::Sqrt(12);
    fSigmaXY = 0.;
  }
  else if ( fType == 2 || fType == 3  || fType==8) {

    if (fStereoF==0. && fStereoB*180/TMath::Pi()<80 ) {
      fSigmaX  = fDx / TMath::Sqrt(12);
      fSigmaY  = fDx / (TMath::Sqrt(6)*TMath::Tan(fStereoB));
      fSigmaXY = (-1. * fDx * fDx)/(12.*TMath::Tan(fStereoB));
    }
    else if (fStereoF==0. && fStereoB*180/TMath::Pi()>80 ) {
      fSigmaX  = fDx / TMath::Sqrt(12);
      fSigmaY  = fDy / TMath::Sqrt(12);
      fSigmaXY = 0.;
    }
    else {
      fSigmaX  = fDx / TMath::Sqrt(24);
      fSigmaY  = fDx / (TMath::Sqrt(24)*TMath::Tan(fStereoB));
      fSigmaXY = 0.;
    }

    Int_t sensorNumber = ( fDetectorId & (7<<1) ) >> 1;

    if ( sensorNumber == 1 ) { //FIX GP;
      fBackStripShift  = 0.;
      fFrontStripShift = 0.;

    }
    else if ( sensorNumber == 2 ) {
      if (fStereoB*180/TMath::Pi()>80) {
        fBackStripShift  = 0.;
        fFrontStripShift = 1. * fLy * TMath::Tan(fStereoF);
      }
      else {
        fBackStripShift  = 1. * fLy * TMath::Tan(fStereoB);
        fFrontStripShift = 1. * fLy * TMath::Tan(fStereoF);
      }
    }
    else if ( sensorNumber == 3 ){
      if (fStereoB*180/TMath::Pi()>80) {
        fBackStripShift  = 0.;
        fFrontStripShift = 2. * fLy * TMath::Tan(fStereoF);
      }
      else {
        fBackStripShift  = 2. * fLy * TMath::Tan(fStereoB);
        fFrontStripShift = 2. * fLy * TMath::Tan(fStereoF);
      }
    }

  }
  else {
    cout << "-E- CbmStsSensor: Illegal sensor type " << fType << endl;
    Fatal("", "Illegal sensor type");
  }

  // Transform errors to global c.s.
  Double_t cosRot = TMath::Cos(fRotation);
  Double_t sinRot = TMath::Sin(fRotation);
  Double_t vX  = cosRot * cosRot * fSigmaX * fSigmaX
               - 2. * cosRot * sinRot * fSigmaXY
               + sinRot * sinRot * fSigmaY * fSigmaY;
  Double_t vY  = sinRot * sinRot * fSigmaX * fSigmaX
               + 2. * cosRot * sinRot * fSigmaXY
               + cosRot * cosRot * fSigmaY * fSigmaY;
  Double_t vXY = cosRot * sinRot * fSigmaX * fSigmaX
               + ( cosRot*cosRot - sinRot*sinRot ) * fSigmaXY
               - cosRot * sinRot * fSigmaY * fSigmaY;
  fSigmaX  = TMath::Sqrt(vX);
  fSigmaY  = TMath::Sqrt(vY);
  fSigmaXY = vXY;

  fXSmearWidth = 0.0005;
  fZSmearSlope = 0.04;

  fFrontLorentzShift = 0;
  fBackLorentzShift  = 0;

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsSensor::CbmStsSensor(Int_t detId, Int_t iType, Double_t x0,
			   Double_t y0, Double_t rotation, Double_t lx,
			   Double_t ly, Double_t dx, Double_t dy,
			   Double_t stereoF, Double_t stereoB)
  :
  fDetectorId(0),
  fType(0),
  fX0(0.),
  fY0(0.),
  fZ0(0.),
  fRotation(0.),
  fLx(0.),
  fLy(0.),
  fDx(0.),
  fDy(0.),
  fStereoF(0.),
  fStereoB(0.),
  fD(0.),
  fNChannelsFront(0),
  fNChannelsBack(0),
  fBackStripShift(0.),
  fFrontStripShift(0.),
  fSigmaX(0.),
  fSigmaY(0.),
  fSigmaXY(0.),
  fXSmearWidth(0.0005),
  fZSmearSlope(0.04),
  fFrontLorentzShift(0),
  fBackLorentzShift(0),
  fFrontActive(),
  fBackActive(),
  fTrueHits()
{
  fDetectorId = detId;
  fType       = iType;
  fX0         = x0;
  fY0         = y0;
  fRotation   = rotation;
  fLx         = lx;
  fLy         = ly;
  fDx         = dx;
  fDy         = dy;
  fStereoF    = stereoF;
  fStereoB    = stereoB;
  fBackStripShift  = 0.;
  fFrontStripShift = 0.;
  Double_t dbNoX = fLx / fDx;
flg=0;
  // Calculate number of channels
  if ( fType == 1 ) {             // Pixel sensor
    Double_t dbNoY = fLy / fDy;
    fNChannelsFront = Int_t( TMath::Ceil ( dbNoX )
			    *TMath::Ceil ( dbNoY ));
    fNChannelsBack  = 0;
  }
  else if ( fType == 2 ) {        // strip sensor
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    Double_t aPlus  = TMath::Abs( fLy * TMath::Tan(fStereoB) );
    Double_t dbNoB = ( fLx + aPlus ) / fDx ;
    fNChannelsBack  = Int_t(TMath::Ceil ( dbNoB ));



    if (fStereoB==0.&&fStereoF<0.)
    {
      //  fStereoB=fStereoF;
      //  fStereoF=0;
        flg=-1;
    }

          if(fLx==10. && fLy==10.)
          {
            fNChannelsFront = 250;
            fNChannelsBack = fNChannelsFront; //small
          }

          if(fLx==66. && fLy==41.)
           {
            fNChannelsFront = 825; //standard big
            fNChannelsBack = 930;
            if(flg==-1)
              {
            //   fNChannelsFront = 930; //standard big
            //   fNChannelsBack = 825;
              }

           }

           if(fLx==15. && fLy==10.)
            {
             fNChannelsFront =188 ; //standard hot
             fNChannelsBack = 214;
             if(flg==-1)
               {
              //  fNChannelsFront = 214; //standard big
              //  fNChannelsBack = 188;
               }

            }

            if(fLx==81.60 && fLy==45.)
             {
              fNChannelsFront =1020; //big big
              fNChannelsBack = 1131;

             }

             if(fLx>=35. && fLx<=43. && fLy==15.)
              {
               fNChannelsFront =500; //big hot
               fNChannelsBack = 508;

              }

  }
  else if ( fType == 3 ) {        // strip sensor with double metal layer
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    fNChannelsBack  = fNChannelsFront;
  }
  else {
    cout << "-E- CbmStsSensor: Illegal sensor type " << fType << endl;
    Fatal("", "Illegal sensor type");
  }

  // Calculate errors and covariance
  if ( fType == 1 ) {
    fSigmaX  = fDx / TMath::Sqrt(12);
    fSigmaY  = fDy / TMath::Sqrt(12);
    fSigmaXY = 0.;
  }
  else if ( fType == 2 || fType == 3 ) {
     if (fStereoF==0. && fStereoB*180/TMath::Pi()<80 ) {
      fSigmaX  = fDx / TMath::Sqrt(12);
      fSigmaY  = fDx / (TMath::Sqrt(6)*TMath::Tan(fStereoB));
      fSigmaXY = (-1. * fDx * fDx)/(12.*TMath::Tan(fStereoB));
    }
    else if (fStereoF==0. && fStereoB*180/TMath::Pi()>80 ) {
      fSigmaX  = fDx / TMath::Sqrt(12);
      fSigmaY  = fDy / TMath::Sqrt(12);
      fSigmaXY = 0.;
    }
    else {
      fSigmaX  = fDx / TMath::Sqrt(24);
      fSigmaY  = fDx / (TMath::Sqrt(24)*TMath::Tan(fStereoB));
      fSigmaXY = 0.;
    }

    Int_t sensorNumber = ( fDetectorId & (7<<1) ) >> 1;
    if ( sensorNumber == 1 ) {
      fBackStripShift  = 0.;
      fFrontStripShift = 0.;
      if (fStereoB < 0) fBackStripShift  = fLy * TMath::Tan(-fStereoB); //AZ
    }
    else if ( sensorNumber == 2 ) {
      if (fStereoB*180/TMath::Pi()>80) {
        fBackStripShift  = 0.;
        fFrontStripShift = 1. * fLy * TMath::Tan(fStereoF);
      }
      else {
        fBackStripShift  = 1. * fLy * TMath::Tan(fStereoB);
        fFrontStripShift = 1. * fLy * TMath::Tan(fStereoF);
      }
    }
    else if ( sensorNumber == 3 ){
      if (fStereoB*180/TMath::Pi()>80) {
        fBackStripShift  = 0.;
        fFrontStripShift = 2. * fLy * TMath::Tan(fStereoF);
      }
      else {
        fBackStripShift  = 2. * fLy * TMath::Tan(fStereoB);
        fFrontStripShift = 2. * fLy * TMath::Tan(fStereoF);
      }
    }

  }
  else {
    cout << "-E- CbmStsSensor: Illegal sensor type " << fType << endl;
    Fatal("", "Illegal sensor type");
  }

  // Transform errors to global c.s.
  Double_t cosRot = TMath::Cos(fRotation);
  Double_t sinRot = TMath::Sin(fRotation);
  Double_t vX  = cosRot * cosRot * fSigmaX * fSigmaX
               - 2. * cosRot * sinRot * fSigmaXY
               + sinRot * sinRot * fSigmaY * fSigmaY;
  Double_t vY  = sinRot * sinRot * fSigmaX * fSigmaX
               + 2. * cosRot * sinRot * fSigmaXY
               + cosRot * cosRot * fSigmaY * fSigmaY;
  Double_t vXY = cosRot * sinRot * fSigmaX * fSigmaX
               + ( cosRot*cosRot - sinRot*sinRot ) * fSigmaXY
               - cosRot * sinRot * fSigmaY * fSigmaY;
  fSigmaX  = TMath::Sqrt(vX);
  fSigmaY  = TMath::Sqrt(vY);
  fSigmaXY = vXY;

  fXSmearWidth = 0.0005;
  fZSmearSlope = 0.04;

  fFrontLorentzShift = 0;
  fBackLorentzShift  = 0;

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsSensor::~CbmStsSensor() {

 };
// -------------------------------------------------------------------------



// -----   Public method GetChannel   --------------------------------------
Int_t CbmStsSensor::GetChannel(Double_t x, Double_t y, Int_t iSide) {

  // Check iSide
  if (iSide !=0 && iSide != 1) {
    cout << "-W- CbmStsSensor::GetChannel: Illegal side number "
	 << iSide << endl;
    return -1;
  }

  // Calculate internal coordinates. Return -1 if outside sensor.
  Double_t xint  = 0;
  Double_t yint  = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Int_t    iChan = 0;

  Double_t dbNoX = fLx / fDx;
  // Case pixel sensor: iChannel = iRow * nColumns + iColumn
  if ( fType == 1 ) {
    Int_t nCol = Int_t(TMath::Ceil( dbNoX));
    Int_t iCol = Int_t(xint / fDx);
    Int_t iRow = Int_t(yint / fDy);
    iChan = iRow * nCol + iCol;
    if ( iChan < 0 || iChan > fNChannelsFront ) {
      cout << "-E- CbmStsSensor::GetChannel: "
	   << "Channel number " << iChan << " exceeds limit "
	   << fNChannelsFront << endl;
      cout << GetStationNr() << " " << GetSensorNr() << endl;
      cout << x << " " << y << " " << iSide << endl;
      Fatal("GetChannel", "illegal channel number");
    }
  }

  // Case strip sensor 1
  else if ( fType == 2 ) {
    if ( iSide == 0 )
     {        // Front side
      iChan = Int_t ( xint / fDx );
      if ( iChan < 0 || iChan > fNChannelsFront )
       {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit "
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
       }
    }
    else {                     // Back side
      // Project point along backside strip to y=0
      Double_t xp = xint + yint * TMath::Tan(fStereoB);
      // Digitise
      iChan = Int_t ( xp / fDx );
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit "
	     << fNChannelsBack << endl;
	cout << "    Sensor " << GetSensorNr() << " of station "
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y
	     << ") cm" << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  // Case strip sensor 2 (with double metal layer)
  else if ( fType == 3 ) {
    if ( iSide == 0 ) {      // Front side
     Double_t xf = xint + fFrontStripShift + yint * TMath::Tan(fStereoF);
     xf = xf - TMath::Floor(xf/fLx) * fLx;
     iChan = Int_t ( xf / fDx );
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit "
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                   // Back side
      if (fStereoB*180/TMath::Pi()>80) {
        Double_t xp = yint;
	xp = xp - TMath::Floor(xp/fLy) * fLy;
	iChan = Int_t( xp / fDy );
      }
      else {
	// Project point along backside strip to y = 0
        Double_t xp = xint + fBackStripShift + yint * TMath::Tan(fStereoB);
        // Calculate modulo w.r.t. sensor x width
        xp = xp - TMath::Floor(xp/fLx) * fLx;
        // Digitise
        iChan = Int_t( xp / fDx );
      }

      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit "
	     << fNChannelsBack << endl;
	cout << "    Sensor " << GetSensorNr() << " of station "
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y
	     << ") cm" << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  return iChan;
}
// -------------------------------------------------------------------------


// -----   Public method GetChannel   --------------------------------------
Float_t CbmStsSensor::GetChannelPlus(Double_t x, Double_t y, Int_t iSide) {

  // Check iSide
  if (iSide !=0 && iSide != 1) {
    cout << "-W- CbmStsSensor::GetChannel: Illegal side number "
	 << iSide << endl;
    return -1;
  }

  // Calculate internal coordinates. Return -1 if outside sensor.
  Double_t xint  = 0;
  Double_t yint  = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Float_t    iChan = 0;

  Double_t dbNoX = fLx / fDx;
  // Case pixel sensor: iChannel = iRow * nColumns + iColumn
  if ( fType == 1 ) {
    Int_t nCol = Int_t(TMath::Ceil( dbNoX));
    Int_t iCol = Int_t(xint / fDx);
    Int_t iRow = Int_t(yint / fDy);
    iChan = iRow * nCol + iCol;
    if ( iChan < 0 || iChan > fNChannelsFront ) {
      cout << "-E- CbmStsSensor::GetChannel: "
	   << "Channel number " << iChan << " exceeds limit "
	   << fNChannelsFront << endl;
      cout << GetStationNr() << " " << GetSensorNr() << endl;
      cout << x << " " << y << " " << iSide << endl;
      Fatal("GetChannel", "illegal channel number");
    }
  }

  // Case strip sensor 1
  else if ( fType == 2 ) {
    if ( iSide == 0 ) {        // Front side
      iChan = Int_t ( xint / fDx );
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit "
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                     // Back side
      // Project point along backside strip to y=0
      //AZ Double_t xp = xint + yint * TMath::Tan(fStereoB);
      Double_t xp = xint + yint * TMath::Tan(fStereoB) + fBackStripShift; //AZ
      // Digitise
      iChan = Int_t ( xp / fDx );
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit "
	     << fNChannelsBack << endl;
	cout << "    Sensor " << GetSensorNr() << " of station "
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y
	     << ") cm" << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  // Case strip sensor 2 (with double metal layer)
  else if ( fType == 3 ) {

    if ( iSide == 0 ) {      // Front side
      Double_t xf = xint + fFrontStripShift + yint * TMath::Tan(fStereoF);
      xf = xf - TMath::Floor(xf/fLx) * fLx;
      iChan = xf / fDx;
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit "
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                   // Back side

      if (fStereoB*180/TMath::Pi()>80) {
        Double_t xp = yint;
        xp = xp - TMath::Floor(xp/fLy) * fLy;
        iChan = xp / fDy;
      }
      else {
	Double_t xp = xint + fBackStripShift + yint * TMath::Tan(fStereoB);
        xp = xp - TMath::Floor(xp/fLx) * fLx;
        iChan = xp / fDx;
      }

      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit "
	     << fNChannelsBack << endl;
	cout << "    Sensor " << GetSensorNr() << " of station "
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y
	     << ") cm" << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  return iChan;
}
// -------------------------------------------------------------------------


// -----   Public method GetFrontChannel   ---------------------------------
Int_t CbmStsSensor::GetFrontChannel(Double_t x, Double_t y, Double_t z) {
  //cout << "frontchannel for " << x << " " << y << " " << z << " (" << fZ0-fD/2. << " " << fZ0+fD/2.<<")" << endl;
  z = z - fZ0 + fD/2.;
  //cout << " temp z = " << z << endl;
  if ( z > fD ) return -1;
  if ( z < 0. ) return -1;

  // Lorentz shift due to movement of the charges in the magnetic field
  // translates into an angle: 8.5 deg for electrons
  x += fFrontLorentzShift*z;

  // Calculate internal coordinates. Return -1 if outside sensor.
  Double_t xint  = 0;
  Double_t yint  = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Int_t    iChan = 0;

//  xint += gRandom->Gaus(0.,fXSmearWidth+fZSmearSlope*z);
//  yint += gRandom->Gaus(0.,fXSmearWidth+fZSmearSlope*z);

  Double_t xf = xint + fFrontStripShift + yint * TMath::Tan(fStereoF);
  //AZ xf = xf - TMath::Floor(xf/fLx) * fLx;
  if (fType != 2) xf = xf - TMath::Floor(xf/fLx) * fLx;

  iChan = (Int_t)(xf/fDx);

  if ( iChan < 0 || iChan > fNChannelsFront ) return -1;
  return iChan;
}
// -------------------------------------------------------------------------


// -----   Public method GetBackChannel   ----------------------------------
Int_t CbmStsSensor::GetBackChannel (Double_t x, Double_t y, Double_t z) {

  z = fZ0 + fD/2. - z;
  if ( z > fD ) return -1;
  if ( z < 0. ) return -1;

  // Lorentz shift due to movement of the charges in the magnetic field
  // translates into an angle: 1.5 deg for electrons
  x += fBackLorentzShift*z;

  // Calculate internal coordinates. Return -1 if outside sensor.
  Double_t xint  = 0;
  Double_t yint  = 0;
  Double_t xp = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Int_t    iChan = 0;

//  xint += gRandom->Gaus(0.,fXSmearWidth+fZSmearSlope*z);
//  yint += gRandom->Gaus(0.,fXSmearWidth+fZSmearSlope*z);
  if (fStereoB*180/TMath::Pi()>80) {
    xp = yint;
    xp = xp - TMath::Floor(xp/fLy) * fLy;
    iChan = (Int_t)(xp/fDy);
  }
  else {
    xp = xint + fBackStripShift + yint * TMath::Tan(fStereoB);
    //AZ xp = xp - TMath::Floor(xp/fLx) * fLx;
    if (fType != 2) xp = xp - TMath::Floor(xp/fLx) * fLx;
    iChan = (Int_t)(xp/fDx);
  }


  //if (fZ0 < 25) cout << "  " << xint << " " << yint << " -> " << xp << " -> " << iChan << " " << fNChannelsBack << endl;
  if ( iChan < 0 || iChan > fNChannelsBack ) return -1;
  return iChan;
}
// -------------------------------------------------------------------------


// -----   Public method Inside   ------------------------------------------
Bool_t CbmStsSensor::Inside(Double_t x, Double_t y) {
  Double_t xint, yint;
  return IntCoord(x, y, xint, yint);
}
// -------------------------------------------------------------------------


// -----   Public method ActivateChannels   --------------------------------
Bool_t CbmStsSensor::ActivateChannels(Int_t ipt,
					   Double_t x, Double_t y) {

  Int_t iFront = FrontStripNumber(x, y);
  if (iFront < 0) return kFALSE;
  Int_t iBack  = BackStripNumber(x, y);
  if (iBack < 0) {
    cout << "-W- CbmStsSensor::ActivateChannels: "
	 << "No back strip number!" << endl;
    cout << "    Coordinates: (" << x << ", " << y << ")" << endl;
    cout << "    Sensor: " << fDetectorId << ", Front Strip: "
	 << iFront << endl;
    return kFALSE;
  }

  fFrontActive.insert(iFront);
  fBackActive.insert(iBack);
  pair<Int_t,Int_t> a(iFront,iBack);
  fTrueHits[a] = ipt;

  return kTRUE;
}
// -------------------------------------------------------------------------




// -----   Public method Intersect   ---------------------------------------
Int_t CbmStsSensor::Intersect(Int_t iFStrip, Int_t iBStrip,
				    vector<Double_t>& xCross,
				    vector<Double_t>& yCross) {

  // Reset STL vector of intersection y coordinates
  xCross.clear();
  yCross.clear();

  // Check for strip numbers
  if ( iFStrip < 0 || iFStrip > fNChannelsFront) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid front channel number ! "
	 << iFStrip << " " << fNChannelsFront << endl;
    return 0;
  }
  if ( iBStrip < 0 || iBStrip > fNChannelsBack) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid back channel number ! "
	 << iBStrip << " " << fNChannelsBack << endl;
    return 0;
  }

  // x coordinate of centre of front side strip readout pad
  Double_t xF = Double_t( iFStrip + 0.5 ) * fDx;

  // x coordinate of centre of back side strip readout pad
  Double_t xB = Double_t( iBStrip + 0.5 ) * fDx;

  Double_t x;
  Double_t y;
  // Maximal number of back & front strip segments
  Int_t nSegB = Int_t ( fLy * TMath::Tan(fStereoB) / fLx ) + 2;
  Int_t nSegF = Int_t ( fLy * TMath::Tan(fStereoF) / fLx ) + 2;

  for (Int_t iSegB=0; iSegB<nSegB; iSegB++) {

    for (Int_t iSegF=0; iSegF<nSegF; iSegF++) {

      x = ((-1./TMath::Tan(fStereoB)) * (xB -  Double_t(iSegB)*fLx) + ((1./TMath::Tan(fStereoF)) * (xF - Double_t(iSegF)*fLx)))/((1./TMath::Tan(fStereoF))-(1./TMath::Tan(fStereoB)));
      y = (-1./TMath::Tan(fStereoB)) * x + (1./TMath::Tan(fStereoB)) * (xB - fBackStripShift + Double_t(iSegB)*fLx);
      // y & x coordinate of intersection of back strip segment with front strip
      //Double_t y = (xB - x - Double_t(iSegB) * fLx) / TMath::Tan(fStereoB);

      if ( y < 0. || y > fLy ) continue;
      if ( x < 0. || x > fLx ) continue;

      // Transform x and y coordinates to the global c.s.
      Double_t xHit = x * TMath::Cos(fRotation) - y * TMath::Sin(fRotation);
      Double_t yHit = y * TMath::Cos(fRotation) + x * TMath::Sin(fRotation);

      // Fill coordinates in return arrays
      xCross.push_back(xHit);
      yCross.push_back(yHit);
    }
  }

  return xCross.size();

}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t CbmStsSensor::Intersect(Int_t iFStrip, Int_t iBStrip,
			      Double_t& xCross, Double_t& yCross, Double_t& zCross) {

  // Check for strip numbers
  if ( iFStrip < 0 || iFStrip > fNChannelsFront) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid front channel number ! "
	 << iFStrip << " " << fNChannelsFront << endl;
    return -1;
  }
  if ( iBStrip < 0 || iBStrip > fNChannelsBack) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid back channel number ! "
	 << iBStrip << " " << fNChannelsBack << endl;
    return -1;
  }
  Double_t xPoint = xCross, yPoint = yCross; //AZ
  xCross = 0.;
  yCross = 0.;
  zCross = 0.;

  // x coordinate of centre of front side strip readout pad
  Double_t xint = ( Double_t(iFStrip) + 0.5 ) * fDx;

  Double_t sinrot = TMath::Sin(fRotation);
  Double_t cosrot = TMath::Cos(fRotation);
  Double_t tanstrB = 0.0, sterB = fStereoB * TMath::RadToDeg();
  if (!(TMath::Abs(sterB-90.) < 1 || TMath::Abs(sterB+90.) < 1)) tanstrB = TMath::Tan(fStereoB);
  Double_t tanstrF = TMath::Tan(fStereoF);
  Int_t nStripMaxB = ( fStereoB<0. ? 0 :  Int_t(fLy*(tanstrB)/fLx)+1 ); // max. number of strips
  Int_t nStripBegB = ( fStereoB>0. ? 0 : -Int_t(fLy*(tanstrB)/fLx)-1 );

  Int_t nStripMaxF = ( fStereoF<=0. ? 0 :  Int_t(fLy*(tanstrF)/fLx)+1 ); // max. number of strips
  Int_t nStripBegF = ( fStereoF>0. ? 0 : -Int_t(fLy*(tanstrF)/fLx)-1 );

  Double_t x0 = ( Double_t(iBStrip) + 0.5 ) * fDx;

  Double_t yint;
  Double_t x;

  Double_t xtemp, ytemp;

  //AZ Y and X strips (0 and 90 degs)
  if (tanstrF==0. && tanstrB == 0.0) {
    yint = ( Double_t(iBStrip) + 0.5 ) * fDy;

    // Translation to centre of sector
    xtemp = xint - fLx/2.;
    ytemp = yint - fLy/2.;

    // Rotation around sector centre
    xCross = xtemp * cosrot - ytemp * sinrot;
    yCross = xtemp * sinrot + ytemp * cosrot;

    // Translation into global c.s.
    xCross = xCross + fX0;
    yCross = yCross + fY0;
    zCross = fZ0;

  } else if (tanstrF==0.) {

    for (Int_t iStrip=nStripBegB; iStrip<=nStripMaxB; iStrip++) {

      yint = ( x0 - xint - fBackStripShift + Double_t(iStrip) * fLx ) / tanstrB;

      if ( ! ( yint>0. && yint<fLy ) ) continue;

      if ( zCross > 0.001 ) {
        Fatal("Intersect","Intersection of two strips in two different points not valid");
        return -1;
      }

      // Translation to centre of sector
      xtemp = xint - fLx/2.;
      ytemp = yint - fLy/2.;

      // Rotation around sector centre
      xCross = xtemp * cosrot - ytemp * sinrot;
      yCross = xtemp * sinrot + ytemp * cosrot;

      // Translation into global c.s.
      xCross = xCross + fX0;
      yCross = yCross + fY0;
      zCross = fZ0;
    }
  }
  else {

    for (Int_t iStripB=0; iStripB<=1; iStripB++) {

      for (Int_t iStripF=-1; iStripF<=0; iStripF++) {

        x = ((-1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx) + (1./tanstrF) * (xint - fFrontStripShift +  Double_t(iStripF)*fLx))/((1./tanstrF)-(1./tanstrB));
        yint = (-1./tanstrB) * x + (1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx);

        if ( ! ( yint>0. && yint<fLy ) ) continue;
        if ( ! ( x>0. && x<fLx ) ) continue;

        if ( zCross > 0.001 ) {
          //AZ Fatal("Intersect","1 Intersection of two strips in two different points not valid");
          //AZ return -1;
          Warning("Intersect","1 Intersection of two strips in two different points not valid");
        }

        // Translation to centre of sector
        xtemp = x - fLx/2.;
        ytemp = yint - fLy/2.;

        // Rotation around sector centre
        //AZ xCross = xtemp * cosrot - ytemp * sinrot;
        //AZ yCross = xtemp * sinrot + ytemp * cosrot;
        Double_t xCross1 = xtemp * cosrot - ytemp * sinrot;
        Double_t yCross1 = xtemp * sinrot + ytemp * cosrot;

        // Translation into global c.s.
        //AZ xCross = xCross + fX0;
        //AZ yCross = yCross + fY0;
        xCross1 += fX0;
        yCross1 += fY0;
        zCross = fZ0;

	//AZ - pixel simulation
	if (TMath::Abs(TMath::Abs(fStereoF-fStereoB)*TMath::RadToDeg()-90) < 0.1) {
	  if (TMath::Abs(xCross1-xPoint) > 0.5 || TMath::Abs(yCross1-yPoint) > 0.5) continue;
	} //
	xCross = xCross1; //AZ
	yCross = yCross1; //AZ
      }
    }
  }
  // No intersection found
  if ( zCross < 0.001 ) return -1;

  return fDetectorId;
}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t CbmStsSensor::IntersectClusters(Double_t fChan, Double_t bChan,
				      Double_t& xCross, Double_t& yCross, Double_t& zCross) {

if(GetStationNr()>3){//fType==2){
 /*TString ssnum("station");
 ssnum+=(Int_t)GetStationNr();
TGeoVolume* vSta=gGeoManager->FindVolumeFast(ssnum);//->GetName();
//cout <<"DBG:     "<< "ssnum: "<<ssnum<<" find ssf: "<<findStation<<endl;
gGeoManager->SetTopVolume(vSta);
*/
//cout<<" GO"<<endl;
  // Check for strip numbers
/*
Int_t module =0;
if(GetSectorNr==1 || GetSectorNr==2) module=0;
if(GetSectorNr==3 || GetSectorNr==4) module=1;

Int_t ip=(GetStationNr()-1)*2 - 4 + module;
*/

//if(GetSectorNr()%2!=0)gGeoManager->SetTopVolume(GeoMaskGem[ (GetStationNr()-3)-1 ][GetSectorNr()-1]);
//else gGeoManager->SetTopVolume(TopVolume);

//fChan=2;
//bChan=2;

 //cout<<"f,b "<<fChan<<","<<bChan<<endl;
 //fChan=0;
 //bChan=0;
 Int_t module =-1;
  //fChan=fChan+0.5;
  if(flgBound){
   // fChan=0.5;
   // bChan=0.5;//0.5;

   // fChan=0.5;
   // bChan=0.5;//0.5;
    
    if(GetSectorNr()==1 || GetSectorNr()==2) module=0;
    if(GetSectorNr()==3 || GetSectorNr()==4) module=1;

    //if( (GetStationNr()==5 ) && module==0) module==1;
    //if( (GetStationNr()==5 ) && module==1) module==0;

    Int_t  ip=-1;
    Int_t  Sect=0;

    if(GetStationNr()>0 && GetStationNr() <= 4) ip= GetStationNr()-1;
    else  ip= (GetStationNr()-1)*2 - 4 + module;


    if(GetSectorNr()==2 || GetSectorNr()==4){
      //  hotf=true;

  fChan+=ColdStripBias[ModType_r7[ (GetStationNr()-1) -3][module] ][0];
  bChan+=ColdStripBias[ModType_r7[ (GetStationNr()-1) -3][module] ][1];

  
    } 
  if ( fChan < 0 || fChan-0.5 > fNChannelsFront ) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid front channel number ! "
	 << fChan << " " << fNChannelsFront << " ST: "<<GetStationNr()<< endl;
    return -1;
  }
    //bChan=bChan+0.5;
  if ( bChan < 0 || bChan-0.5 > fNChannelsBack ) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid back channel number ! "
	 << bChan << " " << fNChannelsBack << " ST: "<<GetStationNr() <<endl;
    return -1;
  }
/*cout<<"\n";
cout<<"where ?: "<<(TString)gGeoManager->GetPath()<<"\n"<< "St, sec: "<< GetStationNr()<<", "<<GetSectorNr()<<endl;
*/
//cout<<" true node !: "<< GeoMask[ (GetStationNr()-3)-1 ][GetSectorNr()-1]->GetName()<<endl;
}
//  Double_t YBe[8]= { 127., 470., 470.2, 432.35, 431.75, 434., -4., 1017.};
//  Double_t UBe[8]= { 126., 521.42, 521.43, 483.28, 482.72, 485.04, 61.12, 1047.64};
//fChan=0.5;
//bChan=0.5;
//cout<<"fX0: "<<fX0<<" fY0: "<<fY0<<endl;
  xCross = 0.;
  yCross = 0.;
  zCross = 0.;

  Double_t sinrot = TMath::Sin(fRotation);
  Double_t cosrot = TMath::Cos(fRotation);

Int_t invert=0;
if(GetStationNr()-1==3 || GetStationNr()-1==5) invert=-1;
else invert=1;

//cout<<" ip: "<<ip_r7[(GetStationNr()-1)-3][module];


 Double_t xint  = ( fChan + 0. ) * fDx;// - invert*fFrontLorentzShift; //+0.5
 Double_t x0  = ( bChan + 0.) * fDy;// - invert*fBackLorentzShift; //+0.5

//cout<<"LORENTZ SHIFTS: f:"<<invert*fFrontLorentzShift<<"  b: "<< invert*fBackLorentzShift<<endl;

  Double_t yint;
  Double_t x;

  Double_t xtemp, ytemp;

  if ( (fStereoF==0.&&fStereoB*180/TMath::Pi()<80 ) || flg==-1 ) {
    Double_t tanstrB = TMath::Tan(fStereoB);
    Double_t tanstrF = TMath::Tan(fStereoF);
    Int_t nStripMaxB = fNChannelsBack;//( fStereoB<0. ? 0 :  Int_t(fLy*tanstrB/fLx)+1 ); // max. number of BSts
    Int_t nStripBegB = 0;//( fStereoB>0. ? 0 : -Int_t(fLy*tanstrB/fLx)-1 );

    Int_t nStripMaxF = fNChannelsFront;//( fStereoF<=0. ? 0 :  Int_t(fLy*tanstrF/fLx)+1 ); // max. number of Fstrips
    Int_t nStripBegF =0 ;//( fStereoF>0. ? 0 : -Int_t(fLy*tanstrF/fLx)-1 );
    if (fType == 2) nStripBegB = nStripMaxB = 0; //AZ
    for (Int_t iStrip=nStripBegB; iStrip<=nStripMaxB; iStrip++) {

     yint = ( x0 - xint - fBackStripShift + Double_t(iStrip) * fLx ) / tanstrB;
  //   cout<<fBackStripShift<<endl;
  //   if(flg==-1) yint = ( x0 - xint - fBackStripShift + Double_t(iStrip) * fLx ) / TMath::Abs(tanstrF);
//cout<<"GEM x1: "<<xint<<" y1:"<<yint<< " fchan: "<<fChan<<" bChan: "<<bChan<< endl;
//Float_t fly=12.60;
//Float_t flx=6.30;




if( tanstrB > 0 ) { // Ruf
if(flgBound) if ( ! ( yint>0. && yint< fLy ) ) continue;
ytemp = yint - fLy/2.;
  } else  {
if(flgBound) if ( ! ( yint>-fLy && yint<0. ) ) continue;
  ytemp = yint + fLy/2.;
  }


      if ( zCross > 0.001 ) {
        Fatal("Intersect","Intersection of two strips in two different points not valid");
        return -1;
      }
//cout<<"xint "<<xint<<" yint "<<yint<<endl;
//  if(GetStationNr()==7) fLx=2*fLx;
      // Translation to centre of sector
   xtemp = xint - fLx/2.;

   //cout<<" xLoc: "<<xtemp<<" yLoc: "<<ytemp;

//if(gGeoManager->FindNode(xint,yint,0)==NULL) return -1;
//if (!((TString)(gGeoManager->FindNode(xint,yint,0)->GetName())).Contains("Sens",TString::kIgnoreCase)   ) return -1;


    //  xtemp = xint - fLx/2.;

    //  ytemp = yint - fLy/2.;
    //  if(GetStationNr()==3 || GetStationNr()==5 || GetStationNr()==7)       ytemp = yint - fLy/2.;
    //  else ytemp = yint + fLy/2.;
//cout<<"xint2 "<<xtemp<<" yint2 "<<ytemp<<endl;
      // Rotation around sector centre
      xCross = xtemp * cosrot - ytemp * sinrot;
     yCross = xtemp * sinrot + ytemp * cosrot;

//Local shift betwee center of GEM planes in one station 
if( (ip_r7[(GetStationNr()-1)-3][module]+14)%2!=0){ yCross+=0.15; // для ip=15,17,19,21,23,25 
//cout<<"Right "<<fX0 <<"(GetStationNr()-1)-3][module]+14 : "<<ip_r7[(GetStationNr()-1)-3][module]+14<<endl;
}
else if( (ip_r7[(GetStationNr()-1)-3][module]+14)%2==0){ yCross-=0.15; // для ip=14,16,18,20,22,24
//cout<<"Left " <<fX0 <<"(GetStationNr()-1)-3][module]+14 : "<<ip_r7[(GetStationNr()-1)-3][module]+14<<endl;
} 

//cout<<"xint3 "<<xCross<<" yint3 "<<yCross<<endl;
      // Translation into global c.s.
  /*   if( GetStationNr()!=7 )
        {
          xCross =xCross- fX0;
          yCross =yCross - fY0;
        }


              if(GetStationNr()==3 || GetStationNr()==5  ) yCross=-yCross;

    //  if(GetStationNr()==3 || GetStationNr()==5)
    //  {
    //    xCross=xCross-6;
  //    }

     if(GetStationNr()==7  )
       {
         xCross =xCross+ fX0-3;
         yCross =yCross + fY0;
      if((GetSectorNr()==3 || GetSectorNr()==4))  xCross=-xCross;

        yCross=-yCross;


      }+
*/
//cout<<" xLab: "<<xCross<<" yLab: "<<yCross<<" zLab: "<<fZ0;


//cout<<" x,y after: "<<xCross<<","<<yCross<<endl;
SetRotZ(AlphaZmRad[ip_r7[(GetStationNr()-1) -3][module]+14], xCross, yCross);
// if(gGeoManager->FindNode(xCross,yCross,0)==NULL) return -1;
//cout<<"find: "<<gGeoManager->FindNode(xCross, yCross, 0)->GetName()<<endl;
//cout<<" x,y before: "<<xCross<<","<<yCross<<endl;

xCross = xCross + fX0;
yCross = yCross + fY0;

//cout<<" xLabr: "<<xCross<<" yLabr: "<<yCross<<endl;

//xCross = xCross + fX0;
//yCross = yCross + fY0;

//cout<<"fX: "<<fX0<<" fY: "<<fY0<<endl;

//cout<<"x,y: "<<xCross<<","<<yCross<<endl;
/*
if(GetStationNr()==7  )
  {
xCross = xCross + fX0;
yCross = yCross + fY0;
} else {
  xCross = xCross - fX0;
  yCross = yCross - fY0;
}

if(GetStationNr()==3 ||  GetStationNr()==5 )   yCross=-yCross;

if(GetStationNr()==7 )
  {
   if(GetSectorNr()==3 || GetSectorNr()==4)  xCross=xCross-80;
   else xCross=xCross+80;
    yCross=-yCross;
  } else xCross=xCross-3.1;
*/


  //  cout<<"fX0 "<<fX0<<" fY0 "<<fY0<<endl;

      zCross = fZ0;
      
     //   cout<< " GEM " <<modip[GetStationNr()-1][GetSectorNr()-1]<<" "<<GetStationNr()<<","<<GetSectorNr()<<" x_cr: "<<xCross<<" y_cr: "<<yCross<<" z: "<<zCross<<endl;
      
      
    }
  }
  else if (fStereoB*180/TMath::Pi()>80 ) {
    yint = ( bChan + 0.) * fDy; //+0.5
   yint = fLy- yint; // Ruf

    if ( zCross > 0.001 ) {
      Fatal("Intersect","Intersection of two strips in two different points not valid");
      return -1;
    }

    // Translation to centre of sector
    xtemp = xint - fLx/2.;
    ytemp = yint - fLy/2.;

    // Rotation around sector centre
    xCross = xtemp * cosrot - ytemp * sinrot;
    yCross = xtemp * sinrot + ytemp * cosrot;

    // Translation into global c.s.
    xCross = xCross +fX0;//+3;
    yCross = yCross + fY0;
    zCross = fZ0;

  }
  else {
    Double_t tanstrB = TMath::Tan(fStereoB);
    Double_t tanstrF = TMath::Tan(fStereoF);
    Int_t nStripMaxB = ( fStereoB<0. ? 0 :  Int_t(fLy*tanstrB/fLx)+1 ); // max. number of Bstrips
    Int_t nStripBegB = ( fStereoB>0. ? 0 : -Int_t(fLy*tanstrB/fLx)-1 );

    Int_t nStripMaxF = ( fStereoF<=0. ? 0 :  Int_t(fLy*tanstrF/fLx)+1 ); // max. number of Fstrips
    Int_t nStripBegF = ( fStereoF>0. ? 0 : -Int_t(fLy*tanstrF/fLx)-1 );

    //AZ for (Int_t iStripB=0; iStripB<=1; iStripB++) {

    //AZ for (Int_t iStripF=-1; iStripF<=0; iStripF++) {

    //AZ
    if (fType == 2) {
      nStripBegB = nStripMaxB = 0;
      nStripBegF = nStripMaxF = 0;
    } else {
      nStripBegB = 0;
      nStripMaxB = 1;
      nStripBegF = -1;
      nStripMaxF = 0;
    }
    for (Int_t iStripB = nStripBegB; iStripB <= nStripMaxB; ++iStripB) {

      for (Int_t iStripF = nStripBegF; iStripF <= nStripMaxF; ++iStripF) {
	//AZ

        //x = ((-1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx) + (1./tanstrF) * (xint - fFrontStripShift +  Double_t(iStripF)*fLx))/((1./tanstrF)-(1./tanstrB));
        x = ((1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx) + (1./tanstrF) * (xint - fFrontStripShift +  Double_t(iStripF)*fLx))/((1./tanstrF)+(1./tanstrB)); //AZ
        //yint = (-1./tanstrB) * x + (1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx);
        yint = (1./tanstrB) * x + (1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx); //AZ
//cout<<"x2: "<<x<<" y2:"<<yint<<endl;
        if ( ! ( yint>0. && yint<fLy ) ) continue;
        if ( ! ( x>0. && x<fLx ) ) continue;

        if ( zCross > 0.001 ) {
          Fatal("Intersect","2 Intersection of two strips in two different points not valid");
          return -1;
        }

        // Translation to centre of sector
        xtemp = x - fLx/2.;
        ytemp = yint - fLy/2.;

        // Rotation around sector centre
        xCross = xtemp * cosrot - ytemp * sinrot;
        yCross = xtemp * sinrot + ytemp * cosrot;

        // Translation into global c.s.
        xCross = xCross + fX0;
        yCross = yCross + fY0;
        zCross = fZ0;
      }
    }
  }

  // No intersection found
  //cout<<"GO2"<<endl;
  if ( zCross < 0.001 ) return -1;
  //cout<<"GO3"<<endl;
//cout<<"zcr"<<zCross<<endl;

 // TString currpath=(TString)gGeoManager->GetPath();

  //AZ - Reject points outside of the sensor boundaries (for complex geometries) //FIX GP;
  if (!(TMath::Abs(fX0) < 0.001 && TMath::Abs(fY0) < 0.001)) {
    //  cout << "X, Y, Z: " << xCross << " " << yCross << " " << zCross << " " 
    //	 << GetStationNr() << " " 
    //	 << gGeoManager->GetCurrentNode()->GetName() << " " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2]
    //	 << " " << gGeoManager->FindNode(xCross, yCross, zCross) << endl;

    if(gGeoManager->FindNode(xCross,yCross,zCross)==NULL) return -1;
    
    
  //if(  gGeoManager->FindNode(xCross, yCross,zCross)!=GeoMaskGem[ (GetStationNr()-3)-1 ][GetSectorNr()-1] ) return -1;
    



  TString where = gGeoManager->FindNode(xCross, yCross,zCross)->GetName();
 // TString findpath=(TString)gGeoManager->GetPath();
  if (!fName.Contains(where)) return -1; 
  if (!where.Contains("Sens",TString::kIgnoreCase)) return -1;
 // if(findpath!=currpath) return -1; //GP FIX filter hit at the edge of sectors
  if(GetSectorNr()%2==0 && !((TString)gGeoManager->GetPath()).Contains("inner") ) return -1;
  
  if(!((TString)gGeoManager->GetPath()).EqualTo(GeoMaskGem[ (GetStationNr()-3)-1 ][GetSectorNr()-1])) return -1;
  //cout<< "FindPath: " <<gGeoManager->GetPath()<<endl;
  //gGeoManager->SetTopVolume(TopVolume);
  //gGeoManager->CdNode(GeoMaskGem[ 0/*(GetStationNr()-3)-1*/ ][0/*GetSectorNr()-1*/]->GetIndex());//SetTopVolume(GeoMaskGem[ (GetStationNr()-3)-1 ][GetSectorNr()-1]);
 // cout<< "CurrPath: " << GeoMaskGem[ (GetStationNr()-3)-1 ][GetSectorNr()-1]<<endl;
 // cout<<endl;
  gGeoManager->SetTopVolume(TopVolume);
 // else if(GetSectorNr()%2!=0 && !((TString)gGeoManager->GetPath()).Contains("module") ) return -1; //GP FIX
//cout<<"c: "<<currpath<<"\n" <<" f: "<<findpath<< "\n" <<endl;

   //cout<<"ST: "<<GetStationNr()<<" x_rec: "<<xCross<<" y_rec: "<<yCross<< " z_rec: "<<zCross<<endl;
 // if(GetStationNr()>5 ) cout<<"Last strip coord: "<<"ST: "<<GetStationNr()<<" module: "<<module<<"   "<<xCross<<","<<yCross<<","<<zCross<<endl;
       } 
}

if(GetStationNr()<4){//fType==8){

  if ( fChan < 0 || fChan >= fNChannelsFront) { // IR 8-JUN-2017 ">=" instead of ">"
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid front channel number ! "
	 << fChan << " " << fNChannelsFront << endl;
    return -1;
  }
  if ( bChan < 0 || bChan >= fNChannelsBack) { // IR 8-JUN-2017 ">=" instead of ">"
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid back channel number ! "
	 << bChan << " " << fNChannelsBack << endl;
    return -1;
  }

//fChan=0.5;
//bChan=613.5;
//fChan=320.0;
//bChan=307.0;


//if(GeoMaskSi[ (GetStationNr())-1 ][GetSectorNr()-1]) gGeoManager->SetTopVolume(GeoMaskSi[ (GetStationNr())-1 ][GetSectorNr()-1]);
//else return -1;
//fNChannelsFront , fNChannelsBack - максимальные номера вертикальных и косых стрипов соответственно
  xCross = 0.;
  yCross = 0.;
  zCross = 0.;

  bChan= (fNChannelsBack)- bChan; // IR 8-JUN-2017 due to opposite directions of X and U in BMN Si
  Double_t sinrot = TMath::Sin(fRotation);
  Double_t cosrot = TMath::Cos(fRotation);

// sinrot, cosrot - глобальны поворот слоя ( не используется)

//  Double_t xint  = ( fChan + 0.5 ) * fDx - cosrot*fFrontLorentzShift*fD/2. ;
//  Double_t x0  = ( bChan + 0.5 ) * fDy - cosrot*fBackLorentzShift*fD/2. ;
  Double_t xint  = ( fChan + 0. ) * fDx;
  Double_t x0  = ( bChan + 0. ) * fDy;

  Double_t xtemp, ytemp;
  Double_t tanstrB = TMath::Tan(fStereoB);

  Double_t yint = ( x0 - xint) / tanstrB;
//cout<<"SI x1: "<<xint<<" y1:"<<yint<< " fchan: "<<fChan<<" bChan: "<<bChan<< " tan: " <<tanstrB<<" lx,ly: "<<fLx<<", "<<fLy <<endl;
  //cout<< GetStationNr()<<","<<GetSectorNr()<<" x_0_2: "<<xint<<" y_0_2: "<<yint<<endl;

 //if(gGeoManager->FindNode(xint,yint,0)==NULL) return -1;
 //if (!((TString)(gGeoManager->FindNode(xint,yint,0)->GetName())).Contains("Sens",TString::kIgnoreCase)   ) return -1; 

  //if( flgBoundary ) if ( ! ( yint>0. && yint<fLy ) ) return -1; //GP FIX hole between bot && top modules
    ytemp = yint - fLy/2. + YSiShift[GetStationNr()-1]; // 11.02.19 IR + GP. The shift for Y. We aren't understanding cause, but it's work.

//построить локальные кординаты шаг 1 мм

    if(GetStationNr()==2 || GetStationNr()==3) { // gap between Si planes implemented for Run-7
    if( ytemp > 0.) ytemp +=0.1151;
    else ytemp -=0.1151;
}  
    //TypeSi=ModTypeS[GetSectorNr()-1];
  // Translation to centre of sector
  xtemp = xint - fLx/2.; // пересчет X,Y координат хита относительно центра слоя


//cout<<modip[GetStationNr()-1][GetSectorNr()-1]<<" "<<GetStationNr()<<","<<GetSectorNr()<<" x_c: "<<xtemp<<" y_c: "<<ytemp<< " shift: "<<YSiShift[GetStationNr()-1] <<endl;

  //if( TypeSi==5 || TypeSi==7) xtemp = -xtemp; // IR 13-JUN-2017 // ? run7
  if( GetStationNr()==3 &&  
  (GetSectorNr()==1 || GetSectorNr()==2 ||
   GetSectorNr()==7 || GetSectorNr()==8) ) xtemp = -xtemp; // ? run7
   if( GetStationNr()==1 &&  
  (GetSectorNr()==1 || GetSectorNr()==4 ) ) xtemp = -xtemp; // ? run7
  if( GetStationNr()==2 &&  GetSectorNr()==1 ) xtemp = -xtemp; // ? run7
  //ytemp = yint - fLy/2.; // Ruf



//cout<< GetStationNr()<<","<<GetSectorNr()<<" x_i: "<<xtemp<<" y_i: "<<ytemp<<endl;
 
// else if(gGeoManager->FindNode(xtemp,ytemp,0)->GetName()!=GeoMask[ (GetStationNr())-1 ][GetSectorNr()-1]->GetName()) return -1;
//cout<<"find: "<<gGeoManager->FindNode(xtemp,ytemp,0)->GetName()<<"\n array: "<<GeoMask[ (GetStationNr())-1 ][GetSectorNr()-1]->GetName()<<endl;
  // Rotation around sector centre
  xCross = xtemp * cosrot - ytemp * sinrot; // пооворот X,Y коорднат относительно центра слоя ( используется)
  yCross = xtemp * sinrot + ytemp * cosrot; // rotate around Z axis

 // cout<< GetStationNr()<<","<<GetSectorNr()<<" x_r: "<<xCross<<" y_r: "<<yCross<<endl;
 
 //cout<<modip[GetStationNr()-1][GetSectorNr()-1]<<" "<<GetStationNr()<<","<<GetSectorNr()<<" x_c2: "<<xCross<<" y_c2: "<<yCross<< " z_c2: "<< fZ0 <<endl;

  //if(  GetSectorNr()==4 ) xCross=xCross*(-1); // rotate around Y axis...  How it's work ?
//cout<<"Si mRAD rot: "<<AlphaZmRad[modip[GetStationNr()-1][GetSectorNr()-1]]<<endl;
  SetRotZ(AlphaZmRad[modip[GetStationNr()-1][GetSectorNr()-1]], xCross, yCross);
  // Translation into global c.s.
 xCross = xCross + fX0; // Смещение X,Y координат относительно центра слоя в глобальной системе координат
 yCross = yCross + fY0;
  //  cout<<"fX0 "<<fX0<<" fY0 "<<fY0<<endl;
  zCross = fZ0;
  
 // cout<<modip[GetStationNr()-1][GetSectorNr()-1]<<" "<<GetStationNr()<<","<<GetSectorNr()<<" x_cr: "<<xCross<<" y_cr: "<<yCross<<" z: "<<zCross<<endl;
  
//cout<<"ST: "<<GetStationNr()<<" x_rec: "<<xCross<<" y_rec: "<<yCross<< " z_rec: "<<zCross<<endl;
  // No intersection found
  //cout<<"GO2"<<endl;
  if ( zCross < 0.001 ) return -1;
  //cout<<"GO3"<<endl;
//cout<<"zcr"<<zCross<<endl;
  //AZ - Reject points outside of the sensor boundaries (for complex geometries) //FIX GP;
   //gGeoManager->SetVisLevel(4);
//   gGeoManager->GetTopVolume()->Draw("ogl");

  if (!(TMath::Abs(fX0) < 0.001 && TMath::Abs(fY0) < 0.001)) {
    //  cout << "X, Y, Z: " << xCross << " " << yCross << " " << zCross << " " 
    //	 << GetStationNr() << " " 
    //	 << gGeoManager->GetCurrentNode()->GetName() << " " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2]
    //	 << " " << gGeoManager->FindNode(xCross, yCross, zCross) << endl;

    if(gGeoManager->FindNode(xCross,yCross,zCross)==NULL) return -1;

    // if(  gGeoManager->FindNode(xCross, yCross,zCross)!=GeoMaskSi[ GetStationNr()-1 ][GetSectorNr()-1] ) return -1;
  
  //TString where = gGeoManager->FindNode(xCross, yCross, zCross)->GetName();
  //cout<<" SI WHERE: "<<where<<" PATH: "<<gGeoManager->GetPath()<<endl;
   //if(GetStationNr()==1) cout<<xCross<<","<<yCross<<","<<zCross<< " name: "<<where <<endl;
  TString cpath=(TString)gGeoManager->GetPath();
  //if (!cpath.Contains(where))  return -1; //GP FIX fName to full path
  //if (!where.Contains("Sens",TString::kIgnoreCase) ) return -1; 
  //if (!cpath.Contains("Sens",TString::kIgnoreCase)   ) return -1; 

  if(cpath.Contains("Sens",TString::kIgnoreCase) )  gGeoManager->CdUp();
  else if(cpath.Contains("mod",TString::kIgnoreCase) ) {};
  if(!((TString)gGeoManager->GetPath()).EqualTo(GeoMaskSi[ GetStationNr()-1 ][GetSectorNr()-1])) return -1;
  //cout<< "FindPath: " <<gGeoManager->GetPath()<<endl;
  //gGeoManager->SetTopVolume(TopVolume);
  //gGeoManager->CdNode(GeoMaskGem[ 0/*(GetStationNr()-3)-1*/ ][0/*GetSectorNr()-1*/]->GetIndex());//SetTopVolume(GeoMaskGem[ (GetStationNr()-3)-1 ][GetSectorNr()-1]);
  //cout<< "CurrPath: " << GeoMaskSi[ GetStationNr()-1 ][GetSectorNr()-1]<<endl;
 // cout<<endl;
  gGeoManager->SetTopVolume(TopVolume);
       } 




  // Int_t g;
   //std::cin>>g;
}



  return fDetectorId;
}

// -------------------------------------------------------------------------


// -----   Public method PointIndex   --------------------------------------
Int_t CbmStsSensor::PointIndex(Int_t iFStrip, Int_t iBStrip) {
  pair<Int_t,Int_t> a(iFStrip, iBStrip);
  if (fTrueHits.find(a) == fTrueHits.end()) return -1;
  return fTrueHits[a];
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmStsSensor::Reset() {
  fFrontActive.clear();
  fBackActive.clear();
  fTrueHits.clear();
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmStsSensor::Print() {
  cout << "   Sensor Nr. ";
  cout.width(3);
  cout << GetSensorNr() << ", Type ";
  cout.width(1);
  cout << fType << ", centre (";
  cout.width(6);
  cout << fX0 << ", ";
  cout.width(6);
  cout << fY0 << ") cm, rotation " ;
  cout.width(6);
  cout << fRotation*180./TMath::Pi() << " deg., lx = ";
  cout.width(3);
  cout << fLx << " cm, ly = ";
  cout.width(3);
  cout << fLy << " cm, channels: " << GetNChannels() << endl;
}
// -------------------------------------------------------------------------



// -----   Private method FrontStripNumber   -------------------------------
Int_t CbmStsSensor::FrontStripNumber(Double_t x, Double_t y) const {

  Double_t xint = 0., yint = 0.;

  // Calculate internal coordinates.
  // If point is inside sensor, return strip number
  if ( IntCoord(x, y, xint, yint) ) {
  Double_t xf = xint + fFrontStripShift + yint * TMath::Tan(fStereoF);
  xf = xf - TMath::Floor(xf/fLx) * fLx;
  Int_t iStrip = Int_t( xf / fDx );

  if (iStrip < 0 || iStrip > fNChannelsFront) {
    cout << "-E- CbmStsSensor::FrontStripNumber: Invalid strip number"
	 << "  " << iStrip << endl;
    cout << "    Sensor " << fDetectorId << ", x = " << xint << ", y = "
	 << yint << endl;
    Fatal("FrontStripNumber", "Invalid strip number" );
  }
  return iStrip;
  }

  // Return -1 if point is outside sensor
  return -1;

}
// -------------------------------------------------------------------------



// -----   Private method BackStripNumber   --------------------------------
Int_t CbmStsSensor::BackStripNumber(Double_t x, Double_t y) const {

  Double_t xint = 0., yint = 0.;

  // Calculate internal coordinates.
  // If point is inside sensor, calculate projection onto readout line
  // and determine channel number
  if ( IntCoord(x, y, xint, yint) ) {
    if (fStereoB*180/TMath::Pi()>80) {
      Double_t xp = yint;
      xp = xp - TMath::Floor(xp/fLy) * fLy;
      Int_t iStrip = (Int_t) ( xp / fDy );
      if (iStrip < 0 || iStrip > fNChannelsBack) {
        cout << "-E- CbmStsSensor::BackStripNumber: Invalid strip number"
	     << "  " << iStrip << endl;
        cout << "    Sensor " << fDetectorId << ", x = " << xint << ", y = "
	     << yint << endl;
        Fatal("BackStripNumber", "Invalid strip number" );
      }
      return iStrip;
    }
    else {
      // Project point along backside strip to y = 0
      Double_t xp = xint + fBackStripShift + yint * TMath::Tan(fStereoB);
      // Calculate modulo w.r.t. sensor x width
      xp = xp - TMath::Floor(xp/fLx) * fLx;
      // Digitise
      Int_t iStrip = (Int_t) ( xp / fDx );
      if (iStrip < 0 || iStrip > fNChannelsBack) {
        cout << "-E- CbmStsSensor::BackStripNumber: Invalid strip number"
	     << "  " << iStrip << endl;
        cout << "    Sensor " << fDetectorId << ", x = " << xint << ", y = "
	     << yint << endl;
        Fatal("BackStripNumber", "Invalid strip number" );
      }
      return iStrip;
    }


  }

  // Return -1 if point is outside sensor
  return -1;

}
// -------------------------------------------------------------------------



// -----   Private method IntCoord   ---------------------------------------
Bool_t CbmStsSensor::IntCoord(Double_t x, Double_t y,
			      Double_t& xint, Double_t& yint) const {

  // Translation into sensor centre system
  x = x - fX0;
  y = y - fY0;

  // Rotation around the sensor centre
  xint = x * TMath::Cos(fRotation) + y * TMath::Sin(fRotation);
  yint = y * TMath::Cos(fRotation) - x * TMath::Sin(fRotation);

  // Translation into sensor corner system
  xint = xint + fLx/2.;
  yint = yint + fLy/2.;

  // Check whether point is inside the sensor
  if ( ! IsInside(xint, yint) ) {
    xint = yint = 0.;
    return kFALSE;
  }

  return kTRUE;

}
// -------------------------------------------------------------------------


// -----   Private method IntCoord   ---------------------------------------
Bool_t CbmStsSensor::IntCoord(Double_t x, Double_t y, Double_t z,
			      Double_t& xint, Double_t& yint, Double_t& zint) const {

  // Translation into sensor centre system
  x = x - fX0;
  y = y - fY0;

  // Rotation around the sensor centre
  xint = x * TMath::Cos(fRotation) + y * TMath::Sin(fRotation);
  yint = y * TMath::Cos(fRotation) - x * TMath::Sin(fRotation);

  // Translation into sensor corner system
  xint = xint + fLx/2.;
  yint = yint + fLy/2.;

  // Check whether point is inside the sensor
  if ( ! IsInside(xint, yint) ) {
    xint = yint = 0.;
    return kFALSE;
  }

  return kTRUE;

}
// -------------------------------------------------------------------------



// -----   Private method IsInside   ---------------------------------------
Bool_t CbmStsSensor::IsInside(Double_t xint, Double_t yint) const {
  if ( xint < 0.  ) return kFALSE;
  if ( xint > fLx ) return kFALSE;
  if ( yint < 0.  ) return kFALSE;
  if ( yint > fLy ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------




ClassImp(CbmStsSensor)
