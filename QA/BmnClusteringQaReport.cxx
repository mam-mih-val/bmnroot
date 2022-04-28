/**
 * \file BmnClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "BmnClusteringQaReport.h"
#include "BmnReportElement.h"
#include "BmnHistManager.h"
#include "BmnDrawHist.h"
#include "BmnUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripStation.h"
#include "TPaveStats.h"

#include <vector>
using namespace std;
using lit::NumberToString;
using lit::Split;
using lit::FindAndReplace;

//string DefaultHitEfficiencyLabelFormatter(const string& histName, const BmnHistManager* hm) {
//    Double_t efficiency = (histName.find("_Eff_") != string::npos) ?
//            BmnClusteringQaReport::CalcEfficiency(hm->H1(FindAndReplace(histName, "_Eff_", "_Rec_")), hm->H1(FindAndReplace(histName, "_Eff_", "_Acc_")), 100.) :
//            BmnClusteringQaReport::CalcEfficiency(hm->H1(FindAndReplace(histName, "_CloneProb_", "_Clone_")), hm->H1(FindAndReplace(histName, "_CloneProb_", "_Acc_")), 100.);
//    vector<string> split = Split(histName, '_');
//    return split[1] + ":" + split[3] + "(" + NumberToString<Double_t > (efficiency, 1) + ")";
//}
//
//string DefaultAccAndRecLabelFormatter(const string& histName, const BmnHistManager* hm) {
//    Int_t nofEvents = hm->H1("_hen_EventNo_ClusteringQa")->GetEntries();
//    vector<string> split = Split(histName, '_');
//    return split[3] + " (" + NumberToString<Double_t > (hm->H1(histName)->GetEntries() / nofEvents, 1) + ")";
//}

BmnClusteringQaReport::BmnClusteringQaReport(Int_t nOfStationsGEM, Int_t nOfStationsSil) :
BmnSimulationReport(),
nStationsGEM(nOfStationsGEM),        
nStationsSil(nOfStationsSil),    
fGemDetector(NULL) {
    SetReportName("clustering_qa");  
}

BmnClusteringQaReport::~BmnClusteringQaReport() {

}

void BmnClusteringQaReport::Create() {
    Out().precision(3);
    Out() << R()->DocumentBegin();
    Out() << R()->Title(0, GetTitle());
    Out() << PrintEventInfo();
    PrintCanvases();
    Out() << R()->DocumentEnd();
}

string BmnClusteringQaReport::PrintEventInfo() {
    Out() << "<h2>Event generator: QGSM</h2>" << endl;
    Out() << "<h2>Energy: 4 GeV/n</h2>" << endl;
    if (GetOnlyPrimes()) Out() << "<h2>Results only for primaries presented</h2>" << endl;
    Out() << "<h2>Number of events: " << HM()->H1("hen_EventNo_ClusteringQa")->GetEntries() << "</h2>" << endl;
    Out() << "<h2>Mean multiplicity: " << HM()->H1("Multiplicity")->GetMean() << "</h2>" << endl;
    return "<hr>";
}

void BmnClusteringQaReport::DrawEventsInfo(const string& canvasName) {
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

void BmnClusteringQaReport::DrawResXbyStation(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 300*((nStationsGEM+nStationsSil)/4+1));
    canvas->Divide(4, ((nStationsGEM+nStationsSil)/4+1));
    canvas->SetGrid();
    for (Int_t i = 0; i < nStationsGEM; ++i) {
        canvas->cd(i + 1);
        TString resXname = Form("ResX_%dst_gem", i);
        DrawH1(HM()->H1(resXname.Data()), kLinear, kLog);
    }
    for (Int_t i = 0; i < nStationsSil; ++i) {
        canvas->cd(i + 1+nStationsGEM);
        TString resXname = Form("ResX_%dst_sil", i);
        DrawH1(HM()->H1(resXname.Data()), kLinear, kLog);
    }
}

void BmnClusteringQaReport::DrawResYbyStation(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 300*((nStationsGEM+nStationsSil)/4+1));
    canvas->Divide(4, ((nStationsGEM+nStationsSil)/4)+1);
    canvas->SetGrid();
    for (Int_t i = 0; i < nStationsGEM; ++i) {
        canvas->cd(i + 1);
        TString resYname = Form("ResY_%dst_gem", i);
        DrawH1(HM()->H1(resYname.Data()), kLinear, kLog);
    }
    for (Int_t i = 0; i < nStationsSil; ++i) {
        canvas->cd(i + 1+nStationsGEM);
        TString resYname = Form("ResY_%dst_sil", i);
        DrawH1(HM()->H1(resYname.Data()), kLinear, kLog);
    }
    
}

void BmnClusteringQaReport::DrawSimXRecXbyStation(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 300*((nStationsGEM+nStationsSil)/4+1));
    canvas->Divide(4, (nStationsGEM/4+1+nStationsSil/4));
    canvas->SetGrid();
    for (Int_t i = 0; i < nStationsGEM; ++i) {
        canvas->cd(i + 1);
        TString pntXhitXname = Form("PntX_vs_HitX_%dst_gem", i);
        DrawH2(HM()->H2(pntXhitXname.Data()), kLinear, kLinear, kLinear, "colz");
    }
    for (Int_t i = 0; i < nStationsSil; ++i) {
        canvas->cd(i + 1+nStationsGEM);
        TString pntXhitXname = Form("PntX_vs_HitX_%dst_sil", i);
        DrawH2(HM()->H2(pntXhitXname.Data()), kLinear, kLinear, kLinear, "colz");
    }
}

void BmnClusteringQaReport::DrawSimYRecYbyStation(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 300*((nStationsGEM+nStationsSil)/4+1));
    canvas->Divide(4, ((nStationsGEM+nStationsSil)/4+1));
    canvas->SetGrid();
    for (Int_t i = 0; i < nStationsGEM; ++i) {
        canvas->cd(i + 1);
        TString pntYhitYname = Form("PntY_vs_HitY_%dst_gem", i);
        DrawH2(HM()->H2(pntYhitYname.Data()), kLinear, kLinear, kLinear, "colz");
    }
    for (Int_t i = 0; i < nStationsSil; ++i) {
        canvas->cd(i + 1+nStationsGEM);
        TString pntYhitYname = Form("PntY_vs_HitY_%dst_sil", i);
        DrawH2(HM()->H2(pntYhitYname.Data()), kLinear, kLinear, kLinear, "colz");
    }
}

void BmnClusteringQaReport::DrawOccupancyByStation(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 300*((nStationsGEM+nStationsSil)/4+1));
    canvas->Divide(4, ((nStationsGEM+nStationsSil)/4+1));
    canvas->SetGrid();
    const Int_t nofEvents = HM()->H1("hen_EventNo_ClusteringQa")->GetEntries();
    const Float_t xWidth = HM()->H2("Occupancy_0st_gem")->GetXaxis()->GetBinWidth(1);
    const Float_t yWidth = HM()->H2("Occupancy_0st_gem")->GetYaxis()->GetBinWidth(1);
    const Float_t square = xWidth * yWidth; //cm^2

    for (Int_t i = 0; i < nStationsGEM; ++i) {
        canvas->cd(i + 1);
        TString occupname = Form("Occupancy_%dst_gem", i);
        HM()->H2(occupname.Data())->Sumw2();
        HM()->H2(occupname.Data())->Scale(1. / nofEvents / square);
        DrawH2(HM()->H2(occupname.Data()), kLinear, kLinear, kLinear, "colz");
    }
    
    for (Int_t i = 0; i < nStationsSil; ++i) {
        canvas->cd(i + 1 + nStationsGEM);
        TString occupname = Form("Occupancy_%dst_sil", i);
        HM()->H2(occupname.Data())->Sumw2();
        HM()->H2(occupname.Data())->Scale(1. / nofEvents / square);
        DrawH2(HM()->H2(occupname.Data()), kLinear, kLinear, kLinear, "colz");
    }
}

void BmnClusteringQaReport::DrawPullXbyStation(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 300*((nStationsGEM+nStationsSil)/4+1));
    canvas->Divide(4, ((nStationsGEM+nStationsSil)/4+1));
    canvas->SetGrid();
    for (Int_t i = 0; i < nStationsGEM; ++i) {
        canvas->cd(i + 1);
        TString pullXname = Form("PullX_%dst_gem", i);
        DrawH1(HM()->H1(pullXname.Data()), kLinear, kLinear,"", kBlue);
        
        HM()->H1(pullXname)->Fit("gaus", "RRQW", "", -5, 5);
        TF1 *fit = HM()->H1(pullXname)->GetFunction("gaus");
              
        Float_t xMax = HM()->H1(pullXname)->GetXaxis()->GetXmax();
        Float_t yMax = HM()->H1(pullXname)->GetMaximum();
        TPaveStats* ps = new TPaveStats(xMax / 1.5, yMax / 5, xMax, yMax);
        ps->SetFillColor(0);
        ps->SetShadowColor(0);
        ps->AddText(Form("#mu = %2.2f", fit->GetParameter(1)));
        ps->AddText(Form("#sigma = %2.2f", fit->GetParameter(2)));
        ps->Draw();
    }
    
    for (Int_t i = 0; i < nStationsSil; ++i) {
        canvas->cd(i + 1 + nStationsGEM);
        TString pullXname = Form("PullX_%dst_sil", i);
        DrawH1(HM()->H1(pullXname.Data()), kLinear, kLinear,"", kBlue);
        
        HM()->H1(pullXname)->Fit("gaus", "RRQW", "", -5, 5);
        TF1 *fit = HM()->H1(pullXname)->GetFunction("gaus");
              
        Float_t xMax = HM()->H1(pullXname)->GetXaxis()->GetXmax();
        Float_t yMax = HM()->H1(pullXname)->GetMaximum();
        TPaveStats* ps = new TPaveStats(xMax / 1.5, yMax / 5, xMax, yMax);
        ps->SetFillColor(0);
        ps->SetShadowColor(0);
        ps->AddText(Form("#mu = %2.2f", fit->GetParameter(1)));
        ps->AddText(Form("#sigma = %2.2f", fit->GetParameter(2)));
        ps->Draw();
            
    }
}

void BmnClusteringQaReport::DrawPullYbyStation(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 300*((nStationsGEM+nStationsSil)/4+1));
    canvas->Divide(4, ((nStationsGEM+nStationsSil)/4+1));
    canvas->SetGrid();
    for (Int_t i = 0; i < nStationsGEM; ++i) {
        canvas->cd(i + 1);
        TString pullYname = Form("PullY_%dst_gem", i);
        DrawH1(HM()->H1(pullYname.Data()), kLinear, kLinear,"", kBlue);
        
        HM()->H1(pullYname)->Fit("gaus", "RRQW", "", -5, 5);
        TF1 *fit = HM()->H1(pullYname)->GetFunction("gaus");
              
        Float_t xMax = HM()->H1(pullYname)->GetXaxis()->GetXmax();
        Float_t yMax = HM()->H1(pullYname)->GetMaximum();
        TPaveStats* ps = new TPaveStats(xMax / 1.5, yMax / 5, xMax, yMax);
        ps->SetFillColor(0);
        ps->SetShadowColor(0);
        ps->AddText(Form("#mu = %2.2f", fit->GetParameter(1)));
        ps->AddText(Form("#sigma = %2.2f", fit->GetParameter(2)));
        ps->Draw();
    }
    
    for (Int_t i = 0; i < nStationsSil; ++i) {
        canvas->cd(i + 1 + nStationsGEM);
        TString pullYname = Form("PullY_%dst_sil", i);
        DrawH1(HM()->H1(pullYname.Data()), kLinear, kLinear,"", kBlue);
        
        HM()->H1(pullYname)->Fit("gaus", "RRQW", "", -5, 5);
        TF1 *fit = HM()->H1(pullYname)->GetFunction("gaus");
              
        Float_t xMax = HM()->H1(pullYname)->GetXaxis()->GetXmax();
        Float_t yMax = HM()->H1(pullYname)->GetMaximum();
        TPaveStats* ps = new TPaveStats(xMax / 1.5, yMax / 5, xMax, yMax);
        ps->SetFillColor(0);
        ps->SetShadowColor(0);
        ps->AddText(Form("#mu = %2.2f", fit->GetParameter(1)));
        ps->AddText(Form("#sigma = %2.2f", fit->GetParameter(2)));
        ps->Draw();
    }
}


string BmnClusteringQaReport::PrintNofObjects() const {
//    vector<TH1*> histos = HM()->H1Vector("_hno_NofObjects_.+_Event");
//    Int_t nofHistos = histos.size();
//    string str = R()->TableBegin("Average number of objects per event", list_of("Name")("Value"));
//    for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
//        string cellName = Split(histos[iHist]->GetName(), '_')[3];
//        str += R()->TableRow(list_of(cellName)(NumberToString<Int_t > (histos[iHist]->GetMean())));
//    }
//    str += R()->TableEnd();
//    return str;
    return "NOT IMPLEMENTED!";
}

void BmnClusteringQaReport::Draw() {

    DrawEventsInfo("Distribution of impact parameter and multiplicity");
    CalculateEfficiencyHistos("Acc", "Rec", "Eff");
    CalculateEfficiencyHistos("Acc", "Clone", "CloneProb");

    DrawOccupancyByStation("Occupancy for each station");
    DrawResXbyStation("X-residuals for each station");
    DrawResYbyStation("Y-residuals for each station");
    DrawPullXbyStation("X-pulls for each station");
    DrawPullYbyStation("Y-pulls for each station");
    DrawSimXRecXbyStation("Reconstructed X vs. Simulated X for each station");
    DrawSimYRecYbyStation("Reconstructed Y vs. Simulated Y for each station");
        
    
    DrawNofObjectsHistograms("Gem", "Event");
    //    DrawNofObjectsHistograms("Tof1", "Event");
    //    DrawNofObjectsHistograms("Dch1", "Event");
    //    DrawNofObjectsHistograms("Dch2", "Event");
    //    DrawNofObjectsHistograms("Tof2", "Event");

    DrawNofObjectsHistograms("Gem", "Station");

    //    DrawH1ByPattern("hpa_.*Cluster_NofDigisInCluster_H1");
    //    DrawH2ByPattern("hpa_.*Cluster_NofDigisInCluster_H2", kLinear, kLinear, kLinear, "colz");

    //    DrawH1ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H1");
    //    DrawH2ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H2", kLinear, kLinear, kLinear, "colz");

//    DrawH1ByPattern("_hpa_.*Hit_Sigma.*_H1");
//    DrawH2ByPattern("_hpa_.*Hit_Sigma.*_H2", kLinear, kLinear, kLinear, "colz");

    DrawResidualsAndPulls("Gem");
    //    DrawResidualsAndPulls("Tof1");
    //    DrawResidualsAndPulls("Dch1");
    //    DrawResidualsAndPulls("Dch2");
    //    DrawResidualsAndPulls("Tof2");

//    DrawH1ByPattern("_hhe_Gem_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
    //    DrawH1ByPattern("_hhe_Tof1_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
    //    DrawH1ByPattern("_hhe_Dch1_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
    //    DrawH1ByPattern("_hhe_Dch2_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
    //    DrawH1ByPattern("_hhe_Tof2_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
}

void BmnClusteringQaReport::DrawNofObjectsHistograms(const string& detName, const string& parameter) {
//    if (!HM()->Exists("_hno_NofObjects_" + detName + "Points_" + parameter)) return;
//    string canvasName = GetReportName() + "_NofObjects_" + detName + "_" + parameter;
//    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
//    canvas->SetGrid();
//    canvas->cd();
//    vector<string> labels = list_of("Points")/*("Digis")("Clusters")*/("Hits");
//    vector<TH1*> histos = list_of(HM()->H1("_hno_NofObjects_" + detName + "Points_" + parameter))(HM()->H1("_hno_NofObjects_" + detName + "Hits_" + parameter)); //(HM()->H1("hno_NofObjects_" + detName + "Digis_" + parameter))(HM()->H1("hno_NofObjects_" + detName + "Clusters_" + parameter));
//    //    if (HM()->Exists("hno_NofObjects_" + detName + "PixelHits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "PixelHits_" + parameter));
//    //    else if (HM()->Exists("hno_NofObjects_" + detName + "StrawHits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "StrawHits_" + parameter));
//    //    else if (HM()->Exists("hno_NofObjects_" + detName + "Hits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "Hits_" + parameter));
//    DrawH1(histos, labels, kLinear, kLinear, true, 0.65, 0.75, 0.95, 0.99, "PE1X0", kFALSE);
}

