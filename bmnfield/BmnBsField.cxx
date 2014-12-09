#include "BmnBsField.h"

#include "TFile.h"
#include "TSystem.h"
#include "TArrayF.h"
 
#include <iostream>
#include <fstream>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;
using std::fabs;

Int_t checked_index_max(Int_t suggested_index,Int_t array_size)  
{
  if (suggested_index<array_size)
    return suggested_index;
  else
    return (suggested_index-5);  
}

BmnBsField::BmnBsField(const char* pBsName, Int_t  symType) 
  : BmnFieldMap(),
    NDIM(0), 
    LL1(0), 
    LL2(0), 
    LL3(0), 
    II1(0), 
    II2(0), 
    II3(0),
    fBsBx(NULL),
    fBsBy(NULL),
    fBsBz(NULL),
    fX(NULL),
    fY(NULL),
    fZ(NULL),
    UX1(NULL),
    UX2(NULL),
    UX3(NULL),
    F0(NULL),
    G0(NULL),
    U0(NULL),
    fBsName(pBsName)
{ 
  fType = 6;               // corresponds symType=3
  this->SetName(pBsName);
  this->SetTitle(pBsName);
}

BmnBsField::BmnBsField()
  : BmnFieldMap(),
    NDIM(0), 
    LL1(0), 
    LL2(0), 
    LL3(0), 
    II1(0), 
    II2(0), 
    II3(0),
    fBsBx(NULL),
    fBsBy(NULL),
    fBsBz(NULL),
    fX(NULL),
    fY(NULL),
    fZ(NULL),
    UX1(NULL),
    UX2(NULL),
    UX3(NULL),
    F0(NULL),
    G0(NULL),
    U0(NULL),
    fBsName("")
{
  fType = 0;  
}

// -------------   Constructor from BmnFieldPar   -------------------------
BmnBsField::BmnBsField(BmnFieldPar* fieldPar)
  : BmnFieldMap(fieldPar),
    NDIM(0), 
    LL1(0), 
    LL2(0), 
    LL3(0), 
    II1(0), 
    II2(0), 
    II3(0),
    fBsBx(NULL),
    fBsBy(NULL),
    fBsBz(NULL),
    fX(NULL),
    fY(NULL),
    fZ(NULL),
    UX1(NULL),
    UX2(NULL),
    UX3(NULL),
    F0(NULL),
    G0(NULL),
    U0(NULL),
    fBsName("")
{
  fType = 6;
}

BmnBsField::~BmnBsField()
{
  delete fX;
  delete fY;
  delete fY;
  delete fBsBx;
  delete fBsBy;
  delete fBsBz;
  delete (FairField*)this;
}

void BmnBsField::Init()
{
  TString tmp_FileName;
  TString dir =  gSystem->Getenv("VMCWORKDIR"); // getenv("VMCWORKDIR");
  tmp_FileName = dir + "/input/" + fBsName+".root";
  readBsRootfile (tmp_FileName);
}


void BmnBsField::CalculateMapFromBs(Int_t pNx, Int_t pNy, Int_t pNz)
{
  Double_t po[3],B[3], sm=100;
  fNx = (pNx<=0)? fX->GetSize()-3 : pNx;
  fNy = (pNy<=0)? fY->GetSize()-3 : pNy;
  fNz = (pNz<=0)? fZ->GetSize()-3 : pNz;
  if ((fNx<2)||(fNy<2)||(fNz<2))
    {
      cout << "CalculateMapFromBs ERROR [Ns]: " << fNx << " " << fNy  << " " << fNz << endl;
      return;
    }
  fXmin = (fX->GetArray())[3] *sm;
  fYmin = (fY->GetArray())[3] *sm;
  fZmin = (fZ->GetArray())[3] *sm;
  fXmax = (fX->GetArray())[fX->GetSize()-1] *sm;
  fYmax = (fY->GetArray())[fY->GetSize()-1] *sm;
  fZmax = (fZ->GetArray())[fZ->GetSize()-1] *sm;
  fXstep = ( fXmax - fXmin ) / Double_t( fNx - 1 );
  fYstep = ( fYmax - fYmin ) / Double_t( fNy - 1 );
  fZstep = ( fZmax - fZmin ) / Double_t( fNz - 1 );
  if ( fBx ) delete fBx;
  if ( fBy ) delete fBy;
  if ( fBz ) delete fBz;
  fBx = new TArrayF(fNx * fNy * fNz);
  fBy = new TArrayF(fNx * fNy * fNz);
  fBz = new TArrayF(fNx * fNy * fNz);


  cout << "CalculateMapFromBs [Ns]: " << fNx << " " << fNy  << " " << fNz << endl;
  cout << "CalculateMapFromBs [MINs]: " << fXmin << " " << fYmin  << " " << fZmin << endl;
  cout << "CalculateMapFromBs [MAXs]: " << fXmax << " " << fYmax  << " " << fZmax << endl;
  cout << "CalculateMapFromBs [STEPs]: " << fXstep << " " << fYstep  << " " << fZstep << endl;
  cout << "CalculateMapFromBs [POSs]: " << fPosX << " " << fPosY  << " " << fPosZ << endl;

  // GetFieldValue (without conversion from kG to T)
  Int_t index = 0;
  for (Int_t ix=0; ix<fNx; ix++) {
    po[0]=(fXmin+fXstep*ix)+fPosX;    // sm
    for (Int_t iy=0; iy<fNy; iy++) {
      po[1]=(fYmin+fYstep*iy)+fPosY;    // sm
      for (Int_t iz=0; iz<fNz; iz++) {
	po[2]=(fZmin+fZstep*iz)+fPosZ;    // sm
	index = ix*fNy*fNz + iy*fNz + iz;
	GetFieldValue(po,B);
	if ( fBx ) (*fBx)[index] = B[0] ;    // kG
	if ( fBy ) (*fBy)[index] = B[1] ;    // kG
	if ( fBz ) (*fBz)[index] = B[2] ;    // kG
      }
    }
  }
  Print();
}

