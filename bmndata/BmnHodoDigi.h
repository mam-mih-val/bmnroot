/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** \file BmnHodoDigi.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 21.01.2022
 **/

/** \class BmnHodoDigi
 ** \brief Data class for BmnHodo digital signal processing
 ** \version 1.0
 **/

#ifndef BmnHodoDigi_H
#define BmnHodoDigi_H 1

#include "BmnDigiContainerTemplate.h"  // for BmnDigiContainerTemplate
#include "BmnDetectorList.h"           // for kHodo
#include "BmnHodoAddress.h"            // for BmnHodoAddress

class BmnHodoDigi : public BmnDigiContainerTemplate {

public:
  /**@brief Default constructor.
       **/
  BmnHodoDigi() : BmnDigiContainerTemplate() {};


  /** @brief Constructor with detailed assignment.
       **/
  BmnHodoDigi(uint32_t address, float signal, double timestamp, 
                int ampl, int zl, int integral, int time_max,
                float fit_ampl, float fit_zl, float fit_integral, float fit_R2, float fit_time_max,
                std::vector<float> wfm, std::vector<float> fit_wfm)

  : BmnDigiContainerTemplate(address, signal, timestamp, 
                ampl, zl, integral, time_max,
                fit_ampl, fit_zl, fit_integral, fit_R2, fit_time_max,
                wfm, fit_wfm)
  {
  }


  /** Destructor **/
  ~BmnHodoDigi() {};


  /** @brief Class name
       ** @return BmnHodoDigi
       **/
  virtual const char* GetClassName() override final { return "BmnHodoDigi"; }


  /** @brief Strip Id
       ** @return Strip Id from Unique channel address (see BmnHodoAddress)
       **/
  uint32_t GetStripId() const { return BmnHodoAddress::GetStripId(GetAddress()); };

  /** @brief Strip Side
       ** @return Strip Side from Unique channel address (see BmnHodoAddress)
       **/
  uint32_t GetStripSide() const { return BmnHodoAddress::GetStripSide(GetAddress()); };

  /** @brief Gain
       ** @return Gain from Unique channel address (see BmnHodoAddress)
       **/
  uint32_t GetGain() const { return BmnHodoAddress::GetGain(GetAddress()); };


  /** @brief System identifier
       ** @return System ID 
       **/
  static int GetSystemId() { return kHODO; }


  const int DrawWfm() {
    TString hist_name = Form("Strip%u. Signal %.2f FitR2 %.2f", GetStripId(), GetSignal(), GetFitR2());
    DrawWfmWithTitle(hist_name);
    return 1;
  }

  ClassDefOverride(BmnHodoDigi, 1);
};

#endif  // BmnHodoDigi_H
