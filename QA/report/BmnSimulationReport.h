/**
 * \file BmnSimulationReport.h
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef BMNSIMULATIONREPORT_H_
#define BMNSIMULATIONREPORT_H_

#include "BmnReport.h"
#include "BmnDrawHist.h"
#include <string>

using std::ostream;
using std::string;

class BmnHistManager;

/**
 * \class BmnSimulationReport
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class BmnSimulationReport : public BmnReport {
public:
    /**
     * \brief Constructor.
     */
    BmnSimulationReport();

    /**
     * \brief Destructor.
     */
    virtual ~BmnSimulationReport();

    /**
     * \brief Main function which creates report data.
     *
     * Non virtual interface pattern is used here.
     * User always creates simulation report using this public non virtual method.
     * In order to configure concrete report generation class
     * user has to implement protected Create() method.
     *
     * \param[in] histManager Pointer to histogram manager for which report is created.
     * \param[in] outputDir Path to directory for output results.
     */
    void Create(
            BmnHistManager* histManager,
            const string& outputDir);

    /**
     * \brief Main function which creates report data.
     *
     * Same pattern is used here.
     *
     * \param[in] fileName Name of the file with histograms.
     * \param[in] outputDir Path to directory for output results.
     */
    void Create(
            const string& fileName,
            const string& outputDir);

    void SetHM(BmnHistManager *hm) {
        fHM = hm;
    }
    /**
     * \brief Return pointer to Histogram manager.
     */
    BmnHistManager* HM() const {
        return fHM;
    }

    /**
     * \brief Select by pattern TH1 histograms and draw each histogram on separate canvas.
     * \param[in] histNamePattern Name pattern for histogram.
     */
    void DrawH1ByPattern(
            const string& histNamePattern);

    /**
     * \brief Select by pattern TH1 histograms and draw all histograms on the same canvas.
     * \param[in] histNamePattern Name pattern for histogram.
     */
    void DrawH1ByPattern(
            const string& histNamePattern,
            string(*labelFormatter)(const string&, const BmnHistManager*));

    /**
     * \brief Select by pattern TH2 histograms and draw each histogram on separate canvas.
     * \param[in] histNamePattern Name pattern for histogram.
     * \param[in] logx Specify logarithmic or linear scale for X axis.
     * \param[in] logy Specify logarithmic or linear scale for Y axis.
     * \param[in] logz Specify logarithmic or linear scale for Z axis.
     * \param[in] drawOpt Other drawing options (see ROOT documentation for details).
     */
    void DrawH2ByPattern(
            const string& histNamePattern,
            HistScale logx = kLinear,
            HistScale logy = kLinear,
            HistScale logz = kLinear,
            const string& drawOpt = "");

    void SetOnlyPrimes(const Bool_t prime) {
        fPrimes = prime;
    }

    Bool_t GetOnlyPrimes() const {
        return fPrimes;
    }

private:
    BmnHistManager* fHM; // Histogram manager
    Bool_t fPrimes;

    BmnSimulationReport(const BmnSimulationReport&) = delete;
    BmnSimulationReport& operator=(const BmnSimulationReport&) = delete;

    ClassDef(BmnSimulationReport, 1)
};

#endif /* BmnSIMULATIONREPORT_H_ */
