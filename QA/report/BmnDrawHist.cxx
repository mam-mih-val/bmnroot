/**
 * \file BmnDrawHist.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/
#include "BmnDrawHist.h"

#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TPad.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TMath.h"
#include "TGaxis.h"

#include <string>
#include <limits>
#include <iostream>
#include <sstream>
#include <cassert>

using std::stringstream;

/* Set default styles for histograms. */
void SetDefaultDrawStyle() {
    gStyle->SetOptStat("rm");
    gStyle->SetOptFit(1);
    gStyle->SetOptTitle(0);

    gStyle->SetCanvasColor(kWhite);
    gStyle->SetFrameFillColor(kWhite);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetPadColor(kWhite);
    gStyle->SetStatColor(kWhite);
    gStyle->SetTitleFillColor(kWhite);
    gStyle->SetPalette(77);
}

/* Draw TH1 histogram.*/
void DrawH1(
        TH1* hist,
        HistScale logx,
        HistScale logy,
        const string& drawOpt,
        Int_t lineColor,
        Int_t lineWidth,
        Int_t lineStyle,
        Float_t markerSize,
        Int_t markerStyle,
        Int_t fillColor) {
    Double_t textSize = BmnDrawingOptions::TextSize();
    hist->SetLineColor(lineColor);
    hist->SetLineWidth(lineWidth);
    hist->SetLineStyle(lineStyle);
    hist->SetMarkerColor(lineColor);
    hist->SetMarkerSize(markerSize);
    hist->SetMarkerStyle(markerStyle);
    if (logx == kLog) {
        gPad->SetLogx();
    }
    if (logy == kLog) {
        gPad->SetLogy();
    }
    hist->GetXaxis()->SetLabelSize(textSize);
    hist->GetXaxis()->SetNdivisions(505, kTRUE);
    hist->GetYaxis()->SetLabelSize(textSize);
    hist->GetXaxis()->SetTitleSize(textSize);
    hist->GetYaxis()->SetTitleSize(textSize);
    hist->GetXaxis()->SetTitleOffset(1.0);
    hist->GetYaxis()->SetTitleOffset(1.3);
    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.15);
    gPad->SetTicks(1, 1);
    if (fillColor > 0) hist->SetFillColor(fillColor);
    hist->Draw(drawOpt.c_str());
    gPad->SetGrid(true, true);
    hist->SetStats(false);
}

/* Draw TH2 histogram.*/
void DrawH2(
        TH2* hist,
        HistScale logx,
        HistScale logy,
        HistScale logz,
        const string& drawOpt) {
    Double_t textSize = BmnDrawingOptions::TextSize();
    if (logx == kLog) {
        gPad->SetLogx();
    }
    if (logy == kLog) {
        gPad->SetLogy();
    }
    if (logz == kLog) {
        gPad->SetLogz();
    }
    hist->GetXaxis()->SetLabelSize(textSize);
    hist->GetXaxis()->SetNdivisions(505, kTRUE);
    hist->GetYaxis()->SetLabelSize(textSize);
    hist->GetYaxis()->SetNdivisions(505, kTRUE);
    hist->GetZaxis()->SetLabelSize(textSize);
    // hist->GetZaxis()->SetNdivisions(505, kTRUE);
    hist->GetXaxis()->SetTitleSize(textSize);
    hist->GetYaxis()->SetTitleSize(textSize);
    hist->GetZaxis()->SetTitleSize(textSize);
    hist->GetXaxis()->SetTitleOffset(1.0);
    hist->GetYaxis()->SetTitleOffset(1.3);
    hist->GetZaxis()->SetTitleOffset(1.5);
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.30);
    gPad->SetBottomMargin(0.15);
    gPad->SetTicks(1, 1);
    hist->Draw(drawOpt.c_str());
    gPad->SetGrid(true, true);
    hist->SetStats(false);
}

