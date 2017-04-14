/**
 * \file BmnHtmlReportElement.h
 * \brief Implementation of BmnReportElement for HTML output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef BMNHTMLREPORTELEMENT_H_
#define BMNHTMLREPORTELEMENT_H_

#include "BmnReportElement.h"
#include <string>

using std::string;
using std::vector;

/**
 * \class BmnHtmlReportElement
 * \brief Implementation of BmnReportElement for text output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class BmnHtmlReportElement: public BmnReportElement
{
public:
   /**
    * \brief Constructor.
    */
   BmnHtmlReportElement();

   /**
    * \brief Destructor.
    */
   virtual ~BmnHtmlReportElement();

   /**
    * \brief Inherited from BmnReportElement.
    */
   virtual string TableBegin(
         const string& caption,
         const vector<string>& colNames) const;

   /**
    * \brief Inherited from BmnReportElement.
    */
   virtual string TableEnd() const;

   /**
    * \brief Inherited from BmnReportElement.
    */
   virtual string TableEmptyRow(
         int nofCols,
         const string& name) const;

   /**
    * \brief Inherited from BmnReportElement.
    */
   virtual string TableRow(
         const vector<string>& row) const;

   /**
    * \brief Inherited from BmnReportElement.
    */
   virtual string Image(
         const string& title,
         const string& file) const;

   /**
    * \brief Inherited from BmnReportElement.
    */
   virtual string DocumentBegin() const;

   /**
    * \brief Inherited from BmnReportElement.
    */
   virtual string DocumentEnd() const;

   /**
    * \brief Inherited from BmnReportElement.
    */
   virtual string Title(
         int size,
         const string& title) const;

   ClassDef(BmnHtmlReportElement, 1)
};

#endif /* BmnHTMLREPORTELEMENT_H_ */
