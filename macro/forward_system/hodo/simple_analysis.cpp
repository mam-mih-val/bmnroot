#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TNtuple.h>
#include <TGraph.h>
#include <TF1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include "TCutG.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <stdio.h>
//#include "utils.h"
#include "TString.h"
#include "TH1D.h"
#include "TMultiGraph.h"
#include "TStyle.h"
#include "TH2.h"

#define kNstrips 16
#define kNsides 2
#define kNgains 2

int make_flat_index(uint32_t StripId, uint32_t StripSide, uint32_t Gain){
    uint32_t address = BmnHodoAddress::GetAddress(0, StripId, StripSide, Gain);
    return BmnHodoAddress::GetFlatIndex(address);
}

void simple_analysis(TString converted_root_file, int max_entries)
{
    int variantes = BmnHodoAddress::GetMaxFlatIndex();
    std::vector<TH1F*> signalHistVect; 
    signalHistVect.resize(variantes);

    for(uint32_t i =1; i < kNstrips+1; i++)
        for(uint32_t j =0; j < kNsides; j++)
            for(uint32_t k =0; k < kNgains; k++) {
                auto idx = make_flat_index(i,j,k);
                TString side = (j == 0)? "DOWN" : "UP";
                TString gain = (k == 0)? "LOW" : "HIGH";
                TString name = Form("strip%i side %s gain %s", i, side.Data(), gain.Data());
                signalHistVect.at(idx) = new TH1F(Form("variant%i", idx),name.Data(), 4000,0,32000);

                cout << idx << " " << name << endl;
            }



    TFile *_file0 = TFile::Open(converted_root_file, "READONLY");
    TTree *tree = (TTree *)_file0->Get("bmndata");

    TClonesArray *DigiArray = nullptr; //leave nullptr!!!
    tree->SetBranchAddress("HodoDigi", &DigiArray);

    int counter = 0;
    int n_events = tree->GetEntries();
    cout << n_events << endl;
    if(n_events < max_entries) max_entries = n_events;
    for (int ev = 0; ev < max_entries; ev++) ////nt1->GetEntries() //get entries from .root
    {
        tree->GetEntry(ev);
        for (int i = 0; i < DigiArray->GetEntriesFast(); i++)
        {
            BmnHodoDigi *ThisDigi = (BmnHodoDigi *)DigiArray->At(i);

            //##################################
            //SELECTION
            if(ThisDigi->GetStripId() == 9) continue;
            if(ThisDigi->fFitR2 > 0.3) continue;
            //##################################

            auto strip = ThisDigi->GetStripId();
            auto side = ThisDigi->GetStripSide();
            auto gain = ThisDigi->GetGain();
            auto idx = make_flat_index(strip,side,gain);

            //cout<<idx<<" "<<strip<<" "<<side<<" "<<gain<<" "<<ThisDigi->GetSignal()<<endl;

            signalHistVect.at(idx)->Fill(ThisDigi->GetSignal());
            
        }
    }

    for(uint32_t i =1; i < kNstrips+1; i++)
        for(uint32_t j =0; j < kNsides; j++)
            for(uint32_t k =0; k < kNgains; k++) {
                auto idx = make_flat_index(i,j,k);
                TCanvas *canv_ptr = new TCanvas();
                canv_ptr->SetLogy();
                signalHistVect.at(idx)->Draw();
            }

}
