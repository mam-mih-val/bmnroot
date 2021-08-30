/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#include "FairCave.h"

#include "FairGeoCave.h"         // for FairGeoCave
#include "FairGeoInterface.h"    // for FairGeoInterface
#include "FairGeoLoader.h"       // for FairGeoLoader
#include "FairGeoNode.h"         // for FairGeoNode
#include "FairGeoPassivePar.h"   // for FairGeoPassivePar
#include "FairGeoVolume.h"       // for FairGeoVolume
#include "FairRun.h"             // for FairRun
#include "FairRuntimeDb.h"       // for FairRuntimeDb

#include <TList.h>       // for TListIter, TList (ptr only)
#include <TObjArray.h>   // for TObjArray
#include <TString.h>     // for TString

void FairCave::ConstructGeometry()
{
    FairGeoLoader* loader = FairGeoLoader::Instance();
    FairGeoInterface* GeoInterface = loader->getGeoInterface();
    FairGeoCave* MGeo = new FairGeoCave();
    MGeo->setGeomFile(GetGeometryFileName());
    GeoInterface->addGeoModule(MGeo);
    Bool_t rc = GeoInterface->readSet(MGeo);
    if (rc) {
        MGeo->create(loader->getGeoBuilder());
    }

    TList* volList = MGeo->getListOfVolumes();
    // store geo parameter
    FairRun* fRun = FairRun::Instance();
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    FairGeoPassivePar* par = static_cast<FairGeoPassivePar*>(rtdb->getContainer("FairGeoPassivePar"));
    TObjArray* fSensNodes = par->GetGeoSensitiveNodes();
    TObjArray* fPassNodes = par->GetGeoPassiveNodes();

    TListIter iter(volList);
    FairGeoNode* node = nullptr;
    FairGeoVolume* aVol = nullptr;

    while ((node = static_cast<FairGeoNode*>(iter.Next()))) {
        aVol = dynamic_cast<FairGeoVolume*>(node);
        if (node->isSensitive()) {
            fSensNodes->AddLast(aVol);
        } else {
            fPassNodes->AddLast(aVol);
        }
    }
    par->setChanged();
    par->setInputVersion(fRun->GetRunId(), 1);
}

FairCave::FairCave(const FairCave& rhs)
    : FairModule(rhs)
{
    world[0] = rhs.world[0];
    world[1] = rhs.world[1];
    world[2] = rhs.world[2];
}

FairCave::FairCave() {}

FairCave::~FairCave() {}

FairCave::FairCave(const char* name, const char* Title)
    : FairModule(name, Title)
{
    world[0] = 0;
    world[1] = 0;
    world[2] = 0;
}

FairModule* FairCave::CloneModule() const { return new FairCave(*this); }

ClassImp(FairCave)
