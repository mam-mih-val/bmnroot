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

#define kNmodules 55


void fhcal_profile(TString converted_root_file, TString result_file_name, int max_entries)
{

    TObjArray *canvas_array = new TObjArray;
	canvas_array->SetName ("Canvas");

    TH1F* calo_summ = new TH1F("calo_summ", "calo_summ", 300,0,300);
    calo_summ->SetTitle(Form("%s; Energy [mip]; Counts", result_file_name.Data()));

    TH2F* h2_calo = new TH2F("calo_2d", "calo_2d", 50,0,1500, 60,0,1200);
    h2_calo->SetTitle(Form("%s; x [mm]; y [mm]; Average signal [mip]", result_file_name.Data()));
    TH2F* h2_text_center = new TH2F("text_center", "text_center", 50,0,1500, 60,0,1200);

    BmnFHCalRaw2Digit *Mapper = new BmnFHCalRaw2Digit();
    Mapper->ParseConfig("FHCAL_map_dry_run_2022.txt");
    auto ThatVectorX = Mapper->GetUniqueXpositions();
    auto ThatVectorY = Mapper->GetUniqueYpositions();
    auto ThatVectorZ = Mapper->GetUniqueZpositions();
    delete Mapper;

    std::vector<TH1F*> profile_hist_vect;
    profile_hist_vect.resize(kNmodules);
    for(int mod_it = 1; mod_it < kNmodules; mod_it++)
    	profile_hist_vect.at(mod_it) = new TH1F(Form("module_%i", mod_it),Form("module_%i", mod_it),10,0.5,10.5);
    	
    TFile *_file0 = TFile::Open(converted_root_file, "READONLY");
    TTree *tree = (TTree *)_file0->Get("bmndata");

    TClonesArray *DigiArray = nullptr; //leave nullptr!!!
    tree->SetBranchAddress("FHCalDigi", &DigiArray);

    TClonesArray *ScWallDigiArray = nullptr; //leave nullptr!!!
    tree->SetBranchAddress("ScWallDigi", &ScWallDigiArray);

    double SumEnergy = 0.;
    double ScWallCtrEnergy = 0.;

    int counter = 0;
    int n_events = (max_entries>0 && tree->GetEntries()>max_entries)? max_entries : tree->GetEntries();
    cout << n_events << endl;
    for (int ev = 0; ev < n_events; ev++) ////nt1->GetEntries() //get entries from .root
    {
        tree->GetEntry(ev);

        ScWallCtrEnergy = 0.;
        for (int i = 0; i < ScWallDigiArray->GetEntriesFast(); i++)
        {
            BmnScWallDigi *ThisDigi = (BmnScWallDigi *)ScWallDigiArray->At(i);

            //##################################
            //SELECTION
            if(ThisDigi->fFitR2 > 0.5) continue;
            auto cell_id = ThisDigi->GetCellId();
            if(cell_id != 14 && cell_id != 15 && cell_id != 24 && cell_id != 25) continue;
            ScWallCtrEnergy += ThisDigi->GetSignal();

        }
        if(ScWallCtrEnergy < 60) continue;

        float responses[kNmodules];
        memset(responses, 0, sizeof(responses[0]) * kNmodules);
        SumEnergy = 0.;
        for (int i = 0; i < DigiArray->GetEntriesFast(); i++)
        {
            counter++;
            BmnFHCalDigi *ThisDigi = (BmnFHCalDigi *)DigiArray->At(i);

            //##################################
            //SELECTION
            if(ThisDigi->fFitR2 > 0.2) continue;
            if(ThisDigi->fTimeMax <= 36 || ThisDigi->fTimeMax >= 39) continue;
            if(ThisDigi->fFitTimeMax <= 34 || ThisDigi->fFitTimeMax >= 39) continue;
            //##################################

            auto mod_id = ThisDigi->GetModuleId();
            //if(mod_id != 16 && mod_id != 11 && mod_id != 17 && mod_id != 20 && mod_id != 40) continue;
            //if(mod_id != 16) continue;
            auto sec_id = ThisDigi->GetSectionId();
            profile_hist_vect.at(mod_id)->Fill(sec_id, ThisDigi->GetSignal()); 

            float realXpos = ThatVectorX.at(ThisDigi->GetX());
            float realYpos = ThatVectorY.at(ThisDigi->GetY());
            float realZpos = ThatVectorZ.at(ThisDigi->GetZ());

            h2_calo->Fill(realXpos,realYpos,ThisDigi->GetSignal()); 
            SumEnergy += ThisDigi->GetSignal();

            auto ibin = h2_text_center->FindBin(realXpos, realYpos+50);
            if(h2_text_center->GetBinContent(ibin) < 1)
                h2_text_center->Fill(realXpos, realYpos+50, ThisDigi->GetModuleId());


        }
        if(SumEnergy > 0.1)
            calo_summ->Fill(SumEnergy);

    }


    for(int mod_it = 1; mod_it < kNmodules; mod_it++)
        profile_hist_vect.at(mod_it)->Scale(1./n_events);

    TCanvas* canv_summ = new TCanvas;
    canvas_array->Add(canv_summ);
    canv_summ->SetLogy();
    calo_summ->Draw();

    TCanvas* canv_2d = new TCanvas;
    gPad->SetRightMargin(0.15);
    canvas_array->Add(canv_2d);
    h2_calo->SetBit(TH1::kNoStats);
    h2_calo->Scale(1./n_events);
    h2_calo->Draw("colz");
    h2_text_center->SetMarkerSize(1.3);
    h2_text_center->Draw("text same");

/*
    TCanvas* canv_all = new TCanvas;
    canv_all->DivideSquare(kNmodules+1);
    canvas_array->Add(canv_all);

    std::vector<TCanvas*> canv_vect;
    canv_vect.resize(kNmodules);
    for(int mod_it = 1; mod_it < kNmodules; mod_it++){
        canv_vect.at(mod_it) = new TCanvas;
        canvas_array->Add(canv_vect.at(mod_it));
        profile_hist_vect.at(mod_it)->SetTitle( Form("module_%i; # sec; Average signal [mip]", mod_it) );
        //profile_hist_vect.at(mod_it)->GetYaxis()->SetRangeUser(0,10);
        profile_hist_vect.at(mod_it)->Draw("hist");

        canv_all->cd(mod_it);
        profile_hist_vect.at(mod_it)->Draw("hist");
    }
*/

	for (Int_t i = 0; i < canvas_array->GetLast (); i++) 
	    ((TCanvas*) canvas_array->At(i))->SaveAs ( (result_file_name + ".pdf(").Data() );
	((TCanvas*) canvas_array->At (canvas_array->GetLast ()))->SaveAs ( (result_file_name + ".pdf)").Data() );
  	printf("file %s was written\n", (result_file_name + ".pdf").Data() );
}
