#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include <TMemFile.h>
#include "TGraph.h"
#include "TMath.h"
#include "TImage.h"
#include <unistd.h>
//#include "../../bmndata/BmnTof2Raw2Digit.h"

#define TOF400_PLANE_COUNT 3
#define TOF400_STRIP_COUNT 48

void monitor(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root",
             TString dstName  = "$VMCWORKDIR/macro/run/bmndst.root"){
    
    Double_t trackXMax = 40;
    Double_t trackYMax = 40;
    Int_t hitBins    = 100;
    // GEM config
    const UInt_t gemLayers = 4;
    const UInt_t gemCount = 8;
    const UInt_t nStrips[gemCount] = {256, 825, 825, 825, 825, 825, 1019, 1019};
    
    // ToF config
    const UInt_t kNPLANES = 16;
    const UInt_t kNREALWIRES = 240;
    const UInt_t kNWIRES = 256;
    const Int_t Number = 4;
    const Int_t nb = 100;
    const Float_t angles[kNPLANES] = {
        135, 135, 45, 45, 90, 90, 180, 180,
        135, 135, 45, 45, 90, 90, 180, 180
    };
    const TString names[kNPLANES] = {
        "VA_1", "VB_1", "UA_1", "UB_1", "YA_1", "YB_1", "XA_1", "XB_1",
        "VA_2", "VB_2", "UA_2", "UB_2", "YA_2", "YB_2", "XA_2", "XB_2"
    };
    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    gROOT->LoadMacro("$VMCWORKDIR/macro/raw/DchDigitsAnalysis.C");
    
    TChain *digiTree = new TChain("cbmsim");
    digiTree->Add(digiName);
    TChain *dstTree = new TChain("cbmsim");
    dstTree->Add(dstName);
    TClonesArray * gemHits = NULL;
    dstTree->SetBranchAddress("BmnGemStripHit", &gemHits);
    TClonesArray * gemDigits = NULL;
    digiTree->SetBranchAddress("GEM", &gemDigits);
    Int_t nEvents = dstTree->GetEntries();
    Int_t layer = 0;
    cout << "Events: " << nEvents << endl;

    // ====================================================================== //
    // Create server
    // ====================================================================== //
    THttpServer* serv = new THttpServer(Form("http:8080/none?top=%s", "BM@N Monitor"));
    //serv->SetJSROOT("https://root.cern.ch/js/4.7/");
    //serv->SetJSROOT("http://web-docs.gsi.de/~linev/js/3.5/");
    
    
    // ====================================================================== //
    // GEM histograms 
    // ====================================================================== // 
    TString name;
    //TH2D * histStripXY[kNST];
    //TH1D * histStripX[kNST];
    TH1F * histStrip[gemCount][gemLayers];

    for (Int_t gemIndex = 0; gemIndex < gemCount; gemIndex++){
        //for (Int_t layer = 0; layer < gemLayers; layer++){
        /*name = Form("XY_strip_distribution_on_%dth_GEM", gemIndex);
        histStripXY[gemIndex] = new TH2D(name, name, hitBins, -trackXMax, trackXMax,
                                                   hitBins, -trackYMax, trackYMax);
        name = Form("X_projection on %dth_GEM", gemIndex);
        histStripX[gemIndex] = new TH1D(name, name, hitBins, -trackXMax, trackXMax);*/
            name = Form("GEM_%d_layer_%d", gemIndex, layer);
            histStrip[gemIndex][layer] = new TH1F(name, name, hitBins, 0, nStrips[gemIndex]);
    }
    
    Int_t canvasW = 800;
    Int_t canvasH = 200 * gemCount;
    TCanvas * canvasGemHits = new TCanvas("GEM_Strip_distribution", "GEM_Strip_distribution", canvasW, canvasH);
    canvasGemHits->Divide(1, gemCount-2);
    canvasGemHits->SetFixedAspectRatio(kTRUE);
    for (Int_t gemIndex = 0; gemIndex < gemCount-2; gemIndex++){
        //for (Int_t layer = 0; layer < gemLayers; layer++){
            canvasGemHits->cd(gemIndex + 1);// * gemLayers + layer);
            histStrip[gemIndex][layer]->Draw();
        }
    
    
    Int_t station = 0;
    Int_t strip = 0;
    for (Long64_t iEv = 0; iEv < nEvents; iEv++){
        digiTree->GetEntry(iEv);
        for (Int_t hitIndex = 0; hitIndex < gemDigits->GetEntriesFast(); hitIndex++){            
            /*BmnGemStripHit *gh = (BmnGemStripHit *)gemHits->At(hitIndex);
            stationIndex = gh->GetStation();
            tfX = gh->GetX();
            tfY = gh->GetY();
            histStripXY[stationIndex]->Fill(tfX, tfY);
            histStripX[stationIndex]->Fill(tfX);
            histStripY[stationIndex]->Fill(tfY);*/                      
            BmnGemStripDigit* gs = (BmnGemStripDigit*) gemDigits->At(hitIndex);
            station = gs->GetStation();
            strip = gs->GetStripNumber();
            if (gs->GetStripLayer() != 0) continue;
            histStrip[station][layer]->Fill(strip);
            //canvasGemHits->Modified();
            //canvasGemHits->Update();            
        }
        //if (res=gSystem->ProcessEvents()) break;
        //cout << "Event " << iEv << " processed with result " << res << endl; 
    }
    /*for (Int_t gemIndex = 0; gemIndex < gemCount; gemIndex++){
        name = Form("X_projection_on_%dth_GEM", gemIndex);
        histHitX[gemIndex] = histHitXY[gemIndex]->ProjectionX(name);
        histHitX[gemIndex]->SetTitle(name);
        name = Form("Y_projection_on_%dth_GEM", gemIndex);
        histHitY[gemIndex] = histHitXY[gemIndex]->ProjectionY(name);
        histHitY[gemIndex]->SetTitle(name);
    }*/
    // ====================================================================== //
    // ToF histograms 
    // ====================================================================== //
    TClonesArray *ToF4Digits;
    digiTree->SetBranchAddress("TOF400", &ToF4Digits);
    name = "ToF400_Leading_Time";
    TH1D *histToF400LeadingTime = new TH1D(name, name, 500, 0, 1000);
    name = "ToF400_Amplitude";
    TH1D *histToF400Amp = new TH1D(name, name, 4096, 0, 96);
    name = "ToF400_Strip";
    TH1I *histToF400Strip = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    name = "ToF400_StripSimult";
    TH1I *histToF400StripSimult = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    name = "ToF400_State";
    TH2F *histToF400State = new TH2F(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT, 2, 0, 2);
    
    Int_t stripIndex = 0;
    for (Int_t evIndex = 0; evIndex < nEvents; evIndex++){
        digiTree->GetEntry(evIndex);
        TH1I *histL = new TH1I("", "", TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
        TH1I *histR = new TH1I("", "", TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
        TH1I histSimultaneous;
        for (Int_t digIndex = 0; digIndex < ToF4Digits->GetEntriesFast(); digIndex++){
            BmnTof1Digit *td = (BmnTof1Digit *)ToF4Digits->At(digIndex);
            stripIndex = td->GetStrip();
            histToF400LeadingTime->Fill(td->GetTime());
            histToF400Amp->Fill(td->GetAmplitude());
            histToF400Strip->Fill(stripIndex);
            if (td->GetSide() == 0)
                histL->Fill(stripIndex);
            else
                histR->Fill(stripIndex);
        }
        histSimultaneous = (*histL) * (*histR);
        *histToF400Strip = *histToF400Strip + histSimultaneous;
        for (Int_t binIndex = 0; binIndex < TOF400_STRIP_COUNT; binIndex++)
            histToF400StripSimult->Fill(histSimultaneous->GetBinContent(binIndex));
    }
    Int_t selectedPlane = 2;
    //if (ToF4Digits->GetEntriesFast() > 0)
    for (Int_t evIndex = 0; evIndex < 10; evIndex++){
        digiTree->GetEntry(evIndex);
    for (Int_t digIndex = 0; digIndex < ToF4Digits->GetEntriesFast(); digIndex++){
        BmnTof1Digit *td = (BmnTof1Digit *)ToF4Digits->At(digIndex);
        if (td->GetPlane() == selectedPlane)
            histToF400State->Fill(td->GetStrip(), td->GetSide(), td->GetAmplitude());
    }}
    cout << "hist size " << histToF400State->GetEntries() << endl;
    name = "ToF400_Now";
    TCanvas * canvasToF400State = new TCanvas(name, name, TOF400_STRIP_COUNT, 2);
    canvasToF400State->cd(1);
    histToF400State->Draw("colz");
    canvasToF400State->Modified();
    canvasToF400State->Update();
    /*const Int_t MaxPlane = 
    for (int i=0; i<MaxPlane; i++) // MaxPlane - число камер в системе
    {
        // гистогаммы срабатывания стрипов для всех камер
        // сработал стрип в событии - в соответствующий бин единичка
	sprintf(name, "Rate_vs_Strip_Chamber_%d",i+1);
	sprintf(title, "Rate vs Strip Chamber %d",i+1);
	RvsS[i] = new TH1F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER);
    }
    */
    // ====================================================================== //
    // DCH histograms    
    // ====================================================================== //
    TClonesArray *DchDigits;
    digiTree->SetBranchAddress("DCH", &DchDigits);
    TH1F * h_wires[kNPLANES];
    Float_t v_wires[kNPLANES][kNWIRES] = {};
    Int_t myPalette[nb];
    Double_t R[Number] = {0.00, 0.00, 1.00, 1.00};
    Double_t G[Number] = {0.00, 1.00, 0.65, 0.00};
    Double_t B[Number] = {1.00, 0.00, 0.00, 0.00};
    Double_t Length[Number] = {0.0, 0.33, 0.66, 1.0};

    Int_t FI = TColor::CreateGradientColorTable(Number, Length, R, G, B, nb);
    for (Int_t i = 0; i < nb; ++i) {
        myPalette[i] = FI + i;
    }
    for (Int_t i = 0; i < kNPLANES; ++i)
        h_wires[i] = new TH1F(names[i], names[i], kNREALWIRES, 0, kNREALWIRES);

    for (Long64_t iEv = 0; iEv < nEvents; iEv++) {
        digiTree->GetEntry(iEv);
        for (Int_t iDig = 0; iDig < DchDigits->GetEntriesFast(); ++iDig) {
            BmnDchDigit* dig = (BmnDchDigit*) DchDigits->At(iDig);
            Int_t plane = dig->GetPlane();
            Int_t wire = dig->GetWireNumber();

            v_wires[plane][wire] += 1;
            if (wire > kNREALWIRES - 1) {
                wire -= 128; //8 * 16 last preamplifier setup behind hole, so move signal in correct place
            }
            h_wires[plane]->Fill(wire);
        }
    }
    for (Int_t i = 0; i < kNPLANES; ++i) {
        Float_t maxSig = 0.0;
        for (Int_t j = 0; j < kNWIRES; ++j) {
            Float_t sig = v_wires[i][j];
            if (sig > maxSig) maxSig = sig;
        }
        for (Int_t j = 0; j < kNWIRES; ++j) {
            v_wires[i][j] /= maxSig;
        }
    }

    TCanvas* chmb1 = new TCanvas("DCH_1", "DCH_1", 1000, 1000);
    chmb1->cd();
    for (Int_t i = 0; i < kNPLANES / 2; ++i) {
        DrawAllWires(chmb1, myPalette, v_wires[i], angles[i], i);
    }
    chmb1->Modified();
    chmb1->Update();
    chmb1->SaveAs("DCH1_wires.png");
    /*TImage * img = TImage::Open("DCH1_wires.png");    
    TCanvas * png_canvas = new TCanvas("DCH-1_pic", "DCH-1_pic", 100, 100);
    png_canvas->cd();
    img->Draw();
    png_canvas->Modified();
    png_canvas->Update();
    img->SetName("DCH_img");
   serv->Register("/", img);*/
    //delete img;
    
    //TFile *hfile = new TMemFile("GEM online","RECREATE","Demo ROOT file with histograms");
    //hfile->Write();
    
}
inline void DrawAllWires(TCanvas* c, Int_t* palette, Float_t * wires, Float_t ang, Short_t planeId) {
    const UInt_t kNANGLE = 8;
    Double_t l = c->GetAbsWNDC();
    Double_t a = l / (1 + Sqrt2());
    Double_t b = l / (2 + Sqrt2());
    Double_t xPol[kNANGLE] = {b, 0, 0, b, b + a, l, l, b + a};
    Double_t yPol[kNANGLE] = {0, b, b + a, l, l, b + a, b, 0};
    TGraph* gr = new TGraph(kNANGLE, xPol, yPol);
    gr->GetXaxis()->SetRangeUser(0.0, 1.0);
    gr->GetYaxis()->SetRangeUser(0.0, 1.0);
    gr->SetTitle("oct");
    gr->SetLineWidth(3);
    gr->GetXaxis()->SetLabelColor(0);
    gr->GetXaxis()->SetTickLength(0);
    gr->GetYaxis()->SetLabelColor(0);
    gr->GetYaxis()->SetTickLength(0);
    gr->SetFillStyle(4000);
    //    gr->Draw("la");
    Float_t w = l / kNREALWIRES;
    Float_t x0 = 0.0;
    Float_t x1 = 0.0;
    Float_t y0 = 0.0;
    Float_t y1 = 0.0;

    TLine line;
    line.SetLineWidth(2);
    for (Int_t j = 0; j < kNWIRES; ++j) {

        if (j >= 112 && j < 128) {
            x0 = w * j;
            x1 = x0;
            y0 = 0;
            y1 = l / 2;
        } else if (j > kNREALWIRES - 1) {
            x0 = w * (j - 128);
            x1 = x0;
            y0 = l / 2;
            y1 = l;
        } else {
            x0 = w * j;
            x1 = x0;
            if (x0 <= b) {
                y0 = -x0 + b;
                y1 = x0 + a + b;
            } else if (x0 >= a + b) {
                y0 = -x0 + 2 * a + 3 * b;
                y1 = x0 - a - b;
            } else {
                y0 = 0;
                y1 = l;
            }
        }

        Int_t colorId = Int_t(wires[j] * (nb - 1));
        line.SetLineColor(palette[colorId]);
        line.SetLineColorAlpha(palette[colorId], 0.9 - 0.9 / 7 * planeId); //transparency
        if (colorId == 0) {
            line.SetLineColor(kWhite);
            line.SetLineColorAlpha(kWhite, planeId * 0.1);
        }
        Float_t sinPhi = Sin(ang * DegToRad());
        Float_t cosPhi = Cos(ang * DegToRad());
        Float_t x0_ = (x0 - l / 2) * cosPhi - (y0 - l / 2) * sinPhi + l / 2;
        Float_t y0_ = (x0 - l / 2) * sinPhi + (y0 - l / 2) * cosPhi + l / 2;
        Float_t x1_ = (x1 - l / 2) * cosPhi - (y1 - l / 2) * sinPhi + l / 2;
        Float_t y1_ = (x1 - l / 2) * sinPhi + (y1 - l / 2) * cosPhi + l / 2;
        line.DrawLine(x0_, y0_, x1_, y1_);
        x0 += w;
        x1 = x0;
    }
    TEllipse* hole = new TEllipse((a + 2 * b) / 2, (a + 2 * b) / 2, (a + 2 * b) / 30);
    hole->Draw("same");
}

void DrawWires(TPad* pad, Int_t* palette, Float_t * wires, Float_t ang) {

    pad->Draw();
    pad->cd();
    const UInt_t kNANGLE = 8;
    Double_t l = pad->GetAbsWNDC();
    Double_t a = l / (1 + Sqrt2());
    Double_t b = l / (2 + Sqrt2());
    Double_t xPol[kNANGLE] = {b, 0, 0, b, b + a, l, l, b + a};
    Double_t yPol[kNANGLE] = {0, b, b + a, l, l, b + a, b, 0};
    TGraph* gr = new TGraph(kNANGLE, xPol, yPol);
    gr->GetXaxis()->SetRangeUser(0.0, 1.0);
    gr->GetYaxis()->SetRangeUser(0.0, 1.0);
    gr->SetTitle("");
    gr->SetLineWidth(3);
    gr->GetXaxis()->SetLabelColor(0);
    gr->GetXaxis()->SetTickLength(0);
    gr->GetYaxis()->SetLabelColor(0);
    gr->GetYaxis()->SetTickLength(0);
    gr->Draw("la");
    Float_t w = l / kNREALWIRES;
    Float_t x0 = 0.0;
    Float_t x1 = 0.0;
    Float_t y0 = 0.0;
    Float_t y1 = 0.0;

    TLine line;
    line.SetLineWidth(3);
    for (Int_t j = 0; j < kNWIRES; ++j) {

        if (j >= 112 && j < 128) {
            x0 = w * j;
            x1 = x0;
            y0 = 0;
            y1 = l / 2;
        } else if (j > kNREALWIRES - 1) {
            x0 = w * (j - 128);
            x1 = x0;
            y0 = l / 2;
            y1 = l;
        } else {
            x0 = w * j;
            x1 = x0;
            if (x0 <= b) {
                y0 = -x0 + b;
                y1 = x0 + a + b;
            } else if (x0 >= a + b) {
                y0 = -x0 + 2 * a + 3 * b;
                y1 = x0 - a - b;
            } else {
                y0 = 0;
                y1 = l;
            }
        }

        Int_t colorId = Int_t(wires[j] * (nb - 1));
        line.SetLineColor(palette[colorId]);
        if (colorId == 0)
            line.SetLineColor(kWhite);
        Float_t sinPhi = Sin(ang * DegToRad());
        Float_t cosPhi = Cos(ang * DegToRad());
        Float_t x0_ = (x0 - l / 2) * cosPhi - (y0 - l / 2) * sinPhi + l / 2;
        Float_t y0_ = (x0 - l / 2) * sinPhi + (y0 - l / 2) * cosPhi + l / 2;
        Float_t x1_ = (x1 - l / 2) * cosPhi - (y1 - l / 2) * sinPhi + l / 2;
        Float_t y1_ = (x1 - l / 2) * sinPhi + (y1 - l / 2) * cosPhi + l / 2;
        line.DrawLine(x0_, y0_, x1_, y1_);
        x0 += w;
        x1 = x0;
    }
    TEllipse* hole = new TEllipse((a + 2 * b) / 2, (a + 2 * b) / 2, (a + 2 * b) / 30);
    hole->Draw("same");
}
