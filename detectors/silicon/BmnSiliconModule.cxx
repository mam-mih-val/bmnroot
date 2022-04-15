#include "BmnSiliconModule.h"

#include "TMath.h"
#include "TRandom.h"
#include "TF1.h"
#include "TH1F.h"

#include <iostream>

using namespace TMath;
using namespace std;

BmnSiliconModule::BmnSiliconModule() {

    Verbosity = true;

    ZStartModulePosition = 0.0;
    ModuleThickness = 0.03; //cm

    ModuleRotationAlgleDeg = 0.0;
    ModuleRotationAlgleRad = ModuleRotationAlgleDeg*TMath::DegToRad();
    ModuleRotationCenterX = 0.0;
    ModuleRotationCenterY = 0.0;

    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;
}

BmnSiliconModule::BmnSiliconModule(Double_t z_start_pos) {

    Verbosity = true;

    ZStartModulePosition = z_start_pos;
    ModuleThickness = 0.03; //cm

    ModuleRotationAlgleDeg = 0.0;
    ModuleRotationAlgleRad = ModuleRotationAlgleDeg*TMath::DegToRad();
    ModuleRotationCenterX = 0.0;
    ModuleRotationCenterY = 0.0;

    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;
}

BmnSiliconModule::~BmnSiliconModule() {

}

Double_t BmnSiliconModule::GetZPositionRegistered() {
    return ZStartModulePosition + ModuleThickness*0.5;
}

Bool_t BmnSiliconModule::SetModuleRotation(Double_t angleDeg, Double_t rot_center_x, Double_t rot_center_y) {
    ModuleRotationAlgleDeg = angleDeg;
    ModuleRotationAlgleRad = ModuleRotationAlgleDeg*TMath::DegToRad();
    ModuleRotationCenterX = rot_center_x;
    ModuleRotationCenterY = rot_center_y;
    return true;
}

void BmnSiliconModule::AddStripLayer(BmnSiliconLayer strip_layer) {
    StripLayers.push_back(strip_layer);

    DefineModuleBorders();
}

Bool_t BmnSiliconModule::SetStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].SetStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnSiliconModule::AddStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].AddStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnSiliconModule::SetStripMatchInLayer(Int_t layer_num, Int_t strip_num, BmnMatch mc_match) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].SetStripMatch(strip_num, mc_match);
    }
    return false;
}

Bool_t BmnSiliconModule::SetStripDigitNumberMatchInLayer(Int_t layer_num, Int_t strip_num, BmnMatch digit_num_match) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].SetStripDigitNumberMatch(strip_num, digit_num_match);
    }
    return false;
}

Bool_t BmnSiliconModule::SetStripSignalInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t signal) {
    Int_t n_layers_the_same_id = 0;
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].SetStripSignal(strip_num, signal)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnSiliconModule::AddStripSignalInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t signal) {
    Int_t n_layers_the_same_id = 0;
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].AddStripSignal(strip_num, signal)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnSiliconModule::SetStripMatchInLayerByZoneId(Int_t zone_id, Int_t strip_num, BmnMatch mc_match) {
    Int_t n_layers_the_same_id = 0;
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].SetStripMatch(strip_num, mc_match)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnSiliconModule::AddStripMatchInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t weight, Int_t mc_num) {
    Int_t n_layers_the_same_id = 0;
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].AddLinkToStripMatch(strip_num, weight, mc_num)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnSiliconModule::SetStripDigitNumberMatchInLayerByZoneId(Int_t zone_id, Int_t strip_num, BmnMatch digit_num_match) {
    Int_t n_layers_the_same_id = 0;
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].SetStripDigitNumberMatch(strip_num, digit_num_match)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnSiliconModule::AddStripDigitNumberMatchInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t weight, Int_t digit_num) {
    Int_t n_layers_the_same_id = 0;
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].AddLinkToStripDigitNumberMatch(strip_num, weight, digit_num)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Double_t BmnSiliconModule::GetStripSignalInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].GetStripSignal(strip_num);
    }
    return -1;
}

BmnMatch BmnSiliconModule::GetStripMatchInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].GetStripMatch(strip_num);
    }
    return BmnMatch();
}

BmnMatch BmnSiliconModule::GetStripDigitNumberMatchInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < StripLayers.size() ) {
        return StripLayers[layer_num].GetStripDigitNumberMatch(strip_num);
    }
    return BmnMatch();
}

