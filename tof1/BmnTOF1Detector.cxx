#include "BmnTOF1Detector.h"

ClassImp(BmnTOF1Detector)

BmnTOF1Detector::BmnTOF1Detector() {
}

//----------------------------------------------------------------------------------------

BmnTOF1Detector::BmnTOF1Detector(Int_t NPlane, Bool_t FillHist = kFALSE) {
    Clear();
    memset(fKilled, 0, sizeof (fKilled));
    memset(CorrLR, 0, sizeof (CorrLR));
    fNEvents = 0;
    KillStrip(0);
    KillStrip(47);
    fFillHist = FillHist;
    fNPlane = NPlane;

    TString Name = Form("Plane_%d", NPlane);
    if (fFillHist == kTRUE) {
        fHistListStat = new TList();
        fHistListCh = new TList();
        fHistListDt = new TList();

        fName.Clear();
        fName = Form("Hist_HitByCh_%s", Name.Data());
        hHitByCh = new TH1I(fName, fName, fNStr, 0, fNStr);
        fHistListStat->Add(hHitByCh);

        fName.Clear();
        fName = Form("Hist_HitPerEv_%s", Name.Data());
        hHitPerEv = new TH1I(fName, fName, fNStr, 0, fNStr);
        fHistListStat->Add(hHitPerEv);

        fName.Clear();
        fName = Form("Hist_HitLR_%s", Name.Data());
        hHitLR = new TH2I(fName, fName, fNStr, 0, fNStr, fNStr, 0, fNStr);
        fHistListStat->Add(hHitLR);

        fName.Clear();
        fName = Form("Hist_Left_DigitToHit_%s", Name.Data());
        hLeftDigitToHit = new TH2I(fName, fName, 48, 0, 48, 40, 0, 20);
        fHistListStat->Add(hLeftDigitToHit);

        fName.Clear();
        fName = Form("Hist_Right_DigitToHit_%s", Name.Data());
        hRightDigitToHit = new TH2I(fName, fName, 48, 0, 48, 40, 0, 20);
        fHistListStat->Add(hRightDigitToHit);

        for (Int_t i = 0; i < fNStr; i++)
            gSlew[i] = NULL;

        for (Int_t i = 0; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_Time_%s_str%d", Name.Data(), i);
            hTime[i] = new TH1I(fName, fName, 2000, 0, 1000);
            fHistListCh->Add(hTime[i]);
        }

        for (Int_t i = 0; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_dtLR_%s_str%d", Name.Data(), i);
            hDtLR[i] = new TH1I(fName, fName, 1024, 0., 24.);
            fHistListCh->Add(hDtLR[i]);
        }

        for (Int_t i = 0; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_Width_%s_str%d", Name.Data(), i);
            hWidth[i] = new TH1I(fName, fName, 1024, 12., 60.);
            fHistListCh->Add(hWidth[i]);
        }

        for (Int_t i = 0; i < fNStr + 1; i++) //
        {
            fName.Clear();
            fName = Form("Hist_ToF_%s_str%d", Name.Data(), i);
            hDt[i] = new TH1I(fName, fName, 1024, -24., 24.);
            fHistListDt->Add(hDt[i]);
        }

        for (Int_t i = 0; i < fNStr; i++) //
        {
            fName.Clear();
            fName = Form("Hist_ToF_vs_AmpDet_%s_str%d", Name.Data(), i);
            hDtvsWidthDet[i] = new TH2I(fName, fName, 1024, 0., 48., 1024, -24., 24.);
            fHistListDt->Add(hDtvsWidthDet[i]);
        }

        for (Int_t i = 0; i < fNStr; i++) //
        {
            fName.Clear();
            fName = Form("Hist_ToF_vs_AmpT0_%s_str%d", Name.Data(), i);
            hDtvsWidthT0[i] = new TH2I(fName, fName, 1024, 0., 48., 1024, -24., 24.);
            fHistListDt->Add(hDtvsWidthT0[i]);
        }


    } else {

        hHitByCh = NULL;
        hHitPerEv = NULL;
        hHitLR = NULL;
        hLeftDigitToHit = NULL;
        hRightDigitToHit = NULL;

        for (Int_t i = 0; i < fNStr; i++)
            gSlew[i] = NULL;

        for (Int_t i = 0; i < fNStr + 1; i++) //
        {
            hTime[i] = NULL;
            hDtLR[i] = NULL;
            hWidth[i] = NULL;
            hDt[i] = NULL;
        }

        for (Int_t i = 0; i < fNStr; i++) //
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
    memset(fTime, 0, sizeof (fTime));
    memset(fWidthL, 0, sizeof (fWidthL));
    memset(fWidthR, 0, sizeof (fWidthR));
    memset(fWidth, 0, sizeof (fWidth));
    memset(fFlagHit, 0, sizeof (fFlagHit));
    memset(fTof, 0, sizeof (fTof));
    memset(fDigitL, 0, sizeof (fDigitL));
    memset(fDigitR, 0, sizeof (fDigitR));
    memset(fHit, 0, sizeof (fHit));
    fHit_Per_Ev = 0;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetDigit(BmnTof1Digit * TofDigit) {
    fStrip = TofDigit->GetStrip();
    if (TofDigit->GetSide() == 0 && fFlagHit[fStrip] == kFALSE && fKilled[fStrip] == kFALSE) {
        fTimeL[fStrip] = TofDigit->GetTime() - CorrLR[fStrip] * 2.;
        fWidthL[fStrip] = TofDigit->GetAmplitude();
        fDigitL[fStrip]++;
    }
    if (TofDigit->GetSide() == 1 && fFlagHit[fStrip] == kFALSE && fKilled[fStrip] == kFALSE) {
        fTimeR[fStrip] = TofDigit->GetTime();
        fWidthR[fStrip] = TofDigit->GetAmplitude();
        fDigitR[fStrip]++;
    }

    if (
            fTimeR[fStrip] != 0 && fTimeL[fStrip] != 0
            && TMath::Abs((fTimeL[fStrip] - fTimeR[fStrip]) * 0.5) <= 2. // cat for length of strip  
            && TMath::Abs((fWidthL[fStrip] - fWidthR[fStrip]) * 0.5) <= 1.5 // cat for Amplitude correlation
            //&& fFlagHit[fStrip] == kFALSE
            )
            if (fFlagHit[fStrip] == kFALSE) {
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
    for (Int_t i = 0; i < fNStr; i++)
        if (
                fWidthL[i] != 0 && fWidthR[i] != 0
                //&& fFlagHit[fStrip] == kTRUE
                ) {
            fHit_Per_Ev++;
            fWidth[i] = fWidthL[i] + fWidthR[i];
            fTime[i] = (fTimeL[i] + fTimeR[i]) * 0.5;
            if (fT0 != NULL) fTof[i] = CalculateDt(i);
        }

    if (fFillHist == kTRUE)
        FillHist();

    return fHit_Per_Ev;
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::FillHist() {
    hHitPerEv->Fill(fHit_Per_Ev);
    for (Int_t i = 0; i < fNStr; i++) {

        if (fHit[i] != 0) {
            if (fDigitL[i] != 0)
                hLeftDigitToHit->Fill(i, fDigitL[i] / fHit[i]);
            if (fDigitR[i] != 0)
                hRightDigitToHit->Fill(i, fDigitR[i] / fHit[i]);

        } else {
            if (fDigitL[i] != 0)
                hLeftDigitToHit->Fill(i, fDigitL[i] / 1.);
            if (fDigitR[i] != 0)
                hRightDigitToHit->Fill(i, fDigitR[i] / 1.);

        }

        for (Int_t j = 0; j < fNStr; j++) {
            if (fWidthL[i] != 0 && fWidthR[j] != 0) {
                hHitLR->Fill(i, j);
                if (
                        i == j
                        //&& fFlagHit[i] == kTRUE
                        ) {
                    hHitByCh->Fill(i);
                    hTime[i]->Fill(fTime[i]);
                    hWidth[i]->Fill(fWidth[i]);
                    hDtLR[i]->Fill((fTimeL[i] - fTimeR[i]) * 0.5);
                    hTime[fNStr]->Fill(fTime[i]);
                    hWidth[fNStr]->Fill(fWidth[i]);
                    hDtLR[fNStr]->Fill((fTimeL[i] - fTimeR[i]) * 0.5);
                    if (fT0 != NULL) {
                        hDt[i]->Fill(fTof[i]);
                        hDt[fNStr]->Fill(fTof[i]);
                        hDtvsWidthDet[i]->Fill(fWidth[i], fTof[i]);
                        hDtvsWidthT0[i]->Fill(fT0->GetAmp(), fTof[i]);
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

    if (gSlew[Str] != NULL) dt = dt - gSlew[Str]->Eval(fWidth[Str]) + 15.; // 15 ns if ToF of light for 4.5 meters

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

void BmnTOF1Detector::SetCorrLR(Double_t* Mass) {
    for (Int_t i = 0; i < 48; i++)
        CorrLR[i] = Mass[i];
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::SetCorrLR(TString NameFile) {
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
    }
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::SetCorrSlewing(TString NameFile) {
    TString PathToFile = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", NameFile.Data());
    TString name, dirname;
    TFile *f_corr = new TFile(PathToFile.Data(), "READ");
    if (f_corr->IsOpen()) {
        dirname = Form("Plane_%d", fNPlane);
        gDirectory->cd(dirname.Data());
        for (Int_t i = 0; i < fNStr; i++) {
            name = Form("Graph_TA_Plane%d_str%d", fNPlane, i);
            gSlew[i] = (TGraphErrors*) gDirectory->Get(name.Data());
        }
    } else {
        cout << "File " << NameFile.Data() << " for Slewing correction is not found" << endl;
        cout << "Check " << PathToFile.Data() << " folder for file" << endl;
    }
}
