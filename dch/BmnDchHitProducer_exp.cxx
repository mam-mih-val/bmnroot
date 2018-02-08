//------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <assert.h>

//#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoMatrix.h>
//#include <TMath.h>
#include <TRandom.h>
#include <TRandom2.h>
#include <TVector3.h>
#include <TVector2.h>
#include <TKey.h>
//#include "TRandom.h"
#include <TRandom2.h>
#include <TSpectrum.h>
#include "BmnDch.h"
#include "BmnDchHit.h"
#include "BmnDchPoint.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmStack.h"
#include "BmnDchHitProducer_exp.h"
//#include "DchTrackCand.h"
//#include "DchTrackManagerSim.h"
//#include <TObjectTable.h>
#include "TVectorD.h"

#include "Math/Vector4D.h"

#include "Math/GenVector/Rotation3D.h"
#include "Math/GenVector/EulerAngles.h"

#include "Math/GenVector/Transform3D.h"
#include "Math/GenVector/Plane3D.h"
#include "Math/GenVector/VectorUtil.h"

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
        const Double_t BmnDchHitProducer_exp::clusdens_Ar=25., BmnDchHitProducer_exp::clusdens_CO2=35.; // cluster density per track length - clusters/cm
        const Double_t BmnDchHitProducer_exp::meanpath_Ar=1./clusdens_Ar, BmnDchHitProducer_exp::meanpath_CO2=1./clusdens_CO2; // cluster density per track length - clusters/cm
        const Double_t BmnDchHitProducer_exp::meanpath=0.7*meanpath_Ar+0.3*meanpath_CO2; // for 70/30 ArCO2
        const Double_t BmnDchHitProducer_exp::clusmean=1./meanpath;
	//const Double_t BmnDchHitProducer_exp::gasDriftSpeed = 5.e-3; // 50 mkm/ns == 5.e-3 cm/ns 
	const Double_t BmnDchHitProducer_exp::gasDriftSpeed = 2.5e-3; // 25 mkm/ns == 2.5e-3 cm/ns 
	const Double_t BmnDchHitProducer_exp::driftTimeMax=stepXYhalf/gasDriftSpeed; // 25 mkm/ns == 2.5e-3 cm/ns 
	const Double_t BmnDchHitProducer_exp::wireDriftSpeed =  20; // 20 cm/ns
	const Double_t BmnDchHitProducer_exp::resolution =  15e-3; // 150 mkm  ==  0.015 cm
	const Double_t BmnDchHitProducer_exp::hitErr[3] = {resolution,resolution,0.};
	const Double_t BmnDchHitProducer_exp::WheelR_2 = pow(MaxRadiusOfActiveVolume,2.); // cm x cm
        const UInt_t BmnDchHitProducer_exp::nintervals=40;
        const TVector2 BmnDchHitProducer_exp::unitU(0.,1.);
        const TVector2 BmnDchHitProducer_exp::unitV(0.,-1.);

//------------------------------------------------------------------------------------------------------------------------
struct __ltstr
{
	bool operator()(Double_t s1, Double_t s2) const
  	{
    		return s1 > s2;
  	}
};

