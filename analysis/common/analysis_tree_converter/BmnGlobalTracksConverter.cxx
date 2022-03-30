/* Copyright (C) 2020-2021 GSI Helmholtzzentrum fuer Schwerionenforschung, Darmstadt
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "BmnGlobalTracksConverter.h"

#include "CbmMCTrack.h"
#include "CbmVertex.h"
#include "BmnGlobalTrack.h"
#include "BmnKalmanFilter.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <AnalysisTree/TaskManager.hpp>
#include <cassert>

#include <cmath>
#include <CbmTrackMatch.h>

#include "AnalysisTree/Matching.hpp"


ClassImp(BmnGlobalTracksConverter);

void BmnGlobalTracksConverter::ProcessData()
{
  assert(in_bmn_global_tracks_ != nullptr);
  out_indexes_map_.clear();
  ReadVertexTracks();
}

BmnGlobalTracksConverter::~BmnGlobalTracksConverter()
{
  delete out_global_tracks_;
  delete global_tracks_2_sts_tracks_;
};

void BmnGlobalTracksConverter::InitInput()
{
  auto* ioman = FairRootManager::Instance();

  in_bmn_vertex_ = (CbmVertex*) ioman->GetObject("PrimaryVertex.");
  in_bmn_global_tracks_ = (TClonesArray*) ioman->GetObject("BmnGlobalTrack");
}

void BmnGlobalTracksConverter::Init()
{
  InitInput();

  AnalysisTree::BranchConfig vtx_tracks_config(out_branch_, AnalysisTree::DetType::kTrack);
  vtx_tracks_config.AddField<float>("chi2", "chi2 of the track fit");
  vtx_tracks_config.AddField<float>("beta400", "Beta measured with TOF-400");
  vtx_tracks_config.AddField<float>("beta700", "Beta measured with TOF-700");
  vtx_tracks_config.AddField<float>("length", "length of the track");
  vtx_tracks_config.AddFields<float>({"dcax", "dcay", "dcaz"},
                                     "not actuall Distance of Closest Approach, but extrapolated to z=z_vtx");
  vtx_tracks_config.AddField<int>("charge", "charge");
  vtx_tracks_config.AddField<int>("ndf", "number degrees of freedom");
  vtx_tracks_config.AddField<int>("n_hits", "number of hits");
  vtx_tracks_config.AddFields<float>({"x_first", "y_first", "z_first", "tx_first", "ty_first", "qp_first"}, "first track parameters");
  vtx_tracks_config.AddFields<float>({"x_last", "y_last", "z_last", "tx_last", "ty_last", "qp_last"}, "last track parameters");

  auto* man = AnalysisTree::TaskManager::GetInstance();

  man->AddBranch(out_branch_, out_global_tracks_, vtx_tracks_config);
  man->AddMatching( out_branch_, str_sts_trk_branch_name_, global_tracks_2_sts_tracks_ );
  man->AddMatching( out_branch_, str_tof400_branch_name_, global_tracks_2_tof400_hits_ );
  man->AddMatching( out_branch_, str_tof700_branch_name_, global_tracks_2_tof700_hits_ );
}

void BmnGlobalTracksConverter::ReadVertexTracks()
{
  assert(in_bmn_vertex_ && in_bmn_global_tracks_);
  BmnKalmanFilter kalman_filter;
  out_global_tracks_->ClearChannels();
  global_tracks_2_sts_tracks_->Clear();
  global_tracks_2_tof400_hits_->Clear();
  global_tracks_2_tof700_hits_->Clear();
  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  const int iq         = branch.GetFieldId("charge");
  const int indf       = branch.GetFieldId("ndf");
  const int ilength      = branch.GetFieldId("length");
  const int ichi2      = branch.GetFieldId("chi2");
  const int inhits     = branch.GetFieldId("n_hits");
  const int idcax      = branch.GetFieldId("dcax");
  const int ix_first = branch.GetFieldId("x_first");
  const int ix_last = branch.GetFieldId("x_last");
  const int itx_first = branch.GetFieldId("tx_first");
  const int itx_last = branch.GetFieldId("tx_last");
  const int ibeta400      = branch.GetFieldId("beta400");
  const int ibeta700      = branch.GetFieldId("beta700");

  const int n_global_tracks = in_bmn_global_tracks_->GetEntries();
  if (n_global_tracks <= 0) {
    LOG(warn) << "There are no global tracks tracks!";
    return;
  }
  out_global_tracks_->Reserve(n_global_tracks);

  auto vertex_x = in_bmn_vertex_->GetX();
  auto vertex_y = in_bmn_vertex_->GetY();
  auto vertex_z = in_bmn_vertex_->GetZ();

  for (short i_track = 0; i_track < n_global_tracks; ++i_track) {
    const int track_index = i_track;
    auto*in_bmn_global_track = dynamic_cast<BmnGlobalTrack*>(in_bmn_global_tracks_->At(track_index) );
    if (!in_bmn_global_track) { throw std::runtime_error("empty out_track!"); }
    auto&out_track = out_global_tracks_->AddChannel(branch);
    FairTrackParam* trackParamFirst =
        in_bmn_global_track->GetParamFirst();

    FairTrackParam* trackParamLast =
        in_bmn_global_track->GetParamLast();

    float x_first = trackParamFirst->GetX();
    float y_first = trackParamFirst->GetY();
    float z_first = trackParamFirst->GetZ();

    float tx_first = trackParamFirst->GetTx();
    float ty_first = trackParamFirst->GetTy();
    float qp_first = trackParamFirst->GetQp();

    float x_last = trackParamLast->GetX();
    float y_last = trackParamLast->GetY();
    float z_last = trackParamLast->GetZ();

    float tx_last = trackParamLast->GetTx();
    float ty_last = trackParamLast->GetTy();
    float qp_last = trackParamLast->GetQp();

    float length = in_bmn_global_track->GetLength();
    float chi2 = in_bmn_global_track->GetChi2();
    int ndf = in_bmn_global_track->GetNDF();
    int n_hits = in_bmn_global_track->GetNHits();
    auto beta400 = in_bmn_global_track->GetBeta(1);
    auto beta700 = in_bmn_global_track->GetBeta(2);

    TVector3 momRec;
    trackParamFirst->Momentum(momRec);
    const Int_t q = trackParamFirst->GetQp() > 0 ? 1 : -1;

    out_track.SetMomentum3(momRec);

    out_track.SetField(int(q), iq);

    out_track.SetField(float(tx_first), itx_first);
    out_track.SetField(float(ty_first), itx_first +1);
    out_track.SetField(float(qp_first), itx_first +2);

    out_track.SetField(float(x_first), ix_first);
    out_track.SetField(float(y_first), ix_first +1);
    out_track.SetField(float(z_first), ix_first +2);

    out_track.SetField(float(tx_last), itx_last);
    out_track.SetField(float(ty_last), itx_last +1);
    out_track.SetField(float(qp_last), itx_last +2);

    out_track.SetField(float(x_last), ix_last);
    out_track.SetField(float(y_last), ix_last +1);
    out_track.SetField(float(z_last), ix_last +2);

    out_track.SetField(float(chi2), ichi2);
    out_track.SetField(float(beta400), ibeta400);
    out_track.SetField(float(beta700), ibeta700);
    out_track.SetField(float(length), ilength);

    out_track.SetField(int(q), iq);
    out_track.SetField(int(ndf), indf);
    out_track.SetField(int(n_hits), inhits);

    out_track.SetField(float(x_first - vertex_x), idcax);
    out_track.SetField(float(y_first - vertex_y), idcax + 1);
    out_track.SetField(float(z_first - vertex_z), idcax + 2);

    global_tracks_2_sts_tracks_->AddMatch( track_index, track_index );
    auto idx_tof400 = in_bmn_global_track->GetTof1HitIndex();
    if( idx_tof400 >= 0 )
      global_tracks_2_tof400_hits_->AddMatch( track_index, idx_tof400 );
    auto idx_tof700 = in_bmn_global_track->GetTof2HitIndex();
    if( idx_tof700 >= 0 )
      global_tracks_2_tof700_hits_->AddMatch( track_index, idx_tof700 );
  }
}

// TODO misleading name, move field filling somewhere else?