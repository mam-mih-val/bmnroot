/** \file BmnFHCalReconstructor.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 03.04.2021
 **/

/** \class BmnFHCalReconstructor
 ** \brief Class for BmnFHCalEvent reconstruction (creation) from BmnFHCalDigi {Data} or BmnFHCalDigit {Sim}
 ** \version 1.0
 **/

#ifndef BMNFHCALRECONSTRUCTOR_H
#define BMNFHCALRECONSTRUCTOR_H

#include "BmnTask.h"
#include "BmnFHCalDigi.h"
#include "BmnFHCalDigit.h"
#include "BmnFHCalModule.h"
#include "BmnFHCalEvent.h"
#include "BmnFHCalRaw2Digit.h"

#include "FairLogger.h"

#include <TStopwatch.h>
#include <TClonesArray.h>

#include <iostream>
#include <fstream>

class BmnFHCalReconstructor : public BmnTask {
  public:
    BmnFHCalReconstructor(TString config_file, bool isExp);
    ~BmnFHCalReconstructor();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    void ParseConfig();
    virtual void Finish();

    virtual InitStatus OnlineInit();
    virtual InitStatus OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree);
    virtual void OnlineWrite(const std::unique_ptr<TTree> &dataTree);

  private:
    FairRootManager* fpFairRootMgr = nullptr;
    TClonesArray* fArrayOfFHCalDigits; // input
    BmnFHCalEvent* fBmnFHCalEvent = nullptr; ; // output

    TString fConfigFile;
    bool fIsExp;

    float fworkTime;
  ClassDef(BmnFHCalReconstructor,1);
};

#endif /* BMNFHCALRECONSTRUCTOR_H */