Int_t BmnSiliconModule::GetFirstStripInZone(Int_t zone_id) {
    Int_t first_strip = 1E6;

    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            Int_t first_strip_curr_layer = StripLayers[ilayer].GetFirstStripNumber();
            if(first_strip > first_strip_curr_layer) first_strip = first_strip_curr_layer;
        }
    }
    return first_strip;
}

Int_t BmnSiliconModule::GetLastStripInZone(Int_t zone_id) {
    Int_t last_strip = 0;

    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            Int_t last_strip_curr_layer = StripLayers[ilayer].GetLastStripNumber();
            if(last_strip < last_strip_curr_layer) last_strip = last_strip_curr_layer;
        }
    }
    return last_strip;
}

Double_t BmnSiliconModule::GetStripSignalInZone(Int_t zone_id, Int_t strip_num) {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if( strip_num >= StripLayers[ilayer].GetFirstStripNumber() && strip_num <= StripLayers[ilayer].GetLastStripNumber() ) {
                return StripLayers[ilayer].GetStripSignal(strip_num);
            }
        }
    }
    return -1;
}

BmnMatch BmnSiliconModule::GetStripMatchInZone(Int_t zone_id, Int_t strip_num) {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if( strip_num >= StripLayers[ilayer].GetFirstStripNumber() && strip_num <= StripLayers[ilayer].GetLastStripNumber() ) {
                return StripLayers[ilayer].GetStripMatch(strip_num);
            }
        }
    }
    return BmnMatch();
}

BmnMatch BmnSiliconModule::GetStripDigitNumberMatchInZone(Int_t zone_id, Int_t strip_num) {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if( strip_num >= StripLayers[ilayer].GetFirstStripNumber() && strip_num <= StripLayers[ilayer].GetLastStripNumber() ) {
                return StripLayers[ilayer].GetStripDigitNumberMatch(strip_num);
            }
        }
    }
    return BmnMatch();
}

void BmnSiliconModule::ResetModuleData() {
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        StripLayers[ilayer].ResetLayer();
    }

    DefineModuleBorders();

    ResetRealPoints();
    ResetIntersectionPoints();
}

Bool_t BmnSiliconModule::IsPointInsideModule(Double_t x, Double_t y) {
    Double_t xloc = ConvertRotatedToLocalX(x, y);
    Double_t yloc = ConvertRotatedToLocalY(x, y);
    for(Int_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( StripLayers[ilayer].IsPointInsideStripLayer(xloc, yloc) ) return true;
    }
    return false;
}

Bool_t BmnSiliconModule::IsPointInsideModule(Double_t x, Double_t y, Double_t z) {
    Double_t coord_eps = 0.01; //100 um
    if( ((z > ZStartModulePosition) || (fabs(z - ZStartModulePosition) < coord_eps)) &&
        ((z < (ZStartModulePosition + ModuleThickness)) || (fabs(z - (ZStartModulePosition + ModuleThickness))) < coord_eps) ) {
        if( IsPointInsideModule(x, y) ) return true;
    }
    return false;
}

Bool_t BmnSiliconModule::IsPointInsideZThickness(Double_t z) {
    Double_t coord_eps = 0.01; //100 um
    if( ((z > ZStartModulePosition) || (fabs(z - ZStartModulePosition) < coord_eps)) &&
        ((z < (ZStartModulePosition + ModuleThickness)) || (fabs(z - (ZStartModulePosition + ModuleThickness))) < coord_eps) ) {
        return true;
    }
    return false;
}

