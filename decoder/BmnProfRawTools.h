/*
 *
 *      Author: Yulia Topko
 */
#ifndef BMNPROFRAWTOOLS_H
#define BMNPROFRAWTOOLS_H

#include <bitset>
#include <vector>
#include <map>
#include <iostream>
using namespace std;


class BmnProfRawTools {

public:
//-------------------------------
//------ functions
//-------------------------------
/*
Data format
| DATA (1)| holdb_cntr | adc_num| fco | hgfedcba ch|
| 31      | 30 : 18    | 17  	| 16  |   15 : 0   |
                    OR
| #Trig(0)|  AA - control word  |     trig_cntr_psd  |
| #Trig(0)|  AB - control word  |     trig_cntr_nsd  |
| 31      |     30 : 23         |       22 : 0       |

Holdb counter width = 13 * 2 = 26;
Holdb for every adc.

trig_p counter width = 23; in adc1
trig_n counter width = 23; in adc2
*/

static void endian_swap(uint32_t& x);
//---------------------
// Return value:
// true - data
// false - trigger
//---------------------
static bool data_or_trig(const uint32_t& word);

//---------------------
// Return value: holdb counter
// if data_or_trig() = true (data)
//---------------------
static uint32_t holdb_cntr(const uint32_t& word);

//---------------------
// Return value: self-trigger counter
// if data_or_trig() = false (trigger)
// bool function for P and N detector side
//---------------------
static bool trig_psd(const uint32_t &word);

static bool trig_nsd(const uint32_t &word);

static uint32_t trig_cntr(const uint32_t &word);

//---------------------
// Return value: number of ADC:
// 0 - 1st ADC; 1 - 2nd ADC
//---------------------
static bool adc_num(const bitset<32>& word);

//---------------------
// Return value: frame
// 0 - data belongs to one frame;
// 1 - to another
//---------------------
static bool fco(const bitset<32>& word);

//---------------------
// Return value:
// value of N ADC channel
//---------------------
static int adc_ch(vector<bitset<32>> &adc_word, char channel_name);
static int adc_ch(vector<bitset<32>> &adc_word, size_t start_index, char channel_name);

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
static vector<int> beam_tracker_map (vector<int> &vec, char adc_channel);

//---- tracker_X1_chE
static vector<int> tracker_X1_chE (vector<int> &vec);

//---- tracker_X2_chF
static vector<int> tracker_X2_chF (vector<int> &vec);

//----- tracker_Y1_chH
static vector<int> tracker_Y1_chH (vector<int> &vec);

//----- tracker_Y2_chG
static vector<int> tracker_Y2_chG (vector<int> &vec);

//----- tracker_test_signal
static vector<int> tracker_tst_sgnl(vector<int> &chE, vector<int> &chF,
							 vector<int> &chH, vector<int> &chG);
};

#endif /* BMNPROFRAWTOOLS_H */
