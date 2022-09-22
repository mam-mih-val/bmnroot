#include "BmnSRCTriggersCheck.h"
#include "BmnTrigWaveDigit.h"
BmnSRCTriggersCheck::BmnSRCTriggersCheck(Bool_t isExp) {
	fIsExp = isExp;

	fT0Branch = "T0";
	fVetoBranch = "VETO";
	//fBC2Branch = "BC2";
	fBC1Branch = "TQDC_BC1";
	fBC2Branch = "TQDC_BC2";
	fBC3Branch = "TQDC_BC3";
	fX1LBranch = "TQDC_X1_Left";
	fX2LBranch = "TQDC_X2_Left";
	fX1RBranch = "TQDC_X1_Right";
	fX2RBranch = "TQDC_X2_Right";
	fY1LBranch = "TQDC_Y1_Left";
	fY2LBranch = "TQDC_Y2_Left";
	fY1RBranch = "TQDC_Y1_Right";
	fY2RBranch = "TQDC_Y2_Right";
	fBDBranch = "BD";
	fBmnEventHeaderBranchName = "EventHeader";
	fBmnEvQualityBranchName = "srcTriggers";
}

InitStatus BmnSRCTriggersCheck::Init() { 
	FairRootManager* ioman = FairRootManager::Instance();

	fBmnEventHeader = (TClonesArray*) ioman->GetObject(fBmnEventHeaderBranchName);
	fT0Array = (TClonesArray*) ioman->GetObject(fT0Branch.Data());
	fVetoArray = (TClonesArray*) ioman->GetObject(fVetoBranch.Data());
	fBC1Array = (TClonesArray*) ioman->GetObject(fBC2Branch.Data());
	fBC2Array = (TClonesArray*) ioman->GetObject(fBC2Branch.Data());
	fBC3Array = (TClonesArray*) ioman->GetObject(fBC3Branch.Data());
	fX1RArray = (TClonesArray*) ioman->GetObject(fX1RBranch.Data());
	fX1LArray = (TClonesArray*) ioman->GetObject(fX1LBranch.Data());
	fX2RArray = (TClonesArray*) ioman->GetObject(fX2RBranch.Data());
	fX2LArray = (TClonesArray*) ioman->GetObject(fX2LBranch.Data());
	fY1RArray = (TClonesArray*) ioman->GetObject(fY1RBranch.Data());
	fY1LArray = (TClonesArray*) ioman->GetObject(fY1LBranch.Data());
	fY2RArray = (TClonesArray*) ioman->GetObject(fY2RBranch.Data());
	fY2LArray = (TClonesArray*) ioman->GetObject(fY2LBranch.Data());
	fBDArray = (TClonesArray*) ioman->GetObject(fBDBranch.Data());

	fBmnEvQuality = new TClonesArray(fBmnEvQualityBranchName);
	ioman->Register(fBmnEvQualityBranchName, "QUALITY", fBmnEvQuality, true);
}

void BmnSRCTriggersCheck::Exec(Option_t* opt) {
	if (!fIsExp)
		return;

	fBmnEvQuality->Delete();

	BmnEventQuality* evQual = new ((*fBmnEvQuality)[fBmnEvQuality->GetEntriesFast()]) BmnEventQuality("GOOD");

	const Int_t kEnergies = 4;
	const Int_t kTargets = 5;
	Double_t energies[kEnergies] = {3.5, 4., 4.5, 5.14};
	TString targets[kTargets] = {"C", "Al", "Cu", "Pb", "C2H4"};

	BmnEventHeader* evHeader = (BmnEventHeader*) fBmnEventHeader->UncheckedAt(0);
	if (!evHeader)
		return;

	UniRun* runInfo = UniRun::GetRun(6, evHeader->GetRunId());

	BmnTriggerType trigType = evHeader->GetTrig();
	if (trigType == kBMNMINBIAS) {
		// Setup BD-threshold
		// To be read subsequently from the UniDB directly when processing RUN7, FIXME
		TString target = *runInfo->GetTargetParticle();
		Double_t energy = *runInfo->GetEnergy();
		Int_t bdThresh;
		if (Abs(energy - energies[0]) < FLT_EPSILON) // 3.5 GeV/n, all targets
			bdThresh = 2;
		else if (Abs(energy - energies[1]) < FLT_EPSILON && (target == targets[0] || target == targets[4])) // 4 GeV/n, C, C2H4
			bdThresh = 2;
		else if (Abs(energy - energies[1]) < FLT_EPSILON && target != targets[0] && target != targets[4]) // 4 GeV/n, Al, Cu, Pb
			bdThresh = 3;
		else if (Abs(energy - energies[2]) < FLT_EPSILON && (target == targets[0] || target == targets[4])) // 4.5 GeV/n, C, C2H4
			bdThresh = 2;
		else if (Abs(energy - energies[2]) < FLT_EPSILON && target != targets[0] && target != targets[4]) // 4 GeV/n, Al, Cu, Pb
			bdThresh = 3;
		else if (Abs(energy - energies[3]) < FLT_EPSILON) // 5.14 GeV/n, all targets
			bdThresh = 2;
		else
			bdThresh = 0;             

		if (fT0Array->GetEntriesFast() != 1 || fBC2Array->GetEntriesFast() != 1 || fVetoArray->GetEntriesFast() != 0 || fBDArray->GetEntriesFast() < bdThresh)
			evQual->SetIsGoodEvent("BAD");
	}

	else if (trigType == kBMNBEAM) {
		if (fVetoArray->GetEntriesFast() > 1 || fBDArray->GetEntriesFast() > 0)
			evQual->SetIsGoodEvent("BAD");
	}

	else
		evQual->SetIsGoodEvent("BAD");

	vector<int> eloss;
    	for (Int_t iDig = 0; iDig < fBC1Array->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fBC1Array->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossBC1(eloss);

	eloss.clear();
    	for (Int_t iDig = 0; iDig < fBC2Array->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fBC2Array->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossBC2(eloss);

	eloss.clear();
    	for (Int_t iDig = 0; iDig < fBC3Array->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fBC3Array->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossBC3(eloss);

	eloss.clear();
    	for (Int_t iDig = 0; iDig < fX1RArray->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fX1RArray->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossX1R(eloss);

	eloss.clear();
    	for (Int_t iDig = 0; iDig < fX1LArray->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fX1LArray->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossX1L(eloss);

	eloss.clear();
    	for (Int_t iDig = 0; iDig < fY1RArray->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fY1RArray->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossY1R(eloss);

	eloss.clear();
    	for (Int_t iDig = 0; iDig < fY1LArray->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fY1LArray->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossY1L(eloss);
	
	eloss.clear();
    	for (Int_t iDig = 0; iDig < fX2RArray->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fX2RArray->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossX2R(eloss);

	eloss.clear();
    	for (Int_t iDig = 0; iDig < fX2LArray->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fX2LArray->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossX2L(eloss);

	eloss.clear();
    	for (Int_t iDig = 0; iDig < fY2RArray->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fY2RArray->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossY2R(eloss);

	eloss.clear();
    	for (Int_t iDig = 0; iDig < fY2LArray->GetEntriesFast(); ++iDig) {        
	        BmnTrigWaveDigit *dig = (BmnTrigWaveDigit*) fY2LArray->At(iDig);
		eloss.push_back(dig->GetIntegral());
	}
	evQual->SetElossY2L(eloss);





}

void BmnSRCTriggersCheck::Finish() {

}

ClassImp(BmnSRCTriggersCheck);
