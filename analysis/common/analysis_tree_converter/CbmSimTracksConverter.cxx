/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Daniel Wielanek, Viktor Klochkov [committer] */

#include "CbmSimTracksConverter.h"

//#include "CbmDefs.h"
//#include "CbmEvent.h"
//#include "CbmMCDataArray.h"
//#include "CbmMCDataManager.h"
#include "CbmMCTrack.h"

#include "FairLogger.h"
#include "FairMCEventHeader.h"
#include "FairRootManager.h"

#include <TClonesArray.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TRandom.h>
#include <TString.h>
#include <TTree.h>

#include <cassert>
#include <vector>

#include "AnalysisTree/TaskManager.hpp"
//#include "UEvent.h"
//#include "UParticle.h"
//#include "URun.h"

ClassImp(CbmSimTracksConverter);

//void CbmSimTracksConverter::InitUnigen()
//{
//  unigen_file_ = TFile::Open(unigen_file_name_.c_str(), "READ");
//  unigen_file_->Print();
//  if (unigen_file_->IsOpen()) {
//    unigen_tree_ = (TTree*) unigen_file_->Get("events");
//    if (unigen_tree_) use_unigen_ = kTRUE;
//    unigen_tree_->SetBranchAddress("event", &unigen_event_);
//    URun* run = dynamic_cast<URun*>(unigen_file_->Get("run"));
//    if (run == nullptr) {
//      LOG(error) << "CbmSimTracksConverter: No run description in urqmd file!";
//      delete unigen_file_;
//      unigen_file_ = nullptr;
//      use_unigen_  = kFALSE;
//    }
//    else {
//      Double_t mProt = 0.938272;
//      Double_t pTarg = run->GetPTarg();  // target momentum per nucleon
//      Double_t pProj = run->GetPProj();  // projectile momentum per nucleon
//      Double_t eTarg = TMath::Sqrt(pProj * pProj + mProt * mProt);
//      beta_cm_       = pTarg / eTarg;
//    }
//  }
//}

void CbmSimTracksConverter::Init()
{
  assert(!out_branch_.empty());
  auto* ioman = FairRootManager::Instance();
  cbm_mc_tracks_ = (TClonesArray*) ioman->GetObject("MCTrack");
  cbm_header_ = (FairMCEventHeader*) ioman->GetObject("MCEventHeader.");

//  cbm_mc_manager_    = dynamic_cast<CbmMCDataManager*>(ioman->GetObject("MCDataManager"));
//  cbm_mc_tracks_new_ = cbm_mc_manager_->InitBranch("MCTrack");

  AnalysisTree::BranchConfig sim_particles_branch(out_branch_, AnalysisTree::DetType::kParticle);
  sim_particles_branch.AddField<int>("mother_id", "id of mother particle, -1 for primaries");
  sim_particles_branch.AddField<int>("cbmroot_id", "track id in CbmRoot transport file");
  sim_particles_branch.AddField<int>("geant_process_id", "");
  sim_particles_branch.AddFields<int>({"n_hits_mvd", "n_hits_sts", "n_hits_trd"}, "Number of hits in the detector");

//  if (!unigen_file_name_.empty()) { InitUnigen(); }
//  else {
//    LOG(info) << "lack of unigen file" << unigen_file_name_;
//  }

  sim_particles_branch.AddFields<float>({"start_x", "start_y", "start_z"}, "Start position, cm");
  sim_particles_branch.AddField<float>("start_t", "t freezout coordinate fm/c");

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->AddBranch(out_branch_, sim_tracks_, sim_particles_branch);
}

