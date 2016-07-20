#ifndef BMNGEMSTRIPREADOUTMODULE_H
#define	BMNGEMSTRIPREADOUTMODULE_H

#include "BmnMatch.h"

#include "Rtypes.h"
#include "TMath.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "TColor.h"

#include <iostream>
#include <vector>

using namespace TMath;
using namespace std;

class StripCluster;
struct CollPoint;
class DeadZoneOfReadoutModule;

enum ElectronDriftDirectionInModule {ForwardZAxisEDrift, BackwardZAxisEDrift};
enum StripNumberingDirection {LeftToRight, RightToLeft};
enum StripBorderPoint {LeftTop, LeftBottom, RightTop, RightBottom};

class BmnGemStripReadoutModule {

public:
//Constructors
    BmnGemStripReadoutModule();
    BmnGemStripReadoutModule(Double_t xsize, Double_t ysize,
                        Double_t xorig, Double_t yorig,
                        Double_t pitch, Double_t adeg,
                        Double_t low_strip_width, Double_t up_strip_width,
                        Double_t zpos_module,
                        ElectronDriftDirectionInModule edrift_direction);
//Destructor
    virtual ~BmnGemStripReadoutModule();

//Resetters
    void CreateReadoutPlanes();
    void RebuildReadoutPlanes(); //clear all data and rebuild the module in accordance with new parameters
    void ResetIntersectionPoints();
    void ResetRealPoints();
    void ResetStripHits();

//Parameter setters
    void SetVerbosity(Bool_t verb) { Verbosity = verb; }
    void SetPitch(Double_t pitch); //cm
    void SetStripWidths(Double_t low_strip_width, Double_t up_strip_width);
    void SetReadoutSizes(Double_t xsize, Double_t ysize, Double_t xorig=0.0, Double_t yorig=0.0);
    void SetAngleDeg(Double_t deg); // plus - clockwise from vertical
    void SetZStartModulePosition(Double_t zpos_module) { ZStartModulePosition = zpos_module; }
    void SetElectronDriftDirection(ElectronDriftDirectionInModule direction) { ElectronDriftDirection = direction; }

    void SetClusterDistortion(Double_t cluster_distortion);  //example: 0.1 is equal 10%
    void SetLandauMPV(Double_t mpv); // setter: MPV (most probably value) of Landau distribution (distribution of energy losses) in keV
    void SetBackgroundNoiseLevel(Double_t background_noise_level); //example: 0.1 is equal 10%
    void SetMinSignalCutThreshold(Double_t min_cut_threshold); //example: 0.1 is equal 10%
    void SetMaxSignalCutThreshold(Double_t max_cut_threshold); //example: 0.1 is equal 10%; value 0 - is not active

    void AddBackgroundNoise(); //Add background noise to strip layers

    void SetStripSignalThreshold(Double_t signal_threshold) { StripSignalThreshold = signal_threshold; }

    //Dead zones methods
    Bool_t AddDeadZone(Int_t n_points, Double_t *x_points, Double_t *y_points);
    Bool_t IsPointInsideDeadZones(Double_t x, Double_t y);
    Bool_t IsPointInsideReadoutModule(Double_t x, Double_t y);
    vector<DeadZoneOfReadoutModule> GetDeadZones() { return DeadZones; }
    void ResetAllDeadZones() { DeadZones.clear(); }

    //Strip numbering methods
    Bool_t SetStripNumberingBorders(Double_t x_left, Double_t y_left, Double_t x_right, Double_t y_right); //or method below
    Bool_t SetStripNumberingBorders(StripBorderPoint left, StripBorderPoint right);
    Bool_t SetStripNumberingOrder(StripNumberingDirection lower_strip_direction, StripNumberingDirection upper_strip_direction);
    StripNumberingDirection GetLowerStripNumberingOrder() { return  LowerStripOrder; }
    StripNumberingDirection GetUpperStripNumberingOrder() { return  UpperStripOrder; }
    Double_t GetXLeftStripBorderPoint() { return XLeftPointOfStripNumbering; }
    Double_t GetYLeftStripBorderPoint() { return YLeftPointOfStripNumbering; }
    Double_t GetXRightStripBorderPoint() { return XRightPointOfStripNumbering; }
    Double_t GetYRightStripBorderPoint() { return YRightPointOfStripNumbering; }

//Parameter getters
    Bool_t GetVerbosity() { return Verbosity; }
    Double_t GetPitch() { return Pitch; }
    Double_t GetLowerStripWidth() { return LowerStripWidth; }
    Double_t GetUpperStripWidth() { return UpperStripWidth; }
    Double_t GetAngleDeg() { return AngleDeg; }
    Double_t GetXSize() { return XMaxReadout-XMinReadout; }
    Double_t GetYSize() { return YMaxReadout-YMinReadout; }
    Double_t GetXMinReadout() { return XMinReadout; }
    Double_t GetXMaxReadout() { return XMaxReadout; }
    Double_t GetYMinReadout() { return YMinReadout; }
    Double_t GetYMaxReadout() { return YMaxReadout; }
    Double_t GetZStartModulePosition() { return ZStartModulePosition; }
    Double_t GetZPositionRegistered() { if(ElectronDriftDirection == ForwardZAxisEDrift) return ZStartModulePosition; else return (ZStartModulePosition+ModuleThickness); } //position for all registered point (hits)
    ElectronDriftDirectionInModule GetElectronDriftDirection() { return ElectronDriftDirection; }
    Double_t GetModuleThickness() { return ModuleThickness; }

