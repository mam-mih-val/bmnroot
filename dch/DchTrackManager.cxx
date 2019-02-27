#include "DchTrackManager.h"
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
#include "TF1.h"

#include "Math/Point3D.h"
#include "Math/Vector3D.h"

//#include "testfunction.h"

using namespace ROOT::Math;

// constants definition
 const Double_t DchTrackManager::sqrt2=sqrt(2.);
 const Double_t DchTrackManager::sqrt2half=sqrt(2.)/2.;

//
//ClassImp(DchTrackCand)
//ClassImp(DchTrackManager)


static Bool_t checkFit=false;
 //Bool_t checkFit=true;
 //const Double_t DchTrackManager::resolution =  15e-3; // 150 mkm  ==  0.015 cm
 //const UInt_t DchTrackManager::nintervals=40;// number of intervals along calibration time axis
 const UInt_t DchTrackManager::ncols=11; //number of matrix columns

static Double_t distance2(Double_t x,Double_t y,Double_t z, Double_t *p);
static void SumDistance2(Int_t &, Double_t *, Double_t & sum, Double_t * par, Int_t ); 
static  void line(double t, double *p, double &x, double &y, double &z); 


// define the parameteric line equation 
static void line(double t, double *p, double &x, double &y, double &z) { 
//void DchTrackManager::line(double t, double *p, double &x, double &y, double &z) { 
     // a parameteric line is defined from 6 parameters but 4 are independent
     // x0,y0,z0,z1,y1,z1 which are the coordinates of two points on the line
     // can choose z0 = 0 if line not parallel to x-y plane and z1 = 1; 
     x = p[0] + p[1]*t; 
     y = p[2] + p[3]*t;
     z = t; 
} 
// calculate distance line-point 

