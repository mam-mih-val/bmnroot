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

#include "BmnLANDHitProducer.h"

using namespace std;

namespace{
	bool istokenish(int a_c)
	{
		return isalnum(a_c) || '_' == a_c || '.' == a_c;
	}
	
	std::vector<std::string> tokenize(std::string const &a_str)
	{
		std::vector<std::string> result;
		unsigned i = 0;
		for (;;){
			for (;;){
				if (a_str.size() == i) {
					return result;
				}
				if (istokenish(a_str.at(i))){
					break;
				}
					++i;
				}
			unsigned start = i;
			for (; i < a_str.size() && istokenish(a_str.at(i)); ++i);
			result.push_back(a_str.substr(start, i-start));
		}
	}
}

static Float_t workTime = 0.0;

ClassImp(BmnLANDHitProducer)
	//--------------------------------------------------------------------------------------------------------------------------------------
BmnLANDHitProducer::BmnLANDHitProducer(const char *name, Bool_t useMCdata, Int_t verbose, Bool_t test)
	:  FairTask(name,verbose),aExpDigits(nullptr), fOnlyPrimary(false), fUseMCData(false), aLandHits(nullptr){
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
	LOG(info) << "Begin [BmnLANDHitProducer::Init].";

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
	

	aLandHits = new TClonesArray("BmnLANDHit");
	FairRootManager::Instance()->Register("BmnLandHit", "LAND", aLandHits, kTRUE);
	
	if (!fUseMCData) {
		TString vScintMap = "neuland_sync_2.txt";
		SetVelMap(vScintMap);
	}
	

	LOG(info) << "Initialization [BmnLANDHitProducer::Init] finished succesfully.";

	return kSUCCESS;
}

void 		BmnLANDHitProducer::Exec(Option_t* opt) {
	

	
	if (!IsActive())
		return;
	clock_t tStart = clock();

	if (fVerbose) cout << endl << "======================== LAND exec started ====================" << endl;
	

	aLandHits->Clear();

	TVector3 	pos, poslab, dpos; 	
	Int_t nT0Digits = aExpDigitsT0->GetEntriesFast();
	

	float dPlane=10.; //spacing bwt planes tmp
	
	//if (nT0Digits == 1) { // T0 digit should exist and only be len 1
		BmnTrigDigit* digT0 = (BmnTrigDigit*) aExpDigitsT0->At(0);

		for (Int_t iDig = 0; iDig < aExpDigits->GetEntriesFast(); ++iDig) {
			BmnLANDDigit* digLand = (BmnLANDDigit*) aExpDigits->At(iDig);
			
			int p = digLand->GetPlane();
			int b = digLand->GetBar();		
			
			//cout << "output: " << digLand->GetX() << " " << digLand-> GetY() << " " << p << " " << b <<" " << m_vscint[digLand->GetPlane()][digLand->GetBar()].vscint <<" " << digLand->GetTime() << " " << digLand->GetEnergy() << "\n"; 
			pos.SetXYZ(digLand->GetX(),digLand->GetY(),(p*dPlane+5.));
			
			// Assume t res is 500ps for now
			float xerr = m_vscint[p][b].vscint*sqrt(2.0)*0.5;
			float yerr = 10./sqrt(12.);
			float zerr = 10./sqrt(12.);		
				// now need to transform error to lab frame
			float lab_xerr = pow(pow(xerr,2)*pow(TMath::Cos(5.2*TMath::DegToRad()),2) + pow(yerr,2)*pow(TMath::Sin(5.2*TMath::DegToRad()),2),0.5);
			float lab_yerr = pow(pow(xerr,2)*pow(TMath::Sin(5.2*TMath::DegToRad()),2) + pow(yerr,2)*pow(TMath::Cos(5.2*TMath::DegToRad()),2),0.5);

			dpos.SetXYZ(lab_xerr , lab_yerr,zerr);			

			poslab.SetXYZ(pos.X()-130.9,pos.Y(),pos.Z()+1425.0);
			poslab.RotateZ(5.2*TMath::DegToRad());
			
    			BmnLANDHit *pHit = new ((*aLandHits)[aLandHits->GetEntriesFast()]) BmnLANDHit(digLand->GetPlane(), digLand->GetBar(), poslab, dpos,digLand->GetTime(), digLand->GetEnergy());
		
			// TODO: apply slewing correction for T0 time
			//pHit->SetTimeStamp(digLand->GetTime()-digT0->GetTime());
			pHit->SetTimeStamp(digLand->GetTime());
			pHit->SetEnergy(digLand->GetEnergy());
			}
		
	//}



	clock_t tFinish = clock();
	workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

	if (fVerbose) cout << "======================== LAND exec finished ====================" << endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnLANDHitProducer::Finish() {
	/*if (fDoTest) {
		LOG(info) << "[BmnLANDHitProducer::Finish] Update " << fTestFlnm.Data() << " file.";
		TFile *ptr = gFile;
		TFile file(fTestFlnm.Data(), "RECREATE");
		fList.Write();
		file.Close();
		gFile = ptr;
		if (!fUseMCData) 
			for (Int_t i = 0; i < fNDetectors; i++)
				pDetector[i] -> SaveHistToFile(fTestFlnm.Data());
	}*/

	cout << "Work time of the LAND hit finder: " << workTime << endl;
}


bool BmnLANDHitProducer::SetVelMap(TString a_vscint_filename)
{
	auto path = std::string(getenv("VMCWORKDIR")) + "/input/" + a_vscint_filename.Data();
	std::ifstream in(path.c_str());
	if (!in.is_open()){
		std::cerr << " Could not open vscint map for LAND " << path << "\n";
	}
	for (unsigned line_no = 1;; ++line_no){
		std::string line;
		std::getline(in,line);
		if( !in.good() ) break;
		int globbar;
		auto const token = tokenize(line);
		if (token.size() < 4) continue;
		char const *p;
		char *end;
		p = token.at(0).c_str();
		globbar = strtol(p, &end, 10);
		if (end == p || globbar < 1 || globbar > 120) continue;
		--globbar;
		int land_plane = globbar / LAND_BAR_N;
		int land_bar = globbar % LAND_BAR_N;
		m_vscint[land_plane][land_bar].vscint = strtod(token.at(3).c_str(), NULL);
	}
	in.close();	

	return true;
}
