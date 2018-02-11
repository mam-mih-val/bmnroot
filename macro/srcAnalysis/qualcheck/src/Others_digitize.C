// file for all of the other detectors..
#include <time.h>
void Others_digitize(TString file, TString outFileName, int startEvent, int stopEvent){
	gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
	bmnloadlibs();

	TChain* rootTree = new TChain("cbmsim");
	rootTree->Add(file);

	TFile * outFile = new TFile(outFileName, "RECREATE");
	
	
	TClonesArray * EventHead = new TClonesArray("BmnEventHeader");
	TClonesArray * MWPC = new TClonesArray("BmnMwpcDigit");
	TClonesArray * ZDC = new TClonesArray("BmnZDCDigit");
	TClonesArray * Sil = new TClonesArray("BmnSiliconDigit");	
	TClonesArray * ToF700 = new TClonesArray("BmnTof2Digit");
	TClonesArray * ToF400 = new TClonesArray("BmnTof1Digit");
	TClonesArray * DCH = new TClonesArray("BmnDchDigit");
	TClonesArray * GEM = new TClonesArray("BmnGemStripDigit");

	time_t startT = time(NULL);
	time_t endT = time(NULL);

	Int_t event_count = rootTree->GetEntries();
	cout << "Number of events in _digi file = " << event_count << endl;

	rootTree->GetBranch("EventHeader")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("MWPC")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("ZDC")->SetAutoDelete(kFALSE);	
	rootTree->GetBranch("SILICON")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TOF700")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TOF400")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("DCH")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("GEM")->SetAutoDelete(kFALSE);

	rootTree->SetBranchAddress("EventHeader", &EventHead);
	rootTree->SetBranchAddress("MWPC", &MWPC);
	rootTree->SetBranchAddress("ZDC", &ZDC);
	rootTree->SetBranchAddress("SILICON", &Sil);
	rootTree->SetBranchAddress("TOF700", &ToF700);
	rootTree->SetBranchAddress("TOF400", &ToF400);
	rootTree->SetBranchAddress("DCH", &DCH);
	rootTree->SetBranchAddress("GEM", &GEM);

	if(stopEvent == -1) stopEvent = event_count;
	if(stopEvent > event_count){
		stopEvent = event_count;
		cout << "WARNING: trying to draw more events than in file\n";
		cout << "WARNING: reference and current runs may have different normalization\n";
	}

	// For MWPC:
	const int mwpc_planes = 12;
	const int mwpc_wireNo = 96;
	char name[50];
	int plane;
	TH1F * mwpc_wires[mwpc_planes];
	TH1F * mwpc_times[mwpc_planes];
	for (int k = 0; k<mwpc_planes; k++){
		sprintf(name,"MWPC_Plane_%d",k);
		mwpc_wires[k] = new TH1F(name,name,mwpc_wireNo,0,mwpc_wireNo);
		sprintf(name,"MWPC_Plane_%d_Time",k);
		mwpc_times[k] = new TH1F(name,name,250,0,500);
	}

	// For ZDC:
	double xAmp, yAmp, amp, totAmp;
	//ZDC_grid = new TH2D("ZDC_Grid", "ZDC Grid", 20, 0 ,20 ,14 ,0 ,14);
	ZDC_profile = new TH2D("ZDC_Profile", "ZDC Profile", 200, -700, 700 , 200, -700, 700);
	//ZDC_grid->GetZaxis()->SetNoExponent(kFALSE);	

	// For Silicon:
	TH1F * silModule  = new TH1F("silModule","Silicon Modules [8 per station]",32,0,32);	

	// For ToF700:
	TH1F * tof700_time = new TH1F("tof700_time","ToF700 Time",16,-800,800);
	TH1F * tof700_strips = new TH1F("tof700_strips","ToF700 Strips [32 in Chamber, 24 Chambers]",32*24,0,32*24);

	// For ToF400:
	TH2D * tof400_state = new TH2D("tof400_state","ToF400 Hits in Strips",48*10,0,48*10,2,0,2);
	TH1D * tof400_time = new TH1D("tof400_time","ToF400 Time",500,0,1000);

	// For DCH:
	const int dch_planes = 16;
	const int dch_wireNo = 256; // numRealWires = 240
	const TString names[dch_planes] = { "VA_1", "VB_1", "UA_1", "UB_1", "YA_1", "YB_1", "XA_1", "XB_1", "VA_2", "VB_2", "UA_2", "UB_2", "YA_2", "YB_2", "XA_2", "XB_2"};
	TH1F * dch_wires[dch_planes];
	TH1F * dch_times[dch_planes];
	for (int k = 0; k<dch_planes; k++){
		dch_wires[k] = new TH1F("DCH_Plane_"+names[k]+"_Wires","DCH_Plane_"+names[k]+"_Wires",dch_wireNo,0,dch_wireNo);
		dch_times[k] = new TH1F("DCH_Plane_"+names[k]+"_Times","DCH_Plane_"+names[k]+"_Times",500,-300,1300);
	}

	// For GEMs:
	TH1F * gemStation = new TH1F("gemStation","Gem Stations",6,0,6);

	for (Int_t i = startEvent; i < stopEvent; i++){
		if (i%1000==0) cout << "\tWorking on entry " << i << "\n";

		EventHead->Clear();
		MWPC->Clear();
		ZDC->Clear();
		Sil->Clear();
		ToF700->Clear();
		ToF400->Clear();
		DCH->Clear();		
		GEM->Clear();

		rootTree->GetEntry(i);

		BmnEventHeader * head = (BmnEventHeader*) EventHead->At(0);
		TDatime time = head->GetEventTime();
		if (i==startEvent) startT = time.Convert();
		if (i==stopEvent-1) endT = time.Convert();
		
		// For MWPC:
		for( int j = 0; j < MWPC->GetEntriesFast() ; j++){
			BmnMwpcDigit* digMWPC = (BmnMwpcDigit*) MWPC->At(j);
			plane = digMWPC->GetPlane();
			mwpc_wires[plane]->Fill(digMWPC->GetWireNumber());
			mwpc_times[plane]->Fill(digMWPC->GetTime());
			// digMWPC->GetPlane()
			// digMWPC->GetWireNumber()
			// digMWPC->GetTime()
			// digMWPC->GetRefId()
			// digMWPC->IsUsed()
		}

		// For ZDC:
		totAmp = 0;
		xAmp = 0;
		yAmp = 0;
		for( int j = 0; j < ZDC->GetEntriesFast() ; j++){
			BmnZDCDigit* digZDC = (BmnZDCDigit*) ZDC->At(j);
			//ix = digZDC->GetIX();
			//iy = digZDC->GetIY();
			// digZDC->GetX()
			// digZDC->GetY()
			// digZDC->GetSize()
			// digZDC->GetChannel()
			// digZDC->GetAmp()
			amp = digZDC->GetAmp();
			totAmp += amp;
			//ZDC_grid->Fill(ix,iy,amp);
			xAmp  += amp * digZDC->GetX();
			yAmp += amp * digZDC->GetY();
		}
		if (totAmp != 0){
			xAmp /= totAmp;
			yAmp /= totAmp;
			ZDC_profile->Fill(xAmp,yAmp,totAmp);
		}

		// For Silicon:
		for( int j = 0 ; j < Sil->GetEntriesFast() ; j++){
			BmnSiliconDigit * digSil = (BmnSiliconDigit*) Sil->At(j);
			// digSil->GetStation()
			// digSil->GetModule()
			// digSil->GetStripLayer() 0 - lower / 1 - upper
			// digsil->GetStripNumber()
			// digSil->GetStripSignal()
			silModule->Fill( digSil->GetStation()*(8) + digSil->GetModule()  );
		}

		// For ToF700:
		for( int j = 0 ; j < ToF700->GetEntriesFast() ; j++){
			BmnTof2Digit * digToF = (BmnTof2Digit*) ToF700->At(j);
			// digToF->GetPlane()
			// digToF->GetStrip()
			// digToF->GetTime()       -- average time
			// digToF->GetAmplitude()
			// digToF->GetDiff()       -- relative time
			tof700_time->Fill( digToF->GetTime()  );
			tof700_strips->Fill( digToF->GetStrip() + digToF->GetPlane()*(32)  );
		}
		
		// For ToF400:
		for( int j = 0 ; j < ToF400->GetEntriesFast() ; j++){
			BmnTof1Digit * digToF1 = (BmnTof1Digit*) ToF400->At(j);
			// digToF1->GetPlane()
			// digToF1->GetSide()
			// digToF1->GetStrip()
			// digToF1->GetAmplitude()
			// digToF1->GetTime()
			tof400_state->Fill( digToF1->GetPlane()*48 + digToF1->GetStrip() , digToF1->GetSide() , digToF1->GetAmplitude()  );
			tof400_time->Fill( digToF1->GetTime() );	
		}
		
		// For DCH:
		for( int j = 0 ; j < DCH->GetEntriesFast() ; j++){
			BmnDchDigit * digDCH = (BmnDchDigit*) DCH->At(j);
			// digDCH->GetPlane()
			// digDCH->GetWireNumber()
			// digDCH->GetTime()
			// digDCH->GetRefID()		
			plane = digDCH->GetPlane();
			dch_wires[plane]->Fill(digDCH->GetWireNumber());
			dch_times[plane]->Fill(digDCH->GetTime());
		}	

		// For GEMs:
		for( int j = 0 ; j < GEM->GetEntriesFast() ; j++){
			BmnGemStripDigit * digGEM = (BmnGemStripDigit*) GEM->At(j);
			// digGEM->GetStation()
			// digGEM->GetModule()
			// digGEM->GetStripLayer()
			// digGEM->GetStripNumber()
			// digGEM->GetStripSignal()
			gemStation->Fill(digGEM->GetStation());
		}
	}
	
	for (int k=0; k<mwpc_planes; k++){
		mwpc_wires[k]->Write();
		mwpc_times[k]->Write();
	}
	for (int k=0; k<dch_planes; k++){
		dch_wires[k]->Write();
		dch_times[k]->Write();
	}
	//ZDC_grid->Write();
	ZDC_profile->Write();
	silModule->Write();
	tof700_time->Write();
	tof700_strips->Write();
	tof400_state->Write();
	tof400_time->Write();
	gemStation->Write();
	outFile->Close();
}
