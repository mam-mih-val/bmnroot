#ifndef BMNSiBTMODULE_H
#define	BMNSiBTMODULE_H

#include "BmnMatch.h"

#include "BmnSiBTLayer.h"

#include <iostream>
#include <vector>

using namespace TMath;
using namespace std;

class BmnSiBTModule {

public:
    //Constructors -------------------------------------------------------------
    BmnSiBTModule();

    BmnSiBTModule(Double_t z_start_pos, Double_t module_thick);
    //--------------------------------------------------------------------------

    //Destructor
    virtual ~BmnSiBTModule();

    void SetVerbosity(Bool_t verb) { Verbosity = verb; }
    Bool_t GetVerbosity() { return Verbosity; }

    //Module parameters --------------------------------------------------------
    Double_t GetXMinModule() { return XMinModule; }
    Double_t GetXMaxModule() { return XMaxModule; }
    Double_t GetYMinModule() { return YMinModule; }
    Double_t GetYMaxModule() { return YMaxModule; }

    Double_t GetXSize() { return XMaxModule-XMinModule; }
    Double_t GetYSize() { return YMaxModule-YMinModule; }

    void SetZStartModulePosition(Double_t zpos_module) { ZStartModulePosition = zpos_module; }
    Double_t GetZStartModulePosition() { return ZStartModulePosition; }

    Double_t GetModuleThickness() { return ModuleThickness; }
    Double_t GetZPositionRegistered(); //position for all registered point (hits)
    //--------------------------------------------------------------------------

    //Strip layers controls ----------------------------------------------------
    void AddStripLayer(BmnSiBTLayer strip_layer);

    Bool_t SetStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal);
    Bool_t AddStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal);
    Bool_t SetStripMatchInLayer(Int_t layer_num, Int_t strip_num, BmnMatch strip_match);

    Double_t GetStripSignalInLayer(Int_t layer_num, Int_t strip_num);
    BmnMatch GetStripMatchInLayer(Int_t layer_num, Int_t strip_num);

    Int_t GetNStripLayers() { return StripLayers.size(); }
    BmnSiBTLayer& GetStripLayer(Int_t num) { return StripLayers.at(num); }
    vector<BmnSiBTLayer>& GetStripLayers() { return StripLayers; }

    void ResetModuleData(); //clear all data in the module and all layers (not delete layers!)
    //--------------------------------------------------------------------------

    //Point ownership of the module --------------------------------------------
    Bool_t IsPointInsideModule(Double_t x, Double_t y); //plane ownership
    Bool_t IsPointInsideModule(Double_t x, Double_t y, Double_t z); //volume ownership
    Bool_t IsPointInsideZThickness(Double_t z); // point with z-coord is between z_start and z_end of the module (inside module thickness)?
    //--------------------------------------------------------------------------

    //Methods to add a point to the module -------------------------------------
    Bool_t AddRealPointFullOne(Double_t x, Double_t y, Double_t z,
                               Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID);

    Bool_t AddRealPointSimple(Double_t x, Double_t y, Double_t z,
                              Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID); //old

    //make a strip cluster from a single point (with gauss smearing)
    StripCluster MakeCluster(Int_t layer_num, Double_t xcoord, Double_t ycoord, Double_t signal, Double_t radius);

    //Methods to calculate intersection points in the module -------------------
    void CalculateStripHitIntersectionPoints();

    //need for a separated test (find x,y intersection coords from strip positions)
    Bool_t SearchIntersectionPoint(Double_t &x, Double_t &y, Double_t strip_pos_layerA, Double_t strip_pos_layerB, Int_t layerA_index, Int_t layerB_index);
    //--------------------------------------------------------------------------

    //Added (real) points ------------------------------------------------------
    Int_t GetNRealPoints() {return RealPointsX.size();} //quantity of added points
    Double_t GetRealPointX(Int_t indx) { return RealPointsX.at(indx); } //X-coord of i-added point
    Double_t GetRealPointMC(Int_t indx) { return RealPointsMC.at(indx); } //MC-index of i-added point

    void ResetRealPoints() { RealPointsX.clear(); RealPointsY.clear(); RealPointsMC.clear(); }
    //--------------------------------------------------------------------------

    //Intersection points ------------------------------------------------------
    Int_t GetNIntersectionPoints() {return IntersectionPointsX.size();} //quantity of intersections
    Double_t GetIntersectionPointX(Int_t indx) { return IntersectionPointsX.at(indx); } //X-coord of indx-intersection point
    Double_t GetIntersectionPointY(Int_t indx) { return IntersectionPointsY.at(indx); } //Y-coord of indx-intersection point
    Double_t GetIntersectionPointXError(Int_t indx) { return IntersectionPointsXErrors.at(indx); } //X-coord error of indx-intersection point
    Double_t GetIntersectionPointYError(Int_t indx) { return IntersectionPointsYErrors.at(indx); } //Y-coord error of indx-intersection point
    Int_t GetIntersectionPoint_LowerLayerClusterSize(Int_t indx) { return IntersectionPoints_LowerLayerClusterSize.at(indx); } //cluster size in the lower layer
    Int_t GetIntersectionPoint_UpperLayerClusterSize(Int_t indx) { return IntersectionPoints_UpperLayerClusterSize.at(indx); } //cluster size in the upper layer
    Double_t GetIntersectionPoint_LowerLayerSripPosition(Int_t indx) { return IntersectionPoints_LowerLayerStripPosition.at(indx); } //strip position in the lower layer
    Double_t GetIntersectionPoint_UpperLayerSripPosition(Int_t indx) { return IntersectionPoints_UpperLayerStripPosition.at(indx); } //strip position in the upper layer
    Double_t GetIntersectionPoint_LowerLayerSripTotalSignal(Int_t indx) { return IntersectionPoints_LowerLayerStripTotalSignal.at(indx); } //total signal  in the lower layer
    Double_t GetIntersectionPoint_UpperLayerSripTotalSignal(Int_t indx) { return IntersectionPoints_UpperLayerStripTotalSignal.at(indx); } //total signal  in the upper layer
    BmnMatch GetIntersectionPointMatch(Int_t indx) { return IntersectionPointMatches.at(indx); } //Intersection point match

    void ResetIntersectionPoints();
    //--------------------------------------------------------------------------

