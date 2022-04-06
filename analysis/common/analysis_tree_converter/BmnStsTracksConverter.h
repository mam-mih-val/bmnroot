/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universität Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_STSTRACKSCONVERTER_H_
#define ANALYSIS_TREE_STSTRACKSCONVERTER_H_

#include <utility>

#include "BmnConverterTask.h"

#include "AnalysisTree/Detector.hpp"

class TClonesArray;
class CbmVertex;
class CbmStsTrack;

namespace AnalysisTree
{
  class Matching;
}

class BmnStsTracksConverter final : public BmnConverterTask {

public:
  explicit BmnStsTracksConverter(const std::string &out_branch_name)
      : BmnConverterTask(out_branch_name) {}
  ~BmnStsTracksConverter() final;

  void Init() final;
  void ProcessData() final;
  void Finish() final {}

private:
  void ReadVertexTracks();
  void InitInput();
  float ExtrapolateToVertex(CbmStsTrack* sts_track, int pdg);
  AnalysisTree::TrackDetector*out_sts_tracks_{nullptr};   ///< raw pointers are needed for TTree::Branch

  CbmVertex*in_bmn_vertex_{nullptr};    ///< non-owning pointer
                                            //  TClonesArray* bmn_mc_tracks_ {nullptr};   ///< non-owning pointer
  TClonesArray*in_sts_tracks_{nullptr};  ///< non-owning pointer

  ClassDef(BmnStsTracksConverter, 1)
};

#endif  // ANALYSIS_TREE_STSTRACKSCONVERTER_H_
