/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "CbmRecEventHeaderConverter.h"

//#include "CbmPsdHit.h"
#include "CbmVertex.h"

#include "FairMCEventHeader.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <AnalysisTree/TaskManager.hpp>

#include "cassert"
#include "iostream"

ClassImp(CbmRecEventHeaderConverter);

void CbmRecEventHeaderConverter::Init()
{
  assert(!out_branch_.empty());
  auto* ioman = FairRootManager::Instance();
  assert(ioman != nullptr);
  cbm_header_      = (FairMCEventHeader*) ioman->GetObject("MCEventHeader.");
  cbm_prim_vertex_ = (CbmVertex*) ioman->GetObject("PrimaryVertex.");
  cbm_sts_tracks_  = (TClonesArray*) ioman->GetObject("StsTrack");
  cbm_psd_hits_    = (TClonesArray*) ioman->GetObject("PsdHit");

  //  ***** RecEventHeader *******
  AnalysisTree::BranchConfig RecEventHeaderBranch("RecEventHeader", AnalysisTree::DetType::kEventHeader);
  RecEventHeaderBranch.AddField<float>("vtx_chi2", "primiry vertex fit chi^2/NDF");
  RecEventHeaderBranch.AddField<int>("M", "total multiplicity in STS(+MVD)");
  RecEventHeaderBranch.AddField<int>("evt_id", "event identifier");
  RecEventHeaderBranch.AddField<float>("start_time", "Start time of the event, ns");
  RecEventHeaderBranch.AddField<float>("end_time", "End time of the event, ns");

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->AddBranch(out_branch_, rec_event_header_, RecEventHeaderBranch);
  rec_event_header_->Init(RecEventHeaderBranch);
}

void CbmRecEventHeaderConverter::ProcessData()
{
  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  if (!cbm_prim_vertex_) { throw std::runtime_error("No fPrimVtx"); }

  rec_event_header_->SetVertexPosition3({cbm_prim_vertex_->GetX(), cbm_prim_vertex_->GetY(), cbm_prim_vertex_->GetZ()});
  rec_event_header_->SetField(float(cbm_prim_vertex_->GetChi2() / cbm_prim_vertex_->GetNDF()),
                              branch.GetFieldId("vtx_chi2"));

  const int n_sts_tracks = cbm_sts_tracks_->GetEntries();
  rec_event_header_->SetField(n_sts_tracks, branch.GetFieldId("M"));

  rec_event_header_->SetField(int(cbm_header_->GetEventID()), branch.GetFieldId("evt_id"));
  rec_event_header_->SetField(float(cbm_header_->GetT()), branch.GetFieldId("start_time"));
  rec_event_header_->SetField(float(cbm_header_->GetT()), branch.GetFieldId("end_time"));
  
  LOG(info) << "CbmRecEventHeaderConverter " << cbm_prim_vertex_->GetX() << " " << cbm_prim_vertex_->GetChi2();


}
