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
#include "TRandom.h"
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
#include "BmnDchHitProducer.h"
//#include <TObjectTable.h>

// constants definition
        const Double_t BmnDchHitProducer::detXshift[numChambers]={DCH1_Xpos,DCH2_Xpos}; // DCH1 and DCH2 x shifts, cm
        const Double_t BmnDchHitProducer::cosPhi_45 = TMath::Cos(-45.*TMath::DegToRad()); 
        const Double_t BmnDchHitProducer::sinPhi_45 = TMath::Sin(-45.*TMath::DegToRad());
        const Double_t BmnDchHitProducer::cosPhi45 = TMath::Cos(45.*TMath::DegToRad()); 
        const Double_t BmnDchHitProducer::sinPhi45 = TMath::Sin(45.*TMath::DegToRad());
	const Double_t BmnDchHitProducer::Phi_45 = -45.*TMath::DegToRad(); 
        const Double_t BmnDchHitProducer::Phi45 = 45.*TMath::DegToRad(); 
        const Double_t BmnDchHitProducer::Phi90 = 90.*TMath::DegToRad();
	const Double_t BmnDchHitProducer::sqrt2=sqrt(2.); // 
        const Double_t BmnDchHitProducer::tg3=tan(angleLayerRad[4]);
        const Double_t BmnDchHitProducer::tg4=tan(angleLayerRad[6]);
	//const Double_t BmnDchHitProducer::gasDriftSpeed = 5.e-3; // 50 mkm/ns == 5.e-3 cm/ns 
	const Double_t BmnDchHitProducer::gasDriftSpeed = 2.5e-3; // 25 mkm/ns == 2.5e-3 cm/ns 
	const Double_t BmnDchHitProducer::wireDriftSpeed =  20; // 5 ns/m  ==  20 cm/ns
	const Double_t BmnDchHitProducer::resolution =  15e-3; // 150 mkm  ==  0.015 cm
	const Double_t BmnDchHitProducer::hitErr[3] = {resolution,resolution,0.};
	const Double_t BmnDchHitProducer::WheelR_2 = pow(MaxRadiusOfActiveVolume,2.); // cm x cm

//------------------------------------------------------------------------------------------------------------------------
struct __ltstr
{
	bool operator()(Double_t s1, Double_t s2) const
  	{
    		return s1 > s2;
  	}
};

