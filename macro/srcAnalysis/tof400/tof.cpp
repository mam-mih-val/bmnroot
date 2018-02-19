/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <istream>
#include <iostream>
#include <ctime>

#include <TMath.h>
#include "TChain.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1S.h"
#include "TH2S.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TList.h"
#include "TDirectory.h"
#include "TPad.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TStopwatch.h"

Double_t CorrLR[10][48] = {0.};

Bool_t FlagHit = kFALSE;
Int_t XforHit = 0, YforHit = 0;

class BmnTrigDigit;
class BmnTOF1Detector;

void tof(TString file = "", Int_t nEvForRead = 0) {
    
    TStopwatch timer1;
    timer1.Start();

    TStopwatch timer;
    timer.Start();

    TCutG *cut;
    TFile *f = new TFile("theta_P2_boost_vs_theta_P1_boost_3p5GeVpercperu.root");
    cut = (TCutG*)f->Get("mycut");

    TCutG *cut1;
    TFile *f1 = new TFile("P1_boost_vs_theta_P1_boost_3p5GeVpercperu.root");
    cut1= (TCutG*)f1->Get("mycut");

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    cout << "Loaded libraries...\n";
    ifstream f_call;
    char line[256];
    Int_t Pl, St;
    Double_t Temp;
    
    TString path = getenv("VMCWORKDIR");    

    TString NameCallFile = path + "/input/TOF400_LRCorr_Period_6.dat";
    
    f_call.open(NameCallFile.Data());
    f_call.getline(line, 256);
    f_call.getline(line, 256);
    while (!f_call.eof()) {
        f_call >> Pl >> St >> Temp >> CorrLR[Pl][St];
        if (TMath::Abs(Temp - CorrLR[Pl][St]) > 2.) CorrLR[Pl][St] = -11.9766;
    }
    cout << "Finished reading Tof400LRcorr.dat\n";
    BmnTOF1Detector * Plane[10];

    //TString name;
    for (Int_t i = 0; i < 10; i++) {
        //TString name = Form("Plane%d", i);
        Plane[i] = new BmnTOF1Detector(i, 2);
        //Plane[i]->SetCorrLR(CorrLR[i]);
        Plane[i]->SetCorrLR("TOF400_LRCorr_Period_6.dat");
        Plane[i]->SetCorrSlewing("TOF400_SlewingCorr_Period_6.root");
        //Plane[i]->SetGeoFile("geofile_full.root");
        Plane[i]->SetGeoFile("geometry_run6.root");
        //Plane[i]->SetGeoFile("geofile_full.root");
    }
	cout << "Loaded input files...\n";
    TList *fList = new TList();
    TList *fListTof = new TList();

    TH2F *h_HitRPC = new TH2F("h_HitRPC", "h_HitRPC", 96, -96, 0, 5, 0, 5);
    fListTof->Add(h_HitRPC);

    TH2F *h_XYRPC = new TH2F("h_XYRPC", "h_XYRPC", 240, -150, 150, 120, -75, 75);
    fListTof->Add(h_XYRPC);

    /////////////////////////////////////////////

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("../../raw/data-Decoded/bmn_run%s_digi.root", file.Data());
    //inName = file;
    cout << "Open file " << inName << endl << endl;
    eveTree->Add(inName);

    TClonesArray *ToF400Digits;
    eveTree->SetBranchAddress("TOF400", &ToF400Digits);

    TClonesArray *T0Digits;
    eveTree->SetBranchAddress("T0", &T0Digits);

    TClonesArray *VetoDigits;
    eveTree->SetBranchAddress("VETO", &VetoDigits);

    TClonesArray *BC1Digits;
    eveTree->SetBranchAddress("BC1", &BC1Digits);

    TClonesArray *BC2Digits;
    eveTree->SetBranchAddress("BC2", &BC2Digits);

    TClonesArray *EventHeader;
    eveTree->SetBranchAddress("EventHeader", &EventHeader);

    ////////////////////////////////////////////
   TTree *T = new TTree("T","T");
   TLorentzVector v, v1, v2;
   TLorentzVector v1_boost, v2_boost;
   TVector3 XYZ;

   double P_[3], P, E, theta_P, phi_P, ToF, d;
   double P_1[3], P1, E1, theta_P1, phi_P1, ToF1, d1;
   double P_2[3], P2, E2, theta_P2, phi_P2, ToF2, d2;
   double P_miss[3], Pmiss, Emiss; 
   double P_1_boost[3], P1_boost, E1_boost, theta_P1_boost, phi_P1_boost;
   double P_2_boost[3], P2_boost, E2_boost, theta_P2_boost, phi_P2_boost;
   double s,t,u;
   double s_boost,t_boost,u_boost;

    T->Branch("d1",&d1,"d1/D");
    T->Branch("d2",&d2,"d2/D");

    T->Branch("P_1_boost",&P_1_boost,"P_1_boost[3]/D");
    T->Branch("P1_boost",&P1_boost,"P1_boost/D");
    T->Branch("E1_boost",&E1_boost,"E1_boost/D");
    T->Branch("theta_P1_boost",&theta_P1_boost,"theta_P1_boost/D");
    T->Branch("phi_P1_boost",&phi_P1_boost,"phi_P1_boost/D");

    T->Branch("P_2_boost",&P_2_boost,"P_2_boost[3]/D");
    T->Branch("P2_boost",&P2_boost,"P2_boost/D");
    T->Branch("E2_boost",&E2_boost,"E2_boost/D");
    T->Branch("theta_P2_boost",&theta_P2_boost,"theta_P2_boost/D");
    T->Branch("phi_P2_boost",&phi_P2_boost,"phi_P2_boost/D");

    T->Branch("s_boost",&s_boost,"s_boost/D");
    T->Branch("t_boost",&t_boost,"t_boost/D");
    T->Branch("u_boost",&u_boost,"u_boost/D");

    T->Branch("P_1",&P_1,"P_1[3]/D");
    T->Branch("P1",&P1,"P1/D");
    T->Branch("E1",&E1,"E1/D");
    T->Branch("theta_P1",&theta_P1,"theta_P1/D");
    T->Branch("phi_P1",&phi_P1,"phi_P1/D");
    T->Branch("ToF1",&ToF1,"ToF1/D");

    T->Branch("P_2",&P_2,"P_2[3]/D");
    T->Branch("P2",&P2,"P2/D");
    T->Branch("E2",&E2,"E2/D");
    T->Branch("theta_P2",&theta_P2,"theta_P2/D");
    T->Branch("phi_P2",&phi_P2,"phi_P2/D");
    T->Branch("ToF2",&ToF2,"ToF2/D");

    T->Branch("P_miss",&P_miss,"P_miss[3]/D");
    T->Branch("Pmiss",&Pmiss,"Pmiss/D");
    T->Branch("Emiss",&Emiss,"Emiss/D");

    T->Branch("s",&s,"s/D");
    T->Branch("t",&t,"t/D");
    T->Branch("u",&u,"u/D");

    //
    //Useful Quantities for boosting  
    //
    const Double_t Ac12=12.;
    const Double_t mC12 = 11.1770; // 12 * 931.494028
    Double_t Pbeam = 3.5; //GeV/c
    Double_t Ebeam_tmp=TMath::Sqrt( pow(Ac12*Pbeam,2) + pow(mC12,2) );
    const Double_t mp = 0.938272;

    TLorentzVector vc12_ForReconstructions(TVector3(0,0,Ac12*Pbeam), Ebeam_tmp);//un-smeared Nucleus beam 4-momentum (DUBNA lab frame)
    TLorentzVector v_proton_beam_Standing( TVector3(0,0,0),mp);  // Standing proton (DUBNA lab frame)
    TLorentzVector v_proton_beam_UnSMEARED( TVector3(0,0,0),mp);  // Standing proton (DUBNA lab frame)
    v_proton_beam_UnSMEARED.Boost(-vc12_ForReconstructions.BoostVector()); // Boost from DUBNA to GSI frame
    int nStripsL=0;
    int nStripsR=0;
    ////////////////////////////////

    Long64_t nEvents = eveTree->GetEntries();
    if (nEvForRead == 0 || nEvForRead > nEvents) nEvForRead = nEvents;
    cout << "Will analyze " << nEvForRead << " events from " << nEvents << endl;

    for (Int_t iEv = 0; iEv < nEvForRead; iEv++) {

        if (iEv % 1000 == 0) {
	cout << "EVENT: " << iEv << endl;
	timer1.Stop();
	cout<<timer1.RealTime()<<endl;
        timer1.Start();
	}	
        eveTree->GetEntry(iEv);

        if (T0Digits->GetEntriesFast() > 0){ 
	    //(VetoDigits->GetEntriesFast()) == 0 && 
	    /*(BC1Digits->GetEntriesFast()) == 1 &&*/
	    //(BC2Digits->GetEntriesFast()) == 1 ) {
	
            BmnTrigDigit* digT0 = (BmnTrigDigit*) T0Digits->At(0);
            //if (digT0->GetAmp() >= 17.3 && digT0->GetAmp() <= 19.2) {
		
                BmnTrigDigit* digT0 = (BmnTrigDigit*) T0Digits->At(0);

                //--------------------------- EventHeader --------------------------------------------------
                Int_t iEvDig = EventHeader->GetEntriesFast();
                if (iEvDig != 1) cout << "iEvDig == " << iEvDig << endl;
                BmnEventHeader* digEvent = (BmnEventHeader*) EventHeader->At(0);

                //--------------------------- RPC --------------------------------------------------
                for (Int_t i = 0; i < 10; i++) Plane[i]->Clear();

                for (Int_t iDig = 0; iDig < ToF400Digits->GetEntriesFast(); ++iDig) {
                    FlagHit = kFALSE;
                    BmnTof1Digit* digTof = (BmnTof1Digit*) ToF400Digits->At(iDig);
                    FlagHit = Plane[digTof->GetPlane()]->SetDigit(digTof);
                    if (FlagHit == kTRUE) {
                        if (digTof->GetPlane() < 5) {
                            XforHit = digTof->GetStrip() * -1. - 48;
                            YforHit = 4 - digTof->GetPlane();
                        } else {
                            XforHit = digTof->GetStrip() * -1.;
                            YforHit = 4 - (digTof->GetPlane() - 5);
                        }
                        h_HitRPC->Fill(XforHit, YforHit);
                    }//end of kTRUE
                }//end of iDig TOF400 entries

		nStripsL=0;
		nStripsR=0;

                for (Int_t i = 0; i < 10; i++) {
                    Plane[i] -> FindHits(digT0);
                    for (Int_t strip = 0; strip < 47; strip++) {

                        XYZ.SetXYZ(0., 0., 0.);
                        ToF = 0.;
                        FlagHit = kFALSE;

                        FlagHit = Plane[i]->GetXYZTime(strip, &XYZ, &ToF);

                        //if (FlagHit == kTRUE) {
			//cout<<XYZ.x()<<" "<<XYZ.y()<<" "<<XYZ.z()<<endl;
			d=sqrt(pow(XYZ.x(),2) + pow(XYZ.y(),2) + pow(XYZ.z(),2) )/100.; //in meters 
			P = sqrt( pow(mp,2) / (pow(ToF/(3.3*d),2) - 1) );
			theta_P = TMath::ACos(XYZ.z()/100./d); // Calculate theta [degrees]
 			phi_P = TMath::ATan2(XYZ.y(),XYZ.x()); // Calculate phi   [degrees]
			P_[0]=P*cos(phi_P)*sin(theta_P);
			P_[1]=P*sin(phi_P)*sin(theta_P);
			P_[2]=P*cos(theta_P);
                        v.SetXYZT(P_[0],P_[1],P_[2],TMath::Sqrt(pow(P,2) + pow(mp,2)));
			v.Boost(-vc12_ForReconstructions.BoostVector());

			if (i<5)
			{
				P_1_boost[0]=P_[0];
				P_1_boost[1]=P_[1];
				P_1_boost[2]=P_[2];
				P1_boost=P;
				E1_boost=TMath::Sqrt(pow(P1_boost,2) + pow(mp,2));
				v1_boost.SetXYZT(P_1_boost[0],P_1_boost[1],P_1_boost[2],E1_boost);
				theta_P1_boost=theta_P*TMath::RadToDeg();
				phi_P1_boost=phi_P*TMath::RadToDeg();
				ToF1=ToF;
				d1=d;

				v1.SetXYZT(v.X(),v.Y(),v.Z(),TMath::Sqrt(pow(v.P(),2) + pow(mp,2)));
				nStripsL++;
			}
			else //if (i>=10)
			{
				P_2_boost[0]=P_[0];
				P_2_boost[1]=P_[1];
				P_2_boost[2]=P_[2];
				P2_boost=P;
				E2_boost=TMath::Sqrt(pow(P2_boost,2) + pow(mp,2));
				v2_boost.SetXYZT(P_2_boost[0],P_2_boost[1],P_2_boost[2],E2_boost);				
				theta_P2_boost=theta_P*TMath::RadToDeg();
				phi_P2_boost=phi_P*TMath::RadToDeg();
				ToF2=ToF;
				d2=d;

				v2.SetXYZT(v.X(),v.Y(),v.Z(),TMath::Sqrt(pow(v.P(),2) + pow(mp,2)));
				nStripsR++;
			}// end of if 

			h_XYRPC -> Fill(XYZ.x(), XYZ.y());
			
			//}//end of kTRUE	
                    }//end of s strips
                }//end of i planes 


		if ((nStripsL==1) && (nStripsR==1)){
			//cout<<"I am in!"<<endl;
			//if (cut->IsInside(theta_P2_boost,theta_P1_boost)){
		    		//if (cut1->IsInside(P1_boost,theta_P1_boost)){  
		    			//if (cut1->IsInside(P2_boost,theta_P2_boost)){ 
		    				//if ((v1.P()+v2.P())>4.0 && (v1.P()+v2.P())<4.5)){

			P_1[0] = v1.X();
			P_1[1] = v1.Y();
			P_1[2] = v1.Z();
			P1 = v1.P();
			E1 = TMath::Sqrt(pow(P1,2)+pow(mp,2));
			theta_P1=v1.Theta()*TMath::RadToDeg();
			phi_P1=v1.Phi()*TMath::RadToDeg();

			P_2[0] = v2.X();
			P_2[1] = v2.Y();
			P_2[2] = v2.Z();
			P2 = v2.P();
			E2 = TMath::Sqrt(pow(P2,2)+pow(mp,2));
			theta_P2=v2.Theta()*TMath::RadToDeg();
			phi_P2=v2.Phi()*TMath::RadToDeg();
			
			P_miss[0] = v1.X() + v2.X();
            		P_miss[1] = v1.Y() + v2.Y();
            		P_miss[2] = v1.Z() + v2.Z() + v_proton_beam_UnSMEARED.P();
			Pmiss = TMath::Sqrt(pow(P_miss[0],2) + pow(P_miss[1],2) + pow(P_miss[2],2));
			Emiss = TMath::Sqrt(pow(Pmiss,2) + pow(mp,2));
			
			s = (v1+v2)*(v1+v2);
			u = (v_proton_beam_UnSMEARED-v2)*(v_proton_beam_UnSMEARED-v2);
			t = (v_proton_beam_UnSMEARED-v1)*(v_proton_beam_UnSMEARED-v1);

			s_boost = (v1_boost+v2_boost)*(v1_boost+v2_boost);
			u_boost = (v_proton_beam_Standing-v2_boost)*(v_proton_beam_Standing-v2_boost);
			t_boost = (v_proton_beam_Standing-v1_boost)*(v_proton_beam_Standing-v1_boost);
			
			T->Fill();
						//} // end of P1 + P2
					//} // end of cut 1 
				//} // end of cut 1 
			//} // end of cut 	
		} // end of if nStripsL, nStripsR

            //}// end if (digT0->GetAmp() >= 17.3 && digT0->GetAmp() <= 19.2)
        } // end if ((T0Digits->GetEntriesFast()) == 1 && (VetoDigits->GetEntriesFast()) == 0)
    } // end for (Int_t iEv = 0; iEv < nEvForRead; iEv++)

    cout << "End Data Analysis" << endl;

    timer.Stop();
    cout << "Readed " << nEvForRead << " events" << endl;
    cout << "Time          = " << timer.RealTime() << " s" << endl;
    cout << "Time/Event    = " << timer.RealTime() / (Double_t) nEvForRead * 1000. << " ms/Event" << endl;
    cout << "TimeCPU       = " << timer.CpuTime() << " s" << endl;
    cout << "TimeCPU/Event = " << timer.CpuTime() / (Double_t) nEvForRead * 1000. << " ms/Event" << endl;

    ofstream f_time;
    TString NameTimeFile = "output/Tof400An_Time.dat";
    //TString NameCallFile = file;
    //Point = NameCallFile.First('.');
    //NameCallFile.Replace(Point, 15, "_LRcorr.dat");
    cout << "Write time to " << NameTimeFile.Data() << endl;
    f_time.open(NameTimeFile.Data(), std::ofstream::out | std::ofstream::app);
    f_time << file << "\t" << timer.RealTime() << "\t" << timer.RealTime() / (Double_t) nEvForRead * 1000. << "\t" << timer.CpuTime() << "\t" << timer.CpuTime() / (Double_t) nEvForRead * 1000. << endl;
    f_time.close();

    timer.Reset();
    timer.Start();
    TFile *fileout = new TFile("output/"+file+"_TofAn6_test.root", "RECREATE");

    TDirectory *Dir;
    Dir = fileout->mkdir("ToF");
    Dir->cd();
    fListTof->Write();

    TDirectory * DirPlane[10];
    TDirectory * Dir1Plane[10];
    TDirectory * Dir2Plane[10];

    for (Int_t i = 0; i < 10; i++) {
        DirPlane[i] = Dir->mkdir(Plane[i]->GetName());
        DirPlane[i] -> cd();
	Plane[i]->GetList(0)->Write();

        Dir1Plane[i] = DirPlane[i]->mkdir("Detector");
        Dir1Plane[i] -> cd();
        Plane[i]->GetList(1)->Write();

        Dir2Plane[i] = DirPlane[i]->mkdir("ToF");
        Dir2Plane[i] -> cd();
        Plane[i]->GetList(2)->Write();
    }

    fileout->cd();
    T->Write();
    fList->Write();
    fileout->Close();

    timer.Stop();

    cout << "Time for write root = " << timer.RealTime() << " s" << endl;

}//end of macro


//----Additional functions ----------------------------------------------------------------