static Double_t distance2(Double_t x,Double_t y,Double_t z,Double_t *p) { 
//Double_t DchTrackManager::distance2(Double_t x,Double_t y,Double_t z,Double_t *p) { 
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
//void DchTrackManager::SumDistance2(Int_t &, Double_t *, Double_t & sum, Double_t * par, Int_t ) { 

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
//void DchTrackManager::myfcn(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) {
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
//void DchTrackManager::myfcn2(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) {
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
ClassImp(DchTrackManager)
//_________________________________________________________________
//DchTrackManager::DchTrackManager(const DchTrackCand *DchTrCand1,const DchTrackCand *DchTrCand2):checkDTM(true) {
DchTrackManager::DchTrackManager(UShort_t runperiod):checkDTM(true) {

     runPeriod = runperiod;
     //DchTrackCand* DchTrCand1;
     //DchTrackCand* DchTrCand2
     //DchTrackCandReader(DchTrackCand* &DchTrCand1,DchTrackCand* &DchTrCand2);
     //dchTrCand1=DchTrCand1;
     //dchTrCand2=DchTrCand2;
     //DchTrackCandReader(dchTrCand1,dchTrCand2);

}
//_________________________________________________________________
DchTrackManager::~DchTrackManager() {

  //delete dchTrCand1;
  //delete dchTrCand2;
}
//_________________________________________________________________
void DchTrackManager::InitDch(const UShort_t Iter, const UShort_t Itermax, const Double_t ranmin[][numLayers_half], const Double_t ranmax[][numLayers_half], const Double_t ranmin_cham[], const Double_t ranmax_cham[], const Double_t Resolution, const UInt_t Nintervals, TList &hList, const Bool_t fDoCheck, const TString outDirectory, const TString residualsFileName_old, const Option_t* Opt) {

 iter=Iter; 
 itermax=Itermax; 
 resolution=Resolution;
 nintervals=Nintervals;  
 opt=Opt; 
 doCheck=fDoCheck;
 HistoBookDch(ranmin,ranmax,ranmin_cham,ranmax_cham, hList, outDirectory, residualsFileName_old); 
 //magfield=magField;

}
//_______________________________________________________________
/*void DchTrackManager::DchTrackCandReader(const UInt_t numevents) {

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
void DchTrackManager::HistoBookDch(const Double_t ranmin[][numLayers_half], const Double_t ranmax[][numLayers_half], const Double_t ranmin_cham[], const Double_t ranmax_cham[], TList &hList, const TString outDirectory, const TString residualsFileName_old) {
//void HistoBookDch(Double_t zLayer[][numSubmodules][numLayersInRow], Int_t numModules, Int_t numSubmodules, Int_t numLayersInRow) {
/*cout<<"ranmin1 = "<<ranmin[0]<<", ranmax1 = "<<ranmax[0]<<endl;
cout<<"ranmin2 = "<<ranmin[1]<<", ranmax2 = "<<ranmax[1]<<endl;
const Double_t ranmin0=0.5*(ranmin[0]+ranmin[1]);
const Double_t ranmax0=0.5*(ranmax[0]+ranmax[1]);
const Int_t tbins0=Int_t(ranmax0-ranmin0);
const Int_t tbins[2] = {Int_t(ranmax[0]-ranmin[0]),Int_t(ranmax[1]-ranmin[1])};
*/
 const Double_t ranmin00=0.5*(ranmin_cham[0]+ranmin_cham[1]);
 const Double_t ranmax00=0.5*(ranmax_cham[0]+ranmax_cham[1]);
 const Int_t tbins00=Int_t(ranmax00-ranmin00);
 const Int_t tbinss[numChambers]={Int_t(ranmax_cham[0]-ranmin_cham[0]),Int_t(ranmax_cham[1]-ranmin_cham[1])};
 Double_t ranmin0[numLayers_half];
 Double_t ranmax0[numLayers_half];
 Int_t tbins0[numLayers_half];
 Int_t tbins[numChambers][numLayers_half];
 tbins0[numLayers_half];
for (UShort_t j = 0; j < numLayers_half; j++) {
 cout<<"ranmin1 = "<<ranmin[0][j]<<", ranmax1 = "<<ranmax[0][j]<<endl;
 cout<<"ranmin2 = "<<ranmin[1][j]<<", ranmax2 = "<<ranmax[1][j]<<endl;
 tbins[0][j] = Int_t(ranmax[0][j]-ranmin[0][j]);
 tbins[1][j] = Int_t(ranmax[1][j]-ranmin[1][j]);
 ranmin0[j]=0.5*(ranmin[0][j]+ranmin[1][j]);
 ranmax0[j]=0.5*(ranmax[0][j]+ranmax[1][j]);
 tbins0[j] = Int_t(ranmax0[j]-ranmin0[j]);
}

   if(iter==0){ 
    hResidRMSvsIter = new TH1F("hResidRMSvsIter", "RMS of residuals vs iteration number", Int_t(itermax)+1, 0., Double_t(itermax+1));
    //hResidRMSvsIter = new TH1F("hResidRMSvsIter", "Gaussian sigma of residuals vs iteration number", Int_t(itermax), 0., Double_t(itermax));
    //hResidRMSvsIter->Sumw2(); 
    hResidRMSvsIter0 = new TH1F("hResidRMSvsIter0", "RMS of residuals vs iteration number for DCH1", Int_t(itermax)+1, 0., Double_t(itermax+1));
    //hResidRMSvsIter0 = new TH1F("hResidRMSvsIter0", "Gaussian sigma of residuals vs iteration number for DCH1", Int_t(itermax), 0., Double_t(itermax));
    hResidRMSvsIter1 = new TH1F("hResidRMSvsIter1", "RMS of residuals vs iteration number for DCH2", Int_t(itermax)+1, 0., Double_t(itermax+1));
    //hResidRMSvsIter1 = new TH1F("hResidRMSvsIter1", "Gaussian sigma of residuals vs iteration number for DCH2", Int_t(itermax), 0., Double_t(itermax));
   }else{
    TString inPut=outDirectory+residualsFileName_old;
    TFile resid(inPut,"read");
    //TH1F *hist=(TH1F*)resid.Get("hResidRMSvsIter");
    hResidRMSvsIter=(TH1F*)resid.Get("hResidRMSvsIter");
    //hResidRMSvsIter = (TH1F*) hist->Clone();
    hResidRMSvsIter->SetDirectory(0);
    hResidRMSvsIter0=(TH1F*)resid.Get("hResidRMSvsIter0");
    hResidRMSvsIter0->SetDirectory(0);
    hResidRMSvsIter1=(TH1F*)resid.Get("hResidRMSvsIter1");
    hResidRMSvsIter1->SetDirectory(0);
    resid.Close();
   }
    TString histtitle,histtitle1,histtitle2=" in both DCHs";
    TString histtitle3,histtitle4,histtitle5;
    TString histtitle_plane,histtitle6;
    TString histtitleDCA,histtitle1DCA;
    chi2linfitHist = new TH1D("chi2linfitiHist","chi2 of linear fit",100,0.,10.); 
    chi2linfitHist->SetDirectory(0); hList.Add(chi2linfitHist);
    trackAngleXbf1 = new TH1D("trackAngleXbf1","track angle along x in DCH1 before fitting",1800,-90.,90.);
    trackAngleXbf1->SetDirectory(0); //hList.Add(trackAngleXbf1);
    trackAngleYbf1 = new TH1D("trackAngleYbf1","track angle along y in DCH1 before fitting",1800,-90.,90.);
    trackAngleYbf1->SetDirectory(0); //hList.Add(trackAngleYbf1);
    trackAngleXbf2 = new TH1D("trackAngleXbf2","track angle along x in DCH2 before fitting",1800,-90.,90.);
    trackAngleXbf2->SetDirectory(0); //hList.Add(trackAngleXbf2);
    trackAngleYbf2 = new TH1D("trackAngleYbf2","track angle along y in DCH2 before fitting",1800,-90.,90.);
    trackAngleYbf2->SetDirectory(0); //hList.Add(trackAngleYbf2);
    trackAngleXaf = new TH1D("trackAngleXaf","track angle along x after fitting",1800,-90.,90.);
    trackAngleXaf->SetDirectory(0); //hList.Add(trackAngleXaf);
    trackAngleYaf = new TH1D("trackAngleYaf","track angle along y after fitting",1800,-90.,90.);
    trackAngleYaf->SetDirectory(0); //hList.Add(trackAngleYaf);
    trckAngX2vs1 = new TH2D("trckAngX2vs1","track angles along x in DCH2 vs DCH1 before fitting",1800,-90.,90.,1800,-90.,90.);
    trckAngX2vs1->SetDirectory(0); hList.Add(trckAngX2vs1);
    trckAngY2vs1 = new TH2D("trckAngY2vs1","track angles along y in DCH2 vs DCH1 before fitting",1800,-90.,90.,1800,-90.,90.);
    trckAngY2vs1->SetDirectory(0); hList.Add(trckAngY2vs1);
    //trackAngleX = new TH1D("trackAngleX","track angle along x",1800,-90.,90.); 
    //trackAngleY = new TH1D("trackAngleY","track angle along y",1800,-90.,90.);
    
//if(strcmp(opt,"reconstruction")==0){ 
    if(doCheck){
     distDch1 = new TH1F("distDch1","distance of reconstructed and extrapolated tracks in DCH1",100,0.,100.); 
     distDch1->SetDirectory(0);hList.Add(distDch1);
     distDch2 = new TH1F("distDch2","distance of reconstructed and extrapolated tracks in DCH2",100,0.,100.); 
     distDch2->SetDirectory(0);hList.Add(distDch2);
//}
     distxDch1 = new TH1F("distxDch1","x distance of reconstructed and extrapolated tracks in DCH1",200,-100.,100.); 
     distxDch1->SetDirectory(0);hList.Add(distxDch1);
     distxDch2 = new TH1F("distxDch2","x distance of reconstructed and extrapolated tracks in DCH2",200,-100.,100.); 
     distxDch2->SetDirectory(0);hList.Add(distxDch2);
     distyDch1 = new TH1F("distyDch1","y distance of reconstructed and extrapolated tracks in DCH1",200,-100.,100.); 
     distyDch1->SetDirectory(0);hList.Add(distyDch1);
     distyDch2 = new TH1F("distyDch2","y distance of reconstructed and extrapolated tracks in DCH2",200,-100.,100.); 
     distyDch2->SetDirectory(0);hList.Add(distyDch2);
     diffAngleTracks = new TH1F("diffAngleTracks","angle between unaligned DCH tracks",100,0.,100.);
     diffAngleTracks->SetDirectory(0);hList.Add(diffAngleTracks);
     diffAngleTracksX = new TH1F("diffAngleTracksX","angle between unaligned DCH tracks in XZ plane",200,-100.,100.);
     diffAngleTracksX->SetDirectory(0);hList.Add(diffAngleTracksX);
     diffAngleTracksY = new TH1F("diffAngleTracksY","angle between unaligned DCH tracks in YZ plane",200,-100.,100.);
     diffAngleTracksY->SetDirectory(0);hList.Add(diffAngleTracksY);
    }
    histtitle1="X hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    //residVsTDCx = new TH2D("residVsTDCx",histtitle,(Int_t)nintervals,0.,ranmax0-ranmin0,tbins0,-1.,1.); 
    residVsTDCx = new TH2D("residVsTDCx",histtitle,(Int_t)nintervals,0.,ranmax0[1]-ranmin0[1],tbins0[1],-1.,1.); 
    histtitle1="Y hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    //residVsTDCy = new TH2D("residVsTDCy",histtitle,(Int_t)nintervals,0.,ranmax0-ranmin0,tbins0,-1.,1.); 
    const Int_t nResolBins=40;
    residVsTDCy = new TH2D("residVsTDCy",histtitle,tbins0[0],0.,ranmax0[0]-ranmin0[0],nResolBins,-1.,1.); 
    histtitle1="U hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    residVsTDCu = new TH2D("residVsTDCu",histtitle,tbins0[2],0.,ranmax0[2]-ranmin0[2],nResolBins,-1.,1.); 
    histtitle1="V hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    residVsTDCv = new TH2D("residVsTDCv",histtitle,tbins0[3],0.,ranmax0[3]-ranmin0[3],nResolBins,-1.,1.); 
    histtitle1="hit residuals vs TDC";
    histtitle=histtitle1+histtitle2;
    residVsTDC = new TH2D("residVsTDC",histtitle,tbins00,0.,ranmax00-ranmin00,nResolBins,-1.,1.); 
    TString str,histname,str2;
    TString histnameDCAm,histnameDCAf;
    histtitle2=" in DCH"; 
    histtitle3=" of DCH"; 
    histtitle4="measured "; 
    histtitle5="fitted "; 
    for (Int_t i = 0; i < 2; ++i) {
     str.Form("%u",i+1);
     str2.Form("%u",i);
     histname=TString("ResidVsTDCy")+str;
     histtitle1="Y hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDCy[i] = new TH2D(histname,histtitle,tbins[i][1],0.,ranmax[i][1]-ranmin[i][1],nResolBins,-1.,1.); 
     histname=TString("ResidVsTDCy_x")+str;
     histtitle6=" (X plane)"; 
     histtitle_plane=histtitle+histtitle6;
     ResidVsTDCy_x[i] = new TH2D(histname,histtitle_plane,tbins[i][0],0.,ranmax[i][0]-ranmin[i][0],nResolBins,-1.,1.); 
     histname=TString("ResidVsTDCy_u")+str;
     histtitle6=" (U plane)"; 
     histtitle_plane=histtitle+histtitle6;
     ResidVsTDCy_u[i] = new TH2D(histname,histtitle_plane,tbins[i][2],0.,ranmax[i][2]-ranmin[i][2],nResolBins,-1.,1.); 
     histname=TString("ResidVsTDCy_v")+str;
     histtitle6=" (V plane)"; 
     histtitle_plane=histtitle+histtitle6;
     ResidVsTDCy_v[i] = new TH2D(histname,histtitle_plane,tbins[i][3],0.,ranmax[i][3]-ranmin[i][3],nResolBins,-1.,1.); 
     if(checkDTM){
      histnameDCAm=TString("mDCAy")+str;
      histtitle1DCA="DCA in plane 1";
      histtitleDCA=histtitle1DCA+histtitle3+str;
      mDCAy[i] = new TH1D(histnameDCAm,histtitle4+histtitleDCA,100,0.,1.); 
      //mDCAy[i]->SetDirectory(0); hList.Add(mDCAy[i]);
      histnameDCAf=TString("fDCAy")+str;
      fDCAy[i] = new TH1D(histnameDCAf,histtitle5+histtitleDCA,100,0.,1.); 
      //fDCAy[i]->SetDirectory(0); hList.Add(fDCAy[i]);
     }
     histname=TString("ResidVsTDCx")+str;
     histtitle1="X hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDCx[i] = new TH2D(histname,histtitle,tbins[i][0],0.,ranmax[i][0]-ranmin[i][0],nResolBins,-1.,1.); 
     histname=TString("ResidVsTDCx_y")+str;
     histtitle6=" (Y plane)"; 
     histtitle_plane=histtitle+histtitle6;
     ResidVsTDCx_y[i] = new TH2D(histname,histtitle_plane,tbins[i][1],0.,ranmax[i][1]-ranmin[i][1],nResolBins,-1.,1.); 
     histname=TString("ResidVsTDCx_u")+str;
     histtitle6=" (U plane)"; 
     histtitle_plane=histtitle+histtitle6;
     ResidVsTDCx_u[i] = new TH2D(histname,histtitle_plane,tbins[i][2],0.,ranmax[i][2]-ranmin[i][2],nResolBins,-1.,1.); 
     histname=TString("ResidVsTDCx_v")+str;
     histtitle6=" (V plane)"; 
     histtitle_plane=histtitle+histtitle6;
     ResidVsTDCx_v[i] = new TH2D(histname,histtitle_plane,tbins[i][3],0.,ranmax[i][3]-ranmin[i][3],nResolBins,-1.,1.); 
     if(checkDTM){
      histnameDCAm=TString("mDCAx")+str;
      histtitle1DCA="DCA in plane 2";
      histtitleDCA=histtitle1DCA+histtitle3+str;
      mDCAx[i] = new TH1D(histnameDCAm,histtitle4+histtitleDCA,100,0.,1.); 
      mDCAx[i]->SetDirectory(0); hList.Add(mDCAx[i]);
      histnameDCAf=TString("fDCAx")+str;
      fDCAx[i] = new TH1D(histnameDCAf,histtitle5+histtitleDCA,100,0.,1.); 
      fDCAx[i]->SetDirectory(0); hList.Add(fDCAx[i]);
      mDCAy[i]->SetDirectory(0); hList.Add(mDCAy[i]);
      fDCAy[i]->SetDirectory(0); hList.Add(fDCAy[i]);
     }
     histname=TString("ResidVsTDCu")+str;
     histtitle1="U hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDCu[i] = new TH2D(histname,histtitle,tbins[i][2],0.,ranmax[i][2]-ranmin[i][2],nResolBins,-1.,1.); 
     if(checkDTM){
      histnameDCAm=TString("mDCAu")+str;
      histtitle1DCA="DCA in plane 3";
      histtitleDCA=histtitle1DCA+histtitle3+str;
      mDCAu[i] = new TH1D(histnameDCAm,histtitle4+histtitleDCA,100,0.,1.); 
      mDCAu[i]->SetDirectory(0); hList.Add(mDCAu[i]);
      histnameDCAf=TString("fDCAu")+str;
      fDCAu[i] = new TH1D(histnameDCAf,histtitle5+histtitleDCA,100,0.,1.); 
      fDCAu[i]->SetDirectory(0); hList.Add(fDCAu[i]);
     }
     histname=TString("ResidVsTDCv")+str;
     histtitle1="V hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDCv[i] = new TH2D(histname,histtitle,tbins[i][3],0.,ranmax[i][3]-ranmin[i][3],nResolBins,-1.,1.); 
     if(checkDTM){
      histnameDCAm=TString("mDCAv")+str;
      histtitle1DCA="DCA in plane 4";
      histtitleDCA=histtitle1DCA+histtitle3+str;
      mDCAv[i] = new TH1D(histnameDCAm,histtitle4+histtitleDCA,100,0.,1.); 
      mDCAv[i]->SetDirectory(0); hList.Add(mDCAv[i]);
      histnameDCAf=TString("fDCAv")+str;
      fDCAv[i] = new TH1D(histnameDCAf,histtitle5+histtitleDCA,100,0.,1.); 
      fDCAv[i]->SetDirectory(0); hList.Add(fDCAv[i]);
     }
     histname=TString("ResidVsTDC")+str;
     histtitle1="hit residuals vs TDC";
     histtitle=histtitle1+histtitle2+str;
     ResidVsTDC[i] = new TH2D(histname,histtitle,tbinss[i],0.,ranmax_cham[i]-ranmin_cham[i],nResolBins,-1.,1.); 
     if(checkDTM){
      for (UShort_t j = 0; j < 4; ++j) {
       TString str3;
       str3.Form("%u",j);
       TString hDeltaXname = TString("hDeltaXname") + str2 + str3;
       hDeltaX[i][j] = new TH1D(hDeltaXname, "x measured-fitted", 100, -1., 1.);
       hDeltaX[i][j]->SetDirectory(0); hList.Add(hDeltaX[i][j]);
       TString hDeltaYname = TString("hDeltaYname") + str2 + str3;
       hDeltaY[i][j] = new TH1D(hDeltaYname, "y measured-fitted", 100, -1., 1.);
       hDeltaY[i][j]->SetDirectory(0); hList.Add(hDeltaY[i][j]);
      }
     }
    }

}
//_______________________________________________________________
void DchTrackManager::DchAlignmentData(const DchTrackCand *dchTrCand1, const DchTrackCand *dchTrCand2, TClonesArray* &fDchTrCand1Align, TClonesArray* &fDchTrCand2Align, UInt_t &eventNum1track, const TMatrixD C0, const XYZVector XYZ1, Double_t &sumDeltaPhi) {

Bool_t sametrack;
Double_t delPhi;
UInt_t nAlignedTracksInEvent=0;
Float_t minTracksAngle=360.;
const Float_t minAngleCut=10.;
Int_t tr1ind,tr2ind;
Bool_t minAngleFound=false;

Int_t ntracks1=dchTrCand1->fDchTrackCand->GetEntriesFast();
Int_t ntracks2=dchTrCand2->fDchTrackCand->GetEntriesFast();
Bool_t oneTrackInBothDCHs=false;
Bool_t tracksInDCHs=false;

if(ntracks1>0&&ntracks2>0){
 tracksInDCHs=true;
 if(ntracks1==1&&ntracks2==1)oneTrackInBothDCHs=true;
}
 
//cout<<"ntracks1,ntracks2 = "<<ntracks1<<", "<<ntracks2<<endl;

//if(ntracks1==1&&ntracks2==1){
if(tracksInDCHs){
 TClonesArray &ffDchTrCand1Align = *fDchTrCand1Align;
 TClonesArray &ffDchTrCand2Align = *fDchTrCand2Align;
 if(oneTrackInBothDCHs){
  TClonesArray &frDchTrackCand1 = *(dchTrCand1->fDchTrackCand);
  TClonesArray &frDchTrackCand2 = *(dchTrCand2->fDchTrackCand);

  TMatrix dch1Mat  = *((TMatrix*) frDchTrackCand1.UncheckedAt(0));
  TMatrix dch2Mat  = *((TMatrix*) frDchTrackCand2.UncheckedAt(0));
  //cout<<"eventNum1track = "<<eventNum1track<<endl; 
  if(iter>0){
   sametrack=TracksDirectionTest(dch1Mat,dch2Mat,C0,XYZ1,delPhi);
   if(iter>1)cout<<"iter = "<<iter<<" "<<"meanDeltaPhi = "<<meanDeltaPhi<<endl;
   if(sametrack){
    new (ffDchTrCand1Align[eventNum1track]) TMatrix(dch1Mat);  
    new (ffDchTrCand2Align[eventNum1track]) TMatrix(dch2Mat);  
    nAlignedTracksInEvent++;
    eventNum1track++; 
    sumDeltaPhi+=delPhi;
   }  
  }else{
   new (ffDchTrCand1Align[eventNum1track]) TMatrix(dch1Mat);  
   new (ffDchTrCand2Align[eventNum1track]) TMatrix(dch2Mat);  
   nAlignedTracksInEvent++;
   eventNum1track++;
   //sumDeltaPhi+=delPhi;
   //cout<<"Iter = "<<Iter<<" "<<"delPhi = "<<delPhi<<", sumDeltaPhi = "<<sumDeltaPhi<<endl;
  } 
 }else{
  for (Int_t i = 0; i < ntracks1; ++i) {
    TMatrix* dch1Mat  = (TMatrix*) dchTrCand1->fDchTrackCand->At(i);
    float* matArr=dch1Mat->GetMatrixArray(); 
    for (Int_t j = 0; j < ntracks2; ++j) {
     TMatrix* dch2Mat  = (TMatrix*) dchTrCand2->fDchTrackCand->At(j);
     float* matArr2=dch2Mat->GetMatrixArray(); 
     //if(TracksDirectionTestReco(matArr,matArr2)){
     Float_t tmpAngle=tracksAngle(matArr,matArr2); 
     if(tmpAngle<minAngleCut&&tmpAngle<minTracksAngle){
      minTracksAngle=tracksAngle(matArr,matArr2); 
      tr1ind=i;
      tr2ind=j;
      if(!minAngleFound)minAngleFound=true;
     }
      //new (ffDchTrCand1Align[eventNum1track]) TMatrix(*dch1Mat);  
      //new (ffDchTrCand2Align[eventNum1track]) TMatrix(*dch2Mat);  
      //nAlignedTracksInEvent++;
      //eventNum1track++; 
     //}
    }// for ntrack2s
  }// for ntracks1
  if(minAngleFound){
   TMatrix* dch1Mat  = (TMatrix*) dchTrCand1->fDchTrackCand->At(tr1ind);
   float* matArr=dch1Mat->GetMatrixArray();
   TMatrix* dch2Mat  = (TMatrix*) dchTrCand2->fDchTrackCand->At(tr2ind);
   float* matArr2=dch2Mat->GetMatrixArray(); 
   new (ffDchTrCand1Align[eventNum1track]) TMatrix(*dch1Mat);  
   new (ffDchTrCand2Align[eventNum1track]) TMatrix(*dch2Mat);  
   nAlignedTracksInEvent++;
   eventNum1track++; 
   diffAngleTracks->Fill(minTracksAngle);
   //cout<<"nAlignedTracksInEvent = "<<nAlignedTracksInEvent<<", minimal tracks angle = "<<minTracksAngle<<endl;
  } 
 }//oneTrackInBothDCHs
}//if tracksInDCHs


}
//_______________________________________________________________
Float_t DchTrackManager::tracksAngle(const float* matArr,const float*
 matArr2) {

   Float_t x1=Matrix(matArr,0,0),x4=Matrix(matArr,3,0);  
   Float_t y1=Matrix(matArr,0,1),y4=Matrix(matArr,3,1); 
   Float_t z1=Matrix(matArr,0,2),z4=Matrix(matArr,3,2);
   Float_t x5=Matrix(matArr2,0,0),x8=Matrix(matArr2,3,0);  
   Float_t y5=Matrix(matArr2,0,1),y8=Matrix(matArr2,3,1); 
   Float_t z5=Matrix(matArr2,0,2),z8=Matrix(matArr2,3,2);
 
   TVector3 v1(x4-x1,y4-y1,z4-z1),v2(x8-x5,y8-y5,z8-z5);

  
return Float_t(TMath::RadToDeg()*v1.Angle(v2));

}

