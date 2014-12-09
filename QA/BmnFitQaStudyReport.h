/**
 * \file CbmLitFitQaStudyReport.h
 * \brief Creates study report for fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
#ifndef CBMLITFITQASTUDYREPORT_H_
#define CBMLITFITQASTUDYREPORT_H_

#include "CbmStudyReport.h"
#include <string>
using std::string;
class TH1;

/**
 * \class CbmLitFitQaStudyReport
 * \brief Creates study report for fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
class CbmLitFitQaStudyReport : public CbmStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFitQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFitQaStudyReport();

protected:
    /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create();

	/**
	* \brief Inherited from CbmLitStudyReport.
	*/
	void Draw();

	void DrawTrackParamsAtVertex();

   ClassDef(CbmLitFitQaStudyReport, 1)
};

#endif /* CBMLITFITQASTUDYREPORT_H_ */
