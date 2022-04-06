/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_TOFHITSCONVERTER_H
#define ANALYSIS_TREE_TOFHITSCONVERTER_H

#include <utility>

#include "BmnConverterTask.h"

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

class BmnTofHitsConverter final : public BmnConverterTask {
public:
  BmnTofHitsConverter(const std::string &out_branch_name, BMNTOF tof_type)
      : BmnConverterTask(out_branch_name), tof_type_(tof_type) {}
  ~BmnTofHitsConverter() final;

  void Init() final;
  void ProcessData() final;
  void Finish() final {}

private:

  void MapTracks();
  // Returns the position-vector of {x,y,x} in exact z
  TVector3 ExtrapolateStraightLine(FairTrackParam* params, float z);

  TClonesArray*in_bmn_tof_hits_{nullptr};
  TClonesArray*in_bmn_global_tracks_{nullptr};
  const BMNTOF tof_type_;

  std::map<int, int> tof_hit_idx_2_global_trk_idx_;
  AnalysisTree::HitDetector*out_tof_hits_{nullptr};

  ClassDef(BmnTofHitsConverter, 1)
};

#endif  // ANALYSIS_TREE_TOFHITSCONVERTER_H
