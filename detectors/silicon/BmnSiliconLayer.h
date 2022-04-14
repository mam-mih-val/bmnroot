#ifndef BMNSILICONLAYER_H
#define BMNSILICONLAYER_H

#include "Rtypes.h"

#include "BmnMatch.h"
#include "BmnStripData.h"

#include <vector>

using namespace std;

class BmnSiliconLayer {

public:

    //Constructors -------------------------------------------------------------
    BmnSiliconLayer();

    BmnSiliconLayer(Int_t zone_id,
                    StripLayerType layer_type, Int_t first_strip_number,
                    Double_t xsize, Double_t ysize,
                    Double_t xorig, Double_t yorig,
                    Double_t pitch, Double_t adeg);
    //--------------------------------------------------------------------------

    //Destructor
    virtual ~BmnSiliconLayer();

    void InitializeLayer();
    void ResetLayer(); //clear all data and rebuild the layer in accordance with new parameters

    Int_t GetNStrips();

    void SetVerbosity(Bool_t verb) { Verbosity = verb; }
    void SetZoneID(Int_t zone_id) { ZoneID = zone_id; }
    void SetFirstStripNumber(Int_t first_strip_number) { FirstStripNumber = first_strip_number; }
    void SetPitch(Double_t pitch);
    void SetLayerSizes(Double_t xsize, Double_t ysize, Double_t xorig=0.0, Double_t yorig=0.0);
    void SetAngleDeg(Double_t deg); // plus - clockwise from vertical

    Bool_t GetVerbosity() { return Verbosity; }
    Int_t GetZoneID() { return ZoneID; }
    Int_t GetFirstStripNumber() { return FirstStripNumber; }
    Int_t GetLastStripNumber() { return (GetNStrips()+FirstStripNumber) - 1; }
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
    Bool_t SetStripMatch(Int_t strip_num, BmnMatch mc_match); // set a MC-match for a strip
    Bool_t AddLinkToStripMatch(Int_t strip_num, Double_t weight, Int_t mc_num); // add a mc-number to a MC-match

    Bool_t SetStripDigitNumberMatch(Int_t strip_num, BmnMatch digit_num_match); // set a digit number match for a strip
    Bool_t AddLinkToStripDigitNumberMatch(Int_t strip_num, Double_t weight, Int_t digit_num); // add a digit number match for a strip

    BmnMatch GetStripMatch(Int_t strip_num); //mc-match for a strip
    BmnMatch GetStripDigitNumberMatch(Int_t strip_num); //digit number match for a strip

    void ResetStripMatches();
    void ResetStripDigitNumberMatches();
    //--------------------------------------------------------------------------

    //Strip hits ---------------------------------------------------------------
    Int_t GetNStripHits() { return StripHits.size(); } //number of hits at the layer
    Double_t GetStripHitPos(Int_t num); //hit position
    Double_t GetStripHitTotalSignal(Int_t num); //sum signal of the hit
    Double_t GetStripHitError(Int_t num); //hit error
    Int_t GetStripHitClusterSize(Int_t num); //number of strips in the cluster for the num-hit
    void ResetStripHits();
    //--------------------------------------------------------------------------

    //Strip Clusters -----------------------------------------------------------
    vector<StripCluster> GetStripClusters() { return StripClusters; };
    static void SetLowerUniqueID(Int_t id) { fUniqueIdL = id; }
    static void SetUpperUniqueID(Int_t id) { fUniqueIdU = id; }
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

    vector<Double_t> GetSmoothStrips() { return SmoothStrips; } //for test (don't forget about FirstStripNumber when you use it)
    //--------------------------------------------------------------------------

private:
    //BmnSiliconLayer(const BmnSiliconLayer&) = delete;
    //BmnSiliconLayer& operator=(const BmnSiliconLayer&) = delete;

private:
    Bool_t Verbosity;

    Int_t ZoneID; // zone id-number to recognize the layers linked together
    Int_t FirstStripNumber; // the number that numbered strips start with
    StripLayerType LayerType; //type of the strip layer: lower or upper

    Double_t Pitch; // as a perpendicular to a strip
    Double_t AngleDeg; //Angle between two sets of parellel strips: angle from vertical
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
    vector<BmnMatch> StripMatches; //MC-point matches for all strips
    vector<BmnMatch> StripDigitNumberMatches; //Digit number matches for all strips

    //Strip hits (It is filled after cluster finding and fitting) --------------
    vector<Double_t> StripHits; //positions of strip hits
    vector<Double_t> StripHitsTotalSignal;
    vector<Double_t> StripHitsErrors;
    vector<Int_t> StripHitsClusterSize; //cluster size (number of strips) for each strip hit
    vector<Double_t> SmoothStrips; //smoothed signal strips (It is filled after smoothing algorithm)
    Double_t ClusterFindingThreshold; //min. threshold cut
    //--------------------------------------------------------------------------

    //Strip Clusters -----------------------------------------------------------
    vector<StripCluster> StripClusters;

    static Int_t fUniqueIdU;
    static Int_t fUniqueIdL;
    //--------------------------------------------------------------------------

    ClassDef(BmnSiliconLayer, 1);
};

//------------------------------------------------------------------------------

#endif /* BMNSILICONLAYER_H */

