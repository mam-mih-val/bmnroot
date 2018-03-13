// MpdBoxSetEditor
//
// Specialization of TGedEditor for proper update propagation to TEveManager

#include "MpdBoxSetEditor.h"

#include "MpdEventManager.h"

#include "TGLabel.h"
#include "TGLayout.h"       // for TGLayoutHints, etc

#include <iostream>
using namespace std;


MpdBoxSetEditor::MpdBoxSetEditor(const TGWindow* p, Int_t width, Int_t height, UInt_t options, Pixel_t back)
  : TGedFrame(p, width, height, options | kVerticalFrame, back),
    fInfoFrame(NULL), fTimeWindowPlus(NULL), fTimeWindowMinus(NULL), fObject(NULL), fM(NULL)
{
    cout<<"MpdBoxSetEditor called!"<<endl;
    Init();
}

void MpdBoxSetEditor::Init()
{
    MakeTitle("MpdBoxSet  Editor");
    fInfoFrame = CreateEditorTabSubFrame("Time");

    TGCompositeFrame* title1 = new TGCompositeFrame(fInfoFrame, 250, 10,
                                    kVerticalFrame | kLHintsExpandX |
                                    kFixedWidth    | kOwnBackground);

    TGLabel* label1 = new TGLabel(title1,"Time window after event time [ns]: ");
    title1->AddFrame(label1, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 2, 1, 1));

    fTimeWindowPlus = new TGNumberEntry(title1);//, 0, 5, -1, TGNumberFormat::kNESRealTwo); //, TGNumberFormat::kNEANonNegative);
    //fTimeWindow->SetLimits(0, MAXE, 2501, TGNumberFormat::kNESRealOne);
    fTimeWindowPlus->GetNumberEntry()->SetToolTipText("Time window in ns for which points are shown");
    fTimeWindowPlus->Connect("ValueSet(Long_t)", "MpdBoxSetEditor",this, "TimeWindow()");
    title1->AddFrame(fTimeWindowPlus, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));

    fInfoFrame->AddFrame(title1);

    TGCompositeFrame* title2 = new TGCompositeFrame(fInfoFrame, 250, 10,
                                    kVerticalFrame | kLHintsExpandX |
                                    kFixedWidth    | kOwnBackground);

    TGLabel* label2 = new TGLabel(title2, "Time window before event time [ns]: ");
    title2->AddFrame(label2, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 2, 1, 1));
    fTimeWindowMinus = new TGNumberEntry(title2, 0., 6, -1, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);//, 1, 5, -1, TGNumberFormat::kNESRealTwo, TGNumberFormat::kNEANonNegative);
    //fTimeWindow->SetLimits(0, MAXE, 2501, TGNumberFormat::kNESRealOne);
    fTimeWindowMinus->GetNumberEntry()->SetToolTipText("Time window in ns for which points are shown");
    fTimeWindowMinus->Connect("ValueSet(Long_t)", "MpdBoxSetEditor",this, "TimeWindow()");
    title2->AddFrame(fTimeWindowMinus, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));

    fInfoFrame->AddFrame(title2);
}

void MpdBoxSetEditor::TimeWindow()
{
    cout<<"MpdBoxSetEditor::TimeWindowPlus "<<fTimeWindowPlus->GetNumber()<<endl;
    cout<<"MpdBoxSetEditor::TimeWindowMinus "<<fTimeWindowMinus->GetNumber()<<endl;
    fM->SetTimeWindowPlus(fTimeWindowPlus->GetNumber());
    fM->SetTimeWindowMinus(fTimeWindowMinus->GetNumber());

    MpdEventManager* man = MpdEventManager::Instance();
    man->GotoEvent(man->GetCurrentEvent());
    Update();
}

ClassImp(MpdBoxSetEditor)
