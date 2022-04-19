//* $Id: *//

// -------------------------------------------------------------------------
// -----                    CbmStsDigiMatch header file                -----
// -----                  Created 31/08/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsDigiMatch.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 31.08.06
 **@version 1.0
 **
 ** Data class for matching CbmStsDigi with CbmStsPoint. Up to three
 ** matches are possible.
 ** The indizes of CbmStsDigi and CbmStsDigiMatch correspond in their
 ** respective arrays.
 ** The class holds the indizes of all StsPoints corresponding to
 ** the StsDigi.
 **/


#ifndef CBMSTSDIGIMATCH_H
#define CBMSTSDIGIMATCH_H 1


#include "TObject.h"


class CbmStsDigiMatch : public TObject
{

 public:

  /** Default constructor **/
  CbmStsDigiMatch();

  /** Standard constructor **/
  CbmStsDigiMatch(Int_t iPoint);

  /** Destructor **/
  virtual ~CbmStsDigiMatch();

  /** Add a StsPoint index
   *@value Number of indizes present **/
  Int_t AddPoint(Int_t iPoint);

  /** Accessor to StsPoint indizes **/
  Int_t GetRefIndex(Int_t i = 0) const;

 private:

  Int_t fRefIndex[3];

  ClassDef(CbmStsDigiMatch,1);

};

#endif


  
