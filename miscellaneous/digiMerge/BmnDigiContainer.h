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
#include <BmnTrigWaveDigit.h>
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
            vector <BmnTrigDigit> _bc1, vector <BmnTrigDigit> _bc2, vector <BmnTrigDigit> _bc3, vector <BmnTrigDigit> _bc4,
            vector <BmnTrigDigit> _vc, vector <BmnTrigDigit> _si, vector <BmnTrigDigit> _bd, vector <BmnTrigDigit> _t0, vector <BmnTrigDigit> _fd,
            vector <BmnTrigDigit> _x1l, vector <BmnTrigDigit> _x2l, vector <BmnTrigDigit> _y1l, vector <BmnTrigDigit> _y2l,
            vector <BmnTrigDigit> _x1r, vector <BmnTrigDigit> _x2r, vector <BmnTrigDigit> _y1r, vector <BmnTrigDigit> _y2r,
            vector <BmnTrigWaveDigit> _tqdc_bc1, vector <BmnTrigWaveDigit> _tqdc_bc2, vector <BmnTrigWaveDigit> _tqdc_bc3, vector <BmnTrigWaveDigit> _tqdc_bc4,
            vector <BmnTrigWaveDigit> _tqdc_vc,
            vector <BmnTrigWaveDigit> _tqdc_x1l, vector <BmnTrigWaveDigit> _tqdc_x2l, vector <BmnTrigWaveDigit> _tqdc_y1l, vector <BmnTrigWaveDigit> _tqdc_y2l,
            vector <BmnTrigWaveDigit> _tqdc_x1r, vector <BmnTrigWaveDigit> _tqdc_x2r, vector <BmnTrigWaveDigit> _tqdc_y1r, vector <BmnTrigWaveDigit> _tqdc_y2r,
            vector <BmnZDCDigit> _zdc,
            vector <BmnGemStripDigit> _gem,
            vector <BmnCSCDigit> _csc,
            vector <BmnSiliconDigit> _silicon,
            vector <BmnMwpcDigit> _mwpc,
            vector <BmnDchDigit> _dch,
            vector <BmnTof1Digit> _tof400,
            vector <BmnTof2Digit> _tof700,
            vector <BmnECALDigit> _ecal) {
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
        else if (detName.Contains("BC1") && !detName.Contains("TQDC"))
            bc1[evId] = _bc1;
        else if (detName.Contains("BC2") && !detName.Contains("TQDC"))
            bc2[evId] = _bc2;
        else if (detName.Contains("BC3") && !detName.Contains("TQDC"))
            bc3[evId] = _bc3;
        else if (detName.Contains("BC4") && !detName.Contains("TQDC"))
            bc4[evId] = _bc4;
        else if ((detName.Contains("VC") || detName.Contains("VETO")) && !detName.Contains("TQDC"))
            veto[evId] = _vc;
        else if (detName.Contains("TQDC_VETO"))
            tqdc_veto[evId] = _tqdc_vc;
        else if (detName.Contains("Si") || detName.Contains("SI"))
            si[evId] = _si;
        else if (detName.Contains("BD"))
            bd[evId] = _bd;
        else if (detName.Contains("ZDC"))
            zdc[evId] = _zdc;
        else if (detName.Contains("T0"))
            t0[evId] = _t0;
        else if (detName.Contains("FD"))
            fd[evId] = _fd;
        else if (detName.Contains("X1L") && !detName.Contains("TQDC"))
            x1l[evId] = _x1l;
        else if (detName.Contains("X2L") && !detName.Contains("TQDC"))
            x2l[evId] = _x2l;
        else if (detName.Contains("Y1L") && !detName.Contains("TQDC"))
            y1l[evId] = _y1l;
        else if (detName.Contains("Y2L") && !detName.Contains("TQDC"))
            y2l[evId] = _y2l;
        else if (detName.Contains("X1R") && !detName.Contains("TQDC") )
            x1r[evId] = _x1r;
        else if (detName.Contains("X2R") && !detName.Contains("TQDC"))
            x2r[evId] = _x2r;
        else if (detName.Contains("Y1R") && !detName.Contains("TQDC"))
            y1r[evId] = _y1r;
        else if (detName.Contains("Y2R") && !detName.Contains("TQDC"))
            y2r[evId] = _y2r;
        else if (detName.Contains("TQDC_X1L"))
            tqdc_x1l[evId] = _tqdc_x1l;
        else if (detName.Contains("TQDC_X2L"))
            tqdc_x2l[evId] = _tqdc_x2l;
        else if (detName.Contains("TQDC_Y1L"))
            tqdc_y1l[evId] = _tqdc_y1l;
        else if (detName.Contains("TQDC_Y2L"))
            tqdc_y2l[evId] = _tqdc_y2l;
        else if (detName.Contains("TQDC_X1R"))
            tqdc_x1r[evId] = _tqdc_x1r;
        else if (detName.Contains("TQDC_X2R"))
            tqdc_x2r[evId] = _tqdc_x2r;
        else if (detName.Contains("TQDC_Y1R"))
            tqdc_y1r[evId] = _tqdc_y1r;
        else if (detName.Contains("TQDC_Y2R"))
            tqdc_y2r[evId] = _tqdc_y2r;
        else if (detName.Contains("TQDC_BC1"))
            tqdc_bc1[evId] = _tqdc_bc1;
        else if (detName.Contains("TQDC_BC2"))
            tqdc_bc2[evId] = _tqdc_bc2;
        else if (detName.Contains("TQDC_BC3"))
            tqdc_bc3[evId] = _tqdc_bc3;
        else if (detName.Contains("TQDC_BC4"))
            tqdc_bc4[evId] = _tqdc_bc4;
        else {
            cout << "::SetDigi()" << endl;
            throw;
        }
    }

    void SetEventHeadersPerEachDetector(TString detName, UInt_t evId, BmnEventHeader header) {
        headerMap[pair <UInt_t, TString> (evId, detName)] = header;
    }

    // Getters

    // Det. digis

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

    // Trig. digis

    map <UInt_t, vector <BmnTrigDigit>> GetTrigDigi(TString det = "") {
        if (det.IsNull()) {
            cout << "Trigger not found!" << endl;
            throw;
        } else if (det.Contains("BC1"))
            return bc1;
        else if (det.Contains("BC2"))
            return bc2;
        else if (det.Contains("BC3"))
            return bc3;
        else if (det.Contains("BC4"))
            return bc4;
        else if (det.Contains("BC4"))
            return bc4;
        else if (det.Contains("VETO"))
            return veto;
        else if (det.Contains("BD"))
            return bd;
        else if (det.Contains("SI"))
            return si;
        else if (det.Contains("T0"))
            return t0;
        else if (det.Contains("FD"))
            return fd;
        else if (det.Contains("X1L"))
            return x1l;
        else if (det.Contains("X2L"))
            return x2l;
        else if (det.Contains("Y1L"))
            return y1l;
        else if (det.Contains("Y2L"))
            return y2l;
        else if (det.Contains("X1R"))
            return x1r;
        else if (det.Contains("X2R"))
            return x2r;
        else if (det.Contains("Y1R"))
            return y1r;
        else if (det.Contains("Y2R"))
            return y2r;
        else {
            cout << "Trigger not defined in a proper way!" << endl;
            throw;
        }
    }

    map <UInt_t, vector <BmnTrigWaveDigit>> GetTqdcTrigDigi(TString det = "") {
        if (det.IsNull()) {
            cout << "TQDC Trigger not found!" << endl;
            throw;
        } else if (det.Contains("BC1"))
            return tqdc_bc1;
        else if (det.Contains("BC2"))
            return tqdc_bc2;
        else if (det.Contains("BC3"))
            return tqdc_bc3;
        else if (det.Contains("BC4"))
            return tqdc_bc4;
        else if (det.Contains("X1L")) 
            return tqdc_x1l;
        else if (det.Contains("X2L"))
            return tqdc_x2l;
        else if (det.Contains("Y1L"))
            return tqdc_y1l;
        else if (det.Contains("Y2L"))
            return tqdc_y2l;
        else if (det.Contains("X1R"))
            return tqdc_x1r;
        else if (det.Contains("X2R"))
            return tqdc_x2r;
        else if (det.Contains("Y1R"))
            return tqdc_y1r;
        else if (det.Contains("Y2R"))
            return tqdc_y2r;
        else if (det.Contains("VETO"))
            return tqdc_veto;
        else {
            cout << "TQDC Trigger not defined in a proper way!" << endl;
            throw;
        }

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
    map <UInt_t, vector <BmnTrigDigit>> bc4;
    map <UInt_t, vector <BmnTrigDigit>> veto;

    map <UInt_t, vector <BmnTrigDigit>> fd;
    map <UInt_t, vector <BmnTrigDigit>> bd;
    map <UInt_t, vector <BmnTrigDigit>> si;
    map <UInt_t, vector <BmnMwpcDigit>> mwpc;
    map <UInt_t, vector <BmnDchDigit>> dch;
    map <UInt_t, vector <BmnTof1Digit>> tof400;
    map <UInt_t, vector <BmnTof2Digit>> tof700;
    map <UInt_t, vector <BmnECALDigit>> ecal;

    map <UInt_t, vector <BmnTrigDigit>> x1l;
    map <UInt_t, vector <BmnTrigDigit>> x2l;
    map <UInt_t, vector <BmnTrigDigit>> y1l;
    map <UInt_t, vector <BmnTrigDigit>> y2l;
    map <UInt_t, vector <BmnTrigDigit>> x1r;
    map <UInt_t, vector <BmnTrigDigit>> x2r;
    map <UInt_t, vector <BmnTrigDigit>> y1r;
    map <UInt_t, vector <BmnTrigDigit>> y2r;

    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_veto;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_bc1;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_bc2;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_bc3;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_bc4;

    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_x1l;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_x2l;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_y1l;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_y2l;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_x1r;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_x2r;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_y1r;
    map <UInt_t, vector <BmnTrigWaveDigit>> tqdc_y2r;

    map <pair <UInt_t, TString>, BmnEventHeader> headerMap;

    ClassDef(BmnDigiContainer, 1);

};

#endif
