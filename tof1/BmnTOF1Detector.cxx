#include "BmnTOF1Detector.h"

ClassImp(BmnTOF1Detector)

BmnTOF1Detector::BmnTOF1Detector() {
}

//----------------------------------------------------------------------------------------

BmnTOF1Detector::BmnTOF1Detector(Int_t NPlane, Int_t FillHist = 0) {
    Clear();
    memset(fKilled, 0, sizeof (fKilled));
    memset(CorrLR, 0, sizeof (CorrLR));
    fNEvents = 0;
    KillStrip(0);
    KillStrip(47);
    fFillHist = FillHist;
    fNPlane = NPlane;
    fStripLength = 30; // cm
    fSignalVelosity = 0.06; // 0.06 ns/cm
    fMaxDelta = (fStripLength + 2.0) * fSignalVelosity; // + 20 mm on the strip edge

    for (Int_t i = 0; i < fNStr; i++)
        gSlew[i] = NULL;

    TString Name = Form("Plane_%d", NPlane);
    if (fFillHist > 0) {
        fHistListStat = new TList();
        fHistListCh = new TList();
        fHistListDt = new TList();

        fName.Clear();
        fName = Form("Hist_HitByCh_%s", Name.Data());
        hHitByCh = new TH1I(fName, fName, fNStr + 1, -0.5, fNStr + 0.5);
        fHistListStat->Add(hHitByCh);

        fName.Clear();
        fName = Form("Hist_HitPerEv_%s", Name.Data());
        hHitPerEv = new TH1I(fName, fName, fNStr + 1, -0.5, fNStr + 0.5);
        fHistListStat->Add(hHitPerEv);

        fName.Clear();
        fName = Form("Hist_HitLR_%s", Name.Data());
        hHitLR = new TH2S(fName, fName, fNStr, 0, fNStr, fNStr, 0, fNStr);
        fHistListStat->Add(hHitLR);

        fName.Clear();
        fName = Form("Hist_XY_%s", Name.Data());
        hXY = new TH2I(fName, fName, 240, -150, 150, 120, -75, 75);
        fHistListStat->Add(hXY);

        hDy_near = new TH1S(Form("hDy_near_%s", fName.Data()), Form("hDy_near_%s", fName.Data()), 400, -20, 20);
        hDtime_near = new TH1S(Form("hDtime_near_%s", fName.Data()), Form("hDtime_near_%s", fName.Data()), 400, -10, 10);
        hDWidth_near = new TH1S(Form("hDWidth_near_%s", fName.Data()), Form("hDWidth_near_%s", fName.Data()), 256, -28., 28.);
        hTempDtimeDy_near = new TH2S(Form("hTempDtimeDy_near_%s", fName.Data()), Form("hTempDtimeDy_near_%s", fName.Data()), 400, -10, 10, 200, -10, 10);
        hDy_acros = new TH1S(Form("hDy_acros_%s", fName.Data()), Form("hDy_acros_%s", fName.Data()), 400, -20, 20);
        hDtime_acros = new TH1S(Form("hDtime_acros_%s", fName.Data()), Form("hDtime_acros_%s", fName.Data()), 400, -10, 10);
        hDWidth_acros = new TH1S(Form("hDWidth_acros_%s", fName.Data()), Form("hDWidth_acros_%s", fName.Data()), 256, -28., 28.);
        hTempDtimeDy_acros = new TH2S(Form("hTempDtimeDy_acros_%s", fName.Data()), Form("hTempDtimeDy_acros_%s", fName.Data()), 400, -10, 10, 200, -10, 10);
        fHistListStat->Add(hDy_near);
        fHistListStat->Add(hDtime_near);
        fHistListStat->Add(hDWidth_near);
        fHistListStat->Add(hTempDtimeDy_near);
        fHistListStat->Add(hDy_acros);
        fHistListStat->Add(hDtime_acros);
        fHistListStat->Add(hDWidth_acros);
        fHistListStat->Add(hTempDtimeDy_acros);

        Int_t istart = 49;
        if (fFillHist == 1) istart = 48;
        if (fFillHist == 2) istart = 0;
        for (Int_t i = istart; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_Time_%s_str%d", Name.Data(), i);
            hTime[i] = new TH1I(fName, fName, 2000, 0, 1000);
            fHistListCh->Add(hTime[i]);
        }

        for (Int_t i = istart; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_dtLR_%s_str%d", Name.Data(), i);
            hDtLR[i] = new TH1I(fName, fName, 1024, -12., 12.);
            fHistListCh->Add(hDtLR[i]);
        }

        for (Int_t i = istart; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_Width_%s_str%d", Name.Data(), i);
            hWidth[i] = new TH1I(fName, fName, 1024, 12., 60.);
            fHistListCh->Add(hWidth[i]);
        }

        for (Int_t i = istart; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_ToF_%s_str%d", Name.Data(), i);
            hDt[i] = new TH1I(fName, fName, 1024, -24., 24.);
            fHistListDt->Add(hDt[i]);
        }

        for (Int_t i = istart; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_ToF_vs_AmpDet_%s_str%d", Name.Data(), i);
            hDtvsWidthDet[i] = new TH2S(fName, fName, 1024, 0., 48., 1024, -24., 24.);
            fHistListDt->Add(hDtvsWidthDet[i]);
        }

        for (Int_t i = istart; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_ToF_vs_AmpT0_%s_str%d", Name.Data(), i);
            hDtvsWidthT0[i] = new TH2S(fName, fName, 1024, 0., 48., 1024, -24., 24.);
            fHistListDt->Add(hDtvsWidthT0[i]);
        }


    } else {

        hHitByCh = NULL;
        hHitPerEv = NULL;
        hHitLR = NULL;
        hXY = NULL;

        for (Int_t i = 0; i < fNStr; i++)
            gSlew[i] = NULL;

        for (Int_t i = 0; i < fNStr + 1; i++) //
        {
            hTime[i] = NULL;
            hDtLR[i] = NULL;
            hWidth[i] = NULL;
            hDt[i] = NULL;
        }

        for (Int_t i = 0; i < fNStr + 1; i++) //
        {
            hDtvsWidthDet[i] = NULL;
            hDtvsWidthT0[i] = NULL;
        }
    }

    fName.Clear();
    fName = Name;
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::Clear() {
    memset(fTimeL, 0, sizeof (fTimeL));
    memset(fTimeR, 0, sizeof (fTimeR));
    memset(fTimeLtemp, 0, sizeof (fTimeLtemp));
    memset(fTimeRtemp, 0, sizeof (fTimeRtemp));
    memset(fTime, 0, sizeof (fTime));
    memset(fWidthL, 0, sizeof (fWidthL));
    memset(fWidthR, 0, sizeof (fWidthR));
    memset(fWidthLtemp, 0, sizeof (fWidthLtemp));
    memset(fWidthRtemp, 0, sizeof (fWidthRtemp));
    memset(fWidth, 0, sizeof (fWidth));
    memset(fFlagHit, 0, sizeof (fFlagHit));
    memset(fTof, 0, sizeof (fTof));
    memset(fDigitL, 0, sizeof (fDigitL));
    memset(fDigitR, 0, sizeof (fDigitR));
    memset(fHit, 0, sizeof (fHit));
    fHit_Per_Ev = 0;

    for (Int_t i = 0; i < fNStr; i++)
        fCrossPoint[i].SetXYZ(0., 0., 0.);
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetDigit(BmnTof1Digit * TofDigit) {
    fStrip = TofDigit->GetStrip();
    if (TofDigit->GetSide() == 0 && fFlagHit[fStrip] == kFALSE && fKilled[fStrip] == kFALSE) {
        fTimeLtemp[fStrip] = TofDigit->GetTime() - CorrLR[fStrip] * 2.;
        fWidthLtemp[fStrip] = TofDigit->GetAmplitude();
        fDigitL[fStrip]++;
    }
    if (TofDigit->GetSide() == 1 && fFlagHit[fStrip] == kFALSE && fKilled[fStrip] == kFALSE) {
        fTimeRtemp[fStrip] = TofDigit->GetTime();
        fWidthRtemp[fStrip] = TofDigit->GetAmplitude();
        fDigitR[fStrip]++;
    }

    if (
            fTimeRtemp[fStrip] != 0 && fTimeLtemp[fStrip] != 0
            && TMath::Abs((fTimeLtemp[fStrip] - fTimeRtemp[fStrip]) * 0.5) <= fMaxDelta // cat for length of strip  
            && TMath::Abs((fWidthLtemp[fStrip] - fWidthRtemp[fStrip]) * 0.5) <= 1.5 // cat for Amplitude correlation
            //&& fFlagHit[fStrip] == kFALSE
            )
        if (fFlagHit[fStrip] == kFALSE) {
            fTimeL[fStrip] = fTimeLtemp[fStrip];
            fTimeR[fStrip] = fTimeRtemp[fStrip];
            fWidthL[fStrip] = fWidthLtemp[fStrip];
            fWidthR[fStrip] = fWidthRtemp[fStrip];
            fFlagHit[fStrip] = kTRUE;
            fHit[fStrip]++;
        } else
            fHit[fStrip]++;

    return fFlagHit[fStrip];
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::KillStrip(Int_t NumberOfStrip) {
    fKilled[NumberOfStrip] = kTRUE;
}

//----------------------------------------------------------------------------------------

Int_t BmnTOF1Detector::FindHits(BmnTrigDigit *T0) {
    fT0 = T0;
    fNEvents++;
    Bool_t flag;
    for (Int_t i = 0; i < fNStr; i++)
        if (
                fWidthL[i] != 0 && fWidthR[i] != 0
                //&& fFlagHit[fStrip] == kTRUE
                ) {
            fHit_Per_Ev++;
            fWidth[i] = fWidthL[i] + fWidthR[i];
            fTime[i] = (fTimeL[i] + fTimeR[i]) * 0.5;
            flag = GetCrossPoint(i);
            if (fT0 != NULL) fTof[i] = CalculateDt(i);
            if (i > 1 && fFillHist > 0) {
                if (fFlagHit[i - 1] == kTRUE) {
                    hDy_near->Fill(fCrossPoint[i].Y() - fCrossPoint[i - 1].Y());
                    hDtime_near->Fill(fTof[i] - fTof[i - 1]);
                    hTempDtimeDy_near->Fill(fTof[i] - fTof[i - 1], fCrossPoint[i].Y() - fCrossPoint[i - 1].Y());
                    hDWidth_near->Fill(fWidth[i] - fWidth[i - 1]);
                }
                if (fFlagHit[i - 2] == kTRUE) {
                    hDy_acros->Fill(fCrossPoint[i].Y() - fCrossPoint[i - 2].Y());
                    hDtime_acros->Fill(fTof[i] - fTof[i - 2]);
                    hTempDtimeDy_acros->Fill(fTof[i] - fTof[i - 2], fCrossPoint[i].Y() - fCrossPoint[i - 2].Y());
                    hDWidth_acros->Fill(fWidth[i] - fWidth[i - 2]);
                }
            }
        }

    if (fFillHist > 0)
        FillHist();

    return fHit_Per_Ev;
}

//----------------------------------------------------------------------------------------

Int_t BmnTOF1Detector::FindHits(BmnTrigDigit *T0, TClonesArray *TofHit) {
    fT0 = T0;
    fNEvents++;
    Bool_t flag;
    for (Int_t i = 0; i < fNStr; i++)
        if (
                fWidthL[i] != 0 && fWidthR[i] != 0
                //&& fFlagHit[fStrip] == kTRUE
                ) {
            fHit_Per_Ev++;
            fWidth[i] = fWidthL[i] + fWidthR[i];
            fTime[i] = (fTimeL[i] + fTimeR[i]) * 0.5;
            flag = GetCrossPoint(i);
            if (fT0 != NULL) fTof[i] = CalculateDt(i);
            if (i > 1 && fFillHist > 0) {
                if (fFlagHit[i - 1] == kTRUE) {
                    hDy_near->Fill(fCrossPoint[i].Y() - fCrossPoint[i - 1].Y());
                    hDtime_near->Fill(fTof[i] - fTof[i - 1]);
                    hTempDtimeDy_near->Fill(fTof[i] - fTof[i - 1], fCrossPoint[i].Y() - fCrossPoint[i - 1].Y());
                    hDWidth_near->Fill(fWidth[i] - fWidth[i - 1]);
                }
                if (fFlagHit[i - 2] == kTRUE) {
                    hDy_acros->Fill(fCrossPoint[i].Y() - fCrossPoint[i - 2].Y());
                    hDtime_acros->Fill(fTof[i] - fTof[i - 2]);
                    hTempDtimeDy_acros->Fill(fTof[i] - fTof[i - 2], fCrossPoint[i].Y() - fCrossPoint[i - 2].Y());
                    hDWidth_acros->Fill(fWidth[i] - fWidth[i - 2]);
                }
            }
            AddHit(i, TofHit);
        }

    if (fFillHist > 0)
        FillHist();

    return fHit_Per_Ev;
}

//------------------------------------------------------------------------------------------------------------------------

void BmnTOF1Detector::AddHit(Int_t Str, TClonesArray *TofHit) {

    fVectorTemp.SetXYZ(0.7, 0.36, 1.); // error for point dx = 0.7 cm; dy = 1.25/SQRT(12) = 0.36 cm; dy = 1(?)cm
    Int_t UID = BmnTOF1Point::GetVolumeUID(0, fNPlane + 1, Str + 1); // strip [0,47] -> [1, 48]
    BmnTofHit *pHit = new ((*TofHit)[TofHit->GetEntriesFast()]) BmnTofHit(UID, fCrossPoint[Str], fVectorTemp, -1);

    pHit->SetTimeStamp(fTof[Str]);
    pHit->AddLink(FairLink(0x1, -1));
    pHit->AddLink(FairLink(0x2, -1));
    pHit->AddLink(FairLink(0x4, UID));
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::FillHist() {
    hHitPerEv->Fill(fHit_Per_Ev);
    for (Int_t i = 0; i < fNStr; i++) {
        for (Int_t j = 0; j < fNStr; j++) {
            if (fWidthL[i] != 0 && fWidthR[j] != 0) {
                hHitLR->Fill(i, j);
                if (i == j) {
                    hHitByCh->Fill(i);
                    hXY->Fill(fCrossPoint[i].x(), fCrossPoint[i].y());
                    hTime[fNStr]->Fill(fTime[i]);
                    hWidth[fNStr]->Fill(fWidth[i]);
                    hDtLR[fNStr]->Fill((fTimeL[i] - fTimeR[i]) * 0.5);
                    if (fFillHist > 1) {
                        hTime[i]->Fill(fTime[i]);
                        hWidth[i]->Fill(fWidth[i]);
                        hDtLR[i]->Fill((fTimeL[i] - fTimeR[i]) * 0.5);
                    }
                    if (fT0 != NULL) {
                        hDt[fNStr]->Fill(fTof[i]);
                        hDtvsWidthDet[fNStr]->Fill(fWidth[i], fTof[i]);
                        hDtvsWidthT0[fNStr]->Fill(fT0->GetAmp(), fTof[i]);
                        if (fFillHist > 1) {
                            hDt[i]->Fill(fTof[i]);
                            hDtvsWidthDet[i]->Fill(fWidth[i], fTof[i]);
                            hDtvsWidthT0[i]->Fill(fT0->GetAmp(), fTof[i]);
                        }
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------

Double_t BmnTOF1Detector::CalculateDt(Int_t Str = 0) {
    Double_t dt = 0;
    Double_t T0Amp;
    dt = fTime[Str] - fT0->GetTime();

    T0Amp = fT0->GetAmp();
    dt = dt - (1.947 - 0.5363 * T0Amp
            + 0.03428 * T0Amp * T0Amp
            - 0.0005853 * T0Amp * T0Amp * T0Amp);

    if (gSlew[Str] != NULL) dt = dt - gSlew[Str]->Eval(fWidth[Str]) + 21.782; // 21.782 ns is ToF of light for 6.53 meters (Distance between TOF400 and T0)

    return dt;
}

//----------------------------------------------------------------------------------------

TList* BmnTOF1Detector::GetList(Int_t n = 0) {
    if (fFillHist == kTRUE) {
        if (n == 0) return fHistListStat;
        if (n == 1) return fHistListCh;
        if (n == 2) return fHistListDt;
    } else return NULL;
}

//----------------------------------------------------------------------------------------

TString BmnTOF1Detector::GetName() {
    return fName;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetCorrLR(Double_t* Mass) {
    for (Int_t i = 0; i < 48; i++)
        CorrLR[i] = Mass[i];
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetCorrLR(TString NameFile) {
    char line[256];
    Int_t Pl, St;
    Double_t Temp;
    ifstream f_corr;
    TString dir = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", NameFile.Data());
    f_corr.open(dir);
    f_corr.getline(line, 256);
    f_corr.getline(line, 256);
    if (f_corr.is_open() == kTRUE) {
        while (!f_corr.eof()) {
            f_corr >> Pl >> St >> Temp;
            if (Pl == fNPlane) {
                f_corr >> CorrLR[St];
                if (TMath::Abs(Temp - CorrLR[St]) > 2.) CorrLR[St] = -11.9766;
            } else
                f_corr >> Temp;
        }
    } else {
        cout << "File " << NameFile.Data() << " for LR correction is not found" << endl;
        cout << "Check " << dir.Data() << " folder for file" << endl;
        return kFALSE;
    }
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetCorrSlewing(TString NameFile) {
    TString PathToFile = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", NameFile.Data());
    TString name, dirname;
    TDirectory *Dir;
    TFile *f_corr = new TFile(PathToFile.Data(), "READ");
    if (f_corr->IsOpen()) {
        dirname = Form("Plane_%d", fNPlane);
        f_corr->cd(dirname.Data());
        Dir = f_corr-> CurrentDirectory();
        for (Int_t i = 0; i < fNStr; i++) {
            name = Form("Graph_TA_Plane%d_str%d", fNPlane, i);
            gSlew[i] = (TGraphErrors*) Dir->Get(name.Data());
        }
    } else {
        cout << "File " << NameFile.Data() << " for Slewing correction is not found" << endl;
        cout << "Check " << PathToFile.Data() << " folder for file" << endl;
        return kFALSE;
    }
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::GetCrossPoint(Int_t NStrip = 0) {

    fVectorTemp.SetXYZ(0., 0., 0.);
    if (TMath::Abs((fTimeL[NStrip] - fTimeR[NStrip]) * 0.5) >= fMaxDelta)
        return kFALSE; // estimated position out the strip edge.
    double dL = (fTimeL[NStrip] - fTimeR[NStrip]) * 0.5 / fSignalVelosity;
    fVectorTemp(0) = 0;
    fVectorTemp(1) = dL;
    fVectorTemp(2) = 0; //TMP ALIGMENT CORRECTIONS
    fCrossPoint[NStrip] = fCentrStrip[NStrip] + fVectorTemp;
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetGeoFile(TString NameFile) {

    // get gGeoManager from ROOT file 
    TString PathToFile = Form("%s%s%s", getenv("VMCWORKDIR"), "/macro/run/geometry_run/", NameFile.Data());
    TFile* geoFile = new TFile(PathToFile, "READ");
    if (!geoFile->IsOpen()) {
        cout << "Error: could not open ROOT file with geometry: " + NameFile << endl;
        return kFALSE;
    }
    TList* keyList = geoFile->GetListOfKeys();
    TIter next(keyList);
    TKey* key = (TKey*) next();
    TString className(key->GetClassName());
    if (className.BeginsWith("TGeoManager"))
        key->ReadObj();
    else {
        cout << "Error: TGeoManager isn't top element in geometry file " + NameFile << endl;
        return kFALSE;
    }

    BmnTof1GeoUtils *pGeoUtils = new BmnTof1GeoUtils();
    pGeoUtils->ParseTGeoManager(false, NULL, true);

    Int_t UID;
    for (Int_t i = 0; i < fNStr; i++) {
        UID = BmnTOF1Point::GetVolumeUID(0, fNPlane + 1, i + 1); // strip [0,47] -> [1, 48]
        const LStrip1 *pStrip = pGeoUtils->FindStrip(UID);
        fCentrStrip[i] = pStrip->center;
        if (fNPlane >= 5) fCentrStrip[i].SetX(fCentrStrip[i].X()+5.5); // for field run only
        else fCentrStrip[i].SetX(fCentrStrip[i].X()+2.5);
    }
    geoFile->Close();
    pGeoUtils->~BmnTof1GeoUtils();
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetGeo(BmnTof1GeoUtils *pGeoUtils) {
    Int_t UID;
    for (Int_t i = 0; i < fNStr; i++) {
        UID = BmnTOF1Point::GetVolumeUID(0, fNPlane + 1, i + 1); // strip [0,47] -> [1, 48]
        const LStrip1 *pStrip = pGeoUtils->FindStrip(UID);
        fCentrStrip[i] = pStrip->center;
//        if (fNPlane >= 5) fCentrStrip[i].SetX(fCentrStrip[i].X()+5.5); // for field run only
//        else fCentrStrip[i].SetX(fCentrStrip[i].X()+2.5);
    }
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::GetXYZTime(Int_t Str, TVector3 *XYZ, Double_t *ToF) {

    if (fTof[Str] == 0) return kFALSE;
    if (NULL == XYZ && NULL == ToF) return kFALSE;
    XYZ->SetXYZ(fCrossPoint[Str].x(), fCrossPoint[Str].y(), fCrossPoint[Str].z());
    *ToF = fTof[Str];
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Double_t BmnTOF1Detector::GetWidth(Int_t Str = 1) {
    return fWidth[Str];
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SaveHistToFile(TString NameFile) {

    TFile *fileout = new TFile(NameFile.Data(), "UPDATE");
    TDirectory *Dir;
    Bool_t flag;

    flag = fileout->cd("Tof400");
    if (flag == kFALSE)
        Dir = fileout->mkdir("Tof400");
    else
        Dir = fileout-> CurrentDirectory();
    Dir->cd();

    TDirectory * DirPlane;
    TDirectory * Dir1Plane;
    TDirectory * Dir2Plane;
    DirPlane = Dir->mkdir(fName.Data());
    DirPlane -> cd();
    fHistListStat->Write();
    Dir1Plane = DirPlane->mkdir("Detector");
    Dir1Plane -> cd();
    fHistListCh->Write();
    Dir2Plane = DirPlane->mkdir("ToF");
    Dir2Plane -> cd();
    fHistListDt->Write();

    fileout->Close();
}
