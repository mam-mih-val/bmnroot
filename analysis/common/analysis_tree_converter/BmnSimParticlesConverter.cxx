/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Daniel Wielanek, Viktor Klochkov [committer] */

#include "BmnSimParticlesConverter.h"
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
#include <TDatabasePDG.h>

#include "AnalysisTree/TaskManager.hpp"
#include "BmnGlobalTrack.h"

ClassImp(BmnSimParticlesConverter);

void BmnSimParticlesConverter::Init()
{
  assert(!out_branch_.empty());
  auto* ioman = FairRootManager::Instance();

  bmn_mc_tracks_ = (TClonesArray*) ioman->GetObject("MCTrack");
  bmn_global_tracks_ = (TClonesArray*) ioman->GetObject("BmnGlobalTrack");

  bmn_header_ = (FairMCEventHeader*) ioman->GetObject("MCEventHeader.");

  AnalysisTree::BranchConfig sim_particles_branch(out_branch_, AnalysisTree::DetType::kParticle);
  sim_particles_branch.AddField<int>("mother_id", "id of mother particle, -1 for primaries");
  sim_particles_branch.AddField<int>("geant_process_id", "");

  sim_particles_branch.AddFields<float>({"start_x", "start_y", "start_z"}, "Start position, cm");
  sim_particles_branch.AddField<float>("start_t", "t freezout coordinate fm/c");

  auto* man = AnalysisTree::TaskManager::GetInstance();

  man->AddBranch(out_branch_, sim_tracks_, sim_particles_branch);
  man->AddMatching(out_branch_, str_global_trk_branch_name_,
                   sim_particles_2_global_tracks_);
  man->AddMatching(out_branch_, str_sts_trk_branch_name_,
                   sim_particles_2_sts_tracks_);
}

void BmnSimParticlesConverter::MapTracks(){
  assert( bmn_global_tracks_ );
  sim_partcles_bmn_tracks_map_.clear();
  const int n_sts_tracks = bmn_global_tracks_->GetEntries();
  for (short i_track = 0; i_track < n_sts_tracks; ++i_track) {
    const int track_index = i_track;
    auto *bmn_global_track = dynamic_cast<BmnGlobalTrack *>(bmn_global_tracks_->At(track_index));
    if (!bmn_global_track) { throw std::runtime_error("empty track!"); }
    auto sim_match = bmn_global_track->GetRefIndex();
    sim_partcles_bmn_tracks_map_.emplace( std::pair{ sim_match, track_index } );
  }
}

void BmnSimParticlesConverter::ProcessData()
{
  assert(bmn_mc_tracks_);
  out_indexes_map_.clear();
  MapTracks();

  sim_tracks_->ClearChannels();
  sim_particles_2_global_tracks_->Clear();
  sim_particles_2_sts_tracks_->Clear();
  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  const int nMcTracks = bmn_mc_tracks_->GetEntriesFast();

  if (nMcTracks <= 0) {
    LOG(warn) << "No MC tracks!";
    return;
  }
  const int imother_id = branch.GetFieldId("mother_id");
  const int igeant_id  = branch.GetFieldId("geant_process_id");
  const int istart_x   = branch.GetFieldId("start_x");

  sim_tracks_->Reserve(nMcTracks);
  LOG(info) << "Number of MC tracks: " << nMcTracks;

  const Double_t nsTofmc = 1. / (0.3356 * 1E-15);

  auto* data_header  = AnalysisTree::TaskManager::GetInstance()->GetDataHeader();
  auto y_beam = data_header->GetBeamRapidity();
  auto beta_beam = tanh(y_beam);

  int passed_idx = 0;
  for (int iMcTrack = 0; iMcTrack < nMcTracks; ++iMcTrack) {
    const auto trackIndex = iMcTrack;  //event ? event->GetIndex(ECbmDataType::kMCTrack, iMcTrack) : iMcTrack;
    const auto* mctrack   = (CbmMCTrack*)bmn_mc_tracks_->At(trackIndex);
    if (mctrack->GetPdgCode() == 50000050) {  //Cherenkov
      continue;
    }
    
    if(mctrack->GetStartZ() > 200){ // NOTE!!
      continue;
    }
    auto& track = sim_tracks_->AddChannel(branch);
    try {
      sim_particles_2_global_tracks_->AddMatch(
          passed_idx, sim_partcles_bmn_tracks_map_.at(trackIndex));
      sim_particles_2_sts_tracks_->AddMatch(
          passed_idx, sim_partcles_bmn_tracks_map_.at(trackIndex));
    }catch(std::exception&){}
    passed_idx++;
    out_indexes_map_.insert(std::make_pair(trackIndex, track.GetId()));

    float mass = 0;
    auto database_particle = TDatabasePDG::Instance()->GetParticle( mctrack->GetPdgCode() );
    if( database_particle ) {
      mass = database_particle->Mass();
    } else {
      auto proton_mass = TDatabasePDG::Instance()->GetParticle( 2212 )->Mass();
      auto neutron_mass = TDatabasePDG::Instance()->GetParticle( 2112 )->Mass();
      auto charge_number = GetIonCharge( mctrack->GetPdgCode() );
      auto mass_number = GetIonMass( mctrack->GetPdgCode() );
      mass = charge_number*proton_mass + (mass_number - charge_number)*neutron_mass;
    }
    track.SetMass(float(mass));
    track.SetPid(int(mctrack->GetPdgCode()));
    TVector3 mom3;
    mctrack->GetMomentum(mom3);
    TLorentzVector mom4;
    mom4.SetVectM(mom3, mass);
    mom4.Boost({0., 0., beta_beam});
    track.SetMomentum(mom4.Px(), mom4.Py(), mom4.Pz());

    if (mctrack->GetMotherId() >= 0) {  // secondary
      track.SetField(float(mctrack->GetStartX() - bmn_header_->GetX()), istart_x);
      track.SetField(float(mctrack->GetStartY() - bmn_header_->GetY()), istart_x + 1);
      track.SetField(float(mctrack->GetStartZ() - bmn_header_->GetZ()), istart_x + 2);
      track.SetField(float(nsTofmc * (mctrack->GetStartT() - bmn_header_->GetT())), istart_x + 3);
    }
    else {  // primary
      track.SetField(0.f, istart_x);
      track.SetField(0.f, istart_x + 1);
      track.SetField(0.f, istart_x + 2);
      track.SetField(0.f, istart_x + 3);
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
BmnSimParticlesConverter::~BmnSimParticlesConverter() { delete sim_tracks_; };
