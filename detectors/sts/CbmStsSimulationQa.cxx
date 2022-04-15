// -------------------------------------------------------------------------
// -----                  CbmStsSimulationQa source file               -----
// -----                  Created 02/02/07  by R. Karabowicz           -----
// -------------------------------------------------------------------------

#include "CbmStsSimulationQa.h"

#include "CbmStsPoint.h"
// #include "CbmStsTrack.h"
//#include "CbmStsTrackMatch.h"
#include "CbmGeoStsPar.h"

#include "BmnDetectorList.h"
#include "FairGeoNode.h"
#include "FairGeoPassivePar.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TList.h"
#include "TVector3.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;
using std::flush;
using std::setprecision;


// -----   Default constructor   -------------------------------------------
CbmStsSimulationQa::CbmStsSimulationQa() 
:  fMCTracks(),
   fSTSPoints(),
   fStsGeo(),
   fPassGeo(),
   fTargetPos(),
   fNStations(0),
   fhMomAll(),
   fhYPtMapAll(),
   fhPdgCodeAll(),
   fhStsPointsAll(),
   fhMomRec(),
   fhYPtMapRec(),
   fhPdgCodeRec(),
   fhStsPointsRec(),
   fhMomStsPoints(),
   fhStsPointsPosition(),
   fhNofEvents(),
   fhNofStsStations(),
   fNEvents(0),
   fHistoList(),
   fOnlineAnalysis(kFALSE),
   fOnlineCanvas()  
{}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsSimulationQa::CbmStsSimulationQa(Bool_t visualizeBool, Int_t iVerbose)
  : FairTask("STS Simulation QA", iVerbose), 
    fMCTracks(),
    fSTSPoints(),
    fStsGeo(),
    fPassGeo(),
    fTargetPos(),
    fNStations(0),
    fhMomAll(),
    fhYPtMapAll(),
    fhPdgCodeAll(),
    fhStsPointsAll(),
    fhMomRec(),
    fhYPtMapRec(),
    fhPdgCodeRec(),
    fhStsPointsRec(),
    fhMomStsPoints(),
    fhStsPointsPosition(),
    fhNofEvents(),
    fhNofStsStations(),
    fNEvents(0),
    fHistoList(),
    fOnlineAnalysis(kFALSE),
    fOnlineCanvas()
{
  fOnlineAnalysis = visualizeBool;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsSimulationQa::~CbmStsSimulationQa() {

  fHistoList->Delete();
  delete fHistoList;
}
// -------------------------------------------------------------------------



// -----   Public method SetParContainers   --------------------------------
void CbmStsSimulationQa::SetParContainers() {

  // Get Run
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) {
    cout << "-E- " << GetName() << "::SetParContainers: No FairRunAna!"
	 << endl;
    return;
  }

  // Get Runtime Database
  FairRuntimeDb* runDb = run->GetRuntimeDb();
  if ( ! run ) {
    cout << "-E- " << GetName() << "::SetParContainers: No runtime database!"
	 << endl;
    return;
  }

  // Get passive geometry parameters
  fPassGeo = (FairGeoPassivePar*) runDb->getContainer("FairGeoPassivePar");
  if ( ! fPassGeo ) {
    cout << "-E- " << GetName() << "::SetParContainers: "
	 << "No passive geometry parameters!" << endl;
    return;
  }

  // Get STS geometry parameters
  fStsGeo = (CbmGeoStsPar*) runDb->getContainer("CbmGeoStsPar");
  if ( ! fStsGeo ) {
    cout << "-E- " << GetName() << "::SetParContainers: "
	 << "No Sts geometry parameters!" << endl;
    return;
  }

}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmStsSimulationQa::Init() {

  cout << "==========================================================="
       << endl;;
  cout << GetName() << ": Initialising..." << endl;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- " << GetName() << "::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get MCTrack array
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    cout << "-E- " << GetName() << "::Init: No MCTrack array!" << endl;
    return kERROR;
  }

  // Get StsPoints array
  fSTSPoints = (TClonesArray*) ioman->GetObject("StsPoint");
  if ( ! fSTSPoints ) {
    cout << "-E- " << GetName() << "::Init: No StsPoint array!" << endl;
    return kERROR;
  }

  // Get the geometry of target and Sts
  InitStatus geoStatus = GetGeometry();
  if ( geoStatus != kSUCCESS ) {
    cout << "-E- " << GetName() << "::Init: Error in reading geometry!"
	 << endl;
    return geoStatus;
  }

  // Create histograms
  CreateHistos();
  Reset();
  if ( fOnlineAnalysis ) {
    fOnlineCanvas = new TCanvas("StsSimOnline","Sts simulation online",10,10,600,600);
    fOnlinePad[0] = new TPad("titlePad",      "Title pad"             ,0.00,0.90,1.00,1.00);
    fOnlinePad[1] = new TPad("momentumPad",   "Momentum pad"          ,0.00,0.35,0.50,0.90);
    fOnlinePad[2] = new TPad("printoutPad","Print information pad    ",0.10,0.10,0.35,0.35);
    fOnlinePad[3] = new TPad("pointPad",      "Points per track pad"  ,0.50,0.50,1.00,0.90);
    fOnlinePad[4] = new TPad("stationPad1","Points on 1st station pad",0.50,0.25,0.75,0.50);
    fOnlinePad[5] = new TPad("stationPad2","Points on 3rd station pad",0.75,0.25,1.00,0.50);
    fOnlinePad[6] = new TPad("stationPad3","Points on 5th station pad",0.50,0.00,0.75,0.25);
    fOnlinePad[7] = new TPad("stationPad4","Points on 7th station pad",0.75,0.00,1.00,0.25);
    fOnlinePad[1]->SetLogy();
    for ( Int_t ipad = 0 ; ipad < 8 ; ipad++ ) {
      fOnlinePad[ipad]->SetFillColor(0);
      fOnlinePad[ipad]->SetBorderMode(0);
      fOnlinePad[ipad]->Draw();
    }

    fOnlinePad[0]->cd();
    TLegend* brp = new TLegend(0.1,0.1,0.9,0.9,"Online Sts simulation");
    brp->SetTextAlign(22);
    brp->SetTextSize(0.6);
    brp->SetTextColor(1);
    brp->SetBorderSize(0);
    brp->SetFillColor(0);
    brp->Draw();
    fOnlinePad[0]->Update();
  }

  fhNofStsStations->SetBinContent(1,fNStations);

  // Output
  cout << "   Number of Sts stations : " << fNStations << endl;
  if (fActive) cout << "   *****   Task is ACTIVE   *****" << endl;
  cout << "==========================================================="
       << endl << endl;

  return geoStatus;

}
// -------------------------------------------------------------------------



