#include "BmnTrigRaw2Digit.h"
#include <climits>
#include <root/RtypesCore.h>
#include <root/TPRegexp.h>

BmnTrigParameters::BmnTrigParameters() {
    for (UInt_t i = 0; i < CHANNEL_COUNT_MAX; i++) {
        for (UInt_t j = 0; j < TDC_BIN_COUNT; j++)
            INL[i][j] = 0u;
        ChannelMap[i] = 0u;
        branchArrayPtr[i] = NULL;
        t[i] = 0.0;
    }
}

BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString PlacementMapFile, TString StripMapFile, TTree *digiTree) {
    ReadPlacementMap(PlacementMapFile);
    for (auto &el : fPlacementMap) {
        BmnTrigParameters* par = el.second;
        ReadINLFromFile(par);
    }
    ReadChannelMap(StripMapFile);
    if (digiTree == NULL)
        return;
    // Create corresponding branches for each trigger.       //
    // different channels of the one trigger will be stored //
    // in one branch with different mod ID  //
    for (BmnTrigChannelData &record : fMap) {
        TString detName = record.name;
        TClass* cl = detName.Contains("TQDC") ?
                BmnTrigWaveDigit::Class() : BmnTrigDigit::Class();
        TBranch* br = digiTree->GetBranch(detName.Data());
        if (!br) {
            TClonesArray *ar = new TClonesArray(cl);
            ar->SetName(detName.Data());
            digiTree->Branch(detName.Data(), &ar);
            trigArrays.push_back(ar);
            record.branchArrayPtr = ar;
        } else
            for (auto *tca : trigArrays) {
                if (TString(tca->GetName()) == detName) {
                    record.branchArrayPtr = tca;
                    break;
                }
            }
    }
    // Fill elements of placement map with channel->(strip, mod, branchRef) map //
    for (BmnTrigChannelData &record : fMap) {
        map< PlMapKey, BmnTrigParameters*>::iterator itPar = fPlacementMap.find(PlMapKey(record.serial, record.slot));
        if (itPar == fPlacementMap.end()) {
            printf("CrateSeral %08X slot %u not found in the placement map!\n", record.serial, record.slot);
            continue;
        }
        BmnTrigParameters *par = itPar->second;
        par->ChannelMap[record.channel] = record.module;
        par->branchArrayPtr[record.channel] = record.branchArrayPtr;
    }
}

