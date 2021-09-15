/**
 * @file BmnCaloTowerDraw.cxx
 * @author 
 * @brief BmnCaloTowerDraw source file 
 * @date 2021-09-09
 * 
 */

#include "BmnCaloTowerDraw.h"
#include "BmnZDCDigit.h"
#include "BmnECALDigit.h"

#include "MpdEventManagerEditor.h"
#include "FairLogger.h"

#include "TEveManager.h"
#include "TGeoManager.h"
#include "TEveViewer.h"
#include "TGeoBBox.h"

#include "json.hpp"

#include <iostream>

using namespace std;
using json = nlohmann::json;

// file containing the characteristics of calorimeters
const char* caloDataFile = gSystem->ExpandPathName("$VMCWORKDIR/eventdisplay/bmn/BmnCaloTowerDraw.json");

BmnCaloTowerDraw::BmnCaloTowerDraw()
  : FairTask("BmnCaloTowerDraw", 0),
    fCaloMinEnergyThreshold(0),
    fCaloType(NULL),
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
    fCaloType(caloType == 0 ? "ZDC" : (caloType == 1 ? "ECAL" : NULL)),
    fEventManager(NULL),
    fDigitList(NULL),
    fEneArr(NULL),
    fResetRequiredFlag(kFALSE),
    fInitDrawFlag(kTRUE),
    fq(NULL)
{}

InitStatus BmnCaloTowerDraw::Init()
{
    if (fCaloType == NULL)
    {
        LOG(ERROR) << "BmnCaloTowerDraw::Init() calorimeter type not defined! Task will be deactivated!";
        SetActive(kFALSE);
        return kERROR;
    }

    if (fVerbose > 0) cout << "BmnCaloTowerDraw::Init() | Type: " << fCaloType << " | " << endl;

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > 1) cout << "BmnCaloTowerDraw::Init() | Type: " << fCaloType << " | get instance of EventManager: " << fEventManager << endl;

    fEventManager->fgRedrawRecoPointsReqired = kTRUE;

    FairRootManager* fManager = FairRootManager::Instance();
    if (fVerbose > 1) cout << "BmnCaloTowerDraw::Init() | Type: " << fCaloType << " | get instance of FairRootManager: " << fManager << endl;

    fDigitList = (TClonesArray*) fManager->GetObject(GetName());
    if (fDigitList == 0)
    {
        LOG(ERROR) << "BmnCaloTowerDraw::Init() | Type: " << fCaloType << " | branch " << GetName() << " not found! Task will be deactivated!";
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose > 1) cout << "BmnCaloTowerDraw::Init() | Type: " << fCaloType << " | get digit list " << fDigitList << endl;

    ifstream file(caloDataFile);
    if (!file)
    {
        LOG(ERROR) << "BmnCaloTowerDraw::Init() | Type: " << fCaloType << " | file " << caloDataFile << " not found! Task will be deactivated!";
        SetActive(kFALSE);
        file.close();
        return kERROR;
    }
    
    json caloData;
    file >> caloData;
    file.close();
    try
    {
        fNumModules = caloData[fCaloType]["numberOfModules"];
        fGeoPath = caloData[fCaloType]["pathToGeometry"];
    }
    catch(const json::type_error& e)
    {
        LOG(ERROR) << "BmnCaloTowerDraw::Init() | Type: " << fCaloType << " | file " << caloDataFile << " doesn't contain the required data! Task will be deactivated!";
        SetActive(kFALSE);
        return kERROR;
    }

    if (gGeoManager->cd(fGeoPath.c_str()) == false)
    {
        LOG(ERROR) << "BmnCaloTowerDraw::Init() | Type: " << fCaloType << " | Path to geometry '" << fGeoPath << "' not found" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fModuleZLen = ((TGeoBBox*) gGeoManager->GetCurrentNode()->GetVolume()->GetNode(0)->GetVolume()->GetShape())->GetDZ();

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

    if (fVerbose > 1) cout << "-----[ BmnCaloTowerDraw::Exec() | Type: " << fCaloType << " | ]-----------------------------------" << endl;

    fMaxE = 0;
    for (Int_t i = 0; i < fNumModules+1; i++)
        fEneArr[i] = 0;

    if (fEventManager->fgShowRecoPointsIsShow)
    {
        Int_t nDigits = fDigitList->GetEntriesFast();
        if (fVerbose > 2) cout << "BmnCaloTowerDraw::Exec() | Type: " << fCaloType << " | Number of Calo digits = " << nDigits << endl;

        for (int i = 0; i < nDigits; i++)
        {
            Int_t channel;
            Float_t energy;

            if (fCaloType[0] == 'Z')
            {
                BmnZDCDigit* dgt = (BmnZDCDigit*) fDigitList->At(i);
                channel = dgt->GetChannel();
                if (channel < 1 || channel > fNumModules) continue;
                energy = dgt->GetAmp() * 1e-3; // MeV to GeV
            }
            else
            {
                BmnECALDigit* dgt = (BmnECALDigit*) fDigitList->At(i);
                channel = dgt->GetChannel();
                if (channel < 1 || channel > fNumModules) continue;
                energy = dgt->GetAmp() * 1e-3; // MeV to GeV
            }

            if (fVerbose > 3) cout << "BmnCaloTowerDraw::Exec() | Type: " << fCaloType << " | Channel = " << channel << ", Energy = " << energy << " GeV" << endl;

            fEneArr[channel] = energy;
        }
        
        for (Int_t i = 1; i < fNumModules+1; i++)
            if (fEneArr[i] > fMaxE) fMaxE = fEneArr[i];

        if (fVerbose > 2) cout << "BmnCaloTowerDraw::Exec() | Type: " << fCaloType << " | Max energy = " << fMaxE << " GeV" << endl;

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

void BmnCaloTowerDraw::DrawTowers()
{
    gGeoManager->cd("/cave_1");
    TGeoNode* caveNode = gGeoManager->GetCurrentNode();

    gGeoManager->cd(fGeoPath.c_str());

    TGeoNode* caloNode = gGeoManager->GetCurrentNode();
    TObjArray* caloArr = caloNode->GetVolume()->GetNodes();

    for (Int_t iModule = 0; iModule < caloArr->GetEntriesFast(); iModule++)
    {
        TGeoNode* moduleNode = (TGeoNode*) caloArr->UncheckedAt(iModule);
        TGeoVolume* moduleVolumeCopy;

        /**
         * the internal structure of modules does not allow changing their shape 
         * (all modules have the same TGeoVolume), so during initialization we 
         * delete and create a new TGeoVolume for each module.
         */
        if (fInitDrawFlag)
            moduleVolumeCopy = (TGeoVolume*) moduleNode->GetVolume()->Clone();
        else
            moduleVolumeCopy = (TGeoVolume*) moduleNode->GetVolume();
        
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
}

ClassImp(BmnCaloTowerDraw);
