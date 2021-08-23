#ifndef BMNDRAWONLINE_H_
#define BMNDRAWONLINE_H_

#include <TFile.h>
#include <THttpServer.h>
#include <TCanvas.h>
#include "BmnDrawHist.h"
#include "TSystem.h"
#include <string>
#include <vector>
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TPad.h"
#include "TLegend.h"
#include "TEfficiency.h"



using std::string;
using std::vector;

class BmnDrawOnline {
public:
	
	/**
     * \brief Constructor.
    */
	 
	BmnDrawOnline(TString param, TString storageName = "canvStorage");
	
	/**
     * \brief Constructor.
    */
	BmnDrawOnline(TFile*, Int_t port = 8080);
	
	/**
     * \brief Destructor.
     */
	~BmnDrawOnline();
	

	

	/**
	 * \fn DrawH1
	 * \brief Draw 1D histogram.
	 * \param[in] canvas Pointer to canvas
	 * \param[in] hist Pointer to histogram.
	 * \param[in] logx Specify logarithmic or linear scale for X axis.
	 * \param[in] logy Specify logarithmic or linear scale for Y axis.
	 * \param[in] drawOpt Other drawing options (see ROOT documentation for details).
	 * \param[in] color Color.
	 * \param[in] lineWidth Line width.
	 * \param[in] lineStyle Line style (see ROOT documentation for details).
	 * \param[in] markerSize Marker size.
	 * \param[in] markerStyle Marker style (see ROOT documentation for details).
	 */
	void DrawH1(
			TCanvas* canvas,
			TH1* hist,
			HistScale logx = kLinear,
			HistScale logy = kLinear,
			const string& drawOpt = "",
			Int_t color = BmnDrawingOptions::Color(0),
			Int_t lineWidth = BmnDrawingOptions::LineWidth(),
			Int_t lineStyle = BmnDrawingOptions::LineStyle(0),
			Float_t markerSize = BmnDrawingOptions::MarkerSize(),
			Int_t markerStyle = BmnDrawingOptions::MarkerStyle(0),
			Int_t fillColor = -1);

	void DrawH1(
			TCanvas* canvas,
			TEfficiency* hist);



	/**
	 * \fn DrawH2
	 * \brief Draw 2D histogram.
	 * \param[in] canvas Pointer to canvas
	 * \param[in] hist Pointer to histogram.
	 * \param[in] logx Specify logarithmic or linear scale for X axis.
	 * \param[in] logy Specify logarithmic or linear scale for Y axis.
	 * \param[in] logz Specify logarithmic or linear scale for Z axis.
	 * \param[in] drawOpt Other drawing options (see ROOT documentation for details).
	 */
	void DrawH2(
			TCanvas* canvas,
			TH2* hist,
			HistScale logx = kLinear,
			HistScale logy = kLinear,
			HistScale logz = kLinear,
			const string& drawOpt = "COLZ");

	/**
	 * \fn DrawH1
	 * \brief Draw several TH1 histograms.
	 * \param[in] canvas Pointer to canvas
	 * \param[in] histos Array of histograms.
	 * \param[in] histLabels Array of histogram labels.
	 * \param[in] logx Specify logarithmic or linear scale for X axis.
	 * \param[in] logy Specify logarithmic or linear scale for Y axis.
	 * \param[in] drawLegend If true than legend will be drawn.
	 * \param[in] x1 X coordinate of legend in current pad.
	 * \param[in] y1 Y coordinate of legend in current pad.
	 * \param[in] x2 X coordinate of legend in current pad.
	 * \param[in] y2 Y coordinate of legend in current pad.
	 * \param[in] drawOpt Draw options for TH1->Draw method.
	 */
	void DrawH1(
			TCanvas* canvas,
			const vector<TH1*>& histos,
			const vector<string>& histLabels,
			HistScale logx = kLinear,
			HistScale logy = kLinear,
			Bool_t drawLegend = true,
			Double_t x1 = 0.25,
			Double_t y1 = 0.99,
			Double_t x2 = 0.55,
			Double_t y2 = 0.75,
			const string& drawOpt = "",
			Bool_t outputMeanValue01 = kFALSE);		
	
	/**
	 * \fn InitServer
	 * \brief Create server on localhost
	 */
	void InitServer(Int_t);
	
	
	
	/**
	 * \fn DrawMainCanvas
	 * \brief Draw All hists on Canvas

	 */

	void DrawMainCanvas(TString);
	
	/**
	 * \fn RegisterCanvas
	 * \brief ...
	 * \param[in] file Pointer to canvasStorage root-file

	 */
	void RegisterCanvases(TFile*);
	

private:
	
	TFile* canvStorage;	
	THttpServer* fServer;
	TString canvName;
	vector<TCanvas*> canvVect;
	vector<TString> detVect;
	Int_t maxHeight;
	
	
	ClassDef(BmnDrawOnline, 1)
};

#endif
