#include "BmnGemStripReadoutModule.h"

ClassImp(BmnGemStripReadoutModule)

BmnGemStripReadoutModule::BmnGemStripReadoutModule() {
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
    Gain = 10000.0; //electons
    DriftGap = 0.3; //cm
    InductionGap = 0.15; //cm

    CreateReadoutPlanes();
}

BmnGemStripReadoutModule::BmnGemStripReadoutModule(Double_t xsize, Double_t ysize,
                                       Double_t xorig, Double_t yorig,
                                       Double_t pitch, Double_t adeg,
                                       Double_t low_strip_width, Double_t up_strip_width,
                                       Double_t zpos_module) {
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
    Gain = 10000.0; //electrons
    DriftGap = 0.3; //cm
    InductionGap = 0.2; //cm

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

    //if(GetYStripsIntersectionSize() > (YMaxReadout-YMinReadout)) cout<<"WARNING: Y-size of a strip intersection is more than y-size of the readout plane ("<<GetYStripsIntersectionSize()<<" > "<< YMaxReadout-YMinReadout<<")\n";

    ResetStripHits();
}

void BmnGemStripReadoutModule::RebuildReadoutPlanes() {
    CreateReadoutPlanes();
}

void BmnGemStripReadoutModule::ResetIntersectionPoints() {
    IntersectionPointsX.clear();
    IntersectionPointsY.clear();

    IntersectionPointsLowerStrip.clear();
    IntersectionPointsUpperStrip.clear();
}

void BmnGemStripReadoutModule::ResetRealPoints() {
    RealPointsLowerStrip.clear();
    RealPointsUpperStrip.clear();

    RealPointsX.clear();
    RealPointsY.clear();

    //NDubbedPoints = 0;
}

void BmnGemStripReadoutModule::ResetStripHits() {
    LowerStripHits.clear();
    UpperStripHits.clear();
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

Bool_t BmnGemStripReadoutModule::AddRealPoint(Double_t x, Double_t y, Double_t z) {
    Int_t numLowStrip = ConvertRealPointToLowerStripNum(x, y);
    Int_t numUpStrip = ConvertRealPointToUpperStripNum(x, y);

    if(numLowStrip < ReadoutLowerPlane.size() && numUpStrip < ReadoutUpperPlane.size()) {

//        if((GetValueOfLowerStrip(numLowStrip) > 0) && (GetValueOfUpperStrip(numUpStrip) > 0)) {
//            NDubbedPoints++;
//        }

        ReadoutLowerPlane.at(numLowStrip)++;
        ReadoutUpperPlane.at(numUpStrip)++;

        RealPointsLowerStrip.push_back(numLowStrip);
        RealPointsUpperStrip.push_back(numUpStrip);

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);

        return true;
    }
    else {
        cout << "WARNING: Point (" << x << " : " << y << ") is out of the readout plane" << "\n";
        return false;
    }
}

