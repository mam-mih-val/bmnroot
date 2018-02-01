#ifndef BMNSILICONALIGNCORRECTIONS_H
#define BMNSILICONALIGNCORRECTIONS_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>
#include "BmnAlignCorrections.h"

using namespace std;

class BmnSiliconAlignCorrections : public BmnAlignCorrections {
public:

    BmnSiliconAlignCorrections() {
    };

    BmnSiliconAlignCorrections(TVector3);
    
    virtual ~BmnSiliconAlignCorrections() { };
  
    ClassDef(BmnSiliconAlignCorrections, 1)
};

#endif
