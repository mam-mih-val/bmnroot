#include "BmnGemStripModule.h"

#include "TRandom.h"
#include "TF1.h"
#include "TH1F.h"

BmnGemStripModule::BmnGemStripModule() {

    Verbosity = true;

    ZStartModulePosition = 0.0;

    DriftGapThickness = 0.3; //cm
    FirstTransferGapThickness = 0.25; //cm
    SecondTransferGapThickness = 0.2; //cm
    InductionGapThickness = 0.15; //cm
    ModuleThickness = DriftGapThickness + FirstTransferGapThickness + SecondTransferGapThickness + InductionGapThickness; //cm

    ElectronDriftDirection = ForwardZAxisEDrift;

    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;

    AvalancheGenerationSeed = 0;
    AvalancheRadius = 0.10; //cm
    Gain = 1.0; //gain level (for each electron signal - in RealPointFull or for strip signal - in RealPointFullOne)
}

BmnGemStripModule::BmnGemStripModule(Double_t z_start_pos,
                                     ElectronDriftDirectionInModule edrift_direction,
                                     Double_t DriftGap, Double_t FTransferGap, Double_t STransferGap, Double_t InductionGap) {

    Verbosity = true;

    ZStartModulePosition = z_start_pos;

    DriftGapThickness = DriftGap; //cm
    FirstTransferGapThickness = FTransferGap; //cm
    SecondTransferGapThickness = STransferGap; //cm
    InductionGapThickness = InductionGap; //cm
    ModuleThickness = DriftGapThickness + FirstTransferGapThickness + SecondTransferGapThickness + InductionGapThickness; //cm

    ElectronDriftDirection = edrift_direction;

    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;

    AvalancheGenerationSeed = 0;
    AvalancheRadius = 0.10; //cm
    Gain = 1.0;
}

BmnGemStripModule::~BmnGemStripModule() {

}

Double_t BmnGemStripModule::GetZPositionRegistered() {
    if(ElectronDriftDirection == ForwardZAxisEDrift) {
        return ZStartModulePosition+DriftGapThickness*0.5;
    }
    else {
        return ZStartModulePosition+ModuleThickness-DriftGapThickness*0.5;
    }
}

void BmnGemStripModule::AddStripLayer(BmnGemStripLayer strip_layer) {
    StripLayers.push_back(strip_layer);

    DefineModuleBorders();
}

Bool_t BmnGemStripModule::SetStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].SetStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnGemStripModule::AddStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].AddStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnGemStripModule::SetStripMatchInLayer(Int_t layer_num, Int_t strip_num, BmnMatch mc_match) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].SetStripMatch(strip_num, mc_match);
    }
    return false;
}

Bool_t BmnGemStripModule::SetStripDigitNumberMatchInLayer(Int_t layer_num, Int_t strip_num, BmnMatch digit_num_match) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].SetStripDigitNumberMatch(strip_num, digit_num_match);
    }
    return false;
}

Double_t BmnGemStripModule::GetStripSignalInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].GetStripSignal(strip_num);
    }
    return -1;
}

BmnMatch BmnGemStripModule::GetStripMatchInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].GetStripMatch(strip_num);
    }
    return BmnMatch();
}

BmnMatch BmnGemStripModule::GetStripDigitNumberMatchInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].GetStripDigitNumberMatch(strip_num);
    }
    return BmnMatch();
}

void BmnGemStripModule::ResetModuleData() {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        StripLayers[ilayer].ResetLayer();
    }

    DefineModuleBorders();

    ResetRealPoints();
    ResetIntersectionPoints();

    ResetElectronPointsAndClusters(); //test
}

Bool_t BmnGemStripModule::IsPointInsideModule(Double_t x, Double_t y) {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( StripLayers[ilayer].IsPointInsideStripLayer(x, y) ) return true;
    }
    return false;
}

Bool_t BmnGemStripModule::IsPointInsideModule(Double_t x, Double_t y, Double_t z) {
    //old condition: if( z >= ZStartModulePosition && z <= (ZStartModulePosition+ModuleThickness) )
    Double_t coord_eps = 0.01; //100 um
    if( ((z > ZStartModulePosition) || (fabs(z - ZStartModulePosition) < coord_eps)) &&
        ((z < (ZStartModulePosition + ModuleThickness)) || (fabs(z - (ZStartModulePosition + ModuleThickness))) < coord_eps) ) {
        if( IsPointInsideModule(x, y) ) return true;
    }
    return false;
}

