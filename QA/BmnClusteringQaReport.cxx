/**
 * \file BmnClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "BmnClusteringQaReport.h"
#include "report/BmnReportElement.h"
#include "report/BmnHistManager.h"
#include "report/BmnDrawHist.h"
#include "BmnUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "TProfile.h"
#include "TCanvas.h"
#include <boost/assign/list_of.hpp>
#include <vector>
using namespace std;
using boost::assign::list_of;
using lit::NumberToString;
using lit::Split;
using lit::FindAndReplace;

string DefaultHitEfficiencyLabelFormatter(const string& histName, const BmnHistManager* hm) {
    Double_t efficiency = (histName.find("_Eff_") != string::npos) ?
            BmnClusteringQaReport::CalcEfficiency(hm->H1(FindAndReplace(histName, "_Eff_", "_Rec_")), hm->H1(FindAndReplace(histName, "_Eff_", "_Acc_")), 100.) :
            BmnClusteringQaReport::CalcEfficiency(hm->H1(FindAndReplace(histName, "_CloneProb_", "_Clone_")), hm->H1(FindAndReplace(histName, "_CloneProb_", "_Acc_")), 100.);
    vector<string> split = Split(histName, '_');
    return split[1] + ":" + split[3] + "(" + NumberToString<Double_t > (efficiency, 1) + ")";
}

string DefaultAccAndRecLabelFormatter(const string& histName, const BmnHistManager* hm) {
    Int_t nofEvents = hm->H1("_hen_EventNo_ClusteringQa")->GetEntries();
    vector<string> split = Split(histName, '_');
    return split[3] + " (" + NumberToString<Double_t > (hm->H1(histName)->GetEntries() / nofEvents, 1) + ")";
}

BmnClusteringQaReport::BmnClusteringQaReport() :
BmnSimulationReport() {
    SetReportName("clustering_qa");
}

BmnClusteringQaReport::~BmnClusteringQaReport() {

}

void BmnClusteringQaReport::Create() {
    Out().precision(3);
    Out() << R()->DocumentBegin();
    Out() << R()->Title(0, GetTitle());

    Out() << "Number of events: " << HM()->H1("_hen_EventNo_ClusteringQa")->GetEntries() << endl;

    Out() << PrintNofObjects();

    PrintCanvases();
    Out() << R()->DocumentEnd();
}

string BmnClusteringQaReport::PrintNofObjects() const {
    vector<TH1*> histos = HM()->H1Vector("_hno_NofObjects_.+_Event");
    Int_t nofHistos = histos.size();
    string str = R()->TableBegin("Average number of objects per event", list_of("Name")("Value"));
    for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
        string cellName = Split(histos[iHist]->GetName(), '_')[3];
        str += R()->TableRow(list_of(cellName)(NumberToString<Int_t > (histos[iHist]->GetMean())));
    }
    str += R()->TableEnd();
    return str;
}

void BmnClusteringQaReport::Draw() {

    ScaleAndShrinkHistograms();
    CalculateEfficiencyHistos("Acc", "Rec", "Eff");
    CalculateEfficiencyHistos("Acc", "Clone", "CloneProb");

    DrawNofObjectsHistograms("Gem", "Event");
    DrawNofObjectsHistograms("Tof1", "Event");
    DrawNofObjectsHistograms("Dch1", "Event");
    DrawNofObjectsHistograms("Dch2", "Event");
    DrawNofObjectsHistograms("Tof2", "Event");

    DrawNofObjectsHistograms("Gem", "Station");

    //    DrawH1ByPattern("hpa_.*Cluster_NofDigisInCluster_H1");
    //    DrawH2ByPattern("hpa_.*Cluster_NofDigisInCluster_H2", kLinear, kLinear, kLinear, "colz");

    //    DrawH1ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H1");
    //    DrawH2ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H2", kLinear, kLinear, kLinear, "colz");

    DrawH1ByPattern("_hpa_.*Hit_Sigma.*_H1");
    DrawH2ByPattern("_hpa_.*Hit_Sigma.*_H2", kLinear, kLinear, kLinear, "colz");

    DrawResidualsAndPulls("Gem");
    DrawResidualsAndPulls("Tof1");
    DrawResidualsAndPulls("Dch1");
    DrawResidualsAndPulls("Dch2");
    DrawResidualsAndPulls("Tof2");

    DrawH1ByPattern("_hhe_Gem_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
    DrawH1ByPattern("_hhe_Tof1_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
    DrawH1ByPattern("_hhe_Dch1_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
    DrawH1ByPattern("_hhe_Dch2_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
    DrawH1ByPattern("_hhe_Tof2_All_(Eff|CloneProb)_Station", DefaultHitEfficiencyLabelFormatter);
}

void BmnClusteringQaReport::DrawNofObjectsHistograms(const string& detName, const string& parameter) {
    if (!HM()->Exists("_hno_NofObjects_" + detName + "Points_" + parameter)) return;
    string canvasName = GetReportName() + "_NofObjects_" + detName + "_" + parameter;
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
    canvas->SetGrid();
    canvas->cd();
    vector<string> labels = list_of("Points")/*("Digis")("Clusters")*/("Hits");
    vector<TH1*> histos = list_of(HM()->H1("_hno_NofObjects_" + detName + "Points_" + parameter))(HM()->H1("_hno_NofObjects_" + detName + "Hits_" + parameter)); //(HM()->H1("hno_NofObjects_" + detName + "Digis_" + parameter))(HM()->H1("hno_NofObjects_" + detName + "Clusters_" + parameter));
    //    if (HM()->Exists("hno_NofObjects_" + detName + "PixelHits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "PixelHits_" + parameter));
    //    else if (HM()->Exists("hno_NofObjects_" + detName + "StrawHits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "StrawHits_" + parameter));
    //    else if (HM()->Exists("hno_NofObjects_" + detName + "Hits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "Hits_" + parameter));
    DrawH1(histos, labels, kLinear, kLinear, true, 0.65, 0.75, 0.95, 0.99);
}

