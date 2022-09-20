/**
 * \file BmnTrackingQaExpReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2011-2014
 */
#include "BmnTrackingQaExpReport.h"
#include <fstream>
#include <set>
#include <vector>
#include "BmnTrackingQa.h"
#include "BmnUtils.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLine.h"
#include "TString.h"
#include "TStyle.h"
#include "map"
#include "BmnDrawHist.h"
//#include "BmnDrawOnline.h"
#include "BmnHistManager.h"
#include "BmnReportElement.h"


using namespace std;
using namespace lit;

BmnTrackingQaExpReport::BmnTrackingQaExpReport() : BmnSimulationReport(),
fGlobalTrackVariants() {
    SetReportName("tracking_qa");
}

BmnTrackingQaExpReport::BmnTrackingQaExpReport(TString name) : BmnSimulationReport(),
fGlobalTrackVariants(),
fPrefix("") {
    SetReportName(name);
}

BmnTrackingQaExpReport::~BmnTrackingQaExpReport() {
}

void BmnTrackingQaExpReport::Create() {
    Out().precision(3);
    Out() << R()->DocumentBegin();
    Out() << R()->Title(0, GetTitle());
    Out() << PrintEventInfo();
    Out() << "<hr>" << endl;
    PrintCanvases();
    Out() << R()->DocumentEnd();
}

string BmnTrackingQaExpReport::PrintEventInfo() {
    // Out() << "<h2>Event generator: QGSM</h2>" << endl;  //FIXME!
    // Out() << "<h2>Energy: 4 GeV/n</h2>" << endl;        //FIXME!
    if (GetOnlyPrimes()) Out() << "<h2>Results only for primaries presented</h2>" << endl;
    Out() << "<h2>Number of events: " << HM()->H1("hen_EventNo_TrackingQa")->GetEntries() << "</h2>" << endl;
    Out() << "<h2>Mean multiplicity: " << HM()->H1("Multiplicity")->GetMean() << "</h2>" << endl;
    Out() << "<hr>" << endl;
    Out() << "<h3><font color=\"red\">Reconstructable</font> MC-track:</h3>"
            << "Monte Carlo track with at least <font color=\"red\">4</font> Monte Carlo points in GEM" << endl;
    Out() << "<h3><font color=\"red\">Good</font> track:</h3>"
            << "Reconstructed track with at least <font color=\"red\">4</font> hits in GEM and <font color=\"red\">60%</font> of them corresponded the same MC-track" << endl;
    Out() << "<h3><font color=\"red\">Clone</font> tracks:</h3>";
    Out() << "Two or more reconstructed tracks with reference to the same MC-track." << endl;
    Out() << "The number of clones is subtracted from number of good tracks before efficiency calculation." << endl;
    Out() << "<hr>" << endl;
    ifstream inTiming;
    inTiming.open("timing.txt");
    if (inTiming.is_open()) {
        TString a1, a2, a3;
        Float_t workTime;
        while (!inTiming.eof()) {
            inTiming >> a1 >> a2 >> a3 >> workTime;
            Out() << "<h3>"
                    << "Average time of " << a1 << " " << a2 << ": " << workTime / HM()->H1("hen_EventNo_TrackingQa")->GetEntries() * 1000.0 << " ms/event "
                    << "</h3>" << endl;
        }
    }
    inTiming.close();
    return "<hr>";
}

