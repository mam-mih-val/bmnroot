/**
 * \file BmnTrackingQaStudyReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for Bmn experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2011-2014
 */
#include "BmnTrackingQaStudyReport.h"
#include "BmnReportElement.h"
#include "BmnHistManager.h"
#include "BmnDrawHist.h"
#include "BmnUtils.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLine.h"
#include <vector>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;
using std::vector;
using lit::Split;
using lit::NumberToString;
using lit::FindAndReplace;

string DefaultRowNameFormatter(
		const string& name)
{
	return name;
}

string EventNoRowNameFormatter(
		const string& name)
{
	return Split(name, '_')[1];
}

string NofObjectsRowNameFormatter(
		const string& name)
{
	return Split(name, '_')[2];
}

string TrackHitsRowNameFormatter(
		const string& name)
{
	vector<string> split = Split(name, '_');
	return split[1] + ":" + split[3];
}

string NofGhostsRowNameFormatter(
		const string& name)
{
	return Split(name, '_')[2];
}

string EventNoCellFormatter(
      const TH1* hist,
      Int_t /* nofEvents */)
{
   return NumberToString<Double_t>(hist->GetEntries());
}

string NofObjectsCellFormatter(
      const TH1* hist,
      Int_t /* nofEvents */)
{
   return NumberToString<Double_t>(hist->GetMean());
}

string TrackHitsCellFormatter(
      const TH1* hist,
      Int_t /* nofEvents */)
{
   return NumberToString<Double_t>(hist->GetMean());
}

string NofGhostsCellFormatter(
      const TH1* hist,
      Int_t nofEvents)
{
   return NumberToString<Double_t>(hist->GetEntries() / nofEvents);
}


BmnTrackingQaStudyReport::BmnTrackingQaStudyReport():
		BmnStudyReport()
{
   SetReportName("tracking_qa_study");
   SetReportTitle("Tracking QA study report");
}

BmnTrackingQaStudyReport::~BmnTrackingQaStudyReport()
{
}

void BmnTrackingQaStudyReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   Out() << PrintTable("Number of events", "hen_EventNo_TrackingQa", EventNoRowNameFormatter, EventNoCellFormatter);
   Out() << PrintTable("Number of objects per event", "hno_NofObjects_.+", NofObjectsRowNameFormatter, NofObjectsCellFormatter);
   Out() << PrintTable("Number of all, true and fake hits in tracks and rings", "hth_.+_TrackHits_.*", TrackHitsRowNameFormatter, TrackHitsCellFormatter);
   Out() << PrintTable("Number of ghosts", "hng_NofGhosts_.+", NofGhostsRowNameFormatter, NofGhostsCellFormatter);
   Out() << PrintEfficiencyTable("Tracking efficiency with RICH", "hte_.*Rich.*_Eff_p");
   Out() << PrintEfficiencyTable("Tracking efficiency w/o RICH", "hte_((?!Rich).)*_Eff_p");
   PrintCanvases();
   Out() <<  R()->DocumentEnd();
}

string BmnTrackingQaStudyReport::PrintTable(
		const string& tableName,
		const string& pattern,
		string (*rowNameFormatter)(const string&),
		string (*cellFormatter)(const TH1*, Int_t)) const
{
   Int_t nofStudies = HM().size();
   vector<vector<TH1*> > histos(nofStudies);
   for (Int_t i = 0; i < nofStudies; i++) {
   	histos[i] = HM(i)->H1Vector(pattern);
	}
   string str = R()->TableBegin(tableName, list_of(string("")).range(GetStudyNames()));
   for (Int_t iHist = 0; iHist < histos[0].size(); iHist++) {
		string cellName = rowNameFormatter(histos[0][iHist]->GetName());
		vector<string> cells(nofStudies);
		for (Int_t i = 0; i < nofStudies; i++) {
		   Int_t nofEvents = HM(i)->H1("hen_EventNo_TrackingQa")->GetEntries();
			cells[i] = cellFormatter(histos[i][iHist], nofEvents);
		}
		str += R()->TableRow(list_of(cellName).range(cells));
	}
   str += R()->TableEnd();
   return str;
}

string BmnTrackingQaStudyReport::PrintEfficiencyTable(
		const string& tableName,
		const string& pattern) const
{
   Int_t nofStudies = HM().size();
   vector<vector<TH1*> > histos(nofStudies);// = fHM->H1Vector(effRegex);
   for (Int_t i = 0; i < nofStudies; i++) {
   	histos[i] = HM(i)->H1Vector(pattern);
	}
   string str = R()->TableBegin(tableName, list_of(string("")).range(GetStudyNames()));

   for (Int_t iHist = 0; iHist != histos[0].size(); iHist++) {
      vector<string> split = Split(histos[0][iHist]->GetName(), '_');
      string cellName = split[1] + "(" + split[2] + "):" + split[3];
      vector<string> cells(nofStudies);
      for (Int_t i = 0; i < nofStudies; i++) {
         Int_t nofEvents = HM(i)->H1("hen_EventNo_TrackingQa")->GetEntries();
         string effName = histos[0][iHist]->GetName();
         string accName = FindAndReplace(effName, "_Eff_", "_Acc_");
         string recName = FindAndReplace(effName, "_Eff_", "_Rec_");
         Double_t acc = HM(i)->H1(accName)->GetEntries() / nofEvents;
         Double_t rec = HM(i)->H1(recName)->GetEntries() / nofEvents;
         Double_t eff = (acc != 0.) ? 100. * rec / acc : 0.;
         string accStr = NumberToString<Double_t>(acc);
         string recStr = NumberToString<Double_t>(rec);
         string effStr = NumberToString<Double_t>(eff);
         cells[i] = effStr + "(" + recStr + "/" + accStr + ")";
      }
      str += R()->TableRow(list_of(cellName).range(cells));
   }
   str += R()->TableEnd();
   return str;
}

