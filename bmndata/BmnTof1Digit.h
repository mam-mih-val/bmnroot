#ifndef BMNTOF1DIGIT_H
#define	BMNTOF1DIGIT_H

#include "TNamed.h"

class BmnTof1Digit : public TNamed
{
  public:
    /** Default constructor **/
    BmnTof1Digit();

    /** Main constructor **/
    BmnTof1Digit(Short_t plane, Float_t signal);

    Short_t GetPlane() const { return fPlane; }
    Float_t GetSignal() const { return fSignal; }

    void SetPlane(Short_t plane) { fPlane = plane; }
    void SetSignal(Float_t signal) { fSignal = signal; }

    /** Destructor **/
    virtual ~BmnTof1Digit();

private:
    Short_t fPlane;
    Float_t fSignal;

    ClassDef(BmnTof1Digit, 1);
};

#endif	/* BMNTOF1DIGIT_H */
