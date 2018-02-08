/*TODO once we have real data files:
1) Make T0 the real trigger time instead of BC1 
2) Do booleans dependent on trigger type
3) Make module efficiency dependent on trigger type
4) Reference and current need to draw same number of triggers 
5) Adjust histogram levels based off of a good run
*/
#include "TVector3.h"
#include <time.h>
void Arms_TDC_digitize(TString file, TString outFileName, int startEvent, int stopEvent){
	
	// this macro will take in a _digi.root file
	// and write out ADC spectra of all our trigger
	// detectors, 2D correlations between PMTs
	// on the same detector, and TDC histograms
	// relative to BC2.

	gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
	bmnloadlibs(); // load BmnRoot libraries
	
	TChain* rootTree = new TChain("cbmsim");
	rootTree->Add(file);

	// Set up out file and histograms to write
	TFile * outFile = new TFile(outFileName,"RECREATE");
	
	TH1D * hits_X1_left = new TH1D("hits_X1_left","",10,0,10);	
	TH1D * hits_X1_right = new TH1D("hits_X1_right","",10,0,10);	
	TH1D * hits_X2_left = new TH1D("hits_X2_left","",10,0,10);	
	TH1D * hits_X2_right = new TH1D("hits_X2_right","",10,0,10);	
	
	TH1D * hits_Y1_left = new TH1D("hits_Y1_left","",10,0,10);	
	TH1D * hits_Y1_right = new TH1D("hits_Y1_right","",10,0,10);	
	TH1D * hits_Y2_left = new TH1D("hits_Y2_left","",10,0,10);	
	TH1D * hits_Y2_right = new TH1D("hits_Y2_right","",10,0,10);	

	TH1D * TDC_X1_left = new TH1D("TDC_X1_left","",200,-1e3,1e3);
	TH1D * TDC_X1_right = new TH1D("TDC_X1_right","",200,-1e3,1e3);
	TH1D * TDC_X2_left = new TH1D("TDC_X2_left","",200,-1e3,1e3);
	TH1D * TDC_X2_right = new TH1D("TDC_X2_right","",200,-1e3,1e3);
		
	TH1D * TDC_Y1_left = new TH1D("TDC_Y1_left","",200,-1e3,1e3);
	TH1D * TDC_Y1_right = new TH1D("TDC_Y1_right","",200,-1e3,1e3);
	TH1D * TDC_Y2_left = new TH1D("TDC_Y2_left","",200,-1e3,1e3);
	TH1D * TDC_Y2_right = new TH1D("TDC_Y2_right","",200,-1e3,1e3);

	TClonesArray * EventHead = new TClonesArray("BmnEventHeader");
	TClonesArray * T0 = new TClonesArray("BmnTrigDigit");
	
	//TClonesArray * X1_left = new TClonesArray("BmnTrigDigit");
	//TClonesArray * X1_right = new TClonesArray("BmnTrigDigit");
	//TClonesArray * X2_left = new TClonesArray("BmnTrigDigit");
	//TClonesArray * X2_right = new TClonesArray("BmnTrigDigit");

	//TClonesArray * Y1_left = new TClonesArray("BmnTrigDigit");
	//TClonesArray * Y1_right = new TClonesArray("BmnTrigDigit");
	//TClonesArray * Y2_left = new TClonesArray("BmnTrigDigit");
	//TClonesArray * Y2_right = new TClonesArray("BmnTrigDigit");

	time_t startT = time(NULL);
	time_t endT = time(NULL);

	Int_t event_count = rootTree->GetEntries();
	cout << "Number of events in _digi file = " <<event_count << endl;
	
	rootTree->GetBranch("EventHeader")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("T0")->SetAutoDelete(kFALSE);

	//rootTree->GetBranch("X1_Left")->SetAutoDelete(kFALSE);
	//rootTree->GetBranch("X1_Right")->SetAutoDelete(kFALSE);
	//rootTree->GetBranch("X2_Left")->SetAutoDelete(kFALSE);
	//rootTree->GetBranch("X2_Right")->SetAutoDelete(kFALSE);
	//rootTree->GetBranch("Y1_Left")->SetAutoDelete(kFALSE);
	//rootTree->GetBranch("Y1_Right")->SetAutoDelete(kFALSE);
	//rootTree->GetBranch("Y2_Left")->SetAutoDelete(kFALSE);
	//rootTree->GetBranch("Y2_Right")->SetAutoDelete(kFALSE);	

	rootTree->SetBranchAddress("EventHeader", &EventHead);
	rootTree->SetBranchAddress("T0", &T0);

	//rootTree->SetBranchAddress("X1_Left", &X1_left);
	//rootTree->SetBranchAddress("X1_Right", &X1_right);
	//rootTree->SetBranchAddress("X2_Left", &X2_left);
	//rootTree->SetBranchAddress("X2_Right", &X2_right);
	//rootTree->SetBranchAddress("Y1_Left", &Y1_left);
	//rootTree->SetBranchAddress("Y1_Right", &Y1_right);
	//rootTree->SetBranchAddress("Y2_Left", &Y2_left);
	//rootTree->SetBranchAddress("Y2_Right", &Y2_right);

	double TQDC_eff = 0;
	if(stopEvent == -1) stopEvent = event_count;
	if(stopEvent > event_count){
                stopEvent = event_count;
                cout << "WARNING: trying drawing more events than in file\n";
                cout << "WARNING: reference and current runs may have different normalization\n";
        }
	// For each entry
	for (Int_t i = startEvent; i < stopEvent; i++){
		if (i%1000==0) cout << "\tWorking on entry " << i << "\n";
		EventHead->Clear();
		T0->Clear();
		//X1_left->Clear();
		//X1_right->Clear();
		//X2_left->Clear();
		//X2_right->Clear();
		//Y1_left->Clear();
		//Y1_right->Clear();
		//Y2_left->Clear();
		//Y2_right->Clear();
		
		rootTree->GetEntry(i);
		
		// For each HIT in the entry window (i.e. can have multiple hits within
		// one event b/c it's multihit TDC/ADC)

		// but let's only take the first one...
		BmnEventHeader * head = (BmnEventHeader*) EventHead->At(0);
		TDatime time = head->GetEventTime();
		if (i==startEvent){
			startT = time.Convert();
		}
		else if(i==stopEvent-1){
			endT = time.Convert();
		} 
		BmnTrigDigit * signal_T0 = (BmnTrigDigit*) T0->At(0);
		double triggerTime = 0.;

		//BmnTrigDigit * signal_X1_left = (BmnTrigDigit*) X1_left->At(0);
		//BmnTrigDigit * signal_X1_right = (BmnTrigDigit*) X1_right->At(0);
		//BmnTrigDigit * signal_X2_left = (BmnTrigDigit*) X2_left->At(0);
		//BmnTrigDigit * signal_X2_right = (BmnTrigDigit*) X2_right->At(0);
		
		//BmnTrigDigit * signal_Y1_left = (BmnTrigDigit*) Y1_left->At(0);
		//BmnTrigDigit * signal_Y1_right = (BmnTrigDigit*) Y1_right->At(0);
		//BmnTrigDigit * signal_Y2_left = (BmnTrigDigit*) Y2_left->At(0);
		//BmnTrigDigit * signal_Y2_right = (BmnTrigDigit*) Y2_right->At(0);
		
		int ifT0 = T0->GetEntriesFast();
                //int ifX1L = X1_left->GetEntriesFast();
                //int ifX1R = X1_right->GetEntriesFast();
                //int ifX2L = X2_left->GetEntriesFast();
                //int ifX2R = X2_right->GetEntriesFast();
                //int ifY1L = Y1_left->GetEntriesFast();
                //int ifY1R = Y1_right->GetEntriesFast();
                //int ifY2L = Y2_left->GetEntriesFast();
                //int ifY2R = Y2_right->GetEntriesFast(); 

		// Getting all TQDC entries
		if (ifT0 > 0) triggerTime = signal_T0->GetTime();
		//if (ifX1L > 0){
		//	hits_X1_left->Fill( ifX1L );
		//	TDC_X1_left->Fill( triggerTime - signal_X1_left->GetTime() );
		//}
		//if (ifX1R > 0){
		//	hits_X1_right->Fill( ifX1R );
		//	TDC_X1_right->Fill( triggerTime - signal_X1_right->GetTime() );
		//}
		//if (ifX2L > 0){
		//	hits_X2_left->Fill( ifX2L );
		//	TDC_X2_left->Fill( triggerTime - signal_X2_left->GetTime() );
		//}
		//if (ifX2R > 0){
		//	hits_X2_right->Fill( ifX2R );
		//	TDC_X2_right->Fill( triggerTime - signal_X2_right->GetTime() );
		//}
		//if (ifY1L > 0){
		//	hits_Y1_left->Fill( ifY1L );
		//	TDC_Y1_left->Fill( triggerTime - signal_Y1_left->GetTime() );
		//}
		//if (ifY1R > 0){
		//	hits_Y1_right->Fill( ifY1R );
		//	TDC_Y1_right->Fill( triggerTime - signal_Y1_right->GetTime() );
		//}
		//if (ifY2L > 0){
		//	hits_Y2_left->Fill( ifY2L );
		//	TDC_Y2_left->Fill( triggerTime - signal_Y2_left->GetTime() );
		//}
		//if (ifY2R > 0){
		//	hits_Y2_right->Fill( ifY2R );
		//	TDC_Y2_right->Fill( triggerTime - signal_Y2_right->GetTime() );
		//}
	}
	hits_X1_left->Write();
	hits_X1_right->Write();
	hits_X2_left->Write();
	hits_X2_right->Write();

	hits_Y1_left->Write();
	hits_Y1_right->Write();
	hits_Y2_left->Write();
	hits_Y2_right->Write();
	
	TDC_X1_left->Write();
	TDC_X1_right->Write();
	TDC_X2_left->Write();
	TDC_X2_right->Write();

	TDC_Y1_left->Write();
	TDC_Y1_right->Write();
	TDC_Y2_left->Write();
	TDC_Y2_right->Write();

	outFile->Close();
	

	
	
	

}
