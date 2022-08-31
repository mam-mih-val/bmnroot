#ifndef BMNPROFILOMETERSOURCE_H
#define BMNPROFILOMETERSOURCE_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <zmq.h>
// ROOT
#include "TChain.h"
#include "TClonesArray.h"
#include <TBufferFile.h>
#include <TPRegexp.h>
// FairRoot
#include "FairRootManager.h"
#include "FairRootFileSink.h"
#include "FairOnlineSource.h"
#include "FairEventHeader.h"
#include "FairRunAna.h"
// BmnRoot
#include "BmnADCDigit.h"
#include "BmnEnums.h"
#include "DigiArrays.h"
#include "BmnEventHeader.h"
#include "BmnProfRawTools.h"

using namespace std;

struct ProfBoard {
    ProfBoard(){
        board_id = 0;
    }
    uint16_t board_id;
    vector<vector<uint16_t > > adc1, adc2;
//    vector<vector<uint32_t > > adc1, adc2;
    vector<uint16_t> adc1_word, adc2_word;
//    vector<uint32_t> adc1_word, adc2_word;
//    vector<vector<int>> adc1_word_char;
};

class BmnProfilometerSource : public FairOnlineSource {
public:
    BmnProfilometerSource(
            vector<string> addr = {"tcp://159.93.49.126:5601", "tcp://159.93.49.126:5602"},
    vector<string> boardIds = {"board1", "board2"}
    );
    virtual ~BmnProfilometerSource();

    Bool_t Init();
    Int_t ReadEvent(UInt_t i = 0);
    void Close();
    //    void Reset();
    void FillEventHeader(FairEventHeader* feh);

private:
    const string DataHeader = "DAT";
    const string DataTrailer = "SPILLEND";
    int ProcessBuffer(uint32_t *word, size_t len, ProfBoard * board);

    void * _ctx;
    void * _rawSocket;

    Bool_t fFirstEvent;

    Int_t fRunId;
    Int_t fPeriodId;

    Int_t iEventNumber;
    TClonesArray* fArr;
    vector<string> fAddrs;
    vector<string> fBoardIds;
//    vector<TClonesArray*> fProfADC;
    map<string, ProfBoard*> fBoardsMap;

//    vector<vector<uint32_t > > adc1, adc2;
//    vector<uint32_t> adc1_word, adc2_word;
//    vector<vector<int>> adc1_word_char;

    ClassDef(BmnProfilometerSource, 1);
};

#endif /* BMNPROFILOMETERSOURCE_H */
