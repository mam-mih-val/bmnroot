#include <TNamed.h>
#include <iostream>
#include <map>

#include <BmnEventHeader.h>
#include <BmnGemStripDigit.h>
#include <BmnSiliconDigit.h>
#include <BmnZDCDigit.h>
#include <BmnTrigDigit.h>
#include <BmnMwpcDigit.h>
#include <BmnDchDigit.h>
#include <BmnTof1Digit.h>
#include <BmnTof2Digit.h>
#include <BmnECALDigit.h>

#ifndef BMNDIGICONTAINER_H
#define BMNDIGICONTAINER_H 1

using namespace std;

class BmnDigiContainer : public TNamed {
public:

    /** Default constructor **/
    BmnDigiContainer() {
    };

    /** Destructor **/
    virtual ~BmnDigiContainer() {
    };

    // Setters
    void SetGemDigi(UInt_t evId, BmnGemStripDigit dig) {
        gem.insert(pair <UInt_t, BmnGemStripDigit> (evId, dig));
    }

    void SetSiliconDigi(UInt_t evId, BmnSiliconDigit dig) {
        silicon.insert(pair <UInt_t, BmnSiliconDigit> (evId, dig));
    }

    void SetZdcDigi(UInt_t evId, BmnZDCDigit dig) {
        zdc.insert(pair <UInt_t, BmnZDCDigit> (evId, dig));
    }

    void SetT0Digi(UInt_t evId, BmnTrigDigit dig) {
        t0.insert(pair <UInt_t, BmnTrigDigit> (evId, dig));
    }
    
    void SetBc1Digi(UInt_t evId, BmnTrigDigit dig) {   
        bc1.insert(pair <UInt_t, BmnTrigDigit> (evId, dig));
    }
    
    void SetBc2Digi(UInt_t evId, BmnTrigDigit dig) {   
        bc2.insert(pair <UInt_t, BmnTrigDigit> (evId, dig));
    }
    
     void SetVetoDigi(UInt_t evId, BmnTrigDigit dig) {   
        veto.insert(pair <UInt_t, BmnTrigDigit> (evId, dig));
    }
     
     void SetFdDigi(UInt_t evId, BmnTrigDigit dig) {   
        fd.insert(pair <UInt_t, BmnTrigDigit> (evId, dig));
    }
     
    void SetBdDigi(UInt_t evId, BmnTrigDigit dig) {   
        bd.insert(pair <UInt_t, BmnTrigDigit> (evId, dig));
    }

    void SetMwpcDigi(UInt_t evId, BmnMwpcDigit dig) {
        mwpc.insert(pair <UInt_t, BmnMwpcDigit> (evId, dig));
    }
    
    void SetDchDigi(UInt_t evId, BmnDchDigit dig) {
        dch.insert(pair <UInt_t, BmnDchDigit> (evId, dig));
    }

    void SetTof400Digi(UInt_t evId, BmnTof1Digit dig) {
        tof400.insert(pair <UInt_t, BmnTof1Digit> (evId, dig));
    }

    void SetTof700Digi(UInt_t evId, BmnTof2Digit dig) {
        tof700.insert(pair <UInt_t, BmnTof2Digit> (evId, dig));
    }

    void SetEcalDigi(UInt_t evId, BmnECALDigit dig) {
        ecal.insert(pair <UInt_t, BmnECALDigit> (evId, dig));
    }
    
    void SetEventHeader(UInt_t evId, BmnEventHeader head) {
        header[evId] = head;
    }
    
    // Getters 
    multimap <UInt_t, BmnGemStripDigit> GetGemDigi() {
        return gem;
    }
    
    multimap <UInt_t, BmnSiliconDigit> GetSiliconDigi() {
        return silicon;
    }
    
    multimap <UInt_t, BmnZDCDigit> GetZdcDigi() {
        return zdc;
    }
    
    multimap <UInt_t, BmnTrigDigit> GetT0Digi() {
        return t0;
    }
    
    multimap <UInt_t, BmnTrigDigit> GetBc1Digi() {
        return bc1;
    }
    
    multimap <UInt_t, BmnTrigDigit> GetBc2Digi() {
        return bc2;
    }
    
    multimap <UInt_t, BmnTrigDigit> GetVetoDigi() {
        return veto;
    }
    
    multimap <UInt_t, BmnTrigDigit> GetFdDigi() {
        return fd;
    }
    
    multimap <UInt_t, BmnTrigDigit> GetBdDigi() {
        return bd;
    }
    
    multimap <UInt_t, BmnMwpcDigit> GetMwpcDigi() {
        return mwpc;
    }
    
    multimap <UInt_t, BmnDchDigit> GetDchDigi() {
        return dch;
    }
    
    multimap <UInt_t, BmnTof1Digit> GetTof400Digi() {
        return tof400;
    }
    
    multimap <UInt_t, BmnTof2Digit> GetTof700Digi() {
        return tof700;
    }
    
    multimap <UInt_t, BmnECALDigit> GetEcalDigi() {
        return ecal;
    }
    
    map <UInt_t, BmnEventHeader> GetEventHeader() {
        return header;
    }

private:
    // eventId --> detector digi
    multimap <UInt_t, BmnGemStripDigit> gem;
    multimap <UInt_t, BmnSiliconDigit> silicon;
    multimap <UInt_t, BmnZDCDigit> zdc;
    multimap <UInt_t, BmnTrigDigit> t0;
    multimap <UInt_t, BmnTrigDigit> bc1;
    multimap <UInt_t, BmnTrigDigit> bc2;
    multimap <UInt_t, BmnTrigDigit> veto;
    multimap <UInt_t, BmnTrigDigit> fd;
    multimap <UInt_t, BmnTrigDigit> bd;
    multimap <UInt_t, BmnMwpcDigit> mwpc;
    multimap <UInt_t, BmnDchDigit> dch;
    multimap <UInt_t, BmnTof1Digit> tof400;
    multimap <UInt_t, BmnTof2Digit> tof700;
    multimap <UInt_t, BmnECALDigit> ecal;
    
    map <UInt_t, BmnEventHeader> header;

    ClassDef(BmnDigiContainer, 1);

};

#endif
