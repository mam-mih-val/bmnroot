#include "ScalarI.h"

#include "Riostream.h"
#include <iomanip>
//#include "FairTask.h"

ClassImp(ScalarI)

//_________________________________________________________________
ScalarI::ScalarI() : TObject() {


}

//_________________________________________________________________
/*ScalarI::ScalarI(const char *name, const char *title):FairTask(name) {


}*/

//_________________________________________________________________
ScalarI::~ScalarI() {
  
  //FairRootManager *fManager = FairRootManager::Instance();
  //fManager->Write();

}

//_________________________________________________________________
void ScalarI::SetSV(Int_t x) {

  scalval=x;

}
//________________________________________________________________
Int_t ScalarI::GetSV() {

  return scalval;

}
//________________________________________________________________
