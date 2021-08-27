// -------------------------------------------------------------------------
// -----                    CbmStsDigiPar header file                  -----
// -----                 Created 28/06/05  by V. Friese                -----
// -------------------------------------------------------------------------


/** CbmStsDigiPar.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Parameter set for digitisation of the STS detector system.
 ** It holds an array of CbmStsStationPar containing the information
 ** for each STS station.
 **/



#ifndef CBMSTSDIGIPAR_H
#define CBMSTSDIGIPAR_H


#include "FairParSet.h"

#include "TObject.h"
#include "TObjArray.h"

#include <fstream>
#include <map>

using namespace std;


class TString;
class FairParIo;
class CbmStsStationDigiPar;


class CbmStsDigiPar : public FairParSet 
{

 public:

  /** Standard constructor **/
  CbmStsDigiPar(const char* name    = "StsDigiPar",
		const char* title   = "STS digitisation parameters",
		const char* context = "Default");


  /** Destructor **/
  virtual ~CbmStsDigiPar();


  /** Initialisation from input device**/
  virtual Bool_t init(FairParIo* input);


  /** Output to file **/
  virtual Int_t write(FairParIo* output);


  /** Reset all parameters **/
  virtual void clear();


  /** Read one line (station) from ASCII input **/
  void readline(const char* buffer, Int_t* set, fstream* f);


  /** Put an Ascii header for Ascii file output  **/
  void putAsciiHeader(TString&);


  /** Accessor to number of stations **/
  Int_t GetNStations() { return fStations->GetEntries(); }


  /** Accessor to station array **/
  TObjArray* GetStations() { return fStations; }


  /** Accessor to station parameters (by index) **/
  CbmStsStationDigiPar* GetStation(Int_t iStation);

  
  /** Accessor to station parameters (by station ID) **/
  CbmStsStationDigiPar* GetStationByNr(Int_t stationId);
  


 private:

  TObjArray* fStations;           // Array of CbmStsStationPar

  map<Int_t, Int_t> fStationMap;  //! Map from station ID to index

  CbmStsDigiPar(const CbmStsDigiPar&);
  CbmStsDigiPar operator=(const CbmStsDigiPar&);

  ClassDef(CbmStsDigiPar,1);
};


#endif
