#ifndef BMNGEMSTRIPLAYER_H
#define	BMNGEMSTRIPLAYER_H

#include "BmnMatch.h"

#include "TMath.h"

#include <iostream>
#include <vector>

using namespace TMath;
using namespace std;

enum StripLayerType {LowerStripLayer=0, UpperStripLayer};
enum StripNumberingDirection {LeftToRight, RightToLeft};
enum StripBorderPoint {LeftTop, LeftBottom, RightTop, RightBottom};
class DeadZoneOfStripLayer;

class StripCluster;

class BmnGemStripLayer {

public:
    //Constructors -------------------------------------------------------------
    BmnGemStripLayer();

    BmnGemStripLayer(Int_t zone_number, StripLayerType layer_type,
                     Double_t xsize, Double_t ysize,
                     Double_t xorig, Double_t yorig,
                     Double_t pitch, Double_t adeg);
    //--------------------------------------------------------------------------

    //Destructor
    virtual ~BmnGemStripLayer();

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
    void ResetStripHits();
    //--------------------------------------------------------------------------

    //Methods to convert point coordinates into strip system -------------------
    Double_t ConvertNormalPointToStripX(Double_t x, Double_t y);
    Double_t ConvertNormalPointToStripY(Double_t x, Double_t y);

    Double_t ConvertPointToStripPosition(Double_t x, Double_t y);

    Double_t CalculateStripEquationB(Double_t strip_pos);
    //--------------------------------------------------------------------------

    //Clustering and hit finding methods ---------------------------------------
    void FindClustersAndStripHits();
    void MakeStripHit(StripCluster &cluster, vector<Double_t> &AnalyzableStrips, Int_t &curcnt);
    void SmoothStripSignal(vector<Double_t>& AnalyzableStrips, Int_t NIterations, Int_t SmoothWindow, Double_t Weight);

    void SetClusterFindingThreshold(Double_t threshold) { ClusterFindingThreshold = threshold; }
    Double_t GetClusterFindingThreshold() { return ClusterFindingThreshold; }

    vector<Double_t> GetSmoothStrips() { return SmoothStrips; } //for test
    //--------------------------------------------------------------------------

private:
    //BmnGemStripLayer(const BmnGemStripLayer&);
    //BmnGemStripLayer& operator=(const BmnGemStripLayer&);

private:
    Bool_t Verbosity;

    Int_t LayerZoneNumber; //zone number of the strip layer
    StripLayerType LayerType; //type of the strip layer: lower or upper

    Double_t Pitch;
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
    vector<BmnMatch> StripMatches; //ID-point matches for all strips

    //Strip hits (It is filled after cluster finding and fitting) --------------
    vector<Double_t> StripHits;
    vector<Double_t> StripHitsTotalSignal;
    vector<Double_t> StripHitsErrors;
    vector<Double_t> SmoothStrips; //smoothed signal strips (It is filled after smoothing algorithm)
    Double_t ClusterFindingThreshold; //min. threshold cut
    //--------------------------------------------------------------------------
};
//------------------------------------------------------------------------------

//Class: dead zone -------------------------------------------------------------
class DeadZoneOfStripLayer {
private:
    Int_t NPoints;
    Double_t *XPoints;
    Double_t *YPoints;

public:
    DeadZoneOfStripLayer() : NPoints(0), XPoints(0), YPoints(0) { }

    DeadZoneOfStripLayer(Int_t n_points, Double_t *xpoints, Double_t *ypoints)
        : NPoints(0), XPoints(0), YPoints(0) {
        if(n_points > 2) {
            NPoints = n_points;
            XPoints = new Double_t[NPoints];
            YPoints = new Double_t[NPoints];
            for(Int_t i = 0; i < NPoints; ++i) {
                XPoints[i] = xpoints[i];
                YPoints[i] = ypoints[i];
            }
        }
    }

    DeadZoneOfStripLayer(const DeadZoneOfStripLayer& obj) {
        NPoints = obj.NPoints;
        XPoints = new Double_t[NPoints];
        YPoints = new Double_t[NPoints];
        for(Int_t i = 0; i < NPoints; ++i) {
            XPoints[i] = obj.XPoints[i];
            YPoints[i] = obj.YPoints[i];
        }
    }

    DeadZoneOfStripLayer& operator=(const DeadZoneOfStripLayer& obj) {
        if(XPoints) delete [] XPoints;
        if(YPoints) delete [] YPoints;
        NPoints = obj.NPoints;
        XPoints = new Double_t[NPoints];
        YPoints = new Double_t[NPoints];
        for(Int_t i = 0; i < NPoints; ++i) {
            XPoints[i] = obj.XPoints[i];
            YPoints[i] = obj.YPoints[i];
        }
        return *this;
    }

