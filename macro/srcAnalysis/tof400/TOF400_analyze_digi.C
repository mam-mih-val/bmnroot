// file for all of the other detectors..
#include <time.h>

void TOF400_analyze_digi(TString infile, int startEvent, int stopEvent){
	gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
	bmnloadlibs();

	TChain* rootTree = new TChain("cbmsim");
	rootTree->Add(infile);

	TClonesArray * EventHead = new TClonesArray("BmnEventHeader");
	TClonesArray * ToF400 = new TClonesArray("BmnTof1Digit");
	TClonesArray * T0 = new TClonesArray("BmnTrigDigit");
	
	Int_t event_count = rootTree->GetEntries();
	cout << "Number of events in _digi file = " << event_count << endl;
	if ( stopEvent == -1) stopEvent = event_count;

	rootTree->GetBranch("EventHeader")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("TOF400")->SetAutoDelete(kFALSE);
	rootTree->GetBranch("BC2")->SetAutoDelete(kFALSE);

	rootTree->SetBranchAddress("EventHeader", &EventHead);
	rootTree->SetBranchAddress("TOF400", &ToF400);
	rootTree->SetBranchAddress("BC2", &T0);


	// For ToF400 plotting:
	const int fNStr = 48;
	const int NPlane = 20;
	const int fNDetectors = 20;
	TClonesArray * aTofHits = new TClonesArray("BmnTofHit");
	BmnTOF1Detector **pDetector = new BmnTOF1Detector *[fNDetectors];
	for (Int_t i = 0; i < fNDetectors; i++) 
                pDetector[i] = new BmnTOF1Detector(i, 2);
        TFile file("test.BmnTof400HitProducer.root", "RECREATE");


	for (Int_t j = startEvent; j < stopEvent; j++){
		if (j%100==0) cout << "\tWorking on entry " << j << "\n";

		int nSingleHits = 0;
		EventHead->Clear();
		ToF400->Clear();
		T0->Clear();

		rootTree->GetEntry(j);

	

		int nT0Digits = T0->GetEntriesFast();
            	if (nT0Digits == 1) { // T0 digit should be
			BmnTrigDigit* digT0 = (BmnTrigDigit*) T0->At(0);
			
			for (Int_t i = 0; i < fNDetectors; i++)
                    		pDetector[i]->Clear();
			
			for (Int_t iDig = 0; iDig < ToF400->GetEntriesFast(); ++iDig) {
                    		BmnTof1Digit* digTof = (BmnTof1Digit*) ToF400->At(iDig);
                        	pDetector[digTof->GetPlane()]->SetDigit(digTof);
                	}

                	for (Int_t i = 0; i < fNDetectors; i++) {
                     		nSingleHits += pDetector[i] -> FindHits(digT0, aTofHits);
				//pDetector[i]->SaveHistToFile("test.BmnTof400HitProducer.root");
			}
		}
		
		//MergeHitsOnStrip();
		//int nFinally = CompressHits();
	}

	for (Int_t i = 0 ; i < fNDetectors; i++){
		TFile *ptr = gFile;
		TList* fList = (TList*) pDetector[i]->GetList(2);
		
		fList->Write();
	}
	


	TFile *ptr = gFile;
        //TList* fList = (TList*) pDetector[1]->GetList(2);
	//fList->Write();
        file.Close();


}