void BmnTrackingQaExpReport::Draw() {
    gStyle->SetPalette(77);
    //	drawHist = new BmnDrawOnline("RECREATE");
    DrawEventsInfo(fPrefix + "Impact parameter and multiplicity");

    //    TString pNamesIn[5] = {"Sim_vs_P", "Rec_vs_P", "Well_vs_P", "Ghost_vs_P", "Split_vs_P"};
    //    TString pNamesOut[3] = {"Eff_vs_P", "Fake_vs_P", "SplitEff_vs_P"};
    //    DrawEffGem(fPrefix + "Efficiency on momentum", pNamesIn, pNamesOut);
    //
    //    TString pNamesInWide[5] = {"Sim_vs_P_wide", "Rec_vs_P_wide", "Well_vs_P_wide", "Ghost_vs_P_wide", "Split_vs_P_wide"};
    //    TString pNamesOutWide[3] = {"Eff_vs_P_wide", "Fake_vs_P_wide", "SplitEff_vs_P_wide"};
    //    DrawEffGem(fPrefix + "Efficiency on momentum in wide range", pNamesInWide, pNamesOutWide);
    //
    //    TString etaNamesIn[5] = {"Sim_vs_Eta", "Rec_vs_Eta", "Well_vs_Eta", "Ghost_vs_Eta", "Split_vs_Eta"};
    //    TString etaNamesOut[3] = {"Eff_vs_Eta", "Fake_vs_Eta", "SplitEff_vs_Eta"};
    //    DrawEffGem(fPrefix + "Efficiency on pseudorapidity", etaNamesIn, etaNamesOut);
    //
    //    TString thetaNamesIn[5] = {"Sim_vs_Theta", "Rec_vs_Theta", "Well_vs_Theta", "Ghost_vs_Theta", "Split_vs_Theta"};
    //    TString thetaNamesOut[3] = {"Eff_vs_Theta", "Fake_vs_Theta", "SplitEff_vs_Theta"};
    //    DrawEffGem(fPrefix + "Efficiency on theta", thetaNamesIn, thetaNamesOut);
    //
    //    TString nhNamesIn[5] = {"Sim_vs_Nh", "Rec_vs_Nh", "Well_vs_Nh", "Ghost_vs_Nh", "Split_vs_Nh"};
    //    TString nhNamesOut[3] = {"Eff_vs_Nh", "Fake_vs_Nh", "SplitEff_vs_Nh"};
    //    DrawEffGem(fPrefix + "Efficiency on number of hits", nhNamesIn, nhNamesOut);

    //DrawNhitsGem(fPrefix + "Distribution of RECO-tracks vs number of hits per track");

    //    DrawTwoH2(fPrefix + "Distribution of tracks over number of hits and Pseudorapidity", "Nh_sim_Eta_sim", "Nh_rec_Eta_rec", 2 * baseW, baseH);
    //    DrawTwoH2(fPrefix + "Distribution of tracks over number of hits and Momentum", "Nh_sim_P_sim", "Nh_rec_P_rec", 2 * baseW, baseH);
    //    DrawTwoH2(fPrefix + "Distribution of tracks over number of hits and Theta", "Nh_sim_Theta_sim", "Nh_rec_Theta_rec", 2 * baseW, baseH);
    //    DrawTwoH2(fPrefix + "Distribution of tracks over Pseudorapidity and Momentum", "EtaP_sim", "EtaP_rec", 2 * baseW, baseH);

    //    for (Int_t i = 1; i < HM()->H2("EtaP_sim")->GetXaxis()->GetNbins(); ++i) {
    //        for (Int_t j = 1; j < HM()->H2("EtaP_sim")->GetYaxis()->GetNbins(); ++j) {
    //            Float_t nom = HM()->H2("EtaP_rec")->GetBinContent(i, j);
    //            Float_t denom = HM()->H2("EtaP_sim")->GetBinContent(i, j);
    //            Float_t content = (denom < 0.001) ? 0.0 : nom / denom * 100.0;
    //            if (content > 100.0) content = 100.0;
    //            HM()->H2("Eff_vs_EtaP")->SetBinContent(i, j, content);
    //        }
    //    }
    //    for (Int_t i = 1; i < HM()->H2("EtaP_rec")->GetXaxis()->GetNbins(); ++i) {
    //        for (Int_t j = 1; j < HM()->H2("EtaP_rec")->GetYaxis()->GetNbins(); ++j) {
    //            Float_t nom = HM()->H2("Clones_vs_EtaP")->GetBinContent(i, j);
    //            Float_t denom = HM()->H2("EtaP_rec")->GetBinContent(i, j);
    //            Float_t content = (denom < 0.001) ? 0.0 : nom / denom * 100.0;
    //            if (content > 100.0) content = 100.0;
    //            HM()->H2("Clones_vs_EtaP")->SetBinContent(i, j, content);
    //        }
    //    }
    //    for (Int_t i = 1; i < HM()->H2("EtaP_rec")->GetXaxis()->GetNbins(); ++i) {
    //        for (Int_t j = 1; j < HM()->H2("EtaP_rec")->GetYaxis()->GetNbins(); ++j) {
    //            Float_t nom = HM()->H2("Fakes_vs_EtaP")->GetBinContent(i, j);
    //            Float_t denom = HM()->H2("EtaP_rec")->GetBinContent(i, j);
    //            Float_t content = (denom < 0.001) ? 0.0 : nom / denom * 100.0;
    //            if (content > 100.0) content = 100.0;
    //            HM()->H2("Fakes_vs_EtaP")->SetBinContent(i, j, content);
    //        }
    //    }

    //    DrawThreeH2(fPrefix + "Distribution of Efficiency, Ghosts and Clones over Pseudorapidity and Momentum", "Eff_vs_EtaP", "Clones_vs_EtaP", "Fakes_vs_EtaP");
    //    DrawOneH2(fPrefix + "Correlation between number of points in MC-track and number of hits in Reco-track", "Nh_rec_Nh_sim");
    //    DrawTwoH2(fPrefix + "Distribution of tracks over Theta and Momentum", "ThetaP_sim", "ThetaP_rec", 2 * baseW, baseH);
    //    DrawTwoH2(fPrefix + "P_reco vs P_mc", "P_rec_P_sim", "Pt_rec_Pt_sim", 2 * baseW, baseH);
    //    DrawOneH2(fPrefix + "Pseudorapidity_reco vs Pseudorapidity_mc", "Eta_rec_Eta_sim");
    //    DrawMomResGem(fPrefix + "Eta resolution", "EtaRes_2D", "EtaRes_1D", "EtaMean_1D", "EtaRes_Mean");
    //    DrawTwoH2(fPrefix + "Tx_reco vs Tx_mc (left) and Ty_reco vs Ty_mc (right) for GLOB", "Tx_rec_Tx_sim", "Ty_rec_Ty_sim", 2 * baseW, baseH);
    //    DrawMomResGem(fPrefix + "Tx resolution", "TxRes_2D", "TxRes_1D", "TxMean_1D", "TxRes_Mean");
    //    DrawMomResGem(fPrefix + "Ty resolution", "TyRes_2D", "TyRes_1D", "TyMean_1D", "TyRes_Mean");
    //    DrawThreeH2(fPrefix + "Reco vs MC for X-, Y- and Z-component of Momentum", "Px_rec_Px_sim", "Py_rec_Py_sim", "Pz_rec_Pz_sim");
    //    DrawMomResGem(fPrefix + "Momentum resolution", "momRes_2D", "momRes_1D", "momMean_1D", "momRes_Mean");
    //    DrawMomResGem(fPrefix + "Px resolution", "PxRes_2D", "PxRes_1D", "PxMean_1D", "PxRes_Mean");
    //    DrawMomResGem(fPrefix + "Py resolution", "PyRes_2D", "PyRes_1D", "PyMean_1D", "PyRes_Mean");
    //    DrawMomResGem(fPrefix + "Pz resolution", "PzRes_2D", "PzRes_1D", "PzMean_1D", "PzRes_Mean");
    //    DrawMomResGem(fPrefix + "Momentum resolution vs. theta", "MomRes_vs_Theta", "MomRes_vs_Theta_1D", "MomMean_vs_Theta_1D", "momRes_Mean");
    //    DrawMomResGem(fPrefix + "Momentum resolution and momentum vs. length of tracks", "MomRes_vs_Length", "MomRes_vs_Length_1D", "MomMean_vs_Length_1D", "momRes_Mean");
    //    DrawMomResGem(fPrefix + "Momentum resolution vs. Number of hits", "MomRes_vs_nHits", "MomRes_vs_nHits_1D", "MomMean_vs_nHits_1D", "momRes_Mean");
    //    DrawTwoH2(fPrefix + "Tracks quality distributions", "MomRes_vs_Chi2", "Mom_vs_Chi2", 2 * baseW, baseH);
    //    DrawTwoH2(fPrefix + "Momentum resolution and momentum vs. length of tracks", "MomRes_vs_Length", "Mom_vs_Length");
    //DrawOneH2(fPrefix + "Momentum resolution vs. Number of hits", "MomRes_vs_nHits");
    //DrawOneH2(fPrefix + "Momentum resolution vs. theta", "MomRes_vs_Theta");
    DrawTwoH1(fPrefix + "Chi-square and length distributions", "Chi2", "Length", "", kFALSE);
    TString namesParF[5] = {"X_f", "Y_f", "Tx_f", "Ty_f", "Qp_f"};
    TString namesParL[5] = {"X_l", "Y_l", "Tx_l", "Ty_l", "Qp_l"};
    DrawPar(fPrefix + "First parameters", namesParF);
    DrawPar(fPrefix + "Last parameters", namesParL);

    DrawThreeH2(fPrefix + "Vertex vs number of tracks in event", "VertX_vs_Ntracks", "VertY_vs_Ntracks", "VertZ_vs_Ntracks");
    DrawVertResGem(fPrefix + "Vertex resolution", "VertResX", "VertResY", "VertResZ");
    DrawThreeH1(fPrefix + "Vertex distribution for X and Y directions", "VertX", "VertY", "VertZ");
    DrawOneH2(fPrefix + "Vertex profile", "VertXY");

    //    TString multNamesIn[5] = {"Sim_vs_mult", "Rec_vs_mult", "Well_vs_mult", "Ghost_vs_mult", "Split_vs_mult"};
    //    TString multNamesOut[3] = {"Eff_vs_mult", "Fake_vs_mult", "SplitEff_vs_mult"};
    //    DrawEffGem(fPrefix + "Efficiency on multiplicity", multNamesIn, multNamesOut);

    //Вызов метода, который поместит все гистограммы на один холст
    //	drawHist -> DrawMainCanvas("Inner tracking");


    // DrawHitRes(fPrefix, "X");
    // DrawHitRes(fPrefix, "Y");
    TString namesResPullsF[15] = {"ResX_f", "ResY_f", "ResTx_f", "ResTy_f", "ResQp_f", "ErrX_f", "ErrY_f", "ErrTx_f", "ErrTy_f", "ErrQp_f", "PullX_f", "PullY_f", "PullTx_f", "PullTy_f", "PullQp_f"};
    TString namesResPullsL[15] = {"ResX_l", "ResY_l", "ResTx_l", "ResTy_l", "ResQp_l", "ErrX_l", "ErrY_l", "ErrTx_l", "ErrTy_l", "ErrQp_l", "PullX_l", "PullY_l", "PullTx_l", "PullTy_l", "PullQp_l"};
    DrawResAndPull(fPrefix + "Residuals and Pulls for first parameters GLOB", namesResPullsF);
    //
    //    //TOF    
    //    TString pNamesInTof400[5] = {"Sim_vs_P_tof400", "Rec_vs_P_tof400", "Well_vs_P_tof400", "Ghost_vs_P_tof400", "Split_vs_P_tof400"};
    //    TString pNamesOutTof400[3] = {"Eff_vs_P_tof400", "Fake_vs_P_tof400", "SplitEff_vs_P_tof400"};
    //    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs P_sim per event for TOF-400", pNamesInTof400, pNamesOutTof400);
    //    TString pNamesInTof700[5] = {"Sim_vs_P_tof700", "Rec_vs_P_tof700", "Well_vs_P_tof700", "Ghost_vs_P_tof700", "Split_vs_P_tof700"};
    //    TString pNamesOutTof700[3] = {"Eff_vs_P_tof700", "Fake_vs_P_tof700", "SplitEff_vs_P_tof700"};
    //    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs P_sim per event for TOF-700", pNamesInTof700, pNamesOutTof700);


    DrawTwoH2(fPrefix + "Distribution of tracks over velocity and rigidity (track and hit have same MCID)", "banana_tof400_good", "banana_tof700_good", 2 * baseW, baseH);
    DrawThreeH2(fPrefix + "Mass correlation and banana plots for TOF-400 and TOF-700", "Mass_correlation", "banana_tof400", "banana_tof700");
    DrawTwoH1(fPrefix + "Distribution of TOF-400 residuals for correct matching", "x_residuals_tof400_good", "y_residuals_tof400_good", "", kTRUE);
    DrawTwoH1(fPrefix + "Distribution of TOF-700 residuals for correct matching", "x_residuals_tof700_good", "y_residuals_tof700_good", "", kTRUE);
    DrawTwoH1(fPrefix + "Distribution of TOF-400 residuals for wrong matching", "x_residuals_tof400_bad", "y_residuals_tof400_bad", "", kTRUE);
    DrawTwoH1(fPrefix + "Distribution of TOF-700 residuals for wrong matching", "x_residuals_tof700_bad", "y_residuals_tof700_bad", "", kTRUE);
    DrawTwoH2(fPrefix + "Residuals over rigidity for TOF-400", "x_resi_vs_mom_tof400_good", "y_resi_vs_mom_tof400_good", 2 * baseW, baseH);
    DrawTwoH2(fPrefix + "Residuals over rigidity for TOF-700", "x_resi_vs_mom_tof700_good", "y_resi_vs_mom_tof700_good", 2 * baseW, baseH);

    //    drawHist->DrawMainCanvas("TOF");

    //CSC
    //    TString pNamesInCsc[5] = {"Sim_vs_P_csc", "Rec_vs_P_csc", "Well_vs_P_csc", "Ghost_vs_P_csc", "Split_vs_P_csc"};
    //    TString pNamesOutCsc[3] = {"Eff_vs_P_csc", "Fake_vs_P_csc", "SplitEff_vs_P_csc"};
    //    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs P_sim per event for CSC", pNamesInCsc, pNamesOutCsc);
    DrawTwoH1(fPrefix + "Distribution of CSC residuals for correct matching", "x_residuals_csc_good", "y_residuals_csc_good", "", kTRUE);
    DrawTwoH1(fPrefix + "Distribution of CSC residuals for wrong matching", "x_residuals_csc_bad", "y_residuals_csc_bad", "", kTRUE);
    DrawTwoH2(fPrefix + "Residuals over rigidity for CSC", "x_resi_vs_mom_csc_good", "y_resi_vs_mom_csc_good", 2 * baseW, baseH);

    //    drawHist->DrawMainCanvas("CSC");

    //DCH1
    //    TString pNamesInDch1[5] = {"Sim_vs_P_dch1", "Rec_vs_P_dch1", "Well_vs_P_dch1", "Ghost_vs_P_dch1", "Split_vs_P_dch1"};
    //    TString pNamesOutDch1[3] = {"Eff_vs_P_dch1", "Fake_vs_P_dch1", "SplitEff_vs_P_dch1"};
    //    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs P_sim per event for DCH1", pNamesInDch1, pNamesOutDch1);
    DrawTwoH1(fPrefix + "Distribution of DCH1 residuals for correct matching", "x_residuals_dch1_good", "y_residuals_dch1_good", "", kTRUE);
    DrawTwoH1(fPrefix + "Distribution of DCH1 residuals for wrong matching", "x_residuals_dch1_bad", "y_residuals_dch1_bad", "", kTRUE);
    DrawTwoH2(fPrefix + "Residuals over rigidity for DCH1", "x_resi_vs_mom_dch1_good", "y_resi_vs_mom_dch1_good", 2 * baseW, baseH);

    //DCH2
    //    TString pNamesInDch2[5] = {"Sim_vs_P_dch2", "Rec_vs_P_dch2", "Well_vs_P_dch2", "Ghost_vs_P_dch2", "Split_vs_P_dch2"};
    //    TString pNamesOutDch2[3] = {"Eff_vs_P_dch2", "Fake_vs_P_dch2", "SplitEff_vs_P_dch2"};
    //    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs P_sim per event for DCH2", pNamesInDch2, pNamesOutDch2);
    DrawTwoH1(fPrefix + "Distribution of DCH2 residuals for correct matching", "x_residuals_dch2_good", "y_residuals_dch2_good", "", kTRUE);
    DrawTwoH1(fPrefix + "Distribution of DCH2 residuals for wrong matching", "x_residuals_dch2_bad", "y_residuals_dch2_bad", "", kTRUE);
    DrawTwoH2(fPrefix + "Residuals over rigidity for DCH2", "x_resi_vs_mom_dch2_good", "y_resi_vs_mom_dch2_good", 2 * baseW, baseH);



    DrawThreeH2(fPrefix + "Tracks Statistics", "Nof_GlobalTracks", "Nof_GemTracks", "Nof_SilTracks");

    if (!fInitCanvasesDone)
        fInitCanvasesDone = kTRUE;
    //    drawHist->DrawMainCanvas("DCH");
}

