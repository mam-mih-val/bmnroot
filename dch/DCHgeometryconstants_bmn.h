#ifndef DCHGEOMETRYCONSTANTS_BMN_H
#define DCHGEOMETRYCONSTANTS_BMN_H

//Basic detector parameters   
const UShort_t numChambers = 2; // number of drift chambers
const UShort_t numLayers = 8; // number of gas volumes in the drift chambers
const UShort_t numLayers_half = numLayers/2; // half number of gas volumes in the drift chambers
const UShort_t twoNumLayers = numLayers*2; // half number of gas volumes in the drift chambers
const UShort_t ngaps = numLayers/2-1; // number of gaps in the drift chambers
const UShort_t dimzarray=numChambers*numLayers; // number of gas volumes in the drift chambers

const UShort_t runType=1; // run type: 1,2,3
//Detector's position
const Double_t DCH1xShift=0.;
const Double_t DCH2xShift=0.;
//const Double_t deflectAngle = 0.3/5.7 ; // tangent of assumed deflection angle
const Double_t deflectAngle = 0. ; // tangent of assumed deflection angle
static Double_t DCH1_Ypos[3] = {0.,0.,0.};
//const Double_t DCH1_Zpos[3] = {550.0,637.85,637.85}; //cm
const Double_t DCH1_Zpos[3] = {550.0,610.,637.85}; //cm
//Double_t DCH1_Xpos[3] = {DCH1_Zpos[0]*deflectAngle,DCH1_Zpos[1]*deflectAngle,DCH1_Zpos[2]*deflectAngle};
static Double_t DCH1_Xpos[3] = {DCH1_Zpos[0]*deflectAngle+DCH1xShift,DCH1_Zpos[1]*deflectAngle+DCH1xShift,DCH1_Zpos[2]*deflectAngle+DCH1xShift};
static Double_t DCH2_Ypos[3] = {0.,0.,0.};;
//Double_t DCH2_Zpos[3] = {650.0,748.35,748.35}; //cm
const Double_t DCH2_Zpos[3] = {650.0,720.,748.35}; //cm
//Double_t DCH2_Xpos[3] = {DCH2_Zpos[0]*deflectAngle,DCH2_Zpos[1]*deflectAngle,DCH2_Zpos[2]*deflectAngle};
static Double_t DCH2_Xpos[3] = {DCH2_Zpos[0]*deflectAngle+DCH2xShift,DCH2_Zpos[1]*deflectAngle+DCH2xShift,DCH2_Zpos[2]*deflectAngle+DCH2xShift};

//const Double_t DCH_ZlayerPos_local[numLayers] = {-7.8, -6.6, -3.0, -1.8, 1.8, 3.0, 6.6, 7.8}; // position of gas volumes (layers) within DCH
//const Double_t DCH_ZlayerPos_local[numLayers] = {7.8, 6.6, 3.0, 1.8, -1.8, -3.0, -6.6, -7.8}; // position of gas volumes (layers) within DCH
const Double_t DCH_ZlayerPos_local[numLayers] = {9.3, 8.1, 3.5, 2.3, -2.3, -3.5, -8.1, -9.3}; // position of gas volumes (layers) within DCH

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
