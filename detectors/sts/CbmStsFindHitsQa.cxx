// -------------------------------------------------------------------------
// -----                    CbmStsFindHitsQa source file               -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------

#include "CbmStsFindHitsQa.h"

#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsCluster.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsPoint.h"
#include "CbmStsHit.h"
#include "CbmStsSensor.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"
#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TPad.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TList.h"
#include "TVector3.h"

#include <iomanip>
using namespace std;

// -----   Default constructor   ------------------------------------------
CbmStsFindHitsQa::CbmStsFindHitsQa() 
    : FairTask("STS Hit Finder", 1),
      fGeoPar(NULL),
      fDigiPar(NULL),
      fDigiScheme(NULL),
      fMCTracks(NULL),          // MCtrack
      fStsPoints(NULL),        // StsPoints
      fStsHits(NULL),          // StsHits
      fMatches(NULL),          // StsTrackMatch
      fStsDigis(NULL),          // StsDigi
      fStsClusters(NULL),          // StsCluster

      fNStations(0),
      fNEvents(0),
      fTime1(0.),

      fhHitFindingEfficiency(),
      fhEffIncAng(),
      fhEffMom(),
      fhEffPdgSec(),
      fhEffPdgPrim(),
      fhHitPointPull(),
      fhHitPointCorr(),
      fHistoList(),
      fTimer(),
      fNofHits(),
      fNofPointsPrim(),
      fNofPointsSec(),
      fNofRecoPrim(),
      fNofRecoSec(),
      fNofPointsMomSum(),
      fNofRecoPointsMomSum(),
      fOnlineAnalysis(kFALSE),
      recoCanvas()
{  
    //AZ fDigiScheme = new CbmStsDigiScheme();
    fDigiScheme = CbmStsDigiScheme::Instance();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsFindHitsQa::CbmStsFindHitsQa(Bool_t visualizeBool, Int_t iVerbose)
    : FairTask("STSFindHitsQa", iVerbose),
      fOnlineAnalysis(visualizeBool),
      fGeoPar(NULL),
      fDigiPar(NULL),
      fDigiScheme(NULL),
      fMCTracks(NULL),          // MCtrack
      fStsPoints(NULL),          // StsPoints
      fStsHits(NULL),          // StsHits
      fMatches(NULL),          // StsTrackMatch
      fStsClusters(NULL),          // StsCluster
      fStsDigis(NULL),          // StsDigi
      fNStations(0),
      fNEvents(0),
      fTime1(0.),

      fhHitFindingEfficiency(),
      fhEffIncAng(),
      fhEffMom(),
      fhEffPdgSec(),
      fhEffPdgPrim(),
      fhHitPointPull(),
      fhHitPointCorr(),
      fHistoList(),
      fTimer(),
      fNofHits(),
      fNofPointsPrim(),
      fNofPointsSec(),
      fNofRecoPrim(),
      fNofRecoSec(),
      fNofPointsMomSum(),
      fNofRecoPointsMomSum(),
      recoCanvas()
{  
    //AZ fDigiScheme = new CbmStsDigiScheme();
    fDigiScheme = CbmStsDigiScheme::Instance();
}
// -------------------------------------------------------------------------

CbmStsFindHitsQa::CbmStsFindHitsQa(Int_t iVerbose)
    : FairTask("STSFindHitsQa", iVerbose),
      fOnlineAnalysis(0),
      fGeoPar(NULL),
      fDigiPar(NULL),
      fDigiScheme(NULL),
      fMCTracks(NULL),          // MCtrack
      fStsPoints(NULL),          // StsPoints
      fStsHits(NULL),          // StsHits
      fMatches(NULL),          // StsTrackMatch
      fStsClusters(NULL),          // StsCluster
      fStsDigis(NULL),          // StsDigi
      fNStations(0),
      fNEvents(0),
      fTime1(0.),

      fhHitFindingEfficiency(),
      fhEffIncAng(),
      fhEffMom(),
      fhEffPdgSec(),
      fhEffPdgPrim(),
      fhHitPointPull(),
      fhHitPointCorr(),
      fHistoList(),
      fTimer(),
      fNofHits(),
      fNofPointsPrim(),
      fNofPointsSec(),
      fNofRecoPrim(),
      fNofRecoSec(),
      fNofPointsMomSum(),
      fNofRecoPointsMomSum(),
      recoCanvas()
{  
    //AZ fDigiScheme = new CbmStsDigiScheme();
    fDigiScheme = CbmStsDigiScheme::Instance();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmStsFindHitsQa::CbmStsFindHitsQa(const char* name, Int_t iVerbose) 
    : FairTask(name, iVerbose),
      fGeoPar(NULL),
      fDigiPar(NULL),
      fDigiScheme(NULL),
      fMCTracks(NULL),          // MCtrack
      fStsPoints(NULL),          // StsPoints
      fStsHits(NULL),          // StsHits
      fMatches(NULL),          // StsTrackMatch
      fStsClusters(NULL),          // StsCluster
      fStsDigis(NULL),          // StsDigi
      fNStations(0),
      fNEvents(0),
      fTime1(0.),

      fhHitFindingEfficiency(),
      fhEffIncAng(),
      fhEffMom(),
      fhEffPdgSec(),
      fhEffPdgPrim(),
      fhHitPointPull(),
      fhHitPointCorr(),
      fHistoList(),
      fTimer(),
      fNofPointsPrim(),
      fNofPointsSec(),
      fNofRecoPrim(),
      fNofRecoSec(),
      fNofPointsMomSum(),
      fNofRecoPointsMomSum(),
      fNofHits(),
      fOnlineAnalysis(kFALSE),
      recoCanvas()
{
    //AZ fDigiScheme = new CbmStsDigiScheme();
    fDigiScheme = CbmStsDigiScheme::Instance();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmStsFindHitsQa::~CbmStsFindHitsQa() {
    fHistoList->Delete();
    delete fHistoList;
    for ( Int_t istat = 0 ; istat < fNStations ; istat++ ) {
        fHistoListPS[istat]->Delete();
    }
    delete[] fHistoList;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmStsFindHitsQa::Exec(Option_t* opt) {

    fTimer.Start();
    Bool_t warn = kFALSE;

    // Check for digi scheme
    if ( ! fDigiScheme ) {
        cerr << "-E- " << fName << "::Exec: No digi scheme!" << endl;
        return;
    }

    CbmStsStation* station = NULL;
    CbmStsSector*  sector  = NULL;

    Int_t nofStsDigis = fStsDigis->GetEntriesFast();
    for ( Int_t idigi = 0 ; idigi < nofStsDigis ; idigi++ ) {
        CbmStsDigi *stsDigi     = (CbmStsDigi*)fStsDigis->At(idigi);
        //  cout << "digi side = " << stsDigi->GetSide() << endl;
        //    fNofFiredDigis[stsDigi->GetStationNr()-1][stsDigi->GetSectorNr()-1][stsDigi->GetSide()] += 1;
        fNofDigisPChip[stsDigi->GetStationNr()-1][stsDigi->GetSectorNr()-1][stsDigi->GetSide()][(Int_t)(stsDigi->GetChannelNr()/128)] += 1;
    }
    Int_t nofStsHits = fStsHits->GetEntriesFast();
    Int_t nofStsPoints = fStsPoints->GetEntriesFast();
    Int_t hitStationLimits[2][100];

    for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
        hitStationLimits[0][ist] = -1;
        hitStationLimits[1][ist] = -1;
    }

    // check for limits of hit indices on different stations...

    for ( Int_t ihit = 0 ; ihit < nofStsHits ; ihit++ ) {
        CbmStsHit *stsHit     = (CbmStsHit*)fStsHits->At(ihit);
        fNofHits[stsHit->GetStationNr()-1][stsHit->GetSectorNr()-1][stsHit->GetSensorNr()-1] += 1; // count hits per sensor
        if ( hitStationLimits[0][stsHit->GetStationNr()-1] == -1 ) {
            hitStationLimits[0][stsHit->GetStationNr()-1] = ihit;
        }
        CbmStsHit *stsHitBack = (CbmStsHit*)fStsHits->At(nofStsHits-ihit-1);
        if ( hitStationLimits[1][stsHitBack->GetStationNr()-1] == -1 ) {
            hitStationLimits[1][stsHitBack->GetStationNr()-1] = nofStsHits-ihit;
        }
    }
    for ( Int_t istat = 0 ; istat < fDigiScheme->GetNStations() ; istat++ ) {
        station = fDigiScheme->GetStation(istat);
        for ( Int_t isect = 0 ; isect < station->GetNSectors() ; isect++ ) {
            sector = station->GetSector(isect);

            CbmStsSensor* sensor = (CbmStsSensor*)sector->GetSensor(0);
            for ( Int_t iside = 0 ; iside<2; iside++){
                for (Int_t iChip = 0; iChip<8; iChip++){
                    if (fNEvents==999) cout<<" St "<<istat+1<<" sect "<<isect+1<<" nofsens "<<sector->GetNSensors()<<" sectX "<<sensor->GetX0()<<" sectY "<<sensor->GetY0()<<" side "<<iside<<" chip "<<iChip+1<<" fNofDigisPChip "<<fNofDigisPChip[istat][isect][iside][iChip]<<endl;
                }
            }

            for ( Int_t isens = 0 ; isens < sector->GetNSensors() ; isens++ ) {
                //	cout << "filling   " << istat << " " << isect << " " << isens << endl;
                fhNofHits[istat][isect][isens] -> Fill(fNofHits[istat][isect][isens]);
                //	cout << "---> done " << isens << endl;

            }
            //       for ( Int_t iside = 0 ; iside < 2 ; iside++ ) {
            // 	fhNofFiredDigis[istat][isect][iside]->Fill(fNofFiredDigis[istat][isect][iside]);
            // 	for ( Int_t ichip = 0 ; ichip < 8 ; ichip++ ) {
            // 	  if ( fhNofDigisPChip[istat][isect][iside][ichip] )
            // 	    fhNofDigisPChip[istat][isect][iside][ichip]->Fill(fNofDigisPChip[istat][isect][iside][ichip]);
            // 	}
            //       }

        }
    }
    for ( Int_t ipnt = 0 ; ipnt < nofStsPoints ; ipnt++ ) {
        CbmStsPoint *stsPoint = (CbmStsPoint*)fStsPoints->At(ipnt);

        //    cout << "point " << ipnt << flush;

        Int_t trackPdg  = 0;
        Int_t motherId  = 0;
        Int_t motherPdg = 0;
        TVector3 trackVertex;
        TVector3 motherVertex;
        TVector3 momentum;

        Double_t xin = stsPoint->GetXIn();
        Double_t yin = stsPoint->GetYIn();
        Double_t zin = stsPoint->GetZIn();
        Double_t xout = stsPoint->GetXOut();
        Double_t yout = stsPoint->GetYOut();
        Double_t zout = stsPoint->GetZOut();

        gGeoManager->FindNode(xin,yin,zin);
        TGeoNode* curNode = gGeoManager->GetCurrentNode();

        Int_t trackId = stsPoint->GetTrackID();
        CbmMCTrack *mcTrack = (CbmMCTrack*) fMCTracks->At(trackId);
        trackPdg = mcTrack -> GetPdgCode();
        motherId = mcTrack -> GetMotherId();
        mcTrack->GetMomentum(momentum);
        Double_t mom = momentum.Mag();

        if (motherId>=0 ) {
            CbmMCTrack *motherTrack = (CbmMCTrack*) fMCTracks->At(motherId);
            motherPdg = motherTrack->GetPdgCode();
            mcTrack->GetStartVertex(trackVertex);
            motherTrack->GetStartVertex(motherVertex);

        }

        //    cout << " in node \"" << curNode->GetName() << "\"" << flush;
        TString curPath = fDigiScheme->GetCurrentPath();
        CbmStsSensor* sensor = fDigiScheme->GetSensorByName(curPath);
        Int_t stationNr = sensor->GetStationNr();

        Int_t startHit = hitStationLimits[0][stationNr-1];
        Int_t finalHit = hitStationLimits[1][stationNr-1];

        //    cout << " at station " << stationNr << " ( " << startHit << " - " << finalHit << " )" << endl;

        fhEnergyLoss[stationNr-1]->Fill(stsPoint->GetXIn(),stsPoint->GetYIn(),stsPoint->GetEnergyLoss());

        fhIncAngle[stationNr-1]->Fill(stsPoint->GetXIn(),stsPoint->GetYIn(),TMath::Abs(TMath::ATan2((stsPoint->GetZOut()-stsPoint->GetZIn()),(stsPoint->GetXOut()-stsPoint->GetXIn()))*180./3.14));
        //     Float_t zP = stsPoint->GetZ();
        //     Float_t xP = stsPoint->GetX(zP);
        //     Float_t yP = stsPoint->GetY(zP);

        Bool_t foundPnt = kFALSE;

        if ( startHit == -1 && finalHit == -1 ) continue;

        for ( Int_t ihit = startHit ; ihit < finalHit ; ihit++ ) {
            CbmStsHit *stsHit= (CbmStsHit*)fStsHits->At(ihit);
            if ( ( TMath::Abs(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ())) < .01 ) &&
                 ( TMath::Abs(stsHit->GetY()-stsPoint->GetY(stsHit->GetZ())) < .04 ) ) {
                fhHitPointCorrelation[stationNr-1]->Fill(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()),
                                                         stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()));
                fhHitPointCorr->Fill(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()),
                                     stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()));
                fhHitPointPull -> Fill( (stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()))/stsHit->GetDx() , (stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()))/stsHit->GetDy());

                if ( stsPoint->GetY(stsHit->GetZ()) > 0. )
                    fhHitPointCorrelationU[stationNr-1]->Fill(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()),
                                                              stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()));
                if ( stsPoint->GetY(stsHit->GetZ()) < 0. )
                    fhHitPointCorrelationB[stationNr-1]->Fill(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()),
                                                              stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()));
            }

            if ( ( TMath::Abs(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ())) < .01 ) &&
                 ( TMath::Abs(stsHit->GetY()-stsPoint->GetY(stsHit->GetZ())) < .04 ) ) {
                foundPnt = kTRUE;
            }
            //       cout <<"St "<<stationNr<<" Sens "<<sensor->GetSensorNr()<< " chan "<<sensor->GetFrontChannel(stsHit->GetX(),stsHit->GetY(),stsHit->GetZ())<<" X "<<stsHit->GetX();
        }
        Int_t incAngle = (Int_t) (TMath::ATan2((stsPoint->GetZOut()-stsPoint->GetZIn()),(stsPoint->GetXOut()-stsPoint->GetXIn()))*180./3.14);
        //    Int_t incAngle = (Int_t) ( TMath::ATan((stsPoint->GetXOut()-stsPoint->GetXIn())/(stsPoint->GetZOut()-stsPoint->GetZIn()))*180./3.14);
        fNofPoints    [stationNr-1][sensor->GetSectorNr()-1][sensor->GetSensorNr()-1] += 1;
        fNofPointsIncAng[TMath::Abs((Int_t)(incAngle))] +=1;
        if (TMath::Abs((Int_t)(mom))<1) fNofPointsMom [TMath::Abs((Int_t)(mom))] +=1;
        if (TMath::Abs((Int_t)(mom))<1) fNofPointsMomSum+=1;
        //  cout << stationNr-1 << " " << sensor->GetSectorNr()-1 << " " << sensor->GetSensorNr()-1 << "/" << flush;
        fhPoints      [stationNr-1]->Fill(stsPoint->GetX(stsPoint->GetZ()),
                                          stsPoint->GetY(stsPoint->GetZ()));
        fNofPointsSec = 1;
        if (trackPdg<10000&&motherId>=0) {
            fNofPointsPdgSec[trackPdg]  += 1;
            fNofPointsSec +=1;
        }
        fNofPointsPrim = 1;
        if (trackPdg<10000&&motherId<0) {
            fNofPointsPdgPrim[trackPdg]  += 1;
            fNofPointsPrim +=1;
        }
        if ( foundPnt ) {
            fNofRecoPoints[stationNr-1][sensor->GetSectorNr()-1][sensor->GetSensorNr()-1] += 1;
            fNofRecoPointsIncAng[TMath::Abs((Int_t)(incAngle))] +=1;
            if (TMath::Abs((Int_t)(mom))<1) fNofRecoPointsMom [TMath::Abs((Int_t)(mom))] +=1;
            if (TMath::Abs((Int_t)(mom))<1) fNofRecoPointsMomSum+=1;
            fhRecoPoints  [stationNr-1]->Fill(stsPoint->GetX(stsPoint->GetZ()),
                                              stsPoint->GetY(stsPoint->GetZ()));
            fNofRecoPrim = 1;
            if (trackPdg<10000&&motherId>=0) {
                fNofRecoPdgSec[trackPdg] += 1;
                fNofRecoSec +=1;
            }
            fNofRecoPrim = 1;
            if (trackPdg<10000&&motherId<0) {
                fNofRecoPdgPrim[trackPdg] += 1;
                fNofRecoPrim +=1;
            }
        }

    }
    for (Int_t ang=0; ang<400; ang++) {
        Double_t factor = (Float_t)(fNofRecoPointsIncAng[ang])/(Float_t)(fNofPointsIncAng[ang]);
        fhEffIncAng->Fill(ang,((Float_t)(fNofRecoPointsIncAng[ang]))/((Float_t)(fNofPointsIncAng[ang])));
    }
    for (Int_t momentum=0; momentum<50; momentum++) {
        fhEffMom->Fill(momentum,((Float_t)(fNofRecoPointsMom[momentum]))/((Float_t)(fNofPointsMom[momentum])));
    }

    if ( fOnlineAnalysis ) {
        recoPad[0]->cd();
        TLegend* brp = new TLegend(0.1,0.1,0.9,0.9,"STS hit finding");
        brp->SetTextAlign(22);
        brp->SetTextSize(0.6);
        brp->SetTextColor(1);
        brp->SetBorderSize(0);
        brp->SetFillColor(0);
        brp->Draw();
        recoPad[0]->Update();

        recoPad[1]->cd();
        fhEffIncAng->SetLineWidth(2);
        fhEffIncAng->SetLineColor(1);
        fhEffIncAng->SetMarkerColor(kGreen);
        fhEffIncAng->SetMarkerStyle(2);
        fhEffIncAng->SetTitle("Efficiency");
        fhEffIncAng->Draw();
        TLine* oneLine = new TLine(0.0,1.0,10.0,1.0);
        oneLine->SetLineStyle(2);
        oneLine->Draw();
        recoPad[1]->Update();

        TH1F* projX[10];
        TH1F* projY[10];
        TF1*   fitX[10];
        TF1*   fitY[10];
        Double_t resolution[2][10];

        for ( Int_t ist = 0 ; ist < 8 ; ist++ ) {
            projX[ist] = (TH1F*)fhHitPointCorrelation[ist]->ProjectionX(Form("projX%i",ist+1));
            projY[ist] = (TH1F*)fhHitPointCorrelation[ist]->ProjectionY(Form("projY%i",ist+1));
            fitX[ist] = new TF1(Form("fitX%i",ist+1),"gaus",-0.02,0.02);
            fitY[ist] = new TF1(Form("fitY%i",ist+1),"gaus",-0.02,0.02);
            projX[ist]->SetAxisRange(-0.02,0.02,"X");
            projY[ist]->SetAxisRange(-0.02,0.02,"X");
            projX[ist]->Fit(fitX[ist],"QN","",-0.02,0.02);
            projY[ist]->Fit(fitY[ist],"QN","",-0.02,0.02);
            resolution[0][ist] = 10000.*fitX[ist]->GetParameter(2);
            resolution[1][ist] = 10000.*fitY[ist]->GetParameter(2);
        }

        TH1F* projXPull;
        TH1F* projYPull;
        TF1*   fitXPull;
        TF1*   fitYPull;
        Double_t resolutionPull[2];


        projXPull = (TH1F*)fhHitPointPull->ProjectionX("projXPull");
        projYPull = (TH1F*)fhHitPointPull->ProjectionY("projYPull");
        fitXPull = new TF1("fitXPull","gaus",-2,2);
        fitYPull = new TF1("fitYPull","gaus",-2,2);
        projXPull->SetAxisRange(-2,2,"X");
        projYPull->SetAxisRange(-2,2,"X");
        projXPull->Fit(fitXPull,"QN","",-2,2);
        projYPull->Fit(fitYPull,"QN","",-2,2);
        resolutionPull[0] = fitXPull->GetParameter(2);
        resolutionPull[1] = fitYPull->GetParameter(2);


        recoPad[3]->cd();
        fhHitPointCorrelation[0]->Draw("col");
        recoPad[3]->Update();

        recoPad[4]->cd();
        fhHitPointPull->Draw("col");
        recoPad[4]->Update();

        recoPad[5]->cd();
        projX[0]->SetLineWidth(2);
        projY[0]->SetLineWidth(2);
        projX[0]->SetLineColor(2);
        projY[0]->SetLineColor(3);
        projY[0]->SetXTitle("#Delta x, y [cm]");
        projX[0]->Draw();
        projY[0]->Draw("same");
        //  fitY[fShowStation1]->Draw("same");
        //  fitX[fShowStation1]->Draw("same");
        TLegend* legend1 = new TLegend(0.55,0.6,1.0,0.94);
        legend1->SetBorderSize(0);
        legend1->SetFillColor(0);
        legend1->AddEntry(projX[0],
                Form("X,#sigma=%3.2f#mum",resolution[0][0]),"l");
        legend1->AddEntry(projY[0],
                Form("Y,#sigma=%3.2f#mum",resolution[1][0]),"l");
        legend1->Draw();
        recoPad[5]->Update();

        recoPad[6]->cd();
        projXPull->SetLineWidth(2);
        projYPull->SetLineWidth(2);
        projXPull->SetLineColor(2);
        projYPull->SetLineColor(3);
        projXPull->SetXTitle("Pull x, y ");
        projXPull->Draw();
        projYPull->Draw("same");
        //  fitY[fShowStation2]->Draw("same");
        //  fitX[fShowStation2]->Draw("same");
        TLegend* legend2a = new TLegend(0.55,0.6,1.0,0.94);
        legend2a->SetBorderSize(0);
        legend2a->SetFillColor(0);
        legend2a->AddEntry(projXPull,Form("X,#sigma=%3.2f#mum",resolutionPull[0]),"l");
        legend2a->AddEntry(projYPull,Form("Y,#sigma=%3.2f#mum",resolutionPull[1]),"l");
        legend2a->Draw();
        recoPad[6]->Update();
        recoPad[2]->cd();
        fhRecoPoints[0]->SetMarkerColor(kCyan);
        fhRecoPoints[0]->Draw();
        recoPad[2]->Update();

        recoPad[7]->cd();
        fhRecoPoints[1]->SetMarkerColor(kMagenta);
        fhRecoPoints[1]->Draw();
        recoPad[7]->Update();

        recoPad[8]->cd();
        fhRecoPoints[2]->SetMarkerColor(kOrange);
        fhRecoPoints[2]->Draw();
        recoPad[8]->Update();

        recoPad[9]->cd();
        fhRecoPoints[4]->SetMarkerColor(kGreen);
        fhRecoPoints[4]->Draw();
        recoPad[9]->Update();

        recoPad[10]->cd();
        fhRecoPoints[5]->SetMarkerColor(kBlue);
        fhRecoPoints[5]->Draw();
        recoPad[10]->Update();

        recoPad[11]->cd();
        fhRecoPoints[7]->SetMarkerColor(kRed);
        fhRecoPoints[7]->Draw();
        recoPad[11]->Update();

        recoPad[12]->cd();
        TPaveText* printoutPave = new TPaveText(0.0,0.0,1.0,1.0);
        printoutPave->SetTextAlign(20);
        printoutPave->SetTextSize(0.08);
        printoutPave->SetTextColor(1);
        printoutPave->SetBorderSize(0);
        printoutPave->SetFillColor(0);
        printoutPave->AddText(Form("%i events",fNEvents+1));
        printoutPave->AddText(Form("Hits/Points %3.2f ",Double_t (nofStsHits)/Double_t (nofStsPoints)));

        printoutPave->AddText(Form("%3.0f points prim, %3.0f reco prim",
                                   Double_t (fNofPointsPrim)/Double_t (fNEvents+1),
                                   Double_t (fNofRecoPrim)/Double_t (fNEvents+1)));
        printoutPave->AddText(Form("%3.0f points sec, %3.0f reco sec",
                                   Double_t (fNofPointsSec)/Double_t (fNEvents+1),
                                   Double_t (fNofRecoSec)/Double_t (fNEvents+1)));

        printoutPave->AddText(Form("Hit Finding Efficiency: %3.2f  prim, %3.2f sec",
                                   (Double_t (fNofRecoPrim)/Double_t (fNofPointsPrim))*100.,
                                   (Double_t (fNofRecoSec) /Double_t (fNofPointsSec))*100.));
        //     printoutPave->AddText("Tracking efficiencies (p>1.0 GeV/c):");
        //     printoutPave->AddText(Form("all = %2.2f%%(%2.2f%%)",100.*effAll,100.*allEff));
        //     printoutPave->AddText(Form("vertex = %2.2f%%(%2.2f%%)",100.*effPrim,100.*primEff));
        //     printoutPave->AddText(Form("reference = %2.2f%%",100.*effRef));
        //     printoutPave->AddText(Form("non-vertex = %2.2f%%(%2.2f%%)",100.*effSec,100.*secEff));
        //     printoutPave->AddText(Form("Momentum resolution = %3.2f%%(%3.2f%%)",momentumResolutionAll,momentumResolutionPrim));
        recoPad[12]->Clear();
        printoutPave->Draw();
        recoPad[12]->Update();

    }

    // Event summary
    if ( fVerbose ) {
        cout << "----------   StsFindHitsQa : Event " << fNEvents+1 << " summary   ------------"
             << endl;
        cout << "-----------------------------------------------------------"
             << endl;
        cout << endl;
    }
    else
        cout << "\r+ " << setw(15) << left << fName << ": event " << fNEvents+1 << "  " << setprecision(4)
             << setw(8) << fixed << right << fTimer.RealTime() << "s." << endl;

    //cout << "--------        " << fNofRecoPoints[1][4][0] << " / " << fNofPoints[1][4][0] << endl;

    fNEvents++;
    fTime1 += fTimer.RealTime();
}
// -------------------------------------------------------------------------


