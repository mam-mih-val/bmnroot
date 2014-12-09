/**
 * \file CbmLitResultChecker.h
 * \brief Automatic checker of QA results.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITRESULTCHECKER_H_
#define CBMLITRESULTCHECKER_H_

#include "TObject.h"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <map>
using boost::property_tree::ptree;
using std::string;
using std::map;

/**
 * \class CbmLitResultChecker
 * \brief Automatic checker of QA results.
 *
 * Checking procedure compares two property trees:
 * "quality" property tree obtained from QA procedure vs.
 * "ideal" property tree containing predefined limits for each value.
 * Results are stored in the third property tree as "0" - "1"
 * if value was compared to ideal result
 * or "-1" if value was not compared.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitResultChecker
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitResultChecker();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitResultChecker();

   /**
    * \brief Check QA results based on predefined values.
    * \param[in] qaFile File name of QA results.
    * \param[in] idealFile File name of predefined values.
    * \param[out] checkFile File name for output results.
    */
   void DoCheck(
         const string& qaFile,
         const string& idealFile,
         const string& checkFile);

   /**
    * \brief Check reconstruction quality results.
    * \param[in] qa Reconstruction quality results stored as property tree.
    * \param[in] ideal Ideal reconstruction quality results as property tree.
    * \param[out] out Output property tree.
    */
   void DoCheck(
         const ptree& qa,
         const ptree& ideal,
         ptree& out);

private:
   /**
    * \brief Build recursively map out of property tree.
    * \param[in] path Initial path. Must be "".
    * \param[in] pt Input property tree.
    * \param[out] mymap Output map<"property name", "value">.
    */
   void PropertyTreeToMap(
         const string& path,
         const ptree& pt,
         map<string, Double_t>& mymap) const;
};

#endif /* CBMLITRESULTCHECKER_H_ */
