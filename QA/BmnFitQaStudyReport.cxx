/**
 * \file CbmLitFitQaStudyReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
#include "CbmLitFitQaStudyReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "CbmLitUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include <vector>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;
using std::vector;
using lit::Split;
using lit::NumberToString;
using lit::FindAndReplace;

CbmLitFitQaStudyReport::CbmLitFitQaStudyReport():
		CbmStudyReport()
{
   SetReportName("fit_qa_study");
   SetReportTitle("Fit QA study report");
}

CbmLitFitQaStudyReport::~CbmLitFitQaStudyReport()
{
}

void CbmLitFitQaStudyReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());
   PrintCanvases();
   Out() <<  R()->DocumentEnd();
}

void CbmLitFitQaStudyReport::Draw()
{
   SetDefaultDrawStyle();
   DrawTrackParamsAtVertex();
}

void CbmLitFitQaStudyReport::DrawTrackParamsAtVertex()
{
   Int_t nofStudies = HM().size();

   TCanvas* canvas2 = CreateCanvas("fit_study_qa_momentum_projection", "fit_study_qa_momentum_projection", 600, 600);
   vector<TH1*> histos1(nofStudies);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
     histos1[iStudy] = HM()[iStudy]->H2("htf_MomRes_Mom")->ProjectionY(string("htf_MomRes_Mom_ProjectionY_" + NumberToString<Int_t>(iStudy)).c_str());
     histos1[iStudy]->SetStats(true);
     histos1[iStudy]->Fit("gaus");
     histos1[iStudy]->SetMaximum(histos1[iStudy]->GetMaximum() * 1.25);
   }
   DrawH1(histos1, GetStudyNames(), kLinear, kLinear);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      histos1[iStudy]->GetFunction("gaus")->SetLineColor(histos1[iStudy]->GetLineColor());
   }
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   TCanvas* canvas3 = CreateCanvas("fit_study_qa_momentum_momres_mom_sigma", "fit_study_qa_momentum_momres_mom_sigma", 600, 600);
   vector<TH1*> histos2(nofStudies);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      HM()[iStudy]->H2("htf_MomRes_Mom")->FitSlicesY();
      histos2[iStudy] = (TH1*) gDirectory->Get("htf_MomRes_Mom_2");
      histos2[iStudy]->SetName(string("htf_MomRes_Mom_2_" + NumberToString<Int_t>(iStudy)).c_str());
      histos2[iStudy]->GetXaxis()->SetTitle("P [GeV/c]");
      histos2[iStudy]->GetYaxis()->SetTitle("dP/P, #sigma [%]");
      histos2[iStudy]->SetMinimum(0.);
      histos2[iStudy]->SetMaximum(3.);
   }
   DrawH1(histos2, GetStudyNames(), kLinear, kLinear);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   TCanvas* canvas4 = CreateCanvas("fit_study_qa_momentum_momres_mom_rms", "fit_study_qa_momentum_momres_mom_rms", 600, 600);
   vector<TH1*> histos3(nofStudies);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      TH2* hMomres = HM()[iStudy]->H2("htf_MomRes_Mom");
      Int_t nBins = hMomres->GetNbinsX();
      histos3[iStudy] = hMomres->ProjectionX(string("htf_MomRes_Mom_ProjectionX_" + NumberToString<Int_t>(iStudy)).c_str());
      for (Int_t i = 1; i < nBins; i++){
         TH1* projY = hMomres->ProjectionY("_py", i, i);
         Double_t rms = projY->GetRMS();
         histos3[iStudy]->SetBinContent(i, rms);
         histos3[iStudy]->SetBinError(i, histos2[iStudy]->GetBinError(i));
      }
      histos3[iStudy]->GetXaxis()->SetTitle("P [GeV/c]");
      histos3[iStudy]->GetYaxis()->SetTitle("dP/P, RMS [%]");
      histos3[iStudy]->SetMinimum(0.);
      histos3[iStudy]->SetMaximum(3.);
   }
   DrawH1(histos3, GetStudyNames(), kLinear, kLinear, "P");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   TCanvas* canvas5 = CreateCanvas("fit_study_qa_chi_primary", "fit_study_qa_chi_primary", 600, 600);
   vector<TH1*> histos4(nofStudies);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      histos4[iStudy] = HM()[iStudy]->H1("htf_ChiPrimary");
      histos4[iStudy]->Scale(1. /  histos4[iStudy]->Integral());
   }
   DrawH1(histos4, GetStudyNames(), kLinear, kLog);
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

ClassImp(CbmLitFitQaStudyReport)
