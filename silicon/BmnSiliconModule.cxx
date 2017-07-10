#include "BmnSiliconModule.h"

#include "TMath.h"

#include <iostream>

using namespace TMath;
using namespace std;

BmnSiliconModule::BmnSiliconModule() {

    Verbosity = true;

    ZStartModulePosition = 0.0;
    ModuleThickness = 0.1; //cm

    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;
}

BmnSiliconModule::BmnSiliconModule(Double_t z_start_pos) {

    Verbosity = true;

    ZStartModulePosition = z_start_pos;
    ModuleThickness = 0.03; //cm


    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;
}

BmnSiliconModule::~BmnSiliconModule() {

}

Double_t BmnSiliconModule::GetZPositionRegistered() {
    return ZStartModulePosition + ModuleThickness*0.5;
}

void BmnSiliconModule::AddStripLayer(BmnSiliconLayer strip_layer) {
    StripLayers.push_back(strip_layer);

    DefineModuleBorders();
}

Bool_t BmnSiliconModule::SetStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].SetStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnSiliconModule::AddStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].AddStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnSiliconModule::SetStripSignalInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t signal) {
    Int_t n_layers_the_same_id = 0;
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].SetStripSignal(strip_num, signal)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnSiliconModule::AddStripSignalInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t signal) {
    Int_t n_layers_the_same_id = 0;
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].AddStripSignal(strip_num, signal)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Double_t BmnSiliconModule::GetStripSignalInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].GetStripSignal(strip_num);
    }
    return -1;
}

void BmnSiliconModule::ResetModuleData() {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        StripLayers[ilayer].ResetLayer();
    }

    DefineModuleBorders();

    ResetRealPoints();
    ResetIntersectionPoints();
}

Bool_t BmnSiliconModule::IsPointInsideModule(Double_t x, Double_t y) {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( StripLayers[ilayer].IsPointInsideStripLayer(x, y) ) return true;
    }
    return false;
}

Bool_t BmnSiliconModule::IsPointInsideModule(Double_t x, Double_t y, Double_t z) {
    if( z >= ZStartModulePosition && z <= (ZStartModulePosition+ModuleThickness) ) {
        if( IsPointInsideModule(x, y) ) return true;
    }
    return false;
}

Bool_t BmnSiliconModule::IsPointInsideZThickness(Double_t z) {
    if( z >= ZStartModulePosition && z <= (ZStartModulePosition+ModuleThickness) ) {
        return true;
    }
    return false;
}

