/* Copyright (C) 2020-2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer] */

#ifndef ANALYSIS_TREE_PSDMODULESCONVERTER_H_
#define ANALYSIS_TREE_PSDMODULESCONVERTER_H_

#include "CbmConverterTask.h"

#include "AnalysisTree/Detector.hpp"

class TClonesArray;

class CbmPsdModulesConverter final : public CbmConverterTask {
public:
  explicit CbmPsdModulesConverter(std::string out_branch_name, std::string match_to = "")
    : CbmConverterTask(std::move(out_branch_name), std::move(match_to)) {};

  ~CbmPsdModulesConverter() final;

  void Init() final;
  void ProcessData(CbmEvent* event) final;
  void Finish() final;

private:
  AnalysisTree::ModuleDetector* psd_modules_ {nullptr};
  TClonesArray* cbm_psd_hits_ {nullptr};

  ClassDef(CbmPsdModulesConverter, 1)
};

#endif  // ANALYSIS_TREE_PSDMODULESCONVERTER_H_
