#ifndef MPDRAINGENERATOR_H
#define MPDRAINGENERATOR_H


#include "FairGenerator.h"

class FairPrimaryGenerator;



class MpdRainGenerator : public FairGenerator
{
private:    
    Int_t         _PDGType;     // Particle type (PDG encoding)
    Int_t         _N;                 // Size of shower
    Double_t _p;                  // Momentum
    Double_t _theta;            // angle from z axis
    Double_t _phi;               // angle in XoY plain
    Double_t _height;         // Height at which rain starts
    Double_t _d;                 // distance between tracks
    

public:
  MpdRainGenerator();

  // pdgid - Particle ID
  // p - momentum of particle
  // theta, phi - angles of rain (particles)
  // d - distance between particles
  // n - (2*n)^2 number of particles
  MpdRainGenerator(Int_t pdgid, Double_t p, Double_t theta, Double_t phi, Double_t d, Double_t height, Int_t n);
  virtual ~MpdRainGenerator() {};

  // Create an Event
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

    ClassDef(MpdRainGenerator,1);

};


#endif
   