void BmnTrackingQaExpReport::DrawEffGem(const TString canvasName, TString* inNames, TString* outNames) {
    TString sim = inNames[0];
    TString rec = inNames[1];
    TString well = inNames[2];
    TString ghost = inNames[3];
    TString split = inNames[4];

    TString eff = outNames[0];
    TString fake = outNames[1];
    TString clon = outNames[2];

    Int_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 2 * baseW, baseH);
    if (!fInitCanvasesDone) {
        canvas->SetGrid();
        canvas->Divide(2, 1);
    }
    canvas->cd(1);
    if (!fInitCanvasesDone) {
        HM()->H1(sim)->Sumw2();
        //        HM()->H1(sim)->Scale(1. / nofEvents);
        HM()->H1(rec)->Sumw2();
        //        HM()->H1(rec)->Scale(1. / nofEvents);
        HM()->H1(well)->Sumw2();
        //        HM()->H1(well)->Scale(1. / nofEvents);
        HM()->H1(ghost)->Sumw2();
        //        HM()->H1(ghost)->Scale(1. / nofEvents);
        HM()->H1(split)->Sumw2();
        //        HM()->H1(split)->Scale(1. / nofEvents);

        HM()->H1(sim)->SetMinimum(0.0);
        HM()->H1(well)->SetMinimum(0.0);
        HM()->H1(ghost)->SetMinimum(0.0);
        HM()->H1(split)->SetMinimum(0.0);
    }

    //Debugging
    // printf("NAME = %s\n", HM()->H1(sim)->GetName());
    // printf("HM()->H1(sim) = %f\n", HM()->H1(sim)->GetEntries());
    // printf("HM()->H1(well) = %f\n", HM()->H1(well)->GetEntries());
    // printf("HM()->H1(ghost) = %f\n", HM()->H1(ghost)->GetEntries());
    // printf("HM()->H1(split) = %f\n", HM()->H1(split)->GetEntries());

    vector<TH1*> histos1;
    histos1.push_back(HM()->H1(sim));
    histos1.push_back(HM()->H1(well));
    histos1.push_back(HM()->H1(ghost));
    histos1.push_back(HM()->H1(split));
    vector<string> labels1;
    labels1.push_back("MC tracks");
    labels1.push_back("Good tracks");
    labels1.push_back("Ghost tracks");
    labels1.push_back("Clones");
    DrawH1(/*canvas,*/ histos1, labels1, kLinear, kLinear, true, 0.5, 0.81, 1.0, 0.93, "PE1", kFALSE);

    canvas->cd(2);
    vector<string> labels2;
    labels2.push_back("Efficiency");
    labels2.push_back("Ghosts");
    labels2.push_back("Clones");

    HM()->H1(eff)->Divide(HM()->H1(well), HM()->H1(sim), 1., 1., "B");
    HM()->H1(eff)->Scale(100.0);
    HM()->H1(fake)->Divide(HM()->H1(ghost), HM()->H1(rec), 1., 1., "B");
    HM()->H1(fake)->Scale(100.0);
    HM()->H1(clon)->Divide(HM()->H1(split), HM()->H1(rec), 1., 1., "B");
    HM()->H1(clon)->Scale(100.0);

    // Boundary checking.
    // These cases shouldn't happen, but they happen sometimes...
    for (Int_t i = 1; i < HM()->H1(eff)->GetNbinsX(); ++i) {
        if (HM()->H1(eff)->GetBinContent(i) > 100.0) {
            HM()->H1(eff)->SetBinContent(i, 100.0);
            HM()->H1(eff)->SetBinError(i, 0.0);
        }
        if (HM()->H1(fake)->GetBinContent(i) > 100.0) {
            HM()->H1(fake)->SetBinContent(i, 100.0);
            HM()->H1(fake)->SetBinError(i, 0.0);
        }
        if (HM()->H1(clon)->GetBinContent(i) > 100.0) {
            HM()->H1(clon)->SetBinContent(i, 100.0);
            HM()->H1(clon)->SetBinError(i, 0.0);
        }
        if (HM()->H1(eff)->GetBinContent(i) < 0.0) {
            HM()->H1(eff)->SetBinContent(i, 0.0);
            HM()->H1(eff)->SetBinError(i, 0.0);
        }
        if (HM()->H1(fake)->GetBinContent(i) < 0.0) {
            HM()->H1(fake)->SetBinContent(i, 0.0);
            HM()->H1(fake)->SetBinError(i, 0.0);
        }
        if (HM()->H1(clon)->GetBinContent(i) < 0.0) {
            HM()->H1(clon)->SetBinContent(i, 0.0);
            HM()->H1(clon)->SetBinError(i, 0.0);
        }
    }

    HM()->H1(eff)->SetMaximum(100.0);
    HM()->H1(fake)->SetMaximum(100.0);
    HM()->H1(clon)->SetMaximum(100.0);
    HM()->H1(eff)->SetMinimum(0.0);
    HM()->H1(fake)->SetMinimum(0.0);
    HM()->H1(clon)->SetMinimum(0.0);

    vector<TH1*> histos2;
    histos2.push_back(HM()->H1(eff));
    histos2.push_back(HM()->H1(fake));
    histos2.push_back(HM()->H1(clon));
    DrawH1(/*canvas,*/ histos2, labels2, kLinear, kLinear, true, 0.5, 0.81, 1.0, 0.93, "PE1X0", kTRUE);
}

