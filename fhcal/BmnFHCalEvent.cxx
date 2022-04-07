/** @file BmnFHCalEvent.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 27.03.2022
 **
 ** Code of Class for Bmn FHCal data container in event 
 **/

#include "BmnFHCalEvent.h"

void BmnFHCalEvent::SummarizeEvent()
{
  float TotalEnergy = 0.;
  for(auto &it:fModulesArr){
    it.SummarizeModule();
    TotalEnergy += it.GetEnergy();
  }
  SetTotalEnergy(TotalEnergy);
}


ClassImp(BmnFHCalEvent)

