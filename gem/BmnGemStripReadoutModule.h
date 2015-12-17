#ifndef BMNGEMSTRIPREADOUTMODULE_H
#define	BMNGEMSTRIPREADOUTMODULE_H

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

struct ClusterParameters;

struct DeadStripZone {
    Double_t Xmin;
    Double_t Xmax;
    Double_t Ymin;
    Double_t Ymax;

    DeadStripZone() : Xmin(0), Xmax(0), Ymin(0), Ymax(0) {}
    DeadStripZone(Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) : Xmin(xmin), Xmax(xmax), Ymin(ymin), Ymax(ymax) {}

    Bool_t IsInside(Double_t x, Double_t y) {
        if(Xmin == Xmax || Ymin == Ymax) return false; //incorrect dead zone
        if(x>=Xmin && x<=Xmax && y>=Ymin && y<=Ymax) return true;
        else return false;
    }
};

class BmnGemStripReadoutModule {

public:
//Constructors
    BmnGemStripReadoutModule();
    BmnGemStripReadoutModule(Double_t xsize, Double_t ysize,
                        Double_t xorig, Double_t yorig,
                        Double_t pitch, Double_t adeg,
                        Double_t low_strip_width, Double_t up_strip_width,
                        Double_t zpos_module);
//Destructor
    virtual ~BmnGemStripReadoutModule();

//Resetters
    void CreateReadoutPlanes();
    void RebuildReadoutPlanes();
    void ResetIntersectionPoints();
    void ResetRealPoints();
    void ResetStripHits();

//Parameter setters
    void SetVerbosity(Bool_t verb) { Verbosity = verb; }
    void SetPitch(Double_t pitch); //cm
    void SetStripWidths(Double_t low_strip_width, Double_t up_strip_width);
    void SetReadoutSizes(Double_t xsize, Double_t ysize, Double_t xorig=0.0, Double_t yorig=0.0);
    Bool_t SetDeadZone(Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax);
    void SetAngleDeg(Double_t deg); //minis - clockwise, plus - anticlockwise
    void SetZPosition(Double_t zpos_module) { ZReadoutModulePosition = zpos_module; }
    void SetAvalancheRadius(Double_t aval_radius) { AvalancheRadius = aval_radius; }
    void SetMeanCollisionDistance(Double_t mcd) { MCD = mcd; }
    void SetDriftGap(Double_t drift_gap) { DriftGap = drift_gap; }
    void SetGain(Double_t gain) { Gain = gain; }
    void SetDistortion(Double_t distortion);  //example: 0.1 is equal 10%

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
    Double_t GetXMinDeadZone() { return DeadZone.Xmin; }
    Double_t GetXMaxDeadZone() { return DeadZone.Xmax; }
    Double_t GetYMinDeadZone() { return DeadZone.Ymin; }
    Double_t GetYMaxDeadZone() { return DeadZone.Ymax; }
    Double_t GetZPositionReadout() { return ZReadoutModulePosition; }
    Double_t GetAvalancheRadius() { return AvalancheRadius; }
    Double_t GetMeanCollisionDistance() { return MCD; }
    Double_t GetDriftGap() { return DriftGap; }
    Double_t GetGain() { return Gain; }
    Double_t GetDistortion() { return SignalDistortion; }
    Double_t GetXStripsIntersectionSize();
    Double_t GetYStripsIntersectionSize();
    Double_t GetXErrorIntersection();
    Double_t GetYErrorIntersection();
    Int_t GetMaxValidTheoreticalIntersections() { return NMaxValidTheoreticalIntersections; }

//Interface methods for adding points
    Bool_t AddRealPoint(Double_t x, Double_t y, Double_t z, Double_t signal); //old
    Bool_t AddRealPointFull(Double_t x, Double_t y, Double_t z,
                            Double_t px, Double_t py, Double_t pz, Double_t signal);

    Bool_t AddRealPointFullOne(Double_t x, Double_t y, Double_t z, Double_t signal);

//Interface methods for calculating intersections points
    void CalculateStripHitIntersectionPoints();
    void CalculateMiddleIntersectionPoints();
    void CalculateLeftIntersectionPoints();
    void CalculateRightIntersectionPoints();
    void CalculateBorderIntersectionPoints();

//Value setters
    //Strips
    Bool_t SetValueOfLowerStrip(Int_t indx, Double_t val);
    Bool_t SetValueOfUpperStrip(Int_t indx, Double_t val);

//Value getters
    //Strips
    Int_t CountLowerStrips(); //strip quantity in the lower layer
    Int_t CountUpperStrips(); //strip quantity in the upper layer
    Double_t GetValueOfLowerStrip(Int_t indx); //signal value of i-strips in lower layer
    Double_t GetValueOfUpperStrip(Int_t indx); //signal value of i-strips in upper layer

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
    //Int_t GetNDubbedPoints() { return NDubbedPoints; }

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

