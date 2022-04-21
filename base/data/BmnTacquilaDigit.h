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
    Float_t GetTDiff() const;

    void SetTCal(Float_t);
    void SetTDiff(BmnTacquilaDigit const &);

  private:
    /* 3 for Tof-Cal, 5 for LAND at JINR. */
    UInt_t fSam;
    /* Two Tacquila crates, so 0 or 1. */
    UInt_t fGtb;
    /* 0..9. */
    UInt_t fModule;
    /* 0..15 = normal channel, 16 = common stop. */
    UInt_t fChannel;
    /* Raw TDC. */
    UInt_t fTdc;
    /* Long range TDC clock. */
    UInt_t fClock;
    /* RAW QDC. */
    UInt_t fQdc;
    /* Calibrated TDC fine-time. */
    Float_t fTCal;
    /* tcal + clock * 25 ns - tcal(ch=17). */
    Float_t fTDiff;

    ClassDef(BmnTacquilaDigit, 1);
};

#endif /* BMNTACQUILADIGIT_H */
