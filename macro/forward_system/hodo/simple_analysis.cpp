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

void simple_analysis(TString converted_root_file, TString result_file_name, int max_entries)
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
                signalHistVect.at(idx) = new TH1F(Form("variant%i", idx),name.Data(), 100,0,4);

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
            if(ThisDigi->fTimeMax > 28 || ThisDigi->fTimeMax < 25) continue;
            //##################################

            auto strip = ThisDigi->GetStripId();
            auto side = ThisDigi->GetStripSide();
            auto gain = ThisDigi->GetGain();
            auto idx = make_flat_index(strip,side,gain);

            signalHistVect.at(idx)->Fill(ThisDigi->GetSignal());
            

            //if(strip == 4 && side == 1 && gain == 1) cout<<ThisDigi->GetSignal()/ThisDigi->fAmpl<<endl;
        }
    }

    BmnHodoRaw2Digit *Mapper = new BmnHodoRaw2Digit();
    Mapper->ParseCalibration("HODO_calibration_2022.txt");
    
	TObjArray *canvas_array = new TObjArray;
	canvas_array->SetName ("Canvas");
    TCanvas *canv_total = new TCanvas("total", "total", 1600,1600);
    canv_total->DivideSquare(64);
    canvas_array->Add(canv_total);
    int cnter = 0;
    for(uint32_t i =1; i < kNstrips+1; i++)
        for(uint32_t j =0; j < kNsides; j++)
            for(uint32_t k =0; k < kNgains; k++) {
                cnter++;
                auto idx = make_flat_index(i,j,k);
                TCanvas *canv_ptr = new TCanvas();
                canv_ptr->SetLogy();
                canvas_array->Add(canv_ptr);
                //if(k == 0) continue;

                double range_low, range_high;
                //if(k == 0) { range_low = 500; range_high= 2000; }
                //if(k == 1) { range_low = 2000; range_high= 10000; }
                range_low = 0.5; range_high= 1.5;
                double mean, sigma;
                FitHistogrammSmartInMaxPeak(signalHistVect.at(idx), mean, sigma, 3, range_low, range_high);
                TString new_name = Form("%s MEAN %.5f", signalHistVect.at(idx)->GetTitle(), 1./mean);
                signalHistVect.at(idx)->SetTitle(new_name.Data());
                signalHistVect.at(idx)->Draw();

                canv_total->cd(cnter);
                signalHistVect.at(idx)->Draw();

                uint32_t address = BmnHodoAddress::GetAddress(0, i, j, k);
                auto calib_pair = Mapper->GetCalibPairFromAddress(address);
                printf("calib    =    S    %i    %i    %i    %.5f    0.0\n", i, j, k, 1.0/mean);
            }


	for (Int_t i = 0; i < canvas_array->GetLast (); i++) 
	    ((TCanvas*) canvas_array->At(i))->SaveAs ((result_file_name + ".pdf(").Data ());
	((TCanvas*) canvas_array->At (canvas_array->GetLast ()))->SaveAs ((result_file_name + ".pdf)").Data ());
  	printf("file %s was written\n", (result_file_name + ".pdf").Data());

}
