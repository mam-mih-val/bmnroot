#include "BmnDchRaw2Digit.h"

BmnDchRaw2Digit::BmnDchRaw2Digit(TString mappingFile) {
    static const char *tmp[16]= {
        "VA_1", "VB_1", "UA_1", "UB_1", "XA_1", "XB_1", "YA_1", "YB_1",
        "VA_2", "VB_2", "UA_2", "UB_2", "XA_2", "XB_2", "YA_2", "YB_2"
    };
    for(int i=0;i<16;i++) sprintf(Bmn_DCH_names[i],"%s",tmp[i]);
    TString dummy, plane;
    int id;
    int group, slot, chan_hi, chan_lo;
    ifstream in;
    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";
    in.open((path + mappingFile).Data());
    in >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy>>dummy;
    while (!in.eof()) {
        in >> plane >> group >> std::hex >> id >> std::dec >> slot >> chan_lo>>chan_hi;
        if (!in.good()) break;

        int i;
        for (i = 0; i < 16; i++) if (plane.Contains(Bmn_DCH_names[i])) break;
        if (i == 16) continue;
        for (int j = chan_lo; j <= chan_hi; j++) dhc_map_element[i][group][j % 16].set((unsigned long) id, (unsigned char) slot, (unsigned char) j);
    }
    in.close();
    for (int i = 0; i < 21; i++)for (int j = 0; j < 64; j++)rel[i][j] = -1;
    for (int i = 0; i < 16; i++)for (int j = 0; j < 16; j++)for (int k = 0; k < 16; k++) rel[dhc_map_element[i][j][k].slot][dhc_map_element[i][j][k].chan] = (i << 8) | (j << 4) | k;
}

void BmnDchRaw2Digit::clear() {
    for (int i = 0; i < 16; i++)for (int j = 0; j < 16; j++)for (int k = 0; k < 16; k++) data[i][j][k].clear();
    for (int i = 0; i < 16; i++)digits[i] = 0;
    T0 = 0;
    EVENT=TIME_SEC=TIME_NS=0;
}

char *BmnDchRaw2Digit::getPlaneName(int i) {
    return (char *)Bmn_DCH_names[i & 0xF];
}

void BmnDchRaw2Digit::getEventInfo(long long *ev,long long *t1,long long *t2){
    *ev=EVENT;
    *t1=TIME_SEC;
    *t2=TIME_NS;
}

int BmnDchRaw2Digit::set(unsigned long id, unsigned char slot, unsigned char ch, int val) {
    if (rel[slot][ch] == 0) return 0;
    int i = (rel[slot][ch] >> 8)&0xF;
    int j = (rel[slot][ch] >> 4)&0xF;
    int k = (rel[slot][ch])&0xF;
    data[i][j][k].set(val);
    digits[i]++;
    return 1;
}

void BmnDchRaw2Digit::print() {
    cout << "Plane\tGroup\tCrate#\t\tSlot\tCh_Lo\tCh_hi\n";
    cout << "=====================================================\n";
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++)for (int k = 0; k < 1; k++) {
                if (dhc_map_element[i][j][k].id) {
                    cout << Bmn_DCH_names[i] << "\t" << j << "\t0x" << std::hex << dhc_map_element[i][j][k].id << std::dec << "\t";
                    cout << (int) dhc_map_element[i][j][k].slot << "\t" << (int) dhc_map_element[i][j][k].chan << "\t" << (int) dhc_map_element[i][j][k].chan + 15 << "\n";
                }
            }
        cout << "\n";
    }
}

int BmnDchRaw2Digit::getndigit(int plane) {
    return digits[plane & 0xF];
}

int BmnDchRaw2Digit::getdata(int p, int ch, unsigned short **d) {
    return data[p & 0xF][(ch / 16)&0xF][ch % 16].get(d);
}

float BmnDchRaw2Digit::get_t0() {
    return T0;
}

void BmnDchRaw2Digit::fillEvent(TClonesArray *dataArray) {
    clear();
    for (int i = 0; i < dataArray->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) dataArray->At(i);
        set(digit->GetSerial(), digit->GetSlot(), digit->GetChannel(), digit->GetValue() / 10.0);
    }
}

void BmnDchRaw2Digit::fillEvent(TClonesArray *dataArray, TClonesArray *sync, TClonesArray *t0) {
    unsigned int t0id = 0;
    long long t0time = 0, digittime = 0;
    clear();
    for (int i = 0; i < t0->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) t0->At(i);
        if (digit->GetLeading()) {
            T0 = digit->GetValue() / 42.6666;
            t0id = digit->GetSerial();
            break;
        }
    }
    for (int i = 0; i < dataArray->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) dataArray->At(i);
        for (int j = 0; j < sync->GetEntriesFast(); j++) {
            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
            if (rec->GetSerial() == t0id){
               EVENT    = rec->GetEvent();
               TIME_SEC = rec->GetTime_sec();
               TIME_NS  = rec->GetTime_ns();
               t0time   = TIME_SEC*1000000000LL+TIME_NS;
            }
            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_sec()*1000000000LL+rec->GetTime_ns();
        }
        set(digit->GetSerial(), digit->GetSlot(), digit->GetChannel(), digit->GetValue() / 10.0 - (T0 + (t0time - digittime)));
    }
}
void BmnDchRaw2Digit::fillEvent(TClonesArray *dataArray, TClonesArray *sync, TClonesArray *t0, TClonesArray *dhcdigit) {
    unsigned int t0id = 0;
    long long t0time = 0, digittime = 0;
    clear();
    for (int i = 0; i < t0->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) t0->At(i);
        if (digit->GetLeading()) {
            T0 = digit->GetValue() / 42.6666;
            t0id = digit->GetSerial();
            break;
        }
    }
    for (int i = 0; i < dataArray->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) dataArray->At(i);
        for (int j = 0; j < sync->GetEntriesFast(); j++) {
            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
            if (rec->GetSerial() == t0id){
               EVENT    = rec->GetEvent();
               TIME_SEC = rec->GetTime_sec();
               TIME_NS  = rec->GetTime_ns();
               t0time   = TIME_SEC*1000000000LL+TIME_NS;
            }
            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_sec()*1000000000LL+rec->GetTime_ns();
        }
        int plane = (rel[digit->GetSlot()][digit->GetChannel()] >> 8)&0xF;
        int group = (rel[digit->GetSlot()][digit->GetChannel()] >> 4)&0xF;
        int chan = (rel[digit->GetSlot()][digit->GetChannel()])&0xF;
        float tm = digit->GetValue() / 10.0 - (T0 + (t0time - digittime));
        TClonesArray &ar_dch = *dhcdigit;
        new(ar_dch[dhcdigit->GetEntriesFast()]) BmnDchDigit(plane,group*16+chan,tm,0);
    }
}
ClassImp(BmnDchRaw2Digit)

