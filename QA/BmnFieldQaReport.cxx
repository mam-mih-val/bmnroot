/**
 * \file CbmLitFieldQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFieldQaReport.h"
#include "CbmReportElement.h"
#include "utils/CbmLitUtils.h"
#include "CbmDrawHist.h"
#include "CbmHistManager.h"
#include <boost/assign/list_of.hpp>
#include "TCanvas.h"
#include "TGraph2D.h"
#include "TGraph.h"

using lit::ToString;
using lit::FindAndReplace;
using lit::Split;
using boost::assign::list_of;

CbmLitFieldQaReport::CbmLitFieldQaReport()
{
   SetReportName("field_qa");
}

CbmLitFieldQaReport::~CbmLitFieldQaReport()
{

}

void CbmLitFieldQaReport::Create()
{
   Out() << R()->DocumentBegin() << std::endl;
   Out() << R()->Title(0, "Magnetic field QA") << std::endl;
   PrintCanvases();
   Out() << R()->DocumentEnd();
}

void CbmLitFieldQaReport::Draw()
{
   DrawFieldSlices();
   DrawFieldAlongZ();
}

void CbmLitFieldQaReport::DrawFieldSlices()
{
   vector<TGraph2D*> graphsBx = HM()->G2Vector("hmf_Bx_Graph2D_.*");
   for (Int_t i = 0; i < graphsBx.size(); i++) {
      string name = graphsBx[i]->GetName();
      string canvasName = GetReportName() + "_map_at_z_" + Split(name, '_')[3];
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1000, 1000);
      canvas->Divide(2, 2);
      TGraph2D* graphBy = HM()->G2(FindAndReplace(name, "_Bx_", "_By_"));
      TGraph2D* graphBz = HM()->G2(FindAndReplace(name, "_Bx_", "_Bz_"));
      TGraph2D* graphMod = HM()->G2(FindAndReplace(name, "_Bx_", "_Mod_"));
      canvas->cd(1);
      DrawGraph2D(graphsBx[i]);
      canvas->cd(2);
      DrawGraph2D(graphBy);
      canvas->cd(3);
      DrawGraph2D(graphBz);
      canvas->cd(4);
      DrawGraph2D(graphMod);
   }
}

void CbmLitFieldQaReport::DrawFieldAlongZ()
{
   // Draw for different angles
   vector<TGraph*> graphsBx = HM()->G1Vector("hmf_BxAlongZAngle_Graph_.*");
   for (Int_t i = 0; i < graphsBx.size(); i++) {
      string name = graphsBx[i]->GetName();
      string canvasName = GetReportName() + "_map_along_z_angle_" + Split(name, '_')[3];
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1000, 1000);
      TGraph* graphBy = HM()->G1(FindAndReplace(name, "_Bx", "_By"));
      TGraph* graphBz = HM()->G1(FindAndReplace(name, "_Bx", "_Bz"));
      TGraph* graphMod = HM()->G1(FindAndReplace(name, "_Bx", "_Mod"));
      DrawGraph(list_of(graphsBx[i])(graphBy)(graphBz)(graphMod),
                  list_of("B_{x}")("B_{y}")("B_{z}")("|B|"), kLinear, kLinear, true, 0.7, 0.5, 0.9, 0.3);
      gPad->SetGrid(true, true);
   }

   // Draw for different XY positions
   graphsBx = HM()->G1Vector("hmf_BxAlongZXY_Graph_.*");
   for (Int_t i = 0; i < graphsBx.size(); i++) {
      string name = graphsBx[i]->GetName();
      vector<string> splits = Split(name, '_');
      string canvasName = GetReportName() + "_map_along_z_xy_" + splits[3] + "_" + splits[4];
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1000, 1000);
      TGraph* graphBy = HM()->G1(FindAndReplace(name, "_Bx", "_By"));
      TGraph* graphBz = HM()->G1(FindAndReplace(name, "_Bx", "_Bz"));
      TGraph* graphMod = HM()->G1(FindAndReplace(name, "_Bx", "_Mod"));
      DrawGraph(list_of(graphsBx[i])(graphBy)(graphBz)(graphMod),
                  list_of("B_{x}")("B_{y}")("B_{z}")("|B|"), kLinear, kLinear, true, 0.7, 0.5, 0.9, 0.3);
      gPad->SetGrid(true, true);
   }

   // Draw integral for different XY positions
   graphsBx = HM()->G1Vector("hmf_BxAlongZXYIntegral_Graph_.*");
   for (Int_t i = 0; i < graphsBx.size(); i++) {
      string name = graphsBx[i]->GetName();
      vector<string> splits = Split(name, '_');
      string canvasName = GetReportName() + "_map_along_z_integral_xy_" + splits[3] + "_" + splits[4];
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1000, 1000);
      TGraph* graphBy = HM()->G1(FindAndReplace(name, "_Bx", "_By"));
      TGraph* graphBz = HM()->G1(FindAndReplace(name, "_Bx", "_Bz"));
      TGraph* graphMod = HM()->G1(FindAndReplace(name, "_Bx", "_Mod"));
      DrawGraph(list_of(graphsBx[i])(graphBy)(graphBz)(graphMod),
                  list_of("B_{x}")("B_{y}")("B_{z}")("|B|"), kLinear, kLinear, true, 0.7, 0.5, 0.9, 0.3);
      gPad->SetGrid(true, true);
   }
}

ClassImp(CbmLitFieldQaReport)
