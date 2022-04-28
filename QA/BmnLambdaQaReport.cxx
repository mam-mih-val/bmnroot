/**
 * \file BmnLambdaQaReport.cxx
 * \brief visualising simulated lambda reconstruction performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Alexander Lytaev <sas-lyt@ya.ru> - modifications for BMN experiment 
 * \date 2018 July
 */

#include "BmnLambdaQaReport.h"
#include "BmnReportElement.h"
#include "BmnHistManager.h"
#include "BmnDrawHist.h"
#include "BmnUtils.h"
#include "TProfile.h"
#include "TF1.h"
#include "TH1.h"
#include "TFitResult.h"
#include "TCanvas.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripStation.h"
#include "TPaveStats.h"
#include "BmnLambdaQa.h"
#include "BmnDrawHist.h"
#include "BmnParticlePairsInfo.h"
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;
using lit::NumberToString;
using lit::Split;
using lit::FindAndReplace;

BmnLambdaQaReport::BmnLambdaQaReport(Bool_t useMCFile, Bool_t useRecoFile, BmnParticlePairsInfo* info, vector <TClonesArray*> branches) :
BmnSimulationReport(),
fUseMCFile(useMCFile),
fMC(branches[0]),
fUseRecoFile(useRecoFile),
fRECO(branches[1]),
fParticlePairsInfo(info),
drawPointsOpt("PE1") {
    SetReportName("lambda_qa");
}

BmnLambdaQaReport::~BmnLambdaQaReport() {
}

void BmnLambdaQaReport::Create() {
    Out() << R()->DocumentBegin();
    Out() << R()->Title(0, GetTitle());
    PrintEventInfo();
    PrintCanvases();
    Out() << R()->DocumentEnd();
}

void BmnLambdaQaReport::PrintEventInfo() {
    Out() << "<h1> <font color='00FF00'>The criterion for lambda to be reconstructable: </font></h1>" << endl;
    Out() << "<h1> <font color='00FF00'>Both of its decay products have at least </font><font color='FF0000'> 4 </font><font color='00FF00'> gem or silicon hits </font></h1>" << endl;

    if (fUseMCFile) {
        Out() << "<h1> Number of lambda:\n" << HM()->H1("numberOfLambdas_LambdaQa")->GetEntries() << "</h1>" << endl;
        Out() << "<h1> Number of reconstructable lambda:\n" << HM()->H1("numberOfReconstructableLambdas_LambdaQa")->GetEntries() << "</h1>" << endl;
        Double_t lRecEff = HM()->H1("numberOfReconstructableLambdas_LambdaQa")->GetEntries() / HM()->H1("numberOfLambdas_LambdaQa")->GetEntries();
        Out() << setprecision(2) << fixed << "<h1> Geometrical efficiency:\n" << lRecEff * 100 << "%" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1> Number of particle pairs reconstructed by MC tracks:\n" << HM()->H1("numberOfMCReconstructedParticlePairsQA")->GetEntries() << "</h1>" << endl;
        Out() << setprecision(2) << "<h1> Number of particle pairs reconstructed by MC tracks being lambda:\n" << HM()->H1("numberOfMCReconstructedLambdasQA")->GetEntries() << "</h1>" << endl;
        Out() << setprecision(2) << fixed << "<h1> Percentage of particle pairs reconstructed by MC tracks being lambda:\n" << HM()->H1("numberOfMCReconstructedLambdasQA")->GetEntries() / HM()->H1("numberOfMCReconstructedParticlePairsQA")->GetEntries()*100 << "% </h1>" << endl;

        Out() << "<h1> <font color='6f00ff'> Kinematic cuts for MC are: </font></h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getMomPart1Min() << " < P_1 < " << fParticlePairsInfo->getMomPart1Max() << " (GeV/c)" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getMomPart2Min() << " < P_2 < " << fParticlePairsInfo->getMomPart2Max() << " (GeV/c)" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getEtaPart1Min() << " < Eta_1 < " << fParticlePairsInfo->getEtaPart1Max() << "" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getEtaPart2Min() << " < Eta_2 < " << fParticlePairsInfo->getEtaPart2Max() << "" << "</h1>" << endl;
        Out() << "<h1> <font color='6f00ff'> Geometrical cuts for MC are: </font></h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getDCAPart1Min() << " < DCA1 < " << fParticlePairsInfo->getDCAPart1Max() << " cm" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getDCAPart2Min() << " < DCA2 < " << fParticlePairsInfo->getDCAPart2Max() << " cm" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getDCA12Min() << " < DCA_12 < " << fParticlePairsInfo->getDCA12Max() << " cm" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getPathMin() << " < PATH < " << fParticlePairsInfo->getPathMax() << " cm" << "</h1>" << endl;
    }

    if (fUseRecoFile) {
        Out() << setprecision(2) << "<h1> Number of reconstructed particle pairs:\n" << HM()->H1("numberOfReconstructedParticlePairsQA")->GetEntries() << "</h1>" << endl;
        Out() << "<h1> <font color='90ff00'> Kinematic cuts for reco are: </font></h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getMomPart1Min() << " < P_1 < " << fParticlePairsInfo->getMomPart1Max() << " (GeV/c)" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getMomPart2Min() << " < P_2 < " << fParticlePairsInfo->getMomPart2Max() << " (GeV/c)" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getEtaPart1Min() << " < Eta_1 < " << fParticlePairsInfo->getEtaPart1Max() << "" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getEtaPart2Min() << " < Eta_2 < " << fParticlePairsInfo->getEtaPart2Max() << "" << "</h1>" << endl;
        Out() << "<h1> <font color='90ff00'> Geometrical cuts for reco are: </font></h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getDCAPart1Min() << " < DCA1 < " << fParticlePairsInfo->getDCAPart1Max() << " cm" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getDCAPart2Min() << " < DCA2 < " << fParticlePairsInfo->getDCAPart2Max() << " cm" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getDCA12Min() << " < DCA_12 < " << fParticlePairsInfo->getDCA12Max() << " cm" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1>" << fParticlePairsInfo->getPathMin() << " < PATH < " << fParticlePairsInfo->getPathMax() << " cm" << "</h1>" << endl;
    }

    if (fUseRecoFile && fUseMCFile) {
        Double_t lRecEff = HM()->H1("numberOfReconstructableLambdas_LambdaQa")->GetEntries() / HM()->H1("numberOfLambdas_LambdaQa")->GetEntries();
        Double_t lRecEffR = HM()->H1("numberOfReconstructedParticlePairWithMatchedLambdaQA")->GetEntries() / HM()->H1("numberOfReconstructableLambdas_LambdaQa")->GetEntries();
        Out() << setprecision(2) << "<h1> Number of reconstructed particle pairs with primary particle having MC matched lambda:\n" << HM()->H1("numberOfReconstructedParticlePairWithMatchedLambdaQA")->GetEntries() << "</h1>" << endl;
        Out() << setprecision(2) << fixed << "<h1> Reconstruction algorithms efficiency:\n" << lRecEffR * 100 << "%" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1> Number of reconstructed fake lambdas:\n" << HM()->H1("numberOfReconstructedParticlePairsQA")->GetEntries() - HM()->H1("numberOfReconstructedParticlePairWithMatchedLambdaQA")->GetEntries() << "</h1>" << endl;
        Out() << setprecision(2) << fixed << "<h1> Total reconstruction efficiency:\n" << lRecEff * lRecEffR * 100 << "%" << "</h1>" << endl;
        Out() << setprecision(2) << "<h1> Number of not reconstructed lambdas:\n" << HM()->H1("numberOfLambdas_LambdaQa")->GetEntries() - HM()->H1("numberOfReconstructedParticlePairWithMatchedLambdaQA")->GetEntries() << "</h1>" << endl;
    }
}

