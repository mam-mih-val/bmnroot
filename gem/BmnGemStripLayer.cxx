#include "BmnGemStripLayer.h"

BmnGemStripLayer::BmnGemStripLayer() {

    Verbosity = true;

    LayerZoneNumber = 0;
    LayerType = LowerStripLayer;

    XMinLayer = 0.0;
    XMaxLayer = 10.0;
    YMinLayer = 0.0;
    YMaxLayer = 10.0;

    Pitch = 0.08; //cm
    AngleDeg = 15.0; //in degrees from a vertical line where a plus value is clockwise
    AngleRad = AngleDeg*Pi()/180;  //in radians

    StripOrder = LeftToRight;

    if(AngleDeg <= 0.0 && AngleDeg >= -90.0) {
        SetStripNumberingBorders(LeftBottom, RightTop);
    }
    else {
        if(AngleDeg > 0.0 && AngleDeg <= 90.0) {
            SetStripNumberingBorders(LeftTop, RightBottom);
        }
    }

    ClusterFindingThreshold = 0.0;
}

BmnGemStripLayer::BmnGemStripLayer(Int_t zone_number, StripLayerType layer_type,
                     Double_t xsize, Double_t ysize,
                     Double_t xorig, Double_t yorig,
                     Double_t pitch, Double_t adeg) {

    Verbosity = true;

    LayerZoneNumber = zone_number;
    LayerType = layer_type;

    XMinLayer = xorig;
    XMaxLayer = xorig + xsize;
    YMinLayer = yorig;
    YMaxLayer = yorig + ysize;

    Pitch = pitch;
    AngleDeg = adeg; //in degrees from a vertical line where a plus value is clockwise
    AngleRad = AngleDeg*Pi()/180;  //in radians

    StripOrder = LeftToRight;

    if(AngleDeg <= 0.0 && AngleDeg >= -90.0) {
        SetStripNumberingBorders(LeftBottom, RightTop);
    }
    else {
        if(AngleDeg > 0.0 && AngleDeg <= 90.0) {
            SetStripNumberingBorders(LeftTop, RightBottom);
        }
    }

    ClusterFindingThreshold = 0.0;
}

BmnGemStripLayer::~BmnGemStripLayer() {

}

void BmnGemStripLayer::InitializeLayer() {
    Int_t NStrips = GetNStrips();

    //strips
    Strips.clear();
    Strips.resize(NStrips, 0.0);

    //strip matches
    ResetStripMatches();

    //strip hits
    ResetStripHits();
}

void BmnGemStripLayer::ResetLayer() {
    InitializeLayer();
}

Int_t BmnGemStripLayer::GetNStrips() {
    Double_t xleft_layer = ConvertNormalPointToStripX(XLeftPointOfStripNumbering, YLeftPointOfStripNumbering);
    Double_t xright_layer = ConvertNormalPointToStripX(XRightPointOfStripNumbering, YRightPointOfStripNumbering);
    Double_t n_strips = (xright_layer - xleft_layer)/Pitch;

    if( (n_strips - (Int_t)n_strips) < 1E-10 ) {
        return (Int_t)n_strips;
    }
    else {
        return (Int_t)(n_strips+1);
    }
}

void BmnGemStripLayer::SetPitch(Double_t pitch) {
    Pitch = pitch;

    InitializeLayer();
}

void BmnGemStripLayer::SetLayerSizes(Double_t xsize, Double_t ysize, Double_t xorig, Double_t yorig) {
    XMinLayer = xorig;
    XMaxLayer = xorig + xsize;
    YMinLayer = yorig;
    YMaxLayer = yorig + ysize;

    InitializeLayer();
}

void BmnGemStripLayer::SetAngleDeg(Double_t deg) {
    if(Abs(deg) <= 90.0) {
         AngleDeg = deg;
    }
    else {
        AngleDeg = 0.0;
        if(Verbosity) cerr << "WARNING: BmnGemStripLayer::SetAngleDeg(): the strip angle is incorrect and has been set to zero\n";
    }

    AngleRad = AngleDeg*Pi()/180;

    InitializeLayer();
}

Bool_t BmnGemStripLayer::SetStripNumberingBorders(Double_t x_left, Double_t y_left, Double_t x_right, Double_t y_right) {
    if(x_right < x_left) return false;

    XLeftPointOfStripNumbering = x_left;
    YLeftPointOfStripNumbering = y_left;
    XRightPointOfStripNumbering = x_right;
    YRightPointOfStripNumbering = y_right;

    ResetLayer();

    return true;
}

