/*
 * 
 *      Author: Yulia Topko
 */

#include "BmnProfRawTools.h"
//-------------------------------
//------ functions
//-------------------------------

void BmnProfRawTools::endian_swap(uint32_t &x) {
    x = (x >> 24) |
            ((x << 8) & 0x00FF0000) |
            ((x >> 8) & 0x0000FF00) |
            (x << 24);
}

bool BmnProfRawTools::data_or_trig(const uint32_t &word) {
    return bool(word >> 31 & 0x1);
}

uint32_t BmnProfRawTools::holdb_cntr(const uint32_t &word) { // trigger = 14bit
    return ((word & 0x7FFC0000) >> 18);
}

bool BmnProfRawTools::trig_psd(const uint32_t &word) {
    return bool((word & 0x7F800000) == 0x55000000);
}

bool BmnProfRawTools::trig_nsd(const uint32_t &word) {
    return bool((word & 0x7F800000) == 0x55800000);
}

uint32_t BmnProfRawTools::trig_cntr(const uint32_t &word) {
    return (word & 0x7FFFFF);
}

bool BmnProfRawTools::adc_num(const bitset<32> &word) {
    return word[17];
}

bool BmnProfRawTools::adc_num(const uint32_t &word) {
    return word & (1 << 17);
}

bool BmnProfRawTools::fco(const bitset<32> &word) {
    return word[16];
}

bool BmnProfRawTools::fco(const uint32_t &word) {
    return word & (1 << 16);
}

int BmnProfRawTools::adc_ch(vector<bitset < 32 >> &adc_word, char channel_name) {
    bitset<12> result;
    map<char, size_t> offsets{
        {'A', 0},
        {'B', 2},
        {'C', 4},
        {'D', 6},
        {'E', 8},
        {'F', 10},
        {'G', 12},
        {'H', 14}};
    for (size_t i = 0; i < 12; i++) {
        result[11 - i] = adc_word[i / 2][i % 2 + offsets[channel_name]];
    }
    return static_cast<int> (result.to_ulong()) - 0x800;
    //	return result.to_ulong();
}

int BmnProfRawTools::adc_ch(vector<bitset < 32 >> &adc_word, size_t start_index, char channel_name) {
    unsigned int result;
    map<char, size_t> offsets{
        {'A', 0},
        {'B', 2},
        {'C', 4},
        {'D', 6},
        {'E', 8},
        {'F', 10},
        {'G', 12},
        {'H', 14}};
    for (size_t i = 0; i < 6; i++) {
        result += (((adc_word[start_index + i] >> offsets[channel_name]) & bitset<32>(3))
                << (6 - i)).to_ulong();
    }
    return static_cast<int> (result - 0x800);
}

//int BmnProfRawTools::adc_ch(vector<uint32_t> &adc_word, size_t start_index, char channel_name) {
//    uint32_t result = 0;
//    map<char, size_t> offsets{
//        {'A', 0},
//        {'B', 2},
//        {'C', 4},
//        {'D', 6},
//        {'E', 8},
//        {'F', 10},
//        {'G', 12},
//        {'H', 14}};
//    for (size_t i = 0; i < 6; i++) {
//        result += ((adc_word[start_index + i] >> offsets[channel_name]) & 1) << (10 - 2 * i + 1);
//        result += ((adc_word[start_index + i] >> offsets[channel_name] + 1) & 1) << (10 - 2 * i);
//    }
//    return result - 0x800;
//    //	return result.to_ulong();
//}





//---------------------------------------------------
// Tracker detector 128x128 strip mapping
// Y - n+ readout card 3 and 4
// X - p+ readout card 1 and 2
// Readout card #1 (X - p+ strips 1-64 ) ADC channel-E.
// Readout card #2 (X - p+ strips 65-128) ADC channel-F.
// Readout card #3 (Y - n+ strips 65-128) ADC channel-G.
// Readout card #4 (Y - n+ strips 1-64 ) ADC channel-H.
//---------------------------------------------------
//---- beam_tracker_map

vector<int> BmnProfRawTools::beam_tracker_map(vector<int> &vec, char adc_channel) {
    vector<int> res(64 * sizeof (int));

    switch (adc_channel) {
        case 'E': res = tracker_X1_chE(vec);
            break;
        case 'F': res = tracker_X2_chF(vec);
            break;
        case 'G': res = tracker_Y1_chH(vec);
            break;
        case 'H': res = tracker_Y2_chG(vec);
            break;
    }
    return res; //never reach
}

//---- tracker_X1_chE

