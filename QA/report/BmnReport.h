/**
 * \file BmnLitReport.h
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef BMNREPORT_H_
#define BMNREPORT_H_

#include "TObject.h"
#include "TCanvas.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <THttpServer.h>

using namespace std;

class BmnReportElement;

/**
 * \class BmnReport
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class BmnReport : public TObject {
public:
   /**
    * \brief Constructor.
    */
   BmnReport();

   /**
    * \brief Destructor.
    */
   virtual ~BmnReport();

   /**
    * \brief Accessor to BmnReportElement object.
    * User has to write the report using available tags from BmnReportElement class.
    */
   const BmnReportElement* R() const { return fR; }

   /**
    * \brief All text output goes to this stream.
    */
   ostream& Out() const { return *fOut; }
   
   void Register(string path);
   void CallDraw(){ Draw();}

   /* Setters */
   void SetReportName(TString name) { fReportName = name.Data(); }
   void SetReportTitle(const string& title) { fReportTitle = title; }
   void SetOutputDir(const string& outputDir) { fOutputDir = outputDir; }
   void SetMonitorMode(const Bool_t mm) { fMonitorMode = mm; }
   void SetObjServer(THttpServer * s) { fServer = s; }

   /* Accessors */
   const string& GetReportName() const { return fReportName; }
   const string& GetReportTitle() const { return fReportTitle; }
   const string& GetOutputDir() const { return fOutputDir; }
   Bool_t GetMonitorMode() const { return fMonitorMode; }
   THttpServer * GetObjServer() const { return fServer; }

protected:
   /**
    * \brief Pure abstract function which is called from public Create() function.
    * This function has to write report using Out() for output stream and
    * R() for report elements. This function is called for each report type.
    */
   virtual void Create() = 0;

   /**
    * \brief Pure abstract function which is called from public Create() function.
    * This function has to draw all necessary histograms and graphs.
    * It is called only once before report creation.
    */
   virtual void Draw() = 0;

   /**
    * \brief Create all available report types.
    */
   void CreateReports();

   /**
    * \brief Create canvas and put it to vector of TCanvases.
    * Canvases created with this function will be automatically saved to image and
    * printed in the report.
    */
   TCanvas* CreateCanvas(
		   const char* name,
		   const char* title,
		   Int_t ww,
		   Int_t wh);

   /**
    * \brief Save all canvases to images.
    */
   void SaveCanvasesAsImages() const;

   /**
    * \brief Write canvases to file.
    */
   void WriteCanvases() const;

   /**
    * \brief Print images created from canvases in the report.
    */
   void PrintCanvases() const;
   
   
    THttpServer * fServer;
   Bool_t fMonitorMode;
   Bool_t fInitCanvasesDone;

private:
   /**
    * \brief Create concrete BmnReportElement instance based on report type.
    * \param[in] reportType Type of the report to be produced.
    */
   void CreateReportElement();

   /**
    * \brief Delete report element. Normally should be called at the end of Create function.
    */
   void DeleteReportElement();

   string fReportName; // Name of report
   string fReportTitle; // Title of report
   string fOutputDir; // Output directory for the report files
   BmnReportElement* fR; // Report element tool
   mutable ostream* fOut; // Output stream

   // Storage for TCanvas. All Canvases in this vector will be automatically saved
   // to image and printed in the report.
   // User can use CreateCanvas function which automatically push created canvas in this vector.
   vector<TCanvas*> fCanvases;
   map <string, TCanvas*> fCanvasMap;

//private:

   BmnReport(const BmnReport&) = delete;
   BmnReport& operator=(const BmnReport&) = delete;

   ClassDef(BmnReport, 1)
};

#endif /* BmnREPORT_H_ */
