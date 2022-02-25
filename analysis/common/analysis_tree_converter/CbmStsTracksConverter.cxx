/* Copyright (C) 2020-2021 GSI Helmholtzzentrum fuer Schwerionenforschung, Darmstadt
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "CbmStsTracksConverter.h"

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


ClassImp(CbmStsTracksConverter);

void CbmStsTracksConverter::ProcessData()
{
  assert(bmn_global_tracks_ != nullptr);

  out_indexes_map_.clear();

  ReadVertexTracks();
}

CbmStsTracksConverter::~CbmStsTracksConverter()
{
  delete vtx_tracks_;
  delete sim_particles_2_vtx_tracks_;
};

void CbmStsTracksConverter::InitInput()
{
  auto* ioman = FairRootManager::Instance();

  bmn_vertex_ = (CbmVertex*) ioman->GetObject("PrimaryVertex.");
  bmn_global_tracks_ = (TClonesArray*) ioman->GetObject("BmnGlobalTrack");
}

void CbmStsTracksConverter::Init()
{
  InitInput();

  AnalysisTree::BranchConfig vtx_tracks_config(out_branch_, AnalysisTree::DetType::kTrack);
  vtx_tracks_config.AddField<float>("chi2", "chi2 of the track fit");
  vtx_tracks_config.AddFields<float>({"dcax", "dcay", "dcaz"},
                                     "not actuall Distance of Closest Approach, but extrapolated to z=z_vtx");
  vtx_tracks_config.AddField<int>("charge", "charge");
  vtx_tracks_config.AddField<int>("ndf", "number degrees of freedom");
  vtx_tracks_config.AddField<int>("nhits", "number of hits");
  vtx_tracks_config.AddFields<float>({"x", "y", "z", "tx", "ty", "qp"}, "track parameters");

  auto* man = AnalysisTree::TaskManager::GetInstance();

  man->AddBranch(out_branch_, vtx_tracks_, vtx_tracks_config);
}

void CbmStsTracksConverter::ReadVertexTracks()
{
  assert(bmn_vertex_ && bmn_global_tracks_);

  vtx_tracks_->ClearChannels();
  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  const int iq         = branch.GetFieldId("charge");
  const int indf       = branch.GetFieldId("ndf");
  const int ichi2      = branch.GetFieldId("chi2");
  const int inhits     = branch.GetFieldId("nhits");
  const int idcax      = branch.GetFieldId("dcax");
  const int ix      = branch.GetFieldId("x");
  const int itx      = branch.GetFieldId("tx");

  const int n_sts_tracks = bmn_global_tracks_->GetEntries();
  if (n_sts_tracks <= 0) {
    LOG(warn) << "No STS tracks!";
    return;
  }
  vtx_tracks_->Reserve(n_sts_tracks);

  auto vertex_x = bmn_vertex_->GetX();
  auto vertex_y = bmn_vertex_->GetY();
  auto vertex_z = bmn_vertex_->GetZ();

  for (short i_track = 0; i_track < n_sts_tracks; ++i_track) {
    const int track_index = i_track;
    auto* bmn_global_track = dynamic_cast<BmnGlobalTrack*>(bmn_global_tracks_->At(track_index) );
    if (!bmn_global_track) { throw std::runtime_error("empty track!"); }
    auto& track = vtx_tracks_->AddChannel(branch);
    const FairTrackParam* trackParamFirst = bmn_global_track->GetParamFirst();

    float x = trackParamFirst->GetX();
    float y = trackParamFirst->GetY();
    float z = trackParamFirst->GetZ();

    float tx = trackParamFirst->GetTx();
    float ty = trackParamFirst->GetTy();
    float qp = trackParamFirst->GetQp();

    float length = bmn_global_track->GetLength();
    float chi2 = bmn_global_track->GetChi2();
    int ndf = bmn_global_track->GetNDF();
    int n_hits = bmn_global_track->GetNHits();

    TVector3 momRec;
    trackParamFirst->Momentum(momRec);
    const Int_t q = trackParamFirst->GetQp() > 0 ? 1 : -1;

    track.SetMomentum3(momRec);

    track.SetField(int(q), iq);

    track.SetField(float(tx), itx);
    track.SetField(float(ty), itx+1);
    track.SetField(float(qp), itx+2);

    track.SetField(float(chi2), ichi2);

    track.SetField(int(q), iq);
    track.SetField(int(ndf), indf);
    track.SetField(int(n_hits), inhits);

    track.SetField(float(x - vertex_x), idcax);
    track.SetField(float(y - vertex_y), idcax + 1);
    track.SetField(float(z - vertex_z), idcax + 2);

    out_indexes_map_.insert(std::make_pair(track_index, track.GetId()));
  }
}

// TODO misleading name, move field filling somewhere else?