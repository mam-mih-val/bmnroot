#ifndef BMNTOF1DIGIT_H
#define	BMNTOF1DIGIT_H

#include "TNamed.h"

class BmnTof1Digit : public TNamed
{
  public:
    /** Default constructor **/
    BmnTof1Digit();

    /** Main constructor **/
    BmnTof1Digit(Short_t plane, Short_t strip, Short_t side,Float_t t,Float_t a);

    Short_t GetPlane()     const { return fPlane;     }
    Short_t GetStrip()     const { return fStrip;     }
    Short_t GetSide()      const { return fSide;      }
    Float_t GetAmplitude() const { return fAmplitude; }
    Float_t GetTime()      const { return fTime;      }
    void SetPlane    (Short_t plane) { fPlane = plane;      }
    void SetStrip    (Short_t strip) { fStrip = strip;      }
    void SetSide     (Short_t side)  { fSide = side;        }
    void SetAmplitude(Float_t signal){ fAmplitude = signal; }
    void SetTime     (Float_t signal){ fAmplitude = signal; }

    /** Destructor **/
    virtual ~BmnTof1Digit();

private:
    Short_t fPlane;
    Short_t fStrip;
    Short_t fSide;
    Float_t fAmplitude;
    Float_t fTime;

    ClassDef(BmnTof1Digit, 1);
};

#endif	/* BMNTOF1DIGIT_H */
