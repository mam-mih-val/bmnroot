#ifndef BMNTRACKINGQAEXPREPORT_H_
#define BMNTRACKINGQAEXPREPORT_H_

#include "BmnSimulationReport.h"
#include "BmnDrawOnline.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
class TH1;

/**
 * \class BmnTrackingQaExpReport
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru> - disentangle Exp and MC processing
 * \date 2011-2022
 */
class BmnTrackingQaExpReport : public BmnSimulationReport {
public:


    /**
     * \brief Constructor.
     */
    BmnTrackingQaExpReport();

    /**
     * \brief Constructor with parameters.
     */
    BmnTrackingQaExpReport(TString name);

    /**
     * \brief Destructor.
     */
    virtual ~BmnTrackingQaExpReport();
    
    /**
     * \brief Inherited from BmnSimulationReport.
     */
    virtual void Draw();

protected:



    /**
     * \brief Inherited from BmnSimulationReport.
     */
    virtual void Create();


    void DrawMomResGem(const TString canvasName, TString name2d, TString nameSigma, TString nameMean, TString nameAver);
    void DrawResAndPull(const TString canvasName, TString* inNames);
    void DrawResAndPull_2D(const TString canvasName, TString* inNames);
    void DrawHitRes(TString pref, TString axis);
    void DrawPar(const TString canvasName, TString* inNames);
    void DrawVertResGem(const TString canvasName, TString name1dX, TString name1dY, TString name1dZ);
    void FillAndFitSlice(TString nameSigma, TString nameMean, TString name2d);

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
//	BmnDrawOnline* drawHist;
	
	

    ClassDef(BmnTrackingQaExpReport, 1)
};

#endif /* BMNTRACKINGQAEXPREPORT_H_ */
