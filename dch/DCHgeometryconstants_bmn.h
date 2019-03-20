#ifndef DCHGEOMETRYCONSTANTS_BMN_H
#define DCHGEOMETRYCONSTANTS_BMN_H

#include "TMath.h"

//Beam entering position
static Double_t beamPos[3] = {0.,0.,-210.};
/*static Double_t beamPos[3][numRunPeriods] = {
                                             {0.,0.,-210.};
                                             {0.,0.,-210.};
                                             {0.,0.,0.};
                                             {0.,0.,0.};
                                             {0.,0.,0.};
                                             {0.,0.,-210.};
                                             {0.,0.,0.};
                                            }*/
//Basic detector parameters   
const UShort_t numChambers = 2; // number of drift chambers
const UShort_t numLayers = 8; // number of anode wire planes in the drift chamber
const UShort_t numLayers_half = numLayers/2; // half number of anode wire planes in the drift chamber
const UShort_t twoNumLayers = numLayers*numChambers; // number of anode wire planes in both the drift chambers
const UShort_t ngaps = numLayers/2-1; // number of gaps in the drift chamber
//const UShort_t dimzarray=numChambers*numLayers; // number of anode wire planes in the drift chambers
const UShort_t numRunPeriods=7;

//const UShort_t runType=1; // run type: 1,2,3
//Detector's position
//const Double_t deflectAngle = 0.3/5.7 ; 
//const Double_t deflectAngle = 0. ; 
//const Double_t angleXZ = 0.029845130209; //1.9 grad = 1.71 degree 
//const Double_t angleXZ = 0.04712389; //2.7 degree 
//const Double_t angleXZ = -0.04712389; //2.7 degree 
//const Double_t angleXZ = 0.087266463; //5 degrees 
const Double_t angleXZ = 0.0; //0. grad = 0. degree 
static Double_t DCH1_Xpos[numRunPeriods] = {0.,0.,0.,0.,0.,0.,0.};
static Double_t DCH1_Ypos[numRunPeriods] = {0.,0.,0.,0.,0.,0.,0.};
//static Double_t DCH1_Xpos[numRunPeriods] = {0.,0.,0.,0.,0.,-39.381,0.};
//static Double_t DCH1_Ypos[numRunPeriods] = {0.,0.,0.,0.,0.,1.2917,0.};
const Double_t DCH1_Zpos[numRunPeriods] = {610.,610.,662.05,0.,0.,515.5,0.}; //cm
//const Double_t DCH1_Zpos[3] = {550.0,637.85,637.85}; //cm
static Double_t DCH2_Xpos[numRunPeriods] = {0.,0.,0.,0.,0.,0.,0.};
static Double_t DCH2_Ypos[numRunPeriods] = {0.,0.,0.,0.,0.,0.,0.};
//static Double_t DCH2_Xpos[numRunPeriods] = {0.,0.,0.,0.,0.,-41.299,0.};
//static Double_t DCH2_Ypos[numRunPeriods] = {0.,0.,0.,0.,0.,2.2445,0.};
const Double_t DCH2_Zpos[numRunPeriods] = {720.,720.,772.05,0.,0.,715.,0.}; //cm
//Double_t DCH2_Zpos[3] = {650.0,748.35,748.35}; //cm
//const Double_t DCH2_Zpos[3] = {650.0,673.,748.35}; //cm

//const Double_t DCH_ZlayerPos_local[numLayers] = {-7.8, -6.6, -3.0, -1.8, 1.8, 3.0, 6.6, 7.8}; // position of gas volumes (layers) within DCH
const Double_t DCH_ZlayerPos_local[numLayers] = {7.8, 6.6, 3.0, 1.8, -1.8, -3.0, -6.6, -7.8}; // position of gas volumes (layers) within DCH
//const Double_t DCH_ZlayerPos_local[numLayers] = {3.0, 1.8, 7.8, 6.6, -6.6, -7.8, -1.8, -3.0}; // position of gas volumes (layers) within DCH
//const Double_t DCH_ZlayerPos_local[numLayers] = {9.3, 8.1, 3.5, 2.3, -2.3, -3.5, -8.1, -9.3}; // position of gas volumes (layers) within DCH
//const Double_t DCH_ZlayerPos_local[numLayers] = {10.2, 9., 3.8, 2.6, -2.6, -3.8, -9., -10.2}; // position of gas volumes (layers) within DCH
//const Double_t DCH_ZlayerPos_local[numLayers] = {-9.3, -8.1, -3.5, -2.3, 2.3, 3.5, 8.1, 9.3}; // position of gas volumes (layers) within DCH

//const Double_t DCH1_ZlayerPos_global[numLayers] = {DCH_ZlayerPos_local[0]+DCH1_Zpos,DCH_ZlayerPos_local[1]+DCH1_Zpos,DCH_ZlayerPos_local[2]+DCH1_Zpos,DCH_ZlayerPos_local[3]+DCH1_Zpos,DCH_ZlayerPos_local[4]+DCH1_Zpos,DCH_ZlayerPos_local[5]+DCH1_Zpos,DCH_ZlayerPos_local[6]+DCH1_Zpos,DCH_ZlayerPos_local[7]+DCH1_Zpos}; // position of gas volumes (layers) of DCH1 in laboratory CS
//const Double_t DCH2_ZlayerPos_global[numLayers] = {DCH_ZlayerPos_local[0]+DCH2_Zpos,DCH_ZlayerPos_local[1]+DCH2_Zpos,DCH_ZlayerPos_local[2]+DCH2_Zpos,DCH_ZlayerPos_local[3]+DCH2_Zpos,DCH_ZlayerPos_local[4]+DCH2_Zpos,DCH_ZlayerPos_local[5]+DCH2_Zpos,DCH_ZlayerPos_local[6]+DCH2_Zpos,DCH_ZlayerPos_local[7]+DCH2_Zpos}; // position of gas volumes (layers) of DCH2 in laboratory CS

