#ifndef BMNSiBTLAYER_H
#define	BMNSiBTLAYER_H

#include "BmnMatch.h"
#include "BmnStripData.h"

#include "TMath.h"

#include <iostream>
#include <vector>

using namespace TMath;
using namespace std;

class BmnSiBTLayer {

public:
    //Constructors -------------------------------------------------------------
    BmnSiBTLayer();

    BmnSiBTLayer(Int_t zone_number, StripLayerType layer_type,
                Double_t xsize, Double_t ysize,
                Double_t xorig, Double_t yorig,
                Double_t pitch, Double_t adeg);
    //--------------------------------------------------------------------------

    //Destructor
    virtual ~BmnSiBTLayer();

    void InitializeLayer();
    void ResetLayer(); //clear all data and rebuild the layer in accordance with new parameters

    Int_t GetNStrips();

    void SetVerbosity(Bool_t verb) { Verbosity = verb; }
    void SetPitch(Double_t pitch);
    void SetLayerSizes(Double_t xsize, Double_t ysize, Double_t xorig=0.0, Double_t yorig=0.0);
    void SetAngleDeg(Double_t deg); // plus - clockwise from vertical

    Bool_t GetVerbosity() { return Verbosity; }
    Int_t GetZoneNumber() { return LayerZoneNumber; }
    StripLayerType GetType() { return LayerType; }
    Double_t GetPitch() { return Pitch; }
    Double_t GetAngleDeg() { return AngleDeg; }
    Double_t GetAngleRad() { return AngleRad; }
    Double_t GetXSize() { return XMaxLayer-XMinLayer; }
    Double_t GetYSize() { return YMaxLayer-YMinLayer; }
    Double_t GetXMinLayer() { return XMinLayer; }
    Double_t GetXMaxLayer() { return XMaxLayer; }
    Double_t GetYMinLayer() { return YMinLayer; }
    Double_t GetYMaxLayer() { return YMaxLayer; }

    //Strip numbering methods --------------------------------------------------
    Bool_t SetStripNumberingBorders(Double_t x_left, Double_t y_left, Double_t x_right, Double_t y_right); //or method below
    Bool_t SetStripNumberingBorders(StripBorderPoint left, StripBorderPoint right);
    Bool_t SetStripNumberingOrder(StripNumberingDirection strip_direction);

    StripNumberingDirection GetStripNumberingOrder() { return  StripOrder; }
    Double_t GetXLeftStripBorderPoint() { return XLeftPointOfStripNumbering; }
    Double_t GetYLeftStripBorderPoint() { return YLeftPointOfStripNumbering; }
    Double_t GetXRightStripBorderPoint() { return XRightPointOfStripNumbering; }
    Double_t GetYRightStripBorderPoint() { return YRightPointOfStripNumbering; }
    //--------------------------------------------------------------------------

    //Dead zones methods -------------------------------------------------------
    Bool_t AddDeadZone(Int_t n_points, Double_t *x_points, Double_t *y_points);
    Bool_t AddDeadZone(DeadZoneOfStripLayer dead_zone);
    Bool_t IsPointInsideDeadZones(Double_t x, Double_t y);
    Bool_t IsPointInsideStripLayer(Double_t x, Double_t y);

    vector<DeadZoneOfStripLayer> GetDeadZones() { return DeadZones; }
    void ResetAllDeadZones() { DeadZones.clear(); }
    //--------------------------------------------------------------------------

    //Direct strip signal manipulation -----------------------------------------
    Bool_t SetStripSignal(Int_t strip_num, Double_t signal); //replace signal
    Bool_t AddStripSignal(Int_t strip_num, Double_t signal); //add signal

    Double_t GetStripSignal(Int_t strip_num); //signal of strip_num-strip
    //--------------------------------------------------------------------------

    //Strip matches controls ---------------------------------------------------
    Bool_t SetStripMatch(Int_t strip_num, BmnMatch strip_match);
    Bool_t AddLinkToStripMatch(Int_t strip_num, Double_t weight, Int_t refID);

    BmnMatch GetStripMatch(Int_t strip_num); //match of strip_num-strip
    void ResetStripMatches();
    //--------------------------------------------------------------------------

    //Strip hits ---------------------------------------------------------------
    Int_t GetNStripHits() { return StripHits.size(); } //number of hits at the layer
    Double_t GetStripHitPos(Int_t num); //hit position
    Double_t GetStripHitTotalSignal(Int_t num); //sum signal of the hit
    Double_t GetStripHitError(Int_t num); //hit error
    Int_t GetStripHitClusterSize(Int_t num); //number of strips in the cluster for the num-hit
    void ResetStripHits();
    //--------------------------------------------------------------------------

    //Methods to convert point coordinates into strip system -------------------
    Double_t ConvertNormalPointToStripX(Double_t x, Double_t y);
    Double_t ConvertNormalPointToStripY(Double_t x, Double_t y);

    Double_t ConvertPointToStripPosition(Double_t x, Double_t y);

    Double_t CalculateStripEquationB(Double_t strip_pos);
    //--------------------------------------------------------------------------

    //Clustering and strip-hit finding methods ---------------------------------------
    void FindClustersAndStripHits();
    void MakeStripHit(StripCluster &cluster, vector<Double_t> &AnalyzableStrips, Int_t &curcnt);
    void SmoothStripSignal(vector<Double_t>& AnalyzableStrips, Int_t NIterations, Int_t SmoothWindow, Double_t Weight);

    void SetClusterFindingThreshold(Double_t threshold) { ClusterFindingThreshold = threshold; }
    Double_t GetClusterFindingThreshold() { return ClusterFindingThreshold; }

    vector<Double_t> GetSmoothStrips() { return SmoothStrips; } //for test
    //--------------------------------------------------------------------------

private:
    //BmnSiBTLayer(const BmnSiBTLayer&) = delete;
    //BmnSiBTLayer& operator=(const BmnSiBTLayer&) = delete;

private:
    Bool_t Verbosity;

    Int_t LayerZoneNumber; //zone number of the strip layer
    StripLayerType LayerType; //type of the strip layer: lower or upper

    Double_t Pitch;
    Double_t AngleDeg; //Angle between two sets of parallel strips: angle from vertical
    Double_t AngleRad;

    Double_t XMinLayer;
    Double_t XMaxLayer;
    Double_t YMinLayer;
    Double_t YMaxLayer;

    vector<DeadZoneOfStripLayer> DeadZones;

    //Strip arrangement
    StripNumberingDirection StripOrder; //strip numbering order (LeftToRight or RightToLeft)
    Double_t XLeftPointOfStripNumbering;
    Double_t XRightPointOfStripNumbering;
    Double_t YLeftPointOfStripNumbering;
    Double_t YRightPointOfStripNumbering;

    //Strip layer implementation
    vector<Double_t> Strips;

    //Strip matches
    vector<BmnMatch> StripMatches; //ID-point matches for all strips

    //Strip hits (It is filled after cluster finding and fitting) --------------
    vector<Double_t> StripHits; //positions of strip hits
    vector<Double_t> StripHitsTotalSignal;
    vector<Double_t> StripHitsErrors;
    vector<Int_t> StripHitsClusterSize; //cluster size (number of strips) for each strip hit
    vector<Double_t> SmoothStrips; //smoothed signal strips (It is filled after smoothing algorithm)
    Double_t ClusterFindingThreshold; //min. threshold cut
    //--------------------------------------------------------------------------
};
//------------------------------------------------------------------------------

#endif	/* BMNSiBTLAYER_H */

