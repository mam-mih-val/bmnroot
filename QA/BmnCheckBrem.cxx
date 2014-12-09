#include "qa/material/CbmLitCheckBrem.h"

#include "data/CbmLitTrackParam.h"
#include "propagation/CbmLitMaterialInfo.h"
#include "propagation/CbmLitMaterialEffectsImp.h"

#include "TGraph.h"
#include "TMultiGraph.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TStyle.h"

#include <cstdlib>
#include <cmath>

CbmLitCheckBrem::CbmLitCheckBrem():
   fMat("iron")
{
   fNofMom = 10;
   fMom.resize(fNofMom);
   fMom[0] = 0.05; // GeV
   fMom[1] = 0.06; // GeV
   fMom[2] = 0.08; // GeV
   fMom[3] = 0.1; // GeV
   fMom[4] = 0.2; // GeV
   fMom[5] = 0.5; // GeV
   fMom[6] = 1.; // GeV
   fMom[7] = 2.; // GeV
   fMom[8] = 5.; // GeV
   fMom[9] = 10.; // GeV

   fNofMaterials = 3;
   fTable.resize(fNofMaterials);
   fCalc.resize(fNofMaterials);
   fMaterials.resize(fNofMaterials);

   //N2 gas
   CbmLitSimpleMaterial m0;
   m0.fA = 14.00674;
   m0.fZ = 7;
   m0.fRho = 1.250;//* 1e-3; // [g/cm3]
   m0.fX0 = 47.1; // [cm]
   m0.fPHIRAD.resize(fNofMom);
   m0.fPHIRAD[0] = 17.63;
   m0.fPHIRAD[1] = 17.99;
   m0.fPHIRAD[2] = 18.51;
   m0.fPHIRAD[3] = 18.87;
   m0.fPHIRAD[4] = 19.81;
   m0.fPHIRAD[5] = 20.60;
   m0.fPHIRAD[6] = 20.96;
   m0.fPHIRAD[7] = 21.18;
   m0.fPHIRAD[8] = 21.36;
   m0.fPHIRAD[9] = 21.43;
   fMaterials[0] = m0;

   // Cu
   CbmLitSimpleMaterial m1;
   m1.fA = 63.546;
   m1.fZ = 29;
   m1.fRho = 8.96; // [g/cm3]
   m1.fX0 = 1.43; // [cm]
   m1.fPHIRAD.resize(fNofMom);
   m1.fPHIRAD[0] = 14.36;
   m1.fPHIRAD[1] = 14.60;
   m1.fPHIRAD[2] = 14.95;
   m1.fPHIRAD[3] = 15.19;
   m1.fPHIRAD[4] = 15.81;
   m1.fPHIRAD[5] = 16.30;
   m1.fPHIRAD[6] = 16.52;
   m1.fPHIRAD[7] = 16.66;
   m1.fPHIRAD[8] = 16.76;
   m1.fPHIRAD[9] = 16.80;
   fMaterials[1] = m1;

   // Sn
   CbmLitSimpleMaterial m2;
   m2.fA = 118.710;
   m2.fZ = 50;
   m2.fRho = 7.31; // [g/cm3]
   m2.fX0 = 1.21; // [cm]
   m2.fPHIRAD.resize(fNofMom);
   m2.fPHIRAD[0] = 13.31;
   m2.fPHIRAD[1] = 13.53;
   m2.fPHIRAD[2] = 13.83;
   m2.fPHIRAD[3] = 14.05;
   m2.fPHIRAD[4] = 14.58;
   m2.fPHIRAD[5] = 15.01;
   m2.fPHIRAD[6] = 15.20;
   m2.fPHIRAD[7] = 15.32;
   m2.fPHIRAD[8] = 15.41;
   m2.fPHIRAD[9] = 15.44;
   fMaterials[2] = m2;
}

CbmLitCheckBrem::~CbmLitCheckBrem()
{
}

void CbmLitCheckBrem::Check()
{
   gStyle->SetCanvasColor(kWhite);
   gStyle->SetFrameFillColor(kWhite);
   gStyle->SetPadColor(kWhite);
   gStyle->SetStatColor(kWhite);
   gStyle->SetTitleFillColor(kWhite);
   gStyle->SetPalette(1);

   CreateGraphs();
   FillGraphs();
   DrawGraphs();
}

