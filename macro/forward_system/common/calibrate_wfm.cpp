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


void calibrate_wfm(TString converted_root_file, TString branch_name, TString calib_file, int max_events_show, int max_entries)
{
    //##################################
    const int model_order = 2;
    const int exponents = 2;
    //##################################

    std::vector<TH2F*> harmHistVect; 
    harmHistVect.resize(exponents);
    for(int i =0; i < exponents; i++)
        harmHistVect.at(i) = new TH2F(Form("harmonic%i", i+1),Form("harmonic%i", i+1), 100,0,1,100,-1,1);

    TFile *_file0 = TFile::Open(converted_root_file, "READONLY");
    TTree *tree = (TTree *)_file0->Get("bmndata");

    TClonesArray *DigiArray = nullptr; //leave nullptr!!!
    tree->SetBranchAddress(branch_name, &DigiArray);

    // TODO think about parameter handling
    BmnHodoRaw2Digit *Mm = new BmnHodoRaw2Digit();
    auto digiPars = Mm->GetDigiPars();
    delete Mm;
    if (branch_name.Contains("ScWall")) {
        BmnScWallRaw2Digit *Mapper = new BmnScWallRaw2Digit();
        Mapper->ParseCalibration(calib_file);
        digiPars = Mapper->GetDigiPars();
        delete Mapper;
    }
    else if (branch_name.Contains("FHCal")) {
        BmnFHCalRaw2Digit *Mapper = new BmnFHCalRaw2Digit();
        Mapper->ParseCalibration(calib_file);
        digiPars = Mapper->GetDigiPars();
        delete Mapper;
    }
    else if (branch_name.Contains("Hodo")) {
        BmnHodoRaw2Digit *Mapper = new BmnHodoRaw2Digit();
        Mapper->ParseCalibration(calib_file);
        digiPars = Mapper->GetDigiPars();
        delete Mapper;
    }

    int counter = 0;
    int n_events = (tree->GetEntries() > max_entries)? max_entries : tree->GetEntries();
    cout << n_events << endl;
    for (int ev = 0; ev < n_events; ev++) ////nt1->GetEntries() //get entries from .root
    {
        tree->GetEntry(ev);
        for (int i = 0; i < DigiArray->GetEntriesFast(); i++)
        {
            BmnDigiContainerTemplate *ThisDigi = (BmnDigiContainerTemplate *)DigiArray->At(i);
            auto wfm = ThisDigi->GetWfm();
            if(wfm.empty()) continue;

            //##################################
            //SELECTION
            //if(BmnHodoAddress::GetStripId(ThisDigi->GetAddress()) == 9) continue;
            //if(BmnScWallAddress::GetCellId(ThisDigi->GetAddress()) == 29) continue;
            //if(BmnScWallAddress::GetCellId(ThisDigi->GetAddress()) == 40) continue;
            //if(BmnScWallAddress::GetCellId(ThisDigi->GetAddress()) == 112) continue;
            //##################################

            PsdSignalFitting::PronyFitter Pfitter(model_order, exponents, digiPars.gateBegin, digiPars.gateEnd);
            //Pfitter.SetDebugMode(1);
            Pfitter.SetWaveform(wfm, ThisDigi->fZL);
            int best_signal_begin = Pfitter.ChooseBestSignalBeginHarmonics(digiPars.gateBegin, ThisDigi->fTimeMax);
            complex<float> *harmonics;
            Pfitter.SetSignalBegin(best_signal_begin);
            Pfitter.CalculateFitHarmonics();
            Pfitter.CalculateFitAmplitudes();
            harmonics = Pfitter.GetHarmonics();
            Float_t fit_integral = Pfitter.GetIntegral(digiPars.gateBegin, digiPars.gateEnd);
            Float_t fit_chi2 = Pfitter.GetChiSquare(digiPars.gateBegin, digiPars.gateEnd, ThisDigi->fTimeMax);
            Float_t fit_R2 = Pfitter.GetRSquare(digiPars.gateBegin, digiPars.gateEnd);
            if(false) printf("fit integral %.0f integral %.0d chi2 %.1f R2 %.3f\n", fit_integral, ThisDigi->fIntegral, fit_chi2, fit_R2);
            auto fit_wfm = Pfitter.GetFitWfm();

            if(fit_R2<0.05)
            {
                counter++;
                for(int i =0; i < exponents; i++)
                    harmHistVect.at(i)->Fill(real(harmonics[i+1]), imag(harmonics[i+1]));

                //##################################
                TString signal_name = Form("channel %i fit_integral %.0f integral %.0d chi2 %.1f fit_R2 %.3f", 
                                    BmnFHCalAddress::GetModuleId(ThisDigi->GetAddress()), fit_integral, ThisDigi->fIntegral, fit_chi2, fit_R2);
                //##################################                    
                                    
                if(counter > max_events_show) continue;
                TCanvas *canv_ptr = new TCanvas();
                std::vector<float> points(wfm.size());
                std::iota(std::begin(points), std::end(points), 0); // Fill with 0, 1, ..., wfm.back().
                TGraph *tgr_ptr = new TGraph(wfm.size(), &points[0], &wfm[0]);
                tgr_ptr->SetTitle(signal_name.Data());
                tgr_ptr->Draw();
                if(!fit_wfm.empty()){
                    TGraph *tgr_ptr_fit = new TGraph(fit_wfm.size(), &points[0], &fit_wfm[0]);
                    tgr_ptr_fit->SetLineColor(kRed);
                    tgr_ptr_fit->SetLineWidth(2);
                    tgr_ptr_fit->Draw("same");
                }

            }

        }
    }

    for(int i =0; i < exponents; i++){
        TCanvas *canv_ptr = new TCanvas();
        harmHistVect.at(i)->Draw("colz");
    }

}