//------------------------------------------------------------------------------------------------------------------------
//BmnDchHitProducer_exp::BmnDchHitProducer_exp(UInt_t num = 1, Int_t verbose, Bool_t test, TString runtype, UShort_t iter) : FairTask("Dch HitProducer", verbose), fDoTest(test), fRSigma(0.2000), fRPhiSigma(0.0200), fOnlyPrimary(kFALSE)
BmnDchHitProducer_exp::BmnDchHitProducer_exp(Int_t verbose, Bool_t test, TString runtype) : FairTask("Dch HitProducer", verbose), fDoTest(test), fRSigma(0.2000), fRPhiSigma(0.0200), fOnlyPrimary(kFALSE)
{
	pRandom = new TRandom2;
        //fDchNum = num;
        runType = runtype;
        //Iter=itermax;
        TString str;
        //str.Form("%u",fDchNum);
        str.Form("%u",1);
        fInputBranchName = TString("DCH") + str + TString("Point");
        //if(checkGraphs){
        //UShort_t suppl=3-fDchNum;
        TString str2;
        str2.Form("%u",2);
        //str2.Form("%u",suppl);
        fInputBranchName2 = TString("DCH") + str2 + TString("Point");
        //}
        fOutputHitsBranchName = "BmnDchHit";

}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHitProducer_exp::~BmnDchHitProducer_exp() 
{
	delete pRandom;	
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::InitDchParameters(){
  //checkDch=kFALSE;
  checkDch=kTRUE;
  checkGraphs=kFALSE;
  //checkGraphs=kTRUE;
  //useCalib=kFALSE;
  //useCalib=kTRUE;
  //fAngleCorrectionFill=kFALSE;
  //fAngleCorrectionFill=kTRUE;
  //calibMethod=1;// maximum interval
  calibMethod=2; //inflex points
  eventNum=0; 
  trCand=0; 
  //if(fDchNum==1){dchtrackcands=new TFile("dchtrackcands1.root", "RECREATE");}
  //else if(fDchNum==2){dchtrackcands=new TFile("dchtrackcands2.root", "RECREATE");}

  detXshift[0]=DCH1_Xpos[RunTypeToNumber(runType)];
  detXshift[1]=DCH2_Xpos[RunTypeToNumber(runType)];
  // DCH1 and DCH2 x shifts, cm
  detYshift[0]=DCH1_Ypos[RunTypeToNumber(runType)];
  detYshift[1]=DCH2_Ypos[RunTypeToNumber(runType)];
  // DCH2 and DCH2 y shifts, cm
 
    //if(fDchNum==1){detXshift[0]=DCH1_Xpos[RunTypeToNumber(runType)];}
    //else if(fDchNum==2){detXshift[1]=DCH2_Xpos[RunTypeToNumber(runType)];}
    // DCH1 and DCH2 x shifts, cm
 
  for (UShort_t i = 0; i < numLayers; i++) {
    DCH1_ZlayerPos_global[i]=DCH_ZlayerPos_local[i]+DCH1_Zpos[RunTypeToNumber(runType)];
    DCH2_ZlayerPos_global[i]=DCH_ZlayerPos_local[i]+DCH2_Zpos[RunTypeToNumber(runType)];
  }

  cout<<"run type = "<<runType<<", run type number = "<< RunTypeToNumber(runType)<<endl;
 
  /*if(fDchNum==1){
        z3121=(DCH1_ZlayerPos_global[4]-DCH1_ZlayerPos_global[0])/(DCH1_ZlayerPos_global[2]-DCH1_ZlayerPos_global[0]);
        z4121=(DCH1_ZlayerPos_global[6]-DCH1_ZlayerPos_global[0])/(DCH1_ZlayerPos_global[2]-DCH1_ZlayerPos_global[0]);
        //z4131=(DCH1_ZlayerPos_global[6]-DCH1_ZlayerPos_global[0])/(DCH1_ZlayerPos_global[4]-DCH1_ZlayerPos_global[0]);
      }else if(fDchNum==2){
        z3121=(DCH2_ZlayerPos_global[4]-DCH2_ZlayerPos_global[0])/(DCH2_ZlayerPos_global[2]-DCH2_ZlayerPos_global[0]);
        z4121=(DCH2_ZlayerPos_global[6]-DCH2_ZlayerPos_global[0])/(DCH2_ZlayerPos_global[2]-DCH2_ZlayerPos_global[0]);
        //z4131=(DCH2_ZlayerPos_global[6]-DCH2_ZlayerPos_global[0])/(DCH2_ZlayerPos_global[4]-DCH2_ZlayerPos_global[0]);
  }*/

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

  for (UInt_t i = 0; i < numChambers; i++) {
   for (UShort_t k = 0; k < numLayers; k++) {
      if(i==0){
        zLayer[i][k]=DCH1_ZlayerPos_global[k]; 
        zLayerExtrap[i][k]=DCH2_ZlayerPos_global[k];
        //zgap[fDchNum-1][k/2]=(DCH1_ZlayerPos_global[k]+DCH1_ZlayerPos_global[k+2])/2.;
      }else if(i==1){
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
  radialRange=3.; //cm (CC)

  uOrt=unitU.Rotate(angleLayerRad[4]);
  vOrt=unitV.Rotate(angleLayerRad[6]);

}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::BookHistograms(){

		htTime = new TH1D("htTime", "Time delay, ns", 1000, 0., 1000.); htTime->SetDirectory(0); fList.Add(htTime);
		htTime->SetXTitle("#Delta_{time delay}, ns"); htTime->SetYTitle("Events");		
		htTimeA = (TH1D*) htTime->Clone("htTimeA"); htTimeA->SetDirectory(0); fList.Add(htTimeA);	

		htGasDrift = new TH1D("htGasDrift", "", 100, 0., 0.5); 	htGasDrift->SetDirectory(0); 	fList.Add(htGasDrift);
		htGasDrift->SetXTitle("#Delta_{gas drift}, cm"); htGasDrift->SetYTitle("Events");
		htGasDriftA = (TH1D*) htGasDrift->Clone("htGasDriftA");	htGasDriftA->SetDirectory(0); 	fList.Add(htGasDriftA);	

		htPerp = new TH1D("htPerp", "", 360, -140.+0.25, 140.+0.25); htPerp->SetDirectory(0);	fList.Add(htPerp);
		htPerp->SetXTitle("wire position, cm"); htPerp->SetYTitle("Events");
		htPerpA = (TH1D*) htPerp->Clone("htPerpA"); htPerpA->SetDirectory(0); 			fList.Add(htPerpA);

		htOccup = new TH1D("htOccup", "Cell occupancy", 100, 0.5, 100.5); 
		htOccup->SetDirectory(0); fList.Add(htOccup);				
		htXYlocal = new TH2D("htXYlocal", "Local XY", 1000, 0., 0.6, 1000, -200., 200.); 		
		htXYlocal->SetDirectory(0); fList.Add(htXYlocal);
		htRvsR = new TH2D("htRvsR", "R global vs R  local", 1000, 0., 150., 1000, 0., 150.); 		
		htRvsR->SetDirectory(0); fList.Add(htRvsR);
		htWireN = new TH1D("htWireN", "#wire", 16400, -200., 16000. + 200.);  
		htWireN->SetDirectory(0); fList.Add(htWireN);	
		htMCTime = new TH1D("htMCTime", "MC time", 100, 0., 1000.); 
		htMCTime->SetDirectory(0); fList.Add(htMCTime);			
		hXYhit = new TH2D("hXYhit", "xy hit distribution", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume); 
		hXYhit->SetDirectory(0); fList.Add(hXYhit);			
		hX = new TH1D("hX", "x coordinate", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume); 
		hX->SetDirectory(0); fList.Add(hX);			
		hY = new TH1D("hY", "y coordinate", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume); 
		hY->SetDirectory(0); fList.Add(hY);			
		hZ = new TH1D("hZ", "z coordinate", 1200, 540., 660.); 
		hZ->SetDirectory(0); fList.Add(hZ);			
		hRadiusRange = new TH1D("hRadiusRange", "track radius range in next plane", 100, 0., 10.); 
		hRadiusRange->SetDirectory(0); fList.Add(hRadiusRange);		
		hResolY = new TH1D("hResolY", "hit Y coordinate resolution", 500, -5., 5.); 
		hResolY->SetDirectory(0); fList.Add(hResolY);		
		hResolX = new TH1D("hResolX", "hit X coordinate resolution", 500, -5., 5.); 
		hResolX->SetDirectory(0); fList.Add(hResolX);		
                hResolU = new TH1D("hResolU", "hit U coordinate resolution", 500, -5., 5.);
                hResolU->SetDirectory(0); fList.Add(hResolU);
                hResolV = new TH1D("hResolV", "hit V coordinate resolution", 500, -5., 5.);
                hResolV->SetDirectory(0); fList.Add(hResolV); 
		//hResolR = new TH1D("hResolR", "hit R coordinate resolution", 500, 0., 5.); 
		//hResolR->SetDirectory(0); fList.Add(hResolR);		
		hDCA = new TH1D("hDCA", "distance of closest approach", 100, 0., 0.6); 
		hDCA->SetDirectory(0); fList.Add(hDCA);		
		hLength = new TH1D("hLength", "length of track in Dch", 100, 0.6, 1.); 
		hLength->SetDirectory(0); fList.Add(hLength);		
		hMinDriftTime = new TH1D("hMinDriftTime", "drift time = dca/(drift velocity)", 100, 0., 250.); 
		hMinDriftTime->SetDirectory(0); fList.Add(hMinDriftTime);		
		//hXYZcombhits1 = new TH3D("hXYZcombhits1", "XYZ of combinatorial hits in first DCH", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 100, 540., 560.); 		
		//hXYZcombhits2 = new TH3D("hXYZcombhits2", "XYZ of combinatorial hits in second DCH", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 100, 640., 660.); 		
                //hXYZcombhits = new TGraph2D(); 
                //hXYZcombhits->SetNameTitle("hXYZcombhits","Hits in DCH");
		//hXYZcombhits->SetDirectory(0); fList.Add(hXYZcombhits);			
		//hXYZpoints1 = new TH3D("hXYZpoints1", "XYZ of GEANT points in first DCH", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 100, 540., 560.); 		
		//hXYZpoints2 = new TH3D("hXYZpoints2", "XYZ of GEANT points in second DCH", 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 240, -MaxRadiusOfActiveVolume, MaxRadiusOfActiveVolume, 100, 640., 660.); 		
                //hXYZpoints = new TGraph2D(); 
                //hXYZpoints->SetNameTitle("hXYZpoints","GEANT points in DCH");
		//hXYZpoints->SetDirectory(0); fList.Add(hXYZpoints);			
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
InitStatus 		BmnDchHitProducer_exp::InitDch() 
{

        cout << " BmnDchHitProducer_exp::Init() " << endl;

        //TString str; 
        //str.Form("%u",fDchNum);
        /*TString nameTrCand=TString("DchTrackCandidates")+str;
        //if(Iter==0)InitDchParameters();
        trackcand = new DchTrackCand();  
        trackcand->SetName(nameTrCand);
        trackcand->SetNameTitle(nameTrCand,"DCH track candidates, event numbers");*/
        //tree = new TTree("T","An example of a ROOT tree");
        //tree->Branch("trackcand", "Event", &trackcand, 64000,0);
        InitDchParameters();
        if(fDoTest)BookHistograms(); 
        //if(fAngleCorrectionFill)BookHistsAngleCorr(); 
        //if(fAngleCorrectionFill==false)FitHistsAngleCorr();
        //if(useCalib)rtCalibration();
        /*if(Iter==0){
          rtCalibration();
          cout << " RT calibration done. " << endl;
        }else{
          rtCalibCorrection();
          cout << " RT calibration correction done. " << endl;
        }*/
        //rtCalibRead();
        rtCalibCorrection();
        cout << " RT calibration reading done. " << endl;
      
        scaleSimExTDC=(ranmax-ranmin)/driftTimeMax;
 
        if (fOnlyPrimary) cout << " Only primary particles are processed!!! " << endl;

    //Get ROOT Manager
        FairRootManager* ioman = FairRootManager::Instance();
        if (ioman == 0)
        {
            cout<<"BmnDchHitProducer_exp::InitDch(): FairRootManager is null! Task will be deactivated"<<endl;
            SetActive(kFALSE);
            return kERROR;
        }
        fBmnDchPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
        if (!fBmnDchPointsArray)
        {
            cout<<"BmnDchHitProducer_exp::InitDch(): branch "<<fInputBranchName<<" not found! Task will be deactivated"<<endl;
            SetActive(kFALSE);
            return kERROR;
        }

        //if(checkGraphs){
         fBmnDchPointsArray2 = (TClonesArray*) ioman->GetObject(fInputBranchName2);
         if (!fBmnDchPointsArray2)
         {
             cout<<"BmnDchHitProducer_exp::InitDch(): branch "<<fInputBranchName2<<" not found! Task will be deactivated"<<endl;
             SetActive(kFALSE);
             return kERROR;
         }
        //}

        fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
        if (!fMCTracksArray)
        {
            cout<<"BmnDchHitProducer_exp::InitDch(): branch MCTrack not found! Task will be deactivated"<<endl;
            SetActive(kFALSE);
            return kERROR;
        }

  	// Create and register output array
        TString folder = TString("DCH");
        TString str0; 
        str0.Form("%d",0);
        TString name0[numChambers],name[numChambers];
        for (UShort_t idch = 0; idch < numChambers; idch++) {
  	 pHitCollection[idch] = new TClonesArray(fOutputHitsBranchName);  
  	 pHitCollection0[idch] = new TClonesArray(fOutputHitsBranchName);  
         TString str; 
         str.Form("%d",idch+1);
         name0[idch] = TString("BmnDch") + str + TString("Hit") + str0;
         name[idch] = TString("BmnDch") + str + TString("Hit");
         ioman->Register(name0[idch].Data(), folder.Data(), pHitCollection0[idch], kTRUE);
         ioman->Register(name[idch].Data(), folder.Data(), pHitCollection[idch], kTRUE);
        }


        cout << " Initialization finished succesfully. " << endl;
	
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
Double_t BmnDchHitProducer_exp::GetDriftDist(FairMCPoint *dchPoint, UShort_t idch, UShort_t uidLocal, Double_t x, Double_t &wirePos){

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
if(fDoTest){
  hDCA->Fill(dca); 
  hLength->Fill(length); 
  hMinDriftTime->Fill(mindist/gasDriftSpeed); 
}

return dca;

}
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

void BmnDchHitProducer_exp::rtCalibration(){

     TFile *fdstread = new TFile("/home/fedorisin/trunk/bmnroot/dch_exp/testCC.BmnDchHitProducer_exp_exp.root","read");
     minDriftTime = (TH1D*)fdstread->Get("minDriftTime1");

     Int_t nbins=minDriftTime->GetXaxis()->GetNbins();

    /*if(checkDch){
     for (Int_t ibin = 1; ibin <= nbins; ibin++) {
        cout<<"hist. time = "<<minDriftTime[fDchNum-1]->GetBinCenter(ibin)<<" "<<minDriftTime[fDchNum-1]->GetBinContent(ibin)<<endl;
     }
    }*/
//minDriftTime[fDchNum-1] = (TH1D*)fdstread->Get("hMinDriftTime");
//minDriftTime[fDchNum-1]->Draw();
//minDriftTime[fDchNum-1]->Smooth(2);
//minDriftTime[fDchNum-1]->Rebin();
minDriftTime->Smooth();
/*TH1D* hMinDriftTimesmoothed = (TH1D*)minDriftTime[fDchNum-1]->Clone("hMinDriftTimesmoothed");
hMinDriftTimesmoothed->Smooth(20);
Int_t nbinsdth2=minDriftTime[fDchNum-1]->GetXaxis()->GetNbins();
for (Int_t j = 1; j <= nbinsdth2; j++) {
 minDriftTime[fDchNum-1]->SetBinError(j,fabs(minDriftTime[fDchNum-1]->GetBinContent(j)-hMinDriftTimesmoothed->GetBinContent(j)));
}
*/
//Int_t binmax = minDriftTime[fDchNum-1]->GetMaximumBin();
Int_t nbinsUsed;
//spline3 = new TSpline3(minDriftTime[fDchNum-1], "", 0., 0.);
spline5 = new TSpline5(minDriftTime, "", 0.,0.,0.,0.);
spline5->SetTitle("TDC");
spline5->SetName("tdc");
if(calibMethod==1){
Double_t hmax = minDriftTime->GetMaximum();
Int_t hmaxbin = minDriftTime->GetMaximumBin();
Double_t hmin = minDriftTime->GetMinimum();
minDriftTime->GetXaxis()->SetRange(1,hmaxbin);
Int_t hminbin1 = minDriftTime->GetMinimumBin();
minDriftTime->GetXaxis()->SetRange(hmaxbin,nbins);
Int_t hminbin2 = minDriftTime->GetMinimumBin();
minDriftTime->GetXaxis()->SetRange(1,nbins);
Double_t hminx1 = minDriftTime->GetXaxis()->GetBinCenter(hminbin1);
Double_t hminx2 = minDriftTime->GetXaxis()->GetBinCenter(hminbin2);
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
   binc=minDriftTime->GetBinContent(j);
   sum=sum+binc;
   jj++;
 }
}
Double_t averbincon=sum/Double_t(jj),upperlim=averbincon+5.*sqrt(averbincon);
Double_t bincon0,bincon1;
Int_t ranminbin;
Int_t i=hmaxbin-1;
do{
 bincon0=minDriftTime->GetBinContent(i+1);
 bincon1=minDriftTime->GetBinContent(i);
 //cout<<"ranmax "<<ranmaxbin<<" "<<hmaxtol<<" "<<i<<endl;
 i--; 
}while(!((bincon1>bincon0&&bincon1<upperlim)||UInt_t(bincon1)==0));
ranminbin=i+2;
//cout<<ranmaxbin<<" "<<ranmaxbin<<endl;
cout<<"ranminbin = "<<ranminbin<<endl;
nbinsUsed=hminbin2-ranminbin+1;
//Int_t nbinsUsed=hminbin2-hminbin1+1;
//ranmin=minDriftTime[fDchNum-1]->GetBinCenter(hminbin1)-0.5*(minDriftTime[fDchNum-1]->GetBinWidth(hminbin1)); 
ranmin=minDriftTime->GetBinCenter(ranminbin)-0.5*(minDriftTime->GetBinWidth(ranminbin)); 
ranmax=minDriftTime->GetBinCenter(hminbin2)+0.5*(minDriftTime->GetBinWidth(hminbin2)); 
}else if(calibMethod==2){
Double_t inflX1,inflX2;
tdcInflexPoints(inflX1,inflX2);
cout<<"inflex points (left,right) = "<<inflX1<<", "<<inflX2<<endl;
ranmin=inflX1;
ranmax=inflX2;
Double_t binwidth=minDriftTime->GetBinWidth(1);
nbinsUsed=Int_t(ranmax/binwidth)-Int_t(ranmin/binwidth)+1;
}
Double_t timeRange=ranmax-ranmin;
cout<<"time interval: "<<ranmin<<" "<<ranmax<<endl;
const UInt_t npoints=nintervals+1;
Double_t deldt=(ranmax-ranmin)/Double_t(nintervals);
//Double_t dt[npoints],r[npoints];
Double_t dt[nintervals],r[nintervals];
//Double_t norm=diagonal/(fitdt->Integral(0.,ranmax));
//Double_t norm=stepXYhalf/(fitdt->Integral(0.,ranmax));
if(checkDch)cout<<"nbinsUsed = "<<nbinsUsed<<endl;
Double_t norm=stepXYhalf/SplineIntegral(ranmin,ranmax,nbinsUsed*5);
for (Int_t j = 0; j < nintervals; j++) {
 dt[j]=deldt*Double_t(j)+deldt/2.;
 //r[j]=norm*(fitdt->Integral(dt[0],dt[j]));
 r[j]=norm*SplineIntegral(dt[0]+ranmin,dt[j]+ranmin,5*TMath::Nint(Double_t(nbinsUsed)*(dt[j]-dt[0])/timeRange));
 if(checkDch)cout<<"calibration curve "<<dt[j]<<" "<<r[j]<<" "<<SplineIntegral(dt[0]+ranmin,dt[j]+ranmin,5*TMath::Nint(Double_t(nbinsUsed)*(dt[j]-dt[0])/timeRange))<<endl;
}
//calib[fDchNum-1] = new TGraph(npoints,dt,r);
calib = new TGraph(nintervals,dt,r);
//spline3rt = new TSpline3("r-t calibration curve",calib, "", 0., 0.);
spline5rt = new TSpline5("r-t calibration curve",calib, "", 0.,0.,0.,0.);
//calib->Fit("pol5");
calib->SetTitle("r-t calibration curve");
calib->SetName("rtCalibCurve");
spline5rt->SetName("rtCalibSpline");
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
void BmnDchHitProducer_exp::tdcInflexPoints(Double_t &inflX1,Double_t &inflX2){

Int_t ngrpoints=minDriftTime->GetNbinsX();
//Double_t xrangemin=minDriftTime[fDchNum-1]->GetXaxis()->GetXmin(); 
//Double_t xrangemax=minDriftTime[fDchNum-1]->GetXaxis()->GetXmax(); 
//Double_t binwidth=(xrangemax-xrangemin)/Double_t(ngrpoints);
Double_t binwidth=minDriftTime->GetBinWidth(1);
Double_t halfbinwidth=0.5*binwidth;
Double_t xstart=minDriftTime->GetBinCenter(1);
Double_t xspl[ngrpoints],yspl[ngrpoints];
for (Int_t ival = 0; ival < ngrpoints; ival++) {
xspl[ival]=xstart+Double_t(ival)*binwidth;
yspl[ival]=spline5->Derivative(xspl[ival]);
}
splinederiv = new TGraph(ngrpoints,xspl,yspl);
splinederiv->SetTitle("derivative of r-t calibration curve");
splinederiv->SetName("rtCalibCurve derivative");
TString spldersm="splinederivsmooth";
TString splineDerivSmooth = spldersm;
splinederivsmooth = new TH1D(splineDerivSmooth,"smoothed hist of spline derivatives",ngrpoints,xspl[0]-halfbinwidth,xspl[ngrpoints-1]+halfbinwidth);
for (Int_t ival = 0; ival < ngrpoints; ival++) {
splinederivsmooth->SetBinContent(ival+1,yspl[ival]);
}
splinederivsmooth->Rebin();
splinederivsmooth->Smooth(1000);
Int_t hmaxbin_splinederiv = splinederivsmooth->GetMaximumBin();
inflX1 = splinederivsmooth->GetBinCenter(hmaxbin_splinederiv); //left inflex point
//Int_t hminbin_splinederiv = splinederivsmooth[fDchNum-1]->GetMinimumBin();
TH1D *hist_new=(TH1D*)splinederivsmooth->Clone();
hist_new->Scale(-1.);
TSpectrum *s = new TSpectrum(10,1.);
Int_t nfound = s->Search(hist_new,2,"nobackground");
cout<<"number of inflection points = "<<nfound<<endl;
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
Float_t *xpeaks = s->GetPositionX();
#else
Double_t *xpeaks = s->GetPositionX();
#endif
if(nfound==2){
 inflX2 = (Double_t)xpeaks[nfound-1]; //right inflection point
}else if(nfound>2){
 inflX2 = 0.5*(xpeaks[nfound-1]+xpeaks[nfound-2]);
}
cout<<"inflection points:"<<nfound<<endl;
for (Int_t ival = 0; ival < nfound; ival++) {
 cout<<xpeaks[ival]<<endl;
}
delete hist_new;

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
void 			BmnDchHitProducer_exp::ExecDch(Option_t* opt) 
{
    if (!IsActive())
        return;

    if(checkGraphs){
        hXYZcombhits = new TGraph2D(); 
        hXYZcombhits->SetNameTitle("hXYZcombhits","Hits in DCH");
        TString str;
        str.Form("%u",eventNum);
        TString hXYZcombhitsName = TString("hXYZcombhits") + str;
        hXYZcombhits->SetName(hXYZcombhitsName);
        hXYZpoints = new TGraph2D(); 
        hXYZpoints->SetNameTitle("hXYZpoints","GEANT points in DCH");
        TString hXYZpointsName = TString("hXYZpoints") + str;
        hXYZpoints->SetName(hXYZpointsName);
      }


      for (UShort_t idch = 0; idch < numChambers; idch++) {
        pHitCollection0[idch]->Delete();
        pHitCollection[idch]->Delete();
        cout<<"number of Dch = "<<idch+1<<endl;
 
        //gObjectTable->Print();

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
          //for (UShort_t k = numLayers; k < numLayers; k++) {
           //  dchhitplane[k] = new DchHitPlane();
          //}
          
        //TString str; 
        //str.Form("%u",eventNum);
        //TString nameTrCand=TString("DchTrackCandidates")+str;
        if(idch==0){
           dchTrCand1 = new DchTrackCand();  
        }else if(idch==1){
           dchTrCand2 = new DchTrackCand();  
        }
        //trackcand->SetName(nameTrCand);
        //trackcand->SetNameTitle(nameTrCand,"DCH track candidates, event number");
        

        //fBmnDchPointsArray->Dump();
	Int_t nDchPoint;
        if(idch==0){
	 nDchPoint = fBmnDchPointsArray->GetEntriesFast();
         eventNum++;
        }else if(idch==1){
	 nDchPoint = fBmnDchPointsArray2->GetEntriesFast();
        }
        if(checkDch)cout << " BmnDchHitProducer_exp::Exec(), Number of BmnDchPoints = " << nDchPoint <<", event number = "<<eventNum<<", chamber = "<<idch+1<<endl;
        if(eventNum%5000==0)cout << "event number = "<<eventNum<<endl;

	FairMCPoint     *dchPoint;
	BmnDchHit	*pHit;
        Double_t	R, Rphi, x, y, xRot, yRot, xRot1, yRot1, driftLength, driftDist, wirePos, L;
        Double_t        PyRot;
	Double_t 	dRphi = 0., dR = 0.;
	Double_t 	dchZpos,corrWirePos=0.,wirePos2;
	Double_t 	drifttimemin;
	TVector3	pos, dpos;	
        UShort_t   	uid, uidLocal, wheel, gasgap, proj;
        Int_t   	detID;
        UInt_t   	hitWire;
        //Int_t           ijkl[numChambers][numLayers]={{0}};
        Int_t jjgr=0; 
        cout.precision(5);

	for(Int_t i = 0; i < nDchPoint; i++ )  // <---Loop over the DCH points
	//for(Int_t i = imin; i < imax; i++ )  // <---Loop over the DCH points
	{
                if(idch==0){
                 dchPoint = (FairMCPoint*) fBmnDchPointsArray->UncheckedAt(i);
                }else if(idch==1){
                 dchPoint = (FairMCPoint*) fBmnDchPointsArray2->UncheckedAt(i);
                }
                if(checkDch){
                 cout<<"DCH points (local): "<<dchPoint->GetX()-detXshift[idch]<<", "<<dchPoint->GetY()<<", "<<dchPoint->GetZ()<<", track ID = "<<dchPoint->GetTrackID()<<", chamber = "<<idch+1<<endl;
                 cout<<"DCH points (global): "<<dchPoint->GetX()<<", "<<dchPoint->GetY()<<", "<<dchPoint->GetZ()<<", track ID = "<<dchPoint->GetTrackID()<<", chamber = "<<idch+1<<endl;
                }

///////		if( ((FairMCTrack*)pMCTracks->UncheckedAt(pPoint1->GetTrackID()))->GetMotherId() != -1)continue; // primary ONLY !!!
                if(checkGraphs)hXYZpoints->SetPoint(jjgr++,dchPoint->GetZ(), dchPoint->GetX(),dchPoint->GetY());
                //RadialRange(dchPoint);
                //if(idch==0){
                 if (fOnlyPrimary) {
                   if (dchPoint->GetTrackID() < 0) continue; 
                   CbmMCTrack* track = (CbmMCTrack*) fMCTracksArray->At(dchPoint->GetTrackID());
                   if (!track) continue;
                   if (track->GetMotherId() != -1) continue;
                 }
                //}
                //cout<<"---------------------------------------------------------------------"<<endl;	
		detID = dchPoint->GetDetectorID();
                uidLocal=0;
                if (idch == 0) {dchZpos=DCH1_Zpos[RunTypeToNumber(runType)];}
                else if (idch == 1) {dchZpos=DCH2_Zpos[RunTypeToNumber(runType)];}
                Double_t zLocal=dchPoint->GetZ()-dchZpos;
                //cout<<zLocal<<" "<<DCH_ZlayerPos_local[uidLocal]<<" "<<uidLocal<<" "<<dchPoint->GetZ()<<" "<<dchZpos<<endl;
                if(zLocal>0.)uidLocal=uidLocal+numLayers_half;
                while(!(zLocal<(DCH_ZlayerPos_local[uidLocal]+ZhalfLength_DCHActiveVolume_W)&&
                       zLocal>(DCH_ZlayerPos_local[uidLocal]-ZhalfLength_DCHActiveVolume_W)))
                {uidLocal++;
                //cout<<"uidlocal = "<<uidLocal<<", zLocal = "<<zLocal<<" "<<DCH_ZlayerPos_local[uidLocal]+ZhalfLength_DCHActiveVolume<<" "<<DCH_ZlayerPos_local[uidLocal]-ZhalfLength_DCHActiveVolume<<endl;
                }
                //cout<<"end "<<"uidlocal = "<<uidLocal<<", zLocal = "<<zLocal<<" "<<DCH_ZlayerPos_local[uidLocal]+ZhalfLength_DCHActiveVolume<<" "<<DCH_ZlayerPos_local[uidLocal]-ZhalfLength_DCHActiveVolume<<endl;
                //if (fDchNum == 1) {uid=uidLocal;}
                //else if (fDchNum == 2) {uid=uidLocal+numLayers;}
                uid=uidLocal; 
                //cout<<"uid = "<<uid<<endl;
                //wheel = GetWheel(uid); // [0-1] == [inner,outer] 
                //wheel = fDchNum-1; // [0-1] == [inner,outer] //not needed now
		proj = GetProj(uid);	    // [0-3] == [x,y,u,v] 
		gasgap = GetGasGap(uid); // [0-1] == [inner,outer] 

                //cout<<"wheel = "<<wheel<<", proj = "<<proj<<", gasgap = "<<gasgap<<endl;

		if(fDoTest){
                  hX->Fill(dchPoint->GetX()-detXshift[idch]);
                  hY->Fill(dchPoint->GetY()-detYshift[idch]);
                  hZ->Fill(dchPoint->GetZ());
                } 
		//Rotate(proj, x = dchPoint->GetX()-detXshift[fDchNum-1], y = dchPoint->GetY(), xRot1, yRot1); // GlobalToLocal
	        //cout<<"y = "<<y<<" "<<"x = "<<x<<" "<<proj<<endl;	
	        //cout<<"yRot1 = "<<yRot1<<" "<<"xRot1 = "<<xRot1<<endl;	
                TVector2 xy2,xy(dchPoint->GetX()-detXshift[idch],dchPoint->GetY()-detYshift[idch]);
                //xy2=xy.Rotate(-angleLayerRad[uidLocal]);//GlobalToLocal
                if(uidLocal==2||uidLocal==3){
                 xy2=xy.Rotate(angleLayerRad[uidLocal]);
                }else{
                 xy2=xy.Rotate(-angleLayerRad[uidLocal]);
                }
                xRot=xy2.X();yRot=xy2.Y();
                //if(uidLocal==2||uidLocal==3)yRot=-yRot;
		driftDist = GetDriftDist(dchPoint,idch,uidLocal,yRot,wirePos); // [cm]
	        if(checkDch)cout<<"driftDist1 = "<<driftDist<<endl;
                //if(useCalib){	
                drifttimemin=driftDist/gasDriftSpeed;
                drifttimemin=drifttimemin*scaleSimExTDC+ranmin;
                cout<<scaleSimExTDC<<" "<<ranmin<<endl;
                driftDist=rtCurve(drifttimemin); 
                //driftDist=drifttimemin*stepXYhalf/driftTimeMax;
	        if(checkDch)cout<<"driftDist2 = "<<driftDist<<endl;	
                //} 
                driftLength = driftDist;
		//driftLength = GetDriftLength(proj, gasgap, yRot, wirePos); // [cm]
		//driftLength = GetDriftLength(proj, gasgap, xRot1, wirePos); // [cm]
	        //cout<<"yRot2 = "<<yRot<<", xRot2 = "<<xRot<<", angle = "<<angleLayerRad[uidLocal]<<", plane ID = "<<uidLocal<<", chamber = "<<fDchNum<<", wirepos = "<<wirePos<<endl;	
	        //cout<<"driftLength = "<<driftLength<<endl;	
	        //if(uidlocal%2)wirepos2 = wirepos-(anglepar[uidlocal][0]+wirepos*anglepar[uidlocal][1]); 
	        /*if(uidLocal%2!=0){ 
                 corrWirePos = 1.2*tan(TMath::DegToRad()*(anglepar[uidLocal][0]+wirePos*anglepar[uidLocal][1]));
	         wirePos2 = (gasgap == 0) ? TMath::Nint(yRot-corrWirePos) : TMath::Nint(yRot-corrWirePos + 0.5) - 0.5;
                 //cout<<"angle corr. = "<<corrWirePos<<", wirePos = "<<wirePos<<", wirePos2= "<<wirePos2<<" "<<anglepar[uidLocal][0]<<", "<<anglepar[uidLocal][1]<<endl; 
                }*/
                if(checkDch)cout<<"wirepos = "<<wirePos<<", xRot = "<<xRot<<", yRot = "<<yRot<<", z = "<<dchPoint->GetZ()<<", angle = "<<-angleLayer[uidLocal]<<endl;

		/*if(fAngleCorrectionFill){
                 TVector2 Pxy2,Pxy(dchPoint->GetPx(),dchPoint->GetPy());
                 Pxy2=Pxy.Rotate(-angleLayerRad[uidLocal]);//GlobalToLocal
                 //PxRot=Pxy2.X();
                 PyRot=Pxy2.Y();
                 if(uidLocal==2||uidLocal==3)PyRot=-PyRot;
                 if(uidLocal%2==0)hAngleVsWirepos[uidLocal]->Fill(wirePos,TMath::RadToDeg()*atan(PyRot/dchPoint->GetPz()));
                 //cout<<" dipangle = "<<TMath::RadToDeg()*atan(PyRot/dchPoint->GetPz())<<", "<<PyRot<<", momentum (z,x,y,full): "<<dchPoint->GetPz()<<", "<<dchPoint->GetPx()<<", "<<dchPoint->GetPy()<<", "<<sqrt(pow(dchPoint->GetPx(),2.)+pow(dchPoint->GetPz(),2.)+pow(dchPoint->GetPz(),2.))<<endl;
                }*/ 
		//R = yRot1; 
		R = xRot; 
		Rphi = driftLength;
		pRandom->Rannor(dRphi,dR);
				
		if(fDoTest)
		{ 
			htWireN->Fill(WireID(uid, wirePos, R));
			htXYlocal->Fill(Rphi, R);
			htRvsR->Fill(sqrt(pow(dchPoint->GetX()-detXshift[idch],2.) + pow(dchPoint->GetY(),2.)), sqrt(R*R + wirePos*wirePos));
			htMCTime->Fill(dchPoint->GetTime());
		}
				
/////		xRot = pRandom->Gaus(xRot, fRSigma/10.); // [cm]
	
/////		Rotate(proj, xRot, yRot, x, y, true); // back rotate
/////		pos.SetXYZ(x, y, pPoint1->GetZ());
	
                Double_t dX = pRandom->Gaus(0., hitErr[0]);	
                Double_t dY = pRandom->Gaus(0., hitErr[1]);	
                //Double_t dX = 0.;	
                ///Double_t dY = 0.;	
		//pos.SetXYZ(dchPoint->GetX(), dchPoint->GetY(), dchPoint->GetZ());
		pos.SetXYZ(dchPoint->GetX()+dX, dchPoint->GetY()+dY, dchPoint->GetZ());
		dpos.SetXYZ(dX,dY,hitErr[2]); 
		//dpos.SetXYZ(hitErr[0],hitErr[1],hitErr[2]); 
		
                //cout<<"hitID = "<<hitID<<", uid = "<<uid<<endl;
                //cout<<pos<<" "<<dpos<<endl;
                //cout<<"track id = "<<dchPoint->GetTrackID()<<", point index = "<<i<<endl;
		pHit = AddHit0(hitID, detID, pos, dpos, dchPoint->GetTrackID(), i, 0, uid, idch);
		//pHit->SetAngleCorrWirePos(wirePos2);
		pHit->SetPhi(GetPhi(proj));
    		pHit->SetMeas(Rphi + dRphi * fRPhiSigma); 	// R-Phi
    		pHit->SetError(fRPhiSigma);
    		pHit->SetMeas(R + dR * fRSigma, 1);		// R
    		pHit->SetError(fRSigma, 1);
						
		pHit->SetDrift(fabs(driftLength));
		pHit->SetWirePosition(wirePos);
		pHit->SetTShift(dchPoint->GetTime() + GetTShift(driftLength, wirePos, R, L));			
		pHit->SetWireDelay(L);
		pHit->SetDriftTime(drifttimemin-ranmin);
		
		fMapOccup.insert(occupMap::value_type(WireID(uid, wirePos, R), hitID++)); // <Hash(wirePos,UID) == cellID, index> pair
                //cout<<"wireID "<<WireID(uid, wirePos, R)<<" "<<wirePos<<endl;
                //cout<<"TShift "<<GetTShift(driftLength, wirePos, R, L)<<endl;

	} // <---Loop over the DCH points

        if(checkGraphs)hXYZpoints->SetPoint(jjgr++,dchPoint->GetZ(), dchPoint->GetX(),dchPoint->GetY());

        
	//Int_t nDchHit = pHitCollection0[idch]->GetEntriesFast();
        if(checkDch)cout<<"Number of DCH hits = "<<pHitCollection0[idch]->GetEntriesFast()<<endl;

	
	// Double-track resolution
	Int_t 		counter;
	Double_t 	timeDelta, gasDriftDelta;
	BmnDchHit 	*hit1, *hit2, *tmp;
	
	typedef map<Double_t, BmnDchHit*, __ltstr> 	delayMap; delayMap mapDelay; // <time delay, index> pair, (invert sorting)
	
	for(occupIter It = fMapOccup.begin(); It != fMapOccup.end(); )
	{
		counter = fMapOccup.count(It->first);	// hits in one cell
		if(fDoTest){ 
                 htOccup->Fill(counter);
                }         
		
                if (counter == 1) 	++It;	// single hit (nothing to do)
		else if(counter == 2)		// double hit
		{
			
                        hit1 = (BmnDchHit*) pHitCollection0[idch]->UncheckedAt(It->second); ++It; // slower hit
                        hit2 = (BmnDchHit*) pHitCollection0[idch]->UncheckedAt(It->second); ++It; // faster hit
			timeDelta = hit1->GetTShift() - hit2->GetTShift(); // [ns]
			
			if(timeDelta < 0.) // swap
			{
				timeDelta *= -1.;
				tmp = hit1;
				hit1 = hit2;
				hit2 = tmp;			
			}
							
			if(fDoTest)
			{
				htTime->Fill(timeDelta);
				htPerp->Fill(wirePos = hit1->GetWirePosition());
				htGasDrift->Fill(gasDriftDelta = TMath::Abs(hit1->GetDrift() - hit2->GetDrift()));
		        //cout<<"timeDelta = "<<timeDelta<<" "<<gasDriftDelta<<endl;		
			}
			if(!HitExist(timeDelta)) // overlap
			{								
				hit2->AddLinks(hit1->GetLinks()); pHitCollection0[idch]->Remove(hit1);  // index2 faster

				// ->SetFlag(warning#=2); // FIXME: define errorFlags enum				
			}
			else if(fDoTest)
			{
				htTimeA->Fill(timeDelta);
				htPerpA->Fill(wirePos);
				htGasDriftA->Fill(gasDriftDelta);
			}
		}
		else if(counter > 2)		// multiple hit
		{
			// update map
			mapDelay.clear();
			for(Int_t i = 0; i < counter; i++)
			{
				hit1 = (BmnDchHit*) pHitCollection0[idch]->UncheckedAt(It->second);
				mapDelay.insert(delayMap::value_type(hit1->GetTShift(), hit1));
                                ++It;
			}
		
			 // Cycle from biggest to smallest time delay
			for(delayMap::iterator it = mapDelay.begin(); ; )
			{
				hit1 = it->second; 				// slower hit
                                ++it;
				
				if(it != mapDelay.end()) hit2 = it->second; 	// faster hit
				else break;		// last pair
				
				timeDelta = hit1->GetTShift() - hit2->GetTShift();
									
				if(fDoTest)
				{ 
					htTime->Fill(timeDelta);
					htPerp->Fill(wirePos = hit1->GetWirePosition());  
					htGasDrift->Fill(gasDriftDelta = TMath::Abs(hit1->GetDrift() - hit2->GetDrift()));	
				}	
				
				if(!HitExist(timeDelta)) // overlap, remove larger delay time
				{ 									
					hit2->AddLinks(hit1->GetLinks()); 
					pHitCollection0[idch]->Remove(hit1); 
					
					// ->SetFlag(warning#=3); // FIXME: define errorFlags enum										
				} 
				else if(fDoTest) 
				{
					htTimeA->Fill(timeDelta);
					htPerpA->Fill(wirePos);
					htGasDriftA->Fill(gasDriftDelta);
				}
				
			} // cycle by mapDelay
		
				
		} // multiple hit		
	}

	pHitCollection0[idch]->Compress();
	//pHitCollection0[idch]->Sort(); // in ascending order in abs(Z)

	BmnDchHit 	*dchHit;
	Int_t nDchHit = pHitCollection0[idch]->GetEntriesFast();
        if(checkDch)cout<<"Number of DCH corrected hits = "<<nDchHit<<endl;

	for(Int_t i = 0; i < nDchHit; i++ )  // <---Loop over the DCH hits
	{
                dchHit = (BmnDchHit*) pHitCollection0[idch]->At(i);
                uidLocal=dchHit->GetLayer(); 
                uidLocal%2==0?(hitWire=UInt_t(Int_t(dchHit->GetWirePosition())+Int_t(halfNumWiresPerLayer))):(hitWire=UInt_t(Int_t(dchHit->GetWirePosition()-0.5)+Int_t(halfNumWiresPerLayer)));
                //uidLocal%2?(hitWire=UInt_t(floor(dchHit->GetWirePosition())+Int_t(halfNumWiresPerLayer))):(hitWire=UInt_t(floor(dchHit->GetWirePosition()+0.5)+Int_t(halfNumWiresPerLayer)));
                if(checkDch)cout<<"hitWire = "<<hitWire<<" "<<dchHit->GetWirePosition()<<", uidLocal = "<<uidLocal<<", trackId = "<<dchHit->GetTrackID()<<endl; 
                //cout<<" angle wire position correction = "<<dchHit->GetAngleCorrWirePos()<<endl;
                //uidLocal%2==0?(hitWire=UInt_t(Int_t(dchHit->GetWirePosition())+Int_t(halfNumWiresPerLayer))):(hitWire=UInt_t(Int_t(dchHit->GetAngleCorrWirePos()-0.5)+Int_t(halfNumWiresPerLayer)));
                //cout<<"hitWire = "<<hitWire<<" "<<dchHit->GetAngleCorrWirePos()<<", uidLocal = "<<uidLocal<<", trackId = "<<dchHit->GetTrackID()<<endl; 
		if(wireUsed[uidLocal][hitWire]==false)wireUsed[uidLocal][hitWire]=true; 
                dchhitplane[uidLocal]->SetDchPlaneHit(ijkl[uidLocal]++, dchHit->GetWirePosition(),0.,dchHit->GetTrackID(),dchHit->GetDetectorID(),hitWire,fabs(dchHit->GetDrift()),dchHit->GetDriftTime(),dchHit->GetRefIndex());
                //cout<<dchHit->GetDriftTime()<<endl;
                //cout<<" hits "<<ijkl[uidLocal]<<" "<<fDchNum-1<<" "<<uidLocal<<endl;
                if(checkDch)cout<<" hit position (global)(x,y,z): "<<dchHit->GetX()<<", "<<dchHit->GetY()<<", "<<dchHit->GetZ()<<", "<<dchHit->GetTrackID()<<", "<<dchHit->GetRefIndex()<<endl;
        }

        //for (UShort_t j = 0; j < numChambers; j++) {
          for (UShort_t k = 0; k < numLayers; k++) {
             dchhitplane[k]->SetDchPlaneHitsNumber(ijkl[k],k);
             if(ijkl[k]>0)planeUsed[k]=true;
             if(checkDch)cout<<"number of hits in plane "<<k<<" of drift chamber "<<idch+1<<" = "<<ijkl[k]<<endl;
             //ijkl[j][k] = 0;
          }
        //}

        UShort_t topol=PlanesTopology(idch);
        //if(topol<2){
        if(topol==0){
        //if(topol==1){
          //dchTrCand = new DchTrackCand();
          //HitFinder(dchTrCand);
          HitFinder(idch);
          //hitsFound=true;
          //dchTrCand->fDchTrackCand->Print();
        }else{
          cout<<"Not enough hits in DCH"<<idch+1<<"! Hits not produced!"<<endl;
        }  

        
        if(checkGraphs){
         //hXYZpoints->Write(); 
         //hXYZcombhits->Write(); 
         gList.Add(hXYZpoints); 
         gList.Add(hXYZcombhits); 
        }
	
 	//cout<<" "<<pHitCollection0[idch]->GetEntriesFast()<<"("<<hitID<<") hits created.\n";

        //for (UShort_t j = 0; j < numChambers; j++) {
        for (UShort_t k = 0; k < numLayers; k++) {
           delete dchhitplane[k];
           dchhitplane[k] = 0;
        }
        //}
        
       //gObjectTable->Print();
      }
      //dchTracks->DchTrackMerger(dchTrCand1,dchTrCand2);
      //delete dchTrCand1;
      //delete dchTrCand2;
      


}

//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::HitFinder(UShort_t idch){ 

  Double_t x[numLayers/2],y[numLayers/2];
  UInt_t numPlaneHits[numLayers];
  //UShort_t ijk=0;
  UShort_t ijk[numLayers/2]; //index of main plane in pair 
  UInt_t trcand=0;
  Int_t hitID=0;
  Double_t hittmp[2],hitx2[2],hity1[2],hitq3[2],hitq4[2];
  Double_t y1,x2,q3,q4; 
  TVector3 pos, dpos;	
  //BmnDchHit *dchCombHit;
  UShort_t nhits[numLayers];
  Int_t jjgr2=0;
  FairMCPoint     *dchPoint;
  ScalarD* hitX[numLayers];ScalarI* trackId[numLayers];ScalarI* hitId[numLayers];ScalarI* detId[numLayers];ScalarUI* hitwire[numLayers];ScalarD* driftlen[numLayers];ScalarD* drifttim[numLayers];ScalarI* pointind[numLayers];

  TVector2 &uOrtRef=uOrt;
  TVector2 &vOrtRef=vOrt;
 
          for(UShort_t i = 0; i < numLayers; i+=2) {
             if(planeUsed[i]){
               ijk[i/2]=i;
               //cout<<"1ijk "<<ijk[i/2]<<" "<<i<<endl;
             }else if ((!planeUsed[i])&&planeUsed[i+1]){
               ijk[i/2]=i+1;
               //cout<<"2ijk "<<ijk[i/2]<<" "<<i<<endl;    
             }
          } 

          numPlaneHits[ijk[0]]=dchhitplane[ijk[0]]->GetDchPlaneHitsNumber(ijk[0]); //hits in plane k
          //cout<<" numPlaneHits0 = "<<numPlaneHits0<<", numLayer = "<<k<<", drift chamber = "<<fDchNum<<endl;
          for (UInt_t i = 0; i < numPlaneHits[ijk[0]]; i++) {
              ReturnPointers(ijk[0],i,hitX[ijk[0]],trackId[ijk[0]],detId[ijk[0]],hitwire[ijk[0]],driftlen[ijk[0]],drifttim[ijk[0]],pointind[ijk[0]]);
              UInt_t hw0=hitwire[ijk[0]]->GetSV();
              Double_t driftdist0=driftlen[ijk[0]]->GetSV();
              LRambiguity(ijk[0],hittmp,hitX[ijk[0]]->GetSV(),driftdist0,hw0,nhits[ijk[0]]);
              for (UInt_t ii = 0; ii < nhits[ijk[0]]; ii++) {
                hity1[ii]=hittmp[ii];
                if(checkDch)cout<<"hittmp0: "<<hittmp[ii]<<" "<<ii<<endl;
              }
              numPlaneHits[ijk[1]]=dchhitplane[ijk[1]]->GetDchPlaneHitsNumber(ijk[1]); //hits in plane 2
              for (UInt_t j = 0; j < numPlaneHits[ijk[1]]; j++) {
               ReturnPointers(ijk[1],j,hitX[ijk[1]],trackId[ijk[1]],detId[ijk[1]],hitwire[ijk[1]],driftlen[ijk[1]],drifttim[ijk[1]],pointind[ijk[1]]);
               UInt_t hw2=hitwire[ijk[1]]->GetSV();
               Double_t driftdist2=driftlen[ijk[1]]->GetSV();
               LRambiguity(ijk[1],hittmp,hitX[ijk[1]]->GetSV(),driftdist2,hw2,nhits[ijk[1]]);
               for (UInt_t ii = 0; ii < nhits[ijk[1]]; ii++) {
                 hitx2[ii]=hittmp[ii];
                 if(checkDch)cout<<"hittmp1: "<<hittmp[ii]<<" "<<ii<<endl;
               }
               //cout<<"kDch = "<<ijk[2]<<endl;
               numPlaneHits[ijk[2]]=dchhitplane[ijk[2]]->GetDchPlaneHitsNumber(ijk[2]); //hits in plane 4 
               for (UInt_t k = 0; k < numPlaneHits[ijk[2]]; k++) {
                ReturnPointers(ijk[2],k,hitX[ijk[2]],trackId[ijk[2]],detId[ijk[2]],hitwire[ijk[2]],driftlen[ijk[2]],drifttim[ijk[2]],pointind[ijk[2]]);
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
                  ReturnPointers(ijk[3],l,hitX[ijk[3]],trackId[ijk[3]],detId[ijk[3]],hitwire[ijk[3]],driftlen[ijk[3]],drifttim[ijk[3]],pointind[ijk[3]]);
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
	                  if(fDoTest){if(k==4)hXYhit->Fill(x[3],y[3]);}
                          Double_t dX = pRandom->Gaus(0., hitErr[0]);	
                          Double_t dY = pRandom->Gaus(0., hitErr[1]);	
	                  pos.SetXYZ(x[jk]+detXshift[idch]+dX, y[jk]+dY,zLayer[idch][ijk[jk]]);
		          dpos.SetXYZ(dX,dY,hitErr[2]); 
		          //dpos.SetXYZ(hitErr[0],hitErr[1],hitErr[2]); 
                          if(fDoTest||checkDch){
                           if(idch==0){
                            dchPoint = (FairMCPoint*) fBmnDchPointsArray->At(pointind[ijk[jk]]->GetSV());
                           }else if(idch==1){ 
                            dchPoint = (FairMCPoint*) fBmnDchPointsArray2->At(pointind[ijk[jk]]->GetSV());
                           }
                          }
                          //if(fDoTest){ 
                           //hResolX->Fill(x[jk]+detXshift[fDchNum-1]-dchPoint->GetX());
                           //hResolY->Fill(y[jk]-dchPoint->GetY());
                           //hResolR->Fill(sqrt(pow(x[jk]+detXshift[fDchNum-1],2.)+pow(y[jk],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.)));
                          //}
                          if(checkGraphs){
                            //hXYZcombhits->SetPoint(jjgr2++,x[jk],y[jk],zLayer[idch][ijk[jk]]);
                            hXYZcombhits->SetPoint(jjgr2++,zLayer[idch][ijk[jk]],x[jk]+detXshift[idch],y[jk]+detYshift[idch]); 
                            if(jk==3)ExtrapToDch(x,y,zLayer,idch,ijk,jjgr2);
                          } 
                          if(checkDch){
                           cout<<"hits2(x,y,z): "<<endl;
                           cout<<"hits2(x,y,z): "<<ijk[jk]<<" "<<pointind[ijk[jk]]->GetSV()<<endl;
                           cout<<"hits2(x,y,z): "<<dchPoint->GetX()<<endl;
                           cout<<"hits(x,y,z): "<<x[jk]<<", "<<y[jk]<<", "<<zLayer[idch][ijk[jk]]<<", hit number = "<<hitID<<endl;
                           cout<<" event = "<<eventNum<<", track cand. = "<<trackId[ijk[jk]]->GetSV()<<", chamber = "<<idch+1<<", difference (x,y,r): "<<x[jk]+detXshift[idch]-dchPoint->GetX()<<" "<<y[jk]-dchPoint->GetY()<<" "<<sqrt(pow(x[jk]+detXshift[idch],2.)+pow(y[jk],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.))<<endl;
                           if(jk==3){
                            cout<<"nhits0 = "<<nhits[0]<<", nhits2 = "<<nhits[2]<<", nhits4 = "<<nhits[4]<<", nhits6 = "<<nhits[6]<<endl; 
                            cout<<"------------------------------------------------------"<<endl; 
                           }
                          }
                          if(jk==3){
                           TVector2 uvec(0.,hitX[ijk[2]]->GetSV()),uvec2; 
                           uvec2=uvec.Rotate(angleLayerRad[4]);
                           TVector2 vvec(0.,hitX[ijk[3]]->GetSV()),vvec2; 
                           vvec2=vvec.Rotate(angleLayerRad[6]);
                           XYZPoint xyzHit1(x[0]+detXshift[idch],y[0]+detYshift[idch],zLayer[idch][ijk[0]]);
                           XYZPoint xyzHit2(x[1]+detXshift[idch],y[1]+detYshift[idch],zLayer[idch][ijk[1]]);
                           XYZPoint xyzHit3(x[2]+detXshift[idch],y[2]+detYshift[idch],zLayer[idch][ijk[2]]);
                           XYZPoint xyzHit4(x[3]+detXshift[idch],y[3]+detYshift[idch],zLayer[idch][ijk[3]]);
                           XYZPoint xyzWire1(0.,hitX[ijk[0]]->GetSV()+detYshift[idch],zLayer[idch][ijk[0]]);
                           XYZPoint xyzWire2(hitX[ijk[1]]->GetSV()+detXshift[idch],0.,zLayer[idch][ijk[1]]);
                           XYZPoint xyzWire3(uvec2.X()+detXshift[idch],uvec2.Y()+detYshift[idch],zLayer[idch][ijk[2]]);
                           XYZPoint xyzWire4(vvec2.X()+detXshift[idch],vvec2.Y()+detYshift[idch],zLayer[idch][ijk[3]]);
                           //if(idch==0)dchTrCand1->SetDchTrackHits(trcand,xyzHit1.X(),xyzHit1.Y(),xyzHit1.Z(),drifttim[ijk[0]]->GetSV(),xyzWire1.Y(),ijk[0],xyzHit2.X(),xyzHit2.Y(),xyzHit2.Z(),drifttim[ijk[1]]->GetSV(),xyzWire2.X(),ijk[1],xyzHit3.X(),xyzHit3.Y(),xyzHit3.Z(),drifttim[ijk[2]]->GetSV(),xyzWire3.X(),xyzWire3.Y(),ijk[2],xyzHit4.X(),xyzHit4.Y(),xyzHit4.Z(),drifttim[ijk[3]]->GetSV(),xyzWire4.X(),xyzWire4.Y(),ijk[3]);
                           if(idch==1)MisAlignment(xyzHit1,xyzHit2,xyzHit3,xyzHit4,xyzWire1,xyzWire2,xyzWire3,xyzWire4); 
                           if(idch==1)DchAlignment(xyzHit1,xyzHit2,xyzHit3,xyzHit4,xyzWire1,xyzWire2,xyzWire3,xyzWire4); 
                           //if(idch==1)dchTrCand2->SetDchTrackHits(trcand,xyzHit1.X(),xyzHit1.Y(),xyzHit1.Z(),drifttim[ijk[0]]->GetSV(),xyzWire1.Y(),ijk[0],xyzHit2.X(),xyzHit2.Y(),xyzHit2.Z(),drifttim[ijk[1]]->GetSV(),xyzWire2.X(),ijk[1],xyzHit3.X(),xyzHit3.Y(),xyzHit3.Z(),drifttim[ijk[2]]->GetSV(),xyzWire3.X(),xyzWire3.Y(),ijk[2],xyzHit4.X(),xyzHit4.Y(),xyzHit4.Z(),drifttim[ijk[3]]->GetSV(),xyzWire4.X(),xyzWire4.Y(),ijk[3]);
                           //trackcand->SetDchTrackCandNumber(trCand,trcand);
                           trcand++;
                           //trackcand->SetDchTrackCandEventNumber(trCand,eventNum);
                           //trCand++;
                           //tree->Fill(); 
                            //trackCand = AddTrack(hitID++, detId[ijk[jk]]->GetSV(), pos, dpos, trackId[ijk[jk]]->GetSV(), pointind[ijk[jk]]->GetSV(), 0, ijk[jk]); 
                            if(checkDch)cout<<"trId0 = "<<trackId[ijk[0]]->GetSV()<<", trId2 = "<<trackId[ijk[1]]->GetSV()<<", trId4 = "<<trackId[ijk[2]]->GetSV()<<", trId6 = "<<trackId[ijk[3]]->GetSV()<<", chamber = "<<idch+1<<endl; 
                            if(trackId[ijk[0]]->GetSV()==trackId[ijk[1]]->GetSV()&&trackId[ijk[1]]->GetSV()==trackId[ijk[2]]->GetSV()&&trackId[ijk[2]]->GetSV()==trackId[ijk[3]]->GetSV()){
                             for (UShort_t jkk = 0; jkk < numLayers/2; jkk++) {
                              if(idch==0){
                               dchPoint = (FairMCPoint*) fBmnDchPointsArray->At(pointind[ijk[jkk]]->GetSV());
                              }else if(idch==1){
                               dchPoint = (FairMCPoint*) fBmnDchPointsArray2->At(pointind[ijk[jkk]]->GetSV());
                              }
                              if(checkDch)cout<<" event = "<<eventNum<<", track cand. = "<<trackId[ijk[jkk]]->GetSV()<<", point ID = "<<pointind[ijk[jkk]]->GetSV()<<", chamber = "<<idch+1<<", difference2 (x,y,r): "<<x[jkk]+detXshift[idch]-dchPoint->GetX()<<" "<<y[jkk]+detYshift[idch]-dchPoint->GetY()<<" "<<sqrt(pow(x[jkk]+detXshift[idch],2.)+pow(y[jkk]+detYshift[idch],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.))<<endl;
                              if(checkDch)cout<<x[jkk]+detXshift[idch]<<", "<<dchPoint->GetX()<<", "<<y[jkk]+detYshift[idch]<<", "<<dchPoint->GetY()<<endl;
                              if(fDoTest){ 
                               //if(numPlaneHits[ijk[0]]==1&&numPlaneHits[ijk[1]]==1&&numPlaneHits[ijk[2]]==1&&numPlaneHits[ijk[3]]==1){ 
                               if(jkk==0)hResolY->Fill(y[jkk]+detYshift[idch]-dchPoint->GetY());
                               if(jkk==1)hResolX->Fill(x[jkk]+detXshift[idch]-dchPoint->GetX());
                               if(jkk==2){
                                TVector2 difVec(x[jkk]+detXshift[idch]-dchPoint->GetX(),y[jkk]+detYshift[idch]-dchPoint->GetY());   
                                TVector2 uProj=difVec.Proj(uOrtRef);
                                //hResolU->Fill(sqrt(pow(x[jkk]+detXshift[idch],2.)+pow(y[jkk]+detYshift[idch],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.)));
                                if(uProj.X()>0.&&uProj.Y()<0.){ 
                                 hResolU->Fill(-uProj.Mod());
                                }else if(uProj.X()<=0.&&uProj.Y()>=0.){
                                 hResolU->Fill(uProj.Mod());
                                }
                               }
                               if(jkk==3){
                                TVector2 difVec(x[jkk]+detXshift[idch]-dchPoint->GetX(),y[jkk]+detYshift[idch]-dchPoint->GetY());   
                                TVector2 vProj=difVec.Proj(vOrtRef);
                                //hResolV->Fill(sqrt(pow(x[jkk]+detXshift[idch],2.)+pow(y[jkk]+detYshift[idch],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.)));
                                if(vProj.X()<0.&&vProj.Y()<0.){
                                 hResolV->Fill(vProj.Mod());
                                }else if(vProj.X()>=0.&&vProj.Y()>=0.){
                                 hResolV->Fill(-vProj.Mod());
                                }
                               } 
                               //}
                              }
                             }
                            }
                          }
	                  dchCombHit = AddHit(hitID++, detId[ijk[jk]]->GetSV(), pos, dpos, trackId[ijk[jk]]->GetSV(), pointind[ijk[jk]]->GetSV(), 0, ijk[jk], idch);
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
                       //cout<<"x = "<<dchPoint->GetX()-detXshift[fDchNum-1]<<", y = "<<dchPoint->GetY()<<endl;
                     } // nn loop 
                    } // mm loop 
                   } // n loop 
                  } // m loop 
                 } // l loop 
               } // k loop 
              } // j loop 
          } // i loop 

       //trackcand->SetDchNumberOfTracksInEvent(trCand,trcand);
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
        //TString str1;
        //str1.Form("%u",Iter);
        fOutputTestFileName = TString("test00CC") + TString(".BmnDchHitProducer_exp") + TString(".root");
        TFile file(fOutputTestFileName, "RECREATE");
	if(fDoTest)
    	{				
             TH1D *h1 = (TH1D*) htTimeA->Clone("htTimeEff"); h1->Divide(htTime); h1->SetYTitle("Efficiency"); h1->Write();		
     	     h1 = (TH1D*) htGasDriftA->Clone("htGasDriftEff"); h1->Divide(htGasDrift); h1->SetYTitle("Efficiency"); h1->Write();
	     h1 = (TH1D*) htPerpA->Clone("htPerpEff"); h1->Divide(htPerp); h1->SetYTitle("Efficiency"); h1->Write();		
      	     fList.Write(); 	
       } 
       //if(Iter==0){
       // splinederivsmooth->Write();
       // spline5rt->Write();
       //}
       calib->Write();
       if(checkGraphs)gList.Write();
       file.Close();
       // TIter nextkey(fdstread1->GetListOfKeys());
       // TKey *key;
       // while (key = (TKey*)nextkey()) {
       // DchTrackCand *trackcand1 = (DchTrackCand*)key->ReadObj();
       // Int_t trackEntries=trackcand1->fDchTrackCand->GetEntriesFast();
       // cout<<"trackEntries1 = "<<trackEntries<<endl;
         /*TGraph *dchtr=calib;
         dchtr->SetName("calibr");
         dchtr->SetTitle("rt calibration curve");
         dchTracks->FinishDch(dchtr);
         delete dchTracks;
         ofstream tmpfile;
         tmpfile.open ("tmpfile");
         tmpfile << ranmin <<endl;
         tmpfile << ranmax <<endl;
         tmpfile.close();*/
 
}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHit* BmnDchHitProducer_exp::AddHit0(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer, UShort_t idch)
{
	BmnDchHit *pHit	=  new ((*pHitCollection0[idch])[index]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	pHit->AddLink(FairLink(1, pointIndex)); 
	pHit->AddLink(FairLink(2, trackIndex)); 
	
return pHit;
}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHit* BmnDchHitProducer_exp::AddHit(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer, UShort_t idch)
{
	//BmnDchHit *pHit	=  new ((*pHitCollection)[index]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	TClonesArray& clref = *pHitCollection[idch];
        Int_t size = clref.GetEntriesFast();
        BmnDchHit *pHit = new(clref[size]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	pHit->AddLink(FairLink(1, pointIndex)); 
	pHit->AddLink(FairLink(2, trackIndex)); 
	
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
    hRadiusRange->Fill(r);
    //cout<<"radius in next plane = "<<r<<endl;

}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::LRambiguity(UInt_t k, Double_t hittmp[2], Double_t hitx, Double_t driftdist, UInt_t hw, UShort_t &nhits){

              if(hw!=0&&hw!=numWiresPerLayer){
               //cout<<"wireUsed: "<<wireUsed[k+1][hw]<<" "<<wireUsed[k+1][hw-1]<<" "<<hw<<endl;
               if(wireUsed[k+1][hw]&&wireUsed[k+1][hw-1]){
                 nhits=2; 
                 hittmp[0]=hitx-driftdist;
                 hittmp[1]=hitx+driftdist;
                 //cout<<"here1"<<endl;
               }else if(!wireUsed[k+1][hw]&&!wireUsed[k+1][hw-1]){
                 nhits=2; 
                 hittmp[0]=hitx-driftdist;
                 hittmp[1]=hitx+driftdist;
                 //cout<<"here2"<<endl;
               }else if(!wireUsed[k+1][hw]&&wireUsed[k+1][hw-1]){
                 hittmp[0]=hitx-driftdist;
                 nhits=1; 
                 //cout<<"here3"<<endl;
               }else if(wireUsed[k+1][hw]&&!wireUsed[k+1][hw-1]){
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
             /*
             x[0]=(x[1]*(k4-k3)+(q4-y[0])/z4121-(q3-y[0])/z3121)/(k4-k3+k3/z3121-k4/z4121);
             x[2]=(x[1]-x[0])*z3121+x[0];
             x[3]=(x[1]-x[0])*z4121+x[0];
             y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
             */
            

             x[0]=(x[1]*(k4-k3)+(q4-y[0])/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]-(q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])])/(k4-k3+k3/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]-k4/zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]);
             //x[2]=(x[1]-x[0])*z3121+x[0];
             x[2]=(x[1]-x[0])*zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]+x[0];
             //x[3]=(x[1]-x[0])*z4121+x[0];
             x[3]=(x[1]-x[0])*zCoeff2[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[3])]+x[0];
             //y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/zCoeff1[ijk[0]][GetGasGap(ijk[1])][GetGasGap(ijk[2])]+y[0];
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
           
 
/*                      cout<<"2------------------------------------------------------"<<endl; 
                      for (UShort_t jk = 0; jk < numLayers/2; jk++) {
                       cout<<"points: "<<x[jk]<<" "<<y[jk]<<endl;
                      }
                      cout<<"2------------------------------------------------------"<<endl; 
*/
} 
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
Double_t BmnDchHitProducer_exp::rtCurve(Double_t time)
{

Double_t radDist;
if(checkDch)cout<<"time = "<<time<<" "<<ranmin<<" "<<ranmax<<endl;
if(time<ranmin){
radDist=0.;
if(checkDch)cout<<"drift time underflow"<<endl;
}else if(time>=ranmin&&time<=ranmax){
radDist=spline5rt->Eval(time-ranmin);
}else{
cout<<"spline5rt "<<time<<" "<<ranmax<<" "<<ranmin<<endl;
radDist=spline5rt->Eval(ranmax-ranmin);
if(checkDch)cout<<"drift time overflow"<<endl;
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
            integral=integral+delta*spline5->Eval(x0)+0.5*delta*(spline5->Eval(x1)-spline5->Eval(x0));   
            //cout<<"splineintegral params.: "<<n<<" "<<delta<<" "<<x0<<" "<<x1<<" "<<spline5->Eval(x0)<<" "<<spline5->Eval(x1)<<" "<<spline5->Eval(x1)-spline5->Eval(x0)<<endl;
            //cout<<"splineintegral = "<<integral<<" "<<a<<" "<<b<<endl;
            x0=x1;
         }
    
return integral;

}
//-------------------------------------------------------------------------------------------------------------------------
UShort_t BmnDchHitProducer_exp::PlanesTopology(UShort_t idch){

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

cout<<"planes topology in DCH "<<idch+1<<" = "<<topol<<", main planes (0 to 4) = "<<pairPlanesUsed<<", planes (0 to 8) = "<<planesUsed<<endl; 

return topol;

}
//-------------------------------------------------------------------------------------------------------------------------
UShort_t BmnDchHitProducer_exp::RunTypeToNumber(TString runType_){

UShort_t j;

           if(runType_=="run1"){j=0;}
           else if(runType_=="run2"){j=1;}
           else if(runType_=="run3"){j=2;}
           else{cout<<"run type not in the list!"<<endl; 
                cout<<" run type = "<<runType_<<endl;}
return j;

}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::ExtrapToDch(const Double_t x[],const Double_t y[],const Double_t zLayer_[][numLayers], const UShort_t idch, const UShort_t ijk[], Int_t &jjgr2){

Double_t delZ=zLayer_[idch][ijk[1]]-zLayer_[idch][ijk[0]],delZ2;
//Double_t delXshift=detXshift[1]-detXshift[0];
Double_t tgx=(x[1]-x[0])/delZ;
Double_t tgy=(y[1]-y[0])/delZ;
if(checkDch)cout<<"tgx = "<<tgx<<", tgy = "<<tgy<<", delZ = "<<delZ<<", x[1] = "<<x[1]<<", x[0] = "<<x[0]<<", y[1] = "<<y[1]<<", y[0] = "<<y[0]<<endl;
/*if(idch==1){
tgx=-tgx;
tgy=-tgy;
}*/
Double_t xExtrap[numLayers],yExtrap[numLayers];

     for(UShort_t i = 0; i < numLayers; i++) {
       delZ2=(zLayerExtrap[idch][i]-zLayer_[idch][ijk[0]]);
       //xExtrap[i]=x[0]+tgx*delZ2+delXshift;
       xExtrap[i]=x[0]+detXshift[idch]+tgx*delZ2;
       if(checkDch)cout<<"xExtrap[i] = "<<xExtrap[i]<<", x[i] = "<<x[i/2]<<", detXshift[idch] = "<<detXshift[idch]<<", x[i] + detXshift[idch] = "<<x[i/2] + detXshift[idch]<<", tgx*delZ2 = "<<tgx*delZ2<<endl;
       //xExtrap[i]=x[0]+tgx*delZ2;
       yExtrap[i]=y[0]+detYshift[idch]+tgy*delZ2;
       if(checkDch)cout<<"yExtrap[i] = "<<yExtrap[i]<<", y[i] = "<<y[i/2]<<", detYshift[idch] = "<<detYshift[idch]<<", y[i] + detYshift[idch] = "<<y[i/2] + detYshift[idch]<<", tgy*delZ2 = "<<tgy*delZ2<<endl;
       hXYZcombhits->SetPoint(jjgr2++,zLayerExtrap[idch][i],xExtrap[i],yExtrap[i]);
       //cout<<"zLayerExtrap: "<<zLayerExtrap[idch][i]<<endl;
     }

}

