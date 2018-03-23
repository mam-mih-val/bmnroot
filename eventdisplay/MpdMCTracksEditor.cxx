// MpdMCTracksEditor
//
// Specialization of TGedEditor for proper update propagation to TEveManager.

#include "MpdMCTracksEditor.h"

#include "FairRootManager.h"
#include "FairRunAna.h"

#include "TFile.h"
#include "TGLabel.h"
#include "TGLayout.h"
#include "TString.h"


MpdMCTracksEditor::MpdMCTracksEditor(const TGWindow * p, Int_t width, Int_t height,
                                     UInt_t options, Pixel_t back)
  : TGedFrame(p, width, height, options | kVerticalFrame, back),
    fObject(NULL),
    fManager(MpdEventManager::Instance())
{
    //Resize(width, height);
    MakeTitle("MpdEventManager  Editor");

    TGVerticalFrame*  fInfoFrame = CreateEditorTabSubFrame("Info");
    TGCompositeFrame* title1 = new TGCompositeFrame(fInfoFrame, 180, 10,
                                                    kVerticalFrame |
                                                    kLHintsExpandX |
                                                    kFixedWidth    |
                                                    kOwnBackground);
    TString Infile= "Input File : ";
    TFile* file = FairRootManager::Instance()->GetInChain()->GetFile();
    Infile += file->GetName();
    TGLabel* TFName = new TGLabel(title1, Infile.Data());
    title1->AddFrame(TFName);

    UInt_t RunId = FairRunAna::Instance()->getRunId();
    TString run = "Run Id : ";
    run += RunId;
    TGLabel* TRunId = new TGLabel(title1, run.Data());
    title1->AddFrame(TRunId);

    fInfoFrame->AddFrame(title1, new TGLayoutHints(kLHintsTop, 0, 0, 2, 0));
}

ClassImp(MpdMCTracksEditor)