Bool_t BmnGemStripLayer::SetStripNumberingBorders(StripBorderPoint left, StripBorderPoint right) {

    if( left != LeftTop && left != LeftBottom )  {
        if(Verbosity) cerr << "WARNING: SetStripNumberingBorders: left strip border point is incorrect\n";
        return false;
    }
    if( right != RightTop && right != RightBottom ) {
        if(Verbosity) cerr << "WARNING: SetStripNumberingBorders: right strip border point is incorrect\n";
        return false;
    }

    XLeftPointOfStripNumbering = XMinLayer;
    XRightPointOfStripNumbering = XMaxLayer;

    switch (left) {
        case LeftTop:
            YLeftPointOfStripNumbering = YMaxLayer;
            break;
        case LeftBottom:
            YLeftPointOfStripNumbering = YMinLayer;
            break;
        default:
            return false;
    }

    switch (right) {
        case RightTop:
            YRightPointOfStripNumbering = YMaxLayer;
            break;
        case RightBottom:
            YRightPointOfStripNumbering = YMinLayer;
            break;
        default:
            return false;
    }

    ResetLayer();

    return true;
}

Bool_t BmnGemStripLayer::SetStripNumberingOrder(StripNumberingDirection strip_direction) {
    StripOrder = strip_direction;

    ResetLayer();

    return true;
}

Bool_t BmnGemStripLayer::AddDeadZone(Int_t n_points, Double_t *x_points, Double_t *y_points) {
    DeadZoneOfStripLayer dead_zone;
    Bool_t status = dead_zone.SetDeadZone(n_points, x_points, y_points);
    if(status == true) {
        DeadZones.push_back(dead_zone);
        return true;
    }
    else {
        return false;
    }
}

Bool_t BmnGemStripLayer::IsPointInsideDeadZones(Double_t x, Double_t y) {
    for(Int_t izone = 0; izone < DeadZones.size(); ++izone) {
        if(DeadZones[izone].IsInside(x,y)) return true;
    }

    return false;
}

Bool_t BmnGemStripLayer::IsPointInsideStripLayer(Double_t x, Double_t y) {
    if( x >= XMinLayer && x <= XMaxLayer &&
        y >= YMinLayer && y <= YMaxLayer &&
        !IsPointInsideDeadZones(x, y) ) return true;
    else
        return false;
}

Bool_t BmnGemStripLayer::SetStripSignal(Int_t strip_num, Double_t signal) {
    if(strip_num >= 0 && strip_num < Strips.size()) {
        Strips[strip_num] = signal;
        return true;
    }
    else return false;
}

Bool_t BmnGemStripLayer::AddStripSignal(Int_t strip_num, Double_t signal) {
    if(strip_num >= 0 && strip_num < Strips.size()) {
        Strips[strip_num] += signal;
        return true;
    }
    else return false;
}

Double_t BmnGemStripLayer::GetStripSignal(Int_t strip_num) {
    if(strip_num >= 0 && strip_num < Strips.size()) {
        return Strips[strip_num];
    }
    else return -1;
}

Bool_t BmnGemStripLayer::SetStripMatch(Int_t strip_num, BmnMatch strip_match) {
    if(strip_num >= 0 && strip_num < StripMatches.size()) {
        StripMatches[strip_num] = strip_match;
        return true;
    }
    else return false;
}

Bool_t BmnGemStripLayer::AddLinkToStripMatch(Int_t strip_num, Double_t weight, Int_t refID) {
    if(strip_num >= 0 && strip_num < StripMatches.size()) {
        StripMatches[strip_num].AddLink(weight, refID);
        return true;
    }
    else return false;
}

BmnMatch BmnGemStripLayer::GetStripMatch(Int_t strip_num) {
    if(strip_num >= 0 && strip_num < StripMatches.size()) {
        return StripMatches[strip_num];
    }
    else return BmnMatch(); //return an empty match
}

void BmnGemStripLayer::ResetStripMatches() {
    Int_t NStrips = GetNStrips();

    StripMatches.clear();
    StripMatches.resize(NStrips, BmnMatch());
}

Double_t BmnGemStripLayer::GetStripHitPos(Int_t num) {
    if(num >= 0 && num < StripHits.size()) {
        return StripHits[num];
    }
    return -1.0;
}

Double_t BmnGemStripLayer::GetStripHitTotalSignal(Int_t num) {
    if(num >= 0 && num < StripHitsTotalSignal.size()) {
        return StripHitsTotalSignal[num];
    }
    return -1.0;
}

