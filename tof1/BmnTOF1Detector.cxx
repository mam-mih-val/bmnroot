#include "BmnTOF1Detector.h"

ClassImp(BmnTOF1Detector)

BmnTOF1Detector::BmnTOF1Detector() { }

//----------------------------------------------------------------------------------------

BmnTOF1Detector::BmnTOF1Detector(Int_t NPlane, Bool_t FillHist = kFALSE)
{
    Clear();
    memset(fKilled, 0, sizeof (fKilled));
    memset(CorrLR, 0, sizeof (CorrLR));
    fNEvents = 0;
    KillStrip(0);
    KillStrip(47);
    fFillHist = FillHist;
    
    TString Name = Form ("Plane_%d",NPlane);
    if (fFillHist == kTRUE)
    {
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
            hDtLR[i] = new TH1I(fName, fName, 1024, -12., 12.);
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
    }

    fName.Clear();
    fName = Name;
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::Clear()
{
    memset(fTimeL, 0, sizeof (fTimeL));
    memset(fTimeR, 0, sizeof (fTimeR));
    memset(fTime, 0, sizeof (fTime));
    memset(fWidthL, 0, sizeof (fWidthL));
    memset(fWidthR, 0, sizeof (fWidthR));
    memset(fWidth, 0, sizeof (fWidth));
    memset(fFlagHit, 0, sizeof (fFlagHit));
    memset(fTof, 0, sizeof (fTof));
    fHit_Per_Ev = 0;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetDigit(BmnTof1Digit * TofDigit)
{
    fStrip = TofDigit->GetStrip();
    if (TofDigit->GetSide() == 0 && fFlagHit[fStrip] == kFALSE && fKilled[fStrip] == kFALSE)
    {
        fTimeL[fStrip] = TofDigit->GetTime() - CorrLR[fStrip];
     //   cout << "Time before LRCorr = " << TofDigit->GetTime() << "; Time after = " << fTimeL[fStrip] << endl;
     //   getchar();
        fWidthL[fStrip] = TofDigit->GetAmplitude();
    }
    if (TofDigit->GetSide() == 1 && fFlagHit[fStrip] == kFALSE && fKilled[fStrip] == kFALSE)
    {
        fTimeR[fStrip] = TofDigit->GetTime();
        fWidthR[fStrip] = TofDigit->GetAmplitude();
    }

    if (
            fTimeR[fStrip] != 0 && fTimeL[fStrip] != 0
            && TMath::Abs((fTimeL[fStrip] - fTimeR[fStrip]) * 0.5) <= 2. // cat for length of strip  
            && TMath::Abs((fWidthL[fStrip] - fWidthR[fStrip]) * 0.5) <= 1.5 // cat for Amplitude correlation
            && fFlagHit[fStrip] == kFALSE
            ) //
        fFlagHit[fStrip] = kTRUE; //*/

    return fFlagHit[fStrip];
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::KillStrip(Int_t NumberOfStrip)
{
    fKilled[NumberOfStrip] = kTRUE;
}

//----------------------------------------------------------------------------------------

Int_t BmnTOF1Detector::FindHits(BmnTrigDigit *T0)
{
    fT0 = T0;
    fNEvents++;
    for (Int_t i = 0; i < fNStr; i++)
        if (
                fWidthL[i] != 0 && fWidthR[i] != 0
                //&& fFlagHit[fStrip] == kTRUE
                )
        {
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

void BmnTOF1Detector::FillHist()
{
    hHitPerEv->Fill(fHit_Per_Ev);
    for (Int_t i = 0; i < fNStr; i++)
        for (Int_t j = 0; j < fNStr; j++)
        {
            if (fWidthL[i] != 0 && fWidthR[j] != 0)
            {
                hHitLR->Fill(i, j);
                if (
                        i == j
                        //&& fFlagHit[i] == kTRUE
                        )
                {
                    hHitByCh->Fill(i);
                    hTime[i]->Fill(fTime[i]);
                    hWidth[i]->Fill(fWidth[i]);
                    hDtLR[i]->Fill((fTimeL[i] - fTimeR[i]) * 0.5);
                    hTime[fNStr]->Fill(fTime[i]);
                    hWidth[fNStr]->Fill(fWidth[i]);
                    hDtLR[fNStr]->Fill((fTimeL[i] - fTimeR[i]) * 0.5);
                    if (fT0 != NULL)
                    {
                        hDt[i]->Fill(fTof[i]);
                        hDt[fNStr]->Fill(fTof[i]);
                        hDtvsWidthDet[i]->Fill(fWidth[i], fTof[i]);
                        hDtvsWidthT0[i]->Fill(fT0->GetAmp(), fTof[i]);
                    }
                }
            }
        }
}

//----------------------------------------------------------------------------------------

Double_t BmnTOF1Detector::CalculateDt(Int_t Str = 0)
{
    Double_t dt = 0;
    Double_t T0Amp;
    dt = fTime[Str] - fT0->GetTime();
    /*  T0Amp = fT0->GetAmp();

      if (fWidth[Str] < 20.46) CorrPlane7_It1 = 0;
      else if (fWidth[Str] >= 20.46 && fWidth[Str] < 27.25) CorrPlane7_It1 = 1;
      else if (fWidth[Str] >= 27.25 && fWidth[Str] < 34.81) CorrPlane7_It1 = 2;
      else if (fWidth[Str] >= 34.81 && fWidth[Str] < 37.11) CorrPlane7_It1 = 3;
      else if (fWidth[Str] >= 37.11) CorrPlane7_It1 = 4;
      dt = dt - (CorrPlane7Coeff_It1[CorrPlane7_It1][0] + CorrPlane7Coeff_It1[CorrPlane7_It1][1] * fWidth[Str] +
              CorrPlane7Coeff_It1[CorrPlane7_It1][2] * fWidth[Str] * fWidth[Str] +
              CorrPlane7Coeff_It1[CorrPlane7_It1][3] * fWidth[Str] * fWidth[Str] * fWidth[Str]); //

      if (T0Amp >= 6.15 && T0Amp < 12.3) CorrT0_It1 = 0; //deutron
      else if (T0Amp >= 12.3 && T0Amp < 17.14) CorrT0_It1 = 1; //deutron
      else if (T0Amp >= 17.9 && T0Amp < 19.63) CorrT0_It1 = 2; //C6+
      else CorrT0_It1 = 3; //skip
      dt = dt - (CorrT0Coeff_It1[CorrT0_It1][0] + CorrT0Coeff_It1[CorrT0_It1][1] * T0Amp +
              CorrT0Coeff_It1[CorrT0_It1][2] * T0Amp * T0Amp +
              CorrT0Coeff_It1[CorrT0_It1][3] * T0Amp * T0Amp * T0Amp); //*/

    return dt;
}

//----------------------------------------------------------------------------------------

TList* BmnTOF1Detector::GetList(Int_t n = 0)
{
    if (fFillHist == kTRUE)
    {
        if (n == 0) return fHistListStat;
        if (n == 1) return fHistListCh;
        if (n == 2) return fHistListDt;
    } else return NULL;
}

//----------------------------------------------------------------------------------------

TString BmnTOF1Detector::GetName()
{
    return fName;
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::SetCorrLR(Double_t* Mass)
{
    for (Int_t i = 0; i < 48; i++)
        CorrLR[i] = Mass[i];

}