vector<int> BmnProfRawTools::tracker_X1_chE(vector<int> &vec) {
    if (vec.size() != 64)
        cout << "E: tracker_X1_chE.size() is not 64!" << endl;

    vector<int> tracker_X1(64);

    tracker_X1[0] = vec[61];
    tracker_X1[1] = vec[61];
    tracker_X1[2] = vec[61];
    tracker_X1[3] = vec[61];
    tracker_X1[4] = vec[61];
    tracker_X1[5] = vec[62];
    tracker_X1[6] = vec[60];
    tracker_X1[7] = vec[58];
    tracker_X1[8] = vec[59];
    tracker_X1[9] = vec[57];
    tracker_X1[10] = vec[55];
    tracker_X1[11] = vec[56];
    tracker_X1[12] = vec[54];
    tracker_X1[13] = vec[52];
    tracker_X1[14] = vec[53];
    tracker_X1[15] = vec[51];
    tracker_X1[16] = vec[49];
    tracker_X1[17] = vec[50];
    tracker_X1[18] = vec[48];
    tracker_X1[19] = vec[46];
    tracker_X1[20] = vec[47];
    tracker_X1[21] = vec[45];
    tracker_X1[22] = vec[43];
    tracker_X1[23] = vec[44];
    tracker_X1[24] = vec[42];
    tracker_X1[25] = vec[40];
    tracker_X1[26] = vec[41];
    tracker_X1[27] = vec[39];
    tracker_X1[28] = vec[37];
    tracker_X1[29] = vec[38];
    tracker_X1[30] = vec[36];
    tracker_X1[31] = vec[34];
    tracker_X1[32] = vec[35];
    tracker_X1[33] = vec[33];
    tracker_X1[34] = vec[31];
    tracker_X1[35] = vec[32];
    tracker_X1[36] = vec[30];
    tracker_X1[37] = vec[28];
    tracker_X1[38] = vec[29];
    tracker_X1[39] = vec[27];
    tracker_X1[40] = vec[25];
    tracker_X1[41] = vec[26];
    tracker_X1[42] = vec[24];
    tracker_X1[43] = vec[22];
    tracker_X1[44] = vec[23];
    tracker_X1[45] = vec[21];
    tracker_X1[46] = vec[19];
    tracker_X1[47] = vec[20];
    tracker_X1[48] = vec[18];
    tracker_X1[49] = vec[16];
    tracker_X1[50] = vec[17];
    tracker_X1[51] = vec[15];
    tracker_X1[52] = vec[13];
    tracker_X1[53] = vec[14];
    tracker_X1[54] = vec[12];
    tracker_X1[55] = vec[10];
    tracker_X1[56] = vec[11];
    tracker_X1[57] = vec[9];
    tracker_X1[58] = vec[7];
    tracker_X1[59] = vec[8];
    tracker_X1[60] = vec[6];
    tracker_X1[61] = vec[4];
    tracker_X1[62] = vec[5];
    tracker_X1[63] = vec[3];
    //  ok = vec[63];
    //	test = vec[0], vec[1], vec[2]
    return tracker_X1;
}

//---- tracker_X2_chF