void BmnBsField::writeBsRootfile(const char *name)    // Write Field Splined 
{
  TFile *oldfile=gFile;
  TFile *f=new TFile(name, "RECREATE");
  this->Write();
  f->Close();
  if(oldfile) oldfile->cd();
}

void BmnBsField::readBsRootfile (const char *name)    // Read  Field Splined 
{
	TFile *oldfile=gFile;
	TFile *f=new TFile(name, "READ");		
	if (f->IsZombie()) {
	    cout << "-E- BmnBsField::readBsRootfile:  can not read from file: " << endl;
	    Fatal("BmnBsField::readBsRootfile:","Can not read from input file");
	  }
	const char *bsname=fBsName.Data();
 	BmnBsField *fnew=new BmnBsField(bsname);

	cout << "-I- BmnBsField Reading splined field : " << bsname << " from root file : "<< name << endl;
	f->GetObject(bsname, fnew);

	if(fBsBx) delete fBsBx;
	if(fBsBy) delete fBsBy;
	if(fBsBz) delete fBsBz;	
	fBsBx=new TArrayF(*fnew->GetBsBx());
	fBsBy=new TArrayF(*fnew->GetBsBy());
	fBsBz=new TArrayF(*fnew->GetBsBz());

	if(fX) delete fX;
	if(fY) delete fY;
	if(fZ) delete fZ;
	fX=new TArrayF(*fnew->GetX());
	fY=new TArrayF(*fnew->GetY());
	fZ=new TArrayF(*fnew->GetZ());

	f->Close();
	if(oldfile) oldfile->cd();

	UX1 = fX->GetArray();
	UX2 = fY->GetArray();
	UX3 = fZ->GetArray();
	F0 = fBsBx->GetArray();
	G0 = fBsBy->GetArray();
	U0 = fBsBz->GetArray();

	LL1 = fX->GetSize();
	LL2 = fY->GetSize();
	LL3 = fZ->GetSize();
	NDIM = fBsBx->GetSize();
	II1 = LL1-1;
	II2 = LL2-1;
	II3 = LL3-1;
}

void BmnBsField::GetFieldValue(const Double_t Point[3], Double_t *Bfield)  
{
  Double_t XX,YY,ZZ,X,Y,Z,FX,FY,FZ,pfScale;
  Float_t fHemiX = 1., fHemiY = 1.,fHemiZ = 1;

  X=Point[0]-fPosX;
  Y=Point[1]-fPosY;
  Z=Point[2]-fPosZ;
  XX=fabs(X);
  YY=fabs(Y);
  ZZ=fabs(Z);
  FX=0.0;  FY=0.0;    FZ=0.0;

  PALC0(XX*0.01,YY*0.01,ZZ*0.01,&FX,&FY,&FZ);   // position for PALC0 should be in meters

  pfScale=fScale*10;   // usually fScale=1;
  FX=FX*pfScale;
  FY=FY*pfScale;
  FZ=FZ*pfScale;

  if (fType)            // 3 or 2
    {
      if ( X < 0. ) 
	fHemiX = -1.;
      if ( Y < 0. ) 
	fHemiY = -1.;

      FX = FX * fHemiX * fHemiY;

      if ((fType==3)&&(Z<0.))     // 3
	fHemiZ = -1.;

      FZ= FZ* fHemiY * fHemiZ;
    }
  
  Bfield[0]=FX;  Bfield[1]=FY;  Bfield[2]=FZ; 
  return;
}

