// -------------------------------------------------------------------------
// -----                     CbmMvdGeoPar source file                  -----
// -----                  Created 07/11/06  by V. Friese               -----
// -----             Copied from CbmGeostsPar.cxx (D. Bertini)         -----
// -------------------------------------------------------------------------

#include "CbmMvdGeoPar.h"

#include "FairParamList.h"
#include "FairGeoNode.h"

#include "TObjArray.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;

// -----   Constructor   ---------------------------------------------------
CbmMvdGeoPar::CbmMvdGeoPar(const char* name, const char* title,
			   const char* context)
  : FairParGenericSet(name, title, context),
    fGeoSensNodes(new TObjArray()),
    fGeoPassNodes(new TObjArray())
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdGeoPar::~CbmMvdGeoPar(void) 
{
  /* commented due to problems during destruction
  // somehow it is not possible to delete the last volume
  // even if one get a correct pointer 
  if ( fGeoSensNodes ) {
    fGeoSensNodes->Delete();
    delete fGeoSensNodes;
  }

  if ( fGeoPassNodes ) {
    fGeoPassNodes->Delete();
    delete fGeoPassNodes;
  }
  
  */
}
// -------------------------------------------------------------------------



// -----   Virtual public method clear   -----------------------------------
void CbmMvdGeoPar::clear() {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}
// -------------------------------------------------------------------------



// -----   Virtual public method putParams   -------------------------------
void CbmMvdGeoPar::putParams(FairParamList* parList) {
  if ( ! parList ) return;
  parList->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  parList->addObject("FairGeoNodes Passive List",   fGeoPassNodes);
}
// -------------------------------------------------------------------------



// -----   Virtual public method getParams   -------------------------------
Bool_t CbmMvdGeoPar::getParams(FairParamList* parList) {
  if ( ! parList ) return kFALSE;
  if ( ! parList->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) 
    return kFALSE;
  if ( ! parList->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) 
    return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------




ClassImp(CbmMvdGeoPar)
