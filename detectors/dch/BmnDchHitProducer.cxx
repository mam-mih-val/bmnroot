#include "BmnDchHitProducer.h"
#include "CbmMCTrack.h"
#include "BmnDchPoint.h"
#include "BmnDchHit.h"

#include "TRandom.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

using std::cout;

TString fhTestFlnm2;

BmnDchHitProducer::BmnDchHitProducer() :
  FairTask("BmnDchHitProducer"),
  fBmnPointsArray(nullptr),
  fMCTracksArray(nullptr),
  fBmnHitsArray(nullptr),
  fPlaneTypes(nullptr),
  hMCtr_ion_dc1(nullptr),
  hMCtr_ion_dc2(nullptr),
  hMCtr_ionID_dc1(nullptr),
  hMCtr_ionID_dc2(nullptr)
{
  fInputBranchName = "DCHPoint";
  fOutputHitsBranchName = "BmnDchHit";
  fhTestFlnm2 = "test.BmnDCH_MC.root";
  // fhList2.Add(hSmearing);

  hMCtr_ion_dc1 = new TH1F("hMCHits_ion_dc1", "ion MC hits in DCH1 ", 9, 0, 9);
  hMCtr_ion_dc2 = new TH1F("hMCHits_ion_dc2", "ion MC hits in DCH2 ", 9, 0, 9);
  hMCtr_ionID_dc1 = new TH1F("hMCHits_ionID_dc1", "ion MC hits ID in DCH1 ", 10000, 0, 10000);
  hMCtr_ionID_dc2 = new TH1F("hMCHits_ionID_dc2", "ion MC hits ID in DCH2 ", 10000, 0, 10000);
  fhList2.Add(hMCtr_ion_dc1);
  fhList2.Add(hMCtr_ion_dc2);
  fhList2.Add(hMCtr_ionID_dc1);
  fhList2.Add(hMCtr_ionID_dc2);
  
  fPlaneTypes = new TString[fNActivePlanes]{"vb", "va", "ub", "ua" ,"yb", "ya", "xb", "xa"};
}

BmnDchHitProducer::~BmnDchHitProducer() {
  if(fPlaneTypes) {
    delete [] fPlaneTypes;
    fPlaneTypes = nullptr;
  }
}

InitStatus BmnDchHitProducer::Init() {

  cout << " BmnDchHitProducer::Init() " << endl;
  rand_gen.SetSeed(5);//needed for repeated simulation

  //Get ROOT Manager
  FairRootManager* ioman = FairRootManager::Instance();

  fBmnPointsArray = (TClonesArray*)ioman->GetObject(fInputBranchName);
  
  if (!fBmnPointsArray) {
        cout << "BmnDchHitProducer::Init(): branch " << fInputBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
  }
  
  fMCTracksArray = (TClonesArray*)ioman->GetObject("MCTrack");

  fBmnHitsArray = new TClonesArray(fOutputHitsBranchName, 100);
  ioman->Register(fOutputHitsBranchName, "DCH", fBmnHitsArray, kTRUE);

  return kSUCCESS;
}

