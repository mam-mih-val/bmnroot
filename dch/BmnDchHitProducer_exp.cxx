//------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <assert.h>

#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoMatrix.h>
#include <TMath.h>
#include <TRandom.h>
#include <TRandom2.h>
#include <TVector3.h>
#include <TVector2.h>
#include <TChain.h>
//#include <TSpectrum.h>
#include "TSpectrum.h"
#include "TRandom2.h"
#include "Math/WrappedTF1.h"
#include "Math/BrentRootFinder.h"
#include "BmnDch1.h"
#include "BmnDch2.h"
#include "BmnDchHit.h"
#include "BmnDch1Point.h"
#include "BmnDch2Point.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmStack.h"
#include "BmnDchHitProducer_exp.h"
#include "BmnDchRaw2Digit.h"
//#include "BmnDchDigit.h"
//#include <TObjectTable.h>

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
        const Double_t BmnDchHitProducer_exp::clusdens_Ar=25., BmnDchHitProducer_exp::clusdens_CO2=35.; // cluster density per track length - clusters/cm
        const Double_t BmnDchHitProducer_exp::meanpath_Ar=1./clusdens_Ar, BmnDchHitProducer_exp::meanpath_CO2=1./clusdens_CO2; // cluster density per track length - clusters/cm
        const Double_t BmnDchHitProducer_exp::meanpath=0.7*meanpath_Ar+0.3*meanpath_CO2; // for 70/30 ArCO2
        const Double_t BmnDchHitProducer_exp::clusmean=1./meanpath;
	//const Double_t BmnDchHitProducer_exp::gasDriftSpeed = 5.e-3; // 50 mkm/ns == 5.e-3 cm/ns 
	const Double_t BmnDchHitProducer_exp::gasDriftSpeed = 2.5e-3; // 25 mkm/ns == 2.5e-3 cm/ns 
	const Double_t BmnDchHitProducer_exp::wireDriftSpeed =  20; // 20 cm/ns
	const Double_t BmnDchHitProducer_exp::resolution =  15e-3; // 150 mkm  ==  0.015 cm
	const Double_t BmnDchHitProducer_exp::hitErr[3] = {resolution,resolution,0.};
	const Double_t BmnDchHitProducer_exp::WheelR_2 = pow(MaxRadiusOfActiveVolume,2.); // cm x cm

//------------------------------------------------------------------------------------------------------------------------
struct __ltstr
{
	bool operator()(Double_t s1, Double_t s2) const
  	{
    		return s1 > s2;
  	}
};