// -----   Public method ReInit   ------------------------------------------
InitStatus CbmStsSimulationQa::ReInit() {

  cout << "==========================================================="
       << endl;;
  cout << GetName() << ": Reinitialising..." << endl;

  // Get the geometry of target and Sts
  InitStatus geoStatus = GetGeometry();
  if ( geoStatus != kSUCCESS ) {
    cout << "-E- " << GetName() << "::ReInit: Error in reading geometry!"
	 << endl;
    return geoStatus;
  }

  // Output
  cout << "   Number of Sts stations : " << fNStations << endl;
  if (fActive) cout << "   *****   Task is ACTIVE   *****" << endl;
  cout << "==========================================================="
       << endl << endl;

  return geoStatus;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsSimulationQa::Exec(Option_t* opt) {
  //  cout << "EXEC" << endl;

  Int_t nofMCTracks = fMCTracks->GetEntriesFast();
  Int_t nofSTSPoints = fSTSPoints->GetEntriesFast();
  
  for ( Int_t itr = 0 ; itr < nofMCTracks ; itr++ ) {
    CbmMCTrack *mctrack= (CbmMCTrack*)fMCTracks->At(itr);
    Int_t pdgCode = mctrack->GetPdgCode();

    // reject funny particles
    if ( ( pdgCode == 10010020) ||
	 ( pdgCode == 10010030) ||
	 ( pdgCode == 50000050) ||
	 ( pdgCode == 50010051) ||
	 ( pdgCode == 10020040) )
      continue;
    
    TVector3 mom;
    mctrack->GetMomentum(mom);
    Float_t pT  = mom.Pt();
    Float_t p   = mom.Mag();

    TVector3 startvtx;
    mctrack->GetStartVertex(startvtx);
    Float_t vertexZ  = startvtx.z();

    TLorentzVector mom4;
    mctrack->Get4Momentum(mom4);
    Float_t rapidity =  mom4.Rapidity();

    Int_t stsPoints = mctrack->GetNPoints(kGEM);

    if(stsPoints>0 && vertexZ<=100) {
      fhMomAll      ->Fill(p);
      fhYPtMapAll   ->Fill(rapidity,pT);
      fhPdgCodeAll  ->Fill(pdgCode);
      fhStsPointsAll->Fill(stsPoints);
      fhMomStsPoints->Fill(stsPoints,p);
    }
    if(stsPoints>3 && vertexZ<=100) {
      fhMomRec      ->Fill(p);
      fhYPtMapRec   ->Fill(rapidity,pT);
      fhPdgCodeRec  ->Fill(pdgCode);
      fhStsPointsRec->Fill(stsPoints);
    }
  }

  //  cout << "track loop done" << endl;

  for ( Int_t ipnt = 0 ; ipnt < nofSTSPoints ; ipnt++ ) {
    CbmStsPoint *stsPoint= (CbmStsPoint*)fSTSPoints->At(ipnt);
    Float_t z  = stsPoint->GetZ(); // [cm]
    Float_t x  = stsPoint->GetX(z); // [cm]
    Float_t y  = stsPoint->GetY(z); // [cm]

    fhStsPointsPosition->Fill(z,x,y);
//     cout << "filled 3d,    MCId = " << stsPoint->GetDetectorID()
// 	 << "             nr = " << fStationNrFromMcId[stsPoint->GetDetectorID()] << endl;
    fhStationPoints[fStationNrFromMcId[stsPoint->GetDetectorID()]]->Fill(x,y);
//     cout << "filled 2d for MCId = " << stsPoint->GetDetectorID()
// 	 << " and station nr = " << fStationNrFromMcId[stsPoint->GetDetectorID()] << endl;
  }

  //  cout << "hello" << endl;

  Double_t tracksPerEvent = (Double_t)(fhMomAll->GetEntries())/((Double_t)fNEvents+1);
  Double_t pointsPerEvent = (Double_t)(fhStsPointsPosition->GetEntries())/((Double_t)fNEvents+1);

  if ( fOnlineAnalysis ) {
    fOnlinePad[1]->cd();
    fhMomRec->Draw();
    fOnlinePad[1]->Update();
    fOnlinePad[2]->cd();
    TPaveText* printoutPave = new TPaveText(0.1,0.1,0.9,0.9);
    printoutPave->SetTextAlign(22);
    printoutPave->SetTextSize(0.1);
    printoutPave->SetTextColor(1);
    printoutPave->SetBorderSize(0);
    printoutPave->SetFillColor(0);
    printoutPave->AddText(Form("%i events",fNEvents+1));
    printoutPave->AddText(Form("tracks/event = %3.2f",tracksPerEvent));
    printoutPave->AddText(Form("points/event = %3.2f",pointsPerEvent));
    fOnlinePad[2]->Clear();
    printoutPave->Draw();
    fOnlinePad[2]->Update();
    fOnlinePad[3]->cd();
    fhStsPointsRec->Draw();
    fOnlinePad[3]->Update();
 
    if ( fNStations ) {
      fOnlinePad[4]->cd();
      fhStationPoints[0]->Draw("colz");
      fOnlinePad[4]->Update();
      if ( fNStations>2 ) {
	fOnlinePad[5]->cd();
	fhStationPoints[2]->Draw("colz");
	fOnlinePad[5]->Update();
	if ( fNStations>4 ) {
	  fOnlinePad[6]->cd();
	  fhStationPoints[4]->Draw("colz");
	  fOnlinePad[6]->Update();
	  if ( fNStations>6 ) {
	    fOnlinePad[7]->cd();
	    fhStationPoints[6]->Draw("colz");
	    fOnlinePad[7]->Update();
	  }
	}
      }
    }

    
  }

  //  cout << "\rEvent #" << fNEvents+1 << flush;
  cout << endl << endl;
  cout << "======================================================="<< endl;
  cout << "===== StsSimulationQa: " << endl;
  cout << "===== \rEvent #" << fNEvents+1 << endl;
  cout << "===== " << setprecision(6) << tracksPerEvent << " tracks/event" << endl;
  cout << "===== " << setprecision(7) << pointsPerEvent << " points/event" << endl;
  cout << "======================================================="<< endl;

  fNEvents++;

  fhNofEvents->SetBinContent(1,fNEvents);
}
// -------------------------------------------------------------------------



// -----   Private method Finish   -----------------------------------------
void CbmStsSimulationQa::Finish()
{
  // Run summary to screen
//   cout << endl << endl;
//   cout << "======================================================="<< endl;
//   cout << "            StsSimulationQa: Run summary" << endl << endl;
//   cout << "======================================================="
//        << endl;
//   cout << endl << endl;

  gDirectory->mkdir("STSSimulationQA");
  gDirectory->cd("STSSimulationQA");
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  gDirectory->cd("..");
}
// -------------------------------------------------------------------------



// -----   Private method GetGeometry   ------------------------------------
InitStatus CbmStsSimulationQa::GetGeometry() {

  cout << "GET GEOMETRY" << endl;
  // Get Sts geometry
  if ( ! fStsGeo ) {
    cout << "-W- " << GetName() << "::GetGeometry: No passive geometry!"
	 <<endl;
    fNStations = 0;
    return kERROR;
  }
  TObjArray* stsNodes = fStsGeo->GetGeoSensitiveNodes();
  if ( ! stsNodes ) {
    cout << "-E- " << GetName() << "::GetGeometry: No Sts node array"
	 << endl;
    fNStations = 0;
    return kERROR;
  }
  Int_t tempNofStations = stsNodes->GetEntries();

//   cout << "There are " << tempNofStations << " nodes" << (tempNofStations > 10 ? "!!!" : "" ) << endl;

  TString geoNodeName;
  fNStations = 0;
  TString stationNames[10];
  for ( Int_t ist = 0 ; ist < tempNofStations ; ist++ ) {
    FairGeoNode* stsNode = (FairGeoNode*)stsNodes->At(ist);
    if ( ! stsNode ) {
      cout << "-W- CbmStsDigiScheme::Init: station#" << ist
	   << " not found among sensitive nodes " << endl;
      continue;
    }
    geoNodeName = stsNode->GetName();
    TArrayD* params = stsNode->getParameters();

    Bool_t stationKnown = kFALSE;
    // check if the node belongs to some station, save the MCId and outer radius
    for ( Int_t ikst = 0 ; ikst < fNStations ; ikst++ )
      if ( geoNodeName.Contains(stationNames[ikst]) ) {
	FairGeoTransform* transform = stsNode->getLabTransform();
	FairGeoVector translat = transform->getTranslation();
	FairGeoTransform center = stsNode->getCenterPosition();
	FairGeoVector centerV = center.getTranslation();
	Double_t radialSize = TMath::Sqrt((translat.X() + centerV.X() + params->At(0))*
					  (translat.X() + centerV.X() + params->At(0)) +
					  (translat.Y() + centerV.Y() + params->At(1))*
					  (translat.Y() + centerV.Y() + params->At(1)));
	if ( radialSize > fStationRadius[ikst] ) {
// 	  cout << "Found bigger radius for station " << ikst << " -> " << radialSize << endl;
	  fStationRadius[ikst] = radialSize;
	}
	fStationNrFromMcId[stsNode->getMCid()] = ikst;
	stationKnown = kTRUE;
      }

    if ( stationKnown ) continue;

    // if not known, register it and save MCId
    fStationNrFromMcId[stsNode->getMCid()] = fNStations;

    if ( geoNodeName.Length() == 12 )
      fStationRadius[fNStations] = params->At(1);
    else {
      FairGeoTransform* transform = stsNode->getLabTransform();
      FairGeoVector translat = transform->getTranslation();
      FairGeoTransform center = stsNode->getCenterPosition();
      FairGeoVector centerV = center.getTranslation();
      Double_t radialSize = TMath::Sqrt((translat.X() + centerV.X() + params->At(0))*
					(translat.X() + centerV.X() + params->At(0)) +
					(translat.Y() + centerV.Y() + params->At(1))*
					(translat.Y() + centerV.Y() + params->At(1)));
      fStationRadius[fNStations] = radialSize;
    }
    // it will work only if the node name is organized as:
    // for station name is "stsstationXX", where XX is the station number (f.e. XX=07 for station number 7)
    // for sector  name is "stsstationXXanythingHereToDistinguishDifferentSectors"
    geoNodeName.Remove(12,geoNodeName.Length()-12);
    stationNames[fNStations] = geoNodeName.Data();
    fNStations++;

//     cout << "station #" << fNStations << " has MCID = " << stsNode->getMCid() << " and name " << stsNode->GetName() << endl;

    //    fStationsMCId[fNStations] = stsNode->getMCid(); // not used
  }
//   cout << "There are " << fNStations << " stations" << endl;

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Private method CreateHistos   -----------------------------------
void CbmStsSimulationQa::CreateHistos() {

  fHistoList = new TList();

  fhMomAll            = new TH1F("hMomAll","Momentum - all in STS",100,0,50);
  fhMomAll->SetXTitle("p [GeV/c]");  fhMomAll->SetYTitle("yield [a.u.]");
  fhYPtMapAll         = new TH2F("hYPtMapAll","Rapidity - trans. mom. map - all in STS",100,-3,7,100,0,5);
  fhYPtMapAll->SetXTitle("rapidity");  fhYPtMapAll->SetYTitle("p_{t} [GeV/c]");
  fhPdgCodeAll        = new TH1F("hPdgCodeAll","PDG code - all in STS",1000,-500,500);
  fhStsPointsAll      = new TH1F("hStsPointsAll","STSPoints per track - all in STS",2*fNStations,0.5,2*fNStations+0.5);
  fhStsPointsAll->SetXTitle("nof points");  fhStsPointsAll->SetYTitle("yield [a.u.]");
  fhMomRec            = new TH1F("hMomRec","Momentum - rec in STS",100,0,50);
  fhMomRec->SetXTitle("p [GeV/c]");  fhMomRec->SetYTitle("yield [a.u.]");
  fhYPtMapRec         = new TH2F("hYPtMapRec","Rapidity - trans. mom. map - rec in STS",100,-3,7,100,0,5);
  fhYPtMapRec->SetXTitle("rapidity");  fhYPtMapRec->SetYTitle("p_{t} [GeV/c]");
  fhPdgCodeRec        = new TH1F("hPdgCodeRec","PDG code - rec in STS",1000,-500,500);
  fhStsPointsRec      = new TH1F("hStsPointsRec","STSPoints per track - rec in STS",2*fNStations,0.5,2*fNStations+0.5);
  fhStsPointsRec->SetXTitle("nof points");  fhStsPointsRec->SetYTitle("yield [a.u.]");
  fhMomStsPoints      = new TH2F("hMomStsPoints","momentum vs STSPoints per track",1000,0,9,100,0,50);
  fhStsPointsPosition = new TH3F("hStsPointsPosition","STS hits",100,0,100,100,-50,50,100,-50,50);
  fHistoList->Add(fhMomAll);
  fHistoList->Add(fhYPtMapAll);
  fHistoList->Add(fhPdgCodeAll);
  fHistoList->Add(fhStsPointsAll);
  fHistoList->Add(fhMomRec);
  fHistoList->Add(fhYPtMapRec);
  fHistoList->Add(fhPdgCodeRec);
  fHistoList->Add(fhStsPointsRec);
  fHistoList->Add(fhMomStsPoints);
  fHistoList->Add(fhStsPointsPosition);

  for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
    Int_t histSize = (Int_t)(1.05*fStationRadius[ist])+1;
    fhStationPoints[ist] = new TH2F(Form("hStationPoints%i",ist+1),
				    Form("Points at station %i",ist+1),
				    40*histSize,-histSize,histSize,
				    40*histSize,-histSize,histSize);
    fhStationPoints[ist]->SetXTitle("x [cm]");  fhStationPoints[ist]->SetYTitle("y [cm]");
    fHistoList->Add(fhStationPoints[ist]);
  }

  fhNofEvents      = new TH1F("hNofEvents","Number of events",1,0.,1.);
  fhNofStsStations = new TH1F("hNofStsStations","Number of stations",1,0.,1.);
  fHistoList->Add(fhNofEvents);
  fHistoList->Add(fhNofStsStations);
}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmStsSimulationQa::Reset() {
  fNEvents = 0;
}
// -------------------------------------------------------------------------


ClassImp(CbmStsSimulationQa)
