#include "ScalarD.h"

#include "Riostream.h"
#include <iomanip>
//#include "FairTask.h"

ClassImp(ScalarD)

//_________________________________________________________________
ScalarD::ScalarD() : TObject() {


}

//_________________________________________________________________
/*ScalarD::ScalarD(const char *name, const char *title):FairTask(name) {


}*/

//_________________________________________________________________
ScalarD::~ScalarD() {
  
  //FairRootManager *fManager = FairRootManager::Instance();
  //fManager->Write();

}

//_________________________________________________________________
void ScalarD::SetSV(Double_t x) {

  scalval=x;

}
//________________________________________________________________
Double_t ScalarD::GetSV() {

  return scalval;

}
//________________________________________________________________
