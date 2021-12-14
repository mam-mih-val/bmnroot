/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "CbmSimEventHeaderConverter.h"

//#include "CbmEvent.h"

#include "FairMCEventHeader.h"
#include "FairRootManager.h"
//#include "CbmMCDataManager.h"
//#include "CbmMCDataArray.h"
//#include "CbmMCDataObject.h"

#include "TClonesArray.h"

#include <AnalysisTree/TaskManager.hpp>

#include "cassert"
#include "iostream"

ClassImp(CbmSimEventHeaderConverter);

void CbmSimEventHeaderConverter::Init()
{
  assert(!out_branch_.empty());
  auto* ioman = FairRootManager::Instance();
  assert(ioman != nullptr);

  cbm_header_ = (FairMCEventHeader*) ioman->GetObject("MCEventHeader.");

  //  ***** SimEventHeader *******
  AnalysisTree::BranchConfig SimEventHeaderBranch("SimEventHeader", AnalysisTree::DetType::kEventHeader);
  SimEventHeaderBranch.AddField<float>("psi_RP", "reaction plane orientation");
  SimEventHeaderBranch.AddField<float>("b", "fm, impact parameter");
  SimEventHeaderBranch.AddFields<float>({"start_time", "end_time"}, "ns (?), event time");
  SimEventHeaderBranch.AddField<int>("run_id", "run identifier");
  SimEventHeaderBranch.AddField<int>("event_id", "event identifier");

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->AddBranch(out_branch_, sim_event_header_, SimEventHeaderBranch);
  sim_event_header_->Init(SimEventHeaderBranch);
}

void CbmSimEventHeaderConverter::ProcessData()
{
  FairMCEventHeader* cbm_header = cbm_header_;

  if (!cbm_header) { throw std::runtime_error("CbmSimEventHeaderConverter::Exec - ERROR! No fHeader!"); }
  auto* out_config_  = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& branch = out_config_->GetBranchConfig(out_branch_);

  TVector3 pos {cbm_header->GetX(), cbm_header->GetY(), cbm_header->GetZ()};
  sim_event_header_->SetVertexPosition3(pos);

  sim_event_header_->SetField(float(cbm_header->GetRotZ()), branch.GetFieldId("psi_RP"));
  sim_event_header_->SetField(float(cbm_header->GetB()), branch.GetFieldId("b"));
  sim_event_header_->SetField(int(cbm_header->GetEventID()), branch.GetFieldId("event_id"));
  sim_event_header_->SetField(int(cbm_header->GetRunID()), branch.GetFieldId("run_id"));
  
  LOG(info) << "CbmSimEventHeaderConverter " << cbm_header->GetX() << " " << cbm_header->GetB() << " " << cbm_header->GetEventID();
  
}
