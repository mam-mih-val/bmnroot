/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "BmnTOF1Conteiner.h"

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1Conteiner::BmnTOF1Conteiner()
{
    Clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1Conteiner::BmnTOF1Conteiner(Short_t plane, Short_t strip, Float_t timeL, Float_t timeR, Float_t time, Float_t widthL, Float_t widthR, Float_t width, Float_t x, Float_t y, Float_t z, Float_t timeT0, Float_t widthT0)
{
    fPlane = plane;
    fStrip = strip;
    fTimeL = timeL;
    fTimeR = timeR;
    fTime = time;
    fWidthL = widthL;
    fWidthR = widthR;
    fWidth = width;
    fX = x;
    fY = y;
    fZ = z;
    fTimeT0 = timeT0;
    fWidthT0 = widthT0;
}

void BmnTOF1Conteiner::SetParameters (Short_t plane, Short_t strip, Float_t timeL, Float_t timeR, Float_t time, Float_t widthL, Float_t widthR, Float_t width, Float_t x, Float_t y, Float_t z, Float_t timeT0, Float_t widthT0)
{
    fPlane = plane;
    fStrip = strip;
    fTimeL = timeL;
    fTimeR = timeR;
    fTime = time;
    fWidthL = widthL;
    fWidthR = widthR;
    fWidth = width;
    fX = x;
    fY = y;
    fZ = z;
    fTimeT0 = timeT0;
    fWidthT0 = widthT0;    
}
//--------------------------------------------------------------------------------------------------------------------------------------
void	BmnTOF1Conteiner::Clear()
{
    fPlane = -1;
    fStrip = -1;
    fTimeL = -1;
    fTimeR = -1;
    fTime = -1;
    fWidthL = -1;
    fWidthR = -1;
    fWidth = -1;
    fX = -1000;
    fY = -1000;
    fZ = -1000;
    fTimeT0 = -1;
    fWidthT0 = -1;
}

BmnTOF1Conteiner::~BmnTOF1Conteiner(){
    
}

ClassImp(BmnTOF1Conteiner)
        