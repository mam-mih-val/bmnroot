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

  void SetIsWriteKFInfo(bool is = true) { is_write_kfinfo_ = is; }
  void SetIsReproduceCbmKFPF(bool is = true) { is_reproduce_cbmkfpf_ = is; }

private:
  void ReadVertexTracks();
  void MapTracks();
  void InitInput();
//   float ExtrapolateToVertex(CbmStsTrack* sts_track, AnalysisTree::Track& track, int pdg);

  void WriteKFInfo(AnalysisTree::Track& track, CbmStsTrack* sts_track, bool is_good_track) const;
  bool IsGoodCovMatrix(CbmStsTrack* sts_track) const;

  AnalysisTree::TrackDetector* vtx_tracks_ {nullptr};   ///< raw pointers are needed for TTree::Branch
  AnalysisTree::Matching* vtx_tracks_2_sim_ {nullptr};  ///< raw pointers are needed for TTree::Branch

  CbmVertex* cbm_prim_vertex_ {nullptr};    ///< non-owning pointer
                                            //  TClonesArray* cbm_mc_tracks_ {nullptr};   ///< non-owning pointer
  TClonesArray* cbm_mc_tracks_ {nullptr};  ///< non-owning pointer
  TClonesArray* cbm_sts_tracks_ {nullptr};  ///< non-owning pointer
  TClonesArray* cbm_sts_match_ {nullptr};   ///< non-owning pointer

  bool is_write_kfinfo_ {true};
  bool is_reproduce_cbmkfpf_ {true};

  int ipar_ {-1};
  int imf_ {-1};
  int icov_ {-1};
  int imc_pdg_ {-1};
  int imother_pdg_ {-1};
  int ipasscuts_ {-1};

  ClassDef(CbmStsTracksConverter, 1)
};

#endif  // ANALYSIS_TREE_STSTRACKSCONVERTER_H_