    //Strip hits
    Int_t GetNLowerStripHits() { return LowerStripHits.size(); } //quatity of hits at the lower layer
    Int_t GetNUpperStripHits() { return UpperStripHits.size(); } //quatity of hits at the upper layer
    Double_t GetLowerStripHitPos(Int_t num); //hit position at the lower layer
    Double_t GetUpperStripHitPos(Int_t num); //hit position at the upper layer
    Double_t GetLowerStripHitTotalSignal(Int_t num); //sum signal of lower hit
    Double_t GetUpperStripHitTotalSignal(Int_t num); //sum signal of upper hit


//Inner methods
public: //private (public - for test)

    //Make cluster from a single point (spread)
    ClusterParameters MakeCluster(TString layer, Double_t xcoord, Double_t ycoord, Double_t signal, Double_t radius);

    //Find clusters and hits
    void FindClustersInLayer(vector<Double_t> &StripLayer, vector<Double_t> &StripHits, vector<Double_t> &StripHitsTotalSignal, vector<Double_t> &StripHitsErrors);
    void MakeStripHit(vector<Int_t> &clusterDigits, vector<Double_t> &clusterValues, vector<Double_t> &Strips, vector<Double_t> &StripHits, vector<Double_t> &StripHitsTotalSignal, vector<Double_t> &StripHitsErrors, Int_t &curcnt);

    Double_t ConvertRealPointToUpperX(Double_t xcoord, Double_t ycoord);
    Double_t ConvertRealPointToUpperY(Double_t xcoord, Double_t ycoord);

    Int_t ConvertRealPointToLowerStripNum(Double_t xcoord, Double_t ycoord);
    Int_t ConvertRealPointToUpperStripNum(Double_t xcoord, Double_t ycoord);

    Double_t CalculateLowerStripZonePosition(Double_t xcoord, Double_t ycoord);
    Double_t CalculateUpperStripZonePosition(Double_t xcoord, Double_t ycoord);

    Double_t FindXMiddleIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip=0);
    Double_t FindYMiddleIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip);

    Double_t FindXLeftIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip=0);
    Double_t FindXRightIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip=0);

    Double_t FindYLowIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip);
    Double_t FindYHighIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip);

    Double_t FindXHitIntersectionPoint(Double_t LowerStripZonePos, Double_t UpperStripZonePos=0);
    Double_t FindYHitIntersectionPoint(Double_t LowerStripZonePos, Double_t UpperStripZonePos);


//Data-members
private:
    Bool_t Verbosity;
    Double_t Pitch;
    Double_t LowerStripWidth;
    Double_t UpperStripWidth;
    Double_t AngleDeg; //Angle between two sets of parellel strip
    Double_t AngleRad;

    Double_t XMinReadout;
    Double_t XMaxReadout;
    Double_t YMinReadout;
    Double_t YMaxReadout;

    DeadStripZone DeadZone;

    Double_t ZReadoutModulePosition;

    Double_t AvalancheRadius;
    Double_t MCD; //Mean collision distance
    Double_t DriftGap;
    Double_t InductionGap;
    Double_t Gain;
    Double_t SignalDistortion;

    vector<Double_t> ReadoutLowerPlane;
    vector<Double_t> ReadoutUpperPlane;

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

    //Int_t NDubbedPoints;
    Int_t NMaxValidTheoreticalIntersections;

    //for hits
    vector<Double_t> LowerStripHits;
    vector<Double_t> UpperStripHits;
    vector<Double_t> LowerStripHitsTotalSignal;
    vector<Double_t> UpperStripHitsTotalSignal;
    vector<Double_t> LowerStripHitsErrors;
    vector<Double_t> UpperStripHitsErrors;

private:
    BmnGemStripReadoutModule(const BmnGemStripReadoutModule&);
    BmnGemStripReadoutModule& operator=(const BmnGemStripReadoutModule&);

    ClassDef(BmnGemStripReadoutModule, 1);
};
//------------------------------------------------------------------------------

struct ClusterParameters {
    Double_t OriginPosition; //origin position of the center point
    Double_t MeanPosition; //position of the cluster (after fitting)
    Double_t TotalSignal; //total signal of the cluster
    Double_t PositionResidual; // residual from origin position
    Bool_t IsCorrect; //correct or incorrect cluster

    vector<Int_t> Strips;
    vector<Double_t> Signals;

    ClusterParameters(Double_t orig_position, Double_t mean_position, Double_t total_signal) : OriginPosition(orig_position), MeanPosition(mean_position), TotalSignal(total_signal) {
        Strips.clear();
        Signals.clear();

        PositionResidual = 0.0;
        IsCorrect = kFALSE;
    }
};

#endif	/* BMNGEMSTRIPREADOUTMODULE_H */