vector<int> BmnProfRawTools::tracker_X2_chF(vector<int> &vec) {
    if (vec.size() != 64)
        cout << "E: tracker_X2_chF.size() is not 64!" << endl;

    vector<int> tracker_X2(64);

    tracker_X2[0] = vec[3];
    tracker_X2[1] = vec[4];
    tracker_X2[2] = vec[5];
    tracker_X2[3] = vec[6];
    tracker_X2[4] = vec[7];
    tracker_X2[5] = vec[8];
    tracker_X2[6] = vec[9];
    tracker_X2[7] = vec[10];
    tracker_X2[8] = vec[11];
    tracker_X2[9] = vec[12];
    tracker_X2[10] = vec[13];
    tracker_X2[11] = vec[14];
    tracker_X2[12] = vec[15];
    tracker_X2[13] = vec[16];
    tracker_X2[14] = vec[17];
    tracker_X2[15] = vec[18];
    tracker_X2[16] = vec[19];
    tracker_X2[17] = vec[20];
    tracker_X2[18] = vec[21];
    tracker_X2[19] = vec[22];
    tracker_X2[20] = vec[23];
    tracker_X2[21] = vec[24];
    tracker_X2[22] = vec[25];
    tracker_X2[23] = vec[26];
    tracker_X2[24] = vec[27];
    tracker_X2[25] = vec[28];
    tracker_X2[26] = vec[29];
    tracker_X2[27] = vec[30];
    tracker_X2[28] = vec[31];
    tracker_X2[29] = vec[32];
    tracker_X2[30] = vec[33];
    tracker_X2[31] = vec[34];
    tracker_X2[32] = vec[35];
    tracker_X2[33] = vec[36];
    tracker_X2[34] = vec[37];
    tracker_X2[35] = vec[38];
    tracker_X2[36] = vec[39];
    tracker_X2[37] = vec[40];
    tracker_X2[38] = vec[41];
    tracker_X2[39] = vec[42];
    tracker_X2[40] = vec[43];
    tracker_X2[41] = vec[44];
    tracker_X2[42] = vec[45];
    tracker_X2[43] = vec[46];
    tracker_X2[44] = vec[47];
    tracker_X2[45] = vec[48];
    tracker_X2[46] = vec[49];
    tracker_X2[47] = vec[50];
    tracker_X2[48] = vec[51];
    tracker_X2[49] = vec[52];
    tracker_X2[50] = vec[53];
    tracker_X2[51] = vec[54];
    tracker_X2[52] = vec[55];
    tracker_X2[53] = vec[56];
    tracker_X2[54] = vec[57];
    tracker_X2[55] = vec[58];
    tracker_X2[56] = vec[59];
    tracker_X2[57] = vec[60];
    tracker_X2[58] = vec[61];
    tracker_X2[59] = vec[62];
    tracker_X2[60] = vec[63];
    tracker_X2[61] = vec[63];
    tracker_X2[62] = vec[63];
    tracker_X2[63] = vec[63];
    //  test = vec[0], vec[1], vec[2]
    return tracker_X2;
}

//----- tracker_Y1_chH

vector<int> BmnProfRawTools::tracker_Y1_chH(vector<int> &vec) {
    if (vec.size() != 64)
        cout << "E: tracker_Y1_chH.size() is not 64!" << endl;

    vector<int> tracker_Y1(64);
    tracker_Y1[0] = vec[63];
    tracker_Y1[1] = vec[63];
    tracker_Y1[2] = vec[63];
    tracker_Y1[3] = vec[63];
    tracker_Y1[4] = vec[61];
    tracker_Y1[5] = vec[62];
    tracker_Y1[6] = vec[60];
    tracker_Y1[7] = vec[58];
    tracker_Y1[8] = vec[59];
    tracker_Y1[9] = vec[57];
    tracker_Y1[10] = vec[55];
    tracker_Y1[11] = vec[56];
    tracker_Y1[12] = vec[54];
    tracker_Y1[13] = vec[52];
    tracker_Y1[14] = vec[53];
    tracker_Y1[15] = vec[51];
    tracker_Y1[16] = vec[49];
    tracker_Y1[17] = vec[50];
    tracker_Y1[18] = vec[48];
    tracker_Y1[19] = vec[46];
    tracker_Y1[20] = vec[47];
    tracker_Y1[21] = vec[45];
    tracker_Y1[22] = vec[43];
    tracker_Y1[23] = vec[44];
    tracker_Y1[24] = vec[42];
    tracker_Y1[25] = vec[40];
    tracker_Y1[26] = vec[41];
    tracker_Y1[27] = vec[39];
    tracker_Y1[28] = vec[37];
    tracker_Y1[29] = vec[38];
    tracker_Y1[30] = vec[36];
    tracker_Y1[31] = vec[34];
    tracker_Y1[32] = vec[35];
    tracker_Y1[33] = vec[33];
    tracker_Y1[34] = vec[31];
    tracker_Y1[35] = vec[32];
    tracker_Y1[36] = vec[30];
    tracker_Y1[37] = vec[28];
    tracker_Y1[38] = vec[29];
    tracker_Y1[39] = vec[27];
    tracker_Y1[40] = vec[25];
    tracker_Y1[41] = vec[26];
    tracker_Y1[42] = vec[24];
    tracker_Y1[43] = vec[22];
    tracker_Y1[44] = vec[23];
    tracker_Y1[45] = vec[21];
    tracker_Y1[46] = vec[19];
    tracker_Y1[47] = vec[20];
    tracker_Y1[48] = vec[18];
    tracker_Y1[49] = vec[16];
    tracker_Y1[50] = vec[17];
    tracker_Y1[51] = vec[15];
    tracker_Y1[52] = vec[13];
    tracker_Y1[53] = vec[14];
    tracker_Y1[54] = vec[12];
    tracker_Y1[55] = vec[10];
    tracker_Y1[56] = vec[11];
    tracker_Y1[57] = vec[9];
    tracker_Y1[58] = vec[7];
    tracker_Y1[59] = vec[8];
    tracker_Y1[60] = vec[6];
    tracker_Y1[61] = vec[4];
    tracker_Y1[62] = vec[5];
    tracker_Y1[63] = vec[3];
    //	test = vec[0],vec[1], vec[2]; goto test_tracker_Y1
    return tracker_Y1;
}

