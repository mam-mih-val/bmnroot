/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universität Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_STSTRACKSCONVERTER_H_
#define ANALYSIS_TREE_STSTRACKSCONVERTER_H_

#include <utility>

#include "CbmConverterTask.h"

#include "AnalysisTree/Detector.hpp"

class TClonesArray;
class CbmVertex;
class CbmStsTrack;

namespace AnalysisTree
{
  class Matching;
}

class BmnGlobalTracksConverter final : public CbmConverterTask {

public:
  BmnGlobalTracksConverter(std::string out_branch_name,
                           std::string str_sts_trk_branch_name,
                           std::string str_tof_400_branch_name,
                           std::string str_tof_700_branch_name)
      : CbmConverterTask(std::move(out_branch_name)),
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
  AnalysisTree::TrackDetector* vtx_tracks_{nullptr};   ///< raw pointers are needed for TTree::Branch
  AnalysisTree::Matching* global_tracks_2_sts_tracks_{nullptr};  ///< raw pointers are needed for TTree::Branch
  CbmVertex* bmn_vertex_{nullptr};    ///< non-owning pointer
                                            //  TClonesArray* bmn_mc_tracks_ {nullptr};   ///< non-owning pointer
  TClonesArray*bmn_global_tracks_{nullptr};  ///< non-owning pointer

  ClassDef(BmnGlobalTracksConverter, 1)
};

#endif  // ANALYSIS_TREE_STSTRACKSCONVERTER_H_
