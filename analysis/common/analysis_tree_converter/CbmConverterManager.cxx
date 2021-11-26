/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "CbmConverterManager.h"

#include "CbmConverterTask.h"
//#include "CbmEvent.h"

#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TClonesArray.h"

#include <iostream>

#include "AnalysisTree/DataHeader.hpp"
#include "AnalysisTree/TaskManager.hpp"

ClassImp(CbmConverterManager);

InitStatus CbmConverterManager::Init()
{
  task_manager_->Init();
  FillDataHeader();
  InitEvent();
  return kSUCCESS;
}

void CbmConverterManager::AddTask(CbmConverterTask* task)
{
  tasks_.emplace_back(task);
  task_manager_->AddTask(reinterpret_cast<AnalysisTree::Task*>(task));
}

void CbmConverterManager::ProcessData(){
  index_map_.clear();

  for (auto* task : tasks_) {
    task->SetIndexesMap(&index_map_);
    task->ProcessData();
    index_map_.insert(std::make_pair(task->GetOutputBranchName(), task->GetOutIndexesMap()));
  }
  task_manager_->FillOutput();
}

void CbmConverterManager::Exec(Option_t* /*opt*/)
{
  LOG(info) << "Event based mode\n";
  ProcessData();
}


void CbmConverterManager::Finish()
{
  TDirectory* curr   = gDirectory;  // TODO check why this is needed
  TFile* currentFile = gFile;

  task_manager_->Finish();

  gFile      = currentFile;
  gDirectory = curr;
}

void CbmConverterManager::FillDataHeader()
{
  // Force user to write data info //TODO is there a way to read it from a file automatically?
  assert(!system_.empty() && beam_mom_);

  auto* data_header = new AnalysisTree::DataHeader();

  std::cout << "ReadDataHeader" << std::endl;
  data_header->SetSystem(system_);
  data_header->SetBeamMomentum(beam_mom_);

  auto& psd_mod_pos              = data_header->AddDetector();
  const int psd_node_id          = 6;
  const char* module_name_prefix = "module";

  auto* geoMan   = gGeoManager;
  auto* caveNode = geoMan->GetTopNode();
  auto* psdNode  = caveNode->GetDaughter(psd_node_id);
  std::cout << "-I- " << psdNode->GetName() << std::endl;

  auto psdGeoMatrix = psdNode->GetMatrix();
  auto psdBox       = (TGeoBBox*) psdNode->GetVolume()->GetShape();
  TVector3 frontFaceLocal(0, 0, -psdBox->GetDZ());

  TVector3 frontFaceGlobal;
  psdGeoMatrix->LocalToMaster(&frontFaceLocal[0], &frontFaceGlobal[0]);

  for (int i_d = 0; i_d < psdNode->GetNdaughters(); ++i_d) {
    auto* daughter = psdNode->GetDaughter(i_d);
    TString daughterName(daughter->GetName());
    if (daughterName.BeginsWith(module_name_prefix)) {

      auto geoMatrix = daughter->GetMatrix();
      TVector3 translation(geoMatrix->GetTranslation());

      int modID = daughter->GetNumber();
      double x  = translation.X();
      double y  = translation.Y();

      std::cout << "mod" << modID << " : " << Form("(%.3f, %3f)", x, y) << std::endl;

      auto* module = psd_mod_pos.AddChannel();
      module->SetPosition(x, y, frontFaceGlobal[2]);
    }
  }

  task_manager_->SetOutputDataHeader(data_header);
}
CbmConverterManager::~CbmConverterManager() = default;
