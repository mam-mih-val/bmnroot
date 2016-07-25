#include "../db_structures.h"

// macro for getting 'noise channels' parameter value
// example runs for all run numbers from 12 to 688 (if run number is not present then error "table record wasn't found" wiil arise - it's ok)
void histo_noise()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    TCanvas* c = new TCanvas("c", "Noise channels", 0, 0, 640, 480);
    //TGraph2D* gr2 = new TGraph2D();
    TH3I* histo =  new TH3I("h3","Noise channels",20,1,20,70,1,70,689,0,688);
    histo->SetMarkerColor(kBlue);
    histo->GetXaxis()->SetTitle("Slot");
    histo->GetYaxis()->SetTitle("Channel");
    histo->GetZaxis()->SetTitle("Run №");

    UniqueRunNumber* run_numbers;
    int run_count = UniDbRun::GetRunNumbers(1, 12, 3, 688, run_numbers);
    if (run_count <= 0)
        return;

    for (int i = 0; i < run_count; i++)
    {
        // get noise parameter values presented by IIStructure: Int+Int (slot:channel)
        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("DCH1", "noise", run_numbers[i].period_number, run_numbers[i].run_number); //(detector_name, parameter_name, period_number, run_number)
        if (pDetectorParameter != NULL)
        {
            IIStructure* pValues;
            int element_count = 0;
            pDetectorParameter->GetIIArray(pValues, element_count);

            cout<<"Element count: "<<element_count<<" for run "<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<" (period:run)"<<endl;
            for (int j = 0; j < element_count; j++)
                histo->Fill(pValues[j].int_1, pValues[j].int_2, run_numbers[i].run_number);

            // clean memory after work
            delete pValues;
            delete pDetectorParameter;
        }
    }

    delete run_numbers;

    histo->Draw();
}
