#ifndef BMNECALDIGIT_H
#define	BMNECALDIGIT_H

#include "TNamed.h"

class BmnEcalDigit : public TNamed
{
  public:
    /** Default constructor **/
    BmnEcalDigit();

    /** Main constructor **/
    BmnEcalDigit(Short_t plane, Float_t signal);

    Short_t GetPlane() const { return fPlane; }
    Float_t GetSignal() const { return fSignal; }

    void SetPlane(Short_t plane) { fPlane = plane; }
    void SetSignal(Float_t signal) { fSignal = signal; }

    /** Destructor **/
    virtual ~BmnEcalDigit();

private:
    Short_t fPlane;
    Float_t fSignal;

    ClassDef(BmnEcalDigit, 1);
};

#endif	/* BMNECALDIGIT_H */
