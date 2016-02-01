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

    /**
     * \brief Return string with number of objects statistics.
     * \return String with number of objects statistics.
     */
    string PrintNofObjects() const;

    /**
     * \brief Return string with hits histogram statistics (nof all, true, fake hits in track/ring).
     * \return String with hits histogram statistics (nof all, true, fake hits in track/ring).
     */
    string PrintTrackHits() const;

    /**
     * \brief Return string with number of ghosts statistics.
     * \return String with number of ghosts statistics.
     */
    string PrintNofGhosts() const;

    /**
     * \brief Return string with tracking efficiency.
     * \param[in] isPidEfficiency True if PID efficiency is plotted.
     * \return String with tracking efficiency.
     */
    string PrintTrackingEfficiency(Bool_t isPidEfficiency) const;

    /**
     * \brief Main function for drawing efficiency histograms.
     */
    void DrawEfficiencyHistos();

    /**
     * \brief Draw efficiency histogram.
     * \param[in] canvasName Name of canvas.
     * \param[in] histNamePattern Histogram name pattern.
     */
    void DrawEfficiency(
            const string& canvasName,
            const string& histNamePattern,
            string(*labelFormatter)(const string&, Float_t));

    void DrawMomResGlob(const string& canvasName);
    void DrawMomResGem(const string& canvasName, TString name2d, TString name1d);
    void DrawMomResChi2Gem(const string& canvasName);

    void DrawEtaP(const string& canvasName);

    void DrawEffGhostSeed(const string& canvasName);
    void DrawEffGhostGem(const string& canvasName);
    void DrawEffGem(const string& canvasName);
    void DrawEffEtaGem(const string& canvasName);
    void DrawEffThetaGem(const string& canvasName);
    void DrawEffGlob(const string& canvasName);
    void DrawNhitsGem(const string& canvasName);
    void DrawNhitsEtaGem(const string& canvasName);
    void DrawNhitsPGem(const string& canvasName);
    void DrawNhitsGlob(const string& canvasName);
    void DrawEffGhostGlob(const string& canvasName);
    void DrawPsimPrec(const string& canvasName);
    void DrawEtaSimEtaRec(const string& canvasName);
    void DrawTxSimTxRec(const string& canvasName);
    void DrawTySimTyRec(const string& canvasName);
    void DrawPtSimPtRec(const string& canvasName);
    void DrawPsimPrecComponentsGem(const string& canvasName);
    void DrawPsimPrecComponentsGlob(const string& canvasName);
    void DrawMeanLine(TH1* hist);

    void DrawEventsInfo(const string& canvasName);

    string PrintEventInfo();

    /**
     * \brief Draw mean efficiency lines on histogram.
     * \param[in] histos Vector of histograms.
     * \param[in] efficiencies Vector of efficiency numbers.
     */
    void DrawMeanEfficiencyLines(
            const vector<TH1*>& histos,
            const vector<Float_t>& efficiencies);

    /**
     * \brief Draw accepted and reconstructed tracks histograms.
     * \param[in] canvasName Name of canvas.
     * \param[in] histNamePattern Histogram name pattern.
     */
    void DrawAccAndRec(
            const string& canvasName,
            const string& histNamePattern);

    /**
     * \brief Draw histograms for hits.
     */
    void DrawHitsHistos();

    /**
     * \brief Draw histograms for hits. This function automatically
     * check the existence of histograms.
     * \param[in] canvasName Name of canvas.
     * \param[in] hist main name of hits histograms.
     */
    void DrawHitsHistos(
            const string& canvasName,
            const string& hist);

    /**
     * \brief Main function for drawing Rapidity-Pt histograms.
     */
    void DrawYPtHistos();

    /**
     * \brief Draw Rapidity-Pt histograms.
     * \param[in] canvasName Name of canvas.
     * \param[in] effHistName Name of the efficiency histogram.
     * \param[in] drawOnlyEfficiency Specify whether you want to draw only efficiency histo or including distributions.
     */
    void DrawYPt(
            const string& canvasName,
            const string& effHistName,
            Bool_t drawOnlyEfficiency = false);

    /**
     * \brief Calculate efficiency for two histograms.
     * \param[in] histReco Reconstruction histogram.
     * \param[in] histAcc Acceptance histogram.
     * \param[in] scale Scaling factor for efficiency.
     */
    Float_t CalcEfficiency(
            const TH1* histRec,
            const TH1* histAcc,
            Float_t scale = 1.) const;

    void FillGlobalTrackVariants();

    /**
     * \brief Divide two histograms.
     * \param[in] histo1 Numerator.
     * \param[in] histo2 Denominator.
     * \param[out] histo3 Output histogram.
     * \param[in] scale Scaling factor.
     */
    void DivideHistos(
            TH1* histo1,
            TH1* histo2,
            TH1* histo3,
            Float_t scale);

    /**
     * \brief Calculate efficiency histograms.
     */
    void CalculateEfficiencyHistos();

    void CalculatePionSuppressionHistos();

    vector<string> fGlobalTrackVariants;

    vector<string> fHeader;

    ClassDef(BmnTrackingQaReport, 1)
};

#endif /* BMNTRACKINGQAREPORT_H_ */
