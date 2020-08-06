/**
 * \file CbmLitFieldApproximationQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "CbmLitFieldApproximationQa.h"
#include "CbmLitFieldApproximationQaReport.h"
#include "base/CbmLitFloat.h"
#include "base/CbmLitFieldFitter.h"
#include "base/CbmLitFieldGridCreator.h"
#include "utils/CbmLitUtils.h"
#include "CbmDrawHist.h"
#include "CbmHistManager.h"

#include "parallel/LitFieldGrid.h"
#include "parallel/LitFieldSlice.h"
#include "parallel/LitFieldValue.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"

#include "TH2D.h"
#include "TGraph2D.h"
#include "TGraph.h"
#include "TF1.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TLegend.h"

#include <sstream>
#include <cmath>
#include <string>
#include <limits>

#include <boost/assign/list_of.hpp>

using lit::ToString;
using lit::SaveCanvasAsImage;
using lit::parallel::LitFieldSliceScal;
using lit::parallel::LitFieldValueScal;
using lit::parallel::LitFieldGrid;
using boost::assign::list_of;
using std::cout;
using std::string;
using std::endl;

CbmLitFieldApproximationQa::CbmLitFieldApproximationQa():
   fField(NULL),
   fNofSlices(0),
   fZSlicePosition(),
   fXSlicePosition(),
   fYSlicePosition(),
   fFixedBounds(true),
   fAcceptanceAngleX(25.),
   fAcceptanceAngleY(25.),
   fNofBinsX(30),
   fNofBinsY(30),
   fUseEllipseAcc(true),
   fOutputDir("./field/"),
   fFitter(),
   fGridCreator(),
   fPolynomDegreeIndex(1),
   fNofPolynoms(4),
   fPolynomDegrees(),
   fHM(NULL)
{

}

CbmLitFieldApproximationQa::~CbmLitFieldApproximationQa()
{
}

InitStatus CbmLitFieldApproximationQa::Init()
{
   fNofSlices = fZSlicePosition.size();
   fNofPolynoms = fPolynomDegrees.size();

   // Calculate (X, Y) window for each slice
   fXSlicePosition.resize(fNofSlices);
   fYSlicePosition.resize(fNofSlices);
   for (Int_t i = 0; i < fNofSlices; i++) {
      Double_t tanXangle = tan(fAcceptanceAngleX * 3.14159265 / 180); //
      Double_t tanYangle = tan(fAcceptanceAngleY * 3.14159265 / 180); //
      fXSlicePosition[i] = fZSlicePosition[i] * tanXangle;
      fYSlicePosition[i] = fZSlicePosition[i] * tanYangle;
   }

   // Create field approximation tool for each polynom degree
   fFitter.resize(fNofPolynoms);
   for (UInt_t i = 0; i < fNofPolynoms; i++) {
      fFitter[i] = new CbmLitFieldFitter(fPolynomDegrees[i]);
      fFitter[i]->SetXangle(fAcceptanceAngleX);
      fFitter[i]->SetYangle(fAcceptanceAngleY);
      fFitter[i]->SetNofBinsX(100);//fNofBinsX);
      fFitter[i]->SetNofBinsY(100);//fNofBinsY);
      fFitter[i]->SetUseEllipseAcc(fUseEllipseAcc);
   }

   // Create grid creator tool
   fGridCreator = new CbmLitFieldGridCreator();

   fField = FairRunAna::Instance()->GetField();

   fHM = new CbmHistManager();

   // Fill and create graphs and histograms
   CreateHistos();

   // This is always needed since field map is always drawn for comparison
   FillBHistos();

   // Check and draw polynomial field approximation
   FillFieldApproximationHistos();

   // Check and draw histograms for grid creator tool
   FillGridCreatorHistos();

   CbmSimulationReport* report = new CbmLitFieldApproximationQaReport();
   report->Create(fHM, fOutputDir);
   delete report;

   fHM->WriteToFile();

   return kSUCCESS;
}

void CbmLitFieldApproximationQa::Exec(
   Option_t* opt)
{

}

void CbmLitFieldApproximationQa::Finish()
{

}

void CbmLitFieldApproximationQa::CreateHistos()
{
   CreateFieldHistos();
   CreateFitterHistos();
   CreateGridHistos();
   cout << fHM->ToString();
}

void CbmLitFieldApproximationQa::CreateFieldHistos()
{
   string names[] = {"Bx", "By", "Bz", "Mod"};
   string zTitle[] = {"B_{x} [kGauss]", "B_{y} [kGauss]", "B_{z} [kGauss]", "|B| [kGauss]"};
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         TGraph2D* graph = new TGraph2D();
         string name = "hfa_" + names[v] + "_Graph2D_" + ToString<Int_t>(fZSlicePosition[i]);
         string title = name + ";X [cm];Y [cm];" + zTitle[v];
         graph->SetNameTitle(name.c_str(), title.c_str());
         fHM->Add(name, graph);
      }
   }
}

void CbmLitFieldApproximationQa::CreateFitterHistos()
{
   string names[] = {"Bx", "By", "Bz", "Mod"};

   Int_t nofBinsX = fNofBinsX;
   Int_t nofBinsY = fNofBinsY;
   Int_t nofBinsErrB = 100;
   Int_t nofBinsRelErrB = 100;
   Int_t nofBinsErrX = 100;
   Int_t nofBinsErrY = 100;
   Double_t minErrB = 0., maxErrB = 0., minRelErrB = 0., maxRelErrB = 0.;
   if (fFixedBounds) {
      minErrB = -0.5;
      maxErrB = 0.5;
      minRelErrB = -10.;
      maxRelErrB = 10.;
   }

   string zTitle[] = {"B_{x} [kGauss]", "B_{y} [kGauss]", "B_{z} [kGauss]", "|B| [kGauss]"};
   string errTitle[] = {"B_{x} error [kGauss]", "B_{y} error [kGauss]", "B_{z} error [kGauss]", "|B| error [kGauss]"};
   string relErrTitle[] = {"B_{x} relative error [%]", "B_{y} relative error [%]", "B_{z} relative error [%]", "|B| relative error [%]"};

   // Create histograms
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         for(Int_t j = 0; j < fNofPolynoms; j++) {
            TGraph2D* graph = new TGraph2D();
            string name = "hfa_" + names[v] + "Apr_Graph2D" + "_" + ToString<Int_t>(fZSlicePosition[i]) + "_" + ToString<Int_t>(fPolynomDegrees[j]);
            string title = name + ";X [cm];Y [cm];" + zTitle[v];
            graph->SetNameTitle(name.c_str(), title.c_str());
            fHM->Add(name, graph);

            name = "hfa_" + names[v] + "ErrApr_H1_" + ToString<Int_t>(fZSlicePosition[i]) + "_" + ToString<Int_t>(fPolynomDegrees[j]);
            title = name + ";" + errTitle[v] + ";Counter";
            fHM->Add(name, new TH1D(name.c_str(), title.c_str(), nofBinsErrB, minErrB, maxErrB));

            name = "hfa_" + names[v] + "ErrApr_H2_" + ToString<Int_t>(fZSlicePosition[i]) + "_" + ToString<Int_t>(fPolynomDegrees[j]);
            title = name + ";X [cm];Y [cm];" + errTitle[v];
            fHM->Add(name, new TH2D(name.c_str(), title.c_str(), nofBinsErrX, -fXSlicePosition[i], fXSlicePosition[i], nofBinsErrY, -fYSlicePosition[i], fYSlicePosition[i]));

            name = "hfa_" + names[v] + "RelErrApr_H1_" + ToString<Int_t>(fZSlicePosition[i]) + "_" + ToString<Int_t>(fPolynomDegrees[j]);
            title = name + ";" + relErrTitle[v] + ";Counter";
            fHM->Add(name, new TH1D(name.c_str(), title.c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB));

            name = "hfa_" + names[v] + "RelErrApr_H2_" + ToString<Int_t>(fZSlicePosition[i]) + "_" + ToString<Int_t>(fPolynomDegrees[j]);
            title = name + ";X [cm];Y [cm];" + relErrTitle[v];
            fHM->Add(name, new TH2D(name.c_str(), title.c_str(), nofBinsErrX, -fXSlicePosition[i], fXSlicePosition[i], nofBinsErrY, -fYSlicePosition[i], fYSlicePosition[i]));
         }
      }
   }
   cout << "-I- CbmLitFieldApproximationQa::CreateFitterErrHistos: Field fitter error histograms created" << endl;
}

void CbmLitFieldApproximationQa::CreateGridHistos()
{
   string names[] = {"Bx", "By", "Bz", "Mod"};

   Int_t nofBinsX = fNofBinsX;
   Int_t nofBinsY = fNofBinsY;
   Int_t nofBinsErrB = 100;
   Int_t nofBinsRelErrB = 100;
   Int_t nofBinsErrX = 100;
   Int_t nofBinsErrY = 100;
   Double_t minErrB = 0., maxErrB = 0., minRelErrB = 0., maxRelErrB = 0.;
   if (fFixedBounds) {
      minErrB = -0.5;
      maxErrB = 0.5;
      minRelErrB = -10.;
      maxRelErrB = 10.;
   }

   string zTitle[] = {"B_{x} [kGauss]", "B_{y} [kGauss]", "B_{z} [kGauss]", "|B| [kGauss]"};
   string errTitle[] = {"B_{x} error [kGauss]", "B_{y} error [kGauss]", "B_{z} error [kGauss]", "|B| error [kGauss]"};
   string relErrTitle[] = {"B_{x} relative error [%]", "B_{y} relative error [%]", "B_{z} relative error [%]", "|B| relative error [%]"};

   // Create histograms
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         TGraph2D* graph = new TGraph2D();
         string name = "hfa_" + names[v] + "Grid_Graph2D_" + ToString<Int_t>(fZSlicePosition[i]);
         string title = name + ";X [cm]; Y [cm];" + zTitle[v];
         graph->SetNameTitle(name.c_str(), title.c_str());
         fHM->Add(name, graph);

         name = "hfa_" + names[v] + "ErrGrid_H1_" + ToString<Int_t>(fZSlicePosition[i]);
         title = name + ";" + errTitle[v] + ";Counter";
         fHM->Add(name, new TH1D(name.c_str(), title.c_str(), nofBinsErrB, minErrB, maxErrB));

         name = "hfa_" + names[v] +"ErrGrid_H2_" + ToString<Int_t>(fZSlicePosition[i]);
         title = name + ";X [cm];Y [cm];" + errTitle[v];
         fHM->Add(name, new TH2D(name.c_str(), title.c_str(), nofBinsErrX, -fXSlicePosition[i], fXSlicePosition[i], nofBinsErrY, -fYSlicePosition[i], fYSlicePosition[i]));

         name = "hfa_" + names[v] + "RelErrGrid_H1_" + ToString<Int_t>(fZSlicePosition[i]);
         title = name + ";" + relErrTitle[v] + ";Counter";
         fHM->Add(name, new TH1D(name.c_str(), title.c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB));

         name = "hfa_" + names[v] + "RelErrGrid_H2_" + ToString<Int_t>(fZSlicePosition[i]);
         title = name + ";X [cm];Y [cm];" + relErrTitle[v];
         fHM->Add(name, new TH2D(name.c_str(), title.c_str(), nofBinsErrX, -fXSlicePosition[i], fXSlicePosition[i], nofBinsErrY, -fYSlicePosition[i], fYSlicePosition[i]));
      }
   }
   cout << "-I- CbmLitFieldApproximationQa::CreateGridErrHistos(): Grid creator error histograms created" << endl;
}

void CbmLitFieldApproximationQa::FillBHistos()
{
   // Fill graphs for magnetic field for each (X, Y) slice
   for (UInt_t iSlice = 0; iSlice < fNofSlices; iSlice++) { // loop over slices
      Double_t Z = fZSlicePosition[iSlice];

      Int_t cnt = 0;
      Double_t HX = 2 * fXSlicePosition[iSlice] / fNofBinsX; // step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / fNofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < fNofBinsX; iX++) { // loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX + 0.5) * HX;
         for (Int_t iY = 0; iY < fNofBinsY; iY++) { // loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY + 0.5)  * HY;

            // Check acceptance for ellipse
            Double_t el = (X*X)/(fXSlicePosition[iSlice]*fXSlicePosition[iSlice]) + (Y*Y)/(fYSlicePosition[iSlice]*fYSlicePosition[iSlice]);
            if (fUseEllipseAcc && el > 1.) { continue; }

            // Get field value
            Double_t pos[3] = {X, Y, Z};
            Double_t B[3];
            fField->GetFieldValue(pos, B);

            Double_t Bmod = sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

            string s = ToString<Int_t>(fZSlicePosition[iSlice]);
            fHM->G2(string("hfa_Bx_Graph2D_") + s)->SetPoint(cnt, X, Y, B[0]);
            fHM->G2(string("hfa_By_Graph2D_") + s)->SetPoint(cnt, X, Y, B[1]);
            fHM->G2(string("hfa_Bz_Graph2D_") + s)->SetPoint(cnt, X, Y, B[2]);
            fHM->G2(string("hfa_Mod_Graph2D_") + s)->SetPoint(cnt, X, Y, Bmod);
            cnt++;
         }
      }
   }
}

void CbmLitFieldApproximationQa::FillFieldApproximationHistos()
{
   vector<vector<LitFieldSliceScal > > slices;
   slices.resize(fNofPolynoms);
   for (UInt_t i = 0; i < fNofPolynoms; i++) {slices[i].resize(fNofSlices);}

   // Approximate field in each slice for each polynomial degree
   for (UInt_t i = 0; i < fNofPolynoms; i++) {
      for(Int_t j = 0; j < fNofSlices; j++) {
         fFitter[i]->FitSlice(fZSlicePosition[j], slices[i][j]);
         cout << "-I- CbmLitFieldApproximationQa::FillFieldApproximationHistos: "
               << " field approximation (degree=" << fPolynomDegrees[i]
               << ", Z=" << fZSlicePosition[j] << ")" << endl;
      }
   }

   // Fill graph for approximated field map
   for (UInt_t iSlice = 0; iSlice < fNofSlices; iSlice++) { // Loop over slices
      Double_t Z = fZSlicePosition[iSlice];
      Int_t cnt = 0;

      Double_t HX = 2 * fXSlicePosition[iSlice] / fNofBinsX; // Step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / fNofBinsY; // Step size for Y position
      for (Int_t iX = 0; iX < fNofBinsX; iX++) { // Loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX + 0.5) * HX;
         for (Int_t iY = 0; iY < fNofBinsY; iY++) { // Loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY + 0.5)  * HY;

            // Check acceptance for ellipse
            Double_t el = (X*X)/(fXSlicePosition[iSlice]*fXSlicePosition[iSlice]) + (Y*Y)/(fYSlicePosition[iSlice]*fYSlicePosition[iSlice]);
            if (fUseEllipseAcc && el > 1.) { continue; }

            for (Int_t p = 0; p < fNofPolynoms; p++) {
               LitFieldValueScal v;
               slices[p][iSlice].GetFieldValue(X, Y, v);
               Double_t mod = sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);
               string s = ToString<Int_t>(fZSlicePosition[iSlice]) + "_" + ToString<Int_t>(fPolynomDegrees[p]);
               fHM->G2(string("hfa_BxApr_Graph2D_") + s)->SetPoint(cnt, X, Y, v.Bx);
               fHM->G2(string("hfa_ByApr_Graph2D_") + s)->SetPoint(cnt, X, Y, v.By);
               fHM->G2(string("hfa_BzApr_Graph2D_") + s)->SetPoint(cnt, X, Y, v.Bz);
               fHM->G2(string("hfa_ModApr_Graph2D_") + s)->SetPoint(cnt, X, Y, mod);
            }
            cnt++;
         } // End loop over y position
      } // End loop over x position
   } // End loop over slices

   // Fill error histograms
   Int_t nofBinsX = 100;
   Int_t nofBinsY = 100;
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      Double_t Z = fZSlicePosition[iSlice];
      Double_t HX = 2 * fXSlicePosition[iSlice] / nofBinsX; // step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / nofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < nofBinsX; iX++) { // loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX+0.5) * HX;
         for (Int_t iY = 0; iY < nofBinsY; iY++) { // loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY+0.5) * HY;

            // Check acceptance for ellipse
            Double_t el = (X*X)/(fXSlicePosition[iSlice]*fXSlicePosition[iSlice]) + (Y*Y)/(fYSlicePosition[iSlice]*fYSlicePosition[iSlice]);
            if (fUseEllipseAcc && el > 1.) { continue; }

            // Get field value
            Double_t pos[3] = {X, Y, Z};
            Double_t B[3];
            fField->GetFieldValue(pos, B);

            Double_t Bmod = sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

            for (Int_t p = 0; p < fNofPolynoms; p++) {
               LitFieldValueScal v;
               slices[p][iSlice].GetFieldValue(X, Y, v);
               Double_t mod = sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);

               Double_t errBx = B[0] - v.Bx;
               Double_t errBy = B[1] - v.By;
               Double_t errBz = B[2] - v.Bz;
               Double_t errMod = Bmod - mod;
               Double_t relErrBx = (B[0] != 0.) ? (errBx / B[0]) * 100. : 0.;
               Double_t relErrBy = (B[1] != 0.) ? (errBy / B[1]) * 100. : 0.;
               Double_t relErrBz = (B[2] != 0.) ? (errBz / B[2]) * 100. : 0.;
               Double_t relErrMod = (Bmod != 0.) ? (errMod / Bmod) * 100. : 0;

               string s = ToString<Int_t>(fZSlicePosition[iSlice]) + "_" + ToString<Int_t>(fPolynomDegrees[p]);
               fHM->H1(string("hfa_BxErrApr_H1_") + s)->Fill(errBx);
               fHM->H1(string("hfa_BxRelErrApr_H1_") + s)->Fill(relErrBx);
               fHM->H2(string("hfa_BxErrApr_H2_") + s)->Fill(X, Y, errBx);
               fHM->H2(string("hfa_BxRelErrApr_H2_") + s)->Fill(X, Y, relErrBx);
               fHM->H1(string("hfa_ByErrApr_H1_") + s)->Fill(errBy);
               fHM->H1(string("hfa_ByRelErrApr_H1_") + s)->Fill(relErrBy);
               fHM->H2(string("hfa_ByErrApr_H2_") + s)->Fill(X, Y, errBy);
               fHM->H2(string("hfa_ByRelErrApr_H2_") + s)->Fill(X, Y, relErrBy);
               fHM->H1(string("hfa_BzErrApr_H1_") + s)->Fill(errBz);
               fHM->H1(string("hfa_BzRelErrApr_H1_") + s)->Fill(relErrBz);
               fHM->H2(string("hfa_BzErrApr_H2_") + s)->Fill(X, Y, errBz);
               fHM->H2(string("hfa_BzRelErrApr_H2_") + s)->Fill(X, Y, relErrBz);
               fHM->H1(string("hfa_ModErrApr_H1_") + s)->Fill(errMod);
               fHM->H1(string("hfa_ModRelErrApr_H1_") + s)->Fill(relErrMod);
               fHM->H2(string("hfa_ModErrApr_H2_") + s)->Fill(X, Y, errMod);
               fHM->H2(string("hfa_ModRelErrApr_H2_") + s)->Fill(X, Y, relErrMod);
            }
         }
      }
   }
}

void CbmLitFieldApproximationQa::FillGridCreatorHistos()
{
   vector<LitFieldGrid> grids;
   grids.resize(fNofSlices);
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      fGridCreator->CreateGrid(fZSlicePosition[iSlice], grids[iSlice]);
   }

   // Fill graph
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      Int_t cnt = 0;
      Double_t Z = fZSlicePosition[iSlice];
      Double_t HX = 2 * fXSlicePosition[iSlice] / fNofBinsX; // step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / fNofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < fNofBinsX; iX++) { // loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX + 0.5) * HX;
         for (Int_t iY = 0; iY < fNofBinsY; iY++) { // loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY + 0.5)  * HY;
            LitFieldValueScal v;
            grids[iSlice].GetFieldValue(X, Y, v);
            Double_t mod = sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);
            string s = ToString<Int_t>(fZSlicePosition[iSlice]);
            fHM->G2(string("hfa_BxGrid_Graph2D_") + s)->SetPoint(cnt, X, Y, v.Bx);
            fHM->G2(string("hfa_ByGrid_Graph2D_") + s)->SetPoint(cnt, X, Y, v.By);
            fHM->G2(string("hfa_BzGrid_Graph2D_") + s)->SetPoint(cnt, X, Y, v.Bz);
            fHM->G2(string("hfa_ModGrid_Graph2D_") + s)->SetPoint(cnt, X, Y, mod);
            cnt++;
         }
      }
   }

   // Fill error histograms
   Int_t nofBinsX = 100;
   Int_t nofBinsY = 100;
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      Double_t Z = fZSlicePosition[iSlice];
      Double_t HX = 2 * fXSlicePosition[iSlice] / nofBinsX; // step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / nofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < nofBinsX; iX++) { // loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX+0.5) * HX;
         for (Int_t iY = 0; iY < nofBinsY; iY++) { // loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY+0.5) * HY;

            // Get field value
            Double_t pos[3] = {X, Y, Z};
            Double_t B[3];
            fField->GetFieldValue(pos, B);

            Double_t Bmod = sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

            LitFieldValueScal v;
            grids[iSlice].GetFieldValue(X, Y, v);
            Double_t mod = sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);

            Double_t errBx = B[0] - v.Bx;
            Double_t errBy = B[1] - v.By;
            Double_t errBz = B[2] - v.Bz;
            Double_t errMod = Bmod - mod;
            Double_t relErrBx = (B[0] != 0.) ? (errBx / B[0]) * 100. : 0.;
            Double_t relErrBy = (B[1] != 0.) ? (errBy / B[1]) * 100. : 0.;
            Double_t relErrBz = (B[2] != 0.) ? (errBz / B[2]) * 100. : 0.;
            Double_t relErrMod = (Bmod != 0.) ? (errMod / Bmod) * 100. : 0;

            string s = ToString<Int_t>(fZSlicePosition[iSlice]);
            fHM->H1(string("hfa_BxErrGrid_H1_") + s)->Fill(errBx);
            fHM->H1(string("hfa_BxRelErrGrid_H1_") + s)->Fill(relErrBx);
            fHM->H2(string("hfa_BxErrGrid_H2_") + s)->Fill(X, Y, errBx);
            fHM->H2(string("hfa_BxRelErrGrid_H2_") + s)->Fill(X, Y, relErrBx);
            fHM->H1(string("hfa_ByErrGrid_H1_") + s)->Fill(errBy);
            fHM->H1(string("hfa_ByRelErrGrid_H1_") + s)->Fill(relErrBy);
            fHM->H2(string("hfa_ByErrGrid_H2_") + s)->Fill(X, Y, errBy);
            fHM->H2(string("hfa_ByRelErrGrid_H2_") + s)->Fill(X, Y, relErrBy);
            fHM->H1(string("hfa_BzErrGrid_H1_") + s)->Fill(errBz);
            fHM->H1(string("hfa_BzRelErrGrid_H1_") + s)->Fill(relErrBz);
            fHM->H2(string("hfa_BzErrGrid_H2_") + s)->Fill(X, Y, errBz);
            fHM->H2(string("hfa_BzRelErrGrid_H2_") + s)->Fill(X, Y, relErrBz);
            fHM->H1(string("hfa_ModErrGrid_H1_") + s)->Fill(errMod);
            fHM->H1(string("hfa_ModRelErrGrid_H1_") + s)->Fill(relErrMod);
            fHM->H2(string("hfa_ModErrGrid_H2_") + s)->Fill(X, Y, errMod);
            fHM->H2(string("hfa_ModRelErrGrid_H2_") + s)->Fill(X, Y, relErrMod);
         }
      }
   }
}

ClassImp(CbmLitFieldApproximationQa);
