#include <fstream>

#include "BmnGemStripReadoutModule.h"

#include "TCanvas.h"
#include "TGraph.h"
#include "TStyle.h"

BmnGemStripReadoutModule::BmnGemStripReadoutModule() {
    Verbosity = kTRUE;

    Pitch = 400*1E-4;           //cm
    LowerStripWidth = Pitch;  //cm
    UpperStripWidth = Pitch;   //cm

    AngleDeg = -15.0;               //in degrees (clockwise is minus)
    AngleRad = AngleDeg*Pi()/180;   //in radians

    XMinReadout = 0.0;
    XMaxReadout = 4.0;
    YMinReadout = 0.0;
    YMaxReadout = 2.0;

    ZReadoutModulePosition = 0.0;

    AvalancheRadius = 0.1; //cm
    MCD = 0.0264; //cm
    Gain = 1.0; //gain level
    DriftGap = 0.3; //cm
    InductionGap = 0.15; //cm
    SignalDistortion = 0.0;

    CreateReadoutPlanes();
}

BmnGemStripReadoutModule::BmnGemStripReadoutModule(Double_t xsize, Double_t ysize,
                                       Double_t xorig, Double_t yorig,
                                       Double_t pitch, Double_t adeg,
                                       Double_t low_strip_width, Double_t up_strip_width,
                                       Double_t zpos_module) {
    Verbosity = kTRUE;

    XMinReadout = xorig;
    XMaxReadout = xorig + xsize;
    YMinReadout = yorig;
    YMaxReadout = yorig + ysize;

    Pitch = pitch;
    LowerStripWidth = low_strip_width;
    UpperStripWidth = up_strip_width;
    AngleDeg = adeg;
    AngleRad = AngleDeg*Pi()/180;

    ZReadoutModulePosition = zpos_module;

    AvalancheRadius = 0.1; //cm
    MCD = 0.0264; //cm
    Gain = 1.0; //gain level
    DriftGap = 0.3; //cm
    InductionGap = 0.2; //cm
    SignalDistortion = 0.0;

    CreateReadoutPlanes();
}

BmnGemStripReadoutModule::~BmnGemStripReadoutModule() {

}

void BmnGemStripReadoutModule::CreateReadoutPlanes() {
    Int_t NLowerStrips = CountLowerStrips();
    Int_t NUpperStrips = CountUpperStrips();

    Bool_t CalcTheoreticalAtFirst = kFALSE;
    if( CalcTheoreticalAtFirst ) {
       //Calculation NMaxValidTheoriticalIntersections
       ReadoutLowerPlane.clear();
       ReadoutUpperPlane.clear();
       ReadoutLowerPlane.resize(NLowerStrips, 1);
       ReadoutUpperPlane.resize(NUpperStrips, 1);
       CalculateBorderIntersectionPoints();
       NMaxValidTheoreticalIntersections = GetNIntersectionPoints();
    }
    else {
        NMaxValidTheoreticalIntersections = 0;
    }

    ReadoutLowerPlane.clear();
    ReadoutUpperPlane.clear();
    ReadoutLowerPlane.resize(NLowerStrips, 0.0);
    ReadoutUpperPlane.resize(NUpperStrips, 0.0);

    ResetIntersectionPoints();
    ResetRealPoints();

    ResetStripHits();
}

void BmnGemStripReadoutModule::RebuildReadoutPlanes() {
    CreateReadoutPlanes();
}

void BmnGemStripReadoutModule::ResetIntersectionPoints() {
    IntersectionPointsX.clear();
    IntersectionPointsY.clear();

    IntersectionPointsLowerStripPos.clear();
    IntersectionPointsUpperStripPos.clear();

    IntersectionPointsLowerTotalSignal.clear();
    IntersectionPointsUpperTotalSignal.clear();

    IntersectionPointsXErrors.clear();
    IntersectionPointsYErrors.clear();
}

void BmnGemStripReadoutModule::ResetRealPoints() {
    RealPointsX.clear();
    RealPointsY.clear();

    RealPointsLowerStripPos.clear();
    RealPointsUpperStripPos.clear();

    RealPointsLowerTotalSignal.clear();
    RealPointsUpperTotalSignal.clear();

    //NDubbedPoints = 0;
}

void BmnGemStripReadoutModule::ResetStripHits() {
    LowerStripHits.clear();
    UpperStripHits.clear();
    LowerStripHitsTotalSignal.clear();
    UpperStripHitsTotalSignal.clear();
    LowerStripHitsErrors.clear();
    UpperStripHitsErrors.clear();
}

void BmnGemStripReadoutModule::SetPitch(Double_t pitch) {
    Pitch = pitch;

    RebuildReadoutPlanes();
}

void BmnGemStripReadoutModule::SetStripWidths(Double_t low_strip_width, Double_t up_strip_width) {
    LowerStripWidth = low_strip_width;
    UpperStripWidth = up_strip_width;

    RebuildReadoutPlanes();
}

void BmnGemStripReadoutModule::SetReadoutSizes(Double_t xsize, Double_t ysize, Double_t xorig, Double_t yorig) {
    XMinReadout = xorig;
    XMaxReadout = xorig + xsize;
    YMinReadout = yorig;
    YMaxReadout = yorig + ysize;

    RebuildReadoutPlanes();
}

Bool_t BmnGemStripReadoutModule::SetDeadZone(Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    if((xmax - xmin) >= 0 && (ymax - ymin) >=0) {
        DeadZone.Xmin = xmin;
        DeadZone.Xmax = xmax;
        DeadZone.Ymin = ymin;
        DeadZone.Ymax = ymax;
        return true;
    }
    else return false;
}

void BmnGemStripReadoutModule::SetAngleDeg(Double_t deg) {
    if(Abs(deg) <= 90.0) {
         AngleDeg = deg;
    }
    else {
        AngleDeg = 0.0;
    }

    AngleRad = AngleDeg*Pi()/180;

    RebuildReadoutPlanes();
}

void BmnGemStripReadoutModule::SetDistortion(Double_t distortion) {
    if( distortion >= 0.0 && distortion <= 1.0 ) SignalDistortion = distortion;
    else SignalDistortion = 0.0;
}

Double_t BmnGemStripReadoutModule::GetXStripsIntersectionSize() {
    return LowerStripWidth;
}

Double_t BmnGemStripReadoutModule::GetYStripsIntersectionSize() {

    Double_t rw = LowerStripWidth*Tan(PiOver2()-Abs(AngleRad));
    Double_t lw = (Sin(Abs(AngleRad)));
    if(lw == 0) lw = 1E-10;
    lw = UpperStripWidth/lw;
    return rw + lw;

}

Double_t BmnGemStripReadoutModule::GetXErrorIntersection() {
    return LowerStripWidth/2;
}

Double_t BmnGemStripReadoutModule::GetYErrorIntersection() {
/*  //if X coord had been concrete value Y coord would has been:
    Double_t lw = (Sin(Abs(AngleRad)));
    if(lw == 0) lw = 1E-10;
    return (UpperStripWidth/lw)/2;
 */
   //As X coord has value with error thus Y coord is:
   return GetYStripsIntersectionSize()/2;
}

