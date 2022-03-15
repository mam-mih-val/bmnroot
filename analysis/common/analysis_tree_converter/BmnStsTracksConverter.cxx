/* Copyright (C) 2020-2021 GSI Helmholtzzentrum fuer Schwerionenforschung, Darmstadt
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "BmnStsTracksConverter.h"

#include "CbmMCTrack.h"
#include "CbmVertex.h"
#include "CbmStsTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <AnalysisTree/TaskManager.hpp>
#include <cassert>

#include <cmath>
#include <CbmTrackMatch.h>

#include "AnalysisTree/Matching.hpp"


ClassImp(BmnStsTracksConverter);

void BmnStsTracksConverter::ProcessData()
{
  assert(in_sts_tracks_ != nullptr);
  out_indexes_map_.clear();
  ReadVertexTracks();
}

BmnStsTracksConverter::~BmnStsTracksConverter()
{
  delete out_sts_tracks_;
};

void BmnStsTracksConverter::InitInput()
{
  auto* ioman = FairRootManager::Instance();

  in_bmn_vertex_ = (CbmVertex*) ioman->GetObject("PrimaryVertex.");
  in_sts_tracks_ = (TClonesArray*) ioman->GetObject("StsTrack");
}

void BmnStsTracksConverter::Init()
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

  man->AddBranch(out_branch_, out_sts_tracks_, vtx_tracks_config);
}

void BmnStsTracksConverter::ReadVertexTracks()
{
  assert(in_bmn_vertex_ && in_sts_tracks_);

  out_sts_tracks_->ClearChannels();
  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  const int iq         = branch.GetFieldId("charge");
  const int indf       = branch.GetFieldId("ndf");
  const int ichi2      = branch.GetFieldId("chi2");
  const int idcax      = branch.GetFieldId("dcax");
  const int ix      = branch.GetFieldId("x");
  const int itx      = branch.GetFieldId("tx");

  const int n_sts_tracks = in_sts_tracks_->GetEntries();
  if (n_sts_tracks <= 0) {
    LOG(warn) << "No STS tracks!";
    return;
  }
  out_sts_tracks_->Reserve(n_sts_tracks);

  auto vertex_x = in_bmn_vertex_->GetX();
  auto vertex_y = in_bmn_vertex_->GetY();
  auto vertex_z = in_bmn_vertex_->GetZ();

  for (short i_track = 0; i_track < n_sts_tracks; ++i_track) {
    const int track_index = i_track;
    auto*in_sts_track = dynamic_cast<CbmStsTrack*>(in_sts_tracks_->At(track_index) );
    if (!in_sts_track) { throw std::runtime_error("empty out_track!"); }
    auto&out_track = out_sts_tracks_->AddChannel(branch);
    const FairTrackParam* trackParamFirst = in_sts_track->GetParamFirst();

    float x = trackParamFirst->GetX();
    float y = trackParamFirst->GetY();
    float z = trackParamFirst->GetZ();

    float tx = trackParamFirst->GetTx();
    float ty = trackParamFirst->GetTy();
    float qp = trackParamFirst->GetQp();

    float chi2 = in_sts_track->GetChi2();
    int ndf = in_sts_track->GetNDF();

    TVector3 momRec;
    trackParamFirst->Momentum(momRec);
    const Int_t q = trackParamFirst->GetQp() > 0 ? 1 : -1;

    out_track.SetMomentum3(momRec);

    out_track.SetField(int(q), iq);

    out_track.SetField(float(tx), itx);
    out_track.SetField(float(ty), itx+1);
    out_track.SetField(float(qp), itx+2);

    out_track.SetField(float(x), ix);
    out_track.SetField(float(y), ix+1);
    out_track.SetField(float(z), ix+2);

    out_track.SetField(float(chi2), ichi2);

    out_track.SetField(int(q), iq);
    out_track.SetField(int(ndf), indf);

    out_track.SetField(float(x - vertex_x), idcax);
    out_track.SetField(float(y - vertex_y), idcax + 1);
    out_track.SetField(float(z - vertex_z), idcax + 2);
  }
}

// TODO misleading name, move field filling somewhere else?