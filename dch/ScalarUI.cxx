#include "ScalarUI.h"

#include "Riostream.h"
#include <iomanip>
//#include "FairTask.h"

ClassImp(ScalarUI)

//_________________________________________________________________
ScalarUI::ScalarUI() : TObject() {


}

//_________________________________________________________________
/*ScalarUI::ScalarUI(const char *name, const char *title):FairTask(name) {


}*/

//_________________________________________________________________
ScalarUI::~ScalarUI() {
  
  //FairRootManager *fManager = FairRootManager::Instance();
  //fManager->Write();

}

//_________________________________________________________________
void ScalarUI::SetSV(UInt_t x) {

  scalval=x;

}
//________________________________________________________________
UInt_t ScalarUI::GetSV() {

  return scalval;

}
//________________________________________________________________