void CbmSimTracksConverter::ProcessData()
{
  assert(cbm_mc_tracks_);
  out_indexes_map_.clear();

  float delta_phi {0.f};
//  if (use_unigen_) {
//    unigen_tree_->GetEntry(entry_++);
//    const Double_t unigen_phi = unigen_event_->GetPhi();
//    const Double_t mc_phi     = cbm_header_->GetRotZ();
//    delta_phi                 = mc_phi - unigen_phi;
//  }

  sim_tracks_->ClearChannels();
  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  int file_id{0}, event_id{0};
  event_id = FairRootManager::Instance()->GetEntryNr();

  LOG(info) << "Writing MC-tracks from event # " << event_id << " file " << file_id;

  const int nMcTracks = cbm_mc_tracks_->GetEntriesFast();

  if (nMcTracks <= 0) {
    LOG(warn) << "No MC tracks!";
    return;
  }
  const int imother_id = branch.GetFieldId("mother_id");
  const int igeant_id  = branch.GetFieldId("geant_process_id");
  const int in_hits    = branch.GetFieldId("n_hits_mvd");
  const int icbm_id    = branch.GetFieldId("cbmroot_id");
  const int istart_x   = branch.GetFieldId("start_x");

  sim_tracks_->Reserve(nMcTracks);

  const Double_t nsTofmc = 1. / (0.3356 * 1E-15);

  for (int iMcTrack = 0; iMcTrack < nMcTracks; ++iMcTrack) {
    const auto trackIndex = iMcTrack;  //event ? event->GetIndex(ECbmDataType::kMCTrack, iMcTrack) : iMcTrack;
    const auto* mctrack   = (CbmMCTrack*) cbm_mc_tracks_->At(trackIndex);
    if (mctrack->GetPdgCode() == 50000050) {  //Cherenkov
      continue;
    }
    auto& track = sim_tracks_->AddChannel(branch);

    out_indexes_map_.insert(std::make_pair(trackIndex, track.GetId()));

    track.SetMomentum(mctrack->GetPx(), mctrack->GetPy(), mctrack->GetPz());
    track.SetMass(float(mctrack->GetMass()));
    track.SetPid(int(mctrack->GetPdgCode()));
//    track.SetField(int(mctrack->GetGeantProcessId()), igeant_id);
//    track.SetField(int(mctrack->GetNPoints(ECbmModuleId::kMvd)), in_hits);
//    track.SetField(int(mctrack->GetNPoints(ECbmModuleId::kSts)), in_hits + 1);
//    track.SetField(int(mctrack->GetNPoints(ECbmModuleId::kTrd)), in_hits + 2);
    track.SetField(int(mctrack->GetUniqueID()), icbm_id);

    if (mctrack->GetMotherId() >= 0) {  // secondary
      track.SetField(float(mctrack->GetStartX() - cbm_header_->GetX()), istart_x);
      track.SetField(float(mctrack->GetStartY() - cbm_header_->GetY()), istart_x + 1);
      track.SetField(float(mctrack->GetStartZ() - cbm_header_->GetZ()), istart_x + 2);
      track.SetField(float(nsTofmc * (mctrack->GetStartT() - cbm_header_->GetT())), istart_x + 3);
    }
    else {  // primary
//      if (use_unigen_ && trackIndex < unigen_event_->GetNpa()) {
//        UParticle* p            = unigen_event_->GetParticle(trackIndex);
//        TLorentzVector boostedX = p->GetPosition();
//        boostedX.Boost(0, 0, -beta_cm_);
//        boostedX.RotateZ(delta_phi);
//        track.SetField(float(boostedX.X() * 1e-13), istart_x);
//        track.SetField(float(boostedX.Y() * 1e-13), istart_x + 1);
//        track.SetField(float(boostedX.Z() * 1e-13), istart_x + 2);
//        track.SetField(float(boostedX.T()), istart_x + 3);
//      }
//      else {
        track.SetField(0.f, istart_x);
        track.SetField(0.f, istart_x + 1);
        track.SetField(0.f, istart_x + 2);
        track.SetField(0.f, istart_x + 3);
//      }
    }

    // mother id should < than track id, so we can do it here
    if (mctrack->GetMotherId() == -1) { track.SetField(int(-1), imother_id); }
    else {
      auto p = out_indexes_map_.find(mctrack->GetMotherId());
      if (p == out_indexes_map_.end())  // match is not found
        track.SetField(int(-999), imother_id);
      else {
        track.SetField(int(p->second), imother_id);
      }
    }
  }
}
CbmSimTracksConverter::~CbmSimTracksConverter() { delete sim_tracks_; };
