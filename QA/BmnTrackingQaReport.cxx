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
#include <boost/assign/list_of.hpp>
#include <vector>
#include <set>
#include "TString.h"

using namespace std;
using namespace lit;
using boost::assign::list_of;

BmnTrackingQaReport::BmnTrackingQaReport() :
BmnSimulationReport(),
fGlobalTrackVariants() {
    SetReportName("tracking_qa");
}

BmnTrackingQaReport::BmnTrackingQaReport(vector<string> header) :
BmnSimulationReport(),
fGlobalTrackVariants(),
fHeader(header) {
    SetReportName("tracking_qa");
}

BmnTrackingQaReport::~BmnTrackingQaReport() {
}

void BmnTrackingQaReport::Create() {
    Out().precision(3);
    Out() << R()->DocumentBegin();
    Out() << R()->Title(0, GetTitle());
    Out() << PrintEventInfo();
    Out() << PrintNofObjects();
    PrintCanvases();
    Out() << R()->DocumentEnd();
}

string BmnTrackingQaReport::PrintEventInfo() {
    Out() << "<h2>Event generator: QGSM</h2>" << endl;
    Out() << "<h2>Energy: 4 GeV/n</h2>" << endl;
    if (GetOnlyPrimes()) Out() << "<h2>Results only for primaries presented</h2>" << endl;
    Out() << "<h2>Number of events: " << HM()->H1("hen_EventNo_TrackingQa")->GetEntries() << "</h2>" << endl;
    Out() << "<h2>Mean multiplicity: " << HM()->H1("Multiplicity")->GetMean() << "</h2>" << endl;
    Out() << "<hr>" << endl;
    Out() << "<h3><font color=\"red\">Reconstructable</font> MC-track:</h3>" << "Monte Carlo track with at least <font color=\"red\">4</font> Monte Carlo points in GEM" << endl;
    Out() << "<h3><font color=\"red\">Good</font> track:</h3>" << "Reconstructed track with at least <font color=\"red\">4</font> hits in GEM and <font color=\"red\">60%</font> of them corresponded the same MC-track" << endl;
    Out() << "<h3><font color=\"red\">Clone</font> tracks:</h3>";
    Out() << "Two or more reconstructed tracks with reference to the same MC-track." << endl;
    Out() << "The number of clones is subtracted from number of good tracks before efficiency calculation." << endl;
    return "<hr>";
}

string BmnTrackingQaReport::PrintNofObjects() const {
    vector<TH1*> histos = HM()->H1Vector("hno_NofObjects_.+");
    Int_t nofHistos = histos.size();
    string str = R()->TableBegin("Average number of objects per event", list_of("Name")("Value"));
    for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
        string cellName = Split(histos[iHist]->GetName(), '_')[2];
        str += R()->TableRow(list_of(cellName)(NumberToString<Int_t > (histos[iHist]->GetMean())));
    }
    str += R()->TableEnd();
    return str;
}