void BmnDchHitProducer::Exec(Option_t* opt) {
  
  if (!IsActive())
    return;

  fBmnHitsArray->Delete();

  if (!fBmnPointsArray) {
    Error("BmnDchHitProducer::Exec()", " !!! Unknown branch name !!! ");
    return;
  }
  
  if (fVerbose) cout << "======================== DCH hit finder exec started ====================" << endl;
    
  for (Int_t iPoint = 0; iPoint < fBmnPointsArray->GetEntriesFast(); iPoint++) {

    BmnDchPoint* dchPoint = (BmnDchPoint*) fBmnPointsArray->UncheckedAt(iPoint);
        
    Int_t track_id = dchPoint->GetTrackID();
    Double_t x = dchPoint->GetX();
    Double_t y = dchPoint->GetY();
    Double_t z = dchPoint->GetZ();
    Double_t px = dchPoint->GetPx();
    Double_t py = dchPoint->GetPy();
    Double_t pz = dchPoint->GetPz();

    Int_t currentPlaneNum = dchPoint->GetPlaneNumber();
    Int_t dchId = (z < 600.)? 1 : 2;//find out which dch the hit is from
    Double_t pdg = dchPoint->GetPdgId();
    Double_t charge = px*px +py*py +pz*pz;
	
    if (dchPoint->GetCharge() == 0)continue;//skip neutral particles
    if(fabs(px*px +py*py +pz*pz)<.25)continue;//skip low momentum particles

    if(dchId == 1){
      hMCtr_ionID_dc1->Fill(pdg);
    }
    if(dchId == 2){
      hMCtr_ionID_dc2->Fill(pdg);
    }
    //X,Y GLOBAL shifts need to be set from a file for geomety creation (X,y pos.) and center
    Double_t x_shift = 0.0;
    Double_t y_shift = 0.0;

    //Convert GLOBAL->LOCAL
    Double_t x_loc = -(x - x_shift); // x-plane wires (inverted)
    Double_t y_loc = y - y_shift;    // y-plane wires
    Double_t v_loc = (x_loc + y_loc)/TMath::Sqrt2(); // v-plane wires
    Double_t u_loc = (y_loc - x_loc)/TMath::Sqrt2(); // u-plane wires

    //current position in wire units
    Double_t wire_pos = DBL_MAX; //default
        
    //distance from the center of DCH to the outermost wire
    Double_t owdist = (fPlaneTypes[currentPlaneNum][1] == 'a') ? 119.0 : 118.5; //119.0 and 118.0 are taken from N.Voytishin's code

    if(fPlaneTypes[currentPlaneNum][0] == 'x') wire_pos = x_loc + owdist;
    if(fPlaneTypes[currentPlaneNum][0] == 'y') wire_pos = y_loc + owdist;
    if(fPlaneTypes[currentPlaneNum][0] == 'v') wire_pos = v_loc + owdist;
    if(fPlaneTypes[currentPlaneNum][0] == 'u') wire_pos = u_loc + owdist;

    //Int_t nearest_wire = (wire_pos - Int_t(wire_pos) < 0.5) ? Int_t(wire_pos) : Int_t(wire_pos) + 1;
    Int_t nearest_wire;
    Bool_t left = false; //pos. relative to the nearest wire (left or right)
    if(wire_pos - Int_t(wire_pos) < 0.5) {
      nearest_wire = Int_t(wire_pos);
      left = false;
    }
    else {
      nearest_wire = Int_t(wire_pos) + 1;
      left = true;
    }

    //distance from the point to the nearest wire
    Double_t wdist = TMath::Abs(wire_pos - nearest_wire);

    //sigma error dependent on the distance to the nearest wire
    Double_t sigm_err = (wdist < 0.02) ? 0.08 : (wdist >= 0.02 && wdist < 0.1) ? 0.06 : (wdist >= 0.1 && wdist < 0.4) ? 0.015 : (wdist >= 0.4 && wdist < 0.41) ? 0.08 : 0.10;

    Double_t dev = rand_gen.Gaus(0, sigm_err);
    Double_t wdist_smeared = wdist+dev ;
	
    if(wdist_smeared > .5 ){//return the point if outside of the possible interval
      // continue;
      wdist_smeared = 1. - wdist_smeared;
      dev = -dev;
    }

    if(wdist_smeared < 0){//return the point if outside of the possible interval
      //  continue;
      dev = -dev;
      wdist_smeared = - wdist_smeared;
    } 
     
    //not needed for further reco for Nikolay's version, can be removed  
    //hit-1 with smearing
    Double_t x1_smeared;// = -x_loc + x_shift + ;
    Double_t y1_smeared;// = y_loc + y_shift ;
    Double_t z1_smeared = z;// + dz1;

    //hit-2 (symmetrical) with smearing
    Double_t x2_smeared;
    Double_t y2_smeared;
    Double_t z2_smeared = z;// + dz2;


    if(fPlaneTypes[currentPlaneNum][0] == 'x') {
      if(left) {
	x1_smeared = -(x_loc - dev) + x_shift;
	y1_smeared = y_loc + y_shift; 
	x2_smeared = (x1_smeared - 2*wdist_smeared) + x_shift;
	y2_smeared = y1_smeared;// + dy2;
      }
      else {
	x1_smeared = -(x_loc + dev) + x_shift;
	y1_smeared = y_loc + y_shift; 
	x2_smeared = (x1_smeared + 2*wdist_smeared) + x_shift;
	y2_smeared = y1_smeared;
      }
    }

    if(fPlaneTypes[currentPlaneNum][0] == 'y') {
      if(left) {
	x1_smeared = -x_loc + x_shift;
	y1_smeared = y_loc + y_shift - dev;
	x2_smeared = x1_smeared;
	y2_smeared = y1_smeared + 2*wdist_smeared + y_shift;
      }
      else {
	x1_smeared = -x_loc + x_shift;
	y1_smeared = y_loc + y_shift + dev;
	x2_smeared = x1_smeared;
	y2_smeared = y1_smeared - 2*wdist_smeared + y_shift;
      }
    }

    if(fPlaneTypes[currentPlaneNum][0] == 'v') {
      if(left) {
	x1_smeared = -(((v_loc - dev) - u_loc)/TMath::Sqrt2()) + x_shift;
	y1_smeared = (((v_loc - dev) + u_loc)/TMath::Sqrt2()) + y_shift;
	x2_smeared = -(((v_loc + 2*wdist_smeared) - u_loc)/TMath::Sqrt2()) + x_shift;
	y2_smeared = (((v_loc + 2*wdist_smeared) + u_loc)/TMath::Sqrt2()) + y_shift;
      }
      else {
	x1_smeared = -(((v_loc + dev) - u_loc)/TMath::Sqrt2()) + x_shift;
	y1_smeared = (((v_loc + dev) + u_loc)/TMath::Sqrt2()) + y_shift;
	x2_smeared = -(((v_loc - 2*wdist_smeared) - u_loc)/TMath::Sqrt2()) + x_shift;
	y2_smeared = (((v_loc - 2*wdist_smeared) + u_loc)/TMath::Sqrt2()) + y_shift;
      }
    }

    if(fPlaneTypes[currentPlaneNum][0] == 'u') {
      if(left) {
	x1_smeared = -((v_loc - (u_loc - dev))/TMath::Sqrt2()) + x_shift;
	y1_smeared = ((v_loc + (u_loc - dev))/TMath::Sqrt2()) + y_shift;
	x2_smeared = -((v_loc  - (u_loc + 2*wdist_smeared))/TMath::Sqrt2()) + x_shift;
	y2_smeared = ((v_loc + (u_loc + 2*wdist_smeared))/TMath::Sqrt2()) + y_shift;
      }
      else {
	x1_smeared = -((v_loc - (u_loc + dev))/TMath::Sqrt2()) + x_shift;
	y1_smeared = ((v_loc + (u_loc + dev))/TMath::Sqrt2()) + y_shift;
	x2_smeared = -((v_loc  - (u_loc - 2*wdist_smeared))/TMath::Sqrt2()) + x_shift;
	y2_smeared = ((v_loc + (u_loc - 2*wdist_smeared))/TMath::Sqrt2()) + y_shift;
      }
    }
	
        
    //Output information for debugging
    Bool_t info_out =false;
    if(info_out) {
      cout << "(x:y:z) = ( " << x << " : " << y << " : " << z << " )\n";
      cout << " pdg code "<<dchPoint->GetPdgId()<< " \n";
      cout << " charge "<<dchPoint->GetCharge()<< " \n";

      cout << "  currentPlaneNum = " << currentPlaneNum << "\n";
      cout << "  PlaneType = " << fPlaneTypes[currentPlaneNum][0] << "\n";
      cout << " dev "<<dev<<" wdist_smeared = " <<wdist_smeared<< "\n";
      cout << "  smeared1(x:y:z) = ( " << x1_smeared << " : " << y1_smeared << " : " << z1_smeared << " )\n";
      cout << "  smeared2(x:y:z) = ( " << x2_smeared << " : " << y2_smeared << " : " << z2_smeared << " )\n";
      cout << "  wdist = " << wdist << "\n";
      cout << "  wire_pos = " << wire_pos << "\n";
      cout << "  nearest_wire = " << nearest_wire << "\n";
      cout << "  sigm_err = " << sigm_err << "\n";
      cout << "  left = " << left << "\n";
      cout << "  dchId = " << dchId << "\n";

      cout << "\n";
    }
        
    //adding a new hit with probability 90%
    if(rand_gen.Uniform() <= 1.) {
      //adding a hit with global x,y,z from sim and wire + smeared distance from wire for further reconstruction
      BmnDchHit* hit = new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])
	BmnDchHit(0, TVector3(x, y, z), TVector3(sigm_err, sigm_err, 0.0), iPoint);

      currentPlaneNum += (dchId == 1) ? 0 : 8;
            
      hit->SetDchLayerNumber(currentPlaneNum);
      hit->SetDchId(dchId-1);
      hit->SetWirePosition(nearest_wire);
      hit->SetDrift(wdist_smeared);
      hit->SetPhi(charge);
      hit->SetHitId(track_id);
    }
  }
  
  if (fVerbose) cout << "======================== DCH hit finder exec finished ===================" << endl;
}

void BmnDchHitProducer::Finish() {
  
  if (fVerbose){
    TFile file(fhTestFlnm2.Data(), "RECREATE");
    fhList2.Write();
    file.Close();

  }
}

ClassImp(BmnDchHitProducer)
