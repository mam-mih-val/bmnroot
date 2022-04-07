/** @file BmnFHCalReconstructor.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 03.04.2021
 **
 ** Code for BmnFHCalEvent reconstruction (creation) from BmnFHCalDigi {Data} or BmnFHCalDigit {Sim} 
 **/

#include "BmnFHCalReconstructor.h"


BmnFHCalReconstructor::BmnFHCalReconstructor() {
  printf("BmnFHCalReconstructor: constructor\n");
}

BmnFHCalReconstructor::~BmnFHCalReconstructor() {
}

InitStatus BmnFHCalReconstructor::Init() {
  fworkTime = 0.;
  fpFairRootMgr = FairRootManager::Instance();
  fArrayOfFHCalDigits = (TClonesArray*) fpFairRootMgr->GetObject("FHCalDigit");

  fpFairRootMgr->RegisterAny("BmnFHCalEvent", fBmnFHCalEvent, kTRUE); //GOOD !! for tobject
  //ioman->Register("BmnFHCalEvent", "FHCal", fBmnFHCalEvent, kTRUE);

  Info(__func__,"FHCal Reconstructor ready");
  return kSUCCESS;
}

void BmnFHCalReconstructor::Exec(Option_t* opt) {
  TStopwatch sw;
  sw.Start();
  
  //fBmnFHCalEvent->reset();
  /*if (!IsActive())
      return;*/

  BmnFHCalEvent event;
  for (int i = 0; i < fArrayOfFHCalDigits->GetEntriesFast(); i++) {
    BmnFHCalDigit *ThisDigi = (BmnFHCalDigit*) fArrayOfFHCalDigits->At(i);
    if(ThisDigi->GetELoss() < 0.) continue;

    auto mod_id = ThisDigi->GetModuleID();
    auto sec_id = ThisDigi->GetSectionID();
    if (mod_id <= 0 || mod_id >= BmnFHCalEvent::fgkMaxModules || sec_id <= 0 || sec_id >= BmnFHCalModule::fgkMaxSections) {
      Error(__func__,"FHCal digi ignored. Mod %d Sec %d", mod_id, sec_id);
      continue;
    }

    event.GetModule(mod_id)->SetNsections(5);
    event.GetModule(mod_id)->SetModuleId(mod_id);
    event.GetModule(mod_id)->SetSectionEnergy(sec_id, ThisDigi->GetELoss());
  }

  event.SetTotalEnergy(58.59);
  fBmnFHCalEvent->emplace_back(event);
  fpFairRootMgr->Fill();

  fBmnFHCalEvent->clear();
  sw.Stop();
  fworkTime += sw.RealTime();


}

void BmnFHCalReconstructor::Finish() {
  printf("Work time of BmnFHCalReconstructor: %4.2f sec.\n", fworkTime);
}

ClassImp(BmnFHCalReconstructor)

