/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** \file BmnScWallDigi.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 09.10.2019
 **/

/** \class BmnScWallDigi
 ** \brief Data class for BmnScWall digital signal processing
 ** \version 1.0
 **/

#ifndef BmnScWallDigi_H
#define BmnScWallDigi_H 1

#include "TObject.h"
#include "BmnDetectorList.h"  // for kSCWALL
#include "BmnScWallAddress.h"  // for BmnScWallAddress

#include <Rtypes.h>      // for THashConsistencyHolder, ClassDefNV
#include <RtypesCore.h>  // for Double_t, UInt_t, Int_t

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

#include <string>  // for string


class BmnScWallDigi : public TObject {

public:
  /**@brief Default constructor.
       **/
  BmnScWallDigi();


  /** @brief Constructor with detailed assignment.
       **/
  BmnScWallDigi(UInt_t address, Double_t signal, Double_t timestamp, 
                Double_t ampl, Double_t zl, Double_t integral, Double_t time_max,
                Double_t fit_ampl, Double_t fit_zl, Double_t fit_integral, Double_t fit_R2, Double_t fit_time_max);


  /**  Copy constructor **/
  BmnScWallDigi(const BmnScWallDigi&);


  /** Move constructor  **/
  BmnScWallDigi(BmnScWallDigi&&);


  /** Assignment operator  **/
  BmnScWallDigi& operator=(const BmnScWallDigi&);


  /** Move Assignment operator  **/
  BmnScWallDigi& operator=(BmnScWallDigi&&);


  /** Destructor **/
  ~BmnScWallDigi();


  /** @brief Class name (static)
       ** @return BmnScWallDigi
       **/
  static const char* GetClassName() { return "BmnScWallDigi"; }


  /** @brief Address
       ** @return Unique channel address (see BmnScWallAddress)
       **/
  UInt_t GetAddress() const { return fuAddress; };


  /** @brief calibrated Signal
       ** @return calibrated Signal
       **/
  Double_t GetSignal() const { return fSignal; };


  /** @brief Signal timestamp
       ** @return Signal timestamp
       **/
  Double_t GetTimestamp() const { return fTimestamp; };


  /** @brief X position
       ** @return X position
       **/
  Double_t GetX() const { return BmnScWallAddress::GetXIdx(GetAddress()); }


  /** @brief Y position
       ** @return Y position
       **/
  Double_t GetY() const { return BmnScWallAddress::GetYIdx(GetAddress()); }


  /** @brief System identifier
       ** @return System ID 
       **/
  static Int_t GetSystemId() { return kSCWALL; }


  /** Modifiers **/
  void SetAddress(UInt_t address) { fuAddress = address; };

  void reset();

  UInt_t fuAddress       = 0;   /// Unique channel address
  Double_t fSignal       = 0.;  /// Signal [MeV]
  Double_t fTimestamp    = -1.; /// Signal timestamp

  Double_t fAmpl         = 0.;  /// Amplitude from waveform [adc counts]
  Double_t fZL           = 0.;  /// ZeroLevel from waveform [adc counts]
  Double_t fIntegral     = 0.;  /// Energy deposition from waveform [adc counts]
  Double_t fTimeMax      = 0.;  /// Time of maximum in waveform [adc samples]

  Double_t fFitAmpl      = 0.;  /// Amplitude from fit of waveform [adc counts]
  Double_t fFitZL        = 0.;  /// ZeroLevel from fit of waveform [adc counts]
  Double_t fFitIntegral  = 0.;  /// Energy deposition from fit of waveform [adc counts]
  Double_t fFitR2        = 2.;  /// Quality of waveform fit [] -- good near 0
  Double_t fFitTimeMax   = -1.; /// Time of maximum in fit of waveform [adc samples]

  template<class Archive>
  void serialize(Archive& ar, const unsigned int /*version*/)
  {
    ar& fuAddress;
    ar& fSignal;
    ar& fTimestamp;

    ar& fAmpl;
    ar& fZL;
    ar& fIntegral;
    ar& fTimeMax;

    ar& fFitAmpl;
    ar& fFitZL;
    ar& fFitIntegral;
    ar& fFitR2;
    ar& fFitTimeMax;

  }

private:
  /// BOOST serialization interface
  friend class boost::serialization::access;


  ClassDefNV(BmnScWallDigi, 1);
};

#endif  // BmnScWallDigi_H
