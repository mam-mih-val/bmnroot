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
  TObjArray *canvas_array = new TObjArray;
  canvas_array->SetName ("Canvas");
  TObjArray *result_array = new TObjArray;
  result_array->SetName("Histo");
	
  int variantes = BmnHodoAddress::GetMaxFlatIndex();
  TH2F* profile_hist = new TH2F("profile", "profile", 18,0,18, 100,0,50);
  profile_hist->SetTitle("Hodoscope profile; Strip # ; Z^{2} ");
  profile_hist->SetStats(0);

  TFile *_file0 = TFile::Open(converted_root_file, "READONLY");
  TTree *tree = (TTree *)_file0->Get("bmndata");

  BmnEventHeader* eventHeader = nullptr;
  tree->SetBranchAddress("BmnEventHeader.", &eventHeader);

  TClonesArray *DigiArray = nullptr; //leave nullptr!!!
  tree->SetBranchAddress("HodoDigi", &DigiArray);
  
  TClonesArray *ScWallDigiArray = nullptr; //leave nullptr!!!
  tree->SetBranchAddress("ScWallDigi", &ScWallDigiArray);

  double ScWallCtrEnergy = 0.;
  int counter = 0;
  int n_events = (max_entries>0 && tree->GetEntries()>max_entries)? max_entries : tree->GetEntries();
  cout << n_events << endl;
  for (int ev = 0; ev < n_events; ev++) ////nt1->GetEntries() //get entries from .root
  {
    tree->GetEntry(ev);
    if (ev%10000 == 0) cout << "Event: " << ev << endl;

    auto TrigPattern = eventHeader->GetInputSignalsAR();
    auto BT = !(TrigPattern ^ (1 << 7)); 
    auto ArmAnd = !(TrigPattern ^ (1 << 2)); 
    auto ArmOr = !(TrigPattern ^ (1 << 3)); 
    if(!ArmOr) continue;
    
    ScWallCtrEnergy = 0.;
    for (int i = 0; i < ScWallDigiArray->GetEntriesFast(); i++)
    {
        BmnScWallDigi *ThisDigi = (BmnScWallDigi *)ScWallDigiArray->At(i);

        //##################################
        //SELECTION
        if(ThisDigi->fFitR2 > 0.5) continue;
        auto cell_id = ThisDigi->GetCellId();
        if(cell_id != 17 && cell_id != 18 && cell_id != 27 && cell_id != 28 ) continue; // MF = 1650
        ScWallCtrEnergy += ThisDigi->GetSignal();

    }
    //if(ScWallCtrEnergy < 40) continue;
    
    float responses[kNstrips+1][kNsides];
    memset(responses, 0, sizeof(responses[0][0]) * (kNstrips+1) * kNsides);
    for (int i = 0; i < DigiArray->GetEntriesFast(); i++)
    {
        BmnHodoDigi *ThisDigi = (BmnHodoDigi *)DigiArray->At(i);

        //##################################
        //SELECTION
        //if(ThisDigi->GetStripId() == 9) continue;
        if(ThisDigi->fFitR2 > 0.4) continue;
        if(ThisDigi->fTimeMax > 28 || ThisDigi->fTimeMax < 25) continue;
        //##################################

        auto strip = ThisDigi->GetStripId();
        auto side = ThisDigi->GetStripSide();
        auto gain = ThisDigi->GetGain();
        auto idx = make_flat_index(strip,side,gain);

        if(gain != 1) continue;
        responses[strip][side] = ThisDigi->GetSignal();       
    }

    for (int i = 1; i <= kNstrips; i++)
        if(responses[i][0] * responses[i][1] > 0.001)
        profile_hist->Fill(i, (responses[i][0] + responses[i][1]));
  }

  TCanvas* prof = new TCanvas("c_prof","c_prof",800,800);
  canvas_array->Add(prof);
  result_array->Add(profile_hist);
  profile_hist->Draw("colz");
  
  for (int i = 1; i <= kNstrips; i++){
    TCanvas *canv_ptr = new TCanvas();
    canvas_array->Add(canv_ptr);
    auto hist_ptr = profile_hist->ProjectionY(Form("projection_strip%i", i), i, i+1);
    hist_ptr->SetTitle(Form("strip%i; Z^{2}; Counts", i));
    result_array->Add(hist_ptr);
    hist_ptr->Draw();
  }
    
  TFile *result_file = new TFile((result_file_name + ".root").Data(), "RECREATE");
  TIter nx_iter((TCollection*)(result_array));
  TObject* obj_ptr;
  while ( (obj_ptr=(TObjArray*)nx_iter()) )
  {
    if(obj_ptr == nullptr) continue; 
    printf("Writing %s object\n", obj_ptr->GetName());
    obj_ptr->Write();
  }
  printf("file %s was written\n", (result_file_name + ".root").Data());
  result_file->Close();
  delete result_file;

	for (Int_t i = 0; i < canvas_array->GetLast (); i++) 
	    ((TCanvas*) canvas_array->At(i))->SaveAs ((result_file_name + ".pdf(").Data ());
	((TCanvas*) canvas_array->At (canvas_array->GetLast ()))->SaveAs ((result_file_name + ".pdf)").Data ());
	printf("file %s was written\n", (result_file_name + ".pdf").Data());
    
}