Bool_t BmnGemStripReadoutModule::AddRealPoint(Double_t x, Double_t y, Double_t z, Double_t signal) {
    if( x >= XMinReadout && x <= XMaxReadout &&
        y >= YMinReadout && y <= YMaxReadout &&
        !DeadZone.IsInside(x, y) ) {

        Int_t numLowStrip = ConvertRealPointToLowerStripNum(x, y);
        Int_t numUpStrip = ConvertRealPointToUpperStripNum(x, y);

        if(numLowStrip < ReadoutLowerPlane.size() && numUpStrip < ReadoutUpperPlane.size()) {
            ReadoutLowerPlane.at(numLowStrip)++;
            ReadoutUpperPlane.at(numUpStrip)++;

            RealPointsX.push_back(x);
            RealPointsY.push_back(y);

            RealPointsLowerStripPos.push_back(numLowStrip);
            RealPointsUpperStripPos.push_back(numUpStrip);

            RealPointsLowerTotalSignal.push_back(signal);
            RealPointsUpperTotalSignal.push_back(signal);
        }
        else return false;
        return true;
    }
    else {
        if(Verbosity) cout << "WARNING: Point (" << x << " : " << y << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

Bool_t BmnGemStripReadoutModule::AddRealPointFull(Double_t x, Double_t y, Double_t z,
                                                  Double_t px, Double_t py, Double_t pz, Double_t signal) {
    if( x >= XMinReadout && x <= XMaxReadout &&
        y >= YMinReadout && y <= YMaxReadout &&
        !DeadZone.IsInside(x, y) ) {

        if(pz == 0) return false;
        if(px == 0 && py == 0) px = 1e-8;

        Double_t zdist = DriftGap - Abs(z-ZReadoutModulePosition);
        if(zdist < 0) {
            if(Verbosity) cout << "WARNING: Point (" << x << " : " << y << " : " << z << ") is out of a drift zone\n";
            return false;
        }

        //Particle direction at the current point
        Double_t dirx = px/Abs(pz);
        Double_t diry = py/Abs(pz);
        Double_t dirz = pz/Abs(pz);

        if(Verbosity) cout << "x = " << x << "\n";
        if(Verbosity) cout << "y = " << y << "\n";
        if(Verbosity) cout << "z = " << z << "\n";

        if(Verbosity) cout << "px = " << px << "\n";
        if(Verbosity) cout << "py = " << py << "\n";
        if(Verbosity) cout << "pz = " << pz << "\n";

        if(Verbosity) cout << "dirx = " << dirx << "\n";
        if(Verbosity) cout << "diry = " << diry << "\n";
        if(Verbosity) cout << "dirz = " << dirz << "\n";

        if(Verbosity) cout << "zdist = " << zdist << "\n";

        Double_t x_out = dirx*zdist + x;
        Double_t y_out = diry*zdist + y;

        //if(x_out < XMinReadout) { x_out = XMinReadout; }
        //if(x_out > XMaxReadout) { x_out = XMaxReadout; }
        //if(y_out < YMinReadout) { y_out = YMinReadout; }
        //if(y_out > YMaxReadout) { y_out = YMaxReadout; }

        if(Verbosity) cout << "x_out = " << x_out << "\n";
        if(Verbosity) cout << "y_out = " << y_out << "\n";
        if(Verbosity) cout << "z_out = " << DriftGap << "\n";

        Double_t dist_track = Sqrt((x_out-x)*(x_out-x) + (y_out-y)*(y_out-y) + zdist*zdist);
        Double_t dist_projXY = Sqrt((x_out-x)*(x_out-x) + (y_out-y)*(y_out-y));

        Double_t dist_projX = Abs(x_out-x);
        Double_t dist_projY = Abs(y_out-y);

        if(Verbosity) cout << "dist_track = " << dist_track << "\n";
        if(Verbosity) cout << "dist_XY = " << dist_projXY << "\n";
        if(Verbosity) cout << "dist_projX = " << dist_projX << "\n";
        if(Verbosity) cout << "dist_projY = " << dist_projY << "\n";

        Int_t NCollisions = (int)(dist_track/MCD);

        if(Verbosity) cout << "NCollisions = " << NCollisions << "\n";

        Double_t xstep = MCD*(dist_projXY/dist_track)*(dist_projX/dist_projXY);
        Double_t ystep = MCD*(dist_projXY/dist_track)*(dist_projY/dist_projXY);

        if(dirx < 0) xstep *= -1.0;
        if(diry < 0) ystep *= -1.0;

        if(Verbosity) cout << "xstep = " << xstep << " (" << MCD << ")" << "\n";
        if(Verbosity) cout << "ystep = " << ystep << " (" << MCD << ")" << "\n";

        Int_t rcoll = 0;
        for(Int_t iColl = 0; iColl < NCollisions+1; iColl++) {
            Double_t xs = x + iColl*xstep;
            Double_t ys = y + iColl*ystep;

            if( (xs < XMinReadout) || (xs > XMaxReadout) || (ys < YMinReadout) || (ys > YMaxReadout) ) break;

            //MakeLowerCluster(xs, ys, signal);
            //MakeUpperCluster(xs, ys, signal);

            if(Verbosity) cout << iColl << ") xys = " << xs << " : " << ys << "\n";
        }
        return true;
    }
    return false;
}

Bool_t BmnGemStripReadoutModule::AddRealPointFullOne(Double_t x, Double_t y, Double_t z, Double_t signal) {
//#define ALIGN_SUM_SIGNAL
#define EQUAL_SIGNAL

    if( x >= XMinReadout && x <= XMaxReadout &&
        y >= YMinReadout && y <= YMaxReadout &&
        !DeadZone.IsInside(x, y) ) {

        if(signal <= 0.0) signal = 1e-16;

        Double_t radius = AvalancheRadius;
        if(radius <= 0.0) return false;

        Int_t cycle_cnt = 0;
        while(true) {
            radius = gRandom->Gaus(AvalancheRadius, AvalancheRadius/6.66);
            if(radius > AvalancheRadius/2.0  && radius < AvalancheRadius*2.0 && radius > 0.0) break;
            cycle_cnt++;

            if(cycle_cnt > 5) {
                radius = AvalancheRadius;
                break;
            }
        }

        ClusterParameters upper_cluster = MakeCluster("upper", x, y, signal, radius);
        if(!upper_cluster.IsCorrect) {
            if(Verbosity)  {
                cout << "WARNING: Incorrect upper cluster for the point (" << x << " : " << y << ")\n";
            }
            return false;
        }

        Double_t remained_signal = signal - upper_cluster.TotalSignal/Gain;
        //Double_t remained_signal = signal;
        ClusterParameters lower_cluster = MakeCluster("lower", x, y, remained_signal, radius);
        if(!lower_cluster.IsCorrect) {
            if(Verbosity)  {
                cout << "WARNING: Incorrect lower cluster for the point (" << x << " : " << y << ")\n";
            }
            return false;
        }

        //add the correct clusters on the readout layers -----------------------
        //upper cluster
        for(Int_t ielement; ielement < upper_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = upper_cluster.Strips.at(ielement);
            Double_t strip_signal = upper_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutUpperPlane.size()) {
                ReadoutUpperPlane.at(strip_num) += strip_signal;
            }
        }
        //lower cluster
        for(Int_t ielement; ielement < lower_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = lower_cluster.Strips.at(ielement);
            Double_t strip_signal = lower_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutLowerPlane.size()) {
                ReadoutLowerPlane.at(strip_num) += strip_signal;
            }
        }
        //----------------------------------------------------------------------

//Signal transformation

        Double_t mean_pos_low_cluster = lower_cluster.MeanPosition;
        Double_t mean_pos_up_cluster = upper_cluster.MeanPosition;
        Double_t total_signal_low_cluster = lower_cluster.TotalSignal;
        Double_t total_signal_up_cluster = upper_cluster.TotalSignal;
        Double_t gained_signal = signal*Gain;

#ifdef ALIGN_SUM_SIGNAL
//Align sum of lower and upper cluster signal with signal*Gain
        Double_t signal_sum = total_signal_low_cluster + total_signal_up_cluster;
        Double_t signal_diff = gained_signal - signal_sum;
        Double_t low_compensated_signal = signal_diff*(total_signal_low_cluster/signal_sum);
        Double_t up_compensated_signal = signal_diff*(total_signal_up_cluster/signal_sum);
        Double_t align_low_coef = low_compensated_signal/total_signal_low_cluster;
        Double_t align_up_coef = up_compensated_signal/total_signal_up_cluster;

        for(Int_t istrip = 0; istrip < lower_cluster.Strips.size(); ++istrip) {
            Int_t strip_num = lower_cluster.Strips.at(istrip);
            Double_t strip_signal = lower_cluster.Signals.at(istrip);
            ReadoutLowerPlane.at(strip_num) -= strip_signal;
            strip_signal += strip_signal*align_low_coef;
            ReadoutLowerPlane.at(strip_num) += strip_signal;
            lower_cluster.Signals.at(istrip) = strip_signal;
            if(ReadoutLowerPlane.at(strip_num) < 0.0) ReadoutLowerPlane.at(strip_num) = 0.0;
        }
        total_signal_low_cluster += low_compensated_signal;
        lower_cluster.TotalSignal = total_signal_low_cluster;

        for(Int_t istrip = 0; istrip < upper_cluster.Strips.size(); ++istrip) {
            Int_t strip_num = upper_cluster.Strips.at(istrip);
            Double_t strip_signal = upper_cluster.Signals.at(istrip);
            ReadoutUpperPlane.at(strip_num) -= strip_signal;
            strip_signal += strip_signal*align_up_coef;
            ReadoutUpperPlane.at(strip_num) += strip_signal;
            upper_cluster.Signals.at(istrip) = strip_signal;
            if(ReadoutUpperPlane.at(strip_num) < 0.0) ReadoutUpperPlane.at(strip_num) = 0.0;
        }
        total_signal_up_cluster += up_compensated_signal;
        upper_cluster.TotalSignal = total_signal_up_cluster;
//------------------------------------------------------------------------------
#endif

#ifdef EQUAL_SIGNAL
//Equalize lower and upper signals
        Double_t equal_low_coef = (gained_signal/2.0)/total_signal_low_cluster;
        Double_t equal_up_coef = (gained_signal/2.0)/total_signal_up_cluster;

        for(Int_t istrip = 0; istrip < lower_cluster.Strips.size(); ++istrip) {
            Int_t strip_num = lower_cluster.Strips.at(istrip);
            Double_t strip_signal = lower_cluster.Signals.at(istrip);
            ReadoutLowerPlane.at(strip_num) -= strip_signal;
            strip_signal *= equal_low_coef;
            ReadoutLowerPlane.at(strip_num) += strip_signal;
            lower_cluster.Signals.at(istrip) = strip_signal;
            if(ReadoutLowerPlane.at(strip_num) < 0.0) ReadoutLowerPlane.at(strip_num) = 0.0;
        }
        total_signal_low_cluster *= equal_low_coef;
        lower_cluster.TotalSignal = total_signal_low_cluster;

        for(Int_t istrip = 0; istrip < upper_cluster.Strips.size(); ++istrip) {
            Int_t strip_num = upper_cluster.Strips.at(istrip);
            Double_t strip_signal = upper_cluster.Signals.at(istrip);
            ReadoutUpperPlane.at(strip_num) -= strip_signal;
            strip_signal *= equal_up_coef;
            ReadoutUpperPlane.at(strip_num) += strip_signal;
            upper_cluster.Signals.at(istrip) = strip_signal;
            if(ReadoutUpperPlane.at(strip_num) < 0.0) ReadoutUpperPlane.at(strip_num) = 0.0;
        }
        total_signal_up_cluster *= equal_up_coef;
        upper_cluster.TotalSignal = total_signal_up_cluster;
//------------------------------------------------------------------------------
#endif

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);

        RealPointsLowerStripPos.push_back(mean_pos_low_cluster);
        RealPointsUpperStripPos.push_back(mean_pos_up_cluster);

        RealPointsLowerTotalSignal.push_back(total_signal_low_cluster);
        RealPointsUpperTotalSignal.push_back(total_signal_up_cluster);

        return true;
    }
    else {
        if(Verbosity) cout << "WARNING: Point (" << x << " : " << y << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

ClusterParameters BmnGemStripReadoutModule::MakeCluster(TString layer, Double_t xcoord, Double_t ycoord, Double_t signal, Double_t radius) {
    //#define DRAW_REAL_CLUSTER_HISTOGRAMS

    ClusterParameters cluster(0.0, 0.0, 0.0);

    if(radius <= 0.0) return cluster;

    Double_t RadiusInZones = radius/Pitch;
    Double_t Sigma = RadiusInZones/3.33;

    TF1 gausF("gausF", "gaus", -4*Sigma, 4*Sigma);
    gausF.SetParameter(0, 1.0); // constant (altitude)
    gausF.SetParameter(1, 0.0); // mean (center position)
    gausF.SetParameter(2, Sigma); //sigma

    Double_t SCluster = gausF.Integral(-4*Sigma, 4*Sigma); //square of the one side distribution (more exactly)

    TRandom rand(0);
    Double_t var_level = SignalDistortion; //signal variation (0.1 is 10%)

    Double_t CenterZonePos;
    Int_t NStripsInLayer = 0;

    if(layer == "lower") {
        CenterZonePos = CalculateLowerStripZonePosition(xcoord, ycoord);
        NStripsInLayer = ReadoutLowerPlane.size();
    }

    if(layer == "upper") {
        CenterZonePos = CalculateUpperStripZonePosition(xcoord, ycoord);
        NStripsInLayer = ReadoutUpperPlane.size();
    }

    if(NStripsInLayer == 0) {
        return cluster;
    }

    gausF.SetParameter(1, CenterZonePos);
    Double_t total_signal = 0.0;

    Double_t LeftZonePos = CenterZonePos - RadiusInZones;
    Double_t RightZonePos = CenterZonePos + RadiusInZones;
    Double_t OutLeftBorder = 0.0;
    Double_t OutRightBorder = 0.0;

    if(LeftZonePos < 0.0)  {
        OutLeftBorder = LeftZonePos;
        LeftZonePos = 0.0;
    }
    if(RightZonePos < 0.0) {
        OutRightBorder = RightZonePos;
        RightZonePos = 0.0;
    }
    if(LeftZonePos >= NStripsInLayer) {
        OutLeftBorder = LeftZonePos;
        LeftZonePos = NStripsInLayer - 0.001;
    }
    if(RightZonePos >= NStripsInLayer) {
        OutRightBorder = RightZonePos;
        RightZonePos = NStripsInLayer - 0.001;
    }

    Double_t h = 0.0;
    Double_t dist = 0.0;

    //avalanche is inside of the one strip
    if((Int_t)LeftZonePos == (Int_t)RightZonePos) {

        Int_t NumCurrentZone = (Int_t) LeftZonePos;

        h = RightZonePos - LeftZonePos;
        if(h < 0.0) h = 0.0;

        Double_t xp = LeftZonePos + dist;
        Double_t S  = gausF.Integral(xp, xp+h);
        Double_t Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;

        if(NumCurrentZone >=0 && NumCurrentZone < NStripsInLayer && Energy > 0.0) {
            cluster.Strips.push_back(NumCurrentZone);
            cluster.Signals.push_back(Energy);
            total_signal += Energy;
        }

        dist += h;

    }
    else {
        //left border strip
        Int_t NumCurrentZone = (Int_t) LeftZonePos;

        h = ((Int_t)LeftZonePos + 1) - LeftZonePos;
        if(h < 0.0) h = 0.0;

        Double_t xp = LeftZonePos + dist;
        Double_t S  = gausF.Integral(xp, xp+h);
        Double_t Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;

        if(NumCurrentZone >=0 && NumCurrentZone < NStripsInLayer && Energy > 0.0) {
            cluster.Strips.push_back(NumCurrentZone);
            cluster.Signals.push_back(Energy);
            total_signal += Energy;
        }

        dist += h;

        //inner strips
        for(Int_t i = (Int_t)LeftZonePos + 1; i < (Int_t)RightZonePos; ++i) {

            NumCurrentZone = i;

            h = 1.0;

            xp = LeftZonePos + dist;
            S  = gausF.Integral(xp, xp+h);
            Energy = (signal*Gain)*(S/SCluster);
            Energy += rand.Gaus(0.0, var_level*Energy);
            if(Energy < 0.0) Energy = 0.0;

            if(NumCurrentZone >=0 && NumCurrentZone < NStripsInLayer && Energy > 0.0) {
                cluster.Strips.push_back(NumCurrentZone);
                cluster.Signals.push_back(Energy);
                total_signal += Energy;
            }

            dist += h;
        }
        //right border strip
        NumCurrentZone = (Int_t)RightZonePos;

        h = RightZonePos - (Int_t)RightZonePos;
        if(h < 0.0) h = 0.0;

        xp = LeftZonePos + dist;
        S  = gausF.Integral(xp, xp+h);
        Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;

        if(NumCurrentZone >=0 && NumCurrentZone < NStripsInLayer && Energy > 0.0) {
            cluster.Strips.push_back(NumCurrentZone);
            cluster.Signals.push_back(Energy);
            total_signal += Energy;
        }

        dist += h;
    }

    if (cluster.Strips.size() <= 0) {
        return cluster;
    }

    //find the mean value of the avalanche position (fitting by gaus function)
    Double_t mean_fit_pos = 0.0;

    Int_t NOutLeftBins = 0;
    Int_t NOutRightBins = 0;
    if(OutLeftBorder != 0.0) {
        NOutLeftBins = (Int_t)(fabs(OutLeftBorder)) + 1;
    }
    if(OutRightBorder != 0.0) {
        NOutRightBins = (Int_t)(OutRightBorder - RightZonePos) + 1;
    }

    Int_t begin_strip_num = cluster.Strips.at(0);
    Int_t last_strip_num = cluster.Strips.at(cluster.Strips.size()-1);
    Int_t nstrips = last_strip_num - begin_strip_num + 1;

    TH1F hist("hist_for_fit", "hist_for_fit", nstrips+NOutLeftBins+NOutRightBins, begin_strip_num-NOutLeftBins, last_strip_num+1+NOutRightBins);
    Int_t hist_index = 0;

    for(Int_t i = 0; i < cluster.Strips.size(); ++i) {
        Double_t value = cluster.Signals.at(i);
        hist.SetBinContent(hist_index+1+NOutLeftBins, value);
        hist_index++;
    }

    //on the left border
    if(NOutLeftBins > 0.0) {
        Double_t first = OutLeftBorder;
        Double_t last = (Int_t)OutLeftBorder;
        Double_t S  = gausF.Integral(first, last);
        Double_t Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;
        Double_t value = Energy;
        hist.SetBinContent(1, value);
        dist = 0.0;

        for(Int_t i = 1; i < NOutLeftBins; ++i) {
            h = 1.0;
            first = (Int_t)OutLeftBorder+dist;
            last = first + h;
            S  = gausF.Integral(first, last);
            Energy = (signal*Gain)*(S/SCluster);
            Energy += rand.Gaus(0.0, var_level*Energy);
            if(Energy < 0.0) Energy = 0.0;
            value = Energy;
            hist.SetBinContent(1+i, value);
            dist += h;
        }
    }

    //on the right border
    if(NOutRightBins > 0.0) {
        dist = 0.0;
        for(Int_t i = hist_index; i < hist_index+NOutRightBins-1; ++i) {
            h = 1.0;
            Double_t first = NStripsInLayer+dist;
            Double_t last = first + h;
            Double_t S  = gausF.Integral(first, last);
            Double_t Energy = (signal*Gain)*(S/SCluster);
            Energy += rand.Gaus(0.0, var_level*Energy);
            if(Energy < 0.0) Energy = 0.0;
            Double_t value = Energy;
            hist.SetBinContent(1+i, value);
            dist += h;
        }

        Double_t first = (Int_t)OutRightBorder;
        Double_t last = first + (OutRightBorder - (Int_t)OutRightBorder);
        Double_t S  = gausF.Integral(first, last);
        Double_t Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;
        Double_t value = Energy;
        hist.SetBinContent(hist_index+NOutRightBins, value);
    }


    TF1* gausFitFunction = 0;
        TString fit_params = "WQ0";

    #ifdef DRAW_REAL_CLUSTER_HISTOGRAMS
        fit_params = "WQ";
    #endif

    if(nstrips > 2) {
        hist.Fit("gaus", fit_params); //Q - quit mode (without information on the screen); 0 - not draw
        gausFitFunction = hist.GetFunction("gaus");
        if(gausFitFunction) {
            mean_fit_pos = gausFitFunction->GetParameter(1);
        }
        else {
            mean_fit_pos = hist.GetMean();
        }
    }
    else {
        mean_fit_pos = hist.GetMean();
    }

    cluster.MeanPosition = mean_fit_pos;
    cluster.TotalSignal = total_signal;
    cluster.PositionResidual = CenterZonePos - mean_fit_pos;

    #ifdef DRAW_REAL_CLUSTER_HISTOGRAMS
    //drawing cluster histograms
    TString hist_fit_title = "";
    hist_fit_title += "sz: "; hist_fit_title += nstrips;
    hist_fit_title += ", mean_fit_pos: "; hist_fit_title += mean_fit_pos;
    hist_fit_title += ", sigma_fit: "; if(gausFitFunction) { hist_fit_title += gausFitFunction->GetParameter(2); } else {  hist_fit_title += "---"; }
    hist_fit_title += ", mean_h "; hist_fit_title += hist.GetMean();
    if(gausFitFunction) { hist_fit_title += ", mean_g:"; hist_fit_title += gausFitFunction->GetParameter(1); }
    hist_fit_title += ", origin_center "; hist_fit_title += CenterZonePos;
    hist.SetTitle(hist_fit_title);
    hist.SetMinimum(0);
    hist.SetFillColor(TColor::GetColor("#ffc0cb"));
    TCanvas canv_fit("canv_fit","canv_fit", 0, 0, 1200, 600);
    hist.Draw();
    TString file_name = "/home/diman/Software/pics_w/test/real_";
    file_name += layer; file_name += "_cluster_";
    file_name += mean_fit_pos; file_name += ".png";
    gPad->GetCanvas()->SaveAs(file_name);
#endif

    cluster.IsCorrect = true; // set correct status of the cluster
    return cluster;
}

ClusterParameters BmnGemStripReadoutModule::MakeLowerCluster(Double_t xcoord, Double_t ycoord,  Double_t signal) {
    //#define DRAW_REAL_LOWER_CLUSTER_HISTOGRAMS
    cout << "====lower\n";
    ClusterParameters cluster(0, 0, 0);

    if( AvalancheRadius <= 0.0 ) AvalancheRadius = 1e-8;

    //Double_t radius = AvalancheRadius;
    Double_t radius = 0.0;
    while(true) {
        radius = gRandom->Gaus(AvalancheRadius, AvalancheRadius/3.0);
        if(radius > AvalancheRadius/2.0 && radius < AvalancheRadius*2.0 && radius > 0.0) break;
    }

    Double_t RadiusInZones = radius/Pitch; //radius in zone units
    Double_t Sigma = RadiusInZones/3.33;

    TF1 gausF("gausF", "gaus", -4*Sigma, 4*Sigma);
    gausF.SetParameter(0, 1.0); // constant (altitude)
    gausF.SetParameter(1, 0.0); // mean (center position)
    gausF.SetParameter(2, Sigma); //sigma

    //Double_t SRadius = gausF(0.0)*RadiusInZones/2; ///rough square of the one side distribution
    Double_t SRadius = gausF.Integral(0.0, 4*Sigma); //square of the one side distribution (more exactly)

    TRandom rand(0);
    Double_t var_level = SignalDistortion; //signal variation (0.1 is 10%)

//Make cluster on lower strips -------------------------------------------------

    Double_t LowerZonePos = CalculateLowerStripZonePosition(xcoord, ycoord);
    Double_t LowerStripFill = LowerStripWidth/Pitch; //fill position of a lower strip

    gausF.SetParameter(1, LowerZonePos);

    Double_t total_signal = 0.0;

    //Processing left radius
    cout << "  left\n";
    Double_t LeftLowerZonePos = LowerZonePos - RadiusInZones;
    if(LeftLowerZonePos < 0) LeftLowerZonePos = 0.0;
    Double_t dist = 0;
    Double_t firstPosInZoneLower = LeftLowerZonePos - (Int_t)LeftLowerZonePos;
    Double_t lastPosInZoneLower = 1.0;

    if((firstPosInZoneLower + RadiusInZones) < 1.0 ) lastPosInZoneLower = firstPosInZoneLower + RadiusInZones;

    while(1) {
        Int_t NumCurrentZone = (Int_t)(LeftLowerZonePos + dist);
        Double_t h = 0; // height of the trap

        if(firstPosInZoneLower <= LowerStripFill) {
            if( lastPosInZoneLower > LowerStripFill ) lastPosInZoneLower = LowerStripFill;

            h = lastPosInZoneLower - firstPosInZoneLower;
            //h = fabs(lastPosInZoneLower) - fabs(firstPosInZoneLower);
            Double_t x = LeftLowerZonePos + dist;
            //Double_t S = (gausF(x) + gausF(x+h))*h/2.0; //rough
            Double_t S = gausF.Integral(x, x+h); //more exactly

            Double_t Energy = (signal*Gain/2)*(S/SRadius); // energy in the segment
            Energy += rand.Gaus(0, var_level*Energy);
            if(Energy < 0) Energy = 0;

            if(NumCurrentZone >=0 && NumCurrentZone < ReadoutLowerPlane.size()) {
                ReadoutLowerPlane.at(NumCurrentZone) += Energy;
                total_signal += Energy;
                cluster.Strips.push_back(NumCurrentZone);
                cluster.Signals.push_back(Energy);
            }
        }
        else {
            lastPosInZoneLower = firstPosInZoneLower;
        }

        dist += h;
        dist += 1.0 - lastPosInZoneLower;

        firstPosInZoneLower = 0.0;
        lastPosInZoneLower = 1.0;

        if((RadiusInZones - dist) < 1.0 ) {
            lastPosInZoneLower = RadiusInZones - dist;
        }

        if(dist >= RadiusInZones) break;
    }

    //Processing right radius
    cout << "  right\n";
    dist = 0;
    firstPosInZoneLower = LowerZonePos - (Int_t)LowerZonePos;
    lastPosInZoneLower = 1.0;
    if((firstPosInZoneLower + RadiusInZones) < 1.0 ) lastPosInZoneLower = firstPosInZoneLower + RadiusInZones;

    while(1) {
        Int_t NumCurrentZone = (Int_t)(LowerZonePos + dist);
        Double_t h = 0;

        if(firstPosInZoneLower <= LowerStripFill) {
            if( lastPosInZoneLower > LowerStripFill ) lastPosInZoneLower = LowerStripFill;

            h = lastPosInZoneLower - firstPosInZoneLower;
            //h = fabs(lastPosInZoneLower) - fabs(firstPosInZoneLower);
            Double_t x = LowerZonePos + dist;
            //Double_t S = (gausF(x) + gausF(x+h))*h/2.0; //rough
            Double_t S = gausF.Integral(x, x+h); //more exactly

            Double_t Energy = (signal*Gain/2)*(S/SRadius);
            Energy += rand.Gaus(0, var_level*Energy);
            if(Energy < 0) Energy = 0;

           if(NumCurrentZone >=0 && NumCurrentZone < ReadoutUpperPlane.size()) {
                cout << "bNumCurrentZone = " << NumCurrentZone << "\n";
                ReadoutUpperPlane.at(NumCurrentZone) += Energy;
                cout << "aNumCurrentZone = " << NumCurrentZone << "\n";
                total_signal += Energy;
                cout << "hui\n";
                if(cluster.Strips.size() > 0) {
                    cout << "pizda\n";
                    if(NumCurrentZone != cluster.Strips.at(cluster.Strips.size()-1)) {
                        cluster.Strips.push_back(NumCurrentZone);
                        cluster.Signals.push_back(Energy);
                        cout << "pizda2\n";
                    }
                    else {
                        cluster.Signals.at(cluster.Strips.size()-1) += Energy;
                        cout << "pizda3\n";
                    }
                }
                else {
                    cluster.Strips.push_back(NumCurrentZone);
                    cluster.Signals.push_back(Energy);
                    cout << "pizda4\n";
                }
                cout << "hui2\n";
            }
        }
        else {
            lastPosInZoneLower = firstPosInZoneLower;
        }
        cout << "hui3\n";
        dist += h;
        dist += 1.0 - lastPosInZoneLower;

        firstPosInZoneLower = 0.0;
        lastPosInZoneLower = 1.0;

        if((RadiusInZones - dist) < 1.0 ) {
            lastPosInZoneLower = RadiusInZones - dist;
        }

        if(dist >= RadiusInZones) break;
    }

//find mean value of avalanche position (fitting by gaus function)
    Double_t mean_fit_pos = 0.0;

    if ( cluster.Strips.size() <= 0 ) return cluster;
    cout << "hui4\n";
    Int_t begin_strip_num = cluster.Strips.at(0);
    Int_t last_strip_num = cluster.Strips.at(cluster.Strips.size()-1);
    Int_t nstrips = last_strip_num - begin_strip_num + 1;
    cout << "hui5\n";
    cout << "begin_strip_num = " << begin_strip_num << "\n";
    cout << "last_strip_num = " << last_strip_num << "\n";
    cout << "nstrips = " << nstrips << "\n";

    TH1F hist("hist_for_fit", "hist_for_fit", nstrips, begin_strip_num, last_strip_num+1);
    Int_t hist_index = 0;

    for(Int_t i = 0; i < cluster.Strips.size(); ++i) {
        Double_t value = cluster.Signals.at(i);
        hist.SetBinContent(hist_index+1, value);
        cout << "  hist_index = " << hist_index << "\n";
        hist_index++;
    }

    TF1* gausFitFunction = 0;
        TString fit_params = "WQ0";

    #ifdef DRAW_REAL_UPPER_CLUSTER_HISTOGRAMS
                fit_params = "WQ";
    #endif

    if(nstrips > 2) {
        hist.Fit("gaus", fit_params); //Q - quit mode (without information on the screen); 0 - not draw
        gausFitFunction = hist.GetFunction("gaus");
        if(gausFitFunction) {
            mean_fit_pos = gausFitFunction->GetParameter(1);
        }
        else {
            mean_fit_pos = hist.GetMean();
        }
    }
    else {
        mean_fit_pos = hist.GetMean();
    }
    cout << "hui6\n";
    cluster.MeanPosition = mean_fit_pos;
    cluster.TotalSignal = total_signal;

#ifdef DRAW_REAL_LOWER_CLUSTER_HISTOGRAMS
    //drawing cluster histograms
    TString hist_fit_title = "";
    hist_fit_title += "sz: "; hist_fit_title += nstrips;
    hist_fit_title += ", mean_fit_pos: "; hist_fit_title += mean_fit_pos;
    hist_fit_title += ", sigma_fit: "; if(gausFitFunction) { hist_fit_title += gausFitFunction->GetParameter(2); } else {  hist_fit_title += "---"; }
    hist_fit_title += ", mean_h "; hist_fit_title += hist.GetMean();
    if(gausFitFunction) { hist_fit_title += ", mean_g:"; hist_fit_title += gausFitFunction->GetParameter(1); }
    hist.SetTitle(hist_fit_title);
    hist.SetMinimum(0);
    hist.SetFillColor(TColor::GetColor("#ffc0cb"));
    TCanvas canv_fit("canv_fit","canv_fit", 0, 0, 1200, 600);
    hist.Draw();
    TString file_name = "/home/diman/Software/pics_w/test/real_low_cluster_";
    file_name += mean_fit_pos; file_name += ".png";
    gPad->GetCanvas()->SaveAs(file_name);
#endif

    cluster.IsCorrect = true;
    cout << "hui7\n";
    return cluster;
}

ClusterParameters BmnGemStripReadoutModule::MakeUpperCluster(Double_t xcoord, Double_t ycoord,  Double_t signal) {
    //#define DRAW_REAL_UPPER_CLUSTER_HISTOGRAMS
    cout << "====upper\n";
    ClusterParameters cluster(0, 0, 0);

    if( AvalancheRadius <= 0.0 ) AvalancheRadius = 1e-8;

    //Double_t radius = AvalancheRadius;
    Double_t radius = 0.0;
    while(true) {
        radius = gRandom->Gaus(AvalancheRadius, AvalancheRadius/3.0);
        if(radius > AvalancheRadius/2.0 && radius < AvalancheRadius*2.0 && radius > 0.0) break;
    }

    Double_t RadiusInZones = radius/Pitch; //radius in zone units
    Double_t Sigma = RadiusInZones/3.33;

    TF1 gausF("gausF", "gaus", -4*Sigma, 4*Sigma);
    gausF.SetParameter(0, 1.0); // constant (altitude)
    gausF.SetParameter(1, 0.0); // mean (center position)
    gausF.SetParameter(2, Sigma); //sigma

    //Double_t SRadius = gausF(0.0)*RadiusInZones/2; ///rough square of the one side distribution
    Double_t SRadius = gausF.Integral(0.0, 4*Sigma); //square of the one side distribution (more exactly)

    TRandom rand(0);
    Double_t var_level = SignalDistortion; //signal variation (0.1 is 10%)

    //Make cluster on upper strips -------------------------------------------------

    Double_t UpperZonePos = CalculateUpperStripZonePosition(xcoord, ycoord);
    Double_t UpperStripFill = UpperStripWidth/Pitch; //fill position of a upper strip

    gausF.SetParameter(1, UpperZonePos);

    Double_t total_signal = 0.0;

    //Processing left radius
    cout << "  left\n";
    Double_t LeftUpperZonePos = UpperZonePos - RadiusInZones;
    if(LeftUpperZonePos < 0) LeftUpperZonePos = 0.0;
    Double_t dist = 0;
    Double_t firstPosInZoneUpper = LeftUpperZonePos - (Int_t)LeftUpperZonePos;
    Double_t lastPosInZoneUpper = 1.0;

    if((firstPosInZoneUpper + RadiusInZones) < 1.0 ) lastPosInZoneUpper = firstPosInZoneUpper + RadiusInZones;

    while(1) {
        Int_t NumCurrentZone = (Int_t)(LeftUpperZonePos + dist);
        Double_t h = 0; // height of the trap

        if(firstPosInZoneUpper <= UpperStripFill) {
            if( lastPosInZoneUpper > UpperStripFill ) lastPosInZoneUpper = UpperStripFill;

            h = lastPosInZoneUpper - firstPosInZoneUpper;
            //h = fabs(lastPosInZoneUpper) - fabs(firstPosInZoneUpper);
            Double_t x = LeftUpperZonePos + dist;
            //Double_t S = (gausF(x) + gausF(x+h))*h/2.0; //rough
            Double_t S = gausF.Integral(x, x+h); //more exactly

            Double_t Energy = (signal*Gain/2)*(S/SRadius); // energy in the segment
            Energy += rand.Gaus(0, var_level*Energy);
            if(Energy < 0) Energy = 0;

        if(NumCurrentZone >=0 && NumCurrentZone < ReadoutUpperPlane.size()) {
                ReadoutUpperPlane.at(NumCurrentZone) += Energy;
                total_signal += Energy;
                cluster.Strips.push_back(NumCurrentZone);
                cluster.Signals.push_back(Energy);
            }
        }
        else {
            lastPosInZoneUpper = firstPosInZoneUpper;
        }

        dist += h;
        dist += 1.0 - lastPosInZoneUpper;

        firstPosInZoneUpper = 0.0;
        lastPosInZoneUpper = 1.0;

        if((RadiusInZones - dist) < 1.0 ) {
            lastPosInZoneUpper = RadiusInZones - dist;
        }

        if(dist >= RadiusInZones) break;
    }

    //Processing right radius
    cout << "  right\n";
    dist = 0;
    firstPosInZoneUpper = UpperZonePos - (Int_t)UpperZonePos;
    lastPosInZoneUpper = 1.0;
    if((firstPosInZoneUpper + RadiusInZones) < 1.0 ) lastPosInZoneUpper = firstPosInZoneUpper + RadiusInZones;

    while(1) {
        Int_t NumCurrentZone = (Int_t)(UpperZonePos + dist);
        Double_t h = 0;

        if(firstPosInZoneUpper <= UpperStripFill) {
            if( lastPosInZoneUpper > UpperStripFill ) lastPosInZoneUpper = UpperStripFill;

            h = lastPosInZoneUpper - firstPosInZoneUpper;
            //h = fabs(lastPosInZoneUpper) - fabs(firstPosInZoneUpper);
            Double_t x = UpperZonePos + dist;
            //Double_t S = (gausF(x) + gausF(x+h))*h/2.0; //rough
            Double_t S = gausF.Integral(x, x+h); //more exactly

            Double_t Energy = (signal*Gain/2)*(S/SRadius);
            Energy += rand.Gaus(0, var_level*Energy);
            if(Energy < 0) Energy = 0;

            if(NumCurrentZone >=0 && NumCurrentZone < ReadoutUpperPlane.size()) {
                cout << "bNumCurrentZone = " << NumCurrentZone << "\n";
                ReadoutUpperPlane.at(NumCurrentZone) += Energy;
                cout << "aNumCurrentZone = " << NumCurrentZone << "\n";
                total_signal += Energy;
                if(cluster.Strips.size() > 0) {
                    if(NumCurrentZone != cluster.Strips.at(cluster.Strips.size()-1)) {
                        cluster.Strips.push_back(NumCurrentZone);
                        cluster.Signals.push_back(Energy);
                    }
                    else {
                        cluster.Signals.at(cluster.Strips.size()-1) += Energy;
                    }
                }
                else {
                    cluster.Strips.push_back(NumCurrentZone);
                    cluster.Signals.push_back(Energy);
                }
            }
        }
        else {
            lastPosInZoneUpper = firstPosInZoneUpper;
        }

        dist += h;
        dist += 1.0 - lastPosInZoneUpper;

        firstPosInZoneUpper = 0.0;
        lastPosInZoneUpper = 1.0;

        if((RadiusInZones - dist) < 1.0 ) {
            lastPosInZoneUpper = RadiusInZones - dist;
        }

        if(dist >= RadiusInZones) break;
    }

//find mean value of avalanche position (fitting by gaus function)
    Double_t mean_fit_pos = 0.0;

    if ( cluster.Strips.size() <= 0 ) return cluster;

    Int_t begin_strip_num = cluster.Strips.at(0);
    Int_t last_strip_num = cluster.Strips.at(cluster.Strips.size()-1);
    Int_t nstrips = last_strip_num - begin_strip_num + 1;

    TH1F hist("hist_for_fit", "hist_for_fit", nstrips, begin_strip_num, last_strip_num+1);
    Int_t hist_index = 0;

    for(Int_t i = 0; i < cluster.Strips.size(); ++i) {
        Double_t value = cluster.Signals.at(i);
        hist.SetBinContent(hist_index+1, value);
        hist_index++;
    }

    TF1* gausFitFunction = 0;
    TString fit_params = "WQ0";

#ifdef DRAW_REAL_UPPER_CLUSTER_HISTOGRAMS
            fit_params = "WQ";
#endif

    if(nstrips > 2) {
        hist.Fit("gaus", fit_params); //Q - quit mode (without information on the screen); 0 - not draw
        gausFitFunction = hist.GetFunction("gaus");
        if(gausFitFunction) {
            mean_fit_pos = gausFitFunction->GetParameter(1);
        }
        else {
            mean_fit_pos = hist.GetMean();
        }
    }
    else {
        mean_fit_pos = hist.GetMean();
    }

    cluster.MeanPosition = mean_fit_pos;
    cluster.TotalSignal = total_signal;

#ifdef DRAW_REAL_UPPER_CLUSTER_HISTOGRAMS
    //drawing cluster histograms
    TString hist_fit_title = "";
    hist_fit_title += "sz: "; hist_fit_title += nstrips;
    hist_fit_title += ", mean_fit_pos: "; hist_fit_title += mean_fit_pos;
    hist_fit_title += ", sigma_fit: "; if(gausFitFunction) { hist_fit_title += gausFitFunction->GetParameter(2); } else {  hist_fit_title += "---"; }
    hist_fit_title += ", mean_h "; hist_fit_title += hist.GetMean();
    if(gausFitFunction) { hist_fit_title += ", mean_g:"; hist_fit_title += gausFitFunction->GetParameter(1); }
    hist.SetTitle(hist_fit_title);
    hist.SetMinimum(0);
    hist.SetFillColor(TColor::GetColor("#b6e1fc"));
    TCanvas canv_fit("canv_fit","canv_fit", 0, 0, 1200, 600);
    hist.Draw();
    TString file_name = "/home/diman/Software/pics_w/test/real_up_cluster_";
    file_name += mean_fit_pos; file_name += ".png";
    gPad->GetCanvas()->SaveAs(file_name);
#endif

    cluster.IsCorrect = true;

    return cluster;
}

void BmnGemStripReadoutModule::FindClustersInLayer(vector<Double_t> &StripLayer, vector<Double_t> &StripHits, vector<Double_t> &StripHitsTotalSignal, vector<Double_t> &StripHitsErrors) {

    Double_t threshold = 0.0;

    vector<Int_t> clusterDigits;
    vector<Double_t> clusterValues;

    Bool_t ascent = false;
    Bool_t descent = false;

//Processing strips
    vector<Double_t> Strips = StripLayer;

    for(Int_t is = 0; is < Strips.size(); is++) {

        if(Strips.at(is) <= threshold) {
            if(descent || ascent) {
                descent = false;
                ascent = false;
                //make strip hit
                MakeStripHit(clusterDigits, clusterValues, Strips, StripHits, StripHitsTotalSignal, StripHitsErrors, is);
            }
            continue;
        }

        if( clusterDigits.size() > 0 ) {
            if( Strips.at(is) >= (clusterValues.at(clusterValues.size()-1)) ) {
                if(descent) {
                    ascent = false;
                    descent = false;
                    //make strip hit
                    MakeStripHit(clusterDigits, clusterValues, Strips, StripHits, StripHitsTotalSignal, StripHitsErrors, is);
                    //continue;
                }
                ascent = true;
                descent = false;
            }
            else {
                ascent = false;
                descent = true;
            }
        }
        else {
            ascent = true;
            descent = false;
        }

        clusterDigits.push_back(is);
        clusterValues.push_back(Strips.at(is));
    }

    if(clusterDigits.size() != 0) {
        //make strip hit
        Int_t lastnum = Strips.size()-1;
        MakeStripHit(clusterDigits, clusterValues, Strips, StripHits, StripHitsTotalSignal, StripHitsErrors, lastnum);
    }
}

void BmnGemStripReadoutModule::MakeStripHit(vector<Int_t> &clusterDigits, vector<Double_t> &clusterValues, vector<Double_t> &Strips, vector<Double_t> &StripHits, vector<Double_t> &StripHitsTotalSignal, vector<Double_t> &StripHitsErrors, Int_t &curcnt) {
    //#define DRAW_FOUND_CLUSTER_HISTOGRAMS

    Double_t total_signal = 0.0;

    //find max strip
    Double_t maxval = 0.0;
    Int_t maxdig = 0;
    for(Int_t i = 0; i < clusterDigits.size(); i++) {
        Double_t signal = clusterValues.at(i);
        if(signal > maxval) {
            maxval = signal;
            maxdig = i;
        }
        total_signal += signal;
    }

    //subtraction from the last bin if the next bin is not zero
    Int_t nextStripNum = clusterDigits.at(clusterDigits.size()-1)+1;
    if(nextStripNum < Strips.size()) {
        if(Strips.at(nextStripNum) > 0.0) {
            /*Double_t diff = 0.0;
            Int_t numObr = 2*maxdig - (clusterValues.size()-1);
            if( numObr >= 0 ) {
                diff = clusterValues.at(clusterValues.size()-1) - clusterValues.at(numObr);
                if(diff > 0) {
                    clusterValues.at(clusterValues.size()-1) -= diff;
                    Strips.at(clusterDigits.at(clusterDigits.size()-1)) = diff;
                    total_signal -= diff;
                }
            }*/

            /*Double_t last_value = clusterValues.at(clusterValues.size()-1);
            clusterValues.at(clusterValues.size()-1) -= last_value/2.0;
            Strips.at(clusterDigits.at(clusterDigits.size()-1)) = last_value/2.0;
            total_signal -= last_value/2.0;*/
        }
    }

    curcnt--;
    if( curcnt < 0 ) curcnt = 0;

    Double_t mean_fit_pos = 0.0;
    Double_t sigma_fit = 0.0;
    Double_t sigma_fit_err = 0.0;

    Int_t begin_strip_num = (Int_t)(clusterDigits.at(0));
    Int_t last_strip_num = (Int_t)(clusterDigits.at(clusterDigits.size()-1));
    Int_t nstrips = last_strip_num - begin_strip_num + 1;

    TH1F hist("hist_for_fit", "hist_for_fit", nstrips, begin_strip_num, last_strip_num+1);
    Int_t hist_index = 0;

    for(Int_t i = 0; i < clusterDigits.size(); ++i) {
        Double_t value = clusterValues.at(i);
        hist.SetBinContent(hist_index+1, value);
        hist_index++;
    }

    TF1* gausFitFunction = 0;
        TString fit_params = "WQ0";

    #ifdef DRAW_REAL_UPPER_CLUSTER_HISTOGRAMS
                fit_params = "WQ";
    #endif

    if(nstrips > 2) {
        hist.Fit("gaus", fit_params); //Q - quit mode (without information on the screen); 0 - not draw
        gausFitFunction = hist.GetFunction("gaus");
        if(gausFitFunction) {
            mean_fit_pos = gausFitFunction->GetParameter(1);
            sigma_fit = gausFitFunction->GetParameter(2);
            //sigma_fit_err = gausFitFunction->GetParError(2); //fix
            sigma_fit_err = sigma_fit;
        }
        else {
            mean_fit_pos = hist.GetMean();
            //sigma_fit = hist.GetRMS();
            sigma_fit = 0.5*nstrips*0.333;
            sigma_fit_err = 0.001;
        }
    }
    else {
        mean_fit_pos = hist.GetMean();
        //sigma_fit = hist.GetRMS();
        sigma_fit = 0.5*nstrips*0.333;
        sigma_fit_err = 0.001;
    }

    StripHits.push_back(mean_fit_pos);
    StripHitsTotalSignal.push_back(total_signal);
    StripHitsErrors.push_back(sigma_fit_err);

    clusterDigits.clear();
    clusterValues.clear();

#ifdef DRAW_FOUND_CLUSTER_HISTOGRAMS
    //drawing cluster histograms
    TString hist_fit_title = "";
    hist_fit_title += "sz: "; hist_fit_title += nstrips;
    hist_fit_title += ", mean_fit_pos: "; hist_fit_title += mean_fit_pos;
    hist_fit_title += ", sigma_fit: "; hist_fit_title += sigma_fit;
    hist_fit_title += ", mean_h "; hist_fit_title += hist.GetMean();
    if(gausFitFunction) { hist_fit_title += ", mean_g:"; hist_fit_title += gausFitFunction->GetParameter(1); }
    hist.SetTitle(hist_fit_title);
    hist.SetMinimum(0);
    hist.SetFillColor(TColor::GetColor("#ffc8a8"));
    TCanvas canv_fit("canv_fit","canv_fit", 0, 0, 1200, 600);
    hist.Draw();
    TRandom rand(0); Int_t rnd = rand.Uniform(1,100);
    TString file_name = "/home/diman/Software/pics_w/test/found_cluster_";
    file_name += mean_fit_pos; file_name += "_"; file_name += rnd; file_name += ".png";
    gPad->GetCanvas()->SaveAs(file_name);
#endif
}

Double_t BmnGemStripReadoutModule::GetLowerStripHitPos(Int_t num) {
    if(num >= 0 && num < LowerStripHits.size()) {
        return LowerStripHits.at(num);
    }
    return -1.0;
}

Double_t BmnGemStripReadoutModule::GetUpperStripHitPos(Int_t num) {
    if(num >= 0 && num < UpperStripHits.size()) {
        return UpperStripHits.at(num);
    }
    return -1.0;
}

Double_t BmnGemStripReadoutModule::GetLowerStripHitTotalSignal(Int_t num) {
    if(num >= 0 && num < LowerStripHits.size()) {
        return LowerStripHitsTotalSignal.at(num);
    }
    return -1.0;
}

Double_t BmnGemStripReadoutModule::GetUpperStripHitTotalSignal(Int_t num) {
     if(num >= 0 && num < UpperStripHits.size()) {
        return UpperStripHitsTotalSignal.at(num);
    }
    return -1.0;
}

Double_t BmnGemStripReadoutModule::ConvertRealPointToUpperX(Double_t xcoord, Double_t ycoord) {
    Double_t XRotationCenter;
    Double_t YRotationCenter;
    if(AngleDeg <= 0 && AngleDeg >= -90) {
        XRotationCenter = XMinReadout;
        YRotationCenter = YMaxReadout;
    }
    else {
        if(AngleDeg > 0 && AngleDeg <= 90.0) {
            XRotationCenter = XMinReadout;
            YRotationCenter = YMinReadout;
        }
    }
    Double_t UpperX = (xcoord - XRotationCenter)*Cos(AngleRad) + (ycoord - YRotationCenter)*Sin(AngleRad) + XRotationCenter;//see
    return UpperX;
}

Double_t BmnGemStripReadoutModule::ConvertRealPointToUpperY(Double_t xcoord, Double_t ycoord) {
    Double_t XRotationCenter;
    Double_t YRotationCenter;
    if(AngleDeg <= 0 && AngleDeg >= -90) {
        XRotationCenter = XMinReadout;
        YRotationCenter = YMaxReadout;
    }
    else {
        if(AngleDeg > 0 && AngleDeg <= 90.0) {
            XRotationCenter = XMinReadout;
            YRotationCenter = YMinReadout;
        }
    }
    Double_t UpperY = -(xcoord - XRotationCenter)*Sin(AngleRad) + (ycoord - YRotationCenter)*Cos(AngleRad) + YRotationCenter;//see
    return UpperY;
}

Int_t BmnGemStripReadoutModule::ConvertRealPointToLowerStripNum(Double_t xcoord, Double_t ycoord) {
    return (int)((xcoord-XMinReadout)/Pitch);
}

Int_t BmnGemStripReadoutModule::ConvertRealPointToUpperStripNum(Double_t xcoord, Double_t ycoord) {
    return (int)((ConvertRealPointToUpperX(xcoord, ycoord)-XMinReadout)/Pitch);
}

Double_t BmnGemStripReadoutModule::CalculateLowerStripZonePosition(Double_t xcoord, Double_t ycoord) {
    //This function returns real(double) value,
    //where integer part - number of zone (lower strip), fractional part - position in this zone (as ratio from begin)
    return (xcoord-XMinReadout)/Pitch;
}

Double_t BmnGemStripReadoutModule::CalculateUpperStripZonePosition(Double_t xcoord, Double_t ycoord) {
    //This function returns real(double) value,
    //where integer part - number of zone (upper strip), fractional part - position in this zone (as ratio from begin)
    return (ConvertRealPointToUpperX(xcoord, ycoord)-XMinReadout)/Pitch;
}

Bool_t BmnGemStripReadoutModule::SetValueOfLowerStrip(Int_t indx, Double_t val) {
    if(indx < ReadoutLowerPlane.size()) {
        ReadoutLowerPlane.at(indx) = val;
        return true;
    }
    else return false;
}

Bool_t BmnGemStripReadoutModule::SetValueOfUpperStrip(Int_t indx, Double_t val) {
    if(indx < ReadoutUpperPlane.size()) {
        ReadoutUpperPlane.at(indx) = val;
        return true;
    }
    else return false;
}

Int_t BmnGemStripReadoutModule::CountLowerStrips(){
    Double_t ratio = (XMaxReadout-XMinReadout)/Pitch;
    if((Abs(ratio) - Abs((int)ratio)) < 1E-10) {
        return ratio;
    }
    else {
        return ratio+1;
    }
}

Int_t BmnGemStripReadoutModule::CountUpperStrips(){
    Double_t ratio;
    if (AngleDeg <= 0 && AngleDeg >=-90.0) {
        ratio = (ConvertRealPointToUpperX(XMaxReadout, YMinReadout) - XMinReadout)/Pitch;

    }
    if (AngleDeg > 0 && AngleDeg <= 90.0){
        ratio = (ConvertRealPointToUpperX(XMaxReadout, YMaxReadout) - XMinReadout)/Pitch;
    }

    if((Abs(ratio) - Abs((int)ratio)) < 1E-10) {
        return ratio;
    }
    else {
        return ratio+1;
    }
}

Double_t BmnGemStripReadoutModule::GetValueOfLowerStrip(Int_t indx) {
    if(indx >= 0 && indx < ReadoutLowerPlane.size()) {
        return ReadoutLowerPlane.at(indx);
    }
    else return -1;
}
Double_t BmnGemStripReadoutModule::GetValueOfUpperStrip(Int_t indx) {
    if(indx >= 0 && indx < ReadoutUpperPlane.size()) {
        return ReadoutUpperPlane.at(indx);
    }
    else return -1;
}

Double_t BmnGemStripReadoutModule::FindXMiddleIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip) {
    return (numLowerStrip*Pitch + Pitch/2) + XMinReadout;
}

