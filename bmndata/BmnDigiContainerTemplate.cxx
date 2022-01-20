/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** @file BmnDigiContainerTemplate.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 18.01.2022
 **
 ** Code for Data class for Bmn digi container template
 **/

#include "BmnDigiContainerTemplate.h"

#include <TBuffer.h>  // for TBuffer
#include <TClass.h>   // for BmnDigiContainerTemplate::IsA()
#include <TString.h>  // for Form, TString

#include <string>  // for basic_string

// --- Default constructor
BmnDigiContainerTemplate::BmnDigiContainerTemplate()
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

  , fWfm()
  , fFitWfm()
{
}

// clang-format off
// --- Constructor with assignment
BmnDigiContainerTemplate::BmnDigiContainerTemplate(uint32_t address, float signal, double timestamp, 
                int ampl, int zl, int integral, int time_max,
                float fit_ampl, float fit_zl, float fit_integral, float fit_R2, float fit_time_max,
                std::vector<float> wfm, std::vector<float> fit_wfm)

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

  , fWfm(wfm)
  , fFitWfm(fit_wfm)
{
}
// clang-format on

// --- Copy constructor
BmnDigiContainerTemplate::BmnDigiContainerTemplate(const BmnDigiContainerTemplate& other)
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

  , fWfm(other.fWfm)
  , fFitWfm(other.fFitWfm)
{
}


// --- Move constructor
BmnDigiContainerTemplate::BmnDigiContainerTemplate(BmnDigiContainerTemplate&& other)
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

  , fWfm(other.fWfm)
  , fFitWfm(other.fFitWfm)
{
}


// --- Assignment operator
BmnDigiContainerTemplate& BmnDigiContainerTemplate::operator=(const BmnDigiContainerTemplate& other)
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

    fWfm = other.fWfm;
    fFitWfm = other.fFitWfm;
  }
  return *this;
}


// --- Move assignment operator
BmnDigiContainerTemplate& BmnDigiContainerTemplate::operator=(BmnDigiContainerTemplate&& other)
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

    fWfm = other.fWfm;
    fFitWfm = other.fFitWfm;
  }
  return *this;
}

void BmnDigiContainerTemplate::reset()
{
  fuAddress     = 0;   /// Unique channel address
  fSignal       = 0.;  /// Signal [MeV]
  fTimestamp    = -1.; /// Signal timestamp

  fAmpl         = 0;  /// Amplitude from waveform [adc counts]
  fZL           = 0;  /// ZeroLevel from waveform [adc counts]
  fIntegral     = 0;  /// Energy deposition from waveform [adc counts]
  fTimeMax      = 0;  /// Time of maximum in waveform [adc samples]

  fFitAmpl      = 0.;  /// Amplitude from fit of waveform [adc counts]
  fFitZL        = 0.;  /// ZeroLevel from fit of waveform [adc counts]
  fFitIntegral  = 0.;  /// Energy deposition from fit of waveform [adc counts]
  fFitR2        = 2.;  /// Quality of waveform fit [] -- good near 0
  fFitTimeMax   = -1.; /// Time of maximum in fit of waveform [adc samples]

  fWfm.clear();
  fFitWfm.clear();
}

void BmnDigiContainerTemplate::DrawWfmWithTitle(TString hist_name)
{
  if(fWfm.empty()) return;
  TCanvas *canv_ptr = new TCanvas();
  std::vector<float> points(fWfm.size());
  std::iota(std::begin(points), std::end(points), 0); // Fill with 0, 1, ..., wfm.back().
  TGraph *tgr_ptr = new TGraph(fWfm.size(), &points[0], &fWfm[0]);
  tgr_ptr->SetTitle(hist_name.Data());
  tgr_ptr->Draw();
  if(!fFitWfm.empty()){
    TGraph *tgr_ptr_fit = new TGraph(fFitWfm.size(), &points[0], &fFitWfm[0]);
    tgr_ptr_fit->SetLineColor(kRed);
    tgr_ptr_fit->SetLineWidth(2);
    tgr_ptr_fit->Draw("same");
  }
}

ClassImp(BmnDigiContainerTemplate)
