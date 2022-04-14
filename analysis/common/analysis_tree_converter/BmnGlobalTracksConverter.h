/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universität Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_GLOBALTRACKSCONVERTER_H_
#define ANALYSIS_TREE_GLOBALTRACKSCONVERTER_H_

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

class BmnGlobalTracksConverter final : public BmnConverterTask {

public:
  explicit BmnGlobalTracksConverter(std::string out_branch_name,
                           std::string str_sts_trk_branch_name="",
                           std::string str_tof_400_branch_name="",
                           std::string str_tof_700_branch_name="")
      : BmnConverterTask(std::move(out_branch_name)),
        str_sts_trk_branch_name_(std::move(str_sts_trk_branch_name)),
        str_tof400_branch_name_(std::move(str_tof_400_branch_name)),
        str_tof700_branch_name_(std::move(str_tof_700_branch_name)) {}
  ~BmnGlobalTracksConverter() final;

  void Init() final;
  void ProcessData() final;
  void Finish() final {}

private:
  void ReadVertexTracks();
  void InitInput();

  std::string str_sts_trk_branch_name_;
  std::string str_tof400_branch_name_;
  std::string str_tof700_branch_name_;
  AnalysisTree::TrackDetector*out_global_tracks_{nullptr};   ///< raw pointers are needed for TTree::Branch
  AnalysisTree::Matching* global_tracks_2_sts_tracks_{nullptr};  ///< raw pointers are needed for TTree::Branch
  AnalysisTree::Matching* global_tracks_2_tof400_hits_{nullptr};  ///< raw pointers are needed for TTree::Branch
  AnalysisTree::Matching* global_tracks_2_tof700_hits_{nullptr};  ///< raw pointers are needed for TTree::Branch
  CbmVertex*in_bmn_vertex_{nullptr};    ///< non-owning pointer
                                            //  TClonesArray* bmn_mc_tracks_ {nullptr};   ///< non-owning pointer
  TClonesArray*in_bmn_global_tracks_{nullptr};  ///< non-owning pointer

  ClassDef(BmnGlobalTracksConverter, 1)
};

#endif  // ANALYSIS_TREE_GLOBALTRACKSCONVERTER_H_