Double_t BmnGemStripReadoutModule::FindYMiddleIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip) {
    Double_t xcoord = FindXMiddleIntersectionPoint(numLowerStrip);
    Double_t hypoten = Pitch/Sin(Abs(AngleRad));
    Double_t ycoord;

    if(AngleDeg <= 0 && AngleDeg >=-90.0) {
        ycoord = Tan(AngleRad+PiOver2())*(xcoord-XMinReadout) +  (YMaxReadout - (numUpperStrip*hypoten + hypoten/2));
    }
    if (AngleDeg > 0 && AngleDeg <= 90.0) {
        ycoord = Tan(AngleRad+PiOver2())*(xcoord-XMinReadout) + (YMinReadout + (numUpperStrip*hypoten + hypoten/2));
    }

    return ycoord;
}

Double_t BmnGemStripReadoutModule::FindXLeftIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip) {
    return (numLowerStrip*Pitch) + XMinReadout;
}

Double_t BmnGemStripReadoutModule::FindXRightIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip) {
    return (numLowerStrip*Pitch + Pitch) + XMinReadout;
}

Double_t BmnGemStripReadoutModule::FindYLowIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip) {
    Double_t xcoord;
    Double_t hypoten = Pitch/Sin(Abs(AngleRad));
    Double_t ycoord;

    if(AngleDeg <= 0 && AngleDeg >=-90.0) {
        xcoord = FindXLeftIntersectionPoint(numLowerStrip);
        ycoord = Tan(AngleRad+PiOver2())*(xcoord-XMinReadout) +  (YMaxReadout - (numUpperStrip*hypoten + hypoten));
    }
    if (AngleDeg > 0 && AngleDeg <= 90.0) {
        xcoord = FindXRightIntersectionPoint(numLowerStrip);
        ycoord = Tan(AngleRad+PiOver2())*(xcoord-XMinReadout) + (YMinReadout +  numUpperStrip*hypoten);
    }

    return ycoord;
}

