/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_RECEVENTHEADERCONVERTER_H_
#define ANALYSIS_TREE_RECEVENTHEADERCONVERTER_H_

#include "CbmConverterTask.h"
//#include "CbmEvent.h"

#include "AnalysisTree/EventHeader.hpp"

class FairMCEventHeader;
class TClonesArray;
class CbmVertex;

class CbmRecEventHeaderConverter final : public CbmConverterTask {
public:
  explicit CbmRecEventHeaderConverter(std::string out_branch_name) : CbmConverterTask(std::move(out_branch_name)) {};
  ~CbmRecEventHeaderConverter() final = default;

  void Init() final;
  void ProcessData() final;
  void Finish() final { delete rec_event_header_; };

private:

  AnalysisTree::EventHeader* rec_event_header_ {nullptr};

  TClonesArray* cbm_psd_hits_ {nullptr};
  TClonesArray* cbm_sts_tracks_ {nullptr};   ///< non-owning pointer
  FairMCEventHeader* cbm_header_ {nullptr};  ///< non-owning pointer
  CbmVertex* cbm_prim_vertex_ {nullptr};     ///< non-owning pointer

  ClassDef(CbmRecEventHeaderConverter, 1)
};

#endif  // ANALYSIS_TREE_RECEVENTHEADERCONVERTER_H_
