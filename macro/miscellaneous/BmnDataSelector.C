// @(#)bmnroot/macro/miscellaneous:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2019-09-24

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnDataSelector                                                            //
//                                                                            //
// A useful macro to select data either for alignment (TString data = "align")//
// or phys. analysis (TString data = "phys")                                  //
// Valid for RUN6 amn 7  (BM@N, SRC)                                          // 
// The  BM@N UniDb used                                                       //
////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <Rtypes.h>
#include <TString.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;

void BmnDataSelector(Int_t run_period = 6, // 6 or 7
        TString setup = "BM@N", // BM@N or SRC
        TString data = "align") { // phys or align
    // ----  Load libraries   --------------------------------------------------
    bmnloadlibs(); // load BmnRoot libraries

    Int_t nTargets;
    TString* targets = nullptr;

    Int_t nEne;
    Double_t* energies = nullptr;

    Int_t kStart = -1;
    Int_t kFinish = -1;

    if (run_period == 7 && setup.Contains("BM@N")) {
        nTargets = 6;
        targets = new TString[nTargets];
        targets[0] = "C";
        targets[1] = "Al";
        targets[2] = "Cu";
        targets[3] = "Sn";
        targets[4] = "Pb";
        targets[5] = "-";

        nEne = 4;
        energies = new Double_t[nEne];
        energies[0] = 2.3;
        energies[1] = 2.6;
        energies[2] = 2.94;
        energies[3] = 3.2;

        kStart = 3589;
        kFinish = 5186;
    }
    else if (run_period == 7 && setup.Contains("SRC")) {
        nTargets = 6;
        targets = new TString[nTargets];
        targets[0] = "H2";
        targets[1] = "Pb";
        targets[2] = "SRC Lead 1";
        targets[3] = "SRC Lead 2";
        targets[4] = "SRC Lead 3";
        targets[5] = "-";

        nEne = 1;
        energies = new Double_t[nEne];
        energies[0] = 3.17;

        kStart = 2041;
        kFinish = 3589;

    } else {
        setup = "";

        nTargets = 6;
        targets = new TString[nTargets];
        targets[0] = "C";
        targets[1] = "Al";
        targets[2] = "Cu";
        targets[3] = "Pb";
        targets[4] = "C2H4";
        targets[5] = "-";

        nEne = 3;
        energies = new Double_t[nEne];
        energies[0] = 3.5;
        energies[1] = 4.0;
        energies[2] = 4.5;

        kStart = 1170;
        kFinish = 1993;
    }

    vector <Double_t> ene;
    vector <TString> targ;
    vector <Int_t> run;
    vector <Double_t> apprCurrent;

    for (Int_t iRun = kStart; iRun < kFinish; iRun++) {
        UniRun* pCurrentRun = UniRun::GetRun(run_period, iRun);
        if (pCurrentRun == NULL) {
            delete pCurrentRun;
            continue;
        }

        if (pCurrentRun->GetEnergy() == NULL ||
                pCurrentRun->GetFieldVoltage() == NULL)
            continue;

        run.push_back(iRun);
        if (pCurrentRun->GetTargetParticle())
            targ.push_back(*pCurrentRun->GetTargetParticle());
        else
            targ.push_back("-");
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

    delete [] targets;
    delete [] energies;
}
