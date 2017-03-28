#include "DchTrackManagerSim.h"
//#include "DchTrackCand.h"

//#include <TMatrix.h>
#include <TMath.h>
#include <TVector3.h>
#include "TVirtualFitter.h"
#include "Math/Vector3D.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "ScalarD.h"
#include "Riostream.h"
#include <iomanip>
#include "DCHgeometryconstants_bmn.h"

#include "Math/Point3D.h"
#include "Math/Vector3D.h"

using namespace ROOT::Math;

//
//ClassImp(DchTrackCand)
//ClassImp(DchTrackManagerSim)

//using namespace ROOT::Math;

static Bool_t checkFit=false;
 //Bool_t checkFit=true;
 //const Double_t DchTrackManagerSim::resolution =  15e-3; // 150 mkm  ==  0.015 cm
 //const UInt_t DchTrackManagerSim::nintervals=40;// number of intervals along calibration time axis
 const UInt_t DchTrackManagerSim::ncols=11; //number of matrix columns


static Double_t distance2(Double_t x,Double_t y,Double_t z, Double_t *p);
static void SumDistance2(Int_t &, Double_t *, Double_t & sum, Double_t * par, Int_t ); 
static void line(double t, double *p, double &x, double &y, double &z); 

// define the parameteric line equation 
static void line(double t, double *p, double &x, double &y, double &z) { 
//void DchTrackManagerSim::line(double t, double *p, double &x, double &y, double &z) { 
     // a parameteric line is defined from 6 parameters but 4 are independent
     // x0,y0,z0,z1,y1,z1 which are the coordinates of two points on the line
     // can choose z0 = 0 if line not parallel to x-y plane and z1 = 1; 
     x = p[0] + p[1]*t; 
     y = p[2] + p[3]*t;
     z = t; 
} 
// calculate distance line-point 

static Double_t distance2(Double_t x,Double_t y,Double_t z,Double_t *p) { 
//Double_t DchTrackManagerSim::distance2(Double_t x,Double_t y,Double_t z,Double_t *p) { 
   // distance line point is D= | (xp-x0) cross  ux | 
   // where ux is direction of line and x0 is a point in the line (like t = 0) 
   XYZVector xp(x,y,z); 
   XYZVector x0(p[0], p[2], 0. ); 
   XYZVector x1(p[0] + p[1], p[2] + p[3], 1. ); 
   XYZVector u = (x1-x0).Unit(); 
   Double_t d2 = ((xp-x0).Cross(u)).Mag2(); 
   if(checkFit)cout<<"d2 = "<<d2<<endl;
   return d2; 
}

//____________________________________________________________________
// function to be minimized 
static void SumDistance2(Int_t &, Double_t *, Double_t & sum, Double_t * par, Int_t ) { 
//void DchTrackManagerSim::SumDistance2(Int_t &, Double_t *, Double_t & sum, Double_t * par, Int_t ) { 

   // the TGraph must be a global variable
   TGraph2DErrors * gr = dynamic_cast<TGraph2DErrors*>( (TVirtualFitter::GetFitter())->GetObjectFit() );
   assert(gr != 0);
   Double_t * x = gr->GetX();
   Double_t * y = gr->GetY();
   Double_t * z = gr->GetZ();
   Int_t npoints = gr->GetN();
   if(checkFit)cout<<*x<<" "<<*y<<" "<<*z<<" "<<npoints<<" x,y,z,npoints"<<endl;
   sum = 0.;
   for (Int_t i  = 0; i < npoints; ++i) { 
      Double_t d = distance2(x[i],y[i],z[i],par); 
      sum += d;
      if(checkFit) std::cout << "point " << i << "\t" 
                           << x[i] << "\t" 
                           << y[i] << "\t" 
                           << z[i] << "\t" 
                           << std::sqrt(d) << std::endl; 
   }
   if(checkFit) std::cout << "Total sum2 = " << sum << std::endl;
}