//Add a single point without smearing
Bool_t BmnSiliconModule::AddRealPointSimple(Double_t x, Double_t y, Double_t z,
                                            Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    Double_t xloc = ConvertRotatedToLocalX(x, y);
    Double_t yloc = ConvertRotatedToLocalY(x, y);

    if( IsPointInsideModule(x, y) ) {

        Int_t n_strip_layers = StripLayers.size();

        //Add a signal to the strips layers ------------------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            if( !StripLayers[ilayer].IsPointInsideStripLayer(xloc, yloc) ) continue;

            Double_t strip_pos = StripLayers[ilayer].ConvertPointToStripPosition(xloc, yloc);
            Int_t zone_id = StripLayers[ilayer].GetZoneID();

            AddStripSignalInLayerByZoneId(zone_id, (Int_t)strip_pos, signal);

            //strip match
            AddStripMatchInLayerByZoneId(zone_id, (Int_t)strip_pos, signal, refID);

        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);
        RealPointsMC.push_back(refID);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnSiliconModule::AddRealPointSimple(): Point (" << x << " : " << y << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

//Add single point with Gaussian smearing
Bool_t BmnSiliconModule::AddRealPointFullOne(Double_t x, Double_t y, Double_t z,
                               Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    Double_t xloc = ConvertRotatedToLocalX(x, y);
    Double_t yloc = ConvertRotatedToLocalY(x, y);

    if( IsPointInsideModule(x, y) ) {

        if(signal <= 0.0) signal = 1e-16;

        Double_t AvalancheRadius = 0.01; //cm

        Double_t radius = AvalancheRadius;
        if(radius <= 0.0) return false;

        //AZ-260322 gRandom->SetSeed(0);

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
            if( !StripLayers[ilayer].IsPointInsideStripLayer(xloc, yloc) ) continue;
            cluster_layers[ilayer] = MakeCluster(ilayer, xloc, yloc, signal, radius);
        }
        //----------------------------------------------------------------------

        //Add the correct clusters to the strip layers -------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(Int_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                Int_t zone_id = StripLayers[ilayer].GetZoneID();
                AddStripSignalInLayerByZoneId(zone_id, strip_num, strip_signal);
            }
        }
        //----------------------------------------------------------------------

        //Fill strip matches ---------------------------------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(Int_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                Int_t zone_id = StripLayers[ilayer].GetZoneID();
                AddStripMatchInLayerByZoneId(zone_id, strip_num, strip_signal, refID);
            }
        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);
        RealPointsMC.push_back(refID);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnSiliconModule::AddRealPointFullOne(): Point (" << x << " : " << y << " : " << z << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

StripCluster BmnSiliconModule::MakeCluster(Int_t layer_num, Double_t xcoord, Double_t ycoord, Double_t signal, Double_t radius) {

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
    Int_t FirstStripInLayer = StripLayers[layer_num].GetFirstStripNumber();
    Int_t LastStripInLayer = StripLayers[layer_num].GetLastStripNumber();

    if(NStripsInLayer == 0) return cluster;

    Double_t CenterZonePos = StripLayers[layer_num].ConvertPointToStripPosition(xcoord, ycoord);

    if( CenterZonePos < FirstStripInLayer || CenterZonePos >= LastStripInLayer+1 ) return cluster;


    gausF.SetParameter(1, CenterZonePos);
    Double_t total_signal = 0.0;

    Double_t LeftZonePos = CenterZonePos - RadiusInZones;
    Double_t RightZonePos = CenterZonePos + RadiusInZones;
    Double_t OutLeftBorder = 0.0;
    Double_t OutRightBorder = 0.0;

    if(LeftZonePos < FirstStripInLayer)  {
        OutLeftBorder = LeftZonePos;
        LeftZonePos = FirstStripInLayer;
    }
    if(RightZonePos < FirstStripInLayer) {
        OutRightBorder = RightZonePos;
        RightZonePos = FirstStripInLayer;
    }
    if(LeftZonePos >= LastStripInLayer+1) {
        OutLeftBorder = LeftZonePos;
        LeftZonePos = LastStripInLayer+1 - 0.001;
    }
    if(RightZonePos >= LastStripInLayer+1) {
        OutRightBorder = RightZonePos;
        RightZonePos = LastStripInLayer+1 - 0.001;
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

        if(NumCurrentZone >= FirstStripInLayer && NumCurrentZone < LastStripInLayer+1 && Energy > 0.0) {
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

        if(NumCurrentZone >= FirstStripInLayer && NumCurrentZone < LastStripInLayer+1 && Energy > 0.0) {
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

            if(NumCurrentZone >= FirstStripInLayer && NumCurrentZone < LastStripInLayer+1 && Energy > 0.0) {
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

        if(NumCurrentZone >= FirstStripInLayer && NumCurrentZone < LastStripInLayer+1 && Energy > 0.0) {
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
        Double_t S = gausF.Integral(first, last);
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
            S = gausF.Integral(first, last);
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
            Double_t S = gausF.Integral(first, last);
            Double_t Energy = (signal*Gain)*(S/SCluster);
            Energy += rand.Gaus(0.0, var_level*Energy);
            if(Energy < 0.0) Energy = 0.0;
            Double_t value = Energy;
            hist.SetBinContent(1+i, value);
            dist += h;
        }

        Double_t first = (Int_t)OutRightBorder;
        Double_t last = first + (OutRightBorder - (Int_t)OutRightBorder);
        Double_t S = gausF.Integral(first, last);
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

void BmnSiliconModule::CalculateStripHitIntersectionPoints() {

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
                                xcoord = StripLayers[ilayer].GetXLeftStripBorderPoint() + (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
                            }
                            else {
                                xcoord = StripLayers[ilayer].GetXRightStripBorderPoint() - (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
                            }

                            Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);
                            ycoord = Tan(j_PiOver2MinusAngleRad)*xcoord + jB;
                        }

                        if( Abs(jAngleRad) < 0.01 ) {

                            if(StripLayers[jlayer].GetStripNumberingOrder() == LeftToRight ) {
                                xcoord = StripLayers[jlayer].GetXLeftStripBorderPoint() + (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
                            }
                            else {
                                xcoord = StripLayers[jlayer].GetXRightStripBorderPoint() - (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
                            }

                            Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
                            ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
                        }
                    }

                    //check: a point belongs to both strip layers together
                    if( StripLayers[ilayer].IsPointInsideStripLayer(xcoord, ycoord) && StripLayers[jlayer].IsPointInsideStripLayer(xcoord, ycoord) ) {

                        Double_t xrot = ConvertLocalToRotatedX(xcoord, ycoord);
                        Double_t yrot = ConvertLocalToRotatedY(xcoord, ycoord);

                        IntersectionPointsX.push_back(xrot);
                        IntersectionPointsY.push_back(yrot);

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

                        //Additional information about the intersection --------
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
Bool_t BmnSiliconModule::SearchIntersectionPoint(Double_t &x, Double_t &y, Double_t strip_pos_layerA, Double_t strip_pos_layerB, Int_t layerA_index, Int_t layerB_index) {

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
                xcoord = StripLayers[ilayer].GetXLeftStripBorderPoint() + (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
            }
            else {
                xcoord = StripLayers[ilayer].GetXRightStripBorderPoint() - (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
            }

            Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);
            ycoord = Tan(j_PiOver2MinusAngleRad)*xcoord + jB;
        }

        if( Abs(jAngleRad) < 0.01 ) {

            if(StripLayers[jlayer].GetStripNumberingOrder() == LeftToRight ) {
                xcoord = StripLayers[jlayer].GetXLeftStripBorderPoint() + (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
            }
            else {
                xcoord = StripLayers[jlayer].GetXRightStripBorderPoint() - (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
            }

            Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
            ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
        }
    }

    Double_t xrot = ConvertLocalToRotatedX(xcoord, ycoord);
    Double_t yrot = ConvertLocalToRotatedY(xcoord, ycoord);

    x = xrot;
    y = yrot;

    //check: a point belongs to both strip layers together
    if( StripLayers[ilayer].IsPointInsideStripLayer(xcoord, ycoord) && StripLayers[jlayer].IsPointInsideStripLayer(xcoord, ycoord) ) {
        return true;
    }

    return false;
}

Double_t BmnSiliconModule::ConvertRotatedToLocalX(Double_t xrot, Double_t yrot) {
    //Getting local X-coordinate by applying an inverse rotation to rotated coordinates
    Double_t xloc = ModuleRotationCenterX + (xrot-ModuleRotationCenterX)*cos(ModuleRotationAlgleRad) - (yrot-ModuleRotationCenterY)*sin(ModuleRotationAlgleRad);
    return xloc;
}

Double_t BmnSiliconModule::ConvertRotatedToLocalY(Double_t xrot, Double_t yrot) {
    //Getting local X-coordinate by applying an inverse rotation to rotated coordinates
    Double_t yloc = ModuleRotationCenterY + (xrot-ModuleRotationCenterX)*sin(ModuleRotationAlgleRad) + (yrot-ModuleRotationCenterY)*cos(ModuleRotationAlgleRad);
    return yloc;
}

Double_t BmnSiliconModule::ConvertLocalToRotatedX(Double_t xloc, Double_t yloc) {
    //Getting rotated X-coordinate by applying a rotation to local (non-rotated) coordinates
    Double_t xrot = ModuleRotationCenterX + (xloc-ModuleRotationCenterX)*cos(-ModuleRotationAlgleRad) - (yloc-ModuleRotationCenterY)*sin(-ModuleRotationAlgleRad);
    return xrot;
}

Double_t BmnSiliconModule::ConvertLocalToRotatedY(Double_t xloc, Double_t yloc) {
    //Getting rotated Y-coordinate by applying a rotation to local (non-rotated) coordinates
    Double_t yrot = ModuleRotationCenterY + (xloc-ModuleRotationCenterX)*sin(-ModuleRotationAlgleRad) + (yloc-ModuleRotationCenterY)*cos(-ModuleRotationAlgleRad);
    return yrot;
}

void BmnSiliconModule::ResetIntersectionPoints() {
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

void BmnSiliconModule::DefineModuleBorders() {

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

ClassImp(BmnSiliconModule)
