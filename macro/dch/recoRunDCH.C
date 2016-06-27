#include "TSpectrum.h"

using namespace TMath;

void recoRunDCH() {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    Int_t bmn_event,bmn_run;
    //UInt_t EventSelect=250000;
    //UInt_t EventSelect=30000;
    UInt_t EventSelect=10000;
    //UInt_t EventSelect=5000;
    //UInt_t EventSelect=1000;
    UInt_t ievProcessed=0;
    Char_t* runname="bmn_run0166_digit.root";
    //Char_t* runname="bmn_run0607_digit.root";
    Char_t runnumber[5]; 
    Char_t* firstdigit=strchr(runname,'0'); 
    strlcpy(runnumber,firstdigit,5);
    Int_t runNumber=atoi(runnumber);
    TString runtype;

    assert((runNumber>=12&&runNumber<=188)||(runNumber>=220&&runNumber<=395)||(runNumber>=403&&runNumber<=688));

    if (runNumber>=12&&runNumber<=188){
      runtype="run1"; 
    }else if (runNumber>=220&&runNumber<=395){
      runtype="run2"; 
    }else if (runNumber>=403&&runNumber<=688){
      runtype="run3"; 
    }else if (runNumber>=689){
      runtype="run4";
    }


    cout<<"Run type = "<<runtype<<", run number = "<<runNumber<<endl;

    TChain *bmnTree = new TChain("BMN_DIGIT");
    bmnTree->Add(runname);

    BmnDchDigit* digit = NULL;
    TClonesArray *dchDigits;
    bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);

    //TClonesArray* dchHits = new TClonesArray("BmnDchHit");
    TFile* fReco = new TFile("bmndst_test.root", "RECREATE");
    TTree* tReco = new TTree("cbmexp", "test_bmn");
    Int_t nevents = bmnTree->GetEntries();
    cout << "nevents = " << nevents << endl;
    //BmnDchHitProducer_exp  *dchexp = new BmnDchHitProducer_exp(iDch,0,true,runtype);
    BmnDchHitProducer_exp  *dchexp1 = new BmnDchHitProducer_exp(1,0,false,runtype);
    BmnDchHitProducer_exp  *dchexp2 = new BmnDchHitProducer_exp(2,0,false,runtype);
    //dchexp->InitDch(bmnTree,dchDigits,tReco);
    dchexp1->InitDch(bmnTree,dchDigits,tReco);
    dchexp2->InitDch(bmnTree,dchDigits,tReco);

    //TClonesArray *dchHits1,*dchHits2; 
    //BmnDchHit *dchHit;

    for (Int_t iev = 0; iev < nevents; iev++) {
        bmnTree->GetEntry(iev);
        if(iev%EventSelect==0){
         cout << "event number = " << iev << endl;
         ievProcessed++;
        }
        if(iev%EventSelect==0)dchexp1->ExecDch(iev,dchDigits);
        if(iev%EventSelect==0)dchexp2->ExecDch(iev,dchDigits);
        //if(iev==20000)dchexp1->ExecDch(iev,dchDigits);
        //if(iev==20000)dchexp2->ExecDch(iev,dchDigits);
        //dchexp->ExecDch(iev,dchDigits);
        /*if(iev%EventSelect==0)dchexp1->ExecDch(iev,dchDigits,dchHits1);
        if(iev%EventSelect==0)dchexp2->ExecDch(iev,dchDigits,dchHits2);
        if(iev%EventSelect==0){
         Int_t nentries=dchHits1->GetEntries();   
         for (Int_t ihit = 0; ihit < nentries; ihit++) {
           dchHit = (BmnDchHit*) dchHits1->UncheckedAt(ihit);
           //cout<<"layer1 = "<<dchHit->GetLayer()<<endl;
         }
         cout<<"nentries = "<<nentries<<endl;
         delete dchHits1;
         Int_t nentries=dchHits2->GetEntries();   
         cout<<"nentries = "<<nentries<<endl;
         delete dchHits2;
        }*/
        tReco->Fill();
        //dchHits1->Clear(); 
        //dchHits2->Clear(); 
    } // event loop


    //dchexp->FinishDch();
    dchexp1->FinishDch();
    dchexp2->FinishDch();
    fReco->cd();
    tReco->Write();
    fReco->Close();
    //delete dchexp;
    delete dchexp1;
    delete dchexp2;
 

 
    cout<<"Number of events processed: "<<ievProcessed<<endl; 
}
