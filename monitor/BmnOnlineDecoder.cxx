/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnOnlineDecoder.cxx
 * Author: ilnur
 * 
 * Created on November 14, 2016, 1:51 PM
 */

#include "BmnOnlineDecoder.h"

BmnOnlineDecoder::BmnOnlineDecoder() : BmnRawDataDecoder() {
}

BmnOnlineDecoder::BmnOnlineDecoder(TString file, ULong_t nEvents) : BmnRawDataDecoder(file, nEvents) {
}

BmnOnlineDecoder::BmnOnlineDecoder(const BmnOnlineDecoder& orig) {
}

BmnStatus BmnOnlineDecoder::ConvertRawToRootOnline(){
    
}

BmnOnlineDecoder::~BmnOnlineDecoder() {
}

ClassImp(BmnOnlineDecoder);
