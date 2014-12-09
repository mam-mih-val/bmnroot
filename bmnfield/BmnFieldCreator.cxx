#include "BmnFieldCreator.h"

#include "BmnFieldPar.h"
#include "BmnFieldConst.h"
#include "BmnFieldMap.h"
#include "BmnFieldMapSym2.h"
#include "BmnFieldMapSym3.h"
#include "BmnBsField.h"
#include "BmnFieldMapDistorted.h"
#include "BmnFieldMapSym1.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

static BmnFieldCreator gBmnFieldCreator;

BmnFieldCreator::BmnFieldCreator()
  :FairFieldFactory(),
   fFieldPar(NULL)
{
	fCreator=this;
}

BmnFieldCreator::~BmnFieldCreator()
{
}

void BmnFieldCreator::SetParm()
{
  FairRunAna *Run = FairRunAna::Instance();
  FairRuntimeDb *RunDB = Run->GetRuntimeDb();
  fFieldPar = (BmnFieldPar*) RunDB->getContainer("BmnFieldPar");

}

FairField* BmnFieldCreator::createFairField()
{ 
  FairField *fMagneticField=0;

  if ( ! fFieldPar ) {
    cerr << "-E-  No field parameters available!"
	 << endl;
  }else{
	// Instantiate correct field type
	Int_t fType = fFieldPar->GetType();
	if      ( fType == 0 ) fMagneticField = new BmnFieldConst(fFieldPar);
	else if ( fType == 1 ) fMagneticField = new BmnFieldMap(fFieldPar);
	else if ( fType == 2 ) fMagneticField = new BmnFieldMapSym2(fFieldPar);
	else if ( fType == 3 ) fMagneticField = new BmnFieldMapSym3(fFieldPar);
	else if ( fType == kTypeDistorted ) fMagneticField = new BmnFieldMapDistorted(fFieldPar); 
	else if ( fType == 5 ) fMagneticField = new BmnFieldMapSym1(fFieldPar);
	else if ( fType == 6 ) fMagneticField = new BmnBsField(fFieldPar);
	else cerr << "-W- FairRunAna::GetField: Unknown field type " << fType
		<< endl;
	cout << "New field at " << fMagneticField << ", type " << fType << endl;
	// Initialise field
	if ( fMagneticField ) {
		fMagneticField->Init();
//		fMagneticField->Print();
	}
  }
  return fMagneticField;
}


ClassImp(BmnFieldCreator)