void BmnTrackingQaExpReport::DrawNhitsGem(const TString canvasName) {
    Int_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), baseW, baseH);
    if (!fInitCanvasesDone) {
        canvas->SetGrid();
    }
    canvas->cd();
    if (!fInitCanvasesDone) {
        HM()->H1("Well_vs_Nh")->Sumw2();
        HM()->H1("Ghost_vs_Nh")->Sumw2();
    }
    HM()->H1("Well_vs_Nh")->Scale(1. / nofEvents);
    HM()->H1("Ghost_vs_Nh")->Scale(1. / nofEvents);
    vector<TH1*> histos1;
    histos1.push_back(HM()->H1("Well_vs_Nh"));
    histos1.push_back(HM()->H1("Ghost_vs_Nh"));
    vector<string> labels1;
    labels1.push_back("Good tracks");
    labels1.push_back("Ghosts");
    DrawH1(/*canvas,*/ histos1, labels1, kLinear, kLinear, true, 0.5, 0.81, 1.0, 0.9, "PE1", kFALSE);
}

void BmnTrackingQaExpReport::DrawEventsInfo(const TString canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 3 * baseW, baseH);
    if (!fInitCanvasesDone) {
        canvas->SetGrid();
        canvas->Divide(3, 1);
    }
    canvas->cd(1);
    DrawH1(/*canvas,*/ HM()->H1("Impact parameter"), kLinear, kLinear, "", kRed, 2, 1, 1.1, 20, 33);
    canvas->cd(2);
    DrawH1(/*canvas,*/ HM()->H1("Multiplicity"), kLinear, kLinear, "", kRed, 2, 1, 1.1, 20, 33);
    canvas->cd(3);
    DrawH2(/*canvas,*/ HM()->H2("Impact_Mult"), kLinear, kLinear, kLinear, "colz");
}

