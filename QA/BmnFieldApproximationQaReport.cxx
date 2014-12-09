/**
 * \file CbmLitFieldApproximationQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFieldApproximationQaReport.h"
#include "CbmReportElement.h"
#include "std/utils/CbmLitUtils.h"
#include "CbmDrawHist.h"
#include "CbmHistManager.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph2D.h"
#include <cassert>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>
using boost::assign::list_of;
using boost::assign::push_back;
using lit::ToString;
using lit::Split;
using lit::FindAndReplace;

CbmLitFieldApproximationQaReport::CbmLitFieldApproximationQaReport()
{
   SetReportName("fieldapr_qa");
}

CbmLitFieldApproximationQaReport::~CbmLitFieldApproximationQaReport()
{

}

void CbmLitFieldApproximationQaReport::Create()
{
   Out() << R()->DocumentBegin() << std::endl;
   Out() << R()->Title(0, "Magnetic field QA") << std::endl;
   Out() << PrintSummaryTable();
   PrintCanvases();
   Out() << R()->DocumentEnd();
}

std::string CbmLitFieldApproximationQaReport::PrintSummaryTable()
{
   std::stringstream ss;
//
//   int nofSlices = fQa.get("NofSlices", -1.);
//   assert(nofSlices > 0);
//   int nofPolynoms = fQa.get("NofPolynoms", -1.);
//   assert(nofPolynoms > 0);
//
//   const Int_t nerr = 4; // number of errors [absMean, absRMS, relMean, relRMS]
//   std::vector<string> colNames = list_of("")("Grid")("Grid")("Grid")("Grid");
//   for (int i = 0; i < nofPolynoms; i++) {
//      std::string pol = ToString<int>(i);
//      int degree = fQa.get("slice0.polynomial" + pol + ".degree", -1.);
//      assert(degree > 0);
//      std::string colName = "Polynomial" + ToString<int>(degree);
//      push_back(colNames).repeat(nerr, colName);
//   }
//   ss << R()->TableBegin("Summary table", colNames);
//
//   std::vector<string> colErrNames(1, "");
//   for (int i = 0; i < nofPolynoms + 1; i++) {
//      push_back(colErrNames)("abs mean")("abs RMS")("rel mean")("rel RMS");
//   }
//   ss << R()->TableRow(colErrNames);
//
//   std::string vnames[4] = {"BX", "BY", "BZ", "MOD"};
//   for (Int_t iSlice = 0; iSlice < nofSlices; iSlice++) {
//      std::string slice = "slice" + ToString<Int_t>(iSlice);
//      std::string sliceZ = ToString<float>(fQa.get(slice + ".Z", -1));
//      int nSpanCols = 1 + (nofPolynoms + 1) * nerr;
//      ss << R()->TableEmptyRow(nSpanCols, slice + " Z=" + sliceZ + " cm");
//      std::string prefix = slice + ".grid";
//      for (Int_t v = 0; v < 4; v++) {
//         std::vector<std::string> row;
//         row.push_back(vnames[v]);
//         row.push_back(ToString<float>(fQa.get(prefix + ".err." + vnames[v] + ".abs.mean", -1.)));
//         row.push_back(ToString<float>(fQa.get(prefix + ".err." + vnames[v] + ".abs.rms", -1.)));
//         row.push_back(ToString<float>(fQa.get(prefix + ".err." + vnames[v] + ".rel.mean", -1.)));
//         row.push_back(ToString<float>(fQa.get(prefix + ".err." + vnames[v] + ".rel.rms", -1.)));
//
//         for (Int_t iPolynom = 0; iPolynom < nofPolynoms; iPolynom++) {
//            std::string prefix = slice + ".polynomial" + ToString<Int_t>(iPolynom);
//            row.push_back(ToString<float>(fQa.get(prefix + ".err." + vnames[v] + ".abs.mean", -1.)));
//            row.push_back(ToString<float>(fQa.get(prefix + ".err." + vnames[v] + ".abs.rms", -1.)));
//            row.push_back(ToString<float>(fQa.get(prefix + ".err." + vnames[v] + ".rel.mean", -1.)));
//            row.push_back(ToString<float>(fQa.get(prefix + ".err." + vnames[v] + ".rel.rms", -1.)));
//         }
//         ss << R()->TableRow(row);
//      }
//   }
//   ss << R()->TableEnd();
//
   return ss.str();
}

void CbmLitFieldApproximationQaReport::Draw()
{
   // Set draw styles
   SetDefaultDrawStyle();

//   DrawSlices("Bx", "Apr");
//   DrawSlices("By", "Apr");
//   DrawSlices("Bz", "Apr");
//   DrawSlices("Mod", "Apr");
   DrawApr("RelErr");
   DrawApr("Err");

   DrawSlices("Bx", "Grid");
   DrawSlices("By", "Grid");
   DrawSlices("Bz", "Grid");
   DrawSlices("Mod", "Grid");
}

void CbmLitFieldApproximationQaReport::DrawSlices(
   const string& b,
   const string& m)
{
   vector<TGraph2D*> graphs2D = HM()->G2Vector("hfa_" + b + "_Graph2D_.*");
   for (Int_t i = 0; i < graphs2D.size(); i++) {
      string name = graphs2D[i]->GetName();
      string canvasName = GetReportName() + "_slice_" + b + "_" + m + "_at_z_" + Split(name, '_')[3];
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1000, 1000);
      canvas->Divide(3, 2);
      TGraph2D* aprGraph = HM()->G2(FindAndReplace(name, "_" + b + "_", "_" + b + m + "_"));
      TH1* errH1 = HM()->H1(FindAndReplace(name, "_" + b + "_Graph2D_", "_" + b + "Err" + m + "_H1_"));
      TH2* errH2 = HM()->H2(FindAndReplace(name, "_" + b + "_Graph2D_", "_" + b + "Err" + m + "_H2_"));
      TH1* relErrH1 = HM()->H1(FindAndReplace(name, "_" + b + "_Graph2D_", "_" + b + "RelErr" + m + "_H1_"));
      TH2* relErrH2 = HM()->H2(FindAndReplace(name, "_" + b + "_Graph2D_", "_" + b + "RelErr" + m + "_H2_"));
      canvas->cd(1);
      DrawGraph2D(graphs2D[i]);
      canvas->cd(2);
      DrawH1(errH1);
      canvas->cd(3);
      DrawH2(errH2);
      canvas->cd(4);
      DrawGraph2D(aprGraph);
      canvas->cd(5);
      DrawH1(relErrH1);
      canvas->cd(6);
      DrawH2(relErrH2);
   }
}

void CbmLitFieldApproximationQaReport::DrawApr(
   const string& err)
{
   vector<TGraph2D*> graphs2D = HM()->G2Vector("hfa_Bx_Graph2D_.*");
   for (Int_t i = 0; i < graphs2D.size(); i++) {
      string z = Split(graphs2D[i]->GetName(), '_')[3];
      string canvasName = GetReportName() + "_" + err + "_degree_z_" + z;
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1000, 1000);
      canvas->Divide(3, 2);

      TLegend* l1 = new TLegend(0.1,0.1,0.9,0.9);
      l1->SetFillColor(kWhite);
      l1->SetTextSize(0.1);
      l1->SetLineWidth(1);
      l1->SetHeader("Polynomial degree");

      Double_t maxBx = std::numeric_limits<Double_t>::min();
      Double_t maxBy = std::numeric_limits<Double_t>::min();
      Double_t maxBz = std::numeric_limits<Double_t>::min();
      Double_t maxMod = std::numeric_limits<Double_t>::min();
      vector<TH1*> errBx = HM()->H1Vector("hfa_Bx" + err + "Apr_H1_" + z + "_.*");
      if (errBx.empty()) return;
      vector<TH1*> errBy(errBx.size());
      vector<TH1*> errBz(errBx.size());
      vector<TH1*> errMod(errBx.size());
      for (Int_t iP = 0; iP < errBx.size(); iP++) {
         string name = errBx[iP]->GetName();

         errBy[iP] = HM()->H1(FindAndReplace(name, "_Bx", "_By"));
         errBz[iP] = HM()->H1(FindAndReplace(name, "_Bx", "_Bz"));
         errMod[iP] = HM()->H1(FindAndReplace(name, "_Bx", "_Mod"));
         string draw_opt = (iP == 0) ? "" : "SAME";
         canvas->cd(2);
         DrawH1(errBx[iP], kLinear, kLog, draw_opt.c_str(), 1 + iP, CbmDrawingOptions::LineWidth(), 1 + iP, CbmDrawingOptions::MarkerSize(), kDot);
         canvas->cd(3);
         DrawH1(errBy[iP], kLinear, kLog, draw_opt.c_str(), 1 + iP, CbmDrawingOptions::LineWidth(), 1 + iP, CbmDrawingOptions::MarkerSize(), kDot);
         canvas->cd(5);
         DrawH1(errBz[iP], kLinear, kLog, draw_opt.c_str(), 1 + iP, CbmDrawingOptions::LineWidth(), 1 + iP, CbmDrawingOptions::MarkerSize(), kDot);
         canvas->cd(6);
         DrawH1(errMod[iP], kLinear, kLog, draw_opt.c_str(), 1 + iP, CbmDrawingOptions::LineWidth(), 1 + iP, CbmDrawingOptions::MarkerSize(), kDot);

         string degree = Split(name, '_')[4];
         l1->AddEntry(errBx[iP], degree.c_str(), "lp");

         maxBx = std::max(maxBx, errBx[iP]->GetMaximum());
         maxBy = std::max(maxBy, errBy[iP]->GetMaximum());
         maxBz = std::max(maxBz, errBz[iP]->GetMaximum());
         maxMod = std::max(maxMod, errMod[iP]->GetMaximum());
      }
      errBx[0]->SetMaximum(1.1 * maxBx);
      errBy[0]->SetMaximum(1.1 * maxBy);
      errBz[0]->SetMaximum(1.1 * maxBz);
      errMod[0]->SetMaximum(1.1 * maxMod);

      canvas->cd(1);
      l1->Draw();
   }
}

ClassImp(CbmLitFieldApproximationQaReport)