void BmnLambdaQaReport::Draw() {
    if (fUseMCFile) {
        DrawNumberOfReconstructableLambdaHistograms("Number of reconstructable lambda determined by the geometry and overall number of lambda histograms");
        DrawGeomEfficiencyHistograms("Geometrical lambda reconstruction efficiency histograms");
        DrawTwoDimensionalRecGeomEfficiencyHistograms("Two dimensional number of lambda histograms");
        DrawNumberOfLambdaDecayProtonsHistograms("Number of reconstructable lambda decay protons determined by the geometry and overall number of protons histograms");
        DrawRecProtonsGeomEffHistograms("Lambda decay protons geometrical reconstruction efficiency");
        DrawTwoDimensionalRecProtonsRecEfficiencyHistograms("Two dimensional number of recontstructive protons determined by the geometry histograms");
        DrawNumberOfLambdaDecayMesonsHistograms("Number of reconstructable lambda decay mesons determined by the geometry  and overall number of mesons histograms");
        DrawRecMesonsGeomRecEffHistograms("Lambda decay mesons geometrical reconstruction efficiency");
        DrawTwoDimensionalRecMesonsGeomRecEfficiencyHistograms("Two dimensional number of lambda decay mesons determined by the geometry histograms");

        if (fMC) {
            DrawReconstructedFromMCLambdasHistograms("Reconstructed from MC particle pairs");
            DrawTwoDimensinalReconstructedFromMCLambdasHistograms("Two dimensional reconstructed particle pairs from MC data");
            DrawReconstructedFromMCWOCutsLambdasHistograms("Reconstructed from MC particle pairs without cuts");
            DrawTwoDimensinalReconstructedFromMCWOCutsLambdasHistograms("Two dimensional reconstructed particle pairs from MC data without cuts");
        }
    }

    if (fUseMCFile && fUseRecoFile) {
        DrawNumberOfNotReconstructedLambdasHistograms("Number of not reconstructed lambda histograms");

        if (fRECO) {
            DrawTwoDimensionalRealRecEfficiencyHistograms("Two dimensional number of reconstructed particle pairs histograms");
            DrawReconstructionEfficiencyHistograms("Particle pairs reconstruction efficiency");
            DrawNumberOfLambdaDecayProtonsReconstructedHistograms("Number of reconstructed lambda decay protons histograms");
            DrawRecProtonsReconstructionEfficiencyHistograms("Lambda decay protons reconstruction efficiency");
            DrawTwoDimensionalRealRecEfficiencyRecProtonsHistograms("Two dimensional number of reconstructed lambda decay protons histograms");
            DrawNumberOfLambdaDecayMesonsReconstructedHistograms("Number of reconstructed lambda decay mesons histograms");
            DrawRecMesonsReconstructionEfficiencyHistograms("Reconstructive mesons reconstruction efficiency");
            DrawTwoDimensionalRealRecEfficiencyRecMesonsHistograms("Two dimensional number of reconstructed lambda decay mesons histograms");
            DrawNumberOfReconstructedLambdaHistograms("Number of reconstructed particle pairs histograms");
        }
    }

    if (fUseRecoFile) {
        if (fRECO) {
            DrawReconstructedLambdasHistograms("Reconstructed particle pairs from reconstruction data");
            DrawTwoDimensinalReconstructedLambdasHistograms("Two dimensional reconstructed particle pairs data");
            DrawReconstructedLambdasWOCutsHistograms("Reconstructed particle pairs without cuts from reconstruction data");
            DrawTwoDimensinalReconstructedLambdasWOCutsHistograms("Two dimensional reconstructed without cuts particle pairs data");
        }
    }
}