Double_t BmnGemStripReadoutModule::FindYHighIntersectionPoint(Int_t numLowerStrip, Int_t numUpperStrip) {
    Double_t xcoord;
    Double_t hypoten = Pitch/Sin(Abs(AngleRad));
    Double_t ycoord;

    if(AngleDeg <= 0 && AngleDeg >=-90.0) {
        xcoord = FindXRightIntersectionPoint(numLowerStrip);
        ycoord = Tan(AngleRad+PiOver2())*(xcoord-XMinReadout) +  (YMaxReadout - numUpperStrip*hypoten);
    }
    if (AngleDeg > 0 && AngleDeg <= 90.0) {
        xcoord = FindXLeftIntersectionPoint(numLowerStrip);
        ycoord = Tan(AngleRad+PiOver2())*(xcoord-XMinReadout) + (YMinReadout + (numUpperStrip*hypoten + hypoten));
    }

    return ycoord;
}

Double_t BmnGemStripReadoutModule::FindXHitIntersectionPoint(Double_t LowerStripZonePos, Double_t UpperStripZonePos) {
    //find real posision on lower strip
    Int_t low_strip_num = (Int_t)LowerStripZonePos;
    Double_t low_strip_pos = LowerStripZonePos - low_strip_num;
    LowerStripZonePos = low_strip_num + low_strip_pos;

    return (LowerStripZonePos*Pitch) + XMinReadout;
}

