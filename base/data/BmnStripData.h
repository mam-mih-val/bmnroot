#ifndef BMNSTRIPDATA_H
#define BMNSTRIPDATA_H

#include "TString.h"

#include <iostream>

enum StripLayerType {LowerStripLayer=0, UpperStripLayer};
enum StripNumberingDirection {LeftToRight, RightToLeft};
enum StripBorderPoint {LeftTop, LeftBottom, RightTop, RightBottom};

//Exeptions --------------------------------------------------------------------
class StationSet_Exception {
public:
    StationSet_Exception(TString message) {
        std::cerr << "StationSet_Exception::" << message << "\n";
    }
};

class Station_Exception {
public:
    Station_Exception(TString message) {
        std::cerr << "Station_Exception::" << message << "\n";
    }
};
//------------------------------------------------------------------------------

//Class: dead zone -------------------------------------------------------------
class DeadZoneOfStripLayer {
private:
    Int_t NPoints;
    Double_t *XPoints;  //[NPoints]
    Double_t *YPoints;  //[NPoints]

public:
    DeadZoneOfStripLayer() : NPoints(0), XPoints(nullptr), YPoints(nullptr) { }

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
        if(XPoints != nullptr) delete [] XPoints;
        if(YPoints != nullptr) delete [] YPoints;
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
        if(XPoints != nullptr) delete [] XPoints;
        if(YPoints != nullptr) delete [] YPoints;
    }

    //Set a new dead zone, the previous zone will be deleted
    Bool_t SetDeadZone(Int_t n_points, Double_t *xpoints, Double_t *ypoints) {
        NPoints = 0;
        if(XPoints != nullptr) {
            delete [] XPoints;
            XPoints = nullptr;
        }
        if(YPoints != nullptr) {
            delete [] YPoints;
            YPoints = nullptr;
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
class StripCluster : public TObject {

public:
    Double_t OriginPosition; //origin position of the center point
    Double_t MeanPosition; //position of the cluster (after fitting)
    Double_t TotalSignal; //total signal of the cluster
    Double_t PositionResidual; // residual from the origin position
    Bool_t IsCorrect; //correct or incorrect cluster (status)
    Int_t Station;
    Int_t Module;
    Int_t Width;

    //Cluster type: 0 - lower, 1 - upper
    Int_t fType;

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
        fType = -1;
        Station = -1;
        Module = -1;
        Width = 0;
    }

    void SetType(Int_t type) { fType = type; }
    Int_t GetType() { return fType; }
    Int_t GetWidth() { return Width; }
    void SetWidth(Int_t w) { Width = w; }
    Int_t GetStation() { return Station; }
    void SetStation(Int_t st) { Station = st; }
    Int_t GetModule() { return Module; }
    void  SetModule(Int_t m) { Module = m; }

    StripCluster(Double_t orig_position, Double_t mean_position, Double_t total_signal) : OriginPosition(orig_position), MeanPosition(mean_position), TotalSignal(total_signal) {
        Strips.clear();
        Signals.clear();

        PositionResidual = 0.0;
        IsCorrect = kFALSE;
    }
    //Add new strip in ascending order of strip number (if cluster has strip with such number - add signal to that strip)
    void AddStrip(Int_t strip_num, Double_t strip_signal) {
        Bool_t StripNumExists = false;
        size_t index;
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
            for(size_t i = 0; i < Strips.size(); ++i) {
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
    
    void Print() {
        printf("\nCLUSTER INFORMATION (#%d)\n", this->GetUniqueID());
        if (fType == 0) printf("TYPE: LOWER\n");
        else if (fType == 1) printf("TYPE: UPPER\n");
        printf("Station: %d\n", Station);
        printf("Module: %d\n", Module);
        printf("Cluster width: %d\n", Width);
        printf("Total cluster signal: %4.2f\n", TotalSignal);
        printf("Strips: ");
        for (auto it : Strips)
            printf("%d ", it);
        printf("\nSignals: ");
        for (auto it : Signals)
            printf("%4.2f ", it);
        printf("\n");
    }

    void Clear() {
        Strips.clear();
        Signals.clear();
        OriginPosition = 0.0;
        MeanPosition = 0.0;
        TotalSignal = 0.0;
        PositionResidual = 0.0;
        IsCorrect = kFALSE;
        fType = -1;
    }
    Int_t GetClusterSize() { return Strips.size(); }
    ClassDef(StripCluster,1);
};
ClassImp(StripCluster);
//------------------------------------------------------------------------------

#endif /* BMNSTRIPDATA_H */

