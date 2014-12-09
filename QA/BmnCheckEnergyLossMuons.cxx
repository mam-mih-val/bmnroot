#include "qa/material/CbmLitCheckEnergyLossMuons.h"

#include "data/CbmLitTrackParam.h"
#include "propagation/CbmLitMaterialInfo.h"
#include "propagation/CbmLitMaterialEffectsImp.h"
#include "utils/CbmLitUtils.h"
#include "CbmDrawHist.h"

#include "TCanvas.h"

#include <boost/assign/list_of.hpp>

#include <cstdlib>

using boost::assign::list_of;

CbmLitCheckEnergyLossMuons::CbmLitCheckEnergyLossMuons():
   fMat("iron"),
   fOutputDir("./test/")
{
   fMom[0] = 47.04;
   fMom[1] = 56.16;
   fMom[2] = 68.02;
   fMom[3] = 85.09;
   fMom[4] = 100.3;
   fMom[5] = 152.7;
   fMom[6] = 176.4;
   fMom[7] = 221.8;
   fMom[8] = 286.8;
   fMom[9] = 364.2;
   fMom[10] = 391.7;
   fMom[11] = 494.5;
   fMom[12] = 899.5;
   fMom[13] = 1101.;
   fMom[14] = 1502.;
   fMom[15] = 2103.;
   fMom[16] = 3104.;
   fMom[17] = 4104.;
   fMom[18] = 8105.;
   fMom[19] = 10110.;
   fMom[20] = 14110.;
   fMom[21] = 20110.;
   fMom[22] = 30110.;
   fMom[23] = 40110.;
   fMom[24] = 80110.;
   fMom[25] = 100100.;
}

CbmLitCheckEnergyLossMuons::~CbmLitCheckEnergyLossMuons()
{
}

void CbmLitCheckEnergyLossMuons::Check()
{
   SetDefaultDrawStyle();

   CreateGraphs();

   if (fMat == "iron") { FillTableIron(); }
   else if (fMat == "tungsten") { FillTableTungsten(); }
   else if (fMat == "carbon") { FillTableCarbon(); }

   CalcEloss();

   DrawGraphs();
}

void CbmLitCheckEnergyLossMuons::DrawGraphs()
{
   TCanvas* c1 = new TCanvas("mean_energy_loss_muons","mean_energy_loss_muons",700, 500);

   fTable[0]->GetXaxis()->SetLimits(45, 100102);
   fTable[0]->SetMinimum(0.1);
   fTable[0]->SetMaximum(10.);

   DrawGraph(list_of(fTable[0])(fTable[1])(fTable[2])(fTable[3])(fCalc[0])(fCalc[1])(fCalc[2])(fCalc[3]),
         list_of("total (table)")("ionization (table)")("bremsstrahlung (table)")("pair production (table)")
         ("total (calculation)")("ionization (calculation)")("bremsstrahlung (calculation)")("pair production (calculation)"),
         kLog, kLog, true, 0.20, 0.97, 0.9, 0.7);

   lit::SaveCanvasAsImage(c1, fOutputDir);
}

void CbmLitCheckEnergyLossMuons::CalcEloss()
{
   CbmLitMaterialInfo material;
   if (fMat == "tungsten") {
      material.SetA(183.84);
      material.SetZ(74.0);
      material.SetRho(19.3);
      material.SetRL(0.35);
   } else if (fMat == "iron") {
      material.SetA(55.85);
      material.SetZ(26.0);
      material.SetRho(7.87);
      material.SetRL(1.757622);
   } else if (fMat == "carbon") {
      material.SetA(12.0107);
      material.SetZ(6);
      material.SetRho(2.265);
      material.SetRL(18.8);
   } else { exit(0); }

   CbmLitMaterialEffectsImp me;
   CbmLitTrackParam par;
   for (Int_t i = 0; i < 26; i++) {
      par.SetQp(1./(fMom[i]*1e-3));
      fCalc[1]->SetPoint(i, fMom[i], me.BetheBloch(&par, &material) * 1e3);
      fCalc[2]->SetPoint(i, fMom[i], me.BetheHeitler(&par, &material) * 1e3);
      fCalc[3]->SetPoint(i, fMom[i], me.PairProduction(&par, &material) * 1e3);
      fCalc[0]->SetPoint(i, fMom[i], me.dEdx(&par, &material) * 1e3);
   }
}

void CbmLitCheckEnergyLossMuons::CreateGraphs()
{
   for (int i = 0; i < 4; i++) {
      fTable[i] = new TGraph();
      fTable[i]->GetXaxis()->SetTitle("Momentum [MeV/c]");
      fTable[i]->GetYaxis()->SetTitle("Energy loss [Mev*cm^2/g]");
      fCalc[i] = new TGraph();
      fCalc[i]->GetXaxis()->SetTitle("Momentum [MeV/c]");
      fCalc[i]->GetYaxis()->SetTitle("Energy loss [Mev*cm^2/g]");
   }
}