void BmnTrackingQaReport::Draw() {
    DrawEventsInfo("Distribution of impact parameter and multiplicity");
    SetDefaultDrawStyle();
    
    TString pNamesIn[5] = {"Sim_vs_P_gem", "Rec_vs_P_gem", "Well_vs_P_gem", "Ghost_vs_P_gem", "Split_vs_P_gem"};
    TString pNamesOut[3] = {"Eff_vs_P_gem", "Fake_vs_P_gem", "SplitEff_vs_P_gem"};
    DrawEffGem("Distribution of MC-tracks, reco-tracks, fakes and clones vs P_sim per event for GEM TRACKS", pNamesIn, pNamesOut);
    
    TString etaNamesIn[5] = {"Sim_vs_Eta_gem", "Rec_vs_Eta_gem", "Well_vs_Eta_gem", "Ghost_vs_Eta_gem", "Split_vs_Eta_gem"};
    TString etaNamesOut[3] = {"Eff_vs_Eta_gem", "Fake_vs_Eta_gem", "SplitEff_vs_Eta_gem"};
    DrawEffGem("Distribution of MC-tracks, reco-tracks, fakes and clones vs Pseudorapidity per event for GEM TRACKS", etaNamesIn, etaNamesOut);
    
    TString thetaNamesIn[5] = {"Sim_vs_Theta_gem", "Rec_vs_Theta_gem", "Well_vs_Theta_gem", "Ghost_vs_Theta_gem", "Split_vs_Theta_gem"};
    TString thetaNamesOut[3] = {"Eff_vs_Theta_gem", "Fake_vs_Theta_gem", "SplitEff_vs_Theta_gem"};
    DrawEffGem("Distribution of MC-tracks, reco-tracks, fakes and clones vs theta per event for GEM TRACKS", thetaNamesIn, thetaNamesOut);
    
    DrawNhitsGem("Distribution of GEM RECO-tracks vs number of hits per track");
    
    DrawTwoH2("Distribution of GEM reconstructable MC-tracks (left) and MC-tracks corresponded to reconstructed tracks (right) vs number of MC-points and Pseudorapidity", "Nh_sim_Eta_sim_gem", "Nh_rec_Eta_rec_gem");
    DrawTwoH2("Distribution of GEM reconstructable MC-tracks (left) and MC-tracks corresponded to reconstructed tracks (right) vs number of MC-points and Momentum", "Nh_sim_P_sim_gem", "Nh_rec_P_rec_gem");
    DrawTwoH2("Distribution of GEM reconstructable MC-tracks (left) and MC-tracks corresponded to reconstructed tracks (right) vs number of MC-points and Theta", "Nh_sim_Theta_sim_gem", "Nh_rec_Theta_rec_gem");

    DrawTwoH2("Distribution of MC-tracks and GEM-tracks in Pseudorapidity and Momentum", "EtaP_sim", "EtaP_rec_gem");
    DrawTwoH2("Distribution of MC-tracks and GEM-tracks in theta and Momentum", "ThetaP_sim", "ThetaP_rec_gem");
    DrawTwoH2("P_reco vs P_mc for GEM-tracks", "P_rec_P_sim_gem", "Pt_rec_Pt_sim_gem");
    
    DrawOneH2("Pseudorapidity_reco vs Pseudorapidity_mc for GEM-tracks", "Eta_rec_Eta_sim_gem");
    DrawTwoH2("Tx_reco vs Tx_mc (left) and Ty_reco vs Ty_mc (right) for GEM-tracks", "Tx_rec_Tx_sim_gem", "Ty_rec_Ty_sim_gem");
    DrawThreeH2("Reco vs MC for X-, Y- and Z-component of Momentum for GEM-tracks", "Px_rec_Px_sim_gem", "Py_rec_Py_sim_gem", "Pz_rec_Pz_sim_gem");
    DrawMomResGem("Momentum resolution for GEM-tracks", "momRes_2D_gem", "momRes_1D_gem");
    DrawOneH2("Momentum resolution vs Chi2", "MomRes_vs_Chi2_gem");
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
    HM()->H1(sim.Data())->Sumw2();
    HM()->H1(sim.Data())->Scale(1. / nofEvents);
    HM()->H1(rec.Data())->Sumw2();
    HM()->H1(rec.Data())->Scale(1. / nofEvents);
    HM()->H1(well.Data())->Sumw2();
    HM()->H1(well.Data())->Scale(1. / nofEvents);
    HM()->H1(ghost.Data())->Sumw2();
    HM()->H1(ghost.Data())->Scale(1. / nofEvents);
    HM()->H1(split.Data())->Sumw2();
    HM()->H1(split.Data())->Scale(1. / nofEvents);

    HM()->H1(sim.Data())->SetMinimum(0.0);
    HM()->H1(well.Data())->SetMinimum(0.0);
    HM()->H1(ghost.Data())->SetMinimum(0.0);
    HM()->H1(split.Data())->SetMinimum(0.0);

    vector<TH1*> histos1;
    histos1.push_back(HM()->H1(sim.Data()));
    histos1.push_back(HM()->H1(well.Data()));
    histos1.push_back(HM()->H1(ghost.Data()));
    histos1.push_back(HM()->H1(split.Data()));
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

    //    HM()->H1("EffGemDistr")->Divide(HM()->H1("recoGemDistr"), HM()->H1("allGemDistr"), 1., 1., "B");
    HM()->H1(eff.Data())->Divide(HM()->H1(well.Data()), HM()->H1(sim.Data()), 1., 1., "B");
    HM()->H1(eff.Data())->Scale(100.0);
    HM()->H1(fake.Data())->Divide(HM()->H1(ghost.Data()), HM()->H1(rec.Data()), 1., 1., "B");
    HM()->H1(fake.Data())->Scale(100.0);
    HM()->H1(clon.Data())->Divide(HM()->H1(split.Data()), HM()->H1(rec.Data()), 1., 1., "B");
    HM()->H1(clon.Data())->Scale(100.0);

    // Boundary checking.
    // These cases shouldn't happen, but they happen sometimes...
    for (Int_t i = 0; i < HM()->H1(eff.Data())->GetNbinsX(); ++i) {
        if (HM()->H1(eff.Data())->GetBinContent(i) > 100.0) {
            HM()->H1(eff.Data())->SetBinContent(i, 100.0);
            HM()->H1(eff.Data())->SetBinError(i, 0.0);
        }
        if (HM()->H1(fake.Data())->GetBinContent(i) > 100.0) {
            HM()->H1(fake.Data())->SetBinContent(i, 100.0);
            HM()->H1(fake.Data())->SetBinError(i, 0.0);
        }
        if (HM()->H1(clon.Data())->GetBinContent(i) > 100.0) {
            HM()->H1(clon.Data())->SetBinContent(i, 100.0);
            HM()->H1(clon.Data())->SetBinError(i, 0.0);
        }
        if (HM()->H1(eff.Data())->GetBinContent(i) < 0.0) {
            HM()->H1(eff.Data())->SetBinContent(i, 0.0);
            HM()->H1(eff.Data())->SetBinError(i, 0.0);
        }
        if (HM()->H1(fake.Data())->GetBinContent(i) < 0.0) {
            HM()->H1(fake.Data())->SetBinContent(i, 0.0);
            HM()->H1(fake.Data())->SetBinError(i, 0.0);
        }
        if (HM()->H1(clon.Data())->GetBinContent(i) < 0.0) {
            HM()->H1(clon.Data())->SetBinContent(i, 0.0);
            HM()->H1(clon.Data())->SetBinError(i, 0.0);
        }
    }

    HM()->H1(eff.Data())->SetMaximum(100.0);
    HM()->H1(fake.Data())->SetMaximum(100.0);
    HM()->H1(clon.Data())->SetMaximum(100.0);
    HM()->H1(eff.Data())->SetMinimum(0.0);
    HM()->H1(fake.Data())->SetMinimum(0.0);
    HM()->H1(clon.Data())->SetMinimum(0.0);

    vector<TH1*> histos2;
    histos2.push_back(HM()->H1(eff.Data()));
    histos2.push_back(HM()->H1(fake.Data()));
    histos2.push_back(HM()->H1(clon.Data()));
    DrawH1(histos2, labels2, kLinear, kLinear, true, 0.5, 0.9, 1.0, 0.99, "PE1X0", kFALSE);
}