Double_t BmnGemStripReadoutModule::FindYHitIntersectionPoint(Double_t LowerStripZonePos, Double_t UpperStripZonePos) {
    Double_t xcoord = FindXHitIntersectionPoint(LowerStripZonePos);
    Double_t hypoten = Pitch/Sin(Abs(AngleRad));
    Double_t ycoord;

    //find real posision on upper strip
    Int_t up_strip_num = (Int_t)UpperStripZonePos;
    Double_t up_strip_pos = UpperStripZonePos - up_strip_num;
    UpperStripZonePos = up_strip_num + up_strip_pos;

    if(AngleDeg <= 0 && AngleDeg >=-90.0) {
        ycoord = Tan(AngleRad+PiOver2())*(xcoord-XMinReadout) +  (YMaxReadout - UpperStripZonePos*hypoten);
        //ycoord = Tan(AngleRad+PiOver2())*(xcoord-XMinReadout) + (YMaxReadout - UpperStripZonePos*Pitch/Sin(Abs(AngleRad))); //or this

    }
    if (AngleDeg > 0 && AngleDeg <= 90.0) {
        ycoord = Tan(AngleRad+PiOver2())*(xcoord-XMinReadout) + (YMinReadout + (UpperStripZonePos*hypoten));
    }

    return ycoord;
}

