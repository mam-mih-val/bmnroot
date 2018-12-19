/**
 * \file BmnTrackingQaReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2011-2014
 */
#include "BmnTrackingQaReport.h"
#include "report/BmnReportElement.h"
#include "report/BmnHistManager.h"
#include "report/BmnDrawHist.h"
#include "BmnUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "map"
#include "TCanvas.h"
#include "TLine.h"
#include "BmnTrackingQa.h"
#include <vector>
#include <set>
#include "TString.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TLatex.h"
#include "TGaxis.h"
#include <fstream>

const Float_t pMax = 5.0;

using namespace std;
using namespace lit;

BmnTrackingQaReport::BmnTrackingQaReport() :
BmnSimulationReport(),
fGlobalTrackVariants() {
    SetReportName("tracking_qa");
}

BmnTrackingQaReport::BmnTrackingQaReport(TString name) :
BmnSimulationReport(),
fGlobalTrackVariants(),
fPrefix(name + ": ") {
    SetReportName(name);
}

BmnTrackingQaReport::~BmnTrackingQaReport() {
}

void BmnTrackingQaReport::Create() {
    Out().precision(3);
    Out() << R()->DocumentBegin();
    Out() << R()->Title(0, GetTitle());
    Out() << PrintEventInfo();
    Out() << "<hr>" << endl;
    PrintCanvases();
    Out() << R()->DocumentEnd();
}

string BmnTrackingQaReport::PrintEventInfo() {
    Out() << "<h2>Event generator: QGSM</h2>" << endl; //FIXME!
    Out() << "<h2>Energy: 4 GeV/n</h2>" << endl; //FIXME!
    if (GetOnlyPrimes()) Out() << "<h2>Results only for primaries presented</h2>" << endl;
    Out() << "<h2>Number of events: " << HM()->H1("hen_EventNo_TrackingQa")->GetEntries() << "</h2>" << endl;
    Out() << "<h2>Mean multiplicity: " << HM()->H1("Multiplicity")->GetMean() << "</h2>" << endl;
    Out() << "<hr>" << endl;
    Out() << "<h3><font color=\"red\">Reconstructable</font> MC-track:</h3>" << "Monte Carlo track with at least <font color=\"red\">4</font> Monte Carlo points in GEM" << endl;
    Out() << "<h3><font color=\"red\">Good</font> track:</h3>" << "Reconstructed track with at least <font color=\"red\">4</font> hits in GEM and <font color=\"red\">60%</font> of them corresponded the same MC-track" << endl;
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
            Out() << "<h3>" << "Average time of " << a1 << " " << a2 << ": " << workTime / HM()->H1("hen_EventNo_TrackingQa")->GetEntries() * 1000.0 << " ms/event " << "</h3>" << endl;
        }
    }
    inTiming.close();
    return "<hr>";
}

