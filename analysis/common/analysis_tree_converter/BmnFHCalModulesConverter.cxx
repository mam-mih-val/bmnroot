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

  AnalysisTree::BranchConfig out_fhcal_branch_config_(out_branch_, AnalysisTree::DetType::kModule);

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


  const int n_fhcal_modules = data_header->GetModulePositions(0).GetNumberOfChannels();
  for( int i=0; i<n_fhcal_modules; ++i ){
    auto& out_module = out_fhcal_branch_->AddChannel(out_branch_config);
  }
  for (int idx = 0; idx < n_fhcal_modules; ++idx) {
    auto in_module = in_fhcal_event_->GetModule(idx+1);
    auto energy = in_module->GetEnergy();

    auto&out_module = out_fhcal_branch_->Channel(idx);
    out_module.SetSignal(energy);
    out_module.SetNumber(idx);

    if( 33 < idx && idx < 44 ){
      auto rel_id = 54 + ( idx - 34 );
      auto& ext_module = out_fhcal_branch_->Channel( rel_id );
      ext_module.SetSignal( energy );
    }
    if( 43 < idx && idx < 54 ){
      auto rel_id = 54 + ( idx - 44 );
      auto& ext_module = out_fhcal_branch_->Channel( rel_id );
      ext_module.SetSignal( energy );
    }
  }
}

void BmnFHCalModulesConverter::Finish() {}

BmnFHCalModulesConverter::~BmnFHCalModulesConverter() { delete out_fhcal_branch_; };
