/**
 * \file BmnReport.cxx
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "BmnReport.h"
#include "BmnLatexReportElement.h"
#include "BmnHtmlReportElement.h"
#include "BmnTextReportElement.h"
#include "TCanvas.h"

#include <fstream>
#include <string>
using std::ofstream;
using std::string;
using std::cout;
using std::endl;

BmnReport::BmnReport():
   fReportName("qa_report"),
   fReportTitle("QA report"),
   fOutputDir("./"),
   fR(NULL),
   fOut(NULL),
   fReportType(kCoutReport),
   fCanvases()
{
}

BmnReport::~BmnReport()
{
}

void BmnReport::CreateReportElement(
      ReportType reportType)
{
   fReportType = reportType;
   if (NULL != fR) delete fR;
   if (NULL != fOut && fReportType != kCoutReport) delete fOut;
   if (reportType == kLatexReport) {
	   fR = new BmnLatexReportElement();
	   fOut = new ofstream(TString(GetOutputDir() + fReportName + ".tex").Data());
   } else if (reportType == kHtmlReport) {
	   fR = new BmnHtmlReportElement();
	   fOut = new ofstream(TString(GetOutputDir() + fReportName + ".html").Data());
   } else if (reportType == kTextReport) {
	   fR = new BmnTextReportElement();
	   fOut = new ofstream(TString(GetOutputDir() + fReportName + ".txt").Data());
   } else if (reportType == kCoutReport) {
	   fR = new BmnTextReportElement();
	   fOut = &std::cout;
   }
}

void BmnReport::DeleteReportElement()
{
 //  if (NULL != fR) delete fR;
 //  if (NULL != fOut && fReportType != kCoutReport) delete fOut;
}

void BmnReport::CreateReports()
{
   Draw(); // User has to implement this function!
   SaveCanvasesAsImages();
   WriteCanvases();

   CreateReportElement(kHtmlReport);
   Create(); // User has to implement this function!
   DeleteReportElement();

   CreateReportElement(kLatexReport);
   Create(); // User has to implement this function!
   DeleteReportElement();

   CreateReportElement(kTextReport);
   Create(); // User has to implement this function!
   DeleteReportElement();

   CreateReportElement(kCoutReport);
   Create(); // User has to implement this function!
   DeleteReportElement();
}

TCanvas* BmnReport::CreateCanvas(const char* name, const char* title, Int_t ww, Int_t wh) {
	TCanvas* canvas = new TCanvas(name, title, ww, wh);
	fCanvases.push_back(canvas);
	return canvas;
}

void BmnReport::SaveCanvasesAsImages() const
{
	if (GetOutputDir() == "") return;
	Int_t nofCanvases = fCanvases.size();
        cout << "nofCanvases = " << nofCanvases << endl;
	for (Int_t i = 0; i < nofCanvases; i++) {
		TCanvas* canvas = fCanvases[i];
		//canvas->SaveAs(string(GetOutputDir() + string(canvas->GetTitle()) + ".eps").c_str());
		canvas->SaveAs(TString(GetOutputDir() + string(canvas->GetTitle()) + ".png").Data());
//		canvas->SaveAs(string(GetOutputDir() + string(canvas->GetTitle()) + ".gif").c_str());
	}
}

void BmnReport::WriteCanvases() const
{
   if (GetOutputDir() == "") return;
   Int_t nofCanvases = fCanvases.size();
   for (Int_t i = 0; i < nofCanvases; i++) {
      fCanvases[i]->Write();
   }
}

void BmnReport::PrintCanvases() const
{
	Int_t nofCanvases = fCanvases.size();
	for (Int_t i = 0; i < nofCanvases; i++) {
		TCanvas* canvas = fCanvases[i];
		Out() << R()->Image(canvas->GetName(), canvas->GetName());
	}
}

ClassImp(BmnReport)
