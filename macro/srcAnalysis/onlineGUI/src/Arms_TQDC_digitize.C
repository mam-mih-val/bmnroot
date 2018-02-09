#include "TVector3.h"
#include <time.h>
void Arms_TQDC_digitize(TString file, TString outFileName, int startEvent, int stopEvent){

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
	
	TH1D * QDC_X1_left = new TH1D("QDC_X1_left","",45,-5000,40000);
	TH1D * QDC_X1_right = new TH1D("QDC_X1_right","",45,-5000,40000);
	TH1D * QDC_X2_left = new TH1D("QDC_X2_left","",45,-5000,40000);
	TH1D * QDC_X2_right = new TH1D("QDC_X2_right","",45,-5000,40000);

	TH1D * QDC_Y1_left = new TH1D("QDC_Y1_left","",45,-5000,40000);
	TH1D * QDC_Y1_right = new TH1D("QDC_Y1_right","",45,-5000,40000);
	TH1D * QDC_Y2_left = new TH1D("QDC_Y2_left","",45,-5000,40000);
	TH1D * QDC_Y2_right = new TH1D("QDC_Y2_right","",45,-5000,40000);

	TH1D * ADC_X1_left = new TH1D("ADC_X1_left","",82,0,8200);
	TH1D * ADC_X1_right = new TH1D("ADC_X1_right","",82,0,8200);
	TH1D * ADC_X2_left = new TH1D("ADC_X2_left","",82,0,8200);
	TH1D * ADC_X2_right = new TH1D("ADC_X2_right","",82,0,8200);

	TH1D * ADC_Y1_left = new TH1D("ADC_Y1_left","",82,0,8200);
	TH1D * ADC_Y1_right = new TH1D("ADC_Y1_right","",82,0,8200);
	TH1D * ADC_Y2_left = new TH1D("ADC_Y2_left","",82,0,8200);
	TH1D * ADC_Y2_right = new TH1D("ADC_Y2_right","",82,0,8200);
	
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
	
	TClonesArray * X1_left = new TClonesArray("BmnTrigWaveDigit");
	TClonesArray * X1_right = new TClonesArray("BmnTrigWaveDigit");
	TClonesArray * X2_left = new TClonesArray("BmnTrigWaveDigit");
	TClonesArray * X2_right = new TClonesArray("BmnTrigWaveDigit");

	TClonesArray * Y1_left = new TClonesArray("BmnTrigWaveDigit");
	TClonesArray * Y1_right = new TClonesArray("BmnTrigWaveDigit");
	TClonesArray * Y2_left = new TClonesArray("BmnTrigWaveDigit");
	TClonesArray * Y2_right = new TClonesArray("BmnTrigWaveDigit");

	time_t startT = time(NULL);
	time_t endT = time(NULL);

	Int_t event_count = rootTree->GetEntries();
	cout << "Number of events in _digi file = " <<event_count << endl;
	
	rootTree->GetBranch("EventHeader")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("T0")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_X1_Left")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_X1_Right")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_X2_Left")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_X2_Right")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_Y1_Left")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_Y1_Right")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_Y2_Left")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_Y2_Right")->SetAutoDelete(kFALSE);	

	rootTree->SetBranchAddress("EventHeader", &EventHead);
	rootTree->SetBranchAddress("T0", &T0);
	rootTree->SetBranchAddress("TQDC_X1_Left", &X1_left);
	rootTree->SetBranchAddress("TQDC_X1_Right", &X1_right);
	rootTree->SetBranchAddress("TQDC_X2_Left", &X2_left);
	rootTree->SetBranchAddress("TQDC_X2_Right", &X2_right);
	rootTree->SetBranchAddress("TQDC_Y1_Left", &Y1_left);
	rootTree->SetBranchAddress("TQDC_Y1_Right", &Y1_right);
	rootTree->SetBranchAddress("TQDC_Y2_Left", &Y2_left);
	rootTree->SetBranchAddress("TQDC_Y2_Right", &Y2_right);

	if(stopEvent == -1) stopEvent = event_count;
	if(stopEvent > event_count){
		stopEvent = event_count;
		cout << "WARNING: trying drawing more events than in file\n";
		cout << "WARNING: reference and current runs may have different normalization\n";
	}
	// For each entry
	int ifX1L, ifX1R, ifX2L, ifX2R, ifY1L, ifY1R, ifY2L, ifY2R;
	double triggerTime =0.;
	for (Int_t i = startEvent; i < stopEvent; i++){
		if (i%1000==0) cout << "\tWorking on entry " << i << "\n";
		EventHead->Clear();
		T0->Clear();
		X1_left->Clear();
		X1_right->Clear();
		X2_left->Clear();
		X2_right->Clear();
		Y1_left->Clear();
		Y1_right->Clear();
		Y2_left->Clear();
		Y2_right->Clear();
		
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
		triggerTime = 0.;

		BmnTrigWaveDigit * signal_X1_left = (BmnTrigWaveDigit*) X1_left->At(0);
		BmnTrigWaveDigit * signal_X1_right = (BmnTrigWaveDigit*) X1_right->At(0);
		BmnTrigWaveDigit * signal_X2_left = (BmnTrigWaveDigit*) X2_left->At(0);
		BmnTrigWaveDigit * signal_X2_right = (BmnTrigWaveDigit*) X2_right->At(0);
		
		BmnTrigWaveDigit * signal_Y1_left = (BmnTrigWaveDigit*) Y1_left->At(0);
		BmnTrigWaveDigit * signal_Y1_right = (BmnTrigWaveDigit*) Y1_right->At(0);
		BmnTrigWaveDigit * signal_Y2_left = (BmnTrigWaveDigit*) Y2_left->At(0);
		BmnTrigWaveDigit * signal_Y2_right = (BmnTrigWaveDigit*) Y2_right->At(0);
		
		ifX1L = X1_left->GetEntriesFast();
		ifX1R = X1_right->GetEntriesFast();
		ifX2L = X2_left->GetEntriesFast();
		ifX2R = X2_right->GetEntriesFast();
		ifY1L = Y1_left->GetEntriesFast();
		ifY1R = Y1_right->GetEntriesFast();
		ifY2L = Y2_left->GetEntriesFast();
		ifY2R = Y2_right->GetEntriesFast();

		// Getting all TQDC entries
		if (T0->GetEntriesFast() > 0) triggerTime = signal_T0->GetTime();
		if (ifX1L > 0){
			hits_X1_left->Fill( ifX1L );
			QDC_X1_left->Fill( signal_X1_left->GetIntegral() );
			ADC_X1_left->Fill( signal_X1_left->GetPeak() );
			TDC_X1_left->Fill( triggerTime - signal_X1_left->GetTime() );
		}
		if (ifX1R > 0){
			hits_X1_right->Fill( ifX1R );
			QDC_X1_right->Fill( signal_X1_right->GetIntegral() );
			ADC_X1_right->Fill( signal_X1_right->GetPeak() );
			TDC_X1_right->Fill( triggerTime - signal_X1_right->GetTime() );
		}
		if (ifX2L > 0){
			hits_X2_left->Fill( ifX2L );
			QDC_X2_left->Fill( signal_X2_left->GetIntegral() );
			ADC_X2_left->Fill( signal_X2_left->GetPeak() );	
			TDC_X2_left->Fill( triggerTime - signal_X2_left->GetTime() );
		}
		if (ifX2R > 0){
			hits_X2_right->Fill( ifX2R );
			QDC_X2_right->Fill( signal_X2_right->GetIntegral() );
			ADC_X2_right->Fill( signal_X2_right->GetPeak() );
			TDC_X2_right->Fill( triggerTime - signal_X2_right->GetTime() );
		}
		if (ifY1L > 0){
			hits_Y1_left->Fill( ifY1L );
			QDC_Y1_left->Fill( signal_Y1_left->GetIntegral() );
			ADC_Y1_left->Fill( signal_Y1_left->GetPeak() );
			TDC_Y1_left->Fill( triggerTime - signal_Y1_left->GetTime() );
		}
		if (ifY1R > 0){
			hits_Y1_right->Fill( ifY1R );
			QDC_Y1_right->Fill( signal_Y1_right->GetIntegral() );
			ADC_Y1_right->Fill( signal_Y1_right->GetPeak() );
			TDC_Y1_right->Fill( triggerTime - signal_Y1_right->GetTime() );
		}
		if (ifY2L > 0){
			hits_Y2_left->Fill( ifY2L );
			QDC_Y2_left->Fill( signal_Y2_left->GetIntegral() );
			ADC_Y2_left->Fill( signal_Y2_left->GetPeak() );
			TDC_Y2_left->Fill( triggerTime - signal_Y2_left->GetTime() );
		}
		if (ifY2R > 0){
			hits_Y2_right->Fill( ifY2R );
			QDC_Y2_right->Fill( signal_Y2_right->GetIntegral() );
			ADC_Y2_right->Fill( signal_Y2_right->GetPeak() );
			TDC_Y2_right->Fill( triggerTime - signal_Y2_right->GetTime() );
		}

	}
	hits_X1_left->Write();
	hits_X1_right->Write();
	hits_X2_left->Write();
	hits_X2_right->Write();

	hits_Y1_left->Write();
	hits_Y1_right->Write();
	hits_Y2_left->Write();
	hits_Y2_right->Write();
	
	QDC_X1_left->Write();
	QDC_X1_right->Write();
	QDC_X2_left->Write();
	QDC_X2_right->Write();
	
	QDC_Y1_left->Write();
	QDC_Y1_right->Write();
	QDC_Y2_left->Write();
	QDC_Y2_right->Write();

	ADC_X1_left->Write();
	ADC_X1_right->Write();
	ADC_X2_left->Write();
	ADC_X2_right->Write();

	ADC_Y1_left->Write();
	ADC_Y1_right->Write();
	ADC_Y2_left->Write();
	ADC_Y2_right->Write();

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