    //возможно в дальнейшем придется избавиться от этого дерьма!
    Double_t GetClusterDistortion() { return ClusterDistortion; }
    Double_t GetLandauMPV() { return LandauMPV; }
    Double_t GetBackgroundNoiseLevel() { return BackgroundNoiseLevel; }
    Double_t GetMinSignalCutThreshold() { return MinSignalCutThreshold; }
    Double_t GetMaxSignalCutThreshold() { return MaxSignalCutThreshold; }

    Double_t GetStripSignalThreshold() { return StripSignalThreshold; }

//Interface methods for adding points
    Bool_t AddRealPointSimple(Double_t x, Double_t y, Double_t z,
                              Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID); //old

    Bool_t AddRealPointFull(Double_t x, Double_t y, Double_t z,
                            Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID);

    Bool_t AddRealPointFullOne(Double_t x, Double_t y, Double_t z,
                               Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID);

//Interface methods for calculating intersections points
    void CalculateStripHitIntersectionPoints();

//Value setters
    //Strips
    Bool_t SetValueOfLowerStrip(Int_t indx, Double_t val);
    Bool_t SetValueOfUpperStrip(Int_t indx, Double_t val);

    Bool_t SetMatchOfLowerStrip(Int_t indx, BmnMatch strip_match);
    Bool_t SetMatchOfUpperStrip(Int_t indx, BmnMatch strip_match);

//Value getters
    //Strips
    Int_t CountLowerStrips(); //strip quantity in the lower layer
    Int_t CountUpperStrips(); //strip quantity in the upper layer
    Double_t GetValueOfLowerStrip(Int_t indx); //signal value of i-strips in lower layer
    Double_t GetValueOfUpperStrip(Int_t indx); //signal value of i-strips in upper layer

    BmnMatch GetMatchOfLowerStrip(Int_t indx); //match of i-strips in lower layer
    BmnMatch GetMatchOfUpperStrip(Int_t indx); //match of i-strips in upper layer

    //Added (real) points
    Int_t GetNRealPoints() {return RealPointsX.size();} //quantity of added points
    Double_t GetRealPointX(Int_t indx) { return RealPointsX.at(indx); } //X-coord of i-added point
    Double_t GetRealPointY(Int_t indx) { return RealPointsY.at(indx); } //Y-coord of i-added point
    Int_t GetRealPointLowerStrip(Int_t indx) { return (Int_t)RealPointsLowerStripPos.at(indx); } //lower strip number of i-added point
    Int_t GetRealPointUpperStrip(Int_t indx) { return (Int_t)RealPointsUpperStripPos.at(indx); } //upper strip number of i-added point
    Double_t GetRealPointLowerStripPos(Int_t indx) { return RealPointsLowerStripPos.at(indx); } //lower strip position of i-added point
    Double_t GetRealPointUpperStripPos(Int_t indx) { return RealPointsUpperStripPos.at(indx); } //upper strip position of i-added point
    Double_t GetRealPointsLowerTotalSignal(Int_t indx) { return RealPointsLowerTotalSignal.at(indx); } //sum signal of all lower strips actived by i-added point
    Double_t GetRealPointsUpperTotalSignal(Int_t indx) { return RealPointsUpperTotalSignal.at(indx); } //sum signal of all upper strips actived by i-added point

