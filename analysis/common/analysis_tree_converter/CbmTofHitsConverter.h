/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_TOFHITSCONVERTER_H
#define ANALYSIS_TREE_TOFHITSCONVERTER_H

#include "CbmConverterTask.h"

#include "AnalysisTree/Detector.hpp"

class TClonesArray;
class FairTrackParam;
class CbmMCDataManager;
class CbmMCDataArray;

namespace AnalysisTree
{
  class Matching;
}

class CbmTofHitsConverter final : public CbmConverterTask {
public:
  explicit CbmTofHitsConverter(std::string out_branch_name, std::string match_to = "")
    : CbmConverterTask(std::move(out_branch_name), std::move(match_to))
  {
  }

  ~CbmTofHitsConverter() final;

  void Init() final;
  void ProcessData(CbmEvent* event) final;
  void Finish() final {}

private:
  static void ExtrapolateStraightLine(FairTrackParam* params, float z);

  const std::map<int, int>& GetMatchMap(const std::string& name) const
  {
    const auto& it = indexes_map_->find(name);
    if (it == indexes_map_->end()) { throw std::runtime_error(name + " is not found to match with TOF hits"); }
    return it->second;
  }
  std::string mc_tracks_ {"SimParticles"};

  TClonesArray* cbm_global_tracks_ {nullptr};
  TClonesArray* cbm_tof_hits_ {nullptr};
  //  TClonesArray* cbm_tof_points_ {nullptr};
  TClonesArray* cbm_tof_match_ {nullptr};
  //  TClonesArray* cbm_mc_tracks_ {nullptr};
  CbmMCDataManager* cbm_mc_manager_ {nullptr};
  CbmMCDataArray* cbm_mc_tracks_new_ {nullptr};
  CbmMCDataArray* cbm_tof_points_new_ {nullptr};

  AnalysisTree::HitDetector* tof_hits_ {nullptr};
  AnalysisTree::Matching* vtx_tracks_2_tof_ {nullptr};
  AnalysisTree::Matching* tof_hits_2_mc_tracks_ {nullptr};

  ClassDef(CbmTofHitsConverter, 1)
};

#endif  // ANALYSIS_TREE_TOFHITSCONVERTER_H