//Detector's construct parameters   
const Double_t angleLayer[numLayers] = {0., 0., 90., 90., 45., 45., -45., -45.}; // angles of inclined wires, degrees // for exp.
//const Double_t angleLayer[numLayers] = {-45., -45., 45., 45., 90., 90., 0., 0.}; // angles of inclined wires, degrees // for exp.
//const Double_t angleLayer[numLayers] = {0., 0., 90., 90., -45., -45., 45., 45.}; // angles of inclined wires, degrees // for sim.
//const Double_t angleStereoRad=angleStereo*TMath::DegToRad();
const Double_t angleLayerRad[numLayers] = {angleLayer[0]*TMath::DegToRad(), angleLayer[1]*TMath::DegToRad(), angleLayer[2]*TMath::DegToRad(), angleLayer[3]*TMath::DegToRad(), angleLayer[4]*TMath::DegToRad(), angleLayer[5]*TMath::DegToRad(), angleLayer[6]*TMath::DegToRad(), angleLayer[7]*TMath::DegToRad()}; // angles  of inclined wires, radians
const Double_t dirCosLayerX[numLayers] = {cos(angleLayerRad[0]), cos(angleLayerRad[1]), cos(angleLayerRad[2]), cos(angleLayerRad[3]), cos(angleLayerRad[4]), cos(angleLayerRad[5]), cos(angleLayerRad[6]), cos(angleLayerRad[7])}; // x direction  cosines of inclined wires
const Double_t dirCosLayerY[numLayers] = {sin(angleLayerRad[0]), sin(angleLayerRad[1]), sin(angleLayerRad[2]), sin(angleLayerRad[3]), sin(angleLayerRad[4]), sin(angleLayerRad[5]), sin(angleLayerRad[6]), sin(angleLayerRad[7])}; // y direction  cosines of inclined wires

const Double_t ZLength_DCH = 20.0;      
const Double_t ZLength_DCHActiveVolume = 0.6;      
const Double_t ZhalfLength_DCHActiveVolume = ZLength_DCHActiveVolume/2.;
const Double_t ZhalfLength_DCHActiveVolume_W = ZhalfLength_DCHActiveVolume+0.1; //wider
//const Double_t DCH1_ZlayerPos_global_End[numLayers] = {DCH1_ZlayerPos_global[0]+ZhalfLength_DCHActiveVolume,DCH1_ZlayerPos_global[1]+ZhalfLength_DCHActiveVolume,DCH1_ZlayerPos_global[2]+ZhalfLength_DCHActiveVolume,DCH1_ZlayerPos_global[3]+ZhalfLength_DCHActiveVolume,DCH1_ZlayerPos_global[4]+ZhalfLength_DCHActiveVolume,DCH1_ZlayerPos_global[5]+ZhalfLength_DCHActiveVolume,DCH1_ZlayerPos_global[6]+ZhalfLength_DCHActiveVolume,DCH1_ZlayerPos_global[7]+ZhalfLength_DCHActiveVolume}; // ends of gas volumes (layers) of DCH1 in laboratory CS
//const Double_t DCH2_ZlayerPos_global_End[numLayers] = {DCH2_ZlayerPos_global[0]+ZhalfLength_DCHActiveVolume,DCH2_ZlayerPos_global[1]+ZhalfLength_DCHActiveVolume,DCH2_ZlayerPos_global[2]+ZhalfLength_DCHActiveVolume,DCH2_ZlayerPos_global[3]+ZhalfLength_DCHActiveVolume,DCH2_ZlayerPos_global[4]+ZhalfLength_DCHActiveVolume,DCH2_ZlayerPos_global[5]+ZhalfLength_DCHActiveVolume,DCH2_ZlayerPos_global[6]+ZhalfLength_DCHActiveVolume,DCH2_ZlayerPos_global[7]+ZhalfLength_DCHActiveVolume}; // ends of gas volumes (layers) of DCH2 in laboratory CS
const Double_t HoleSize_DCH = 12.0;

const Double_t SideLengthOfOctagon = 120.0;
const Double_t InnerRadiusOfOctagon = (2.414*SideLengthOfOctagon)/2.0;

const Double_t MaxRadiusOfActiveVolume = 120.0;
const Double_t MinRadiusOfActiveVolume = 12.0;

//const UShort_t numWiresPerLayer = 256;
const UInt_t numWiresPerLayer = 240;
const UInt_t halfNumWiresPerLayer = numWiresPerLayer/2;
//const Double_t stepXY=2.*MaxRadiusOfActiveVolume/Double_t(numWiresPerLayer); 
const Double_t stepXY=1.; 
const Double_t stepXYhalf=stepXY/2.;
const Int_t detIdDch1=13;
const Int_t detIdDch2=17;
//const Double_t diagonal=sqrt(stepXYhalf*stepXYhalf+ZhalfLength_DCHActiveVolume*ZhalfLength_DCHActiveVolume);

#endif
