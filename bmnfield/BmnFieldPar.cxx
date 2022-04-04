// -------------------------------------------------------------------------
// -----                      BmnFieldPar source file                  -----
// -----                   Created 20/02/06  by V. Friese              -----
// -------------------------------------------------------------------------
#include "BmnFieldPar.h"

#include "BmnFieldConst.h"
#include "BmnFieldMap.h"

#include "FairParamList.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

const int kMaxLen = 2048;

// ------   Constructor   --------------------------------------------------

BmnFieldPar::BmnFieldPar(const char* name, const char* title,
        const char* context)
: FairParGenericSet(name, title, context),
fType(-1),
fXmin(0.),
fXmax(0.),
fYmin(0.),
fYmax(0.),
fZmin(0.),
fZmax(0.),
fBx(0.),
fBy(0.),
fBz(0.),
fMapName(""),
fPosX(0.),
fPosY(0.),
fPosZ(0.),
fScale(0.),
fIsOff(kFALSE),
fDistortionFilename(""),
fParentName(""),
fTypeOfParent(0) {
}
// -------------------------------------------------------------------------

BmnFieldPar::BmnFieldPar()
: FairParGenericSet(),
fType(-1),
fXmin(0.),
fXmax(0.),
fYmin(0.),
fYmax(0.),
fZmin(0.),
fZmax(0.),
fBx(0.),
fBy(0.),
fBz(0.),
fMapName(""),
fPosX(0.),
fPosY(0.),
fPosZ(0.),
fScale(0.),
fIsOff(kFALSE),
fDistortionFilename(""),
fParentName(""),
fTypeOfParent(0) {
}
// -------------------------------------------------------------------------

// ------   Destructor   ---------------------------------------------------

BmnFieldPar::~BmnFieldPar() {
}
// -------------------------------------------------------------------------



// ------   Put parameters   -----------------------------------------------

void BmnFieldPar::putParams(FairParamList* list) {

    if (!list) return;

    list->add("Field Type", fType);

    if (fType == 0) { // constant field
        list->add("Field min x", fXmin);
        list->add("Field max x", fXmax);
        list->add("Field min y", fYmin);
        list->add("Field max y", fYmax);
        list->add("Field min z", fZmin);
        list->add("Field max z", fZmax);
        list->add("Field Bx", fBx);
        list->add("Field By", fBy);
        list->add("Field Bz", fBz);
        list->add("Field is off", fIsOff);
    }
    else if (fType >= 1 && fType <= kMaxFieldMapType) { // field map
        list->add("Field map name", fMapName);
        list->add("Field x position", fPosX);
        list->add("Field y position", fPosY);
        list->add("Field z position", fPosZ);
        list->add("Field scaling factor", fScale);
        list->add("Field is off", fIsOff);

        if (fType == kTypeDistorted) { // BmnFieldMapDistorted case   
            list->add("Field map distortion filename", fDistortionFilename.Data());
            list->add("Field name of parent field", fParentName.Data());
            list->add("Field type of parent field", fTypeOfParent);
        }
    }
}
// -------------------------------------------------------------------------



// --------   Get parameters   ---------------------------------------------

Bool_t BmnFieldPar::getParams(FairParamList* list) {

    if (!list) return kFALSE;

    if (!list->fill("Field Type", &fType)) return kFALSE;

    if (fType == 0) { // constant field
        if (!list->fill("Field min x", &fXmin)) return kFALSE;
        if (!list->fill("Field max x", &fXmax)) return kFALSE;
        if (!list->fill("Field min y", &fYmin)) return kFALSE;
        if (!list->fill("Field max y", &fYmax)) return kFALSE;
        if (!list->fill("Field min z", &fZmin)) return kFALSE;
        if (!list->fill("Field max z", &fZmax)) return kFALSE;
        if (!list->fill("Field Bx", &fBx)) return kFALSE;
        if (!list->fill("Field By", &fBy)) return kFALSE;
        if (!list->fill("Field Bz", &fBz)) return kFALSE;
        if (!list->fill("Field is off", &fIsOff)) return kFALSE;
    }
    else if (fType >= 1 && fType <= kMaxFieldMapType) { // field map
        Text_t mapName[80];
        if (!list->fill("Field map name", mapName, 80)) return kFALSE;
        fMapName = mapName;
        if (!list->fill("Field x position", &fPosX)) return kFALSE;
        if (!list->fill("Field y position", &fPosY)) return kFALSE;
        if (!list->fill("Field z position", &fPosZ)) return kFALSE;
        if (!list->fill("Field scaling factor", &fScale)) return kFALSE;
        if (!list->fill("Field is off", &fIsOff)) return kFALSE;

        if (fType == kTypeDistorted) { // BmnFieldMapDistorted case 
            Text_t tmp[kMaxLen];
            fDistortionFilename = "";
            if (!list->fill("Field map distortion filename", tmp, kMaxLen)) return kFALSE;
            fDistortionFilename = tmp;
            fParentName = "";
            if (!list->fill("Field name of parent field", tmp, kMaxLen)) return kFALSE;
            fParentName = tmp;
            if (!list->fill("Field type of parent field", &fTypeOfParent)) return kFALSE;
        }

    }

    return kTRUE;

}
// -------------------------------------------------------------------------



// ---------   Set parameters from BmnField   ------------------------------

void BmnFieldPar::SetParameters(FairField* field) {

    if (!field) {
        cerr << "-W- BmnFieldPar::SetParameters: Empty field pointer!" << endl;
        return;
    }

    fType = field->GetType();

    if (fType == 0) { // constant field
        BmnFieldConst* fieldConst = (BmnFieldConst*) field;
        fBx = fieldConst->GetBx();
        fBy = fieldConst->GetBy();
        fBz = fieldConst->GetBz();
        fXmin = fieldConst->GetXmin();
        fXmax = fieldConst->GetXmax();
        fYmin = fieldConst->GetYmin();
        fYmax = fieldConst->GetYmax();
        fZmin = fieldConst->GetZmin();
        fZmax = fieldConst->GetZmax();
        fMapName = "";
        fPosX = fPosY = fPosZ = fScale = 0.;
        fIsOff = fieldConst->IsFieldOff();
    }
    else if (fType >= 1 && fType <= kMaxFieldMapType) { // field map
        BmnFieldMap* fieldMap = (BmnFieldMap*) field;
        fBx = fBy = fBz = 0.;
        fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;

        fMapName = field->GetName();
        fPosX = fieldMap->GetPositionX();
        fPosY = fieldMap->GetPositionY();
        fPosZ = fieldMap->GetPositionZ();
        fScale = fieldMap->GetScale();
        fIsOff = fieldMap->IsFieldOff();
    }
    else {
        cerr << "-W- BmnFieldPar::SetParameters: Unknown field type "
                << fType << "!" << endl;
        fBx = fBy = fBz = 0.;
        fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;
        fMapName = "";
        fPosX = fPosY = fPosZ = fScale = 0.;
        fIsOff = kFALSE;
    }

    return;

}

ClassImp(BmnFieldPar)

