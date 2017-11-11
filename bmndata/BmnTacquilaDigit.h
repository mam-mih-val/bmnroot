#ifndef BMNTACQUILADIGIT_H
#define BMNTACQUILADIGIT_H

#include "TObject.h"

class BmnTacquilaDigit: public TObject {
  public:
    BmnTacquilaDigit();
    BmnTacquilaDigit(UInt_t, UInt_t, UInt_t, UInt_t, UInt_t, UInt_t, UInt_t);
    virtual ~BmnTacquilaDigit();

    UInt_t GetGtb() const;
    UInt_t GetModule() const;
    UInt_t GetChannel() const;
    UInt_t GetTdc() const;
    UInt_t GetClock() const;
    UInt_t GetQdc() const;
    Float_t GetTCal() const;
    Float_t GetTime() const;

    void SetTime(BmnTacquilaDigit const &);
    void SetTCal(Float_t);

  private:
    UInt_t fSam;
    UInt_t fGtb;
    UInt_t fModule;
    UInt_t fChannel;
    UInt_t fTdc;
    UInt_t fClock;
    UInt_t fQdc;
    Float_t fTCal;
    Float_t fTime;

    ClassDef(BmnTacquilaDigit, 1);
};

#endif /* BMNTACQUILADIGIT_H */
