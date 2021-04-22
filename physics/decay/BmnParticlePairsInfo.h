// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-06-20
// Author: Alexander Lytaev <sas-lyt@ya.ru> 2018-08-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnParticlePairsInfo                                                       //
//                                                                            //
//  A class to contain an overall information about a set of particle pairs   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNPARTICLEPAIRSINFO_H
#define BMNPARTICLEPAIRSINFO_H 1

#include <TNamed.h>
#include <TVector3.h>
#include <iostream>

using namespace std;

class BmnParticlePairsInfo : public TObject{
public:

    /** Default constructor **/
    BmnParticlePairsInfo();

    /** Destructor **/
    virtual ~BmnParticlePairsInfo();

    Double_t getMomPart1Min();
    Double_t getMomPart1Max();
    Double_t getMomPart2Min();
    Double_t getMomPart2Max();

    Double_t getEtaPart1Min();
    Double_t getEtaPart1Max();
    Double_t getEtaPart2Min();
    Double_t getEtaPart2Max();
    
    Double_t getDCAPart1Min();
    Double_t getDCAPart1Max();
    Double_t getDCAPart2Min();
    Double_t getDCAPart2Max();
    
    Double_t getDCA12Min();
    Double_t getDCA12Max();
    Double_t getPathMin();
    Double_t getPathMax();
    
    void setMomPart1Min(Double_t val);
    void setMomPart1Max(Double_t val);
    void setMomPart2Min(Double_t val);
    void setMomPart2Max(Double_t val);

    void setEtaPart1Min(Double_t val);
    void setEtaPart1Max(Double_t val);
    void setEtaPart2Min(Double_t val);
    void setEtaPart2Max(Double_t val);
    
    void setDCAPart1Min(Double_t val);
    void setDCAPart1Max(Double_t val);
    void setDCAPart2Min(Double_t val);
    void setDCAPart2Max(Double_t val);
    
    void setDCA12Min(Double_t val);
    void setDCA12Max(Double_t val);
    void setPathMin(Double_t val);
    void setPathMax(Double_t val);
    
private:
    
    // Kin. cuts info
    
    Double_t fMomPart1Min;
    Double_t fMomPart1Max;
    Double_t fMomPart2Min;
    Double_t fMomPart2Max;
    
    Double_t fEtaPart1Min;
    Double_t fEtaPart1Max;
    Double_t fEtaPart2Min;
    Double_t fEtaPart2Max;
    
    // Geom. cuts are applied
    
    Double_t fDCAPart1Min;
    Double_t fDCAPart1Max;
    Double_t fDCAPart2Min;
    Double_t fDCAPart2Max;
    Double_t fDCA12Min;
    Double_t fDCA12Max;
    Double_t fPathMin;
    Double_t fPathMax;
    
    ClassDef(BmnParticlePairsInfo, 1);

};

#endif