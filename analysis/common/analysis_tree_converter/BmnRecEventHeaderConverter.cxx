/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "BmnRecEventHeaderConverter.h"

//#include "CbmPsdHit.h"
#include "CbmVertex.h"
#include "BmnFHCalDigit.h"

#include "FairMCEventHeader.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <AnalysisTree/TaskManager.hpp>

#include "cassert"
#include "iostream"

ClassImp(BmnRecEventHeaderConverter);

void BmnRecEventHeaderConverter::Init()
{
  assert(!out_branch_.empty());
  auto* ioman = FairRootManager::Instance();
  assert(ioman != nullptr);
  in_event_header_ = (FairMCEventHeader*) ioman->GetObject("MCEventHeader.");
  in_prim_vertex_ = (CbmVertex*) ioman->GetObject("PrimaryVertex.");
  in_global_tracks_ = (TClonesArray*) ioman->GetObject("BmnGlobalTrack");
  in_fhcal_digits_ = (TClonesArray*) ioman->GetObject("FHCalDigit");

  //  ***** RecEventHeader *******
  AnalysisTree::BranchConfig RecEventHeaderBranch("RecEventHeader", AnalysisTree::DetType::kEventHeader);
  RecEventHeaderBranch.AddField<float>("vtx_chi2", "primiry vertex fit chi^2/NDF");
  RecEventHeaderBranch.AddField<int>("M", "total global tracks multiplicity");
  RecEventHeaderBranch.AddField<float>("total_fhcal_energy", "total energy in FHCal");
  RecEventHeaderBranch.AddField<int>("evt_id", "event identifier");
  RecEventHeaderBranch.AddField<float>("start_time", "Start time of the event, ns");
  RecEventHeaderBranch.AddField<float>("end_time", "End time of the event, ns");

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->AddBranch(out_branch_, rec_event_header_, RecEventHeaderBranch);
  rec_event_header_->Init(RecEventHeaderBranch);
}

void BmnRecEventHeaderConverter::ProcessData()
{
  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  if (!in_prim_vertex_) { throw std::runtime_error("No fPrimVtx"); }

  rec_event_header_->SetVertexPosition3({in_prim_vertex_->GetX(),
                                         in_prim_vertex_->GetY(),
                                         in_prim_vertex_->GetZ()});
  rec_event_header_->SetField(float(in_prim_vertex_->GetChi2() / in_prim_vertex_->GetNDF()),
                              branch.GetFieldId("vtx_chi2"));

  const int n_global_tracks = in_global_tracks_->GetEntries();
  rec_event_header_->SetField(n_global_tracks, branch.GetFieldId("M"));

  rec_event_header_->SetField(int(in_event_header_->GetEventID()), branch.GetFieldId("evt_id"));
  rec_event_header_->SetField(float(in_event_header_->GetT()), branch.GetFieldId("start_time"));
  rec_event_header_->SetField(float(in_event_header_->GetT()), branch.GetFieldId("end_time"));

  float e_fhcal = 0;
  for (int i = 0; i < in_fhcal_digits_->GetEntriesFast(); ++i) {
    auto* in_digit = dynamic_cast<BmnFHCalDigit*>(in_fhcal_digits_->At(i));
    if( in_digit->GetModuleID() == 0 )
      continue;
    if( in_digit->GetSectionID() != 0 )
      continue;
    float energy_scaled = in_digit->GetELossDigi();
    e_fhcal+=energy_scaled;
  }
  rec_event_header_->SetField(float(e_fhcal), branch.GetFieldId("total_fhcal_energy"));

  LOG(info) << "BmnRecEventHeaderConverter " << in_prim_vertex_->GetX() << " " << in_prim_vertex_->GetChi2();
}