void BmnLambdaQaReport::DrawNumberOfReconstructableLambdaHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 1000);
    canvas->Divide(4, 2);
    canvas->SetGrid();

    TString momRecN = Form("simNReconstructableLambda_P_sim");
    TString etaRecN = Form("simNReconstructableLambda_eta_sim");
    TString thetaRecN = Form("simNReconstructableLambda_theta_sim");
    TString thetaRRecN = Form("simNReconstructableLambda_theta_r_sim");
    TString momXRecN = Form("simNReconstructableLambda_P_x_sim");
    TString momYRecN = Form("simNReconstructableLambda_P_y_sim");
    TString momZRecN = Form("simNReconstructableLambda_P_z_sim");
    TString momN = Form("simNLambda_P_sim");
    TString etaN = Form("simNLambda_eta_sim");
    TString thetaN = Form("simNLambda_theta_sim");
    TString thetaRN = Form("simNLambda_theta_r_sim");
    TString momXN = Form("simNLambda_P_x_sim");
    TString momYN = Form("simNLambda_P_y_sim");
    TString momZN = Form("simNLambda_P_z_sim");

    canvas->cd(1);
    vector<TH1*> histos1;
    histos1.push_back(HM()->H1(momRecN.Data()));
    histos1.push_back(HM()->H1(momN.Data()));

    vector<string> labels1;
    labels1.push_back("Reconstructable lambda");
    labels1.push_back("All #Lambda^{0}");

    DrawH1(histos1, labels1, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(2);
    vector<TH1*> histos2;
    histos2.push_back(HM()->H1(etaRecN.Data()));
    histos2.push_back(HM()->H1(etaN.Data()));

    vector<string> labels2;
    labels2.push_back("Reconstructable #Lambda^{0}");
    labels2.push_back("All #Lambda^{0}");

    DrawH1(histos2, labels2, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(3);
    vector<TH1*> histos3;
    histos3.push_back(HM()->H1(thetaRecN.Data()));
    histos3.push_back(HM()->H1(thetaN.Data()));

    vector<string> labels3;
    labels3.push_back("Reconstructable #Lambda^{0}");
    labels3.push_back("All #Lambda^{0}");

    DrawH1(histos3, labels3, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(4);
    vector<TH1*> histos4;
    histos4.push_back(HM()->H1(thetaRRecN.Data()));
    histos4.push_back(HM()->H1(thetaRN.Data()));

    vector<string> labels4;
    labels4.push_back("Reconstructable #Lambda^{0}");
    labels4.push_back("All #Lambda^{0}");

    DrawH1(histos4, labels4, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(5);
    vector<TH1*> histos5;
    histos5.push_back(HM()->H1(momXRecN.Data()));
    histos5.push_back(HM()->H1(momXN.Data()));

    vector<string> labels5;
    labels5.push_back("Reconstructable #Lambda^{0}");
    labels5.push_back("All #Lambda^{0}");
    DrawH1(histos5, labels5, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(6);
    vector<TH1*> histos6;
    histos6.push_back(HM()->H1(momYRecN.Data()));
    histos6.push_back(HM()->H1(momYN.Data()));

    vector<string> labels6;
    labels6.push_back("Reconstructable #Lambda^{0}");
    labels6.push_back("All #Lambda^{0}");
    DrawH1(histos6, labels6, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(7);
    vector<TH1*> histos7;
    histos7.push_back(HM()->H1(momZRecN.Data()));
    histos7.push_back(HM()->H1(momZN.Data()));

    vector<string> labels7;
    labels7.push_back("Reconstructable #Lambda^{0}");
    labels7.push_back("All #Lambda^{0}");
    DrawH1(histos7, labels7, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);


}

void BmnLambdaQaReport::DrawNumberOfLambdaHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1500, 1000);

    canvas->Divide(3, 2);
    canvas->SetGrid();

    TString momN = Form("simNLambda_P_sim");
    TString etaN = Form("simNLambda_eta_sim");
    TString thetaN = Form("simNLambda_theta_sim");

    TString momXN = Form("simNLambda_P_x_sim");
    TString momYN = Form("simNLambda_P_y_sim");
    TString momZN = Form("simNLambda_P_z_sim");

    canvas->cd(1);
    DrawH1(HM()->H1(momN.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen);

    canvas->cd(2);
    DrawH1(HM()->H1(etaN.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen);

    canvas->cd(3);
    DrawH1(HM()->H1(thetaN.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen);

    canvas->cd(4);
    DrawH1(HM()->H1(momXN.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen);

    canvas->cd(5);
    DrawH1(HM()->H1(momYN.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen);

    canvas->cd(6);
    DrawH1(HM()->H1(momZN.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen);
}

void BmnLambdaQaReport::DrawGeomEfficiencyHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 1000);

    canvas->Divide(4, 2);
    canvas->SetGrid();

    TString momEff = Form("simLambdaRecEff_P_sim");
    TString etaEff = Form("simLambdaRecEff_eta_sim");
    TString thetaEff = Form("simLambdaRecEff_theta_sim");
    TString thetaREff = Form("simLambdaRecEff_theta_r_sim");
    TString momXEff = Form("simLambdaRecEff_P_x_sim");
    TString momYEff = Form("simLambdaRecEff_P_y_sim");
    TString momZEff = Form("simLambdaRecEff_P_z_sim");

    canvas->cd(1);
    HM()->H1(momEff)->Divide(HM()->H1("simNReconstructableLambda_P_sim"), HM()->H1("simNLambda_P_sim"), 100., 1., "B");
    HM()->H1(momEff)->SetMaximum(100.0);
    HM()->H1(momEff)->SetMinimum(0.0);
    DrawH1(HM()->H1(momEff.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(2);
    HM()->H1(etaEff)->Divide(HM()->H1("simNReconstructableLambda_eta_sim"), HM()->H1("simNLambda_eta_sim"), 100., 1., "B");
    HM()->H1(etaEff)->SetMaximum(100.0);
    HM()->H1(etaEff)->SetMinimum(0.0);
    DrawH1(HM()->H1(etaEff.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(3);
    HM()->H1(thetaEff)->Divide(HM()->H1("simNReconstructableLambda_theta_sim"), HM()->H1("simNLambda_theta_sim"), 100., 1., "B");
    HM()->H1(thetaEff)->SetMaximum(100.0);
    HM()->H1(thetaEff)->SetMinimum(0.0);
    DrawH1(HM()->H1(thetaEff.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(4);
    HM()->H1(thetaREff)->Divide(HM()->H1("simNReconstructableLambda_theta_r_sim"), HM()->H1("simNLambda_theta_r_sim"), 100., 1., "B");
    HM()->H1(thetaREff)->SetMaximum(100.0);
    HM()->H1(thetaREff)->SetMinimum(0.0);
    DrawH1(HM()->H1(thetaREff.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(5);
    HM()->H1(momXEff)->Divide(HM()->H1("simNReconstructableLambda_P_x_sim"), HM()->H1("simNLambda_P_x_sim"), 100., 1., "B");
    HM()->H1(momXEff)->SetMaximum(100.0);
    HM()->H1(momXEff)->SetMinimum(0.0);
    DrawH1(HM()->H1(momXEff.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(6);
    HM()->H1(momYEff)->Divide(HM()->H1("simNReconstructableLambda_P_y_sim"), HM()->H1("simNLambda_P_y_sim"), 100., 1., "B");
    HM()->H1(momYEff)->SetMaximum(100.0);
    HM()->H1(momYEff)->SetMinimum(0.0);
    DrawH1(HM()->H1(momYEff.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(7);
    HM()->H1(momZEff)->Divide(HM()->H1("simNReconstructableLambda_P_z_sim"), HM()->H1("simNLambda_P_z_sim"), 100., 1., "B");
    HM()->H1(momZEff)->SetMaximum(100.0);
    HM()->H1(momZEff)->SetMinimum(0.0);
    DrawH1(HM()->H1(momZEff.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);
}

void BmnLambdaQaReport::DrawTwoDimensionalRecGeomEfficiencyHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1500, 500);
    TString momEtaN = Form("simNLambda_eta_P_sim");
    TString momEtaRecN = Form("simNReconstructableLambda_eta_P_sim");
    TString momEtaEff = Form("simEffRecLambda_eta_P_sim");

    canvas->Divide(3, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(momEtaN.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    DrawH2(HM()->H2(momEtaRecN.Data()), kLinear, kLinear, kLinear);

    canvas->cd(3);
    HM()->H2(momEtaEff.Data())->Divide(HM()->H2(momEtaRecN.Data()), HM()->H2(momEtaN.Data()), 100., 1., "B");
    DrawH2(HM()->H2(momEtaEff.Data()), kLinear, kLinear, kLinear);
}

void BmnLambdaQaReport::DrawNumberOfLambdaDecayProtonsHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString nProtonsMom = Form("NProtons_P_sim");
    TString nProtonsEta = Form("NProtons_eta_sim");
    TString nProtonsTheta = Form("NProtons_theta_sim");
    TString nProtonsThetaR = Form("NProtons_theta_r_sim");

    TString nRecProtonsMom = Form("NRecProtons_P_sim");
    TString nRecProtonsEta = Form("NRecProtons_eta_sim");
    TString nRecProtonsTheta = Form("NRecProtons_theta_sim");
    TString nRecProtonsThetaR = Form("NRecProtons_theta_r_sim");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);

    vector<TH1*> histos1;
    histos1.push_back(HM()->H1(nRecProtonsMom.Data()));
    histos1.push_back(HM()->H1(nProtonsMom.Data()));

    vector<string> labels1;
    labels1.push_back("Reconstructable lambda decay p");
    labels1.push_back("Lambda decay p");

    DrawH1(histos1, labels1, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(2);

    vector<TH1*> histos2;
    histos2.push_back(HM()->H1(nRecProtonsEta.Data()));
    histos2.push_back(HM()->H1(nProtonsEta.Data()));

    vector<string> labels2;
    labels2.push_back("Reconstructable lambda decay p");
    labels2.push_back("Lambda decay p");

    DrawH1(histos2, labels2, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(3);

    vector<TH1*> histos3;
    histos3.push_back(HM()->H1(nRecProtonsTheta.Data()));
    histos3.push_back(HM()->H1(nProtonsTheta.Data()));

    vector<string> labels3;
    labels3.push_back("Reconstructable lambda decay p");
    labels3.push_back("Lambda decay p");

    DrawH1(histos3, labels3, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(4);

    vector<TH1*> histos4;
    histos4.push_back(HM()->H1(nRecProtonsThetaR.Data()));
    histos4.push_back(HM()->H1(nProtonsThetaR.Data()));

    vector<string> labels4;
    labels4.push_back("Reconstructable lambda decay p");
    labels4.push_back("Lambda decay p");

    DrawH1(histos4, labels4, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);
}

void BmnLambdaQaReport::DrawRecProtonsGeomEffHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString recProtonsRecEffMom = Form("simRecProtonsRecEff_P_sim");
    TString recProtonsRecEffEta = Form("simRecProtonsRecEff_eta_sim");
    TString recProtonsRecEffTheta = Form("simRecProtonsRecEff_theta_sim");
    TString recProtonsRecEffThetaR = Form("simRecProtonsRecEff_theta_r_sim");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    HM()->H1(recProtonsRecEffMom.Data())->Divide(HM()->H1("NRecProtons_P_sim"), HM()->H1("NProtons_P_sim"), 100., 1., "B");
    DrawH1(HM()->H1(recProtonsRecEffMom.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(2);
    HM()->H1(recProtonsRecEffEta.Data())->Divide(HM()->H1("NRecProtons_eta_sim"), HM()->H1("NProtons_eta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(recProtonsRecEffEta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(3);
    HM()->H1(recProtonsRecEffTheta.Data())->Divide(HM()->H1("NRecProtons_theta_sim"), HM()->H1("NProtons_theta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(recProtonsRecEffTheta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(4);
    HM()->H1(recProtonsRecEffThetaR.Data())->Divide(HM()->H1("NRecProtons_theta_r_sim"), HM()->H1("NProtons_theta_r_sim"), 100., 1., "B");
    DrawH1(HM()->H1(recProtonsRecEffThetaR.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

}

void BmnLambdaQaReport::DrawNumberOfLambdaDecayMesonsHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString nMesonsMom = Form("NMesons_P_sim");
    TString nMesonsEta = Form("NMesons_eta_sim");
    TString nMesonsTheta = Form("NMesons_theta_sim");
    TString nMesonsThetaR = Form("NMesons_theta_r_sim");

    TString nRecMesonsMom = Form("NRecMesons_P_sim");
    TString nRecMesonsEta = Form("NRecMesons_eta_sim");
    TString nRecMesonsTheta = Form("NRecMesons_theta_sim");
    TString nRecMesonsThetaR = Form("NRecMesons_theta_r_sim");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);

    vector<TH1*> histos1;
    histos1.push_back(HM()->H1(nRecMesonsMom.Data()));
    histos1.push_back(HM()->H1(nMesonsMom.Data()));

    vector<string> labels1;
    labels1.push_back("Reconstructable lambda decay #pi^{-}");
    labels1.push_back("Lambda decay #pi^{-}");

    DrawH1(histos1, labels1, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(2);

    vector<TH1*> histos2;
    histos2.push_back(HM()->H1(nRecMesonsEta.Data()));
    histos2.push_back(HM()->H1(nMesonsEta.Data()));

    vector<string> labels2;
    labels2.push_back("Reconstructable lambda decay #pi^{-}");
    labels2.push_back("Lambda decay #pi^{-}");

    DrawH1(histos2, labels2, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(3);

    vector<TH1*> histos3;
    histos3.push_back(HM()->H1(nRecMesonsTheta.Data()));
    histos3.push_back(HM()->H1(nMesonsTheta.Data()));

    vector<string> labels3;
    labels3.push_back("Reconstructable lambda decay #pi^{-}");
    labels3.push_back("Lambda decay #pi^{-}");

    DrawH1(histos3, labels3, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);

    canvas->cd(4);

    vector<TH1*> histos4;
    histos4.push_back(HM()->H1(nRecMesonsThetaR.Data()));
    histos4.push_back(HM()->H1(nMesonsThetaR.Data()));

    vector<string> labels4;
    labels4.push_back("Reconstructable lambda decay #pi^{-}");
    labels4.push_back("Lambda decay #pi^{-}");

    DrawH1(histos4, labels4, kLinear, kLog, kTRUE, 0.16, 0.91, 0.9, 0.99, drawPointsOpt.Data(), kFALSE);
}

void BmnLambdaQaReport::DrawRecMesonsGeomRecEffHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString nMesonsMom = Form("NMesons_P_sim");
    TString nMesonsEta = Form("NMesons_eta_sim");
    TString nMesonsTheta = Form("NMesons_theta_sim");

    TString nRecMesonsMom = Form("NRecMesons_P_sim");
    TString nRecMesonsEta = Form("NRecMesons_eta_sim");
    TString nRecMesonsTheta = Form("NRecMesons_theta_sim");

    TString recMesonsRecEffMom = Form("simRecMesonsRecEff_P_sim");
    TString recMesonsRecEffEta = Form("simRecMesonsRecEff_eta_sim");
    TString recMesonsRecEffTheta = Form("simRecMesonsRecEff_theta_sim");
    TString recMesonsRecEffThetaR = Form("simRecMesonsRecEff_theta_r_sim");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    HM()->H1(recMesonsRecEffMom.Data())->Divide(HM()->H1("NRecMesons_P_sim"), HM()->H1("NMesons_P_sim"), 100., 1., "B");
    DrawH1(HM()->H1(recMesonsRecEffMom.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(2);
    HM()->H1(recMesonsRecEffEta.Data())->Divide(HM()->H1("NRecMesons_eta_sim"), HM()->H1("NMesons_eta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(recMesonsRecEffEta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(3);
    HM()->H1(recMesonsRecEffTheta.Data())->Divide(HM()->H1("NRecMesons_theta_sim"), HM()->H1("NMesons_theta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(recMesonsRecEffTheta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);

    canvas->cd(4);
    HM()->H1(recMesonsRecEffThetaR.Data())->Divide(HM()->H1("NRecMesons_theta_r_sim"), HM()->H1("NMesons_theta_r_sim"), 100., 1., "B");
    DrawH1(HM()->H1(recMesonsRecEffThetaR.Data()), kLinear, kLinear, drawPointsOpt.Data(), kGreen + 2);
}

void BmnLambdaQaReport::DrawTwoDimensionalRecProtonsRecEfficiencyHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1500, 500);

    TString momEtaNProtons = Form("simNRecProtons_eta_P_sim");
    TString momEtaRecNProtons = Form("simNReconstructableRecProtons_eta_P_sim");
    TString momEtaEffProtons = Form("simEffRecRecProtons_eta_P_sim");

    canvas->Divide(3, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(momEtaNProtons.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    DrawH2(HM()->H2(momEtaRecNProtons.Data()), kLinear, kLinear, kLinear);

    canvas->cd(3);
    HM()->H2(momEtaEffProtons.Data())->Divide(HM()->H2(momEtaRecNProtons.Data()), HM()->H2(momEtaNProtons.Data()), 100., 1., "B");
    DrawH2(HM()->H2(momEtaEffProtons.Data()), kLinear, kLinear, kLinear);
}

void BmnLambdaQaReport::DrawTwoDimensionalRecMesonsGeomRecEfficiencyHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString momEtaNMesons = Form("simNRecMesons_eta_P_sim");
    TString momEtaRecNMesons = Form("simNReconstructableRecMesons_eta_P_sim");
    TString momEtaEffMesons = Form("simEffRecRecMesons_eta_P_sim");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(momEtaNMesons.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    DrawH2(HM()->H2(momEtaRecNMesons.Data()), kLinear, kLinear, kLinear);

    canvas->cd(3);
    HM()->H2(momEtaEffMesons.Data())->Divide(HM()->H2(momEtaRecNMesons.Data()), HM()->H2(momEtaNMesons.Data()), 100., 1., "B");
    DrawH2(HM()->H2(momEtaEffMesons.Data()), kLinear, kLinear, kLinear);
}

void BmnLambdaQaReport::DrawNumberOfReconstructedLambdaHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString nLambdasReconstructedP = Form("recNLambda_P_rec");
    TString nLambdasReconstructedTheta = Form("recNLambda_theta_rec");
    TString nLambdasReconstructedEta = Form("recNLambda_eta_rec");
    TString nLambdasReconstructedThetaR = Form("recNLambda_theta_r_rec");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH1(HM()->H1(nLambdasReconstructedP.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);

    canvas->cd(2);
    DrawH1(HM()->H1(nLambdasReconstructedEta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);

    canvas->cd(3);
    DrawH1(HM()->H1(nLambdasReconstructedTheta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);

    canvas->cd(4);
    DrawH1(HM()->H1(nLambdasReconstructedThetaR.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);
}

void BmnLambdaQaReport::DrawReconstructionEfficiencyHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString effLambdasReconstructedP = Form("recEffLambda_P_rec");
    TString effLambdasReconstructedTheta = Form("recEffLambda_theta_rec");
    TString effLambdasReconstructedEta = Form("recEffLambda_eta_rec");
    TString effLambdasReconstructedThetaR = Form("recEffLambda_theta_r_rec");

    TString nLambdasReconstructedP = Form("recNLambda_P_rec");
    TString nLambdasReconstructedTheta = Form("recNLambda_theta_rec");
    TString nLambdasReconstructedEta = Form("recNLambda_eta_rec");
    TString nLambdasReconstructedThetaR = Form("recNLambda_theta_r_rec");

    TString momRecN = Form("simNReconstructableLambda_P_sim");
    TString etaRecN = Form("simNReconstructableLambda_eta_sim");
    TString thetaRecN = Form("simNReconstructableLambda_theta_sim");
    TString thetaRRecN = Form("simNReconstructableLambda_theta_r_sim");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    HM()->H1(effLambdasReconstructedP)->Divide(HM()->H1("recNLambda_P_rec"), HM()->H1("simNReconstructableLambda_P_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effLambdasReconstructedP.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);

    canvas->cd(2);
    HM()->H1(effLambdasReconstructedEta)->Divide(HM()->H1("recNLambda_eta_rec"), HM()->H1("simNReconstructableLambda_eta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effLambdasReconstructedEta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);

    canvas->cd(3);
    HM()->H1(effLambdasReconstructedTheta)->Divide(HM()->H1("recNLambda_theta_rec"), HM()->H1("simNReconstructableLambda_theta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effLambdasReconstructedTheta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);

    canvas->cd(4);
    HM()->H1(effLambdasReconstructedThetaR)->Divide(HM()->H1("recNLambda_theta_r_rec"), HM()->H1("simNReconstructableLambda_theta_r_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effLambdasReconstructedThetaR.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);
}

void BmnLambdaQaReport::DrawTwoDimensionalRealRecEfficiencyHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString momEtaN = Form("recNLambda_eta_P_sim");
    TString momEtaRecEff = Form("recEffRecLambda_eta_P_sim");
    TString momEtaRecN = Form("simNReconstructableLambda_eta_P_sim");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(momEtaN.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    HM()->H2(momEtaRecEff.Data())->Divide(HM()->H2(momEtaN.Data()), HM()->H2(momEtaRecN.Data()), 100., 1., "B");
    DrawH2(HM()->H2(momEtaRecEff.Data()), kLinear, kLinear, kLinear);

}

void BmnLambdaQaReport::DrawNumberOfLambdaDecayProtonsReconstructedHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString nRecProtonsReconstructedP = Form("recNRecProtons_P_rec");
    TString nRecProtonsReconstructedTheta = Form("recNRecProtons_theta_rec");
    TString nRecProtonsReconstructedEta = Form("recNRecProtons_eta_rec");
    TString nRecProtonsReconstructedThetaR = Form("recNRecProtons_theta_r_rec");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH1(HM()->H1(nRecProtonsReconstructedP.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);

    canvas->cd(2);
    DrawH1(HM()->H1(nRecProtonsReconstructedEta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);

    canvas->cd(3);
    DrawH1(HM()->H1(nRecProtonsReconstructedTheta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);

    canvas->cd(4);
    DrawH1(HM()->H1(nRecProtonsReconstructedThetaR.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);
}

void BmnLambdaQaReport::DrawRecProtonsReconstructionEfficiencyHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString effRecProtonsReconstructedP = Form("recEffRecProtons_P_rec");
    TString effRecProtonsReconstructedTheta = Form("recEffRecProtons_theta_rec");
    TString effRecProtonsReconstructedEta = Form("recEffRecProtons_eta_rec");
    TString effRecProtonsReconstructedThetaR = Form("recEffRecProtons_theta_r_rec");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    HM()->H1(effRecProtonsReconstructedP)->Divide(HM()->H1("recNRecProtons_P_rec"), HM()->H1("NRecProtons_P_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effRecProtonsReconstructedP.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);

    canvas->cd(2);
    HM()->H1(effRecProtonsReconstructedEta)->Divide(HM()->H1("recNRecProtons_eta_rec"), HM()->H1("NRecProtons_eta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effRecProtonsReconstructedEta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);

    canvas->cd(3);
    HM()->H1(effRecProtonsReconstructedTheta)->Divide(HM()->H1("recNRecProtons_theta_rec"), HM()->H1("NRecProtons_theta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effRecProtonsReconstructedTheta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);

    canvas->cd(4);
    HM()->H1(effRecProtonsReconstructedThetaR)->Divide(HM()->H1("recNRecProtons_theta_r_rec"), HM()->H1("NRecProtons_theta_r_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effRecProtonsReconstructedThetaR.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);
}

void BmnLambdaQaReport::DrawTwoDimensionalRealRecEfficiencyRecProtonsHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString momEtaNProt = Form("recNRecProtons_eta_P_sim");
    TString momEtaRecEff = Form("recEffRecProtons_eta_P_sim");
    TString momEtaRecN = Form("simNReconstructableRecProtons_eta_P_sim");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(momEtaNProt.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    HM()->H2(momEtaRecEff.Data())->Divide(HM()->H2(momEtaNProt.Data()), HM()->H2(momEtaRecN.Data()), 100., 1., "B");
    DrawH2(HM()->H2(momEtaRecEff.Data()), kLinear, kLinear, kLinear);

}

void BmnLambdaQaReport::DrawNumberOfLambdaDecayMesonsReconstructedHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString nRecMesonsReconstructedP = Form("recNRecMesons_P_rec");
    TString nRecMesonsReconstructedTheta = Form("recNRecMesons_theta_rec");
    TString nRecMesonsReconstructedEta = Form("recNRecMesons_eta_rec");
    TString nRecMesonsReconstructedThetaR = Form("recNRecMesons_theta_r_rec");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH1(HM()->H1(nRecMesonsReconstructedP.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);

    canvas->cd(2);
    DrawH1(HM()->H1(nRecMesonsReconstructedEta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);

    canvas->cd(3);
    DrawH1(HM()->H1(nRecMesonsReconstructedTheta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);

    canvas->cd(4);
    DrawH1(HM()->H1(nRecMesonsReconstructedThetaR.Data()), kLinear, kLinear, drawPointsOpt.Data(), kMagenta);
}

void BmnLambdaQaReport::DrawRecMesonsReconstructionEfficiencyHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString effRecMesonsReconstructedP = Form("recEffRecMesons_P_rec");
    TString effRecMesonsReconstructedTheta = Form("recEffRecMesons_theta_rec");
    TString effRecMesonsReconstructedEta = Form("recEffRecMesons_eta_rec");
    TString effRecMesonsReconstructedThetaR = Form("recEffRecMesons_theta_r_rec");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    HM()->H1(effRecMesonsReconstructedP)->Divide(HM()->H1("recNRecMesons_P_rec"), HM()->H1("NRecMesons_P_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effRecMesonsReconstructedP.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);

    canvas->cd(2);
    HM()->H1(effRecMesonsReconstructedEta)->Divide(HM()->H1("recNRecMesons_eta_rec"), HM()->H1("NRecMesons_eta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effRecMesonsReconstructedEta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);

    canvas->cd(3);
    HM()->H1(effRecMesonsReconstructedTheta)->Divide(HM()->H1("recNRecMesons_theta_rec"), HM()->H1("NRecMesons_theta_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effRecMesonsReconstructedTheta.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);

    canvas->cd(4);
    HM()->H1(effRecMesonsReconstructedThetaR)->Divide(HM()->H1("recNRecMesons_theta_r_rec"), HM()->H1("NRecMesons_theta_r_sim"), 100., 1., "B");
    DrawH1(HM()->H1(effRecMesonsReconstructedThetaR.Data()), kLinear, kLinear, drawPointsOpt.Data(), kCyan);
}

void BmnLambdaQaReport::DrawTwoDimensionalRealRecEfficiencyRecMesonsHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString momEtaNProt = Form("recNRecMesons_eta_P_sim");
    TString momEtaRecEff = Form("recEffRecMesons_eta_P_sim");
    TString momEtaRecN = Form("simNReconstructableRecMesons_eta_P_sim");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(momEtaNProt.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    HM()->H2(momEtaRecEff.Data())->Divide(HM()->H2(momEtaNProt.Data()), HM()->H2(momEtaRecN.Data()), 100., 1., "B");
    DrawH2(HM()->H2(momEtaRecEff.Data()), kLinear, kLinear, kLinear);
}

void BmnLambdaQaReport::DrawNumberOfNotReconstructedLambdasHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TString momNonRecLambda = Form("recNNotRecLambda_P_rec");
    TString thetaNonRecLambda = Form("recNNotRecLambda_theta_rec");
    TString etaNonRecLambda = Form("recNNotRecLambda_eta_rec");
    TString thetaRNonRecLambda = Form("recNNotRecLambda_theta_r_rec");

    TString momN = Form("simNLambda_P_sim");
    TString etaN = Form("simNLambda_eta_sim");
    TString thetaN = Form("simNLambda_theta_sim");
    TString thetaRN = Form("simNLambda_theta_r_sim");

    TString nLambdasReconstructedP = Form("recNLambda_P_rec");
    TString nLambdasReconstructedTheta = Form("recNLambda_theta_rec");
    TString nLambdasReconstructedEta = Form("recNLambda_eta_rec");
    TString nLambdasReconstructedThetaR = Form("recNLambda_theta_r_rec");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    HM()->H1(momNonRecLambda.Data())->Add(HM()->H1(momN.Data()), 1);
    HM()->H1(momNonRecLambda.Data())->Add(HM()->H1(nLambdasReconstructedP.Data()), -1);
    DrawH1(HM()->H1(momNonRecLambda.Data()), kLinear, kLinear, drawPointsOpt.Data(), kRed);

    canvas->cd(2);
    HM()->H1(thetaNonRecLambda.Data())->Add(HM()->H1(thetaN.Data()), 1);
    HM()->H1(thetaNonRecLambda.Data())->Add(HM()->H1(nLambdasReconstructedTheta.Data()), -1);
    DrawH1(HM()->H1(thetaNonRecLambda.Data()), kLinear, kLinear, drawPointsOpt.Data(), kRed);

    canvas->cd(3);
    HM()->H1(etaNonRecLambda.Data())->Add(HM()->H1(etaN.Data()), 1);
    HM()->H1(etaNonRecLambda.Data())->Add(HM()->H1(nLambdasReconstructedEta.Data()), -1);
    DrawH1(HM()->H1(etaNonRecLambda.Data()), kLinear, kLinear, drawPointsOpt.Data(), kRed);

    canvas->cd(4);
    HM()->H1(thetaRNonRecLambda.Data())->Add(HM()->H1(thetaRN.Data()), 1);
    HM()->H1(thetaRNonRecLambda.Data())->Add(HM()->H1(nLambdasReconstructedThetaR.Data()), -1);
    DrawH1(HM()->H1(thetaRNonRecLambda.Data()), kLinear, kLinear, drawPointsOpt.Data(), kRed);
}

void BmnLambdaQaReport::DrawReconstructedFromMCLambdasHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);
    TString nPairsRecoFromMCInvMass = Form("NPairsRecoFromMCInvMass");
    canvas->Divide(4, 1);
    canvas->SetGrid();
    canvas->cd(1);
    DrawH1(HM()->H1(nPairsRecoFromMCInvMass.Data()), kLinear, kLinear, drawPointsOpt.Data(), kOrange);
}

void BmnLambdaQaReport::DrawTwoDimensinalReconstructedFromMCLambdasHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 3000, 1000);
    TString nPairsRecoFromMCInvMassMomPart1 = Form("NPairsRecoFromMCInvMassMomProton");
    TString nPairsRecoFromMCInvMassEtaPart1 = Form("NPairsRecoFromMCInvMassEtaProton");
    TString nPairsRecoFromMCInvMassDCA1 = Form("NPairsRecoFromMCInvMassDCA1");
    TString nPairsRecoFromMCInvMassDCA12X = Form("NPairsRecoFromMCInvMassDCA12X");
    TString nPairsRecoFromMCInvMassMomPart2 = Form("NPairsRecoFromMCInvMassMomMeson");
    TString nPairsRecoFromMCInvMassEtaPart2 = Form("NPairsRecoFromMCInvMassEtaMeson");
    TString nPairsRecoFromMCInvMassDCA2 = Form("NPairsRecoFromMCInvMassDCA2");
    TString nPairsRecoFromMCInvMassDCA12Y = Form("NPairsRecoFromMCInvMassDCA12Y");
    TString nPairsRecoFromMCInvMassDCA12XsubY = Form("NPairsRecoFromMCInvMassDCA12XsubY");
    TString nPairsRecoFromMCInvMassPath = Form("NPairsRecoFromMCInvMassPath");

    canvas->Divide(4, 3);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassMomPart1.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassEtaPart1.Data()), kLinear, kLinear, kLinear);

    canvas->cd(3);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassDCA1.Data()), kLinear, kLinear, kLinear);

    canvas->cd(4);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassDCA12X.Data()), kLinear, kLinear, kLinear);

    canvas->cd(5);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassMomPart2.Data()), kLinear, kLinear, kLinear);

    canvas->cd(6);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassEtaPart2.Data()), kLinear, kLinear, kLinear);

    canvas->cd(7);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassDCA2.Data()), kLinear, kLinear, kLinear);

    canvas->cd(8);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassDCA12Y.Data()), kLinear, kLinear, kLinear);

    canvas->cd(9);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassDCA12XsubY.Data()), kLinear, kLinear, kLinear);

    canvas->cd(10);
    DrawH2(HM()->H2(nPairsRecoFromMCInvMassPath.Data()), kLinear, kLinear, kLinear);
}

void BmnLambdaQaReport::DrawReconstructedFromMCWOCutsLambdasHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);
    TString nPairsRecoFromMCWOCutsInvMass = Form("NPairsRecoFromMCWOCutsInvMass");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH1(HM()->H1(nPairsRecoFromMCWOCutsInvMass.Data()), kLinear, kLinear, drawPointsOpt.Data(), kOrange);
}

void BmnLambdaQaReport::DrawTwoDimensinalReconstructedFromMCWOCutsLambdasHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 3000, 1000);
    TString nPairsRecoFromMCWOCutsInvMassMomPart1 = Form("NPairsRecoFromMCWOCutsInvMassMomProton");
    TString nPairsRecoFromMCWOCutsInvMassEtaPart1 = Form("NPairsRecoFromMCWOCutsInvMassEtaProton");
    TString nPairsRecoFromMCWOCutsInvMassDCA1 = Form("NPairsRecoFromMCWOCutsInvMassDCA1");
    TString nPairsRecoFromMCWOCutsInvMassDCA12X = Form("NPairsRecoFromMCWOCutsInvMassDCA12X");

    TString nPairsRecoFromMCWOCutsInvMassMomPart2 = Form("NPairsRecoFromMCWOCutsInvMassMomMeson");
    TString nPairsRecoFromMCWOCutsInvMassEtaPart2 = Form("NPairsRecoFromMCWOCutsInvMassEtaMeson");
    TString nPairsRecoFromMCWOCutsInvMassDCA2 = Form("NPairsRecoFromMCWOCutsInvMassDCA2");
    TString nPairsRecoFromMCWOCutsInvMassDCA12Y = Form("NPairsRecoFromMCWOCutsInvMassDCA12Y");
    TString nPairsRecoFromMCWOCutsInvMassDCA12XsubY = Form("NPairsRecoFromMCWOCutsInvMassDCA12XsubY");
    TString nPairsRecoFromMCWOCutsInvMassPath = Form("NPairsRecoFromMCWOCutsInvMassPath");

    canvas->Divide(4, 3);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassMomPart1.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassEtaPart1.Data()), kLinear, kLinear, kLinear);

    canvas->cd(3);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassDCA1.Data()), kLinear, kLinear, kLinear);

    canvas->cd(4);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassDCA12X.Data()), kLinear, kLinear, kLinear);

    canvas->cd(5);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassMomPart2.Data()), kLinear, kLinear, kLinear);

    canvas->cd(6);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassEtaPart2.Data()), kLinear, kLinear, kLinear);

    canvas->cd(7);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassDCA2.Data()), kLinear, kLinear, kLinear);

    canvas->cd(8);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassDCA12Y.Data()), kLinear, kLinear, kLinear);

    canvas->cd(9);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassDCA12XsubY.Data()), kLinear, kLinear, kLinear);

    canvas->cd(10);
    DrawH2(HM()->H2(nPairsRecoFromMCWOCutsInvMassPath.Data()), kLinear, kLinear, kLinear);
}

void BmnLambdaQaReport::DrawReconstructedLambdasHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);

    TH1* hist = HM()->H1("NPairsRecoInvMass");
    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);

    Double_t par[8];
    TF1* bg = new TF1("BG", "pol4", 1.07, 1.22);
    TF1* sig = new TF1("SIG", "gaus", 1.11, 1.12);
    hist->Fit(bg, "R");
    //sig->SetParameter(1, 1.115);
    //    sig->SetParameter(2, 0.002);
    hist->Fit(sig, "R");
    bg->GetParameters(&par[0]);
    sig->GetParameters(&par[5]);
    TF1 *f = new TF1("f", "pol4(0)+gaus(5)", 1.07, 1.22);
    f->SetNpx(500);

    f->SetParameters(par);
    //    f->SetLineColor(kMagenta + 1);
    //    f->SetLineWidth(3);
    //f->SetParameter(6, 1.1152);
    f->SetParameter(7, 0.002);
    TFitResultPtr fitRes = hist->Fit(f, "RS");
    //    Double_t mean = fitRes->Parameter(6);
    //    Double_t sigma = fitRes->Parameter(7);
    //    Double_t T = hist->Integral(hist->FindBin(mean - 3 * sigma), hist->FindBin(mean + 3 * sigma));
    //    Double_t B = bg->Integral(mean - 3 * sigma, mean + 3 * sigma) / hist->GetBinWidth(1);
    //    Double_t S_to_B = T / B - 1;
    //    Double_t Signif = (T - B) / TMath::Sqrt(T);

    //    hist->SetMarkerStyle(20);
    //    hist->SetMarkerColor(kSpring - 6);
    //    hist->SetLineColor(kSpring - 6);
    //    hist->SetLineWidth(1);
    //    hist->GetXaxis()->SetTitle("M_{(p + #pi^{-})}, GeV/c^{2}");
    //    hist->GetYaxis()->SetTitle("Entries / 2 MeV/c^{2}");

    DrawH1(hist, kLinear, kLinear, drawPointsOpt.Data(), kOrange);

}

void BmnLambdaQaReport::DrawTwoDimensinalReconstructedLambdasHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 3000, 1000);

    TString nPairsRecoInvMassMomPart1 = Form("NPairsRecoInvMassMomProton");
    TString nPairsRecoInvMassEtaPart1 = Form("NPairsRecoInvMassEtaProton");
    TString nPairsRecoInvMassDCA1 = Form("NPairsRecoInvMassDCA1");
    TString nPairsRecoInvMassDCA12X = Form("NPairsRecoInvMassDCA12X");

    TString nPairsRecoInvMassMomPart2 = Form("NPairsRecoInvMassMomMeson");
    TString nPairsRecoInvMassEtaPart2 = Form("NPairsRecoInvMassEtaMeson");
    TString nPairsRecoInvMassDCA2 = Form("NPairsRecoInvMassDCA2");
    TString nPairsRecoInvMassDCA12Y = Form("NPairsRecoInvMassDCA12Y");
    TString nPairsRecoInvMassDCA12XsubY = Form("NPairsRecoInvMassDCA12XsubY");
    TString nPairsRecoInvMassPath = Form("NPairsRecoInvMassPath");

    canvas->Divide(4, 3);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(nPairsRecoInvMassMomPart1.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    DrawH2(HM()->H2(nPairsRecoInvMassEtaPart1.Data()), kLinear, kLinear, kLinear);

    canvas->cd(3);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA1.Data()), kLinear, kLinear, kLinear);

    canvas->cd(4);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA12X.Data()), kLinear, kLinear, kLinear);

    canvas->cd(5);
    DrawH2(HM()->H2(nPairsRecoInvMassMomPart2.Data()), kLinear, kLinear, kLinear);

    canvas->cd(6);
    DrawH2(HM()->H2(nPairsRecoInvMassEtaPart2.Data()), kLinear, kLinear, kLinear);

    canvas->cd(7);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA2.Data()), kLinear, kLinear, kLinear);

    canvas->cd(8);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA12Y.Data()), kLinear, kLinear, kLinear);

    canvas->cd(9);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA12XsubY.Data()), kLinear, kLinear, kLinear);

    canvas->cd(10);
    DrawH2(HM()->H2(nPairsRecoInvMassPath.Data()), kLinear, kLinear, kLinear);
}

void BmnLambdaQaReport::DrawReconstructedLambdasWOCutsHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 2000, 500);
    TString nPairsRecoInvMassWOCuts = Form("NPairsRecoInvMassWOCuts");

    canvas->Divide(4, 1);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH1(HM()->H1(nPairsRecoInvMassWOCuts.Data()), kLinear, kLinear, drawPointsOpt.Data(), kOrange);
}

void BmnLambdaQaReport::DrawTwoDimensinalReconstructedLambdasWOCutsHistograms(const string& canvasName) {
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 3000, 1000);

    TString nPairsRecoInvMassMomPart1WOCuts = Form("NPairsRecoInvMassMomProtonWOCuts");
    TString nPairsRecoInvMassEtaPart1WOCuts = Form("NPairsRecoInvMassEtaProtonWOCuts");
    TString nPairsRecoInvMassDCA1WOCuts = Form("NPairsRecoInvMassDCA1WOCuts");
    TString nPairsRecoInvMassDCA12XWOCuts = Form("NPairsRecoInvMassDCA12XWOCuts");

    TString nPairsRecoInvMassMomPart2WOCuts = Form("NPairsRecoInvMassMomMesonWOCuts");
    TString nPairsRecoInvMassEtaPart2WOCuts = Form("NPairsRecoInvMassEtaMesonWOCuts");
    TString nPairsRecoInvMassDCA2WOCuts = Form("NPairsRecoInvMassDCA2WOCuts");
    TString nPairsRecoInvMassDCA12YWOCuts = Form("NPairsRecoInvMassDCA12YWOCuts");
    TString nPairsRecoInvMassDCA12XsubYWOCuts = Form("NPairsRecoInvMassDCA12XsubYWOCuts");
    TString nPairsRecoInvMassPathWOCuts = Form("NPairsRecoInvMassPathWOCuts");

    canvas->Divide(4, 3);
    canvas->SetGrid();

    canvas->cd(1);
    DrawH2(HM()->H2(nPairsRecoInvMassMomPart1WOCuts.Data()), kLinear, kLinear, kLinear);

    canvas->cd(2);
    DrawH2(HM()->H2(nPairsRecoInvMassEtaPart1WOCuts.Data()), kLinear, kLinear, kLinear);

    canvas->cd(3);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA1WOCuts.Data()), kLinear, kLinear, kLinear);

    canvas->cd(4);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA12XWOCuts.Data()), kLinear, kLinear, kLinear);

    canvas->cd(5);
    DrawH2(HM()->H2(nPairsRecoInvMassMomPart2WOCuts.Data()), kLinear, kLinear, kLinear);

    canvas->cd(6);
    DrawH2(HM()->H2(nPairsRecoInvMassEtaPart2WOCuts.Data()), kLinear, kLinear, kLinear);

    canvas->cd(7);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA2WOCuts.Data()), kLinear, kLinear, kLinear);

    canvas->cd(8);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA12YWOCuts.Data()), kLinear, kLinear, kLinear);

    canvas->cd(9);
    DrawH2(HM()->H2(nPairsRecoInvMassDCA12XsubYWOCuts.Data()), kLinear, kLinear, kLinear);

    canvas->cd(10);
    DrawH2(HM()->H2(nPairsRecoInvMassPathWOCuts.Data()), kLinear, kLinear, kLinear);
}

ClassImp(BmnLambdaQaReport)
