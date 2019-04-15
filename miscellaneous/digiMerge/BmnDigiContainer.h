#include <TNamed.h>
#include <iostream>
#include <map>
#include <TClonesArray.h>

#include <BmnEventHeader.h>
#include <BmnGemStripDigit.h>
#include <BmnCSCDigit.h>
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

    void SetDigi(TString detName, UInt_t evId,
            vector <BmnGemStripDigit> _gem,
            vector <BmnCSCDigit> _csc,
            vector <BmnSiliconDigit> _silicon,
            vector <BmnZDCDigit> _zdc,
            vector <BmnTrigDigit> _bc1,
            vector <BmnTrigDigit> _bc2,
            vector <BmnTrigDigit> _bc3,
            vector <BmnTrigDigit> _veto,
            vector <BmnTrigDigit> _bd,
            vector <BmnTrigDigit> _si,
            vector <BmnMwpcDigit> _mwpc,
            vector <BmnDchDigit> _dch,
            vector <BmnTof1Digit> _tof400,
            vector <BmnTof2Digit> _tof700,
            vector <BmnECALDigit> _ecal, 
            vector <BmnTrigDigit> _t0,  
            vector <BmnTrigDigit> _fd) {
        if (detName.Contains("GEM")) 
            gem[evId] = _gem;
        else if (detName.Contains("CSC"))
            csc[evId] = _csc;
        else if (detName.Contains("SILICON"))
            silicon[evId] = _silicon;
        else if (detName.Contains("MWPC"))
            mwpc[evId] = _mwpc;
        else if (detName.Contains("DCH"))
            dch[evId] = _dch;
        else if (detName.Contains("TOF400"))
            tof400[evId] = _tof400;
        else if (detName.Contains("TOF700"))
            tof700[evId] = _tof700;
        else if (detName.Contains("ECAL"))
            ecal[evId] = _ecal;
        else if (detName.Contains("BC1"))
            bc1[evId] = _bc1;
        else if (detName.Contains("BC2"))
            bc2[evId] = _bc2;
        else if (detName.Contains("BC3"))
            bc3[evId] = _bc3;
        else if (detName.Contains("VC") || detName.Contains("VETO"))
            veto[evId] = _veto;
        else if (detName.Contains("Si"))
            si[evId] = _si;
        else if (detName.Contains("BD"))
            bd[evId] = _bd;
        else if (detName.Contains("ZDC"))
            zdc[evId] = _zdc;
        else if (detName.Contains("T0"))
            t0[evId] = _t0;
         else if (detName.Contains("FD"))
            fd[evId] = _fd;
        else
            throw;
    }

    void SetEventHeadersPerEachDetector(TString detName, UInt_t evId, BmnEventHeader header) {
        headerMap[pair <UInt_t, TString> (evId, detName)] = header;
    }

    // Getters

    map <UInt_t, vector <BmnGemStripDigit>> GetGemDigi() {
        return gem;
    }
    
    map <UInt_t, vector <BmnCSCDigit>> GetCscDigi() {
        return csc;
    }

    map <UInt_t, vector <BmnSiliconDigit>> GetSiliconDigi() {
        return silicon;
    }

    map <UInt_t, vector <BmnZDCDigit>> GetZdcDigi() {
        return zdc;
    }

    map <UInt_t, vector <BmnTrigDigit>> GetBc1Digi() {
        return bc1;
    }

    map <UInt_t, vector <BmnTrigDigit>> GetBc2Digi() {
        return bc2;
    }

    map <UInt_t, vector <BmnTrigDigit>> GetBc3Digi() {
        return bc3;
    }

    map <UInt_t, vector <BmnTrigDigit>> GetVetoDigi() {
        return veto;
    }

    map <UInt_t, vector <BmnTrigDigit>> GetBdDigi() {
        return bd;
    }

    map <UInt_t, vector <BmnTrigDigit>> GetSiDigi() {
        return si;
    }

    map <UInt_t, vector <BmnMwpcDigit>> GetMwpcDigi() {
        return mwpc;
    }

    map <UInt_t, vector <BmnDchDigit>> GetDchDigi() {
        return dch;
    }

    map <UInt_t, vector <BmnTof1Digit>> GetTof400Digi() {
        return tof400;
    }

    map <UInt_t, vector <BmnTof2Digit>> GetTof700Digi() {
        return tof700;
    }

    map <UInt_t, vector <BmnECALDigit>> GetEcalDigi() {
        return ecal;
    }
    
    map <UInt_t, vector <BmnTrigDigit>> GetT0Digi() {
        return t0;
    }
    
    map <UInt_t, vector <BmnTrigDigit>> GetFdDigi() {
        return fd;
    }
    
    map <pair <UInt_t, TString>, BmnEventHeader> GetEventHeaderMap() {
        return headerMap;
    }

private:
    // eventId --> detector digis
    map <UInt_t, vector <BmnGemStripDigit>> gem;
    map <UInt_t, vector <BmnCSCDigit>> csc;
    map <UInt_t, vector <BmnSiliconDigit>> silicon;
    map <UInt_t, vector <BmnZDCDigit>> zdc;
    map <UInt_t, vector <BmnTrigDigit>> t0;
    map <UInt_t, vector <BmnTrigDigit>> bc1;
    map <UInt_t, vector <BmnTrigDigit>> bc2;
    map <UInt_t, vector <BmnTrigDigit>> bc3;
    map <UInt_t, vector <BmnTrigDigit>> veto;
    map <UInt_t, vector <BmnTrigDigit>> fd;
    map <UInt_t, vector <BmnTrigDigit>> bd;
    map <UInt_t, vector <BmnTrigDigit>> si;
    map <UInt_t, vector <BmnMwpcDigit>> mwpc;
    map <UInt_t, vector <BmnDchDigit>> dch;
    map <UInt_t, vector <BmnTof1Digit>> tof400;
    map <UInt_t, vector <BmnTof2Digit>> tof700;
    map <UInt_t, vector <BmnECALDigit>> ecal;
    
    map <pair <UInt_t, TString>, BmnEventHeader> headerMap;

    ClassDef(BmnDigiContainer, 1);

};

#endif
