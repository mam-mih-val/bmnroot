#ifndef BMNMULTIFIELDPAR_H
#define BMNMULTIFIELDPAR_H 1

#include "BmnFieldPar.h"
#include "BmnMultiField.h"
#include "BmnFieldPar.h"
#include "BmnMapPar.h"

class FairParamList;

class BmnMultiFieldPar : public BmnMapPar {
public:

    /** Standard constructor  **/
    BmnMultiFieldPar(const char* name, const char* title, const char* context);

    /** default constructor  **/
    BmnMultiFieldPar();

    /** Destructor **/
    ~BmnMultiFieldPar();

    void putParams(FairParamList* list);

    /** Get parameters **/
    Bool_t getParams(FairParamList* list);

    /** Set parameters from FairField  **/
    void SetParameters(FairField* field);

    TObjArray *GetParArray() {
        return fParArray;
    }

protected:

    TObjArray *fParArray;

    ClassDef(BmnMultiFieldPar, 1);

};

#endif
