#ifndef BMNDCHDIGIT_H
#define	BMNDCHDIGIT_H

#include "TNamed.h"

class BmnDchDigit : public TNamed
{
  public:
    /** Default constructor **/
    BmnDchDigit();

    /** Main constructor **/
    BmnDchDigit(Short_t plane, Float_t signal);

    Short_t GetPlane() const { return fPlane; }
    Float_t GetSignal() const { return fSignal; }

    void SetPlane(Short_t plane) { fPlane = plane; }
    void SetSignal(Float_t signal) { fSignal = signal; }

    /** Destructor **/
    virtual ~BmnDchDigit();

private:
    Short_t fPlane;
    Float_t fSignal;

    ClassDef(BmnDchDigit, 1);
};

#endif	/* BMNDCHDIGIT_H */
