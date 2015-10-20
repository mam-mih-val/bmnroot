#include "../db_structures.h"

// macro for getting parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void histo_noise()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    TCanvas* c = new TCanvas("c", "Noise channels", 0, 0, 640, 480);
    //TGraph2D* gr2 = new TGraph2D();
    TH3I* histo =  new TH3I("h3","Noise channels",20,1,20,70,1,70,689,0,688);
    histo->SetMarkerColor(kBlue);
    //histo->GetXaxis()->SetTitle("Slot");
    //histo->GetYaxis()->SetTitle("Channel");
    //histo->GetZaxis()->SetTitle("Run №");

    for (int i = 12; i < 688; i++)
    {
        // get noise parameter values presented by IIStructure: Int+Int (slot:channel)
        UniDbRunParameter* pRunParameter = UniDbRunParameter::GetRunParameter(i, "DCH1", "noise"); //(run_number, detector_name, parameter_name)
        if (pRunParameter != NULL)
        {
            IIStructure* pValues;
            int element_count = 0;
            pRunParameter->GetIIArray(pValues, element_count);

            cout<<"Element count: "<<element_count<<" for run number: "<<i<<endl;
            for (int j = 0; j < element_count; j++)
            {
                histo->Fill(pValues[j].int_1, pValues[j].int_2, i);
            }

            // clean memory after work
            delete pValues;
            delete pRunParameter;
        }
    }

    histo->Draw();
}
