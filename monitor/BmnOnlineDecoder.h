/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnOnlineDecoder.h
 * Author: ilnur
 *
 * Created on November 14, 2016, 1:51 PM
 */

#ifndef BMNONLINEDECODER_H
#define BMNONLINEDECODER_H
#include <../bmndata/BmnRawDataDecoder.h>

class BmnOnlineDecoder: public BmnRawDataDecoder {
public:
    BmnOnlineDecoder();
    BmnOnlineDecoder(TString file, ULong_t nEvents = 0);
    BmnStatus ConvertRawToRootOnline();
    virtual ~BmnOnlineDecoder();
private:

    ClassDef(BmnOnlineDecoder, 1)
};

#endif /* BMNONLINEDECODER_H */