void BmnTrackingQaReport::DrawNhitsGem(const string& canvasName) {
    Int_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 600);
    canvas->SetGrid();
    HM()->H1("Well_vs_Nh_gem")->Sumw2();
    HM()->H1("Well_vs_Nh_gem")->Scale(1. / nofEvents);
    HM()->H1("Ghost_vs_Nh_gem")->Sumw2();
    HM()->H1("Ghost_vs_Nh_gem")->Scale(1. / nofEvents);
    vector<TH1*> histos1;
    histos1.push_back(HM()->H1("Well_vs_Nh_gem"));
    histos1.push_back(HM()->H1("Ghost_vs_Nh_gem"));
    vector<string> labels1;
    labels1.push_back("Good tracks");
    labels1.push_back("Ghosts");
    DrawH1(histos1, labels1, kLinear, kLinear, true, 0.5, 0.9, 1.0, 0.99, "PE1", kFALSE);
}

void BmnTrackingQaReport::DrawEventsInfo(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1500, 500);
    canvas->SetGrid();
    canvas->Divide(3, 1);
    canvas->cd(1);
    DrawH1(HM()->H1("Impact parameter"), kLinear, kLinear, "", kRed, 2, 1, 1.1, 20, 33);
    canvas->cd(2);
    DrawH1(HM()->H1("Multiplicity"), kLinear, kLinear, "", kRed, 2, 1, 1.1, 20, 33);
    canvas->cd(3);
    DrawH2(HM()->H2("Impact_Mult"), kLinear, kLinear, kLinear, "colz");
}

