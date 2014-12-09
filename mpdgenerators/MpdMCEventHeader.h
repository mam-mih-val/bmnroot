/** MpdMCEventHeader.h
 ** @author V.Voronyuk, origin CBMRoot
 ** Data class (level MC) containing information about the input event.
 ** New MPD class derived from FairMCEventHeader
 **/

#ifndef MPDMCEVENTHEADER_H
#define MPDMCEVENTHEADER_H 1

#include "FairMCEventHeader.h"

class MpdMCEventHeader : public FairMCEventHeader
{

 public:

  /** Default constructor **/
  MpdMCEventHeader();

  /** Constructor with all members
   **
   *@param runId    run identifier
   *@param iEvent   event identifier
   *@param x,y,z    vertex oordinates [cm]
   *@param t        event time [ns]
   *@param b        impact parameter [fm] (if relevant)
   *@param phi      event plane angle [rad]
   *@param nPrim    number of input tracks
   **/
  MpdMCEventHeader(UInt_t runId, Int_t iEvent, 
      Double_t x, Double_t y, Double_t z, Double_t t, 
      Double_t b, Double_t phi, Int_t nPrim);

  /** Standard constructor with run identifier **/
  MpdMCEventHeader(UInt_t runId);

  /** Destructor **/
  virtual ~MpdMCEventHeader();

  /** Accessors **/
  Double_t GetPhi()   const { return fPhi; }       // event plane angle [rad]

  /** Modifiers **/
  void SetPhi(Double_t phi) { fPhi = phi;  }

  /** Reset all members **/
  virtual void Reset();

  /** Register the class as data branch to the poutput */
  virtual void Register();

  ClassDef(MpdMCEventHeader,1);
 
 private:

  Double32_t fPhi;         //  Event plane angle [rad] (if relevant)
};

#endif