Double_t BmnGemStripLayer::GetStripHitError(Int_t num) {
    if(num >= 0 && num < StripHitsErrors.size()) {
        return StripHitsErrors[num];
    }
    return -1.0;
}

Int_t BmnGemStripLayer::GetStripHitClusterSize(Int_t num) {
    if(num >= 0 && num < StripHitsClusterSize.size()) {
        return StripHitsClusterSize[num];
    }
    return -1.0;
}

void BmnGemStripLayer::ResetStripHits() {
    StripHits.clear();
    StripHitsTotalSignal.clear();
    StripHitsErrors.clear();
    StripHitsClusterSize.clear();
}

Double_t BmnGemStripLayer::ConvertNormalPointToStripX(Double_t x, Double_t y) {
    Double_t XRotationCenter;
    Double_t YRotationCenter;

    if(StripOrder == LeftToRight) {
        XRotationCenter =  XLeftPointOfStripNumbering;
        YRotationCenter =  YLeftPointOfStripNumbering;
    }
    else {
        if(StripOrder == RightToLeft) {
            XRotationCenter =  XRightPointOfStripNumbering;
            YRotationCenter =  YRightPointOfStripNumbering;
        }
    }
    Double_t StripX = (x - XRotationCenter)*Cos(-AngleRad) + (y - YRotationCenter)*Sin(-AngleRad) + XRotationCenter;//see
    return StripX;
}

Double_t BmnGemStripLayer::ConvertNormalPointToStripY(Double_t x, Double_t y) {
    Double_t XRotationCenter;
    Double_t YRotationCenter;

    if(StripOrder == LeftToRight) {
        XRotationCenter =  XLeftPointOfStripNumbering;
        YRotationCenter =  YLeftPointOfStripNumbering;
    }
    else {
        if(StripOrder == RightToLeft) {
            XRotationCenter =  XRightPointOfStripNumbering;
            YRotationCenter =  YRightPointOfStripNumbering;
        }
    }
    Double_t StripY = -(x - XRotationCenter)*Sin(-AngleRad) + (y - YRotationCenter)*Cos(-AngleRad) + YRotationCenter;//see
    return StripY;
}

Double_t BmnGemStripLayer::ConvertPointToStripPosition(Double_t x, Double_t y) {
    //This function returns real(double) value,
    //where integer part - number of a strip, fractional part - position on this strip (as ratio from begin)

    if(StripOrder == LeftToRight) {
        return (ConvertNormalPointToStripX(x, y)-XLeftPointOfStripNumbering)/Pitch;
    }
    else {
        if(StripOrder == RightToLeft) {
            return (XRightPointOfStripNumbering-ConvertNormalPointToStripX(x, y))/Pitch;
        }
    }
}

Double_t BmnGemStripLayer::CalculateStripEquationB(Double_t strip_pos) {

    Double_t b;

    if(Abs(AngleDeg) != 90.0) { //case: y=a*x+b
        Double_t x_strip_shift = (strip_pos*Pitch)/Cos(Abs(AngleRad));
        Double_t xcoord;

        if(StripOrder == LeftToRight) {
            xcoord = XLeftPointOfStripNumbering + x_strip_shift;
            b = YLeftPointOfStripNumbering - Tan(PiOver2()-AngleRad)*xcoord;
        }
        else {
            if(StripOrder == RightToLeft) {
                xcoord = XRightPointOfStripNumbering-x_strip_shift;
                b = YRightPointOfStripNumbering - Tan(PiOver2()-AngleRad)*xcoord;
            }
        }
    }
    else { //case: y=b
        Double_t y_strip_shift = strip_pos*Pitch*Sin(AngleRad);

        if(StripOrder == LeftToRight) {
            b = YLeftPointOfStripNumbering - y_strip_shift;
        }
        else {
            if(StripOrder == RightToLeft) {
                b = YRightPointOfStripNumbering + y_strip_shift;
            }
        }
    }

    return b;
}