void BmnTrackingQaExpReport::DrawMomResGem(const TString canvasName, TString name2d, TString nameSigma, TString nameMean, TString nameAver) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 4 * baseW, baseH);
    if (!fInitCanvasesDone) {
        canvas->SetGrid();
        canvas->Divide(4, 1);
    }
    canvas->cd(1);
    DrawH2(/*canvas,*/ HM()->H2(name2d), kLinear, kLinear, kLinear, "colz");

    canvas->cd(2);
    FillAndFitSlice(nameSigma, nameMean, name2d);
    HM()->H1(nameSigma)->SetMaximum(6.0);
    HM()->H1(nameSigma)->SetMinimum(0.0);
    DrawH1(/*canvas,*/ HM()->H1(nameSigma), kLinear, kLinear, "PE1", kRed, 1, 0.75, 1.1, 20);

    canvas->cd(3);
    HM()->H1(nameMean)->SetMaximum(2.0);
    HM()->H1(nameMean)->SetMinimum(-2.0);
    DrawH1(/*canvas,*/ HM()->H1(nameMean), kLinear, kLinear, "PE1", kRed, 1, 0.75, 1.1, 20);

    canvas->cd(4);
    TH1D* projY = HM()->H2(name2d)->ProjectionY("tmp1");
    for (Int_t iBin = 0; iBin <= projY->GetNbinsX() + 1; ++iBin) {
        HM()->H1(nameAver)->SetBinContent(iBin, projY->GetBinContent(iBin));
    }

    DrawH1(/*canvas,*/ HM()->H1(nameAver), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    if (HM()->H1(nameAver)->GetEntries() < MinNEntries4Fit)
        return;
    HM()->H1(nameAver)->Fit("gaus", "RQWW", "", -2, 2);
    HM()->H1(nameAver)->SetMaximum(HM()->H1(nameAver)->GetMaximum() * 1.05);
    TF1* fit = HM()->H1(nameAver)->GetFunction("gaus");
    if (!fit) return;
    TPaveStats* ps = new TPaveStats(3.0, HM()->H1(nameAver)->GetMaximum() / 1.5, 10.0, HM()->H1(nameAver)->GetMaximum());
    ps->SetFillColor(0);
    ps->SetShadowColor(0);
    ps->AddText(Form("#mu = %2.2f", fit->GetParameter(1)));
    ps->AddText(Form("#sigma = %2.2f", fit->GetParameter(2)));
    ps->Draw();
}

