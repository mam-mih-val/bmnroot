/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_RECEVENTHEADERCONVERTER_H_
#define ANALYSIS_TREE_RECEVENTHEADERCONVERTER_H_

#include "BmnConverterTask.h"
#include "BmnFHCalEvent.h"
//#include "CbmEvent.h"

#include "AnalysisTree/EventHeader.hpp"

class FairMCEventHeader;
class TClonesArray;
class CbmVertex;

class BmnRecEventHeaderConverter final : public BmnConverterTask {
public:
  explicit BmnRecEventHeaderConverter(std::string out_branch_name) : BmnConverterTask(std::move(out_branch_name)) {};
  ~BmnRecEventHeaderConverter() final = default;

  void Init() final;
  void ProcessData() final;
  void Finish() final { delete rec_event_header_; };

private:

  AnalysisTree::EventHeader* rec_event_header_ {nullptr};

  BmnFHCalEvent*in_fhcal_event_{nullptr};
  FairMCEventHeader*in_event_header_{nullptr};  ///< non-owning pointer
  CbmVertex*in_prim_vertex_{nullptr};     ///< non-owning pointer
  TClonesArray*in_global_tracks_{nullptr};   ///< non-owning pointer

  ClassDef(BmnRecEventHeaderConverter, 1)
};

#endif  // ANALYSIS_TREE_RECEVENTHEADERCONVERTER_H_
