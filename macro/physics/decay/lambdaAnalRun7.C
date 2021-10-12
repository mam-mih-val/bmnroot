#include <Rtypes.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;
using namespace TMath;

// Lambda anal. relatively to path bins ...

void lambdaAnalRun7(TString tar, TString dirMc, TString dirData) {
    BmnLambdaAnalRun7* lAna = new BmnLambdaAnalRun7(dirMc, dirData, tar);
    //lAna->SetNFiles(10);

    vector <TString> targets{"Pb", "Sn", "Cu", "Al"};

    // Common cuts ...
    for (TString target : targets)
        lAna->SetCuts(target, 1.2, 0.3, 2.2, 0.7); // Path cut is not set!
    
    // Setting trigg. efficiencies (if available) ...
    /*
      lAna->SetTriggerEffData("BT+FD3", "eff_BT+FD3.root");
      lAna->SetTriggerEffData("BT+FD2", "eff_BT+FD2.root");
      lAna->SetTriggerEffData("BT+BD3", "eff_BT+BD3.root");
      lAna->SetTriggerEffData("BT+BD2", "eff_BT+BD2.root");
      lAna->SetTriggerEffData("BT+BD1+FD2", "eff_BT+BD1+FD2.root");
    */
    
    // Setting Pt and Y bins for lambda's ...
    vector <pair <Double_t, Double_t>> ptBins;
    
    ptBins.push_back(make_pair(0.1, 0.35));
    ptBins.push_back(make_pair(0.35, 0.6));
    ptBins.push_back(make_pair(0.6, 1.3));    
    lAna->SetPtBins(ptBins);

    vector <pair <Double_t, Double_t>> yBins;
    
    yBins.push_back(make_pair(0.5, 1.3));
    yBins.push_back(make_pair(1.3, 1.7));
    yBins.push_back(make_pair(1.7, 2.3));
    lAna->SetYBins(yBins);
    
    vector <pair <Double_t, Double_t>> pathBins;
    pathBins.push_back(make_pair(8., 50.));
    pathBins.push_back(make_pair(12., 50.));
    pathBins.push_back(make_pair(16., 50.));
    pathBins.push_back(make_pair(20., 50.));
    pathBins.push_back(make_pair(24., 50.));

    lAna->SetPathBins(pathBins);
    
    lAna->doTargetAnal(tar);

    delete lAna;
}

void lambdaAnalRun7(TString tar, TString dir, Bool_t isMc) {
    //
    BmnLambdaAnalRun7* lAna = new BmnLambdaAnalRun7(dir);
    lAna->SetIsMc(isMc);

    vector <TString> targets{"Pb", "Sn", "Cu", "Al"};

    // Common cuts ...
    for (TString target : targets)
        lAna->SetCuts(target, 1.2, 0.3, 2.2, 0.7, 12.); 

    // Setting trigg. efficiencies (if available) ...
    /*
      lAna->SetTriggerEffData("BT+FD3", "eff_BT+FD3.root");
      lAna->SetTriggerEffData("BT+FD2", "eff_BT+FD2.root");
      lAna->SetTriggerEffData("BT+BD3", "eff_BT+BD3.root");
      lAna->SetTriggerEffData("BT+BD2", "eff_BT+BD2.root");
      lAna->SetTriggerEffData("BT+BD1+FD2", "eff_BT+BD1+FD2.root");
    */

    // Setting Pt and Y bins for lambda's ...
    vector <pair <Double_t, Double_t>> ptBins;
   
    ptBins.push_back(make_pair(0.1, 0.35));
    ptBins.push_back(make_pair(0.35, 0.6));
    ptBins.push_back(make_pair(0.6, 1.3));    
    lAna->SetPtBins(ptBins);

    vector <pair <Double_t, Double_t>> yBins;
    
    yBins.push_back(make_pair(0.5, 1.3));
    yBins.push_back(make_pair(1.3, 1.7));
    yBins.push_back(make_pair(1.7, 2.3));
    lAna->SetYBins(yBins);

    lAna->doTargetAnal(tar);

    delete lAna;
}

// To be used for bin optimization if necessary ...

void lambdaAnalRun7(TString dataPath, Bool_t doBinOptimizationPt, Bool_t doBinOptimizationY, Double_t min, Double_t max) {
    BmnLambdaAnalRun7* lAna = new BmnLambdaAnalRun7(dataPath);
    lAna->SetNFiles(10);

    vector <TString> targets{"Pb", "Sn", "Cu", "Al"};

    // Common cuts ...
    for (TString target : targets)
        lAna->SetCuts(target, 1.2, 0.3, 2.2, 0.7, 12.);

    // Setting trigg. efficiencies ...
    /*
    lAna->SetTriggerEffData("BT+FD3", "eff_BT+FD3.root");
    lAna->SetTriggerEffData("BT+FD2", "eff_BT+FD2.root");
    lAna->SetTriggerEffData("BT+BD3", "eff_BT+BD3.root");
    lAna->SetTriggerEffData("BT+BD2", "eff_BT+BD2.root");
    lAna->SetTriggerEffData("BT+BD1+FD2", "eff_BT+BD1+FD2.root");
    */
    
    if (doBinOptimizationPt && doBinOptimizationY)
        return;

    if (doBinOptimizationPt) {
        // Optimizing Pt ranges ...
        vector <pair <Double_t, Double_t>> ptBin;
        ptBin.push_back(make_pair(min, max));
        lAna->SetPtBins(ptBin);
    } else if (doBinOptimizationY) {
        // Optimizing Y ranges ...
        vector <pair <Double_t, Double_t>> yBin;
        yBin.push_back(make_pair(min, max));
        lAna->SetYBins(yBin);
    }

    lAna->doTargetAnal(targets);

    // Save results for further anal. ...
    FILE* f = fopen(Form("%s_min%G_max%G.dat", (doBinOptimizationPt) ? "Pt" : "Y", min, max), "w+");

    auto it = (doBinOptimizationPt) ? lAna->GetSignalPtBinMap().begin() : lAna->GetSignalYBinMap().begin();
    fprintf(f, "min %G max %G signal %G error %G\n", min, max, it->second.first, it->second.second);

    fclose(f);

    delete lAna;
}

// Lambda anal. for a set of targets 

void lambdaAnalRun7(TString dataPath) {
    BmnLambdaAnalRun7* lAna = new BmnLambdaAnalRun7(dataPath);
    // lAna->SetNFiles(5);

    vector <TString> targets{"Pb", "Sn", "Cu", "Al"};

    // Common cuts ...
    for (TString target : targets)
        lAna->SetCuts(target, 1.2, 0.3, 2.2, 0.7, 12.);

    // Setting trigg. efficiencies ...
    /*
    lAna->SetTriggerEffData("BT+FD3", "eff_BT+FD3.root");
    lAna->SetTriggerEffData("BT+FD2", "eff_BT+FD2.root");
    lAna->SetTriggerEffData("BT+BD3", "eff_BT+BD3.root");
    lAna->SetTriggerEffData("BT+BD2", "eff_BT+BD2.root");
    lAna->SetTriggerEffData("BT+BD1+FD2", "eff_BT+BD1+FD2.root");
    */

    lAna->doAllTargetsAnal();

    delete lAna;
}
