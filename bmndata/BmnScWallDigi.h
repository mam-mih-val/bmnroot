/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** \file BmnScWallDigi.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 06.01.2022
 **/

/** \class BmnScWallDigi
 ** \brief Data class for BmnScWall digital signal processing
 ** \version 1.0
 **/

#ifndef BmnScWallDigi_H
#define BmnScWallDigi_H 1

#include "BmnDigiContainerTemplate.h"  // for BmnDigiContainerTemplate
#include "BmnDetectorList.h"           // for kSCWALL
#include "BmnScWallAddress.h"          // for BmnScWallAddress

class BmnScWallDigi : public BmnDigiContainerTemplate {

public:
  /**@brief Default constructor.
       **/
  BmnScWallDigi() : BmnDigiContainerTemplate() {};


  /** @brief Constructor with detailed assignment.
       **/
  BmnScWallDigi(uint32_t address, float signal, double timestamp, 
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
  ~BmnScWallDigi() {};


  /** @brief Class name
       ** @return BmnScWallDigi
       **/
  virtual const char* GetClassName() override final { return "BmnScWallDigi"; } 

  /** @brief Cell Id
       ** @return Cell Id from Unique channel address (see BmnScWallAddress)
       **/
  uint32_t GetCellId() const { return BmnScWallAddress::GetCellId(GetAddress()); };


  /** @brief X position
       ** @return X position
       **/
  uint32_t GetX() const { return BmnScWallAddress::GetXIdx(GetAddress()); }


  /** @brief Y position
       ** @return Y position
       **/
  uint32_t GetY() const { return BmnScWallAddress::GetYIdx(GetAddress()); }


  /** @brief System identifier
       ** @return System ID 
       **/
  static int GetSystemId() { return kSCWALL; }


  const int DrawWfm() {
    TString hist_name = Form("Cell%u. Signal %.2f FitR2 %.2f", GetCellId(), GetSignal(), GetFitR2());
    DrawWfmWithTitle(hist_name);
    return 1;
  }

  ClassDefOverride(BmnScWallDigi, 1);
};

#endif  // BmnScWallDigi_H