Bool_t BmnGemStripModule::IsPointInsideZThickness(Double_t z) {
    Double_t coord_eps = 0.01; //100 um
    if( ((z > ZStartModulePosition) || (fabs(z - ZStartModulePosition) < coord_eps)) &&
        ((z < (ZStartModulePosition + ModuleThickness)) || (fabs(z - (ZStartModulePosition + ModuleThickness))) < coord_eps) ) {
        return true;
    }
    return false;
}

//Add point with full realistic behaviour (tracking through the module, avalanche effects)
Bool_t BmnGemStripModule::AddRealPointFull(Double_t x, Double_t y, Double_t z,
                                                  Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    //Condition: a start point is inside the module and outside its dead zone (in strip layers)
    if( IsPointInsideModule(x, y) ) {

      //AZ-260322 gRandom->SetSeed(AvalancheGenerationSeed);

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
            if(Verbosity) cerr << "WARNING: BmnGemStripModule::AddRealPointFull(): Point (" << x << " : " << y << " : " << z << ") has a track with an incorrect length\n";
            return false;
        }

        //Scale coefficient of the track vector (the path from entry point to exit point)
        Double_t vector_coeff = fabs(z_distance_from_in_to_out/pz);

        //Components of the track vector
        Double_t x_vec_from_in_to_out = vector_coeff*px;
        Double_t y_vec_from_in_to_out = vector_coeff*py;
        Double_t z_vec_from_in_to_out = vector_coeff*pz;

        //Exit point coordinates (x_out, y_out, z_out)
        Double_t x_out = x + x_vec_from_in_to_out;
        Double_t y_out = y + y_vec_from_in_to_out;
        Double_t z_out = z + z_vec_from_in_to_out;

        //Check if the exit point is outside the module then calculate new x_out, y_out, z_out values
        //Condition: x-coordinate of the exit point is inside the module
        if(x_out < XMinModule || x_out > XMaxModule ) {
            if(x_out < XMinModule ) x_out = XMinModule;
            if(x_out > XMaxModule ) x_out = XMaxModule;

            x_vec_from_in_to_out = x_out - x;
            vector_coeff = x_vec_from_in_to_out/px;

            y_vec_from_in_to_out= vector_coeff*py;
            z_vec_from_in_to_out = vector_coeff*pz;

            y_out = y + y_vec_from_in_to_out;
            z_out = z + z_vec_from_in_to_out;
        }
        //Condition: y-coordinate of the exit point is inside the module
        if(y_out < YMinModule  || y_out > YMaxModule ) {
            if(y_out < YMinModule ) y_out = YMinModule;
            if(y_out > YMaxModule ) y_out = YMaxModule;

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

            //Reading the MCD value (mean collision distance = mean free flight path) [cm]
            MCD = BmnGemStripMedium::GetInstance().MCD;

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

        //Sigma electron smearing
        Double_t sigma; // the dependence fitted by polynomial: f(x) = (p0 + p1*x + p2*x^2 + p3*x^3+ p4*x^4 + p5*x^5)

        //cout << "$$$ BmnGemStripMedium::GetInstance().NameOfCurrentMedium = " << BmnGemStripMedium::GetInstance().NameOfCurrentMedium << "\n"; //test cout

        //Reading the medium configuration -------------------------------------
        Double_t p0_xmean = BmnGemStripMedium::GetInstance().p0_xmean;
        Double_t p1_xmean = BmnGemStripMedium::GetInstance().p1_xmean;
        Double_t p2_xmean = BmnGemStripMedium::GetInstance().p2_xmean;
        Double_t p3_xmean = BmnGemStripMedium::GetInstance().p3_xmean;

        Double_t p0_sigma = BmnGemStripMedium::GetInstance().p0_sigma;
        Double_t p1_sigma = BmnGemStripMedium::GetInstance().p1_sigma;
        Double_t p2_sigma = BmnGemStripMedium::GetInstance().p2_sigma;
        Double_t p3_sigma = BmnGemStripMedium::GetInstance().p3_sigma;
        Double_t p4_sigma = BmnGemStripMedium::GetInstance().p4_sigma;
        Double_t p5_sigma = BmnGemStripMedium::GetInstance().p5_sigma;
        //----------------------------------------------------------------------

        //Strip cluster in each strip layer
        Int_t n_strip_layers = StripLayers.size();
        vector<StripCluster> cluster_layers(n_strip_layers, StripCluster());

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

            //polynomials
            xmean = p0_xmean + p1_xmean*zdist + p2_xmean*zdist*zdist + p3_xmean*zdist*zdist*zdist;
            sigma = p0_sigma + p1_sigma*zdist + p2_sigma*zdist*zdist + p3_sigma*zdist*zdist*zdist + p4_sigma*zdist*zdist*zdist*zdist + p5_sigma*zdist*zdist*zdist*zdist*zdist;

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
                        x_readout = gRandom->Gaus(xcoll+xmean, sigma);
                    }
                    else {
                        x_readout = gRandom->Gaus(xcoll-xmean, sigma);
                    }

                    y_readout = gRandom->Gaus(ycoll, sigma);

                    for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
                        //Condition: end electron position must be inside the layer
                        if( !StripLayers[ilayer].IsPointInsideStripLayer(x_readout, y_readout) ) continue;

                        Double_t strip_pos = StripLayers[ilayer].ConvertPointToStripPosition(x_readout, y_readout);

                        //Add strips with signals to lower and upper clusters
                        //StripLayers[ilayer].AddStripSignal((Int_t)strip_pos, 1.0); //Instead of 1.0 we can use Gain in future
                        cluster_layers[ilayer].AddStrip((Int_t)strip_pos, 1.0); //Instead of 1.0 we can use Gain in future
                    }
