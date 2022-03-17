/* Copyright (C) 2020-2021 GSI Helmholtzzentrum fuer Schwerionenforschung, Darmstadt
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "BmnGlobalTracksConverter.h"

#include "CbmMCTrack.h"
#include "CbmVertex.h"
#include "BmnGlobalTrack.h"

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
  vtx_tracks_config.AddField<float>("length", "length of the track");
  vtx_tracks_config.AddFields<float>({"dcax", "dcay", "dcaz"},
                                     "not actuall Distance of Closest Approach, but extrapolated to z=z_vtx");
  vtx_tracks_config.AddField<int>("charge", "charge");
  vtx_tracks_config.AddField<int>("ndf", "number degrees of freedom");
  vtx_tracks_config.AddField<int>("nhits", "number of hits");
  vtx_tracks_config.AddFields<float>({"x", "y", "z", "tx", "ty", "qp"}, "track parameters");

  auto* man = AnalysisTree::TaskManager::GetInstance();

  man->AddBranch(out_branch_, out_global_tracks_, vtx_tracks_config);
  man->AddMatching( out_branch_, str_sts_trk_branch_name_, global_tracks_2_sts_tracks_ );
  man->AddMatching( out_branch_, str_tof400_branch_name_, global_tracks_2_tof400_hits_ );
  man->AddMatching( out_branch_, str_tof700_branch_name_, global_tracks_2_tof700_hits_ );
}

void BmnGlobalTracksConverter::ReadVertexTracks()
{
  assert(in_bmn_vertex_ && in_bmn_global_tracks_);

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
  const int inhits     = branch.GetFieldId("nhits");
  const int idcax      = branch.GetFieldId("dcax");
  const int ix      = branch.GetFieldId("x");
  const int itx      = branch.GetFieldId("tx");

  const int n_sts_tracks = in_bmn_global_tracks_->GetEntries();
  if (n_sts_tracks <= 0) {
    LOG(warn) << "No STS tracks!";
    return;
  }
  out_global_tracks_->Reserve(n_sts_tracks);

  auto vertex_x = in_bmn_vertex_->GetX();
  auto vertex_y = in_bmn_vertex_->GetY();
  auto vertex_z = in_bmn_vertex_->GetZ();

  for (short i_track = 0; i_track < n_sts_tracks; ++i_track) {
    const int track_index = i_track;
    auto*in_bmn_global_track = dynamic_cast<BmnGlobalTrack*>(in_bmn_global_tracks_->At(track_index) );
    if (!in_bmn_global_track) { throw std::runtime_error("empty track!"); }
    auto& track = out_global_tracks_->AddChannel(branch);
    const FairTrackParam* trackParamFirst =
        in_bmn_global_track->GetParamFirst();

    float x = trackParamFirst->GetX();
    float y = trackParamFirst->GetY();
    float z = trackParamFirst->GetZ();

    float tx = trackParamFirst->GetTx();
    float ty = trackParamFirst->GetTy();
    float qp = trackParamFirst->GetQp();

    float length = in_bmn_global_track->GetLength();
    float chi2 = in_bmn_global_track->GetChi2();
    int ndf = in_bmn_global_track->GetNDF();
    int n_hits = in_bmn_global_track->GetNHits();

    TVector3 momRec;
    trackParamFirst->Momentum(momRec);
    const Int_t q = trackParamFirst->GetQp() > 0 ? 1 : -1;

    track.SetMomentum3(momRec);

    track.SetField(int(q), iq);

    track.SetField(float(tx), itx);
    track.SetField(float(ty), itx+1);
    track.SetField(float(qp), itx+2);

    track.SetField(float(x), ix);
    track.SetField(float(y), ix+1);
    track.SetField(float(z), ix+2);

    track.SetField(float(chi2), ichi2);
    track.SetField(float(length), ilength);

    track.SetField(int(q), iq);
    track.SetField(int(ndf), indf);
    track.SetField(int(n_hits), inhits);

    track.SetField(float(x - vertex_x), idcax);
    track.SetField(float(y - vertex_y), idcax + 1);
    track.SetField(float(z - vertex_z), idcax + 2);

    global_tracks_2_sts_tracks_->AddMatch( track_index, track_index );
    auto idx_tof400 = in_bmn_global_track->GetTof1HitIndex();
    if( idx_tof400 > 0 )
      global_tracks_2_tof400_hits_->AddMatch( track_index, idx_tof400 );
    auto idx_tof700 = in_bmn_global_track->GetTof2HitIndex();
    if( idx_tof700 > 0 )
      global_tracks_2_tof700_hits_->AddMatch( track_index, idx_tof700 );
  }
}

// TODO misleading name, move field filling somewhere else?