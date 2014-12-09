// --------------------------------------------------------------------------
// -----                 Class CbmTofHitProducer                       ------
// -----           Created  by E. Cordier 14/09/05                     ------
// -----           Modified by D. Gonzalez-Diaz 07/09/06               ------
// -----           Modified by D. Gonzalez-Diaz 02/02/07               ------
// --------------------------------------------------------------------------

#include "CbmTofHitProducer.h"

#include "CbmTofPoint.h"
#include "CbmTofHit.h"
#include "CbmMCTrack.h"
#include "CbmTofGeoHandler.h"
#include "CbmTofCell.h"
#include "CbmTofDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParam.h"

#include "TRandom.h"
#include "TString.h"
#include "TVector3.h"
#include "TSystem.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;
int entrys = 0;

// ---- Default constructor -------------------------------------------

CbmTofHitProducer::CbmTofHitProducer() :
      FairTask("CbmTofHitProducer"),
      fVerbose(1),
      fTofPoints(NULL),
      fMCTracks(NULL),
      fTofHits(NULL),
      X(),
      Dx(),
      Y(),
      Dy(),
      Z(),
      Ch(),
      type(),
      tl(),
      tr(),
      trackID_left(),
      trackID_right(),
      point_left(),
      point_right(),
      fSigmaT(0.),
      fSigmaXY(0.),
      fSigmaX(0.),
      fSigmaZ(0.),
      fVersion(""),
      fNHits(-1),
      fZposition(800.),
      fXshift(90.),
      fGeoHandler(new CbmTofGeoHandler()),
      fDigiPar(NULL),
      fCellInfo(NULL),
      fParInitFromAscii(kTRUE)
{
}

// ---- Constructor ----------------------------------------------------

CbmTofHitProducer::CbmTofHitProducer(const char *name, Int_t verbose) :
      FairTask(name, verbose),
      fVerbose(verbose),
      fTofPoints(NULL),
      fMCTracks(NULL),
      fTofHits(NULL),
      X(),
      Dx(),
      Y(),
      Dy(),
      Z(),
      Ch(),
      type(),
      tl(),
      tr(),
      trackID_left(),
      trackID_right(),
      point_left(),
      point_right(),
      fSigmaT(0.),
      fSigmaXY(0.),
      fSigmaX(0.),
      fSigmaZ(0.),
      fVersion(""),
      fNHits(-1),
      fZposition(800.),
      fXshift(90.),
      fGeoHandler(new CbmTofGeoHandler()),
      fDigiPar(NULL),
      fCellInfo(NULL),
      fParInitFromAscii(kTRUE)
//      fParInitFromAscii(0) //hihi
{
      SetHistogram();
}

// ---- Destructor ----------------------------------------------------

CbmTofHitProducer::~CbmTofHitProducer()
{
  if (fGeoHandler) {
    
    delete fGeoHandler;
  } 
//  f->Write();
//  f->Close(); 
}
//-------------------------------------------------------
void CbmTofHitProducer::SetHistogram()
{
  f = new TFile("TimeRec.root","RECREATE","Test");
  fhist1[0] = new TH2F("hist11","Velocity_vs_Momentum_Proton",100,0.,10.,1000,0.,50.);
  fhist1[1] = new TH2F("hist12","Velocity_vs_Momentum_Pion",100,0.,10.,1000,0.,50.);
  fhist1[2] = new TH2F("hist13","Velocity_vs_Momentum_Electron",100,0.,10,1000,0.,50.);
  fhist1[3] = new TH2F("hist14","Velocity_vs_Momentum_Muon",100,0.,10,1000,0.,50.);
  fhist1[4] = new TH2F("hist15","Velocity_vs_Momentum_Kaon",100,0.,10.,1000,0.,50.);
  fhist1[5] = new TH2F("hist16","Velocity_vs_Momentum_All_particle",100,0.,10.,1000,0.,50.);
  fhist1[6] = new TH2F("hist17","Tof bm@n",100,0.,10.,1000,-0.1,1.9);
  fhist1[7] = new TH2F("hist18","m**2_vs_Momentum_Proton",100,0.,10.,1000,-0.1,1.9);
  fhist1[8] = new TH2F("hist19","m**2_vs_Momentum_Pion",100,0.,10.,1000,-0.1,1.9);
  fhist1[9] = new TH2F("hist20","m**2_vs_Momentum_Eletron",100,0.,10.,1000,-0.1,1.9);
  fhist1[10] = new TH2F("hist21","m**2_vs_Momentum_Muon",100,0.,10.,1000,-0.1,1.9);
  fhist1[11] = new TH2F("hist22","m**2_vs_Momentum_Kaon",100,0.,10.,1000,-0.1,1.9);
//  fhist2 = new TH1F("hist2","PdgCode",4000,-1000.,3000.);
  fhist2 = new TH1F("hist2","X hit",100,-100.,300.);
  fhist3 = new TH1F("hist3","Time",1000,0.,1000.);
  fhist4 = new TH2F("hist4","Hits_in _event/MCTrack_in_event",1000,0.,1000.,150,0.,150.);
}

