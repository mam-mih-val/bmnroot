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
                pDetector[i] = new BmnTOF1Detector(i, 0);
        TFile file("test.BmnTof400HitProducer.root", "RECREATE");

	double fTimeL[fNDetectors][fNStr], fTimeR[fNDetectors][fNStr];
	int hits_usable;

	TH1D *tofL[fNDetectors][fNStr+1];
	TH1D *tofR[fNDetectors][fNStr+1];
	TString fname;
	
	for( int k = 0 ; k < fNDetectors ; k++){
		for( int j = 0 ; j < fNStr+1 ; j++){
			fname = Form("HistL_ToF_Plane_%d_str%d", k, j);
			cout << fname << "\n";
			tofL[k][j] = new TH1D(fname,"",2000,-10000,10000);
			fname = Form("HistR_ToF_Plane_%d_str%d", k, j);
			tofR[k][j] = new TH1D(fname,"",2000,-10000,10000);
		}
	}


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
			for (Int_t i = 0; i < fNDetectors; i++){
                    		pDetector[i]->Clear();
				for (int w = 0 ; w < fNStr ; w++){
					fTimeL[i][w] = 0.;
					fTimeR[i][w] = 0.;
				}
			}
			
			for (Int_t iDig = 0; iDig < ToF400->GetEntriesFast(); ++iDig) {
				BmnTof1Digit* digTof = (BmnTof1Digit*) ToF400->At(iDig);
                        	
				pDetector[digTof->GetPlane()]->SetDigit(digTof);
				
				if(digTof->GetTime() == 0) continue;

				if(digTof->GetSide()==0){
					fTimeL[digTof->GetPlane()][digTof->GetStrip()] = digTof->GetTime();
					tofL[digTof->GetPlane()][digTof->GetStrip()]->Fill(digTof->GetTime());
				}		
				if(digTof->GetSide()==1){
					fTimeR[digTof->GetPlane()][digTof->GetStrip()] = digTof->GetTime();
					tofR[digTof->GetPlane()][digTof->GetStrip()]->Fill(digTof->GetTime());
				}

				//tof[digTof->GetPlane()][digTof->GetStrip()]->Fill(digTof->GetTime());
				//cout << digTof->GetTime() - digT0->GetTime() << "\n";
			}

                	for (Int_t i = 0; i < fNDetectors; i++) {
                     		for (int w = 0 ; w < fNStr ; w++){
					if( (fTimeL[i][w]!=0) || (fTimeR[i][w]!=0) ){
						//cout << "\t\tcandidate event in strip: " << w << "\n";	
						hits_usable++;
					}
				}
				nSingleHits += pDetector[i] -> FindHits(digT0, aTofHits);
			}
		}
	}
	/*
	for (Int_t i = 0 ; i < fNDetectors; i++){
		cout << "Writing Plane: " << i << "\n";
		TFile *ptr = gFile;
		TList* fList = (TList*) pDetector[i]->GetList(2);
		
		fList->Write();
	}
	*/
	cout << "Number of total hits usable in file: " << hits_usable << "\n";
	
	TFile *ptr = gFile;
	gFile->cd();
	for( int i = 0 ; i < fNDetectors ; i++){
		for( int j = 0 ; j < fNStr ; j++){
			tofL[i][j]->Write();
			tofR[i][j]->Write();
		}
	}
        //TList* fList = (TList*) pDetector[1]->GetList(2);
	//fList->Write();
        file.Close();


}
