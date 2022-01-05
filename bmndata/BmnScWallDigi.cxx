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
  : fuAddress()
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

  : fuAddress(address)
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


ClassImp(BmnScWallDigi)
