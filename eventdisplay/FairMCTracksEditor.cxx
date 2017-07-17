//______________________________________________________________________________
// FairMCTracksEditor
//
// Specialization of TGedEditor for proper update propagation to
// TEveManager.

#include "FairMCTracksEditor.h"

#include "FairRootManager.h"
#include "FairRunAna.h"

#include "TFile.h"
#include "TGLabel.h"
#include "TString.h"

#include <stddef.h>

//______________________________________________________________________________
FairMCTracksEditor::FairMCTracksEditor(const TGWindow* p, Int_t width, Int_t height,
                                       UInt_t options, Pixel_t back)
  : TGedFrame(p, width, height, options | kVerticalFrame, back),
    fObject(NULL),
    fManager(FairEventManager::Instance())
{
  MakeTitle("FairEventManager Editor");

  TGVerticalFrame* fInfoFrame = CreateEditorTabSubFrame("Info");
  TGCompositeFrame* title1 = new TGCompositeFrame(fInfoFrame, 180, 10,
      kVerticalFrame | kLHintsExpandX | kFixedWidth | kOwnBackground);

  TFile* file = FairRootManager::Instance()->GetInChain()->GetFile();
  TString Infile = TString::Format("Input File: %s", file->GetName());
  TGLabel* TFName = new TGLabel(title1, Infile.Data());
  title1->AddFrame(TFName);

  TString run = "Run Id: ";
  run += FairRunAna::Instance()->getRunId();
  TGLabel* TRunId = new TGLabel(title1, run.Data());
  title1->AddFrame(TRunId);

  fInfoFrame->AddFrame(title1, new TGLayoutHints(kLHintsTop, 0, 0, 2, 0));
}

ClassImp(FairMCTracksEditor)