Bool_t BmnGemStripReadoutModule::AddRealPointFull(Double_t x, Double_t y, Double_t z,
                                                  Double_t px, Double_t py, Double_t pz) {

    if(pz == 0) return false;
    if(px == 0 && py == 0) px = 1e-8;

    Double_t zdist = DriftGap - Abs(z-ZReadoutModulePosition);
    if(zdist < 0) {
        cout << "WARNING: Point (" << x << " : " << y << " : " << z << ") is out of a drift zone\n";
        return false;
    }

    //Particle direction at the current point
    Double_t dirx = px/Abs(pz);
    Double_t diry = py/Abs(pz);
    Double_t dirz = pz/Abs(pz);

    cout << "x = " << x << "\n";
    cout << "y = " << y << "\n";
    cout << "z = " << z << "\n";

    cout << "px = " << px << "\n";
    cout << "py = " << py << "\n";
    cout << "pz = " << pz << "\n";

    cout << "dirx = " << dirx << "\n";
    cout << "diry = " << diry << "\n";
    cout << "dirz = " << dirz << "\n";

    cout << "zdist = " << zdist << "\n";

    Double_t x_out = dirx*zdist + x;
    Double_t y_out = diry*zdist + y;

    //if(x_out < XMinReadout) { x_out = XMinReadout; }
    //if(x_out > XMaxReadout) { x_out = XMaxReadout; }
    //if(y_out < YMinReadout) { y_out = YMinReadout; }
    //if(y_out > YMaxReadout) { y_out = YMaxReadout; }

    cout << "x_out = " << x_out << "\n";
    cout << "y_out = " << y_out << "\n";
    cout << "z_out = " << DriftGap << "\n";

    Double_t dist_track = Sqrt((x_out-x)*(x_out-x) + (y_out-y)*(y_out-y) + zdist*zdist);
    Double_t dist_projXY = Sqrt((x_out-x)*(x_out-x) + (y_out-y)*(y_out-y));

    Double_t dist_projX = Abs(x_out-x);
    Double_t dist_projY = Abs(y_out-y);

    cout << "dist_track = " << dist_track << "\n";
    cout << "dist_XY = " << dist_projXY << "\n";
    cout << "dist_projX = " << dist_projX << "\n";
    cout << "dist_projY = " << dist_projY << "\n";

    Int_t NCollisions = (int)(dist_track/MCD);

    cout << "NCollisions = " << NCollisions << "\n";

    Double_t xstep = MCD*(dist_projXY/dist_track)*(dist_projX/dist_projXY);
    Double_t ystep = MCD*(dist_projXY/dist_track)*(dist_projY/dist_projXY);

    if(dirx < 0) xstep *= -1.0;
    if(diry < 0) ystep *= -1.0;

    cout << "xstep = " << xstep << " (" << MCD << ")" << "\n";
    cout << "ystep = " << ystep << " (" << MCD << ")" << "\n";

    Int_t rcoll = 0;
    for(Int_t iColl = 0; iColl < NCollisions+1; iColl++) {
        Double_t xs = x + iColl*xstep;
        Double_t ys = y + iColl*ystep;

        if( (xs < XMinReadout) || (xs > XMaxReadout) || (ys < YMinReadout) || (ys > YMaxReadout) ) break;

        MakeCluster(xs, ys);

        cout << iColl << ") xys = " << xs << " : " << ys << "\n";
    }

/*
    static TCanvas *canv = new TCanvas("canv", "canv_title", 10, 10, 700, 700);
    static TPad *pad = new TPad("pad", "pad_title", 0.05, 0.05, 0.95, 0.95, TColor::GetColor("#eeeeee"));

    pad->Range(XMinReadout, YMinReadout,  XMaxReadout, YMaxReadout);
    //pad->Range(x, y,  x_out, y_out);

    pad->Draw();
    pad->cd();

    TEllipse *point_in = new TEllipse(x,y, 0.02);
    point_in->SetFillColor(kBlue);
    TEllipse *point_out = new TEllipse(x_out, y_out, 0.02);
    point_out->SetFillColor(kBlue);

    point_in->Draw();
    point_out->Draw();

    //canv->SaveAs("/home/diman/Software/test.png");
    //delete box;
    //delete canv;

 */
}

Bool_t BmnGemStripReadoutModule::AddRealPointFullOne(Double_t x, Double_t y, Double_t z) {

    if( x >= XMinReadout && x <= XMaxReadout &&
        y >= YMinReadout && y <= YMaxReadout ) {

        MakeCluster(x, y);

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);

        Int_t numLowStrip = ConvertRealPointToLowerStripNum(x, y);
        Int_t numUpStrip = ConvertRealPointToUpperStripNum(x, y);

        RealPointsLowerStrip.push_back(numLowStrip);
        RealPointsUpperStrip.push_back(numUpStrip);

        return true;
    }
    else {
        cout << "WARNING: Point (" << x << " : " << y << ") is out of the readout plane" << "\n";
        return false;
    }
}