    //Intersection points
    Int_t GetNIntersectionPoints() {return IntersectionPointsX.size();} //quantity of intersections
    Double_t GetIntersectionPointX(Int_t indx) { return IntersectionPointsX.at(indx); } //X-coord of i-intersection point
    Double_t GetIntersectionPointY(Int_t indx) { return IntersectionPointsY.at(indx); } //Y-coord of i-intersection point
    Int_t GetIntersectionPointLowerStrip(Int_t indx) { return (Int_t)IntersectionPointsLowerStripPos.at(indx); } //Lower strip number of i-intersection point
    Int_t GetIntersectionPointUpperStrip(Int_t indx) { return (Int_t)IntersectionPointsUpperStripPos.at(indx); } //Upper strip number of i-intersection point
    Double_t GetIntersectionPointLowerStripPos(Int_t indx) { return IntersectionPointsLowerStripPos.at(indx); } //Lower strip position of i-intersection point
    Double_t GetIntersectionPointUpperStripPos(Int_t indx) { return IntersectionPointsUpperStripPos.at(indx); } //Upper strip position of i-intersection point
    Double_t GetIntersectionPointsLowerTotalSignal(Int_t indx) { return IntersectionPointsLowerTotalSignal.at(indx); } //sum signal of all lower strips for i-intersection point
    Double_t GetIntersectionPointsUpperTotalSignal(Int_t indx) { return IntersectionPointsUpperTotalSignal.at(indx); } //sum signal of all upper strips for i-intersection point
    Double_t GetIntersectionPointXError(Int_t indx) { return IntersectionPointsXErrors.at(indx); } //X-coord error of i-intersection point
    Double_t GetIntersectionPointYError(Int_t indx) { return IntersectionPointsYErrors.at(indx); } //Y-coord error of i-intersection point
    BmnMatch GetIntersectionPointMatch(Int_t indx) { return IntersectionPointMatches.at(indx); } //Intersection point matches

    //Strip hits
    Int_t GetNLowerStripHits() { return LowerStripHits.size(); } //quatity of hits at the lower layer
    Int_t GetNUpperStripHits() { return UpperStripHits.size(); } //quatity of hits at the upper layer
    Double_t GetLowerStripHitPos(Int_t num); //hit position at the lower layer
    Double_t GetUpperStripHitPos(Int_t num); //hit position at the upper layer
    Double_t GetLowerStripHitTotalSignal(Int_t num); //sum signal of lower hit
    Double_t GetUpperStripHitTotalSignal(Int_t num); //sum signal of upper hit


//Inner methods
public: //private (public - for test)

    //Make cluster from a single point (with smearing)
    StripCluster MakeCluster(TString layer, Double_t xcoord, Double_t ycoord, Double_t signal, Double_t radius);

    //Find clusters and hits
    void FindClustersInLayer(vector<Double_t> &StripLayer, vector<Double_t> &StripHits, vector<Double_t> &StripHitsTotalSignal, vector<Double_t> &StripHitsErrors);
    void MakeStripHit(StripCluster &cluster, vector<Double_t> &Strips, vector<Double_t> &StripHits, vector<Double_t> &StripHitsTotalSignal, vector<Double_t> &StripHitsErrors, Int_t &curcnt);
    void SmoothStripSignal(vector<Double_t>& Strips, Int_t NIterations, Int_t SmoothWindow, Double_t Weight);

    Double_t ConvertRealPointToUpperX(Double_t xcoord, Double_t ycoord);
    Double_t ConvertRealPointToUpperY(Double_t xcoord, Double_t ycoord);

    Double_t CalculateLowerStripZonePosition(Double_t xcoord, Double_t ycoord);
    Double_t CalculateUpperStripZonePosition(Double_t xcoord, Double_t ycoord);

    Double_t FindXHitIntersectionPoint(Double_t LowerStripZonePos, Double_t UpperStripZonePos=0);
    Double_t FindYHitIntersectionPoint(Double_t LowerStripZonePos, Double_t UpperStripZonePos);


//Data-members
private:
    Bool_t Verbosity;
    Double_t Pitch;
    Double_t LowerStripWidth;
    Double_t UpperStripWidth;
    Double_t AngleDeg; //Angle between two sets of parellel strips: angle from vertical
    Double_t AngleRad;

    Double_t XMinReadout;
    Double_t XMaxReadout;
    Double_t YMinReadout;
    Double_t YMaxReadout;

    vector<DeadZoneOfReadoutModule> DeadZones;

    Double_t ZStartModulePosition;

    Double_t DriftGapThickness;
    Double_t FirstTransferGapThickness;
    Double_t SecondTransferGapThickness;
    Double_t InductionGapThickness;
    Double_t ModuleThickness;

    ElectronDriftDirectionInModule ElectronDriftDirection;

    Double_t AvalancheRadius;
    Double_t MCD; //Mean collision distance
    Double_t Gain;

    Double_t ClusterDistortion; //signal noise of maked clusters (%)
    Double_t LandauMPV; //MPV (most probably value) of Landau distribution (distribution of energy losses) in keV
    Double_t BackgroundNoiseLevel; // % of Landau MPV
    Double_t MinSignalCutThreshold; // % of Landau MPV
    Double_t MaxSignalCutThreshold; // % of Landau MPV

    //Strip signal threshold (for finding strip clusters)
    Double_t StripSignalThreshold;

