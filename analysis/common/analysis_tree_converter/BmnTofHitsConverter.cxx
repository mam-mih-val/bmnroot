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

  if( tof_type_ == BMNTOF::TOF400 )
    in_bmn_tof_hits_ = (TClonesArray*) ioman->GetObject("BmnTof400Hit");
  if( tof_type_ == BMNTOF::TOF700 )
    in_bmn_tof_hits_ = (TClonesArray*) ioman->GetObject("BmnTof700Hit");

  in_bmn_global_tracks_ = (TClonesArray*) ioman->GetObject("BmnGlobalTrack");

  AnalysisTree::BranchConfig tof_branch(out_branch_, AnalysisTree::DetType::kHit);
  tof_branch.AddField<float>("mass2", "Mass squared");
  tof_branch.AddField<float>("length", "Track length");
  tof_branch.AddField<float>("time", "ps(?), Measured time ");

  tof_branch.AddField<float>("error_x");
  tof_branch.AddField<float>("error_y");
  tof_branch.AddField<float>("error_z");

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->AddBranch(out_branch_, out_tof_hits_, tof_branch);
}

void BmnTofHitsConverter::ProcessData()
{
  assert(in_bmn_tof_hits_);
  out_tof_hits_->ClearChannels();

  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  const int i_mass2 = branch.GetFieldId("mass2");
  const int i_t     = branch.GetFieldId("time");
  const int i_l     = branch.GetFieldId("length");
  const int i_dx    = branch.GetFieldId("error_x");

  const int n_global_tracks = in_bmn_tof_hits_->GetEntriesFast();

  for (Int_t idx_global_track = 0; idx_global_track < n_global_tracks; idx_global_track++) {
    auto in_global_trk = dynamic_cast<BmnGlobalTrack*>(in_bmn_global_tracks_->At(idx_global_track) );
    int idx_tof_hit=-1;
    if( tof_type_ == BMNTOF::TOF400 )
      idx_tof_hit = in_global_trk->GetTof1HitIndex();
    if(tof_type_ == BMNTOF::TOF700)
      idx_tof_hit = in_global_trk->GetTof2HitIndex();

    if( idx_tof_hit < 0 )
      continue;
    auto* in_tof_hit = dynamic_cast<BmnTofHit*>(in_bmn_tof_hits_->At(idx_tof_hit));

    auto track_param = in_global_trk->GetParamLast();
    TVector3 mom3;
    track_param->Momentum(mom3);
    const Float_t p    = mom3.Mag();
    const Int_t q      = track_param->GetQp() > 0 ? 1 : -1;

    const Float_t l    = in_global_trk->GetLength();
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
};