Double_t BmnBsField::GetBx(Double_t x, Double_t y, Double_t z) 
{
  Double_t Point[3];
  Double_t Bfield[3];
  Point[0]=x; Point[1]=y; Point[2]=z;
  GetFieldValue(Point, Bfield);
  return (Bfield[0]);
}

Double_t BmnBsField::GetBy(Double_t x, Double_t y, Double_t z) 
{
  Double_t Point[3];
  Double_t Bfield[3];
  Point[0]=x; Point[1]=y; Point[2]=z;
  GetFieldValue(Point, Bfield);
  return (Bfield[1]);
}

Double_t BmnBsField::GetBz(Double_t x, Double_t y, Double_t z) 
{
  Double_t Point[3];
  Double_t Bfield[3];
  Point[0]=x; Point[1]=y; Point[2]=z;
  GetFieldValue(Point, Bfield);
  return (Bfield[2]);
}


void BmnBsField::readFortranAsciifiles(const char *BsFortranAsciiFileName1,const char *BsFortranAsciiFileName2,
				       const char *BsFortranAsciiFileName3)
{
  Float_t val;
  Int_t i;

  TString fileNam1 = BsFortranAsciiFileName1;
  ifstream fBsFortranAscii1(fileNam1);
  if ( ! fBsFortranAscii1.is_open() ) 
    {
      cerr << "ERROR: File " <<BsFortranAsciiFileName1 << " not opened!" << endl;
    }
  else
    {
      fBsFortranAscii1 >> NDIM >> LL1 >> LL2 >> LL3 >> II1 >> II2 >> II3;

      fX=new TArrayF(LL1);
      UX1 = fX->GetArray();
      for(i=0; i<LL1; i++) 
	{
	  fBsFortranAscii1 >> val;
	  fX->AddAt(val,i);
	}

      fBsBx = new TArrayF(NDIM);
      F0 = fBsBx->GetArray();
      for(i=0; i<NDIM; i++) 
	{
	  fBsFortranAscii1 >> val;
	  fBsBx->AddAt(val,i);
	}

      fBsFortranAscii1.close();
    }

  TString fileNam2 = BsFortranAsciiFileName2;
  ifstream fBsFortranAscii2(fileNam2);
  if ( ! fBsFortranAscii2.is_open() ) 
    {
      cerr << "ERROR: File " <<BsFortranAsciiFileName2 << " not opened!" << endl;
    }
  else
    {
      fBsFortranAscii2 >> NDIM >> LL1 >> LL2 >> LL3 >> II1 >> II2 >> II3;
      fY=new TArrayF(LL2);
      UX2 = fY->GetArray();
      for(i=0; i<LL2; i++) 
	{
	  fBsFortranAscii2 >> val;
	  fY->AddAt(val,i);
	}
      fBsBy = new TArrayF(NDIM);
      G0 = fBsBy->GetArray();
      for(i=0; i<NDIM; i++) 
	{
	  fBsFortranAscii2 >> val;
	  fBsBy->AddAt(val,i);
	}
      fBsFortranAscii2.close();
   }

  TString fileNam3 = BsFortranAsciiFileName3;
  ifstream fBsFortranAscii3(fileNam3);
  if ( ! fBsFortranAscii3.is_open() ) 
    {
      cerr << "ERROR: File " <<BsFortranAsciiFileName3 << " not opened!" << endl;
    }
  else
    {
      fBsFortranAscii3 >> NDIM >> LL1 >> LL2 >> LL3 >> II1 >> II2 >> II3;
      fZ=new TArrayF(LL3);
      UX3 = fZ->GetArray();
      for(i=0; i<LL3; i++) 
	{
	  fBsFortranAscii3 >> val;
	  fZ->AddAt(val,i);
	}
      fBsBz = new TArrayF(NDIM);
      U0 = fBsBz->GetArray();
      for(i=0; i<NDIM; i++) 
	{
	  fBsFortranAscii3 >> val;
	  fBsBz->AddAt(val,i);
	}
      fBsFortranAscii3.close();
    }

 }