void BmnGemStripReadoutModule::CalculateStripHitIntersectionPoints() {
    ResetIntersectionPoints();
    ResetStripHits();

    FindClustersInLayer(ReadoutLowerPlane, LowerStripHits, LowerStripHitsTotalSignal, LowerStripHitsErrors);
    FindClustersInLayer(ReadoutUpperPlane, UpperStripHits, UpperStripHitsTotalSignal, UpperStripHitsErrors);

    for(UInt_t i = 0; i < LowerStripHits.size(); ++i) {
        for(UInt_t j = 0; j < UpperStripHits.size(); ++j) {
            Double_t xcoord = FindXHitIntersectionPoint(LowerStripHits.at(i),UpperStripHits.at(j));
            Double_t ycoord = FindYHitIntersectionPoint(LowerStripHits.at(i),UpperStripHits.at(j));

            if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) && !DeadZone.IsInside(xcoord, ycoord) ) {
                IntersectionPointsX.push_back(xcoord);
                IntersectionPointsY.push_back(ycoord);

                IntersectionPointsLowerStripPos.push_back(LowerStripHits.at(i));
                IntersectionPointsUpperStripPos.push_back(UpperStripHits.at(j));

                IntersectionPointsLowerTotalSignal.push_back(LowerStripHitsTotalSignal.at(i));
                IntersectionPointsUpperTotalSignal.push_back(UpperStripHitsTotalSignal.at(j));

                //error correction coefficient to pull sigma ~ 1.0
                Double_t xerr_correct_coef = 1.0;
                Double_t yerr_correct_coef = 1.0;
/*
                if(Pitch == 0.04) {
                    xerr_correct_coef = 0.04;
                    yerr_correct_coef = 0.0065;
                }
                if(Pitch == 0.08) {
                    xerr_correct_coef = 0.09;
                    yerr_correct_coef = 0.028;
                }
*/
                IntersectionPointsXErrors.push_back(LowerStripHitsErrors.at(i)*Pitch/xerr_correct_coef);
                IntersectionPointsYErrors.push_back(UpperStripHitsErrors.at(j)*(Pitch/Sin(Abs(AngleRad)))/yerr_correct_coef); //FIX IT
            }
        }
    }
}

