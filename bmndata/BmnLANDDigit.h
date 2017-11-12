#ifndef BMNLANDDIGIT_H
#define	BMNLANDDIGIT_H

#include "TNamed.h"

class BmnTacquilaDigit;

class BmnLANDDigit: public TNamed
{
  public:
    BmnLANDDigit();
    BmnLANDDigit(UChar_t, UChar_t, BmnTacquilaDigit const &, BmnTacquilaDigit
	const &, Float_t, Float_t, Float_t, Float_t, Float_t);
    virtual ~BmnLANDDigit();

    Float_t GetBarPosition() const;
    Bool_t IsVertical() const;

    UChar_t GetPlane() const;
    UChar_t GetBar() const;
    UChar_t GetGlobBar() const;
    Float_t GetPosition() const;
    Float_t GetX() const;
    Float_t GetY() const;
    UShort_t GetTdc(Char_t) const;
    UShort_t GetQdc(Char_t) const;
    Float_t GetTDiff(Char_t) const;
    Float_t GetTime(Char_t = -1) const;
    Float_t GetEnergy(Char_t = -1) const;

private:
    /* 0..4 = LAND, 5 = VETO. */
    UChar_t fPlane;
    /* 0..19. */
    UChar_t fBar;
    /* GlobBar goes from 0..119 for LAND + VETO. */
    UShort_t fGlobBar;
    /* Raw TDC from electronics. */
    UShort_t fTdc0, fTdc1;
    /* Raw QDC from electronics. */
    UShort_t fQdc0, fQdc1;
    /* tcal + clock - cal(17). */
    Float_t fTDiff0, fTDiff1;
    /* Calibrated time. */
    Float_t fTime0, fTime1, fTime;
    /* Calibrated energy. */
    Float_t fEnergy0, fEnergy1, fEnergy;
    /* time1 - time0. */
    Float_t fPosition;
    /* Object space coordinates. */
    Float_t fX;
    Float_t fY;

    ClassDef(BmnLANDDigit, 1);
};

#endif /* BMNLANDDIGIT_H */
