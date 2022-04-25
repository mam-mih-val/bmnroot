/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universität Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "BmnFHCalModulesConverter.h"

#include "BmnFHCalDigit.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <AnalysisTree/TaskManager.hpp>
#include <cassert>
#include <vector>

#include "AnalysisTree/Detector.hpp"

ClassImp(BmnFHCalModulesConverter);

void BmnFHCalModulesConverter::Init()
{
  assert(!out_branch_.empty());
  auto* ioman = FairRootManager::Instance();
  assert(ioman != nullptr);
  in_fhcal_event_ = dynamic_cast<BmnFHCalEvent*>(ioman->GetObject("FHCalEvent"));

  AnalysisTree::BranchConfig out_fhcal_branch_config_(out_branch_, AnalysisTree::DetType::kHit);
  out_fhcal_branch_config_.AddField<int>("id", "ID of the module");

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->AddBranch(out_branch_, out_fhcal_branch_, out_fhcal_branch_config_);
}


void BmnFHCalModulesConverter::ProcessData()
{
  if(!in_fhcal_event_)
    throw std::runtime_error( std::string(__func__) + ": Input FHCal event is not initialized");

  out_fhcal_branch_->ClearChannels();

  auto* data_header  = AnalysisTree::TaskManager::GetInstance()->GetDataHeader();
  auto* config       = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto&out_branch_config = config->GetBranchConfig(out_branch_);

  auto i_id = out_branch_config.GetFieldId("id");

  const int n_fhcal_modules = data_header->GetModulePositions(0).GetNumberOfChannels();
  for (int idx = 1; idx <= n_fhcal_modules; ++idx) {
    auto in_module = in_fhcal_event_->GetModule(idx);
    auto energy = in_module->GetEnergy();

    if( energy < std::numeric_limits<float>::min() )
      continue;

    auto x = in_module->GetX();
    auto y = in_module->GetY();

    auto&out_module = out_fhcal_branch_->AddChannel(out_branch_config);
    out_module.SetField( int(idx), i_id );
    out_module.SetSignal(energy);
    out_module.SetPosition(x, y, 900); // FIXME: hardcoded z position of FHCal
  }
}

void BmnFHCalModulesConverter::Finish() {}

BmnFHCalModulesConverter::~BmnFHCalModulesConverter() { delete out_fhcal_branch_; };