void BmnGemStripReadoutModule::CalculateMiddleIntersectionPoints() {
    ResetIntersectionPoints();
    for(UInt_t i = 0; i < ReadoutLowerPlane.size(); i++) {
        if(ReadoutLowerPlane.at(i)) {
            for(UInt_t j = 0; j < ReadoutUpperPlane.size(); j++) {
                if(ReadoutUpperPlane.at(j)) {
                    Double_t xcoord = FindXMiddleIntersectionPoint(i,j);
                    Double_t ycoord = FindYMiddleIntersectionPoint(i,j);

                    if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) && !DeadZone.IsInside(xcoord, ycoord) )
                    {
                        IntersectionPointsX.push_back(xcoord);
                        IntersectionPointsY.push_back(ycoord);

                        IntersectionPointsLowerStripPos.push_back(i);
                        IntersectionPointsUpperStripPos.push_back(j);

                        IntersectionPointsLowerTotalSignal.push_back(ReadoutLowerPlane.at(i));
                        IntersectionPointsUpperTotalSignal.push_back(ReadoutUpperPlane.at(j));

                        IntersectionPointsXErrors.push_back(0); //FIX IT
                        IntersectionPointsYErrors.push_back(0); //FIX IT
                    }
                }
            }
        }
    }
}

void BmnGemStripReadoutModule::CalculateLeftIntersectionPoints() {
    ResetIntersectionPoints();
    for(UInt_t i = 0; i < ReadoutLowerPlane.size(); i++) {
        if(ReadoutLowerPlane.at(i)) {
            for(UInt_t j = 0; j < ReadoutUpperPlane.size(); j++) {
                if(ReadoutUpperPlane.at(j)) {
                    Double_t xcoord;
                    Double_t ycoord;
                    if(AngleDeg <= 0 && AngleDeg >=-90.0) {
                        xcoord = FindXLeftIntersectionPoint(i,j);
                        ycoord = FindYLowIntersectionPoint(i,j);
                    }
                    if(AngleDeg > 0 && AngleDeg <= 90.0) {
                        xcoord = FindXLeftIntersectionPoint(i,j);
                        ycoord = FindYHighIntersectionPoint(i,j);
                    }

                    if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) && !DeadZone.IsInside(xcoord, ycoord) )
                    {
                        IntersectionPointsX.push_back(xcoord);
                        IntersectionPointsY.push_back(ycoord);

                        IntersectionPointsLowerStripPos.push_back(i);
                        IntersectionPointsUpperStripPos.push_back(j);

                        IntersectionPointsLowerTotalSignal.push_back(ReadoutLowerPlane.at(i));
                        IntersectionPointsUpperTotalSignal.push_back(ReadoutUpperPlane.at(j));

                        IntersectionPointsXErrors.push_back(0); //FIX IT
                        IntersectionPointsYErrors.push_back(0); //FIX IT
                    }
                }
            }
        }
    }
}