void BmnTrackingQaReport::Draw() {
    DrawEventsInfo(fPrefix + "Distribution of impact parameter and multiplicity");
    SetDefaultDrawStyle();

    TString pNamesIn[5] = {"Sim_vs_P", "Rec_vs_P", "Well_vs_P", "Ghost_vs_P", "Split_vs_P"};
    TString pNamesOut[3] = {"Eff_vs_P", "Fake_vs_P", "SplitEff_vs_P"};
    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs P_sim per event", pNamesIn, pNamesOut);

    TString pNamesInWide[5] = {"Sim_vs_P_wide", "Rec_vs_P_wide", "Well_vs_P_wide", "Ghost_vs_P_wide", "Split_vs_P_wide"};
    TString pNamesOutWide[3] = {"Eff_vs_P_wide", "Fake_vs_P_wide", "SplitEff_vs_P_wide"};
    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs P_sim per event in wide momentum range", pNamesInWide, pNamesOutWide);

    TString etaNamesIn[5] = {"Sim_vs_Eta", "Rec_vs_Eta", "Well_vs_Eta", "Ghost_vs_Eta", "Split_vs_Eta"};
    TString etaNamesOut[3] = {"Eff_vs_Eta", "Fake_vs_Eta", "SplitEff_vs_Eta"};
    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs Pseudorapidity per event", etaNamesIn, etaNamesOut);

    TString thetaNamesIn[5] = {"Sim_vs_Theta", "Rec_vs_Theta", "Well_vs_Theta", "Ghost_vs_Theta", "Split_vs_Theta"};
    TString thetaNamesOut[3] = {"Eff_vs_Theta", "Fake_vs_Theta", "SplitEff_vs_Theta"};
    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs theta per event", thetaNamesIn, thetaNamesOut);

    TString nhNamesIn[5] = {"Sim_vs_Nh", "Rec_vs_Nh", "Well_vs_Nh", "Ghost_vs_Nh", "Split_vs_Nh"};
    TString nhNamesOut[3] = {"Eff_vs_Nh", "Fake_vs_Nh", "SplitEff_vs_Nh"};
    DrawEffGem(fPrefix + "Distribution of MC-tracks, reco-tracks, fakes and clones vs Nh per event", nhNamesIn, nhNamesOut);

    DrawNhitsGem(fPrefix + "Distribution of RECO-tracks vs number of hits per track");

    DrawTwoH2(fPrefix + "Distribution of tracks over number of hits and Pseudorapidity", "Nh_sim_Eta_sim", "Nh_rec_Eta_rec");
    DrawTwoH2(fPrefix + "Distribution of tracks over number of hits and Momentum", "Nh_sim_P_sim", "Nh_rec_P_rec");
    DrawTwoH2(fPrefix + "Distribution of tracks over number of hits and Theta", "Nh_sim_Theta_sim", "Nh_rec_Theta_rec");
    DrawTwoH2(fPrefix + "Distribution of tracks over Pseudorapidity and Momentum", "EtaP_sim", "EtaP_rec");

    for (Int_t i = 1; i < HM()->H2("EtaP_sim")->GetXaxis()->GetNbins(); ++i) {
        for (Int_t j = 1; j < HM()->H2("EtaP_sim")->GetYaxis()->GetNbins(); ++j) {
            Float_t nom = HM()->H2("EtaP_rec")->GetBinContent(i, j);
            Float_t denom = HM()->H2("EtaP_sim")->GetBinContent(i, j);
            Float_t content = (denom < 0.001) ? 0.0 : nom / denom * 100.0;
            if (content > 100.0) content = 100.0;
            HM()->H2("Eff_vs_EtaP")->SetBinContent(i, j, content);
        }
    }
    for (Int_t i = 1; i < HM()->H2("EtaP_rec")->GetXaxis()->GetNbins(); ++i) {
        for (Int_t j = 1; j < HM()->H2("EtaP_rec")->GetYaxis()->GetNbins(); ++j) {
            Float_t nom = HM()->H2("Clones_vs_EtaP")->GetBinContent(i, j);
            Float_t denom = HM()->H2("EtaP_rec")->GetBinContent(i, j);
            Float_t content = (denom < 0.001) ? 0.0 : nom / denom * 100.0;
            if (content > 100.0) content = 100.0;
            HM()->H2("Clones_vs_EtaP")->SetBinContent(i, j, content);
        }
    }
    for (Int_t i = 1; i < HM()->H2("EtaP_rec")->GetXaxis()->GetNbins(); ++i) {
        for (Int_t j = 1; j < HM()->H2("EtaP_rec")->GetYaxis()->GetNbins(); ++j) {
            Float_t nom = HM()->H2("Fakes_vs_EtaP")->GetBinContent(i, j);
            Float_t denom = HM()->H2("EtaP_rec")->GetBinContent(i, j);
            Float_t content = (denom < 0.001) ? 0.0 : nom / denom * 100.0;
            if (content > 100.0) content = 100.0;
            HM()->H2("Fakes_vs_EtaP")->SetBinContent(i, j, content);
        }
    }

    DrawThreeH2(fPrefix + "Distribution of Efficiency, Ghosts and Clones over Pseudorapidity and Momentum", "Eff_vs_EtaP", "Clones_vs_EtaP", "Fakes_vs_EtaP");

    DrawTwoH2(fPrefix + "Distribution of tracks over Theta and Momentum", "ThetaP_sim", "ThetaP_rec");
    DrawTwoH2(fPrefix + "P_reco vs P_mc", "P_rec_P_sim", "Pt_rec_Pt_sim");
    DrawOneH2(fPrefix + "Pseudorapidity_reco vs Pseudorapidity_mc", "Eta_rec_Eta_sim");
    //    DrawThreeH2(fPrefix + "Tx_reco vs Tx_mc (left) and Ty_reco vs Ty_mc (right) for GLOB", "Tx_rec_Tx_sim", "Ty_rec_Ty_sim", "Qp_rec_Qp_sim");
    DrawThreeH2(fPrefix + "Reco vs MC for X-, Y- and Z-component of Momentum", "Px_rec_Px_sim", "Py_rec_Py_sim", "Pz_rec_Pz_sim");
    DrawMomResGem(fPrefix + "Momentum resolution", "momRes_2D", "momRes_1D", "momMean_1D", "momRes_Mean");
    DrawTwoH2(fPrefix + "Tracks quality distributions", "MomRes_vs_Chi2", "Mom_vs_Chi2");
    DrawTwoH2(fPrefix + "Momentum resolution and momentum vs. length of tracks", "MomRes_vs_Length", "Mom_vs_Length");
    DrawOneH2(fPrefix + "Momentum resolution vs. Number of hits", "MomRes_vs_nHits");
    DrawOneH2(fPrefix + "Momentum resolution vs. theta", "MomRes_vs_Theta");
    DrawTwoH1(fPrefix + "Chi-square and length distributions", "Chi2", "Length", "");

    DrawHitRes(fPrefix, "X");
    DrawHitRes(fPrefix, "Y");

    TString namesResPullsF[15] = {"ResX_f", "ResY_f", "ResTx_f", "ResTy_f", "ResQp_f", "ErrX_f", "ErrY_f", "ErrTx_f", "ErrTy_f", "ErrQp_f", "PullX_f", "PullY_f", "PullTx_f", "PullTy_f", "PullQp_f"};
    //    TString namesRes_vs_ParF[5] = {"ResX_vs_X_f", "ResY_vs_Y_f", "ResTx_vs_Tx_f", "ResTy_vs_Ty_f", "ResQp_vs_Qp_f"};
    TString namesResPullsL[15] = {"ResX_l", "ResY_l", "ResTx_l", "ResTy_l", "ResQp_l", "ErrX_l", "ErrY_l", "ErrTx_l", "ErrTy_l", "ErrQp_l", "PullX_l", "PullY_l", "PullTx_l", "PullTy_l", "PullQp_l"};

    DrawResAndPull(fPrefix + "Residuals and Pulls for first parameters GLOB", namesResPullsF);
    //    DrawResAndPull_2D(fPrefix + "Residuals vs Parameters for first parameters GLOB", namesRes_vs_ParF);
    DrawResAndPull(fPrefix + "Residuals and Pulls for last parameters GLOB", namesResPullsL);

    TString namesParF[5] = {"X_f", "Y_f", "Tx_f", "Ty_f", "Qp_f"};
    TString namesParL[5] = {"X_l", "Y_l", "Tx_l", "Ty_l", "Qp_l"};
    DrawPar(fPrefix + "First parameters", namesParF);
    DrawPar(fPrefix + "Last parameters", namesParL);

    DrawThreeH1(fPrefix + "Vertex", "VertX", "VertY", "VertZ");
    DrawThreeH2(fPrefix + "Vertex vs number of tracks in event", "VertX_vs_Ntracks", "VertY_vs_Ntracks", "VertZ_vs_Ntracks");
    DrawVertResGem(fPrefix + "Vertex resolution", "VertResX", "VertResY", "VertResZ");
}