/*
//Electron points on the readout (testing) -------------------------------------
                    XElectronPos.push_back(x_readout);
                    YElectronPos.push_back(y_readout);
                    ElectronSignal.push_back(1.0);
//------------------------------------------------------------------------------
*/
                }
            }
        }

        //Calculate cluster parameters -----------------------------------------
        vector<Double_t> cluster_mean_position(n_strip_layers, 0.0);
        vector<Double_t> cluster_total_signal(n_strip_layers, 0.0);

        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {

            if(cluster_layers[ilayer].GetClusterSize() > 0) {
                for(int i = 0; i < cluster_layers[ilayer].GetClusterSize(); ++i) {
                    cluster_mean_position[ilayer] += (cluster_layers[ilayer].Strips[i]+0.5)*cluster_layers[ilayer].Signals[i]; //as sum of all positions
                    cluster_total_signal[ilayer] += cluster_layers[ilayer].Signals[i];
                }
                cluster_mean_position[ilayer] /= cluster_total_signal[ilayer];
            }

            cluster_layers[ilayer].MeanPosition = cluster_mean_position[ilayer]; //mean cluster position
            cluster_layers[ilayer].TotalSignal = cluster_total_signal[ilayer]; //total signal of the cluster
        }

        if(ElectronDriftDirection == ForwardZAxisEDrift) {
            if(pz > 0.0) {
                for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
                    cluster_layers[ilayer].OriginPosition = StripLayers[ilayer].ConvertPointToStripPosition(x, y);
                }
            }
            else {
                for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
                    cluster_layers[ilayer].OriginPosition = StripLayers[ilayer].ConvertPointToStripPosition(x_out, y_out);
                }
            }
        }
        else {
            if(pz > 0.0) {
                for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
                    cluster_layers[ilayer].OriginPosition = StripLayers[ilayer].ConvertPointToStripPosition(x_out, y_out);
                }
            }
            else {
                for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
                    cluster_layers[ilayer].OriginPosition = StripLayers[ilayer].ConvertPointToStripPosition(x, y);
                }
            }
        }

        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            cluster_layers[ilayer].PositionResidual = cluster_layers[ilayer].MeanPosition - cluster_layers[ilayer].OriginPosition;
        }
        //----------------------------------------------------------------------
