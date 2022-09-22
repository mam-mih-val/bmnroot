/**
 * @file BmnCaloTowerDraw.cxx
 * @author 
 * @brief BmnCaloTowerDraw source file 
 * @date 2021-09-09
 * 
 */

#include "BmnCaloTowerDraw.h"
#include "BmnCaloDigit.h"
#include "BmnDetectorList.h"

#include "MpdEventManagerEditor.h"
#include "FairLogger.h"

#include "TEveManager.h"
#include "TGeoManager.h"
#include "TEveViewer.h"
#include "TGeoBBox.h"

#include "json.hpp"

#include <iostream>

using json = nlohmann::json;

// file containing the characteristics of calorimeters
const char* caloDataFile = gSystem->ExpandPathName("$VMCWORKDIR/config/eventdisplay.json");

BmnCaloTowerDraw::BmnCaloTowerDraw()
  : FairTask("BmnCaloTowerDraw", 0),
    fCaloMinEnergyThreshold(0),
    fCaloName("unknown"),
    fEventManager(NULL),
    fDigitList(NULL),
    fEneArr(NULL),
    fResetRequiredFlag(kFALSE),
    fInitDrawFlag(kTRUE),
    fq(NULL)
{}

BmnCaloTowerDraw::BmnCaloTowerDraw(const char* name, Int_t caloType, Float_t caloMinEnergyThreshold, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fCaloMinEnergyThreshold(caloMinEnergyThreshold),
    fEventManager(NULL),
    fDigitList(NULL),
    fEneArr(NULL),
    fResetRequiredFlag(kFALSE),
    fInitDrawFlag(kTRUE),
    fq(NULL)
{ BmnDetectorList::GetSystemNameCaps(caloType, fCaloName); }

InitStatus BmnCaloTowerDraw::Init()
{
    if (fVerbose > 0) cout << "BmnCaloTowerDraw::Init() | Type: " << fCaloName << " | " << endl;
    //----------------------------------------
    if (fCaloName == "unknown")
    {
        LOG(error) << "BmnCaloTowerDraw::Init() calorimeter type not defined! Task will be deactivated!";
        SetActive(kFALSE);
        return kERROR;
    }
    //----------------------------------------
    ifstream file(caloDataFile);
    if (!file.is_open())
    {
        LOG(error) << "BmnCaloTowerDraw::Init() | Type: " << fCaloName << " | file " << caloDataFile << " not found! Task will be deactivated!";
        SetActive(kFALSE);
        file.close();
        return kERROR;
    }
    
    json caloData;
    file >> caloData;
    file.close();
    try
    {
        fNumModules = caloData["calorimeters"][fCaloName.Data()]["numberOfModules"];
        fGeoPath = caloData["calorimeters"][fCaloName.Data()]["pathToGeometry"];
    }
    catch(const json::type_error& e)
    {
        LOG(error) << "BmnCaloTowerDraw::Init() | Type: " << fCaloName << " | file " << caloDataFile << " doesn't contain the required data or the calorimeter type is incorrectly set! Task will be deactivated!";
        SetActive(kFALSE);
        return kERROR;
    }
    //----------------------------------------
    fEventManager = MpdEventManager::Instance();
    if (fVerbose > 1) cout << "BmnCaloTowerDraw::Init() | Type: " << fCaloName << " | get instance of EventManager: " << fEventManager << endl;

    fEventManager->fgRedrawRecoPointsReqired = kTRUE;

    FairRootManager *fManager = FairRootManager::Instance();
    if (fVerbose > 1) cout << "BmnCaloTowerDraw::Init() | Type: " << fCaloName << " | get instance of FairRootManager: " << fManager << endl;

    fDigitList = (TClonesArray *) fManager->GetObject(GetName());
    if (fDigitList == 0)
    {
        LOG(error) << "BmnCaloTowerDraw::Init() | Type: " << fCaloName << " | branch " << GetName() << " not found! Task will be deactivated!";
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose > 1) cout << "BmnCaloTowerDraw::Init() | Type: " << fCaloName << " | get digit list " << fDigitList << endl;

    if (gGeoManager->cd(fGeoPath.c_str()) == false)
    {
        LOG(error) << "BmnCaloTowerDraw::Init() | Type: " << fCaloName << " | Path to geometry '" << fGeoPath << "' not found" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    //----------------------------------------
    fModuleZLen = new Double_t[fNumModules+1];
    fMaxModuleZLen = 0;

    fEneArr = new Float_t[fNumModules+1];
    fMaxE = 1;
    for (Int_t i = 0; i < fNumModules+1; i++)
        fEneArr[i] = 1;
    DrawTowers();
    fInitDrawFlag = kFALSE;
    fResetRequiredFlag = kFALSE;

    fq = 0;

    return kSUCCESS;
}

void BmnCaloTowerDraw::Exec(Option_t* option)
{
    if (!IsActive()) return;
    Reset();

    if (fVerbose > 1) cout << "-----[ BmnCaloTowerDraw::Exec() | Type: " << fCaloName << " | ]-----------------------------------" << endl;

    fMaxE = 0;
    for (Int_t i = 0; i < fNumModules+1; i++)
        fEneArr[i] = 0;

    if (fEventManager->fgShowShowMCPoints || fEventManager->fgShowShowRecoPoints || fEventManager->fgShowShowRecoTracks)
    {
        Int_t nDigits = fDigitList->GetEntriesFast();
        if (fVerbose > 2) cout << "BmnCaloTowerDraw::Exec() | Type: " << fCaloName << " | Number of Calo digits = " << nDigits << endl;

        for (int i = 0; i < nDigits; i++)
        {
            BmnCaloDigit *dgt = (BmnCaloDigit *) fDigitList->At(i);

            Int_t channel = dgt->GetChannel();
            if (channel < 1 || channel > fNumModules) continue;
            Float_t energy = dgt->GetAmp() * 1e-3; // MeV to GeV

            if (fVerbose > 3) cout << "BmnCaloTowerDraw::Exec() | Type: " << fCaloName << " | Channel = " << channel << ", Energy = " << energy << " GeV" << endl;

            fEneArr[channel] = energy;
        }
        
        for (Int_t i = 1; i < fNumModules+1; i++)
            if (fEneArr[i] > fMaxE) fMaxE = fEneArr[i];

        if (fVerbose > 2) cout << "BmnCaloTowerDraw::Exec() | Type: " << fCaloName << " | Max energy = " << fMaxE << " GeV" << endl;

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

    TEvePointSet *q = new TEvePointSet(GetName(), fDigitList->GetEntriesFast(), TEvePointSelectorConsumer::kTVT_XYZ);
    q->SetOwnIds(kTRUE);
    
    fEventManager->AddEventElement(q, RecoPointList);

    fq = q;

    gEve->FullRedraw3D(kFALSE);
}

void BmnCaloTowerDraw::DrawTowers()
{
    gGeoManager->cd(fGeoPath.c_str());

    auto rootNode = gGeoManager->GetCurrentNode();
    auto rootArr = rootNode->GetVolume()->GetNodes();

    for (Int_t iModule = 0; iModule < rootArr->GetEntriesFast(); iModule++)
    {
        auto caloNode = (TGeoNode *) rootArr->UncheckedAt(iModule);
        auto caloArr = caloNode->GetVolume()->GetNodes();

        for (Int_t iTower = 0; iTower < caloArr->GetEntriesFast(); iTower++)
        {
            auto moduleNode = (TGeoNode *) caloArr->UncheckedAt(iTower);
            TGeoVolume *moduleVolumeCopy;

            /**
             * the internal structure of modules does not allow changing their shape 
             * (all modules have the same TGeoVolume), so during initialization we 
             * delete and create a new TGeoVolume for each module.
             */
            
            if (fInitDrawFlag)
            {
                moduleVolumeCopy = (TGeoVolume *) moduleNode->GetVolume()->Clone();
                fModuleZLen[iTower] = ((TGeoBBox *) moduleVolumeCopy->GetShape())->GetDZ();
                if (fMaxModuleZLen < fModuleZLen[iTower])
                    fMaxModuleZLen = fModuleZLen[iTower];
            }
            else
            {
                moduleVolumeCopy = (TGeoVolume *) moduleNode->GetVolume();
            
                auto box = (TGeoBBox *) moduleVolumeCopy->GetShape();
                auto mat = moduleNode->GetMatrix();

                if (fEneArr[iTower+1] != 0)
                {
                    box->SetBoxDimensions(box->GetDX(), box->GetDY(), fModuleZLen[iTower] * fEneArr[iTower+1] / fMaxE);
                    ((TGeoTranslation *) mat)->SetDz(fModuleZLen[iTower] * fEneArr[iTower+1] / fMaxE - fMaxModuleZLen);

                    moduleNode->SetVisibility(kTRUE);
                }
                else
                    moduleNode->SetVisibility(kFALSE);
            }

            moduleNode->SetVolume(moduleVolumeCopy);
        }
    }
    
    fResetRequiredFlag = kTRUE;
}

void BmnCaloTowerDraw::Reset()
{
    if (fq != 0)
    {
        fq->Reset();
        gEve->RemoveElement(fq, fEventManager->EveRecoPoints);
    }
}

void BmnCaloTowerDraw::Finish() {}

BmnCaloTowerDraw::~BmnCaloTowerDraw()
{
    fDigitList->Delete();
    delete[] fModuleZLen;
    delete[] fEneArr;
}

ClassImp(BmnCaloTowerDraw);