void BmnTrackingQaReport::DrawEffGem(const TString canvasName, TString* inNames, TString* outNames) {

    TString sim = inNames[0];
    TString rec = inNames[1];
    TString well = inNames[2];
    TString ghost = inNames[3];
    TString split = inNames[4];

    TString eff = outNames[0];
    TString fake = outNames[1];
    TString clon = outNames[2];

    Int_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1200, 600);
    canvas->SetGrid();
    canvas->Divide(2, 1);
    canvas->cd(1);
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

    printf("NAME = %s\n", HM()->H1(sim)->GetName());
    printf("HM()->H1(sim) = %f\n", HM()->H1(sim)->GetEntries());
    printf("HM()->H1(well) = %f\n", HM()->H1(well)->GetEntries());
    printf("HM()->H1(ghost) = %f\n", HM()->H1(ghost)->GetEntries());
    printf("HM()->H1(split) = %f\n", HM()->H1(split)->GetEntries());

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
    DrawH1(histos1, labels1, kLinear, kLinear, true, 0.5, 0.8, 1.0, 0.99, "PE1", kFALSE);

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
    DrawH1(histos2, labels2, kLinear, kLinear, true, 0.5, 0.9, 1.0, 0.99, "PE1X0", kTRUE);
}

void BmnTrackingQaReport::DrawNhitsGem(const TString canvasName) {
    Int_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 600, 600);
    canvas->SetGrid();
    HM()->H1("Well_vs_Nh")->Sumw2();
    HM()->H1("Well_vs_Nh")->Scale(1. / nofEvents);
    HM()->H1("Ghost_vs_Nh")->Sumw2();
    HM()->H1("Ghost_vs_Nh")->Scale(1. / nofEvents);
    vector<TH1*> histos1;
    histos1.push_back(HM()->H1("Well_vs_Nh"));
    histos1.push_back(HM()->H1("Ghost_vs_Nh"));
    vector<string> labels1;
    labels1.push_back("Good tracks");
    labels1.push_back("Ghosts");
    DrawH1(histos1, labels1, kLinear, kLinear, true, 0.5, 0.9, 1.0, 0.99, "PE1", kFALSE);
}

