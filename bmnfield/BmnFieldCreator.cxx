#include "BmnFieldCreator.h"
#include "BmnFieldConst.h"
#include "BmnFieldMap.h"
#include "BmnFieldMapSym3.h"
#include "BmnNewFieldMap.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

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

    if ( !fFieldPar )
        cerr << "-E-  No field parameters available!" << endl;
    else
    {
        // Instantiate correct field type
        Int_t fType = fFieldPar->GetType();
        if      ( fType == 0 ) fMagneticField = new BmnFieldConst(fFieldPar);
        else if ( fType == 1 ) fMagneticField = new BmnNewFieldMap(fFieldPar);
        else if ( fType == 3 ) fMagneticField = new BmnFieldMapSym3(fFieldPar);
        else cerr << "-W- FairRunAna::GetField: Unknown field type " << fType << endl;

        //cout << "New field at " << fMagneticField << ", type " << fType << endl;
        // Initialise field
        if ( fMagneticField )
        {
            fMagneticField->Init();
            //fMagneticField->Print();
        }
    }
    return fMagneticField;
}

ClassImp(BmnFieldCreator)
