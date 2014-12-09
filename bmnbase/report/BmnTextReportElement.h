/**
 * \file BmnTextReportElement.h
 * \brief Implementation of BmnLitReportElement for text output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef BMNTEXTREPORTELEMENT_H_
#define BMNTEXTREPORTELEMENT_H_

#include "BmnReportElement.h"
#include "TObject.h"
#include <string>

using std::string;
using std::vector;

/**
 * \class BmnTextReportElement
 * \brief Implementation of BmnLitReportElement for text output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class BmnTextReportElement: public BmnReportElement
{
public:
   /**
    * \brief Constructor.
    */
   BmnTextReportElement();

   /**
    * \brief Destructor.
    */
   virtual ~BmnTextReportElement();

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
         Int_t nofCols,
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
         Int_t size,
         const string& title) const;

private:
   string FormatCell(
         const string& cell) const;

   Int_t fColW; // column width

   ClassDef(BmnTextReportElement, 1)
};

#endif /* BmnTEXTREPORTELEMENT_H_ */