void BmnTrackingQaReport::DrawEventsInfo(const TString canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1500, 500);
    canvas->SetGrid();
    canvas->Divide(3, 1);
    canvas->cd(1);
    DrawH1(HM()->H1("Impact parameter"), kLinear, kLinear, "", kRed, 2, 1, 1.1, 20, 33);
    canvas->cd(2);
    DrawH1(HM()->H1("Multiplicity"), kLinear, kLinear, "", kRed, 2, 1, 1.1, 20, 33);
    canvas->cd(3);
    DrawH2(HM()->H2("Impact_Mult"), kLinear, kLinear, kLinear, "colz");
}

void BmnTrackingQaReport::DrawMomResGem(const TString canvasName, TString name2d, TString nameSigma, TString nameMean, TString nameAver) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1600, 400);
    canvas->SetGrid();
    canvas->Divide(4, 1);
    canvas->cd(1);
    DrawH2(HM()->H2(name2d), kLinear, kLinear, kLinear, "colz");

    canvas->cd(2);
    FillAndFitSlice(nameSigma, nameMean, name2d);
    HM()->H1(nameSigma)->SetMaximum(10.0);
    HM()->H1(nameSigma)->SetMinimum(0.0);
    DrawH1(HM()->H1(nameSigma), kLinear, kLinear, "PE1", kRed, 1, 0.75, 1.1, 20);

    canvas->cd(3);
    HM()->H1(nameMean)->SetMaximum(10.0);
    HM()->H1(nameMean)->SetMinimum(-10.0);
    DrawH1(HM()->H1(nameMean), kLinear, kLinear, "PE1", kRed, 1, 0.75, 1.1, 20);

    canvas->cd(4);
    TH1D* projY = HM()->H2(name2d)->ProjectionY("tmp1");
    for (Int_t iBin = 0; iBin <= projY->GetNbinsX() + 1; ++iBin) {
        HM()->H1(nameAver)->SetBinContent(iBin, projY->GetBinContent(iBin));
    }

    DrawH1(HM()->H1(nameAver), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    //    DrawH1(projY, kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    HM()->H1(nameAver)->Fit("gaus", "RQWW", "", -10, 10);
    HM()->H1(nameAver)->SetMaximum(HM()->H1(nameAver)->GetMaximum() * 1.05);
    TF1 *fit = HM()->H1(nameAver)->GetFunction("gaus");
    if (!fit) return;
    TPaveStats* ps = new TPaveStats(3.0, HM()->H1(nameAver)->GetMaximum() / 1.5, 10.0, HM()->H1(nameAver)->GetMaximum());
    ps->SetFillColor(0);
    ps->SetShadowColor(0);
    ps->AddText(Form("#mu = %2.2f", fit->GetParameter(1)));
    ps->AddText(Form("#sigma = %2.2f", fit->GetParameter(2)));
    ps->Draw();
}

