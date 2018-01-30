#include "../../UniDbParser.h"

//#define ScaleBorderInfo

// print summary information about beam spill for a given 'run' in a 'period'
// if 'run' parameter is zero or absent then all runs in the period will be shown
void run_beam_info(int period, int run = 0)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // get spill info
    UniDbParser parser;
    int res_code;
    vector<BeamSpillStructure*> beam_spill = parser.ParseTxt2Struct("../../macros/parse_schemes/summary.txt", res_code);
    if (res_code > 0)
    {
        cout<<"Macro finished with errors: parser error #"<<res_code<<endl;
        exit(-1);
    }

    UniqueRunNumber* run_numbers = NULL;
    int run_count = 1;
    if (run == 0)
    {
        run_count = UniDbRunPeriod::GetRunNumbers(period, run_numbers);
        if (run_count <= 0)
        {
            cout<<"Macro finished with errors: no runs exists in the given period #"<<period<<endl;
            exit(-2);
        }
    }

    for (int i = 0; i < run_count; i++)
    {
        if (run_numbers != NULL) run = run_numbers[i].run_number;

        // get run time
        UniDbRun* pRun = UniDbRun::GetRun(period, run);
        if (pRun == NULL)
        {
            cout<<"Macro finished with errors: no experimental run was found for given numbers"<<endl;
            exit(-3);
        }

        TDatime dtStart = pRun->GetStartDatetime();
        TDatime* dateEnd = pRun->GetEndDatetime();
        if (dateEnd == NULL)
        {
            cout<<"Macro finished with errors: no end datetime in the database for this run"<<endl;
            delete pRun;
            exit(-4);
        }
        TDatime dtEnd = *dateEnd;
        delete pRun;

        Long64_t sumBeamDaq = 0, sumBeamAll = 0, sumTriggerDaq = 0, sumTriggerAll = 0;
        BeamSpillStructure* stPrevious;
        bool isFound = false;
        for (int ind = 0; ind < beam_spill.size(); ind++)
        {
            BeamSpillStructure* st = beam_spill.at(ind);
            if (st->spill_end > dtEnd)
            {
                if (isFound)
                {
                    int curBeamDaq = 0, curBeamAll = 0, curTriggerDaq = 0, curTriggerAll = 0;
                    stPrevious = beam_spill.at(ind - 1);
                    curBeamDaq = st->beam_daq; curBeamAll = st->beam_all;
                    curTriggerDaq = st->trigger_daq, curTriggerAll = st->trigger_all;
                    //cout<<"Spill End: "<<st->spill_end.AsSQLString()<<". Beam DAQ: "<<st->beam_daq<<". Beam All: "<<st->beam_all<<". Trigger DAQ: "<<st->trigger_daq<<". Trigger All: "<<st->trigger_all<<endl;

                    #ifdef ScaleBorderInfo
                    // scale values
                    int sec1 = st->spill_end.Convert() - stPrevious->spill_end.Convert();
                    int sec2 = dtEnd.Convert() - stPrevious->spill_end.Convert();
                    double sec_ratio = sec2 / sec1;
                    curBeamDaq = TMath::Nint(curBeamDaq * sec_ratio); curBeamAll = TMath::Nint(curBeamAll * sec_ratio);
                    curTriggerDaq = TMath::Nint(curTriggerDaq * sec_ratio), curTriggerAll = TMath::Nint(curTriggerAll * sec_ratio);
                    #endif

                    sumBeamDaq += curBeamDaq; sumBeamAll += curBeamAll;
                    sumTriggerDaq += curTriggerDaq; sumTriggerAll += curTriggerAll;
                }
                break;
            }

            if (st->spill_end > dtStart)
            {
                int curBeamDaq = 0, curBeamAll = 0, curTriggerDaq = 0, curTriggerAll = 0;
                if (ind > 0)
                {
                    stPrevious = beam_spill.at(ind - 1);
                    curBeamDaq = st->beam_daq; curBeamAll = st->beam_all;
                    curTriggerDaq = st->trigger_daq, curTriggerAll = st->trigger_all;
                    //cout<<"Spill End: "<<st->spill_end.AsSQLString()<<". Beam DAQ: "<<st->beam_daq<<". Beam All: "<<st->beam_all<<". Trigger DAQ: "<<st->trigger_daq<<". Trigger All: "<<st->trigger_all<<endl;

                    if (!isFound)
                    {
                        isFound = true;

                        #ifdef ScaleBorderInfo
                        // scale values
                        int sec1 = st->spill_end.Convert() - stPrevious->spill_end.Convert();
                        int sec2 = st->spill_end.Convert() - dtStart.Convert();
                        double sec_ratio = sec2 / sec1;
                        curBeamDaq = TMath::Nint(curBeamDaq * sec_ratio); curBeamAll = TMath::Nint(curBeamAll * sec_ratio);
                        curTriggerDaq = TMath::Nint(curTriggerDaq * sec_ratio), curTriggerAll = TMath::Nint(curTriggerAll * sec_ratio);
                        #endif
                    }
                }

                sumBeamDaq += curBeamDaq; sumBeamAll += curBeamAll;
                sumTriggerDaq += curTriggerDaq; sumTriggerAll += curTriggerAll;
            }//if (st->spill_end > dtStart)
        }

        cout<<endl<<"Run #"<<run<<endl
           <<"sumBeamDaq: "<<sumBeamDaq<<". sumBeamAll: "<<sumBeamAll<<". sumTriggerDaq: "<<sumTriggerDaq<<". sumTriggerAll: "<<sumTriggerAll<<endl<<endl;
    }

    cout<<"Macro finished successfully"<<endl;
}
