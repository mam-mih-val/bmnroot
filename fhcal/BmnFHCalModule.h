/** \file BmnFHCalModule.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 27.03.2022
 **/

/** \class BmnFHCalModule
 ** \brief Class for Bmn FHCal module data container in event
 ** \version 1.0
 **/

#ifndef BMNFHCALMODULE_H
#define BMNFHCALMODULE_H

#include <array>
#include <numeric>
#include <TClonesArray.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

class BmnFHCalModule
{
public:
  /** @brief Default constructor.
   **/
  BmnFHCalModule();


  /**  Copy constructor **/
  BmnFHCalModule(const BmnFHCalModule&);


  /** Move constructor  **/
  BmnFHCalModule(BmnFHCalModule&&);


  /** Assignment operator  **/
  BmnFHCalModule& operator=(const BmnFHCalModule&);


  /** Move Assignment operator  **/
  BmnFHCalModule& operator=(BmnFHCalModule&&);


  /** Destructor **/
  virtual ~BmnFHCalModule(){};


  /** @brief Class name
   ** @return BmnFHCalModule
   **/
  virtual const char *GetClassName() { return "BmnFHCalModule"; }


  /** @brief Module id
   ** @return BmnFHCalModule index
   **/
  int GetModuleId() const { return fModId; };


  /** @brief Deposited Energy
   ** @return Deposited Energy in module in event [mip]
   **/
  float GetEnergy() const { return fModuleEnergy; };
  

  /** @brief Sections number
   ** @return number of sections in module
   **/
  int GetNsections() const { return fNsections; };


  /** @brief Section energy
   ** @return Get Energy in section with index sec_id
   **/
  float GetSectionEnergy(int sec_id) const { return fSectArr.at(sec_id-1); }

  /** @brief X
   ** @return module X position [mm]
   **/
  float GetX() const { return fX; };


  /** @brief Y
   ** @return module Y position [mm]
   **/
  float GetY() const { return fY; };


  void SetModuleId(int ModId) { fModId = ModId; }
  void SetEnergy(float Energy) { fModuleEnergy = Energy; }
  void SetX(float X) { fX = X; }
  void SetY(float Y) { fY = Y; }
  void SetNsections(int Nsections) { fNsections = Nsections; }
  void SetSectionEnergy(int sec_id, float Energy) { fSectArr.at(sec_id-1) = Energy; }
  void ResetSections() { fSectArr.fill({}); }

  void SummarizeModule(){ SetEnergy(std::accumulate(std::begin(fSectArr), std::end(fSectArr), 0.0)); }
  void reset();

  virtual void Print(Option_t *option = "") const;


  static const int fgkMaxSections = 10; // 10 sections numbered from 1 to 10 inclusively
private:
  int fModId;
  float fModuleEnergy;
  float fX;
  float fY;
  int fNsections;
  std::array<float, fgkMaxSections> fSectArr;

  ClassDef(BmnFHCalModule, 1);
};

#endif /* BMNFHCALMODULE_H */