void BmnGemStripLayer::FindClustersAndStripHits() {

    ResetStripHits();

    Double_t threshold = ClusterFindingThreshold;
    //Double_t threshold = 500.0;

    StripCluster cluster;

    Bool_t ascent = false;
    Bool_t descent = false;

    //Processing strips
    vector<Double_t> AnalyzableStrips = Strips;

    //Smooth strip signal ------------------------------------------------------
    //if(Pitch > 0.079) SmoothStripSignal(AnalyzableStrips, 1, 1, 1.0);
    //else SmoothStripSignal(AnalyzableStrips, 2, 1, 1.0);
    //--------------------------------------------------------------------------

    for(Int_t is = 0; is < AnalyzableStrips.size(); is++) {

        if(AnalyzableStrips.at(is) <= threshold) {
            if(descent || ascent) {
                descent = false;
                ascent = false;
                //make strip hit
                MakeStripHit(cluster, AnalyzableStrips, is);
            }
            continue;
        }

        if( cluster.GetClusterSize() > 0 ) {
            if( AnalyzableStrips.at(is) >= (cluster.Signals.at(cluster.GetClusterSize()-1)) ) {
                if(descent) {
                    ascent = false;
                    descent = false;
                    //make strip hit
                    MakeStripHit(cluster, AnalyzableStrips, is);
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

        cluster.AddStrip(is, AnalyzableStrips.at(is));
    }

    if(cluster.GetClusterSize() != 0) {
        //make strip hit
        Int_t lastnum = AnalyzableStrips.size()-1;
        MakeStripHit(cluster, AnalyzableStrips, lastnum);
    }
}

 void BmnGemStripLayer::MakeStripHit(StripCluster &cluster, vector<Double_t> &AnalyzableStrips, Int_t &curcnt) {

    Double_t mean_strip_position = 0.0;
    Double_t total_cluster_signal = 0.0;
    Double_t cluster_rms = 0.0;

    Int_t NStripsInCluster = cluster.GetClusterSize();

    //mean position and cluster signal -----------------------------------------
    for(Int_t i = 0; i < NStripsInCluster; ++i) {
        Double_t strip_num = cluster.Strips.at(i);
        Double_t signal = cluster.Signals.at(i);
        total_cluster_signal += signal;
        mean_strip_position += (strip_num+0.5)*signal;
    }
    mean_strip_position /= total_cluster_signal;

    if(mean_strip_position < 0.0) mean_strip_position = 0.0;
    if(mean_strip_position >= AnalyzableStrips.size()) mean_strip_position = AnalyzableStrips.size() - 0.001;
    //--------------------------------------------------------------------------
   
    //cluster standard deviation (sigma): RMS ----------------------------------
    if(NStripsInCluster > 1) {
        for(Int_t i = 0; i < NStripsInCluster; ++i) {
            Double_t strip_num = cluster.Strips.at(i);
            Double_t signal = cluster.Signals.at(i);
            Double_t residual = (strip_num+0.5) - mean_strip_position;
            cluster_rms += residual*residual*signal;
        }
        cluster_rms /= total_cluster_signal;
        cluster_rms = TMath::Sqrt(cluster_rms);
    }
    else {
        cluster_rms = 1.0/TMath::Sqrt(12.0);
    }
    //--------------------------------------------------------------------------

    StripHits.push_back(mean_strip_position);
    StripHitsTotalSignal.push_back(total_cluster_signal);
    StripHitsErrors.push_back(cluster_rms);
    StripHitsClusterSize.push_back(NStripsInCluster);

    //return to a previous strip
    curcnt--;
    if( curcnt < 0 ) curcnt = 0;

    cluster.Clear();
 }

 void BmnGemStripLayer::SmoothStripSignal(vector<Double_t>& AnalyzableStrips, Int_t NIterations, Int_t SmoothWindow, Double_t Weight) {
     //It's the Simple Moving Average (SMA) method
    //AnalyzableStrips - analyzable strip layer (ref)
    //NIterations - number of smooth iterations (usually 1)
    //SmoothWindow - number of strips on the left-right of the current strip (usually 1)
    //Weight - weight of the current strip (usually 1.0 - for simplicity, greater - for weighted value))

    SmoothStrips.clear();
    Int_t NStrips = AnalyzableStrips.size();

    for(Int_t iteration = 0; iteration < NIterations; ++iteration) {
        SmoothStrips.clear();
        for(Int_t istrip = 0; istrip < NStrips; ++istrip) {
            Double_t mean_value = 0.0;
            for(Int_t iw = istrip-SmoothWindow; iw <= istrip+SmoothWindow; ++iw) {
                if(iw >= 0 && iw < NStrips) {
                    if(iw == istrip) mean_value += AnalyzableStrips[iw]*Weight;
                    else mean_value += AnalyzableStrips[iw];
                }
            }
            mean_value /= 2.0*SmoothWindow + Weight;
            SmoothStrips.push_back(mean_value);
        }
        AnalyzableStrips = SmoothStrips;
    }

    return;
 }

//------------------------------------------------------------------------------

ClassImp(BmnGemStripLayer)