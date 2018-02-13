//--------------------------------------------------------------------------------------------------------------------------------------
#include<assert.h>
#include<map>

#include <TRandom2.h>
#include <TGeoManager.h>
#include <TGeoBBox.h>
#include <TGeoMatrix.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TEfficiency.h>
#include <TVector3.h>

#include "FairLogger.h"

#include "CbmMCTrack.h"
//#include "BmnTofHit.h"
//#include "BmnTOF1Point.h"

//#include "BmnTof1GeoUtils.h"

#include "BmnLANDHitProducer.h"

using namespace std;

static Float_t workTime = 0.0;

ClassImp(BmnLANDHitProducer)
	//--------------------------------------------------------------------------------------------------------------------------------------
BmnLANDHitProducer::BmnLANDHitProducer(const char *name, Bool_t useMCdata, Int_t verbose, Bool_t test)
	:  fairTask{
		//	pGeoUtils = new BmnTof1GeoUtils;

	}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnLANDHitProducer::~BmnLANDHitProducer() 
{
	//delete pGeoUtils;
}
//--------------------------------------------------------------------------------------------------------------------------------------
InitStatus 		BmnLANDHitProducer::Init() 
{
	FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Begin [BmnLANDHitProducer::Init].");

	if(fOnlyPrimary) cout<<" Only primary particles are processed!!! \n"; // FIXME NOT used now ADDD

	aExpDigits = (TClonesArray*) FairRootManager::Instance()->GetObject("LAND");
	if (!aExpDigits)
	{
		cout<<"BmnLANDHitProducer::Init(): branch LAND not found! Task will be deactivated"<<endl;
		SetActive(kFALSE);
		return kERROR;
	}
	aExpDigitsT0 = (TClonesArray*) FairRootManager::Instance()->GetObject("T0");
	if (!aExpDigitsT0)
	{
		cout<<"BmnLANDHitProducer::Init(): branch T0 not found! Task will be deactivated"<<endl;
		SetActive(kFALSE);
		return kERROR;
	}

	pDetector = new BmnLANDDigit *[fNDetectors];

	aTofHits = new TClonesArray("BmnLANDHit");
	FairRootManager::Instance()->Register("BmnLandHit", "LAND", aTofHits, kTRUE);
	// Create and register output array
	/*	aTofHits = new TClonesArray("BmnTofHit");
		FairRootManager::Instance()->Register("BmnTof1Hit", "TOF1", aTofHits, kTRUE);

		fNDetectors = pGeoUtils->ParseTGeoManager(fUseMCData, h2TestStrips, true);
		pGeoUtils->FindNeighborStrips(h1TestDistance, h2TestNeighborPair, fDoTest);
		if (!fUseMCData) {
		pDetector = new BmnTOF1Detector *[fNDetectors];
		TString NameFileLRcorrection, NameFileSlewingCorrection, NameFileTimeShiftCorrection;
		NameFileLRcorrection = Form("TOF400_LRCorr_Period_%i.dat", NPeriod);
		NameFileSlewingCorrection = Form("TOF400_SlewingCorr_Period_%i.root", NPeriod);
		NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_Period_%i.dat", NPeriod);
		for (Int_t i = 0; i < fNDetectors; i++) {
		Int_t DoTestForDetector = 0;
		if (fDoTest == kTRUE) DoTestForDetector = 1; // Level of Histograms filling (0-don't fill, 1-low, 2-high)
		pDetector[i] = new BmnTOF1Detector(i, DoTestForDetector);
		pDetector[i]->SetCorrLR(NameFileLRcorrection);
		pDetector[i]->SetCorrSlewing(NameFileSlewingCorrection);
		pDetector[i]->SetCorrTimeShift(NameFileTimeShiftCorrection);
		pDetector[i]->SetGeo(pGeoUtils);
		}
		}
		*/
	FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Initialization [BmnLANDHitProducer::Init] finished succesfully.");

	return kSUCCESS;
}

void 		BmnLANDHitProducer::Exec(Option_t* opt) {
	if (!IsActive())
		return;
	clock_t tStart = clock();

	if (fVerbose) cout << endl << "======================== LAND exec started ====================" << endl;
	static const TVector3 XYZ_err(fErrX, fErrY, 0.); 

	aTofHits->Clear();

//	Int_t 		UID, trackID;	
	TVector3 	pos, poslab; 	
	Int_t nT0Digits = aExpDigitsT0->GetEntriesFast();
	float dPlane=10; //spacing bwt planes tmp
	float MinTime=0;
	if (nT0Digits == 1) { // T0 digit should be
		BmnTrigDigit* digT0 = (BmnTrigDigit*) aExpDigitsT0->At(0);

		//for (Int_t i = 0; i < fNDetectors; i++)
			//pDetector[i]->Clear();

		for (Int_t iDig = 0; iDig < aExpDigits->GetEntriesFast(); ++iDig) {
			BmnLANDDigit* digTof = (BmnLANDDigit*) aExpDigits->At(iDig);
			//pDetector[digTof->GetPlane()]->SetDigit(digTof);
			/*if(digTof->GetPlane()==5) continue;
			if(digTof->GetTime()<MinTime) 
			{
				MiniDig=iDig;
			}
			*/
			
			pos.SetXYZ(digTof->GetX(),digTof->GetY(),digTof->GetPlane()*dPlane);
			poslab.SetXYZ(pos.X(),pos.Y(),pos.Z()+14250);
			poslab.RotateZ(5.2*TMath::DegToRad());
			
    			BmnLANDHit *pHit = new ((*TofHit)[TofHit->GetEntriesFast()]) BmnLANDHit(digiTof->GetPlane(), digiTof->GetBar(), poslab, digiTof->GetTime(), digiTof->GetEnergy);
			pHit->SetTimeStamp(digiTof->GetTime());
			pHit->SetEnergy(digiTof->GetEnergy());
			}
		
	}



	clock_t tFinish = clock();
	workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

	if (fVerbose) cout<<"LAND  single hits= "<<nSingleHits<<", double hits= "<<nDoubleHits<<", final hits= "<<nFinally<<endl;
	if (fVerbose) cout << "======================== LAND exec finished ====================" << endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnLANDHitProducer::Finish() {
	if (fDoTest) {
		FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[BmnLANDHitProducer::Finish] Update  %s file. ", fTestFlnm.Data());
		TFile *ptr = gFile;
		TFile file(fTestFlnm.Data(), "RECREATE");
		fList.Write();
		file.Close();
		gFile = ptr;
		if (!fUseMCData) 
			for (Int_t i = 0; i < fNDetectors; i++)
				pDetector[i] -> SaveHistToFile(fTestFlnm.Data());
	}

	cout << "Work time of the LAND hit finder: " << workTime << endl;
}