void BmnTrackingQaReport::DrawResAndPull(const TString canvasName, TString* inNames) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1500, 1000);
    canvas->SetGrid();
    canvas->Divide(5, 3);

    for (Int_t i = 0; i < 15; ++i) {
        canvas->cd(i + 1);
        if (i < 5 || i > 9) HM()->H1(inNames[i])->Fit("gaus", "RQWW", "", -20, 20);
        DrawH1(HM()->H1(inNames[i]), kLinear, kLog, "", kBlue, 1, 0.75, 1.1, 20);
        //if (i > 4) {
        TF1 *fit = HM()->H1(inNames[i])->GetFunction("gaus");
        if (!fit) continue;
        Float_t xMax = HM()->H1(inNames[i])->GetXaxis()->GetXmax();
        Float_t yMax = HM()->H1(inNames[i])->GetMaximum();
        TPaveStats* ps = new TPaveStats(xMax / 2, yMax / 10, xMax, yMax);
        ps->SetFillColor(0);
        ps->SetShadowColor(0);
        ps->AddText(Form("#mu = %2.2f", fit->GetParameter(1)));
        ps->AddText(Form("#sigma = %2.2f", fit->GetParameter(2)));
        ps->Draw();
        //}
    }
}

void BmnTrackingQaReport::DrawResAndPull_2D(const TString canvasName, TString* inNames) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1500, 300);
    canvas->SetGrid();
    canvas->Divide(5, 1);

    for (Int_t i = 0; i < 5; ++i) {
        canvas->cd(i + 1);
        DrawH2(HM()->H2(inNames[i]), kLinear, kLinear, kLinear, "colz");
    }
}

void BmnTrackingQaReport::DrawPar(const TString canvasName, TString* inNames) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1500, 300);
    canvas->SetGrid();
    canvas->Divide(5, 1);

    for (Int_t i = 0; i < 5; ++i) {
        canvas->cd(i + 1);
        DrawH1(HM()->H1(inNames[i]), kLinear, kLog, "", kRed, 1, 0.75, 1.1, 20);
    }
}

void BmnTrackingQaReport::FillAndFitSlice(TString nameSigma, TString nameMean, TString name2d) {
    Int_t nBins = HM()->H1(nameSigma)->GetXaxis()->GetNbins();
    Int_t momResStep = HM()->H2(name2d)->GetNbinsX() / nBins;
    Int_t bin = 0;
    for (Int_t iBin = 1; iBin < HM()->H2(name2d)->GetNbinsX(); iBin += momResStep) {
        TH1D* proj = HM()->H2(name2d)->ProjectionY("tmp", iBin, iBin + (momResStep - 1));
        bin++;
        if (proj->GetEntries() < 5) continue;
        //        proj->Integral()
        proj->Fit("gaus", "SQRww", "", -5.0, 5.0);
        TF1 *fit = proj->GetFunction("gaus");
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

void BmnTrackingQaReport::DrawVertResGem(const TString canvasName, TString name1dX, TString name1dY, TString name1dZ) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1500, 500);
    canvas->SetGrid();
    canvas->Divide(3, 1);

    canvas->cd(1);
    HM()->H1(name1dX)->Fit("gaus", "RQWW", "", -1, 1);
    DrawH1(HM()->H1(name1dX), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    DrawMuSigma(canvas->cd(1), HM()->H1(name1dX));
    canvas->cd(2);
    HM()->H1(name1dY)->Fit("gaus", "RQWW", "", -1, 1);
    DrawH1(HM()->H1(name1dY), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    DrawMuSigma(canvas->cd(2), HM()->H1(name1dY));
    canvas->cd(3);
    HM()->H1(name1dZ)->Fit("gaus", "RQWW", "", -2, 2);
    DrawH1(HM()->H1(name1dZ), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    DrawMuSigma(canvas->cd(3), HM()->H1(name1dZ));
}

void BmnTrackingQaReport::DrawMuSigma(TVirtualPad* pad, TH1* h) {
    pad->cd();
    TF1 *fit = h->GetFunction("gaus");
    if (!fit) return;
    Float_t xMax = h->GetXaxis()->GetXmax();
    Float_t yMax = h->GetMaximum();
    TPaveStats* ps = new TPaveStats(xMax / 2, yMax / 2, xMax, yMax);
    ps->SetFillColor(0);
    ps->SetShadowColor(0);
    ps->AddText(Form("#mu = %2.3f", fit->GetParameter(1)));
    ps->AddText(Form("#sigma = %2.3f", fit->GetParameter(2)));
    ps->Draw();
}

void BmnTrackingQaReport::DrawOneH1(const TString canvasName, const TString name1, const TString drawOpt) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 500, 500);
    canvas->SetGrid();
    DrawH1(HM()->H1(name1), kLinear, kLog, drawOpt.Data(), kRed, 1, 0.75, 1.1, 20);
}

void BmnTrackingQaReport::DrawTwoH1(const TString canvasName, const TString name1, const TString name2, const TString drawOpt) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1000, 500);
    canvas->SetGrid();
    canvas->Divide(2, 1);
    canvas->cd(1);
    DrawH1(HM()->H1(name1), kLinear, kLog, drawOpt.Data(), kRed, 1, 0.75, 1.1, 20);
    canvas->cd(2);
    DrawH1(HM()->H1(name2), kLinear, kLinear, drawOpt.Data(), kRed, 1, 0.75, 1.1, 20);
}

