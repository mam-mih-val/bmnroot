#include "BmnTrigRaw2Digit.h"
#include <climits>

const UShort_t kNCHANNELS = 8; // number of channels in one HPTDC

BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString mappingFile) {
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    //========== read mapping file            ==========//
    fMapFile.open((fMapFileName).Data());
    if (!fMapFile.is_open()) {
        cout << "Error opening map-file (" << fMapFileName << ")!" << endl;
    }

    TString dummy;
    TString name;
    UInt_t ser;
    Short_t slot, ch;

    fMapFile >> dummy >> dummy >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> name >> hex >> ser >> dec >> slot >> ch;
        if (!fMapFile.good()) break;
        BmnTrigMapping record;
        record.name = name;
        record.serial = ser;
        record.slot = slot;
        record.channel = ch;
        fMap.push_back(record);
    }
    fMapFile.close();
    //==================================================//
}

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *t0, TClonesArray *bc1, TClonesArray *bc2, TClonesArray *veto, TClonesArray *fd, TClonesArray *bd, Double_t& t0time, Double_t *t0width, Double_t *dnlcor) {

    const int tdc72vhl_tdcid2tdcnum[16] = {2, 1, 0, 5, 4, 3, 8, 7, 6, -1, -1, -1, -1, -1, -1, -1};
    const int tdc72vhl_tdcch2ch[32] = {7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0};
    //    cout <<" IN\n";
    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
        BmnTrigMapping tM = fMap[iMap];
        for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); ++iTdc) {
            BmnTDCDigit* tdcDig1 = (BmnTDCDigit*) tdc->At(iTdc);
            if (tdcDig1->GetSerial() != tM.serial || tdcDig1->GetSlot() != tM.slot) continue;
            if (!tdcDig1->GetLeading()) continue; // use only leading digits
//            UShort_t rChannel1 = tdcDig1->GetHptdcId() * kNCHANNELS + tdcDig1->GetChannel();
            UShort_t rChannel1 = tdc72vhl_tdcid2tdcnum[tdcDig1->GetHptdcId()]*8 + tdc72vhl_tdcch2ch[(tdcDig1->GetChannel()*4) % 32];
            if (rChannel1 != tM.channel) continue;
            BmnTDCDigit* nearestDig = NULL;
            UInt_t nearestTime = UINT_MAX;

            for (Int_t jTdc = 0; jTdc < tdc->GetEntriesFast(); ++jTdc) {
                if (iTdc == jTdc) continue;
                BmnTDCDigit* tdcDig2 = (BmnTDCDigit*) tdc->At(jTdc);
                if (tdcDig2->GetSerial() != tM.serial || tdcDig2->GetSlot() != tM.slot) continue;
                if (tdcDig2->GetLeading()) continue; // use only trailing digits as a pair to leading one
//                UShort_t rChannel2 = tdcDig2->GetHptdcId() * kNCHANNELS + tdcDig2->GetChannel();
                UShort_t rChannel2 = tdc72vhl_tdcid2tdcnum[tdcDig2->GetHptdcId()]*8 + tdc72vhl_tdcch2ch[(tdcDig2->GetChannel()*4) % 32];
                if (rChannel1 != rChannel2) continue; // we need the same hptdc & channel to create pair
                if (tdcDig2->GetValue() < tdcDig1->GetValue()) continue; //time should be positive
                if (tdcDig2->GetValue() - tdcDig1->GetValue() < nearestTime) {
                    nearestTime = tdcDig2->GetValue() - tdcDig1->GetValue();
                    nearestDig = tdcDig2;
                }
            }

            if (nearestDig != NULL) {
                UInt_t dnl = (0x3FF & (tdcDig1->GetValue()));
                Double_t timednl = tdcDig1->GetValue();
                UInt_t chin = 0;
                if (dnlcor) {
                    chin = tdc72vhl_tdcid2tdcnum[tdcDig1->GetHptdcId()]*8 + tdc72vhl_tdcch2ch[(tdcDig1->GetChannel()*4) % 32];
                    //		    printf("raw %u double %f dnl %u cor %f\n", tdcDig1->GetValue(), timednl, dnl, *(dnlcor+chin*1024+dnl));
                    timednl += *(dnlcor + chin * 1024 + dnl);
                }
                BmnTrigDigit dig(0, rChannel1, timednl * HPTIMEBIN, (nearestDig->GetValue() - tdcDig1->GetValue()) * HPTIMEBIN);
                
                //                if (tM.name == "BC2") //in summer run there was no to, we use bc2 instead
                if (tM.name == "T0") {
                    //		    if (dnlcor) printf("raw %u double %f dnl %u cor %f chin %d\n", tdcDig1->GetValue(), timednl, dnl, *(dnlcor+chin*1024+dnl), chin);
                    t0time = timednl * HPTIMEBIN; //ns
                    if (t0width != NULL) *t0width = (nearestDig->GetValue() - tdcDig1->GetValue()) * HPTIMEBIN;
                }
                if (tM.name == "T0") {
                    TClonesArray& ar_t0 = *t0;
                    if (t0) new(ar_t0[t0->GetEntriesFast()]) BmnTrigDigit(dig);
                } else if (tM.name == "BC1") {
                    TClonesArray& ar_bc1 = *bc1;
                    if (bc1) new(ar_bc1[bc1->GetEntriesFast()]) BmnTrigDigit(dig);
                } else if (tM.name == "BC2") {
                    TClonesArray& ar_bc2 = *bc2;
                    if (bc2) new(ar_bc2[bc2->GetEntriesFast()]) BmnTrigDigit(dig);
                } else if (tM.name == "VETO") {
                    TClonesArray& ar_veto = *veto;
                    if (veto) new(ar_veto[veto->GetEntriesFast()]) BmnTrigDigit(dig);
                } else if (tM.name == "FD") {
                    TClonesArray& ar_fd = *fd;
                    if (fd) new(ar_fd[fd->GetEntriesFast()]) BmnTrigDigit(dig);
                } else {
                    for (Int_t i = 0; i < KNBDCHANNELS; ++i) {
                        if (tM.name == (TString("BD") + TString(i))) {
                            TClonesArray& ar_bd = *bd;
                            if (bd) new(ar_bd[bd->GetEntriesFast()]) BmnTrigDigit(dig);
                        }
                    }
                }
            }
        }
    }
}

ClassImp(BmnTrigRaw2Digit)

