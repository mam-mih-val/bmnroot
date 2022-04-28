/**
 * \file BmnPidQaReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2011-2021
 */
#include "BmnPidQaReport.h"
#include <fstream>
#include <set>
#include <vector>
#include "BmnPidQa.h"
#include "BmnUtils.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TGaxis.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLine.h"
#include "TPaveStats.h"
#include "TString.h"
#include "TStyle.h"
#include "map"
#include "BmnDrawHist.h"
#include "BmnDrawOnline.h"
#include "BmnHistManager.h"
#include "BmnReportElement.h"

const Float_t pMax = 5.0;

using namespace std;
using namespace lit;

BmnPidQaReport::BmnPidQaReport() :BmnSimulationReport(),
                                  fGlobalTrackVariants(){
    SetReportName("pid_qa");
}

BmnPidQaReport::BmnPidQaReport(TString name, TString storageName, unordered_map<Double_t,string> massTable)
                                                         : BmnSimulationReport(),
                                                         fGlobalTrackVariants(),
                                                         fPrefix(name + ": "),
                                                         fMassTable(massTable),
                                                         fStorageName(storageName),
                                                         effTof400(nullptr),
                                                         effTof700(nullptr),
                                                         contTof400(nullptr),
                                                         contTof700(nullptr),
                                                         effHitsTof400(nullptr),
                                                         contHitsTof400(nullptr),
                                                         effHitsTof700(nullptr),
                                                         contHitsTof700(nullptr){
    SetReportName(name);
}

BmnPidQaReport::~BmnPidQaReport() {	
}

void BmnPidQaReport::Create() {
    Out().precision(3);
    Out() << R()->DocumentBegin();
    Out() << R()->Title(0, GetTitle());
    Out() << "<hr>" << endl;
    PrintCanvases();
    Out() << R()->DocumentEnd();
}