void BmnTrackingQaReport::DrawOneH2(const TString canvasName, const TString name1) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 500, 500);
    canvas->SetGrid();
    DrawH2(HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
}

void BmnTrackingQaReport::DrawHitRes(TString pref, TString axis) {
    TString name = Form("%s %s Residuals between hits and tracks", pref.Data(), axis.Data());
    TCanvas* canvas = CreateCanvas(name.Data(), name.Data(), 1200, 1200);
    canvas->SetGrid();
    canvas->Divide(3, 3);
    for (Int_t i = 0; i < 9; ++i) {
        canvas->cd(i + 1);
        HM()->H1(Form("Res%s_%dst", axis.Data(), i))->Fit("gaus", "RQ", "", -0.05, 0.05);
        DrawH1(HM()->H1(Form("Res%s_%dst", axis.Data(), i)), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
        //        HM()->H1(Form("Res%s_%dst", axis.Data(), i))->GetYaxis()->SetMaxDigits(2);
        //        TGaxis::SetMaxDigits(2);
        DrawMuSigma(canvas->cd(i + 1), HM()->H1(Form("Res%s_%dst", axis.Data(), i)));
    }
}

void BmnTrackingQaReport::DrawTwoH2(const TString canvasName, const TString name1, const TString name2) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1000, 500);
    canvas->SetGrid();
    canvas->Divide(2, 1);
    canvas->cd(1);
    DrawH2(HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    DrawH2(HM()->H2(name2), kLinear, kLinear, kLinear, "colz");
}

void BmnTrackingQaReport::DrawThreeH2(const TString canvasName, const TString name1, const TString name2, const TString name3) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1500, 500);
    canvas->SetGrid();
    canvas->Divide(3, 1);
    canvas->cd(1);
    DrawH2(HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    DrawH2(HM()->H2(name2), kLinear, kLinear, kLinear, "colz");
    canvas->cd(3);
    DrawH2(HM()->H2(name3), kLinear, kLinear, kLinear, "colz");
}

void BmnTrackingQaReport::DrawThreeH1(const TString canvasName, const TString name1, const TString name2, const TString name3) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1500, 500);
    canvas->SetGrid();
    canvas->Divide(3, 1);
    canvas->cd(1);
    DrawH1(HM()->H1(name1), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    canvas->cd(2);
    DrawH1(HM()->H1(name2), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    canvas->cd(3);
    DrawH1(HM()->H1(name3), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
}

ClassImp(BmnTrackingQaReport)