void BmnTrackingQaExpReport::DrawResAndPull(const TString canvasName, TString* inNames) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 5 * baseW, 3 * baseH);
    if (!fInitCanvasesDone) {
        canvas->SetGrid();
        canvas->Divide(5, 3);
    }
    for (Int_t i = 0; i < 15; ++i) {
        if (HM()->H1(inNames[i])->GetEntries() < MinNEntries4Fit)
            continue;
        canvas->cd(i + 1);
        if (i < 5 || i > 9) HM()->H1(inNames[i])->Fit("gaus", "RQWW", "", -20, 20);
        DrawH1(/*canvas,*/ HM()->H1(inNames[i]), kLinear, kLog, "", kBlue, 1, 0.75, 1.1, 20);
        //if (i > 4) {
        TF1* fit = HM()->H1(inNames[i])->GetFunction("gaus");
        if (!fit) continue;
        Float_t xMax = HM()->H1(inNames[i])->GetXaxis()->GetXmax();
        Float_t yMax = HM()->H1(inNames[i])->GetMaximum();
        TPaveStats* ps = new TPaveStats(xMax * 0.2, yMax * 0.1, xMax, yMax);
        ps->SetFillColor(0);
        ps->SetShadowColor(0);
        ps->AddText(Form("#mu = %2.2f", fit->GetParameter(1)));
        ps->AddText(Form("#sigma = %2.2f", fit->GetParameter(2)));
        ps->Draw();
        //}
    }
}

