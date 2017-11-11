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
    UChar_t GetGlobBar() const;
    Float_t GetPosition() const;
    Float_t GetX() const;
    Float_t GetY() const;
    UShort_t GetTdc(UChar_t) const;
    UShort_t GetQdc(UChar_t) const;
    Float_t GetTCal(UChar_t) const;
    Float_t GetTime(UChar_t) const;

private:
    /* 0..4 = LAND, 5 = VETO. */
    UChar_t fPlane;
    UChar_t fBar;
    /* GlobBar goes from 0..119 for LAND + VETO. */
    UShort_t fGlobBar;
    UShort_t fTdc0, fTdc1;
    UShort_t fQdc0, fQdc1;
    Float_t fTCal0, fTCal1;
    Float_t fTime0, fTime1;
    Float_t fPosition;
    Float_t fX;
    Float_t fY;

    ClassDef(BmnLANDDigit, 1);
};

#endif /* BMNLANDDIGIT_H */