//____________________________________________________________________
static void myfcn(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) {
//void DchTrackManagerSim::myfcn(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) {
   //minimisation function computing the sum of squares of residuals
   //TGraph *gr;
   TGraph2DErrors * gr = dynamic_cast<TGraph2DErrors*>( (TVirtualFitter::GetFitter())->GetObjectFit() );
   //TGraph2DErrors * gr = dynamic_cast<TGraph2DErrors*>( fitter->GetObjectFit() );
   assert(gr != 0);
   Int_t np = gr->GetN();
   f = 0;
   Double_t *x = gr->GetX();
   Double_t *y = gr->GetY();
   for (Int_t i=0;i<np;i++) {
      Double_t u = x[i] - par[0];
      Double_t v = y[i] - par[1];
      Double_t dr = par[2] - TMath::Sqrt(u*u+v*v);
      f += dr*dr;
   }
}

//____________________________________________________________________
static void myfcn2(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) {
//void DchTrackManagerSim::myfcn2(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) {
   //minimisation function computing the sum of squares of residuals
   //TGraph *gr;
   TGraphErrors * gr = dynamic_cast<TGraphErrors*>( (TVirtualFitter::GetFitter())->GetObjectFit() );
   //TGraph2DErrors * gr = dynamic_cast<TGraph2DErrors*>( fitter->GetObjectFit() );
   assert(gr != 0);
   Int_t np = gr->GetN();
   f = 0;
   Double_t *x = gr->GetX();
   Double_t *y = gr->GetY();
   for (Int_t i=0;i<np;i++) {
      Double_t u = x[i] - par[0];
      Double_t v = y[i] - par[1];
      Double_t dr = par[2] - TMath::Sqrt(u*u+v*v);
      f += dr*dr;
   }
}

//_________________________________________________________________

