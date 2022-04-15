/** @file BmnFHCalModule.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 27.03.2022
 **
 ** Code of Class for Bmn FHCal module data container in event 
 **/

#include "BmnFHCalModule.h"

#include <iostream>
using namespace std;

BmnFHCalModule::BmnFHCalModule()
  : fModId()
  , fModuleEnergy()
  , fX()
  , fY()
  , fNsections()
  , fSectArr()
{
}


// --- Copy constructor
BmnFHCalModule::BmnFHCalModule(const BmnFHCalModule& other)
  : fModId(other.fModId)
  , fModuleEnergy(other.fModuleEnergy)
  , fX(other.fX)
  , fY(other.fY)
  , fNsections(other.fNsections)
  , fSectArr(other.fSectArr)
{
}


// --- Move constructor
BmnFHCalModule::BmnFHCalModule(BmnFHCalModule&& other)
  : fModId(other.fModId)
  , fModuleEnergy(other.fModuleEnergy)
  , fX(other.fX)
  , fY(other.fY)
  , fNsections(other.fNsections)
  , fSectArr(other.fSectArr)
{
}


// --- Assignment operator
BmnFHCalModule& BmnFHCalModule::operator=(const BmnFHCalModule& other)
{
  if (this != &other) {
    fModId = other.fModId;
    fModuleEnergy = other.fModuleEnergy;
    fX = other.fX;
    fY = other.fY;
    fNsections = other.fNsections;
    fSectArr = other.fSectArr;
  }
  return *this;
}


// --- Move assignment operator
BmnFHCalModule& BmnFHCalModule::operator=(BmnFHCalModule&& other)
{
  if (this != &other) {
    fModId = other.fModId;
    fModuleEnergy = other.fModuleEnergy;
    fX = other.fX;
    fY = other.fY;
    fNsections = other.fNsections;
    fSectArr = other.fSectArr;
  }
  return *this;
}


void BmnFHCalModule::Print(Option_t *option) const {

}

void BmnFHCalModule::reset() {
  fModuleEnergy = 0.;
  fModId = 0;
  fX = 0.;
  fY = 0.;
  fNsections = 0;
  fSectArr.fill({});
}


ClassImp(BmnFHCalModule)

