#include <iomanip>
#include <iostream>
#include <fstream>

#include "BmnMultiField.h"
#include "BmnRegion.h"
#include "BmnFieldConst.h"
#include "BmnFieldMap.h"
#include "TObjArray.h"
#include "BmnMapPar.h"
#include "BmnMultiFieldPar.h"


using namespace std;

// -------------   Default constructor  ----------------------------------
BmnMultiField::BmnMultiField() {

    fMaps= new TObjArray(10);
    fNoOfMaps=0;
    fType = 5;
}

// ------------   Constructor from MpdFieldPar   --------------------------
BmnMultiField::BmnMultiField(BmnMultiFieldPar* fieldPar) {
   fType = 5;
   fMaps= new TObjArray(10);
   fNoOfMaps=0;
   TObjArray *fArray= fieldPar->GetParArray();
   if(fArray->IsEmpty()) fType=-1;
}

// ------------   Destructor   --------------------------------------------
BmnMultiField::~BmnMultiField() { }

// -----------   Adding fields   ------------------------------------------
void BmnMultiField::AddField(FairField *field) {
	
   if(field){
      fMaps->AddLast(field);	
      fNoOfMaps++;
   }
}

// -----------   Intialisation   ------------------------------------------
void BmnMultiField::Init() {
  BmnFieldConst *field=0;
  BmnFieldMap *fieldMap=0;
  for (Int_t n=0; n<=fNoOfMaps; n++){
    fieldMap = dynamic_cast<BmnFieldMap *>(fMaps->At(n));
    field = dynamic_cast<BmnFieldConst *>(fMaps->At(n));
    if(fieldMap){
      fieldMap->Init();
      fFieldMaps.insert( pair<BmnRegion*, FairField*>(new BmnRegion(fieldMap->GetZmin(),fieldMap->GetZmax()) , fieldMap ));
    }else if(field){
      field->Init();
      fFieldMaps.insert( pair<BmnRegion*, FairField*>(new BmnRegion(field->GetZmin(),field->GetZmax() ), field ));
    }
  }
}

// -----------   Get x component of the field   ---------------------------
Double_t BmnMultiField::GetBx(Double_t x, Double_t y, Double_t z) {
  BmnRegion *fReg=0;
  FairField *fField=0;
  std::map <BmnRegion*, FairField* >::iterator fMapIter;
  for (fMapIter=fFieldMaps.begin(); fMapIter!= fFieldMaps.end();fMapIter++ ){
    fReg=fMapIter->first;
    if(fReg->IsInside(z)){
      fField=fMapIter->second;
      break;
    }
  }
  if(fField)  return  fField->GetBx( x, y,  z);	
  else return 0;
}

// -----------   Get y component of the field   ---------------------------
Double_t BmnMultiField::GetBy(Double_t x, Double_t y, Double_t z) {
  
  BmnRegion *fReg=0;
  FairField *fField=0;
  std::map <BmnRegion*, FairField* >::iterator fMapIter;
  for (fMapIter=fFieldMaps.begin(); fMapIter!= fFieldMaps.end();fMapIter++ ){
    fReg=fMapIter->first;
    if(fReg->IsInside(z)){
      fField=fMapIter->second;
      break;
    }
  }
  if(fField)  return fField->GetBy( x, y,  z);
  else return 0;
}

// -----------   Get z component of the field   ---------------------------
Double_t BmnMultiField::GetBz(Double_t x, Double_t y, Double_t z) {
  
  BmnRegion *fReg=0;
  FairField *fField=0;
  std::map <BmnRegion*, FairField* >::iterator fMapIter;
  for (fMapIter=fFieldMaps.begin(); fMapIter!= fFieldMaps.end();fMapIter++ ){
    fReg=fMapIter->first;
    if(fReg->IsInside(z)){
      fField=fMapIter->second;
      break;
    }
  }
  if(fField) return fField->GetBz( x, y,  z);
  else return 0;
}

//---------------------------------------------------------------------------
void BmnMultiField::GetFieldValue(const Double_t point[3], Double_t* bField)
{
  
  BmnRegion *fReg=0;
  FairField *fField=0;
  std::map <BmnRegion*, FairField* >::iterator fMapIter;
  for (fMapIter=fFieldMaps.begin(); fMapIter!= fFieldMaps.end();fMapIter++ ){
    fReg=fMapIter->first;
    if(fReg->IsInside(point[2])){
      fField=fMapIter->second;
      break;
    }
  }
  if(fField){
    /* bField[0] = fField->GetBx(point[0], point[1], point[2]);
       bField[1] = fField->GetBy(point[0], point[1], point[2]);
       bField[2] = fField->GetBz(point[0], point[1], point[2]);
    */
    fField->GetBxyz(point, bField);
  }
  else{
    bField[0] = 0;
    bField[1] = 0;
    bField[2] = 0;
  }
}

// ---------   Screen output   --------------------------------------------
void BmnMultiField::Print() {  
  for (Int_t n=0; n<=fNoOfMaps; n++){
    FairField *fieldMap = dynamic_cast<FairField *>(fMaps->At(n));
    if(fieldMap) fieldMap->Print("");
  }
}

ClassImp(BmnMultiField)
  
