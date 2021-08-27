// -------------------------------------------------------------------------
// -----                 CbmStsStationDigiPar header file              -----
// -----                  Created 27/06/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsStationDigiPar.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Parameter container for the digitisation of a STS strip station.
 ** Holds and manages an array of CbmStsSectorDigiPar objects.
 **/


#ifndef CBMSTSSTATIONDIGIPAR_H
#define CBMSTSSTATIONDIGIPAR_H 1


#include "CbmStsSectorDigiPar.h"

#include "TObjArray.h"


class CbmStsStationDigiPar : public TObject
{

 public:

  /** Default constructor **/
  CbmStsStationDigiPar();


  /** Standard constructor
   *@param  iStation    number of station
   *@param  rotation    Rotation w.r.t. global system [degrees]
   **/
  CbmStsStationDigiPar(Int_t iStation, Double_t rotation);


  /** Destructor */
  virtual ~CbmStsStationDigiPar();


  /** Accessors **/
  Int_t      GetStationNr()   { return fStationNr; }
  Double_t   GetRotation()    { return fRotation; }
  Int_t      GetNSectors()    { return fSectors->GetEntries(); }
  TObjArray* GetSectorArray() { return fSectors; }
  CbmStsSectorDigiPar* GetSector(Int_t iSector) {
    return (CbmStsSectorDigiPar*) fSectors->At(iSector); 
  }


  /** Add parameters of one sector **/
  void AddSector(CbmStsSectorDigiPar* sec) { fSectors->Add(sec); }



 private:

  Int_t      fStationNr;     //  Station identifier
  Double_t   fRotation;      //  Rotation angle in global c.s. [rad]
  TObjArray* fSectors;       //  Array of sector parameters

  CbmStsStationDigiPar(const CbmStsStationDigiPar&);
  CbmStsStationDigiPar operator=(const CbmStsStationDigiPar&);

  ClassDef(CbmStsStationDigiPar,1);

};


#endif
