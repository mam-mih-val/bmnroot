/**
 * \file BmnMCPoint.h
 * \brief Monte-Carlo point.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#ifndef BMNMCPOINT_H_
#define BMNMCPOINT_H_

#include "TObject.h"
#include <string>
#include <sstream>
#include <cmath>

/**
 * \class BmnMCPoint
 * \brief Monte-Carlo point.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
class BmnMCPoint {
public:
   /*
    * \brief Constructor.
    */
   BmnMCPoint():
      fXIn(0.), fYIn(0.), fZIn(0.),
      fPxIn(0.), fPyIn(0.), fPzIn(0.),
      fXOut(0.), fYOut(0.), fZOut(0.),
      fPxOut(0.), fPyOut(0.), fPzOut(0.),
      fRefId(-1), fStationId(-1), fQ(1.) {}

   /*
    * \brief Destructor.
    */
   virtual ~BmnMCPoint() {}

   /* Getters */
   Double_t GetX() const { return (fXIn + fXOut) / 2.; }
   Double_t GetY() const { return (fYIn + fYOut) / 2.; }
   Double_t GetZ() const { return (fZIn + fZOut) / 2.; }
   Double_t GetPx() const { return (fPxIn + fPxOut) / 2.; }
   Double_t GetPy() const { return (fPyIn + fPyOut) / 2.; }
   Double_t GetPz() const { return (fPzIn + fPzOut) / 2.; }
   Double_t GetTx() const { return GetPx() / GetPz(); }
   Double_t GetTy() const { return GetPy() / GetPz(); }
   Double_t GetP() const { return std::sqrt(GetPx() * GetPx() + GetPy() * GetPy() + GetPz() * GetPz()); }
   Double_t GetQp() const {
      Double_t p = GetP();
      Double_t res = (p != 0.) ? fQ / p : 0.;
      return res;
   }
   Double_t GetQ() const { return fQ; }
   Double_t GetXIn() const { return fXIn; }
   Double_t GetYIn() const { return fYIn; }
   Double_t GetZIn() const { return fZIn; }
   Double_t GetPxIn() const { return fPxIn; }
   Double_t GetPyIn() const { return fPyIn; }
   Double_t GetPzIn() const { return fPzIn; }
   Double_t GetTxIn() const { return fPxIn / fPzIn; }
   Double_t GetTyIn() const { return fPyIn / fPzIn; }
   Double_t GetPIn() const { return std::sqrt(fPxIn * fPxIn + fPyIn * fPyIn + fPzIn * fPzIn); }
   Double_t GetQpIn() const {
      Double_t p = GetPIn();
      Double_t res = (p != 0.) ? fQ / p : 0.;
      return res;
   }
   Double_t GetXOut() const { return fXOut; }
   Double_t GetYOut() const { return fYOut; }
   Double_t GetZOut() const { return fZOut; }
   Double_t GetPxOut() const { return fPxOut; }
   Double_t GetPyOut() const { return fPyOut; }
   Double_t GetPzOut() const { return fPzOut; }
   Double_t GetTxOut() const { return fPxOut / fPzOut; }
   Double_t GetTyOut() const { return fPyOut / fPzOut; }
   Double_t GetPOut() const { return std::sqrt(fPxOut * fPxOut + fPyOut * fPyOut + fPzOut * fPzOut); }
   Double_t GetQpOut() const {
      Double_t p = GetPOut();
      Double_t res = (p != 0.) ? fQ / p : 0.;
      return res;
   }
   Int_t GetRefId() const { return fRefId; }
   Int_t GetStationId() const { return fStationId; }



   /* Setters */
   void SetXIn(Double_t x) { fXIn = x; }
   void SetYIn(Double_t y) { fYIn = y; }
   void SetZIn(Double_t z) { fZIn = z; }
   void SetPxIn(Double_t px) { fPxIn = px; }
   void SetPyIn(Double_t py) { fPyIn = py; }
   void SetPzIn(Double_t pz) { fPzIn = pz; }
   void SetXOut(Double_t x) { fXOut = x; }
   void SetYOut(Double_t y) { fYOut = y; }
   void SetZOut(Double_t z) { fZOut = z; }
   void SetPxOut(Double_t px) { fPxOut = px; }
   void SetPyOut(Double_t py) { fPyOut = py; }
   void SetPzOut(Double_t pz) { fPzOut = pz; }
   void SetQ(Double_t q) { fQ = q; }
   void SetRefId(Int_t refId) { fRefId = refId; }
   void SetStationId(Int_t stationId) { fStationId = stationId; }

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "MCPoint: pos=(" << GetX() << "," << GetY() << "," << GetZ() << ") "
         << "mom=(" << GetPx() << "," << GetPy() << "," << GetPz() << ") "
         << "refId=" << GetRefId() << " stationId=" << GetStationId() << std::endl;
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const BmnMCPoint& point) {
      strm << point.ToString();
      return strm;
   }

private:
   Double_t fXIn, fYIn, fZIn; // Space coordinates on entrance plane [cm].
   Double_t fPxIn, fPyIn, fPzIn; // Momentum components on entrance plane. [GeV/c]
   Double_t fXOut, fYOut, fZOut; // Space coordinates on exit plane [cm].
   Double_t fPxOut, fPyOut, fPzOut; // Momentum components on exit plane. [GeV/c]
   Int_t fRefId; // Reference index.
   Int_t fStationId; // Station index.
   Double_t fQ; // +/-1 - negative or positive charge
};

#endif /* BmnMCPOINT_H_ */