/*
//Testing ----------------------------------------------------------------------
        if(AddedClusters.size() == 0) {
            for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
                AddedClusters.push_back(vector<StripCluster>());
            }
        }

        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            AddedClusters[ilayer].push_back(cluster_layers[ilayer]);
        }
//------------------------------------------------------------------------------
*/
        //Add the correct clusters to the strip layers -------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(Int_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                StripLayers[ilayer].AddStripSignal(strip_num, strip_signal);
            }
        }
        //----------------------------------------------------------------------

        //Fill strip matches ---------------------------------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(Int_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                StripLayers[ilayer].AddLinkToStripMatch(strip_num, strip_signal/cluster_layers[ilayer].TotalSignal, refID);
            }
        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);
        RealPointsMC.push_back(refID);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnGemStripModule::AddRealPointFull(): Point (" << x << " : " << y << " : " << z << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

//Add single point with Gaussian smearing
Bool_t BmnGemStripModule::AddRealPointFullOne(Double_t x, Double_t y, Double_t z,
                               Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    if( IsPointInsideModule(x, y) ) {

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

        //Make strip cluster in each strip layer -------------------------------
        Int_t n_strip_layers = StripLayers.size();
        vector<StripCluster> cluster_layers(n_strip_layers, StripCluster());

        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            if( !StripLayers[ilayer].IsPointInsideStripLayer(x,y) ) continue;
            cluster_layers[ilayer] = MakeCluster(ilayer, x, y, signal, radius);
        }
        //----------------------------------------------------------------------

//Testing ----------------------------------------------------------------------
        /*if(AddedClusters.size() == 0) {
            for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
                AddedClusters.push_back(vector<StripCluster>());
            }
        }

        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            AddedClusters[ilayer].push_back(cluster_layers[ilayer]);
        }*/
//------------------------------------------------------------------------------

        //Add the correct clusters to the strip layers -------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(Int_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                StripLayers[ilayer].AddStripSignal(strip_num, strip_signal);
            }
        }
        //----------------------------------------------------------------------

        //Fill strip matches ---------------------------------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(Int_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                StripLayers[ilayer].AddLinkToStripMatch(strip_num, strip_signal/cluster_layers[ilayer].TotalSignal, refID);
            }
        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);
        RealPointsMC.push_back(refID);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnGemStripModule::AddRealPointFullOne(): Point (" << x << " : " << y << " : " << z << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

//Add single point without smearing and avalanch effects
Bool_t BmnGemStripModule::AddRealPointSimple(Double_t x, Double_t y, Double_t z,
                                             Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    if( IsPointInsideModule(x, y) ) {

        Int_t n_strip_layers = StripLayers.size();

        //Add a signal to the strips layers ------------------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {

            if( !StripLayers[ilayer].IsPointInsideStripLayer(x,y) ) continue;

            Double_t strip_pos = StripLayers[ilayer].ConvertPointToStripPosition(x, y);
            StripLayers[ilayer].AddStripSignal((Int_t)strip_pos, signal);

            //strip match
            StripLayers[ilayer].AddLinkToStripMatch((Int_t)strip_pos, 1.0, refID);
        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);
        RealPointsMC.push_back(refID);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnGemStripModule::AddRealPointSimple(): Point (" << x << " : " << y << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

StripCluster BmnGemStripModule::MakeCluster(Int_t layer_num, Double_t xcoord, Double_t ycoord, Double_t signal, Double_t radius) {

    Double_t ClusterDistortion = 0.0; //signal variation (0.1 is 10%)

    StripCluster cluster;

    if(radius <= 0.0) return cluster;

    Double_t Pitch = StripLayers[layer_num].GetPitch();

    Double_t RadiusInZones = radius/Pitch;
    Double_t Sigma = RadiusInZones/3.33;

    TF1 gausF("gausF", "gaus", -4*Sigma, 4*Sigma);
    gausF.SetParameter(0, 1.0); // constant (altitude)
    gausF.SetParameter(1, 0.0); // mean (center position)
    gausF.SetParameter(2, Sigma); //sigma

    Double_t SCluster = gausF.Integral(-4*Sigma, 4*Sigma); //square of the one side distribution (more exactly)

    TRandom rand(0);
    Double_t var_level = ClusterDistortion; //signal variation (0.1 is 10%)

    Int_t NStripsInLayer = StripLayers[layer_num].GetNStrips();

    if(NStripsInLayer == 0) return cluster;

    Double_t CenterZonePos = StripLayers[layer_num].ConvertPointToStripPosition(xcoord, ycoord);

    if( CenterZonePos < 0.0 || CenterZonePos >= NStripsInLayer ) return cluster;


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

    cluster.IsCorrect = true; // set correct status of the cluster
    return cluster;
}

void BmnGemStripModule::SetAvalancheRadius(Double_t aval_radius) {
    if(aval_radius >= 0.0) AvalancheRadius = aval_radius;
    else {
        if(Verbosity) cerr << "WARNING: BmnGemStripModule::SetAvalancheRadius(): incorrect value for AvalancheRadius! It'll be set by default!\n";
        AvalancheRadius = 0.1; //default
    }
}

Double_t BmnGemStripModule::GetAvalancheRadius() {
    return AvalancheRadius;
}

void BmnGemStripModule::CalculateStripHitIntersectionPoints() {

    ResetIntersectionPoints();

    Int_t n_strip_layers = StripLayers.size();

    //Find strip clusters and hits in the strip layers -------------------------
    for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
        StripLayers[ilayer].FindClustersAndStripHits();
    }
    //--------------------------------------------------------------------------

    for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {

        for(Int_t jlayer = ilayer+1; jlayer < n_strip_layers; ++jlayer) {
            //cout << "(i-layer : j-layer) = ( " << ilayer << " : " << jlayer << " )\n";

            if(StripLayers[ilayer].GetType() == StripLayers[jlayer].GetType()) continue;

            Double_t iAngleRad = StripLayers[ilayer].GetAngleRad();
            Double_t jAngleRad = StripLayers[jlayer].GetAngleRad();

            if( Abs(iAngleRad - jAngleRad) < 0.01 ) continue;

            Double_t i_PiOver2MinusAngleRad = PiOver2() - iAngleRad;
            Double_t j_PiOver2MinusAngleRad = PiOver2() - jAngleRad;

            for(Int_t i_strip_hit = 0; i_strip_hit < StripLayers[ilayer].GetNStripHits(); ++i_strip_hit) {
                for(Int_t j_strip_hit = 0; j_strip_hit < StripLayers[jlayer].GetNStripHits(); ++j_strip_hit) {

                    Double_t iStripHitPos = StripLayers[ilayer].GetStripHitPos(i_strip_hit);
                    Double_t jStripHitPos = StripLayers[jlayer].GetStripHitPos(j_strip_hit);

                    Double_t iStripHitError = StripLayers[ilayer].GetStripHitError(i_strip_hit);
                    Double_t jStripHitError = StripLayers[jlayer].GetStripHitError(j_strip_hit);

                    Double_t xcoord;
                    Double_t ycoord;

                    if( Abs(iAngleRad) >= 0.01 && Abs(jAngleRad) >= 0.01 ) {

                        Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
                        Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);

                        xcoord = (jB - iB)/(Tan(i_PiOver2MinusAngleRad) - Tan(j_PiOver2MinusAngleRad));
                        ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
                    }
                    else {

                        if( Abs(iAngleRad) < 0.01 ) {

                            if(StripLayers[ilayer].GetStripNumberingOrder() == LeftToRight ) {
                                xcoord = StripLayers[ilayer].GetXLeftStripBorderPoint() + iStripHitPos*StripLayers[ilayer].GetPitch();
                            }
                            else {
                                xcoord = StripLayers[ilayer].GetXRightStripBorderPoint() - iStripHitPos*StripLayers[ilayer].GetPitch();
                            }

                            Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);
                            ycoord = Tan(j_PiOver2MinusAngleRad)*xcoord + jB;
                        }

                        if( Abs(jAngleRad) < 0.01 ) {

                            if(StripLayers[jlayer].GetStripNumberingOrder() == LeftToRight ) {
                                xcoord = StripLayers[jlayer].GetXLeftStripBorderPoint() + jStripHitPos*StripLayers[jlayer].GetPitch();
                            }
                            else {
                                xcoord = StripLayers[jlayer].GetXRightStripBorderPoint() - jStripHitPos*StripLayers[jlayer].GetPitch();
                            }

                            Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
                            ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
                        }
                    }

                    //check: a point belongs to both strip layers together
                    if( StripLayers[ilayer].IsPointInsideStripLayer(xcoord, ycoord) && StripLayers[jlayer].IsPointInsideStripLayer(xcoord, ycoord) ) {

                        IntersectionPointsX.push_back(xcoord);
                        IntersectionPointsY.push_back(ycoord);

                        //Intersection (x,y)-errors from the strip-errors ------
                        Double_t AngleIntersecRad = Abs(iAngleRad - jAngleRad);

                        //a rhomb zone is the intersection of the strip errors
                        Double_t RhombSide1 = (iStripHitError*StripLayers[ilayer].GetPitch())/Sin(AngleIntersecRad);
                        Double_t RhombSide2 = (jStripHitError*StripLayers[jlayer].GetPitch())/Sin(AngleIntersecRad);

                        //x-strip error
                        Double_t xerr1 = Sin(Abs(jAngleRad))*RhombSide1;
                        Double_t xerr2 = Sin(Abs(iAngleRad))*RhombSide2;
                        Double_t xcoord_err = xerr1 + xerr2;

                        //y-strip error
                        Double_t yerr1 = Cos(Abs(jAngleRad))*RhombSide1;
                        Double_t yerr2 = Cos(Abs(iAngleRad))*RhombSide2;
                        Double_t ycoord_err = yerr1 + yerr2;

                        IntersectionPointsXErrors.push_back(xcoord_err);
                        IntersectionPointsYErrors.push_back(ycoord_err);
                        //------------------------------------------------------

                        //intersection MC-matching -----------------------------
                        BmnMatch iStripMatch = StripLayers[ilayer].GetStripMatch((Int_t)iStripHitPos);
                        BmnMatch jStripMatch = StripLayers[jlayer].GetStripMatch((Int_t)jStripHitPos);

                        BmnMatch intersection_match;
                        intersection_match.AddLink(iStripMatch);
                        intersection_match.AddLink(jStripMatch);

                        IntersectionPointMatches.push_back(intersection_match);
                        //------------------------------------------------------

                        //intersection digit number matching -------------------
                        BmnMatch iStripDigitNumberMatch = StripLayers[ilayer].GetStripDigitNumberMatch((Int_t)iStripHitPos);
                        BmnMatch jStripDigitNumberMatch = StripLayers[jlayer].GetStripDigitNumberMatch((Int_t)jStripHitPos);

                        BmnMatch intersection_digit_num_match;
                        intersection_digit_num_match.AddLink(iStripDigitNumberMatch);
                        intersection_digit_num_match.AddLink(jStripDigitNumberMatch);

                        IntersectionPointDigitNumberMatches.push_back(intersection_digit_num_match);
                        //------------------------------------------------------

                        //Additional information about the intersection ------------
                            //cluster size (number strips) in both strip layers for each intersection
                            Int_t iLayerClusterSize = StripLayers[ilayer].GetStripHitClusterSize(i_strip_hit);
                            Int_t jLayerClusterSize = StripLayers[jlayer].GetStripHitClusterSize(j_strip_hit);

                            UpperClusters.push_back(StripLayers[jlayer].GetStripClusters().at(j_strip_hit));
                            LowerClusters.push_back(StripLayers[ilayer].GetStripClusters().at(i_strip_hit));

                            //strip position in both strip layers for each intersection
                            Double_t iLayerStripPosition = StripLayers[ilayer].ConvertPointToStripPosition(xcoord, ycoord);
                            Double_t jLayerStripPosition = StripLayers[jlayer].ConvertPointToStripPosition(xcoord, ycoord);

                            IntersectionPoints_LowerLayerStripTotalSignal.push_back(StripLayers[ilayer].GetStripHitTotalSignal(i_strip_hit));
                            IntersectionPoints_UpperLayerStripTotalSignal.push_back(StripLayers[jlayer].GetStripHitTotalSignal(j_strip_hit));

                            if(StripLayers[ilayer].GetType() == LowerStripLayer) {
                                IntersectionPoints_LowerLayerClusterSize.push_back(iLayerClusterSize);
                                IntersectionPoints_UpperLayerClusterSize.push_back(jLayerClusterSize);
                                IntersectionPoints_LowerLayerStripPosition.push_back(iLayerStripPosition);
                                IntersectionPoints_UpperLayerStripPosition.push_back(jLayerStripPosition);
                            }
                            else {
                                IntersectionPoints_LowerLayerClusterSize.push_back(jLayerClusterSize);
                                IntersectionPoints_UpperLayerClusterSize.push_back(iLayerClusterSize);
                                IntersectionPoints_LowerLayerStripPosition.push_back(jLayerStripPosition);
                                IntersectionPoints_UpperLayerStripPosition.push_back(iLayerStripPosition);
                            }
                        //------------------------------------------------------
                    }
                }
            }
        }
    }
}

