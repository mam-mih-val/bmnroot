#include "BmnDCHdetector.h"

BmnDCHdetector::BmnDCHdetector(TString mappingFile) {
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

void BmnDCHdetector::clear() {
    for (int i = 0; i < 16; i++)for (int j = 0; j < 16; j++)for (int k = 0; k < 16; k++) data[i][j][k].clear();
    for (int i = 0; i < 16; i++)hits[i] = 0;
    T0 = 0;
}

char *BmnDCHdetector::getPlaneName(int i) {
    return (char *)Bmn_DCH_names[i & 0xF];
}

int BmnDCHdetector::set(unsigned long id, unsigned char slot, unsigned char ch, int val) {
    if (rel[slot][ch] == 0) return 0;
    int i = (rel[slot][ch] >> 8)&0xF;
    int j = (rel[slot][ch] >> 4)&0xF;
    int k = (rel[slot][ch])&0xF;
    data[i][j][k].set(val);
    hits[i]++;
    return 1;
}

void BmnDCHdetector::print() {
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

int BmnDCHdetector::getnhits(int plane) {
    return hits[plane & 0xF];
}

int BmnDCHdetector::get(int p, int ch, unsigned short **d) {
    return data[p & 0xF][(ch / 16)&0xF][ch % 16].get(d);
}

float BmnDCHdetector::get_t0() {
    return T0;
}

void BmnDCHdetector::fillEvent(TClonesArray *data) {
    clear();
    for (int i = 0; i < data->GetEntriesFast(); i++) {
        BmnTDCDigit *hit = (BmnTDCDigit*) data->At(i);
        set(hit->GetSerial(), hit->GetSlot(), hit->GetChannel(), hit->GetValue() / 10.0);
    }
}

void BmnDCHdetector::fillEvent(TClonesArray *data, TClonesArray *sync, TClonesArray *t0) {
    unsigned int t0id = 0;
    unsigned int t0time = 0, hittime = 0;
    clear();
    for (int i = 0; i < t0->GetEntriesFast(); i++) {
        BmnTDCDigit *hit = (BmnTDCDigit*) t0->At(i);
        if (hit->GetLeading()) {
            T0 = hit->GetValue() / 42.6666;
            t0id = hit->GetSerial();
            break;
        }
    }
    for (int i = 0; i < data->GetEntriesFast(); i++) {
        BmnTDCDigit *hit = (BmnTDCDigit*) data->At(i);
        for (int j = 0; j < sync->GetEntriesFast(); j++) {
            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
            if (rec->GetSerial() == t0id) t0time = rec->GetTime_ns();
            if (rec->GetSerial() == hit->GetSerial()) hittime = rec->GetTime_ns();
        }
        set(hit->GetSerial(), hit->GetSlot(), hit->GetChannel(), hit->GetValue() / 10.0 - (T0 + (t0time - hittime)));
    }
}

ClassImp(BmnDCHdetector)