//------------------------------------------------------------------------------------------------------------------------
//void	BmnDchHitProducer_exp::rtCalibRead(){
void	BmnDchHitProducer_exp::rtCalibCorrection(){


//ScalarD* Ranmin = (ScalarD*)resid.Get("Ranmin");
//ranmin=Ranmin->GetSV();

C0.ResizeTo(4,4);
ifstream myfile;
myfile.open("DCHpar");
myfile >> Iter;
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

TString str;
str.Form("%u",Iter-1);
TString residfile = TString("dchresidsim")+str+TString(".root");
TFile resid(residfile,"read");

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
   *(calibGrY+ipoint)=*(calibGrY+ipoint)-(par[0]+par[1]*x+par[2]*pow(x,2.)+par[3]*pow(x,3.)+par[4]*pow(x,4.)); 

}

//calib[fDchNum-1] = new TGraph(npoints,dt,r);
//spline3rt = new TSpline3("r-t calibration curve",calib, "", 0., 0.);
spline5rt = new TSpline5("r-t calibration curve",calib, "", 0.,0.,0.,0.);
////calib->Fit("pol5");
//calib[fDchNum-1]->SetTitle("r-t calibration curve");
//calib[fDchNum-1]->SetName("rtCalibCurve");
spline5rt->SetName("rtCalibSpline");

}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer_exp::MisAlignment(XYZPoint &xyzHit1,XYZPoint &xyzHit2,XYZPoint &xyzHit3,XYZPoint &xyzHit4,XYZPoint &xyzWire1,XYZPoint &xyzWire2,XYZPoint &xyzWire3,XYZPoint &xyzWire4){

//first chamber position
XYZVector XYZ1(DCH1_Xpos[RunTypeToNumber(runType)],DCH1_Ypos[RunTypeToNumber(runType)],DCH1_Zpos[RunTypeToNumber(runType)]);
//second chamber position
XYZVector XYZ2(DCH2_Xpos[RunTypeToNumber(runType)],DCH2_Ypos[RunTypeToNumber(runType)],DCH2_Zpos[RunTypeToNumber(runType)]);
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
void BmnDchHitProducer_exp::DchAlignment(XYZPoint &xyzHit1,XYZPoint &xyzHit2,XYZPoint &xyzHit3,XYZPoint &xyzHit4,XYZPoint &xyzWire1,XYZPoint &xyzWire2,XYZPoint &xyzWire3,XYZPoint &xyzWire4){
 
//first chamber position
XYZVector XYZ1(DCH1_Xpos[RunTypeToNumber(runType)],DCH1_Ypos[RunTypeToNumber(runType)],DCH1_Zpos[RunTypeToNumber(runType)]);

TVectorD misaligned(4);
TVectorD aligned(4);

XYZPoint xyz5loc1,xyz6loc1,xyz7loc1,xyz8loc1;
XYZPoint xyz5loc1new,xyz6loc1new,xyz7loc1new,xyz8loc1new;
XYZPoint xyz5wireLoc1,xyz6wireLoc1,xyz7wireLoc1,xyz8wireLoc1;
XYZPoint xyz5wireLoc1new,xyz6wireLoc1new,xyz7wireLoc1new,xyz8wireLoc1new;

 xyz5loc1=xyzHit1-XYZ1;
 xyz6loc1=xyzHit2-XYZ1;
 xyz7loc1=xyzHit3-XYZ1;
 xyz8loc1=xyzHit4-XYZ1;
 xyz5wireLoc1=xyzWire1-XYZ1;
 xyz6wireLoc1=xyzWire2-XYZ1;
 xyz7wireLoc1=xyzWire3-XYZ1;
 xyz8wireLoc1=xyzWire4-XYZ1;

 misaligned(0)=xyz5loc1.X();misaligned(1)=xyz5loc1.Y();misaligned(2)=xyz5loc1.Z();misaligned(3)=1.;
 aligned=C0*misaligned;
 xyz5loc1new.SetX(aligned(0));xyz5loc1new.SetY(aligned(1));xyz5loc1new.SetZ(aligned(2));
 misaligned(0)=xyz6loc1.X();misaligned(1)=xyz6loc1.Y();misaligned(2)=xyz6loc1.Z();misaligned(3)=1.;
 aligned=C0*misaligned;
 xyz6loc1new.SetX(aligned(0));xyz6loc1new.SetY(aligned(1));xyz6loc1new.SetZ(aligned(2));
 misaligned(0)=xyz7loc1.X();misaligned(1)=xyz7loc1.Y();misaligned(2)=xyz7loc1.Z();misaligned(3)=1.;
 aligned=C0*misaligned;
 xyz7loc1new.SetX(aligned(0));xyz7loc1new.SetY(aligned(1));xyz7loc1new.SetZ(aligned(2));
 misaligned(0)=xyz8loc1.X();misaligned(1)=xyz8loc1.Y();misaligned(2)=xyz8loc1.Z();misaligned(3)=1.;
 aligned=C0*misaligned;
 xyz8loc1new.SetX(aligned(0));xyz8loc1new.SetY(aligned(1));xyz8loc1new.SetZ(aligned(2));

 xyzHit1=xyz5loc1new+XYZ1;
 xyzHit2=xyz6loc1new+XYZ1;
 xyzHit3=xyz7loc1new+XYZ1;
 xyzHit4=xyz8loc1new+XYZ1;

 misaligned(0)=xyz5wireLoc1.X();misaligned(1)=xyz5wireLoc1.Y();misaligned(2)=xyz5wireLoc1.Z();misaligned(3)=1.;
 aligned=C0*misaligned;
 xyz5wireLoc1new.SetX(aligned(0));xyz5wireLoc1new.SetY(aligned(1));xyz5wireLoc1new.SetZ(aligned(2));
 misaligned(0)=xyz6wireLoc1.X();misaligned(1)=xyz6wireLoc1.Y();misaligned(2)=xyz6wireLoc1.Z();misaligned(3)=1.;
 aligned=C0*misaligned;
 xyz6wireLoc1new.SetX(aligned(0));xyz6wireLoc1new.SetY(aligned(1));xyz6wireLoc1new.SetZ(aligned(2));
 misaligned(0)=xyz7wireLoc1.X();misaligned(1)=xyz7wireLoc1.Y();misaligned(2)=xyz7wireLoc1.Z();misaligned(3)=1.;
 aligned=C0*misaligned;
 xyz7wireLoc1new.SetX(aligned(0));xyz7wireLoc1new.SetY(aligned(1));xyz7wireLoc1new.SetZ(aligned(2));
 misaligned(0)=xyz8wireLoc1.X();misaligned(1)=xyz8wireLoc1.Y();misaligned(2)=xyz8wireLoc1.Z();misaligned(3)=1.;
 aligned=C0*misaligned;
 xyz8wireLoc1new.SetX(aligned(0));xyz8wireLoc1new.SetY(aligned(1));xyz8wireLoc1new.SetZ(aligned(2));

 xyzWire1=xyz5wireLoc1new+XYZ1;
 xyzWire2=xyz6wireLoc1new+XYZ1;
 xyzWire3=xyz7wireLoc1new+XYZ1;
 xyzWire4=xyz8wireLoc1new+XYZ1;

}
//-------------------------------------------------------------------------------------------------------------------------
ClassImp(BmnDchHitProducer_exp)
