/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_TOFHITSCONVERTER_H
#define ANALYSIS_TREE_TOFHITSCONVERTER_H

#include <utility>

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

enum class BMNTOF{
  TOF400,
  TOF700
};

class BmnTofHitsConverter final : public CbmConverterTask {
public:
  BmnTofHitsConverter(const std::string &out_branch_name, BMNTOF tof_type)
      : CbmConverterTask(out_branch_name), tof_type_(tof_type) {}
  ~BmnTofHitsConverter() final;

  void Init() final;
  void ProcessData() final;
  void Finish() final {}

private:

  const std::map<int, int>& GetMatchMap(const std::string& name) const
  {
    const auto& it = indexes_map_->find(name);
    if (it == indexes_map_->end()) { throw std::runtime_error(name + " is not found to match with TOF hits"); }
    return it->second;
  }

  TClonesArray*in_bmn_tof_hits_{nullptr};
  TClonesArray*in_bmn_global_tracks_{nullptr};
  BMNTOF tof_type_;

  AnalysisTree::HitDetector*out_tof_hits_{nullptr};

  ClassDef(BmnTofHitsConverter, 1)
};

#endif  // ANALYSIS_TREE_TOFHITSCONVERTER_H
