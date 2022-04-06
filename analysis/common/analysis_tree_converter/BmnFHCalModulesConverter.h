/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_PSDMODULESCONVERTER_H_
#define ANALYSIS_TREE_PSDMODULESCONVERTER_H_

#include "BmnConverterTask.h"

#include "AnalysisTree/Detector.hpp"

class TClonesArray;

class BmnFHCalModulesConverter final : public BmnConverterTask {
public:
  explicit BmnFHCalModulesConverter(std::string out_branch_name)
      : BmnConverterTask(std::move(out_branch_name)) {}

  ~BmnFHCalModulesConverter() final;

  void Init() final;
  void ProcessData() final;
  void Finish() final;

private:
  AnalysisTree::ModuleDetector*out_fhcal_branch_{nullptr};
  TClonesArray*in_fhcal_digits_{nullptr};

  ClassDef(BmnFHCalModulesConverter, 1)
};

#endif  // ANALYSIS_TREE_PSDMODULESCONVERTER_H_
