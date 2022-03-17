/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "BmnTofHitsConverter.h"

#include <BmnGlobalTrack.h>
#include <BmnTofHit.h>

#include <FairMCPoint.h>
#include <FairRootManager.h>

#include "TClonesArray.h"

#include <AnalysisTree/TaskManager.hpp>
#include <cassert>

#include "AnalysisTree/Matching.hpp"

ClassImp(BmnTofHitsConverter);

void BmnTofHitsConverter::Init()
{
  assert(!out_branch_.empty());
  auto* ioman = FairRootManager::Instance();

  std::string str_in_tof_hits;

  if( tof_type_ == BMNTOF::TOF400 )
    str_in_tof_hits = "BmnTof400Hit";
  if( tof_type_ == BMNTOF::TOF700 )
    str_in_tof_hits = "BmnTof700Hit";
  in_bmn_tof_hits_ = (TClonesArray*) ioman->GetObject(str_in_tof_hits.c_str());
  if( !in_bmn_tof_hits_ )
    throw std::runtime_error( "There is no branch for TOF hits" );
  std::cout << "Branch " << str_in_tof_hits << " have been successfully read from tree" << std::endl;
  in_bmn_global_tracks_ = (TClonesArray*) ioman->GetObject("BmnGlobalTrack");
  if( !in_bmn_global_tracks_ )
    throw std::runtime_error( "There is no branch for Global tracks" );

  AnalysisTree::BranchConfig tof_branch_config(out_branch_, AnalysisTree::DetType::kHit);
  tof_branch_config.AddField<float>("mass2", "Mass squared");
  tof_branch_config.AddField<float>("beta", "Beta v/c");
  tof_branch_config.AddField<float>("length", "Track length");
  tof_branch_config.AddField<float>("time", "ps(?), Measured time ");

  tof_branch_config.AddField<float>("error_x");
  tof_branch_config.AddField<float>("error_y");
  tof_branch_config.AddField<float>("error_z");

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->AddBranch(out_branch_, out_tof_hits_, tof_branch_config);
}

void BmnTofHitsConverter::ProcessData()
{
  assert(in_bmn_tof_hits_);
  this->MapTracks();
  out_tof_hits_->ClearChannels();

  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  const int i_mass2 = branch.GetFieldId("mass2");
  const int i_beta = branch.GetFieldId("beta");
  const int i_t     = branch.GetFieldId("time");
  const int i_l     = branch.GetFieldId("length");
  const int i_dx    = branch.GetFieldId("error_x");

  const int n_global_tracks = in_bmn_global_tracks_->GetEntries();
  const int n_tof_hits = in_bmn_tof_hits_->GetEntries();

  for (Int_t idx_tof_hit = 0; idx_tof_hit < n_tof_hits; idx_tof_hit++) {

    auto* in_tof_hit = dynamic_cast<BmnTofHit*>(in_bmn_tof_hits_->At(idx_tof_hit));
    if( !in_tof_hit )
      throw std::runtime_error( "TOF hit is empty" );

    Float_t p    = 0;
    Int_t q      = 0;
    Float_t l    = 0;

    try{
      auto idx_global_trk = tof_hit_idx_2_global_trk_idx_.at(idx_tof_hit);
      auto in_global_trk = dynamic_cast<BmnGlobalTrack*>(in_bmn_global_tracks_->At(idx_global_trk) );
      auto track_param = in_global_trk->GetParamLast();
      TVector3 mom3;
      track_param->Momentum(mom3);
      p = mom3.Mag();
      q = track_param->GetQp() > 0 ? 1 : -1;
      l = in_global_trk->GetLength();
    }catch(std::exception&){}

    const Float_t time = in_tof_hit->GetTimeStamp();

    const Float_t beta = l / (time * 29.9792458);
    const Float_t m2   = p * p * (1. / (beta * beta) - 1.);

    const Float_t hit_x = in_tof_hit->GetX();
    const Float_t hit_y = in_tof_hit->GetY();
    const Float_t hit_z = in_tof_hit->GetZ();

    const Float_t hit_xerr = in_tof_hit->GetDx();
    const Float_t hit_yerr = in_tof_hit->GetDy();
    const Float_t hit_zerr = in_tof_hit->GetDz();

    auto&out_hit = out_tof_hits_->AddChannel(branch);

    out_hit.SetPosition(hit_x, hit_y, hit_z);
    out_hit.SetSignal(time);
    out_hit.SetField(m2, i_mass2);
    out_hit.SetField(beta, i_beta);
    out_hit.SetField(l, i_l);
    out_hit.SetField(time, i_t);
    out_hit.SetField(hit_xerr, i_dx);
    out_hit.SetField(hit_yerr, i_dx+1);
    out_hit.SetField(hit_zerr, i_dx+2);
  }
}

BmnTofHitsConverter::~BmnTofHitsConverter()
{
  delete out_tof_hits_;
}
void BmnTofHitsConverter::MapTracks(){
    tof_hit_idx_2_global_trk_idx_.clear();
    auto n_global_tracks = in_bmn_global_tracks_->GetEntries();
    for( int idx_global_trk=0; idx_global_trk < n_global_tracks; ++idx_global_trk ){
      auto in_global_trk = dynamic_cast<BmnGlobalTrack*>(in_bmn_global_tracks_->At(idx_global_trk) );
      auto tof_idx=-1;
      if( tof_type_ == BMNTOF::TOF400 )
        tof_idx=in_global_trk->GetTof1HitIndex();
      if( tof_type_ == BMNTOF::TOF700 )
        tof_idx=in_global_trk->GetTof2HitIndex();
      if( tof_idx < 0 )
        continue;
      tof_hit_idx_2_global_trk_idx_.insert({tof_idx, idx_global_trk});
    }
};
