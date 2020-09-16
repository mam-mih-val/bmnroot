/** BmnVertex.h
 ** @author V.Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @author S. Merts
 **
 ** Data class for a vertex in BM@N/SRC.
 ** Data level: RECO
 **/

#ifndef BMNVERTEX_H
#define BMNVERTEX_H 1

#include <vector>

#include "TMatrixFSym.h"
#include "TObject.h"
#include "TVector3.h"

using namespace std;

class BmnVertex : public TObject {
   public:
    /** Default constructor  **/
    BmnVertex();

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
   *@param flag      vertex type
   *@param idxs      Vector of indexes of tracks used to reconstruct current vertex
   **/
    BmnVertex(Double_t x, Double_t y, Double_t z, Double_t chi2,
              Int_t ndf, Int_t nTracks, const TMatrixFSym& covMat, Int_t flag, vector<Int_t> idxs);

    /** Destructor **/
    virtual ~BmnVertex();

    /** Ouput to screen **/
    void Print();

    /** Accessors **/
    Double_t GetX() const { return fX; };           // x position [cm]
    Double_t GetY() const { return fY; };           // y position [cm]
    Double_t GetZ() const { return fZ; };           // z posiiton [cm]
    Double_t GetChi2() const { return fChi2; };     // chi2
    Int_t GetNDF() const { return fNDF; };          // nof degrees of freedom
    Int_t GetNTracks() const { return fNTracks; };  // nof tracks used
    void Position(TVector3& pos) const { pos.SetXYZ(fX, fY, fZ); };
    void CovMatrix(TMatrixFSym& covMat) const;
    Double_t GetCovariance(Int_t i, Int_t j) const;
    std::vector<Int_t>& GetTrackIdxs() { return fTrackIdxs; }
    Int_t GetFlag() { return fFlag; }

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
   *@param flag      vertex type
   *@param idxs      Vector of indexes of tracks used to reconstruct current vertex
   **/
    void SetVertex(Double_t x, Double_t y, Double_t z, Double_t chi2,
                   Int_t ndf, Int_t nTracks, const TMatrixFSym& covMat, Int_t flag, vector<Int_t> idxs);

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

    /** Vector of indexes of tracks used to reconstruct current vertex **/
    vector<Int_t> fTrackIdxs;

    /** Flag to mark vertexes by types
     *  for SRC setup next types are used:
     * -1 - default value, vertex not defined
     * 10 - vertex reconstructed by left arm track + right arm track + global tracks with upstream part
     * 11 - vertex reconstructed by left arm track + right arm track
     * 12 - vertex reconstructed by left arm track + global tracks with upstream part
     * 13 - vertex reconstructed by right arm track + global tracks with upstream part**/
    Int_t fFlag;

    ClassDef(BmnVertex, 1);
};

#endif
