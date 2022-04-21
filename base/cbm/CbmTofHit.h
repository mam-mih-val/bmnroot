/**
 * \file CbmTofHit.h
 * \author E. Cordier
 * Modified by D. Gonzalez-Diaz 06/09/06
 * Modified by A.Lebedev 26/05/09
 * Modified by nh 16/12/12
 * Modified by A.Lebedev 15/05/13
**/

#ifndef CBMTOFHIT_H_
#define CBMTOFHIT_H_

#include "BmnHit.h"
#include "CbmBaseHit.h"

class CbmTofHit : public BmnHit
{
public:
  /**
   * \brief Default constructor.
   **/
  CbmTofHit();

  /**
   * \brief Constructor with hit parameters (1a).
   **/
  CbmTofHit(Int_t address, TVector3 pos, TVector3 dpos, Int_t refIndex, Double_t tof, Int_t flag, Int_t channel);

  /**
   * \brief Constructor with hit parameters (1).
   **/
  CbmTofHit(Int_t address, TVector3 pos, TVector3 dpos, Int_t refIndex, Double_t tof, Int_t flag);

  /**
   * \brief Constructor with hit parameters (2) [not the flag]
   **/
  CbmTofHit(Int_t address, TVector3 pos, TVector3 dpos, Int_t refIndex, Double_t tof);

  /**
   * \brief Destructor.
   **/
  virtual ~CbmTofHit();

  /**
   * \brief Inherited from CbmBaseHit.
   */
  virtual string ToString() const;

  /**
   * \brief Inherited from CbmBaseHit.
   */
  Int_t GetPlaneId() const { return 0; }

  Int_t GetRefId() const { return GetIndex(); }

  /** Accessors **/
  Double_t GetTime() const { return fTime; }
  Int_t GetCh() const { return fChannel; }

  /** Modifiers **/
  void SetTime(Double_t time) { fTime = time; };
  void SetType(HitType_o type) { fType = type; };

private:

  HitType_o fType; ///< Hit type
  Double32_t fTime; ///< Time since event start [ns]
  Int_t fChannel; ///< Channel identifier

  ClassDef(CbmTofHit, 3)
};

#endif
