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
  in_fhcal_digits_ = (TClonesArray*) ioman->GetObject("FHCalDigit");

  AnalysisTree::BranchConfig out_fhcal_branch_config_(out_branch_, AnalysisTree::DetType::kModule);

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->AddBranch(out_branch_, out_fhcal_branch_, out_fhcal_branch_config_);
}


void BmnFHCalModulesConverter::ProcessData()
{
  assert(in_fhcal_digits_);
  out_fhcal_branch_->ClearChannels();

  auto* data_header  = AnalysisTree::TaskManager::GetInstance()->GetDataHeader();
  auto* config       = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto&out_branch_config = config->GetBranchConfig(out_branch_);

//  const int n_fhcal_modules = data_header->GetModulePositions(0).GetNumberOfChannels();
//  for (int i = 0; i < n_fhcal_modules; ++i) {
//    auto& module = out_fhcal_branch_->AddChannel(out_branch_config);
//    module.SetSignal(0.0f);
//  }
  auto n_digits = in_fhcal_digits_->GetEntriesFast();

  for (int i = 0; i < n_digits; ++i) {
    auto* in_digit = dynamic_cast<BmnFHCalDigit*>(in_fhcal_digits_->At(i));
    if( in_digit->GetModuleID() == 0 )
      continue;
    if( in_digit->GetSectionID() != 0 )
      continue;
    auto module_id = in_digit->GetModuleID();
    auto module = out_fhcal_branch_->AddChannel();
    float energy_scaled = in_digit->GetELossDigi();
    module->SetSignal(energy_scaled);
    module->SetNumber(module_id);
  }
}

void BmnFHCalModulesConverter::Finish() {}

BmnFHCalModulesConverter::~BmnFHCalModulesConverter() { delete out_fhcal_branch_; };
