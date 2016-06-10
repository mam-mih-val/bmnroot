#include <fstream>
#include <chrono>
#include <iomanip>

#include "BmnGemStripReadoutModule.h"

#include "TCanvas.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TLine.h"
#include "TLegend.h"
#include "TMarker.h"
#include "TH2F.h"

BmnGemStripReadoutModule::BmnGemStripReadoutModule() {
    Verbosity = kTRUE;

    Pitch = 400*1E-4;           //cm
    LowerStripWidth = Pitch;  //cm
    UpperStripWidth = Pitch;   //cm

    AngleDeg = 15.0;               //in degrees from a vertical line where a plus value is clockwise
    AngleRad = AngleDeg*Pi()/180;  //in radians

    XMinReadout = 0.0;
    XMaxReadout = 4.0;
    YMinReadout = 0.0;
    YMaxReadout = 2.0;

    ZReadoutModulePosition = 0.0;

    AvalancheRadius = 0.10; //cm
    MCD = 0.0264; //cm
    Gain = 1.0; //gain level
    DriftGap = 0.3; //cm
    InductionGap = 0.2; //cm
    ClusterDistortion = 0.0;
    LandauMPV = 1.6; //keV (default)
    BackgroundNoiseLevel = 0.03;
    MinSignalCutThreshold = 0.05;
    MaxSignalCutThreshold = 6.25;

    NGoodHits = 0.0;

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

    AvalancheRadius = 0.10; //cm
    MCD = 0.0264; //cm
    Gain = 1.0; //gain level
    DriftGap = 0.3; //cm
    InductionGap = 0.2; //cm
    ClusterDistortion = 0.0;
    LandauMPV = 1.6; //keV (default)
    BackgroundNoiseLevel = 0.03;
    MinSignalCutThreshold = 0.05;
    MaxSignalCutThreshold = 6.25;

    NGoodHits = 0.0;

    CreateReadoutPlanes();
}

BmnGemStripReadoutModule::~BmnGemStripReadoutModule() {

}

void BmnGemStripReadoutModule::CreateReadoutPlanes() {
    Int_t NLowerStrips = CountLowerStrips();
    Int_t NUpperStrips = CountUpperStrips();

    ReadoutLowerPlane.clear();
    ReadoutUpperPlane.clear();
    ReadoutLowerPlane.resize(NLowerStrips, 0.0);
    ReadoutUpperPlane.resize(NUpperStrips, 0.0);

    //strip match
    LowerStripMatches.clear();
    UpperStripMatches.clear();
    LowerStripMatches.resize(NLowerStrips, BmnMatch());
    UpperStripMatches.resize(NUpperStrips, BmnMatch());

    if(BackgroundNoiseLevel > 0.0) {
        AddBackgroundNoise();
    }

    NGoodHits = 0.0;

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

    IntersectionPointMatches.clear();
}

