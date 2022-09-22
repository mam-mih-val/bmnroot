#include "BmnDataTriggerInfo.h"

BmnDataTriggerInfo::BmnDataTriggerInfo() :
fPeriod(7) {
    // Defining trigger map ...
    fTriggerMap["Beam Trigger + BD ( > 0) & Si (> 2)"] = "BT+BD0+FD2";
    fTriggerMap["Beam Trigger + BD (>3) & Si(>3)"] = "BT+BD3+FD3";
    fTriggerMap["Beam Trigger + BD > 1 & Si > 3"] = "BT+BD1+FD3";
    fTriggerMap["Beam Trigger + BD >5"] = "BT+BD5";
    fTriggerMap["Beam Trigger + BD(>1) and Si(>2)"] = "BT+BD1+FD2";
    fTriggerMap["Beam Trigger + BD(>=2) and FD(>=2)"] = "BT+BD1+FD1";
    fTriggerMap["Beam Trigger + BD(>=2) and FD(>=3)"] = "BT+BD1+FD2";
    fTriggerMap["Beam Trigger + BD(>=2) and FD(>=4)"] = "BT+BD1+FD3";
    fTriggerMap["Beam Trigger + BD(>=3)"] = "BT+BD2";
    fTriggerMap["Beam Trigger + BD(>=3) and FD(>=3)"] = "BT+BD2+FD2";
    fTriggerMap["Beam Trigger + BD(>=3) and FD(>=4)"] = "BT+BD2+FD3";
    fTriggerMap["Beam Trigger + BD>3"] = "BT+BD3";
    fTriggerMap["Beam Trigger + Si (> 4)"] = "BT+FD4";
    fTriggerMap["Beam Trigger + Si >3"] = "BT+FD3";
    fTriggerMap["Beam Trigger + Si >= 3"] = "BT+FD2";
    fTriggerMap["Beam Trigger + Veto Off + BD(>=4)"] = "BT+BD3+VetoOff";
    fTriggerMap["Beam Trigger (BC1+BC2+T0+Veto)"] = "BT";
    fTriggerMap["Beam BC1 + BC2"] = "BC1+BC2";
    fTriggerMap["Beam BC1"] = "BC1";
}

TString BmnDataTriggerInfo::GetTrigger(Int_t run) {
    TObjArray* runRecord = ElogRecord::GetRecords(fPeriod, run);

    TString trigger = "";

    TIter it(runRecord);
    ElogRecord* curRecord;
    while ((curRecord = (ElogRecord*) it())) {
        if (curRecord->GetTriggerId())
            trigger = ElogTrigger::GetTrigger(*(curRecord->GetTriggerId()))->GetTriggerInfo();
    }

    auto trigIter = fTriggerMap.find(trigger);

    if (trigIter != fTriggerMap.end())
        trigger = trigIter->second;

    return trigger;
}