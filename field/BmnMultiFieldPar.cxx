#include "BmnMultiFieldPar.h"
#include "BmnFieldConst.h"
#include "BmnConstPar.h"
#include "FairParamList.h"
#include "BmnFieldPar.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "BmnFieldMap.h"
#include "BmnFieldPar.h"

// ------   Constructor   --------------------------------------------------
BmnMultiFieldPar::BmnMultiFieldPar(const char* name, const char* title, const char* context) 
  : BmnMapPar(name, title, context) 
{
  
  fParArray=new TObjArray();
}

//-----------------------------------------------------------------------
BmnMultiFieldPar::BmnMultiFieldPar() { }

//-----------------------------------------------------------------------
BmnMultiFieldPar::~BmnMultiFieldPar() { }

//-----------------------------------------------------------------------
void BmnMultiFieldPar::putParams(FairParamList* list)
{
  if ( ! list ) return;
  list->addObject("List of Field par", fParArray);
  list->add("Field Type", fType);
}

//-----------------------------------------------------------------------
Bool_t BmnMultiFieldPar::getParams(FairParamList* l)
{
  if (!l->fillObject("list of fields Par",fParArray))return kFALSE;
  if ( ! l->fill("Field Type", &fType) ) return kFALSE;
  
  return kTRUE;
} 

//-----------------------------------------------------------------------
void BmnMultiFieldPar:: SetParameters(FairField* field)
{	 
  fType=5;
  FairRuntimeDb *rtdb=FairRuntimeDb::instance();
  FairRun *fRun= FairRun::Instance();
  BmnMultiField *fmField = (BmnMultiField *)field;
  TObjArray *fArray=fmField->GetFieldList();
  TIterator *Iter=fArray->MakeIterator();
  Iter->Reset();
  FairField* fField = NULL;
  Int_t Type=-1;
  while( (fField = (FairField*)Iter->Next() ) ) {
    Type=fField->GetType();
    if(Type==0) {
      BmnFieldConst *fc= (BmnFieldConst *)fField;
      BmnConstPar *cp = (BmnConstPar*) rtdb->getContainer("BmnConstPar");
      cp->SetParameters(fc);
      cp->setInputVersion(fRun->GetRunId(),1);
      fParArray->AddLast(cp);
    }
    
    else if(Type==1) {
      BmnFieldMap *fm= (BmnFieldMap*)fField;
      BmnFieldPar* fmp = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
      fmp->SetParameters(fm);
      fmp->setInputVersion(fRun->GetRunId(),1);
      fParArray->AddLast(fmp);
    } 
  }
  delete  Iter;
}                      

ClassImp(BmnMultiFieldPar)
  