//------------------------------------------------------------------------------------------------------------------------
BmnDchHitProducer_exp::BmnDchHitProducer_exp(UInt_t num = 1, Int_t verbose, Bool_t test, TString runtype) : FairTask("Dch HitProducer", verbose), fDoTest(test), fRSigma(0.2000), fRPhiSigma(0.0200), fOnlyPrimary(kFALSE)
{
	pRandom = new TRandom2;
        fDchNum = num;
        runType = runtype;
        TString str;
        str.Form("%u",fDchNum);
        //fInputBranchName = TString("DCH") + str + TString("Point");
        fOutputHitsBranchName = "BmnDchHit";
        //fOutputHitsBranchName = "BmnDch"+str+"Hit";

}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHitProducer_exp::~BmnDchHitProducer_exp() 
{
	delete pRandom;	
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::InitDchParameters(){
  checkDch=kFALSE;
  //checkDch=kTRUE;
  checkGraphs=kFALSE;
  //checkGraphs=kTRUE;
  fDoOccupancy=kFALSE;
  //fDoOccupancy=kTRUE;
  //useCalib=kFALSE;
  //useCalib=kTRUE;
  //fAngleCorrectionFill=kFALSE;
  //fAngleCorrectionFill=kTRUE;
  eventNum=0; 
  neventsUsed=0;
 
  detXshift[0]=DCH1_Xpos[RunTypeToNumber(runType)];
  detXshift[1]=DCH2_Xpos[RunTypeToNumber(runType)];
  // DCH1 and DCH2 x shifts, cm


  for (UShort_t i = 0; i < numLayers; i++) {
    //if(fDchNum==1){DCH1_ZlayerPos_global[i]=DCH_ZlayerPos_local[i]+DCH1_Zpos[RunTypeToNumber(runType)];}
    //else if(fDchNum==2){DCH2_ZlayerPos_global[i]=DCH_ZlayerPos_local[i]+DCH2_Zpos[RunTypeToNumber(runType)];}
    DCH1_ZlayerPos_global[i]=DCH_ZlayerPos_local[i]+DCH1_Zpos[RunTypeToNumber(runType)];
    DCH2_ZlayerPos_global[i]=DCH_ZlayerPos_local[i]+DCH2_Zpos[RunTypeToNumber(runType)];
   }
 
  cout<<"run type number = "<<RunTypeToNumber(runType)<<endl; 
  
 
  for (UShort_t i = 0; i < 2; i++) {
   for (UShort_t j = 2; j < 4; j++) {
    for (UShort_t k = 4; k < 6; k++) {
       zCoeff1[i][GetGasGap(j)][GetGasGap(k)]=(DCH_ZlayerPos_local[k]-DCH_ZlayerPos_local[i])/(DCH_ZlayerPos_local[j]-DCH_ZlayerPos_local[i]);
       if(checkDch)cout<<"zCoeff1: "<<zCoeff1[i][GetGasGap(j)][GetGasGap(k)]<<endl;
    }
    for (UShort_t l = 6; l < numLayers; l++) {
       zCoeff2[i][GetGasGap(j)][GetGasGap(l)]=(DCH_ZlayerPos_local[l]-DCH_ZlayerPos_local[i])/(DCH_ZlayerPos_local[j]-DCH_ZlayerPos_local[i]);
       if(checkDch)cout<<"zCoeff2: "<<zCoeff2[i][GetGasGap(j)][GetGasGap(l)]<<endl;
    }
   }
  }
        //z3121=(DCH_ZlayerPos_local[4]-DCH_ZlayerPos_local[0])/(DCH_ZlayerPos_local[2]-DCH_ZlayerPos_local[0]);
        //z4121=(DCH_ZlayerPos_local[6]-DCH_ZlayerPos_local[0])/(DCH_ZlayerPos_local[2]-DCH_ZlayerPos_local[0]);
        //z4131=(DCH_ZlayerPos_local[6]-DCH_ZlayerPos_local[0])/(DCH_ZlayerPos_local[4]-DCH_ZlayerPos_local[0]);

  for (UShort_t k = 0; k < numLayers; k++) {
    //for (UInt_t i = 0; i < numChambers; i++) {
      if(fDchNum==1){
        zLayer[k]=DCH1_ZlayerPos_global[k]; 
        zLayerExtrap[k]=DCH2_ZlayerPos_global[k]; 
        //zgap[fDchNum-1][k/2]=(DCH1_ZlayerPos_global[k]+DCH1_ZlayerPos_global[k+2])/2.;
      }else if(fDchNum==2){
        zLayer[k]=DCH2_ZlayerPos_global[k]; 
        zLayerExtrap[k]=DCH1_ZlayerPos_global[k]; 
        //zgap[fDchNum-1][k/2]=(DCH2_ZlayerPos_global[k]+DCH2_ZlayerPos_global[k+2])/2.;
      }
    //}
  }

  //if(checkDch){
   cout<<"DCH = "<<fDchNum<<endl;
   cout<<"Z of DCH layer (lab coordinate frame): "<<endl;
   for (UShort_t k = 0; k < numLayers; k++) {
    if(fDchNum==1){
     cout<<DCH1_ZlayerPos_global[k]<<" ";
    }else if(fDchNum==2){
     cout<<DCH2_ZlayerPos_global[k]<<" ";
    }
   }
   cout<<endl;
   cout<<"DCH X shift = "<<detXshift[fDchNum-1]<<endl;
  //}
  //xyTolerance=3.*resolution; 
  //radialRange=2.; //cm (AuAu)
  radialRange=3.; //cm (CC)

}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::BookHistograms(){

                TString str;
                str.Form("%u",fDchNum);
                TString hXYhitName = TString("hXYhit") + str;
		hXYhit[fDchNum-1] = new TH2D(hXYhitName, "xy hit distribution", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume); 
		hXYhit[fDchNum-1]->SetDirectory(0); fList.Add(hXYhit[fDchNum-1]);			
                TString hXname = TString("hX") + str;
		hX[fDchNum-1] = new TH1D(hXname, "x coordinate", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume); 
		hX[fDchNum-1]->SetDirectory(0); fList.Add(hX[fDchNum-1]);			
                TString hYname = TString("hY") + str;
		hY[fDchNum-1] = new TH1D(hYname, "y coordinate", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume); 
		hY[fDchNum-1]->SetDirectory(0); fList.Add(hY[fDchNum-1]);			
                TString hZname = TString("hZ") + str;
		hZ[fDchNum-1] = new TH1D(hZname, "z coordinate", 1200, 540., 660.); 
		hZ[fDchNum-1]->SetDirectory(0); fList.Add(hZ[fDchNum-1]);			
                TString hRadiusRangeName = TString("hRadiusRange") + str;
		hRadiusRange[fDchNum-1] = new TH1D(hRadiusRangeName, "track radius range in next plane", 100, 0., 10.); 
		hRadiusRange[fDchNum-1]->SetDirectory(0); fList.Add(hRadiusRange[fDchNum-1]);		
		//hResolX = new TH1D("hResolX", "hit X coordinate resolution", 500, -5., 5.); 
		//hResolX->SetDirectory(0); fList.Add(hResolX);		
		//hResolY = new TH1D("hResolY", "hit Y coordinate resolution", 500, -5., 5.); 
		//hResolY->SetDirectory(0); fList.Add(hResolY);		
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::BookHistsAngleCorr(){

                TString str; 
                for (UShort_t i = 0; i < numLayers; i+=2) {
                 str.Form("%u",i);
                 TString name   = TString("hAngleVsWirepos") + str;
		 hAngleVsWirepos[i]= new TH2D(name, "angle vs wire position", numWiresPerLayer, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 180, -90., 90.); 		
		 hAngleVsWirepos[i]->SetDirectory(0); fList.Add(hAngleVsWirepos[i]);	
                }
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::FitHistsAngleCorr(){

     Double_t hMax,minval;
     Int_t nbinsX,nbinsY;
     
     TFile *corrfile; 
     if (fDchNum == 1) {corrfile = new TFile("test01.BmnDchHitProducer_exp.root","read");}
     else if (fDchNum == 2) {corrfile = new TFile("test02.BmnDchHitProducer_exp.root","read");}
    
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
//InitStatus 		BmnDchHitProducer_exp::Init() 
void	BmnDchHitProducer_exp::InitDch(TChain *bmnTree,TClonesArray *dchDigits,TTree* tReco ) 
{

        //cout << " BmnDchHitProducer_exp::Init() " << endl;

        TString str; 
        TString hOccuptitle="number of hits per plane in DCH",hOccupname="hoccup";
        TString minDriftTimeName,mindrifttimename="minDriftTime";
        str.Form("%u",fDchNum);
        hOccupname=hOccupname+str;
        hOccuptitle=hOccuptitle+str; 
        minDriftTimeName=mindrifttimename+str; 
        hOccup[fDchNum-1]  = new TH1D(hOccuptitle,hOccupname,20,0,20);
        InitDchParameters();
        if(fDoTest)BookHistograms(); 
	minDriftTime[fDchNum-1] = new TH1D(minDriftTimeName,"minimal drift times",900, 300.,1200.); 
        //if(fAngleCorrectionFill)BookHistsAngleCorr(); 
        //if(fAngleCorrectionFill==false)FitHistsAngleCorr();
        //if(useCalib)rtCalibration();
        rtCalibration(bmnTree,dchDigits);
        cout << " RT calibration done. " << endl;
        
        //if (fOnlyPrimary) cout << " Only primary particles are processed!!! " << endl;


    //Get ROOT Manager
        //FairRootManager* ioman = FairRootManager::Instance();

  	//if(ioman==0){ Error("BmnDchHitProducer_exp::Init","FairRootManager XUINJA"); return kERROR; }
        //fBmnDchPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
        //fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
  	//if(!fBmnDchPointsArray  || !fMCTracksArray){ Error("BmnDchHitProducer_exp::Init","Branch not found!"); return kERROR; }
  	// Create and register output array
  	//pHitCollection0 = new TClonesArray(fOutputHitsBranchName);  
  	
        pHitCollection = new TClonesArray(fOutputHitsBranchName);  

        TString OutputHitsBranchName = "BmnDch"+str+"Hit";
        //tReco->Branch("BmnDchHit", &pHitCollection); 
        tReco->Branch(OutputHitsBranchName, &pHitCollection); 
        str.Form("%u",fDchNum);
        TString str0; 
        str0.Form("%u",0);
        //TString name0   = TString("BmnDch") + str + TString("Hit") + str0;
        TString name   = TString("BmnDch") + str + TString("Hit");
        TString folder = TString("DCH") + str;
        //ioman->Register(name0.Data(), folder.Data(), pHitCollection0, kTRUE);
        //ioman->Register(name.Data(), folder.Data(), pHitCollection, kTRUE);

        cout << " Initialization finished succesfully. " << endl;
	
//return kSUCCESS;

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
Double_t BmnDchHitProducer_exp::GetDriftLength(UShort_t gasgap, Double_t x, Double_t& wirePos)
{
	//   ... -1	0	1	...  - first(0) gap wire  position (X) [cm]
	//   ...    -0.5   0.5     1.5  ...  - second(1) gap wire  position (X) [cm]
	
	wirePos = (gasgap == 0) ? TMath::Nint(x) : TMath::Nint(x + 0.5) - 0.5;
		
	//AZ return TMath::Abs(x - wirePos);	// [cm]
return x - wirePos;	// [cm]
}
//------------------------------------------------------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::wirePosition(UShort_t gasgap, UInt_t wirenum)
{

//wire position in local frame
        Double_t wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
        //Double_t wirePos = (gasgap == 0) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)-stepXYhalf);
        //Double_t wirePos = (gasgap == 1) ? (Double_t(wirenum)-Double_t(halfNumWiresPerLayer)):(Double_t(wirenum)-Double_t(halfNumWiresPerLayer)+stepXYhalf);
		
return wirePos;	// [cm]
}
//------------------------------------------------------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::GetDriftDist(FairMCPoint *dchPoint, UShort_t uidLocal, Double_t x, Double_t &wirePos){

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

dca=LineLineDCA(dchPoint->GetX()-detXshift[fDchNum-1], dchPoint->GetY(), dchPoint->GetZ(), dircosx1, dircosy1, dircosz1, xy2.X(), xy2.Y(), zLayer[uidLocal], dirCosLayerX[uidLocal], dirCosLayerY[uidLocal], 0., x2, y2, z2, xdchwmin, ydchwmin, zdchwmin, length, mu0, lambda0); 
cout<<dchPoint->GetX()<<" "<<dchPoint->GetY()<<" "<<dchPoint->GetZ()<<" "<<dchPoint->GetPx()/mom<<" "<<dchPoint->GetPy()/mom<<" "<<dchPoint->GetPz()/mom<<" "<<xy2.X()<<" "<<xy2.Y()<<" "<<zLayer[uidLocal]<<" "<<dirCosLayerX[uidLocal]<<" "<<dirCosLayerY[uidLocal]<<" "<<uidLocal<<endl;
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
   cout<<"dist to wire = "<<dist<<endl;  
}
cout<<"min dist to wire = "<<mindist<<endl; 
/*if(fDoTest){
  hDCA->Fill(dca); 
  hLength->Fill(length); 
  //hMinDriftTime->Fill(mindist/gasDriftSpeed); 
}*/

return dca;

}
//---------------------------------------------------------------------------

void BmnDchHitProducer_exp::rtCalibration(TChain *bmnTree, TClonesArray *dchDigits){

/*TFile *fdstread; 
if(fDchNum==1){
     fdstread = new TFile("/home/fedorisin/trunk/bmnroot/macro/run/test001.BmnDchHitProducer_exp_calib.root","read");
}else if(fDchNum==2){
     fdstread = new TFile("/home/fedorisin/trunk/bmnroot/macro/run/test002.BmnDchHitProducer_exp_calib.root","read");
}*/


    
    BmnDchDigit* digit = NULL;
    //TClonesArray *dchDigits;
    //bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);
    //cout<<bmnTree->GetEvent(5919)<<endl;

    nevents = bmnTree->GetEntries();
    cout << "nevents = " << nevents << endl;

    for (Int_t iev = 0; iev < nevents; iev++) {
        bmnTree->GetEntry(iev);
        //if(iev%1000==0)cout << "event number = " << iev << endl;
        Int_t nhits=dchDigits->GetEntriesFast();
        //cout<<"nhits in Dch = "<<nhits<<endl;
      for (Int_t ihit = 0; ihit < nhits; ihit++) {
        digit = (BmnDchDigit*) dchDigits->At(ihit);
        //cout<<"time = "<<digit->GetTime()<<endl;
        minDriftTime[fDchNum-1]->Fill(digit->GetTime());
      }
    } // event loop

    Int_t nbins=minDriftTime[fDchNum-1]->GetXaxis()->GetNbins();
    if(checkDch){
     for (Int_t ibin = 1; ibin <= nbins; ibin++) {
        cout<<"hist. time = "<<minDriftTime[fDchNum-1]->GetBinCenter(ibin)<<" "<<minDriftTime[fDchNum-1]->GetBinContent(ibin)<<endl;
     }
    }
//minDriftTime[fDchNum-1] = (TH1D*)fdstread->Get("hMinDriftTime");
//minDriftTime[fDchNum-1]->Draw();
//minDriftTime[fDchNum-1]->Smooth(2);
//minDriftTime[fDchNum-1]->Rebin();
minDriftTime[fDchNum-1]->Smooth();
/*TH1D* hMinDriftTimesmoothed = (TH1D*)minDriftTime[fDchNum-1]->Clone("hMinDriftTimesmoothed");
hMinDriftTimesmoothed->Smooth(20);
Int_t nbinsdth2=minDriftTime[fDchNum-1]->GetXaxis()->GetNbins();
for (Int_t j = 1; j <= nbinsdth2; j++) {
 minDriftTime[fDchNum-1]->SetBinError(j,fabs(minDriftTime[fDchNum-1]->GetBinContent(j)-hMinDriftTimesmoothed->GetBinContent(j)));
}
*/
//Int_t binmax = minDriftTime[fDchNum-1]->GetMaximumBin();
Double_t hmax = minDriftTime[fDchNum-1]->GetMaximum();
Int_t hmaxbin = minDriftTime[fDchNum-1]->GetMaximumBin();
Double_t hmin = minDriftTime[fDchNum-1]->GetMinimum();
minDriftTime[fDchNum-1]->GetXaxis()->SetRange(1,hmaxbin);
Int_t hminbin1 = minDriftTime[fDchNum-1]->GetMinimumBin();
minDriftTime[fDchNum-1]->GetXaxis()->SetRange(hmaxbin,nbins);
Int_t hminbin2 = minDriftTime[fDchNum-1]->GetMinimumBin();
minDriftTime[fDchNum-1]->GetXaxis()->SetRange(1,nbins);
Double_t hminx1 = minDriftTime[fDchNum-1]->GetXaxis()->GetBinCenter(hminbin1);
Double_t hminx2 = minDriftTime[fDchNum-1]->GetXaxis()->GetBinCenter(hminbin2);
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
   binc=minDriftTime[fDchNum-1]->GetBinContent(j);
   sum=sum+binc;
   jj++;
 }
}
Double_t averbincon=sum/Double_t(jj),upperlim=averbincon+5.*sqrt(averbincon);
Double_t bincon0,bincon1;
Int_t ranminbin;
Int_t i=hmaxbin-1;
do{
 bincon0=minDriftTime[fDchNum-1]->GetBinContent(i+1);
 bincon1=minDriftTime[fDchNum-1]->GetBinContent(i);
 //cout<<"ranmax "<<ranmaxbin<<" "<<hmaxtol<<" "<<i<<endl;
 i--; 
}while(!((bincon1>bincon0&&bincon1<upperlim)||UInt_t(bincon1)==0));
ranminbin=i+2;
Int_t nbinsUsed=hminbin2-ranminbin+1;
//Int_t nbinsUsed=hminbin2-hminbin1+1;
//ranmin=minDriftTime[fDchNum-1]->GetBinCenter(hminbin1)-0.5*(minDriftTime[fDchNum-1]->GetBinWidth(hminbin1)); 
ranmin=minDriftTime[fDchNum-1]->GetBinCenter(ranminbin)-0.5*(minDriftTime[fDchNum-1]->GetBinWidth(ranminbin)); 
ranmax=minDriftTime[fDchNum-1]->GetBinCenter(hminbin2)+0.5*(minDriftTime[fDchNum-1]->GetBinWidth(hminbin2)); 
Double_t timeRange=ranmax-ranmin;
//spline3 = new TSpline3(minDriftTime[fDchNum-1], "", 0., 0.);
spline5[fDchNum-1] = new TSpline5(minDriftTime[fDchNum-1], "", 0.,0.,0.,0.);
spline5[fDchNum-1]->SetTitle("TDC");
spline5[fDchNum-1]->SetName("tdc");
//cout<<ranminbin<<" "<<ranmaxbin<<endl;
cout<<"ranminbin = "<<ranminbin<<endl;
cout<<"time interval: "<<ranmin<<" "<<ranmax<<endl;
const UInt_t nintervals=40,npoints=nintervals+1;
Double_t deldt=(ranmax-ranmin)/Double_t(nintervals);
Double_t dt[npoints],r[npoints];
//Double_t norm=diagonal/(fitdt->Integral(0.,ranmax));
//Double_t norm=stepXYhalf/(fitdt->Integral(0.,ranmax));
Double_t norm=stepXYhalf/SplineIntegral(ranmin,ranmax,nbinsUsed*5);
for (Int_t j = 0; j <= nintervals; j++) {
 dt[j]=deldt*Double_t(j);
 //r[j]=norm*(fitdt->Integral(dt[0],dt[j]));
 r[j]=norm*SplineIntegral(dt[0]+ranmin,dt[j]+ranmin,5*TMath::Nint(Double_t(nbinsUsed)*(dt[j]-dt[0])/timeRange));
 if(checkDch)cout<<"calibration curve "<<dt[j]<<" "<<r[j]<<" "<<SplineIntegral(dt[0]+ranmin,dt[j]+ranmin,5*TMath::Nint(Double_t(nbinsUsed)*(dt[j]-dt[0])/timeRange))<<endl;
}
calib[fDchNum-1] = new TGraph(npoints,dt,r);
//spline3rt = new TSpline3("r-t calibration curve",calib, "", 0., 0.);
spline5rt[fDchNum-1] = new TSpline5("r-t calibration curve",calib[fDchNum-1], "", 0.,0.,0.,0.);
//calib->Fit("pol5");
calib[fDchNum-1]->SetTitle("r-t calibration curve");
calib[fDchNum-1]->SetName("rtCalibCurve");
spline5rt[fDchNum-1]->SetName("rtCalibSpline");
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
Double_t inflX1,inflX2;
tdcInflexPoints(inflX1,inflX2);
cout<<"inflex points (left,right) = "<<inflX1<<", "<<inflX2<<endl;


}

//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::tdcInflexPoints(Double_t &inflX1,Double_t &inflX2){

Int_t ngrpoints=minDriftTime[fDchNum-1]->GetNbinsX();
//Double_t xrangemin=minDriftTime[fDchNum-1]->GetXaxis()->GetXmin(); 
//Double_t xrangemax=minDriftTime[fDchNum-1]->GetXaxis()->GetXmax(); 
//Double_t binwidth=(xrangemax-xrangemin)/Double_t(ngrpoints);
Double_t binwidth=minDriftTime[fDchNum-1]->GetBinWidth(1);
Double_t halfbinwidth=0.5*binwidth;
Double_t xstart=minDriftTime[fDchNum-1]->GetBinCenter(1);
Double_t xspl[ngrpoints],yspl[ngrpoints];
for (Int_t ival = 0; ival < ngrpoints; ival++) {
xspl[ival]=xstart+Double_t(ival)*binwidth;
yspl[ival]=spline5[fDchNum-1]->Derivative(xspl[ival]);
}
splinederiv[fDchNum-1] = new TGraph(ngrpoints,xspl,yspl);
splinederiv[fDchNum-1]->SetTitle("derivative of r-t calibration curve");
splinederiv[fDchNum-1]->SetName("rtCalibCurve derivative");
TString str,spldersm="splinederivsmooth";
str.Form("%u",fDchNum);
TString splineDerivSmooth = spldersm + str;
splinederivsmooth[fDchNum-1] = new TH1D(splineDerivSmooth,"smoothed hist of spline derivatives",ngrpoints,xspl[0]-halfbinwidth,xspl[ngrpoints-1]+halfbinwidth);
for (Int_t ival = 0; ival < ngrpoints; ival++) {
splinederivsmooth[fDchNum-1]->SetBinContent(ival+1,yspl[ival]);
}
splinederivsmooth[fDchNum-1]->Rebin();
splinederivsmooth[fDchNum-1]->Smooth(1000);
Int_t hmaxbin_splinederiv = splinederivsmooth[fDchNum-1]->GetMaximumBin();
inflX1 = splinederivsmooth[fDchNum-1]->GetBinCenter(hmaxbin_splinederiv); //left inflex point
//Int_t hminbin_splinederiv = splinederivsmooth[fDchNum-1]->GetMinimumBin();
TH1D *hist_new=(TH1D*)splinederivsmooth[fDchNum-1]->Clone();
hist_new->Scale(-1.);
TSpectrum *s = new TSpectrum(10,1.);
Int_t nfound = s->Search(hist_new,2,"nobackground");
Float_t *xpeaks = s->GetPositionX();
inflX2 = Double_t(xpeaks[1]); //right inflex point

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
Double_t		BmnDchHitProducer_exp::GetTShift(Double_t driftLength, Double_t wirePos, Double_t R, Double_t& L)
{

	driftLength = driftLength > 0 ? driftLength : -1.*driftLength;
	L = sqrt(WheelR_2 -  wirePos*wirePos); 	 // half wire length
	if(wirePos > -MinRadiusOfActiveVolume && wirePos < MinRadiusOfActiveVolume)	L = L - TMath::Abs(R);// two wires 
	else					L = L + R; // one wire 
        //cout<<"L = "<<L<<" "<<R<<endl;
	
//cout<<"\n t1="<<driftLength / gasDriftSpeed<<"  ("<<driftLength<<") t2="<<(wireLength + R) / wireDriftSpeed<<" L="<<wireLength<<" Y="<<R;
//cout<<"drift time (gas+wire) "<<driftLength/gasDriftSpeed<<" "<<L/wireDriftSpeed<<" "<<driftLength<<" "<<L<<endl;	
return driftLength / gasDriftSpeed + L / wireDriftSpeed;
}
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
void 			BmnDchHitProducer_exp::ExecDch(Int_t iev, TClonesArray *dchDigits) 
{
  	pHitCollection->Delete();
        
        //gObjectTable->Print();
  
        if(checkGraphs){	
         hXYZcombhits = new TGraph2D(); 
         hXYZcombhits->SetNameTitle("hXYZcombhits","Hits in DCH");
         TString str;
         str.Form("%d",iev);
         TString hXYZcombhitsName = TString("hXYZcombhits") + str;
         hXYZcombhits->SetName(hXYZcombhitsName);
        }
	Int_t           hitID = 0;
        UInt_t           ijkl[numLayers];
	fMapOccup.clear();
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
          for (UShort_t k = numLayers; k < numLayers; k++) {
             dchhitplane[k] = new DchHitPlane();
          }

        if(checkDch)cout <<"BmnDchHitProducer_exp::ExecDch(): "<<" event number = "<<eventNum++<<", chamber = "<<fDchNum<<endl;

	BmnDchHit	*pHit;
	Double_t 	drifttimemin;
        UShort_t   	uid, uidLocal, wheel, gasgap, proj;
        Int_t   	detID;
        UInt_t   	hitWire;
        cout.precision(5);

	BmnDchHit 	*dchHit = NULL;
        BmnDchDigit* digit = NULL;
        Int_t nDchHit=dchDigits->GetEntriesFast();
        if(checkDch)cout<<"Number of digits = "<<nDchHit<<endl;
        if(nDchHit>0)neventsUsed++;

	for(Int_t i = 0; i < nDchHit; i++ )  // <---Loop over the DCH hits
	{
                digit = (BmnDchDigit*) dchDigits->At(i);
                uid=UShort_t(digit->GetPlane());
                if(fDchNum==1&&uid>=numLayers)continue;
                if(fDchNum==2&&uid<numLayers)continue;
                uid<numLayers?uidLocal=uid:uidLocal=uid-numLayers;  
                //uidLocal%2==0?(hitWire=UInt_t(Int_t(dchHit->GetWirePosition())+Int_t(halfNumWiresPerLayer))):(hitWire=UInt_t(Int_t(dchHit->GetWirePosition()-0.5)+Int_t(halfNumWiresPerLayer)));
                hitWire=UInt_t(digit->GetWireNumber()); 
		gasgap = GetGasGap(uid); 
                if(checkDch)cout<<"hitWire = "<<hitWire<<" "<<wirePosition(gasgap,hitWire)<<", uidLocal = "<<uidLocal<<endl; 
                //cout<<" angle wire position correction = "<<dchHit->GetAngleCorrWirePos()<<endl;
                //uidLocal%2==0?(hitWire=UInt_t(Int_t(dchHit->GetWirePosition())+Int_t(halfNumWiresPerLayer))):(hitWire=UInt_t(Int_t(dchHit->GetAngleCorrWirePos()-0.5)+Int_t(halfNumWiresPerLayer)));
                //cout<<"hitWire = "<<hitWire<<" "<<dchHit->GetAngleCorrWirePos()<<", uidLocal = "<<uidLocal<<", trackId = "<<dchHit->GetTrackID()<<endl; 
                uid<numLayers?detID=detIdDch1:detID=detIdDch2;  
		if(wireUsed[uidLocal][hitWire]==false)wireUsed[uidLocal][hitWire]=true; 
                dchhitplane[uidLocal]->SetDchPlaneHit(ijkl[uidLocal]++, wirePosition(gasgap,hitWire),0.,0,detID,hitWire,rtCurve(digit->GetTime()),i);
                //cout<<" drift distance = "<<fitrt->Eval(digit->GetTime()-timeOffset)<<endl;
                //cout<<" hits "<<ijkl[uidLocal]<<" "<<fDchNum-1<<" "<<uidLocal<<endl;
                //cout<<" hit position (global)(x,y,z): "<<dchHit->GetX()<<" "<<dchHit->GetY()<<" "<<dchHit->GetZ()<<endl;
        }

        //for (UShort_t j = 0; j < numChambers; j++) {
          for (UShort_t k = 0; k < numLayers; k++) {
             dchhitplane[k]->SetDchPlaneHitsNumber(ijkl[k],k);
	     if(ijkl[k]>0)planeUsed[k]=true; 
             if(checkDch)cout<<"number of hits in plane "<<k<<" of drift chamber "<<fDchNum<<" = "<<ijkl[k]<<endl;
             if(fDoOccupancy)hOccup[fDchNum]->Fill(ijkl[k]);
             //ijkl[j][k] = 0;
          }
        //}
       

        UShort_t topol=PlanesTopology(); 
        if(topol<2){
        //if(topol==0){
         HitFinder();
        }else{
         cout<<"Not enough hits in DCH! Hits not produced!"<<endl; 
        }
        
        if(checkGraphs){
         gList.Add(hXYZcombhits); 
        }
	
        //for (UShort_t j = 0; j < numChambers; j++) {
          for (UShort_t k = 0; k < numLayers; k++) {
             delete dchhitplane[k];
             dchhitplane[k] = 0;
          }
        //}
       //gObjectTable->Print();
}

//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::HitFinder(){ 

  Double_t x[numLayers/2],y[numLayers/2];
  UInt_t numPlaneHits[numLayers];
  //const UInt_t ijk=0;
  UInt_t trcand=0;
  Int_t hitID=0;
  Double_t hittmp[2],hitx2[2],hity1[2],hitq3[2],hitq4[2];
  Double_t y1,x2,q3,q4; 
  TVector3 pos, dpos;	
  //BmnDchHit *dchCombHit;
  UShort_t nhits[numLayers];
  UShort_t ijk[numLayers/2]; //index of main plane in pair
  Int_t jjgr2=0;
  //FairMCPoint     *dchPoint;
  ScalarD* hitX[numLayers];ScalarI* trackId[numLayers];ScalarI* hitId[numLayers];ScalarI* detId[numLayers];ScalarUI* hitwire[numLayers];ScalarD* driftlen[numLayers];ScalarI* pointind[numLayers];
 
           
          for(UShort_t i = 0; i < numLayers; i+=2) {
           if(planeUsed[i]){
             ijk[i/2]=i;
           }else if ((!planeUsed[i])&&planeUsed[i+1]){
             ijk[i/2]=i+1;
           }
          }

          numPlaneHits[ijk[0]]=dchhitplane[ijk[0]]->GetDchPlaneHitsNumber(ijk[0]); //hits in plane k
          //cout<<" numPlaneHits0 = "<<numPlaneHits0<<", numLayer = "<<k<<", drift chamber = "<<fDchNum<<endl;
          for (UInt_t i = 0; i < numPlaneHits[ijk[0]]; i++) {
              ReturnPointers(ijk[0],i,hitX[ijk[0]],trackId[ijk[0]],detId[ijk[0]],hitwire[ijk[0]],driftlen[ijk[0]],pointind[ijk[0]]);
              UInt_t hw0=hitwire[ijk[0]]->GetSV();
              Double_t driftdist0=driftlen[ijk[0]]->GetSV();
              LRambiguity(ijk[0],hittmp,hitX[ijk[0]]->GetSV(),driftdist0,hw0,nhits[ijk[0]]);
              for (UInt_t ii = 0; ii < nhits[ijk[0]]; ii++) {
                hity1[ii]=hittmp[ii];
                if(checkDch)cout<<"hittmp0: "<<hittmp[ii]<<" "<<ii<<endl;
              }
              numPlaneHits[ijk[1]]=dchhitplane[ijk[1]]->GetDchPlaneHitsNumber(ijk[1]); //hits in plane 2
              for (UInt_t j = 0; j < numPlaneHits[ijk[1]]; j++) {
               ReturnPointers(ijk[1],j,hitX[ijk[1]],trackId[ijk[1]],detId[ijk[1]],hitwire[ijk[1]],driftlen[ijk[1]],pointind[ijk[1]]);
               UInt_t hw2=hitwire[ijk[1]]->GetSV();
               Double_t driftdist2=driftlen[ijk[1]]->GetSV();
               LRambiguity(ijk[1],hittmp,hitX[ijk[1]]->GetSV(),driftdist2,hw2,nhits[ijk[1]]);
               for (UInt_t ii = 0; ii < nhits[ijk[1]]; ii++) {
                 hitx2[ii]=hittmp[ii];
                 if(checkDch)cout<<"hittmp1: "<<hittmp[ii]<<" "<<ii<<endl;
               }
               numPlaneHits[ijk[2]]=dchhitplane[ijk[2]]->GetDchPlaneHitsNumber(ijk[2]); //hits in plane 4 
               for (UInt_t k = 0; k < numPlaneHits[ijk[2]]; k++) {
                ReturnPointers(ijk[2],k,hitX[ijk[2]],trackId[ijk[2]],detId[ijk[2]],hitwire[ijk[2]],driftlen[ijk[2]],pointind[ijk[2]]);
                UInt_t hw4=hitwire[ijk[2]]->GetSV();
                Double_t driftdist4=driftlen[ijk[2]]->GetSV();
                LRambiguity(ijk[2],hittmp,hitX[ijk[2]]->GetSV(),driftdist4,hw4,nhits[ijk[2]]);
                for (UInt_t ii = 0; ii < nhits[ijk[2]]; ii++) {
                  hitq3[ii]=hittmp[ii]*sqrt2;
                  //if(checkDch)cout<<"hittmp2: "<<hittmp[ii]<<" "<<ii<<endl;
                  if(checkDch)cout<<"hittmp2: "<<hittmp[ii]<<" "<<ii<<", hitX = "<<hitX[ijk[2]]->GetSV()<<", driftdist = "<<driftdist4<<endl;
                }
                numPlaneHits[ijk[3]]=dchhitplane[ijk[3]]->GetDchPlaneHitsNumber(ijk[3]); //hits in plane 6 
                 for (UInt_t l = 0; l < numPlaneHits[ijk[3]]; l++) {
                  ReturnPointers(ijk[3],l,hitX[ijk[3]],trackId[ijk[3]],detId[ijk[3]],hitwire[ijk[3]],driftlen[ijk[3]],pointind[ijk[3]]);
                  UInt_t hw6=hitwire[ijk[3]]->GetSV();
                  Double_t driftdist6=driftlen[ijk[3]]->GetSV();
                  LRambiguity(ijk[3],hittmp,hitX[ijk[3]]->GetSV(),driftdist6,hw6,nhits[ijk[3]]);
                  for (UInt_t ii = 0; ii < nhits[ijk[3]]; ii++) {
                    hitq4[ii]=hittmp[ii]*sqrt2;
                    if(checkDch)cout<<"hittmp3: "<<hittmp[ii]<<" "<<ii<<endl;
                  }
                  for (UInt_t m = 0; m < nhits[ijk[0]]; m++) {
                   y1=hity1[m];
                   for (UInt_t n = 0; n < nhits[ijk[1]]; n++) {
                    x2=hitx2[n];
                    for (UInt_t mm = 0; mm < nhits[ijk[2]]; mm++) {
                     q3=hitq3[mm];
                     for (UInt_t nn = 0; nn < nhits[ijk[3]]; nn++) {
                      q4=hitq4[nn];
                      CoordinateFinder(y1,x2,tg3,q3,tg4,q4,ijk,x,y); 
                      //cout<<"------------------------------------------------------"<<endl; 
                       Bool_t radcond=true;
                       if(sqrt(pow(x[1]-x[0],2.)+pow(y[1]-y[0],2.))>radialRange)radcond=false;
                       //cout<<"radcond = "<<radcond<<" "<<1<<" "<<sqrt(pow(x[1]-x[0],2.)+pow(y[1]-y[0],2.))<<endl;
                       if(radcond){
                        for (UShort_t jk = 0; jk < numLayers/2; jk++) {
                         if(sqrt(pow(x[jk],2.)+pow(y[jk],2.))>MinRadiusOfActiveVolume&&sqrt(pow(x[jk],2.)+pow(y[jk],2.))<MaxRadiusOfActiveVolume){
	                  if(fDoTest){if(jk==3)hXYhit[fDchNum-1]->Fill(x[jk],y[jk]);}
                          //Double_t dX = pRandom->Gaus(0., hitErr[0]);	
                          //Double_t dY = pRandom->Gaus(0., hitErr[1]);	
	                  //pos.SetXYZ(x[jk]+detXshift[fDchNum-1]+dX, y[jk]+dY,zLayer[2*jk]);
	                  pos.SetXYZ(x[jk]+detXshift[fDchNum-1], y[jk],zLayer[ijk[jk]]);
		          //dpos.SetXYZ(dX,dY,hitErr[2]); 
		          dpos.SetXYZ(hitErr[0],hitErr[1],hitErr[2]); 
                          //if(fDoTest||checkDch)dchPoint = (FairMCPoint*) fBmnDchPointsArray->At(pointind[2*jk]->GetSV());
                          //if(fDoTest){ 
                           //hResolX->Fill(x[jk]+detXshift[fDchNum-1]-dchPoint->GetX());
                           //hResolY->Fill(y[jk]-dchPoint->GetY());
                           //hResolR->Fill(sqrt(pow(x[jk]+detXshift[fDchNum-1],2.)+pow(y[jk],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.)));
                          //}
                          //if(checkGraphs)hXYZcombhits->SetPoint(jjgr2++,zLayer[2*jk],x[jk],y[jk]);
                          if(checkGraphs){
                           hXYZcombhits->SetPoint(jjgr2++,zLayer[ijk[jk]],x[jk]+detXshift[fDchNum-1],y[jk]);
                           //cout<<"hits(x,y,z): "<<x[jk]<<", "<<y[jk]<<", "<<zLayer[ijk[jk]]<<", hit number = "<<jjgr2<<endl;
                           if(jk==3)ExtrapToDch(x,y,zLayer,ijk,jjgr2);
                          }
                          if(checkDch){
                           //cout<<"hits(x,y,z): "<<x[jk]<<", "<<y[jk]<<", "<<zLayer[2*jk]<<", hit number = "<<jjgr2<<endl;
                           cout<<"hits(x,y,z): "<<x[jk]<<", "<<y[jk]<<", "<<zLayer[ijk[jk]]<<", hit number = "<<jjgr2<<endl;
                           //if(fabs(x[jk]+detXshift[fDchNum-1]-dchPoint->GetX())<0.015&&fabs(y[jk]-dchPoint->GetY())<0.015) {
                           //cout<<" event = "<<eventNum<<", chamber = "<<fDchNum<<endl;
                           if(jk==3){
                            cout<<"nhits0 = "<<nhits[0]<<", nhits2 = "<<nhits[2]<<", nhits4 = "<<nhits[4]<<", nhits6 = "<<nhits[6]<<endl; 
                            cout<<"------------------------------------------------------"<<endl; 
                           }
                           //}
                          }
                          /*if(jk==3){
                            cout<<"trId0 = "<<trackId[0]->GetSV()<<", trId2 = "<<trackId[2]->GetSV()<<", trId4 = "<<trackId[4]->GetSV()<<", trId6 = "<<trackId[6]->GetSV()<<", chamber = "<<fDchNum<<endl; 
                            if(trackId[0]->GetSV()==trackId[2]->GetSV()&&trackId[2]->GetSV()==trackId[4]->GetSV()&&trackId[4]->GetSV()==trackId[6]->GetSV()){
                             for (UShort_t jkk = 0; jkk < numLayers/2; jkk++) {
                              dchPoint = (FairMCPoint*) fBmnDchPointsArray->At(pointind[2*jkk]->GetSV());
                              cout<<" event = "<<eventNum<<", track cand. = "<<trackId[2*jkk]->GetSV()<<", chamber = "<<fDchNum<<", difference2 (x,y,r): "<<x[jkk]+detXshift[fDchNum-1]-dchPoint->GetX()<<" "<<y[jkk]-dchPoint->GetY()<<" "<<sqrt(pow(x[jkk]+detXshift[fDchNum-1],2.)+pow(y[jkk],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.))<<endl;
                              if(fDoTest){ 
                               hResolX->Fill(x[jkk]+detXshift[fDchNum-1]-dchPoint->GetX());
                               hResolY->Fill(y[jkk]-dchPoint->GetY());
                               hResolR->Fill(sqrt(pow(x[jkk]+detXshift[fDchNum-1],2.)+pow(y[jkk],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.)));
                              }
                             }
                            }
                          }*/
                          //hitID++;
	                  //dchCombHit = AddHit(hitID++, detId[2*jk]->GetSV(), pos, dpos, trackId[2*jk]->GetSV(), pointind[2*jk]->GetSV(), 0, jk*2);
                          //cout<<"I am writing hit number "<<hitID<<endl;
                          if(checkDch)cout<<"detID = "<<detId[ijk[jk]]->GetSV()<<", layer = "<<ijk[jk]<<endl;
	                  dchCombHit = AddHit(hitID++, detId[ijk[jk]]->GetSV(), pos, dpos, trcand, 0, 0, ijk[jk]);
	                  if(jk==3)++trcand;
                          dchCombHit->SetDchId(Short_t(fDchNum));
                          //cout<<"Detector ID ="<<dchCombHit->GetDchId()<<" "<<dchCombHit->GetDetectorID()<<endl;	
                         }// radii test
                        }// layers
                       }// radcond
                       //cout<<"hitID = "<<hitID<<" "<<k<<" "<<i<<" "<<j<<endl;
                        
                       //cout<<"trackId = "<<dchCombHit->GetTrackID()<<", hitId = "<<dchCombHit->GetRefIndex()<<endl;
                       //cout<<"hitID = "<<hitID<<" "<<k<<" "<<i<<" "<<j<<endl;
                       //cout<<"x = "<<dchPoint->GetX()-detXshift[fDchNum-1]<<", y = "<<dchPoint->GetY()<<endl;
                     } // nn loop 
                    } // mm loop 
                   } // n loop 
                  } // m loop 
                 } // l loop 
               } // k loop 
              } // j loop 
          } // i loop 


       if(checkDch)cout<<"Number of DCH hit candidates = "<<hitID<<endl;
       if(checkDch)cout<<"Number of DCH track candidates = "<<trcand<<endl;
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
void		BmnDchHitProducer_exp::FinishDch()
{
        TString str;
        str.Form("%u",fDchNum);
        fOutputTestFileName = TString("test") + str + TString(".BmnDchHitProducer_exp.root");
	TFile file(fOutputTestFileName, "RECREATE");
        if(fDoOccupancy){
         assert(neventsUsed!=0);
         hOccup[fDchNum-1]->Scale(1./(Double_t(neventsUsed)*Double_t(numLayers)));
         cout<<"neventsUsed = "<<neventsUsed<<endl;
	 hOccup[fDchNum-1]->Write();
        }       
        if(checkGraphs)gList.Write(); 	
        minDriftTime[fDchNum-1]->Write();
        calib[fDchNum-1]->Write();
        //fitdt->Write();
        spline5[fDchNum-1]->Write();
        spline5rt[fDchNum-1]->Write();
        splinederiv[fDchNum-1]->Write();
        splinederivsmooth[fDchNum-1]->Write();
        //fitrt->Write();

      	file.Close();
}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHit* BmnDchHitProducer_exp::AddHit0(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer)
{
	BmnDchHit *pHit	=  new ((*pHitCollection0)[index]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	pHit->AddLink(FairLink(1, pointIndex)); 
	pHit->AddLink(FairLink(2, trackIndex)); 
	
return pHit;
}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHit* BmnDchHitProducer_exp::AddHit(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer)
{
	//BmnDchHit *pHit	=  new ((*pHitCollection)[index]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	TClonesArray& clref = *pHitCollection;
        Int_t size = clref.GetEntriesFast();
        BmnDchHit *pHit = new(clref[size]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	//pHit->AddLink(FairLink(1, pointIndex)); 
	//pHit->AddLink(FairLink(2, trackIndex)); 
return pHit;
}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::RadialRange(FairMCPoint* dchPoint){

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
    hRadiusRange[fDchNum-1]->Fill(r);
    //cout<<"radius in next plane = "<<r<<endl;

}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::LRambiguity(const UInt_t k, Double_t hittmp[2], const Double_t hitx, const Double_t driftdist, const UInt_t hw, UShort_t &nhits){

UInt_t kk;
k%2==0?kk=k+1:kk=k-1;

              if(hw!=0&&hw!=numWiresPerLayer){
               //cout<<"wireUsed: "<<wireUsed[kk][hw]<<" "<<wireUsed[kk][hw-1]<<" "<<hw<<endl;
               if(wireUsed[kk][hw]&&wireUsed[kk][hw-1]){
                 nhits=2; 
                 hittmp[0]=hitx-driftdist;
                 hittmp[1]=hitx+driftdist;
                 //cout<<"here1"<<endl;
               }else if(!wireUsed[kk][hw]&&!wireUsed[kk][hw-1]){
                 nhits=2; 
                 hittmp[0]=hitx-driftdist;
                 hittmp[1]=hitx+driftdist;
                 //cout<<"here2"<<endl;
               }else if(!wireUsed[kk][hw]&&wireUsed[kk][hw-1]){
                 hittmp[0]=hitx-driftdist;
                 nhits=1; 
                 //cout<<"here3"<<endl;
               }else if(wireUsed[kk][hw]&&!wireUsed[kk][hw-1]){
                 hittmp[0]=hitx+driftdist;
                 nhits=1; 
                 //cout<<"here4"<<endl;
               }
              }else if(hw==0){
               nhits=1; 
               hittmp[0]=hitx+driftdist;
               //cout<<"here5"<<endl;
              }else if(hw==numWiresPerLayer){ 
               nhits=1; 
               hittmp[0]=hitx-driftdist;
               //cout<<"here6"<<endl;
              }
}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::CoordinateFinder(const Double_t y1,const Double_t x2,const Double_t k3,const Double_t q3,const Double_t k4,const Double_t q4,const UShort_t *ijk,Double_t x[],Double_t y[]){

   Double_t x1,x3,x4,y2,y3,y4;

   x[1]=x2;y[0]=y1;

             //x[0]=(x[1]*(k4-k3)+(q4-y[0])/z4121-(q3-y[0])/z3121)/(k4-k3+k3/z3121-k4/z4121);
             x[0]=(x[1]*(k4-k3)+(q4-y[0])/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]-(q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])])/(k4-k3+k3/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]-k4/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]);
             //x[2]=(x[1]-x[0])*z3121+x[0];
             x[2]=(x[1]-x[0])*zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]+x[0];
             //x[3]=(x[1]-x[0])*z4121+x[0];
             x[3]=(x[1]-x[0])*zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]+x[0];
             //y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]+y[0];
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
            
 
             if(checkDch){ 
                      cout<<"2------------------------------------------------------"<<endl; 
                      for (UShort_t jk = 0; jk < numLayers/2; jk++) {
                       cout<<"DCH points (x,y): "<<x[jk]<<" "<<y[jk]<<endl;
                      }
                      cout<<"2------------------------------------------------------"<<endl; 
             }
} 
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::ReturnPointers(UInt_t ijk,UInt_t i,ScalarD* &hitX,ScalarI* &trackId,ScalarI* &detId,ScalarUI* &hitwire,ScalarD* &driftlen,ScalarI* &pointind){
              hitX = (ScalarD*) dchhitplane[ijk]->fDchHitPlane1->UncheckedAt(i); 
              trackId = (ScalarI*) dchhitplane[ijk]->fDchHitPlane2->UncheckedAt(i); 
              detId = (ScalarI*) dchhitplane[ijk]->fDchHitPlane4->UncheckedAt(i); 
              hitwire = (ScalarUI*) dchhitplane[ijk]->fDchHitPlane5->UncheckedAt(i); 
              driftlen = (ScalarD*) dchhitplane[ijk]->fDchHitPlane6->UncheckedAt(i); 
              pointind = (ScalarI*) dchhitplane[ijk]->fDchHitPlane7->UncheckedAt(i); 
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
int BmnDchHitProducer_exp::NumericalRootFinder()
{
 
   // Create the function and wrap it
       //TF1 f("Sin Function", "sin(x)", TMath::PiOver2(), TMath::TwoPi() );
       ROOT::Math::WrappedTF1 wf1(*fitdt);
           
   //           // Create the Integrator
       ROOT::Math::BrentRootFinder brf;
                  
                     // Set parameters of the method
       brf.SetFunction(wf1, 0., fitdt->GetMaximumX());
       brf.Solve();
                            
       cout <<"Time offset of drift time spectrum (ns): "<<brf.Root() << ", interval: "<<"0."<<" "<<fitdt->GetMaximumX()<<endl;
       ranmin=brf.Root();//
                                
       return 0;
}
//-------------------------------------------------------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::rtCurve(Double_t time)
{

Double_t radDist;
if(time<ranmin){
radDist=0.;
}else if(time>=ranmin&&time<=ranmax){
radDist=spline5rt[fDchNum-1]->Eval(time-ranmin);
}else{
//cout<<"spline5rt[fDchNum-1] "<<time<<" "<<ranmax<<" "<<ranmin<<endl;
radDist=spline5rt[fDchNum-1]->Eval(ranmax-ranmin);
}
if(radDist<0.)radDist=0.;
  
return radDist;
 
}
//-------------------------------------------------------------------------------------------------------------------------
Double_t BmnDchHitProducer_exp::SplineIntegral(const Double_t a, const Double_t b, const Int_t n)
{
  Double_t integral=0.,delta=(b-a)/Double_t(n);
  Double_t x0,x1;

         x0=a;    
 
         for(Int_t i = 0; i < n; i++) {
            x1=x0+delta; 
            integral=integral+delta*spline5[fDchNum-1]->Eval(x0)+0.5*delta*(spline5[fDchNum-1]->Eval(x1)-spline5[fDchNum-1]->Eval(x0));   
            //cout<<"splineintegral params.: "<<n<<" "<<delta<<" "<<x0<<" "<<x1<<" "<<spline5[fDchNum-1]->Eval(x0)<<" "<<spline5[fDchNum-1]->Eval(x1)<<" "<<spline5[fDchNum-1]->Eval(x1)-spline5[fDchNum-1]->Eval(x0)<<endl;
            //cout<<"splineintegral = "<<integral<<" "<<a<<" "<<b<<endl;
            x0=x1;
         }
    
return integral;

}
//-------------------------------------------------------------------------------------------------------------------------
UShort_t BmnDchHitProducer_exp::PlanesTopology(){

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

UShort_t pairPlanesUsed=0,planesUsed=0,topol;

     for(UShort_t i = 0; i < numLayers; i+=2) {
       if(planeUsed[i]||planeUsed[i+1])pairPlanesUsed=++pairPlanesUsed;
       planesUsed=planesUsed+UShort_t(planeUsed[i])+UShort_t(planeUsed[i+1]);
     }

     if(pairPlanesUsed==4){
      topol=1;
     }else{
      topol=2;
     }
     if(planesUsed==8)topol=0;

cout<<"planes topology = "<<topol<<", main planes (0 to 4) = "<<pairPlanesUsed<<", planes (0 to 8) = "<<planesUsed<<endl; 

return topol;

}
//-------------------------------------------------------------------------------------------------------------------------
UShort_t BmnDchHitProducer_exp::RunTypeToNumber(const TString runType){

UShort_t j;

           if(runType=="run1"){j=0;}
           else if(runType=="run2"){j=1;}
           else if(runType=="run3"){j=2;}
           else{cout<<"run type not in the list!"<<endl; 
                cout<<" run type = "<<runType<<endl;}
return j;

}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::ExtrapToDch(const Double_t x[],const Double_t y[],const Double_t zLayer[],const UShort_t ijk[],Int_t &jjgr2){

Double_t delZ=zLayer[ijk[1]]-zLayer[ijk[0]],delZ2;
//Double_t delXshift=detXshift[1]-detXshift[0];
Double_t tgx=(x[1]-x[0])/delZ;
Double_t tgy=(y[1]-y[0])/delZ;
if(checkDch)cout<<"tgx = "<<tgx<<", tgy = "<<tgy<<endl;
/*if(fDchNum==2){
tgx=-tgx;
tgy=-tgy;
}*/
Double_t xExtrap[numLayers],yExtrap[numLayers];

     for(UShort_t i = 0; i < numLayers; i++) {
       delZ2=(zLayerExtrap[i]-zLayer[ijk[0]]);
       //xExtrap[i]=x[0]+tgx*delZ2+delXshift;
       xExtrap[i]=x[0]+detXshift[fDchNum-1]+tgx*delZ2;
       if(checkDch)cout<<"xExtrap[i] = "<<xExtrap[i]<<", x[i] = "<<x[i]<<", detXshift[fDchNum-1] = "<<detXshift[fDchNum-1]<<", x[i] + detXshift[fDchNum-1] = "<<x[i] + detXshift[fDchNum-1]<<", tgx*delZ2 = "<<tgx*delZ2<<endl;
       //xExtrap[i]=x[0]+tgx*delZ2;
       yExtrap[i]=y[0]+tgy*delZ2;
       if(checkDch)cout<<"yExtrap[i] = "<<yExtrap[i]<<", y[0] = "<<y[0]<<", tgy*delZ2 = "<<tgy*delZ2<<endl;
       hXYZcombhits->SetPoint(jjgr2++,zLayerExtrap[i],xExtrap[i],yExtrap[i]);
       //cout<<"zLayerExtrap: "<<zLayerExtrap[i]<<endl;
     }


}
//-------------------------------------------------------------------------------------------------------------------------
ClassImp(BmnDchHitProducer_exp)