/* Draw several TH1 histograms. */
void DrawH1(
        const vector<TH1*>& histos,
        const vector<string>& histLabels,
        HistScale logx,
        HistScale logy,
        Bool_t drawLegend,
        Double_t x1,
        Double_t y1,
        Double_t x2,
        Double_t y2,
        const string& drawOpt,
        Bool_t outputMeanValue01) {
    assert(histos.size() != 0 && histLabels.size() == histos.size());
    Double_t max = std::numeric_limits<Double_t>::min();
    Int_t nofHistos = histos.size();
    TLegend* legend = new TLegend(x1, y1, x2, y2);
    legend->SetFillColor(kWhite);
    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
        TH1* hist = histos[iHist];
        string opt = (iHist == 0) ? drawOpt : (iHist == nofHistos - 1) ? "SAME" + drawOpt : "SAME" + drawOpt;
        //string opt = drawOpt;
        DrawH1(hist, logx, logy, opt, BmnDrawingOptions::Color(iHist), BmnDrawingOptions::LineWidth(),
                BmnDrawingOptions::LineStyle(0), BmnDrawingOptions::MarkerSize(), BmnDrawingOptions::MarkerStyle(iHist));
        max = std::max(max, hist->GetMaximum());
        Int_t nonZeroBins = 0;
        for (Int_t i = 0; i < hist->GetNbinsX(); ++i) {
            if (hist->GetBinContent(i) != 0.0) nonZeroBins++;
        }

        Double_t xMax = hist->GetBinCenter(hist->GetNbinsX());
        Double_t xGev = 1.0; //1 GeV
        Int_t nGev = Int_t(hist->GetNbinsX() * xGev / xMax);
        Double_t effAll = hist->Integral() / nonZeroBins;
        Double_t eff01 = hist->Integral(0.0, nGev) / nGev; // efficiency from 0 to 1 GeV
        TString legText;
        if (outputMeanValue01) legText = TString::Format("%s | mean =  %3.1f | mean01 =  %3.1f", histLabels[iHist].c_str(), effAll, eff01).Data();
        else legText = TString::Format("%s | mean =  %3.1f", histLabels[iHist].c_str(), effAll).Data();
        legend->AddEntry(hist, legText, "lp");
        //To choose the best combination of parameters
        FILE* f = fopen("test.dat", "a+");
        if (((TString) hist->GetName()).Contains("vs_P")) fprintf(f, "%s %s %f %f\n", hist->GetName(), histLabels[iHist].c_str(), effAll, eff01);
        fclose(f);
    }
    histos[0]->SetMaximum(max * 1.10);

    if (drawLegend) {
        legend->Draw();
    }
}

///* Draw several TH1 histograms. */
//void DrawH1(
//        const vector<TH1*>& histos,
//        const vector<string>& histLabels,
//        HistScale logx,
//        HistScale logy,
//        Bool_t drawLegend,
//        Double_t x1,
//        Double_t y1,
//        Double_t x2,
//        Double_t y2,
//        const string& drawOpt,
//        Bool_t drawMeanLine,
//        Bool_t printMeanValue) {
//    assert(histos.size() != 0 && histLabels.size() == histos.size());
//    Double_t max = std::numeric_limits<Double_t>::min();
//    Int_t nofHistos = histos.size();
//    TLegend* legend = new TLegend(x1, y1, x2, y2);
//    legend->SetFillColor(kWhite);
//    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
//        TH1* hist = histos[iHist];
//        string opt = (iHist == 0) ? drawOpt : (iHist == nofHistos - 1) ? "SAME" + drawOpt : "SAME" + drawOpt;
//        //string opt = drawOpt;
//        DrawH1(hist, logx, logy, opt, BmnDrawingOptions::Color(iHist), BmnDrawingOptions::LineWidth(),
//                BmnDrawingOptions::LineStyle(0), BmnDrawingOptions::MarkerSize(), BmnDrawingOptions::MarkerStyle(iHist));
//        max = std::max(max, hist->GetMaximum());
//        Int_t nonZeroBins = 0;
//        for (Int_t i = 0; i < hist->GetNbinsX(); ++i) {
//            if (hist->GetBinContent(i) != 0.0) nonZeroBins++;
//        }
//        if (printMeanValue == kTRUE) {
//            legend->AddEntry(hist, TString::Format("%s | mean =  %3.1f", histLabels[iHist].c_str(), hist->Integral() / nonZeroBins).Data(), "lp");
//
//        } else {
//            Double_t xMax = hist->GetBinCenter(hist->GetNbinsX());
//            Double_t xGev = 1.0; //1 GeV
//            Int_t nGev = Int_t(hist->GetNbinsX() * xGev / xMax);
//            Double_t effAll = hist->Integral() / nonZeroBins;
//            Double_t eff01 = hist->Integral(0.0, nGev) / nGev; // efficiency from 0 to 1 GeV
//            legend->AddEntry(hist, TString::Format("%s | mean =  %3.1f | mean01 =  %3.1f", histLabels[iHist].c_str(), effAll, eff01).Data(), "lp");
//            FILE* f = fopen("test.dat", "a+");
//            if (((TString) hist->GetName()).Contains("Eff_vs_P_glob") ||
//                    ((TString) hist->GetName()).Contains("Fake_vs_P_glob") ||
//                    ((TString) hist->GetName()).Contains("SplitEff_vs_P_glob"))
//                fprintf(f, "%s %s %f\n", hist->GetName(), histLabels[iHist].c_str(), hist->Integral() / nonZeroBins);
//            fclose(f);
//        }
//        if (drawMeanLine == kTRUE) DrawMeanLine(hist);
//    }
//    histos[0]->SetMaximum(max * 1.10);
//
//    if (drawLegend) {
//        legend->Draw();
//    }
//}