Bool_t BmnGemStripReadoutModule::MakeCluster(Double_t x, Double_t y) {
    //Double_t ratio = InductionGap/AvalancheRadius;
    if( AvalancheRadius <= 0.0 ) AvalancheRadius = 1e-8;
    Double_t RadiusInZones = AvalancheRadius/Pitch; //radius in zone units
    Double_t InductionGapInZones = InductionGap/Pitch; // induction in zone units
    Double_t Sigma = RadiusInZones/3.33;

    TF1 gausF("gausF", "gaus", -4*Sigma, 4*Sigma);
    gausF.SetParameter(0, 1.0); // constant (altitude)
    gausF.SetParameter(1, 0.0); // mean (center position)
    gausF.SetParameter(2, Sigma); //sigma

    Double_t SRadius = gausF(0.0)*RadiusInZones/2; ///rough square of the one side distribution
    //Double_t SRadius = gausF.Integral(0.0, 4*Sigma); //square of the one side distribution

    TRandom rand(0);
    Double_t var_level = 0.0; //signal variation (0.1 is 10%)

//Make cluster on lower strips -------------------------------------------------

    Double_t LowerZonePos = CalculateLowerStripZonePosition(x, y);
    Int_t LowerZoneNum = (int)LowerZonePos;
    Double_t LowerStripFill = LowerStripWidth/Pitch; //fill position of a lower strip

    gausF.SetParameter(1, LowerZonePos);

    //Processing left radius
    Double_t LeftLowerZonePos = LowerZonePos - RadiusInZones;
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
            Double_t x = LeftLowerZonePos + dist;
            Double_t S = (gausF(x) + gausF(x+h))*h/2.0; //rough
            //Double_t S = gausF.Integral(x, x+h); //more exactly

            Double_t nelectrons = (Gain/2)*(S/SRadius); // number of electrons in the segment
            nelectrons += rand.Gaus(0, var_level*nelectrons);
            if(nelectrons < 0) nelectrons = 0;

        if(NumCurrentZone >=0 && NumCurrentZone < ReadoutLowerPlane.size()) {
                ReadoutLowerPlane.at(NumCurrentZone) += nelectrons;
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
    Double_t RightLowerZonePos = LowerZonePos + RadiusInZones;
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
            Double_t x = LowerZonePos + dist;
            Double_t S = (gausF(x) + gausF(x+h))*h/2.0; //rough
            //Double_t S = gausF.Integral(x, x+h); //more exactly

            Double_t nelectrons = (Gain/2)*(S/SRadius);
            nelectrons += rand.Gaus(0, var_level*nelectrons);
            if(nelectrons < 0) nelectrons = 0;

            if(NumCurrentZone >=0 && NumCurrentZone < ReadoutLowerPlane.size()) {
                ReadoutLowerPlane.at(NumCurrentZone) += nelectrons;
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

//Make cluster on upper strips -------------------------------------------------

    Double_t UpperZonePos = CalculateUpperStripZonePosition(x, y);
    Int_t UpperZoneNum = (int)UpperZonePos;
    Double_t UpperStripFill = UpperStripWidth/Pitch; //fill position of a upper strip

    gausF.SetParameter(1, UpperZonePos);

    //Processing left radius
    Double_t LeftUpperZonePos = UpperZonePos - RadiusInZones;
    dist = 0;
    Double_t firstPosInZoneUpper = LeftUpperZonePos - (Int_t)LeftUpperZonePos;
    Double_t lastPosInZoneUpper = 1.0;

    if((firstPosInZoneUpper + RadiusInZones) < 1.0 ) lastPosInZoneUpper = firstPosInZoneUpper + RadiusInZones;

    while(1) {
        Int_t NumCurrentZone = (Int_t)(LeftUpperZonePos + dist);
        Double_t h = 0; // height of the trap

        if(firstPosInZoneUpper <= UpperStripFill) {
            if( lastPosInZoneUpper > UpperStripFill ) lastPosInZoneUpper = UpperStripFill;

            h = lastPosInZoneUpper - firstPosInZoneUpper;
            Double_t x = LeftUpperZonePos + dist;
            Double_t S = (gausF(x) + gausF(x+h))*h/2.0; //rough
            //Double_t S = gausF.Integral(x, x+h); //more exactly

            Double_t nelectrons = (Gain/2)*(S/SRadius); // number of electrons in the segment
            nelectrons += rand.Gaus(0, var_level*nelectrons);
            if(nelectrons < 0) nelectrons = 0;

        if(NumCurrentZone >=0 && NumCurrentZone < ReadoutUpperPlane.size()) {
                ReadoutUpperPlane.at(NumCurrentZone) += nelectrons;
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
    Double_t RightUpperZonePos = UpperZonePos + RadiusInZones;
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
            Double_t x = UpperZonePos + dist;
            Double_t S = (gausF(x) + gausF(x+h))*h/2.0; //rough
            //Double_t S = gausF.Integral(x, x+h); //more exactly

            Double_t nelectrons = (Gain/2)*(S/SRadius);
            nelectrons += rand.Gaus(0, var_level*nelectrons);
            if(nelectrons < 0) nelectrons = 0;

            if(NumCurrentZone >=0 && NumCurrentZone < ReadoutUpperPlane.size()) {
                ReadoutUpperPlane.at(NumCurrentZone) += nelectrons;
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

    //cout << "LowerZonePos = " << LowerZonePos << "\n";
    //cout << "UpperZonePos =      " << UpperZonePos << "\n";
}

void BmnGemStripReadoutModule::FindClusterHitsInReadoutPlane() {
    ResetStripHits();
    FindClustersInLayer(ReadoutLowerPlane, LowerStripHits);
    FindClustersInLayer(ReadoutUpperPlane, UpperStripHits);
}

void BmnGemStripReadoutModule::FindClustersInLayer(vector<Double_t> &StripLayer, vector<Double_t> &StripHits) {

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
                MakeStripHit(clusterDigits, clusterValues, Strips, StripHits, is);
            }
            continue;
        }

        if( clusterDigits.size() > 0 ) {
            if( Strips.at(is) >= (clusterValues.at(clusterValues.size()-1)) ) {
                if(descent) {
                    ascent = false;
                    descent = false;
                    //make strip hit
                    MakeStripHit(clusterDigits, clusterValues, Strips, StripHits, is);
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
        MakeStripHit(clusterDigits, clusterValues, Strips, StripHits, lastnum);
    }
}

void BmnGemStripReadoutModule::MakeStripHit(vector<Int_t> &clusterDigits, vector<Double_t> &clusterValues, vector<Double_t> &Strips, vector<Double_t> &StripHits, Int_t &curcnt) {

    //find max strip
    Double_t maxval = 0.0;
    Int_t maxdig = 0;
    for(Int_t i = 0; i < clusterDigits.size(); i++) {
        if(clusterValues.at(i) > maxval) {
            maxval = clusterValues.at(i);
            maxdig = i;
        }
    }
    //subtraction from the last bin
    Double_t diff = 0.0;
    Int_t numObr = 2*maxdig - (clusterValues.size()-1);
    if( numObr >= 0 ) {
        diff = clusterValues.at(clusterValues.size()-1) - clusterValues.at(numObr);
        if(diff > 0) {
            clusterValues.at(clusterValues.size()-1) -= diff;
            Strips.at(clusterDigits.at(clusterDigits.size()-1)) = diff;
        }
    }

    curcnt--;
    if( curcnt < 0 ) curcnt = 0;

    //TCanvas canv("canv", "canv");

    Double_t Mean = 0.0;
    TH1F hist("hist", "hist", clusterDigits.size()+1, 0, clusterDigits.size()+1);
    for(Int_t i = 0; i < clusterDigits.size(); i++) {
        hist.SetBinContent(i+1, clusterValues.at(i));
    }
    if(clusterDigits.size() > 1) {
        hist.Fit("gaus", "WQ0"); //Q - quit mode (without information on the screen)
                                 //0 - not draw
        Mean = hist.GetFunction("gaus")->GetParameter(1) + clusterDigits.at(0);
    }
    else {
        Mean = clusterDigits.at(0) + 0.5;
    }

    StripHits.push_back(Mean);

    clusterDigits.clear();
    clusterValues.clear();

    //for testing
    //TString str = "/home/diman/Software/pics/hist";
    //str += curcnt;
    //str += ".png";
    //canv.SaveAs(str);
}

Double_t BmnGemStripReadoutModule::GetLowerStripHit(Int_t num) {
    if(num >= 0 && num < LowerStripHits.size()) {
        return LowerStripHits.at(num);
    }
    return -1.0;
}

Double_t BmnGemStripReadoutModule::GetUpperStripHit(Int_t num) {
    if(num >= 0 && num < UpperStripHits.size()) {
        return UpperStripHits.at(num);
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
    if(indx < ReadoutLowerPlane.size()) {
        return ReadoutLowerPlane.at(indx);
    }
    else return -1;
}
Double_t BmnGemStripReadoutModule::GetValueOfUpperStrip(Int_t indx) {
    if(indx < ReadoutUpperPlane.size()) {
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
    return (LowerStripZonePos*Pitch) + XMinReadout;
}

Double_t BmnGemStripReadoutModule::FindYHitIntersectionPoint(Double_t LowerStripZonePos, Double_t UpperStripZonePos) {
    Double_t xcoord = FindXHitIntersectionPoint(LowerStripZonePos);
    Double_t hypoten = Pitch/Sin(Abs(AngleRad));
    Double_t ycoord;

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

    FindClusterHitsInReadoutPlane();

    for(UInt_t i = 0; i < LowerStripHits.size(); ++i) {
        for(UInt_t j = 0; j < UpperStripHits.size(); ++j) {
            Double_t xcoord = FindXHitIntersectionPoint(LowerStripHits.at(i),UpperStripHits.at(j));
            Double_t ycoord = FindYHitIntersectionPoint(LowerStripHits.at(i),UpperStripHits.at(j));

            if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) ) {
                IntersectionPointsX.push_back(xcoord);
                IntersectionPointsY.push_back(ycoord);

                IntersectionPointsLowerStrip.push_back((int)LowerStripHits.at(i));
                IntersectionPointsUpperStrip.push_back((int)UpperStripHits.at(j));
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

                    if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) )
                    {
                        IntersectionPointsX.push_back(xcoord);
                        IntersectionPointsY.push_back(ycoord);

                        IntersectionPointsLowerStrip.push_back(i);
                        IntersectionPointsUpperStrip.push_back(j);
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

                    if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) )
                    {
                        IntersectionPointsX.push_back(xcoord);
                        IntersectionPointsY.push_back(ycoord);

                        IntersectionPointsLowerStrip.push_back(i);
                        IntersectionPointsUpperStrip.push_back(j);
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

                    if( (ycoord <= YMaxReadout) && (ycoord >= YMinReadout) )
                    {
                        IntersectionPointsX.push_back(xcoord);
                        IntersectionPointsY.push_back(ycoord);

                        IntersectionPointsLowerStrip.push_back(i);
                        IntersectionPointsUpperStrip.push_back(j);
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
                    if( (ycoord_min <= YMaxReadout) && (ycoord_min >= YMinReadout) ) InYMinBound = true;
                    if( (ycoord_max <= YMaxReadout) && (ycoord_max >= YMinReadout) ) InYMaxBound = true;

                    if( InYMinBound && InYMaxBound )
                    {
                        Double_t xcoord_point = (xcoord_min+xcoord_max)/2;
                        Double_t ycoord_point = (ycoord_min+ycoord_max)/2;

                        if( ycoord_point >= YMinReadout && ycoord_point <= YMaxReadout ) {
                            IntersectionPointsX.push_back(xcoord_point);
                            IntersectionPointsY.push_back(ycoord_point);

                            IntersectionPointsLowerStrip.push_back(i);
                            IntersectionPointsUpperStrip.push_back(j);
                        }
                    }
                    else {
                        if( InYMinBound ) {
                            Double_t xcoord_point = (xcoord_min+xcoord_max)/2;
                            Double_t ycoord_point = (ycoord_min+ycoord_max)/2;

                            if( ycoord_point >= YMinReadout && ycoord_point <= YMaxReadout ) {
                                IntersectionPointsX.push_back((xcoord_min+xcoord_max)/2);
                                IntersectionPointsY.push_back((ycoord_min+ycoord_max)/2);

                                IntersectionPointsLowerStrip.push_back(i);
                                IntersectionPointsUpperStrip.push_back(j);
                            }
                        }
                        if( InYMaxBound ) {
                            Double_t xcoord_point = (xcoord_min+xcoord_max)/2;
                            Double_t ycoord_point = (ycoord_min+ycoord_max)/2;

                            if( ycoord_point >= YMinReadout && ycoord_point <= YMaxReadout ) {
                                IntersectionPointsX.push_back((xcoord_min+xcoord_max)/2);
                                IntersectionPointsY.push_back((ycoord_min+ycoord_max)/2);

                                IntersectionPointsLowerStrip.push_back(i);
                                IntersectionPointsUpperStrip.push_back(j);
                            }
                        }

                    }
                }
            }
        }
    }
}