void CbmLitCheckEnergyLossMuons::FillTableIron()
{
   Double_t table_iron[26][4] = {
      //ion    //brems   //pair   //total
      {5.494,   0.000,    0.000,   5.494},
      {4.321,   0.000,    0.000,   4.321},
      {3.399,   0.000,    0.000,   3.399},
      {2.654,   0.000,    0.000,   2.654},
      {2.274,   0.000,    0.000,   2.274},
      {1.717,   0.000,    0.000,   1.717},

      {1.616,   0.000,    0.000,   1.616},
      {1.516,   0.000,    0.000,   1.516},
      {1.463,   0.000,    0.000,   1.463},
      {1.451,   0.000,    0.000,   1.451},
      {1.452,   0.000,    0.000,   1.453},
      {1.467,   0.000,    0.000,   1.467},
      {1.548,   0.000,    0.000,   1.548},

      {1.581,   0.001,    0.000,   1.582},
      {1.635,   0.001,    0.000,   1.637},
      {1.694,   0.002,    0.001,   1.697},
      {1.760,   0.003,    0.002,   1.767},
      {1.806,   0.004,    0.004,   1.816},
      {1.911,   0.010,    0.011,   1.936},

      {1.942,   0.014,    0.015,   1.975},
      {1.987,   0.021,    0.024,   2.039},
      {2.032,   0.033,    0.039,   2.113},
      {2.080,   0.054,    0.068,   2.214},
      {2.112,   0.076,    0.099,   2.303},
      {2.184,   0.171,    0.236,   2.623},
      {2.207,   0.221,    0.310,   2.777}
   };

   for (Int_t i = 0; i < 26; i++) {
      fTable[1]->SetPoint(i, fMom[i], table_iron[i][0]);
      fTable[2]->SetPoint(i, fMom[i], table_iron[i][1]);
      fTable[3]->SetPoint(i, fMom[i], table_iron[i][2]);
      fTable[0]->SetPoint(i, fMom[i], table_iron[i][3]);
   }
}

void CbmLitCheckEnergyLossMuons::FillTableTungsten()
{
   Double_t table_tungsten[26][4] = {
      //ion    //brems   //pair   //total
      {4.000,   0.000,    0.000,   4.00},
      {3.185,   0.000,    0.000,   3.185},
      {2.534,   0.000,    0.000,   2.534},
      {2.000,   0.000,    0.000,   2.000},
      {1.726,   0.000,    0.000,   1.726},
      {1.323,   0.000,    0.000,   1.323},

      {1.251,   0.000,    0.000,   1.251},
      {1.182,   0.000,    0.000,   1.182},
      {1.149,   0.000,    0.000,   1.149},
      {1.145,   0.000,    0.000,   1.145},
      {1.150,   0.000,    0.000,   1.150},
      {1.168,   0.000,    0.000,   1.168},
      {1.247,   0.001,    0.000,   1.249},

      {1.279,   0.001,    0.000,   1.281},
      {1.329,   0.002,    0.001,   1.333},
      {1.384,   0.004,    0.002,   1.391},
      {1.446,   0.006,    0.005,   1.459},
      {1.489,   0.009,    0.009,   1.509},
      {1.587,   0.023,    0.026,   1.640},

      {1.616,   0.031,    0.036,   1.687},
      {1.658,   0.047,    0.057,   1.768},
      {1.700,   0.073,    0.092,   1.874},
      {1.745,   0.120,    0.159,   2.035},
      {1.774,   0.169,    0.231,   2.190},
      {1.840,   0.383,    0.548,   2.800},
      {1.860,   0.496,    0.717,   3.110}
   };

   for (Int_t i = 0; i < 26; i++) {
      fTable[1]->SetPoint(i, fMom[i], table_tungsten[i][0]);
      fTable[2]->SetPoint(i, fMom[i], table_tungsten[i][1]);
      fTable[3]->SetPoint(i, fMom[i], table_tungsten[i][2]);
      fTable[0]->SetPoint(i, fMom[i], table_tungsten[i][3]);
   }
}

void CbmLitCheckEnergyLossMuons::FillTableCarbon()
{
   Double_t table_carbon[26][4] = {
      //ion    //brems   //pair   //total
      {7.119,   0.000,    0.000,   7.119},
      {5.551,   0.000,    0.000,   5.551},
      {4.332,   0.000,    0.000,   4.332},
      {3.356,   0.000,    0.000,   3.356},
      {2.861,   0.000,    0.000,   2.861},
      {2.127,   0.000,    0.000,   2.127},

      {1.992,   0.000,    0.000,   1.992},
      {1.854,   0.000,    0.000,   1.854},
      {1.775,   0.000,    0.000,   1.775},
      {1.745,   0.000,    0.000,   1.745},
      {1.745,   0.000,    0.000,   1.745},
      {1.751,   0.000,    0.000,   1.751},
      {1.819,   0.000,    0.000,   1.820},

      {1.850,   0.000,    0.000,   1.851},
      {1.900,   0.000,    0.000,   1.901},
      {1.955,   0.000,    0.000,   1.957},
      {2.018,   0.001,    0.001,   2.021},
      {2.062,   0.001,    0.001,   2.066},
      {2.161,   0.003,    0.003,   2.171},

      {2.191,   0.004,    0.004,   2.204},
      {2.234,   0.006,    0.007,   2.254},
      {2.278,   0.010,    0.011,   2.308},
      {2.326,   0.016,    0.019,   2.374},
      {2.359,   0.022,    0.028,   2.427},
      {2.434,   0.050,    0.068,   2.587},
      {2.458,   0.065,    0.089,   2.655}
   };

   for (Int_t i = 0; i < 26; i++) {
      fTable[1]->SetPoint(i, fMom[i], table_carbon[i][0]);
      fTable[2]->SetPoint(i, fMom[i], table_carbon[i][1]);
      fTable[3]->SetPoint(i, fMom[i], table_carbon[i][2]);
      fTable[0]->SetPoint(i, fMom[i], table_carbon[i][3]);
   }

}

ClassImp(CbmLitCheckEnergyLossMuons);
