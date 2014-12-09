#ifndef CBMLITRADLENGTHGENERATOR_H_
#define CBMLITRADLENGTHGENERATOR_H_

#include "FairGenerator.h"
#include <vector>

class FairPrimaryGenerator;

using std::vector;

class CbmLitRadLengthGenerator : public FairGenerator
{
public:

   CbmLitRadLengthGenerator();

   void SetXY(
         const vector<Double_t>& x,
         const vector<Double_t>& y) {
      fX = x;
      fY = y;
   }

   Bool_t Init();

   virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

private:
   vector<Double_t> fX; // Vector with X, Y start coordinates for each event
   vector<Double_t> fY;

   ClassDef(CbmLitRadLengthGenerator, 1);
};

#endif
