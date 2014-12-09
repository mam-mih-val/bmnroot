/**
 * \file BmnStudyReport.h
 * \brief Base class for study reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef BMNSTUDYREPORT_H_
#define BMNSTUDYREPORT_H_

#include "BmnReport.h"
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::ostream;

class BmnHistManager;

/**
 * \class BmnStudyReport
 * \brief Base class for study reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class BmnStudyReport : public BmnReport
{
public:
   /**
    * \brief Constructor.
    */
   BmnStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~BmnStudyReport();

   /**
    * \brief Main function which creates report data.
    *
    * Non virtual interface pattern is used here.
    * User always creates simulation report using this public non virtual method.
    * In order to configure concrete report generation class
    * user has to implement protected Create() method
    * and getters for the file names.
    *
    * \param[in] histManagers Array of histogram managers for which report is created.
    * \param[in] studyNames Names of studies.
    * \param[in] outputDir name of the output directory.
    */
   void Create(
         const vector<BmnHistManager*>& histManagers,
         const vector<string>& studyNames,
         const string& outputDir);

   /**
    * \brief Main function which creates report data.
    *
    * Same pattern is used here.
    *
    * \param[in] fileNames Array of file names for which report is created.
    * \param[in] studyNames Names of studies.
    * \param[in] outputDir name of the output directory.
    */
   void Create(
         const vector<string>& fileNames,
         const vector<string>& studyNames,
         const string& outputDir);

   /* Accessors */
   const vector<BmnHistManager*>& HM() const { return fHM; }
   BmnHistManager* HM(Int_t index) const { return fHM[index]; }
   const vector<string>& GetStudyNames() const { return fStudyNames; }
   const string& GetStudyName(Int_t index) const { return fStudyNames[index]; }

private:
   vector<BmnHistManager*> fHM; // Histogram managers for each study
   vector<string> fStudyNames; // Names of studies

   ClassDef(BmnStudyReport, 1)
};

#endif /* BMNSTUDYREPORT_H_ */
