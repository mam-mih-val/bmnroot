#ifndef CBMLITCHECKENERGYLOSSMUONS_H_
#define CBMLITCHECKENERGYLOSSMUONS_H_

#include "FairTask.h"

#include <string>
using std::string;

class TGraph;

class CbmLitCheckEnergyLossMuons : public FairTask
{
public:
   CbmLitCheckEnergyLossMuons();
   virtual ~CbmLitCheckEnergyLossMuons();

   virtual void Check();

   void FillTableIron();
   void FillTableTungsten();
   void FillTableCarbon();
   void CreateGraphs();
   void CalcEloss();
   void DrawGraphs();

   void SetMaterial(const std::string& material) {
      fMat = material;
   }

private:
   Double_t fMom[26];
   TGraph* fTable[4];
   TGraph* fCalc[4];
   std::string fMat;

   string fOutputDir;

   CbmLitCheckEnergyLossMuons(const CbmLitCheckEnergyLossMuons&);
   CbmLitCheckEnergyLossMuons& operator=(const CbmLitCheckEnergyLossMuons&);
   
   ClassDef(CbmLitCheckEnergyLossMuons, 1);
};

#endif /* CBMLITCHECKENERGYLOSSMUONS_H_ */