void BmnTrackingQaReport::DrawMomResGem(const string& canvasName, TString name2d, TString name1d) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1000, 500);
    canvas->SetGrid();
    canvas->Divide(2, 1);
    canvas->cd(1);
    
    DrawH2(HM()->H2(name2d.Data()), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    
    Int_t momResStep = 10;
    for (Int_t iBin = 0; iBin < HM()->H2(name2d.Data())->GetNbinsX(); iBin += momResStep) {
        TH1D* proj = HM()->H2(name2d.Data())->ProjectionY("tmp", iBin, iBin + (momResStep - 1));
        proj->Fit("gaus", "SQRww", "", -10.0, 10.0);
        TF1 *fit = proj->GetFunction("gaus");
        Float_t mean = TMath::Abs(fit->GetParameter(1));//(fit->GetParameter(1) < 50.0) ? fit->GetParameter(1) : 0.0;
        Float_t sigma = fit->GetParameter(2);
        Float_t mom = HM()->H2(name2d.Data())->GetXaxis()->GetBinCenter(iBin);
        Int_t nBins = HM()->H1(name1d.Data())->GetXaxis()->GetNbins();
        Int_t bin = (mom - 0.0) / (5.0 - 0.0) * nBins;
        HM()->H1(name1d.Data())->SetBinContent(bin, mean);
//        fHM->H1("momRes_1D_gem")->SetBinContent(bin, projMax);
//        HM()->H1("momRes_1D_gem")->SetBinError(bin, sigma);
        HM()->H1(name1d.Data())->SetBinError(bin, 0.0);
    }
    
    HM()->H1(name1d.Data())->SetMaximum(10.0);
    HM()->H1(name1d.Data())->SetMinimum(0.0);
    DrawH1(HM()->H1(name1d.Data()), kLinear, kLinear, "PE1", kRed, 0.7, 0.75, 1.1, 20);
}

void BmnTrackingQaReport::DrawOneH2(const TString canvasName, const TString name1) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 500, 500);
    canvas->SetGrid();
    DrawH2(HM()->H2(name1.Data()), kLinear, kLinear, kLinear, "colz");
}

void BmnTrackingQaReport::DrawTwoH2(const TString canvasName, const TString name1, const TString name2) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1000, 500);
    canvas->SetGrid();
    canvas->Divide(2, 1);
    canvas->cd(1);
    DrawH2(HM()->H2(name1.Data()), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    DrawH2(HM()->H2(name2.Data()), kLinear, kLinear, kLinear, "colz");
}

void BmnTrackingQaReport::DrawThreeH2(const TString canvasName, const TString name1, const TString name2, const TString name3) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1500, 500);
    canvas->SetGrid();
    canvas->Divide(3, 1);
    canvas->cd(1);
    DrawH2(HM()->H2(name1.Data()), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    DrawH2(HM()->H2(name2.Data()), kLinear, kLinear, kLinear, "colz");
    canvas->cd(3);
    DrawH2(HM()->H2(name3.Data()), kLinear, kLinear, kLinear, "colz");
}

ClassImp(BmnTrackingQaReport)