// -----   Private method CreateHistos   -----------------------------------
void CbmStsFindHitsQa::CreateHistos() {


    // Histogram list
    fHistoList = new TList();
    for ( Int_t ist = 0 ; ist < fNStations ; ist++ )
        fHistoListPS[ist] = new TList();

    // Momentum distributions
    Double_t minMom   =  0.;
    Double_t maxMom   = 10.;
    Int_t    nBinsMom = 40;

    CbmStsStation* station = NULL;
    CbmStsSector*  sector  = NULL;
    CbmStsSensor*  sensor  = NULL;

    fhHitFindingEfficiency = new TH1F("hHitFindingEfficiency","Hit finding efficiency",1000000,
                                      (Float_t)(2<<24)-0.5,(Float_t)(2<<24)-0.5+1000000.);
    fhHitPointCorr = new TH2F(Form("hHitPointCorrelation"),
                              Form("Hit vs point correlation "),
                              500,-.1, .1,500,-.1,.1);
    fhEffMom     = new TH2F("hEffMom","hEffMom",500,0,25,100,0,1);
    fhEffIncAng  = new TH2F("hEffIncAng","hEffIncAng",100,0,100,100,0,1);
    fhEffPdgSec  = new TH2F("hEffPdgSec", "hEffPdgSec", 350,-1000,2500,10,0,1);
    fhEffPdgPrim = new TH2F("hEffPdgPrim","hEffPdgPrim",350,-1000,2500,10,0,1);
    fhHitPointPull = new TH2F("hHitPointPull","Hit vs sigma at station %i",500,-3., 3.,500,-3.,3.);

    fHistoList->Add(fhEffPdgSec);
    fHistoList->Add(fhEffPdgPrim);
    fHistoList->Add(fhHitFindingEfficiency);
    fHistoList->Add(fhEffIncAng);
    fHistoList->Add(fhHitPointPull);
    fHistoList->Add(fhEffMom);
    fHistoList->Add(fhHitPointCorr);
    for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
        fhHitPointCorrelation[ist] = new TH2F(Form("hHitPointCorrelation%i",ist+1),
                                              Form("Hit vs point correlation at station %i",ist+1),
                                              500,-.1, .1,500,-.1,.1);
        fhHitPointCorrelation[ist]->SetXTitle("#Delta x [cm]");
        fhHitPointCorrelation[ist]->SetYTitle("#Delta y [cm]");




        fHistoListPS[ist]->Add(fhHitPointCorrelation[ist]);


        fhHitPointCorrelationU[ist] = new TH2F(Form("hHitPointCorrelationU%i",ist+1),
                                               Form("Hit vs point correlation at station %i",ist+1),
                                               500,-.1, .1,500,-.1,.1);
        fhHitPointCorrelationB[ist] = new TH2F(Form("hHitPointCorrelationB%i",ist+1),
                                               Form("Hit vs point correlation at station %i",ist+1),
                                               500,-.1, .1,500,-.1,.1);
        fHistoListPS[ist]->Add(fhHitPointCorrelationU[ist]);
        fHistoListPS[ist]->Add(fhHitPointCorrelationB[ist]);

        fhPoints    [ist] = new TH2F(Form("hPoints%i",ist+1),Form("Points on station %i",ist+1),
                                     100,-100.,100.,
                                     100,-100.,100.);
        fhRecoPoints[ist] = new TH2F(Form("hRecoPoints%i",ist+1),Form("Reconstructed points on station %i",ist+1),
                                     100,-100.,100.,
                                     100,-100.,100.);
        fHistoListPS[ist]->Add(fhPoints    [ist]);
        fHistoListPS[ist]->Add(fhRecoPoints[ist]);

    }

    Double_t binningNofHits[501];
    binningNofHits[0] = -0.5;
    for ( Int_t itemp = 0 ; itemp < 100 ; itemp++ ) {
        binningNofHits[itemp+  1] = 0.5+(Double_t)itemp;
        binningNofHits[itemp+101] = 101.5+2.*(Double_t)itemp;
        binningNofHits[itemp+201] = 302.5+3.*(Double_t)itemp;
        binningNofHits[itemp+301] = 602.5+3.*(Double_t)itemp;
        binningNofHits[itemp+401] = 902.5+3.*(Double_t)itemp;
    }

    Double_t fSectorWidth = 0.;
    for ( Int_t istat = 0 ; istat < fNStations ; istat++ ) {
        station = fDigiScheme->GetStation(istat);

        fhEnergyLoss[istat] = new TH3F(Form("hEnergyLossSt%d",istat+1),
                                       Form("Energy loss on station %d",istat+1),
                                       20,-100.,100.,20,-100.,100.,100,0,0.01);
        fhIncAngle [istat] = new TH3F(Form("hIncAngleSt%d",istat+1),
                                      Form("Incident Angle on station %d;X;Y;Angle",istat+1),
                                      100,-50,50,100,-40,40,20,0,100.);
        fHistoListPS[istat]->Add(fhEnergyLoss[istat]);
        fHistoListPS[istat]->Add(fhIncAngle[istat]);
        for ( Int_t isect = 0 ; isect < station->GetNSectors() ; isect++ ) {
            sector = station->GetSector(isect);
            for ( Int_t isens = 0 ; isens < sector->GetNSensors() ; isens++ ) {
                sensor = sector->GetSensor(isens);
                fSectorWidth = 10.*sensor->GetLx();
                fhNofHits[istat][isect][isens] = new TH1F(Form("hNofHitsSt%dSect%dSens%d",istat+1,isect+1,isens+1),
                                                          Form("Number of hits in sector %d, sensor %d of station %d",isect+1,isens+1,istat+1),
                                                          500,binningNofHits);
                //					 500,-0.5,500.5);
                fHistoListPS[istat]->Add(fhNofHits[istat][isect][isens]);
            }
        }
    }

}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmStsFindHitsQa::Reset() {


    TIter next(fHistoList);
    while ( TH1* histo = ((TH1*)next()) ) histo->Reset();
    for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
        TIter next0(fHistoListPS[ist]);
        while ( TH1* histo = ((TH1*)next0()) ) histo->Reset();
    }

}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void CbmStsFindHitsQa::SetParContainers() {


    // Get run and runtime database
    FairRunAna* run = FairRunAna::Instance();
    if ( ! run ) Fatal("SetParContainers", "No analysis run");

    FairRuntimeDb* db = run->GetRuntimeDb();
    if ( ! db ) Fatal("SetParContainers", "No runtime database");

    // Get STS geometry parameter container
    fGeoPar = (CbmGeoStsPar*) db->getContainer("CbmGeoStsPar");

    // Get STS digitisation parameter container
    fDigiPar = (CbmStsDigiPar*) db->getContainer("CbmStsDigiPar");

}
// -------------------------------------------------------------------------