void BmnBsField::PALC0(Double_t X,Double_t Y,Double_t Z,Double_t *BX,Double_t *BY,Double_t *BZ)
{
  Double_t UX[4],UY[4],UZ[4],X0,X1,X2,X3,X4,X5,X6,X7,Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7,VX0,VX1,VX2,VX3,VX4,VX5,VX6,VX7;
  Long64_t INT1[4],INT2[4],INT3[4],KK,K1,K2,K3,KK1,KK2,KK3,NN0,I1,JJ0,I2,JJ1,I3,JJ2;
  Double_t EPS=1.0e-7, XRZYX,YRZYX,ZRZYX,XRZY,YRZY,ZRZY,XRZ,YRZ,ZRZ,SS3,SS2,SS1;
  Int_t izero4=3, izero5=4;
  if (X<(UX1[izero4]-EPS)) goto  m100 ;
  for (KK=izero5;KK<LL1;KK++)
    {
      if (X<=UX1[KK]) goto  m52;    
    }
  goto  m100 ;
 m52:
  K1=KK ;                                                          
  if (Y<(UX2[izero4]-EPS)) goto  m100 ;
  for (KK=izero5;KK<LL2;KK++)
    { 
      if (Y<=UX2[KK])goto  m54;   
    }                        
  goto  m100 ;
 m54:
  K2=KK;                                                           
  if (Z<(UX3[izero4]-EPS))goto  m100; 
  //  for (KK=izero5;KK<=LL3;KK++)
  for (KK=izero5;KK<LL3;KK++)
    { 
      if (Z<=UX3[KK])goto  m56 ;  
    }                            
  goto  m100 ;
 m56:
  K3=KK;                         
  X7=UX1[checked_index_max(K1+3,LL1)]; 
  X6=UX1[checked_index_max(K1+2,LL1)];
  X5=UX1[checked_index_max(K1+1,LL1)];
  X4=UX1[K1];
  X3=UX1[K1-1];
  X2=UX1[K1-2];
  X1=UX1[K1-3];
  X0=UX1[K1-4];
  UX[1-1]=SPL0(X,X0,X1,X2,X3,X4);
  UX[2-1]=SPL0(X,X1,X2,X3,X4,X5);
  UX[3-1]=SPL0(X,X2,X3,X4,X5,X6);
  UX[4-1]=SPL0(X,X3,X4,X5,X6,X7);

  Y7=UX2[checked_index_max(K2+3,LL2)];  
  Y6=UX2[checked_index_max(K2+2,LL2)];
  Y5=UX2[checked_index_max(K2+1,LL2)];
  Y4=UX2[K2];
  Y3=UX2[K2-1];
  Y2=UX2[K2-2];
  Y1=UX2[K2-3];
  Y0=UX2[K2-4];
  UY[1-1]=SPL0(Y,Y0,Y1,Y2,Y3,Y4);
  UY[2-1]=SPL0(Y,Y1,Y2,Y3,Y4,Y5);
  UY[3-1]=SPL0(Y,Y2,Y3,Y4,Y5,Y6);
  UY[4-1]=SPL0(Y,Y3,Y4,Y5,Y6,Y7);

  VX7=UX3[checked_index_max(K3+3,LL3)]; 
  VX6=UX3[checked_index_max(K3+2,LL3)];
  VX5=UX3[checked_index_max(K3+1,LL3)];
  VX4=UX3[K3];
  VX3=UX3[K3-1];
  VX2=UX3[K3-2];
  VX1=UX3[K3-3];
  VX0=UX3[K3-4];
  UZ[1-1]=SPL0(Z,VX0,VX1,VX2,VX3,VX4);
  UZ[2-1]=SPL0(Z,VX1,VX2,VX3,VX4,VX5);
  UZ[3-1]=SPL0(Z,VX2,VX3,VX4,VX5,VX6);
  UZ[4-1]=SPL0(Z,VX3,VX4,VX5,VX6,VX7);
  KK1=K1-4;
  KK2=K2-5+1;
  if (KK2<0)
    KK2=0;
  KK3=K3-5+1;
  if (KK3<0)
    KK3=0;
  NN0=KK1+II1*(KK2+II2*(KK3));
  INT1[1-1]=NN0;
  INT1[2-1]=NN0+1;
  INT1[3-1]=NN0+2;
  INT1[4-1]=NN0+3;

  INT2[1-1]=0;
  INT2[2-1]=(1)*II1;
  INT2[3-1]=(2)*II1;
  INT2[4-1]=(3)*II1;


  INT3[1-1]=0;
  INT3[2-1]=(1)*II1*II2;
  INT3[3-1]=(2)*II1*II2;
  INT3[4-1]=(3)*II1*II2;

  XRZYX=0.;
  YRZYX=0.;
  ZRZYX=0.;

  for (I1=1;I1<=4;I1++)
    {
      JJ0=INT1[I1-1];
      XRZY=0.;
      YRZY=0.;
      ZRZY=0.;

      for (I2=1;I2<=4;I2++)
	{
	  JJ1=JJ0+INT2[I2-1];
	  XRZ=0.;
	  YRZ=0.;
	  ZRZ=0.;
	  for (I3=1;I3<=4;I3++)
	    {
	      JJ2=JJ1+INT3[I3-1];
	      SS3=UZ[I3-1];
	      XRZ=XRZ+F0[JJ2]*SS3;
	      YRZ=YRZ+G0[JJ2]*SS3;
	      ZRZ=ZRZ+U0[JJ2]*SS3;
	    }
	  SS2=UY[I2-1];
	  XRZY=XRZY+XRZ*SS2;
	  YRZY=YRZY+YRZ*SS2;
	  ZRZY=ZRZY+ZRZ*SS2;
	}
      SS1=UX[I1-1];
      XRZYX=XRZYX+XRZY*SS1;
      YRZYX=YRZYX+YRZY*SS1;
      ZRZYX=ZRZYX+ZRZY*SS1;
    }
  *BX=XRZYX;
  *BY=YRZYX;
  *BZ=ZRZYX;
  return;
 m100:
//       print *, 'ATTENTION !!! Point is out of range'
//  cout << "PALC0 - XYZ out of range: ";
//  cout << X << " " << Y  << " " << Z << " " << ";" << endl;
  *BX=0;
  *BY=0;
  *BZ=0;
  return;
}

