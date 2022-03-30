/* Copyright (C) 2020-2021 GSI Helmholtzzentrum fuer Schwerionenforschung, Darmstadt
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "BmnStsTracksConverter.h"

#include "CbmVertex.h"
#include "CbmStsTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <AnalysisTree/TaskManager.hpp>
#include <cassert>

#include <cmath>
#include <CbmTrackMatch.h>

#include "AnalysisTree/Matching.hpp"
#include "CbmL1PFFitter.h"
#include "L1Field.h"
#include "CbmKFVertex.h"

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
  vtx_tracks_config.AddField<float>("chi2_vertex", "chi2 of the track extrapolation to the vertex");
  vtx_tracks_config.AddField<float>("length", "length of the track");
  vtx_tracks_config.AddFields<float>({"dcax", "dcay", "dcaz"},
                                     "not actuall Distance of Closest Approach, but extrapolated to z=z_vtx");
  vtx_tracks_config.AddField<int>("charge", "charge");
  vtx_tracks_config.AddField<int>("ndf", "number degrees of freedom");
  vtx_tracks_config.AddField<int>("n_hits", "number of hits in STS+GEM");
  vtx_tracks_config.AddFields<float>({"x_first", "y_first", "z_first", "tx_first", "ty_first", "qp_first"}, "first track parameters");
  vtx_tracks_config.AddFields<float>({"x_last", "y_last", "z_last", "tx_last", "ty_last", "qp_last"}, "last track parameters");

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
  const int inhits     = branch.GetFieldId("n_hits");
  const int ichi2      = branch.GetFieldId("chi2");
  const int ichi2_vertex      = branch.GetFieldId("chi2_vertex");
  const int idcax      = branch.GetFieldId("dcax");
  const int ix_first = branch.GetFieldId("x_first");
  const int ix_last = branch.GetFieldId("x_last");
  const int itx_first = branch.GetFieldId("tx_first");
  const int itx_last = branch.GetFieldId("tx_last");

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

    float chi2_vertex = -999.;
//    chi2_vertex = ExtrapolateToVertex( in_sts_track, 2212 );

    const FairTrackParam* trackParamFirst = in_sts_track->GetParamFirst();
    const FairTrackParam* trackParamLast = in_sts_track->GetParamLast();

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

    float chi2 = in_sts_track->GetChi2();
    int ndf = in_sts_track->GetNDF();
    int n_hits = in_sts_track->GetNStsHits();

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
    out_track.SetField(float(chi2_vertex), ichi2_vertex);

    out_track.SetField(int(q), iq);
    out_track.SetField(int(ndf), indf);
    out_track.SetField(int(n_hits), inhits);

    out_track.SetField(float(x_first - vertex_x), idcax);
    out_track.SetField(float(y_first - vertex_y), idcax + 1);
    out_track.SetField(float(z_first - vertex_z), idcax + 2);
  }
}
float BmnStsTracksConverter::ExtrapolateToVertex(CbmStsTrack* sts_track, int pdg){
  std::vector<CbmStsTrack> tracks = {*sts_track};
  CbmL1PFFitter fitter;
  std::vector<float> chi2_to_vtx;
  std::vector<L1FieldRegion> field;
  CbmKFVertex kfVertex = CbmKFVertex(*in_bmn_vertex_);
  std::cout << "Fitting the track to the vertex" << std::endl;
  fitter.Fit(tracks, pdg);
  std::cout << "Calculating the chi2 of the extrapolation" << std::endl;
  fitter.GetChiToVertex(tracks, field, chi2_to_vtx, kfVertex, 3.);
  *sts_track = tracks[0];
  return chi2_to_vtx[0];
}
// TODO misleading name, move field filling somewhere else?