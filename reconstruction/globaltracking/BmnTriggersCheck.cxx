#include "BmnTriggersCheck.h"
#include "BmnTrigDigit.h"

BmnTriggersCheck::BmnTriggersCheck(Bool_t isExp, Int_t runP, Int_t runN) {
    fIsExp = isExp;
    fRunPeriod = runP;
    fRunNumber = runN;
    
    fT0Branch = "T0";
    fVetoBranch = "VC";
    fBC2Branch = "BC2";
    fBC1Branch = "BC1";
    fBC3Branch = "BC3";
    fBDBranch = "BD";
    fBmnEventHeaderBranchName = "EventHeader";
    fBmnEvQualityBranchName = "BmnEventQuality";
}

InitStatus BmnTriggersCheck::Init() { 
    FairRootManager* ioman = FairRootManager::Instance();
      
    fBmnEventHeader = (TClonesArray*) ioman->GetObject(fBmnEventHeaderBranchName);
    fT0Array = (TClonesArray*) ioman->GetObject(fT0Branch.Data());
    fVetoArray = (TClonesArray*) ioman->GetObject(fVetoBranch.Data());
    fBC2Array = (TClonesArray*) ioman->GetObject(fBC2Branch.Data());
    fBDArray = (TClonesArray*) ioman->GetObject(fBDBranch.Data());
    fBC1Array = (TClonesArray*) ioman->GetObject(fBC1Branch.Data());
    fBC3Array = (TClonesArray*) ioman->GetObject(fBC3Branch.Data());
    
    fBmnEvQuality = new TClonesArray(fBmnEvQualityBranchName);
    ioman->Register(fBmnEvQualityBranchName, "QUALITY", fBmnEvQuality, true);

    return kSUCCESS;
}

void BmnTriggersCheck::Exec(Option_t* opt) {
    if (!fIsExp)
        return;
    
    fBmnEvQuality->Delete();

    BmnEventQuality* evQual = new ((*fBmnEvQuality)[fBmnEvQuality->GetEntriesFast()]) BmnEventQuality("GOOD");

    Int_t cntr2 = 0; Bool_t time2 = 0;
	for (Int_t iDig = 0; iDig < fBC2Array->GetEntriesFast(); ++iDig) {
	  BmnTrigDigit *dig = (BmnTrigDigit*) fBC2Array->At(iDig);
	  if (dig->GetMod() == 0 ) {
	    cntr2++; 
	    //  if (dig->GetTime() < 420 || dig->GetTime() > 600) time2 =1;
	    // cout<<" time BC2 "<<dig->GetTime()<<" time2 "<<time2<<endl;
	  }
	}
	if (cntr2 > 2 || time2 == 1)
	  evQual->SetIsGoodEvent("BAD");

	Int_t cntr1 = 0;  Bool_t time1 = 0;
	for (Int_t iDig = 0; iDig < fBC1Array->GetEntriesFast(); ++iDig) {
	  BmnTrigDigit *dig = (BmnTrigDigit*) fBC1Array->At(iDig);
	  if (dig->GetMod() == 0){
	    cntr1++;
	    // if (dig->GetTime() < 460 ||  dig->GetTime() > 600) time1 =1;
	    // cout<<" time BC1 "<<dig->GetTime()<<" time1 "<<time1<<endl;
	  }
	}
	if (cntr1 >2 || time1 == 1)
	  evQual->SetIsGoodEvent("BAD");

	 Int_t cntr3 = 0; Bool_t time3 = 0;
	 for (Int_t iDig = 0; iDig < fBC3Array->GetEntriesFast(); ++iDig) {
	   BmnTrigDigit *dig = (BmnTrigDigit*) fBC3Array->At(iDig);
	   if (dig->GetMod() == 0 ){
	     cntr1++;
	     //  if (dig->GetTime() < 470 || dig->GetTime() > 620) time3 =1;
	     // cout<<" time BC3 "<<dig->GetTime()<<" time3 "<<time3<<endl;
	   }
	 }
	 if (cntr3 > 2 || time3 == 1)
         evQual->SetIsGoodEvent("BAD");

	 // if (fVetoArray != NULL){
         Int_t cntrv = 0;
        for (Int_t iDig = 0; iDig < fVetoArray->GetEntriesFast(); ++iDig) {
	   BmnTrigDigit *dig = (BmnTrigDigit*) fVetoArray->At(iDig);
	   if (dig->GetMod() == 0)
	     cntrv++; 	   
	 }
        if (cntrv >= 1)
	   evQual->SetIsGoodEvent("BAD");

	// } else cout<<" fVetoArray not found "<<endl;

   //  BmnEventQuality* evQual = new ((*fBmnEvQuality)[fBmnEvQuality->GetEntriesFast()]) BmnEventQuality("GOOD");
        
//     const Int_t kEnergies = 4;
//     const Int_t kTargets = 5;
//     Double_t energies[kEnergies] = {3.5, 4., 4.5, 5.14};
//     TString targets[kTargets] = {"C", "Al", "Cu", "Pb", "C2H4"};
   
//     BmnEventHeader* evHeader = (BmnEventHeader*) fBmnEventHeader->UncheckedAt(0);
//     if (!evHeader)
//         return;
              
//     UniRun* runInfo = UniRun::GetRun(fRunPeriod, fRunNumber);

//     BmnTriggerType trigType = evHeader->GetTrig(); 
//     if (trigType == kBMNMINBIAS) {
//         // Setup BD-threshold
//         // To be read subsequently from the UniDB directly when processing RUN7, FIXME
//         TString target = *runInfo->GetTargetParticle();
//         Double_t energy = *runInfo->GetEnergy();
//         Int_t bdThresh;
//         if (Abs(energy - energies[0]) < FLT_EPSILON) // 3.5 GeV/n, all targets
//             bdThresh = 2;
//         else if (Abs(energy - energies[1]) < FLT_EPSILON && (target == targets[0] || target == targets[4])) // 4 GeV/n, C, C2H4
//             bdThresh = 2;
//         else if (Abs(energy - energies[1]) < FLT_EPSILON && target != targets[0] && target != targets[4]) // 4 GeV/n, Al, Cu, Pb
//             bdThresh = 3;
//         else if (Abs(energy - energies[2]) < FLT_EPSILON && (target == targets[0] || target == targets[4])) // 4.5 GeV/n, C, C2H4
//             bdThresh = 2;
//         else if (Abs(energy - energies[2]) < FLT_EPSILON && target != targets[0] && target != targets[4]) // 4 GeV/n, Al, Cu, Pb
//             bdThresh = 3;
//         else if (Abs(energy - energies[3]) < FLT_EPSILON) // 5.14 GeV/n, all targets
//             bdThresh = 2;
//         else
//             bdThresh = 0;             
        
//         if (fT0Array->GetEntriesFast() != 1 || fBC2Array->GetEntriesFast() != 1 || fVetoArray->GetEntriesFast() != 0 || fBDArray->GetEntriesFast() < bdThresh)
//             evQual->SetIsGoodEvent("BAD");        
//     }

//     else if (trigType == kBMNBEAM) {
//         if (fVetoArray->GetEntriesFast() > 1 || fBDArray->GetEntriesFast() > 0)
//             evQual->SetIsGoodEvent("BAD");
//     }

//     else
//         evQual->SetIsGoodEvent("BAD");
}

void BmnTriggersCheck::Finish() {
 
}

ClassImp(BmnTriggersCheck);