void BmnPidQaReport::Draw() {
    SetDefaultDrawStyle();
	drawHist = new BmnDrawOnline("RECREATE", fStorageName);

        
    DrawTwoBananas(fPrefix + "Banana-plots",
                "Banana-plot TOF-400","Banana-plot TOF-700");
    
    drawHist->DrawMainCanvas("pid");

    for (auto iter = fMassTable.begin(); iter != fMassTable.end(); ++iter){

        string nameOfParticle = (*iter).second;

        DrawThreeH1(fPrefix + "Total identified particles TOF-400 for " + nameOfParticle, 
                    "TOF-400 total_vs_P for " + nameOfParticle,
                    "TOF-400 true_vs_P for " + nameOfParticle,
                    "TOF-400 false_vs_P for " + nameOfParticle);
       
        DrawThreeH1(fPrefix + "Total identified particles TOF-700 for " + nameOfParticle, 
                    "TOF-700 total_vs_P for " + nameOfParticle, 
                    "TOF-700 true_vs_P for " + nameOfParticle, 
                    "TOF-700 false_vs_P for " + nameOfParticle);

                          
        DrawThreeH2(fPrefix + "Total-True-False identified particles TOF-400 in P_Beta for " + nameOfParticle,
                    "TOF-400 total rigidity-momentum for " + nameOfParticle,
                    "TOF-400 true rigidity-momentum for " + nameOfParticle,
                    "TOF-400 false rigidity-momentum for " + nameOfParticle);

       /* DrawTwoH2(fPrefix + "Undivided in time TOF400 for " + nameOfParticle,
                    "Undivided TOF-400 rigidity-momentum for " + nameOfParticle,
                    "Undivided TOF-400 time from P for " + nameOfParticle);*/

        DrawThreeH2(fPrefix + "Total-True-False identified particles TOF-700 in P_Beta for " + nameOfParticle,
                    "TOF-700 total rigidity-momentum for " + nameOfParticle,
                    "TOF-700 true rigidity-momentum for " + nameOfParticle,
                    "TOF-700 false rigidity-momentum for " + nameOfParticle);

        /*DrawTwoH2(fPrefix + "Undivided in time TOF700 for " + nameOfParticle,
                    "Undivided TOF-700 rigidity-momentum for " + nameOfParticle,
                    "Undivided TOF-700 time from P for " + nameOfParticle);*/
       
        //Efficiency TOF-400
        
        effTof400 = new TEfficiency(*(HM()->H1("TOF-400 true_vs_P for " + nameOfParticle)),
        *(HM()->H1("TOF-400 total_vs_P for " + nameOfParticle)));
    
        //Contamination TOF-400
        HM()->H1("TOF-400 true-false_vs_P for " + nameOfParticle)-> Add(HM()->H1("TOF-400 true_vs_P for " + nameOfParticle),
        HM()->H1("TOF-400 false_vs_P for " + nameOfParticle));
        
        contTof400 = new TEfficiency(*(HM()->H1("TOF-400 false_vs_P for " + nameOfParticle)), *(HM()->H1("TOF-400 true-false_vs_P for " + nameOfParticle)));

        // Efficiency Nhits TOF-400

        effHitsTof400 = new TEfficiency(*(HM()->H1("TOF-400 true_vs_NOfHits for " + nameOfParticle)),
        *(HM()->H1("TOF-400 total_vs_NOfHits for " + nameOfParticle)));

        // Contamination Nhits TOF-400
        HM()->H1("TOF-400 true-false_vs_NOfHits for " + nameOfParticle)-> Add(HM()->H1("TOF-400 true_vs_NOfHits for " + nameOfParticle),
        HM()->H1("TOF-400 false_vs_NOfHits for " + nameOfParticle));
        
        contHitsTof400 = new TEfficiency(*(HM()->H1("TOF-400 false_vs_NOfHits for " + nameOfParticle)), *(HM()->H1("TOF-400 true-false_vs_NOfHits for " + nameOfParticle)));

        //Efficiency TOF-700           
        effTof700 = new TEfficiency(*(HM()->H1("TOF-700 true_vs_P for " + nameOfParticle)),
        *(HM()->H1("TOF-700 total_vs_P for " + nameOfParticle)));
        
                
        //Contamination TOF-700
        HM()->H1("TOF-700 true-false_vs_P for " + nameOfParticle)-> Add(HM()->H1("TOF-700 true_vs_P for " + nameOfParticle),
        HM()->H1("TOF-700 false_vs_P for " + nameOfParticle));
        
        contTof700 = new TEfficiency(*(HM()->H1("TOF-700 false_vs_P for " + nameOfParticle)), *(HM()->H1("TOF-700 true-false_vs_P for " + nameOfParticle)));

        // Efficiency Nhits TOF-700
        effHitsTof700 = new TEfficiency(*(HM()->H1("TOF-700 true_vs_NOfHits for " + nameOfParticle)),
        *(HM()->H1("TOF-700 total_vs_NOfHits for " + nameOfParticle)));

        // Contamination Nhits TOF-700
        HM()->H1("TOF-700 true-false_vs_NOfHits for " + nameOfParticle)-> Add(HM()->H1("TOF-700 true_vs_NOfHits for " + nameOfParticle),
        HM()->H1("TOF-700 false_vs_NOfHits for " + nameOfParticle));
        
        contHitsTof700 = new TEfficiency(*(HM()->H1("TOF-700 false_vs_NOfHits for " + nameOfParticle)), *(HM()->H1("TOF-700 true-false_vs_NOfHits for " + nameOfParticle)));
   

        DrawEffCont(fPrefix + "TOF-400 and TOF-700 Efficiency for " + nameOfParticle, effTof400, effTof700);
        DrawEffCont(fPrefix + "TOF-400 and TOF-700 Contamination for " + nameOfParticle, contTof400, contTof700);    
        DrawEffCont(fPrefix + "TOF-400 and TOF-700 Efficiency  NOfHits for " + nameOfParticle, effHitsTof400, effHitsTof700);
        DrawEffCont(fPrefix + "TOF-400 and TOF-700 Contamination  NOfHits for " + nameOfParticle, contHitsTof400, contHitsTof700);   
       
     
       //Velocity

        DrawThreeH2(fPrefix + "Total-True-False velocity from P TOF-400 for " + nameOfParticle,
                    "Total velocity from P TOF-400 for " + nameOfParticle,
                    "True velocity from P TOF-400 for " + nameOfParticle, "False velocity from P TOF-400 for " + nameOfParticle);
        
             
        DrawThreeH2(fPrefix + "Total-True-False velocity from P TOF-700 for " + nameOfParticle,
                    "Total velocity from P TOF-700 for " + nameOfParticle,
                    "True velocity from P TOF-700 for " + nameOfParticle, "False velocity from P TOF-700 for " + nameOfParticle);

        //Time

        DrawThreeH2(fPrefix + "Total-True-False time from P TOF-400 for " + nameOfParticle,
                    "Total time from P TOF-400 for " + nameOfParticle,
                    "True time from P TOF-400 for " + nameOfParticle, "False time from P TOF-400 for " + nameOfParticle);
        

        DrawThreeH2(fPrefix + "Total-True-False time from P TOF-700 for " + nameOfParticle,
                    "Total time from P TOF-700 for " + nameOfParticle,
                    "True time from P TOF-700 for " + nameOfParticle, "False time from P TOF-700 for " + nameOfParticle);

        //Mass^2  

        DrawThreeH2(fPrefix + "Total-True-False mass^2 from P TOF-400 for " + nameOfParticle,
                    "Total mass^2 from P TOF-400 for " + nameOfParticle,
                    "True mass^2 from P TOF-400 for " + nameOfParticle, "False mass^2 from P TOF-400 for " + nameOfParticle);

        
        
        
             
        DrawThreeH2(fPrefix + "Total-True-False mass^2 from P TOF-700 for " + nameOfParticle,
                    "Total mass^2 from P TOF-700 for " + nameOfParticle,
                    "True mass^2 from P TOF-700 for " + nameOfParticle, "False mass^2 from P TOF-700 for " + nameOfParticle);

                  
        drawHist->DrawMainCanvas(nameOfParticle);
       
    }    

}