//need for a separated test (find x,y intersection coords from strip positions)
Bool_t BmnGemStripModule::SearchIntersectionPoint(Double_t &x, Double_t &y, Double_t strip_pos_layerA, Double_t strip_pos_layerB, Int_t layerA_index, Int_t layerB_index) {

    Int_t ilayer = layerA_index;
    Int_t jlayer = layerB_index;

    if(StripLayers[ilayer].GetType() == StripLayers[jlayer].GetType()) return false;

    Double_t iAngleRad = StripLayers[ilayer].GetAngleRad();
    Double_t jAngleRad = StripLayers[jlayer].GetAngleRad();

    if( Abs(iAngleRad - jAngleRad) < 0.01 ) return false;

    Double_t i_PiOver2MinusAngleRad = PiOver2() - iAngleRad;
    Double_t j_PiOver2MinusAngleRad = PiOver2() - jAngleRad;

    Double_t iStripHitPos = strip_pos_layerA;
    Double_t jStripHitPos = strip_pos_layerB;

    Double_t xcoord;
    Double_t ycoord;

    if( Abs(iAngleRad) >= 0.01 && Abs(jAngleRad) >= 0.01 ) {

        Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
        Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);

        xcoord = (jB - iB)/(Tan(i_PiOver2MinusAngleRad) - Tan(j_PiOver2MinusAngleRad));
        ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
    }
    else {

        if( Abs(iAngleRad) < 0.01 ) {

            if(StripLayers[ilayer].GetStripNumberingOrder() == LeftToRight ) {
                xcoord = StripLayers[ilayer].GetXLeftStripBorderPoint() + iStripHitPos*StripLayers[ilayer].GetPitch();
            }
            else {
                xcoord = StripLayers[ilayer].GetXRightStripBorderPoint() - iStripHitPos*StripLayers[ilayer].GetPitch();
            }

            Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);
            ycoord = Tan(j_PiOver2MinusAngleRad)*xcoord + jB;
        }

        if( Abs(jAngleRad) < 0.01 ) {

            if(StripLayers[jlayer].GetStripNumberingOrder() == LeftToRight ) {
                xcoord = StripLayers[jlayer].GetXLeftStripBorderPoint() + jStripHitPos*StripLayers[jlayer].GetPitch();
            }
            else {
                xcoord = StripLayers[jlayer].GetXRightStripBorderPoint() - jStripHitPos*StripLayers[jlayer].GetPitch();
            }

            Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
            ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
        }
    }

    x = xcoord;
    y = ycoord;

    //check: a point belongs to both strip layers together
    if( StripLayers[ilayer].IsPointInsideStripLayer(xcoord, ycoord) && StripLayers[jlayer].IsPointInsideStripLayer(xcoord, ycoord) ) {
        return true;
    }

    return false;
}

