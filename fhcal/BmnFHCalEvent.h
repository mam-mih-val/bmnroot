/** \file BmnFHCalEvent.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 27.03.2022
 **/

/** \class BmnFHCalEvent
 ** \brief Class for Bmn FHCal data container in event
 ** \version 1.0
 **/

#ifndef BMNFHCALEVENT_H
#define BMNFHCALEVENT_H

#include <vector>

#include <TObject.h>
#include <TClonesArray.h>
#include "BmnFHCalModule.h"


class BmnFHCalEvent : public TObject
{
public:
  /** @brief Default constructor.
   **/
  BmnFHCalEvent()
    : TObject()
    , fTotalEnergy()
    , fModulesArr()
  {
  }

  /** @brief Constructor with detailed assignment.
   **//*
  BmnFHCalEvent(float Energy, std::vector<BmnFHCalModule> ModulesArr)
    : TObject()
    , fTotalEnergy(Energy)
    , fModulesArr(ModulesArr)
  {
  }
*/
  /** Destructor **/
  ~BmnFHCalEvent(){ reset(); }

  /** @brief Class name
   ** @return BmnFHCalEvent
   **/
  virtual const char *GetClassName() { return "BmnFHCalEvent"; }

  /** @brief Deposited Energy
   ** @return Deposited Energy in FHCal in event [mip]
   **/
  float GetTotalEnergy() const { return fTotalEnergy; };

  /** @brief Module info
   ** @return BmnFHCalModule object with index mod_id
   **/
  BmnFHCalModule* GetModule(uint8_t mod_id) { return &fModulesArr.at(mod_id); };


  /** @brief Set Deposited Energy
   ** @return Set Deposited Energy in FHCal in event [mip]
   **/
  void SetTotalEnergy(float Energy) { fTotalEnergy = Energy; };

  /** @brief Set ModulesArrtor
   ** @return Set Vector of BmnFHCalModule objects
   **/
  void SetModule(uint8_t mod_id, BmnFHCalModule Module) { fModulesArr.at(mod_id) = Module; };

  /** @brief Zero all fiels
   ** @return Set All fields to zero
   **/
  void reset() { fTotalEnergy = 0.; fModulesArr.fill({}); }

  float GetAsymmetry();
  float GetMoment();

  virtual void Print(Option_t *option = "") const {};


  static const int fgkMaxModules = 55; // 54 modules numbered from 1 to 54 inclusively
private:
  /// BOOST serialization interface
  friend class boost::serialization::access;


  float fTotalEnergy;
  std::array<BmnFHCalModule, fgkMaxModules> fModulesArr;

  ClassDef(BmnFHCalEvent, 1);
};

#endif /* BMNFHCALEVENT_H */
