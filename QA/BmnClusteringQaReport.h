/**
 * \file BmnClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef BMNCLUSTERINGQAREPORT_H_
#define BMNCLUSTERINGQAREPORT_H_

#include "BmnSimulationReport.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include <string>
using std::string;

/**
 * \class BmnClusteringQaReport
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class BmnClusteringQaReport : public BmnSimulationReport {
public:
    /**
     * \brief Constructor.
     */
    BmnClusteringQaReport(Int_t nOfStationsGEM, Int_t nOfStationsSil);

    /**
     * \brief Destructor.
     */
    virtual ~BmnClusteringQaReport();

    static Double_t CalcEfficiency(
            const TH1* histRec,
            const TH1* histAcc,
            Double_t scale);

private:
    /**
     * \brief Inherited from CbmSimulationReport.
     */
    virtual void Create();

    /**
     * \brief Inherited from CbmSimulationReport.
     */
    virtual void Draw();

    /**
     * \brief Print number of objects table.
     */
    string PrintNofObjects() const;

    void DrawNofObjectsHistograms(const string& detName, const string& parameter);

    void DrawResidualsAndPulls(const string& detName);
    
    void DrawResXbyStation(const string& canvasName);
    void DrawResYbyStation(const string& canvasName);
    void DrawSimXRecXbyStation(const string& canvasName);
    void DrawSimYRecYbyStation(const string& canvasName);
    void DrawOccupancyByStation(const string& canvasName);
    
    void DrawPullXbyStation(const string& canvasName);
    void DrawPullYbyStation(const string& canvasName);

    //   void DrawAccAndRec(
    //         const string& canvasName,
    //         const string& histNamePattern);

    //   void DrawEfficiency(
    //         const string& canvasName,
    //         const string& histNamePattern,
    //         string (*labelFormatter)(const string&, const CbmHistManager*));
    
    
    void DrawEventsInfo(const string& canvasName);
    
    string PrintEventInfo();

    void ScaleAndShrinkHistograms();

    void DivideHistos(
            TH1* histo1,
            TH1* histo2,
            TH1* histo3,
            Double_t scale);

    void CalculateEfficiencyHistos(
            const string& acc,
            const string& rec,
            const string& eff);
    
    BmnGemStripStationSet* fGemDetector;
    
    Int_t nStationsGEM;
    Int_t nStationsSil;
    Int_t nStationsDCH1;

    ClassDef(BmnClusteringQaReport, 1)
};

#endif /* BMNCLUSTERINGQAREPORT_H_ */
