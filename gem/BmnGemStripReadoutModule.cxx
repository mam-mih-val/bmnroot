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

    ZStartModulePosition = 0.0;

    DriftGapThickness = 0.3; //cm
    FirstTransferGapThickness = 0.25; //cm
    SecondTransferGapThickness = 0.2; //cm
    InductionGapThickness = 0.15; //cm
    ModuleThickness = DriftGapThickness + FirstTransferGapThickness + SecondTransferGapThickness + InductionGapThickness; //cm

    ElectronDriftDirection = ForwardZAxisEDrift;

    MCD = 0.0333; //mean collision distance (mean free flight path) [cm]

    AvalancheRadius = 0.10; //cm
    Gain = 1.0; //gain level (for each electron signal - in RealPointFull or for strip signal - in RealPointFullOne)

    //возможно в дальнейшем придется избавиться от этого дерьма!
    ClusterDistortion = 0.0;
    LandauMPV = 1.6; //keV (default)
    BackgroundNoiseLevel = 0.0; //was 0.03
    MinSignalCutThreshold = 0.05;
    MaxSignalCutThreshold = 6.25;

    LowerStripOrder = LeftToRight;
    UpperStripOrder = LeftToRight;

    if(AngleDeg <= 0.0 && AngleDeg >= -90.0) {
        SetStripNumberingBorders(LeftBottom, RightTop);
    }
    else {
        if(AngleDeg > 0.0 && AngleDeg <= 90.0) {
            SetStripNumberingBorders(LeftTop, RightBottom);
        }
    }

    CreateReadoutPlanes();
}