ClassImp(DchTrackCand)
ClassImp(DchTrackManagerSim)
//_________________________________________________________________
//DchTrackManagerSim::DchTrackManagerSim(const DchTrackCand *DchTrCand1,const DchTrackCand *DchTrCand2):checkDTM(true) {
DchTrackManagerSim::DchTrackManagerSim(TString runtype):checkDTM(true) {

runType = runtype;
     //DchTrackCand* DchTrCand1;
     //DchTrackCand* DchTrCand2
     //DchTrackCandReader(DchTrackCand* &DchTrCand1,DchTrackCand* &DchTrCand2);
     //dchTrCand1=DchTrCand1;
     //dchTrCand2=DchTrCand2;
     //DchTrackCandReader(dchTrCand1,dchTrCand2);

}
//_________________________________________________________________
DchTrackManagerSim::~DchTrackManagerSim() {

  //delete dchTrCand1;
  //delete dchTrCand2;
}
//_________________________________________________________________
void DchTrackManagerSim::InitDch(const UShort_t Iter, const Double_t ranmin[], const Double_t ranmax[], const Double_t Resolution, const UInt_t Nintervals) {

 iter=Iter; 
 resolution=Resolution;
 nintervals=Nintervals;  
 HistoBookDch(ranmin,ranmax); 

}
//_______________________________________________________________
/*void DchTrackManagerSim::DchTrackCandReader(const UInt_t numevents) {

  fdstread1 = new TFile("/home/fedorisin/trunk/bmnroot/macro/run/dchtrackcands1.root","read");
  fdstread2 = new TFile("/home/fedorisin/trunk/bmnroot/macro/run/dchtrackcands2.root","read");

  TString str,str1,str2;
  for (UInt_t jk = 0; jk < numevents; jk++) {
   if(jk%5000==0)cout<<"number of events in track merging = "<<jk<<endl;
   str.Form("%d",jk);
   str1=TString("DchTrackCandidates")+str;
   fdstread1->cd();
   dchTrCand1 = (DchTrackCand*)fdstread1->Get(str1);
   Int_t trackEntries=dchTrCand1->fDchTrackCand->GetEntriesFast();
   //cout<<"trackEntries1 = "<<trackEntries<<endl;
   str2=TString("DchTrackCandidates")+str;
   fdstread2->cd();
   dchTrCand2 = (DchTrackCand*)fdstread2->Get(str2);
   trackEntries=dchTrCand1->fDchTrackCand->GetEntriesFast();
   //cout<<"trackEntries2 = "<<trackEntries<<endl;
   DchTrackMerger();
  }

}*/
//_______________________________________________________________
void DchTrackManagerSim::HistoBookDch(const Double_t ranmin[], const Double_t ranmax[]) {
//void HistoBookDch(Double_t zLayer[][numSubmodules][numLayersInRow], Int_t numModules, Int_t numSubmodules, Int_t numLayersInRow) {
cout<<"ranmin1 = "<<ranmin[0]<<", ranmax1 = "<<ranmax[0]<<endl;
cout<<"ranmin2 = "<<ranmin[1]<<", ranmax2 = "<<ranmax[1]<<endl;
const Double_t ranmin0=0.5*(ranmin[0]+ranmin[1]);
const Double_t ranmax0=0.5*(ranmax[0]+ranmax[1]);
const Int_t tbins0=Int_t(ranmax0-ranmin0);
const Int_t tbins[2] = {Int_t(ranmax[0]-ranmin[0]),Int_t(ranmax[1]-ranmin[1])};

    TString histtitle,histtitle1,histtitle2=" in both DCHs";
    chi2linfit = new TH1D("chi2linfit","chi2 of linear fit",500,0.,100.); 
    trackAngleX = new TH1D("trackAngleX","track angle along x",180,-90.,90.); 
    trackAngleY = new TH1D("trackAngleY","track angle along y",180,-90.,90.); 
    histtitle1="Y hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    residVsTDCy = new TH2D("residVsTDCy",histtitle,(Int_t)nintervals,0.,ranmax0-ranmin0,tbins0,-1.,1.); 
    histtitle1="X hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    residVsTDCx = new TH2D("residVsTDCx",histtitle,(Int_t)nintervals,0.,ranmax0-ranmin0,tbins0,-1.,1.); 
    histtitle1="U hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    residVsTDCu = new TH2D("residVsTDCu",histtitle,(Int_t)nintervals,0.,ranmax0-ranmin0,tbins0,-1.,1.); 
    histtitle1="V hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    residVsTDCv = new TH2D("residVsTDCv",histtitle,(Int_t)nintervals,0.,ranmax0-ranmin0,tbins0,-1.,1.); 
    histtitle1="hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    residVsTDC = new TH2D("residVsTDC",histtitle,(Int_t)nintervals,0.,ranmax0-ranmin0,tbins0,-1.,1.); 
    TString str,histname;
    histtitle2=" in DCH"; 
    for (Int_t i = 0; i < 2; ++i) {
     str.Form("%u",i+1);
     histname=TString("ResidVsTDCy")+str;
     histtitle1="Y hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDCy[i] = new TH2D(histname,histtitle,(Int_t)nintervals,0.,ranmax[i]-ranmin[i],tbins[i],-1.,1.); 
     histname=TString("ResidVsTDCx")+str;
     histtitle1="X hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDCx[i] = new TH2D(histname,histtitle,(Int_t)nintervals,0.,ranmax[i]-ranmin[i],tbins[i],-1.,1.); 
     histname=TString("ResidVsTDCu")+str;
     histtitle1="U hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDCu[i] = new TH2D(histname,histtitle,(Int_t)nintervals,0.,ranmax[i]-ranmin[i],tbins[i],-1.,1.); 
     histname=TString("ResidVsTDCv")+str;
     histtitle1="V hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDCv[i] = new TH2D(histname,histtitle,(Int_t)nintervals,0.,ranmax[i]-ranmin[i],tbins[i],-1.,1.); 
     histname=TString("ResidVsTDC")+str;
     histtitle1="hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDC[i] = new TH2D(histname,histtitle,(Int_t)nintervals,0.,ranmax[i]-ranmin[i],tbins[i],-1.,1.); 
    }

}
//_______________________________________________________________
void DchTrackManagerSim::DchAlignmentData(const DchTrackCand *dchTrCand1, const DchTrackCand *dchTrCand2, TClonesArray* &fDchTrCand1Align, TClonesArray* &fDchTrCand2Align, UInt_t &eventNum1track, const TMatrixD C0, const UShort_t Iter, Double_t &sumDeltaPhi) {

Bool_t sametrack;
Double_t delPhi;

Int_t ntracks1=dchTrCand1->fDchTrackCand->GetEntriesFast();
Int_t ntracks2=dchTrCand2->fDchTrackCand->GetEntriesFast();

//cout<<"ntracks1 = "<<ntracks1<<endl;
if(ntracks1==1&&ntracks2==1){
  TClonesArray &frDchTrackCand1 = *(dchTrCand1->fDchTrackCand);
  TClonesArray &frDchTrackCand2 = *(dchTrCand2->fDchTrackCand);
  TClonesArray &ffDchTrCand1Align = *fDchTrCand1Align;
  TClonesArray &ffDchTrCand2Align = *fDchTrCand2Align;

  TMatrix dch1Mat  = *((TMatrix*) frDchTrackCand1.UncheckedAt(0));
  TMatrix dch2Mat  = *((TMatrix*) frDchTrackCand2.UncheckedAt(0));
  //cout<<"eventNum1track = "<<eventNum1track<<endl; 
  if(Iter>0){
   sametrack=TracksDirectionTest(dch1Mat,dch2Mat,C0,delPhi);
   //if(Iter>1)cout<<"Iter = "<<Iter<<" "<<"meanDeltaPhi = "<<meanDeltaPhi<<endl;
   if(sametrack){
    new (ffDchTrCand1Align[eventNum1track]) TMatrix(dch1Mat);  
    new (ffDchTrCand2Align[eventNum1track]) TMatrix(dch2Mat);  
    eventNum1track++; 
    sumDeltaPhi+=delPhi;
   }  
  }else{
   new (ffDchTrCand1Align[eventNum1track]) TMatrix(dch1Mat);  
   new (ffDchTrCand2Align[eventNum1track]) TMatrix(dch2Mat);  
   eventNum1track++;
  } 

}//if ntracks

}
//_______________________________________________________________
Bool_t DchTrackManagerSim::TracksDirectionTest(const TMatrix dch1Mat, const TMatrix dch2Mat,const TMatrixD C0, Double_t &delPhi) {

Bool_t delPhiTest;

XYZVector XYZ1(DCH1_Xpos[RunTypeToNumber(runType)], DCH1_Ypos[RunTypeToNumber(runType)],DCH1_Zpos[RunTypeToNumber(runType)]);

XYZPoint tr1pl0Point(dch1Mat(0,0),dch1Mat(0,1),dch1Mat(0,2));
XYZPoint tr1pl3Point(dch1Mat(3,0),dch1Mat(3,1),dch1Mat(3,2));
XYZPoint tr2pl0Point(dch2Mat(0,0),dch2Mat(0,1),dch2Mat(0,2));
XYZPoint tr2pl3Point(dch2Mat(3,0),dch2Mat(3,1),dch2Mat(3,2));

XYZPoint tr1pl0Point1=tr1pl0Point-XYZ1;
XYZPoint tr1pl3Point1=tr1pl3Point-XYZ1;
XYZPoint tr2pl0Point1=tr2pl0Point-XYZ1;
XYZPoint tr2pl3Point1=tr2pl3Point-XYZ1;

TVector3 misaligned0(tr2pl0Point1.X(),tr2pl0Point1.Y(),tr2pl0Point1.Z());
TVector3 misaligned3(tr2pl3Point1.X(),tr2pl3Point1.Y(),tr2pl3Point1.Z());

TVector3 tr1pl0(tr1pl0Point1.X(),tr1pl0Point1.Y(),tr1pl0Point1.Z());
TVector3 tr1pl3(tr1pl3Point1.X(),tr1pl3Point1.Y(),tr1pl3Point1.Z());
TVector3 tr2pl0=C0*misaligned0;
TVector3 tr2pl3=C0*misaligned3;

TVector3 dif1=tr1pl3-tr1pl0;
TVector3 dif2=tr2pl3-tr2pl0;

//cout<<"tracks angles"<<endl;
//Double_t delphi=dif1.DeltaPhi(dif2);
delPhi=fabs(dif1.DeltaPhi(dif2));
cout<<"tracks delta phi = "<<delPhi<<endl;
if(iter=1){
 delPhiTest = (fabs(delPhi)<0.5?true:false);
}else if(iter>1){
 delPhiTest = (fabs(delPhi)<5.*meanDeltaPhi?true:false);
}
//Bool_t delPhiTest = (fabs(delPhi)<0.5?true:false);
//Bool_t delPhiTest = true;
cout<<"delPhi= "<<delPhi<<endl;
return delPhiTest;

}
//-------------------------------------------------------------------------------------------------------------------------
UShort_t DchTrackManagerSim::RunTypeToNumber(TString runType){

UShort_t j;

           if(runType=="run1"){j=0;}
           else if(runType=="run2"){j=1;}
           else if(runType=="run3"){j=2;}
           else{cout<<"run type not in the list!"<<endl; 
                cout<<" run type = "<<runType<<endl;}
return j;

}
//_______________________________________________________________
void DchTrackManagerSim::DchTrackMerger(DchTrackCand *dchTrCand1,DchTrackCand *dchTrCand2, const TMatrixD C0) {

Double_t parFitL[4];
Double_t resY0,resX1,resU,resV;
Double_t res1,res2,resx,resy;
Double_t resX0rad,resY0rad,resX1rad,resY1rad,res0rad,res1rad;
Double_t fullLength,r1,r2;

Int_t ntracks1=dchTrCand1->fDchTrackCand->GetEntriesFast();
Int_t ntracks2=dchTrCand2->fDchTrackCand->GetEntriesFast();

//cout<<"ntracks1 = "<<ntracks1<<", ntracks2 = "<<ntracks2<<endl;

if(ntracks1==1&&ntracks2==1){

//TClonesArray &frDchTrackCand = *(dchTrCand1->fDchTrackCand);
//frDchTrackCand.Dump();
  for (Int_t i = 0; i < ntracks1; ++i) {
    //TMatrix* dch1Mat  = (TMatrix*) frDchTrackCand.UncheckedAt(i);
    TMatrix* dch1Mat  = (TMatrix*) dchTrCand1->fDchTrackCand->At(i);
    //dch1Mat->Print(); 
    float* matArr=dch1Mat->GetMatrixArray(); 
    TGraph2DErrors* dchtrgr = new TGraph2DErrors();
    UInt_t kl=0;
    //for (UShort_t k = 0; k < 2; ++k) {
    for (UShort_t k = 0; k < 4; ++k) {
      //cout<<"1 "<<Matrix(matArr,k,0)<<" "<<Matrix(matArr,k,1)<<" "<<Matrix(matArr,k,2)<<" "<<Matrix(matArr,k,3)<<" "<<Matrix(matArr,k,4)<<endl;
      dchtrgr->SetPoint(kl,Matrix(matArr,k,0),Matrix(matArr,k,1),Matrix(matArr,k,2));
      dchtrgr->SetPointError(kl,resolution,resolution,0.); 
      kl++;
    }
    //dchMat->Print(); 
    for (Int_t j = 0; j < ntracks2; ++j) {
      TMatrix* dch2Mat  = (TMatrix*) dchTrCand2->fDchTrackCand->At(j);
      //dch2Mat->Print(); 
      float* matArr2=dch2Mat->GetMatrixArray(); 
      for (UShort_t k = 0; k < 4; ++k) {
        dchtrgr->SetPoint(kl,Matrix(matArr2,k,0),Matrix(matArr2,k,1),Matrix(matArr2,k,2));
        dchtrgr->SetPointError(kl,resolution,resolution,0.); 
        kl++;
      }
      DchLinearTrackFitter(dchtrgr, parFitL, chi2linfit);
      trackAngleX->Fill(TMath::RadToDeg()*atan(parFitL[1]));
      trackAngleY->Fill(TMath::RadToDeg()*atan(parFitL[3]));
      for (UShort_t k = 0; k < 4; ++k) {
       if(k==0){
         res1=abs(parFitL[2] + parFitL[3]*Matrix(matArr,k,2)-Matrix(matArr,k,5))-abs(Matrix(matArr,k,1)-Matrix(matArr,k,5));
         residVsTDCy->Fill(Matrix(matArr,k,3),res1);
         ResidVsTDCy[0]->Fill(Matrix(matArr,k,3),res1);
         residVsTDC->Fill(Matrix(matArr,k,3),res1);  
         ResidVsTDC[0]->Fill(Matrix(matArr,k,3),res1);
         fullLength=sqrt(pow(Matrix(matArr2,k,6)-Matrix(matArr2,k,4),2.)+pow(Matrix(matArr2,k,7)-Matrix(matArr2,k,5),2.)); 
         r1=PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2),parFitL[2] + parFitL[3]*Matrix(matArr2,k,2),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,6)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,7)-Matrix(matArr2,k,5))/fullLength, 0.); 
         r2=PointLineDistance(Matrix(matArr2,k,0), Matrix(matArr2,k,1),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,6)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,7)-Matrix(matArr2,k,5))/fullLength, 0.); 
         residVsTDCy->Fill(Matrix(matArr2,k,3),r1-r2);
         ResidVsTDCy[1]->Fill(Matrix(matArr2,k,3),r1-r2);
         residVsTDC->Fill(Matrix(matArr2,k,3),r1-r2);
         ResidVsTDC[1]->Fill(Matrix(matArr2,k,3),r1-r2);
       }else if(k==1){
         res1=abs(parFitL[0] + parFitL[1]*Matrix(matArr,k,2)-Matrix(matArr,k,4))-abs(Matrix(matArr,k,0)-Matrix(matArr,k,4));
         residVsTDCx->Fill(Matrix(matArr,k,3),res1);
         ResidVsTDCx[0]->Fill(Matrix(matArr,k,3),res1);
         residVsTDC->Fill(Matrix(matArr,k,3),res1);
         ResidVsTDC[0]->Fill(Matrix(matArr,k,3),res1);
         fullLength=sqrt(pow(Matrix(matArr2,k,6)-Matrix(matArr2,k,4),2.)+pow(Matrix(matArr2,k,7)-Matrix(matArr2,k,5),2.)); 
         r1=PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2),parFitL[2] + parFitL[3]*Matrix(matArr2,k,2),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,6)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,7)-Matrix(matArr2,k,5))/fullLength, 0.); 
         r2=PointLineDistance(Matrix(matArr2,k,0), Matrix(matArr2,k,1),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,6)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,7)-Matrix(matArr2,k,5))/fullLength, 0.); 
         residVsTDCx->Fill(Matrix(matArr2,k,3),r1-r2);
         ResidVsTDCx[1]->Fill(Matrix(matArr2,k,3),r1-r2);
         residVsTDC->Fill(Matrix(matArr2,k,3),r1-r2);
         ResidVsTDC[1]->Fill(Matrix(matArr2,k,3),r1-r2);
       }else if(k==2||k==3){
         r1=PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr,k,2),parFitL[2] + parFitL[3]*Matrix(matArr,k,2),0., Matrix(matArr,k,4), Matrix(matArr,k,5), 0., dirCosLayerX[2*k], dirCosLayerY[2*k],0.); 
         r2=PointLineDistance(Matrix(matArr,k,0), Matrix(matArr,k,1),0., Matrix(matArr,k,4), Matrix(matArr,k,5), 0., dirCosLayerX[2*k], dirCosLayerY[2*k],0.); 
         if(k==2){
          residVsTDCu->Fill(Matrix(matArr,k,3),r1-r2);
          ResidVsTDCu[0]->Fill(Matrix(matArr,k,3),r1-r2);
          residVsTDC->Fill(Matrix(matArr,k,3),r1-r2);
          ResidVsTDC[0]->Fill(Matrix(matArr,k,3),r1-r2);
         }
         if(k==3){
          residVsTDCv->Fill(Matrix(matArr,k,3),r1-r2);
          ResidVsTDCv[0]->Fill(Matrix(matArr,k,3),r1-r2);
          residVsTDC->Fill(Matrix(matArr,k,3),r1-r2);
          ResidVsTDC[0]->Fill(Matrix(matArr,k,3),r1-r2);
         } 
         fullLength=sqrt(pow(Matrix(matArr2,k,6)-Matrix(matArr2,k,4),2.)+pow(Matrix(matArr2,k,7)-Matrix(matArr2,k,5),2.)); 
         r1=PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2),parFitL[2] + parFitL[3]*Matrix(matArr2,k,2),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,6)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,7)-Matrix(matArr2,k,5))/fullLength, 0.); 
         r2=PointLineDistance(Matrix(matArr2,k,0), Matrix(matArr2,k,1),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,6)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,7)-Matrix(matArr2,k,5))/fullLength, 0.); 
         if(k==2){
          residVsTDCu->Fill(Matrix(matArr2,k,3),r1-r2);
          ResidVsTDCu[1]->Fill(Matrix(matArr2,k,3),r1-r2);
          residVsTDC->Fill(Matrix(matArr2,k,3),r1-r2);
          ResidVsTDC[1]->Fill(Matrix(matArr2,k,3),r1-r2);
         }
         if(k==3){
          residVsTDCv->Fill(Matrix(matArr2,k,3),r1-r2);
          ResidVsTDCv[1]->Fill(Matrix(matArr2,k,3),r1-r2);
          residVsTDC->Fill(Matrix(matArr2,k,3),r1-r2);
          ResidVsTDC[1]->Fill(Matrix(matArr2,k,3),r1-r2);
         }
       }
      }//dch planes
    }//nentries2
    delete dchtrgr;
  }//nentries1
}//if ntracks

}
//_______________________________________________________________
//void DchTrackManagerSim::FinishDch(UShort_t fDchNum, TGraph *calib[fDchNum-1]) {
void DchTrackManagerSim::FinishDch(TGraph *calibr[],TH1D* &hResidRMSvsIter, TH1D* &hResidRMSvsIter0, TH1D* &hResidRMSvsIter1, UShort_t Iter, TString datatype) {

//cout<<"RMS = "<<residVsTDC->GetRMS(2)<<" "<<iter<<endl;
Double_t RMS=residVsTDC->GetRMS(2);
hResidRMSvsIter->Fill(Iter,RMS);
RMS=ResidVsTDC[0]->GetRMS(2);
hResidRMSvsIter0->Fill(Iter,RMS);
RMS=ResidVsTDC[1]->GetRMS(2);
hResidRMSvsIter1->Fill(Iter,RMS);
TString str;
str.Form("%d",Iter);
//TString fResiduals = TString("dchresid_2dch_sim") + str + TString(".root");
TString fResiduals = TString("dchresidsim") + str + TString(".root");
TFile *fdch  = new TFile(fResiduals, "RECREATE");
residVsTDCy->Write();
residVsTDCx->Write();
residVsTDCu->Write();
residVsTDCv->Write();
residVsTDC->Write();
chi2linfit->Write();
trackAngleX->Write();
trackAngleY->Write();
//calibr->Write();
hResidRMSvsIter->Write();
hResidRMSvsIter0->Write();
hResidRMSvsIter1->Write();
for (Int_t i = 0; i < 2; ++i) {
 ResidVsTDCy[i]->Write();
 ResidVsTDCx[i]->Write();
 ResidVsTDCu[i]->Write();
 ResidVsTDCv[i]->Write();
 ResidVsTDC[i]->Write();
 calibr[i]->Write();
 delete ResidVsTDCy[i];delete ResidVsTDCx[i];delete ResidVsTDCu[i];delete ResidVsTDCv[i];delete ResidVsTDC[i];delete calibr[i];
}

delete chi2linfit;delete trackAngleX;delete residVsTDCy;
delete residVsTDCx;delete residVsTDCu; delete residVsTDCv;
delete residVsTDC;
/*ScalarD *Ranmin;
Ranmin->SetSV(rtcalibrange->X());
Ranmin->SetName("Ranmin");
cout<<"RANMIN1 = "<<rtcalibrange->X()<<endl;
cout<<"RANMIN2 = "<<Ranmin->GetSV()<<endl;
Ranmin->Write();
*/

//fdch->ls();
fdch->Close();

}
//_______________________________________________________________
/*void DchTrackManagerSim::SetDchTrackHits(UInt_t n, Double_t x0, Double_t y0, Double_t z0, Double_t x3, Double_t y3, Double_t z3) {

TClonesArray &ffDchTrackManagerSim = *fDchTrackManagerSim;
TMatrix hits(2,3);
hits(0,0)=x0;hits(0,1)=y0;hits(0,2)=z0;
hits(1,0)=x3;hits(1,1)=y3;hits(1,2)=z3;
//hits.Print();
new (ffDchTrackManagerSim[n]) TMatrix(hits);

}

//________________________________________________________________
void DchTrackManagerSim::SetDchTrackManagerSimNumber(UInt_t n){

// setting number fo Dch track candidate

 nTrCand=n;

}
//________________________________________________________________
UInt_t DchTrackManagerSim::GetDchTrackManagerSimNumber(){

// returns number of Dch track candidate

 return nTrCand;

}*/