// --------------------------------------------------
void CbmTofHitProducer::SetParContainers()
{
  if (!fParInitFromAscii) {
    LOG(INFO)<<" Get the digi parameters for tof"<<FairLogger::endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTofDigiPar*)
               (rtdb->getContainer("CbmTofDigiPar"));

  }
}
// --------------------------------------------------------------------
InitStatus CbmTofHitProducer::ReInit()
{
  if (!fParInitFromAscii) {
    LOG(INFO)<<"Reinitialize the digi parameters for tof"<<FairLogger::endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTofDigiPar*)
               (rtdb->getContainer("CbmTofDigiPar"));

  }
}

// ---- Init ----------------------------------------------------------

InitStatus CbmTofHitProducer::Init() {
   Int_t nregions = 2, nmodules = 40, ncells = 40;
   for (int i = 0; i < nregions; i++) {
     for (int j = 0; j < nmodules; j++) {
        for (int k = 0; k < ncells; k++) {
           X[i][j][k] = -1;
           Y[i][j][k] = -1;
           Dx[i][j][k] = -1;
           Dy[i][j][k] = -1;
           Z[i][j][k] = -1;
	   Ch[1][j][k] = -1;
        }
     }
  }
   FairRootManager* ioman = FairRootManager::Instance();

   fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   
   // Initialize the TOF GeoHandler
   Bool_t isSimulation=kFALSE;
   Int_t bla = fGeoHandler->Init(isSimulation);

   if (fParInitFromAscii) {
     ReadTofZPosition();
     InitParametersFromAscii();
   } else {
     InitParametersFromContainer();
   }


   fTofHits = new TClonesArray("CbmTofHit");
   ioman->Register("BmnTof2Hit", "TOF2", fTofHits, kTRUE);

   LOG(INFO) << "-I- CbmTofHitProducer: Intialization successfull" << FairLogger::endl;

   return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------

void CbmTofHitProducer::Exec(Option_t * option) {
   int nID = -1;
   CbmTofPoint* mcPoint = 0;
   CbmMCTrack* mcTrack = 0;
   TVector3 pos;
   Double_t m = 0, v, Moment;
   // Some numbers on TOF distributions
   Int_t tof_tracks = 0, tof_tracks_vert = 0, tof_tracks_local = 0, trackvert = 0;
   // FIXME: these parameters must be provided externally
   Int_t nregions = 2, nmodules = 40, ncells = 40, ngaps = 10, flag, ref;
   Double_t tl_new, tr_new;
   Double_t Dz = 2.04; //FIXME: Introduce also Dz and Z as (constant) parameters
   Float_t sigma_T = 0.0, sigma_X = 0.7, sigma_t_gap, t_o, sigma_el=0.0, vprop = 20, Pgap = 0.75, T_smearing = 0,sigma_P = 0.0455;
   Float_t sigma_Trig = 0.0 ;
   Int_t iCh = 0;
   TVector3 hitPos;
   TVector3 hitPosErr;
   Int_t trackID = 0;
   Int_t detID = 0;
   Int_t cell = 0;
   Int_t module = 0;
   Int_t region = 0;

   fTofHits->Clear();
   fNHits = -1; // Must start in -1
   Int_t nofMCTracks = fMCTracks->GetEntries();
   for (Int_t p = 0; p < nofMCTracks; p++) {
      mcTrack = (CbmMCTrack*) fMCTracks->At(p);
      if (mcTrack->GetNPoints(kTOF) > 0) tof_tracks++;
//      if (mcTrack->GetNPoints(kTOF) > 0 && mcTrack->GetStartZ() > (fTofZPosition - 4.)) tof_tracks_local++;
      if (mcTrack->GetNPoints(kTOF) > 0 && mcTrack->GetMotherId() == -1) tof_tracks_vert++;   
   }
   cout << "-I- CbmTofHitProducer : Event " << entrys++ << endl;
   cout << "-I- CbmTofHitProducer : " << tof_tracks << " tracks in Tof\n";
   cout << "-I- CbmTofHitProducer : " << tof_tracks_vert << " tracks in Tof from vertex\n";
//   cout << "-I- CbmTofHitProducer : " << tof_tracks_local << " tracks in Tof produced locally (Z > " << fTofZPosition - 4 << " cm)\n";
//   cout << "-I- CbmTofHitProducer : " << tof_tracks - tof_tracks_local << " tracks in Tof able to produce a hit\n";
   cout << "-I- CbmTofHitProducer : " << fTofPoints->GetEntriesFast() << " MC points in this event\n";

   // Here check for the validity of the parameters
   if (fSigmaX > 1)
      cout << "IRREALISTIC TOF POSITION RESOLUTION!! (HitProducer may crash)" << endl;
   if ((fSigmaT < 0.07 && fSigmaT > 0) || fSigmaT > 0.2)
      cout << "IRREALISTIC TOF RESOLUTION!! (HitProducer may crash)" << endl;

   //Initialization of cell times
   for (Int_t i = 0; i < nregions; i++) {
      for (Int_t j = 0; j < nmodules; j++) {
         for (Int_t k = 0; k < ncells; k++) {
            tl[i][j][k] = 1e+16;
            tr[i][j][k] = 1e+16;
         }
      }
   }

   // Loop over the TOF points
   Int_t nofTofPoints = fTofPoints->GetEntries();
   for (Int_t j = 0; j < nofTofPoints; j++) {
//      Probability that the avalanche is detected
//      if (gRandom->Uniform(1) > Pgap) continue;  // hihi 

      mcPoint = (CbmTofPoint*) fTofPoints->At(j);
      if (mcPoint == NULL) {
         cout << "Be careful: hole in the CbmTofPoint TClonesArray!" << endl;
         continue;
      }
      if (mcPoint->GetTrackID() < 0) continue; //hihi

      mcTrack = (CbmMCTrack*) fMCTracks->At(mcPoint->GetTrackID());
        
      //Get relevant information from the point
      trackID = mcPoint->GetTrackID();
      detID = mcPoint->GetDetectorID();
      cell = fGeoHandler->GetCell(detID);
      module = fGeoHandler->GetCounter(detID)-1;
      region = fGeoHandler->GetRegion(detID)-1;
//      cout << "cell module region " <<cell<<" "<<module<<" "<<region<< endl;
      if (module < 0) continue; //hihi         
      // Reject particles produced in the last 4 cm. Better job must be done here. For example:
      // it could better to go up to the parent particle and get its trackID, then the
      // secondary is processed. FIXME.
      //if ((mcTrack->GetStartZ()) > 996) continue;
      //if ((mcTrack->GetStartZ()) > (Z[region][module][cell] - 4.)) continue; // hihi 
      LOG(DEBUG2)<<" trackID  " << trackID << " detID " << detID << " cell " << cell << " module " << module << " region " << region << FairLogger::endl;
      LOG(DEBUG2)<<"Det System: "<<fGeoHandler->GetDetSystemId(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"SMtype: "<<fGeoHandler->GetSMType(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"SModule: "<<fGeoHandler->GetSModule(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"Counter: "<<fGeoHandler->GetCounter(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"Gap: "<<fGeoHandler->GetGap(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"Cell: "<<fGeoHandler->GetCell(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"Region: "<<fGeoHandler->GetRegion(detID)<<FairLogger::endl;
      
      mcPoint->Position(pos);
      T_smearing      = gRandom->Gaus(0., sigma_T);
      
      Float_t X_local = pos.X()-X[region][module][cell];
      tl_new = mcPoint->GetTime() + T_smearing - X_local/vprop
           + gRandom->Gaus(0,sigma_el);
      tr_new = mcPoint->GetTime() + T_smearing + X_local/vprop
           + gRandom->Gaus(0,sigma_el);
      LOG(DEBUG2)<<" X position = "<< X[region][module][cell] <<" tl_new = "<< tl_new << " tr_new = "<< tr_new << FairLogger::endl;
      if (tl_new < tl[region][module][cell]) {
         tl[region][module][cell] = tl_new;
         trackID_left[region][module][cell] = trackID;
         point_left[region][module][cell] = j;
      }
      if (tr_new < tr[region][module][cell]) {
         tr[region][module][cell] = tr_new;
         trackID_right[region][module][cell] = trackID;
         point_right[region][module][cell] = j;
      }
   } // loop on TOF points

   // Parameters of the Hit. Different for pad or strip read-out.
   Double_t xHit = 0., yHit = 0., zHit = 0., tHit = 0.;
   Double_t xHitErr = 0., yHitErr = 0., zHitErr = 0.;
   Int_t nFl1=0;
   Int_t nFl2=0;
   Int_t ii=0;
   sigma_T = 0.08;
   sigma_el = 0.05;
   sigma_Trig = 0.04;
   T_smearing      = gRandom->Gaus(0., sigma_T) + gRandom->Gaus(0., sigma_Trig);
   for (Int_t i = 0; i < nregions; i++)
   {
    for (Int_t j = 0; j < nmodules; j++)
    {
     for (Int_t k = 0; k < ncells; k++)
     {
      if (!strcmp(type[i][j][k],"s"))
      {    
       if( tl[i][j][k]<1e+15 && tr[i][j][k]<1e+15)
       {
       fNHits++;
       tl[i][j][k] = tl[i][j][k] + T_smearing+ gRandom->Gaus(0,sigma_el);
       tr[i][j][k] = tr[i][j][k] + T_smearing+ gRandom->Gaus(0,sigma_el);
       yHit    = Y[i][j][k]; 
       yHitErr = Dy[i][j][k]/sqrt(12.);
       zHit    = Z[i][j][k];
       zHitErr = Dz/sqrt(12.);
       xHit    = (tr[i][j][k] - tl[i][j][k])*vprop*0.5
                  + X[i][j][k];
       
    //Reference to the point that contributes to the left side.
       xHitErr = sigma_T*vprop;
       tHit    = 0.5*(tl[i][j][k] + tr[i][j][k]);
//       ref     = trackID_left[i][j][k];
       ref     = point_left[i][j][k];
       if (ref < 0) continue;
       mcPoint = (CbmTofPoint*) fTofPoints->At(point_left[i][j][k]);
       mcTrack = (CbmMCTrack*) fMCTracks->At(trackID_left[i][j][k]);
       mcPoint->Position(pos);
       // Check consistency
       if(trackID_left[i][j][k]==trackID_right[i][j][k]){
	   flag = 1;
           nFl1++;
           if(fVerbose >2) {
           cout << " pos check for point "<<point_left[i][j][k]<<" x:  "<< xHit << " " << pos.X() 
                << " y: " << yHit << " " << pos.Y() << endl;   
	   }
       }
       else {
	   flag = 0;
           nFl2++;
       }
       iCh = Ch[i][j][k];
       if(fVerbose >1) {
	 cout << ii++ << " Add hit smt " << " sm " << i << " mod " << j << " str " << k
	 <<" Ch " << iCh
         <<" tl " << tl[i][j][k] << " tr " << tr[i][j][k] 
	 <<" xh " << xHit << " yh " << yHit << " fl "<< flag << " refPoi " << point_left[i][j][k]
	 <<" TID "<<  trackID_left[i][j][k] <<","<<trackID_right[i][j][k]<< endl;
       }   
       hitPos.SetXYZ(xHit, yHit, zHit);
       hitPosErr.SetXYZ(xHitErr, yHitErr, zHitErr);
       trackID = mcPoint->GetTrackID();
       detID = mcPoint->GetDetectorID();
       cell = fGeoHandler->GetCell(detID);
       module = fGeoHandler->GetCounter(detID)-1;
       region = fGeoHandler->GetRegion(detID)-1;      
//       if ( fNHits == -1 ) continue;
       if((mcPoint->GetLength() > (fZposition-20.)) && flag == 1 && mcTrack->GetNPoints(kTOF) > 0 && mcTrack->GetMotherId() == -1 )
       {
        trackvert++;
        Moment = mcTrack->GetP();
        Moment = Moment + Moment*(gRandom->Gaus(0., sigma_P));
        fhist1[5]->Fill(Moment,mcPoint->GetLength()/tHit);
        v = ((mcPoint->GetLength())/(tHit))/29.9792458;
        m =Moment*Moment/(v*v) - Moment*Moment ;
        if(m > 1.2){
	    LOG(DEBUG2)<< " Mass**2 "<<" X position = "<< X[i][j][k] <<" tl_new = "<<tl[i][j][k]  << " tr_new = "<<tr[i][j][k] << FairLogger::endl;
    	    LOG(DEBUG2)<< " Mass**2 "<<" Region = "<< i <<" module = "<< j  << " Cell = "<<k << FairLogger::endl;
    	    LOG(DEBUG2)<< " Mass**2 "<<" Moment = "<< mcTrack->GetP() << " tHit  = " << tHit << " Len = "<<  mcPoint->GetLength() << FairLogger::endl;
    	    LOG(DEBUG2)<< " Mass**2 "<<" V/c = " << v << " Pdg = " <<  mcTrack->GetPdgCode() << " V = " << mcPoint->GetLength()/tHit << FairLogger::endl;
    	    LOG(DEBUG2)<< " Mass**2 "<<" trackID = " << trackID << " Time = " << mcPoint->GetTime()<<endl;            
        }
        fhist1[6]->Fill(Moment,m);

        if(mcTrack->GetPdgCode()==2212) {
         fhist1[0]->Fill(Moment,mcPoint->GetLength()/tHit);
         fhist1[7]->Fill(Moment,m);
        }

        if(mcTrack->GetPdgCode()==211 || mcTrack->GetPdgCode()==-211) {
         fhist1[1]->Fill(Moment,mcPoint->GetLength()/tHit);
         fhist1[8]->Fill(Moment,m);
        }

        if(mcTrack->GetPdgCode()==11 || mcTrack->GetPdgCode()==-11) {
         fhist1[2]->Fill(Moment,mcPoint->GetLength()/tHit);
         fhist1[9]->Fill(Moment,m);
        } 

        if(mcTrack->GetPdgCode()==13 || mcTrack->GetPdgCode()==-13) {
         fhist1[3]->Fill(Moment,mcPoint->GetLength()/tHit);
         fhist1[10]->Fill(Moment,m);
        }

        if(mcTrack->GetPdgCode()==321 || mcTrack->GetPdgCode()==-321) {
         fhist1[4]->Fill(Moment,mcPoint->GetLength()/tHit);
         fhist1[11]->Fill(Moment,m);
        }

//        fhist2->Fill(mcTrack->GetPdgCode());
        fhist3->Fill(tHit); 
       } // tracks from primary vertex
       fhist2->Fill(hitPos.X());
       AddHit(mcPoint->GetDetectorID(), hitPos, hitPosErr, ref, tHit, flag, iCh);
       } // valid 'tr' and 'tl'
      } // point type equal "s"
     } // loop on cells
    } // loop on modules
   } // loop on regions
   if(tof_tracks_vert > 0) fhist4->Fill(entrys,((Float_t)(trackvert)/(Float_t)(tof_tracks_vert))*100.);  
   cout << "-I- CbmTofHitProducer : " << fNHits+1 << " hits in Tof created (" << nFl2 <<" multihits, " << trackvert << " 'useful' hits ) " << endl;
}

// ---- Add Hit to HitCollection --------------------------------------

void CbmTofHitProducer::AddHit(Int_t detID, TVector3& posHit,
      TVector3& posHitErr, Int_t ref, Double_t tHit, Int_t flag, Int_t iChannel) {
   new ((*fTofHits)[fNHits]) CbmTofHit(detID, posHit, posHitErr, ref, tHit, flag, iChannel);
   CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(fNHits);
   tofHit->SetStation(15);
   if (fVerbose > 1) {
      tofHit->Print();
      cout << endl;
   }
}

// ---- Finish --------------------------------------------------------

void CbmTofHitProducer::Finish() {
    f->Write();
    f->Close();
}

void CbmTofHitProducer::ReadTofZPosition()
{
   TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
   for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
      TGeoNode* tof = (TGeoNode*) nodes->At(iNode);
      if (TString(tof->GetName()).Contains("tof")) {
         const Double_t* tofPos = tof->GetMatrix()->GetTranslation();
         TGeoBBox* shape = (TGeoBBox*) tof->GetVolume()->GetShape();
         fTofZPosition = tofPos[2] - shape->GetDZ();
         LOG(INFO)<<tofPos[2]<<" , "<< shape->GetDZ()<<FairLogger::endl;
      }
   }
   LOG(INFO)<< "Set TOF z position from geometry manager= " << fTofZPosition << FairLogger::endl;
}

// ---- SetSigmaT -----------------------------------------------------

void CbmTofHitProducer::SetSigmaT(Double_t sigma) {
   fSigmaT = sigma;
}

// ---- SetSigmaXY -----------------------------------------------------

void CbmTofHitProducer::SetSigmaXY(Double_t sigma) {
   fSigmaXY = sigma;
}

// ---- SetSigmaX -----------------------------------------------------

void CbmTofHitProducer::SetSigmaX(Double_t sigma) {
   fSigmaX = sigma;
}

// ---- SetSigmaZ -----------------------------------------------------

void CbmTofHitProducer::SetSigmaZ(Double_t sigma) {
   fSigmaZ = sigma;
}

// ---- GetSigmaXY -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaT() {
   return fSigmaT;
}

// ---- GetSigmaXY -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaXY() {
   return fSigmaXY;
}

// ---- GetSigmaX -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaX() {
   return fSigmaX;
}

// ---- GetSigmaZ -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaZ() {
   return fSigmaZ;
}


void CbmTofHitProducer::InitParametersFromAscii()
{
  LOG(INFO)<<"Initializing the tof parameters from the old ascii file."<<FairLogger::endl;

  FILE *par;

  //Reading the parameter file. In the future this must be done in a different way.

  char header = '#', cell_type[2] = "#";
  int region, module, cell;
  Int_t nregions = 2, nmodules = 40, ncells = 40;
  Float_t X_tmp, Y_tmp, Dx_tmp, Dy_tmp, Z_tmp;
  Int_t iCh = 0;

  //    Initialize the matrixes [make this index visible in all the macro]. FIXME

  for (int i = 0; i < nregions; i++) {
     for (int j = 0; j < nmodules; j++) {
        for (int k = 0; k < ncells; k++) {
           X[i][j][k] = -1;
           Y[i][j][k] = -1;
           Dx[i][j][k] = -1;
           Dy[i][j][k] = -1;
           Z[i][j][k] = -1;
	   Ch[1][j][k] = -1;
        }
     }
  }

  TString tofGeoFile = gSystem->Getenv("VMCWORKDIR");
  tofGeoFile += "/parameters/tof_standard.geom.par";
  par = fopen(tofGeoFile, "r");

  if (par == NULL) {
     LOG(FATAL)<<"ERROR WHILE OPENING THE PARAMETER FILE IN TOF HIT PRODUCER!"<<FairLogger::endl;
  }

  //Skip the header. In the future the header structure must be defined. FIXME

  while (fscanf(par, "%c", &header) >= 0) {
     if ((int) (header - '0') == 0) break;
  }

  //Read the first line
  region = 0;
  fscanf(par, "%d%d%s%f%f%f%f%f", &module, &cell, &cell_type[0], &X_tmp, &Y_tmp, &Dx_tmp, &Dy_tmp, &Z_tmp);

//  Double_t x_tof_shift = 0.;   // fScaleFactor = 0.88
//  Double_t x_tof_shift = -45.; // fScaleFactor = 0.44
  X[region][module][cell] = X_tmp/10. + (fXshift - 90.);
  Y[region][module][cell] = Y_tmp/10.;
  Z[region][module][cell] = Z_tmp/10. + (fZposition - 800.);
  Dx[region][module][cell] = Dx_tmp/10.;
  Dy[region][module][cell] = Dy_tmp/10.;
  strcpy(&type[region][module][cell][0], cell_type);
  Ch[region][module][cell]    = iCh++;
//  cout << "Params 0 " << " region = " << region << " module = "<< module <<" cell = " << cell << " cell typ "<< cell_type << endl; //hihi
//  cout << "Params 0 " << " X ="<< X_tmp << " Y =" << Y_tmp <<" Dx =" << Dx_tmp <<" Dy =" << Dy_tmp <<" Z ="<<Z_tmp<<  endl; //hihi
  //Read all the lines

  while (fscanf(par, "%d%d%d%s%f%f%f%f%f", &region, &module, &cell, &cell_type[0],
        &X_tmp, &Y_tmp, &Dx_tmp, &Dy_tmp, &Z_tmp) >= 0) {
     X[region][module][cell] = X_tmp/10 +  (fXshift - 90.);
     Y[region][module][cell] = Y_tmp/10;
     Z[region][module][cell] = Z_tmp/10. + (fZposition - 800.);
     Dx[region][module][cell] = Dx_tmp/10;
     Dy[region][module][cell] = Dy_tmp/10;
     strcpy(&type[region][module][cell][0], cell_type);
     Ch[region][module][cell]    = iCh++;
//     cout << "Params 1 " << " region = " << region << " module = "<< module <<" cell = " << cell << " cell typ "<< cell_type << endl; //hihi     
//     cout << "Params 1 " << " X ="<< X_tmp << " Y =" << Y_tmp <<" Dx =" << Dx_tmp <<" Dy =" << Dy_tmp << " iCh = " << iCh <<" Z ="<<Z_tmp << endl; //hihi
  }      

  fclose(par);
}

void CbmTofHitProducer::InitParametersFromContainer()
{

   Int_t nregions = 2, nmodules = 40, ncells = 40;
   

   //    Initialize the matrixes [make this index visible in all the macro]. FIXME

   for (int i = 0; i < nregions; i++) {
      for (int j = 0; j < nmodules; j++) {
         for (int k = 0; k < ncells; k++) {
            X[i][j][k] = -1;
            Y[i][j][k] = -1;
            Z[i][j][k] = -1;
            Dx[i][j][k] = -1;
            Dy[i][j][k] = -1;
	    Ch[1][j][k] = -1;
         }
      }
   }

   Int_t nrOfCells = fDigiPar->GetNrOfModules();
   LOG(INFO)<<"Parameter container contain "<<nrOfCells<<" cells."<<FairLogger::endl;
//   cout << "InitParameters" << " Cell = " << nrOfCells << endl; //hihi
   for (Int_t icell = 0; icell < nrOfCells; ++icell) {
     Int_t cellId = fDigiPar->GetCellId(icell);
     fCellInfo =fDigiPar->GetCell(cellId);

     Int_t region = fGeoHandler->GetRegion(cellId)-1;
     Int_t module = fGeoHandler->GetCounter(cellId)-1;
     Int_t cell = fGeoHandler->GetCell(cellId);
     Double_t x = fCellInfo->GetX();
     Double_t y = fCellInfo->GetY();
     Double_t z = fCellInfo->GetZ();
     Double_t dx = fCellInfo->GetSizex();
     Double_t dy = fCellInfo->GetSizey();
//     cout << "InitParameters1" << " Region = " << region << " Module = " << module << " Cell = " << cell << endl; //hihi
     // The conversions have to be done to have the same dimensions as for
     // the old parameters taken from the file.
     // X, Y, DX and DY are in mm in the code but in cm in the param container
     // Z is in cm in the code and in cm in the param container.
     // The magic value 1.42 which is subtracted is to calculate the front position of
     // the detector. Don't know exactely why this is needed. The position of the hit should be
     // in the middle of all gaps. To be fixed.
     // The dx and dy are the full size of the cell in the code. In the file the size is
     // stored as the half size of the box in cm.
     //
     X[region][module][cell] = x;
     Y[region][module][cell] = y;
     Z[region][module][cell] = z;
     Dx[region][module][cell] = dx;
     Dy[region][module][cell] = dy;
     Ch[region][module][cell] = icell;
     if (TMath::Abs(dx-dy) < 0.001) {
       strcpy(type[region][module][cell], "p");
     } else {
       strcpy(type[region][module][cell], "s");
     }

      LOG(INFO)<<"X, Y, Z, Dx, Dy, type: "<<x<<" , "<<y<<" , "<<z<<" , "
         <<dx<<" , "<<dy<<" , "<<type<<" , "<<FairLogger::endl;
//      cout << " Params 2 " <<"Region, Module, Cell: "<<region<<" , "<<module<<" , "<<cell<<" , "<<endl;     
//      cout << " Params 2 " <<"X, Y, Z, Dx, Dy, type: "<<x<<" , "<<y<<" , "<<z<<" , "
//         <<dx<<" , "<<dy<<" , "<<type<<" , "<<endl;           
   }
}


ClassImp(CbmTofHitProducer)

