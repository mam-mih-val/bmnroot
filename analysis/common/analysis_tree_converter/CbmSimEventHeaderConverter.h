/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_SIMEVENTHEADERCONVERTER_H_
#define ANALYSIS_TREE_SIMEVENTHEADERCONVERTER_H_

#include "CbmConverterTask.h"

#include "AnalysisTree/EventHeader.hpp"

class FairMCEventHeader;
class CbmVertex;
//class CbmMCDataManager;
//class CbmMCDataArray;
//class CbmMCDataObject;

class CbmSimEventHeaderConverter final : public CbmConverterTask {
public:
  explicit CbmSimEventHeaderConverter(std::string out_branch_name) : CbmConverterTask(std::move(out_branch_name)) {};
  ~CbmSimEventHeaderConverter() final = default;

  void Init() final;
  void ProcessData() final;
  void Finish() final { delete sim_event_header_; };

private:
  AnalysisTree::EventHeader* sim_event_header_ {nullptr};
  FairMCEventHeader* cbm_header_ {nullptr};
//  CbmMCDataManager* cbm_mc_manager_ {nullptr};
//  CbmMCDataObject* cbm_header_obj_ {nullptr};

  ClassDef(CbmSimEventHeaderConverter, 1)
};

#endif  // ANALYSIS_TREE_SIMEVENTHEADERCONVERTER_H_
