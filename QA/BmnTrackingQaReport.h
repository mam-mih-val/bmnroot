/**
 * \file BmnTrackingQaReport.h
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef BMNTRACKINGQAREPORT_H_
#define BMNTRACKINGQAREPORT_H_

#include "report/BmnSimulationReport.h"
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
    BmnTrackingQaReport(vector<string> header);

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

    void DrawMomResGem(const string& canvasName, TString name2d, TString nameSigma, TString nameMean);
    void DrawResAndPull(const TString canvasName, TString* inNames);
    void DrawPar(const TString canvasName, TString* inNames);
    void DrawVertResGem(const string& canvasName, TString name1dX, TString name1dY, TString name1dZ);
    void FillAndFitSlice(TString name2d, TString name1d);
    void DrawMuSigma(TVirtualPad* pad, TH1* h);
   
    void DrawOneH1(const TString canvasName, const TString name1, const TString drawOpt);
    void DrawTwoH1(const TString canvasName, const TString name1, const TString name2, const TString drawOpt);
    void DrawOneH2(const TString canvasName, const TString name1);
    void DrawTwoH2(const TString canvasName, const TString name1, const TString name2);
    void DrawThreeH1(const TString canvasName, const TString name1, const TString name2, const TString name3);
    void DrawThreeH2(const TString canvasName, const TString name1, const TString name2, const TString name3);

    void DrawEffGhostGem(const string& canvasName);
    void DrawEffGem(const TString canvasName, TString* inNames, TString* outNames);

    void DrawNhitsGem(const string& canvasName);
    void DrawPtSimPtRec(const string& canvasName);
    void DrawMeanLine(TH1* hist);

    void DrawEventsInfo(const string& canvasName);

    string PrintEventInfo();

    vector<string> fGlobalTrackVariants;

    vector<string> fHeader;

    ClassDef(BmnTrackingQaReport, 1)
};

#endif /* BMNTRACKINGQAREPORT_H_ */
