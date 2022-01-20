/* 
 * File:   BmnCaloDigit.h
 * Author: Konstantin Gertsenberger
 * Description: base class for calorimeter digits
 *
 * Created on September 27, 2021
 */

#ifndef BMNCALODIGIT_H
#define	BMNCALODIGIT_H

#include "TNamed.h"

class BmnCaloDigit : public TNamed
{
  public:
    /** Default constructor **/
    BmnCaloDigit();

    /** Main constructor **/
    BmnCaloDigit(Float_t x, Float_t y, UShort_t ch, Float_t amp);

    /** Destructor **/
    virtual ~BmnCaloDigit() {}

    void SetX(Float_t x)         { fX = x; }
    void SetY(Float_t y)         { fY = y; }
    void SetChannel(UShort_t ch) { fChannel = ch; }
    void SetAmp(Float_t amp)     { fAmp = amp; }

    Float_t GetX() const        { return fX; }
    Float_t GetY() const        { return fY; }
    UShort_t GetChannel() const { return fChannel; }
    Float_t GetAmp() const      { return fAmp; }

  protected:
    Float_t   fX;
    Float_t   fY;
    UShort_t  fChannel;
    Float_t   fAmp;

  ClassDef(BmnCaloDigit, 1);
};

#endif	/* BMNCALODIGIT_H */

