/* 
 * File:   BmnIdentifiableTrack.h
 * Author: plot
 *
 * Created on September 28, 2017, 4:24 PM
 */

#ifndef BMNIDENTIFIABLETRACK_H
#define BMNIDENTIFIABLETRACK_H

#include "TObject.h"

class BmnIdentifiableTrack : public  TObject  {

private:
  Double_t fMassQ, fBeta, fLength, fPq, fTime;
  Double_t fTOF700Amplitude;
  Int_t fTOF700Plane, fTOF700Strip, fTOF700StripHits;

public:
  //constructor
  BmnIdentifiableTrack();
  //destructor
  virtual ~BmnIdentifiableTrack();

  //accessors
  Double_t GetMassQ()    const { return fMassQ; }
  Double_t GetBeta()     const { return fBeta; }
  Double_t GetLength()   const { return fLength; }
  Double_t GetPq()       const { return fPq; }
  Double_t GetTime()     const { return fTime; }
  Int_t GetTOF700Plane() const { return fTOF700Plane; }
  Int_t GetTOF700Strip() const { return fTOF700Strip; }
  Double_t GetTOF700Amplitude() const { return fTOF700Amplitude; }
  Int_t GetTOF700StripHits() const { return fTOF700StripHits; }

  //modifiers
  void SetMassQ(Double_t massQ)          {fMassQ = massQ;}
  void SetBeta(Double_t beta)            {fBeta = beta;}
  void SetLength(Double_t length)        {fLength = length;}
  void SetPq(Double_t pq)                {fPq = pq;}
  void SetTime(Double_t time)            {fTime = time;}
  void SetTOF700Plane(Int_t tof700Plane) {fTOF700Plane = tof700Plane;}
  void SetTOF700Strip(Int_t tof700Strip) {fTOF700Strip = tof700Strip;}
  void SetTOF700Amplitude(Double_t tof700Amplitude) {fTOF700Amplitude = tof700Amplitude;}
  void SetTOF700StripHits(Int_t tof700Strip) {fTOF700Strip = tof700Strip;}

  ClassDef(BmnIdentifiableTrack, 1);
};

#endif /* BMNIDENTIFIABLETRACK_H */