//_______________________________________________________________
Bool_t DchTrackManager::TracksDirectionTest(const TMatrix dch1Mat, const TMatrix dch2Mat, const TMatrixD C0, const XYZVector XYZ1, Double_t &delPhi) {

//coarse check of the compatibility of alignment tracks in both DCH's

Bool_t delPhiTest;

//XYZVector XYZ1(DCH1_Xpos[runPeriod-1], DCH1_Ypos[runPeriod],DCH1_Zpos[runPeriod]);

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
//cout<<"tracks delta phi = "<<delPhi<<endl;
if(iter==1){
 delPhiTest = (fabs(delPhi)<0.5?true:false);
}else if(iter>1){
 delPhiTest = (fabs(delPhi)<5.*meanDeltaPhi?true:false);
} 
//Bool_t delPhiTest = (fabs(delPhi)<0.5?true:false);
//Bool_t delPhiTest = true;
//cout<<"delPhiTest = "<<delPhi<<endl;
return delPhiTest;

}
//_______________________________________________________________
Bool_t DchTrackManager::TracksDirectionTestReco(const float* matArr,const float* matArr2) {

  
   Float_t x1=Matrix(matArr,0,0),x4=Matrix(matArr,3,0);  
   Float_t y1=Matrix(matArr,0,1),y4=Matrix(matArr,3,1); 
   Float_t z1=Matrix(matArr,0,2),z4=Matrix(matArr,3,2); 
   Float_t tgx1=(x4-x1)/(z4-z1); 
   Float_t tgy1=(y4-y1)/(z4-z1); 
   Float_t qx1=x1-tgx1*z1;
   Float_t qy1=y1-tgy1*z1;
   Float_t x5=Matrix(matArr2,0,0),x8=Matrix(matArr2,3,0);  
   Float_t y5=Matrix(matArr2,0,1),y8=Matrix(matArr2,3,1); 
   Float_t z5=Matrix(matArr2,0,2),z8=Matrix(matArr2,3,2); 
   Float_t tgx2=(x8-x5)/(z8-z5); 
   Float_t tgy2=(y8-y5)/(z8-z5); 
   Float_t qx2=x5-tgx2*z5;
   Float_t qy2=y5-tgy2*z5;

   TVector3 v1(x4-x1,y4-y1,z4-z1),v2(x8-x5,y8-y5,z8-z5);

   Float_t xExtrapDCH1toDCH2,yExtrapDCH1toDCH2;
   Float_t xExtrapDCH2toDCH1,yExtrapDCH2toDCH1;

   xExtrapDCH1toDCH2=tgx1*z5+qx1;
   yExtrapDCH1toDCH2=tgy1*z5+qy1;
   xExtrapDCH2toDCH1=tgx2*z1+qx2;
   yExtrapDCH2toDCH1=tgy2*z1+qy2;
  
   Float_t distDCH1=sqrt(pow(x1-xExtrapDCH2toDCH1,2.)+pow(y1-yExtrapDCH2toDCH1,2.)) ;
   Float_t distDCH2=sqrt(pow(x5-xExtrapDCH1toDCH2,2.)+pow(y5-yExtrapDCH1toDCH2,2.)) ;

   //cout<<"distDCH1,distDCH2: "<<distDCH1<<", "<<distDCH2<<endl;

   if(doCheck){ 
    if(strcmp(opt,"alignment")==0){
    //if(strcmp(opt,"autocalibration")==0){
    //if(strcmp(opt,"reconstruction")==0){
     distDch1->Fill(distDCH1);
     distDch2->Fill(distDCH2);
     distxDch1->Fill(x1-xExtrapDCH2toDCH1);
     distxDch2->Fill(x5-xExtrapDCH1toDCH2);
     distyDch1->Fill(y1-yExtrapDCH2toDCH1);
     distyDch2->Fill(y5-yExtrapDCH1toDCH2);
    }
   }
  
   Bool_t dirTest=false; 
   if(strcmp(opt,"reconstruction")==0){ 
    if(distDCH1<5.&&distDCH2<5.){//run<=3
     cout<<"distDCH: "<<TMath::RadToDeg()*tgx1<<" "<<TMath::RadToDeg()*tgy1<<" "<<TMath::RadToDeg()*tgx2<<" "<<TMath::RadToDeg()*tgy2<<" "<<endl;
     dirTest=true;
    }
   //}else if(strcmp(opt,"alignment")==0){
   }else if(!(strcmp(opt,"reconstruction")==0)){
    Float_t alphaX1,alphaX2,alphaY1,alphaY2;
    alphaX1=TMath::RadToDeg()*atan(tgx1);
    alphaX2=TMath::RadToDeg()*atan(tgx2);
    alphaY1=TMath::RadToDeg()*atan(tgy1);
    alphaY2=TMath::RadToDeg()*atan(tgy2);
    //if(fabs(alphaX1-alphaX2)<2.&&fabs(alphaY1-alphaY2)<2.)dirTest=true;   
    if(TMath::RadToDeg()*v1.Angle(v2)<10.)dirTest=true;  //run6 
    //if(fabs(alphaX1-alphaX2)<20.&&fabs(alphaY1-alphaY2)<20.)dirTest=true;   
    //cout<<"alphaX, alphaY: "<<alphaX1<<" "<<alphaX2<<" "<<alphaY1<<" "<<alphaY2<<" "<<fabs(alphaX1-alphaX2)<<" "<<fabs(alphaY1-alphaY2)<<" "<<fabs(alphaX1-alphaX2)+fabs(alphaY1-alphaY2)<<endl;
    //cout<<"angle between DCH's tracks = "<<TMath::RadToDeg()*v1.Angle(v2)<<endl;
    if(strcmp(opt,"alignment")==0){
     //if(strcmp(opt,"autocalibration")==0){
     diffAngleTracks->Fill(TMath::RadToDeg()*v1.Angle(v2));
     diffAngleTracksX->Fill(alphaX1-alphaX2);
     diffAngleTracksY->Fill(alphaY1-alphaY2);
    } 
   }

   return dirTest;
      //trackAngleX->Fill(TMath::RadToDeg()*atan(parFitL[1]));

}
//-------------------------------------------------------------------------------------------------------------------------
/*UShort_t DchTrackManager::RunTypeToNumber(TString runPeriod_){

UShort_t j;

           if(runType_=="run1"){j=0;}
           else if(runPeriod_=="run2"){j=1;}
           else if(runPeriod_=="run3"){j=2;}
           else if(runPeriod_=="run4"){j=3;}
           else if(runPeriod_=="run5"){j=4;}
           else if(runPeriod_=="run6"){j=5;}
           else{cout<<"run period not in the list!"<<endl; 
                cout<<" run period = "<<runPeriod_<<endl;}
return j;

}*/
//_______________________________________________________________
void DchTrackManager::DchTrackMerger(DchTrackCand *dchTrCand1,DchTrackCand *dchTrCand2, const TMatrixD C0, Int_t DCHtrackIDglob[], const Int_t eventnum, const Option_t* Opt, TClonesArray *pTrackCollection[], const TMatrixFSym covMat[], Double_t parFitL[], Int_t &fitErrorcode) {

opt=Opt;
//Double_t parFitL[4];
Double_t res;
Double_t fullLength,r1,r2;
//Double_t X[numChambers],Y[numChambers],U[numChambers],V[numChambers];
//Double_t X_Y[numChambers],X_U[numChambers],X_V[numChambers];
//Double_t Y_X[numChambers],Y_U[numChambers],Y_V[numChambers];
//Double_t muX[numChambers],muY[numChambers],muU[numChambers],muV[numChambers];
Int_t DCHtrackID[3];
for (UShort_t j = 0; j < 3; j++) {DCHtrackID[j] = 0;} 

Int_t ntracks1=dchTrCand1->fDchTrackCand->GetEntriesFast();
Int_t ntracks2=dchTrCand2->fDchTrackCand->GetEntriesFast();
//Int_t trMerged=0; 

//fitErrorcode=10;
//cout<<" ntracks1 = "<<ntracks1<<", ntracks2 = "<<ntracks2<<endl;

Bool_t oneTrackInBothDCHs=false;
Bool_t tracksInDCHs=false;
Bool_t graphFilled,trackFilledDCH1,trackFilledDCH2;

if(ntracks1>0&&ntracks2>0){
 tracksInDCHs=true;
 if(ntracks1==1&&ntracks2==1)oneTrackInBothDCHs=true;
}

//if(ntracks1>0&&ntracks2>0){
// tracksInDCHs=true;
// if(ntracks1==1&&ntracks2==1)oneTrackInBothDCHs=true;
//}
 
Bool_t tracksmatch=false;
//cout<<opt<<" "<<tracksInDCHs<<endl;
if((strcmp(opt,"reconstruction")==0&&tracksInDCHs)||oneTrackInBothDCHs||(strcmp(opt,"autocalibration")==0&&tracksInDCHs))tracksmatch=true;

//if(ntracks1==1&&ntracks2==1){
if(tracksmatch){

//TClonesArray &frDchTrackCand = *(dchTrCand1->fDchTrackCand);
//frDchTrackCand.Dump();
  for (Int_t i = 0; i < ntracks1; ++i) {
    graphFilled=false;
    trackFilledDCH1=false;
    //TMatrix* dch1Mat  = (TMatrix*) frDchTrackCand.UncheckedAt(i);
    TMatrix* dch1Mat  = (TMatrix*) dchTrCand1->fDchTrackCand->At(i);
    //dch1Mat->Print(); 
    float* matArr=dch1Mat->GetMatrixArray(); 
    //TGraph2DErrors* dchtrgr = new TGraph2DErrors();
    UInt_t kl=0;
    //for (UShort_t k = 0; k < 2; ++k) {
    TGraph2DErrors* dchtrgr = new TGraph2DErrors();
    for (Int_t j = 0; j < ntracks2; ++j) {
     trackFilledDCH2=false;
     TMatrix* dch2Mat  = (TMatrix*) dchTrCand2->fDchTrackCand->At(j);
     //dch2Mat->Print(); 
     float* matArr2=dch2Mat->GetMatrixArray(); 
     //if(fabs(Matrix(matArr2,0,0)-Matrix(matArr2,0,4))>0.5)cout<<"matArr2x: "<<i<<" "<<j<<" "<<Matrix(matArr2,0,0)<<" "<<Matrix(matArr2,0,1)<<" "<<Matrix(matArr2,0,2)<<" "<<Matrix(matArr2,0,3)<<" "<<Matrix(matArr2,0,4)<<" "<<Matrix(matArr2,0,5)<<" "<<Matrix(matArr2,0,6)<<" "<<Matrix(matArr2,0,7)<<" "<<Matrix(matArr2,0,8)<<" "<<Matrix(matArr2,0,9)<<" "<<Matrix(matArr2,0,10)<<" "<<endl;
     //if(fabs(Matrix(matArr2,1,1)-Matrix(matArr2,1,5))>0.5)cout<<"matArr2y: "<<i<<" "<<j<<" "<<Matrix(matArr2,1,0)<<" "<<Matrix(matArr2,1,1)<<" "<<Matrix(matArr2,1,2)<<" "<<Matrix(matArr2,1,3)<<" "<<Matrix(matArr2,1,4)<<" "<<Matrix(matArr2,1,5)<<" "<<Matrix(matArr2,1,6)<<" "<<Matrix(matArr2,1,7)<<" "<<Matrix(matArr2,1,8)<<" "<<Matrix(matArr2,1,9)<<" "<<Matrix(matArr2,1,10)<<" "<<endl;
     //cout<<"TracksDirectionTestReco = "<<TracksDirectionTestReco(matArr,matArr2)<<" "<<j<<endl; 
     if(TracksDirectionTestReco(matArr,matArr2)){
      if(!graphFilled){ 
       for (UShort_t k = 0; k < 4; ++k) {
        //cout<<"dch1 "<<Matrix(matArr,k,0)<<" "<<Matrix(matArr,k,1)<<" "<<Matrix(matArr,k,2)<<" "<<Matrix(matArr,k,3)<<" "<<Matrix(matArr,k,4)<<endl;
        dchtrgr->SetPoint(kl,Matrix(matArr,k,0),Matrix(matArr,k,1),Matrix(matArr,k,2));
        dchtrgr->SetPointError(kl,resolution,resolution,0.); 
        //cout<<"i = "<<i<<" "<<",kl = "<<kl<<" "<<Matrix(matArr,k,0)<<" "<<Matrix(matArr,k,1)<<" "<<Matrix(matArr,k,2)<<endl;  
        /*if(k==0)dchtrgr->SetPointError(kl,0.,resolution,0.);
        if(k==1)dchtrgr->SetPointError(kl,resolution,0.,0.);
        if(k==2)dchtrgr->SetPointError(kl,sqrt(2)*resolution,sqrt(2)*resolution,0.);
        if(k==3)dchtrgr->SetPointError(kl,sqrt(2)*resolution,sqrt(2)*resolution,0.);*/
        kl++;
       }
       graphFilled=true;
      }//if(j==0)
    //dchMat->Print(); 
      if(!trackFilledDCH1){ 
       if(strcmp(opt,"reconstruction")==0){
        Double_t delZ=Matrix(matArr,3,2)-Matrix(matArr,0,2);
        Double_t slopeX1=(Matrix(matArr,3,0)-Matrix(matArr,0,0))/delZ;
        Double_t slopeY1=(Matrix(matArr,3,1)-Matrix(matArr,0,1))/delZ;
        //slopeX2=slopeX1,slopeY2=slopeY1    
        BmnDchTrack *dchTrack = AddTrack(DCHtrackID[0], DCHtrackIDglob[0], pTrackCollection[1], TVector3(Matrix(matArr,0,0),Matrix(matArr,0,1),Matrix(matArr,0,2)),TVector3(Matrix(matArr,3,0),Matrix(matArr,3,1),Matrix(matArr,3,2)),slopeX1,slopeY1,slopeX1,slopeY1,eventnum,covMat[0],covMat[1],"DCH1");
        DCHtrackID[0]++;
        DCHtrackIDglob[0]++;
        trackFilledDCH1=true;
       }
      }//if(i==0&&j==0)
    //for (Int_t j = 0; j < ntracks2; ++j) {
      //TMatrix* dch2Mat  = (TMatrix*) dchTrCand2->fDchTrackCand->At(j);
      //dch2Mat->Print(); 
      //float* matArr2=dch2Mat->GetMatrixArray(); 
      //for (UShort_t k = 0; k < 2; ++k) {
      for (UShort_t k = 0; k < 4; ++k) {
        //cout<<"dch2 "<<Matrix(matArr2,k,0)<<" "<<Matrix(matArr2,k,1)<<" "<<Matrix(matArr2,k,2)<<" "<<Matrix(matArr2,k,3)<<" "<<Matrix(matArr2,k,4)<<endl;
        dchtrgr->SetPoint(kl,Matrix(matArr2,k,0),Matrix(matArr2,k,1),Matrix(matArr2,k,2));
        dchtrgr->SetPointError(kl,resolution,resolution,0.); 
        //cout<<"i = "<<i<<",j = "<<j<<" "<<",kl = "<<kl<<" "<<Matrix(matArr2,k,0)<<" "<<Matrix(matArr2,k,1)<<" "<<Matrix(matArr2,k,2)<<endl;  
        kl++;
      }
      //DchLinearTrackFitter(dchtrgr, parFitL, chi2linfitHist, fitErrorcode);
      //DchLinearTrackFitter(dchtrgr, parFitL, fitErrorcode);
      //cout<<"parFitL1: "<<parFitL[0]<<" "<<parFitL[1]<<" "<<parFitL[2]<<" "<<parFitL[3]<<endl;
      //cout<<"chi2linfit = "<<chi2linfit<<endl;
      DchAverageLinearTrack(dchtrgr, parFitL);
      fitErrorcode=0;
      //cout<<"parFitL2: "<<parFitL[0]<<" "<<parFitL[1]<<" "<<parFitL[2]<<" "<<parFitL[3]<<endl;
      if(strcmp(opt,"reconstruction")==0){
       //if(TracksDirectionTestReco(matArr,matArr2)){
       BmnDchTrack *dchTrack = AddTrack(DCHtrackID[2], DCHtrackIDglob[2], pTrackCollection[0], TVector3(parFitL[0]+Matrix(matArr,0,2)*parFitL[1],parFitL[2]+Matrix(matArr,0,2)*parFitL[3],Matrix(matArr,0,2)),TVector3(parFitL[0]+Matrix(matArr2,0,2)*parFitL[1],parFitL[2]+Matrix(matArr2,0,2)*parFitL[3],Matrix(matArr2,0,2)),parFitL[1],parFitL[3],parFitL[1],parFitL[3],eventnum,covMat[4],covMat[5],"DCH1DCH2");
       DCHtrackID[2]++;
       DCHtrackIDglob[2]++;
       FillAngles(dchtrgr,parFitL);
       //}
      }
      if(strcmp(opt,"reconstruction")==0){
       if(!trackFilledDCH2){
        Double_t delZ=Matrix(matArr2,3,2)-Matrix(matArr2,0,2);
        Double_t slopeX1=(Matrix(matArr2,3,0)-Matrix(matArr2,0,0))/delZ;
        Double_t slopeY1=(Matrix(matArr2,3,1)-Matrix(matArr2,0,1))/delZ;
        //slopeX2=slopeX1,slopeY2=slopeY1 
        BmnDchTrack *dchTrack = AddTrack(DCHtrackID[1], DCHtrackIDglob[1], pTrackCollection[2], TVector3(Matrix(matArr2,0,0),Matrix(matArr2,0,1),Matrix(matArr2,0,2)),TVector3(Matrix(matArr2,3,0),Matrix(matArr2,3,1),Matrix(matArr2,3,2)),slopeX1,slopeY1,slopeX1,slopeY1,eventnum,covMat[2],covMat[3],"DCH2");
        DCHtrackID[1]++;
        DCHtrackIDglob[1]++;
        trackFilledDCH2=true;
       }//if(i==0&&j==0)
      }
      //trackGlobID++;
      //trMerged++;
      //cout<<"trMerged = "<<trMerged<<endl;
      //cout<<"fitErrorcode1 = "<<fitErrorcode<<endl;
      //if(fitErrorcode!=0)cout<<"fitErrorcode = "<<fitErrorcode<<endl;
      if(fitErrorcode!=0)continue;
      if(checkDTM){
       for (UShort_t k = 0; k < 4; ++k) {
        //cout<<"differences (x,y,DCH1): "<<Matrix(matArr,k,0)-(parFitL[0]+Matrix(matArr,k,2)*parFitL[1])<<" "<<Matrix(matArr,k,1)-(parFitL[2]+Matrix(matArr,k,2)*parFitL[3])<<" "<<k<<" 1"<<endl;
        //cout<<"differences (x,y,DCH2): "<<Matrix(matArr2,k,0)-(parFitL[0]+Matrix(matArr2,k,2)*parFitL[1])<<" "<<Matrix(matArr2,k,1)-(parFitL[2]+Matrix(matArr2,k,2)*parFitL[3])<<" "<<k<<" 2"<<endl;
        hDeltaX[0][k]->Fill(Matrix(matArr,k,0)-(parFitL[0]+Matrix(matArr,k,2)*parFitL[1])); 
        hDeltaY[0][k]->Fill(Matrix(matArr,k,1)-(parFitL[2]+Matrix(matArr,k,2)*parFitL[3])); 
        hDeltaX[1][k]->Fill(Matrix(matArr2,k,0)-(parFitL[0]+Matrix(matArr2,k,2)*parFitL[1])); 
        hDeltaY[1][k]->Fill(Matrix(matArr2,k,1)-(parFitL[2]+Matrix(matArr2,k,2)*parFitL[3])); 
       }
      }
      if(!(strcmp(opt,"reconstruction")==0))FillAngles(dchtrgr,parFitL);
      //Double_t magfieldint=MagFieldIntegral(parFitL);
      //trackAngleX->Fill(TMath::RadToDeg()*atan(parFitL[1]));
      //trackAngleY->Fill(TMath::RadToDeg()*atan(parFitL[3]));
      for (UShort_t k = 0; k < 4; ++k) {
       if(k==1){
         res=abs(parFitL[2] + parFitL[3]*Matrix(matArr,k,2)-Matrix(matArr,k,5))-abs(Matrix(matArr,k,1)-Matrix(matArr,k,5));
         residVsTDCy->Fill(Matrix(matArr,k,3),res);
         ResidVsTDCy[0]->Fill(Matrix(matArr,k,3),res);
         residVsTDC->Fill(Matrix(matArr,k,3),res);  
         ResidVsTDC[0]->Fill(Matrix(matArr,k,3),res);
         res=abs(parFitL[0] + parFitL[1]*Matrix(matArr,k,2)-Matrix(matArr,k,4))-abs(Matrix(matArr,k,0)-Matrix(matArr,k,4));
         ResidVsTDCx_y[0]->Fill(Matrix(matArr,k,3),res);
         //Y[0]=Matrix(matArr,k,1);
         //X_Y[0]=Matrix(matArr,k,0);
         //muY[0]=parFitL[2] + parFitL[3]*Matrix(matArr,k,2);
         if(checkDTM){
          //mDCAy[0]->Fill(fabs(Matrix(matArr,k,1)-Matrix(matArr,k,5)));
          fullLength=sqrt(pow(Matrix(matArr,k,7)-Matrix(matArr,k,4),2.)+pow(Matrix(matArr,k,8)-Matrix(matArr,k,5),2.)+pow(Matrix(matArr,k,9)-Matrix(matArr,k,6),2.)); 
          mDCAy[0]->Fill(PointLineDistance(Matrix(matArr,k,0),Matrix(matArr,k,1),Matrix(matArr,k,2), Matrix(matArr,k,4), Matrix(matArr,k,5),Matrix(matArr,k,6), (Matrix(matArr,k,7)-Matrix(matArr,k,4))/fullLength, (Matrix(matArr,k,8)-Matrix(matArr,k,5))/fullLength,(Matrix(matArr,k,9)-Matrix(matArr,k,6))/fullLength));
          //cout<<"distance in y plane, DCH1:"<<PointLineDistance(Matrix(matArr,k,0),Matrix(matArr,k,1),Matrix(matArr,k,2), Matrix(matArr,k,4), Matrix(matArr,k,5),Matrix(matArr,k,6), (Matrix(matArr,k,7)-Matrix(matArr,k,4))/fullLength, (Matrix(matArr,k,8)-Matrix(matArr,k,5))/fullLength,(Matrix(matArr,k,9)-Matrix(matArr,k,6))/fullLength)<<", "<<fullLength<<endl;
          //if(fabs(Matrix(matArr,k,1)-Matrix(matArr,k,5))>0.5)cout<<"mDCAy1: "<<Matrix(matArr,k,1)<<" "<<Matrix(matArr,k,5)<<" "<<Matrix(matArr,k,1)-Matrix(matArr,k,5)<<endl;
          fDCAy[0]->Fill(abs(parFitL[2] + parFitL[3]*Matrix(matArr,k,2)-Matrix(matArr,k,5)));
          //fDCAy[0]->Fill(PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr,k,2),parFitL[2] + parFitL[3]*Matrix(matArr,k,2),Matrix(matArr,k,2),Matrix(matArr,k,4), Matrix(matArr,k,5),Matrix(matArr,k,6), (Matrix(matArr,k,7)-Matrix(matArr,k,4))/fullLength, (Matrix(matArr,k,8)-Matrix(matArr,k,5))/fullLength, (Matrix(matArr,k,9)-Matrix(matArr,k,6))/fullLength));
          //if(abs(parFitL[2] + parFitL[3]*Matrix(matArr,k,2)-Matrix(matArr,k,5))>0.5)cout<<"fDCAy0: "<<abs(parFitL[2] + parFitL[3]*Matrix(matArr,k,2)-Matrix(matArr,k,5))<<" "<<parFitL[2] + parFitL[3]*Matrix(matArr,k,2)<<" "<<Matrix(matArr,k,5)<<endl;
         }
         res=abs(parFitL[2] + parFitL[3]*Matrix(matArr2,k,2)-Matrix(matArr2,k,5))-abs(Matrix(matArr2,k,1)-Matrix(matArr2,k,5));
         //fullLength=sqrt(pow(Matrix(matArr2,k,7)-Matrix(matArr2,k,4),2.)+pow(Matrix(matArr2,k,8)-Matrix(matArr2,k,5),2.)); 
         //fullLength=sqrt(pow(Matrix(matArr2,k,7)-Matrix(matArr2,k,4),2.)+pow(Matrix(matArr2,k,8)-Matrix(matArr2,k,5),2.)+pow(Matrix(matArr2,k,9)-Matrix(matArr2,k,6),2.)); 
         //r1=PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2),parFitL[2] + parFitL[3]*Matrix(matArr2,k,2),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength, 0.); 
         //r1=PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2),parFitL[2] + parFitL[3]*Matrix(matArr2,k,2), Matrix(matArr2,k,2), Matrix(matArr2,k,4), Matrix(matArr2,k,5), Matrix(matArr2,k,6), (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength, (Matrix(matArr2,k,9)-Matrix(matArr2,k,6))/fullLength); 
         //r2=PointLineDistance(Matrix(matArr2,k,0), Matrix(matArr2,k,1),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength, 0.); 
         //r2=PointLineDistance(Matrix(matArr2,k,0), Matrix(matArr2,k,1),Matrix(matArr2,k,2), Matrix(matArr2,k,4), Matrix(matArr2,k,5), Matrix(matArr2,k,6), (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength, (Matrix(matArr2,k,9)-Matrix(matArr2,k,6))/fullLength); 
         //cout<<"r1r2 "<<r1<<" "<<r2<<endl;
         //cout<<"fullLength "<<fullLength<<endl;
         //cout<<Matrix(matArr2,k,7)-Matrix(matArr2,k,4)<<" "<<Matrix(matArr2,k,8)-Matrix(matArr2,k,5)<<" "<<Matrix(matArr2,k,9)-Matrix(matArr2,k,6)<<endl;
         //cout<<"r2: "<<Matrix(matArr2,k,0)<<" "<<Matrix(matArr2,k,1)<<" "<<Matrix(matArr2,k,2)<<" "<<Matrix(matArr2,k,4)<<" "<<Matrix(matArr2,k,5)<<" "<<Matrix(matArr2,k,6)<<" "<<(Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength<<" "<<(Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength<<" "<<(Matrix(matArr2,k,9)-Matrix(matArr2,k,6))/fullLength<<endl;
         residVsTDCy->Fill(Matrix(matArr2,k,3),res);//res instead of r1-r2
         ResidVsTDCy[1]->Fill(Matrix(matArr2,k,3),res);
         residVsTDC->Fill(Matrix(matArr2,k,3),res);
         //cout<<"residVsTDC2: "<<Matrix(matArr2,k,3)<<" "<<res<<endl;
         ResidVsTDC[1]->Fill(Matrix(matArr2,k,3),res);
         res=abs(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2)-Matrix(matArr2,k,4))-abs(Matrix(matArr2,k,0)-Matrix(matArr2,k,4));
         ResidVsTDCx_y[1]->Fill(Matrix(matArr2,k,3),res);
         //Y[1]=Matrix(matArr2,k,1);
         //X_Y[1]=Matrix(matArr2,k,0);
         //muY[1]=parFitL[2] + parFitL[3]*Matrix(matArr2,k,2);
         if(checkDTM){
          //cout<<"r1,r2: "<<r1<<" "<<r2<<endl;
          //mDCAy[1]->Fill(r2);
          //fDCAy[1]->Fill(r1);
          //mDCAy[1]->Fill(fabs(Matrix(matArr2,k,1)-Matrix(matArr2,k,5)));
          fullLength=sqrt(pow(Matrix(matArr2,k,7)-Matrix(matArr2,k,4),2.)+pow(Matrix(matArr2,k,8)-Matrix(matArr2,k,5),2.)+pow(Matrix(matArr2,k,9)-Matrix(matArr2,k,6),2.)); 
          mDCAy[1]->Fill(PointLineDistance(Matrix(matArr2,k,0),Matrix(matArr2,k,1),Matrix(matArr2,k,2), Matrix(matArr2,k,4), Matrix(matArr2,k,5),Matrix(matArr2,k,6), (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength,(Matrix(matArr2,k,9)-Matrix(matArr2,k,6))/fullLength));
          //cout<<"distance in y plane, DCH2:"<<PointLineDistance(Matrix(matArr2,k,0),Matrix(matArr2,k,1),Matrix(matArr2,k,2), Matrix(matArr2,k,4), Matrix(matArr2,k,5),Matrix(matArr2,k,6), (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength,(Matrix(matArr2,k,9)-Matrix(matArr2,k,6))/fullLength)<<", "<<fullLength<<endl;
          //if(fabs(Matrix(matArr2,k,1)-Matrix(matArr2,k,5))>0.5)cout<<"mDCAy2: "<<i<<" "<<j<<" "<<Matrix(matArr2,k,1)<<" "<<Matrix(matArr2,k,5)<<" "<<Matrix(matArr2,k,1)-Matrix(matArr2,k,5)<<endl;
          fDCAy[1]->Fill(abs(parFitL[2] + parFitL[3]*Matrix(matArr2,k,2)-Matrix(matArr2,k,5)));
         }
         //if(r2>0.45&&iter==0){
         //if(iter==0){
          //cout<<"r2_045 = "<<r2<<", iter = "<<iter<<endl;
          //cout<<Matrix(matArr2,k,0)<<" "<<Matrix(matArr2,k,1)<<endl;
          //cout<<Matrix(matArr2,k,4)<<" "<<Matrix(matArr2,k,5)<<endl;
          //cout<<Matrix(matArr2,k,7)<<" "<<Matrix(matArr2,k,8)<<endl;
          //cout<<Matrix(matArr2,k,9)<<" "<<Matrix(matArr2,k,6)<<endl;
          //cout<<Matrix(matArr2,k,7)-Matrix(matArr2,k,4)<<" "<<Matrix(matArr2,k,8)-Matrix(matArr2,k,5)<<" "<<Matrix(matArr2,k,9)-Matrix(matArr2,k,6)<<endl;
          //cout<<(Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength<<" "<<(Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength<<" "<<(Matrix(matArr2,k,9)-Matrix(matArr2,k,6))/fullLength<<endl;
          //cout<<fullLength<<endl;
         //}
       }else if(k==0){
         res=abs(parFitL[0] + parFitL[1]*Matrix(matArr,k,2)-Matrix(matArr,k,4))-abs(Matrix(matArr,k,0)-Matrix(matArr,k,4));
         residVsTDCx->Fill(Matrix(matArr,k,3),res);
         ResidVsTDCx[0]->Fill(Matrix(matArr,k,3),res);
         residVsTDC->Fill(Matrix(matArr,k,3),res);
         //cout<<"residVsTDC3: "<<Matrix(matArr,k,3)<<" "<<res<<endl;
         ResidVsTDC[0]->Fill(Matrix(matArr,k,3),res);
         res=abs(parFitL[2] + parFitL[3]*Matrix(matArr,k,2)-Matrix(matArr,k,5))-abs(Matrix(matArr,k,1)-Matrix(matArr,k,5));
         ResidVsTDCy_x[0]->Fill(Matrix(matArr,k,3),res);
         //X[0]=Matrix(matArr,k,0);
         //Y_X[0]=Matrix(matArr,k,1);
         //muX[0]=parFitL[0] + parFitL[1]*Matrix(matArr,k,2);
         if(checkDTM){
          //mDCAx[0]->Fill(fabs(Matrix(matArr,k,0)-Matrix(matArr,k,4)));
          fullLength=sqrt(pow(Matrix(matArr,k,7)-Matrix(matArr,k,4),2.)+pow(Matrix(matArr,k,8)-Matrix(matArr,k,5),2.)+pow(Matrix(matArr,k,9)-Matrix(matArr,k,6),2.)); 
          mDCAx[0]->Fill(PointLineDistance(Matrix(matArr,k,0),Matrix(matArr,k,1),Matrix(matArr,k,2), Matrix(matArr,k,4), Matrix(matArr,k,5),Matrix(matArr,k,6), (Matrix(matArr,k,7)-Matrix(matArr,k,4))/fullLength, (Matrix(matArr,k,8)-Matrix(matArr,k,5))/fullLength,(Matrix(matArr,k,9)-Matrix(matArr,k,6))/fullLength));
          //if(fabs(Matrix(matArr,k,0)-Matrix(matArr,k,4))>0.5)cout<<"mDCAx1: "<<Matrix(matArr,k,0)<<" "<<Matrix(matArr,k,4)<<" "<<Matrix(matArr,k,0)-Matrix(matArr,k,4)<<endl;
          fDCAx[0]->Fill(abs(parFitL[0] + parFitL[1]*Matrix(matArr,k,2)-Matrix(matArr,k,4)));
         }
         res=abs(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2)-Matrix(matArr2,k,4))-abs(Matrix(matArr2,k,0)-Matrix(matArr2,k,4));
         //fullLength=sqrt(pow(Matrix(matArr2,k,7)-Matrix(matArr2,k,4),2.)+pow(Matrix(matArr2,k,8)-Matrix(matArr2,k,5),2.)); 
         //r1=PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2),parFitL[2] + parFitL[3]*Matrix(matArr2,k,2),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength, 0.); 
         //r2=PointLineDistance(Matrix(matArr2,k,0), Matrix(matArr2,k,1),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength, 0.); 
         residVsTDCx->Fill(Matrix(matArr2,k,3),res);//res instead of r1-r2
         //cout<<"deltaR1 = "<<r1-r2<<endl;
         ResidVsTDCx[1]->Fill(Matrix(matArr2,k,3),res);
         //cout<<"deltaR2 = "<<r1-r2<<endl;
         residVsTDC->Fill(Matrix(matArr2,k,3),res);
         //cout<<"residVsTDC4: "<<Matrix(matArr2,k,3)<<" "<<res<<endl;
         ResidVsTDC[1]->Fill(Matrix(matArr2,k,3),res);
         res=abs(parFitL[2] + parFitL[3]*Matrix(matArr2,k,2)-Matrix(matArr2,k,5))-abs(Matrix(matArr2,k,1)-Matrix(matArr2,k,5));
         ResidVsTDCy_x[1]->Fill(Matrix(matArr2,k,3),res);
         //X[1]=Matrix(matArr2,k,0);
         //Y_X[1]=Matrix(matArr2,k,1);
         //muX[1]=parFitL[0] + parFitL[1]*Matrix(matArr2,k,2);
         if(checkDTM){
          //mDCAx[1]->Fill(r2);
          //fDCAx[1]->Fill(r1);
          mDCAx[1]->Fill(fabs(Matrix(matArr2,k,0)-Matrix(matArr2,k,4)));
          fullLength=sqrt(pow(Matrix(matArr2,k,7)-Matrix(matArr2,k,4),2.)+pow(Matrix(matArr2,k,8)-Matrix(matArr2,k,5),2.)+pow(Matrix(matArr2,k,9)-Matrix(matArr2,k,6),2.)); 
          mDCAx[1]->Fill(PointLineDistance(Matrix(matArr2,k,0),Matrix(matArr2,k,1),Matrix(matArr2,k,2), Matrix(matArr2,k,4), Matrix(matArr2,k,5),Matrix(matArr2,k,6), (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength,(Matrix(matArr2,k,9)-Matrix(matArr2,k,6))/fullLength));
          //if(fabs(Matrix(matArr2,k,0)-Matrix(matArr2,k,4))>0.5)cout<<"mDCAx2: "<<i<<" "<<j<<" "<<Matrix(matArr2,k,0)<<" "<<Matrix(matArr2,k,4)<<" "<<Matrix(matArr2,k,0)-Matrix(matArr2,k,4)<<endl;
          fDCAx[1]->Fill(abs(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2)-Matrix(matArr2,k,4)));
         }
          //cout<<fullLength<<endl;
       }else if(k==2||k==3){
         r1=PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr,k,2),parFitL[2] + parFitL[3]*Matrix(matArr,k,2),0., Matrix(matArr,k,4), Matrix(matArr,k,5), 0., dirCosLayerX[2*k], dirCosLayerY[2*k],0.); 
         r2=PointLineDistance(Matrix(matArr,k,0), Matrix(matArr,k,1),0., Matrix(matArr,k,4), Matrix(matArr,k,5), 0., dirCosLayerX[2*k], dirCosLayerY[2*k],0.); 
         if(k==2){
          residVsTDCu->Fill(Matrix(matArr,k,3),r1-r2);
          ResidVsTDCu[0]->Fill(Matrix(matArr,k,3),r1-r2);
          residVsTDC->Fill(Matrix(matArr,k,3),r1-r2);
         //cout<<"residVsTDC5: "<<Matrix(matArr,k,3)<<" "<<res<<endl;
          ResidVsTDC[0]->Fill(Matrix(matArr,k,3),r1-r2);
          res=abs(parFitL[0] + parFitL[1]*Matrix(matArr,k,2)-Matrix(matArr,k,4))-abs(Matrix(matArr,k,0)-Matrix(matArr,k,4));
          ResidVsTDCx_u[0]->Fill(Matrix(matArr,k,3),res);
          res=abs(parFitL[2] + parFitL[3]*Matrix(matArr,k,2)-Matrix(matArr,k,5))-abs(Matrix(matArr,k,1)-Matrix(matArr,k,5));
          ResidVsTDCy_u[0]->Fill(Matrix(matArr,k,3),res);
          //U[0]=sqrt2half*(Matrix(matArr,k,1)-Matrix(matArr,k,0));
          //X_U[0]=Matrix(matArr,k,0);
          //Y_U[0]=Matrix(matArr,k,1);
          //muU[0]=sqrt2half*(parFitL[2] + parFitL[3]*Matrix(matArr,k,2) - parFitL[0] - parFitL[1]*Matrix(matArr,k,2));
          if(checkDTM){
           mDCAu[0]->Fill(r2);
           fDCAu[0]->Fill(r1);
          }
         }
         if(k==3){
          residVsTDCv->Fill(Matrix(matArr,k,3),r1-r2);
          ResidVsTDCv[0]->Fill(Matrix(matArr,k,3),r1-r2);
          residVsTDC->Fill(Matrix(matArr,k,3),r1-r2);
          //cout<<"residVsTDC6: "<<Matrix(matArr,k,3)<<" "<<res<<endl;
          ResidVsTDC[0]->Fill(Matrix(matArr,k,3),r1-r2);
          res=abs(parFitL[0] + parFitL[1]*Matrix(matArr,k,2)-Matrix(matArr,k,4))-abs(Matrix(matArr,k,0)-Matrix(matArr,k,4));
          ResidVsTDCx_v[0]->Fill(Matrix(matArr,k,3),res);
          res=abs(parFitL[2] + parFitL[3]*Matrix(matArr,k,2)-Matrix(matArr,k,5))-abs(Matrix(matArr,k,1)-Matrix(matArr,k,5));
          ResidVsTDCy_v[0]->Fill(Matrix(matArr,k,3),res);
          //V[0]=sqrt2half*(Matrix(matArr,k,1)+Matrix(matArr,k,0));
          //X_V[0]=Matrix(matArr,k,0);
          //Y_V[0]=Matrix(matArr,k,1);
          //muV[0]=sqrt2half*(parFitL[2] + parFitL[3]*Matrix(matArr,k,2) + parFitL[0] + parFitL[1]*Matrix(matArr,k,2));
          if(checkDTM){
           mDCAv[0]->Fill(r2);
           fDCAv[0]->Fill(r1);
          }
         } 
         fullLength=sqrt(pow(Matrix(matArr2,k,7)-Matrix(matArr2,k,4),2.)+pow(Matrix(matArr2,k,8)-Matrix(matArr2,k,5),2.)); 
         //cout<<fullLength<<endl;
         r1=PointLineDistance(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2),parFitL[2] + parFitL[3]*Matrix(matArr2,k,2),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength, 0.); 
         r2=PointLineDistance(Matrix(matArr2,k,0), Matrix(matArr2,k,1),0., Matrix(matArr2,k,4), Matrix(matArr2,k,5), 0., (Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength, (Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength, 0.); 
         //cout<<"dircosX2 = "<<(Matrix(matArr2,k,7)-Matrix(matArr2,k,4))/fullLength<<endl;
         //cout<<"dircosY2 = "<<(Matrix(matArr2,k,8)-Matrix(matArr2,k,5))/fullLength<<endl;
         if(k==2){
          residVsTDCu->Fill(Matrix(matArr2,k,3),r1-r2);
          ResidVsTDCu[1]->Fill(Matrix(matArr2,k,3),r1-r2);
          residVsTDC->Fill(Matrix(matArr2,k,3),r1-r2);
          //cout<<"residVsTDC7: "<<Matrix(matArr2,k,3)<<" "<<res<<endl;
          ResidVsTDC[1]->Fill(Matrix(matArr2,k,3),r1-r2);
          res=abs(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2)-Matrix(matArr2,k,4))-abs(Matrix(matArr2,k,0)-Matrix(matArr2,k,4));
          ResidVsTDCx_u[1]->Fill(Matrix(matArr2,k,3),res);
          res=abs(parFitL[2] + parFitL[3]*Matrix(matArr2,k,2)-Matrix(matArr2,k,5))-abs(Matrix(matArr2,k,1)-Matrix(matArr2,k,5));
          ResidVsTDCy_u[1]->Fill(Matrix(matArr2,k,3),res);
          //U[1]=sqrt2half*(Matrix(matArr2,k,1)-Matrix(matArr2,k,0));
          //X_U[1]=Matrix(matArr2,k,0);
          //Y_U[1]=Matrix(matArr2,k,1);
          //muU[1]=sqrt2half*(parFitL[2] + parFitL[3]*Matrix(matArr2,k,2) - parFitL[0] - parFitL[1]*Matrix(matArr2,k,2));
          if(checkDTM){
           mDCAu[1]->Fill(r2);
           fDCAu[1]->Fill(r1);
          }
         }
         if(k==3){
          residVsTDCv->Fill(Matrix(matArr2,k,3),r1-r2);
          ResidVsTDCv[1]->Fill(Matrix(matArr2,k,3),r1-r2);
          residVsTDC->Fill(Matrix(matArr2,k,3),r1-r2);
          //cout<<"residVsTDC8: "<<Matrix(matArr2,k,3)<<" "<<res<<endl;
          ResidVsTDC[1]->Fill(Matrix(matArr2,k,3),r1-r2);
          res=abs(parFitL[0] + parFitL[1]*Matrix(matArr2,k,2)-Matrix(matArr2,k,4))-abs(Matrix(matArr2,k,0)-Matrix(matArr2,k,4));
          ResidVsTDCx_v[1]->Fill(Matrix(matArr2,k,3),res);
          res=abs(parFitL[2] + parFitL[3]*Matrix(matArr2,k,2)-Matrix(matArr2,k,5))-abs(Matrix(matArr2,k,1)-Matrix(matArr2,k,5));
          ResidVsTDCy_v[1]->Fill(Matrix(matArr2,k,3),res);
          //V[1]=sqrt2half*(Matrix(matArr2,k,1)+Matrix(matArr2,k,0));
          //X_V[1]=Matrix(matArr2,k,0);
          //Y_V[1]=Matrix(matArr2,k,1);
          //muV[1]=sqrt2half*(parFitL[2] + parFitL[3]*Matrix(matArr2,k,2) + parFitL[0] + parFitL[1]*Matrix(matArr2,k,2));
          if(checkDTM){
           mDCAv[1]->Fill(r2);
           fDCAv[1]->Fill(r1);
          }
         }
       }
      }//dch planes
     }//TracksDirectionTestReco
    }//nentries2
    delete dchtrgr;
  }//nentries1
}//if ntracks (tracksmatch)

      //cout<<"fitErrorcode2 = "<<fitErrorcode<<endl;
}
//_______________________________________________________________
//void DchTrackManager::FinishDch(UShort_t fDchNum, TGraph *calib[fDchNum-1]) {
//void DchTrackManager::FinishDch(TGraph *calibr,TH1D* &hResidRMSvsIter, UShort_t Iter, TString datatype) {
//void DchTrackManager::FinishDch(TGraph *calibr[],TH1D* &hResidRMSvsIter, UShort_t Iter, TString datatype) {
void DchTrackManager::FinishDch(TGraph *calibr_cham[],TGraph *calibr[][numLayers_half], UShort_t runNumber, TString outDirectory, TString residualsFileName, Double_t &locAngleX, Double_t &locAngleY, Bool_t &noError) {

if(noError){
//cout<<"RMS = "<<residVsTDC->GetRMS(2)<<" "<<iter<<endl;
locAngleX=trackAngleXaf->GetMean();
locAngleY=trackAngleYaf->GetMean();

Double_t par[3];
TH1D* projYresidVsTDC = (TH1D*)residVsTDC->ProjectionY("projYresidVsTDC");
projYresidVsTDC->Fit("gaus","q");
TF1 *myfit = (TF1*) projYresidVsTDC->GetFunction("gaus");
par[2]=myfit->GetParameter(2);
//Double_t RMS=residVsTDC->GetRMS(2);
//hResidRMSvsIter->Fill(iter,RMS);
hResidRMSvsIter->Fill(iter,par[2]);
TH1D* projYresidVsTDC1 = (TH1D*)ResidVsTDC[0]->ProjectionY("projYresidVsTDC1");
projYresidVsTDC1->Fit("gaus","q");
TF1 *myfit1 = (TF1*) projYresidVsTDC1->GetFunction("gaus");
par[2]=myfit1->GetParameter(2);
//RMS=ResidVsTDC[0]->GetRMS(2);
//hResidRMSvsIter0->Fill(iter,RMS);
hResidRMSvsIter0->Fill(iter,par[2]);
TH1D* projYresidVsTDC2 = (TH1D*)ResidVsTDC[1]->ProjectionY("projYresidVsTDC2");
projYresidVsTDC2->Fit("gaus","q");
TF1 *myfit2 = (TF1*) projYresidVsTDC2->GetFunction("gaus");
par[2]=myfit2->GetParameter(2);
//RMS=ResidVsTDC[1]->GetRMS(2);
//hResidRMSvsIter1->Fill(iter,RMS);
hResidRMSvsIter1->Fill(iter,par[2]);
if(strcmp(opt,"reconstruction")==0){
 Ssiz_t slength=residualsFileName.Length();
 residualsFileName.Insert(slength-5,"reconstruction");
}
TString outPut=outDirectory+residualsFileName;
//TFile *fdch  = new TFile(fResiduals, "RECREATE");
TFile *fdch  = new TFile(outPut, "RECREATE");
residVsTDCx->Write();
residVsTDCy->Write();
residVsTDCu->Write();
residVsTDCv->Write();
residVsTDC->Write();
//chi2linfit->Write();
trackAngleXaf->Write();
trackAngleYaf->Write();
trackAngleXbf1->Write();
trackAngleYbf1->Write();
trackAngleXbf2->Write();
trackAngleYbf2->Write();
//trckAngX2vs1->Write();
//trckAngY2vs1->Write();
//trackAngleX->Write();
//trackAngleY->Write();
//calibr->Write();
hResidRMSvsIter->Write();
hResidRMSvsIter0->Write();
hResidRMSvsIter1->Write();
for (Int_t i = 0; i < numChambers; ++i) {
 ResidVsTDCx[i]->Write();
 ResidVsTDCy[i]->Write();
 ResidVsTDCu[i]->Write();
 ResidVsTDCv[i]->Write();
 ResidVsTDCx_y[i]->Write();
 ResidVsTDCx_u[i]->Write();
 ResidVsTDCx_v[i]->Write();
 ResidVsTDCy_x[i]->Write();
 ResidVsTDCy_u[i]->Write();
 ResidVsTDCy_v[i]->Write();
 ResidVsTDC[i]->Write();
 calibr_cham[i]->Write();
 //if(iter>0){
  for (UShort_t k = 0; k < numLayers_half; ++k) {
   calibr[i][k]->Write();
  }
 //} 
 /*if(checkDTM){
  mDCAy[i]->Write();
  fDCAy[i]->Write();
  mDCAx[i]->Write();
  fDCAx[i]->Write();
  mDCAu[i]->Write();
  fDCAu[i]->Write();
  mDCAv[i]->Write();
  fDCAv[i]->Write();
  for (UShort_t k = 0; k < numLayers_half; ++k) {
   hDeltaX[i][k]->Write();
   hDeltaY[i][k]->Write();
  }
 }*/
}

/*ScalarD *Ranmin;
Ranmin->SetSV(rtcalibrange->X());
Ranmin->SetName("Ranmin");
cout<<"RANMIN1 = "<<rtcalibrange->X()<<endl;
cout<<"RANMIN2 = "<<Ranmin->GetSV()<<endl;
Ranmin->Write();
*/

//fdch->ls();
fdch->Close();
delete fdch;
//ResidVsTDCy->Delete();
Bool_t minFound=false;
if(iter>=3){
 //if(iter>=0){
 hResidRMSvsIter->Fit("pol2");
 TF1 *residfit = (TF1*) hResidRMSvsIter->GetFunction("pol2");
 Double_t minResX=residfit->GetMinimumX(0.,100.);
 cout<<"minResX = "<<minResX<<", iter = "<<iter<<endl;
 //if(minResX<Double_t(iter-1)||iter==itermax)minFound=true;
 //if(iter==itermax)minFound=true;
 if(minResX<Double_t(iter-1))minFound=true;
}
if(minFound==true){ 
 noError=false;
 cout<<"Minimum found when number of iterations reached "<<iter<<"."<<endl;
}
if(iter==itermax){
 noError=false;
 cout<<"Maximum number of iterations reached at "<<iter<<"."<<endl;
}
if(!noError&&strcmp(opt,"autocalibration")==0){
 cout << " DCH autocalibration and alignment done!" << endl;
 Ssiz_t slength=residualsFileName.Length();
 residualsFileName.Remove(slength-7,2);
 cout<<"residuals file name: "<<residualsFileName<<endl;
 TString outPut2=outDirectory+residualsFileName;
 rename(outPut,outPut2);
 cout<<"ultimate residuals file name: "<<outPut2<<endl;
}
}//if(noError)
//if(!noError){
 delete hResidRMSvsIter;delete hResidRMSvsIter0;delete hResidRMSvsIter1;
//}
for (Int_t i = 0; i < 2; ++i) {
 delete ResidVsTDCx[i];delete ResidVsTDCy[i];delete ResidVsTDCu[i];delete ResidVsTDCv[i];delete ResidVsTDC[i];
delete ResidVsTDCx_y[i];delete ResidVsTDCy_x[i];delete ResidVsTDCx_u[i];delete ResidVsTDCy_u[i];delete ResidVsTDCx_v[i];delete ResidVsTDCy_v[i];
//delete calibr_cham[i];
/* for (UShort_t k = 0; k < numLayers_half; ++k) {
  delete calibr[i][k];
 }*/
 //if(checkDTM){
  /*delete mDCAy[i]; delete fDCAy[i]; delete mDCAx[i]; delete fDCAx[i]; delete mDCAu[i]; delete fDCAu[i]; delete mDCAv[i]; delete fDCAv[i];
  for (UShort_t k = 0; k < 4; ++k) {
   delete hDeltaX[i][k];
   delete hDeltaY[i][k];
  }*/
  //if(Iter>0){
  //}
 //}
}
delete residVsTDCx;delete residVsTDCy;delete residVsTDCu;delete residVsTDCv;delete residVsTDC;
//delete chi2linfit; 
delete trackAngleXaf; delete trackAngleYaf; delete trackAngleXbf1; delete trackAngleYbf1; delete trackAngleXbf2; delete trackAngleYbf2; 
//delete trckAngX2vs1; delete trckAngY2vs1;
}
//_______________________________________________________________
/*void DchTrackManager::SetDchTrackHits(UInt_t n, Double_t x0, Double_t y0, Double_t z0, Double_t x3, Double_t y3, Double_t z3) {

TClonesArray &ffDchTrackManager = *fDchTrackManager;
TMatrix hits(2,3);
hits(0,0)=x0;hits(0,1)=y0;hits(0,2)=z0;
hits(1,0)=x3;hits(1,1)=y3;hits(1,2)=z3;
//hits.Print();
new (ffDchTrackManager[n]) TMatrix(hits);

}

//________________________________________________________________
void DchTrackManager::SetDchTrackManagerNumber(UInt_t n){

// setting number fo Dch track candidate

 nTrCand=n;

}
//________________________________________________________________
UInt_t DchTrackManager::GetDchTrackManagerNumber(){

// returns number of Dch track candidate

 return nTrCand;

}*/

