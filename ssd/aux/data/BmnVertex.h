// -------------------------------------------------------------------------
// -----                      BmnVertex header file                    -----
// -----                  Created 13.12.2018  by D. Baranov            -----
// -------------------------------------------------------------------------


/** BmnVertex.h
 ** @author V.Friese <v.friese@gsi.de>
 ** @author D. Baranov
 **
 ** Data class for a vertex in BMN.
 ** Data level: RECO
 **/


#ifndef BMNVERTEX_H
#define BMNVERTEX_H 1


#include <string>
#include "TMatrixFSym.h"
#include "TNamed.h"
#include "TVector3.h"


class BmnVertex : public TNamed
{

 public:

  /** Default constructor  **/
  BmnVertex();


  /** Constructor with name and title **/
  BmnVertex(const char* name, const char* title);


  /** Constructor with all member variables
   *@param name      Name of object
   *@param title     Title of object
   *@param x         x coordinate [cm]
   *@param y         y coordinate [cm]
   *@param z         z coordinate [cm]
   *@param chi2      chi square of vertex fit
   *@param ndf       Number of degrees of freedom of vertex fit
   *@param nTracks   Number of tracks used for vertex fit
   *@param covMat    Covariance Matrix (symmetric, 3x3)
   **/
  BmnVertex(const char* name, const char* title,
	    Double_t x, Double_t y, Double_t z, Double_t chi2,
	    Int_t ndf, Int_t nTracks, const TMatrixFSym& covMat);


  /** Destructor **/
  virtual ~BmnVertex();


  /** Ouput to screen **/
  virtual void Print(Option_t* opt = "") const ;


  /** Accessors **/
  Double_t GetX()    const { return fX;       };  // x position [cm]
  Double_t GetY()    const { return fY;       };  // y position [cm]
  Double_t GetZ()    const { return fZ;       };  // z posiiton [cm]
  Double_t GetChi2() const { return fChi2;    };  // chi2
  Int_t GetNDF()     const { return fNDF;     };  // nof degrees of freedom
  Int_t GetNTracks() const { return fNTracks; };  // nof tracks used
  void Position(TVector3& pos) const { pos.SetXYZ(fX,fY,fZ); };
  void CovMatrix(TMatrixFSym& covMat) const;
  Double_t GetCovariance(Int_t i, Int_t j) const;


  /** Reset the member variables **/
  void Reset();


  /** Set the member variables
   *@param x         x coordinate [cm]
   *@param y         y coordinate [cm]
   *@param z         z coordinate [cm]
   *@param chi2      chi square of vertex fit
   *@param ndf       Number of degrees of freedom of vertex fit
   *@param nTracks   Number of tracks used for vertex fit
   *@param covMat    Covariance Matrix (symmetric, 3x3)
   **/
  void SetVertex(Double_t x, Double_t y, Double_t z, Double_t chi2,
		 Int_t ndf, Int_t nTracks, const TMatrixFSym& covMat);


  /** String output **/
  virtual std::string ToString() const;



 private:

  /** Position coordinates  [cm] **/
  Double32_t fX, fY, fZ;

  /** Chi2 of vertex fit **/
  Double32_t fChi2;

  /** Number of degrees of freedom of vertex fit **/
  Int_t fNDF;

  /** Number of tracks used for the vertex fit **/
  Int_t fNTracks;

  /** Covariance matrix for x, y, and z stored in an array. The
   ** sequence is a[0,0], a[0,1], a[0,2], a[1,1], a[1,2], a[2,2]
   **/
  Double32_t fCovMatrix[6];


  ClassDef(BmnVertex,1);

};


#endif
