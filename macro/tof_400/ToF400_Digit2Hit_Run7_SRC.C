/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ToF400_Digit2Hit_Run7_SRC.C
 * Author: mikhailr
 *
 * Created on May 24, 2018, 3:17 PM
 */

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;

Double_t AmpBD, TimeBD, NHitBD;

Bool_t FlagHit = kFALSE;
Int_t XforHit = 0, YforHit = 0;
const Int_t NDet = 20;

void ToF400_Digit2Hit_Run7_SRC(TString file = "/home/storage/digits/SRC/bmn_run3482_digi.root", Int_t nEvForRead = 0) {

    bmnloadlibs(); // load BmnRoot libraries

    // Init output tree and file
    TTree *treeHitIn = new TTree("TOF400", "TOF400");
    vector <double> *XTof = new vector<double>;
    vector <double> *YTof = new vector<double>;
    vector <double> *ZTof = new vector<double>;
    vector <double> *TimeTof = new vector<double>;
    UInt_t EventHeader_EventID;

    treeHitIn->Branch("EventID", &EventHeader_EventID);
    treeHitIn->Branch("XTof", &XTof);
    treeHitIn->Branch("YTof", &YTof);
    treeHitIn->Branch("ZTof", &ZTof);
    treeHitIn->Branch("TimeTof", &TimeTof);

    TString outName = file;
    Int_t Point = outName.First('.');
    outName.Replace(Point, 20, "_Tof400Hit.root");
    TFile *FileHitOut = new TFile(outName.Data(), "RECREATE");

    // Init BmnTOF1Detector
    BmnTOF1Detector * Plane[NDet];
    TString name;
    for (Int_t i = 0; i < NDet; i++) {
        name = Form("Plane%d", i);
        Plane[i] = new BmnTOF1Detector(i, 2, NULL);
        Plane[i]->SetCorrLR("TOF400_LRcorr_RUN7.dat");
        Plane[i]->SetGeoFile("geofile_full_src.root");
    }//*/

    // Init tree with Digi
    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("%s", file.Data());
    cout << endl << endl << "Open file " << inName << endl << endl;
    Int_t AddReturn;
    AddReturn = eveTree->Add(inName);
    //cout << "AddReturn = " << AddReturn << endl;

    TClonesArray *ToF400Digits = new TClonesArray("BmnTof1Digit");
    eveTree->SetBranchAddress("TOF400", &ToF400Digits);

    TClonesArray *T0Digits = new TClonesArray("BmnTrigDigit");
    eveTree->SetBranchAddress("BC2", &T0Digits);

    TClonesArray *EventHeaderDig = new TClonesArray("BmnEventHeader");
    eveTree->SetBranchAddress("EventHeader", &EventHeaderDig);

    Long64_t nEvents = eveTree->GetEntries();
    if (nEvForRead == 0 || nEvForRead > nEvents) nEvForRead = nEvents;
    cout << "Will be read " << nEvForRead << " events from " << nEvents << endl;

    //eveTree->Print();

    // Read digi tree
    for (Long64_t iEv = 0; iEv < nEvForRead; iEv++) {

        ToF400Digits->Delete();
        T0Digits->Delete();
        EventHeaderDig->Delete();

        if (iEv % 10000 == 0) cout << "#EVENT: " << iEv << endl;
        //        cout << "Read event " << iEv << endl;
        eveTree->GetEntry(iEv);

        BmnEventHeader* digHeader = (BmnEventHeader*) EventHeaderDig->At(0);
        EventHeader_EventID = digHeader->GetEventId();
        if (iEv % 10000 == 0) cout << "#EVENTHeader: " << EventHeader_EventID << endl;

        //        cout << "Check T0 digits " << endl;
        Int_t CountMod0 = 0, CountMod1 = 0;
        for (Int_t i = 0; i < T0Digits->GetEntriesFast(); i++) {
            BmnTrigDigit* digT0 = (BmnTrigDigit*) T0Digits->At(i);
            if (digT0->GetMod() == 0) CountMod0++;
            if (digT0->GetMod() == 1) CountMod1++;
        }

        if (CountMod0 == 1 && CountMod1 >= 0) // check T0
        {
            //            cout << "Looking for right T0 digit" << "; nDigits = " << T0Digits->GetEntriesFast() << endl;
            BmnTrigDigit* digT0 = NULL;
            for (Int_t i = 0; i < T0Digits->GetEntriesFast(); i++) {
                digT0 = (BmnTrigDigit*) T0Digits->At(i);
                if (digT0->GetMod() == 0) break; // take first T0 digit. needed for ToF calculation.
            }
            if (digT0->GetAmp() >= 19.26 && digT0->GetAmp() <= 22.06) {

                XTof->clear();
                YTof->clear();
                ZTof->clear();
                TimeTof->clear();  

                //--------------------------- RPC --------------------------------------------------
                for (Int_t i = 0; i < NDet; i++)
                    Plane[i]->Clear();

                //               cout << "Read TOF400 Digits" << "; NDigits = " << ToF400Digits->GetEntriesFast() << endl;
                for (Int_t iDig = 0; iDig < ToF400Digits->GetEntriesFast(); ++iDig) {
                    FlagHit = kFALSE;
                    BmnTof1Digit* digTof = (BmnTof1Digit*) ToF400Digits->At(iDig);
                    FlagHit = Plane[digTof->GetPlane()]->SetDigit(digTof);
                }

                //                cout << "Fiil vectors" << endl;
                TVector3 XYZ;
                XYZ.SetXYZ(0., 0., 0.);
                Double_t ToF = 0;
                FlagHit = kFALSE;
                for (Int_t i = 0; i < NDet; i++) {// loop over TOF400 Planes
                    Int_t nHits = Plane[i] -> FindHits(digT0);
                    //                    cout << "nHits in Plane " << i << " = " << nHits << endl;
                    if (nHits > 0) {
                        for (Int_t s = 0; s < 47; s++) {//loop over Plane's strips
                            XYZ.SetXYZ(0., 0., 0.);
                            ToF = 0;
                            FlagHit = kFALSE;
                            FlagHit = Plane[i]->GetXYZTime(s, &XYZ, &ToF);
                            if (FlagHit == kTRUE) {
                                //                                cout << "Plane = " << i << "; Strip = " << s << "; X=" << XYZ.X() << "; Y=" << XYZ.Y() << "; Z=" << XYZ.Z() << "; Time=" << ToF << endl;
                                XTof->push_back(XYZ.X());
                                YTof->push_back(XYZ.Y());
                                ZTof->push_back(XYZ.Z());
                                TimeTof->push_back(ToF);
                            }
                        } // end for (Int_t s = 0; s < 47; s++)
                    }
                } // end for (Int_t i = 0; i < NDet; i++)

                //                cout << "Fill output tree" << endl;
                treeHitIn->Fill();

            }// end if (digT0->GetAmp() >= 17.3 && digT0->GetAmp() <= 19.2)
        } // end if (CountMod0 == 1 && CountMod1 <= 0)
    } // end for (Int_t iEv = 0; iEv < nEvForRead; iEv++)

    cout << "End Data Analysis" << endl;
    cout << "Save data in " << outName << endl;
    FileHitOut->cd();
    treeHitIn->Write();
    FileHitOut->Close();

}
