/**
 * \file CbmLitTofQaReport.h
 * \brief Create report for TOF QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
#ifndef CBMLITTOFQAREPORT_H_
#define CBMLITTOFQAREPORT_H_

#include "CbmSimulationReport.h"
#include "TSystem.h"

/**
 * \class CbmLitTofQaReport
 * \brief Create report for TOF QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
class CbmLitTofQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTofQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTofQaReport();

protected:
   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Create();

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Draw();

   void FitHistograms();

   void FitFunction(
   		TGraph* graph);

	ClassDef(CbmLitTofQaReport, 1);
};

#endif /* CBMLITTOFQAREPORT_H_ */