void BmnClusteringQaReport::DrawResidualsAndPulls(const string& detName) {
//    if (!(HM()->Exists("_hrp_" + detName + "_ResidualX_H2") && HM()->Exists("_hrp_" + detName + "_ResidualY_H2")
//            && HM()->Exists("_hrp_" + detName + "_PullX_H2") && HM()->Exists("_hrp_" + detName + "_ResidualX_H2"))) return;
//    vector<string> par = list_of("ResidualX")("ResidualY")("PullX")("PullY");
//    Int_t nofCanvases = par.size();
//    for (Int_t iCanvas = 0; iCanvas < nofCanvases; iCanvas++) {
//        string histName = "_hrp_" + detName + "_" + par[iCanvas] + "_H2";
//        TH2* hist = HM()->H2(histName);
//        string canvasName = GetReportName() + "_" + histName + "_station";
//        TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 900);
//
//        Int_t nofBins, nofColumns, nofRows;
//        if (detName == "Gem") {
//            nofBins = 12;
//            nofColumns = 4;
//            nofRows = (nofBins / nofColumns) + ((nofBins % 4 == 0) ? 0 : 1);
//            canvas->Divide(nofColumns, nofRows);
//        } else {
//            nofBins = 1;
//        }
//        for (Int_t iBin = 1; iBin <= nofBins; iBin++) {
//            stringstream ss;
//            ss << histName << "_" << iBin << "_py";
//            TH1* projY = hist->ProjectionY(ss.str().c_str(), iBin, iBin);
//            projY->SetNameTitle(ss.str().c_str(), ss.str().c_str());
//            projY->SetXTitle(par[iCanvas].c_str());
//            projY->SetYTitle("Yield");
//            canvas->cd(iBin);
//            DrawH1(projY, kLinear, kLinear);
//            projY->Fit("gaus", "SQ");
//        }
//    }
//    DrawH2ByPattern("_hrp_" + detName + "_.*_H2", kLinear, kLinear, kLinear, "colz");
}

Double_t BmnClusteringQaReport::CalcEfficiency(const TH1* histRec, const TH1* histAcc, Double_t scale) {
    if (histAcc->Integral() == 0 || histRec->Integral() == 0) {
        return 0.;
    } else {
        return scale * Double_t(histRec->Integral()) / Double_t(histAcc->Integral());
    }
}

void BmnClusteringQaReport::ScaleAndShrinkHistograms() {}

void BmnClusteringQaReport::DivideHistos(TH1* histo1, TH1* histo2, TH1* histo3, Double_t scale) {
    histo1->Sumw2();
    histo2->Sumw2();
    histo3->Sumw2();
    histo3->Divide(histo1, histo2, 1., 1., "B");
    histo3->Scale(scale);
}

void BmnClusteringQaReport::CalculateEfficiencyHistos(const string& acc, const string& rec, const string& eff) {}

ClassImp(BmnClusteringQaReport)
