/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



#include <stdio.h>
#include <fstream>
#include <istream>
#include <iostream>
#include <ctime>
#include <vector>

#include <TMath.h>
#include "TChain.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TList.h"
#include "TDirectory.h"
#include "TPad.h"
#include "TClonesArray.h"
#include "TVector3.h"

void Tof400Identification(TString NameFileIn = "bmndst.root", Int_t nEvForRead = 0) {

    TStopwatch timer;
    timer.Start();
    Int_t nBytes = 0;

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    //gStyle->SetOptStat(kFALSE);

    TH1I *hBetta = new TH1I("hBetta", "hBetta", 400, -2., 2.);
    hBetta->GetXaxis()->SetTitle("beta ");
    hBetta->SetMarkerStyle(8);
    hBetta->SetMarkerColor(2);

    TH2I *hMomentumBetta = new TH2I("hMomentumBeta", "hMomentumBeta", 1000, -10., 10., 400, -2., 2.);
    hMomentumBetta->GetXaxis()->SetTitle("momentum, GeV/c ");
    hMomentumBetta->GetYaxis()->SetTitle("beta ");

    //--------Data Analysis-----------------------------------------------------------------

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("$VMCWORKDIR/macro/run/%s", NameFileIn.Data());
    //inName = file;
    cout << "Open file " << inName << endl << endl;
    eveTree->Add(inName);

    TClonesArray *Tof400Hits;
    eveTree->SetBranchAddress("BmnTof400Hit", &Tof400Hits);

    TClonesArray *BmnGlobTr;
    eveTree->SetBranchAddress("BmnGlobalTrack", &BmnGlobTr);

    Long64_t nEvents = eveTree->GetEntries();
    if (nEvForRead == 0 || nEvForRead > nEvents) nEvForRead = nEvents;
    cout << "Will be read " << nEvForRead << " events from " << nEvents << endl;

    for (Int_t iEv = 0; iEv < nEvForRead; iEv++) {

        if (iEv % 50000 == 0)
            cout << "EVENT: " << iEv << endl;
        nBytes = eveTree->GetEntry(iEv);

        for (Int_t iGlobTrack = 0; iGlobTrack < BmnGlobTr->GetEntriesFast(); iGlobTrack++) {
            BmnGlobalTrack* Track = (BmnGlobalTrack*) BmnGlobTr->At(iGlobTrack);
            FairTrackParam parLast(*(Track->GetParamLast()));
            FairTrackParam parFirst(*(Track->GetParamFirst()));
            Int_t IndexTof1Hit = Track->GetTof1HitIndex();
            if (IndexTof1Hit == -1) continue;
            //cout << "Event#" << iEv << endl;
            BmnTofHit* hitTof400 = (BmnTofHit*) Tof400Hits->At(IndexTof1Hit);
            Double_t Velocity = hitTof400->GetLength() / hitTof400->GetTimeStamp() / 10.; // * 10^8 m/s
            Double_t Betta = Velocity / 2.99792458; // 
            hBetta->Fill(Betta);
            Double_t Momentum = 1. / Track->GetParamLast()->GetQp();
            hMomentumBetta ->Fill(Momentum, Betta);

        } // end for (Int_t iGlobTrack = 0; iGlobTrack < BmnGlobalTrack->GetEntriesFast(); iGlobTrack++) 
    }// end for (Int_t iEv = 0; iEv < nEvForRead; iEv++)

    timer.Stop();
    cout << "Readed " << nEvForRead << " events" << endl;
    cout << "Time          = " << timer.RealTime() << " s" << endl;
    cout << "Time/Event    = " << timer.RealTime() / (Double_t) nEvForRead * 1000. << " ms/Event" << endl;
    cout << "TimeCPU       = " << timer.CpuTime() << " s" << endl;
    cout << "TimeCPU/Event = " << timer.CpuTime() / (Double_t) nEvForRead * 1000. << " ms/Event" << endl;

    timer.Reset();
    timer.Start();
    TString outName = NameFileIn;
    Int_t Point = outName.First('.');
    outName.Replace(Point, 15, "_Identification.root");
    outName = Form("/home/storage/analysis/%s", outName.Data());
    cout << "Save data to " << outName.Data() << endl;
    TFile *fileout = new TFile(outName.Data(), "RECREATE");

    hBetta->Write();
    hMomentumBetta->Write();

    fileout->Close();

    timer.Stop();

    cout << "Time for write root = " << timer.RealTime() << " s" << endl;

}