//------------------------------------------------------------------------------------------------------------------------
BmnDchHitProducer::BmnDchHitProducer(UInt_t num = 1, Int_t verbose, Bool_t test) : FairTask("Dch HitProducer", verbose), fDoTest(test), fRSigma(0.2000), fRPhiSigma(0.0200), fOnlyPrimary(kFALSE)
{
	pRandom = new TRandom2;
        fDchNum = num;
        TString str;
        str.Form("%d",fDchNum);
        fInputBranchName = TString("DCH") + str + TString("Point");
        fOutputHitsBranchName0 = "BmnDchHit0";
        fOutputHitsBranchName = "BmnDchHit";

}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHitProducer::~BmnDchHitProducer() 
{
	delete pRandom;	
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer::InitDchParameters(){
  checkDch=kFALSE;
  //checkDch=kTRUE;
  eventNum=0; 
  if(fDoTest)BookHistograms(); 
  
  if(fDchNum==1){
        z3121=(DCH1_ZlayerPos_global[4]-DCH1_ZlayerPos_global[0])/(DCH1_ZlayerPos_global[2]-DCH1_ZlayerPos_global[0]);
        z4121=(DCH1_ZlayerPos_global[6]-DCH1_ZlayerPos_global[0])/(DCH1_ZlayerPos_global[2]-DCH1_ZlayerPos_global[0]);
        //z4131=(DCH1_ZlayerPos_global[6]-DCH1_ZlayerPos_global[0])/(DCH1_ZlayerPos_global[4]-DCH1_ZlayerPos_global[0]);
      }else if(fDchNum==2){
        z3121=(DCH2_ZlayerPos_global[4]-DCH2_ZlayerPos_global[0])/(DCH2_ZlayerPos_global[2]-DCH2_ZlayerPos_global[0]);
        z4121=(DCH2_ZlayerPos_global[6]-DCH2_ZlayerPos_global[0])/(DCH2_ZlayerPos_global[2]-DCH2_ZlayerPos_global[0]);
        //z4131=(DCH2_ZlayerPos_global[6]-DCH2_ZlayerPos_global[0])/(DCH2_ZlayerPos_global[4]-DCH2_ZlayerPos_global[0]);
  }

  for (UInt_t k = 0; k < numLayers; k++) {
    //for (UInt_t i = 0; i < numChambers; i++) {
      if(fDchNum==1){
        zLayer[k]=DCH1_ZlayerPos_global[k]; 
        //zgap[fDchNum-1][k/2]=(DCH1_ZlayerPos_global[k]+DCH1_ZlayerPos_global[k+2])/2.;
      }else if(fDchNum==2){
        zLayer[k]=DCH2_ZlayerPos_global[k]; 
        //zgap[fDchNum-1][k/2]=(DCH2_ZlayerPos_global[k]+DCH2_ZlayerPos_global[k+2])/2.;
      }
    //}
  }

  //if(checkDch){
   cout<<"DCH = "<<fDchNum<<endl;
   cout<<"Z of DCH layer (lab coordinate frame): "<<endl;
   for (UInt_t k = 0; k < numLayers; k++) {
    if(fDchNum==1){
     cout<<DCH1_ZlayerPos_global[k]<<" ";
    }else if(fDchNum==2){
     cout<<DCH2_ZlayerPos_global[k]<<" ";
    }
   }
   cout<<endl;
   cout<<"DCH X shift = "<<detXshift[fDchNum-1]<<endl;
  //}
  xyTolerance=3.*resolution; 
  //radialRange=2.; //cm (AuAu)
  radialRange=3.; //cm (CC)

}
//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer::BookHistograms(){

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
		hResolX = new TH1D("hResolX", "hit X coordinate resolution", 100, -10., 10.); 
		hResolX->SetDirectory(0); fList.Add(hResolX);		
		hResolY = new TH1D("hResolY", "hit Y coordinate resolution", 100, -10., 10.); 
		hResolY->SetDirectory(0); fList.Add(hResolY);		
		hResolR = new TH1D("hResolR", "hit R coordinate resolution", 100, 0., 10.); 
		hResolR->SetDirectory(0); fList.Add(hResolR);		
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
InitStatus 		BmnDchHitProducer::Init() 
{

        cout << " BmnDchHitProducer::Init() " << endl;

        InitDchParameters();
        
        if (fOnlyPrimary) cout << " Only primary particles are processed!!! " << endl;

    //Get ROOT Manager
        FairRootManager* ioman = FairRootManager::Instance();

  	if(ioman==0){ Error("BmnDchHitProducer::Init","FairRootManager XUINJA"); return kERROR; }
        fBmnDchPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
        fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
  	if(!fBmnDchPointsArray  || !fMCTracksArray){ Error("BmnDchHitProducer::Init","Branch not found!"); return kERROR; }
  	// Create and register output array
  	pHitCollection0 = new TClonesArray(fOutputHitsBranchName);  
  	pHitCollection = new TClonesArray(fOutputHitsBranchName);  
        TString str; 
        str.Form("%d",fDchNum);
        TString str0; 
        str0.Form("%d",0);
        TString name0   = TString("BmnDch") + str + TString("Hit") + str0;
        TString name   = TString("BmnDch") + str + TString("Hit");
        TString folder = TString("DCH") + str;
        ioman->Register(name0.Data(), folder.Data(), pHitCollection0, kTRUE);
        ioman->Register(name.Data(), folder.Data(), pHitCollection, kTRUE);

        cout << " Initialization finished succesfully. " << endl;
	
return kSUCCESS;

}
//------------------------------------------------------------------------------------------------------------------------
void			BmnDchHitProducer::Rotate(UInt_t proj, Double_t x, Double_t y, Double_t& xRot, Double_t& yRot, Bool_t back)
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
inline Double_t			BmnDchHitProducer::GetPhi(UInt_t proj)
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
Double_t		BmnDchHitProducer::GetDriftLength(UInt_t proj, UInt_t gasgap, Double_t x, Double_t& wirePos)
{
	//   ... -1	0	1	...  - first(0) gap wire  position (X) [cm]
	//   ...    -0.5   0.5     1.5  ...  - second(1) gap wire  position (X) [cm]
	
	wirePos = (gasgap == 0) ? TMath::Nint(x) : TMath::Nint(x + 0.5) - 0.5;
		
	//AZ return TMath::Abs(x - wirePos);	// [cm]
return x - wirePos;	// [cm]
}
//------------------------------------------------------------------------------------------------------------------------
Bool_t 			BmnDchHitProducer::HitExist(Double_t delta) // [ns] 
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
Double_t		BmnDchHitProducer::GetTShift(Double_t driftLength, Double_t wirePos, Double_t R, Double_t& L)
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
Int_t		BmnDchHitProducer::WireID(UInt_t uid, Double_t wirePos, Double_t R)
{
	//uid--; // uid [0,15]
	uid++; // uid [1,16]
	// wirePos: [-MaxRadiusOfActiveVolume,MaxRadiusOfActiveVolume]cm
	// tube R: MinRadiusOfActiveVolume

	if(wirePos > -MinRadiusOfActiveVolume && wirePos < MinRadiusOfActiveVolume) if(R > 0)return (int)(wirePos + 1000.*uid + 500); // two wires 

return (int)(wirePos + 1000.*uid);  // one wire 
}
//------------------------------------------------------------------------------------------------------------------------
void 			BmnDchHitProducer::Exec(Option_t* opt) 
{
  	pHitCollection0->Delete();
  	pHitCollection->Delete();
        
        //gObjectTable->Print();
  
        if(checkDch){	
         hXYZcombhits = new TGraph2D(); 
         hXYZcombhits->SetNameTitle("hXYZcombhits","Hits in DCH");
         hXYZpoints = new TGraph2D(); 
         hXYZpoints->SetNameTitle("hXYZpoints","GEANT points in DCH");
        }
	Int_t           hitID = 0;
        UInt_t           ijkl[numLayers];
	fMapOccup.clear();
        //for (UShort_t j = 0; j < numChambers; j++) {
          for (UShort_t k = 0; k < numLayers; k++) {
             ijkl[k]=0;
             dchhitplane[k] = new DchHitPlane();
             for (UInt_t j = 0; j < numWiresPerLayer; j++) {
              wireUsed[k][j]=false;
             }
          }
        //}
          for (UShort_t k = numLayers; k < numLayers+ngaps; k++) {
             dchhitplane[k] = new DchHitPlane();
          }
          for (UShort_t k = 0; k < ngaps; k++) {
             ijkl2[k]=0;
          }

	Int_t nDchPoint = fBmnDchPointsArray->GetEntriesFast();
        if(checkDch)cout << " BmnDchHitProducer::Exec(), Number of BmnDchPoints = " << nDchPoint <<", event number = "<<eventNum++<<", chamber = "<<fDchNum<<endl;

	FairMCPoint     *dchPoint;
	BmnDchHit	*pHit;
        Double_t	R, Rphi, x, y, xRot, yRot, xRot1, yRot1, driftLength, wirePos, L;
	Double_t 	dRphi = 0., dR = 0.;
	Double_t 	dchZpos;
	TVector3	pos, dpos;	
        UInt_t   	uid, uidLocal, wheel, gasgap, proj;
        Int_t   	detID;
        UInt_t   	hitWire;
        //Int_t           ijkl[numChambers][numLayers]={{0}};
        Int_t jjgr=0; 

        if (!fBmnDchPointsArray) {
         Error("BmnDchHitProducer::Init()", " !!! Unknown branch name !!! ");
         return;
        }
	for(Int_t i = 0; i < nDchPoint; i++ )  // <---Loop over the DCH points
	{
                dchPoint = (FairMCPoint*) fBmnDchPointsArray->UncheckedAt(i);
                if(checkDch){
                 cout<<"DCH points (local): "<<dchPoint->GetX()-detXshift[fDchNum-1]<<", "<<dchPoint->GetY()<<", "<<dchPoint->GetZ()<<endl;
                 cout<<"DCH points (global): "<<dchPoint->GetX()<<", "<<dchPoint->GetY()<<", "<<dchPoint->GetZ()<<endl;
                }

///////		if( ((FairMCTrack*)pMCTracks->UncheckedAt(pPoint1->GetTrackID()))->GetMotherId() != -1)continue; // primary ONLY !!!
                if(checkDch)hXYZpoints->SetPoint(jjgr++,dchPoint->GetX()-detXshift[fDchNum-1],dchPoint->GetY(),dchPoint->GetZ());
                //RadialRange(dchPoint);
                if (fOnlyPrimary) {
                  if (dchPoint->GetTrackID() < 0) continue; 
                  CbmMCTrack* track = (CbmMCTrack*) fMCTracksArray->At(dchPoint->GetTrackID());
                  if (!track) continue;
                  if (track->GetMotherId() != -1) continue;
                }
	      
                //cout<<"---------------------------------------------------------------------"<<endl;	
		detID = dchPoint->GetDetectorID();
                uidLocal=0;
                if (fDchNum == 1) {dchZpos=DCH1_Zpos;}
                else if (fDchNum == 2) {dchZpos=DCH2_Zpos;}
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
                wheel = fDchNum-1; // [0-1] == [inner,outer] 
		proj = GetProj(uid);	    // [0-3] == [x,y,u,v] 
		gasgap = GetGasGap(uid); // [0-1] == [inner,outer] 

                //cout<<"wheel = "<<wheel<<", proj = "<<proj<<", gasgap = "<<gasgap<<endl;

		if(fDoTest){
                  hX->Fill(dchPoint->GetX()-detXshift[fDchNum-1]);
                  hY->Fill(dchPoint->GetY());
                  hZ->Fill(dchPoint->GetZ());
                } 
		//Rotate(proj, x = dchPoint->GetX()-detXshift[fDchNum-1], y = dchPoint->GetY(), xRot1, yRot1); // GlobalToLocal
	        //cout<<"y = "<<y<<" "<<"x = "<<x<<" "<<proj<<endl;	
	        //cout<<"yRot1 = "<<yRot1<<" "<<"xRot1 = "<<xRot1<<endl;	
                TVector2 xy2,xy(dchPoint->GetX()-detXshift[fDchNum-1],dchPoint->GetY());
                xy2=xy.Rotate(-angleLayerRad[uidLocal]);//GlobalToLocal
                xRot=xy2.X();yRot=xy2.Y();
                if(uidLocal==2||uidLocal==3)yRot=-yRot;
		driftLength = GetDriftLength(proj, gasgap, yRot, wirePos); // [cm]
		//driftLength = GetDriftLength(proj, gasgap, xRot1, wirePos); // [cm]
	        //cout<<"yRot2 = "<<yRot<<", xRot2 = "<<xRot<<", angle = "<<angleLayerRad[uidLocal]<<", plane ID = "<<uidLocal<<", chamber = "<<fDchNum<<", wirepos = "<<wirePos<<endl;	
	        //cout<<"driftLength = "<<driftLength<<endl;	
                //cout<<"wirepos = "<<wirePos<<", xRot = "<<xRot<<", yRot = "<<yRot<<", z = "<<dchPoint->GetZ()<<", angle = "<<-angleLayer[uidLocal]<<endl;
		//R =  yRot1; 
		R =  xRot; 
		Rphi = driftLength;
		pRandom->Rannor(dRphi,dR);
				
		if(fDoTest)
		{ 
			htWireN->Fill(WireID(uid, wirePos, R));
			htXYlocal->Fill(Rphi, R);
			htRvsR->Fill(sqrt(pow(dchPoint->GetX()-detXshift[fDchNum-1],2.) + pow(dchPoint->GetY(),2.)), sqrt(R*R + wirePos*wirePos));
			htMCTime->Fill(dchPoint->GetTime());
		}
				
/////		xRot = pRandom->Gaus(xRot, fRSigma/10.); // [cm]
	
/////		Rotate(proj, xRot, yRot, x, y, true); // back rotate
/////		pos.SetXYZ(x, y, pPoint1->GetZ());
	
                Double_t dX = pRandom->Gaus(0., hitErr[0]);	
                Double_t dY = pRandom->Gaus(0., hitErr[1]);	
		//pos.SetXYZ(dchPoint->GetX(), dchPoint->GetY(), dchPoint->GetZ());
		pos.SetXYZ(dchPoint->GetX()+dX, dchPoint->GetY()+dY, dchPoint->GetZ());
		//dpos.SetXYZ(dX,dY,hitErr[2]); 
		dpos.SetXYZ(hitErr[0],hitErr[1],hitErr[2]); 
		
                //cout<<"hitID = "<<hitID<<", uid = "<<uid<<endl;
                //cout<<pos<<" "<<dpos<<endl;
                //cout<<"track id = "<<dchPoint->GetTrackID()<<", point index = "<<i<<endl;
		pHit = AddHit0(hitID, detID, pos, dpos, dchPoint->GetTrackID(), i, 0, uid);
		pHit->SetPhi(GetPhi(proj));
    		pHit->SetMeas(Rphi + dRphi * fRPhiSigma); 	// R-Phi
    		pHit->SetError(fRPhiSigma);
    		pHit->SetMeas(R + dR * fRSigma, 1);		// R
    		pHit->SetError(fRSigma, 1);
						
		pHit->SetDrift(fabs(driftLength));
		pHit->SetWirePosition(wirePos);
		pHit->SetTShift(dchPoint->GetTime() + GetTShift(driftLength, wirePos, R, L));			
		pHit->SetWireDelay(L);
		
		fMapOccup.insert(occupMap::value_type(WireID(uid, wirePos, R), hitID++)); // <Hash(wirePos,UID) == cellID, index> pair
                //cout<<"wireID "<<WireID(uid, wirePos, R)<<" "<<wirePos<<endl;
                //cout<<"TShift "<<GetTShift(driftLength, wirePos, R, L)<<endl;

	} // <---Loop over the DCH points

        
	//Int_t nDchHit = pHitCollection0->GetEntriesFast();
        if(checkDch)cout<<"Number of DCH hits = "<<pHitCollection0->GetEntriesFast()<<endl;

	
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
			
                        hit1 = (BmnDchHit*) pHitCollection0->UncheckedAt(It->second); ++It; // slower hit
                        hit2 = (BmnDchHit*) pHitCollection0->UncheckedAt(It->second); ++It; // faster hit
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
				hit2->AddLinks(hit1->GetLinks()); pHitCollection0->Remove(hit1);  // index2 faster

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
				hit1 = (BmnDchHit*) pHitCollection0->UncheckedAt(It->second);
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
					pHitCollection0->Remove(hit1); 
					
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

	pHitCollection0->Compress();
	//pHitCollection0->Sort(); // in ascending order in abs(Z)

	BmnDchHit 	*dchHit;
	Int_t nDchHit = pHitCollection0->GetEntriesFast();
        if(checkDch)cout<<"Number of DCH corrected hits = "<<nDchHit<<endl;

	for(Int_t i = 0; i < nDchHit; i++ )  // <---Loop over the DCH hits
	{
                dchHit = (BmnDchHit*) pHitCollection0->At(i);
                uidLocal=dchHit->GetLayer(); 
                uidLocal%2?(hitWire=UInt_t(Int_t(dchHit->GetWirePosition())+Int_t(halfNumWiresPerLayer))):(hitWire=UInt_t(Int_t(dchHit->GetWirePosition()+0.5)+Int_t(halfNumWiresPerLayer)));
                //cout<<"hitWire = "<<hitWire<<" "<<dchHit->GetWirePosition()<<endl; 
		if(wireUsed[uidLocal][hitWire]==false)wireUsed[uidLocal][hitWire]=true; 
                dchhitplane[uidLocal]->SetDchPlaneHit(ijkl[uidLocal]++, dchHit->GetWirePosition(),0.,dchHit->GetTrackID(),dchHit->GetDetectorID(),hitWire,fabs(dchHit->GetDrift()),dchHit->GetRefIndex());
                //cout<<" hits "<<ijkl[uidLocal]<<" "<<fDchNum-1<<" "<<uidLocal<<endl;
                //cout<<" hit position (global)(x,y,z): "<<dchHit->GetX()<<" "<<dchHit->GetY()<<" "<<dchHit->GetZ()<<endl;
        }

        //for (UShort_t j = 0; j < numChambers; j++) {
          for (UShort_t k = 0; k < numLayers; k++) {
             dchhitplane[k]->SetDchPlaneHitsNumber(ijkl[k],k);
             if(checkDch)cout<<"number of hits in plane "<<k<<" of drift chamber "<<fDchNum<<" = "<<ijkl[k]<<endl;
             //ijkl[j][k] = 0;
          }
        //}

        HitFinder();
        
        if(checkDch){
         hXYZpoints->Write(); 
         hXYZcombhits->Write(); 
        }
	
 	//cout<<" "<<pHitCollection0->GetEntriesFast()<<"("<<hitID<<") hits created.\n";

        //for (UShort_t j = 0; j < numChambers; j++) {
          for (UShort_t k = 0; k < numLayers+ngaps; k++) {
             delete dchhitplane[k];
             dchhitplane[k] = 0;
          }
        //}
       //gObjectTable->Print();
}

//------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer::HitFinder(){ 

  Double_t x[numLayers/2],y[numLayers/2];
  UInt_t numPlaneHits[numLayers];
  //const UInt_t ijk=0;
  UInt_t ijk=0,trcand=0;
  Int_t hitID=0;
  Double_t hittmp[2],hitx2[2],hity1[2],hitq3[2],hitq4[2];
  Double_t y1,x2,q3,q4; 
  TVector3 pos, dpos;	
  //BmnDchHit *dchCombHit;
  UShort_t nhits[numLayers];
  Int_t jjgr2=0;
  FairMCPoint     *dchPoint;
  ScalarD* hitX[numLayers];ScalarI* trackId[numLayers];ScalarI* hitId[numLayers];ScalarI* detId[numLayers];ScalarUI* hitwire[numLayers];ScalarD* driftlen[numLayers];ScalarI* pointind[numLayers];
  
          numPlaneHits[ijk]=dchhitplane[ijk]->GetDchPlaneHitsNumber(ijk); //hits in plane k
          //cout<<" numPlaneHits0 = "<<numPlaneHits0<<", numLayer = "<<k<<", drift chamber = "<<fDchNum<<endl;
          for (UInt_t i = 0; i < numPlaneHits[ijk]; i++) {
              ReturnPointers(ijk,i,hitX[ijk],trackId[ijk],detId[ijk],hitwire[ijk],driftlen[ijk],pointind[ijk]);
              UInt_t hw0=hitwire[ijk]->GetSV();
              Double_t driftdist0=driftlen[ijk]->GetSV();
              LRambiguity(ijk,hittmp,hitX[ijk]->GetSV(),driftdist0,hw0,nhits[ijk]);
              for (UInt_t ii = 0; ii < nhits[ijk]; ii++) {
                hity1[ii]=hittmp[ii];
                //cout<<"hittmp0: "<<hittmp[ii]<<" "<<ii<<endl;
              }
              numPlaneHits[ijk+2]=dchhitplane[ijk+2]->GetDchPlaneHitsNumber(ijk+2); //hits in plane 2
              for (UInt_t j = 0; j < numPlaneHits[ijk+2]; j++) {
               ReturnPointers(ijk+2,j,hitX[ijk+2],trackId[ijk+2],detId[ijk+2],hitwire[ijk+2],driftlen[ijk+2],pointind[ijk+2]);
               UInt_t hw2=hitwire[ijk+2]->GetSV();
               Double_t driftdist2=driftlen[ijk+2]->GetSV();
               LRambiguity(ijk+2,hittmp,hitX[ijk+2]->GetSV(),driftdist2,hw2,nhits[ijk+2]);
               for (UInt_t ii = 0; ii < nhits[ijk+2]; ii++) {
                 hitx2[ii]=hittmp[ii];
                 //cout<<"hittmp1: "<<hittmp[ii]<<" "<<ii<<endl;
               }
               numPlaneHits[ijk+4]=dchhitplane[ijk+4]->GetDchPlaneHitsNumber(ijk+4); //hits in plane 4 
               for (UInt_t k = 0; k < numPlaneHits[ijk+4]; k++) {
                ReturnPointers(ijk+4,k,hitX[ijk+4],trackId[ijk+4],detId[ijk+4],hitwire[ijk+4],driftlen[ijk+4],pointind[ijk+4]);
                UInt_t hw4=hitwire[ijk+4]->GetSV();
                Double_t driftdist4=driftlen[ijk+4]->GetSV();
                LRambiguity(ijk+4,hittmp,hitX[ijk+4]->GetSV(),driftdist4,hw4,nhits[ijk+4]);
                for (UInt_t ii = 0; ii < nhits[ijk+4]; ii++) {
                  hitq3[ii]=hittmp[ii]*sqrt2;
                  //cout<<"hittmp2: "<<hittmp[ii]<<" "<<ii<<endl;
                }
                numPlaneHits[ijk+6]=dchhitplane[ijk+6]->GetDchPlaneHitsNumber(ijk+6); //hits in plane 6 
                 for (UInt_t l = 0; l < numPlaneHits[ijk+6]; l++) {
                  ReturnPointers(ijk+6,l,hitX[ijk+6],trackId[ijk+6],detId[ijk+6],hitwire[ijk+6],driftlen[ijk+6],pointind[ijk+6]);
                  UInt_t hw6=hitwire[ijk+6]->GetSV();
                  Double_t driftdist6=driftlen[ijk+6]->GetSV();
                  LRambiguity(ijk+6,hittmp,hitX[ijk+6]->GetSV(),driftdist6,hw6,nhits[ijk+6]);
                  for (UInt_t ii = 0; ii < nhits[ijk+6]; ii++) {
                    hitq4[ii]=hittmp[ii]*sqrt2;
                    //cout<<"hittmp3: "<<hittmp[ii]<<" "<<ii<<endl;
                  }
                  for (UInt_t m = 0; m < nhits[ijk]; m++) {
                   y1=hity1[m];
                   for (UInt_t n = 0; n < nhits[ijk+2]; n++) {
                    x2=hitx2[n];
                    for (UInt_t mm = 0; mm < nhits[ijk+4]; mm++) {
                     q3=hitq3[mm];
                     for (UInt_t nn = 0; nn < nhits[ijk+6]; nn++) {
                      q4=hitq4[nn];
                      CoordinateFinder(y1,x2,tg3,q3,tg4,q4,x,y); 
                      //cout<<"------------------------------------------------------"<<endl; 
                       Bool_t radcond=true;
                       if(sqrt(pow(x[1]-x[0],2.)+pow(y[1]-y[0],2.))>radialRange)radcond=false;
                       //cout<<"radcond = "<<radcond<<" "<<1<<" "<<sqrt(pow(x[1]-x[0],2.)+pow(y[1]-y[0],2.))<<endl;
                       if(radcond){
                        for (UInt_t jk = 0; jk < numLayers/2; jk++) {
                         if(sqrt(pow(x[jk],2.)+pow(y[jk],2.))>MinRadiusOfActiveVolume&&sqrt(pow(x[jk],2.)+pow(y[jk],2.))<MaxRadiusOfActiveVolume){
	                  if(fDoTest){if(k==4)hXYhit->Fill(x[3],y[3]);}
                          Double_t dX = pRandom->Gaus(0., hitErr[0]);	
                          Double_t dY = pRandom->Gaus(0., hitErr[1]);	
	                  pos.SetXYZ(x[jk]+detXshift[fDchNum-1]+dX, y[jk]+dY,zLayer[2*jk]);
		          //dpos.SetXYZ(dX,dY,hitErr[2]); 
		          dpos.SetXYZ(hitErr[0],hitErr[1],hitErr[2]); 
                          if(fDoTest||checkDch)dchPoint = (FairMCPoint*) fBmnDchPointsArray->At(pointind[2*jk]->GetSV());
                          if(fDoTest){ 
                           hResolX->Fill(x[jk]+detXshift[fDchNum-1]-dchPoint->GetX());
                           hResolY->Fill(y[jk]-dchPoint->GetY());
                           hResolR->Fill(sqrt(pow(x[jk]+detXshift[fDchNum-1],2.)+pow(y[jk],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.)));
                          }
                          if(checkDch){
                           cout<<"hits(x,y,z): "<<x[jk]<<", "<<y[jk]<<", "<<zLayer[2*jk]<<", hit number = "<<jjgr2<<endl;
                           hXYZcombhits->SetPoint(jjgr2++,x[jk],y[jk],zLayer[2*jk]);
                           //if(fabs(x[jk]+detXshift[fDchNum-1]-dchPoint->GetX())<0.015&&fabs(y[jk]-dchPoint->GetY())<0.015) {
                           cout<<" event = "<<eventNum<<", track cand. = "<<trackId[2*jk]->GetSV()<<", chamber = "<<fDchNum<<", difference (x,y,r): "<<x[jk]+detXshift[fDchNum-1]-dchPoint->GetX()<<" "<<y[jk]-dchPoint->GetY()<<" "<<sqrt(pow(x[jk]+detXshift[fDchNum-1],2.)+pow(y[jk],2.))-sqrt(pow(dchPoint->GetX(),2.)+pow(dchPoint->GetY(),2.))<<endl;
                           if(jk==3){
                            cout<<"nhits0 = "<<nhits[0]<<", nhits2 = "<<nhits[2]<<", nhits4 = "<<nhits[4]<<", nhits6 = "<<nhits[6]<<endl; 
                            cout<<"------------------------------------------------------"<<endl; 
                           }
                           //}
                          }
	                  dchCombHit = AddHit(hitID++, detId[2*jk]->GetSV(), pos, dpos, trackId[2*jk]->GetSV(), pointind[2*jk]->GetSV(), 0, jk*2);
                          /*if (fDchNum == 1) {
                           dchCombHit->SetDetId(kDCH1);
                           dchCombHit->SetStation(13);
                          } else if (fDchNum == 2) { 
                           dchCombHit->SetDetId(kDCH2);
                           dchCombHit->SetStation(14);
                          }*/
                         }
                        }// layers
                        if(checkDch)++trcand;
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


       if(checkDch)cout<<"Number of DCH hit candidates = "<<hitID<<endl;
       if(checkDch)cout<<"Number of DCH track candidates = "<<trcand<<endl;
}
//---------------------------------------------------------------------------
Double_t BmnDchHitProducer::PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz){

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
void		BmnDchHitProducer::Finish()
{
	if(fDoTest)
    	{				
             TString str;
             str.Form("%d",fDchNum);
             fOutputTestFileName = TString("test") + str + TString(".BmnDchHitProducer.root");
	     TFile file(fOutputTestFileName, "RECREATE");
             TH1D *h1 = (TH1D*) htTimeA->Clone("htTimeEff"); h1->Divide(htTime); h1->SetYTitle("Efficiency"); h1->Write();		
     	     h1 = (TH1D*) htGasDriftA->Clone("htGasDriftEff"); h1->Divide(htGasDrift); h1->SetYTitle("Efficiency"); h1->Write();
	     h1 = (TH1D*) htPerpA->Clone("htPerpEff"); h1->Divide(htPerp); h1->SetYTitle("Efficiency"); h1->Write();		
      	     fList.Write(); 	
      	     file.Close();
    	}
}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHit* BmnDchHitProducer::AddHit0(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer)
{
	BmnDchHit *pHit	=  new ((*pHitCollection0)[index]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	pHit->AddLink(FairLink(1, pointIndex)); 
	pHit->AddLink(FairLink(2, trackIndex)); 
	
return pHit;
}
//------------------------------------------------------------------------------------------------------------------------
BmnDchHit* BmnDchHitProducer::AddHit(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer)
{
	//BmnDchHit *pHit	=  new ((*pHitCollection)[index]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	TClonesArray& clref = *pHitCollection;
        Int_t size = clref.GetEntriesFast();
        BmnDchHit *pHit = new(clref[size]) BmnDchHit(detID, posHit, posHitErr, pointIndex, flag, trackIndex, dchlayer);
	pHit->AddLink(FairLink(1, pointIndex)); 
	pHit->AddLink(FairLink(2, trackIndex)); 
	
return pHit;
}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer::RadialRange(FairMCPoint* dchPoint){

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
void BmnDchHitProducer::LRambiguity(UInt_t k, Double_t hittmp[2], Double_t hitx, Double_t driftdist, UInt_t hw, UShort_t &nhits){

              if(hw!=0&&hw!=numWiresPerLayer){
               if(wireUsed[k+1][hw]&&wireUsed[k+1][hw-1]){
                 nhits=2; 
                 hittmp[0]=hitx-driftdist;
                 hittmp[1]=hitx+driftdist;
               }else if(!wireUsed[k+1][hw]&&!wireUsed[k+1][hw-1]){
                 nhits=2; 
                 hittmp[0]=hitx-driftdist;
                 hittmp[1]=hitx+driftdist;
               }else if(!wireUsed[k+1][hw]&&wireUsed[k+1][hw-1]){
                 hittmp[0]=hitx-driftdist;
                 nhits=1; 
               }else if(wireUsed[k+1][hw]&&!wireUsed[k+1][hw-1]){
                 hittmp[0]=hitx+driftdist;
                 nhits=1; 
               }
              }else if(hw==0){
               nhits=1; 
               hittmp[0]=hitx+driftdist;
              }else if(hw==numWiresPerLayer){ 
               nhits=1; 
               hittmp[0]=hitx-driftdist;
              }
}
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer::CoordinateFinder(Double_t y1,Double_t x2,Double_t k3,Double_t q3,Double_t k4,Double_t q4,Double_t x[],Double_t y[]){

   Double_t x1,x3,x4,y2,y3,y4;

   x[1]=x2;y[0]=y1;

             x[0]=(x[1]*(k4-k3)+(q4-y[0])/z4121-(q3-y[0])/z3121)/(k4-k3+k3/z3121-k4/z4121);
             x[2]=(x[1]-x[0])*z3121+x[0];
             x[3]=(x[1]-x[0])*z4121+x[0];
             y[1]=k3*(x[1]-x[0])+(k3*x[0]+q3-y[0])/z3121+y[0];
             y[2]=k3*x[2]+q3;
             y[3]=k4*x[3]+q4;
             
/*                      cout<<"2------------------------------------------------------"<<endl; 
                      for (UInt_t jk = 0; jk < numLayers/2; jk++) {
                       cout<<"points: "<<x[jk]<<" "<<y[jk]<<endl;
                      }
                      cout<<"2------------------------------------------------------"<<endl; 
*/
} 
//-------------------------------------------------------------------------------------------------------------------------
void BmnDchHitProducer::ReturnPointers(UInt_t ijk,UInt_t i,ScalarD* &hitX,ScalarI* &trackId,ScalarI* &detId,ScalarUI* &hitwire,ScalarD* &driftlen,ScalarI* &pointind){
              hitX = (ScalarD*) dchhitplane[ijk]->fDchHitPlane1->UncheckedAt(i); 
              trackId = (ScalarI*) dchhitplane[ijk]->fDchHitPlane2->UncheckedAt(i); 
              detId = (ScalarI*) dchhitplane[ijk]->fDchHitPlane4->UncheckedAt(i); 
              hitwire = (ScalarUI*) dchhitplane[ijk]->fDchHitPlane5->UncheckedAt(i); 
              driftlen = (ScalarD*) dchhitplane[ijk]->fDchHitPlane6->UncheckedAt(i); 
              pointind = (ScalarI*) dchhitplane[ijk]->fDchHitPlane7->UncheckedAt(i); 
}
//-------------------------------------------------------------------------------------------------------------------------
ClassImp(BmnDchHitProducer)
