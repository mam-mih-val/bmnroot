/* 
 * File:   BmnGemDchTrack.h
 * Author: plot
 */

#ifndef BMNGEMDCHTRACK_H
#define BMNGEMDCHTRACK_H

#include "TObject.h"
#include "CbmStsTrack.h"
#include "BmnTrack.h"

class BmnGemDchTrack : public  TObject  {

private:
  CbmStsTrack fGemTrack; // GEM+CSC track
  BmnTrack fDchTrack;     // DCH track
  Double_t fLength;
  Int_t fEventId, fEventHits, fFlag, fGemHits, fSiHits, fCSCHits;
  Double_t fDx, fDy, fDtx, fDty, fDdx, fDdy, fDdtx, fDdty, fChi2;
  Double_t fXpv, fYpv, fZpv;
  Double_t fTrackAngDch, fTrackAngPV, fIntMagFieldTr, fIntMagFieldDch, fTrackAng360, fXSigma, fYSigma ;
public:
  //constructor
  BmnGemDchTrack();
  //destructor
  virtual ~BmnGemDchTrack();

  //accessors
  CbmStsTrack* GetGemTrack() { return &fGemTrack; }
  BmnTrack* GetDchTrack() { return &fDchTrack; }
  
  Double_t GetLength()   const { return fLength; }
  Int_t GetEventId() const { return fEventId; }
  Int_t GetEventHits() const { return fEventHits; }
  Int_t GetGemHits() const { return fGemHits; }
  Int_t GetSiHits() const { return fSiHits; }
  Int_t GetCSCHits() const { return fCSCHits; }
  Double_t GetDx() const { return fDx; }
  Double_t GetDy() const { return fDy; }
  Double_t GetDtx() const { return fDtx; }
  Double_t GetDty() const { return fDty; }
  Double_t GetDdx() const { return fDdx; }
  Double_t GetDdy() const { return fDdy; }
  Double_t GetDdtx() const { return fDdtx; }
  Double_t GetDdty() const { return fDdty; }
  Double_t GetChi2() const { return fChi2; }
  Double_t GetXpv() const { return fXpv; }
  Double_t GetYpv() const { return fYpv; }
  Double_t GetZpv() const { return fZpv; }
  Double_t GetTrackAngDch() const { return fTrackAngDch; }
  Double_t GetTrackAngPV() const { return fTrackAngPV; }
  Double_t GetIntMagFieldTr() const { return fIntMagFieldTr; }
  Double_t GetIntMagFieldDch() const { return fIntMagFieldDch; }
  Double_t GetXSigma() const { return fXSigma; }
  Double_t GetYSigma() const { return fYSigma; }

  Int_t GetFlag() const {
        return fFlag;
    }
  Double_t GetDistance();

  //modifiers
  void SetGemTrack(CbmStsTrack& track)   {fGemTrack = track;}
  void SetDchTrack(BmnTrack& track)      {fDchTrack = track;}
  
  void SetLength(Double_t length)        {fLength = length;}
  void SetEventId(Int_t eventId) {fEventId = eventId;}  
  void SetEventHits(Int_t eventHits) {fEventHits = eventHits;}
  void SetGemHits(Int_t gemHits) {fGemHits = gemHits;}
  void SetSiHits(Int_t siHits) {fSiHits = siHits;}
  void SetCSCHits(Int_t cscHits) {fCSCHits = cscHits;}
  void SetDx(Double_t dx) {fDx = dx;}
  void SetDy(Double_t dy) {fDy = dy;}
  void SetDtx(Double_t dtx) {fDtx = dtx;}
  void SetDty(Double_t dty) {fDty = dty;}
  void SetDdx(Double_t ddx) {fDdx = ddx;}
  void SetDdy(Double_t ddy) {fDdy = ddy;}
  void SetDdtx(Double_t ddtx) {fDdtx = ddtx;}
  void SetDdty(Double_t ddty) {fDdty = ddty;}
  void SetChi2(Double_t chi2) {fChi2 = chi2;}
  void SetXpv(Double_t xpv) {fXpv = xpv;}
  void SetYpv(Double_t ypv) {fYpv = ypv;}
  void SetZpv(Double_t zpv) {fZpv = zpv;}
  void SetTrackAngDch(Double_t trackAngDch) {fTrackAngDch = trackAngDch;}
  void SetTrackAngPV(Double_t trackAngPV) {fTrackAngPV = trackAngPV;}
  void SetIntMagFieldTr(Double_t intMagFieldTr) {fIntMagFieldTr = intMagFieldTr;}
  void SetIntMagFieldDch(Double_t intMagFieldDch) {fIntMagFieldDch = intMagFieldDch;}
  void SetFlag(Int_t flag) {
        fFlag = flag;
    }
  void SetXSigma(Double_t xSigma) {fXSigma = xSigma;}
  void SetYSigma(Double_t ySigma) {fYSigma = ySigma;}


  ClassDef(BmnGemDchTrack, 1);
};

#endif /* BMNGEMDCHTRACK_H */

