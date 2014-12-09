/**
 * \file CbmLitClusteringQaStudyReport.h
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQASTUDYREPORT_H_
#define CBMLITCLUSTERINGQASTUDYREPORT_H_

#include "CbmStudyReport.h"
#include <string>
using std::string;

/**
 * \class CbmLitClusteringQaStudyReport
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitClusteringQaStudyReport : public CbmStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaStudyReport();

private:
   /**
    * \brief Inherited from CbmStudyReport.
    */
   void Create();

   /**
    * \brief Inherited from CbmStudyReport.
    */
   void Draw();

   /**
    * \brief Print number of objects table.
    */
   string PrintNofObjects() const;

   ClassDef(CbmLitClusteringQaStudyReport, 1)
};

#endif /* CBMLITCLUSTERINGQASTUDYREPORT_H_ */
