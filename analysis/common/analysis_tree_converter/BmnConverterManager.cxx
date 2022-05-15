/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#include "BmnConverterManager.h"

#include "BmnConverterTask.h"
//#include "CbmEvent.h"

#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "BmnFHCalGeoPar.h"

#include <iostream>
#include <FairGeoParSet.h>

#include "AnalysisTree/DataHeader.hpp"
#include "AnalysisTree/TaskManager.hpp"

ClassImp(BmnConverterManager);

InitStatus BmnConverterManager::Init()
{
//   task_manager_->SetOutputTreeConfig(AnalysisTree::eBranchWriteMode::kCreateNewTree);
  task_manager_->Init();
   FillDataHeader();
  InitEvent();
  return kSUCCESS;
}

void BmnConverterManager::AddTask(BmnConverterTask * task)
{
  tasks_.emplace_back(task);
  task_manager_->AddTask(reinterpret_cast<AnalysisTree::Task*>(task));
}

void BmnConverterManager::ProcessData(){
  index_map_.clear();

  for (auto* task : tasks_) {
    task->SetIndexesMap(&index_map_);
    task->ProcessData();
    index_map_.insert(std::make_pair(task->GetOutputBranchName(), task->GetOutIndexesMap()));
  }
  task_manager_->FillOutput();
}

void BmnConverterManager::Exec(Option_t* /*opt*/)
{
  LOG(info) << "Event based mode\n";
  ProcessData();
}


void BmnConverterManager::Finish()
{
  LOG(info) << "BmnConverterManager::Finish()";
  TDirectory* curr   = gDirectory;  // TODO check why this is needed
  TFile* currentFile = gFile;

  task_manager_->Finish();

  gFile      = currentFile;
  gDirectory = curr;
}

void BmnConverterManager::FillDataHeader()
{
  // Force user to write data info //TODO is there a way to read it from a file automatically?
  assert(!system_.empty() && beam_mom_);

  auto* data_header = new AnalysisTree::DataHeader();

  std::cout << "ReadDataHeader" << std::endl;
  data_header->SetSystem(system_);
  data_header->SetBeamMomentum(beam_mom_);

  auto& psd_mod_pos              = data_header->AddDetector();
  const int psd_node_id          = 15;
  const char* module_name_prefix = "module";

  const char* fairGeom         = "FAIRGeom";

  std::cout << "Reading geometry from geomtry file" << std::endl;
  if( geometry_file_.empty() )
    throw std::runtime_error("BmnConverterManager::FillDataHeader(): Geometry file is not set");
  TGeoManager* geoMan = TGeoManager::Import(geometry_file_.c_str(), fairGeom);
  if( !geoMan )
    throw std::runtime_error("BmnConverterManager::FillDataHeader(): There is no TGeoManager in file "+geometry_file_);
  TGeoNode* caveNode  = geoMan->GetTopNode();
  if( !caveNode )
    throw std::runtime_error("BmnConverterManager::FillDataHeader(): There is no cave node in TGeoManager");
  TGeoNode* fhCalNode   = nullptr;
  TString nodeName;

  for (int i = 0; i < caveNode->GetNdaughters(); i++) {
    fhCalNode  = caveNode->GetDaughter(i);
    nodeName = fhCalNode->GetName();
    nodeName.ToLower();
    if (nodeName.Contains("zdc")) break;
  }
  fhCalNode = fhCalNode->GetDaughter(0);
  std::cout << "FHCal node name: " << fhCalNode->GetName() << std::endl;

  auto fhCalGeoMatrix = fhCalNode->GetMatrix();
  auto fhCalBox       = (TGeoBBox*) fhCalNode->GetVolume()->GetShape();
  TVector3 frontFaceLocal(0, 0, -fhCalBox->GetDZ());

  TVector3 frontFaceGlobal;
  fhCalGeoMatrix->LocalToMaster(&frontFaceLocal[0], &frontFaceGlobal[0]);

  std::cout << "FHCal modules:\n";
  auto n_fhcal_modules = fhCalNode->GetNdaughters();
  for (int i_d = 0; i_d < fhCalNode->GetNdaughters(); ++i_d){ psd_mod_pos.AddChannel(); }

  for (int i_d = 0; i_d < fhCalNode->GetNdaughters(); ++i_d) {
    auto* daughter = fhCalNode->GetDaughter(i_d);
    auto geoMatrix = daughter->GetMatrix();
    TVector3 translation(geoMatrix->GetTranslation());

    int modID = daughter->GetNumber() - 1;
    double x  = translation.X();
    double y  = translation.Y();
    translation.SetZ(frontFaceGlobal.Z());
    double z  = translation.Z();

    auto& module = psd_mod_pos.Channel( modID );
    module.SetPosition(x, y, frontFaceGlobal[2]);

    std::cout << modID << ", ";
  }
  std::cout << std::endl;

  for (int i = 0; i < caveNode->GetNdaughters(); i++) {
    fhCalNode  = caveNode->GetDaughter(i);
    nodeName = fhCalNode->GetName();
    nodeName.ToLower();
    if (nodeName.Contains("scwall")) break;
  }
  fhCalNode = fhCalNode->GetDaughter(0);
  std::cout << "ScWall node name: " << fhCalNode->GetName() << std::endl;

  auto ScWallGeoMatrix = fhCalNode->GetMatrix();
  auto ScWallBox       = (TGeoBBox*) fhCalNode->GetVolume()->GetShape();
  frontFaceLocal = TVector3(0, 0, -fhCalBox->GetDZ());

  fhCalGeoMatrix->LocalToMaster(&frontFaceLocal[0], &frontFaceGlobal[0]);


  std::cout << "ScWall modules:\n";
  for (int i_d = 0; i_d < fhCalNode->GetNdaughters(); ++i_d) {
    auto* sub_node = fhCalNode->GetDaughter(i_d);
    for (int j_d = 0; j_d < sub_node->GetNdaughters(); ++j_d){ psd_mod_pos.AddChannel(); }
  }

  for (int i_d = 0; i_d < fhCalNode->GetNdaughters(); ++i_d) {
    auto* sub_node = fhCalNode->GetDaughter(i_d);
    for( int j_d = 0; j_d < sub_node->GetNdaughters(); ++j_d ) {
      auto *daughter = sub_node->GetDaughter(j_d);
      auto geoMatrix = daughter->GetMatrix();
      TVector3 translation(geoMatrix->GetTranslation());

      int modID = n_fhcal_modules + daughter->GetNumber() - 1;
      double x = translation.X();
      double y = translation.Y();
      translation.SetZ(frontFaceGlobal.Z());
      double z = translation.Z();

      try {
        auto &module = psd_mod_pos.Channel(modID);
        module.SetPosition(x, y, frontFaceGlobal[2]);
        std::cout << daughter->GetNumber() - 1 << ", ";
      }catch( std::exception &e ){
        std::cout << "Exception catched. Debug information: i_d=" << i_d << " j_d=" << j_d << " modID=" << modID << std::endl;
        throw e;
      }
    }
  }
  std::cout << std::endl;

  std::cout << "FHCal and ScWall geometry have been successfully written" << std::endl;

  geoMan->GetListOfVolumes()->Delete();
  geoMan->GetListOfShapes()->Delete();
  delete geoMan;

  task_manager_->SetOutputDataHeader(data_header);
}
BmnConverterManager::~BmnConverterManager() = default;
