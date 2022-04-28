/**
 * \file BmnTrackingQaStudyReport.h
 * \brief Creates study report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2011-2014
 */
#ifndef BMNTRACKINGQASTUDYREPORT_H_
#define BMNTRACKINGQASTUDYREPORT_H_

#include "BmnStudyReport.h"
#include <string>
using std::string;
class TH1;

/**
 * \class BmnTrackingQaStudyReport
 * \brief Creates study report for tracking QA.
 *
 * Generates tracking QA report from a number of results of different
 * simulations and reconstructions. Creates comparison tables.
 * Very useful for studies since all numbers are automatically
 * put in the comparison tables.
 *
 */
class BmnTrackingQaStudyReport : public BmnStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   BmnTrackingQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~BmnTrackingQaStudyReport();

protected:
    /**
    * \brief Inherited from BmnStudyReport.
    */
   void Create();

	/**
	* \brief Inherited from BmnStudyReport.
	*/
	void Draw();

    /**
     * \brief Return formated string with table of numbers.
     * \param[in] tableName Table name.
     * \param[in] pattern Pattern of histogram name.
     * \param[in] rowNameFormatter Function which formats row name.
     * \param[in] cellFormatter Function formats cell string.
     * \return Formated string with table of numbers.
     */
	  string PrintTable(
	      const string& tableName,
	      const string& pattern,
	      string (*rowNameFormatter)(const string&),
	      string (*cellFormatter)(const TH1*, Int_t)) const;

    /**
     * \brief Return formated string with table of efficiency numbers.
     * \param[in] tableName Table name.
     * \param[in] pattern Pattern of histogram name.
     * \return Formated string with table of efficiency numbers.
     */
    string PrintEfficiencyTable(
    		const string& tableName,
    		const string& pattern) const;

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
       Double_t scale);

    /**
     * \brief Calculate efficiency histograms.
     */
    void CalculateEfficiencyHistos();

    /**
     * \brief Main function for drawing efficiency histograms.
     */
    void DrawEfficiencyHistos();

    /**
     * \brief Draw efficiency histogram.
     * \param[in] canvasName Name of canvas.
     * \param[in] histName Histogram name.
     */
    void DrawEfficiency(
      const string& canvasName,
      const string& histName);

    /**
     * \brief Draw mean efficiency lines on histogram.
     * \param[in] histos Vector of histograms.
     * \param[in] efficiencies Vector of efficiency numbers.
     */
    void DrawMeanEfficiencyLines(
       const vector<TH1*>& histos,
       const vector<Double_t>& efficiencies);

    /**
     * \brief Draw accepted and reconstructed tracks histograms.
     * \param[in] canvasName Name of canvas.
     * \param[in] histName Histogram name.
     */
    void DrawAccAndRec(
          const string& canvasName,
          const string& histName);

    /**
     * \brief Calculate efficiency for two histograms.
     * \param[in] histReco Reconstruction histogram.
     * \param[in] histAcc Acceptance histogram.
     * \param[in] scale Scaling factor for efficiency.
     */
    Double_t CalcEfficiency(
       const TH1* histRec,
       const TH1* histAcc,
       Double_t scale = 1.) const;

    ClassDef(BmnTrackingQaStudyReport, 1)
};

#endif /* BMNTRACKINGQASTUDYREPORT_H_ */
