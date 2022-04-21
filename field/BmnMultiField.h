#ifndef BMNMULTIMAP_H
#define BMNMULTIMAP_H 1

#include "FairField.h"
#include "TObjArray.h"
#include <map>

class BmnRegion;
class BmnMultiFieldPar;

class BmnMultiField : public FairField {
public:

    /** Default constructor **/
    BmnMultiField();

    /** Constructor from MpdFieldPar **/
    BmnMultiField(BmnMultiFieldPar* fieldPar);

    /** Destructor **/
    virtual ~BmnMultiField();

    /** Initialisation (read map from file) **/
    void Init();

    /**Adding a field to the collection*/

    void AddField(FairField *field);

    TObjArray *GetFieldList() {
        return fMaps;
    }

    /** Get the field components at a certain point 
     ** @param x,y,z     Point coordinates (global) [cm]
     ** @value Bx,By,Bz  Field components [kG]
     **/

    virtual Double_t GetBx(Double_t x, Double_t y, Double_t z);
    virtual Double_t GetBy(Double_t x, Double_t y, Double_t z);
    virtual Double_t GetBz(Double_t x, Double_t y, Double_t z);

    void GetFieldValue(const Double_t point[3], Double_t* bField);

    /** Screen output **/
    virtual void Print();
    ClassDef(BmnMultiField, 1)

protected:

    TObjArray *fMaps;
    Int_t fNoOfMaps;
    std::map <BmnRegion*, FairField* > fFieldMaps; //!
};

#endif
