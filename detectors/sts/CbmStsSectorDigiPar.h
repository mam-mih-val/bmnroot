// -------------------------------------------------------------------------
// -----                 CbmStsSectorDigiPar header file               -----
// -----                 Created 27/06/05  by V. Friese                -----
// -------------------------------------------------------------------------


/** CbmStsSectorDigiPar.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Parameter container for the digitisation of a sector of a STS
 ** strip station.
 ** Supports the following digitisation types:
 ** 1 = hybrid pixel
 ** 2 = strip, readout of all strips
 ** 3 = strip, readout at bottom corner, double metal layer
 ** Coordinates and rot. angle are in the global coordinate system. 
 ** In the internal coordinate system, x is across the front side strips
 ** and y along the front side strips. Irrelevant for pixel detectors.
 ** Angles are stored and accessed in radians, but must be given in
 ** degrees in the constructor.
 **/



#ifndef CBMSTSSECTORDIGIPAR_H
#define CBMSTSSECTORDIGIPAR_H 1


#include "CbmStsSensorDigiPar.h"

#include "TObjArray.h"



class CbmStsSectorDigiPar : public TObject
{

 public:

  /** Default constructor **/
  CbmStsSectorDigiPar();


  /** Standard constructor. For the parameters, see description of
   ** private data members. Angles must be given in radians. **/
  CbmStsSectorDigiPar(Int_t iSector);


  /** Destructor **/
  virtual ~CbmStsSectorDigiPar();


  /** Accessors. See private data members for description **/
  Int_t    GetSectorNr()   const { return fSectorNr; }

  /** Accessors **/
  Int_t      GetSectorNr()    { return fSectorNr; }
  Int_t      GetNSensors()    { return fSensors->GetEntries(); }
  TObjArray* GetSensorArray() { return fSensors; }
  CbmStsSensorDigiPar* GetSensor(Int_t iSensor) {
    return (CbmStsSensorDigiPar*) fSensors->At(iSensor); 
  }


  /** Add parameters of one sensor **/
  void AddSensor(CbmStsSensorDigiPar* sen) { fSensors->Add(sen); }


 private:

  /** Sector number in station **/	
  Int_t      fSectorNr;
  TObjArray* fSensors;       //  Array of sensor parameters

  CbmStsSectorDigiPar(const CbmStsSectorDigiPar&);
  CbmStsSectorDigiPar operator=(const CbmStsSectorDigiPar&);

  ClassDef(CbmStsSectorDigiPar,1);

};


#endif