void DrawMeanLine(TH1* hist) {

    Float_t minX = hist->GetXaxis()->GetXmin();
    Float_t maxX = hist->GetXaxis()->GetXmax();
    Int_t nonZeroBins = 0;
    for (Int_t i = 0; i < hist->GetNbinsX(); ++i) {
        if (hist->GetBinContent(i) != 0.0) nonZeroBins++;
    }
    TLine* line = new TLine(minX, hist->Integral() / nonZeroBins, maxX, hist->Integral() / nonZeroBins);
    line->SetLineWidth(2);
    line->SetLineColor(hist->GetLineColor());
    line->Draw();
}

void DrawGraph(
        TGraph* graph,
        HistScale logx,
        HistScale logy,
        const string& drawOpt,
        Int_t color,
        Int_t lineWidth,
        Int_t lineStyle,
        Int_t markerSize,
        Int_t markerStyle) {
    Double_t textSize = BmnDrawingOptions::TextSize();
    graph->SetLineColor(color);
    graph->SetLineWidth(lineWidth);
    graph->SetLineStyle(lineStyle);
    graph->SetMarkerColor(color);
    graph->SetMarkerSize(markerSize);
    graph->SetMarkerStyle(markerStyle);
    if (drawOpt.find("A") != string::npos) {
        if (logx == kLog) {
            gPad->SetLogx();
        }
        if (logy == kLog) {
            gPad->SetLogy();
        }
        graph->GetXaxis()->SetLabelSize(textSize);
        graph->GetXaxis()->SetNdivisions(505, kTRUE);
        graph->GetYaxis()->SetLabelSize(textSize);
        graph->GetXaxis()->SetTitleSize(textSize);
        graph->GetYaxis()->SetTitleSize(textSize);
        graph->GetXaxis()->SetTitleOffset(1.0);
        graph->GetYaxis()->SetTitleOffset(1.3);
    }
    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.15);
    graph->Draw(drawOpt.c_str());
    gPad->SetGrid(true, true);
}

/* Draw several TGraphs. */
void DrawGraph(
        const vector<TGraph*>& graphs,
        const vector<string>& graphLabels,
        HistScale logx,
        HistScale logy,
        Bool_t drawLegend,
        Double_t x1,
        Double_t y1,
        Double_t x2,
        Double_t y2) {
    assert(graphs.size() != 0 && graphs.size() == graphLabels.size());

    Double_t max = std::numeric_limits<Double_t>::min();
    Double_t min = std::numeric_limits<Double_t>::max();
    TLegend* legend = new TLegend(x1, y1, x2, y2);
    legend->SetFillColor(kWhite);
    Int_t nofGraphs = graphs.size();
    for (UInt_t iGraph = 0; iGraph < nofGraphs; iGraph++) {
        TGraph* graph = graphs[iGraph];
        string opt = (iGraph == 0) ? "ACP" : "CP";
        DrawGraph(graph, logx, logy, opt, BmnDrawingOptions::Color(iGraph), BmnDrawingOptions::LineWidth(),
                BmnDrawingOptions::LineStyle(iGraph), BmnDrawingOptions::MarkerSize(), BmnDrawingOptions::MarkerStyle(iGraph));
        max = std::max(graph->GetYaxis()->GetXmax(), max);
        min = std::min(graph->GetYaxis()->GetXmin(), min);
        legend->AddEntry(graph, graphLabels[iGraph].c_str(), "lp");
    }
    graphs[0]->SetMaximum(max);
    graphs[0]->SetMinimum(min);
    if (drawLegend) {
        legend->Draw();
    }
}

/* Draws 2D graph.*/
void DrawGraph2D(
        TGraph2D* graph,
        HistScale logx,
        HistScale logy,
        HistScale logz,
        const string& drawOpt) {
    Double_t textSize = BmnDrawingOptions::TextSize();
    if (logx == kLog) {
        gPad->SetLogx();
    }
    if (logy == kLog) {
        gPad->SetLogy();
    }
    if (logz == kLog) {
        gPad->SetLogz();
    }
    graph->GetXaxis()->SetLabelSize(textSize);
    graph->GetXaxis()->SetNdivisions(505, kTRUE);
    graph->GetYaxis()->SetLabelSize(textSize);
    graph->GetYaxis()->SetNdivisions(505, kTRUE);
    graph->GetZaxis()->SetLabelSize(textSize);
    //   graph->GetZaxis()->SetNdivisions(505, kTRUE);
    graph->GetXaxis()->SetTitleSize(textSize);
    graph->GetYaxis()->SetTitleSize(textSize);
    graph->GetZaxis()->SetTitleSize(textSize);
    graph->GetXaxis()->SetTitleOffset(1.0);
    graph->GetYaxis()->SetTitleOffset(1.3);
    graph->GetZaxis()->SetTitleOffset(1.5);
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.30);
    gPad->SetBottomMargin(0.15);
    gPad->SetTicks(1, 1);
    graph->Draw(drawOpt.c_str());
    gPad->SetGrid(true, true);
}