void BmnTrackingQaExpReport::DrawPar(const TString canvasName, TString* inNames) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 5 * baseW, baseH);
    if (!fInitCanvasesDone) {
        canvas->SetGrid();
        canvas->Divide(5, 1);
    }
    for (Int_t i = 0; i < 5; ++i) {
        canvas->cd(i + 1);
        DrawH1(/*canvas,*/ HM()->H1(inNames[i]), kLinear, kLog, "", kRed, 1, 0.75, 1.1, 20);
    }
}

void BmnTrackingQaExpReport::FillAndFitSlice(TString nameSigma, TString nameMean, TString name2d) {
    Int_t nBins = HM()->H1(nameSigma)->GetXaxis()->GetNbins();
    Int_t momResStep = HM()->H2(name2d)->GetNbinsX() / nBins;
    Int_t bin = 0;
    for (Int_t iBin = 1; iBin < HM()->H2(name2d)->GetNbinsX(); iBin += momResStep) {
        TH1D* proj = HM()->H2(name2d)->ProjectionY("tmp", iBin, iBin + (momResStep - 1));
        bin++;
        if (proj->GetEntries() < MinNEntries4Fit) continue;
        //        proj->Integral()
        proj->Fit("gaus", "SQRww", "", -5.0, 5.0);
        TF1* fit = proj->GetFunction("gaus");
        if (!fit) continue;
        Float_t sigma = fit->GetParameter(2);
        Float_t sigmaError = fit->GetParError(2);
        Float_t mean = fit->GetParameter(1);
        Float_t meanError = fit->GetParError(1);
        HM()->H1(nameSigma)->SetBinContent(bin, sigma);
        HM()->H1(nameSigma)->SetBinError(bin, sigmaError);
        HM()->H1(nameMean)->SetBinContent(bin, mean);
        HM()->H1(nameMean)->SetBinError(bin, meanError);
    }
}

