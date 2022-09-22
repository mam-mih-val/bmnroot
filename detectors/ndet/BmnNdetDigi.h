/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** \file BmnNdetDigi.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 16.01.2022
 **/

/** \class BmnNdetDigi
 ** \brief Data class for Bmn Ndet digital signal processing
 ** \version 1.0
 **/

#ifndef BmnNdetDigi_H
#define BmnNdetDigi_H 1

#include "BmnDigiContainerTemplate.h"  // for BmnDigiContainerTemplate
#include "BmnDetectorList.h"           // for kNDET
#include "BmnNdetAddress.h"            // for BmnNdetAddress

class BmnNdetDigi : public BmnDigiContainerTemplate {

public:
  /**@brief Default constructor.
       **/
  BmnNdetDigi() : BmnDigiContainerTemplate() {};


  /** @brief Constructor with detailed assignment.
       **/
  BmnNdetDigi(uint32_t address, float signal, double timestamp, 
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
  ~BmnNdetDigi() {};


  /** @brief Class name
       ** @return BmnNdetDigi
       **/
  virtual const char* GetClassName() override final { return "BmnNdetDigi"; }


  /** @brief Cell Id
       ** @return Cell Id from Unique channel address (see BmnNdetAddress)
       **/
  uint32_t GetCellId() const { return BmnNdetAddress::GetCellId(GetAddress()); };


  /** @brief Layer Id
       ** @return Layer Id from Unique channel address (see BmnNdetAddress)
       **/
  uint32_t GetLayerId() const { return BmnNdetAddress::GetLayerId(GetAddress()); };


  /** @brief X position
       ** @return X position
       **/
  uint32_t GetX() const { return BmnNdetAddress::GetXIdx(GetAddress()); }


  /** @brief Y position
       ** @return Y position
       **/
  uint32_t GetY() const { return BmnNdetAddress::GetYIdx(GetAddress()); }


  /** @brief Z position
       ** @return Z position
       **/
  uint32_t GetZ() const { return BmnNdetAddress::GetZIdx(GetAddress()); }


  /** @brief System identifier
       ** @return System ID 
       **/
  static int GetSystemId() { return kNDET; }


  const int DrawWfm() {
    TString hist_name = Form("Cell%u Layer%u. Signal %.2f FitR2 %.2f", GetCellId(), GetLayerId(), GetSignal(), GetFitR2());
    DrawWfmWithTitle(hist_name);
    return 1;
  }

  ClassDefOverride(BmnNdetDigi, 1);
};

#endif  // BmnNdetDigi_H
