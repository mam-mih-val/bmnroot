/**
 * \file BmnTrackingQaReport.h
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef BMNTRACKINGQAREPORT_H_
#define BMNTRACKINGQAREPORT_H_

#include "BmnSimulationReport.h"
#include "BmnDrawOnline.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
class TH1;

/**
 * \class BmnTrackingQaReport
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class BmnTrackingQaReport : public BmnSimulationReport {
public:


    /**
     * \brief Constructor.
     */
    BmnTrackingQaReport();

    /**
     * \brief Constructor with parameters.
     */
    BmnTrackingQaReport(TString name);

    /**
     * \brief Destructor.
     */
    virtual ~BmnTrackingQaReport();

protected:



    /**
     * \brief Inherited from BmnSimulationReport.
     */
    virtual void Create();

    /**
     * \brief Inherited from BmnSimulationReport.
     */
    virtual void Draw();

    void DrawMomResGem(const TString canvasName, TString name2d, TString nameSigma, TString nameMean, TString nameAver);
    void DrawResAndPull(const TString canvasName, TString* inNames);
    void DrawResAndPull_2D(const TString canvasName, TString* inNames);
    void DrawHitRes(TString pref, TString axis);
    void DrawPar(const TString canvasName, TString* inNames);
    void DrawVertResGem(const TString canvasName, TString name1dX, TString name1dY, TString name1dZ);
    void FillAndFitSlice(TString nameSigma, TString nameMean, TString name2d);
    void DrawMuSigma(TVirtualPad* pad, TH1* h);
   
    void DrawOneH1(const TString canvasName, const TString name1, const TString drawOpt);
    void DrawTwoH1(const TString canvasName, const TString name1, const TString name2, const TString drawOpt, Bool_t doFit);
    void DrawOneH2(const TString canvasName, const TString name1);
    void DrawTwoH2(const TString canvasName, const TString name1, const TString name2, Int_t w = 1000, Int_t h = 500);
    void DrawThreeH1(const TString canvasName, const TString name1, const TString name2, const TString name3);
    void DrawThreeH2(const TString canvasName, const TString name1, const TString name2, const TString name3);

    void DrawEffGhostGem(const TString canvasName);
    void DrawEffGem(const TString canvasName, TString* inNames, TString* outNames);

    void DrawNhitsGem(const TString canvasName);
    void DrawPtSimPtRec(const TString canvasName);
    void DrawMeanLine(TH1* hist);

    void DrawEventsInfo(const TString canvasName);

    string PrintEventInfo();

    vector<string> fGlobalTrackVariants;

    TString fPrefix;
private:
	BmnDrawOnline* drawHist;
	
	

    ClassDef(BmnTrackingQaReport, 1)
};

#endif /* BMNTRACKINGQAREPORT_H_ */