//____________________________________________________________________
double DchTrackManager::Matrix(const float* matArr, const UInt_t k, const UInt_t l) {

return  *(matArr+(k*ncols)+l);

}

//____________________________________________________________________
void DchTrackManager::DchAverageLinearTrack(TGraph2DErrors* dchtrgr, Double_t *parFitL) {

   Double_t* fX=dchtrgr->GetX();  
   Double_t* fY=dchtrgr->GetY();
   Double_t* fZ=dchtrgr->GetZ();
   Double_t x1=*fX,x4=*(fX+3);  
   Double_t y1=*fY,y4=*(fY+3); 
   Double_t z1=*fZ,z4=*(fZ+3); 
   Double_t tgx1=(x4-x1)/(z4-z1); 
   Double_t tgy1=(y4-y1)/(z4-z1); 
   Double_t qx1=x1-tgx1*z1;
   Double_t qy1=y1-tgy1*z1;
   Double_t x5=*(fX+4),x8=*(fX+7);  
   Double_t y5=*(fY+4),y8=*(fY+7); 
   Double_t z5=*(fZ+4),z8=*(fZ+7); 
   Double_t tgx2=(x8-x5)/(z8-z5); 
   Double_t tgy2=(y8-y5)/(z8-z5); 
   Double_t qx2=x5-tgx2*z5;
   Double_t qy2=y5-tgy2*z5;
   //cout<<x1<<" "<<x4<<" "<<y1<<" "<<y4<<" "<<z1<<" "<<z4<<endl;
   //cout<<x5<<" "<<x8<<" "<<y5<<" "<<y8<<" "<<z5<<" "<<z8<<endl;
   //cout<<tgx1<<" "<<tgy1<<" "<<qx1<<" "<<qy1<<endl;
   //cout<<tgx2<<" "<<tgy2<<" "<<qx2<<" "<<qy2<<endl;

   Double_t xmiddle1=(x4+x1)/2.,xmiddle2=(x8+x5)/2.;
   Double_t ymiddle1=(y4+y1)/2.,ymiddle2=(y8+y5)/2.;
   Double_t zmiddle1=(z4+z1)/2.,zmiddle2=(z8+z5)/2.;

   //cout<<xmiddle1<<" "<<xmiddle2<<" "<<ymiddle1<<" "<<ymiddle2<<" "<<zmiddle1<<" "<<zmiddle2<<endl;

   /*Double_t xmiddleExtrap2to1=tgx2*zmiddle1+qx2;
   Double_t ymiddleExtrap2to1=tgy2*zmiddle1+qy2;
   Double_t xmiddleExtrap1to2=tgx1*zmiddle2+qx1;
   Double_t ymiddleExtrap1to2=tgy1*zmiddle2+qy1;

   cout<<xmiddleExtrap2to1<<" "<<ymiddleExtrap2to1<<" "<<xmiddleExtrap1to2<<" "<<ymiddleExtrap1to2<<endl;

   Double_t xaverage1=(xmiddle1+xmiddleExtrap2to1)/2.;
   Double_t yaverage1=(ymiddle1+ymiddleExtrap2to1)/2.;
   Double_t xaverage2=(xmiddle2+xmiddleExtrap1to2)/2.;
   Double_t yaverage2=(ymiddle2+ymiddleExtrap1to2)/2.;

   cout<<xaverage1<<" "<<xaverage2<<" "<<yaverage1<<" "<<yaverage2<<endl;
  
   parFitL[1]=(xaverage2-xaverage1)/(zmiddle2-zmiddle1); 
   parFitL[0]=xaverage1-parFitL[1]*zmiddle1;
   parFitL[3]=(yaverage2-yaverage1)/(zmiddle2-zmiddle1); 
   parFitL[2]=yaverage1-parFitL[3]*zmiddle1;*/
   parFitL[1]=(xmiddle2-xmiddle1)/(zmiddle2-zmiddle1); 
   parFitL[0]=xmiddle1-parFitL[1]*zmiddle1;
   parFitL[3]=(ymiddle2-ymiddle1)/(zmiddle2-zmiddle1); 
   parFitL[2]=ymiddle1-parFitL[3]*zmiddle1;
 

 
}