// -----   Private method Init   -------------------------------------------
InitStatus CbmStsFindHitsQa::Init() {


    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");

    // Get MCTrack array
    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    if ( ! fMCTracks ) {
        cout << "-E- " << GetName() << "::Init: No MCTrack array!" << endl;
        return kFATAL;
    }
    // Get StsPoint array
    fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
    if ( ! fStsPoints ) {
        cout << "-E- " << GetName() << "::Init: No StsPoint array!" << endl;
        return kFATAL;
    }

    // Get StsHit array
    fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
    if ( ! fStsHits ) {
        cout << "-E- " << GetName() << "::Init: No StsHit array!" << endl;
        return kFATAL;
    }

    // Get StsDigis array
    fStsDigis = (TClonesArray*) ioman->GetObject("StsDigi");
    if ( ! fStsDigis ) {
        cout << "-E- " << GetName() << "::Init: No StsDigi array!"
             << endl;
        return kERROR;
    }

    // Get StsClusters array
    fStsClusters = (TClonesArray*) ioman->GetObject("StsCluster");
    if ( ! fStsClusters ) {
        cout << "-w- " << GetName() << "::Init: No StsCluster array!"
             << endl;
        //  return kERROR;
    }

    // Build digitisation scheme
    Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
    if ( ! success ) return kERROR;

    fNStations = fDigiScheme->GetNStations();

    CbmStsStation* station = NULL;
    CbmStsSector*  sector  = NULL;
    for ( Int_t istat = fNStations ; istat > 0 ; ) {
        station = fDigiScheme->GetStation(--istat);
        for ( Int_t isect = station->GetNSectors() ; isect > 0 ; ) {
            sector = station->GetSector(--isect);
            for ( Int_t isens = sector->GetNSensors() ; isens > 0 ; ) {
                fNofPoints    [istat][isect][--isens] = 0;
                fNofRecoPoints[istat][isect][  isens] = 0;
            }
        }
    }
    for (Int_t iang=0; iang<400; iang++) {
        fNofPointsIncAng[iang] = 0;
        fNofRecoPointsIncAng[iang] = 0;
    }
    for (Int_t ipdg=-10000; ipdg<10000; ipdg++) {
        fNofRecoPdgSec    [ipdg] = 0;
        fNofPointsPdgSec  [ipdg] = 0;
        fNofRecoPdgPrim   [ipdg] = 0;
        fNofPointsPdgPrim [ipdg] = 0;
    }

    CreateHistos();
    Reset();
    if ( fOnlineAnalysis ) {
        recoCanvas = new TCanvas("StsRecoCanvas","Sts reconstruction",10,10,600,900);
        //     TPad* recoPad[10];

        recoPad[0] = new TPad("titlePad",   "Title pad"                       ,0.00,0.90,1.00,1.00);
        recoPad[1] = new TPad("efficiencyPad","Efficiency pad"                ,0.00,0.60,0.40,0.90);
        recoPad[3] = new TPad("hitVspoint1Pad","Hit vs point on st.1 pad"     ,0.40,0.65,0.70,0.90);
        recoPad[4] = new TPad("hitPullst1","Hit vs sigma on st.1 pad"         ,0.70,0.65,1.00,0.90);
        recoPad[5] = new TPad("hitVspntXY1Pad","Hit vs point XY on st.1 pad"  ,0.40,0.40,0.70,0.65);
        recoPad[6] = new TPad("hitVssigmaXY6Pad","Hit vs sigma XY on st.1 pad",0.70,0.40,1.00,0.65);
        recoPad[2] = new TPad("Reco Points st1","Reco Points st1"         	  ,0.00,0.20,0.33,0.40);
        recoPad[7] = new TPad("Reco Points st2","Reco Points st2"             ,0.33,0.20,0.66,0.40);
        recoPad[8] = new TPad("Reco Points st3","Reco Points st3"             ,0.66,0.20,1.00,0.40);
        recoPad[9] = new TPad("Reco Points st5","Reco Points st5"             ,0.00,0.00,0.33,0.20);
        recoPad[10] = new TPad("Reco Points st6","Reco Points st6"            ,0.33,0.00,0.66,0.20);
        recoPad[11] = new TPad("Reco Points st8","Reco Points st8"            ,0.66,0.00,1.00,0.20);
        recoPad[12] = new TPad("printoutPad","Print information pad"          ,0.00,0.40,0.40,0.60);
        //     recoPad[12]->SetLogz();
        for ( Int_t ipad = 0 ; ipad < 13 ; ipad++ ) {
            recoPad[ipad]->SetFillColor(0);
            recoPad[ipad]->SetBorderMode(0);
            recoPad[ipad]->Draw();
        }
    }
    // Control output
    if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
    else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
    cout << "-I- " << fName << "::Init: "	<< "STS digitisation scheme succesfully initialised" << endl;
    cout << "    Stations: " << fDigiScheme->GetNStations()
         << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: "
         << fDigiScheme->GetNChannels() << endl;

    return kSUCCESS;

}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsFindHitsQa::ReInit()
{
    // Clear digitisation scheme
    fDigiScheme->Clear();

    // Build new digitisation scheme
    Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
    if ( ! success ) return kERROR;

    // Control output
    if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
    else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
    cout << "-I- " << fName << "::Init: "
         << "STS digitisation scheme succesfully reinitialised" << endl;
    cout << "    Stations: " << fDigiScheme->GetNStations()
         << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: "
         << fDigiScheme->GetNChannels() << endl;

    return kSUCCESS;

}
// -------------------------------------------------------------------------