Float_t  BmnBsField::SPL0(Double_t T,Double_t X0,Double_t X1,Double_t X2,Double_t X3,Double_t X4)
{
  Double_t W0,C0,TT,RR,TPL,W1,W2,W3,value_SPL0;
  if (T<X0)goto  n100;
  if (T<X1)goto  n200;
  if (T<X2)goto  n300;
  if (T<X3)goto  n400;
  if (T>X4)goto  n100;
  C0=X4-X0;
  W0=(X1-X0)*(X2-X0)*(X3-X0)*(X4-X0);
  TT=T-X0;
  RR=C0/W0;
  TPL=RR*(TT*TT*TT);
  W1=(X0-X1)*(X2-X1)*(X3-X1)*(X4-X1);
  TT=T-X1;
  RR=C0/W1;
  TPL=RR*(TT*TT*TT)+TPL;
  W2=(X0-X2)*(X1-X2)*(X3-X2)*(X4-X2);
  TT=T-X2;
  RR=C0/W2;
  TPL=RR*(TT*TT*TT)+TPL;
  W3=(X0-X3)*(X1-X3)*(X2-X3)*(X4-X3);
  TT=T-X3;
  RR=C0/W3;
  value_SPL0=RR*(TT*TT*TT)+TPL;
  return ((Float_t)value_SPL0);
 n200:
  C0=X4-X0;
  W0=(X1-X0)*(X2-X0)*(X3-X0)*(X4-X0);
  TT=T-X0;
  RR=C0/W0;
  value_SPL0=RR*(TT*TT*TT);
  return ((Float_t)value_SPL0);
 n300:
  C0=X4-X0;
  W0=(X1-X0)*(X2-X0)*(X3-X0)*(X4-X0);
  TT=T-X0;
  RR=C0/W0;
  TPL=RR*(TT*TT*TT);
  W1=(X0-X1)*(X2-X1)*(X3-X1)*(X4-X1);
  TT=T-X1;
  RR=C0/W1;
  value_SPL0=RR*(TT*TT*TT)+TPL;
  return ((Float_t)value_SPL0);
  n400:
    C0=X4-X0;
  W0=(X1-X0)*(X2-X0)*(X3-X0)*(X4-X0);
  TT=T-X0;
  RR=C0/W0;
  TPL=RR*(TT*TT*TT);
  W1=(X0-X1)*(X2-X1)*(X3-X1)*(X4-X1);
  TT=T-X1;
  RR=C0/W1;
  TPL=RR*(TT*TT*TT)+TPL;
  W2=(X0-X2)*(X1-X2)*(X3-X2)*(X4-X2);
  TT=T-X2;
  RR=C0/W2;
  value_SPL0=RR*(TT*TT*TT)+TPL;
  return ((Float_t)value_SPL0);
 n100:
  value_SPL0=0.0;
  return ((Float_t)value_SPL0) ;
}