    //Strip arrangement
    StripNumberingDirection LowerStripOrder; //strip numbering order (LeftToRight or RightToLeft)
    StripNumberingDirection UpperStripOrder; //strip numbering order (LeftToRight or RightToLeft)
    Double_t XLeftPointOfStripNumbering;
    Double_t XRightPointOfStripNumbering;
    Double_t YLeftPointOfStripNumbering;
    Double_t YRightPointOfStripNumbering;

    //Strip layers
    vector<Double_t> ReadoutLowerPlane;
    vector<Double_t> ReadoutUpperPlane;

    vector<BmnMatch> LowerStripMatches; //ID-point matches for all lower strips
    vector<BmnMatch> UpperStripMatches; //ID-point matches for all upper strips

    vector<Double_t> RealPointsX;
    vector<Double_t> RealPointsY;
    vector<Double_t> RealPointsLowerStripPos;
    vector<Double_t> RealPointsUpperStripPos;
    vector<Double_t> RealPointsLowerTotalSignal;
    vector<Double_t> RealPointsUpperTotalSignal;

    vector<Double_t> IntersectionPointsX;
    vector<Double_t> IntersectionPointsY;
    vector<Double_t> IntersectionPointsLowerStripPos;
    vector<Double_t> IntersectionPointsUpperStripPos;
    vector<Double_t> IntersectionPointsLowerTotalSignal;
    vector<Double_t> IntersectionPointsUpperTotalSignal;
    vector<Double_t> IntersectionPointsXErrors;
    vector<Double_t> IntersectionPointsYErrors;

    vector<BmnMatch> IntersectionPointMatches;

    //for hits on the strip layers
    vector<Double_t> LowerStripHits;
    vector<Double_t> UpperStripHits;
    vector<Double_t> LowerStripHitsTotalSignal;
    vector<Double_t> UpperStripHitsTotalSignal;
    vector<Double_t> LowerStripHitsErrors;
    vector<Double_t> UpperStripHitsErrors;

private:
    BmnGemStripReadoutModule(const BmnGemStripReadoutModule&);
    BmnGemStripReadoutModule& operator=(const BmnGemStripReadoutModule&);

//testing part
public:
    vector<Double_t> XElectronPos;
    vector<Double_t> YElectronPos;
    vector<Double_t> ElectronSignal;

    vector<StripCluster> LowerAddedClusters; //test
    vector<StripCluster> UpperAddedClusters; //test

    void ResetElectronPointsAndClusters() {
        XElectronPos.clear();
        YElectronPos.clear();
        ElectronSignal.clear();
        LowerAddedClusters.clear();
        UpperAddedClusters.clear();
    }

    ClassDef(BmnGemStripReadoutModule, 1);
};
//------------------------------------------------------------------------------

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
    /*
        //old govnetso
        void AddStrip(Int_t strip_num, Double_t strip_signal) {
        Strips.push_back(strip_num);
        Signals.push_back(strip_signal);
    }*/
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

//Struct: a collision point position (an ionizing cluster position)
struct CollPoint {
    CollPoint(Double_t _x, Double_t _y, Double_t _z) : x(_x), y(_y), z(_z) {}
    Double_t x;
    Double_t y;
    Double_t z;
};

//Class: dead zone -------------------------------------------------------------
class DeadZoneOfReadoutModule {
private:

    Int_t NPoints;
    Double_t *XPoints;
    Double_t *YPoints;

public:
    DeadZoneOfReadoutModule() : NPoints(0), XPoints(0), YPoints(0) { }

    DeadZoneOfReadoutModule(Int_t n_points, Double_t *xpoints, Double_t *ypoints)
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

    DeadZoneOfReadoutModule(const DeadZoneOfReadoutModule& obj) {
        NPoints = obj.NPoints;
        XPoints = new Double_t[NPoints];
        YPoints = new Double_t[NPoints];
        for(Int_t i = 0; i < NPoints; ++i) {
            XPoints[i] = obj.XPoints[i];
            YPoints[i] = obj.YPoints[i];
        }
    }

    DeadZoneOfReadoutModule& operator=(const DeadZoneOfReadoutModule& obj) {
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

    ~DeadZoneOfReadoutModule() {
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
            //cerr << "\nWARNING:DeadZoneOfReadoutModule: out of range (x)\n";
            //throw;
        }
    }

    Double_t GetYPoint(Int_t index) {
        if(index >= 0 && index < NPoints) {
            return YPoints[index];
        }
        else {
            return 0.0;
            //cerr << "\nWARNING:DeadZoneOfReadoutModule: out of range (y)\n";
            //throw;
        }
    }
};
//------------------------------------------------------------------------------

#endif	/* BMNGEMSTRIPREADOUTMODULE_H */

