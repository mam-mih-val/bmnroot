/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_SIMEVENTHEADERCONVERTER_H_
#define ANALYSIS_TREE_SIMEVENTHEADERCONVERTER_H_

#include "BmnConverterTask.h"

#include "AnalysisTree/EventHeader.hpp"

class FairMCEventHeader;
class CbmVertex;

class BmnSimEventHeaderConverter final : public BmnConverterTask {
public:
  explicit BmnSimEventHeaderConverter(std::string out_branch_name) : BmnConverterTask(std::move(out_branch_name)) {};
  ~BmnSimEventHeaderConverter() final = default;

  void Init() final;
  void ProcessData() final;
  void Finish() final { delete sim_event_header_; };

private:
  AnalysisTree::EventHeader* sim_event_header_ {nullptr};
  FairMCEventHeader* cbm_header_ {nullptr};

  ClassDef(BmnSimEventHeaderConverter, 1)
};

#endif  // ANALYSIS_TREE_SIMEVENTHEADERCONVERTER_H_
