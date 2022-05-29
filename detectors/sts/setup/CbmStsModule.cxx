/** @file CbmStsModule.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 14.05.2013
 **/


#include "FairLogger.h"
#include "setup/CbmStsAddress.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorType.h"


// -----   Default constructor   -------------------------------------------
CbmStsModule::CbmStsModule() : CbmStsElement(), fSensors(), fBuffer()
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsModule::CbmStsModule(const char* name, const char* title,
                           TGeoPhysicalNode* node) :
                           CbmStsElement(name, title, kStsModule, node),
                           fSensors(), fBuffer()
{
}
// -------------------------------------------------------------------------



// --- Destructor   --------------------------------------------------------
CbmStsModule::~CbmStsModule() {
}
// -------------------------------------------------------------------------



// ----- Add a sensor  -----------------------------------------------------
void CbmStsModule::AddSensor(CbmStsSenzor* sensor) {

  // TODO: Unify adding elements from base class and adding sensors
  // If AddDaughter is used, compatibility of sensor types is not assured.
  // Still unclear how sensor types are assigned when setup is read from
  // geometry.

  // Check compatibility with already present sensors
  if ( ! fSensors.empty() ) {
    if ( sensor->GetType() != fSensors[0]->GetType() ) {
      LOG(error) << "Sensor type " << sensor->GetType()->GetTypeId()
                 << " incompatible with sensor type "
                 << fSensors[0]->GetType()->GetTypeId();
      return;
    }
  }

  // Add sensor to the array
  fSensors.push_back(sensor);

  // Set sensor address and pointer to module
  Int_t sensorId = fDaughters.size() - 1;
  UInt_t address = CbmStsAddress::SetElementId(fAddress, kStsSensor,
                                               sensorId);
  sensor->SetAddress(address);
  sensor->SetModule(this);

  return;
}
// -------------------------------------------------------------------------


// -----   Add a signal to the buffer   ------------------------------------
void CbmStsModule::AddSignal(Int_t channel, Double_t time,
                             Double_t charge) {

// TODO: implementation

}
// -------------------------------------------------------------------------

ClassImp(CbmStsModule)
