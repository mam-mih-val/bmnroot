/** @file BmnFHCalReconstructor.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 03.04.2021
 **
 ** Code for BmnFHCalEvent reconstruction (creation) from BmnFHCalDigi {Data} or BmnFHCalDigit {Sim} 
 **/

#include "BmnFHCalReconstructor.h"


BmnFHCalReconstructor::BmnFHCalReconstructor(TString config_file, bool isExp) {
  fConfigFile = config_file;
  fIsExp = isExp;
  printf("BmnFHCalReconstructor: Config %s; isExp %i\n", fConfigFile.Data(), fIsExp);
}

BmnFHCalReconstructor::~BmnFHCalReconstructor() {
  if(fBmnFHCalEvent) delete fBmnFHCalEvent;
}

InitStatus BmnFHCalReconstructor::Init() {
  fworkTime = 0.;
  fpFairRootMgr = FairRootManager::Instance();
  (fIsExp)? fArrayOfFHCalDigits = (TClonesArray*) fpFairRootMgr->GetObject("FHCalDigi") :
    fArrayOfFHCalDigits = (TClonesArray*)fpFairRootMgr->GetObject("FHCalDigit");

  if (!fArrayOfFHCalDigits) {
        cout << "BmnFHCalReconstructor::Init(): branch with Digits not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
  }

  fBmnFHCalEvent = new BmnFHCalEvent();
  fBmnFHCalEvent->reset();
  ParseConfig();
  fpFairRootMgr->RegisterAny("FHCalEvent", fBmnFHCalEvent, kTRUE);

  Info(__func__,"FHCal Reconstructor ready");
  return kSUCCESS;
}

InitStatus BmnFHCalReconstructor::OnlineInit() {
  fworkTime = 0.;

  fArrayOfFHCalDigits = new TClonesArray("BmnFHCalDigi");

  fBmnFHCalEvent = new BmnFHCalEvent();
  fBmnFHCalEvent->reset();
  ParseConfig();

  return kSUCCESS;
}

InitStatus BmnFHCalReconstructor::OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree) {
  if (!IsActive()) return kERROR;

  SetOnlineActive();

  fArrayOfFHCalDigits->Delete();
  if (dataTree->SetBranchAddress("FHCalDigi", &fArrayOfFHCalDigits)) {
    LOG(error) << "BmnFHCalReconstructor::OnlineReadData(): branch FHCalDigi not found! Task will be deactivated";
    SetOnlineActive(kFALSE);
    return kERROR;
  }

  fBmnFHCalEvent->ResetEnergies();

  return kSUCCESS;
}

void BmnFHCalReconstructor::ParseConfig() {
  BmnFHCalRaw2Digit *Mapper = new BmnFHCalRaw2Digit();
  Mapper->ParseConfig(fConfigFile);
  auto uniAddrVect = Mapper->GetChannelVect();
  auto ThatVectorX = Mapper->GetUniqueXpositions();
  auto ThatVectorY = Mapper->GetUniqueYpositions();
  for(auto it : uniAddrVect){
    if(it == 0) continue;
    auto mod_id = BmnFHCalAddress::GetModuleId(it);
    auto this_mod = fBmnFHCalEvent->GetModule(mod_id);
    this_mod->SetModuleId(mod_id);

    if(BmnFHCalAddress::GetModuleType(it) == 1) this_mod->SetNsections(7);
    if(BmnFHCalAddress::GetModuleType(it) == 2) this_mod->SetNsections(10);
    this_mod->SetX(ThatVectorX.at(BmnFHCalAddress::GetXIdx(it)));
    this_mod->SetY(ThatVectorY.at(BmnFHCalAddress::GetYIdx(it)));
  }
  delete Mapper;
}

void BmnFHCalReconstructor::Exec(Option_t* opt) {
  
  if (!IsActive())
    return;
    
  TStopwatch sw;
  sw.Start();
  fBmnFHCalEvent->ResetEnergies();

  if(fIsExp) {
    for (int i = 0; i < fArrayOfFHCalDigits->GetEntriesFast(); i++) {
      BmnFHCalDigi *ThisDigi = (BmnFHCalDigi*) fArrayOfFHCalDigits->At(i);
      if(ThisDigi->GetSignal() < 0.) continue;

      auto mod_id = ThisDigi->GetModuleId(); // 1 to 54
      auto sec_id = ThisDigi->GetSectionId();// 1 to 10
      if (mod_id <= 0 || mod_id > BmnFHCalEvent::fgkMaxModules || sec_id <= 0 || sec_id > BmnFHCalModule::fgkMaxSections) {
        //Error(__func__,"FHCal digi ignored. Mod %d Sec %d", mod_id, sec_id);
        continue;
      }
      fBmnFHCalEvent->GetModule(mod_id)->SetSectionEnergy(sec_id, ThisDigi->GetSignal());
    }
    fBmnFHCalEvent->SummarizeEvent();

  }
  else {
    for (int i = 0; i < fArrayOfFHCalDigits->GetEntriesFast(); i++) {
      BmnFHCalDigit *ThisDigi = (BmnFHCalDigit*) fArrayOfFHCalDigits->At(i);
      if(ThisDigi->GetELoss() < 0.) continue;

      auto mod_id = ThisDigi->GetModuleID(); // 1 to 54
      auto sec_id = ThisDigi->GetSectionID();// 1 to 10
      if (mod_id <= 0 || mod_id > BmnFHCalEvent::fgkMaxModules || sec_id <= 0 || sec_id > BmnFHCalModule::fgkMaxSections) {
        //Error(__func__,"FHCal digi ignored. Mod %d Sec %d", mod_id, sec_id);
        continue;
      }
      fBmnFHCalEvent->GetModule(mod_id)->SetSectionEnergy(sec_id, ThisDigi->GetELoss());
    }
    fBmnFHCalEvent->SummarizeEvent();
  }

  sw.Stop();
  fworkTime += sw.RealTime();
}

void BmnFHCalReconstructor::OnlineWrite(const std::unique_ptr<TTree>& dataTree) {
  if (!IsActive() || !IsOnlineActive()) return;

  dataTree->Branch("FHCalEvent", &fBmnFHCalEvent);
  dataTree->Fill();
}

void BmnFHCalReconstructor::Finish() {
  printf("Work time of BmnFHCalReconstructor: %4.2f sec.\n", fworkTime);
}

ClassImp(BmnFHCalReconstructor)