void BmnGemStripReadoutModule::CalculateRightIntersectionPoints() {
    ResetIntersectionPoints();
    for(UInt_t i = 0; i < ReadoutLowerPlane.size(); i++) {
        if(ReadoutLowerPlane.at(i)) {
            for(UInt_t j = 0; j < ReadoutUpperPlane.size(); j++) {
                if(ReadoutUpperPlane.at(j)) {
                    Double_t xcoord;
                    Double_t ycoord;
                    if(AngleDeg <= 0 && AngleDeg >=-90.0) {
                        xcoord = FindXRightIntersectionPoint(i,j);
                        ycoord = FindYHighIntersectionPoint(i,j);
                    }
                    if(AngleDeg > 0 && AngleDeg <= 90.0) {
                        xcoord = FindXRightIntersectionPoint(i,j);
                        ycoord = FindYLowIntersectionPoint(i,j);
                    }

                    if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) && !DeadZone.IsInside(xcoord, ycoord) )
                    {
                        IntersectionPointsX.push_back(xcoord);
                        IntersectionPointsY.push_back(ycoord);

                        IntersectionPointsLowerStripPos.push_back(i);
                        IntersectionPointsUpperStripPos.push_back(j);

                        IntersectionPointsLowerTotalSignal.push_back(ReadoutLowerPlane.at(i));
                        IntersectionPointsUpperTotalSignal.push_back(ReadoutUpperPlane.at(j));

                        IntersectionPointsXErrors.push_back(0); //FIX IT
                        IntersectionPointsYErrors.push_back(0); //FIX IT
                    }
                }
            }
        }
    }
}

void BmnGemStripReadoutModule::CalculateBorderIntersectionPoints() {
    ResetIntersectionPoints();
    for(UInt_t i = 0; i < ReadoutLowerPlane.size(); i++) {
        if(ReadoutLowerPlane.at(i)) {
            for(UInt_t j = 0; j < ReadoutUpperPlane.size(); j++) {
                if(ReadoutUpperPlane.at(j)) {
                    Double_t xcoord_min;
                    Double_t ycoord_min;

                    Double_t xcoord_max;
                    Double_t ycoord_max;

                    if(AngleDeg <= 0 && AngleDeg >=-90.0) {
                        xcoord_min = FindXLeftIntersectionPoint(i,j);
                        ycoord_min = FindYLowIntersectionPoint(i,j);
                        xcoord_max = FindXRightIntersectionPoint(i,j);
                        ycoord_max = FindYHighIntersectionPoint(i,j);
                    }
                    if(AngleDeg > 0 && AngleDeg <= 90.0) {
                        xcoord_min = FindXLeftIntersectionPoint(i,j);
                        ycoord_min = FindYLowIntersectionPoint(i,j);
                        xcoord_max = FindXRightIntersectionPoint(i,j);
                        ycoord_max = FindYHighIntersectionPoint(i,j);
                    }
                    Bool_t InYMinBound = false;
                    Bool_t InYMaxBound = false;
                    if( (ycoord_min <= YMaxReadout) && (ycoord_min >= YMinReadout) && (ycoord_min < DeadZone.Ymax) && (ycoord_min > DeadZone.Ymin) ) InYMinBound = true;
                    if( (ycoord_max <= YMaxReadout) && (ycoord_max >= YMinReadout) && (ycoord_max < DeadZone.Ymax) && (ycoord_max > DeadZone.Ymin) ) InYMaxBound = true;

                    if( InYMinBound && InYMaxBound ) {
                        Double_t xcoord_point = (xcoord_min+xcoord_max)/2;
                        Double_t ycoord_point = (ycoord_min+ycoord_max)/2;

                        if( ycoord_point >= YMinReadout && ycoord_point <= YMaxReadout ) {
                            IntersectionPointsX.push_back(xcoord_point);
                            IntersectionPointsY.push_back(ycoord_point);

                            IntersectionPointsLowerStripPos.push_back(i);
                            IntersectionPointsUpperStripPos.push_back(j);

                            IntersectionPointsLowerTotalSignal.push_back(ReadoutLowerPlane.at(i));
                            IntersectionPointsUpperTotalSignal.push_back(ReadoutUpperPlane.at(j));

                            IntersectionPointsXErrors.push_back(0); //FIX IT
                            IntersectionPointsYErrors.push_back(0); //FIX IT
                        }
                    }
                    else {
                        if( InYMinBound ) {
                            Double_t xcoord_point = (xcoord_min+xcoord_max)/2;
                            Double_t ycoord_point = (ycoord_min+ycoord_max)/2;

                            if( ycoord_point >= YMinReadout && ycoord_point <= YMaxReadout ) {
                                IntersectionPointsX.push_back((xcoord_min+xcoord_max)/2);
                                IntersectionPointsY.push_back((ycoord_min+ycoord_max)/2);

                                IntersectionPointsLowerStripPos.push_back(i);
                                IntersectionPointsUpperStripPos.push_back(j);

                                IntersectionPointsLowerTotalSignal.push_back(ReadoutLowerPlane.at(i));
                                IntersectionPointsUpperTotalSignal.push_back(ReadoutUpperPlane.at(j));

                                IntersectionPointsXErrors.push_back(0); //FIX IT
                                IntersectionPointsYErrors.push_back(0); //FIX IT
                            }
                        }
                        if( InYMaxBound ) {
                            Double_t xcoord_point = (xcoord_min+xcoord_max)/2;
                            Double_t ycoord_point = (ycoord_min+ycoord_max)/2;

                            if( ycoord_point >= YMinReadout && ycoord_point <= YMaxReadout ) {
                                IntersectionPointsX.push_back((xcoord_min+xcoord_max)/2);
                                IntersectionPointsY.push_back((ycoord_min+ycoord_max)/2);

                                IntersectionPointsLowerStripPos.push_back(i);
                                IntersectionPointsUpperStripPos.push_back(j);

                                IntersectionPointsLowerTotalSignal.push_back(ReadoutLowerPlane.at(i));
                                IntersectionPointsUpperTotalSignal.push_back(ReadoutUpperPlane.at(j));

                                IntersectionPointsXErrors.push_back(0); //FIX IT
                                IntersectionPointsYErrors.push_back(0); //FIX IT
                            }
                        }
                    }
                }
            }
        }
    }
}

ClassImp(BmnGemStripReadoutModule)