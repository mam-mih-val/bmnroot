/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Daniel Wielanek, Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_SIMTRACKSCONVERTER_H_
#define ANALYSIS_TREE_SIMTRACKSCONVERTER_H_

#include "CbmConverterTask.h"

#include <TString.h>

#include "AnalysisTree/Detector.hpp"

class TClonesArray;
class UEvent;
class TFile;
class TTree;
class FairMCEventHeader;

class CbmSimTracksConverter final : public CbmConverterTask {

public:
  explicit CbmSimTracksConverter(std::string out_branch_name, std::string match_to = "")
    : CbmConverterTask(std::move(out_branch_name), std::move(match_to)) {};

  ~CbmSimTracksConverter() final;

//  void SetUnigenInfo(const std::string& unigen_name)
//  {
//    use_unigen_       = true;
//    unigen_file_name_ = unigen_name;
//  }

  void Init() final;
  void ProcessData() final;
  void Finish() final {};

private:
  AnalysisTree::Particles* sim_tracks_ {nullptr};
  FairMCEventHeader* cbm_header_ {nullptr};
  TClonesArray* cbm_mc_tracks_ {nullptr};

//  CbmMCDataManager* cbm_mc_manager_ {nullptr};
//  CbmMCDataArray* cbm_mc_tracks_new_ {nullptr};

//  void InitUnigen();

//  UEvent* unigen_event_ {nullptr};
//  TFile* unigen_file_ {nullptr};
//  TTree* unigen_tree_ {nullptr};
//  std::string unigen_file_name_;
//  Int_t entry_ {0};
//  Double_t beta_cm_ {0};  ///< CM velocity in the lab frame
//  Bool_t use_unigen_ {false};

  ClassDef(CbmSimTracksConverter, 1)
};

#endif
