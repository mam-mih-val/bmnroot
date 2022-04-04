#include "BmnDrawHist.h"
#include "BmnDrawOnline.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TPad.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TMath.h"
#include "TGaxis.h"
#include "TPaveLabel.h"

#include <string>
#include <limits>
#include <iostream>
#include <sstream>
#include <cassert>


using std::stringstream;


BmnDrawOnline::BmnDrawOnline(TString param, TString storageName){
	//For param use:
	//RECREATE	Create a new file, if the file already exists it will be overwritten.
	//UPDATE	Open an existing file for writing. If no file exists, it is created.
	//canvStorage = TFile::Open("canvStorage.root",param);
	canvStorage = new TFile(storageName + ".root", param);
}

BmnDrawOnline::BmnDrawOnline(TFile* file, Int_t  port):	fServer(nullptr){
	InitServer(port);
	RegisterCanvases(file);
}								

BmnDrawOnline::~BmnDrawOnline() {
    if (fServer) delete fServer;
}


void BmnDrawOnline::DrawH1(
        TCanvas* canvas,
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
    
	gPad->SetTopMargin(0.20);
    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.15);
    gPad->SetTicks(1, 1);
    if (fillColor > 0) hist->SetFillColor(fillColor);
    
    if (canvName != canvas->GetName()) {
        canvName = canvas->GetName();
		canvVect.push_back(canvas);
		maxHeight += canvas->GetWh();
    }
        
    hist->Draw(drawOpt.c_str());
    gPad->SetGrid(true, true);
    hist->SetStats(false);
    
}

/* Draw TH2 histogram.*/
void BmnDrawOnline::DrawH2(
        TCanvas* canvas,
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
	
    
	gPad->SetTopMargin(0.20);
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.30);
    gPad->SetBottomMargin(0.15);
    gPad->SetTicks(1, 1);

    if (canvName != canvas->GetName()) {
        canvName = canvas->GetName();
		canvVect.push_back(canvas);
		maxHeight += canvas->GetWh();
    }
 
    hist->Draw(drawOpt.c_str());
    gPad->SetGrid(true, true);
    hist->SetStats(false);
}

/* Draw several TH1 histograms. */
void BmnDrawOnline::DrawH1(
        TCanvas* canvas,
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
        DrawH1(canvas, hist, logx, logy, opt, BmnDrawingOptions::Color(iHist), BmnDrawingOptions::LineWidth(),
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

void BmnDrawOnline::DrawH1(
        TCanvas* canvas,
        TEfficiency* hist) {
        
	gPad->SetTopMargin(0.20);
    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.15);
    gPad->SetTicks(1, 1);
       
    if (canvName != canvas->GetName()) {
        canvName = canvas->GetName();
		canvVect.push_back(canvas);
		maxHeight += canvas->GetWh();
    }
        
    hist->Draw();
    gPad->SetGrid(true, true);
       
}




void BmnDrawOnline::InitServer(Int_t port){
	
    const Char_t* httpStr = Form("http:%d;cors;noglobal", port);

    // Start http-server ...
    fServer = new THttpServer(httpStr);


}


/* Принцип работы DrawMainCanvas():
1.Создается холст с высотой, равной суммарной высоте всех холстов с гистограммами. 2.Размечаем его так, чтобы он состоял из одного столбца из числа строк, равному общему числу холстов. 
3.Для каждого холста создаем TPad и TPaveLabel  - будем использовать их, чтобы подписывать холсты.
4.Запускаем цикл, с первой строчки общего холста, до последней, в котором:
	С помощью метода DrawClonePad() на нужную строку холста добавляем набор 	гистограмм из вектора canvVect.
	Поверх нарисованных гистограмм добавляем панель  TPad, на этой панели, с помощью 	TPaveLabel подписываем заголовок для нужного подхолста.
5.Полученный холст записывается в root-файл canvStorage.root
6.Зануляются значения  общей высоты, вектора холстов и имени холста.
*/


void BmnDrawOnline::DrawMainCanvas(TString nameOfCanv){
	
	Int_t height = maxHeight;
	TCanvas* mainCanvas = new TCanvas(nameOfCanv,"title",1280, height);
	Int_t numOfCanvases = (canvVect.size());
	mainCanvas -> Divide(1,numOfCanvases,0.01,0.002);
	TPad* newpad[numOfCanvases];
	TPaveLabel* title[numOfCanvases];
			
	for( Int_t i=0; i < numOfCanvases; ++i) {
		mainCanvas->cd(i+1);
		canvVect[i] -> DrawClonePad();
		
		TString newpadName = "newpad" + std::to_string(i);
		newpad[i]=new TPad(newpadName,"a transparent pad",0.0,0.93,1.0,1.0);
		newpad[i]->SetFillStyle(4000);
		newpad[i]->Draw();
		newpad[i]->cd();
		title[i] = new TPaveLabel(0.0,0.0,1.0,1.0,canvVect[i] ->GetName());
		title[i]->SetFillColor(16);
		title[i]->SetTextFont(52);
		title[i]->Draw();
		
	}
	mainCanvas -> Write();
	maxHeight = 0;
	canvName = "null";
	vector<TCanvas*>().swap(canvVect);
}
				

void BmnDrawOnline:: RegisterCanvases(TFile* file){
	TString namesOfCanv[] = {"inner", "TOF", "CSC", "DCH", "pid", "He3", "pi+", "e-", "proton", "K+", "D", "T", "He4"};
	Int_t size = sizeof(namesOfCanv)/sizeof(namesOfCanv[0]);
	for (Int_t i = 0; i < size; ++i) {
        TCanvas* canv = (TCanvas *)file->Get(namesOfCanv[i]);
		if (!canv) continue;
		fServer -> Register("/Objects", canv);
        
    }
}

ClassImp(BmnDrawOnline);

