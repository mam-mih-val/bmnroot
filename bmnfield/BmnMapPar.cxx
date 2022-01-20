#include <iostream>
#include "BmnFieldMap.h"
#include "BmnMapPar.h"
#include "FairParamList.h"

using namespace std;

// ------   Constructor   --------------------------------------------------
BmnMapPar::BmnMapPar(const char* name, const char* title,
		     const char* context) 
  : FairParGenericSet(name, title, context) {
  fType = -1;
  fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;
  fMapName = "";
  fPosX = fPosY = fPosZ = 0.;
  fScale = 0.;
}

// -------------------------------------------------------------------------
BmnMapPar::BmnMapPar() 
{
  fType = -1;
  fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;
  fMapName = "";
  fPosX = fPosY = fPosZ = 0.;
  fScale = 0.;
}

// ------   Destructor   ---------------------------------------------------
BmnMapPar::~BmnMapPar() { }

// ------   Put parameters   -----------------------------------------------
void BmnMapPar::putParams(FairParamList* list) {
  
  if ( ! list ) return;
  
  list->add("Field Type", fType);
  list->add("Field map name", fMapName);
  list->add("Field x position", fPosX);
  list->add("Field y position", fPosY);
  list->add("Field z position", fPosZ);
  list->add("Field scaling factor", fScale);
}

// --------   Get parameters   ---------------------------------------------
Bool_t BmnMapPar::getParams(FairParamList* list) {
  
  if ( ! list ) return kFALSE;
  
  if ( ! list->fill("Field Type", &fType) ) return kFALSE;
  
  Text_t mapName[80];
  if ( ! list->fill("Field map name", mapName, 80) ) return kFALSE;
  fMapName = mapName;
  if ( ! list->fill("Field x position", &fPosX) )  return kFALSE;
  if ( ! list->fill("Field y position", &fPosY) )  return kFALSE;
  if ( ! list->fill("Field z position", &fPosZ) )  return kFALSE;
  if ( ! list->fill("Field scaling factor", &fScale) ) return kFALSE;
  return kTRUE;
  
}

// ---------   Set parameters from FairField   ------------------------------
void BmnMapPar::SetParameters(FairField* field) {
  
  if ( ! field ) {
    cerr << "-W- BmnMapPar::SetParameters: Empty field pointer!" << endl;
    return;
  }
  
  BmnFieldMap* fieldMap = (BmnFieldMap*) field;
  fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;
  fMapName = field->GetName();
  fPosX   = fieldMap->GetPositionX();
  fPosY   = fieldMap->GetPositionY();
  fPosZ   = fieldMap->GetPositionZ();
  fScale  = fieldMap->GetScale();
  fType   = fieldMap->GetType();
}

ClassImp(BmnMapPar)

