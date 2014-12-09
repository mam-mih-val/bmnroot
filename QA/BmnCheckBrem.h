#ifndef CBMLITCHECKBREM_H_
#define CBMLITCHECKBREM_H_

#include "TObject.h"
#include <string>
#include <vector>
class TGraph;

class CbmLitSimpleMaterial
{
public:
   CbmLitSimpleMaterial() : fZ(0.), fA(0.), fRho(0.), fX0(0.), fPHIRAD() {;}
   Double_t fZ;
   Double_t fA;
   Double_t fRho;
   Double_t fX0;
   std::vector<Double_t> fPHIRAD;
};

class CbmLitCheckBrem : public TObject
{
public:
   CbmLitCheckBrem();
   virtual ~CbmLitCheckBrem();

   virtual void Check();

   void CreateGraphs();
   void FillGraphs();
   void DrawGraphs();

   void SetMaterial(const std::string& material) {
      fMat = material;
   }

private:
   Int_t fNofMom;
   std::vector<double> fMom;
   Int_t fNofMaterials;
   std::vector<TGraph*> fTable;
   std::vector<TGraph*> fCalc;
   std::vector<CbmLitSimpleMaterial> fMaterials;
   std::string fMat;

   ClassDef(CbmLitCheckBrem, 1);
};
#endif /* CBMLITCHECKBREM_H_ */