//----- tracker_Y2_chG

vector<int> BmnProfRawTools::tracker_Y2_chG(vector<int> &vec) {
    if (vec.size() != 64)
        cout << "E: tracker_Y2_chG.size() is not 64!" << endl;

    vector<int> tracker_Y2(64);

    tracker_Y2[0] = vec[63];
    tracker_Y2[1] = vec[62];
    tracker_Y2[2] = vec[61];
    tracker_Y2[3] = vec[60];
    tracker_Y2[4] = vec[59];
    tracker_Y2[5] = vec[58];
    tracker_Y2[6] = vec[57];
    tracker_Y2[7] = vec[56];
    tracker_Y2[8] = vec[55];
    tracker_Y2[9] = vec[54];
    tracker_Y2[10] = vec[53];
    tracker_Y2[11] = vec[52];
    tracker_Y2[12] = vec[51];
    tracker_Y2[13] = vec[50];
    tracker_Y2[14] = vec[49];
    tracker_Y2[15] = vec[48];
    tracker_Y2[16] = vec[47];
    tracker_Y2[17] = vec[46];
    tracker_Y2[18] = vec[45];
    tracker_Y2[19] = vec[44];
    tracker_Y2[20] = vec[43];
    tracker_Y2[21] = vec[42];
    tracker_Y2[22] = vec[41];
    tracker_Y2[23] = vec[40];
    tracker_Y2[24] = vec[39];
    tracker_Y2[25] = vec[38];
    tracker_Y2[26] = vec[37];
    tracker_Y2[27] = vec[36];
    tracker_Y2[28] = vec[35];
    tracker_Y2[29] = vec[34];
    tracker_Y2[30] = vec[33];
    tracker_Y2[31] = vec[32];
    tracker_Y2[32] = vec[31];
    tracker_Y2[33] = vec[30];
    tracker_Y2[34] = vec[29];
    tracker_Y2[35] = vec[28];
    tracker_Y2[36] = vec[27];
    tracker_Y2[37] = vec[26];
    tracker_Y2[38] = vec[25];
    tracker_Y2[39] = vec[24];
    tracker_Y2[40] = vec[23];
    tracker_Y2[41] = vec[22];
    tracker_Y2[42] = vec[21];
    tracker_Y2[43] = vec[20];
    tracker_Y2[44] = vec[19];
    tracker_Y2[45] = vec[18];
    tracker_Y2[46] = vec[17];
    tracker_Y2[47] = vec[16];
    tracker_Y2[48] = vec[15];
    tracker_Y2[49] = vec[14];
    tracker_Y2[50] = vec[13];
    tracker_Y2[51] = vec[12];
    tracker_Y2[52] = vec[11];
    tracker_Y2[53] = vec[10];
    tracker_Y2[54] = vec[9];
    tracker_Y2[55] = vec[8];
    tracker_Y2[56] = vec[7];
    tracker_Y2[57] = vec[6];
    tracker_Y2[58] = vec[5];
    tracker_Y2[59] = vec[4];
    tracker_Y2[60] = vec[3];
    tracker_Y2[61] = vec[3];
    tracker_Y2[62] = vec[3];
    tracker_Y2[63] = vec[3];
    //  test vec[0], vec[1], vec[2]
    return tracker_Y2;
}

//----- tracker_test_signal

vector<int> BmnProfRawTools::tracker_tst_sgnl(vector<int> &chE, vector<int> &chF,
        vector<int> &chH, vector<int> &chG) {
    vector<int> tst_sgnl(4 * 3);

    tst_sgnl[0] = chE[0];
    tst_sgnl[1] = chE[1];
    tst_sgnl[2] = chE[2];
    tst_sgnl[3] = chF[0];
    tst_sgnl[4] = chF[1];
    tst_sgnl[5] = chF[2];
    tst_sgnl[6] = chH[0];
    tst_sgnl[7] = chH[1];
    tst_sgnl[8] = chH[2];
    tst_sgnl[9] = chG[0];
    tst_sgnl[10] = chG[1];
    tst_sgnl[11] = chG[2];

    return tst_sgnl;
}

