#include <fstream>

#include "BmnCSCModule.h"

#include "TMath.h"
#include "TRandom.h"
#include "TF1.h"
#include "TH1F.h"

BmnCSCModule::BmnCSCModule() {

    Verbosity = true;

    ZStartModulePosition = 0.0;

    ModuleThickness = 0.76; //cm

    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;
}

BmnCSCModule::BmnCSCModule(Double_t z_start_pos, Double_t module_thick) {

    Verbosity = true;

    ZStartModulePosition = z_start_pos;

    ModuleThickness = module_thick; //cm

    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;
}

BmnCSCModule::~BmnCSCModule() {

}

Double_t BmnCSCModule::GetZPositionRegistered() {

    return ZStartModulePosition+ModuleThickness*0.5;
}

void BmnCSCModule::AddStripLayer(BmnCSCLayer strip_layer) {
    StripLayers.push_back(strip_layer);

    DefineModuleBorders();
}

Bool_t BmnCSCModule::SetStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].SetStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnCSCModule::AddStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].AddStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnCSCModule::SetStripMatchInLayer(Int_t layer_num, Int_t strip_num, BmnMatch strip_match) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].SetStripMatch(strip_num, strip_match);
    }
    return false;
}

Double_t BmnCSCModule::GetStripSignalInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].GetStripSignal(strip_num);
    }
    return -1;
}

BmnMatch BmnCSCModule::GetStripMatchInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].GetStripMatch(strip_num);
    }
    return BmnMatch();
}

void BmnCSCModule::ResetModuleData() {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        StripLayers[ilayer].ResetLayer();
    }

    DefineModuleBorders();

    ResetRealPoints();
    ResetIntersectionPoints();
}

Bool_t BmnCSCModule::IsPointInsideModule(Double_t x, Double_t y) {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( StripLayers[ilayer].IsPointInsideStripLayer(x, y) ) return true;
    }
    return false;
}

Bool_t BmnCSCModule::IsPointInsideModule(Double_t x, Double_t y, Double_t z) {
    Double_t coord_eps = 0.01; //100 um
    if( ((z > ZStartModulePosition) || (fabs(z - ZStartModulePosition) < coord_eps)) &&
        ((z < (ZStartModulePosition + ModuleThickness)) || (fabs(z - (ZStartModulePosition + ModuleThickness))) < coord_eps) ) {
        if( IsPointInsideModule(x, y) ) return true;
    }
    return false;
}

Bool_t BmnCSCModule::IsPointInsideZThickness(Double_t z) {
    Double_t coord_eps = 0.01; //100 um
    if( ((z > ZStartModulePosition) || (fabs(z - ZStartModulePosition) < coord_eps)) &&
        ((z < (ZStartModulePosition + ModuleThickness)) || (fabs(z - (ZStartModulePosition + ModuleThickness))) < coord_eps) ) {
        return true;
    }
    return false;
}

//Add single point with Gaussian smearing
Bool_t BmnCSCModule::AddRealPointFullOne(Double_t x, Double_t y, Double_t z,
                                         Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    if( IsPointInsideModule(x, y) ) {

        if(signal <= 0.0) signal = 1e-16;

        Double_t AvalancheRadius = 0.01; //cm

        Double_t radius = AvalancheRadius;
        if(radius <= 0.0) return false;

        gRandom->SetSeed(0);

        Int_t cycle_cnt = 0;
        while(true) {
            radius = gRandom->Gaus(AvalancheRadius, 0.005);
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
        if(Verbosity) cerr << "WARNING: BmnCSCModule::AddRealPointFullOne(): Point (" << x << " : " << y << " : " << z << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

//Add single point without smearing and avalanch effects
Bool_t BmnCSCModule::AddRealPointSimple(Double_t x, Double_t y, Double_t z,
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
        if(Verbosity) cerr << "WARNING: BmnCSCModule::AddRealPointSimple(): Point (" << x << " : " << y << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

StripCluster BmnCSCModule::MakeCluster(Int_t layer_num, Double_t xcoord, Double_t ycoord, Double_t signal, Double_t radius) {

    Double_t ClusterDistortion = 0.0; //signal variation (0.1 is 10%)

    Double_t Gain = 1.0;

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

void BmnCSCModule::CalculateStripHitIntersectionPoints() {

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

                        //intersection matching ----------------------------------------
                        BmnMatch iStripMatch = StripLayers[ilayer].GetStripMatch((Int_t)iStripHitPos);
                        BmnMatch jStripMatch = StripLayers[jlayer].GetStripMatch((Int_t)jStripHitPos);

                        BmnMatch intersection_match;
                        intersection_match.AddLink(iStripMatch);
                        intersection_match.AddLink(jStripMatch);

                        IntersectionPointMatches.push_back(intersection_match);
                        //--------------------------------------------------------------

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
Bool_t BmnCSCModule::SearchIntersectionPoint(Double_t &x, Double_t &y, Double_t strip_pos_layerA, Double_t strip_pos_layerB, Int_t layerA_index, Int_t layerB_index) {

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

void BmnCSCModule::ResetIntersectionPoints() {
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
    UpperClusters.clear();
    LowerClusters.clear();
}

void BmnCSCModule::DefineModuleBorders() {

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

ClassImp(BmnCSCModule)