void BmnGemStripReadoutModule::ResetRealPoints() {
    RealPointsX.clear();
    RealPointsY.clear();

    RealPointsLowerStripPos.clear();
    RealPointsUpperStripPos.clear();

    RealPointsLowerTotalSignal.clear();
    RealPointsUpperTotalSignal.clear();
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

void BmnGemStripReadoutModule::SetClusterDistortion(Double_t cluster_distortion) {
    if( cluster_distortion >= 0.0 && cluster_distortion <= 1.0 ) ClusterDistortion = cluster_distortion;
    else ClusterDistortion = 0.0;
}

void BmnGemStripReadoutModule::SetLandauMPV(Double_t mpv) {
    if( mpv > 0.0 ) LandauMPV = mpv;
}

void BmnGemStripReadoutModule::SetBackgroundNoiseLevel(Double_t background_noise_level) {
    if( background_noise_level >= 0.0 && background_noise_level <= 1.0 ) BackgroundNoiseLevel = background_noise_level;
    else BackgroundNoiseLevel = 0.0;
}

void BmnGemStripReadoutModule::SetMinSignalCutThreshold(Double_t min_cut_threshold) {
    if( min_cut_threshold >= 0.0 ) MinSignalCutThreshold = min_cut_threshold;
    else MinSignalCutThreshold = 0.0;
 }

void BmnGemStripReadoutModule::SetMaxSignalCutThreshold(Double_t max_cut_threshold) {
    if( max_cut_threshold >= 0.0 ) MaxSignalCutThreshold = max_cut_threshold;
    else MaxSignalCutThreshold = 0.0;
 }

void BmnGemStripReadoutModule::AddBackgroundNoise() {
    if(BackgroundNoiseLevel > 0.0) {

        Double_t sigma = LandauMPV*BackgroundNoiseLevel/3.333;
        sigma *= Gain;

        for(Int_t i = 0; i < ReadoutLowerPlane.size(); ++i) {
            Double_t low_layer_noise_val = gRandom->Gaus(0, sigma);
            if(low_layer_noise_val < 0.0) low_layer_noise_val *= -1;
            if(low_layer_noise_val > LandauMPV*BackgroundNoiseLevel*Gain) low_layer_noise_val = LandauMPV*BackgroundNoiseLevel*Gain;
            ReadoutLowerPlane.at(i) += low_layer_noise_val;
        }
        for(Int_t i = 0; i < ReadoutUpperPlane.size(); ++i) {
            Double_t up_layer_noise_val = gRandom->Gaus(0, sigma);
            if(up_layer_noise_val < 0.0) up_layer_noise_val *= -1;
            if(up_layer_noise_val > LandauMPV*BackgroundNoiseLevel*Gain) up_layer_noise_val = LandauMPV*BackgroundNoiseLevel*Gain;
            ReadoutUpperPlane.at(i) += up_layer_noise_val;
        }
    }
}

Bool_t BmnGemStripReadoutModule::AddRealPoint(Double_t x, Double_t y, Double_t z, Double_t signal, Int_t refID) {
    if( x >= XMinReadout && x <= XMaxReadout &&
        y >= YMinReadout && y <= YMaxReadout &&
        !DeadZone.IsInside(x, y) ) {

        Int_t numLowStrip = (Int_t)CalculateLowerStripZonePosition(x, y);
        Int_t numUpStrip = (Int_t)CalculateUpperStripZonePosition(x, y);

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
                                                  Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {
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

Bool_t BmnGemStripReadoutModule::AddRealPointFullOne(Double_t x, Double_t y, Double_t z, Double_t signal, Int_t refID) {

    if( x >= XMinReadout && x <= XMaxReadout &&
        y >= YMinReadout && y <= YMaxReadout &&
        !DeadZone.IsInside(x, y) ) {

        if(signal <= 0.0) signal = 1e-16;

        Double_t radius = AvalancheRadius;
        if(radius <= 0.0) return false;

        Int_t cycle_cnt = 0;
        while(true) {
            radius = gRandom->Gaus(AvalancheRadius, 0.02);
            if(radius > AvalancheRadius/2.0  && radius < AvalancheRadius*2.0 && radius > 0.0) break;
            cycle_cnt++;

            if(cycle_cnt > 5) {
                radius = AvalancheRadius;
                break;
            }
        }

        StripCluster upper_cluster = MakeCluster("upper", x, y, signal, radius);
        if(!upper_cluster.IsCorrect) {
            if(Verbosity)  {
                cout << "WARNING: Incorrect upper cluster for the point (" << x << " : " << y << ")\n";
            }
            return false;
        }
        StripCluster lower_cluster = MakeCluster("lower", x, y, signal, radius);
        if(!lower_cluster.IsCorrect) {
            if(Verbosity)  {
                cout << "WARNING: Incorrect lower cluster for the point (" << x << " : " << y << ")\n";
            }
            return false;
        }

        //Align upper and lower cluster signals to each other ------------------
        if(upper_cluster.TotalSignal != lower_cluster.TotalSignal) {
            Double_t shrink_coeff = lower_cluster.TotalSignal/upper_cluster.TotalSignal;

            if(upper_cluster.TotalSignal > lower_cluster.TotalSignal) {
                for(Int_t istrip = 0; istrip < upper_cluster.Strips.size(); ++istrip) {
                    upper_cluster.Signals.at(istrip) *= shrink_coeff;
                }
                upper_cluster.TotalSignal = lower_cluster.TotalSignal;
            }
            else {
                for(Int_t istrip = 0; istrip < lower_cluster.Strips.size(); ++istrip) {
                    lower_cluster.Signals.at(istrip) /= shrink_coeff;
                }
                lower_cluster.TotalSignal = upper_cluster.TotalSignal;
            }
        }
        //----------------------------------------------------------------------

        //Fill strip matches ---------------------------------------------------

        //upper layer
        for(Int_t ielement; ielement < upper_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = upper_cluster.Strips.at(ielement);
            Double_t strip_signal = upper_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutUpperPlane.size()) {
                UpperStripMatches.at(strip_num).AddLink(strip_signal/upper_cluster.TotalSignal, refID);
            }
        }
        //lower layer
        for(Int_t ielement; ielement < lower_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = lower_cluster.Strips.at(ielement);
            Double_t strip_signal = lower_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutLowerPlane.size()) {
                LowerStripMatches.at(strip_num).AddLink(strip_signal/lower_cluster.TotalSignal, refID);
            }
        }
        //----------------------------------------------------------------------

        //Add the correct clusters on the readout layers -----------------------

        Double_t max_signal_level = LandauMPV*MaxSignalCutThreshold*Gain; //max signal value of the strip

        //upper cluster
        for(Int_t ielement; ielement < upper_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = upper_cluster.Strips.at(ielement);
            Double_t strip_signal = upper_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutUpperPlane.size()) {
                ReadoutUpperPlane.at(strip_num) += strip_signal;
                //cut the signal
                if(MaxSignalCutThreshold > 0.0) {
                    if(ReadoutUpperPlane.at(strip_num) > max_signal_level) {
                        ReadoutUpperPlane.at(strip_num) = max_signal_level;
                    }
                }
            }
        }
        //lower cluster
        for(Int_t ielement; ielement < lower_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = lower_cluster.Strips.at(ielement);
            Double_t strip_signal = lower_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutLowerPlane.size()) {
                ReadoutLowerPlane.at(strip_num) += strip_signal;
                //cut the signal
                if(MaxSignalCutThreshold > 0.0) {
                    if(ReadoutLowerPlane.at(strip_num) > max_signal_level) {
                        ReadoutLowerPlane.at(strip_num) = max_signal_level;
                    }
                }
            }
        }
        //----------------------------------------------------------------------


        RealPointsX.push_back(x);
        RealPointsY.push_back(y);

        RealPointsLowerStripPos.push_back(lower_cluster.MeanPosition);
        RealPointsUpperStripPos.push_back(upper_cluster.MeanPosition);

        RealPointsLowerTotalSignal.push_back(lower_cluster.TotalSignal);
        RealPointsUpperTotalSignal.push_back(upper_cluster.TotalSignal);

        return true;
    }
    else {
        if(Verbosity) cout << "WARNING: Point (" << x << " : " << y << " : " << z << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

StripCluster BmnGemStripReadoutModule::MakeCluster(TString layer, Double_t xcoord, Double_t ycoord, Double_t signal, Double_t radius) {
    //#define DRAW_REAL_CLUSTER_HISTOGRAMS

    StripCluster cluster;

    if(radius <= 0.0) return cluster;

    Double_t RadiusInZones = radius/Pitch;
    Double_t Sigma = RadiusInZones/3.33;

    TF1 gausF("gausF", "gaus", -4*Sigma, 4*Sigma);
    gausF.SetParameter(0, 1.0); // constant (altitude)
    gausF.SetParameter(1, 0.0); // mean (center position)
    gausF.SetParameter(2, Sigma); //sigma

    Double_t SCluster = gausF.Integral(-4*Sigma, 4*Sigma); //square of the one side distribution (more exactly)

    TRandom rand(0);
    Double_t var_level = ClusterDistortion; //signal variation (0.1 is 10%)

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
            cluster.AddStrip(NumCurrentZone, Energy);
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
            cluster.AddStrip(NumCurrentZone, Energy);
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
                cluster.AddStrip(NumCurrentZone, Energy);
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
            cluster.AddStrip(NumCurrentZone, Energy);
            total_signal += Energy;
        }

        dist += h;
    }

    if (cluster.GetClusterSize() <= 0) {
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
    Int_t last_strip_num = cluster.Strips.at(cluster.GetClusterSize()-1);
    Int_t nstrips = last_strip_num - begin_strip_num + 1;

    TH1F hist("hist_for_fit", "hist_for_fit", nstrips+NOutLeftBins+NOutRightBins, begin_strip_num-NOutLeftBins, last_strip_num+1+NOutRightBins);
    Int_t hist_index = 0;

    for(Int_t i = 0; i < cluster.GetClusterSize(); ++i) {
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

    if(nstrips > 1) {
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

void BmnGemStripReadoutModule::FindClustersInLayer(vector<Double_t> &StripLayer, vector<Double_t> &StripHits, vector<Double_t> &StripHitsTotalSignal, vector<Double_t> &StripHitsErrors) {

    Double_t threshold = LandauMPV*MinSignalCutThreshold*Gain;

    StripCluster cluster;

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
                MakeStripHit(cluster, Strips, StripHits, StripHitsTotalSignal, StripHitsErrors, is);
            }
            continue;
        }

        if( cluster.GetClusterSize() > 0 ) {
            if( Strips.at(is) >= (cluster.Signals.at(cluster.GetClusterSize()-1)) ) {
                if(descent) {
                    ascent = false;
                    descent = false;
                    //make strip hit
                    MakeStripHit(cluster, Strips, StripHits, StripHitsTotalSignal, StripHitsErrors, is);
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

        cluster.AddStrip(is, Strips.at(is));
    }

    if(cluster.GetClusterSize() != 0) {
        //make strip hit
        Int_t lastnum = Strips.size()-1;
        MakeStripHit(cluster, Strips, StripHits, StripHitsTotalSignal, StripHitsErrors, lastnum);
    }
}

void BmnGemStripReadoutModule::MakeStripHit(StripCluster &cluster, vector<Double_t> &Strips, vector<Double_t> &StripHits, vector<Double_t> &StripHitsTotalSignal, vector<Double_t> &StripHitsErrors, Int_t &curcnt) {
    //#define DRAW_FOUND_CLUSTER_HISTOGRAMS

    Double_t total_signal = 0.0;

    //find max strip
    Double_t maxval = 0.0;
    Int_t maxdig = 0;

    for(Int_t i = 0; i < cluster.GetClusterSize(); i++) {
        Double_t signal = cluster.Signals.at(i);
        if(signal > maxval) {
            maxval = signal;
            maxdig = i;
        }
        total_signal += signal;
    }

    Int_t nextStripNum = cluster.Strips.at(cluster.GetClusterSize()-1)+1;
    if(nextStripNum < Strips.size()) {
        //if(Strips.at(nextStripNum) > 0.0) {
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
        //}
    }

    curcnt--;
    if( curcnt < 0 ) curcnt = 0;

    Double_t mean_fit_pos = 0.0;
    Double_t sigma_fit = 0.0;
    Double_t sigma_fit_err = 0.0;

    Int_t begin_strip_num = (Int_t)(cluster.Strips.at(0));
    Int_t last_strip_num = (Int_t)(cluster.Strips.at(cluster.GetClusterSize()-1));
    Int_t nstrips = last_strip_num - begin_strip_num + 1;

    TH1F hist("hist_for_fit", "hist_for_fit", nstrips, begin_strip_num, last_strip_num+1);
    Int_t hist_index = 0;

    Double_t cluster_signal_sum = 0.0;
    Double_t mean_mass_center_pos = 0.0;

    for(Int_t i = 0; i < cluster.GetClusterSize(); ++i) {
        Double_t value = cluster.Signals.at(i);
        hist.SetBinContent(hist_index+1, value);
        hist_index++;
        cluster_signal_sum += value;
    }

    for(Int_t i = 0; i < cluster.GetClusterSize(); ++i) {
        Double_t strip_num = cluster.Strips.at(i);
        Double_t value = cluster.Signals.at(i);
        mean_mass_center_pos += (strip_num+0.5)*value;
    }
    mean_mass_center_pos /= cluster_signal_sum;

    TF1* gausFitFunction = 0;
        TString fit_params = "WQ0";

    #ifdef DRAW_FOUND_CLUSTER_HISTOGRAMS
        fit_params = "WQ";
    #endif

    if(nstrips > 1) {
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

    //Double_t mean_pos =  mean_fit_pos;
    Double_t mean_pos =  mean_mass_center_pos;

    if(mean_pos < 0.0) mean_pos = 0.0;
    if(mean_pos >= Strips.size()) mean_pos = Strips.size() - 0.001;

    StripHits.push_back(mean_pos);
    StripHitsTotalSignal.push_back(total_signal);
    StripHitsErrors.push_back(sigma_fit_err);

    cluster.Clear();

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
    if(AngleDeg <= 0.0 && AngleDeg >= -90) {
        XRotationCenter = XMinReadout;
        YRotationCenter = YMinReadout;
    }
    else {
        if(AngleDeg > 0.0 && AngleDeg <= 90.0) {
            XRotationCenter = XMinReadout;
            YRotationCenter = YMaxReadout;
        }
    }
    Double_t UpperX = (xcoord - XRotationCenter)*Cos(-AngleRad) + (ycoord - YRotationCenter)*Sin(-AngleRad) + XRotationCenter;//see
    return UpperX;
}

Double_t BmnGemStripReadoutModule::ConvertRealPointToUpperY(Double_t xcoord, Double_t ycoord) {
    Double_t XRotationCenter;
    Double_t YRotationCenter;
    if(AngleDeg <= 0 && AngleDeg >= -90) {
        XRotationCenter = XMinReadout;
        YRotationCenter = YMinReadout;
    }
    else {
        if(AngleDeg > 0 && AngleDeg <= 90.0) {
            XRotationCenter = XMinReadout;
            YRotationCenter = YMaxReadout;
        }
    }
    Double_t UpperY = -(xcoord - XRotationCenter)*Sin(-AngleRad) + (ycoord - YRotationCenter)*Cos(-AngleRad) + YRotationCenter;//see
    return UpperY;
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

Bool_t BmnGemStripReadoutModule::SetMatchOfLowerStrip(Int_t indx, BmnMatch strip_match) {
    if(indx >= 0 && indx < LowerStripMatches.size()) {
        LowerStripMatches.at(indx) = strip_match;
        return true;
    }
    else return false;
}

Bool_t BmnGemStripReadoutModule::SetMatchOfUpperStrip(Int_t indx, BmnMatch strip_match) {
    if(indx >= 0 && indx < UpperStripMatches.size()) {
        UpperStripMatches.at(indx) = strip_match;
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
        ratio = (ConvertRealPointToUpperX(XMaxReadout, YMaxReadout) - XMinReadout)/Pitch;;

    }
    if (AngleDeg > 0 && AngleDeg <= 90.0){
        ratio = (ConvertRealPointToUpperX(XMaxReadout, YMinReadout) - XMinReadout)/Pitch;
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

BmnMatch BmnGemStripReadoutModule::GetMatchOfLowerStrip(Int_t indx) {
    if(indx >= 0 && indx < LowerStripMatches.size()) {
        return LowerStripMatches.at(indx);
    }
    else return BmnMatch(); //return an empty match
}

BmnMatch BmnGemStripReadoutModule::GetMatchOfUpperStrip(Int_t indx) {
    if(indx >= 0 && indx < UpperStripMatches.size()) {
        return UpperStripMatches.at(indx);
    }
    else return BmnMatch(); //return an empty match
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
        ycoord = Tan(PiOver2()-AngleRad)*(xcoord-XMinReadout) + (YMinReadout + (UpperStripZonePos*hypoten));

    }
    if (AngleDeg > 0 && AngleDeg <= 90.0) {
        ycoord = Tan(PiOver2()-AngleRad)*(xcoord-XMinReadout) +  (YMaxReadout - UpperStripZonePos*hypoten);
    }

    return ycoord;
}

void BmnGemStripReadoutModule::CalculateStripHitIntersectionPoints() {
    //#define DRAW_STRIP_LAYERS_HISTOGRAMS

    ResetIntersectionPoints();
    ResetStripHits();

    FindClustersInLayer(ReadoutLowerPlane, LowerStripHits, LowerStripHitsTotalSignal, LowerStripHitsErrors);
    FindClustersInLayer(ReadoutUpperPlane, UpperStripHits, UpperStripHitsTotalSignal, UpperStripHitsErrors);

    for(UInt_t istrip_low = 0; istrip_low < LowerStripHits.size(); ++istrip_low) {
        for(UInt_t istrip_up = 0; istrip_up < UpperStripHits.size(); ++istrip_up) {
            Double_t xcoord = FindXHitIntersectionPoint(LowerStripHits.at(istrip_low),UpperStripHits.at(istrip_up));
            Double_t ycoord = FindYHitIntersectionPoint(LowerStripHits.at(istrip_low),UpperStripHits.at(istrip_up));

            if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) && !DeadZone.IsInside(xcoord, ycoord) ) {
                IntersectionPointsX.push_back(xcoord);
                IntersectionPointsY.push_back(ycoord);

                IntersectionPointsLowerStripPos.push_back(LowerStripHits.at(istrip_low));
                IntersectionPointsUpperStripPos.push_back(UpperStripHits.at(istrip_up));

                IntersectionPointsLowerTotalSignal.push_back(LowerStripHitsTotalSignal.at(istrip_low));
                IntersectionPointsUpperTotalSignal.push_back(UpperStripHitsTotalSignal.at(istrip_up));

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
                IntersectionPointsXErrors.push_back(LowerStripHitsErrors.at(istrip_low)*Pitch/xerr_correct_coef);
                IntersectionPointsYErrors.push_back(UpperStripHitsErrors.at(istrip_up)*(Pitch/Sin(Abs(AngleRad)))/yerr_correct_coef); //FIX IT

                //intersection matching ----------------------------------------
                BmnMatch strip_match_lower_layer = LowerStripMatches.at((Int_t)LowerStripHits.at(istrip_low));
                BmnMatch strip_match_upper_layer = UpperStripMatches.at((Int_t)UpperStripHits.at(istrip_up));

                BmnMatch intersection_match;
                intersection_match.AddLink(strip_match_lower_layer);
                intersection_match.AddLink(strip_match_upper_layer);

                IntersectionPointMatches.push_back(intersection_match);
                //--------------------------------------------------------------
            }
        }
    }

    //number of recognized clusters in the layers
    Int_t N_low_hits = LowerStripHits.size();
    Int_t N_up_hits = UpperStripHits.size();
    if(N_low_hits > N_up_hits) NGoodHits = N_low_hits;
    else NGoodHits = N_up_hits;


    #ifdef DRAW_STRIP_LAYERS_HISTOGRAMS
    //upper layer --------------------------------------------------------------
        TString upper_layer_name = "upper_layer_";
            upper_layer_name += "z"; upper_layer_name += GetZPositionReadout();
            upper_layer_name += "_xmin"; upper_layer_name += GetXMinReadout();
            upper_layer_name += "_xmax"; upper_layer_name += GetXMaxReadout();
            upper_layer_name += "_ymin"; upper_layer_name += GetYMinReadout();
            upper_layer_name += "_ymax"; upper_layer_name += GetYMaxReadout();

        TCanvas upper_layer_canv("upper_layer_canv", "upper_layer_canv", 10, 10, 1200, 800);
        upper_layer_canv.SetGrid();

        TH1F upper_layer_hist("upper_layer_hist", "upper_layer_hist", ReadoutUpperPlane.size()+1, 0.0, ReadoutUpperPlane.size()+1);
        upper_layer_hist.SetMaximum(15.0*Gain);

        for(int i = 0; i < ReadoutUpperPlane.size(); i++) {
            upper_layer_hist.SetBinContent(i+1, ReadoutUpperPlane.at(i));
        }
        upper_layer_hist.SetFillColor(TColor::GetColor("#e3f3ff"));
        upper_layer_hist.SetTitle(upper_layer_name);
        upper_layer_hist.Draw();

        TLegend upper_layer_leg(0.75, 0.7, 0.98, 0.95);
        upper_layer_leg.AddEntry("", TString("nstrips = ")+=(ReadoutUpperPlane.size()) , "p");
        upper_layer_leg.AddEntry("", TString("pitch = ")+=(GetPitch()) , "p");
        upper_layer_leg.AddEntry("", TString("dangle = ")+=(GetAngleDeg()) , "p");
        upper_layer_leg.AddEntry("", TString("LandauMPV = ")+=(GetLandauMPV()) , "p");
        upper_layer_leg.AddEntry("", TString("noise   (%)   = ")+=(GetBackgroundNoiseLevel()) , "p");
        upper_layer_leg.AddEntry("", TString("noise   (val) = ")+=(GetBackgroundNoiseLevel()*GetLandauMPV()*GetGain()) , "p");
        upper_layer_leg.AddEntry("", TString("lthresh (%)   = ")+=(GetMinSignalCutThreshold()) , "p");
        upper_layer_leg.AddEntry("", TString("lthresh (val) = ")+=(GetMinSignalCutThreshold()*GetLandauMPV()*GetGain()) , "p");
        upper_layer_leg.AddEntry("", TString("uthresh (%)   = ")+=(GetMaxSignalCutThreshold()) , "p");
        upper_layer_leg.AddEntry("", TString("uthresh (val) = ")+=(GetMaxSignalCutThreshold()*GetLandauMPV()*GetGain()) , "p");
        upper_layer_leg.AddEntry("", TString("gain = ")+=(GetGain()) , "p");
        upper_layer_leg.AddEntry("", TString("N recognized hits = ")+=(N_up_hits) , "p");
        upper_layer_leg.Draw();

        TLine upper_layer_noise_line(0, LandauMPV*BackgroundNoiseLevel*Gain, ReadoutUpperPlane.size(), LandauMPV*BackgroundNoiseLevel*Gain);
        upper_layer_noise_line.SetLineColor(TColor::GetColor("#00ff00"));
        upper_layer_noise_line.Draw();

        TLine upper_layer_min_thresh_line(0, LandauMPV*MinSignalCutThreshold*Gain, ReadoutUpperPlane.size(), LandauMPV*MinSignalCutThreshold*Gain);
        upper_layer_min_thresh_line.SetLineColor(TColor::GetColor("#ff0000"));
        upper_layer_min_thresh_line.Draw();

        TLine upper_layer_max_thresh_line(0, LandauMPV*MaxSignalCutThreshold*Gain, ReadoutUpperPlane.size(), LandauMPV*MaxSignalCutThreshold*Gain);
        upper_layer_max_thresh_line.SetLineColor(TColor::GetColor("#ccaa00"));
        upper_layer_max_thresh_line.Draw();

        TString file_upper_layer_name = "/home/diman/Software/pics_w/test/";
            file_upper_layer_name += upper_layer_name;
        upper_layer_canv.SaveAs(file_upper_layer_name+".root");
        upper_layer_canv.SaveAs(file_upper_layer_name+".png");
    //--------------------------------------------------------------------------

    //lower layer --------------------------------------------------------------
        TString lower_layer_name = "lower_layer_";
            lower_layer_name += "z"; lower_layer_name += GetZPositionReadout();
            lower_layer_name += "_xmin"; lower_layer_name += GetXMinReadout();
            lower_layer_name += "_xmax"; lower_layer_name += GetXMaxReadout();
            lower_layer_name += "_ymin"; lower_layer_name += GetYMinReadout();
            lower_layer_name += "_ymax"; lower_layer_name += GetYMaxReadout();

        TCanvas lower_layer_canv("lower_layer_canv", "lower_layer_canv", 10, 10, 1200, 800);
        lower_layer_canv.SetGrid();
        TH1F lower_layer_hist("lower_layer_hist", "lower_layer_hist", ReadoutLowerPlane.size()+1, 0.0, ReadoutLowerPlane.size()+1);
        lower_layer_hist.SetMaximum(15.0*Gain);
        for(int i = 0; i < ReadoutLowerPlane.size(); i++) {
            lower_layer_hist.SetBinContent(i+1, ReadoutLowerPlane.at(i));
        }
        lower_layer_hist.SetFillColor(TColor::GetColor("#e3f3ff"));
        lower_layer_hist.SetTitle(lower_layer_name);
        lower_layer_hist.Draw();

        TLegend lower_layer_leg(0.75, 0.7, 0.98, 0.95);
        lower_layer_leg.AddEntry("", TString("nstrips = ")+=(ReadoutLowerPlane.size()) , "p");
        lower_layer_leg.AddEntry("", TString("pitch = ")+=(GetPitch()) , "p");
        lower_layer_leg.AddEntry("", TString("dangle = ")+=(GetAngleDeg()) , "p");
        lower_layer_leg.AddEntry("", TString("LandauMPV = ")+=(GetLandauMPV()) , "p");
        lower_layer_leg.AddEntry("", TString("noise   (%)   = ")+=(GetBackgroundNoiseLevel()) , "p");
        lower_layer_leg.AddEntry("", TString("noise   (val) = ")+=(GetBackgroundNoiseLevel()*GetLandauMPV()*GetGain()) , "p");
        lower_layer_leg.AddEntry("", TString("lthresh (%)   = ")+=(GetMinSignalCutThreshold()) , "p");
        lower_layer_leg.AddEntry("", TString("lthresh (val) = ")+=(GetMinSignalCutThreshold()*GetLandauMPV()*GetGain()) , "p");
        lower_layer_leg.AddEntry("", TString("uthresh (%)   = ")+=(GetMaxSignalCutThreshold()) , "p");
        lower_layer_leg.AddEntry("", TString("uthresh (val) = ")+=(GetMaxSignalCutThreshold()*GetLandauMPV()*GetGain()) , "p");
        lower_layer_leg.AddEntry("", TString("gain = ")+=(GetGain()) , "p");
        lower_layer_leg.AddEntry("", TString("N recognized hits = ")+=(N_low_hits) , "p");
        lower_layer_leg.Draw();

        TLine lower_layer_noise_line(0, LandauMPV*BackgroundNoiseLevel*Gain, ReadoutLowerPlane.size(), LandauMPV*BackgroundNoiseLevel*Gain);
        lower_layer_noise_line.SetLineColor(TColor::GetColor("#00ff00"));
        lower_layer_noise_line.Draw();

        TLine lower_layer_min_thresh_line(0, LandauMPV*MinSignalCutThreshold*Gain, ReadoutLowerPlane.size(), LandauMPV*MinSignalCutThreshold*Gain);
        lower_layer_min_thresh_line.SetLineColor(TColor::GetColor("#ff0000"));
        lower_layer_min_thresh_line.Draw();

        TLine lower_layer_max_thresh_line(0, LandauMPV*MaxSignalCutThreshold*Gain, ReadoutLowerPlane.size(), LandauMPV*MaxSignalCutThreshold*Gain);
        lower_layer_max_thresh_line.SetLineColor(TColor::GetColor("#ccaa00"));
        lower_layer_max_thresh_line.Draw();

        TString file_lower_layer_name = "/home/diman/Software/pics_w/test/";
            file_lower_layer_name += lower_layer_name;
        lower_layer_canv.SaveAs(file_lower_layer_name+".root");
        lower_layer_canv.SaveAs(file_lower_layer_name+".png");

    //--------------------------------------------------------------------------
    #endif
}
//------------------------------------------------------------------------------

void BmnGemStripReadoutModule::GenerateAvalanche_Test() {
 //NO c++11 - no new code!
/*
    gRandom->SetSeed(0);
    cout << "seed = " << gRandom->GetSeed() << "\n";

    double const mean_free_path = 0.0333;

    double x_in = 5.0, y_in = 5.0, z_in = 0.0;
    double x_out = 5.0, y_out = 5.5, z_out = 0.9;

    double px = x_out - x_in;
    double py = y_out - y_in;
    double pz = z_out - z_in;

    double length = std::sqrt( (x_out-x_in)*(x_out-x_in) + (y_out-y_in)*(y_out-y_in) + (z_out-z_in)*(z_out-z_in) );

    std::streamsize old_precision(cout.precision());
    cout << setprecision(2) << setiosflags(ios::fixed);
    cout << "start point  = ( " << x_in << " : " << y_in << " : " << z_in << " )\n";
    cout << "end point    = ( " << x_out << " : " << y_out << " : " << z_out << " )\n";
    cout << "direction    = ( " << px << " : " << py << " : " << pz << " )\n";
    cout << "track length = " << length << "\n";
    cout.unsetf(ios::fixed);
    cout.precision(old_precision);

//------------------------------------------------------------------------------
    TCanvas *mean_free_path_distr_canv = new TCanvas("mean_free_path_distr_canv", "mean_free_path_distr_canv", 10, 10, 1000, 800);
    mean_free_path_distr_canv->SetGrid();
    TH1F *mean_free_path_distr_hist = new TH1F("mean_free_path_distr_hist", "mean_free_path_distr_hist", 100, 0.0, 0.5);

    for(int i = 0; i < 10000; ++i) {
        double rand_val = gRandom->Exp(mean_free_path);
        mean_free_path_distr_hist->Fill(rand_val);
    }

    mean_free_path_distr_canv->cd();
    mean_free_path_distr_hist->Draw();
//------------------------------------------------------------------------------

    TCanvas *eletrons_per_cluster_distr_canv = new TCanvas("eletrons_per_cluster_distr_canv", "eletrons_per_cluster_distr_canv", 10, 10, 1000, 800);
    eletrons_per_cluster_distr_canv->SetGrid();
    TH1F *eletrons_per_cluster_distr_hist = new TH1F("eletrons_per_cluster_distr_hist", "eletrons_per_cluster_distr_hist", 100, 0.0, 100.0);

    for(int i = 0; i < 10000; ++i) {
        double rand_val = gRandom->Landau(1.027, 0.11);
        if(rand_val < 1.0) {
            rand_val = 1.0;
        }
        eletrons_per_cluster_distr_hist->Fill(rand_val);
    }

    eletrons_per_cluster_distr_canv->cd();
    eletrons_per_cluster_distr_hist->Draw();

//------------------------------------------------------------------------------

    double current_length = 0.0;
    double current_length_ratio = 0.0;
    int collisions_cnt = 0;
    double current_step = 0.0;

    struct CollPoint {
        CollPoint(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
        double x;
        double y;
        double z;
    };

    std::vector<CollPoint> collision_points;

    while(current_length < length) {
        current_step = gRandom->Exp(mean_free_path);
        current_length += current_step;

        if(current_length > length) break;

        current_length_ratio = current_length/length;

        double current_x = x_in + current_length_ratio*px;
        double current_y = y_in + current_length_ratio*py;
        double current_z = z_in + current_length_ratio*pz;

        collision_points.push_back(CollPoint(current_x, current_y, current_z));

        cout << "  collision[" << collisions_cnt << "]: " << " current_step = " << current_step << ", current_length = " << current_length << ", ratio = " << current_length_ratio << "\n";
        cout << "     coordinates(x:y:z) = ( " << current_x << " : " << current_y << " : " << current_z << " )\n";

        collisions_cnt++;
    }
    cout << "N collisions at the track = " << collisions_cnt << "\n";

//------------------------------------------------------------------------------
    double x_min_visible_area = XMinReadout;
    double y_min_visible_area = YMinReadout;
    double x_max_visible_area = XMaxReadout;
    double y_max_visible_area = YMaxReadout;
    double z_min_visible_area = 0.0;
    double z_max_visible_area = 0.9;

    x_min_visible_area = 4.5;
    y_min_visible_area = 4.5;
    x_max_visible_area = 5.5;
    y_max_visible_area = 5.5;
    z_min_visible_area = 0.0;
    z_max_visible_area = 1.0;

    //xy track projection ------------------------------------------------------
    TCanvas *xy_track_slice_canv = new TCanvas("xy_track_slice_canv", "xy_track_slice_canv", 10, 10, 1000, 800);
    xy_track_slice_canv->Range(x_min_visible_area, y_min_visible_area, x_max_visible_area, y_max_visible_area);

    TLine *xy_track_line = new TLine(x_in, y_in, x_out, y_out);
    xy_track_line->SetLineColor(TColor::GetColor("#33ff33"));
    xy_track_line->Draw();

    for(int ipoint = 0; ipoint < collision_points.size(); ++ipoint) {
        TMarker *xy_point_mark = new TMarker(collision_points[ipoint].x, collision_points[ipoint].y, 20);
        xy_point_mark->SetMarkerSize(0.5);
        xy_point_mark->SetMarkerColor(TColor::GetColor("#33ff33"));
        xy_point_mark->Draw();
    }
    //--------------------------------------------------------------------------

    //xz track projection ------------------------------------------------------
    TCanvas *xz_track_slice_canv = new TCanvas("xz_track_slice_canv", "xz_track_slice_canv", 10, 10, 1000, 800);
    xz_track_slice_canv->Range(x_min_visible_area, z_min_visible_area, x_max_visible_area, z_max_visible_area);


    TLine *xz_track_line = new TLine(x_in, z_in, x_out, z_out);
    xz_track_line->SetLineColor(TColor::GetColor("#33ff33"));
    xz_track_line->Draw();

    for(int ipoint = 0; ipoint < collision_points.size(); ++ipoint) {
        TMarker *xz_point_mark = new TMarker(collision_points[ipoint].x, collision_points[ipoint].z, 20);
        xz_point_mark->SetMarkerSize(0.5);
        xz_point_mark->SetMarkerColor(TColor::GetColor("#33ff33"));
        xz_point_mark->Draw();
    }
    //--------------------------------------------------------------------------

    //xz track projection ------------------------------------------------------
    TCanvas *yz_track_slice_canv = new TCanvas("yz_track_slice_canv", "yz_track_slice_canv", 10, 10, 1000, 800);
    yz_track_slice_canv->Range(y_min_visible_area, z_min_visible_area, y_max_visible_area, z_max_visible_area);


    TLine *yz_track_line = new TLine(y_in, z_in, y_out, z_out);
    yz_track_line->SetLineColor(TColor::GetColor("#33ff33"));
    yz_track_line->Draw();

    for(int ipoint = 0; ipoint < collision_points.size(); ++ipoint) {
        TMarker *yz_point_mark = new TMarker(collision_points[ipoint].y, collision_points[ipoint].z, 20);
        yz_point_mark->SetMarkerSize(0.5);
        yz_point_mark->SetMarkerColor(TColor::GetColor("#33ff33"));
        yz_point_mark->Draw();
    }
    //--------------------------------------------------------------------------


    TCanvas *xy_readout_canv = new TCanvas("xy_readout_canv", "xy_readout_canv", 10, 10, 900, 900);
    xy_readout_canv->SetGrid();

    Double_t bin_size = 0.005; // cm
    Int_t xbins = fabs(x_max_visible_area -x_min_visible_area)/bin_size;
    Int_t ybins = fabs(y_max_visible_area -y_min_visible_area)/bin_size;
    cout << " >>> Histogram bins: "<< "xbins = " << xbins << ",  ybins = " << ybins << " ( one bin size = " << bin_size << " cm )"<< "\n";

    TH2F *xy_readout_hist = new TH2F("xy_readout_hist", "xy_readout_hist", xbins, x_min_visible_area, x_max_visible_area,     ybins, y_min_visible_area, y_max_visible_area);
    xy_readout_hist->GetXaxis()->SetTitle("x [cm]"); xy_readout_hist->GetXaxis()->CenterTitle();
    xy_readout_hist->GetYaxis()->SetTitle("y [cm]"); xy_readout_hist->GetYaxis()->CenterTitle();

    for(int ipoint = 0; ipoint < collision_points.size(); ++ipoint) {
        double x = collision_points[ipoint].x;
        double y = collision_points[ipoint].y;
        double z = collision_points[ipoint].z;

        double rdist = fabs(z_out - z); //distance to readout

        //x_displacement
        double x_displacement = 0.000227984 + 0.0614758*rdist + 0.157119*rdist*rdist + (-0.0799265)*rdist*rdist*rdist;

        //smear
        double sigma = 0.0;
        if(rdist < 0.1) {
            sigma = std::sqrt(0.000663416*rdist);
        }

        if(rdist >= 0.1 && rdist < 0.3) {
            sigma = 0.003897 + 0.045375*rdist + (-0.03355)*rdist*rdist;
        }

        if(rdist >= 0.3 && rdist < 0.6) {
            sigma = 0.004095 + 0.0424*rdist + (-0.026)*rdist*rdist;
        }

        if(rdist >= 0.6) {
            sigma = 0.0118343 + 0.0200945*rdist + (-0.010325)*rdist*rdist;
        }

        cout << "  point[" << ipoint << "]: dist_to_readout = " << rdist << ",  xdispl = " << x_displacement << ",  sigma = " << sigma << "\n";

        int n_electrons_cluster = gRandom->Landau(1.027, 0.11);
        if(n_electrons_cluster < 1) n_electrons_cluster = 1;
        if(n_electrons_cluster > 6) n_electrons_cluster = 6;

        cout << "    electrons in cluster = " << n_electrons_cluster << "\n";

        for(int ielectron = 0; ielectron < n_electrons_cluster; ++ielectron) {

            cout << "       electron[" << ielectron << "]:\n";

            int gain_gem1 = gRandom->Exp(25);
            int gain_gem2 = gRandom->Exp(25);
            int gain_gem3 = gRandom->Exp(25);

            int total_gain = 0;

            if(rdist < 0.15) {
                total_gain = 1;
            }

            if(rdist >= 0.15 && rdist < 0.35) {
                total_gain = gain_gem3;
            }

            if(rdist >= 0.35 && rdist < 0.6) {
                total_gain = gain_gem3*gain_gem2;
            }

            if(rdist >= 0.6) {
                total_gain = gain_gem3*gain_gem2*gain_gem1;
            }

            cout << "        total gain for electron = " << total_gain << "\n";



            double x_readout, y_readout;
            for(int igen = 0; igen < total_gain; ++igen) {
                x_readout = gRandom->Gaus(x-x_displacement, sigma);
                y_readout = gRandom->Gaus(y, sigma);

                xy_readout_hist->Fill(x_readout, y_readout);
            }
        }

    }

    xy_readout_canv->cd();
    xy_readout_hist->Draw("colz");



    return;
*/
}

ClassImp(BmnGemStripReadoutModule)
