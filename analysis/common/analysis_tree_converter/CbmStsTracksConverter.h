/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universität Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_STSTRACKSCONVERTER_H_
#define ANALYSIS_TREE_STSTRACKSCONVERTER_H_

#include "CbmConverterTask.h"

#include "AnalysisTree/Detector.hpp"

class TClonesArray;
class CbmVertex;
class CbmStsTrack;

namespace AnalysisTree
{
  class Matching;
}

class CbmStsTracksConverter final : public CbmConverterTask {

public:
  explicit CbmStsTracksConverter(std::string out_branch_name, std::string match_to = "")
    : CbmConverterTask(std::move(out_branch_name), std::move(match_to))
  {
  }

  ~CbmStsTracksConverter() final;

  void Init() final;
  void ProcessData() final;
  void Finish() final {}

private:
  void ReadVertexTracks();
  void InitInput();

  AnalysisTree::TrackDetector* vtx_tracks_{nullptr};   ///< raw pointers are needed for TTree::Branch
  AnalysisTree::Matching* sim_particles_2_vtx_tracks_{nullptr};  ///< raw pointers are needed for TTree::Branch
  CbmVertex* bmn_vertex_{nullptr};    ///< non-owning pointer
                                            //  TClonesArray* bmn_mc_tracks_ {nullptr};   ///< non-owning pointer
  TClonesArray*bmn_global_tracks_{nullptr};  ///< non-owning pointer

  ClassDef(CbmStsTracksConverter, 1)
};

#endif  // ANALYSIS_TREE_STSTRACKSCONVERTER_H_
