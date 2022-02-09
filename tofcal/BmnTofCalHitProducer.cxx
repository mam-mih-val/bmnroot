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

#include "BmnTofCalHitProducer.h"

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

ClassImp(BmnTofCalHitProducer)
	//--------------------------------------------------------------------------------------------------------------------------------------
BmnTofCalHitProducer::BmnTofCalHitProducer(const char *name, Bool_t useMCdata, Int_t verbose, Bool_t test)
	:  FairTask(name,verbose),aExpDigits(nullptr), fOnlyPrimary(false), fUseMCData(false), aTofcalHits(nullptr){
		//	pGeoUtils = new BmnTof1GeoUtils;

	}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTofCalHitProducer::~BmnTofCalHitProducer() 
{
	//delete pGeoUtils;
}
//--------------------------------------------------------------------------------------------------------------------------------------
InitStatus 		BmnTofCalHitProducer::Init() 
{
	LOG(info) << "Begin [BmnTofCalHitProducer::Init].";

	if(fOnlyPrimary) cout<<" Only primary particles are processed!!! \n"; // FIXME NOT used now ADDD

	aExpDigits = (TClonesArray*) FairRootManager::Instance()->GetObject("TofCal");
	if (!aExpDigits)
	{
		cout<<"BmnTofCalHitProducer::Init(): branch TofCal not found! Task will be deactivated"<<endl;
		SetActive(kFALSE);
		return kERROR;
	}
	aExpDigitsT0 = (TClonesArray*) FairRootManager::Instance()->GetObject("T0");
	if (!aExpDigitsT0)
	{
		cout<<"BmnTofCalHitProducer::Init(): branch T0 not found! Task will be deactivated"<<endl;
		SetActive(kFALSE);
		return kERROR;
	}
	

	aTofcalHits = new TClonesArray("BmnTofCalHit");
	FairRootManager::Instance()->Register("BmnTofcalHit", "TofCal", aTofcalHits, kTRUE);
	
	if (!fUseMCData) {
		TString vScintMap = "neuland_sync_2.txt";
		SetVelMap(vScintMap);
	}
	

	LOG(info) << "Initialization [BmnTofCalHitProducer::Init] finished succesfully.";

	return kSUCCESS;
}

void 		BmnTofCalHitProducer::Exec(Option_t* opt) {
	

	
	if (!IsActive())
		return;
	clock_t tStart = clock();

	if (fVerbose) cout << endl << "======================== TofCal exec started ====================" << endl;
	

	aTofcalHits->Clear();

	TVector3 	pos, poslab, dpos; 	
	Int_t nT0Digits = aExpDigitsT0->GetEntriesFast();
	

	float dPlane=10.; //spacing bwt planes tmp
	
	//if (nT0Digits == 1) { // T0 digit should exist and only be len 1
		BmnTrigDigit* digT0 = (BmnTrigDigit*) aExpDigitsT0->At(0);

		for (Int_t iDig = 0; iDig < aExpDigits->GetEntriesFast(); ++iDig) {
			BmnTofCalDigit* digTofcal = (BmnTofCalDigit*) aExpDigits->At(iDig);
			
			int a = digTofcal->GetArm();
			int p = digTofcal->GetPlane();
			int b = digTofcal->GetBar();		
			
			//cout << "output: " << digTofcal->GetX() << " " << digTofcal-> GetY() << " " << p << " " << b <<" " << m_vscint[digTofcal->GetPlane()][digTofcal->GetBar()].vscint <<" " << digTofcal->GetTime() << " " << digTofcal->GetEnergy() << "\n"; 
			pos.SetXYZ(digTofcal->GetX(),digTofcal->GetY(),(p*dPlane+5.));
			
			// Assume t res is 500ps for now
			float xerr = m_vscint[a][p][b].vscint*sqrt(2.0)*0.5;
			float yerr = 10./sqrt(12.);
			float zerr = 10./sqrt(12.);		
				// now need to transform error to lab frame
			float lab_xerr = pow(pow(xerr,2)*pow(TMath::Cos(5.2*TMath::DegToRad()),2) + pow(yerr,2)*pow(TMath::Sin(5.2*TMath::DegToRad()),2),0.5);
			float lab_yerr = pow(pow(xerr,2)*pow(TMath::Sin(5.2*TMath::DegToRad()),2) + pow(yerr,2)*pow(TMath::Cos(5.2*TMath::DegToRad()),2),0.5);

			dpos.SetXYZ(lab_xerr , lab_yerr,zerr);			

			poslab.SetXYZ(pos.X()-130.9,pos.Y(),pos.Z()+1425.0);
			poslab.RotateZ(5.2*TMath::DegToRad());
			
    			BmnTofCalHit *pHit = new ((*aTofcalHits)[aTofcalHits->GetEntriesFast()]) BmnTofCalHit(digTofcal->GetPlane(), digTofcal->GetBar(), poslab, dpos,digTofcal->GetTime(), digTofcal->GetEnergy());
		
			// TODO: apply slewing correction for T0 time
			//pHit->SetTimeStamp(digTofcal->GetTime()-digT0->GetTime());
			pHit->SetTimeStamp(digTofcal->GetTime());
			pHit->SetEnergy(digTofcal->GetEnergy());
			}
		
	//}



	clock_t tFinish = clock();
	workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

	if (fVerbose) cout << "======================== TofCal exec finished ====================" << endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTofCalHitProducer::Finish() {
	/*if (fDoTest) {
		LOG(info) << "[BmnTofCalHitProducer::Finish] Update " << fTestFlnm.Data() << " file.";
		TFile *ptr = gFile;
		TFile file(fTestFlnm.Data(), "RECREATE");
		fList.Write();
		file.Close();
		gFile = ptr;
		if (!fUseMCData) 
			for (Int_t i = 0; i < fNDetectors; i++)
				pDetector[i] -> SaveHistToFile(fTestFlnm.Data());
	}*/

	cout << "Work time of the TofCal hit finder: " << workTime << endl;
}


bool BmnTofCalHitProducer::SetVelMap(TString a_vscint_filename)
{
	auto path = std::string(getenv("VMCWORKDIR")) + "/input/" + a_vscint_filename.Data();
	std::ifstream in(path.c_str());
	if (!in.is_open()){
		std::cerr << " Could not open vscint map for TofCal " << path << "\n";
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
		int tofcal_arm = (globbar < 60) ? 0 : 1;
		int tofcal_plane = globbar / TOFCAL_BAR_N;
		int tofcal_bar = globbar % TOFCAL_BAR_N;
		m_vscint[tofcal_arm][tofcal_plane][tofcal_bar].vscint = strtod(token.at(3).c_str(), NULL);
	}
	in.close();	

	return true;
}