private:
    void DefineModuleBorders(); //calculate min-max coordinates of the module from layers` parameters

private:
    BmnSiBTModule(const BmnSiBTModule&) = delete;
    BmnSiBTModule& operator=(const BmnSiBTModule&) = delete;

private:
    Bool_t Verbosity;

    Double_t XMinModule;
    Double_t XMaxModule;
    Double_t YMinModule;
    Double_t YMaxModule;

    Double_t ZStartModulePosition;

    Double_t ModuleThickness;

    vector<BmnSiBTLayer> StripLayers;

    vector<Double_t> RealPointsX;
    vector<Double_t> RealPointsY;
    vector<Double_t> RealPointsMC; // index of MC point

    vector<Double_t> IntersectionPointsX;
    vector<Double_t> IntersectionPointsY;
    vector<Double_t> IntersectionPointsXErrors;
    vector<Double_t> IntersectionPointsYErrors;

    vector<Int_t> IntersectionPoints_LowerLayerClusterSize; //cluster size (number of strips) in the lower layer for each intersection point
    vector<Int_t> IntersectionPoints_UpperLayerClusterSize; //cluster size (number of strips) in the upper layer for each intersection point

    vector<Double_t> IntersectionPoints_LowerLayerStripPosition; //strip position in the lower layer for each intersection point
    vector<Double_t> IntersectionPoints_UpperLayerStripPosition; //strip position in the upper layer for each intersection point

    vector<Double_t> IntersectionPoints_LowerLayerStripTotalSignal; //total signal in the lower layer for each intersection point
    vector<Double_t> IntersectionPoints_UpperLayerStripTotalSignal; //total signal in the upper layer for each intersection point

    vector<BmnMatch> IntersectionPointMatches;

};
//------------------------------------------------------------------------------

#endif	/* BMNSiBTMODULE_H */

