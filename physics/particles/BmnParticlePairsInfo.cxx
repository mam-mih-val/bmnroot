// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-06-20
// Author: Alexander Lytaev <sas-lyt@ya.ru> 2018-08-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnParticlePairsInfo                                                       //
//                                                                            //
//  A class to contain an information about a set of particle pairs           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "BmnParticlePairsInfo.h"
#include <iostream>

BmnParticlePairsInfo::BmnParticlePairsInfo() :
fMomPart1Min(0.),
fMomPart2Min(0.),
fMomPart1Max(0.),
fMomPart2Max(0.),
fEtaPart1Min(0.),
fEtaPart1Max(0.),
fEtaPart2Min(0.),
fEtaPart2Max(0.),
fDCAPart1Min(0.),
fDCAPart1Max(0.),
fDCAPart2Min(0.),
fDCAPart2Max(0.),
fDCA12Min(0.),
fDCA12Max(0.),
fPathMin(0.),
fPathMax(0.) {
}

// Setters

void BmnParticlePairsInfo::setMomPart1Min(Double_t val) {
    fMomPart1Min = val;
}

void BmnParticlePairsInfo::setMomPart1Max(Double_t val) {
    fMomPart1Max = val;
}

void BmnParticlePairsInfo::setMomPart2Min(Double_t val) {
    fMomPart2Min = val;
}

void BmnParticlePairsInfo::setMomPart2Max(Double_t val) {
    fMomPart2Max = val;
}

void BmnParticlePairsInfo::setEtaPart1Min(Double_t val) {
    fEtaPart1Min = val;
}

void BmnParticlePairsInfo::setEtaPart1Max(Double_t val) {
    fEtaPart1Max = val;
}

void BmnParticlePairsInfo::setEtaPart2Min(Double_t val) {
    fEtaPart2Min = val;
}

void BmnParticlePairsInfo::setEtaPart2Max(Double_t val) {
    fEtaPart2Max = val;
}

void BmnParticlePairsInfo::setDCAPart1Min(Double_t val) {
    fDCAPart1Min = val;
}

void BmnParticlePairsInfo::setDCAPart1Max(Double_t val) {
    fDCAPart1Max = val;
}

void BmnParticlePairsInfo::setDCAPart2Min(Double_t val) {
    fDCAPart2Min = val;
}

void BmnParticlePairsInfo::setDCAPart2Max(Double_t val) {
    fDCAPart2Max = val;
}

void BmnParticlePairsInfo::setDCA12Min(Double_t val) {
    fDCA12Min = val;
}

void BmnParticlePairsInfo::setDCA12Max(Double_t val) {
    fDCA12Max = val;
}

void BmnParticlePairsInfo::setPathMin(Double_t val) {
    fPathMin = val;
}

void BmnParticlePairsInfo::setPathMax(Double_t val) {
    fPathMax = val;
}

// Getters

Double_t BmnParticlePairsInfo::getMomPart1Min() {
    return fMomPart1Min;
}

Double_t BmnParticlePairsInfo::getMomPart1Max() {
    return fMomPart1Max;
}

Double_t BmnParticlePairsInfo::getMomPart2Min() {
    return fMomPart2Min;
}

Double_t BmnParticlePairsInfo::getMomPart2Max() {
    return fMomPart2Max;
}

Double_t BmnParticlePairsInfo::getEtaPart1Min() {
    return fEtaPart1Min;
}

Double_t BmnParticlePairsInfo::getEtaPart1Max() {
    return fEtaPart1Max;
}

Double_t BmnParticlePairsInfo::getEtaPart2Min() {
    return fMomPart2Min;
}

Double_t BmnParticlePairsInfo::getEtaPart2Max() {
    return fMomPart2Max;
}

Double_t BmnParticlePairsInfo::getDCAPart1Min() {
    return fDCAPart1Min;
}

Double_t BmnParticlePairsInfo::getDCAPart1Max() {
    return fDCAPart1Max;
}

Double_t BmnParticlePairsInfo::getDCAPart2Min() {
    return fDCAPart2Min;
}

Double_t BmnParticlePairsInfo::getDCAPart2Max() {
    return fDCAPart2Max;
}

Double_t BmnParticlePairsInfo::getDCA12Min() {
    return fDCA12Min;
}

Double_t BmnParticlePairsInfo::getDCA12Max() {
    return fDCA12Max;
}

Double_t BmnParticlePairsInfo::getPathMin() {
    return fPathMin;
}

Double_t BmnParticlePairsInfo::getPathMax() {
    return fPathMax;
}

BmnParticlePairsInfo::~BmnParticlePairsInfo() {
    
}