void BmnPidQaReport::DrawOneH1(const TString canvasName, const TString name1, const TString drawOpt) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 500, 500);
    canvas->SetGrid();
    drawHist->DrawH1(canvas, HM()->H1(name1), kLinear, kLinear, drawOpt.Data(), kRed, 1, 0.75, 1.1, 20);
}

void BmnPidQaReport::DrawTwoH1(const TString canvasName, const TString name1, const TString name2, const TString drawOpt) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1000, 500);
    canvas->SetGrid();
    canvas->Divide(2, 1);
    canvas->cd(1);
    drawHist->DrawH1(canvas, HM()->H1(name1), kLinear, kLinear, drawOpt.Data(), kRed, 1, 0.75, 1.1, 20);
    canvas->cd(2);
    drawHist->DrawH1(canvas, HM()->H1(name2), kLinear, kLinear, drawOpt.Data(), kRed, 1, 0.75, 1.1, 20);
}

void BmnPidQaReport::DrawOneH2(const TString canvasName, const TString name1) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 500, 500);
    canvas->SetGrid();
    drawHist->DrawH2(canvas, HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
}

void BmnPidQaReport::DrawTwoH2(const TString canvasName, const TString name1, const TString name2) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1000, 500);
    canvas->SetGrid();
    canvas->Divide(2, 1);
    canvas->cd(1);
    drawHist->DrawH2(canvas, HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    drawHist->DrawH2(canvas, HM()->H2(name2), kLinear, kLinear, kLinear, "colz");
}

void BmnPidQaReport::DrawThreeH2(const TString canvasName, const TString name1, const TString name2, const TString name3) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 2100, 500);
    canvas->SetGrid();
    canvas->Divide(3, 1);
    canvas->cd(1);
    drawHist->DrawH2(canvas, HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    drawHist->DrawH2(canvas, HM()->H2(name2), kLinear, kLinear, kLinear, "colz");
    canvas->cd(3);
    drawHist->DrawH2(canvas, HM()->H2(name3), kLinear, kLinear, kLinear, "colz");
}

void BmnPidQaReport::DrawTwoBananas(const TString canvasName, const TString name1, const TString name2) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1000, 500);
    canvas->SetGrid();
    canvas->Divide(2, 1);
    canvas->cd(1);    
    drawHist->DrawH2(canvas, HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    drawHist->DrawH2(canvas, HM()->H2(name2), kLinear, kLinear, kLinear, "colz");
}

void BmnPidQaReport::DrawThreeH1(const TString canvasName, const TString name1, const TString name2, const TString name3) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1500, 500);
    canvas->SetGrid();
    canvas->Divide(3, 1);
    canvas->cd(1);
    drawHist->DrawH1(canvas, HM()->H1(name1), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    canvas->cd(2);
    drawHist->DrawH1(canvas, HM()->H1(name2), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    canvas->cd(3);
    drawHist->DrawH1(canvas, HM()->H1(name3), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
}

void BmnPidQaReport::DrawEffCont(const TString canvasName, TEfficiency* efficiency, TEfficiency* contamination) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 1000, 500);
    canvas->SetGrid();
    canvas->Divide(2, 1);
    canvas->cd(1);
    drawHist->DrawH1(canvas, efficiency);
    canvas->cd(2);
    drawHist->DrawH1(canvas, contamination);
}

ClassImp(BmnPidQaReport)