BmnStatus BmnTrigRaw2Digit::ReadPlacementMap(TString mappingFile) {
    TString PlMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    printf("Reading Triggers placement mapping file %s...\n", PlMapFileName.Data());
    ifstream pmFile;
    pmFile.open(PlMapFileName.Data());
    if (!pmFile.is_open()) {
        cout << "Error opening map-file (" << PlMapFileName << ")!" << endl;
    }
    string dummy;
    string name;
    UInt_t crateSerial, boardSerial;
    UShort_t slot;

    //    regex reBoardName("(\\D+)(\\d+)(.*)");
    TPRegexp reBoardName("(\\D+)(\\d+)(.*)");
    pmFile >> dummy >> dummy >> dummy >> dummy;
    pmFile >> dummy;
    while (!pmFile.eof()) {
        pmFile >> name >> hex >> crateSerial >> dec >> slot >> hex >> boardSerial >> dec;
        if (!pmFile.good()) break;
        TString channelCountStr = name;
        //        string channelCountStr = name;
        UInt_t channelCount = CHANNEL_COUNT_MAX;
        if (reBoardName.MatchB(name)) {
            //        if (regex_match(name, reBoardName)){
            //            channelCountStr = regex_replace(name, reBoardName, "$2");
            reBoardName.Substitute(channelCountStr, "$2");
            channelCount = strtoul(channelCountStr.Data(), nullptr, 10);
            //            channelCount = strtoul(channelCountStr.c_str(), nullptr, 10);
        }
        BmnTrigParameters * par = new BmnTrigParameters();
        par->BoardSerial = boardSerial;
        par->CrateSerial = crateSerial;
        par->slot = slot;
        par->name = name;
        par->ChannelCount = channelCount;
        fPlacementMap.insert(pair<PlMapKey, BmnTrigParameters*> (PlMapKey(par->CrateSerial, par->slot), par));
    }
    pmFile.close();
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::ReadChannelMap(TString mappingFile) {
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    printf("Reading Triggers strip mapping file %s...\n", fMapFileName.Data());
    //========== read mapping file            ==========//
    fMapFile.open((fMapFileName).Data());
    if (!fMapFile.is_open()) {
        cout << "Error opening map-file (" << fMapFileName << ")!" << endl;
    }

    TString dummy;
    TString name;
    UInt_t ser;
    Short_t slot, ch, mod;

    fMapFile >> dummy >> dummy >> dummy >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> name >> mod >> hex >> ser >> dec >> slot >> ch;
        if (!fMapFile.good()) break;
        BmnTrigChannelData record;
        record.branchArrayPtr = NULL;
        record.name = name;
        record.serial = ser;
        record.module = mod;
        record.slot = slot;
        record.channel = ch;
        fMap.push_back(record);
    }
    fMapFile.close();
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::ReadINLFromFile(BmnTrigParameters* par) {
    fstream ff;
    fINLFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") +
            Serial2FileName(par->name, par->BoardSerial);
    ff.open((fINLFileName).Data(), ios::in);
    if (!ff.is_open()) {
        cout << "Error opening INL-file (" << fINLFileName << ")!" << endl;
        return kBMNERROR;
    }
    printf("Open INL file %s\n", fINLFileName.Data());
    TPRegexp reInlHdr("\\[.*(inl_corr).*\\]"); // INL block header
    //    regex reInlHdr("\\[.*(inl_corr).*\\]"); // INL block header
    //    regex reInlChannel("\\s*(\\d+)=(.+)"); // chID=c0, c1, ...
    Bool_t isInlHdr = kFALSE;
    while (!ff.eof()) {
        string line;
        std::getline(ff, line, '\n');
        if (reInlHdr.MatchB(line)) {
            //        if (regex_match(line, reInlHdr)) {
            isInlHdr = kTRUE;
            break;
        }
    }
    if (!isInlHdr) {
        printf("Incorrect INL file format!\n");
        return kBMNERROR;
    }
    UShort_t channelID = 0;
    while (!ff.eof()) {
        string line;
        std::getline(ff, line, '\n');
        //        printf("Read %lu \n", line.length());
        //        if (!regex_match(line, reInlChannel))
        //            continue;
        //        line = regex_replace(line, reInlChannel, "$1 $2");
        //        printf("%s\n", line.c_str());
        istringstream ss(line);
        ss >> channelID;
        //printf("Channel ID = %u\n", channelID);
        UShort_t i_bin = 0;
        while (ss.tellg() != -1) {
            if (i_bin > TDC_BIN_COUNT) {
                perror("INL File contains too many bins in channel.\n");
                ff.close();
                return kBMNERROR;
            }
            if (ss.peek() == ',' || ss.peek() == '=') {
                ss.ignore();
            }
            ss >> par->INL[channelID][i_bin];
            i_bin++;
        }
    }
    ff.close();
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *adc) {
    // Matching of ADC/TDC based on the fact that (TDC_i - TDC_j) > 296ns correspond to different ADC
    // and (TDC_i - (ADC_i - Trig_i) ) ~ 0
    std::vector<Double_t> times;
    std::vector<Double_t> diff;
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); iAdc++) {
        times.clear();
        diff.clear();
        BmnTQDCADCDigit *adcDig = (BmnTQDCADCDigit*) adc->At(iAdc);
        UShort_t iChannel = adcDig->GetChannel();
        auto plIter = fPlacementMap.find(PlMapKey(adcDig->GetSerial(), adcDig->GetSlot()));
        if (plIter == fPlacementMap.end())
            continue;
        BmnTrigParameters * par = plIter->second;
        UShort_t iMod = par->ChannelMap[iChannel];
        Double_t adcTimestamp = adcDig->GetAdcTimestamp() * ADC_CLOCK_TQDC16VS;
        Double_t trgTimestamp = adcDig->GetTrigTimestamp() * ADC_CLOCK_TQDC16VS;

        for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); ++iTdc) {
            BmnTDCDigit* tdcDig = (BmnTDCDigit*) tdc->At(iTdc);
            if (tdcDig->GetSerial() != adcDig->GetSerial() || tdcDig->GetSlot() != adcDig->GetSlot()) continue;
            if (tdcDig->GetChannel() != iChannel) continue;
            Double_t time = (tdcDig->GetValue() + par->INL[iChannel][tdcDig->GetValue() % TDC_BIN_COUNT]) * TDC_CLOCK / TDC_BIN_COUNT;
            // Double_t tdcTimestamp = tdcDig->GetTimestamp() * TDC_CLOCK;
            diff.push_back(fabs(time - (adcTimestamp - trgTimestamp)));
            times.push_back(time);
        }
        Double_t matchTime = -999.0;
        Double_t minUsed = 999.0;
        TClonesArray *trigAr = NULL;
        if (diff.size() > 0) {
            auto result = min_element(begin(diff), end(diff));
            int idx = std::distance(begin(diff), result);
            // Found the match, so let's save that as and ADC and corresponding TDC
            matchTime = times.at(idx);
            minUsed = diff.at(idx);
            trigAr = par->branchArrayPtr[iChannel];
        }
        if (trigAr != NULL/* && minUsed < 296*/) { // ADC window
            new ((*trigAr)[trigAr->GetEntriesFast()]) BmnTrigWaveDigit(
                    iMod,
                    adcDig->GetShortValue(),
                    adcDig->GetNSamples(),
                    trgTimestamp,
                    adcTimestamp,
                    matchTime);
        }

    }
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray *tdc) {
//        printf("Event \n");
    for (auto &el : fPlacementMap)
        for (Int_t i = 0; i < CHANNEL_COUNT_MAX; i++)
            el.second->t[i] = -1.0;
    for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); ++iTdc) {
        BmnTDCDigit* tdcDig = (BmnTDCDigit*) tdc->At(iTdc);
        auto plIter = fPlacementMap.find(PlMapKey(tdcDig->GetSerial(), tdcDig->GetSlot()));
        if (plIter == fPlacementMap.end())
            continue;
        BmnTrigParameters * par = plIter->second;
        UShort_t rChannel = tdcDig->GetHptdcId() * kNCHANNELS + tdcDig->GetChannel();
        Double_t time = (tdcDig->GetValue() + par->INL[rChannel][tdcDig->GetValue() % TDC_BIN_COUNT]) * TDC_CLOCK / TDC_BIN_COUNT;
        //        if (tdcDig->GetSerial() == 0x076D3892 && tdcDig->GetSlot() == 18 && rChannel == 14)
//                if (tdcDig->GetSerial() == 0x076D2E12 && tdcDig->GetSlot() == 10 && rChannel == 15)
//                    printf("\tCrateSeral %08X slot %02u channel %02u  time %+2.2f  leading %d\n", tdcDig->GetSerial(), tdcDig->GetSlot(), rChannel, time, tdcDig->GetLeading());
        if (tdcDig->GetLeading() ^ (tdcDig->GetSlot() == 18 && rChannel == 14)) {
            par->t[rChannel] = time;
        } else {
            if (time < par->t[rChannel])
                continue;
            if (par->t[rChannel] < 0)
                continue;
            UShort_t iMod = par->ChannelMap[rChannel];
            TClonesArray *trigAr = par->branchArrayPtr[rChannel];
            if (trigAr == NULL)
                continue;
            Double_t tL = par->t[rChannel];
            Double_t tT = time;
            par->t[rChannel] = -1.0;
            //            printf("OK:   tT = %f    tL = %f\n", tT, tL);
            new ((*trigAr)[trigAr->GetEntriesFast()]) BmnTrigDigit(iMod, tL, tT - tL);
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::ClearArrays() {
    for (TClonesArray *ar : trigArrays)
        ar->Clear("C");
}

ClassImp(BmnTrigRaw2Digit)

