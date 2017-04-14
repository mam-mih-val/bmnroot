/**
 * \file BmnLatexReportElement.h
 * \brief Implementation of BmnReportElement for Latex output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef BMNLATEXREPORTELEMENT_H_
#define BMNLATEXREPORTELEMENT_H_

#include "BmnReportElement.h"
#include <string>

using std::string;
using std::vector;

/**
 * \class BmnLatexReportElement
 * \brief Implementation of BmnLitReportElement for Latex output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class BmnLatexReportElement: public BmnReportElement
{
public:
   /**
    * \brief Constructor.
    */
   BmnLatexReportElement();

   /**
    * \brief Destructor.
    */
   virtual ~BmnLatexReportElement();

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

   ClassDef(BmnLatexReportElement, 1)
};

#endif /* BmnLATEXREPORTELEMENT_H_ */
