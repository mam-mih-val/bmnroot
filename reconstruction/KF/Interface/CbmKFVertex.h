#ifndef CBMKFVERTEX_H
#define CBMKFVERTEX_H

#include "CbmKFVertexInterface.h"

class CbmKFVertex :public CbmKFVertexInterface {

 private:

  Double_t fX, fY, fZ, fC[6], fChi2;
  Int_t  fNDF, fNTracks;

 public:  

  CbmKFVertex(): fX(0), fY(0), fZ(0), fChi2(0), fNDF(0), fNTracks(0) { for( int i = 0; i < 6; ++i ) fC[i] = 0; };
  CbmKFVertex(CbmVertex &v): fX(0), fY(0), fZ(0), fChi2(0), fNDF(0), fNTracks(0){ SetVertex(v); }
  ~CbmKFVertex(){}

  Double_t &GetRefX(){ return fX; } 
  Double_t &GetRefY(){ return fY; }  
  Double_t &GetRefZ(){ return fZ; }  
  Double_t *GetCovMatrix(){ return fC; }
  Double_t &GetRefChi2(){ return fChi2; }  
  Int_t    &GetRefNDF(){ return fNDF; }
  Int_t    &GetRefNTracks(){ return fNTracks; }

  ClassDef( CbmKFVertex, 1 )
};

#endif
