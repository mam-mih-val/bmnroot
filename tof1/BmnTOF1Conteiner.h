/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnTOF1Conteiner.h
 * Author: mikhailr
 *
 * Created on May 3, 2018, 3:46 PM
 */

#ifndef BMNTOF1CONTEINER_H
#define BMNTOF1CONTEINER_H 

#include "TObject.h"
#include "TNamed.h"
#include <iostream>

using namespace std;
//------------------------------------------------------------------------------------------------------------------------
class BmnTOF1Conteiner : public TNamed {

public:
    BmnTOF1Conteiner();
    BmnTOF1Conteiner(Short_t plane, Short_t strip, Float_t timeL, Float_t timeR, Float_t time, Float_t widthL, Float_t widthR, Float_t width, Float_t x, Float_t y, Float_t z, Float_t timeT0, Float_t widthT0);
    virtual ~BmnTOF1Conteiner();

    Short_t GetPlane()      const { return fPlane; }
    Short_t GetStrip()      const { return fStrip; }
    Float_t GetTimeL()     const { return fTimeL; }
    Float_t GetTimeR()     const { return fTimeR; }
    Float_t GetTime()       const { return fTime; }
    Float_t GetWidthL()     const { return fWidthL; }
    Float_t GetWidthR()     const { return fWidthR; }
    Float_t GetWidth()      const { return fWidth; }
    Float_t GetX()       const { return fX; }
    Float_t GetY()       const { return fY; }
    Float_t GetZ()       const { return fZ; }
    Float_t GetTimeT0()       const { return fTimeT0; }
    Float_t GetWidthT0()     const { return fWidthT0; }
    
    void SetParameters(Short_t plane, Short_t strip, Float_t timeL, Float_t timeR, Float_t time, Float_t widthL, Float_t widthR, Float_t width, Float_t x, Float_t y, Float_t z, Float_t timeT0, Float_t widthT0);
    void SetPlane(Short_t v)      { fPlane = v; }
    void SetStrip(Short_t v)      { fStrip = v; }
    void SetTimeL(Float_t v)      { fTimeL = v; }
    void SetTimeR(Float_t v)      { fTimeR = v; }
    void SetTime(Float_t v)       { fTime = v; }
    void SetWidthL(Float_t v)     { fWidthL = v; }
    void SetWidthR(Float_t v)     { fWidthR = v; }
    void SetWidth(Float_t v)      { fWidth = v; }
    void SetX(Float_t v)          { fX = v; }
    void SetY(Float_t v)          { fY = v; }
    void SetZ(Float_t v)          { fZ = v; }
    void SetTimeT0(Float_t v)     { fTimeT0 = v; }
    void SetWidthT0(Float_t v)    { fWidthT0 = v; }
    
    void Clear();
    
private: 

    Short_t fPlane;
    Short_t fStrip;
    Float_t fTimeL;
    Float_t fTimeR;
    Float_t fTime;
    Float_t fWidthL;
    Float_t fWidthR;
    Float_t fWidth;
    Float_t fX;
    Float_t fY;
    Float_t fZ;
    Float_t fTimeT0;
    Float_t fWidthT0;

    ClassDef(BmnTOF1Conteiner, 2);
};


#endif /* BMNTOF1CONTEINER_H */

