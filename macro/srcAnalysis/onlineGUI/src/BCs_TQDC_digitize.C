#include "TVector3.h"
#include <time.h>
void BCs_TQDC_digitize(TString file, TString outFileName, int startEvent, int stopEvent){

	gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
	bmnloadlibs(); // load BmnRoot libraries
	
	TChain* rootTree = new TChain("cbmsim");
	rootTree->Add(file);

	// Set up out file and histograms to write
	TFile * outFile = new TFile(outFileName,"RECREATE");
	
	TH1D * hits_BC1 = new TH1D("hits_BC1","",10,0,10);
	TH1D * hits_BC2 = new TH1D("hits_BC2","",10,0,10);
	TH1D * hits_BC3 = new TH1D("hits_BC3","",10,0,10);
	TH1D * hits_VC = new TH1D("hits_VC","",10,0,10);
	
	TH1D * QDC_BC1 = new TH1D("QDC_BC1","",45,-5000,40000);
	TH1D * QDC_BC2 = new TH1D("QDC_BC2","",45,-5000,40000);
	TH1D * QDC_BC3 = new TH1D("QDC_BC3","",45,-5000,40000);
	TH1D * QDC_VC  = new TH1D("QDC_VC" ,"",45,-5000,40000);	

	TH1D * ADC_BC1 = new TH1D("ADC_BC1","",82,0,8200);
	TH1D * ADC_BC2 = new TH1D("ADC_BC2","",82,0,8200);
	TH1D * ADC_BC3 = new TH1D("ADC_BC3","",82,0,8200);
	TH1D * ADC_VC  = new TH1D("ADC_VC", "",82,0,8200);

	TH1D * TDC_BC1 = new TH1D("TDC_BC1","",200,-1e3,1e3);
	TH1D * TDC_BC2 = new TH1D("TDC_BC2","",200,-1e3,1e3);
	TH1D * TDC_BC3 = new TH1D("TDC_BC3","",200,-1e3,1e3);
	TH1D * TDC_VC  = new TH1D("TDC_VC" ,"",200,-1e3,1e3);

	TH1D * moduleEff = new TH1D("moduleEff","",2,0,2);
	TH1D * trigScalars = new TH1D("trigScalars","",12,0,12);
	TH1D * coinScalars = new TH1D("coinScalars","",11,0,11);

	TClonesArray * EventHead = new TClonesArray("BmnEventHeader");
	TClonesArray * T0 = new TClonesArray("BmnTrigDigit");
	TClonesArray * BC1 = new TClonesArray("BmnTrigWaveDigit");
	TClonesArray * BC2 = new TClonesArray("BmnTrigWaveDigit");
	TClonesArray * BC3 = new TClonesArray("BmnTrigWaveDigit");
	TClonesArray * VC = new TClonesArray("BmnTrigWaveDigit");
	
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
	rootTree->GetBranch("TQDC_BC1")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_BC2")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_BC3")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TQDC_VC")->SetAutoDelete(kFALSE);

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
	rootTree->SetBranchAddress("TQDC_BC1", &BC1);
	rootTree->SetBranchAddress("TQDC_BC2", &BC2);
	rootTree->SetBranchAddress("TQDC_BC3", &BC3);
	rootTree->SetBranchAddress("TQDC_VC", &VC);

	rootTree->SetBranchAddress("TQDC_X1_Left", &X1_left);
	rootTree->SetBranchAddress("TQDC_X1_Right", &X1_right);
	rootTree->SetBranchAddress("TQDC_X2_Left", &X2_left);
	rootTree->SetBranchAddress("TQDC_X2_Right", &X2_right);
	rootTree->SetBranchAddress("TQDC_Y1_Left", &Y1_left);
	rootTree->SetBranchAddress("TQDC_Y1_Right", &Y1_right);
	rootTree->SetBranchAddress("TQDC_Y2_Left", &Y2_left);
	rootTree->SetBranchAddress("TQDC_Y2_Right", &Y2_right);

	double TQDC_eff = 0;
	if(stopEvent == -1) stopEvent = event_count;
	if(stopEvent > event_count){
		stopEvent = event_count;
		cout << "WARNING: trying drawing more events than in file\n";
		cout << "WARNING: reference and current runs may have different normalization\n";
	}
	// For each entry
	double triggerTime = 0.;
	int ifBC1, ifBC2, ifBC3, ifBC4, ifVC;
	int ifX1L, ifX1R, ifX2L, ifX2R, ifY1L, ifY1R, ifY2L, ifY2R;
	bool beam, X1, X2, Y1, Y2, leftArm, rightArm, pair, trig;
	for (Int_t i = startEvent; i < stopEvent; i++){
		if (i%1000==0) cout << "\tWorking on entry " << i << "\n";
		EventHead->Clear();
		T0->Clear();
		BC1->Clear();
		BC2->Clear();
		BC3->Clear();
		VC->Clear();
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
		triggerTime = 0;		

		BmnTrigWaveDigit * signal_BC1 = (BmnTrigWaveDigit*) BC1->At(0);
		BmnTrigWaveDigit * signal_BC2 = (BmnTrigWaveDigit*) BC2->At(0);
		BmnTrigWaveDigit * signal_BC3 = (BmnTrigWaveDigit*) BC3->At(0);
		BmnTrigWaveDigit * signal_VC  = (BmnTrigWaveDigit*) VC->At(0);

		BmnTrigWaveDigit * signal_X1_left = (BmnTrigWaveDigit*) X1_left->At(0);
		BmnTrigWaveDigit * signal_X1_right = (BmnTrigWaveDigit*) X1_right->At(0);
		BmnTrigWaveDigit * signal_X2_left = (BmnTrigWaveDigit*) X2_left->At(0);
		BmnTrigWaveDigit * signal_X2_right = (BmnTrigWaveDigit*) X2_right->At(0);
		
		BmnTrigWaveDigit * signal_Y1_left = (BmnTrigWaveDigit*) Y1_left->At(0);
		BmnTrigWaveDigit * signal_Y1_right = (BmnTrigWaveDigit*) Y1_right->At(0);
		BmnTrigWaveDigit * signal_Y2_left = (BmnTrigWaveDigit*) Y2_left->At(0);
		BmnTrigWaveDigit * signal_Y2_right = (BmnTrigWaveDigit*) Y2_right->At(0);
		
		ifBC1 = BC1->GetEntriesFast();
		ifBC2 = BC2->GetEntriesFast();
		ifBC3 = BC3->GetEntriesFast();
		ifVC  = VC ->GetEntriesFast();
		ifX1L = X1_left->GetEntriesFast();
		ifX1R = X1_right->GetEntriesFast();
		ifX2L = X2_left->GetEntriesFast();
		ifX2R = X2_right->GetEntriesFast();
		ifY1L = Y1_left->GetEntriesFast();
		ifY1R = Y1_right->GetEntriesFast();
		ifY2L = Y2_left->GetEntriesFast();
		ifY2R = Y2_right->GetEntriesFast();

		if (T0->GetEntriesFast() > 0) triggerTime = signal_T0->GetTime() ;
		// Getting all TQDC entries
		if (ifBC1 > 0){
			hits_BC1->Fill( ifBC1 );
			trigScalars->Fill( 0 );
			QDC_BC1->Fill( signal_BC1->GetIntegral() );
			ADC_BC1->Fill( signal_BC1->GetPeak() );
			TDC_BC1->Fill( triggerTime - signal_BC1->GetTime() );
		}
		if (ifBC2 > 0){
			hits_BC2->Fill( ifBC2 );
			trigScalars->Fill( 1 );
			QDC_BC2->Fill( signal_BC2->GetIntegral() );
			ADC_BC2->Fill( signal_BC2->GetPeak() );
			TDC_BC2->Fill( triggerTime - signal_BC2->GetTime() );
		}
		if (ifBC3 > 0){
			hits_BC3->Fill( ifBC3 );
			trigScalars->Fill( 2 );
			QDC_BC3->Fill( signal_BC3->GetIntegral() );
			ADC_BC3->Fill( signal_BC3->GetPeak() );
			TDC_BC3->Fill( triggerTime - signal_BC3->GetTime() );
		}
		if (ifVC > 0){
			hits_VC->Fill( ifVC );
			trigScalars->Fill( 3 );
			QDC_VC->Fill( signal_VC->GetIntegral() );
			ADC_VC->Fill( signal_VC->GetPeak() );
			TDC_VC->Fill( triggerTime - signal_VC->GetTime() );
		}
		if (ifX1L > 0){
			trigScalars->Fill( 4 );
		}
		if (ifX1R > 0){
			trigScalars->Fill( 5 );
		}
		if (ifX2L > 0){
			trigScalars->Fill( 6 );
		}
		if (ifX2R > 0){
			trigScalars->Fill( 7 );
		}			
		if (ifY1L > 0){
			trigScalars->Fill( 8 );
		}
		if (ifY1R > 0){
			trigScalars->Fill( 9 );
		}
		if (ifY2L > 0){
			trigScalars->Fill( 10 );
		}
		if (ifY2R > 0){
			trigScalars->Fill(11 );
		}

		// TQDC boolean operations to figure out TQDC efficiency
		beam = ifBC1 && ifBC2;
		X1 = ifX1L || ifX1R;
		X2 = ifX2L || ifX2R;
	
		Y1 = ifY1L && ifY1R;
		Y2 = ifY2L && ifY2R;		

		leftArm = X1 && Y1;
		rightArm = X2 && Y2;
		pair = leftArm || rightArm;

		trig = beam && pair;
		if ( trig == 1){
			moduleEff->Fill( 0 );
			coinScalars->Fill( 0 );
		}
		if ( beam == 1 ) coinScalars->Fill( 1 );
		if ( ifBC1 == 1 ) coinScalars->Fill( 2 );
		if ( ifBC2 == 1 ) coinScalars->Fill( 3 );
		if ( pair == 1 ){
			coinScalars->Fill( 4 );
			moduleEff->Fill( 0 );
		}
		if ( leftArm == 1) coinScalars->Fill( 5 );
		if ( rightArm == 1) coinScalars->Fill( 6 );
		if ( X1 == 1) coinScalars->Fill( 7 );
		if ( Y1 == 1) coinScalars->Fill( 8 );
		if ( X2 == 1) coinScalars->Fill( 9 );
		if ( Y2 == 1) coinScalars->Fill( 10 );
		
		//if ( BC1_tdc->GetEntries() > 0) TDC_tdc_BC1->Fill( signal_tdc_BC1->GetTime() );
		//if ( X1_left_tdc->GetEntries() > 0) TDC_tdc_X1_left->Fill( signal_tdc_X1_left->GetTime() );
		// Others are: signal->GetTime() , signal->GetIntegral() , signal->GetNSamples()		
		/*for (Int_t j = 0; j <detector->GetEntriesFast() ; j++ ){
			double adc = 0;

			// For each hit, it's a BmnTrigWaveDigit with TDC/ADC
			BmnTrigWaveDigit * signal = (BmnTrigWaveDigit*) detector->At(j);
			
			// Get the waveform captured
			short * waveform = signal->GetShortValue();
			for (Int_t m = 0 ; m < signal->GetNSamples() ; m++)
				// currently just getting qdc value again, just
				// to show how to access the waveform
				adc += waveform[m];
			}*/

	}
	TVector3 runInfo(difftime(endT,startT),stopEvent-startEvent,0);
	runInfo.Write("runInfo");
	
	moduleEff->SetTitle("Efficiency of Modules Relative to Trigger");
	moduleEff->GetXaxis()->SetBinLabel(1,"TQDC Efficiency");
	moduleEff->GetXaxis()->SetBinLabel(2,"TDC Efficiency");
	moduleEff->Scale(100./(stopEvent-startEvent));
	moduleEff->SetMaximum(100.);
	moduleEff->Write();
	
	coinScalars->GetXaxis()->SetBinLabel(1,"BC1 & BC2 & PS");
	coinScalars->GetXaxis()->SetBinLabel(2,"BC1 & BC2");
	coinScalars->GetXaxis()->SetBinLabel(3,"BC1");
	coinScalars->GetXaxis()->SetBinLabel(4,"BC2");
	coinScalars->GetXaxis()->SetBinLabel(5,"PS: [X1 & Y1] || [X2 & Y2]");
	coinScalars->GetXaxis()->SetBinLabel(6,"X1 & Y1");
	coinScalars->GetXaxis()->SetBinLabel(7,"X2 & Y2");
	coinScalars->GetXaxis()->SetBinLabel(8,"X1");
	coinScalars->GetXaxis()->SetBinLabel(9,"Y1");
	coinScalars->GetXaxis()->SetBinLabel(10,"X2");
	coinScalars->GetXaxis()->SetBinLabel(11,"Y2");
	coinScalars->Scale(1./(endT-startT));
	coinScalars->Write();
	
	int seconds = difftime(endT,startT);
	int hours = seconds/60/60;
	int minutes = (seconds - hours*60*60)/60;
	seconds = (seconds - hours*60*60 - minutes*60);
	char str[100];
	sprintf(str, "Captured %d triggers in %d hours and %d minutes and %d seconds",stopEvent-startEvent,hours,minutes,seconds);
	cout << "\t" << str << "\n";
	
	trigScalars->GetXaxis()->SetBinLabel(1,"BC1");
	trigScalars->GetXaxis()->SetBinLabel(2,"BC2");
	trigScalars->GetXaxis()->SetBinLabel(3,"BC3");
	trigScalars->GetXaxis()->SetBinLabel(4,"VC");
	trigScalars->GetXaxis()->SetBinLabel(5,"X1 Left");
	trigScalars->GetXaxis()->SetBinLabel(6,"X1 Right");
	trigScalars->GetXaxis()->SetBinLabel(7,"X2 Left");
	trigScalars->GetXaxis()->SetBinLabel(8,"X2 Right");
	trigScalars->GetXaxis()->SetBinLabel(9,"Y1 Left");
	trigScalars->GetXaxis()->SetBinLabel(10,"Y1 Right");
	trigScalars->GetXaxis()->SetBinLabel(11,"Y2 Left");
	trigScalars->GetXaxis()->SetBinLabel(12,"Y2 Right");
	trigScalars->Scale(1./(endT-startT));
	trigScalars->Write();

	hits_BC1->Write();
	hits_BC2->Write();
	hits_BC3->Write();
	hits_VC ->Write();

	QDC_BC1->Write();
	QDC_BC2->Write();
	QDC_BC3->Write();
	QDC_VC->Write();

	ADC_BC1->Write();
	ADC_BC2->Write();
	ADC_BC3->Write();
	ADC_VC->Write();	

	TDC_BC1->Write();
	TDC_BC2->Write();
	TDC_BC3->Write();
	TDC_VC->Write();

	outFile->Close();
	

	
	
	

}
