#include "MpdRainGenerator.h"
#include <TMath.h>
#include "FairPrimaryGenerator.h"

#include <iostream>

MpdRainGenerator::MpdRainGenerator(): 
_PDGType(-1), _N(0), _p(0), _theta(0), _phi(0), _d(-1), _height(0)
{
}

MpdRainGenerator::MpdRainGenerator(Int_t pdgid, Double_t p, Double_t theta, Double_t phi, 
								Double_t d, Double_t height, Int_t n):
_PDGType(pdgid), _N(n), _p(p), _theta(theta), _phi(phi), _height(height), _d(d)
{
}

Bool_t MpdRainGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

  // Check for particle type
  if ( _PDGType == -1 ) 
    Fatal("MpdRainGenerator","PDG code not defined.");

  for(int i = -_N; i < _N; i++)
    for(int j = -_N; j < _N; j++)
    {
      TVector3 point(i*_d, j*_d, _height);
      point.RotateY(_theta);
      point.RotateZ(_phi);

      TVector3 m;
      m.SetMagThetaPhi(_p, _theta, _phi);
      m = -m;
      
      primGen->AddTrack(_PDGType, m.X(), m.Y(), m.Z(), point.X(), point.Y(), point.Z() );
    }

  return kTRUE;

}


ClassImp(MpdRainGenerator)

