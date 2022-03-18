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
//#include "BmnHodoDigi.h"
#include "TClonesArray.h"

#include "../common/utilites.h"

#define kNstrips 16
#define kNsides 2
#define kNgains 2

int make_flat_index(uint32_t StripId, uint32_t StripSide, uint32_t Gain){
    uint32_t address = BmnHodoAddress::GetAddress(0, StripId, StripSide, Gain);
    return BmnHodoAddress::GetFlatIndex(address);
}

void hodo_profile(TString converted_root_file, TString result_file_name, int max_entries)
{
    int variantes = BmnHodoAddress::GetMaxFlatIndex();

    TH2F* profile_hist = new TH2F("profile", "profile", 20,0,20, 1000,0,5);


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
        float responses[17][2];
        memset(responses, 0, sizeof(responses[0][0]) * 17 * 2);
        for (int i = 0; i < DigiArray->GetEntriesFast(); i++)
        {
            BmnHodoDigi *ThisDigi = (BmnHodoDigi *)DigiArray->At(i);

            //##################################
            //SELECTION
            if(ThisDigi->GetStripId() == 9) continue;
            if(ThisDigi->fFitR2 > 0.3) continue;
            if(ThisDigi->fTimeMax > 28 || ThisDigi->fTimeMax < 25) continue;
            //##################################

            auto strip = ThisDigi->GetStripId();
            auto side = ThisDigi->GetStripSide();
            auto gain = ThisDigi->GetGain();
            auto idx = make_flat_index(strip,side,gain);

            if(gain != 1) continue;
            responses[strip][side] = ThisDigi->GetSignal();       
        }

        for (int i = 1; i <= 16; i++)
            if(responses[i][0] * responses[i][1] > 0.001)
            profile_hist->Fill(i, responses[i][0] + responses[i][1] );
    }

    TCanvas* prof = new TCanvas;
    profile_hist->Draw("colz");
}