//____________________________________________________________________
double DchTrackManagerSim::Matrix(float* matArr, const UInt_t k, const UInt_t l) {

return  *(matArr+(k*ncols)+l);

}

//____________________________________________________________________
void DchTrackManagerSim::DchLinearTrackFitter(TGraph2DErrors* dchtrgr, Double_t *parFitL, TH1D* &chi2linfit) {

//Fitting of a TGraph2D with a 3D straight line using TVirtualFitter
   //gStyle->SetOptStat(0);
   //gStyle->SetOptFit();
   //
   TVirtualFitter *min = TVirtualFitter::Fitter(0,4);
   min->SetObjectFit(dchtrgr);
   //min->SetFCN((void (*)(Int_t&, Double_t*, Double_t&, Double_t*, Int_t)) SumDistance2);
   min->SetFCN(SumDistance2);
  
   Double_t arglist[10];
   arglist[0] = -1;
   //arglist[0] = 3;
   min->ExecuteCommand("SET PRINT",arglist,1);
   Int_t ierr=0; 
   min->ExecuteCommand("SET NOWarnings",0,ierr); 
  
 
   Double_t pStart[4];
   Double_t* fX=dchtrgr->GetX();  
   Double_t* fY=dchtrgr->GetY();
   Double_t* fZ=dchtrgr->GetZ();
   Double_t x1=*fX,x2=*(fX+1);  
   Double_t y1=*fY,y2=*(fY+1); 
   Double_t z1=*fZ,z2=*(fZ+1); 
   Double_t tgx=(x2-x1)/(z2-z1); 
   Double_t tgy=(y2-y1)/(z2-z1); 
   Double_t delX=tgx*z2,delY=tgy*z2;
   Double_t ax=x2-delX,ay=y2-delY; 
   pStart[0] = ax;
   pStart[1] = tgx;
   pStart[2] = ay;
   pStart[3] = tgy;
   if(checkDTM)cout<<" pStart[0] = "<<pStart[0]<<" pStart[1] = "<<pStart[1]<<" pStart[2] = "<<pStart[2]<<" pStart[3] = "<<pStart[3]<<endl;

   min->SetParameter(0,"x0",pStart[0],0.01,0,0);
   min->SetParameter(1,"tgx",pStart[1],0.01,0,0);
   min->SetParameter(2,"y0",pStart[2],0.01,0,0);
   min->SetParameter(3,"tgy",pStart[3],0.01,0,0);
    
   arglist[0] = 1000; // number of function calls 
   arglist[1] = 0.001; // tolerance 
   min->ExecuteCommand("MIGRAD",arglist,2);
   //min->ExecuteCommand("MINO",arglist,2);

  //if (minos) min->ExecuteCommand("MINOS",arglist,0);
   Int_t nvpar,nparx; 
   Double_t amin,edm,errdef;
   min->GetStats(amin,edm,errdef,nvpar,nparx);
   if(checkDTM)cout<<"line fit"<<endl;
   //min->PrintResults(1,amin);
   //cout<<"chi2= "<<chi2<<endl;
   if(checkDTM)cout<<"chi2 = "<<amin<<" "<<edm<<" "<<errdef<<" "<<nvpar<<" "<<nparx<<endl;
   chi2linfit->Fill(amin);

   // get fit parameters
   //Double_t parFit[4];
   for (Int_t i = 0; i < 4; ++i) {
      parFitL[i] = min->GetParameter(i);
   }
}
//____________________________________________________________________
//---------------------------------------------------------------------------
Double_t DchTrackManagerSim::PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz){

// x0,y0,z0 - point coordinates
// x,y,z - point on line coordinates

    TVector3 pos0(x0,y0,z0);
    TVector3 pos(x,y,z);
    TVector3 dir(dircosx,dircosy,dircosz);
    TVector3 dif=pos-pos0;
    
    Double_t distance = ((dif.Cross(dir)).Mag())/dir.Mag();
    
    return distance;

}
//____________________________________________________________________