BmnGemStripReadoutModule::BmnGemStripReadoutModule(Double_t xsize, Double_t ysize,
                                       Double_t xorig, Double_t yorig,
                                       Double_t pitch, Double_t adeg,
                                       Double_t low_strip_width, Double_t up_strip_width,
                                       Double_t zpos_module,
                                       ElectronDriftDirectionInModule edrift_direction) {
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

    ZStartModulePosition = zpos_module;

    DriftGapThickness = 0.3; //cm
    FirstTransferGapThickness = 0.25; //cm
    SecondTransferGapThickness = 0.2; //cm
    InductionGapThickness = 0.15; //cm
    ModuleThickness = DriftGapThickness + FirstTransferGapThickness + SecondTransferGapThickness + InductionGapThickness; //cm

    ElectronDriftDirection = edrift_direction;

    MCD = 0.0333; //mean collision distance (mean free flight path) [cm]

    AvalancheRadius = 0.10; //cm
    Gain = 1.0; //gain level (for each electron signal - in RealPointFull or for strip signal - in RealPointFullOne)

    //возможно в дальнейшем придется избавиться от этого дерьма!
    ClusterDistortion = 0.0;
    LandauMPV = 1.6; //keV (default)
    BackgroundNoiseLevel = 0.0; //was 0.03
    MinSignalCutThreshold = 0.05;
    MaxSignalCutThreshold = 6.25;

    LowerStripOrder = LeftToRight;
    UpperStripOrder = LeftToRight;

    if(AngleDeg <= 0.0 && AngleDeg >= -90.0) {
        SetStripNumberingBorders(LeftBottom, RightTop);
    }
    else {
        if(AngleDeg > 0.0 && AngleDeg <= 90.0) {
            SetStripNumberingBorders(LeftTop, RightBottom);
        }
    }

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

    ResetIntersectionPoints();
    ResetRealPoints();

    ResetStripHits();

    ResetElectronPointsAndClusters(); //test
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

Bool_t BmnGemStripReadoutModule::AddDeadZone(Int_t n_points, Double_t *x_points, Double_t *y_points) {
    DeadZoneOfReadoutModule dead_zone;
    Bool_t status = dead_zone.SetDeadZone(n_points, x_points, y_points);
    if(status == true) {
        DeadZones.push_back(dead_zone);
        return true;
    }
    else {
        return false;
    }
}

Bool_t BmnGemStripReadoutModule::IsPointInsideDeadZones(Double_t x, Double_t y) {
    for(Int_t izone = 0; izone < DeadZones.size(); ++izone) {
        if(DeadZones[izone].IsInside(x,y)) return true;
    }

    return false;
}

Bool_t BmnGemStripReadoutModule::IsPointInsideReadoutModule(Double_t x, Double_t y) {
    if( x >= XMinReadout && x <= XMaxReadout &&
        y >= YMinReadout && y <= YMaxReadout &&
        !IsPointInsideDeadZones(x, y) ) return true;
    else
        return false;
}

Bool_t BmnGemStripReadoutModule::SetStripNumberingBorders(Double_t x_left, Double_t y_left, Double_t x_right, Double_t y_right) {
    if(x_right < x_left) return false;

    XLeftPointOfStripNumbering = x_left;
    YLeftPointOfStripNumbering = y_left;
    XRightPointOfStripNumbering = x_right;
    YRightPointOfStripNumbering = y_right;

    RebuildReadoutPlanes();

    return true;
}

Bool_t BmnGemStripReadoutModule::SetStripNumberingBorders(StripBorderPoint left, StripBorderPoint right) {

    if( left != LeftTop && left != LeftBottom )  {
        if(Verbosity) cout << "WARNING: SetStripNumberingBorders: left strip border point is incorrect\n";
        return false;
    }
    if( right != RightTop && right != RightBottom ) {
        if(Verbosity) cout << "WARNING: SetStripNumberingBorders: right strip border point is incorrect\n";
        return false;
    }

    XLeftPointOfStripNumbering = XMinReadout;
    XRightPointOfStripNumbering = XMaxReadout;

    switch (left) {
        case LeftTop:
            YLeftPointOfStripNumbering = YMaxReadout;
            break;
        case LeftBottom:
            YLeftPointOfStripNumbering = YMinReadout;
            break;
        default:
            return false;
    }

    switch (right) {
        case RightTop:
            YRightPointOfStripNumbering = YMaxReadout;
            break;
        case RightBottom:
            YRightPointOfStripNumbering = YMinReadout;
            break;
        default:
            return false;
    }

    RebuildReadoutPlanes();

    return true;
}

Bool_t BmnGemStripReadoutModule::SetStripNumberingOrder(StripNumberingDirection lower_strip_direction, StripNumberingDirection upper_strip_direction) {
    LowerStripOrder = lower_strip_direction;
    UpperStripOrder = upper_strip_direction;

    RebuildReadoutPlanes();

    return true;
}

//Add single point without smearing and avalanch effects
Bool_t BmnGemStripReadoutModule::AddRealPointSimple(Double_t x, Double_t y, Double_t z,
                                                    Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    if( IsPointInsideReadoutModule(x, y) ) {

        Int_t numLowStrip = (Int_t)CalculateLowerStripZonePosition(x, y);
        Int_t numUpStrip = (Int_t)CalculateUpperStripZonePosition(x, y);

        if( (numLowStrip >= 0 && numLowStrip < ReadoutLowerPlane.size())
         && (numUpStrip  >= 0 && numUpStrip  < ReadoutUpperPlane.size()) ) {
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

//Add point with full realistic behaviour (tracking through the module, avalanche effects)
Bool_t BmnGemStripReadoutModule::AddRealPointFull(Double_t x, Double_t y, Double_t z,
                                                  Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {
    //Condition: a start point is inside the module and outside its dead zone
    if( IsPointInsideReadoutModule(x, y) ) {

        gRandom->SetSeed(0);

        //Distance from entry point (x,y,z) to exit point (along z-axis)
        Double_t z_distance_from_in_to_out;

        //Condition: track direction along z-axis
        if(pz > 0.0) {
            z_distance_from_in_to_out = ModuleThickness - (z-ZStartModulePosition);
        }
        else {
            z_distance_from_in_to_out = z - ZStartModulePosition;
        }

        //Condition: track distance along z-axis must be not zero
        if(z_distance_from_in_to_out <= 0.0) {
            if(Verbosity) cout << "WARNING: Point (" << x << " : " << y << " : " << z << ") has a track with incorrect length\n";
            return false;
        }

        //Scale coefficient of the track vector (the path from entry point to exit point)
        Double_t vector_coeff = fabs(z_distance_from_in_to_out/pz);

        //Components of the track vector
        Double_t x_vec_from_in_to_out = vector_coeff*px;
        Double_t y_vec_from_in_to_out= vector_coeff*py;
        Double_t z_vec_from_in_to_out = vector_coeff*pz;

        //Exit point coordinates (x_out, y_out, z_out)
        Double_t x_out = x + x_vec_from_in_to_out;
        Double_t y_out = y + y_vec_from_in_to_out;
        Double_t z_out = z + z_vec_from_in_to_out;

        //Check if the exit point is outside the module then calculate new x_out, y_out, z_out values
        //Condition: x-coordinate of the exit point is inside the module
        if(x_out < XMinReadout || x_out > XMaxReadout) {
            if(x_out < XMinReadout) x_out = XMinReadout;
            if(x_out > XMaxReadout) x_out = XMaxReadout;

            x_vec_from_in_to_out = x_out - x;
            vector_coeff = x_vec_from_in_to_out/px;

            y_vec_from_in_to_out= vector_coeff*py;
            z_vec_from_in_to_out = vector_coeff*pz;

            y_out = y + y_vec_from_in_to_out;
            z_out = z + z_vec_from_in_to_out;
        }
        //Condition: y-coordinate of the exit point is inside the module
        if(y_out < YMinReadout || y_out > YMaxReadout) {
            if(y_out < YMinReadout) y_out = YMinReadout;
            if(y_out > YMaxReadout) y_out = YMaxReadout;

            y_vec_from_in_to_out = y_out - y;
            vector_coeff = y_vec_from_in_to_out/py;

            x_vec_from_in_to_out= vector_coeff*px;
            z_vec_from_in_to_out = vector_coeff*pz;

            x_out = x + x_vec_from_in_to_out;
            z_out = z + z_vec_from_in_to_out;
        }
        //----------------------------------------------------------------------

        //Common track length (Fix: a square root!)
        Double_t track_length = std::sqrt( (x_vec_from_in_to_out)*(x_vec_from_in_to_out) + (y_vec_from_in_to_out)*(y_vec_from_in_to_out) + (z_vec_from_in_to_out)*(z_vec_from_in_to_out) );

        Double_t current_length = 0.0;  //traversed length (counter)
        Double_t current_length_ratio = 0.0; //ratio of the traversed length to common track length (counter)

        Int_t collisions_cnt = 0; //total collision counter
        Double_t current_step = 0.0; //current distance between two collision points

        //Collection of collision points
        std::vector<CollPoint> collision_points;

        while(current_length < track_length) {

            current_step = gRandom->Exp(MCD);
            current_length += current_step;

            if(current_length > track_length) break;

            current_length_ratio = current_length/track_length;

            Double_t current_x = x + current_length_ratio*x_vec_from_in_to_out;
            Double_t current_y = y + current_length_ratio*y_vec_from_in_to_out;
            Double_t current_z = z + current_length_ratio*z_vec_from_in_to_out;

            collision_points.push_back(CollPoint(current_x, current_y, current_z));

            collisions_cnt++;
        }


        //Each level - distance to the readout plane
        Double_t level1 = InductionGapThickness;
        Double_t level2 = InductionGapThickness+SecondTransferGapThickness;
        Double_t level3 = InductionGapThickness+SecondTransferGapThickness+FirstTransferGapThickness;
        //Double_t level4 = InductionGapThickness+SecondTransferGapThickness+FirstTransferGapThickness+DriftGapThickness; // not used yet

        //Mean electron shift along x-axis (under the influence of the Lorentz force)
        Double_t xmean; // the dependence fitted by polynomial: f(x) = (p0 + p1*x + p2*x^2 + p3*x^3)
            Double_t p0_xmean = +0.000118365;
            Double_t p1_xmean = +0.0551321;
            Double_t p2_xmean = +0.110804;
            Double_t p3_xmean = -0.0530758;

        //Sigma electron smearing
        Double_t sigma; //depends on the distance from current z-position to the readout plane

        //GEM clusters on the lower and upper planes of the readout
        StripCluster lower_cluster;
        StripCluster upper_cluster;

        //Electron avalanche producing for each collision point at the track
        for(Int_t icoll = 0; icoll < collision_points.size(); ++icoll) {

            Double_t xcoll = collision_points[icoll].x;
            Double_t ycoll = collision_points[icoll].y;
            Double_t zcoll = collision_points[icoll].z;

            Double_t zdist; // current z-distance to the readout

            //Find z-distance to the readout depending on the electron drift direction
            if(ElectronDriftDirection == ForwardZAxisEDrift) {
                zdist = (ModuleThickness + ZStartModulePosition) - zcoll;
            }
            else {
                zdist = zcoll - ZStartModulePosition;
            }
            //------------------------------------------------------------------

            xmean = p0_xmean + p1_xmean*zdist + p2_xmean*zdist*zdist + p3_xmean*zdist*zdist*zdist;
            xmean = 0; // no xmean - no shift effect //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            //Condition: beacause we have piecewise fitting function (different polynomials on each gap)
            if(zdist < 0.1) {
                sigma = std::sqrt(0.000663416*zdist);
            }
            if(zdist >= 0.1 && zdist < 0.3) {
                sigma = 0.003897 + 0.045375*zdist + (-0.03355)*zdist*zdist;
            }
            if(zdist >= 0.3 && zdist < 0.6) {
                sigma = 0.004095 + 0.0424*zdist + (-0.026)*zdist*zdist;
            }
            if(zdist >= 0.6) {
                sigma = 0.0118343 + 0.0200945*zdist + (-0.010325)*zdist*zdist;
            }


            //Number of electrons in the current collision
            Int_t n_electrons_cluster = gRandom->Landau(1.027, 0.11);
            if(n_electrons_cluster < 1) n_electrons_cluster = 1; //min
            if(n_electrons_cluster > 6) n_electrons_cluster = 6; //max

            for(Int_t ielectron = 0; ielectron < n_electrons_cluster; ++ielectron) {

                //Electron gain in each GEM cascade
                //Polya distribution is better, but Exponential is good too in our case
                Double_t gain_gem1 = gRandom->Exp(15); //...->Exp(V), where V is the mean value of the exponential distribution
                Double_t gain_gem2 = gRandom->Exp(15);
                Double_t gain_gem3 = gRandom->Exp(15);

                if(gain_gem1 < 1.0) gain_gem1 = 1.0;
                if(gain_gem2 < 1.0) gain_gem2 = 1.0;
                if(gain_gem3 < 1.0) gain_gem3 = 1.0;

                int total_gain = 0;

                if(zdist < level1) {
                    total_gain = 1.0;
                }
                if(zdist >= level1 && zdist < level2) {
                    total_gain = gain_gem3;
                }
                if(zdist >= level2 && zdist < level3) {
                    total_gain = gain_gem3*gain_gem2;
                }
                if(zdist >= level3) {
                    total_gain = gain_gem3*gain_gem2*gain_gem1;
                }

                //Projection of the current electron on the readout (x,y-coordinates)
                double x_readout, y_readout;

                for(int igain = 0; igain < total_gain; ++igain) {

                    //x-shift of the electon depends on the electron drift direction
                    if(ElectronDriftDirection == ForwardZAxisEDrift) {
                        x_readout = gRandom->Gaus(xcoll-xmean, sigma);
                    }
                    else {
                        x_readout = gRandom->Gaus(xcoll+xmean, sigma);
                    }

                    y_readout = gRandom->Gaus(ycoll, sigma);

                    //Condition: end electron position must be inside the module
                    if( IsPointInsideDeadZones(x_readout, y_readout) ) continue;

                    //Convert a coordinate position to a strip position
                    Double_t lower_strip_pos = CalculateLowerStripZonePosition(x_readout, y_readout);
                    Double_t upper_strip_pos = CalculateUpperStripZonePosition(x_readout, y_readout);

                    //Add strips with signals to lower and upper clusters
                    if( lower_strip_pos >= 0 && lower_strip_pos < ReadoutLowerPlane.size() ) {
                        lower_cluster.AddStrip((Int_t)lower_strip_pos, 1.0); //Instead of 1.0 we can use Gain in future
                    }
                    if( upper_strip_pos >= 0 && upper_strip_pos < ReadoutUpperPlane.size() ) {
                        upper_cluster.AddStrip((Int_t)upper_strip_pos, 1.0);
                    }

//Electron points on the readout (testing) -------------------------------------
                    XElectronPos.push_back(x_readout);
                    YElectronPos.push_back(y_readout);
                    ElectronSignal.push_back(1.0);
//------------------------------------------------------------------------------

                }
            }
        }

        //Condition: both clusters have to be not empty!
        if(lower_cluster.GetClusterSize() == 0 || upper_cluster.GetClusterSize() == 0) {
            if(Verbosity) cout << "WARNING: Point (" << x << " : " << y << " : " << z << ") produced an empty cluster\n";
            return false;
        }

        //Calculate cluster parameters -----------------------------------------
        Double_t lower_cluster_mean_position = 0.0;
        Double_t upper_cluster_mean_position = 0.0;

        Double_t lower_cluster_total_signal = 0.0;
        Double_t upper_cluster_total_signal = 0.0;

        for(int i = 0; i < lower_cluster.GetClusterSize(); ++i) {
            lower_cluster_mean_position += (lower_cluster.Strips[i]+0.5)*lower_cluster.Signals[i]; //as sum of all positions
            lower_cluster_total_signal += lower_cluster.Signals[i];
        }
        for(int i = 0; i < upper_cluster.GetClusterSize(); ++i) {
            upper_cluster_mean_position += (upper_cluster.Strips[i]+0.5)*upper_cluster.Signals[i]; //as sum of all positions
            upper_cluster_total_signal += upper_cluster.Signals[i];
        }

        lower_cluster_mean_position /= lower_cluster_total_signal;
        upper_cluster_mean_position /= upper_cluster_total_signal;

        lower_cluster.MeanPosition = lower_cluster_mean_position; //mean lower cluster position
        upper_cluster.MeanPosition = upper_cluster_mean_position; //mean upper cluster position

        lower_cluster.TotalSignal = lower_cluster_total_signal; //total signal of the lower cluster
        upper_cluster.TotalSignal = upper_cluster_total_signal; //total signal of the upper cluster

        if(ElectronDriftDirection == ForwardZAxisEDrift) {
            if(pz > 0.0) {
                lower_cluster.OriginPosition = CalculateLowerStripZonePosition(x, y); //position of the real(entry) point on the lower cluster
                upper_cluster.OriginPosition = CalculateUpperStripZonePosition(x, y); //position of the real(entry) point on the upper cluster
            }
            else {
                lower_cluster.OriginPosition = CalculateLowerStripZonePosition(x_out, y_out);
                upper_cluster.OriginPosition = CalculateUpperStripZonePosition(x_out, y_out);
            }
        }
        else {
            if(pz > 0.0) {
                lower_cluster.OriginPosition = CalculateLowerStripZonePosition(x_out, y_out);
                upper_cluster.OriginPosition = CalculateUpperStripZonePosition(x_out, y_out);
            }
            else {
                lower_cluster.OriginPosition = CalculateLowerStripZonePosition(x, y);
                upper_cluster.OriginPosition = CalculateUpperStripZonePosition(x, y);
            }
        }

        lower_cluster.PositionResidual = lower_cluster.MeanPosition - lower_cluster.OriginPosition; //residual between mean and origin positions (lower cluster): x-residual = x_finded(current) - x_orig(average)
        upper_cluster.PositionResidual = upper_cluster.MeanPosition - upper_cluster.OriginPosition; //residual between mean and origin positions (upper cluster): y-residual = y_finded(current) - y_orig(average)
        //----------------------------------------------------------------------

//Testing ----------------------------------------------------------------------
        LowerAddedClusters.push_back(lower_cluster);
        UpperAddedClusters.push_back(upper_cluster);
//------------------------------------------------------------------------------

        //Add the correct clusters to the readout layers -----------------------
        //lower cluster
        for(Int_t ielement = 0; ielement < lower_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = lower_cluster.Strips.at(ielement);
            Double_t strip_signal = lower_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutLowerPlane.size()) {
                ReadoutLowerPlane.at(strip_num) += strip_signal;
            }
        }
        //upper cluster
        for(Int_t ielement = 0; ielement < upper_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = upper_cluster.Strips.at(ielement);
            Double_t strip_signal = upper_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutUpperPlane.size()) {
                ReadoutUpperPlane.at(strip_num) += strip_signal;
            }
        }
        //----------------------------------------------------------------------

        //Fill strip matches ---------------------------------------------------
        //lower layer
        for(Int_t ielement = 0; ielement < lower_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = lower_cluster.Strips.at(ielement);
            Double_t strip_signal = lower_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutLowerPlane.size()) {
                LowerStripMatches.at(strip_num).AddLink(strip_signal/lower_cluster.TotalSignal, refID);
            }
        }
        //upper layer
        for(Int_t ielement = 0; ielement < upper_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = upper_cluster.Strips.at(ielement);
            Double_t strip_signal = upper_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutUpperPlane.size()) {
                UpperStripMatches.at(strip_num).AddLink(strip_signal/upper_cluster.TotalSignal, refID);
            }
        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);

        RealPointsLowerStripPos.push_back(lower_cluster.OriginPosition);
        RealPointsUpperStripPos.push_back(upper_cluster.OriginPosition);

        RealPointsLowerTotalSignal.push_back(lower_cluster.TotalSignal);
        RealPointsUpperTotalSignal.push_back(upper_cluster.TotalSignal);

        return true;
    }
    else {
        if(Verbosity) cout << "WARNING: Point (" << x << " : " << y << " : " << z << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

//Add single point with Gaussian smearing
Bool_t BmnGemStripReadoutModule::AddRealPointFullOne(Double_t x, Double_t y, Double_t z,
                                                     Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    if( IsPointInsideReadoutModule(x, y) ) {

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
        for(Int_t ielement = 0; ielement < upper_cluster.Strips.size(); ++ielement) {
            Int_t strip_num = upper_cluster.Strips.at(ielement);
            Double_t strip_signal = upper_cluster.Signals.at(ielement);
            if(strip_num >= 0 && strip_num < ReadoutUpperPlane.size()) {
                UpperStripMatches.at(strip_num).AddLink(strip_signal/upper_cluster.TotalSignal, refID);
            }
        }
        //lower layer
        for(Int_t ielement = 0; ielement < lower_cluster.Strips.size(); ++ielement) {
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
        for(Int_t ielement = 0; ielement < upper_cluster.Strips.size(); ++ielement) {
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
        for(Int_t ielement = 0; ielement < lower_cluster.Strips.size(); ++ielement) {
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
        if( CenterZonePos < 0.0 || CenterZonePos >= ReadoutLowerPlane.size() ) return cluster;
        NStripsInLayer = ReadoutLowerPlane.size();
    }

    if(layer == "upper") {
        CenterZonePos = CalculateUpperStripZonePosition(xcoord, ycoord);
        if( CenterZonePos < 0.0 || CenterZonePos >= ReadoutUpperPlane.size() ) return cluster;
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

        if(NumCurrentZone >= 0 && NumCurrentZone < NStripsInLayer && Energy > 0.0) {
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
    cluster.PositionResidual = mean_fit_pos - CenterZonePos; //residual between mean and origin positions (lower cluster): residual = finded(current) - orig(average)

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

    //Double_t threshold = LandauMPV*MinSignalCutThreshold*Gain;
    Double_t threshold = 0.0; //temporary for test

    StripCluster cluster;

    Bool_t ascent = false;
    Bool_t descent = false;

    //Processing strips
    vector<Double_t> Strips = StripLayer;

    //Smooth strip signal
    //if(Pitch > 0.079) SmoothStripSignal(Strips, 1, 1, 1.0);
    //else SmoothStripSignal(Strips, 2, 1, 1.0);

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

void BmnGemStripReadoutModule::SmoothStripSignal(vector<Double_t>& Strips, Int_t NIterations, Int_t SmoothWindow, Double_t Weight) {

    //It's Simple Moving Average method (SMA)
    //Strips - analyzable strip layer (ref)
    //NIterations - number of smooth iterations (usually 1)
    //SmoothWindow - number of strips on the left-right of the current strip (usually 1)
    //Weight - weight of the current strip (usually 1.0 - for simplicity, greater - for weighted value))

    vector<Double_t> SmoothStrips;
    Int_t NStrips = Strips.size();

    for(Int_t iteration = 0; iteration < NIterations; ++iteration) {
        SmoothStrips.clear();
        for(Int_t istrip = 0; istrip < NStrips; ++istrip) {
            Double_t mean_value = 0.0;
            for(Int_t iw = istrip-SmoothWindow; iw <= istrip+SmoothWindow; ++iw) {
                if(iw >= 0 && iw < NStrips) {
                    if(iw == istrip) mean_value += Strips[iw]*Weight;
                    else mean_value += Strips[iw];
                }
            }
            mean_value /= 2.0*SmoothWindow + Weight;
            SmoothStrips.push_back(mean_value);
        }
        Strips = SmoothStrips;
    }

    return;
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

    if(UpperStripOrder == LeftToRight) {
        XRotationCenter =  XLeftPointOfStripNumbering;
        YRotationCenter =  YLeftPointOfStripNumbering;
    }
    else {
        if(UpperStripOrder == RightToLeft) {
            XRotationCenter =  XRightPointOfStripNumbering;
            YRotationCenter =  YRightPointOfStripNumbering;
        }
    }
    Double_t UpperX = (xcoord - XRotationCenter)*Cos(-AngleRad) + (ycoord - YRotationCenter)*Sin(-AngleRad) + XRotationCenter;//see
    return UpperX;
}

Double_t BmnGemStripReadoutModule::ConvertRealPointToUpperY(Double_t xcoord, Double_t ycoord) {
    Double_t XRotationCenter;
    Double_t YRotationCenter;

    if(UpperStripOrder == LeftToRight) {
        XRotationCenter =  XLeftPointOfStripNumbering;
        YRotationCenter =  YLeftPointOfStripNumbering;
    }
    else {
        if(UpperStripOrder == RightToLeft) {
            XRotationCenter =  XRightPointOfStripNumbering;
            YRotationCenter =  YRightPointOfStripNumbering;
        }
    }
    Double_t UpperY = -(xcoord - XRotationCenter)*Sin(-AngleRad) + (ycoord - YRotationCenter)*Cos(-AngleRad) + YRotationCenter;//see
    return UpperY;
}

Double_t BmnGemStripReadoutModule::CalculateLowerStripZonePosition(Double_t xcoord, Double_t ycoord) {
    //This function returns real(double) value,
    //where integer part - number of zone (lower strip), fractional part - position in this zone (as ratio from begin)

    if(LowerStripOrder == LeftToRight) {
        return (xcoord-XLeftPointOfStripNumbering)/Pitch;
    }
    else {
        if(LowerStripOrder == RightToLeft) {
            return (XRightPointOfStripNumbering-xcoord)/Pitch;
        }
    }
}

Double_t BmnGemStripReadoutModule::CalculateUpperStripZonePosition(Double_t xcoord, Double_t ycoord) {
    //This function returns real(double) value,
    //where integer part - number of zone (upper strip), fractional part - position in this zone (as ratio from begin)

    if(UpperStripOrder == LeftToRight) {
        return (ConvertRealPointToUpperX(xcoord, ycoord)-XLeftPointOfStripNumbering)/Pitch;
    }
    else {
        if(UpperStripOrder == RightToLeft) {
            return (XRightPointOfStripNumbering-ConvertRealPointToUpperX(xcoord, ycoord))/Pitch;
        }
    }
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
    Double_t n_strips = (XRightPointOfStripNumbering - XLeftPointOfStripNumbering)/Pitch;
    if( (n_strips - (Int_t)n_strips) < 1E-10 ) {
        return (Int_t)n_strips;
    }
    else {
        return (Int_t)(n_strips+1);
    }
}

Int_t BmnGemStripReadoutModule::CountUpperStrips(){
    Double_t xleft_on_upper_layer = ConvertRealPointToUpperX(XLeftPointOfStripNumbering, YLeftPointOfStripNumbering);
    Double_t xright_on_upper_layer = ConvertRealPointToUpperX(XRightPointOfStripNumbering, YRightPointOfStripNumbering);
    Double_t n_strips = (xright_on_upper_layer - xleft_on_upper_layer)/Pitch;

    if( (n_strips - (Int_t)n_strips) < 1E-10 ) {
        return (Int_t)n_strips;
    }
    else {
        return (Int_t)(n_strips+1);
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
    if(LowerStripOrder == LeftToRight) {
        return (LowerStripZonePos*Pitch) + XLeftPointOfStripNumbering;
    }
    else {
        if(LowerStripOrder == RightToLeft) {
            return XRightPointOfStripNumbering - (LowerStripZonePos*Pitch);
        }
    }
}

Double_t BmnGemStripReadoutModule::FindYHitIntersectionPoint(Double_t LowerStripZonePos, Double_t UpperStripZonePos) {
    Double_t xcoord = FindXHitIntersectionPoint(LowerStripZonePos);
    Double_t hypoten = UpperStripZonePos*Pitch/Sin(fabs(AngleRad));
    Double_t ycoord;

    if(UpperStripOrder == LeftToRight) {

        if(AngleDeg > 0 && AngleDeg <= 90.0) {
            ycoord = Tan(PiOver2()-AngleRad)*(xcoord-XLeftPointOfStripNumbering) + (YLeftPointOfStripNumbering - hypoten);
        }

        if(AngleDeg <= 0 && AngleDeg >=-90.0) {
            ycoord = Tan(PiOver2()-AngleRad)*(xcoord-XLeftPointOfStripNumbering) + (YLeftPointOfStripNumbering + hypoten);
        }
    }
    else {
        if(UpperStripOrder == RightToLeft) {

            if(AngleDeg > 0 && AngleDeg <= 90.0) {
                ycoord = Tan(PiOver2()-AngleRad)*(xcoord-XRightPointOfStripNumbering) + (YRightPointOfStripNumbering + hypoten);
            }

            if(AngleDeg <= 0 && AngleDeg >=-90.0) {
                ycoord = Tan(PiOver2()-AngleRad)*(xcoord-XRightPointOfStripNumbering) + (YRightPointOfStripNumbering - hypoten);
            }
        }
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

            //if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) && !DeadZone.IsInside(xcoord, ycoord) ) {
            if( IsPointInsideReadoutModule(xcoord, ycoord) ) {
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

ClassImp(BmnGemStripReadoutModule)