void CbmLitCheckBrem::DrawGraphs()
{
   TCanvas* c1 = new TCanvas("brem_loss","c1",800, 800);
   c1->SetGrid();

   for (int i = 0; i < fNofMaterials; i++) {
      fTable[i]->SetLineStyle(3);
      fTable[i]->SetLineColor(2);
      fTable[i]->SetMarkerColor(2);
      fTable[i]->SetLineWidth(3);
      fTable[i]->SetMarkerSize(2);

      fCalc[i]->SetLineStyle(1);
      fCalc[i]->SetLineColor(4);
      fCalc[i]->SetMarkerColor(4);
      fCalc[i]->SetLineWidth(3);
      fCalc[i]->SetMarkerSize(2);
   }
   fTable[0]->SetMarkerStyle(20);
   fTable[1]->SetMarkerStyle(26);
   fTable[2]->SetMarkerStyle(27);
// fTable[3]->SetMarkerStyle(25);

   fCalc[0]->SetMarkerStyle(20);
   fCalc[1]->SetMarkerStyle(26);
   fCalc[2]->SetMarkerStyle(27);
// fCalc[3]->SetMarkerStyle(25);

   TMultiGraph* mg = new TMultiGraph();
   for (int i = 0; i < fNofMaterials; i++) {
      mg->Add(fTable[i]);
      mg->Add(fCalc[i]);
   }
   mg->SetMinimum(0.001);
   mg->SetMaximum(10.);
   gPad->SetLogx();
   gPad->SetLogy();

   mg->Draw("ALP");

   mg->GetXaxis()->SetTitle("Electron momentum [GeV/c]");
   mg->GetYaxis()->SetTitle("Brehmstrahlung energy loss [GeV/cm]");
   mg->GetXaxis()->SetLimits(0.04, 11);

   TLegend* l1 = new TLegend(0.20, 0.97, 0.9, 0.7);
   l1->SetFillColor(kWhite);
   l1->SetHeader("Energy losses for muons in iron vs. momentum");
   l1->AddEntry(fTable[0],"N2 (table)","lp");
   l1->AddEntry(fCalc[0],"N2 (calculation)","lp");
   l1->AddEntry(fTable[1],"Cu (table)","lp");
   l1->AddEntry(fCalc[1],"Cu (calculation)","lp");
   l1->AddEntry(fTable[2],"Sn (table)","lp");
   l1->AddEntry(fCalc[2],"Sn (calculation)","lp");
   l1->Draw();

   c1->SaveAs("brem_loss.gif");
   c1->SaveAs("brem_loss.eps");
   c1->SaveAs("brem_loss.svg");

}

void CbmLitCheckBrem::FillGraphs()
{
   for (Int_t i = 0; i < fNofMom; i++) {
      Double_t p = fMom[i];

      for (Int_t j = 0; j < fNofMaterials; j++) {
         CbmLitSimpleMaterial& m = fMaterials[j];
//       Double_t brem_calc = p / (m.fX0);
//       Double_t Z2 = (m.fZ*m.fZ);
//       Double_t X0 = 1./(m.fA * 716.408) * (Z2 * (std::log(184.15/std::pow(Z, 1./3.)) ) + Z*)
         Double_t X0 = (716.4 * m.fA) / (m.fZ * (m.fZ + 1) * std::log(287./std::sqrt(m.fZ)));
         std::cout << "X0=" << m.fX0* m.fRho << " X0calc=" << X0 << std::endl;
         Double_t brem_calc = p / X0;
         fCalc[j]->SetPoint(i, p, brem_calc);

         Double_t brem_table = (6.022045e23 / m.fA) * 5.794661e-28 * (m.fZ*m.fZ) * (p + 0.000511) * m.fPHIRAD[i];// * m.fRho;
         fTable[j]->SetPoint(i, p, brem_table);
      }
   }
}

void CbmLitCheckBrem::CreateGraphs()
{
   for (int i = 0; i < fNofMaterials; i++) {
      fTable[i] = new TGraph();
      fCalc[i] = new TGraph();
   }
}

ClassImp(CbmLitCheckBrem)
