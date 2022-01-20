#include <Rtypes.h>
#include <TString.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TH2F.h>
#include <TCanvas.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;

// Experimental data ...

void efficiency() {
    BmnEfficiencyTools* eTools = new BmnEfficiencyTools();
    eTools->SetOutputFile("effOut_reco.root");

    eTools->SetGeometryFile(""); // path to geometry
    eTools->isL1Input(kTRUE);

    vector <TString> beams{"Ar"};
    vector <TString> targets{"Al", "Cu", "Sn", "Pb"};
    vector <TString> triggers{"BT+FD2", "BT+FD3", "BT+BD2", "BT+BD3", "BT+BD1+FD2"};

    // Setting beam, target and triggers to be processed ...
    eTools->SetBeams(beams);
    eTools->SetTargets(targets);
    eTools->SetTriggers(triggers);

    // Setting manually a list of files to be processed (if necessary) ...
    // Do not forget to unset (commment) beams, targets, triggers set before !!!
    // vector <Int_t> list{4649, 4651, 4652, 4653, 4654, 4656, 4657, 4658};
    // eTools->SetFilelist(list);

    eTools->SetDstPath(""); // Path to data

    // Setting Y slices for GEM ...
    pair <Double_t, Double_t> r1 = make_pair(-1., 15.);
    pair <Double_t, Double_t> r2 = make_pair(15., 30.);
    pair <Double_t, Double_t> r3 = make_pair(30., 50.);

    vector <pair <Double_t, Double_t>> ranges;
    ranges.push_back(r1);
    ranges.push_back(r2);
    ranges.push_back(r3);

    map <Int_t, vector <pair <Double_t, Double_t>>> gPairMap;

    const Int_t nGemStats = 6;
    for (Int_t iGemStat = 0; iGemStat < nGemStats; iGemStat++)
        gPairMap[iGemStat] = ranges;

    eTools->SetGemYRanges(gPairMap);

    // Setting Y slices for SILICON ...
    map <Int_t, vector <pair <Double_t, Double_t>>> sPairMapY;

    // Stat 0 (y-ranges) 
    ranges.clear();
    pair <Double_t, Double_t> r11y = make_pair(1., 3.);
    pair <Double_t, Double_t> r12y = make_pair(3., 5.);
    pair <Double_t, Double_t> r13y = make_pair(5., 8.);
    ranges.push_back(r11y);
    ranges.push_back(r12y);
    ranges.push_back(r13y);
    sPairMapY[0] = ranges;
    
    // Stat 1 (y-ranges)
    ranges.clear();
    pair <Double_t, Double_t> r21y = make_pair(1., 3.);
    pair <Double_t, Double_t> r22y = make_pair(3., 5.);
    pair <Double_t, Double_t> r23y = make_pair(5., 8.);
    ranges.push_back(r21y);
    ranges.push_back(r22y);
    ranges.push_back(r23y);
    sPairMapY[1] = ranges;

    // Stat 2 (y-ranges)
    ranges.clear();
    pair <Double_t, Double_t> r31y = make_pair(1., 4.);
    pair <Double_t, Double_t> r32y = make_pair(4., 7.);
    pair <Double_t, Double_t> r33y = make_pair(7., 10.);
    ranges.push_back(r31y);
    ranges.push_back(r32y);
    ranges.push_back(r33y);
    sPairMapY[2] = ranges;
     
    eTools->SetSiliconYRanges(sPairMapY);

    // Processing ...
    eTools->Process();

    delete eTools;
}

// Monte Carlo data ...

void efficiency(TString dst) {
    BmnEfficiencyTools* eTools = new BmnEfficiencyTools(dst);
    eTools->SetOutputFile("effOut_mc.root");

    eTools->SetGeometryFile(""); // path to geometry
    eTools->isL1Input(kTRUE);

    pair <Double_t, Double_t> r1 = make_pair(-1., 15.);
    pair <Double_t, Double_t> r2 = make_pair(15., 30.);
    pair <Double_t, Double_t> r3 = make_pair(30., 50.);

    vector <pair <Double_t, Double_t>> ranges;
    ranges.push_back(r1);
    ranges.push_back(r2);
    ranges.push_back(r3);

    map <Int_t, vector <pair <Double_t, Double_t>>> gPairMap;

    const Int_t nGemStats = 6;
    for (Int_t iGemStat = 0; iGemStat < nGemStats; iGemStat++)
        gPairMap[iGemStat] = ranges;

    eTools->SetGemYRanges(gPairMap);

    // Setting Y slices for SILICON ...
    map <Int_t, vector <pair <Double_t, Double_t>>> sPairMap;

    // Stat 0
    ranges.clear();
    pair <Double_t, Double_t> r11 = make_pair(1., 3.);
    pair <Double_t, Double_t> r12 = make_pair(3., 5.);
    pair <Double_t, Double_t> r13 = make_pair(5., 8.);
    ranges.push_back(r11);
    ranges.push_back(r12);
    ranges.push_back(r13);
    sPairMap[0] = ranges;

    // Stat 1
    ranges.clear();
    pair <Double_t, Double_t> r21 = make_pair(1., 3.);
    pair <Double_t, Double_t> r22 = make_pair(3., 5.);
    pair <Double_t, Double_t> r23 = make_pair(5., 8.);
    ranges.push_back(r21);
    ranges.push_back(r22);
    ranges.push_back(r23);
    sPairMap[1] = ranges;

    // Stat 2
    ranges.clear();
    pair <Double_t, Double_t> r31 = make_pair(1., 4.);
    pair <Double_t, Double_t> r32 = make_pair(4., 7.);
    pair <Double_t, Double_t> r33 = make_pair(7., 10.);
    ranges.push_back(r31);
    ranges.push_back(r32);
    ranges.push_back(r33);
    sPairMap[2] = ranges;

    eTools->SetSiliconYRanges(sPairMap);

    eTools->Process();

    delete eTools;
}