void BmnClusteringQaReport::DrawResidualsAndPulls(const string& detName) {
    if (!(HM()->Exists("_hrp_" + detName + "_ResidualX_H2") && HM()->Exists("_hrp_" + detName + "_ResidualY_H2")
            && HM()->Exists("_hrp_" + detName + "_PullX_H2") && HM()->Exists("_hrp_" + detName + "_ResidualX_H2"))) return;
    vector<string> par = list_of("ResidualX")("ResidualY")("PullX")("PullY");
    Int_t nofCanvases = par.size();
    for (Int_t iCanvas = 0; iCanvas < nofCanvases; iCanvas++) {
        string histName = "_hrp_" + detName + "_" + par[iCanvas] + "_H2";
        TH2* hist = HM()->H2(histName);
        string canvasName = GetReportName() + "_" + histName + "_station";
        TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 900);

        Int_t nofBins, nofColumns, nofRows;
        if (detName == "Gem") {
            nofBins = 12;
            nofColumns = 4;
            nofRows = (nofBins / nofColumns) + ((nofBins % 4 == 0) ? 0 : 1);
            canvas->Divide(nofColumns, nofRows);
        } else {
            nofBins = 1;
        }
        for (Int_t iBin = 1; iBin <= nofBins; iBin++) {
            stringstream ss;
            ss << histName << "_" << iBin << "_py";
            TH1* projY = hist->ProjectionY(ss.str().c_str(), iBin, iBin);
            projY->SetNameTitle(ss.str().c_str(), ss.str().c_str());
            projY->SetXTitle(par[iCanvas].c_str());
            projY->SetYTitle("Yield");
            canvas->cd(iBin);
            DrawH1(projY, kLinear, kLinear);
            projY->Fit("gaus", "SQ");
        }
    }
    DrawH2ByPattern("_hrp_" + detName + "_.*_H2", kLinear, kLinear, kLinear, "colz");
}

Double_t BmnClusteringQaReport::CalcEfficiency(const TH1* histRec, const TH1* histAcc, Double_t scale) {
    if (histAcc->Integral() == 0 || histRec->Integral() == 0) {
        return 0.;
    } else {
        return scale * Double_t(histRec->Integral()) / Double_t(histAcc->Integral());
    }
}

void BmnClusteringQaReport::ScaleAndShrinkHistograms() {
    Int_t nofEvents = HM()->H1("_hen_EventNo_ClusteringQa")->GetEntries();

    HM()->ScaleByPattern("_hhe_.+_.+_(Acc|Rec|Clone)_Station", 1. / nofEvents);

    HM()->ScaleByPattern("_hno_NofObjects_.*_Station", 1. / nofEvents);
    HM()->ShrinkEmptyBinsH1ByPattern("_hno_NofObjects_.*_Station");

    //    HM()->NormalizeToIntegralByPattern("hpa_.*Cluster_NofDigisInCluster_.*");
    //    HM()->ShrinkEmptyBinsH1ByPattern("hpa_.*Cluster_NofDigisInCluster_H1");
    //    HM()->ShrinkEmptyBinsH2ByPattern("hpa_.*Cluster_NofDigisInCluster_H2");

    HM()->NormalizeToIntegralByPattern("_hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_.*");
    HM()->ShrinkEmptyBinsH1ByPattern("_hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H1");
    HM()->ShrinkEmptyBinsH2ByPattern("_hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H2");

    HM()->NormalizeToIntegralByPattern("_hrp_.*_.*_H2");
    HM()->ShrinkEmptyBinsH2ByPattern("_hrp_.*_.*_H2");

    HM()->NormalizeToIntegralByPattern("_hpa_.*Hit_Sigma.*_.*");
    HM()->ShrinkEmptyBinsH1ByPattern("_hpa_.*Hit_Sigma.*_H1");
    HM()->ShrinkEmptyBinsH2ByPattern("_hpa_.*Hit_Sigma.*_H2");
}

void BmnClusteringQaReport::DivideHistos(TH1* histo1, TH1* histo2, TH1* histo3, Double_t scale) {
    histo1->Sumw2();
    histo2->Sumw2();
    histo3->Sumw2();
    histo3->Divide(histo1, histo2, 1., 1., "B");
    histo3->Scale(scale);
}

void BmnClusteringQaReport::CalculateEfficiencyHistos(const string& acc, const string& rec, const string& eff) {
    vector<TH1*> effHistos = HM()->H1Vector("_hhe_.+_" + eff + "_.+");
    Int_t nofEffHistos = effHistos.size();
    for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
        TH1* effHist = effHistos[iHist];
        string effHistName = effHist->GetName();
        string accHistName = FindAndReplace(effHistName, "_" + eff + "_", "_" + acc + "_");
        string recHistName = FindAndReplace(effHistName, "_" + eff + "_", "_" + rec + "_");
        DivideHistos(HM()->H1(recHistName), HM()->H1(accHistName), effHist, 100.);
        effHist->SetMinimum(0.);
        effHist->SetMaximum(100.);
    }
}

ClassImp(BmnClusteringQaReport)