void BmnTrackingQaExpReport::DrawVertResGem(const TString canvasName, TString name1dX, TString name1dY, TString name1dZ) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 3 * baseW, baseH);
    if (!fInitCanvasesDone) {
        canvas->SetGrid();
        canvas->Divide(3, 1);
    }
    canvas->cd(1);
    if (HM()->H1(name1dX)->GetEntries() > MinNEntries4Fit)
        HM()->H1(name1dX)->Fit("gaus", "RQWW", "", -1, 1);
    DrawH1(/*canvas,*/ HM()->H1(name1dX), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    DrawMuSigma(canvas->cd(1), HM()->H1(name1dX));
    canvas->cd(2);
    if (HM()->H1(name1dY)->GetEntries() > MinNEntries4Fit)
        HM()->H1(name1dY)->Fit("gaus", "RQWW", "", -1, 1);
    DrawH1(/*canvas,*/ HM()->H1(name1dY), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    DrawMuSigma(canvas->cd(2), HM()->H1(name1dY));
    canvas->cd(3);
    if (HM()->H1(name1dZ)->GetEntries() > MinNEntries4Fit)
        HM()->H1(name1dZ)->Fit("gaus", "RQWW", "", -2, 2);
    DrawH1(/*canvas,*/ HM()->H1(name1dZ), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    DrawMuSigma(canvas->cd(3), HM()->H1(name1dZ));
}

ClassImp(BmnTrackingQaExpReport)