//____________________________________________________________________
//void DchTrackManager::DchLinearTrackFitter(TGraph2DErrors* dchtrgr, Double_t *parFitL, TH1D* &chi2linfitHist_, Int_t &fitErrorcode) {
void DchTrackManager::DchLinearTrackFitter(TGraph2DErrors* dchtrgr, Double_t *parFitL, Int_t &fitErrorcode) {

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
   pStart[0] = 1.;
   pStart[1] = 1.;
   pStart[2] = 1.;
   pStart[3] = 1.;
   //if(checkFit)cout<<" pStart[0] = "<<pStart[0]<<" pStart[1] = "<<pStart[1]<<" pStart[2] = "<<pStart[2]<<" pStart[3] = "<<pStart[3]<<endl;

   min->SetParameter(0,"x0",pStart[0],0.01,0,0);
   min->SetParameter(1,"tgx",pStart[1],0.01,0,0);
   min->SetParameter(2,"y0",pStart[2],0.01,0,0);
   min->SetParameter(3,"tgy",pStart[3],0.01,0,0);
    
   arglist[0] = 1000; // number of function calls 
   arglist[1] = 0.001; // tolerance 
   //min->ExecuteCommand("MIGRAD",arglist,2);
   //min->ExecuteCommand("MINO",arglist,2);
   Int_t errorcodeSim0 = min->ExecuteCommand("SIMPLEX",arglist,0);
   Int_t errorcodeMig0 = min->ExecuteCommand("MIGRAD",arglist,0);
   if(errorcodeSim0!=0)cout<<"errorcodeSim0 = "<<errorcodeSim0<<endl;
   //if(errorcodeMig0!=0)cout<<"errorcodeMig0 = "<<errorcodeMig0<<endl;
   fitErrorcode=errorcodeMig0;  
   if(errorcodeMig0!=0){
    //cout<<x1<<" "<<y1<<" "<<z1<<" "<<x2<<" "<<y2<<" "<<z2<<" "<<tgx<<" "<<tgy<<" "<<delX<<" "<<delY<<" "<<ax<<" "<<ay<<endl;
    }
   /*
    cout<<" pStart[0] = "<<pStart[0]<<" pStart[1] = "<<pStart[1]<<" pStart[2] = "<<pStart[2]<<" pStart[3] = "<<pStart[3]<<endl;
    min->SetParameter(0,"x0",pStart[0],0.1,0,0);
    min->SetParameter(1,"tgx",pStart[1],0.1,0,0);
    min->SetParameter(2,"y0",pStart[2],0.1,0,0);
    min->SetParameter(3,"tgy",pStart[3],0.1,0,0);
    Int_t errorcodeSim = min->ExecuteCommand("SIMPLEX",arglist,0);
    //Int_t errorcodeMino = min->ExecuteCommand("MINO",arglist,0);
    //Int_t errorcode2 = min->ExecuteCommand("MIGRAD",arglist,0);
    if(errorcodeSim!=0)cout<<"errorcodeSim = "<<errorcodeSim<<endl;
    //if(errorcodeMino!=0)cout<<"errorcodeMino = "<<errorcodeMino<<endl;
    //fitErrorcode=errorcodeMino;  
    fitErrorcode=errorcodeSim;  
   }*/

   if(errorcodeMig0!=0){
   for (Int_t i = 0; i < 8; ++i) {
   // cout<<"fXfYfZ "<<*(fX+i)<<" "<<*(fY+i)<<" "<<*(fZ+i)<<" "<<i<<" "<<endl;
   }
   Double_t X1_1=*fX,X2_1=*(fX+1),Y1_1=*fY,Y2_1=*(fY+1),Z1_1=*fZ,Z2_1=*(fZ+1);
   Double_t X1_2=*(fX+4),X2_2=*(fX+5),Y1_2=*(fY+4),Y2_2=*(fY+5),Z1_2=*(fZ+4),Z2_2=*(fZ+5);
   Double_t tgx_1=(X2_1-X1_1)/(Z2_1-Z1_1),tgx_2=(X2_2-X1_2)/(Z2_2-Z1_2);
   Double_t tgy_1=(Y2_1-Y1_1)/(Z2_1-Z1_1),tgy_2=(Y2_2-Y1_2)/(Z2_2-Z1_2);
   //cout<<"first line params X: "<<tgx_1<<" "<<-Z1_1*tgx_1+X1_1<<" "<<TMath::RadToDeg()*atan(tgx_1)<<endl;
   //cout<<"second line params X: "<<tgx_2<<" "<<-Z1_2*tgx_2+X1_2<<" "<<TMath::RadToDeg()*atan(tgx_2)<<endl;
   //cout<<"first line params Y: "<<tgy_1<<" "<<-Z1_1*tgy_1+Y1_1<<" "<<TMath::RadToDeg()*atan(tgy_1)<<endl;
   //cout<<"second line params Y: "<<tgy_2<<" "<<-Z1_2*tgy_2+Y1_2<<" "<<TMath::RadToDeg()*atan(tgy_2)<<endl;
   }

  //if (minos) min->ExecuteCommand("MINOS",arglist,0);
   Int_t nvpar,nparx; 
   Double_t amin,edm,errdef;
   min->GetStats(amin,edm,errdef,nvpar,nparx);
   if(checkFit)cout<<"line fit"<<endl;
   //min->PrintResults(1,amin);
   //cout<<"chi2= "<<chi2<<endl;
   if(checkFit)cout<<"chi2 = "<<amin<<" "<<edm<<" "<<errdef<<" "<<nvpar<<" "<<nparx<<endl;
   chi2linfitHist->Fill(amin);

   // get fit parameters
   //Double_t parFit[4];
   for (Int_t i = 0; i < 4; ++i) {
      parFitL[i] = min->GetParameter(i);
      //cout<<"parFitL "<<i<<" "<<parFitL[i]<<endl;
      //if(i==3&&TMath::RadToDeg()*atan(parFitL[i])>20.)cout<<"tgy too large!"<<endl;
   }
      //cout<<"fitErrorcode0 = "<<fitErrorcode<<endl;
}
//____________________________________________________________________
void DchTrackManager::FillAngles(const TGraph2DErrors* dchtrgr, const Double_t *parFitL){

    trackAngleXaf->Fill(TMath::RadToDeg()*atan(parFitL[1]));
    trackAngleYaf->Fill(TMath::RadToDeg()*atan(parFitL[3]));
    Double_t* fX=dchtrgr->GetX();
    Double_t* fY=dchtrgr->GetY();
    Double_t* fZ=dchtrgr->GetZ();
    Double_t X1_1=*fX,X2_1=*(fX+1),Y1_1=*fY,Y2_1=*(fY+1),Z1_1=*fZ,Z2_1=*(fZ+1);
    Double_t X1_2=*(fX+4),X2_2=*(fX+5),Y1_2=*(fY+4),Y2_2=*(fY+5),Z1_2=*(fZ+4),Z2_2=*(fZ+5);
    Double_t tgx_1=(X2_1-X1_1)/(Z2_1-Z1_1),tgx_2=(X2_2-X1_2)/(Z2_2-Z1_2);
    Double_t tgy_1=(Y2_1-Y1_1)/(Z2_1-Z1_1),tgy_2=(Y2_2-Y1_2)/(Z2_2-Z1_2);
    //cout<<"first line params X: "<<tgx_1<<" "<<-Z1_1*tgx_1+X1_1<<" "<<TMath::RadToDeg()*atan(tgx_1)<<endl;
    //cout<<"second line params X: "<<tgx_2<<" "<<-Z1_2*tgx_2+X1_2<<" "<<TMath::RadToDeg()*atan(tgx_2)<<endl;
    //cout<<"first line params Y: "<<tgy_1<<" "<<-Z1_1*tgy_1+Y1_1<<" "<<TMath::RadToDeg()*atan(tgy_1)<<endl;
    //cout<<"second line params Y: "<<tgy_2<<" "<<-Z1_2*tgy_2+Y1_2<<" "<<TMath::RadToDeg()*atan(tgy_2)<<endl;
    trackAngleXbf1->Fill(TMath::RadToDeg()*atan(tgx_1));
    trackAngleYbf1->Fill(TMath::RadToDeg()*atan(tgy_1));
    trackAngleXbf2->Fill(TMath::RadToDeg()*atan(tgx_2));
    trackAngleYbf2->Fill(TMath::RadToDeg()*atan(tgy_2));
    trckAngX2vs1->Fill(TMath::RadToDeg()*atan(tgx_1),TMath::RadToDeg()*atan(tgx_2));
    trckAngY2vs1->Fill(TMath::RadToDeg()*atan(tgy_1),TMath::RadToDeg()*atan(tgy_2));

}
//____________________________________________________________________
/*Double_t DchTrackManager::MagFieldIntegral(const Double_t *parFitL){

  Double_t integral=0.,x,y,z,z0=-250.,delz;//z0 -> z of target
  const Double_t zstep=1.;

  for (UShort_t i = 0; i < 1000; i++) {
   delz=zstep*Double_t(i);
   z=z0+delz;
   x=parFitL[0]+parFitL[1]*z;
   y=parFitL[2]+parFitL[3]*z;
   integral+=zstep*magfield->GetBy(x,y,z);
   cout<<"x,y,z: "<<x<<" "<<y<<" "<<z<<endl;
   cout<<"mag. field (By), integral: "<<magfield->GetBy(x,y,z)<<", "<<integral<<endl;
  }
  cout<<"mag. field integral = "<<integral<<endl;

return integral;

}*/
//---------------------------------------------------------------------------
Double_t DchTrackManager::PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz){

// x0,y0,z0 - point coordinates
// x,y,z - point on line coordinates, dircosx,dircosy,dircosz - direction cosines

    TVector3 pos0(x0,y0,z0);
    TVector3 pos(x,y,z);
    TVector3 dir(dircosx,dircosy,dircosz);
    TVector3 dif=pos-pos0;
    
    Double_t distance = ((dif.Cross(dir)).Mag())/dir.Mag();
    
    return distance;

}
//------------------------------------------------------------------------------
//DchTrackCand* BmnDchHitProducerCalib_exp::AddTrack(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer)
BmnDchTrack* DchTrackManager::AddTrack(const Int_t DCHtrID, const Int_t DCHtrIDglob, TClonesArray *pTrackCollection, const TVector3 linePointFirst, const TVector3 linePointLast, const Double_t slopeX1, const Double_t slopeY1, const Double_t slopeX2, const Double_t slopeY2, const Int_t eventnum, const TMatrixFSym covMat1, const TMatrixFSym covMat2, const TString whichDCH)
{
	BmnDchTrack *dchTrack	=  new ((*pTrackCollection)[DCHtrIDglob]) BmnDchTrack();
        cout<<"eventnumAddTrack = "<<eventnum<<endl;
        cout<<"trackId = "<<DCHtrID<<endl;
        dchTrack->SetTrackId(DCHtrID);
        dchTrack->SetEventID(eventnum);
        if(strcmp(whichDCH,"DCH1DCH2")==0){
         dchTrack->SetNHits(numLayers);
        }else{
         dchTrack->SetNHits(numLayers_half);
        } 
        FairTrackParam parFirst;
        parFirst.SetX(linePointFirst.X());
        parFirst.SetY(linePointFirst.Y());
        parFirst.SetZ(linePointFirst.Z());
        parFirst.SetTx(slopeX1);
        parFirst.SetTy(slopeY1);
        parFirst.SetCovMatrix(covMat1);
        dchTrack->SetParamFirst(parFirst);
        FairTrackParam parLast;
        parLast.SetX(linePointLast.X());
        parLast.SetY(linePointLast.Y());
        parLast.SetZ(linePointLast.Z());
        parLast.SetTx(slopeX2);
        parLast.SetTy(slopeY2);
        parLast.SetCovMatrix(covMat2);
        dchTrack->SetParamLast(parLast);
        dchTrack->SetLength(sqrt(pow(linePointFirst.X()-beamPos[0],2.)+pow(linePointFirst.Y()-beamPos[1],2.)+pow(linePointFirst.Z()-beamPos[2],2.)));
        Double_t interceptX1=parFirst.GetX()-slopeX1*parFirst.GetZ();
        Double_t interceptY1=parFirst.GetY()-slopeY1*parFirst.GetZ();
        dchTrack->SetB(sqrt(pow(interceptX1-slopeX1*beamPos[2]+beamPos[0],2.)+pow(interceptY1-slopeY1*beamPos[2]+beamPos[1],2.)));
        dchTrack->SetNDF(0);
        //cout<<interceptX<<" "<<slopeX<<" "<<interceptY<<" "<<slopeY<<" "<<trGlobId<<endl;
	//pTrack->AddLink(FairLink(1, pointIndex)); 
	//pTrack->AddLink(FairLink(2, trackIndex)); 
	
return dchTrack;
}
//____________________________________________________________________
