/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** @file BmnScWallDigi.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 04.07.2021
 **
 ** Code for Data class for BmnScWall digital signal processing
 **/

#include "BmnScWallDigi.h"

#include <TBuffer.h>  // for TBuffer
#include <TClass.h>   // for BmnScWallDigi::IsA()
#include <TString.h>  // for Form, TString

#include <string>  // for basic_string

// --- Default constructor
BmnScWallDigi::BmnScWallDigi()
  : TObject()
  , fuAddress()
  , fSignal()
  , fTimestamp()

  , fAmpl()
  , fZL()
  , fIntegral()
  , fTimeMax()

  , fFitAmpl()
  , fFitZL()
  , fFitIntegral()
  , fFitR2()
  , fFitTimeMax()
{
}

// clang-format off
// --- Constructor with assignment
BmnScWallDigi::BmnScWallDigi(UInt_t address, Double_t signal, Double_t timestamp, 
                Double_t ampl, Double_t zl, Double_t integral, Double_t time_max,
                Double_t fit_ampl, Double_t fit_zl, Double_t fit_integral, Double_t fit_R2, Double_t fit_time_max)

  : TObject()
  , fuAddress(address)
  , fSignal(signal)
  , fTimestamp(timestamp)

  , fAmpl(ampl)
  , fZL(zl)
  , fIntegral(integral)
  , fTimeMax(time_max)

  , fFitAmpl(fit_ampl)
  , fFitZL(fit_zl)
  , fFitIntegral(fit_integral)
  , fFitR2(fit_R2)
  , fFitTimeMax(fit_time_max)
{
}
// clang-format on

// --- Copy constructor
BmnScWallDigi::BmnScWallDigi(const BmnScWallDigi& other)
  : fuAddress(other.fuAddress)
  , fSignal(other.fSignal)
  , fTimestamp(other.fTimestamp)

  , fAmpl(other.fAmpl)
  , fZL(other.fZL)
  , fIntegral(other.fIntegral)
  , fTimeMax(other.fTimeMax)

  , fFitAmpl(other.fFitAmpl)
  , fFitZL(other.fFitZL)
  , fFitIntegral(other.fFitIntegral)
  , fFitR2(other.fFitR2)
  , fFitTimeMax(other.fFitTimeMax)

{
}


// --- Move constructor
BmnScWallDigi::BmnScWallDigi(BmnScWallDigi&& other)
  : fuAddress(other.fuAddress)
  , fSignal(other.fSignal)
  , fTimestamp(other.fTimestamp)

  , fAmpl(other.fAmpl)
  , fZL(other.fZL)
  , fIntegral(other.fIntegral)
  , fTimeMax(other.fTimeMax)

  , fFitAmpl(other.fFitAmpl)
  , fFitZL(other.fFitZL)
  , fFitIntegral(other.fFitIntegral)
  , fFitR2(other.fFitR2)
  , fFitTimeMax(other.fFitTimeMax)

{
}


// --- Destructor
BmnScWallDigi::~BmnScWallDigi()
{
}


// --- Assignment operator
BmnScWallDigi& BmnScWallDigi::operator=(const BmnScWallDigi& other)
{
  if (this != &other) {
    fuAddress = other.fuAddress;
    fSignal    = other.fSignal;
    fTimestamp  = other.fTimestamp;

    fAmpl    = other.fAmpl;
    fZL      = other.fZL;
    fIntegral   = other.fIntegral;
    fTimeMax = other.fTimeMax;

    fFitAmpl = other.fFitAmpl;
    fFitZL    = other.fFitZL;
    fFitIntegral = other.fFitIntegral;
    fFitR2 = other.fFitR2;
    fFitTimeMax     = other.fFitTimeMax;

  }
  return *this;
}


// --- Move assignment operator
BmnScWallDigi& BmnScWallDigi::operator=(BmnScWallDigi&& other)
{
  if (this != &other) {
    fuAddress = other.fuAddress;
    fSignal    = other.fSignal;
    fTimestamp  = other.fTimestamp;

    fAmpl    = other.fAmpl;
    fZL      = other.fZL;
    fIntegral   = other.fIntegral;
    fTimeMax = other.fTimeMax;

    fFitAmpl = other.fFitAmpl;
    fFitZL    = other.fFitZL;
    fFitIntegral = other.fFitIntegral;
    fFitR2 = other.fFitR2;
    fFitTimeMax     = other.fFitTimeMax;

  }
  return *this;
}

void BmnScWallDigi::reset()
{
  fuAddress     = 0;   /// Unique channel address
  fSignal       = 0.;  /// Signal [MeV]
  fTimestamp    = -1.; /// Signal timestamp

  fAmpl         = 0.;  /// Amplitude from waveform [adc counts]
  fZL           = 0.;  /// ZeroLevel from waveform [adc counts]
  fIntegral     = 0.;  /// Energy deposition from waveform [adc counts]
  fTimeMax      = 0.;  /// Time of maximum in waveform [adc samples]

  fFitAmpl      = 0.;  /// Amplitude from fit of waveform [adc counts]
  fFitZL        = 0.;  /// ZeroLevel from fit of waveform [adc counts]
  fFitIntegral  = 0.;  /// Energy deposition from fit of waveform [adc counts]
  fFitR2        = 2.;  /// Quality of waveform fit [] -- good near 0
  fFitTimeMax   = -1.; /// Time of maximum in fit of waveform [adc samples]

}

ClassImp(BmnScWallDigi)
