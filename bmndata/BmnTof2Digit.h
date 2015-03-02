#ifndef BMNTOF2DIGIT_H
#define	BMNTOF2DIGIT_H

#include "TNamed.h"

class BmnTof2Digit : public TNamed
{
  public:
    /** Default constructor **/
    BmnTof2Digit();

    /** Main constructor **/
    BmnTof2Digit(Short_t plane, Float_t signal);

    Short_t GetPlane() const { return fPlane; }
    Float_t GetSignal() const { return fSignal; }

    void SetPlane(Short_t plane) { fPlane = plane; }
    void SetSignal(Float_t signal) { fSignal = signal; }

    /** Destructor **/
    virtual ~BmnTof2Digit();

private:
    Short_t fPlane;
    Float_t fSignal;

    ClassDef(BmnTof2Digit, 1);
};

#endif	/* BMNTOF2DIGIT_H */
