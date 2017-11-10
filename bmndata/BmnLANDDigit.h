#ifndef BMNLANDDIGIT_H
#define	BMNLANDDIGIT_H

#include "TNamed.h"

class BmnTacquilaDigit;

class BmnLANDDigit: public TNamed
{
  public:
    BmnLANDDigit();
    BmnLANDDigit(UChar_t, UChar_t, BmnTacquilaDigit const &, BmnTacquilaDigit
	const &);
    virtual ~BmnLANDDigit();

    Float_t GetBarPosition() const;
    Bool_t IsVertical() const;

    UChar_t GetPlane() const;
    UChar_t GetBar() const;
    Float_t GetPosition() const;
    Float_t GetX() const;
    Float_t GetY() const;
    UChar_t GetTdc(UChar_t) const;
    UChar_t GetQdc(UChar_t) const;
    UChar_t GetTime(UChar_t) const;

private:
    /* 0..4 = LAND, 5 = VETO. */
    UChar_t fPlane;
    UChar_t fBar;
    UShort_t fTdc[2];
    UShort_t fQdc[2];
    Float_t fTime[2];
    Float_t fPosition;

    ClassDef(BmnLANDDigit, 1);
};

#endif /* BMNLANDDIGIT_H */
