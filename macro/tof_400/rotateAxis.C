/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   rotateAxis.C
 * Author: mikhailr
 *
 * Created on May 22, 2018, 4:52 PM
 */

#include <cstdlib>
#include <TMath.h>

using namespace std;

void rotateAxis(Double_t X = 0, Double_t Y = 0, Double_t Z = 0, Double_t angle = 0) {

    //gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    //bmnloadlibs(); // load libraries

    Double_t XX, YY, ZZ, AngleRad;
    AngleRad = angle * TMath::DegToRad();
    //z- beam, y - vertical
    //Rotate around Y
    XX = X * TMath::Cos(AngleRad) - Z * TMath::Sin(AngleRad);
    YY = Y;
    ZZ = X * TMath::Sin(AngleRad) + Z * TMath::Cos(AngleRad);
    cout << "New X = " << XX << "; New Y = " << YY << "; New Z = " << ZZ << endl;
}

