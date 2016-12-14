#ifndef BMNTOF2DIGIT_H
#define	BMNTOF2DIGIT_H 2

#include "TNamed.h"

class BmnTof2Digit : public TNamed
{
  public:
    /** Default constructor **/
    BmnTof2Digit();

    /** Main constructor **/
    BmnTof2Digit(Short_t plane, Short_t strip, Float_t t, Float_t a, Float_t d);

    /** Destructor **/
    virtual ~BmnTof2Digit();

    Short_t GetPlane()     const { return fPlane;     }
    Short_t GetStrip()     const { return fStrip;     }
    Float_t GetAmplitude() const { return fAmplitude; }
    Float_t GetTime()      const { return fTime;      }
    Float_t GetDiff()      const { return fDiff;      }

  protected:

    void SetPlane    (Short_t plane) { fPlane = plane;      }
    void SetStrip    (Short_t strip) { fStrip = strip;      }
    void SetAmplitude(Float_t signal){ fAmplitude = signal; }
    void SetTime     (Float_t signal){ fTime = signal; }
    void SetDiff     (Float_t signal){ fDiff = signal; }

  private:

    Short_t fPlane;
    Short_t fStrip;
    Float_t fAmplitude;
    Float_t fTime;
    Float_t fDiff;

    ClassDef(BmnTof2Digit, 2);
};

#endif	/* BMNTOF2DIGIT_H */
