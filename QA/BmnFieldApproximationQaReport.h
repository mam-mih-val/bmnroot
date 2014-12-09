/**
 * \file CbmLitFieldApproximationQaReport.h
 * \brief Creates field QA report.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITFIELDAPPROXIMATIONQAREPORT_H_
#define CBMLITFIELDAPPROXIMATIONQAREPORT_H_

#include "CbmSimulationReport.h"
#include "TSystem.h"
#include <string>

/**
 * \class CbmLitFieldApproximationQaReport
 * \brief Creates field QA report.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitFieldApproximationQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFieldApproximationQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldApproximationQaReport();

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
    * \brief Draw canvas with histograms for each approximated slice.
    * \param[in] v Defines field component (BX, BY, BZ, MOD).
    * \param[in] opt Drawing option:
    *                   "grid" to plot results for grid creator
    *                   "apr" to plot results for field fitter.
    */
   void DrawSlices(
         const string& component,
         const std::string& opt);

   /**
    * \brief Draw comparison for different polynomial orders for each slice.
    * \param[in] err "rel" for relative errors or "abs" for absolute errors.
    */
   void DrawApr(
      const string& err);

   /**
    * \brief Return  string with summary table for field QA.
    * \return String with summary table for field QA.
    */
   string PrintSummaryTable();

   ClassDef(CbmLitFieldApproximationQaReport, 1)
};

#endif /* CBMLITFIELDAPPROXIMATIONQAREPORT_H_ */