//Add a single point without smearing
Bool_t BmnSiliconModule::AddRealPointSimple(Double_t x, Double_t y, Double_t z,
                                            Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    if( IsPointInsideModule(x, y) ) {

        Int_t n_strip_layers = StripLayers.size();

        //Add a signal to the strips layers ------------------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            if( !StripLayers[ilayer].IsPointInsideStripLayer(x,y) ) continue;

            Double_t strip_pos = StripLayers[ilayer].ConvertPointToStripPosition(x, y);
            Int_t zone_id = StripLayers[ilayer].GetZoneID();

            AddStripSignalInLayerByZoneId(zone_id, (Int_t)strip_pos, signal);
        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnSiliconModule::AddRealPointSimple(): Point (" << x << " : " << y << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

void BmnSiliconModule::CalculateStripHitIntersectionPoints() {

    ResetIntersectionPoints();

    Int_t n_strip_layers = StripLayers.size();

    //Find strip clusters and hits in the strip layers -------------------------
    for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
        StripLayers[ilayer].FindClustersAndStripHits();
    }
    //--------------------------------------------------------------------------

    for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {

        for(Int_t jlayer = ilayer+1; jlayer < n_strip_layers; ++jlayer) {
            //cout << "(i-layer : j-layer) = ( " << ilayer << " : " << jlayer << " )\n";

            if(StripLayers[ilayer].GetType() == StripLayers[jlayer].GetType()) continue;

            Double_t iAngleRad = StripLayers[ilayer].GetAngleRad();
            Double_t jAngleRad = StripLayers[jlayer].GetAngleRad();

            if( Abs(iAngleRad - jAngleRad) < 0.01 ) continue;

            Double_t i_PiOver2MinusAngleRad = PiOver2() - iAngleRad;
            Double_t j_PiOver2MinusAngleRad = PiOver2() - jAngleRad;

            for(Int_t i_strip_hit = 0; i_strip_hit < StripLayers[ilayer].GetNStripHits(); ++i_strip_hit) {
                for(Int_t j_strip_hit = 0; j_strip_hit < StripLayers[jlayer].GetNStripHits(); ++j_strip_hit) {

                    Double_t iStripHitPos = StripLayers[ilayer].GetStripHitPos(i_strip_hit);
                    Double_t jStripHitPos = StripLayers[jlayer].GetStripHitPos(j_strip_hit);

                    Double_t iStripHitError = StripLayers[ilayer].GetStripHitError(i_strip_hit);
                    Double_t jStripHitError = StripLayers[jlayer].GetStripHitError(j_strip_hit);

                    Double_t xcoord;
                    Double_t ycoord;

                    if( Abs(iAngleRad) >= 0.01 && Abs(jAngleRad) >= 0.01 ) {

                        Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
                        Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);

                        xcoord = (jB - iB)/(Tan(i_PiOver2MinusAngleRad) - Tan(j_PiOver2MinusAngleRad));
                        ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
                    }
                    else {

                        if( Abs(iAngleRad) < 0.01 ) {

                            if(StripLayers[ilayer].GetStripNumberingOrder() == LeftToRight ) {
                                xcoord = StripLayers[ilayer].GetXLeftStripBorderPoint() + (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
                            }
                            else {
                                xcoord = StripLayers[ilayer].GetXRightStripBorderPoint() - (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
                            }

                            Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);
                            ycoord = Tan(j_PiOver2MinusAngleRad)*xcoord + jB;
                        }

                        if( Abs(jAngleRad) < 0.01 ) {

                            if(StripLayers[jlayer].GetStripNumberingOrder() == LeftToRight ) {
                                xcoord = StripLayers[jlayer].GetXLeftStripBorderPoint() + (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
                            }
                            else {
                                xcoord = StripLayers[jlayer].GetXRightStripBorderPoint() - (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
                            }

                            Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
                            ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
                        }
                    }

                    //check: a point belongs to both strip layers together
                    if( StripLayers[ilayer].IsPointInsideStripLayer(xcoord, ycoord) && StripLayers[jlayer].IsPointInsideStripLayer(xcoord, ycoord) ) {

                        IntersectionPointsX.push_back(xcoord);
                        IntersectionPointsY.push_back(ycoord);

                        //Intersection (x,y)-errors from the strip-errors ------
                        Double_t AngleIntersecRad = Abs(iAngleRad - jAngleRad);

                        //a rhomb zone is the intersection of the strip errors
                        Double_t RhombSide1 = (iStripHitError*StripLayers[ilayer].GetPitch())/Sin(AngleIntersecRad);
                        Double_t RhombSide2 = (jStripHitError*StripLayers[jlayer].GetPitch())/Sin(AngleIntersecRad);

                        //x-strip error
                        Double_t xerr1 = Sin(Abs(jAngleRad))*RhombSide1;
                        Double_t xerr2 = Sin(Abs(iAngleRad))*RhombSide2;
                        Double_t xcoord_err = xerr1 + xerr2;

                        //y-strip error
                        Double_t yerr1 = Cos(Abs(jAngleRad))*RhombSide1;
                        Double_t yerr2 = Cos(Abs(iAngleRad))*RhombSide2;
                        Double_t ycoord_err = yerr1 + yerr2;

                        IntersectionPointsXErrors.push_back(xcoord_err);
                        IntersectionPointsYErrors.push_back(ycoord_err);
                        //------------------------------------------------------

                        //Additional information about the intersection ------------
                            //cluster size (number strips) in both strip layers for each intersection
                            Int_t iLayerClusterSize = StripLayers[ilayer].GetStripHitClusterSize(i_strip_hit);
                            Int_t jLayerClusterSize = StripLayers[jlayer].GetStripHitClusterSize(j_strip_hit);

                            //strip position in both strip layers for each intersection
                            Double_t iLayerStripPosition = StripLayers[ilayer].ConvertPointToStripPosition(xcoord, ycoord);
                            Double_t jLayerStripPosition = StripLayers[jlayer].ConvertPointToStripPosition(xcoord, ycoord);

                            if(StripLayers[ilayer].GetType() == LowerStripLayer) {
                                IntersectionPoints_LowerLayerClusterSize.push_back(iLayerClusterSize);
                                IntersectionPoints_UpperLayerClusterSize.push_back(jLayerClusterSize);
                                IntersectionPoints_LowerLayerStripPosition.push_back(iLayerStripPosition);
                                IntersectionPoints_UpperLayerStripPosition.push_back(jLayerStripPosition);
                            }
                            else {
                                IntersectionPoints_LowerLayerClusterSize.push_back(jLayerClusterSize);
                                IntersectionPoints_UpperLayerClusterSize.push_back(iLayerClusterSize);
                                IntersectionPoints_LowerLayerStripPosition.push_back(jLayerStripPosition);
                                IntersectionPoints_UpperLayerStripPosition.push_back(iLayerStripPosition);
                            }
                        //------------------------------------------------------
                    }
                }
            }
        }
    }
}

//need for a separated test (find x,y intersection coords from strip positions)
Bool_t BmnSiliconModule::SearchIntersectionPoint(Double_t &x, Double_t &y, Double_t strip_pos_layerA, Double_t strip_pos_layerB, Int_t layerA_index, Int_t layerB_index) {

    Int_t ilayer = layerA_index;
    Int_t jlayer = layerB_index;

    if(StripLayers[ilayer].GetType() == StripLayers[jlayer].GetType()) return false;

    Double_t iAngleRad = StripLayers[ilayer].GetAngleRad();
    Double_t jAngleRad = StripLayers[jlayer].GetAngleRad();

    if( Abs(iAngleRad - jAngleRad) < 0.01 ) return false;

    Double_t i_PiOver2MinusAngleRad = PiOver2() - iAngleRad;
    Double_t j_PiOver2MinusAngleRad = PiOver2() - jAngleRad;

    Double_t iStripHitPos = strip_pos_layerA;
    Double_t jStripHitPos = strip_pos_layerB;

    Double_t xcoord;
    Double_t ycoord;

    if( Abs(iAngleRad) >= 0.01 && Abs(jAngleRad) >= 0.01 ) {

        Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
        Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);

        xcoord = (jB - iB)/(Tan(i_PiOver2MinusAngleRad) - Tan(j_PiOver2MinusAngleRad));
        ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
    }
    else {

        if( Abs(iAngleRad) < 0.01 ) {

            if(StripLayers[ilayer].GetStripNumberingOrder() == LeftToRight ) {
                xcoord = StripLayers[ilayer].GetXLeftStripBorderPoint() + (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
            }
            else {
                xcoord = StripLayers[ilayer].GetXRightStripBorderPoint() - (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
            }

            Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);
            ycoord = Tan(j_PiOver2MinusAngleRad)*xcoord + jB;
        }

        if( Abs(jAngleRad) < 0.01 ) {

            if(StripLayers[jlayer].GetStripNumberingOrder() == LeftToRight ) {
                xcoord = StripLayers[jlayer].GetXLeftStripBorderPoint() + (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
            }
            else {
                xcoord = StripLayers[jlayer].GetXRightStripBorderPoint() - (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
            }

            Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
            ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
        }
    }

    x = xcoord;
    y = ycoord;

    //check: a point belongs to both strip layers together
    if( StripLayers[ilayer].IsPointInsideStripLayer(xcoord, ycoord) && StripLayers[jlayer].IsPointInsideStripLayer(xcoord, ycoord) ) {
        return true;
    }

    return false;
}

void BmnSiliconModule::ResetIntersectionPoints() {
    IntersectionPointsX.clear();
    IntersectionPointsY.clear();
    IntersectionPointsXErrors.clear();
    IntersectionPointsYErrors.clear();
    IntersectionPoints_LowerLayerClusterSize.clear();
    IntersectionPoints_UpperLayerClusterSize.clear();
    IntersectionPoints_LowerLayerStripPosition.clear();
    IntersectionPoints_UpperLayerStripPosition.clear();
}

void BmnSiliconModule::DefineModuleBorders() {

    if( StripLayers.size() == 0 ) return;

    XMinModule = 1.0E+10;
    XMaxModule = -1.0E+10;
    YMinModule = 1.0E+10;
    YMaxModule = -1.0E+10;

    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( StripLayers[ilayer].GetXMinLayer() < XMinModule ) XMinModule = StripLayers[ilayer].GetXMinLayer();
        if( StripLayers[ilayer].GetXMaxLayer() > XMaxModule ) XMaxModule = StripLayers[ilayer].GetXMaxLayer();
        if( StripLayers[ilayer].GetYMinLayer() < YMinModule ) YMinModule = StripLayers[ilayer].GetYMinLayer();
        if( StripLayers[ilayer].GetYMaxLayer() > YMaxModule ) YMaxModule = StripLayers[ilayer].GetYMaxLayer();
    }
    return;
}

//------------------------------------------------------------------------------

ClassImp(BmnSiliconModule)
