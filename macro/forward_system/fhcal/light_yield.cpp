#include <fstream>
#include <iostream>

#include <TDirectory.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>

#define kTotalModules 54


void light_yield()
{
    BmnFHCalRaw2Digit *Mapper = new BmnFHCalRaw2Digit();
    Mapper->ParseCalibration("FHCAL_calibration_2022.txt");

    TString led_files_path = "/home/nikolay/BMN/FHCal/LED/";


    double final_calib [kTotalModules+1][11];
    memset(final_calib, 0, sizeof(final_calib[0][0]) * (kTotalModules+1) * 11);
    for(uint32_t mod_iter = 1; mod_iter <=54; mod_iter++){

        TString led_calib_par_file = led_files_path + Form("mod%02d.txt", mod_iter);
        ifstream led_params_file(led_calib_par_file.Data(), std::ifstream::in);
        cout<<led_calib_par_file<<endl;
        vector<double> ledVec;
        if(led_params_file.is_open())
        {
            //Got to the last character before EOF
            led_params_file.seekg(-1, std::ios_base::end);
            if(led_params_file.peek() == '\n')
            {
            //Start searching for \n occurrences
            led_params_file.seekg(-1, std::ios_base::cur);
            int i = led_params_file.tellg();
            for(i;i > 0; i--)
            {
                if(led_params_file.peek() == '{')
                {
                //Found
                led_params_file.get();
                break;
                }
                //Move one character back
                led_params_file.seekg(i, std::ios_base::beg);
            }
            }

            ledVec.clear();
            ledVec.resize(10);
            std::string lastline;
            getline(led_params_file, lastline );
            stringstream ss( lastline );
            string result;

            int counter = 0;
            while( ss.good() )
            {
                getline(ss, result, ',' );
                cout<<result<<endl;

                ledVec.at(counter) = stod(result);
                counter++;
            }

            for(auto it : ledVec) cout<<it<<" ";
            cout<<endl;
        }

        for(uint32_t sec_iter = 1; sec_iter <=10; sec_iter++){
            uint32_t address = BmnFHCalAddress::GetAddress(0, mod_iter, sec_iter, 0, 0, 0);
            auto calib_pair = Mapper->GetCalibPairFromAddress(address);

            double mip_calib = calib_pair.first;

            cout<<mod_iter<<" "<<sec_iter<<" "<<mip_calib<<endl;
            if(mip_calib < 1e-6) mip_calib = 0.;
            else mip_calib = 1./mip_calib;

            final_calib [mod_iter][sec_iter] = mip_calib*ledVec.at(sec_iter-1);

            

        }



    }

    TH1 *th1_hist_ptr = nullptr;
    TCanvas *tcanv_ptr = nullptr;

    TObjArray *result_array = new TObjArray;
    result_array->SetName("calo_npe_calib");
    result_array->SetOwner();

    TObjArray *canvas_array = new TObjArray;
    canvas_array->SetName ("Canvas");

    for(Int_t module_iter = 1; module_iter <= kTotalModules; module_iter++)
    {
	TString module_name = Form("bmn_fhcal_module_%02d", module_iter);
	th1_hist_ptr = new TH1F(Form("Module_%s", module_name.Data()), 
                                Form("Module_%s", module_name.Data()), 
                                12, -0.5, 11.5);
        th1_hist_ptr->SetTitle(Form("%s; #section; Light yield [ph.e./mip]", module_name.Data()));
        th1_hist_ptr->SetMarkerStyle(23);
	result_array->Add(th1_hist_ptr);
    }


    for(Int_t module_iter = 1; module_iter <= kTotalModules; module_iter++)
    {
    TString module_name = Form("bmn_fhcal_module_%02d", module_iter);
	th1_hist_ptr = ((TH1*)(gDirectory->FindObjectAny( Form("Module_%s", module_name.Data()) )));
    for(int sec_iter = 1; sec_iter <= 10; sec_iter++)
	    th1_hist_ptr->Fill(sec_iter, final_calib [module_iter][sec_iter]);
    }

    TCanvas *canvas_ly = new TCanvas[kTotalModules+1];
    gStyle->SetOptStat(0);
    for(Int_t module_iter = 1; module_iter <= kTotalModules; module_iter++)
    {	
        tcanv_ptr = &canvas_ly[module_iter];
        tcanv_ptr->SetName(Form("c_light_yield_mod_%i", module_iter));
        tcanv_ptr->SetTitle(Form("c_light_yield_mod_%i", module_iter));
        canvas_array->Add(tcanv_ptr);
        tcanv_ptr->cd();

	TString module_name = Form("bmn_fhcal_module_%02d", module_iter);
	th1_hist_ptr = ((TH1*)(gDirectory->FindObjectAny( Form("Module_%s", module_name.Data()) )));
	th1_hist_ptr->GetYaxis()->SetRangeUser(0, 1.1*th1_hist_ptr->GetMaximum());
	th1_hist_ptr->Draw("hist P");
    }

    TString result_file_name = "resss";
    for (Int_t i = 0; i < canvas_array->GetLast (); i++) 
	((TCanvas*) canvas_array->At(i))->SaveAs ((result_file_name + ".pdf(").Data ());
    ((TCanvas*) canvas_array->At (canvas_array->GetLast ()))->SaveAs ((result_file_name + ".pdf)").Data ());
    printf("file %s was written\n", (result_file_name + ".pdf").Data());

    TFile *result_file = new TFile((result_file_name + ".root").Data(), "RECREATE");
    TString start_path = result_array->GetName();
    result_file->mkdir(start_path.Data());
    result_file->cd(start_path.Data());
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



}
