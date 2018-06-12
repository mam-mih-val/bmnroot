// @(#)bmnroot/macro/miscellaneous:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-06-12

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnDataSelector                                                            //
//                                                                            //
// A useful macro to select data either for alignment (TString data = "align")//
// or phys. analysis (TString data = "phys")                                  //
//                                                                            // 
// The  BM@N UniDb used                                                       //
////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <Rtypes.h>
#include <TString.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;

void BmnDataSelector(Int_t run_period = 7, TString data = "align") {
    // ----  Load libraries   --------------------------------------------------
    bmnloadlibs(); // load BmnRoot libraries

    const Int_t nTargets = 5;
    const TString targets[nTargets] = {"C", "Al", "Cu", "Sn", "Pb"};

    const Int_t nEne = 4;
    const Double_t energies[nEne] = {2.3, 2.6, 2.94, 3.2};

    vector <Double_t> ene;
    vector <TString> targ;
    vector <Int_t> run;
    vector <Double_t> apprCurrent;

    // BM@N file numbers ...
    const Int_t kStart = 3681;
    const Int_t kFinish = 5185;

    for (Int_t iRun = kStart; iRun < kFinish; iRun++) {
        // cout << iRun << endl;
        UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, iRun);
        if (pCurrentRun == NULL) {
            delete pCurrentRun;
            continue;
        }

        if (pCurrentRun->GetTargetParticle() == NULL ||
                pCurrentRun->GetEnergy() == NULL ||
                pCurrentRun->GetFieldVoltage() == NULL)
            continue;

        run.push_back(iRun);
        targ.push_back(*pCurrentRun->GetTargetParticle());
        ene.push_back(*pCurrentRun->GetEnergy());
        apprCurrent.push_back(*pCurrentRun->GetFieldVoltage());

        delete pCurrentRun;
    }

    if (data != "align" && data != "phys") {
        cout << "Specify correct arguments!!" << endl;
        throw;
    }
    const Int_t size = run.size();
    FILE* outFile = fopen(TString("file_" + data + "Events.txt").Data(), "w");
    fprintf(outFile, (data.Contains("align") ? "Files to be used for ALIGNMENT\n" : "Files to be used for PHYSICS\n"));
    fprintf(outFile, "\n");
    for (Int_t iEne = 0; iEne < nEne; iEne++) {
        fprintf(outFile, "T = %G GeV/n\n", energies[iEne]);
        fprintf(outFile, "------------------------------------------------------\n");

        TString buffs[nTargets];
        for (Int_t iEle = 0; iEle < nTargets; iEle++)
            buffs[iEle] = TString::Format("%s --> ", targets[iEle].Data());

        for (Int_t iSize = 0; iSize < size; iSize++) {
            // Get energy we are considering ...
            if (ene[iSize] != energies[iEne])
                continue;
            if (data.Contains("align")) {
                if (apprCurrent[iSize] > 5.)
                    continue;
            } else {
                if (apprCurrent[iSize] < 5.)
                    continue;
            }

            // Get target ...
            for (Int_t iTarg = 0; iTarg < nTargets; iTarg++)
                if (targ[iSize] == targets[iTarg])
                    buffs[iTarg] += TString::Format("%d ", run[iSize]);
        }

        for (Int_t iTarg = 0; iTarg < nTargets; iTarg++) {
            fprintf(outFile, "%s", buffs[iTarg].Data());
            fprintf(outFile, "\n\n");
        }
        fprintf(outFile, "\n");
    }
    fclose(outFile);

}