void BmnTrackingQaStudyReport::Draw()
{
   CalculateEfficiencyHistos();
   SetDefaultDrawStyle();
   DrawEfficiencyHistos();
}

void BmnTrackingQaStudyReport::DrawEfficiencyHistos()
{
   string histNamePattern = "hte_.+_.+_(All|Electron|Muon)_Eff_(p|Angle)";
   vector<TH1*> histos = HM()[0]->H1Vector(histNamePattern);
   for (UInt_t i = 0; i < histos.size(); i++) {
      string histName = histos[i]->GetName();
      DrawEfficiency("tracking_qa_study_" + histName, histName);
   }

   histNamePattern = "hte_.+_.+_(All|Electron|Muon)_Acc_(p|Angle)";
   histos = HM()[0]->H1Vector(histNamePattern);
   for (UInt_t i = 0; i < histos.size(); i++) {
      string histName = histos[i]->GetName();
      DrawAccAndRec("tracking_qa_study_" + histName, histName);
   }

   histNamePattern = "hte_.+_.+_(All|Electron|Muon)_Rec_(p|Angle)";
   histos = HM()[0]->H1Vector(histNamePattern);
   for (UInt_t i = 0; i < histos.size(); i++) {
      string histName = histos[i]->GetName();
      DrawAccAndRec("tracking_qa_study_" + histName, histName);
   }
}

void BmnTrackingQaStudyReport::DrawEfficiency(
      const string& canvasName,
      const string& histName)
{
   TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();
   canvas->cd();

   Int_t nofStudies = HM().size();
   vector<string> labels(nofStudies);
   vector<TH1*> histos(nofStudies);
   vector<Double_t> efficiencies(nofStudies);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      BmnHistManager* hm = HM()[iStudy];
      histos[iStudy] = hm->H1(histName);
      efficiencies[iStudy] = CalcEfficiency(hm->H1(FindAndReplace(histName, "_Eff_", "_Rec_")), hm->H1(FindAndReplace(histName, "_Eff_", "_Acc_")), 100.);
      labels[iStudy] = GetStudyName(iStudy) + "(" + NumberToString<Double_t>(efficiencies[iStudy], 1) + ")";
   }

   DrawH1(histos, labels, kLinear, kLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
   DrawMeanEfficiencyLines(histos, efficiencies);
}

void BmnTrackingQaStudyReport::DrawAccAndRec(
      const string& canvasName,
      const string& histName)
{
   TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();
   canvas->cd();

   Int_t nofStudies = HM().size();
   vector<string> labels(nofStudies);
   vector<TH1*> histos(nofStudies);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      BmnHistManager* hm = HM()[iStudy];
      Int_t nofEvents = hm->H1("hen_EventNo_TrackingQa")->GetEntries();
      histos[iStudy] = hm->H1(histName);
      histos[iStudy]->Scale(1./nofEvents);
      Double_t nofObjects = histos[iStudy]->GetEntries() / nofEvents;
      labels[iStudy] = GetStudyName(iStudy) + "(" + NumberToString<Double_t>(nofObjects, 1) + ")";
   }

   DrawH1(histos, labels, kLinear, kLinear, true, 0.65, 0.75, 0.95, 0.99);
}

void BmnTrackingQaStudyReport::DivideHistos(
   TH1* histo1,
   TH1* histo2,
   TH1* histo3,
   Double_t scale)
{
   histo1->Sumw2();
   histo2->Sumw2();
   histo3->Sumw2();
   histo3->Divide(histo1, histo2, 1., 1., "B");
   histo3->Scale(scale);
}

void BmnTrackingQaStudyReport::DrawMeanEfficiencyLines(
   const vector<TH1*>& histos,
   const vector<Double_t>& efficiencies)
{
   assert(histos.size() != 0 && efficiencies.size() == histos.size());

   Double_t minX = histos[0]->GetXaxis()->GetXmin();
   Double_t maxX = histos[0]->GetXaxis()->GetXmax();
   Int_t nofHistos = histos.size();
   for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
      TLine* line = new TLine(minX, efficiencies[iHist], maxX, efficiencies[iHist]);
      line->SetLineWidth(1);
      line->SetLineColor(histos[iHist]->GetLineColor());
      line->Draw();
   }
}

Double_t BmnTrackingQaStudyReport::CalcEfficiency(
   const TH1* histRec,
   const TH1* histAcc,
   Double_t scale) const
{
   if (histAcc->Integral() == 0 || histRec->Integral() == 0) {
      return 0.;
   } else {
      return scale * Double_t(histRec->Integral()) / Double_t(histAcc->Integral());
   }
}

void BmnTrackingQaStudyReport::CalculateEfficiencyHistos()
{
   Int_t nofStudies = HM().size();
   for (Int_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      vector<TH1*> effHistos = HM()[iStudy]->H1Vector("hte_.+_Eff_.+");
      Int_t nofEffHistos = effHistos.size();
      for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
         TH1* effHist = effHistos[iHist];
         string effHistName = effHist->GetName();
         string accHistName = FindAndReplace(effHistName, "_Eff_", "_Acc_");
         string recHistName = FindAndReplace(effHistName, "_Eff_", "_Rec_");
         DivideHistos(HM()[iStudy]->H1(recHistName), HM()[iStudy]->H1(accHistName), effHist, 100.);
         effHist->SetMinimum(0.);
         effHist->SetMaximum(100.);
      }
   }
}

ClassImp(BmnTrackingQaStudyReport)
