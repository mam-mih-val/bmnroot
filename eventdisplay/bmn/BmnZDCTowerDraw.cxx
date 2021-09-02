/**
 * @file BmnZDCTowerDraw.cxx
 * @author 
 * @brief BmnZDCTowerDraw source file 
 * @version 1.0
 * @date 2021-08-13
 * 
 * @copyright 
 * 
 */

#include "BmnZDCTowerDraw.h"
#include "BmnZDCDigit.h"

#include "MpdEventManagerEditor.h"
#include "FairLogger.h"

#include "TEveManager.h"
#include "TGeoManager.h"
#include "TEveViewer.h"
#include "TGeoBBox.h"

#include <iostream>
using namespace std;

BmnZDCTowerDraw::BmnZDCTowerDraw()
  : FairTask("BmnZDCTowerDraw", 0),
    fZDCMinEnergyThreshold(0),
    fEventManager(NULL),
    fDigitList(NULL),
    fEneArr(NULL),
    fResetRequiredFlag(kFALSE),
    fq(NULL) {}

BmnZDCTowerDraw::BmnZDCTowerDraw(const char* name, Float_t zdcMinEnergyThreshold, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fZDCMinEnergyThreshold(zdcMinEnergyThreshold),
    fEventManager(NULL),
    fDigitList(NULL),
    fEneArr(NULL),
    fResetRequiredFlag(kFALSE),
    fq(NULL) {}

InitStatus BmnZDCTowerDraw::Init()
{
    if (fVerbose > 0) cout << "BmnZdcTowerDraw::Init()" << endl;

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > 1) cout << "BmnZdcTowerDraw::Init() get instance of EventManager: " << fEventManager << endl;

    fEventManager->fgRedrawRecoPointsReqired = kTRUE;

    FairRootManager* fManager = FairRootManager::Instance();
    if (fVerbose > 1) cout << "BmnZdcTowerDraw::Init() get instance of FairRootManager: " << fManager << endl;

    fDigitList = (TClonesArray*) fManager->GetObject(GetName());
    if (fDigitList == 0)
    {
        LOG(ERROR) << "BmnZdcTowerDraw::Init() branch " << GetName() << " not found! Task will be deactivated";
        SetActive(kFALSE);
    }
    if (fVerbose > 1) cout << "BmnZdcTowerDraw::Init() get digit list " << fDigitList << endl;

    // -------------------------------------------------------
    fNumModules = 104;
    fModuleZLen = 52.5;
    // -------------------------------------------------------

    fEneArr = new Float_t[fNumModules+1];
    for (Int_t i = 0; i < fNumModules+1; i++)
        fEneArr[i] = 0;
    fMaxE = 0;

    fq = 0;

    return kSUCCESS;
}

void BmnZDCTowerDraw::Exec(Option_t* option)
{
    if (!IsActive()) return;
    Reset();

    if (fVerbose > 1) cout << "-----[ BmnZDCTowerDraw::Exec() ]-----------------------------------" << endl;

    fMaxE = 0;
    for (Int_t i = 0; i < fNumModules+1; i++)
        fEneArr[i] = 0;

    if (fEventManager->fgShowRecoPointsIsShow)
    {
        Int_t nDigits = fDigitList->GetEntriesFast();
        if (fVerbose > 2) cout << "BmnZdcTowerDraw::Exec() Number of ZDC digits = " << nDigits << endl;

        for (int i = 0; i < nDigits; i++)
        {
            BmnZDCDigit* dgt = (BmnZDCDigit*) fDigitList->At(i);

            Int_t channel = dgt->GetChannel();
            if (channel < 1 || channel > fNumModules) continue;

            Float_t energy = dgt->GetAmp() * 1e-3; // MeV to GeV

            if (fVerbose > 3) cout << "BmnZdcTowerDraw::Exec() Channel = " << channel << ", Energy = " << energy << " GeV" << endl;

            fEneArr[channel] = energy;
        }
        
        for (Int_t i = 1; i < fNumModules+1; i++)
            if (fEneArr[i] > fMaxE) fMaxE = fEneArr[i];

        if (fVerbose > 2) cout << "BmnZdcTowerDraw::Exec() Max energy = " << fMaxE << " GeV" << endl;

        DrawTowers();
    }
    else 
    {
        if (fResetRequiredFlag)
        {
            fMaxE = 1;
            for (Int_t i = 0; i < fNumModules+1; i++)
                fEneArr[i] = 1;

            DrawTowers();

            fResetRequiredFlag = kFALSE;
        }
    }

    TEvePointSet* q = new TEvePointSet(GetName(), fDigitList->GetEntriesFast(), TEvePointSelectorConsumer::kTVT_XYZ);
    q->SetOwnIds(kTRUE);
    
    fEventManager->AddEventElement(q, RecoPointList);

    fq = q;

    gEve->FullRedraw3D(kFALSE);
}

void BmnZDCTowerDraw::DrawTowers()
{
    gGeoManager->cd("/cave_1");
    TGeoNode* caveNode = gGeoManager->GetCurrentNode();

    TString zdc_path = "/cave_1/ZDC_common_0/zdc01_1";
    if (gGeoManager->cd(zdc_path) == false)
    {
        LOG(ERROR) << "BmnZdcTowerDraw::DrawTowers(): Path '" << zdc_path << "' not found" << endl;
        return;
    }

    TGeoNode* zdcNode = gGeoManager->GetCurrentNode();
    TGeoVolume* zdcVolumeClone = zdcNode->GetVolume()->CloneVolume();
    TObjArray* zdcArr = zdcNode->GetVolume()->GetNodes();

    for (Int_t iModule = 0; iModule < zdcNode->GetVolume()->GetNdaughters(); iModule++)
    {
        /**
         * * the internal structure of modules does not allow changing
         * * their shape (all modules have the same TGeoVolume), 
         * * so we delete and create a new TGeoVolume with a unique shape for each module
         */
        TGeoNode* moduleNode = (TGeoNode*) zdcArr->UncheckedAt(iModule);
        TGeoVolume* moduleVolumeCopy = (TGeoVolume*) moduleNode->GetVolume()->Clone();
        
        TGeoBBox* box = (TGeoBBox*) moduleVolumeCopy->GetShape();
        TGeoMatrix* mat = moduleNode->GetMatrix();

        if (fEneArr[iModule+1] != 0)
        {
            box->SetBoxDimensions(box->GetDX(), box->GetDY(), fModuleZLen * fEneArr[iModule+1] / fMaxE);
            ((TGeoTranslation*) mat)->SetDz(fModuleZLen * fEneArr[iModule+1] / fMaxE - fModuleZLen);

            moduleNode->SetVisibility(kTRUE);
        }
        else
            moduleNode->SetVisibility(kFALSE);

        moduleNode->SetVolume(moduleVolumeCopy);
    }
    
    fResetRequiredFlag = kTRUE;
}

void BmnZDCTowerDraw::Reset()
{
    if (fq != 0)
    {
        fq->Reset();
        gEve->RemoveElement(fq, fEventManager->EveRecoPoints);
    }
}

void BmnZDCTowerDraw::Finish() {}

BmnZDCTowerDraw::~BmnZDCTowerDraw()
{
    fDigitList->Delete();
}

ClassImp(BmnZDCTowerDraw);