// -----   Virtual method Finish   -----------------------------------------
void CbmStsFindHitsQa::Finish() {

    CbmStsStation* station = NULL;
    CbmStsSector*  sector  = NULL;
    Int_t NofRecoPoints = 0;
    Int_t HFE_reco = 0;
    Int_t HFE_point = 0;
    for ( Int_t istat = 0 ; istat < fDigiScheme->GetNStations() ; istat++ ) {
        station = fDigiScheme->GetStation(istat);
        for ( Int_t isect = 0 ; isect < station->GetNSectors() ; isect++ ) {
            sector = station->GetSector(isect);
            for ( Int_t isens = 0 ; isens < sector->GetNSensors() ; isens++ ) {
                HFE_reco = 1;
                HFE_point = 1;
                if ( fNofPoints[istat][isect][isens] ) {
                    fhHitFindingEfficiency->Fill(2 << 24 | (istat+1) << 16 | (isect+1) << 4 | (isens+1) << 1,
                                                 ((Float_t)(fNofRecoPoints[istat][isect][isens]))/
                                                 ((Float_t)(fNofPoints[istat][isect][isens])));
                    HFE_reco += (fNofRecoPoints[istat][isect][isens]);
                    HFE_point += (fNofPoints[istat][isect][isens]);
                }
            }
        }
    }
    for (Int_t iPdg=-10000; iPdg<10000; iPdg++) {
        fhEffPdgSec  -> Fill(iPdg, ((Float_t)(fNofRecoPdgSec[iPdg]))  / ((Float_t)(fNofPointsPdgSec[iPdg])));
        fhEffPdgPrim -> Fill(iPdg, ((Float_t)(fNofRecoPdgPrim[iPdg])) / ((Float_t)(fNofPointsPdgPrim[iPdg])));
    }

    cout << endl;
    cout << "============================================================"
         << endl;
    cout << "===== " << fName << ": Run summary " << endl;
    cout << "===== Hit finding efficiency = "<<(((Float_t)(HFE_reco))/((Float_t)(HFE_point)))*100.<<"%"<< endl;
    cout << "============================================================"
         << endl;

    gDirectory->mkdir("STSFindHitsQA");
    gDirectory->cd("STSFindHitsQA");
    TIter next(fHistoList);
    while ( TH1* histo = ((TH1*)next()) ) histo->Write();

    for ( Int_t istat = 0 ; istat < fNStations ; istat++ ) {
        gDirectory->mkdir(Form("Station%d",istat+1));
        gDirectory->cd(Form("Station%d",istat+1));
        TIter nextO(fHistoListPS[istat]);
        while ( TH1* histo = ((TH1*)nextO()) ) histo->Write();

        gDirectory->cd("..");
    }
    gDirectory->cd("..");
}					       
// -------------------------------------------------------------------------

ClassImp(CbmStsFindHitsQa)