void BmnGemStripModule::ResetIntersectionPoints() {
    IntersectionPointsX.clear();
    IntersectionPointsY.clear();
    IntersectionPointsXErrors.clear();
    IntersectionPointsYErrors.clear();
    IntersectionPoints_LowerLayerClusterSize.clear();
    IntersectionPoints_UpperLayerClusterSize.clear();
    IntersectionPoints_LowerLayerStripPosition.clear();
    IntersectionPoints_UpperLayerStripPosition.clear();
    IntersectionPoints_LowerLayerStripTotalSignal.clear();
    IntersectionPoints_UpperLayerStripTotalSignal.clear();
    IntersectionPointMatches.clear();
    IntersectionPointDigitNumberMatches.clear();
    UpperClusters.clear();
    LowerClusters.clear();
}

void BmnGemStripModule::DefineModuleBorders() {

    if( StripLayers.size() == 0 ) return;

    XMinModule = 1.0E+10;
    XMaxModule = -1.0E+10;
    YMinModule = 1.0E+10;
    YMaxModule = -1.0E+10;

    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( StripLayers[ilayer].GetXMinLayer() < XMinModule ) XMinModule = StripLayers[ilayer].GetXMinLayer();
        if( StripLayers[ilayer].GetXMaxLayer() > XMaxModule ) XMaxModule = StripLayers[ilayer].GetXMaxLayer();
        if( StripLayers[ilayer].GetYMinLayer() < YMinModule ) YMinModule = StripLayers[ilayer].GetYMinLayer();
        if( StripLayers[ilayer].GetYMaxLayer() > YMaxModule ) YMaxModule = StripLayers[ilayer].GetYMaxLayer();
    }
    return;
}

//------------------------------------------------------------------------------

ClassImp(BmnGemStripModule)
