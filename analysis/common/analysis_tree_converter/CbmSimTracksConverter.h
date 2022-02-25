/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Daniel Wielanek, Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_SIMTRACKSCONVERTER_H_
#define ANALYSIS_TREE_SIMTRACKSCONVERTER_H_

#include "CbmConverterTask.h"

#include <TString.h>

#include "AnalysisTree/Detector.hpp"
#include "AnalysisTree/Matching.hpp"

class TClonesArray;
class TFile;
class TTree;
class FairMCEventHeader;

class CbmSimTracksConverter final : public CbmConverterTask {

public:
  explicit CbmSimTracksConverter(std::string out_branch_name, std::string match_to = "")
    : CbmConverterTask(std::move(out_branch_name), std::move(match_to)) {};

  ~CbmSimTracksConverter() final;

  void Init() final;
  void ProcessData() final;
  void Finish() final {};

private:
  void MapTracks();
  AnalysisTree::Particles* sim_tracks_ {nullptr};
  AnalysisTree::Matching* sim_particles_2_vtx_tracks_{nullptr};  ///< raw pointers are needed for TTree::Branch
  FairMCEventHeader*bmn_header_{nullptr};
  TClonesArray*bmn_mc_tracks_{nullptr};
  TClonesArray*bmn_global_tracks_{nullptr};  ///< non-owning pointer

  std::map<int, int> sim_partcles_bmn_tracks_map_;
  
  ClassDef(CbmSimTracksConverter, 1)
};

#endif
