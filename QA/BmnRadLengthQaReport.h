/**
 * \file CbmLitRadLengthQaReport.h
 * \brief Create report for radiation length QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
#ifndef CBMLITRADLENGTHQAREPORT_H_
#define CBMLITRADLENGTHQAREPORT_H_

#include "CbmSimulationReport.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
class TH1;

/**
 * \class CbmLitRadLengthQaReport
 * \brief Create report for radiation length QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
class CbmLitRadLengthQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitRadLengthQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitRadLengthQaReport();

protected:
   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Create();

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Draw();

   /**
    * \brief
    */
   void DrawDetector(
         const string& detName);

   /**
    * \brief
    */
   void DrawDetectorStation(
         const string& detName);

   ClassDef(CbmLitRadLengthQaReport, 1)
};

#endif /* CBMLITTRACKINGQAREPORT_H_ */
