/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** \file BmnFHCalDigi.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 16.01.2022
 **/

/** \class BmnFHCalDigi
 ** \brief Data class for Bmn FHCal digital signal processing
 ** \version 1.0
 **/

#ifndef BmnFHCalDigi_H
#define BmnFHCalDigi_H 1

#include "BmnDigiContainerTemplate.h"  // for BmnDigiContainerTemplate
#include "BmnDetectorList.h"           // for kFHCaL
#include "BmnFHCalAddress.h"           // for BmnFHCalAddress

class BmnFHCalDigi : public BmnDigiContainerTemplate {

public:
  /**@brief Default constructor.
       **/
  BmnFHCalDigi() : BmnDigiContainerTemplate() {};


  /** @brief Constructor with detailed assignment.
       **/
  BmnFHCalDigi(uint32_t address, float signal, double timestamp, 
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
  ~BmnFHCalDigi() {};


  /** @brief Class name
       ** @return BmnFHCalDigi
       **/
  virtual const char* GetClassName() override final { return "BmnFHCalDigi"; }


  /** @brief Module Type
       ** @return Module Type from Unique channel address (see BmnFHCalAddress)
       **/
  uint32_t GetModuleType() const { return BmnFHCalAddress::GetModuleType(GetAddress()); };


  /** @brief Module Id
       ** @return Module Id from Unique channel address (see BmnFHCalAddress)
       **/
  uint32_t GetModuleId() const { return BmnFHCalAddress::GetModuleId(GetAddress()); };


  /** @brief Section Id
       ** @return Section Id from Unique channel address (see BmnFHCalAddress)
       **/
  uint32_t GetSectionId() const { return BmnFHCalAddress::GetSectionId(GetAddress()); };


  /** @brief X position
       ** @return X position
       **/
  uint32_t GetX() const { return BmnFHCalAddress::GetXIdx(GetAddress()); }


  /** @brief Y position
       ** @return Y position
       **/
  uint32_t GetY() const { return BmnFHCalAddress::GetYIdx(GetAddress()); }


  /** @brief Z position
       ** @return Z position
       **/
  uint32_t GetZ() const { return BmnFHCalAddress::GetZIdx(GetAddress()); }


  /** @brief System identifier
       ** @return System ID 
       **/
  static int GetSystemId() { return kFHCAL; }


  const int DrawWfm() {
    TString hist_name = Form("Mod%u Sec%u. Signal %.2f FitR2 %.2f", GetModuleId(), GetSectionId(), GetSignal(), GetFitR2());
    DrawWfmWithTitle(hist_name);
    return 1;
  }

  ClassDefOverride(BmnFHCalDigi, 1);
};

#endif  // BmnFHCalDigi_H