    ~DeadZoneOfStripLayer() {
        if(XPoints) delete [] XPoints;
        if(YPoints) delete [] YPoints;
    }

    //Set a new dead zone, the previous zone will be deleted
    Bool_t SetDeadZone(Int_t n_points, Double_t *xpoints, Double_t *ypoints) {
        NPoints = 0;
        if(XPoints) {
            delete [] XPoints;
            XPoints = 0;
        }
        if(YPoints) {
            delete [] YPoints;
            YPoints = 0;
        }
        if(n_points > 2) {
            NPoints = n_points;
            XPoints = new Double_t[NPoints];
            YPoints = new Double_t[NPoints];
            for(Int_t i = 0; i < NPoints; ++i) {
                XPoints[i] = xpoints[i];
                YPoints[i] = ypoints[i];
            }
            return true;
        }
        else {
            return false;
        }
    }

    Bool_t IsInside(Double_t x, Double_t y) {
        //crossing number (count) algorithm
        if(NPoints == 0) return false;
        Bool_t check_flag = false;
        for (int i = 0, j = NPoints-1; i < NPoints; j = i++) {

          if ( (((YPoints[i] <= y) && (y < YPoints[j])) || ((YPoints[j] <= y) && (y < YPoints[i]))) &&
               (x > (XPoints[j] - XPoints[i]) * (y - YPoints[i]) / (YPoints[j] - YPoints[i]) + XPoints[i]) )
              check_flag = !check_flag;
        }
        return check_flag;
    }

    Int_t GetNPoints() { return NPoints; }

    Double_t GetXPoint(Int_t index) {
        if(index >= 0 && index < NPoints) {
            return XPoints[index];
        }
        else {
            return 0.0;
            //cerr << "\nWARNING:DeadZoneOfStripLayer: out of range (x)\n";
            //throw;
        }
    }

    Double_t GetYPoint(Int_t index) {
        if(index >= 0 && index < NPoints) {
            return YPoints[index];
        }
        else {
            return 0.0;
            //cerr << "\nWARNING:DeadZoneOfStripLayer: out of range (y)\n";
            //throw;
        }
    }
};
//------------------------------------------------------------------------------

//Class: strip cluster ---------------------------------------------------------
class StripCluster {

public:
    Double_t OriginPosition; //origin position of the center point
    Double_t MeanPosition; //position of the cluster (after fitting)
    Double_t TotalSignal; //total signal of the cluster
    Double_t PositionResidual; // residual from the origin position
    Bool_t IsCorrect; //correct or incorrect cluster (status)

    vector<Int_t> Strips;
    vector<Double_t> Signals;

    StripCluster() {
        Strips.clear();
        Signals.clear();
        OriginPosition = 0.0;
        MeanPosition = 0.0;
        TotalSignal = 0.0;
        PositionResidual = 0.0;
        IsCorrect = kFALSE;
    }

    StripCluster(Double_t orig_position, Double_t mean_position, Double_t total_signal) : OriginPosition(orig_position), MeanPosition(mean_position), TotalSignal(total_signal) {
        Strips.clear();
        Signals.clear();

        PositionResidual = 0.0;
        IsCorrect = kFALSE;
    }
    //Add new strip in ascending order of strip number (if cluster has strip with such number - add signal to that strip)
    void AddStrip(Int_t strip_num, Double_t strip_signal) {
        Bool_t StripNumExists = false;
        Int_t index;
        for(index = 0; index < Strips.size(); ++index) {
            if(Strips[index] == strip_num) {
                StripNumExists = true;
                break;
            }
        }
        if(StripNumExists) Signals[index] += strip_signal;
        else {
            vector<Int_t>::iterator strip_iter = Strips.end();
            vector<Double_t>::iterator signal_iter = Signals.end();
            for(Int_t i = 0; i < Strips.size(); ++i) {
                if( strip_num < Strips[i] ) {
                    strip_iter = Strips.begin() + i;
                    signal_iter = Signals.begin() + i;
                    break;
                }
            }
            Strips.insert(strip_iter, strip_num);
            Signals.insert(signal_iter, strip_signal);
        }
    }

    void Clear() {
        Strips.clear();
        Signals.clear();
        OriginPosition = 0.0;
        MeanPosition = 0.0;
        TotalSignal = 0.0;
        PositionResidual = 0.0;
        IsCorrect = kFALSE;
    }
    Int_t GetClusterSize() { return Strips.size(); }
};
//------------------------------------------------------------------------------

#endif	/* BMNGEMSTRIPLAYER_H */

