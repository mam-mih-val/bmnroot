// Author: Vasilisa Lenivenko (VBLHEP) <vasilisa@jinr.ru> 2019-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnSiliconTrackFinder                                                      //
//                                                                            //
// Implementation of an algorithm developed by                                //
// V.Lenivenko and V.Paltchik                                                 //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for track segments                      //
// in the Silicon detectors of the BM@N experiment                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "BmnSiliconTrackFinder.h"

#include "FairLogger.h"
#include "FairMCPoint.h"

#include <TMath.h>
#include "TFile.h"

#include <vector>

static Double_t workTime = 0.0;
//TList fhList;

//----------------------------------------------------------------------
void BmnSiliconTrackFinder::Exec(Option_t* opt) {
    if (!IsActive()) return;

    clock_t tStart = clock();
    PrepareArraysToProcessEvent();
    if (fDebug) cout << "\n======================== Silicon track finder exec started  ===================\n" << endl;
    if (fDebug) cout << "Event number: " << fEventNo << endl;
    fEventNo++ ;

    //------------------------Digi-file reading---------------------------
    if(expData) StripsReading(DigitsArrayX,DigitsArrayXp);

    if(fDebug)  cout<<"------------Clustering----------------------"<<endl;
    if(expData) Clustering(DigitsArrayX, DigitsArrayXp, XClust, XClust_width, sumQx, NclustX, XpClust, XpClust_width, sumQxp, NclustXp);

    if(fDebug)  cout<<"------------Coordinate Calculation----------"<<endl;
    CoordinateCalculation(NclustX, NclustXp, XCoord, XpCoord, SigmaX, SigmaXp, XspCoord, XpspCoord, YspCoord, SigmspX, SigmspXp, SigmspY, NhitsXYmod,
                          XClust, XClust_width, sumQx,XpClust, XpClust_width, sumQxp,
                          XspClust, XspClust_width, sumQxsp,XpspClust, XpspClust_width, sumQxpsp, vec_points, Sp_pdg);

    Bool_t Skip = 0;//SkipEvent(NclustX, NclustXp, XCoord, XpCoord);
    if (Skip) return;
    if(expData) CoordinateAlignment(NhitsXYmod,  XspCoord, XpspCoord, YspCoord,  NclustX, NclustXp, XCoord, XpCoord);

    //----------------------------Tracking--------------------------------
    if(fRunPeriod == 7){
        if(fDebug) cout<<"------------Case1: spatial track------------"<<endl;
        CountSpatialPoints(NhitsXYmod,Nsp_st);
        if ( Nsp_st[1] > 0 && Nsp_st[2] > 0 && Nsp_st[3] > 0 ){
            Case1(NhitsXYmod, XspCoord, XpspCoord, YspCoord, SigmspX, SigmspXp, SigmspY, vec_tracks,
                  XspClust, XspClust_width, sumQxsp,XpspClust, XpspClust_width, sumQxpsp);
            RecordingTracksAfterSpatialPoints(vec_tracks,CleanTr);
        }
        CheckPoints(NclustX, NclustXp, XCoord, XpCoord, SigmaX, SigmaXp, XspCoord, XpspCoord, YspCoord, SigmspX, SigmspXp, SigmspY, NhitsXYmod, CleanTr,
                    Nleftoversp, NleftoverX, NleftoverXp,leftoverXsp,leftoverXpsp,leftoverYsp,leftoverXsigsp,leftoverXpsigsp,leftoverYsigsp,leftoverX,leftoverXp,leftoverXsig,leftoverXpsig);
        if(fDebug) cout<<"------------Case1.--------------------------"<<endl;

        if(fDebug) cout<<"------------Case2: 2spatial points + XorX'---"<<endl;
        CountSpatialPoints(Nleftoversp,Nsp_st);
        if ( Nsp_st[3] > 0 && (Nsp_st[1] + Nsp_st[2]) > 0 ) {
            for (Int_t Istn = 1; Istn < 3; ++Istn) {
                Case2( Istn, Nleftoversp, leftoverXsp, leftoverXpsp, leftoverYsp, leftoverXsigsp,  leftoverXpsigsp, leftoverYsigsp, NleftoverX,  NleftoverXp,
                       leftoverX, leftoverXp, leftoverXsig, leftoverXpsig, vec_tracks,
                       XClust, XClust_width, sumQx,XpspClust, XpClust_width, sumQxp,
                       XspClust, XspClust_width, sumQxsp,XpspClust, XpspClust_width, sumQxpsp);
                RecordingTracks(vec_tracks,CleanTr);
                CheckPoints(NclustX, NclustXp, XCoord, XpCoord, SigmaX, SigmaXp, XspCoord, XpspCoord, YspCoord, SigmspX, SigmspXp, SigmspY, NhitsXYmod, CleanTr,
                            Nleftoversp, NleftoverX, NleftoverXp,leftoverXsp,leftoverXpsp,leftoverYsp,leftoverXsigsp,leftoverXpsigsp,leftoverYsigsp,leftoverX,leftoverXp,leftoverXsig,leftoverXpsig);
            }
        }
        if(fDebug) cout<<"------------Case2.--------------------------"<<endl;

        if(fDebug) cout<<"------------Case3:2spatial points + check to the vertex--"<<endl;
        CountSpatialPoints(Nleftoversp,Nsp_st);
        if ( Nsp_st[1] > 0 && Nsp_st[2] > 0 ) {
            Case3( Nleftoversp, leftoverXsp, leftoverXpsp, leftoverYsp, leftoverXsigsp,  leftoverXpsigsp, leftoverYsigsp,
                   NleftoverX,  NleftoverXp, leftoverX, leftoverXp, leftoverXsig, leftoverXpsig, vec_tracks,
                   XClust, XClust_width, sumQx,XpspClust, XpClust_width, sumQxp,
                   XspClust, XspClust_width, sumQxsp,XpspClust, XpspClust_width, sumQxpsp);
            RecordingTracks_case3(vec_tracks,CleanTr);
            CheckPoints(NclustX, NclustXp, XCoord, XpCoord, SigmaX, SigmaXp, XspCoord, XpspCoord, YspCoord, SigmspX, SigmspXp, SigmspY, NhitsXYmod, CleanTr,
                        Nleftoversp, NleftoverX, NleftoverXp,leftoverXsp,leftoverXpsp,leftoverYsp,leftoverXsigsp,leftoverXpsigsp,leftoverYsigsp,leftoverX,leftoverXp,leftoverXsig,leftoverXpsig);
        }
        if(fDebug) cout<<"------------Case3.--------------------------"<<endl;

        if(fDebug) cout<<"------------Case4:1spatial point in 3st + X/X' in st 1&2-"<<endl;
        CountSpatialPoints(Nleftoversp,Nsp_st);
        if ( Nsp_st[3] > 0) {
            for (Int_t Istn = 1; Istn < 3; ++Istn) {
                Case4( Istn, Nleftoversp, leftoverXsp, leftoverXpsp, leftoverYsp, leftoverXsigsp,  leftoverXpsigsp, leftoverYsigsp, NleftoverX,
                       NleftoverXp, leftoverX, leftoverXp, leftoverXsig, leftoverXpsig, vec_tracks,
                       XClust, XClust_width, sumQx,XpspClust, XpClust_width, sumQxp,
                       XspClust, XspClust_width, sumQxsp,XpspClust, XpspClust_width, sumQxpsp);
                RecordingTracks(vec_tracks,CleanTr);
                CheckPoints(NclustX, NclustXp, XCoord, XpCoord, SigmaX, SigmaXp, XspCoord, XpspCoord, YspCoord, SigmspX, SigmspXp, SigmspY, NhitsXYmod, CleanTr,
                            Nleftoversp, NleftoverX, NleftoverXp,leftoverXsp,leftoverXpsp,leftoverYsp,leftoverXsigsp,leftoverXpsigsp,leftoverYsigsp,leftoverX,leftoverXp,leftoverXsig,leftoverXpsig);
            }
        }
        if(fDebug)cout<<"-------------Case4.--------------------------"<<endl;
        CheckLeftover(Nleftoversp, NleftoverX, NleftoverXp,leftoverXsp,leftoverXpsp,leftoverYsp,leftoverXsigsp,leftoverXpsigsp,leftoverYsigsp,leftoverX,leftoverXp,leftoverXsig,leftoverXpsig, CleanTr,
                      NhitsXYmod, XspCoord, YspCoord, SigmspX, SigmspY);
    }//run7
    if(fRunPeriod == 8){
        return;
    }

    if(fDebug) PrintAllTracks(CleanTr);

    if(!expData) MCefficiencyCalculation(vec_points, CleanTr);
    if(fDebug)cout<<"---------------------------------------------"<<endl;
    //------Write all Silicon-hits & Tracks------
    WriteSiliconHits(NclustX, NclustXp, NhitsXYmod, XCoord, XClust_width, XClust, sumQx, XpCoord, XpClust_width, XpClust, sumQxp, XspCoord, YspCoord, XspClust_width, XpspClust_width,
                     XspClust, XpspClust, sumQxsp, sumQxpsp, SigmaX, SigmaXp,SigmspX, SigmspXp, SigmspY, Sp_pdg);
    WriteSiliconTracks(CleanTr);

    if (fDebug) cout << endl;
    if (fDebug) cout << "======================== Silicon track finder exec finished ===================" << endl;
    clock_t tFinish = clock();
    workTime += ((Double_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}//exec
//----------------------------------------------------------------------


void BmnSiliconTrackFinder::WriteSiliconTracks(vector<tracksX>& Tr){
    if (fDebug) cout << "--WriteSiliconTracks--"<<endl;

    Int_t Track_counter = 0;


    for (Int_t InIter = 0; InIter < Tr.size(); ++InIter) {

        Double_t Fit = Tr.at(InIter).param[2] * Tr.at(InIter).CoordZ[3] + Tr.at(InIter).param[3];
        // if (fDebug && Fit > 2.) cout<<" Fit > 2. "<<Fit<<fEventNo<<endl;

        Double_t z1 = Tr.at(InIter).CoordZ[1] + Zcentr;
        Double_t x1 = Tr.at(InIter).param[0] * Tr.at(InIter).CoordZ[1] + Tr.at(InIter).param[1]; //ax(z)+bx
        Double_t y1 = Tr.at(InIter).param[2] * Tr.at(InIter).CoordZ[1] + Tr.at(InIter).param[3]; //y
        Double_t z3 = Tr.at(InIter).CoordZ[3] + Zcentr;
        Double_t x3 = Tr.at(InIter).param[0] * Tr.at(InIter).CoordZ[3] + Tr.at(InIter).param[1]; //ax(z)+bx
        Double_t y3 = Tr.at(InIter).param[2] * Tr.at(InIter).CoordZ[3] + Tr.at(InIter).param[3]; //y

        //if(fDebug)cout<<" itr "<<InIter<<" ax "<<Tr.at(InIter).param[0]<<" bx "<<Tr.at(InIter).param[1]<<endl;

        for (Int_t st = 1; st < fNstations; ++st) {
            if (Tr.at(InIter).CoordX[st] < -900. && Tr.at(InIter).CoordXp[st] > -900.){
                Tr.at(InIter).CoordY[st] = Tr.at(InIter).param[2] * Tr.at(InIter).CoordZ[st] + Tr.at(InIter).param[3];//y
                Tr.at(InIter).SigmaY[st] = 0.105581;
            }

            //if(fDebug)cout<<" itr "<<InIter<<" st "<<st<<" hit: X "<<Tr.at(InIter).CoordX[st]<<" Y "<<Tr.at(InIter).CoordY[st]<<" SigX "<<Tr.at(InIter).SigmaX[st]<<" SigY "<<Tr.at(InIter).SigmaY[st]<<endl;
            BmnSiliconHit* hit = new ((*fBmnSiliconHitsArray)[Track_counter])
                    BmnSiliconHit(0, TVector3(Tr.at(InIter).CoordX[st]+Shift_toCenterOfMagnetX, Tr.at(InIter).CoordY[st]+Shift_toCenterOfMagnetY, Tr.at(InIter).CoordZ[st]+ Zcentr),
                                  TVector3(Tr.at(InIter).SigmaX[st], Tr.at(InIter).SigmaY[st], -1), InIter);
            Track_counter++;
            hit->SetStation(st);
            hit->SetModule(Tr.at(InIter).ModNum[st]);
            hit->SetIndex(InIter);

            hit->SetClusterSizeInLowerLayer(Tr.at(InIter).ClSizeX[st]);     //cluster size   (lower layer |||)
            hit->SetClusterSizeInUpperLayer(Tr.at(InIter).ClSizeXp[st]);    //cluster size   (upper layer ///or\\\)
            hit->SetStripPositionInLowerLayer(Tr.at(InIter).StripNumX[st]); //strip position (lower layer |||)
            hit->SetStripPositionInUpperLayer(Tr.at(InIter).StripNumXp[st]);//strip position (upper layer ///or\\\)
            hit->SetStripTotalSignalInLowerLayer(Tr.at(InIter).SumQX[st]);  //sumQx
            hit->SetStripTotalSignalInUpperLayer(Tr.at(InIter).SumQXp[st]); //sumQxp
        }

        FairTrackParam trackParamf;
        trackParamf.SetPosition(TVector3(x1+Shift_toCenterOfMagnetX, y1+Shift_toCenterOfMagnetY, z1));
        trackParamf.SetTx(Tr.at(InIter).param[0]+ Shift_toCenterOfMagnetAX + Shift_toCenterOfMagnetAX * Tr.at(InIter).param[0]* Tr.at(InIter).param[0]);
        trackParamf.SetTy(Tr.at(InIter).param[2]+ Shift_toCenterOfMagnetAY + Shift_toCenterOfMagnetAY * Tr.at(InIter).param[2]* Tr.at(InIter).param[2]);

        FairTrackParam trackParaml;
        trackParaml.SetPosition(TVector3(x3+Shift_toCenterOfMagnetX, y3+Shift_toCenterOfMagnetY, z3));
        trackParaml.SetTx(Tr.at(InIter).param[0]+ Shift_toCenterOfMagnetAX + Shift_toCenterOfMagnetAX * Tr.at(InIter).param[0]* Tr.at(InIter).param[0]);
        trackParaml.SetTy(Tr.at(InIter).param[2]+ Shift_toCenterOfMagnetAY + Shift_toCenterOfMagnetAY * Tr.at(InIter).param[2]* Tr.at(InIter).param[2]);

        //if(fDebug)cout<<" itr "<<InIter<<" ax "<<Tr.at(InIter).param[0]+ Shift_toCenterOfMagnetAX + Shift_toCenterOfMagnetAX * Tr.at(InIter).param[0]* Tr.at(InIter).param[0]<<" bx "<<x3+Shift_toCenterOfMagnetX<<" z "<<z3<<endl;
        BmnTrack *track = new ((*fSiTracks)[fSiTracks->GetEntriesFast()]) BmnTrack();
        track->SetChi2(Tr.at(InIter).Chi2);
        track->SetNHits(Tr.at(InIter).Nhits);
        track->SetFlag(InIter);
        track->SetNDF(Tr.at(InIter).Pdg);
        track->SetParamFirst(trackParamf);
        track->SetParamLast(trackParaml);

    }//InIter
    //--------------------------------------------------------------------

}

void BmnSiliconTrackFinder::WriteSiliconHits(Int_t **NclustX_, Int_t **NclustXp_, Int_t **NhitsXYmod_,
                                             Double_t ***XCoord_, Double_t ***XClust_width_, Double_t ***XClust_, Double_t ***sumQx_,
                                             Double_t ***XpCoord_, Double_t ***XpClust_width_, Double_t ***XpClust_, Double_t ***sumQxp_,
                                             Double_t ***XspCoord_, Double_t ***YspCoord_, Double_t ***XspClust_width_, Double_t ***XpspClust_width_,
                                             Double_t ***XspClust_, Double_t ***XpspClust_, Double_t ***sumQxsp_, Double_t ***sumQxpsp_,
                                             Double_t ***SigmaX_, Double_t ***SigmaXp_,Double_t ***SigmspX_, Double_t ***SigmspXp_, Double_t ***SigmspY_, Int_t *** Sp_pdg_ ){
    Int_t countX = 0;
    for (Int_t ist = 1; ist < fNstations; ist++) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3) continue;
            if (ist == 2 && imod > 1) continue;
            for (Int_t cl = 0; cl < NclustX_[ist][imod]; cl++) {

                if (XCoord_[ist][imod][cl] < -900.) continue;
                //if(fDebug)cout<<" st "<<ist<<" hit: X "<<XCoord[ist][imod][cl] <<" Y "<<-100.<<" SigX "<<SigmaX[ist][imod][cl]<<" SigY "<<-100.<<endl;
                BmnSiliconHit* hit = new ((*fBmnSiliconHitsXArray)[countX])
                        BmnSiliconHit(0, TVector3(XCoord_[ist][imod][cl]+Shift_toCenterOfMagnetX, -100., Zstation[ist][imod]),
                                      TVector3(SigmaX_[ist][imod][cl], -100, -1), cl);
                hit->SetStation(ist);
                hit->SetModule(imod);
                hit->SetIndex(countX);

                hit->SetClusterSizeInLowerLayer(XClust_width_[ist][imod][cl]);//cluster size   (lower layer |||)
                hit->SetClusterSizeInUpperLayer(-1.);                        //cluster size   (upper layer ///or\\\)
                hit->SetStripPositionInLowerLayer(XClust_[ist][imod][cl]);    //strip position (lower layer |||)
                hit->SetStripPositionInUpperLayer(-1.);                      //strip position (upper layer ///or\\\)
                hit->SetStripTotalSignalInLowerLayer(sumQx_[ist][imod][cl]);  //sumQx
                hit->SetStripTotalSignalInUpperLayer(-1.);                   //sumQxp
                countX++;
            }
        }//imod
    }//ist

    Int_t countXp = 0;
    for (Int_t ist = 1; ist < fNstations; ist++) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3) continue;
            if (ist == 2 && imod > 1) continue;
            for (Int_t cl = 0; cl < NclustXp_[ist][imod]; cl++) {

                if (XpCoord_[ist][imod][cl] < -900.) continue;
                //if(fDebug)cout<<" st "<<ist<<" hit: X "<<XCoord_[ist][imod][cl] <<" Y "<<-100.<<" SigX "<<SigmaX_[ist][imod][cl]<<" SigY "<<-100.<<endl;
                BmnSiliconHit* hit = new ((*fBmnSiliconHitsXpArray)[countXp])
                        BmnSiliconHit(0, TVector3(XpCoord_[ist][imod][cl]+Shift_toCenterOfMagnetX, -100., Zstation[ist][imod]),
                                      TVector3(SigmaXp_[ist][imod][cl], -100, -1), cl);
                hit->SetStation(ist);
                hit->SetModule(imod);
                hit->SetIndex(countXp);

                hit->SetClusterSizeInLowerLayer(-1.);                         //cluster size   (lower layer |||)
                hit->SetClusterSizeInUpperLayer(XpClust_width_[ist][imod][cl]);//cluster size   (upper layer ///or\\\)
                hit->SetStripPositionInLowerLayer(-1.);                       //strip position (lower layer |||)
                hit->SetStripPositionInUpperLayer(XpClust_[ist][imod][cl]);    //strip position (upper layer ///or\\\)
                hit->SetStripTotalSignalInLowerLayer(-1.);                    //sumQx
                hit->SetStripTotalSignalInUpperLayer(sumQxp_[ist][imod][cl]);  //sumQxp
                countXp++;
            }
        }//imod
    }//ist

    Int_t countXY = 0;
    for (Int_t ist = 1; ist < fNstations; ist++) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3) continue;
            if (ist == 2 && imod > 1) continue;
            for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {

                if(fDebug)cout<<" st "<<ist<<" hit: X "<<XspCoord_[ist][imod][cl] <<" Y "<<YspCoord_[ist][imod][cl]<<" pdg "<<Sp_pdg_[ist][imod][cl]<<endl;
                if (XspCoord_[ist][imod][cl] < -900.) continue;
                BmnSiliconHit* hit = new ((*fBmnSiliconHitsXYArray)[countXY])
                        BmnSiliconHit(0, TVector3(XspCoord_[ist][imod][cl]+Shift_toCenterOfMagnetX, YspCoord_[ist][imod][cl]+Shift_toCenterOfMagnetY, Zstation[ist][imod]),
                                      TVector3(SigmspX_[ist][imod][cl], SigmspY_[ist][imod][cl], -1), cl);
                hit->SetStation(ist);
                hit->SetModule(imod);
                hit->SetIndex(countXY);
                hit->SetPdg(Sp_pdg_[ist][imod][cl]);
                hit->SetClusterSizeInLowerLayer(XspClust_width_[ist][imod][cl]);                         //cluster size   (lower layer |||)
                hit->SetClusterSizeInUpperLayer(XpspClust_width_[ist][imod][cl]);//cluster size   (upper layer ///or\\\)
                hit->SetStripPositionInLowerLayer(XspClust_[ist][imod][cl]);                       //strip position (lower layer |||)
                hit->SetStripPositionInUpperLayer(XpspClust_[ist][imod][cl]);    //strip position (upper layer ///or\\\)
                hit->SetStripTotalSignalInLowerLayer(sumQxsp_[ist][imod][cl]);                    //sumQx
                hit->SetStripTotalSignalInUpperLayer(sumQxpsp_[ist][imod][cl]);  //sumQxp
                countXY++;
            }
        }//imod
    }//ist
}


void BmnSiliconTrackFinder::MCefficiencyCalculation(vector<MC_points>& vec, vector<tracksX>& Tr){

    if (fDebug) cout<<" ---Silicon MC tracks association--"<<endl;
    //                   ax,   bx,    ay,  by
    Double_t delt[4] = {-999.,-999.,-999.,-999.};
    Double_t sig[4]  = {0.04, 0.08, 0.05, 0.08};
    
    Double_t dmatch = 0.;
    Double_t dmc_match[kMaxMC];
    Int_t    mc_tr_assoc[kMaxMC];
    
    for (Int_t j = 0; j < kMaxMC; j++) {
        dmc_match[j]   = 1000.;
        mc_tr_assoc[j] = -1;
    }
    
    Double_t dax = -999.;
    Double_t day = -999.;
    Double_t dx  = -999.;
    Double_t dy  = -999.;
    
    if (fDebug) cout<<" Nmc "<<vec.size()<<" Nsi "<<Tr.size()<<endl;
    
    Int_t Ngood_mc_tracks = 0;
    Int_t Ngood_reco_tracks = 0;
    Bool_t wasLi7 = 0;
    Bool_t wasHe4 = 0;

    for (Int_t itr = 0; itr < vec.size(); itr++) {//mc_tr

        //---MC Eff ---
        //---Den
        if (fDebug && vec.at(itr).Np >= 3 && vec.at(itr).wo3st == 0){
            hDen_mctrSi->Fill(0);
            Ngood_mc_tracks++;
            cout<<" SiDen "<<endl;
            if (vec.at(itr).Pdg == PDG_Li7) wasLi7 = 1;
            if (vec.at(itr).Pdg == PDG_He4) wasHe4 = 1;
        }
        
        for (Int_t InIter = 0;  InIter < Tr.size(); InIter++){//reco_tr

            delt[0] = vec.at(itr).param[0] - Tr.at(InIter).param[0];
            delt[1] = vec.at(itr).param[1] - Tr.at(InIter).param[1];
            delt[2] = vec.at(itr).param[2] - Tr.at(InIter).param[2];
            delt[3] = vec.at(itr).param[3] - Tr.at(InIter).param[3];
            
            if (fDebug){
                if (delt[0] > -900.) hdAx_MC_tr_comb->Fill(dax);
                if (delt[1] > -900.) hdX_MC_tr_comb ->Fill(dx);
                if (delt[2] > -900.) hdAy_MC_tr_comb->Fill(day);
                if (delt[3] > -900.) hdY_MC_tr_comb ->Fill(dy);
            }

            dmatch = 0.;
            dmatch = (delt[0]*delt[0])/(sig[0]*sig[0])+ (delt[1]*delt[1])/(sig[1]*sig[1])+
                    (delt[2]*delt[2])/(sig[2]*sig[2])+ (delt[3]*delt[3])/(sig[3]*sig[3]);

            if ( dmc_match[itr] > dmatch){
                dmc_match[itr]   = dmatch;
                mc_tr_assoc[itr] = InIter;
                dax = delt[0];
                dx  = delt[1];
                day = delt[2];
                dy  = delt[3];
            }
        }//reco

        if (mc_tr_assoc[itr] > -1){
            //if (fDebug && dax < -.00017)
            //cout<<"Siiiii fEventNo "<<fEventNo
            if (fDebug) cout <<" itr "<<itr<<" Np "<<vec.at(itr).Np<<" mc_Id "<<vec.at(itr).Id<<
                               " ax_mc "<<vec.at(itr).param[0]<<" reco_ind "<<mc_tr_assoc[itr]<<" ax "<<Tr.at(mc_tr_assoc[itr]).param[0]<<
                               " dmc_match "<<dmc_match[itr]<<endl;
            if (fDebug){
                if (dax > -900.) hdAx_MC_tr->Fill(dax);
                if (dx > -900.)  hdX_MC_tr ->Fill(dx);
                if (day > -900.) hdAy_MC_tr->Fill(day);
                if (dy > -900.)  hdY_MC_tr ->Fill(dy);
            }
        }
    }//mc_tr
    
    if (wasLi7 && wasHe4) {
        if (fDebug) cout <<" wasLi7 && wasHe4 "<<wasLi7<<" "<<wasHe4<<endl;
        hDen_mcreaction->Fill(0);
    }
    Int_t Nassoc = 0;
    
    if (fDebug) cout<<"Si reject poorly chosen association segments "<<endl;
    for (Int_t itr = 0; itr < vec.size(); itr++) {//mc_tr
        if (mc_tr_assoc[itr] == -1) continue;

        for (Int_t itr2 = 0; itr2 < vec.size(); itr2++) {//mc_tr
            if (itr2 == itr) continue;
            if (mc_tr_assoc[itr2] == -1) continue;

            if (mc_tr_assoc[itr] ==  mc_tr_assoc[itr2]){
                if (dmc_match[itr2] > 2*dmc_match[itr] ) mc_tr_assoc[itr2] = -1;
                else {
                    //mc_tr_assoc[itr] = -1;
                    continue;//break;
                }
            }
        }//itr2
        //---MC Eff ---
        //---Num
        if (fDebug) cout<<" mc_Id "<<vec.at(itr).Id<<" assoc "<<mc_tr_assoc[itr]<<" pdg "<<vec.at(itr).Pdg<<endl;
        if (fDebug && mc_tr_assoc[itr] > -1 && vec.at(itr).Np >= 3  && vec.at(itr).wo3st == 0){
            Nassoc++;
            Ngood_reco_tracks++;
            hNum_mctrSi->Fill(0);
            cout<<" Num "<<endl;
        }
    }//itr
    
    if (fDebug) cout<<" Nassoc "<<Nassoc<<endl;
    if (Nassoc == 2 && wasLi7 && wasHe4){
        if (fDebug) cout<<" hNum_mcreaction "<<endl;
        hNum_mcreaction->Fill(0);
    }
    if (fDebug){
        hNtrsi_mc ->Fill(Ngood_mc_tracks);
        hNtrsi_reco->Fill(Ngood_reco_tracks);
        hNtrsi_mc_vs_reco ->Fill(Ngood_reco_tracks,Ngood_mc_tracks);
    }
    
    for (Int_t InIter = 0;  InIter < Tr.size(); InIter++){//reco_tr
        for (Int_t itr = 0; itr < vec.size(); itr++) {//mc_tr
            if ( mc_tr_assoc[itr] == InIter){
                Tr.at(InIter).Pdg = vec.at(itr).Pdg;
            }
        }
    }
    for (Int_t InIter = 0;  InIter < Tr.size(); InIter++){//reco_tr
        if (fDebug) cout<<" InIter "<<InIter<<" Pdg "<<Tr.at(InIter).Pdg<<endl;
    }
}

void BmnSiliconTrackFinder::StripsReading(Double_t ***DigitsArrayX_, Double_t ***DigitsArrayXp_){

    Int_t stat, mod, layer, strip, new_strip; Double_t Ampl;
    for (Int_t iDig = 0; iDig < fBmnSiDigitsArray->GetEntriesFast(); ++iDig) {
        BmnSiliconDigit* digit = (BmnSiliconDigit*) fBmnSiDigitsArray->UncheckedAt(iDig);

        stat  = digit ->GetStation() +1;
        mod   = digit ->GetModule();
        layer = digit ->GetStripLayer();
        if (strip = digit ->GetStripNumber() - 2 < 0) continue;
        strip = digit ->GetStripNumber() -1;// Kapishin's digits start 1th strip & shift for src
        Ampl  = digit ->GetStripSignal();
        //if(fDebug) cout<<" stat "<< stat <<" mod "<< mod <<" layer "<<layer<< " strip "<<strip<<" Ampl "<<Ampl<<endl;
        if (layer == 0) {
            DigitsArrayX_[stat][mod][strip] = Ampl;
            // if(fDebug) cout<<" DigitsArrayX["<<stat<<"]["<<mod<<"]["<<strip<<"] = "<<DigitsArrayX_[stat][mod][strip]<<endl;
        }
        if (layer == 1) {
            if (fRunPeriod == 7 && stat == 1) {
                new_strip = nstripXpsm - strip;
                if (new_strip < 0) continue;
                DigitsArrayXp_[stat][mod][new_strip] = Ampl;
            } else {
                new_strip = nstripXp - strip;
                if (new_strip < 0) continue;
                DigitsArrayXp_[stat][mod][new_strip] = Ampl;
            }//else
            // if (stat == 3){
            // if (new_strip > 639) continue;
            // DigitsArrayXp_[stat][mod][new_strip+1] = DigitsArrayXp_[stat][mod][new_strip];
            // }
            // if(fDebug) cout<<" DigitsArrayXp["<<stat<<"]["<<mod<<"]["<<new_strip<<"] = "<<DigitsArrayXp_[stat][mod][new_strip]<<endl;
        }//layer == 1
    }//iDig
}


//-------------------Spatial coordinate calculation---------------------
void BmnSiliconTrackFinder::GetXYspatial(Int_t **NClX, Int_t **NClXp, Double_t ***XCoor, Double_t ***XpCoor, Double_t ***Xsp,
                                         Double_t ***Xpsp,  Double_t ***Ysp, Int_t **NXYsp, Double_t ***SigmX, Double_t ***SigmXp, Double_t ***SigspX, Double_t ***SigspXp, Double_t ***SigspY,
                                         Double_t ***XCl,   Double_t ***XCl_width, Double_t ***sQx, Double_t ***XpCl, Double_t ***XpCl_width, Double_t ***sQxp,
                                         Double_t ***XspCl, Double_t ***XspCl_width, Double_t ***sQxsp, Double_t ***XpspCl, Double_t ***XpspCl_width, Double_t ***sQxpsp){

    SensitiveAreaY = new Double_t[fNstations];

    for (Int_t istat = 1; istat < fNstations; istat++) {
        SensitiveAreaY[istat] = 2 * 6.0705 + 0.1148 + 0.2303;
    }
    if (fRunPeriod == 7){
        SensitiveAreaY[1] = 6.0705 + 0.1148;
    }
    SensitiveAreaY[0] = 0.;
    Double_t YCoor_cand = -1.;

    for (Int_t istat = 1; istat < fNstations; istat++) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            NXYsp[istat][imod] = 0;

            for (Int_t clx = 0; clx < NClX[istat][imod]; ++clx) {
                for (Int_t clxp = 0; clxp < NClXp[istat][imod]; ++clxp) {
                    if (XCoor[istat][imod][clx] > -900. && XpCoor[istat][imod][clxp] > -900.) {
                        //if ( fabs(XpCoor[istat][imod][clxp] - XCoor[istat][imod][clx]) > 0.3 ) continue;// VP 11.11.2021

                        YCoor_cand = (XpCoor[istat][imod][clxp] - XCoor[istat][imod][clx]) / tg2_5;

                        //if ( fDebug ) cout<<" istat "<<istat<<" imod "<<imod<<" X "<<XCoor[istat][imod][clx]<<" Xp "<<XpCoor[istat][imod][clxp] <<" Ycand "<<YCoor_cand <<" SensitiveAreaY[istat] "<<SensitiveAreaY[istat]<<endl;

                        if (YCoor_cand >= 0.1148 && YCoor_cand <= SensitiveAreaY[istat]) { // if (YCoor_cand >= 0 && YCoor_cand <= SensitiveAreaY[istat]) {

                            if(fRunPeriod == 7 && istat > 1 && YCoor_cand > 6.0705 + 0.1148  &&  YCoor_cand < 6.0705 + 0.1148 + 0.2303 ) continue;
                            //if (fRunPeriod == 7 && istat == 1)

                            if (fRunPeriod == 7 && istat > 1 && YCoor_cand <= half_module) {
                                YCoor_cand -= shift_after_zigzag;
                            }
                            // if ( fDebug ) cout<<" Y "<<YCoor_cand<<endl;
                            Xsp[istat][imod][NXYsp[istat][imod]]         = XCoor[istat][imod][clx];
                            SigspX[istat][imod][NXYsp[istat][imod]]      = SigmX[istat][imod][clx];
                            XspCl[istat][imod][NXYsp[istat][imod]]       = XCl[istat][imod][clx];
                            XspCl_width[istat][imod][NXYsp[istat][imod]] = XCl_width[istat][imod][clx];
                            sQxsp[istat][imod][NXYsp[istat][imod]]       = sQx[istat][imod][clx];

                            Xpsp[istat][imod][NXYsp[istat][imod]]        = XpCoor[istat][imod][clxp];
                            SigspXp[istat][imod][NXYsp[istat][imod]]     = SigmXp[istat][imod][clxp];
                            XpspCl[istat][imod][NXYsp[istat][imod]]      = XpCl[istat][imod][clxp];
                            XpspCl_width[istat][imod][NXYsp[istat][imod]]= XpCl_width[istat][imod][clxp];
                            sQxpsp[istat][imod][NXYsp[istat][imod]]      = sQxp[istat][imod][clxp];

                            Ysp[istat][imod][NXYsp[istat][imod]]         = YCoor_cand;
                            SigspY[istat][imod][NXYsp[istat][imod]]      = sqrt( pow (SigmX[istat][imod][clx],2) + pow(SigmXp[istat][imod][clxp],2) ) / tg2_5;
                            NXYsp[istat][imod]++;
                        }
                        // if (fDebug) cout<<" NXYsp["<<istat<<"]["<<imod<<"] "<<NXYsp[istat][imod]<<endl;

                    }//if X & Y > -900
                }//NClXp
            }//NClX
            // cout<<" NXYsp= "<<NXYsp[istat]<<endl;
        }//imod
    }// istat
    // if (fDebug) cout<<" end GetXYspatial "<<endl;
}// 
//----------------------------------------------------------------------



//------------------Cluster search function-----------------------------


void BmnSiliconTrackFinder::Clustering( Double_t*** DigitsArrayX_, Double_t *** DigitsArrayXp_, 
                                        Double_t *** XClust_,  Double_t *** XClust_width_, Double_t  *** sumQx_, Int_t **NclustX_,
                                        Double_t *** XpClust_, Double_t *** XpClust_width_, Double_t *** sumQxp_, Int_t **NclustXp_){
    // if (fDebug) cout<<" start clustering "<<endl;
    Double_t CoG;
    Int_t Strip_counterX;

    /*
  for (Int_t istat = 0; istat < fNstations; istat++) {
    for (Int_t imod = 0; imod < fNmodules; imod++) {
      // if (fDebug) cout<<" NclustX_["<<istat<<"]["<<imod<<"] "<<NclustX_[istat][imod]<<endl;
      for (Int_t istr = 0; istr < fNstrips; istr++) {
       if( DigitsArrayX_[istat][imod][istr] > 0.) printf("DigitsArrayX[%d][%d][%d] = %8.4f\n", istat, imod, istr, DigitsArrayX_[istat][imod][istr]);
      }
    }
  }
  */

    Int_t counter_strx_mod=0;
    Int_t indX = 0;

    for (Int_t istat = 1; istat < fNstations; istat++) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t istr = 0; istr < fNstrips; istr++) {
                if (istat == 1 && imod > 3) continue;
                if (istat == 2 && imod > 1) continue;

                if (fDebug && istat == 1 && DigitsArrayX_[istat][imod][istr] > 0.)  hoccupancyX1.at(imod) ->Fill(istr);
                if (fDebug && istat == 2 && DigitsArrayX_[istat][imod][istr] > 0.)  hoccupancyX2.at(imod) ->Fill(istr);
                if (fDebug && istat == 3 && DigitsArrayX_[istat][imod][istr] > 0.)  hoccupancyX3.at(imod) ->Fill(istr);

                if (fDebug && istat == 1 && DigitsArrayXp_[istat][imod][istr] > 0.)  hoccupancyXp1.at(imod) ->Fill(istr);
                if (fDebug && istat == 2 && DigitsArrayXp_[istat][imod][istr] > 0.)  hoccupancyXp2.at(imod) ->Fill(istr);
                if (fDebug && istat == 3 && DigitsArrayXp_[istat][imod][istr] > 0.)  hoccupancyXp3.at(imod) ->Fill(istr);

                if ( DigitsArrayX_[istat][imod][istr] == 0. && DigitsArrayX_[istat][imod][istr-1] > Cut_AmplX && DigitsArrayX_[istat][imod][istr + 1] > Cut_AmplX)
                    DigitsArrayX_[istat][imod][istr] = 0.5 * ( DigitsArrayX_[istat][imod][istr-1] + DigitsArrayX_[istat][imod][istr + 1] ) ;

                if (istat == 1 && imod == 0 ) {
                    // dead zone 405-412 strips
                    if ( DigitsArrayX_[istat][imod][404] > Cut_AmplX ) DigitsArrayX_[istat][imod][405] = 0.5* DigitsArrayX_[istat][imod][404] ;
                    if ( DigitsArrayX_[istat][imod][413] > Cut_AmplX ) DigitsArrayX_[istat][imod][412] = 0.5* DigitsArrayX_[istat][imod][413] ;
                }

                if ( DigitsArrayXp_[istat][imod][istr] == 0. && DigitsArrayXp_[istat][imod][istr-1] > 120. && DigitsArrayXp_[istat][imod][istr + 1] > 120.)
                    DigitsArrayXp_[istat][imod][istr] = 0.5 * ( DigitsArrayXp_[istat][imod][istr-1] + DigitsArrayXp_[istat][imod][istr + 1] );
            }
        }
    }

    Int_t NfirstX, NlastX, NfirstXp, NlastXp, ClusterSizeX, ClusterSizeXp;
    Double_t SumAmpl, SumAmplXp;

    for (Int_t istat = 1; istat < fNstations; istat++) {

        for (Int_t imod = 0; imod < fNmodules; imod++) {
            NfirstX = -1;
            NlastX = -1;
            ClusterSizeX = 0;
            SumAmpl = 0.;
            Strip_counterX = 0;

            // if ( fDebug) cout<<"----x-----"<<endl;
            for (Int_t istr = 0; istr < fNstrips; istr++){

                // if ( fDebug && DigitsArrayX_[istat][imod][istr] > 0.) cout<<" DigitsArrayX_["<<istat<<"]["<<imod<<"]["<<istr<<"] "<<DigitsArrayX_[istat][imod][istr]<<endl;

                // looking for start and end of cluster
                if (NfirstX < 0 && DigitsArrayX_[istat][imod][istr] == 0.) continue;
                if (NfirstX < 0 && DigitsArrayX_[istat][imod][istr] > 0.)  NfirstX = istr;
                if (NfirstX >= 0 && DigitsArrayX_[istat][imod][istr + 1] == 0. ) NlastX = istr;

                if (NfirstX >= 0 && NlastX > 0) {
                    // if(fDebug) cout<<" NfirstX "<<NfirstX<<" NlastX "<<NlastX<<endl;
                    Int_t max_ampl_strip = -1,
                            num_max_ampl_strip = -1,
                            num_left_strip = 0,
                            num_right_strip = 0,
                            num_centr_strip = -1 ;

                    ClusterSizeX = NlastX - NfirstX + 1;
                    // if ( fDebug) hClusterSizeX[istat][imod]->Fill(ClusterSizeX);
                    SumAmpl = 0.;
                    Int_t Counter_overflow = 0;

                    for (Int_t is = NfirstX; is < NlastX + 1; is++) {
                        Ampl_strX[is - NfirstX] = DigitsArrayX_[istat][imod][is];

                        if ( DigitsArrayX_[istat][imod][is] > Cut_overflow) Counter_overflow++;

                        if ( DigitsArrayX_[istat][imod][is] > max_ampl_strip ) {
                            max_ampl_strip = DigitsArrayX_[istat][imod][is];
                            num_max_ampl_strip = is;
                        }
                        //calculate center of cluster
                        CoG = FindClusterCenter(Ampl_strX, ClusterSizeX, SumAmpl);
                        //if(fDebug) cout<<" CoG "<<CoG <<endl;
                    }

                    for (Int_t is = NfirstX; is < NlastX + 1; is++) {
                        if ( max_ampl_strip > 0) {

                            Int_t dif_ampl = DigitsArrayX_[istat][imod][is] - max_ampl_strip;
                            Int_t dif_strip = is - num_max_ampl_strip;
                            if ( dif_strip == 0) num_centr_strip = is;
                            if ( dif_strip == 1) num_right_strip = is;
                            if ( dif_strip == -1) num_left_strip = is;
                        }
                    }


                    // if(fDebug) cout<<" ampl_centr strip "<<DigitsArrayX_[istat][imod][num_centr_strip]<<" Cut_AmplStripX "<<Cut_AmplStripX<<endl;
                    if ( num_centr_strip > 0 && DigitsArrayX_[istat][imod][num_centr_strip] < Cut_AmplStripX) {
                        //if(fDebug) cout<<" num_centr_strip > Cut_AmplStripX "<<endl;
                        NfirstX = -1;
                        NlastX = -1;
                        continue;
                    }

                    if ( SumAmpl < Cut_AmplX ) {
                        NfirstX = -1;
                        NlastX = -1;
                        continue;
                    }

                    //if(fDebug) cout<<" NfirstX "<<NfirstX<<" CoG "<<CoG<<" NfirstX + CoG "<<NfirstX + CoG<<endl;

                    XClust_[istat][imod][NclustX_[istat][imod]] = NfirstX + CoG;
                    XClust_width_[istat][imod][NclustX_[istat][imod]] = ClusterSizeX;
                    sumQx_[istat][imod][NclustX_[istat][imod]] = SumAmpl;
                    //  if(fDebug) cout<<" NclustX_["<<istat<<"]["<<imod<<"] "<<NclustX_[istat][imod]<<" NfX "<<NfirstX<<" NlX "<<NlastX<<" ClustSizeX "<<ClusterSizeX<<" XClust_ "<<XClust_[istat][imod][NclustX_[istat][imod]]<<endl;

                    NclustX_[istat][imod] ++;
                    // if(fDebug) cout<<" NclustX_["<<istat<<"]["<<imod<<"] "<<NclustX_[istat][imod]<<endl;

                    NfirstX = -1;
                    NlastX = -1;

                    // if ( fDebug) hNclustX[istat][imod]->Fill(NclustX_[istat][imod]);
                }// if (NfirstX >= 0 && NlastX > 0){
            }//end loop over X

            if (NclustX_[istat][imod] == 0) continue;

            NfirstXp = -1;
            NlastXp = -1;
            ClusterSizeXp = 0;
            SumAmplXp = 0.;
            //if ( fDebug) cout<<"----xp-----"<<endl;
            for (Int_t istr = 0; istr < fNstrips; istr++) {

                //if ( fDebug && DigitsArrayXp_[istat][imod][istr] > 0.) cout<<" DigitsArrayXp_["<<istat<<"]["<<imod<<"]["<<istr<<"] "<<DigitsArrayXp_[istat][imod][istr]<<endl;

                // looking for start and end of cluster
                if (NfirstXp < 0 && DigitsArrayXp_[istat][imod][istr] == 0.)      continue;
                if (NfirstXp < 0 && DigitsArrayXp_[istat][imod][istr] > 0.)       NfirstXp = istr;
                if (NfirstXp >= 0 && DigitsArrayXp_[istat][imod][istr + 1] == 0.) NlastXp = istr;

                if (NfirstXp >= 0 && NlastXp > 0) {

                    Int_t max_ampl_strip = -1,
                            num_max_ampl_strip = -1,
                            num_left_strip = 0,
                            num_right_strip = 0,
                            num_centr_strip = -1 ;

                    ClusterSizeXp = NlastXp - NfirstXp + 1;
                    // if ( fDebug) hClusterSizeXp[istat][imod]->Fill(ClusterSizeXp);
                    SumAmplXp = 0.;
                    Int_t Counter_overflow = 0;

                    for (Int_t is = NfirstXp; is < NlastXp + 1; is++) {
                        Ampl_strXp[is - NfirstXp] = DigitsArrayXp_[istat][imod][is];

                        if ( DigitsArrayXp_[istat][imod][is] > Cut_overflow) Counter_overflow++;

                        if ( DigitsArrayXp_[istat][imod][is] > max_ampl_strip ) {
                            max_ampl_strip = DigitsArrayXp_[istat][imod][is];
                            num_max_ampl_strip = is;
                        }

                        //calculate center of cluster
                        CoG = FindClusterCenter(Ampl_strXp, ClusterSizeXp, SumAmplXp);
                    }

                    for (Int_t is = NfirstX; is < NlastX + 1; is++) {
                        if ( max_ampl_strip > 0) {
                            Int_t dif_ampl = DigitsArrayXp_[istat][imod][is] - max_ampl_strip;
                            Int_t dif_strip = is - num_max_ampl_strip;
                            if ( dif_strip == 0) num_centr_strip = is;
                            if ( dif_strip == 1) num_right_strip = is;
                            if ( dif_strip == -1) num_left_strip = is;
                        }
                    }

                    //if ( fDebug) hsumQ_vs_clusterSize[istat][imod][1]->Fill(ClusterSizeXp, SumAmplXp);
                    //if ( fDebug && SumAmplXp > 0) hsumQ[istat][imod][1]->Fill(log10(float(SumAmplXp))/log10(float(2)));//Fill(SumAmplXp);

                    if ( num_centr_strip > 0 && DigitsArrayXp_[istat][imod][num_centr_strip] < Cut_AmplStripXp) {
                        // if(fDebug) cout<<" num_centr_strip > Cut_AmplStripX "<<endl;
                        NfirstXp = -1;
                        NlastXp = -1;
                        continue;
                    }

                    if ( SumAmplXp < Cut_AmplXp) {
                        NfirstXp = -1;
                        NlastXp = -1;
                        continue;
                    }
                    /*
          if ( Counter_overflow >= 2 ) {
           NfirstXp = -1;
           NlastXp = -1;
           continue;
          }
          */
                    //  if(fDebug) cout<<" NfirstXp "<<NfirstXp<<" CoG "<<CoG<<" NfirstXp + CoG "<<NfirstXp + CoG<<endl;
                    XpClust_[istat][imod][NclustXp_[istat][imod]] = NfirstXp + CoG;
                    XpClust_width_[istat][imod][NclustXp_[istat][imod]] = ClusterSizeXp;
                    sumQxp_[istat][imod][NclustXp_[istat][imod]] = SumAmplXp;
                    NclustXp_[istat][imod]++;
                    // if ( fDebug) cout<<"NclustXp_["<<istat<<"]["<<imod<<"] "<<NclustXp_[istat][imod]<<endl;

                    NfirstXp = -1;
                    NlastXp = -1;
                }// if (NfirstXp >= 0 && NlastXp > 0)
                //if ( fDebug) hNclustXp[istat][imod]->Fill(NclustXp_[istat][imod]);
            }//end loop over Xp
        }//mod

    }// end loop over station
}
//---clustering. -------------------------------------------------------



//--------------CoordinateCalculation-----------------------------------
void BmnSiliconTrackFinder::CoordinateCalculation(Int_t **NclustX_, Int_t **NclustXp_, Double_t ***XCoord_, Double_t ***XpCoord_, Double_t ***SigmaX_, Double_t ***SigmaXp_,
                                                  Double_t ***XspCoord_, Double_t ***XpspCoord_, Double_t ***YspCoord_, Double_t ***SigmspX_, Double_t ***SigmspXp_, Double_t ***SigmspY_,
                                                  Int_t **NhitsXYmod_,
                                                  Double_t ***XClust_, Double_t ***XClust_width_, Double_t ***sumQx_, Double_t ***XpClust_, Double_t ***XpClust_width_, Double_t ***sumQxp_,
                                                  Double_t ***XspClust_, Double_t ***XspClust_width_, Double_t ***sumQxsp_, Double_t ***XpspClust_, Double_t ***XpspClust_width_, Double_t ***sumQxpsp_,
                                                  vector<MC_points>& vec, Int_t *** Sp_pdg_ ){

    if(expData){
        //move X and Xp coordinates on frame size
        for (Int_t ist = 1; ist < fNstations; ist++) {
            for (Int_t imod = 0; imod < fNmodules; imod++) {
                if (ist == 1 && imod > 3) continue;
                if (ist == 2 && imod > 1) continue;
                // if(fDebug && NclustX_[ist][imod] > 0) cout<<" NclustX_["<<ist<<"]["<<imod<<"] "<<NclustX_[ist][imod]<<" NclustXp_["<<ist<<"]["<<imod<<"] "<<NclustXp_[ist][imod]<<endl;
                for (Int_t cl = 0; cl < NclustX_[ist][imod]; cl++) {
                    XCoord_[ist][imod][cl] = deltaX * XClust_[ist][imod][cl]; //cm
                    if (XClust_width[ist][imod][cl] > 1 ) SigmaX_[ist][imod][cl] = (XClust_width_[ist][imod][cl] * deltaX) / (2.*sq12); //VP empiric
                    else SigmaX_[ist][imod][cl] = deltaX / sq12;
                    //SigmaX_[ist][imod][cl] = (XClust_width_[ist][imod][cl] * deltaX) / sq12;

                    // if(fDebug) cout<<" XCoord_["<<ist<<"]["<<imod<<"]["<<cl<<"] "<<XCoord_[ist][imod][cl]<<" Sigma "<<SigmaX_[ist][imod][cl]<<endl;
                    // if(fDebug) cout<<" XCoord_["<<ist<<"]["<<imod<<"]["<<cl<<"] "<<XCoord_[ist][imod][cl]<<endl;
                }

                for (Int_t cl = 0; cl < NclustXp_[ist][imod]; cl++) {
                    XpCoord_[ist][imod][cl] = deltaXp * XpClust_[ist][imod][cl]; //cm
                    if (XpClust_width[ist][imod][cl] > 1 ) SigmaXp_[ist][imod][cl] = (XpClust_width_[ist][imod][cl] * deltaXp) / (2.*sq12); //VP empiric
                    else SigmaXp_[ist][imod][cl] = deltaXp / sq12;
                    // SigmaXp_[ist][imod][cl] = (XpClust_width_[ist][imod][cl] * deltaXp) / sq12;
                    //  if(fDebug) cout<<"XpCoord_["<<ist<<"]["<<imod<<"]["<<cl<<"] "<<XpCoord_[ist][imod][cl]<<" Sigma "<<SigmaXp_[ist][imod][cl]<<endl;
                    //if(fDebug) cout<<"XpCoord_["<<ist<<"]["<<imod<<"]["<<cl<<"] "<<XpCoord_[ist][imod][cl]<<endl;
                }
            }//imod
            // if(fDebug) cout<<endl;
        }//ist

        /*
  if (fDebug) cout << endl;
  for (Int_t ist = 1; ist < fNstations; ist++) {
    if (fDebug) cout << " XCoord_[" << ist << "] ";
    for (Int_t imod = 0; imod < fNmodules; imod++) {
      for (Int_t cl = 0; cl < NclustX_[ist][imod]; cl++) {
        if (fDebug) cout << XCoord_[ist][imod][cl] << " ";
      }
    }
    if (fDebug) cout << endl;
  }
  if (fDebug) cout << endl;
  for (Int_t ist = 1; ist < fNstations; ist++) {
    if (fDebug) cout << " XpCoord_[" << ist << "] ";
    for (Int_t imod = 0; imod < fNmodules; imod++) {
      for (Int_t cl = 0; cl < NclustXp_[ist][imod]; cl++) {
        if (fDebug) cout << XpCoord_[ist][imod][cl] << " ";
      }
    }
    if (fDebug) cout << endl;
  }
  
  if (fDebug) cout<<" GetXYspatial "<<endl;
  */
        GetXYspatial(NclustX_, NclustXp_, XCoord_, XpCoord_, XspCoord_, XpspCoord_, YspCoord_, NhitsXYmod_, SigmaX_, SigmaXp_, SigmspX_, SigmspXp_, SigmspY_,
                     XClust_, XClust_width_, sumQx_, XpClust_, XpClust_width_, sumQxp_, XspClust_, XspClust_width_, sumQxsp_, XpspClust_, XpspClust_width_, sumQxpsp_);

        /*
  for(Int_t ist = 1; ist < fNstations; ist++) {
    if(fDebug) cout<<" Xspatial["<<ist<<"] ";
     for (Int_t imod = 0; imod < fNmodules; imod++) {
      if (ist == 1 && imod > 3 ) continue;
      if (ist == 2 && imod > 1 ) continue;

      for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {
      if(fDebug) cout<<XspCoord_[ist][imod][cl]<<" ";
     }
    }
    if(fDebug) cout<<endl;
  }
  if(fDebug) cout<<endl;
  
  for(Int_t ist = 1; ist < fNstations; ist++) {
    if(fDebug) cout<<" Xpspatial["<<ist<<"] ";
     for (Int_t imod = 0; imod < fNmodules; imod++) {
      if (ist == 1 && imod > 3 ) continue;
      if (ist == 2 && imod > 1 ) continue;

      for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {
      if(fDebug) cout<<XpspCoord_[ist][imod][cl]<<" ";
     }
    }
    if(fDebug) cout<<endl;
  }
  if(fDebug) cout<<endl;

  for(Int_t ist = 1; ist < fNstations; ist++) {
    if(fDebug) cout<<" Yspatial["<<ist<<"] ";
     for (Int_t imod = 0; imod < fNmodules; imod++) {
      if (ist == 1 && imod > 3 ) continue;
      if (ist == 2 && imod > 1 ) continue;

      for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {
      if(fDebug) cout<<YspCoord_[ist][imod][cl]<<" SigmspY_"<<SigmspY_[ist][imod][cl] <<" ";
     }
    }
    if(fDebug) cout<<endl;
  }
  if(fDebug) cout<<endl;
 */

    }else{//--------------------MC------------------------------------------

        MC_points tmpTr;
        if (fDebug) cout<<" Sim: "<<endl;
        for (Int_t iMC = 0; iMC < fBmnHitsArray2->GetEntriesFast(); ++iMC) {
            BmnSiliconHit* hit = (BmnSiliconHit*)fBmnHitsArray2->UncheckedAt(iMC);

            Double_t x_MC     = hit->GetX();
            Double_t xp_MC    = hit->GetY();
            Double_t z_MC     = hit->GetZ();
            Int_t      Id     = hit->GetIndex();
            Int_t    st_mc    = -1;
            Double_t sigx_MC  = hit->GetDx();
            Double_t sigxp_MC = hit->GetDy();

            if (sigx_MC < 0 )  sigx_MC = -sigx_MC;
            if (sigxp_MC < 0 ) sigxp_MC= -sigxp_MC;

            if (z_MC > -320.){ st_mc = 3; if (fDebug) hSi_st3mc->Fill(x_MC,(xp_MC - x_MC)/tg2_5 );}
            if (z_MC < -434. && z_MC > -436.) {st_mc = 2; if (fDebug) hSi_st2mc->Fill(x_MC,(xp_MC - x_MC)/tg2_5 );}
            if (z_MC < -438.) {st_mc = 1; if (fDebug) hSi_st1mc->Fill(x_MC,(xp_MC - x_MC)/tg2_5 );}
            int imod = 2;
            if (st_mc == 2) imod = 1;

            XCoord_[st_mc][imod][NclustX[st_mc][imod]]   = x_MC;
            XpCoord_[st_mc][imod][NclustXp[st_mc][imod]] = xp_MC;
            SigmaX_[st_mc][imod][NclustXp[st_mc][imod]]  = sigx_MC;
            SigmaXp_[st_mc][imod][NclustXp[st_mc][imod]] = sigxp_MC;
            NclustX_[st_mc][imod]++;
            NclustXp_[st_mc][imod]++;

            XspCoord_[st_mc][imod][NhitsXYmod[st_mc][imod]]  = x_MC;
            if (xp_MC > -900.) XpspCoord_[st_mc][imod][NhitsXYmod[st_mc][imod]] = xp_MC;
            YspCoord_[st_mc][imod][NhitsXYmod[st_mc][imod]]  = (xp_MC - x_MC)/tg2_5;
            Sp_pdg_[st_mc][imod][NhitsXYmod[st_mc][imod]]    = hit->GetType();
            SigmspX_[st_mc][imod][NhitsXYmod[st_mc][imod]]   = sigx_MC;
            SigmspXp_[st_mc][imod][NhitsXYmod[st_mc][imod]]  = sigxp_MC;
            SigmspY_[st_mc][imod][NhitsXYmod[st_mc][imod]]   = sqrt( pow (sigx_MC,2) + pow(sigxp_MC,2) ) / tg2_5;
            if (fDebug) cout<<" Id "<<Id<<" x "<<x_MC<<" xp "<<xp_MC<<" y "<<YspCoord_[st_mc][imod][NhitsXYmod[st_mc][imod]] <<" z_MC "<<z_MC<<" st_mc "<<st_mc<<" pdg "<<hit->GetType()<<endl;

            NhitsXYmod_[st_mc][imod]++;
            //if (fDebug) cout<<" NhitsXYmod["<<st_mc<<"] "<<NhitsXYmod[st_mc][imod]<<endl;
        }//iMC

        if (fDebug) cout<<"True MC BmnSiliconHitClean"<<endl;
        int tr_before  = -1;
        int Nmc_tracks = -1;

        int Nst_mc[kMaxMC];
        int Np_in_3st[kMaxMC];
        int mcTracksArray[kMaxMC];
        int mcPdgCode[kMaxMC];
        double Xmc[kMaxMC][fNstations];
        double Ymc[kMaxMC][fNstations];
        double Zmc[kMaxMC][fNstations];
        for (Int_t  Id= 0; Id < kMaxMC; Id++) {
            Nst_mc[Id]        = 0;
            Np_in_3st[Id]     = 0;
            mcTracksArray[Id] = -1;
            mcPdgCode[Id]     = 0;
            for (Int_t  i = 0; i < fNstations; i++) {
                Xmc[Id][i] = -999.;
                Ymc[Id][i] = -999.;
                Zmc[Id][i] = -999.;
            }
        }

        for (Int_t iMC = 0; iMC < fBmnHitsArray->GetEntriesFast(); ++iMC) {
            BmnSiliconHit* hit = (BmnSiliconHit*)fBmnHitsArray->UncheckedAt(iMC);

            Double_t x_MC   = hit->GetX();
            Double_t y_MC   = hit->GetY();
            Double_t z_MC   = hit->GetZ();
            Int_t trackId_MC= hit->GetIndex();
            Int_t     st_mc = -1;
            Int_t   pdgCode = hit->GetType();//pdg

            if (tr_before != trackId_MC) {
                Nmc_tracks++;
                mcTracksArray[Nmc_tracks] = trackId_MC;
            }
            tr_before = trackId_MC;

            if (z_MC > -320.) {st_mc = 3; Np_in_3st[Nmc_tracks]++; }
            if (z_MC < -434. && z_MC > -436. ) st_mc = 2;
            if (z_MC < -438.) st_mc = 1;
            if (fDebug) cout<<" pdgId "<<trackId_MC<<" pdgCode "<<pdgCode<<" x "<<x_MC<<" y "<<y_MC<<" z "<<z_MC<<" st_mc "<<st_mc<<endl;

            Xmc[Nmc_tracks][st_mc] = x_MC;
            Ymc[Nmc_tracks][st_mc] = y_MC;
            Zmc[Nmc_tracks][st_mc] = z_MC;
            mcPdgCode[Nmc_tracks]  = pdgCode;
            Nst_mc[Nmc_tracks]++;

        }//iMC
        Nmc_tracks++;

        for (Int_t  Id= 0; Id < kMaxMC; Id++) {
            if (Nst_mc[Id] > 0 ){
                for (Int_t  i = 0; i < fNstations; i++) {
                    tmpTr.x[i]  = Xmc[Id][i];
                    tmpTr.y[i]  = Ymc[Id][i];
                    tmpTr.z[i]  = Zmc[Id][i];
                }
                tmpTr.Id = mcTracksArray[Id];
                tmpTr.Np = Nst_mc[Id];
                tmpTr.Pdg= mcPdgCode[Id];
                if (Nst_mc[Id] >= 2 && Np_in_3st[Id] > 0 ) vec.push_back(tmpTr);
            }
        }

        if (fDebug) cout<<" vec_points.size() "<<vec.size()<<endl;

        for (Int_t  itr = 0; itr < vec.size(); itr++) {
            //if (fDebug) cout<<" itr "<<itr<<" Pdg "<<vec.at(itr).Pdg<<endl;
            if (vec.at(itr).x[3] < -900.) vec.at(itr).wo3st = 1;
            if (vec.at(itr).x[1] > -900.){
                vec.at(itr).param[0]  = (vec.at(itr).x[1] - vec.at(itr).x[3])/ (vec.at(itr).z[1] - vec.at(itr).z[3]);
                vec.at(itr).param[1]  = (vec.at(itr).x[1] + vec.at(itr).x[3])*0.5;
                vec.at(itr).param[2]  = (vec.at(itr).y[1] - vec.at(itr).y[3])/ (vec.at(itr).z[1] - vec.at(itr).z[3]);
                vec.at(itr).param[3]  = (vec.at(itr).y[1] + vec.at(itr).y[3])*0.5;
            }else{
                vec.at(itr).param[0]  = (vec.at(itr).x[2] - vec.at(itr).x[3])/ (vec.at(itr).z[2] - vec.at(itr).z[3]);
                vec.at(itr).param[1]  = (vec.at(itr).x[2] + vec.at(itr).x[3])*0.5;
                vec.at(itr).param[2]  = (vec.at(itr).y[2] - vec.at(itr).y[3])/ (vec.at(itr).z[2] - vec.at(itr).z[3]);
                vec.at(itr).param[3]  = (vec.at(itr).y[2] + vec.at(itr).y[3])*0.5;
            }
            if (fDebug) cout<<" mcId "<<vec.at(itr).Id<<" Pdg "<<vec.at(itr).Pdg<<" Ax "<<vec.at(itr).param[0]<<" bx "<<vec.at(itr).param[1]<<" Ay "<<vec.at(itr).param[2]<<" by "<<vec.at(itr).param[3]<<" Np "<<vec.at(itr).Np<<endl;
            if (fDebug) cout<<" Xt "<<vec.at(itr).param[0]*( Z0_SRC_target - Zcentr ) + vec.at(itr).param[1]<<
                                                                                                               " Yt "<<vec.at(itr).param[2]*( Z0_SRC_target - Zcentr ) + vec.at(itr).param[3]<<endl;

            if (fDebug) hAxsi_mctrue->Fill(vec.at(itr).param[0]);
            if (fDebug) hAysi_mctrue->Fill(vec.at(itr).param[2]);
            if (fDebug) hBxsi_mctrue->Fill(vec.at(itr).param[1]);
            if (fDebug) hBysi_mctrue->Fill(vec.at(itr).param[3]);

        }

    }//if(!expData)
}
//----------------------------------------------------------------------



//-------------------Alignment------------------------------------------
void BmnSiliconTrackFinder::CoordinateAlignment( Int_t **NhitsXYmod_,  Double_t ***XspCoord_, Double_t ***XpspCoord_, Double_t ***YspCoord_,  
                                                 Int_t **NclustX_, Int_t **NclustXp_, Double_t ***XCoord_, Double_t ***XpCoord_){
    //if (fDebug) cout<<" Shift coordinates "<<endl;


    for (Int_t ist = 1; ist < fNstations; ist++) {
        //if ( fDebug ) cout<<" shiftStXtoGlob["<<ist<<"]= "<<shiftStXtoGlob[ist]<<" Y "<< shiftStYtoGlob[ist]<<endl;
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            // if ( fDebug ) cout<<" ist "<<ist<<" imod "<<imod<<endl;
            if (ist == 1 && imod > 3 ) continue;
            if (ist == 2 && imod > 1 ) continue;
            // if ( fDebug ) cout<<" shiftX["<<ist<<"]["<<imod<<"]= "<<shiftX[ist][imod]<<" shiftY["<<ist<<"]["<<imod<<"]= "<<shiftY[ist][imod]<<endl;
        }
    }

    if (fRunPeriod == 7){

        for(Int_t ist = 1; ist < fNstations; ist++) {
            for (Int_t imod = 0; imod < fNmodules; imod++) {
                if (ist == 1 && imod > 3 ) continue;
                if (ist == 2 && imod > 1 ) continue;
                for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {

                    if( (ist == 2 || ist == 3)) { // gap between Si planes
                        if( YspCoord_[ist][imod][cl] > 0. && YspCoord_[ist][imod][cl] <= 0.1151) continue;
                        if( YspCoord_[ist][imod][cl] < 0. && YspCoord_[ist][imod][cl] >= -0.1151) continue;
                        if( YspCoord_[ist][imod][cl] > 0. ) YspCoord_[ist][imod][cl] += 0.1151;
                    }
                    //rotate around X
                    if (
                            (ist == 1 && (imod == 1 || imod == 3)) ||
                            (ist == 2 && imod == 1) ||
                            (ist == 3 && (imod == 2 || imod ==3 || imod ==6 || imod ==7) )

                            ){
                        XspCoord_[ist][imod][cl]  = -XspCoord_[ist][imod][cl];
                        XpspCoord_[ist][imod][cl] = -XpspCoord_[ist][imod][cl];
                    }
                    //if(fDebug) cout<<" 2 "<<endl;
                    // if(fDebug) cout<<" after X -> -X "<<endl;
                    // if(fDebug) cout<<ist<<","<<imod<<" X "<<XspCoord_[ist][imod][cl] <<" Y "<<YspCoord_[ist][imod][cl]<<endl;

                    //rotate around Y
                    if (
                            (ist == 1 && (imod == 2 || imod == 3) )||
                            (ist == 3 && imod > 3 )
                            ) {
                        YspCoord_[ist][imod][cl] = -YspCoord_[ist][imod][cl];
                    }
                    // if(fDebug) cout<<" after Y -> -Y "<<endl;
                    // if(fDebug) cout<<ist<<","<<imod<<" X "<<XspCoord_[ist][imod][cl] <<endl;
                    // if(fDebug) cout<<" Y "<<YspCoord_[ist][imod][cl]<<endl;
                    // if(fDebug) cout<<" shift "<<shiftX[ist][imod]<<endl;
                    //if(fDebug) cout<<" 3 "<<endl;

                    XspCoord_[ist][imod][cl] += shiftX[ist][imod];
                    YspCoord_[ist][imod][cl] += shiftY[ist][imod];

                    // change the sign to global coord
                    XspCoord_[ist][imod][cl] = -XspCoord_[ist][imod][cl];
                    // if(fDebug) hXYspatial[ist]->Fill(XspCoord_[ist][imod][cl], YspCoord_[ist][imod][cl]);
                    // if(fDebug) cout<<" 4 "<<endl;
                    //--shift Xp---
                    //shift from Zubarev
                    XpspCoord_[ist][imod][cl] += shiftY[ist][imod]*(-Angle(ist,imod))*tg2_5 + shiftX[ist][imod];
                    //change global X
                    XpspCoord_[ist][imod][cl] = -XpspCoord_[ist][imod][cl];
                    //shift from Kolesnikov
                    XpspCoord_[ist][imod][cl] += shiftStYtoGlob[ist]*(Angle(ist,imod))*tg2_5 + shiftStXtoGlob[ist];
                    // if(fDebug) cout<<" 5 "<<endl;
                    //---shift X Y to global---
                    XspCoord_[ist][imod][cl] += shiftStXtoGlob[ist];
                    YspCoord_[ist][imod][cl] += shiftStYtoGlob[ist];

                    //if(fDebug){
                    // hYspatialst[ist]-> Fill( YspCoord_[ist][imod][cl]);
                    // cout<<" after shift in the end"<<endl;
                    // cout<<" X "<<XspCoord_[ist][imod][cl] <<" Y "<<YspCoord_[ist][imod][cl]<<endl;
                    // }
                }//cl
            }//imod
        }//ist


        for (Int_t ist = 1; ist < fNstations; ist++) {
            for (Int_t imod = 0; imod < fNmodules; imod++) {
                if (ist == 1 && imod > 3 ) continue;
                if (ist == 2 && imod > 1 ) continue;

                for (Int_t cl = 0; cl < NclustX_[ist][imod]; cl++) {
                    if (
                            (ist == 1 && (imod == 1 || imod == 3)) ||
                            (ist == 2 && imod == 1) ||
                            (ist == 3 && (imod == 2 || imod ==3 || imod ==6 || imod ==7) )
                            ) {
                        XCoord_[ist][imod][cl]= -XCoord_[ist][imod][cl];
                    }
                    XCoord_[ist][imod][cl] += shiftX[ist][imod];

                    //change the sign to global coord
                    XCoord_[ist][imod][cl] = -XCoord_[ist][imod][cl];
                    XCoord_[ist][imod][cl] += shiftStXtoGlob[ist];
                }//xcl

                for (Int_t cl = 0; cl < NclustXp_[ist][imod]; cl++) {
                    //rotate around X
                    if (
                            (ist == 1 && (imod == 1 || imod == 3)) ||
                            (ist == 2 && imod == 1) ||
                            (ist == 3 && (imod == 2 || imod ==3 || imod ==6 || imod ==7) )
                            ) {
                        XpCoord_[ist][imod][cl]= -XpCoord_[ist][imod][cl];
                    }
                    // if(fDebug) cout<<" after Xp -> -Xp "<<endl;
                    // if(fDebug) cout<<ist<<","<<imod<<" Xp "<<XpCoord_[ist][imod][cl] <<endl;
                    XpCoord_[ist][imod][cl] += shiftY[ist][imod]*(-Angle(ist,imod))*tg2_5 + shiftX[ist][imod];
                    // if(fDebug) cout<<" after shift "<<endl;
                    // if(fDebug) cout<<ist<<","<<imod<<" Xp "<<XpCoord_[ist][imod][cl] <<endl;

                    //change the sign to global coord
                    XpCoord_[ist][imod][cl] = -XpCoord_[ist][imod][cl];
                    // if(fDebug) cout<<"change the sign to global coord"<<endl;
                    // if(fDebug) cout<<ist<<","<<imod<<" Xp "<<XpCoord_[ist][imod][cl] <<endl;
                    XpCoord_[ist][imod][cl] += shiftStYtoGlob[ist]*(Angle(ist,imod))*tg2_5 + shiftStXtoGlob[ist];
                }//xp cl
            }//imod
        }//ist

    }//fRunPeriod == 7

    if (fRunPeriod == 8){
        Double_t Xtmp;
        //--Shift X & Y spatial coordinates--
        for(Int_t ist = 1; ist < fNstations; ist++) {
            for (Int_t imod = 0; imod < fNmodules; imod++) {
                for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {

                    //rotate around X
                    if (imod == 1 ){
                        XspCoord_[ist][imod][cl]  = -XspCoord_[ist][imod][cl];
                        XpspCoord_[ist][imod][cl] = -XpspCoord_[ist][imod][cl];
                    }
                    // if(fDebug) cout<<" after X -> -X "<<endl;
                    // if(fDebug) cout<<ist<<","<<imod<<" X "<<XspCoord_[ist][imod][cl] <<" Y "<<YspCoord_[ist][imod][cl]<<endl;

                    //rotate around Y -> the start of counting strips is at the top
                    YspCoord_[ist][imod][cl] = -YspCoord_[ist][imod][cl];

                    // if(fDebug) cout<<" after Y -> -Y "<<endl;
                    // if(fDebug) cout<<ist<<","<<imod<<" X "<<XspCoord_[ist][imod][cl] <<endl;
                    // if(fDebug) cout<<" Y "<<YspCoord_[ist][imod][cl]<<endl;

                    XspCoord_[ist][imod][cl]  += shiftX[ist][imod];
                    YspCoord_[ist][imod][cl]  += shiftY[ist][imod];
                    XpspCoord_[ist][imod][cl] += shiftY[ist][imod]*(-Angle(ist,imod))*tg2_5 + shiftX[ist][imod];
                    // if(fDebug) hXYspatial[ist]->Fill(XspCoord_[ist][imod][cl], YspCoord_[ist][imod][cl]);

                    //shift from Zubarev to Global orientation
                    if( ist == 1 ||  ist == 3){
                        Xtmp = XspCoord_[ist][imod][cl];
                        XspCoord_[ist][imod][cl] = YspCoord_[ist][imod][cl];
                        YspCoord_[ist][imod][cl] = Xtmp;
                    }
                    if( ist == 2 ||  ist == 4){
                        XspCoord_[ist][imod][cl]  = -XspCoord_[ist][imod][cl];
                        YspCoord_[ist][imod][cl]  = -YspCoord_[ist][imod][cl];
                    }

                    //---shift X Y to global---to beam line axis
                    XspCoord_[ist][imod][cl]  += shiftStXtoGlob[ist];
                    YspCoord_[ist][imod][cl]  += shiftStYtoGlob[ist];
                    XpspCoord_[ist][imod][cl] += shiftStYtoGlob[ist]*(Angle(ist,imod))*tg2_5 + shiftStXtoGlob[ist];

                    //if(fDebug){
                    // hYspatialst[ist]-> Fill( YspCoord_[ist][imod][cl]);
                    // cout<<" after shift in the end"<<endl;
                    // cout<<" X "<<XspCoord_[ist][imod][cl] <<" Y "<<YspCoord_[ist][imod][cl]<<endl;
                    // }
                }//cl
            }//imod
        }//ist

        //--Shift X & X' coordinates--
        for (Int_t ist = 1; ist < fNstations; ist++) {
            for (Int_t imod = 0; imod < fNmodules; imod++) {
                for (Int_t cl = 0; cl < NclustX_[ist][imod]; cl++) {
                    if (imod == 1) {
                        XCoord_[ist][imod][cl]= -XCoord_[ist][imod][cl];
                    }
                    XCoord_[ist][imod][cl] += shiftX[ist][imod];
                    //if ist == 1 ||  ist == 3 -> Y
                    if( ist == 2 ||  ist == 4){
                        XCoord_[ist][imod][cl] = - XCoord_[ist][imod][cl];
                    }
                    XCoord_[ist][imod][cl] += shiftStXtoGlob[ist];
                }//xcl

                for (Int_t cl = 0; cl < NclustXp_[ist][imod]; cl++) {
                    //rotate around X
                    if (imod == 1) {
                        XpCoord_[ist][imod][cl]= -XpCoord_[ist][imod][cl];
                    }
                    // if(fDebug) cout<<" after Xp -> -Xp "<<endl;
                    // if(fDebug) cout<<ist<<","<<imod<<" Xp "<<XpCoord_[ist][imod][cl] <<endl;
                    XpCoord_[ist][imod][cl] += shiftY[ist][imod]*(-Angle(ist,imod))*tg2_5 + shiftX[ist][imod];
                    //if ist == 1 ||  ist == 3 -> Yp
                    if( ist == 2 ||  ist == 4){
                        XpCoord_[ist][imod][cl] = - XpCoord_[ist][imod][cl];
                    }
                    // if(fDebug) cout<<" after shift "<<endl;
                    // if(fDebug) cout<<ist<<","<<imod<<" Xp "<<XpCoord_[ist][imod][cl] <<endl;
                    XpCoord_[ist][imod][cl] += shiftStYtoGlob[ist]*(Angle(ist,imod))*tg2_5 + shiftStXtoGlob[ist];
                }//xp cl
            }//imod
        }//ist

    }//RUN8

    if(fDebug) cout<<" --after shift-- "<<endl;

    for (Int_t ist = 1; ist < fNstations; ist++) {
        if (fDebug) cout << " XCoord_[" << ist << "] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < NclustX_[ist][imod]; cl++) {
                if (fDebug) cout << XCoord_[ist][imod][cl] << " ";
            }
        }
        if (fDebug) cout << endl;
    }
    if (fDebug) cout << endl;
    for (Int_t ist = 1; ist < fNstations; ist++) {
        if (fDebug) cout << " XpCoord_[" << ist << "] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < NclustXp_[ist][imod]; cl++) {
                if (fDebug) cout << XpCoord_[ist][imod][cl] << " ";
            }
        }
        if (fDebug) cout << endl;
    }
    if(fDebug) cout<<" --spatial-- "<<endl;
    
    for(Int_t ist = 1; ist < fNstations; ist++) {
        if(fDebug) cout<<" Xspatial["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3 ) continue;
            if (ist == 2 && imod > 1 ) continue;

            for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {
                if(fDebug) cout<<XspCoord_[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }
    if(fDebug) cout<<endl;
    
    for(Int_t ist = 1; ist < fNstations; ist++) {
        if(fDebug) cout<<" Xpspatial["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3 ) continue;
            if (ist == 2 && imod > 1 ) continue;

            for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {
                if(fDebug) cout<<XpspCoord_[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }
    if(fDebug) cout<<endl;
    
    for(Int_t ist = 1; ist < fNstations; ist++) {
        if(fDebug) cout<<" Yspatial["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3 ) continue;
            if (ist == 2 && imod > 1 ) continue;
            for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {
                if(fDebug) cout<<YspCoord_[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }
    if(fDebug) cout<<endl;

    for (Int_t ist = 1; ist < fNstations ; ist++) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3 ) continue;
            if (ist == 2 && imod > 1 ) continue;
            for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {
                if(fDebug && ist == 1)  hY_XSisp1bef->Fill(XspCoord_[ist][imod][cl],YspCoord_[ist][imod][cl]);
                if(fDebug && ist == 2)  hY_XSisp2bef->Fill(XspCoord_[ist][imod][cl],YspCoord_[ist][imod][cl]);
            }

        }
    }

}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnSiliconTrackFinder::CountSpatialPoints(Int_t **Nleftoversp_, Int_t *Nsp_st_){
    if(fDebug) cout<<" CountSpatialPoints "<<endl;

    for (Int_t st = 1; st < fNstations; ++st) {
        Nsp_st_[st] = 0;
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            //if ( fDebug) cout<<" Nleftoversp_["<<st<<"]["<<imod<<"] "<<Nleftoversp_[st][imod]<<endl;
            Nsp_st_[st] += Nleftoversp_[st][imod];
        }
    }
    if ( fDebug) cout<<" next case? Nsp_st[1] "<<Nsp_st[1]<<" Nsp_st[2] "<<Nsp_st[2]<<" Nsp_st[3] "<<Nsp_st[3]<<endl;
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
Bool_t BmnSiliconTrackFinder::SkipEvent(Int_t **NclustX_, Int_t **NclustXp_, Double_t ***XCoord_, Double_t ***XpCoord_){
    Bool_t skip = 0;

    Int_t NfithitsX[fNstations], NfithitsXp[fNstations];
    for (Int_t ist = 0; ist < fNstations; ist++) {
        NfithitsX[ist] = 0;
        NfithitsXp[ist] = 0;
    }

    for (Int_t ist = 1; ist < fNstations; ist++) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3 ) continue;
            if (ist == 2 && imod > 1 ) continue;

            for (Int_t cl = 0; cl < NclustX_[ist][imod]; cl++) {
                NfithitsX[ist]++;
            }//xcl

            for (Int_t cl = 0; cl < NclustXp_[ist][imod]; cl++) {
                NfithitsXp[ist]++;
            }//xp cl
        }//imod
    }//ist

    if (fDebug){
        cout<<" Number of hits: "<<endl;
        for (Int_t ist = 1; ist < fNstations; ist++) {
            cout<<" NfithitsX["<<ist<<"] "<<NfithitsX[ist]<<" NfithitsXp["<<ist<<"] "<<NfithitsXp[ist]<<endl;
        }
        cout<<endl;
    }
    //---------Skip event!!!!! with high Si-hits multiplicity ------------

    if ( (NfithitsX[1] + NfithitsX[2] + NfithitsX[3]) > 39 && (NfithitsXp[1] + NfithitsXp[2] + NfithitsXp[3]) > 36 ){
        if(fDebug) cout<<" --Skip event!!!!! with high Si-hits multiplicity --"<<endl;
        skip = 1;
    }
    if ( (NfithitsX[1] + NfithitsX[2] + NfithitsX[3]) > 51 ){
        if(fDebug) cout<<" --Skip event!!!!! with high Si-hits multiplicity --"<<endl;
        skip = 1;
    }
    return(skip);
}
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void BmnSiliconTrackFinder::RecordingTracksAfterSpatialPoints(vector<tracksX> & vec_tracks_, vector<tracksX> & CleanTr_){

    if(fDebug) cout<<endl;
    if(fDebug) cout<<" RecordingTracksAfterSpatialPoints size:"<<vec_tracks_.size()<<endl;
    for(int itr = 0; itr< vec_tracks_.size(); ++itr) {
        CleanTr_.push_back(vec_tracks_.at(itr));

        if (fDebug) cout<<" itr "<<itr<<" Chi2 "<<CleanTr_.at(itr).Chi2<<" Nhits "<<CleanTr_.at(itr).Nhits<<" Xv "<<CleanTr_.at(itr).Xv<<" Yv "<<CleanTr_.at(itr).Yv<<endl;
        for (Int_t st = 1; st < fNstations; ++st) {
            if (fDebug) cout<<" CleanTr_.at("<<itr<<").CoordX["<<st<<"] "<<CleanTr_.at(itr).CoordX[st]<< " Xp "<<CleanTr_.at(itr).CoordXp[st]<<endl;
        }
    }//itr
    if(fDebug) cout<<" CleanTr_.size() "<<CleanTr_.size()<<endl;
    if(fDebug) cout<<endl;

    //sorting vector
    sort(CleanTr_.begin(), CleanTr_.end(), compareSegments);
    if (fDebug) cout <<" sorting vector"<<endl;
    for (int itr = 0; itr < CleanTr_.size(); ++itr) {
        if (fDebug) cout << " itr " << itr << " Chi2 " << CleanTr_.at(itr).Chi2 <<endl;
    }
    for (int itr = 0; itr < CleanTr_.size(); ++itr) {
        for (Int_t st = 1; st < fNstations; ++st) {
            if (fDebug) cout<<" st "<<st<<" Z "<<CleanTr_.at(itr).CoordZ[st]<<endl;
            if (fDebug) cout<<" CleanTr.CoordX["<<st <<"]  "<<CleanTr_.at(itr).CoordX[st]<<" strip "<<CleanTr_.at(itr).StripNumX[st]<<" Clsize "<<CleanTr_.at(itr).ClSizeX[st]<<" Q "<<CleanTr_.at(itr).SumQX[st]<<endl;
            if (fDebug) cout<<" CleanTr.CoordXp["<<st <<"] "<<CleanTr_.at(itr).CoordXp[st]<<" strip "<<CleanTr_.at(itr).StripNumXp[st]<<" Clsize "<<CleanTr_.at(itr).ClSizeXp[st]<<" Q "<<CleanTr_.at(itr).SumQXp[st]<<endl;
        }//st
    }
    vec_tracks_.clear();
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnSiliconTrackFinder::RecordingTracks(vector<tracksX> & vec_tracks_, vector<tracksX> & CleanTr_){
    if(fDebug) cout<<endl;
    if(fDebug) cout<<" RecordingTracks size:"<<vec_tracks_.size()<<endl;
    if(fDebug) cout<<" CleanTr size: "<<CleanTr_.size()<<endl;
    Double_t ZposC[fNstations];
    ZposC[0] = 0.;
    ZposC[1] = 213.999 + 0.03 -6.9;
    ZposC[2] = 213.999 + 0.03 -6.9;
    ZposC[3] = 327.130 + 0.03;

    for (Int_t is = 1; is < fNstations; is++) {
        ZposC[is] += Z0_SRC_target;
    }
    for(int itr = 0; itr< vec_tracks_.size(); ++itr) {


        Double_t X1_cand = vec_tracks_.at(itr).param[0] * (ZposC[1] - Zcentr) + vec_tracks_.at(itr).param[1];
        Double_t Y1_cand = vec_tracks_.at(itr).param[2] * (ZposC[1] - Zcentr) + vec_tracks_.at(itr).param[3];

        Double_t X3_cand = vec_tracks_.at(itr).param[0] * (ZposC[3] - Zcentr) + vec_tracks_.at(itr).param[1];
        Double_t Y3_cand = vec_tracks_.at(itr).param[2] * (ZposC[3] - Zcentr) + vec_tracks_.at(itr).param[3];


        Bool_t Cross_6p = 0;

        for(int InIter = 0; InIter < CleanTr_.size(); ++InIter) {

            Double_t X1_tr = CleanTr_.at(InIter).param[0] * (ZposC[1] - Zcentr) + CleanTr_.at(InIter).param[1];
            Double_t Y1_tr = CleanTr_.at(InIter).param[2] * (ZposC[1] - Zcentr) + CleanTr_.at(InIter).param[3];

            Double_t X3_tr = CleanTr_.at(InIter).param[0] * (ZposC[3] - Zcentr) + CleanTr_.at(InIter).param[1];
            Double_t Y3_tr = CleanTr_.at(InIter).param[2] * (ZposC[3] - Zcentr) + CleanTr_.at(InIter).param[3];


            if ( X1_cand < X1_tr && X3_cand > X3_tr) {
                Cross_6p = 1;
                break;
            }
            if ( Y1_cand < Y1_tr && Y3_cand > Y3_tr) {
                Cross_6p = 1;
                break;
            }
            if ( X1_cand > X1_tr && X3_cand < X3_tr) {
                Cross_6p = 1;
                break;
            }
            if ( Y1_cand > Y1_tr && Y3_cand < Y3_tr) {
                Cross_6p = 1;
                break;
            }
        }

        if ( !Cross_6p) {
            if (fDebug) cout<<" itr "<<itr<<" CleanTr size: "<<CleanTr_.size()<<endl;
            CleanTr_.push_back(vec_tracks_.at(itr));

            if (fDebug) cout<<" itr "<<itr<<" Chi2 "<<vec_tracks_.at(itr).Chi2<<" Nhits "<<vec_tracks_.at(itr).Nhits<<" Xv "<<vec_tracks_.at(itr).Xv<<" Yv "<<vec_tracks_.at(itr).Yv<<endl;
            for (Int_t st = 1; st < fNstations; ++st) {
                if (fDebug){
                    cout<<" st "<<st<<" Z "<<vec_tracks_.at(itr).CoordZ[st]<<endl;
                    cout<<" CleanTr.CoordX["<<st <<"]  "<<vec_tracks_.at(itr).CoordX[st]<<" strip "<<vec_tracks_.at(itr).StripNumX[st]<<" Clsize "<<vec_tracks_.at(itr).ClSizeX[st]<<" Q "<<vec_tracks_.at(itr).SumQX[st]<<endl;
                    cout<<" CleanTr.CoordXp["<<st <<"] "<<vec_tracks_.at(itr).CoordXp[st]<<" strip "<<vec_tracks_.at(itr).StripNumXp[st]<<" Clsize "<<vec_tracks_.at(itr).ClSizeXp[st]<<" Q "<<vec_tracks_.at(itr).SumQXp[st]<<endl;
                }
            }//st
        }//Cross
    }//itr
    if(fDebug) cout<<endl;
    if(fDebug) cout<<" now CleanTr.size() "<<CleanTr_.size()<<endl;
    if(fDebug) cout<<endl;
    vec_tracks_.clear();
}
//----------------------------------------------------------------------



void BmnSiliconTrackFinder::RecordingTracks_case3(vector<tracksX> & vec_tracks_, vector<tracksX> & CleanTr_){
    if(fDebug) cout<<endl;
    if(fDebug) cout<<" RecordingTracks size:"<<vec_tracks_.size()<<endl;
    Double_t ZposC[fNstations];
    ZposC[0] = 0.;
    ZposC[1] = 213.999 + 0.03 -6.9;
    ZposC[2] = 213.999 + 0.03 -6.9;
    ZposC[3] = 327.130 + 0.03;

    if (fDebug) cout<<" after case 3 write cand: "<<vec_tracks_.size()<<endl;
    //---write candidate and check crossing---
    
    Double_t sigma_y = 0.1; //cm//only case3
    for(int itr = 0; itr< vec_tracks_.size(); ++itr){

        Double_t X1_cand = vec_tracks_.at(itr).param[0] * (ZposC[1] - Zcentr) + vec_tracks_.at(itr).param[1];
        Double_t Y1_cand = vec_tracks_.at(itr).param[2] * (ZposC[1] - Zcentr) + vec_tracks_.at(itr).param[3];

        Double_t X3_cand = vec_tracks_.at(itr).param[0] * (ZposC[3] - Zcentr) + vec_tracks_.at(itr).param[1];
        Double_t Y3_cand = vec_tracks_.at(itr).param[2] * (ZposC[3] - Zcentr) + vec_tracks_.at(itr).param[3];

        // if (fDebug) cout<<" X1_cand "<<X1_cand<<" X3_cand "<<X3_cand<<endl;
        // if (fDebug) cout<<" Y1_cand "<<Y1_cand<<" Y3_cand "<<Y3_cand<<endl;

        Bool_t Cross_6p = 0, rej_cand = 0;
        
        for(int InIter = 0; InIter < CleanTr_.size(); ++InIter){
            
            Double_t X1_tr = CleanTr_.at(InIter).param[0] * (ZposC[1] - Zcentr) + CleanTr_.at(InIter).param[1];
            Double_t Y1_tr = CleanTr_.at(InIter).param[2] * (ZposC[1] - Zcentr) + CleanTr_.at(InIter).param[3];
            
            Double_t X3_tr = CleanTr_.at(InIter).param[0] * (ZposC[3] - Zcentr) + CleanTr_.at(InIter).param[1];
            Double_t Y3_tr = CleanTr_.at(InIter).param[2] * (ZposC[3] - Zcentr) + CleanTr_.at(InIter).param[3];
            
            // if (fDebug) cout<<" X1_tr "<<X1_tr<<" X3_tr "<<X3_tr<<endl;
            // if (fDebug) cout<<" Y1_tr "<<Y1_tr<<" Y3_tr "<<Y3_tr<<endl;
            
            if (  X1_cand <  X1_tr &&  X3_cand  > X3_tr)            {Cross_6p = 1;  }
            if (  Y1_cand <  Y1_tr &&  (Y3_cand - sigma_y) > Y3_tr) {Cross_6p = 1;  }
            
            if (  X1_cand >  X1_tr &&  X3_cand < X3_tr)             {Cross_6p = 1;  }
            if (  Y1_cand >  Y1_tr &&  (Y3_cand + sigma_y) < Y3_tr) {Cross_6p = 1;  }
            

            if ( Cross_6p){
                if ( CleanTr_.at(InIter).Nhits > vec_tracks_.at(itr).Nhits ) {
                    rej_cand =1;

                    // if (fDebug) cout<<" rej itr "<<itr<<" rej_cand "<<rej_cand<<" Cross_6p "<<Cross_6p<<endl;
                }
                if ( !rej_cand && CleanTr_.at(InIter).Nhits < vec_tracks_.at(itr).Nhits ){
                    rej_cand = 0;
                    Cross_6p = 0;
                    CleanTr_.erase(CleanTr_.begin()+ InIter); //reject clean track
                    // if (fDebug) cout<<" rej Clean "<<InIter<<" rej_cand "<<rej_cand<<" Cross_6p "<<Cross_6p<<endl;
                }
                if( !rej_cand && Cross_6p ){
                    if (CleanTr_.at(InIter).Nhits == vec_tracks_.at(itr).Nhits && CleanTr_.at(InIter).Nhits == 4 ){
                        rej_cand = 1;
                        //if (fDebug) cout<<" rej itr "<<itr<<" rej_cand "<<rej_cand<<" Cross_6p "<<Cross_6p<<endl;
                    }
                }
                if ( Cross_6p ){
                    if ( CleanTr_.at(InIter).Nhits == vec_tracks_.at(itr).Nhits && CleanTr_.at(InIter).Nhits > 4 ){
                        if ( CleanTr_.at(InIter).Chi2 < vec_tracks_.at(itr).Chi2 ) rej_cand = 1;
                        if ( CleanTr_.at(InIter).Chi2 > vec_tracks_.at(itr).Chi2 ) {
                            CleanTr_.erase(CleanTr_.begin()+ InIter);
                            Cross_6p = 0;
                            rej_cand = 0;
                            //  if (fDebug) cout<<" rej Clean "<<InIter<<" rej_cand "<<rej_cand<<" Cross_6p "<<Cross_6p<<endl;
                        }
                    }

                }
            }//if ( Cross_6p)
            
        }//CleanTr_.size


        if ( !Cross_6p && !rej_cand ) {
            if (fDebug) cout<<" write good itr "<<itr<<endl;
            CleanTr_.push_back(vec_tracks_.at(itr));
            if (fDebug) cout<<" push good itr "<<itr<<endl;

            if (fDebug) cout<<" itr "<<itr<<" Chi2 "<<vec_tracks_.at(itr).Chi2<<" Nhits "<<vec_tracks_.at(itr).Nhits<<" Xv "<<vec_tracks_.at(itr).Xv<<" Yv "<<vec_tracks_.at(itr).Xv<<endl;
            for (Int_t st = 1; st < fNstations; ++st) {
                if (fDebug) cout<<" st "<<st<<" Z "<<CleanTr_.at(itr).CoordZ[st]<<endl;
                if (fDebug) cout<<" CleanTr.CoordX["<<st <<"]  "<<vec_tracks_.at(itr).CoordX[st]<<" strip "<<vec_tracks_.at(itr).StripNumX[st]<<" Clsize "<<vec_tracks_.at(itr).ClSizeX[st]<<" Q "<<vec_tracks_.at(itr).SumQX[st]<<endl;
                if (fDebug) cout<<" CleanTr.CoordXp["<<st <<"] "<<vec_tracks_.at(itr).CoordXp[st]<<" strip "<<vec_tracks_.at(itr).StripNumXp[st]<<" Clsize "<<vec_tracks_.at(itr).ClSizeXp[st]<<" Q "<<vec_tracks_.at(itr).SumQXp[st]<<endl;
            }
            
        }// if ( !Cross_6p && !rej_cand )
    }//itr
    if(fDebug) cout<<endl;
    if(fDebug) cout<<" now CleanTr_.size() "<<CleanTr_.size()<<endl;
    if(fDebug) cout<<endl;
    vec_tracks_.clear();
    //--write candidate and check crossing.--
}
//----------------------------------------------------------------------



//-----------------------Tracking:Case1---------------------------------
void BmnSiliconTrackFinder::Case1(Int_t **NhitsXYmod_, Double_t ***XspCoord_, Double_t ***XpspCoord_, Double_t ***YspCoord_, Double_t ***SigmspX_, Double_t ***SigmspXp_, Double_t ***SigmspY_,
                                  vector<tracksX> & out_tracks,
                                  Double_t ***XspClust_, Double_t ***XspClust_width_, Double_t ***sumQxsp_, Double_t ***XpspClust_, Double_t ***XpspClust_width_, Double_t ***sumQxpsp_){
    tracksX tmpTr;
    Int_t    NpX_in_line = 0;
    Double_t dminx1_x2   = 999.;
    Double_t dminx1_x3   = 999.;
    Double_t dminx3_xp3  = 999.;
    Double_t dminx1_xp1  = 999.;
    Double_t dminx2_xp2  = 999.;

    for ( Int_t i = 1; i < fNstations; ++i){
        iClustXMod[i]= -1;// reset pointers
        iModX[i]= -1;
    }

    Double_t bx_candidate = 999., by_candidate = 999.;
    Int_t iClustXMod_candidate = -1, iModX_candidate = -1;
    Double_t dxV_thr = .2, dyV_thr = .4;

    if(fDebug) cout<<endl;
    //----Spatial track----
    if(fDebug) cout<<" case1: Spatial track "<<endl;

    for (Int_t imod1 = 0; imod1 < fNmodules1; imod1++) {
        if(fDebug && NhitsXYmod_[1][imod1] > 0 ) cout<<" NhitsXYmod_[1]["<<imod1<<"] "<<NhitsXYmod_[1][imod1]<<endl;
        for (Int_t cl1 = 0; cl1 < NhitsXYmod_[1][imod1]; cl1++){
            dminx1_x2 = 999.;

            for (Int_t imod2 = 0; imod2 < fNmodules2; imod2++) {
                if(fDebug && NhitsXYmod_[2][imod2]) cout<<" NhitsXYmod_[2]["<<imod2<<"] "<<NhitsXYmod_[2][imod2]<<endl;
                for (Int_t cl2 = 0; cl2 < NhitsXYmod_[2][imod2]; cl2++){

                    NpX_in_line = 0;
                    // if(fDebug){
                    //cout<<" dX(1-2) ("<<half_roadX1_X2<<") "<<XspCoord_[1][imod1][cl1]  - XspCoord_[2][imod2][cl2]<<endl;
                    //cout<<" X1 -X1p ("<<half_roadX1_Xp1<<") "<<XspCoord_[1][imod1][cl1]  - XpspCoord_[1][imod1][cl1]<<endl;
                    //cout<<" X2_Xp2 ("<<half_roadX2_Xp2<<") "<<XspCoord_[2][imod2][cl2]  - XpspCoord_[2][imod2][cl2]<<endl;
                    //cout<< " Y1_Y2 ("<<half_roadY1_Y2<<") "<<YspCoord_[1][imod1][cl1]  - YspCoord_[2][imod2][cl2] <<endl;
                    //  }

                    if (fabs(  XspCoord_[1][imod1][cl1]  - XspCoord_[2][imod2][cl2]  )   > half_roadX1_X2)  continue; //X1-X2
                    if (fabs(  XspCoord_[1][imod1][cl1]  - XpspCoord_[1][imod1][cl1]  )  > half_roadX1_Xp1) continue; //X1-Xp1
                    if (fabs(  XspCoord_[2][imod2][cl2]  - XpspCoord_[2][imod2][cl2]  )  > half_roadX2_Xp2) continue; //X2-Xp2
                    if (fabs(  YspCoord_[1][imod1][cl1]  - YspCoord_[2][imod2][cl2]  )   > half_roadY1_Y2)  continue; //Y1-Y2

                    //if(fDebug) cout<<" aft. hrd "<<endl;

                    //if(fDebug) cout<<" dX(1-2) "<<XspCoord_[1][imod1][cl1]  - XspCoord_[2][imod2][cl2]<<endl;
                    if ( dminx1_x2 > fabs( XspCoord_[1][imod1][cl1]  - XspCoord_[2][imod2][cl2] ) ){
                        dminx1_x2 = fabs( XspCoord_[1][imod1][cl1]  - XspCoord_[2][imod2][cl2] );
                        iClustXMod[1] = cl1;
                        iClustXMod[2] = cl2;
                        iModX[1]      = imod1;
                        iModX[2]      = imod2;
                        NpX_in_line   = 2;
                    }

                    if ( NpX_in_line < 2) continue;

                    Bool_t was_candidate = 0;
                    bx_candidate = 999., by_candidate = 999.;
                    Int_t Imod1_candidate = -1, Imod2_candidate = -1;
                    Int_t iCl1_candidate  = -1, iCl2_candidate  = -1;

                    for (Int_t imod3 = 0; imod3 < fNmodules3; imod3++) {
                        if(fDebug && NhitsXYmod_[3][imod3] > 0 ) cout<<" NhitsXYmod_[3]["<<imod3<<"] "<<NhitsXYmod_[3][imod3]<<endl;
                        for (Int_t cl3 = 0; cl3 < NhitsXYmod_[3][imod3]; cl3++){

                            // if(fDebug) cout<<" X3 -X3p "<<XspCoord_[3][imod3][cl3]  - XpspCoord_[3][imod3][cl3]<<" half_roadX3_Xp3 "<<half_roadX3_Xp3<<endl;
                            // if(fDebug) cout<<" X3 "<<XspCoord_[3][imod3][cl3]<<endl;


                            if (fabs(  XspCoord_[3][imod3][cl3]  - XpspCoord_[3][imod3][cl3]  )  > half_roadX3_Xp3) continue;
                            /*
              Double_t x1_2 = 0.5 * ( XspCoord_[1][imod1][cl1] + XspCoord_[2][imod2][cl2] );
              Double_t y1_2 = 0.5 * ( YspCoord_[1][imod1][cl1] + YspCoord_[2][imod2][cl2] );
              Double_t z1_2 = 0.5 * ( Zstation[1][imod1] + Zstation[2][imod2]);

              Double_t ax_target_region = (XspCoord_[3][imod3][cl3] - x1_2 ) / (Zstation[3][imod3] - z1_2 );
              Double_t bx_target_region = ax_target_region * ( Z0_SRC_target - Zstation[3][imod3] ) + XspCoord_[3][imod3][cl3];//Zcentr ??????
              
              Double_t ay_target_region = (YspCoord_[3][imod3][cl3] - y1_2 ) / (Zstation[3][imod3] - z1_2 );
              Double_t by_target_region = ay_target_region * ( Z0_SRC_target - Zstation[3][imod3] ) + YspCoord_[3][imod3][cl3];
              
*/
                            Double_t ax = (XspCoord_[3][imod3][cl3] - XspCoord_[1][imod1][cl1] ) / (Zstation[3][imod3] - Zstation[1][imod1]);
                            Double_t bx_target_region = ax * ( Z0_SRC_target - Zstation[3][imod3] ) + XspCoord_[3][imod3][cl3];

                            Double_t ay = (YspCoord_[3][imod3][cl3] - YspCoord_[1][imod1][cl1] ) / (Zstation[3][imod3] - Zstation[1][imod1]);
                            Double_t by_target_region = ay * ( Z0_SRC_target - Zstation[3][imod3] ) + YspCoord_[3][imod3][cl3];

                            if(fDebug)   cout<<" X1 "<< XspCoord_[1][imod1][cl1]<<" X2 "<< XspCoord_[2][imod2][cl2]<<" X3 "<< XspCoord_[3][imod3][cl3]<<" was_candidate "<<was_candidate<<
                                               " ax "<<ax<<" ay "<<ay<<" Z1 "<<Zstation[1][imod1]<<" Z3 "<<Zstation[3][imod1]<<
                                               // " x_candidate "<<bx_candidate<<" x_target_region ("<<half_target_regionX <<") "<<bx_target_region<<" y_target_region ("<<half_target_regionY<<") "<<
                                               // by_target_region<<
                                               endl;

                            if(fDebug)   cout<<" bx_target_region "<<bx_target_region<<" half_target_regionX "<<half_target_regionX<<endl;
                            if ( fabs(kX_target - bx_target_region) > half_target_regionX) continue;
                            if(fDebug)   cout<<" by_target_region "<<by_target_region<<" half_target_regionY "<<half_target_regionY<<endl;
                            if ( fabs(kY_target - by_target_region) > half_target_regionY) continue;

                            if ( was_candidate == 0 ) {
                                bx_candidate         = bx_target_region;
                                by_candidate         = by_target_region;
                                iModX_candidate      = imod3;
                                iClustXMod_candidate = cl3;
                                was_candidate = 1;
                                if (fDebug) cout<<" write candidate: bx_target_region "<<bx_target_region<<" bx_candidate "<<bx_candidate<<" by_target_region "<<by_target_region<<" by_candidate "<<by_candidate<<endl;
                            }else{
                                //if (fDebug) cout<<" bx_target_region "<<bx_target_region<<" bx_candidate "<<" by_target_region "<<by_target_region<<" by_candidate "<<by_candidate<<endl;
                                if ( iModX_candidate == imod3 && iClustXMod_candidate == cl3 ) {if (fDebug) cout<<" 1"<<endl; continue;}
                                if ( fabs(kX_target - bx_target_region) > (fabs(kX_target - bx_candidate) + dxV_thr) ) {if (fDebug) cout<<" 2"<<endl; continue;}//0.2

                                if ( fabs(kY_target - by_target_region) > (fabs(kY_target - by_candidate) + dyV_thr) ) {if (fDebug) cout<<" 3"<<endl; continue;}//0.4
                                //if ( fabs(by_target_region) > (fabs(by_candidate) + dyV_thr) ) {if (fDebug) cout<<" 3"<<endl; continue;}//0.4
                                //if (fDebug) cout<<" left = "<<fabs(bx_target_region) + 0.55*fabs(by_target_region)<<" right = "<<fabs(bx_candidate) + 0.25*fabs(by_candidate)<<endl;

                                if ((fabs(kX_target - bx_target_region) + 0.25*fabs(kY_target - by_target_region) ) > (fabs(kX_target - bx_candidate) + 0.25*fabs(kY_target - by_candidate)) )
                                {if (fDebug) cout<<" 4"<<endl; continue;}// v2 closed to v1


                                bx_candidate         = bx_target_region;
                                by_candidate         = by_target_region;
                                iModX_candidate      = imod3;//iModX[3];
                                iClustXMod_candidate = cl3;//iClustXMod[3];
                                if (fDebug) cout<<" re - write "<<endl;
                            }

                        }//imod3
                    }//cl3;

                    if (bx_candidate > 900.) continue;

                    if ( tmpTr.CoordX[3]  == XspCoord_[3][iModX_candidate][iClustXMod_candidate] )  continue;
                    if ( tmpTr.CoordXp[3] == XpspCoord_[3][iModX_candidate][iClustXMod_candidate] ) continue;
                    //if(fDebug)   cout<<" X used "<<Xforglfit[3][iModX[3]]<<" X cand "<< XspCoord_[3][iModX_candidate][iClustXMod_candidate] <<endl;

                    if ( iModX_candidate > -1){
                        iModX[3]      = iModX_candidate;
                        iClustXMod[3] = iClustXMod_candidate;
                    }

                    //if(fDebug)   cout<<"end 3st: X1 "<< XspCoord_[1][iModX[1]][iClustXMod[1]]<<" X2 "<< XspCoord_[2][iModX[2]][iClustXMod[2]]<<
                    //        "X3 "<< XspCoord_[3][iModX_candidate][iClustXMod_candidate]<<" Xp3 "<< XpspCoord_[3][iModX_candidate][iClustXMod_candidate]<<
                    //        " Xv_candidate "<<bx_candidate<<" Yv_candidate "<<by_candidate<<endl;

                    for (Int_t ist = 1; ist < fNstations; ++ist) {
                        for (Int_t imod = 0; imod < fNmodules; imod++) {
                            Xforglfit[ist][imod]      = -999.;
                            Xpforglfit[ist][imod]     = -999.;
                            SigmXforglfit[ist][imod]  = -999.;
                            SigmXpforglfit[ist][imod] = -999.;
                        }
                    }

                    for (Int_t idx = 1; idx < fNstations; ++idx) {
                        if ( iClustXMod[idx] < 0 ) continue;
                        //if(fDebug)   cout<<" idx" <<idx<<" iModX "<<iModX[idx]<<" iCl "<<iClustXMod[idx]<<" X["<<idx<<"]["<<iClustXMod[idx]<<"] "<<XspCoord_[idx][iModX[idx]][iClustXMod[idx]]<<" Xp["<<idx<<"]["<<iClustXMod[idx]<<"] "<<XpspCoord_[idx][iModX[idx]][iClustXMod[idx]]<<" Z "<<Zstation[idx][iModX[idx]]<<" sigmaX "<<SigmspX_[idx][iModX[idx]][iClustXMod[idx]]<<" sigmaXp "<<SigmspXp_[idx][iModX[idx]][iClustXMod[idx]]<<endl;

                        Xforglfit[idx][iModX[idx]]     = XspCoord_[idx][iModX[idx]][iClustXMod[idx]];
                        SigmXforglfit[idx][iModX[idx]] = SigmspX_[idx][iModX[idx]][iClustXMod[idx]];

                        Xpforglfit[idx][iModX[idx]]    = XpspCoord_[idx][iModX[idx]][iClustXMod[idx]];
                        SigmXpforglfit[idx][iModX[idx]]= SigmspXp_[idx][iModX[idx]][iClustXMod[idx]];

                        Yforglfit[idx][iModX[idx]]     = YspCoord_[idx][iModX[idx]][iClustXMod[idx]];
                        SigmYforglfit[idx][iModX[idx]] = SigmspY_[idx][iModX[idx]][iClustXMod[idx]];
                    }//idx

                    //---------------------------	GlobalFit---------------------------------
                    Int_t    TotalNumberOfHits = 0;
                    Double_t ChiSquareNdf      = 0.;
                    ChiSquare                  = 0.;
                    line[4];

                    for (Int_t coeff = 0; coeff < 4; ++coeff) {
                        line[coeff] = 0.;
                    }

                    calculationOfChi2(Xforglfit, Xpforglfit, SigmXforglfit, SigmXpforglfit, iClustXMod, iModX, ChiSquare, line);

                    TotalNumberOfHits = 6;
                    ChiSquareNdf = ChiSquare/(TotalNumberOfHits - 4);

                    if ( fDebug) cout<<" before cut ChiSquareNDF "<<ChiSquareNdf<<" Chi2_Globcut "<<Chi2_Globcut<<" TotalNumberOfHits  "<<TotalNumberOfHits <<endl;
                    if (fDebug) cout<<endl;

                    if ( ChiSquareNdf > Chi2_Globcut ) continue;
                    /*
          if ( fabs(line[2] * ( Z0_SRC_target - Zcentr ) + line[3]) > half_target_regionY){
           if (fDebug) cout<<" case1:--------half_target_regionY cut: "<<half_target_regionY<<" Yv "<<fabs(line[2] * ( Z0_SRC_target - Zcentr ) + line[3])<<endl;
           ChiSquareNdf = 999.;
           continue;
          }
          */
                    if (TotalNumberOfHits ==  6  ){

                        for(int itr = 0; itr< out_tracks.size(); ++itr){
                            //deleting randomly recorded tracks
                            if (
                                    (out_tracks.at(itr).CoordX[1] ==  Xforglfit[1][iModX[1]] && out_tracks.at(itr).CoordXp[1] == Xpforglfit[1][iModX[1]] )
                                    ||
                                    (out_tracks.at(itr).CoordX[2] ==  Xforglfit[2][iModX[2]] && out_tracks.at(itr).CoordXp[2] == Xpforglfit[2][iModX[2]] )
                                    ||
                                    (out_tracks.at(itr).CoordX[3] ==  Xforglfit[3][iModX[3]] && out_tracks.at(itr).CoordXp[3] == Xpforglfit[3][iModX[3]] )){
                                if ( out_tracks.at(itr).Chi2 >  ChiSquareNdf) out_tracks.erase(out_tracks.begin()+ itr);
                                else  ChiSquareNdf = 999.;
                            }//if
                        }//itr

                        if ( ChiSquareNdf > Chi2_Globcut ) continue;
                        // write candidate to tmp vector
                        tmpTr.Chi2     = ChiSquareNdf ;
                        tmpTr.Nhits    = TotalNumberOfHits;
                        tmpTr.Xv       = bx_candidate;
                        tmpTr.Yv       = by_candidate;
                        tmpTr.param[0] = line[0];
                        tmpTr.param[1] = line[1];
                        tmpTr.param[2] = line[2];
                        tmpTr.param[3] = line[3];

                        if (fDebug) cout<<" Xv_candidate  "<<bx_candidate<<" Yv_candidate "<<by_candidate<<endl;
                        for (Int_t st = 1; st < fNstations; ++st) {
                            if (iModX[st] < 0) continue;

                            tmpTr.ModNum[st]  = iModX[st];
                            tmpTr.CoordX[st]  = Xforglfit[st][iModX[st]];
                            tmpTr.CoordY[st]  = Yforglfit[st][iModX[st]];
                            tmpTr.CoordXp[st] = Xpforglfit[st][iModX[st]];
                            tmpTr.SigmaX[st]  = SigmXforglfit[st][iModX[st]];
                            tmpTr.SigmaY[st]  = SigmYforglfit[st][iModX[st]];
                            tmpTr.SigmaXp[st] = SigmXpforglfit[st][iModX[st]];
                            tmpTr.CoordZ[st]  = ZstnCforglfit[st][iModX[st]];

                            tmpTr.StripNumX[st] = XspClust_[st][iModX[st]][iClustXMod[st]];
                            tmpTr.StripNumXp[st]= XpspClust_[st][iModX[st]][iClustXMod[st]];
                            tmpTr.ClSizeX[st]   = XspClust_width_[st][iModX[st]][iClustXMod[st]];
                            tmpTr.ClSizeXp[st]  = XpspClust_width_[st][iModX[st]][iClustXMod[st]];
                            tmpTr.SumQX[st]     = sumQxsp_[st][iModX[st]][iClustXMod[st]];
                            tmpTr.SumQXp[st]    = sumQxpsp_[st][iModX[st]][iClustXMod[st]];

                            if (fDebug) cout<<" st "<<st<<" Z "<<tmpTr.CoordZ[st]<<endl;
                            if (fDebug) cout<<" CoordX["<<st <<"]  "<<tmpTr.CoordX[st]<<" strip "<<tmpTr.StripNumX[st]<<" Clsize "<<tmpTr.ClSizeX[st]<<" Q "<<tmpTr.SumQX[st]<<endl;
                            if (fDebug) cout<<" CoordXp["<<st <<"] "<<tmpTr.CoordXp[st]<<" strip "<<tmpTr.StripNumXp[st]<<" Clsize "<<tmpTr.ClSizeXp[st]<<" Q "<<tmpTr.SumQXp[st]<<endl;
                        }//st

                        out_tracks.push_back(tmpTr);

                        if (fDebug) cout<<" ax "<<tmpTr.param[0]<<" bx "<<tmpTr.param[1]<<" ay "<<tmpTr.param[2]<<" by "<<tmpTr.param[3]<<" Chi2 "<<tmpTr.Chi2<<endl;
                        if (fDebug) cout<<" write "<<out_tracks.size()<<endl;
                    }//if 6 points
                }//2
            }//2
        }//1
    }//1

}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnSiliconTrackFinder::CheckPoints(Int_t **NclustX_, Int_t **NclustXp_, 
                                        Double_t ***XCoord_, Double_t ***XpCoord_, Double_t ***SigmaX_, Double_t ***SigmaXp_,Double_t ***XspCoord_, Double_t ***XpspCoord_, Double_t ***YspCoord_, Double_t ***SigmspX_, Double_t ***SigmspXp_, Double_t ***SigmspY_,
                                        Int_t **NhitsXYmod_, vector<tracksX> & tracks,Int_t **Nspatial, Int_t **NpointsX,Int_t **NpointsXp,
                                        Double_t ***pointsXsp, Double_t ***pointsXpsp,Double_t ***pointsYsp,Double_t ***pointsXsigsp,Double_t ***pointsXpsigsp, Double_t ***pointsYsigsp,
                                        Double_t ***pointsX, Double_t ***pointsXp, Double_t ***pointsXsig,Double_t ***pointsXpsig){

    if(fDebug) cout<<" CheckPoints "<<endl;

    for (Int_t st = 0; st < fNstations; ++st) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            Nspatial[st][imod]  = 0;
            NpointsX[st][imod]   = 0;
            NpointsXp[st][imod]  = 0;
            for (Int_t j = 0; j < kBig; j++) {
                pointsXsp[st][imod][j]     = -999.;
                pointsXpsp[st][imod][j]    = -999.;
                pointsYsp[st][imod][j]     = -999.;
                pointsXsigsp[st][imod][j]  = -999.;
                pointsXpsigsp[st][imod][j] = -999.;
                pointsYsigsp[st][imod][j]  = -999.;
                pointsX[st][imod][j]       = -999.;
                pointsXp[st][imod][j]      = -999.;
                pointsXsig[st][imod][j]    = -999.;
                pointsXpsig[st][imod][j]   = -999.;
            }
        }
    }
    //--spatial points---
    for (Int_t st = 1; st < fNstations; ++st){
        for (Int_t imod = 0; imod < fNmodules; imod++) {

            for (Int_t cl = 0; cl < NhitsXYmod_[st][imod]; cl++){ //spatial points
                Int_t clux = -1;
                for (int itr = 0 ; itr < tracks.size(); ++itr){

                    if ( XspCoord[st][imod][cl] == tracks.at(itr).CoordX[st]){
                        clux = cl;
                        continue;
                    }
                    if ( XpspCoord[st][imod][cl] == tracks.at(itr).CoordXp[st]){
                        clux = cl;
                        continue;
                    }
                }//tr
                if ( clux > -1) continue;
                pointsXsp[st][imod][Nspatial[st][imod]]    = XspCoord[st][imod][cl] ;
                pointsXpsp[st][imod][Nspatial[st][imod]]   = XpspCoord[st][imod][cl] ;
                pointsYsp[st][imod][Nspatial[st][imod]]    = YspCoord[st][imod][cl] ;
                pointsXsigsp[st][imod][Nspatial[st][imod]] = SigmspX[st][imod][cl];
                pointsXpsigsp[st][imod][Nspatial[st][imod]]= SigmspXp[st][imod][cl];
                pointsYsigsp[st][imod][Nspatial[st][imod]] = SigmspY[st][imod][cl];
                Nspatial[st][imod]++;

            }//cl
        }//mod
    }//st
    //----line points---
    for (Int_t st = 1; st < fNstations; ++st) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < NclustX[st][imod]; cl++){//Xpoints
                Int_t clux = -1;
                for (int itr = 0 ; itr < tracks.size(); ++itr){
                    if ( XCoord[st][imod][cl] == tracks.at(itr).CoordX[st]){
                        clux = cl;
                        continue;
                    }
                }//tr
                if ( clux > -1) continue;
                pointsX[st][imod][NpointsX[st][imod]]  = XCoord[st][imod][cl] ;
                pointsXsig[st][imod][NpointsX[st][imod]]= SigmaX[st][imod][cl] ;
                NpointsX[st][imod]++;
            }//cl

            for (Int_t cl = 0; cl < NclustXp[st][imod]; cl++){//Xp points
                Int_t clux = -1;
                for (int itr = 0 ; itr < tracks.size(); ++itr){
                    if ( XpCoord[st][imod][cl] == tracks.at(itr).CoordXp[st]){
                        clux = cl;
                        continue;
                    }
                }//tr
                if ( clux > -1) continue;
                pointsXp[st][imod][NpointsXp[st][imod]]  = XpCoord[st][imod][cl] ;
                pointsXpsig[st][imod][NpointsX[st][imod]] = SigmaXp[st][imod][cl] ;
                NpointsXp[st][imod]++;
            }//cl
        }//mod
    }//st

    if(fDebug) cout<<" after rejection "<<endl;

    for(Int_t ist = 1; ist < fNstations; ist++){
        if(fDebug) cout<<" pointsXsp["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < Nspatial[ist][imod]; cl++){
                if(fDebug) cout<<pointsXsp[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }
    if(fDebug) cout<<endl;

    for(Int_t ist = 1; ist < fNstations; ist++) {
        if(fDebug) cout<<" pointsX["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < NpointsX[ist][imod]; cl++){
                if(fDebug) cout<<pointsX[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }
    if(fDebug) cout<<endl;

    for(Int_t ist = 1; ist < fNstations; ist++) {
        if(fDebug) cout<<" pointsXp["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < NpointsXp[ist][imod]; cl++){
                if(fDebug) cout<<pointsXp[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }
    if(fDebug) cout<<endl;

}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void BmnSiliconTrackFinder::CheckLeftover(Int_t **Nspatial, Int_t **NpointsX,Int_t **NpointsXp, 
                                          Double_t ***pointsXsp, Double_t ***pointsXpsp,Double_t ***pointsYsp,
                                          Double_t ***pointsXsigsp,Double_t ***pointsXpsigsp, Double_t ***pointsYsigsp,
                                          Double_t ***pointsX, Double_t ***pointsXp, Double_t ***pointsXsig,
                                          Double_t ***pointsXpsig,  vector<tracksX> & CleanTr_,
                                          Int_t **NhitsXYmod_, Double_t ***XspCoord_, Double_t ***YspCoord_, Double_t ***SigmspX_, Double_t ***SigmspY_){

    if(fDebug) cout<<" CheckLeftover"<<endl;
    bool woSp[fNstations];
    for (Int_t st = 0; st < fNstations ; ++st) {
        woSp[st]= 0;
    }
    //adding a hit from a track. the case when the plane didn't work
    if(fDebug) cout<<endl;
    if (fDebug && CleanTr_.size() > 0)  cout<<"track was but: "<<endl;

    for (Int_t InIter = 0;  InIter < CleanTr_.size(); ++InIter){
        //only for 5 point tracks
        if (CleanTr_.at(InIter).Nhits < 5 || CleanTr_.at(InIter).Nhits == 6) continue;

        if(fDebug) cout<<" itr "<<InIter<<" if Nhits = 5: "<<CleanTr_.at(InIter).Nhits<<endl;
        for (Int_t st = 1; st < fNstations -1 ; ++st) {
            woSp[st]= 0;

            //if the track has no point in 1 or 2 stations
            if ( CleanTr_.at(InIter).CoordX[st] < -900.){

                if (fDebug)  cout<<"no X["<<st<<"]: "<< CleanTr_.at(InIter).CoordX[st]<<endl;
                if (fDebug)  cout<<" was sp points? "<<endl;
                for (Int_t imod = 0; imod < fNmodules; imod++) {
                    if ( st == 1 && imod > 3) continue;
                    if ( st == 2 && imod > 1) continue;

                    if (fDebug)  cout<<" Nspatial["<<st<<"]= "<<NhitsXYmod_[st][imod]<<endl;
                    //checking the number of spatial points in 1 or 2 stations
                    if (NhitsXYmod_[st][imod] > 0 ) woSp[st] = 1;

                    for (Int_t cl = 0; cl < NhitsXYmod_[st][imod]; cl++){
                        if(fDebug) cout<<"st["<<st<<"] "<<" X "<<XspCoord_[st][imod][cl]<<endl;
                    }

                }//mod
                Double_t FitX = CleanTr_.at(InIter).param[0] * CleanTr_.at(InIter).CoordZ[st] + CleanTr_.at(InIter).param[1];
                Double_t FitY = CleanTr_.at(InIter).param[2] * CleanTr_.at(InIter).CoordZ[st] + CleanTr_.at(InIter).param[3];

                //adding a spatial point from the track if there is no point
                if ( woSp[st] == 0){
                    if(fDebug) cout<<" woSp["<<st<<"] "<<woSp[st]<<"-> FitX "<<FitX<<" FitY "<<FitY<<endl;
                    XspCoord_[st][1][NhitsXYmod_[st][1]] = FitX;
                    YspCoord_[st][1][NhitsXYmod_[st][1]] = FitY;
                    SigmspX_[st][1][NhitsXYmod_[st][1]]  = 0.0060;
                    SigmspY_[st][1][NhitsXYmod_[st][1]]  = 0.2;
                    NhitsXYmod_[st][1]++;
                }//woSp[st] == 1

            }//CoordX[st] < -900.

            if ( CleanTr_.at(InIter).CoordY[st] < -900.){
                if (fDebug)  cout<<"no Y["<<st<<"] "<< CleanTr_.at(InIter).CoordY[st]<<endl;
                for (Int_t imod = 0; imod < fNmodules; imod++) {
                    if ( st == 1 && imod > 3) continue;
                    if ( st == 2 && imod > 1) continue;

                    if (fDebug)  cout<<" Nspatial = "<<NhitsXYmod_[st][imod]<<endl;
                    //checking the number of spatial points in 1 or 2 stations
                    if (NhitsXYmod_[st][imod] > 0 ) woSp[st] = 1;

                    for (Int_t cl = 0; cl < NhitsXYmod_[st][imod]; cl++){
                        if(fDebug) cout<<"st["<<st<<"] "<<" Y "<<YspCoord_[st][imod][cl]<<endl;
                    }
                }//mod
                Double_t FitX = CleanTr_.at(InIter).param[0] * CleanTr_.at(InIter).CoordZ[st] + CleanTr_.at(InIter).param[1];
                Double_t FitY = CleanTr_.at(InIter).param[2] * CleanTr_.at(InIter).CoordZ[st] + CleanTr_.at(InIter).param[3];

                if ( woSp[st] == 0){
                    if(fDebug) cout<<" woSp["<<st<<"] "<<woSp[st]<<"-> FitX "<<FitX<<" FitY "<<FitY<<endl;
                    XspCoord_[st][1][NhitsXYmod_[st][1]] = FitX;
                    YspCoord_[st][1][NhitsXYmod_[st][1]] = FitY;
                    SigmspX_[st][1][NhitsXYmod_[st][1]]  = 0.0060;
                    SigmspY_[st][1][NhitsXYmod_[st][1]]  = 0.2;
                    NhitsXYmod_[st][1]++;
                }//woSp[st] == 1

            }//CoordXp[st] < -900.
        }//st
        //NhitsXYmod_[ist][imod]
    }// CleanTr_.size()
    //adding a hit from a track.


    //print all unused hits
    for(Int_t ist = 1; ist < fNstations; ist++){
        if(fDebug) cout<<" pointsXsp["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < Nspatial[ist][imod]; cl++){
                if(fDebug) cout<<pointsXsp[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }
    if(fDebug) cout<<endl;

    for(Int_t ist = 1; ist < fNstations; ist++){
        if(fDebug) cout<<" pointsXpsp["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < Nspatial[ist][imod]; cl++){
                if(fDebug) cout<<pointsXpsp[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }
    if(fDebug) cout<<endl;
    for(Int_t ist = 1; ist < fNstations; ist++) {
        if(fDebug) cout<<" pointsX["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < NpointsX[ist][imod]; cl++){
                if(fDebug) cout<<pointsX[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }
    for(Int_t ist = 1; ist < fNstations; ist++) {
        if(fDebug) cout<<" pointsXp["<<ist<<"] ";
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            for (Int_t cl = 0; cl < NpointsXp[ist][imod]; cl++){
                if(fDebug) cout<<pointsXp[ist][imod][cl]<<" ";
            }
        }
        if(fDebug) cout<<endl;
    }

    //recording points from crossed planes
    for(Int_t ist = 1; ist < fNstations -1; ist++){
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if ( ist == 1 && imod > 3) continue;
            if ( ist == 2 && imod > 1) continue;
            woSp[ist]= 0;

            if (NhitsXYmod_[ist][imod] > 0 ) woSp[ist] = 1;
        }
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (woSp[ist] == 1) continue;
            if (fDebug)  cout<< " woSp["<<ist<<"] "<<woSp[ist]<<endl;

            for (Int_t cl = 0; cl < NpointsX[ist][imod]; cl++){
                for(Int_t is = 1; is < fNstations -1 ; is++) {
                    for (Int_t im = 0; im < fNmodules; im++) {
                        if ( is == 1 && im > 3) continue;
                        if ( is == 2 && im > 1) continue;

                        for (Int_t clp = 0; clp < NpointsXp[is][im]; clp++){
                            if (ist == 1 && is == 2){
                                if(fDebug)  cout<< " X["<<ist<<"] "<<pointsX[ist][imod][cl]<<" Xp["<<is<<"] "<< pointsXp[is][im][clp]<<" XXp "<<pointsX[ist][imod][cl] - pointsXp[is][im][clp]<<endl;
                                if (fDebug && fabs(pointsX[ist][imod][cl] - pointsXp[is][im][clp]) > 0.) hXXp12CheckLeftover->Fill(pointsX[ist][imod][cl] - pointsXp[is][im][clp]);
                                if (fDebug && fabs(pointsX[ist][imod][cl] - pointsXp[is][im][clp]) > 0. && fabs(pointsX[ist][imod][cl] - pointsXp[is][im][clp]) < 0.2 ){
                                    hXXp12CheckLeftover03->Fill(pointsX[ist][imod][cl] - pointsXp[is][im][clp]);

                                    XspCoord_[ist][imod][NhitsXYmod_[ist][imod]] = pointsX[ist][imod][cl];
                                    YspCoord_[ist][imod][NhitsXYmod_[ist][imod]] =(pointsX[ist][imod][cl] - pointsXp[is][im][clp]) / tg2_5;
                                    SigmspX_[ist][imod][NhitsXYmod_[ist][imod]]  = pointsXsig[ist][imod][cl];
                                    SigmspY_[ist][imod][NhitsXYmod_[ist][imod]]  = 0.2;
                                    if(fDebug)  cout<< " Ysp "<<(pointsX[ist][imod][cl] - pointsXp[is][im][clp]) / tg2_5<<endl;
                                    NhitsXYmod_[ist][imod]++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if(fDebug) cout<<endl;


    for(Int_t ist = 1; ist < fNstations -1 ; ist++) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if ( ist == 1 && imod > 3) continue;
            if ( ist == 2 && imod > 1) continue;
            woSp[ist]= 0;

            //checking the number of spatial points in 1 or 2 stations
            if (NhitsXYmod_[ist][imod] > 0 ) woSp[ist] = 1;
        }
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (woSp[ist] == 1) continue;
            if (fDebug)  cout<< " woSp["<<ist<<"] "<<woSp[ist]<<endl;

            for (Int_t cl = 0; cl < NpointsXp[ist][imod]; cl++){
                for(Int_t is = 1; is < fNstations -1; is++) {
                    for (Int_t im = 0; im < fNmodules; im++) {
                        if ( is == 1 && im > 3) continue;
                        if ( is == 2 && im > 1) continue;

                        for (Int_t clp = 0; clp < NpointsX[is][im]; clp++){
                            if (ist == 1 && is == 2){
                                if (fDebug)  cout<< " Xp["<<ist<<"] "<<pointsXp[ist][imod][cl]<<" X["<<is<<"] "<< pointsX[is][im][clp]<<" XXp "<<pointsXp[ist][imod][cl] - pointsX[is][im][clp]<<endl;
                                if (fDebug && fabs(pointsXp[ist][imod][cl] - pointsX[is][im][clp]) > 0.)   hXXp12CheckLeftover->Fill(pointsXp[ist][imod][cl] - pointsX[is][im][clp]);
                                if (fDebug && fabs(pointsXp[ist][imod][cl] - pointsX[is][im][clp]) > 0. && fabs(pointsXp[ist][imod][cl] - pointsX[is][im][clp]) < 0.3 ){
                                    hXXp12CheckLeftover03->Fill(pointsXp[ist][imod][cl] - pointsX[is][im][clp]);

                                    XspCoord_[ist][imod][NhitsXYmod_[ist][imod]] = pointsX[is][im][clp];
                                    YspCoord_[ist][imod][NhitsXYmod_[ist][imod]] =(pointsX[is][im][clp] - pointsXp[ist][imod][cl]) / tg2_5;
                                    SigmspX_[ist][imod][NhitsXYmod_[ist][imod]]  = pointsXsig[is][im][clp];
                                    SigmspY_[ist][imod][NhitsXYmod_[ist][imod]]  = 0.2;
                                    if(fDebug)  cout<< " Ysp "<<(pointsX[is][im][clp] - pointsXp[ist][imod][cl]) / tg2_5<<endl;
                                    NhitsXYmod_[ist][imod]++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    
    for(Int_t is = 1; is < fNstations -1; is++) {
        for (Int_t im = 0; im < fNmodules; im++) {
            if ( is == 1 && im > 3) continue;
            if ( is == 2 && im > 1) continue;
            // if (fDebug)  cout<< " NhitsXYmod_["<<is<<"]["<<im<<"] "<< NhitsXYmod_[is][im]<<endl;
            for(Int_t icl = 0; icl < NhitsXYmod_[is][im]; icl++) {
                // if (fDebug)  cout<< " Xsp "<< XspCoord_[is][im][icl]<<endl;
            }
        }
    }

    for (Int_t ist = 1; ist < fNstations ; ist++) {
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3 ) continue;
            if (ist == 2 && imod > 1 ) continue;
            for (Int_t cl = 0; cl < NhitsXYmod_[ist][imod]; cl++) {
                if(fDebug && ist == 1)  hY_XSisp1->Fill(XspCoord_[ist][imod][cl],YspCoord_[ist][imod][cl]);
                if(fDebug && ist == 2)  hY_XSisp2->Fill(XspCoord_[ist][imod][cl],YspCoord_[ist][imod][cl]);
            }

        }
    }


}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void BmnSiliconTrackFinder::PrintAllTracks(vector<tracksX> & CleanTr_){
    cout<<endl;
    cout<<"-----end of track finding ------  Ntr "<<  CleanTr_.size()<<endl;
    Double_t ver_averx = 0., ver_avery = 0., sum_xv = 0., sum_yv = 0.;
    hNtracks->Fill(CleanTr_.size());
    for (Int_t InIter = 0;  InIter < CleanTr_.size(); ++InIter){

        hNpoint->Fill(CleanTr_.at(InIter).Nhits  );
        hAxglob->Fill(CleanTr_.at(InIter).param[0]);
        hBxglob->Fill(CleanTr_.at(InIter).param[1]);
        hAyglob->Fill(CleanTr_.at(InIter).param[2]);
        hByglob->Fill(CleanTr_.at(InIter).param[3]);


        //if (CleanTr_.at(InIter).param[3] < -4.) cout<<" !!!!! -4. "<<endl;
        hChiSquareNdf->Fill(CleanTr_.at(InIter).Chi2);
        if (InIter == 0) hAx_first_tr->Fill(CleanTr_.at(InIter).param[0]);
        else hAx_more_first_tr->Fill(CleanTr_.at(InIter).param[0]);
        hvertexXY->Fill(CleanTr_.at(InIter).Xv, CleanTr_.at(InIter).Yv);

        sum_xv +=  CleanTr_.at(InIter).Xv;
        sum_yv +=  CleanTr_.at(InIter).Yv;
        //for Run8
        Double_t Z1 = Z0_SRC_target + 126.;
        Double_t Z2 = Z0_SRC_target + 176.;

        Double_t X1   = CleanTr_.at(InIter).param[0] * ( Z1 -Zcentr) + CleanTr_.at(InIter).param[1];
        Double_t Y1   = CleanTr_.at(InIter).param[2] * ( Z1 -Zcentr) + CleanTr_.at(InIter).param[3];

        Double_t X2   = CleanTr_.at(InIter).param[0] * ( Z2 -Zcentr) + CleanTr_.at(InIter).param[1];
        Double_t Y2   = CleanTr_.at(InIter).param[2] * ( Z2 -Zcentr) + CleanTr_.at(InIter).param[3];

        hYXSi1_run8->Fill(X1,Y1);
        hYXSi2_run8->Fill(X2,Y2);
        hXSi1_run8->Fill(X1);
        hYSi1_run8->Fill(Y1);
        hXSi2_run8->Fill(X2);
        hYSi2_run8->Fill(Y2);

        cout<<endl;
        cout<<" ---iTr "<<InIter<<" Nhits "<<CleanTr_.at(InIter).Nhits<<
              " ax "<<CleanTr_.at(InIter).param[0]<<" bx "<<CleanTr_.at(InIter).param[1]<<" ay "<<CleanTr_.at(InIter).param[2] <<" by "<<CleanTr_.at(InIter).param[3]<<
              " Chi2 "<<CleanTr_.at(InIter).Chi2<<endl;
        cout<<"    Xv "<<CleanTr_.at(InIter).Xv<<" Yv "<<CleanTr_.at(InIter).Yv<<endl;
        cout<<" --X--"<<endl;
        Double_t X1_0 = 999., X1_1= 999., X1_2= 999., X1_3= 999., X2_0= 999.,X2_1= 999.,X3_1= 999.,X3_2= 999.;
        //-------X--------
        for (Int_t st = 1; st < fNstations; ++st) {
            if ( CleanTr_.at(InIter).CoordX[st] < -900.) continue;
            Double_t Hit = CleanTr_.at(InIter).CoordX[st];
            Double_t Fit = CleanTr_.at(InIter).param[0] * CleanTr_.at(InIter).CoordZ[st] + CleanTr_.at(InIter).param[1];
            cout<<" st "<<st<<" mod "<<CleanTr_.at(InIter).ModNum[st]<<" Xhit "<<Hit<<" Xfit "<< Fit<<" dX "<<Hit - Fit<<" Z "<<CleanTr_.at(InIter).CoordZ[st]<< " strip "<<CleanTr_.at(InIter).StripNumX[st]<<" Clsize "<<CleanTr_.at(InIter).ClSizeX[st]<<" Q "<<CleanTr_.at(InIter).SumQX[st]<<endl;

            if (st == 1 ){
                hX_st_1 -> Fill(CleanTr_.at(InIter).CoordX[st]);
                hdXvsXst_1 -> Fill(Hit,Hit - Fit);
                hdX_st_1-> Fill(Hit - Fit);
                if (CleanTr_.at(InIter).ModNum[st] == 0 ) {
                    X1_0 = CleanTr_.at(InIter).CoordX[1];
                    hdXvsXst1_0 -> Fill(CleanTr_.at(InIter).CoordX[1],CleanTr_.at(InIter).CoordX[1] - Fit);
                }
                if (CleanTr_.at(InIter).ModNum[st] == 1 ){
                    X1_1 = CleanTr_.at(InIter).CoordX[1];
                    hdXvsXst1_1 -> Fill(CleanTr_.at(InIter).CoordX[1],CleanTr_.at(InIter).CoordX[1] - Fit);
                }
                if (CleanTr_.at(InIter).ModNum[st] == 2 ){
                    X1_2 = CleanTr_.at(InIter).CoordX[1];
                    hdXvsXst1_2 -> Fill(CleanTr_.at(InIter).CoordX[1],CleanTr_.at(InIter).CoordX[1] - Fit);
                }
                if (CleanTr_.at(InIter).ModNum[st] == 3 ){
                    X1_3 = CleanTr_.at(InIter).CoordX[1];
                    hdXvsXst1_3 -> Fill(CleanTr_.at(InIter).CoordX[1],CleanTr_.at(InIter).CoordX[1] - Fit);
                }
            }
            if (st == 2 ){
                hX_st_2 -> Fill(CleanTr_.at(InIter).CoordX[st]);
                hdXvsXst_2 -> Fill(Hit,Hit - Fit);
                hdX_st_2-> Fill(Hit - Fit);
                if (CleanTr_.at(InIter).ModNum[2] == 0 ){
                    X2_0 = CleanTr_.at(InIter).CoordX[2];
                    hdXvsXst2_0 -> Fill(CleanTr_.at(InIter).CoordX[2],CleanTr_.at(InIter).CoordX[2] - Fit);
                }
                if (CleanTr_.at(InIter).ModNum[2] == 1 ){
                    X2_1 = CleanTr_.at(InIter).CoordX[2];
                    hdXvsXst2_1 -> Fill(CleanTr_.at(InIter).CoordX[2],CleanTr_.at(InIter).CoordX[2] - Fit);
                }

            }
            if (st == 3 ){
                hX_st_3 -> Fill(CleanTr_.at(InIter).CoordX[st]);
                hdXvsXst_3 -> Fill(Hit,Hit - Fit);
                hdX_st_3-> Fill(Hit - Fit);
                if (CleanTr_.at(InIter).ModNum[3] == 1 ){
                    X3_1 = CleanTr_.at(InIter).CoordX[3];
                    hdXvsXst3_1 -> Fill(CleanTr_.at(InIter).CoordX[3],CleanTr_.at(InIter).CoordX[3] - Fit);
                    hdXXp3_mod1 -> Fill(CleanTr_.at(InIter).CoordX[st] - CleanTr_.at(InIter).CoordXp[st]);
                }
                if (CleanTr_.at(InIter).ModNum[3] == 2 ){
                    X3_2 = CleanTr_.at(InIter).CoordX[3];
                    hdXvsXst3_2 -> Fill(CleanTr_.at(InIter).CoordX[3],CleanTr_.at(InIter).CoordX[3] - Fit);
                    hdXXp3_mod2 -> Fill(CleanTr_.at(InIter).CoordX[st] - CleanTr_.at(InIter).CoordXp[st]);
                }
            }
        }//st
        if (X1_0 < 900. && X2_0 < 900.) hdXst1_0_st2_0->Fill(X1_0 - X2_0);
        if (X1_0 < 900. && X2_1 < 900.) hdXst1_0_st2_1->Fill(X1_0 - X2_1);
        if (X1_1 < 900. && X2_0 < 900.) hdXst1_1_st2_0->Fill(X1_1 - X2_0);
        if (X1_1 < 900. && X2_1 < 900.) hdXst1_1_st2_1->Fill(X1_1 - X2_1);
        if (X1_2 < 900. && X2_0 < 900.) hdXst1_2_st2_0->Fill(X1_2 - X2_0);
        if (X1_2 < 900. && X2_1 < 900.) hdXst1_2_st2_1->Fill(X1_2 - X2_1);
        if (X1_3 < 900. && X2_0 < 900.) hdXst1_3_st2_0->Fill(X1_3 - X2_0);
        if (X1_3 < 900. && X2_1 < 900.) hdXst1_3_st2_1->Fill(X1_3 - X2_1);
        if (X2_0 < 900. && X3_1 < 900.) hdXst2_0_st3_1->Fill(X2_0 - X3_1);
        if (X2_0 < 900. && X3_2 < 900.) hdXst2_0_st3_2->Fill(X2_0 - X3_2);
        if (X2_1 < 900. && X3_1 < 900.) hdXst2_1_st3_1->Fill(X2_1 - X3_1);
        if (X2_1 < 900. && X3_2 < 900.) hdXst2_1_st3_2->Fill(X2_1 - X3_2);

        if(CleanTr_.at(InIter).Nhits == 6 ){
            Double_t Ax = (CleanTr_.at(InIter).CoordX[1] -CleanTr_.at(InIter).CoordX[3])/(CleanTr_.at(InIter).CoordZ[1] -CleanTr_.at(InIter).CoordZ[3]);
            if (X2_0 < 900.) hX13_X2_m0->Fill( X2_0 - (Ax*(CleanTr_.at(InIter).CoordZ[2] - CleanTr_.at(InIter).CoordZ[1]) + CleanTr_.at(InIter).CoordX[1]) );
            if (X2_1 < 900.) hX13_X2_m1->Fill( X2_1 - (Ax*(CleanTr_.at(InIter).CoordZ[2] - CleanTr_.at(InIter).CoordZ[1]) + CleanTr_.at(InIter).CoordX[1]) );
        }
        hdX_st1_st2->Fill( CleanTr_.at(InIter).CoordX[1] -CleanTr_.at(InIter).CoordX[2] );
        hdX_st2_st3->Fill( CleanTr_.at(InIter).CoordX[2] -CleanTr_.at(InIter).CoordX[3] );
        hdX_st1_st3->Fill( CleanTr_.at(InIter).CoordX[1] -CleanTr_.at(InIter).CoordX[3] );

        //-------Xp--------
        cout<<" --Xp--"<<endl;
        Double_t Xp2_0= 999.,Xp2_1= 999.;
        for (Int_t st = 1; st < fNstations; ++st) {
            if ( CleanTr_.at(InIter).CoordXp[st] < -900.) continue;
            Double_t Hit = CleanTr_.at(InIter).CoordXp[st];
            Double_t Fit = ( CleanTr_.at(InIter).param[0] * CleanTr_.at(InIter).CoordZ[st] + CleanTr_.at(InIter).param[1] ) +
                    ( CleanTr_.at(InIter).param[2] * CleanTr_.at(InIter).CoordZ[st] + CleanTr_.at(InIter).param[3] ) * Angle(st,CleanTr_.at(InIter).ModNum[st]) * tg2_5 ;
            // x' = x + y*tg
            hdXp_st_1-> Fill(Hit - Fit);
            cout<<" st "<<st<<" Xphit "<<Hit<<" Xpfit "<< Fit<<" dXp "<<Hit - Fit<<" Z "<<CleanTr_.at(InIter).CoordZ[st]<< " strip "<<CleanTr_.at(InIter).StripNumXp[st]<<" Clsize "<<CleanTr_.at(InIter).ClSizeXp[st]<<" Q "<<CleanTr_.at(InIter).SumQXp[st]<<endl;
            if (st == 2 ){
                hdXp_st_2-> Fill(Hit - Fit);
                if (CleanTr_.at(InIter).ModNum[2] == 0 ){
                    Xp2_0 = CleanTr_.at(InIter).CoordXp[2];
                }
                if (CleanTr_.at(InIter).ModNum[2] == 1 ){
                    Xp2_1 = CleanTr_.at(InIter).CoordXp[2];
                }
            }
            if (st == 3 ){
                hdXp_st_3-> Fill(Hit - Fit);
                if ( CleanTr_.at(InIter).ModNum[3] == 1 ){
                    hdXp3_mod1 -> Fill(Hit - Fit);
                }
                if ( CleanTr_.at(InIter).ModNum[3] == 2 ){
                    hdXp3_mod2 -> Fill(Hit - Fit);
                }
            }
        }
        if(CleanTr_.at(InIter).Nhits == 6 ){
            Double_t Axp = (CleanTr_.at(InIter).CoordXp[1] -CleanTr_.at(InIter).CoordXp[3])/(CleanTr_.at(InIter).CoordZ[1] -CleanTr_.at(InIter).CoordZ[3]);
            if (Xp2_0 < 900.) hXp13_Xp2_m0->Fill( Xp2_0 - (Axp*(CleanTr_.at(InIter).CoordZ[2] - CleanTr_.at(InIter).CoordZ[1]) + CleanTr_.at(InIter).CoordXp[1]) );
            if (Xp2_1 < 900.) hXp13_Xp2_m1->Fill( Xp2_1 - (Axp*(CleanTr_.at(InIter).CoordZ[2] - CleanTr_.at(InIter).CoordZ[1]) + CleanTr_.at(InIter).CoordXp[1]) );
        }

        if (fDebug) cout<<" --Y--"<<endl;
        Double_t Y1_0 = 999., Y1_1= 999., Y1_2= 999., Y1_3= 999., Y2_0= 999.,Y2_1= 999.,Y3_1= 999.,Y3_2= 999.;
        //-------Y--------
        for (Int_t st = 1; st < fNstations; ++st) {
            if ( CleanTr_.at(InIter).CoordY[st] < -900. ) continue;
            Double_t Hit = CleanTr_.at(InIter).CoordY[st];
            Double_t Fit = CleanTr_.at(InIter).param[2] * CleanTr_.at(InIter).CoordZ[st] + CleanTr_.at(InIter).param[3];
            cout<<" st "<<st<<" Yhit "<<Hit<<" Yfit "<< Fit<<" dY "<<Hit - Fit<<" Z "<<CleanTr_.at(InIter).CoordZ[st]<<endl;

            if (st == 1 ) {hY_st_1 -> Fill(CleanTr_.at(InIter).CoordY[st]);hdYvsYst_1 -> Fill(Hit,Hit - Fit); hdY_st_1-> Fill(Hit - Fit);}
            if (st == 2 ) {hY_st_2 -> Fill(CleanTr_.at(InIter).CoordY[st]);hdYvsYst_2 -> Fill(Hit,Hit - Fit); hdY_st_2-> Fill(Hit - Fit);}
            if (st == 3 ) {hY_st_3 -> Fill(CleanTr_.at(InIter).CoordY[st]);hdYvsYst_3 -> Fill(CleanTr_.at(InIter).CoordY[3],CleanTr_.at(InIter).CoordY[3] - Fit);
                hdY_st_3-> Fill(CleanTr_.at(InIter).CoordY[3] - Fit);
                if (fabs(Hit - Fit) > 0.1) cout<<" > 0.1 "<<endl;
            }


            if (st == 1 ){
                if ( CleanTr_.at(InIter).ModNum[1] == 0 ){
                    hdYvsYst1_mod0 -> Fill(Hit,Hit - Fit);
                    Y1_0 = CleanTr_.at(InIter).CoordY[st];
                }
                if ( CleanTr_.at(InIter).ModNum[1] == 1 ){
                    hdYvsYst1_mod1 -> Fill(Hit,Hit - Fit);
                    Y1_1 = CleanTr_.at(InIter).CoordY[st];
                }
                if ( CleanTr_.at(InIter).ModNum[1] == 2 ){
                    hdYvsYst1_mod2 -> Fill(Hit,Hit - Fit);
                    Y1_2 = CleanTr_.at(InIter).CoordY[st];
                }
                if ( CleanTr_.at(InIter).ModNum[1] == 3 ){
                    hdYvsYst1_mod3 -> Fill(Hit,Hit - Fit);
                    Y1_3 = CleanTr_.at(InIter).CoordY[st];
                }
            }
            if (st == 2){
                if (CleanTr_.at(InIter).ModNum[2] == 0 ){
                    hdYvsYst2_mod0 -> Fill(Hit,Hit - Fit);
                    Y2_0 = CleanTr_.at(InIter).CoordY[st];
                }
                if (CleanTr_.at(InIter).ModNum[2] == 1 ){
                    hdYvsYst2_mod1 -> Fill(Hit,Hit - Fit);
                    Y2_1 = CleanTr_.at(InIter).CoordY[st];
                }

            }
            if (st == 3){
                if ( CleanTr_.at(InIter).ModNum[3] == 1 ){
                    hdYvsYst3_mod1 -> Fill(Hit,Hit - Fit);
                    Y3_1 = CleanTr_.at(InIter).CoordY[st];
                }
                if ( CleanTr_.at(InIter).ModNum[3] == 2 ){
                    hdYvsYst3_mod2 -> Fill(Hit,Hit - Fit);
                    Y3_2 = CleanTr_.at(InIter).CoordY[st];
                }
            }
        }//st

        double Ay_pr= -999.;double Y_pr = -999.;
        if (CleanTr_.at(InIter).CoordY[1] > -900. && CleanTr_.at(InIter).CoordY[3] > -900.){
            Ay_pr = (CleanTr_.at(InIter).CoordY[1] - CleanTr_.at(InIter).Yv)/(CleanTr_.at(InIter).CoordZ[1] - Z0_SRC_target);
            Y_pr  = Ay_pr* (CleanTr_.at(InIter).CoordZ[3] - CleanTr_.at(InIter).CoordZ[1]) + CleanTr_.at(InIter).CoordY[1];
        }
        if (CleanTr_.at(InIter).CoordY[2] > -900. && CleanTr_.at(InIter).CoordY[3] > -900.){
            Ay_pr = (CleanTr_.at(InIter).CoordY[2] - CleanTr_.at(InIter).Yv)/(CleanTr_.at(InIter).CoordZ[2] - Z0_SRC_target);
            Y_pr  = Ay_pr* (CleanTr_.at(InIter).CoordZ[3] - CleanTr_.at(InIter).CoordZ[2]) + CleanTr_.at(InIter).CoordY[2];
        }
        cout<<" Y3 predicted "<<Y_pr<<" Y "<<CleanTr_.at(InIter).CoordY[3]<<endl;

        hdY_st1_st2->Fill( CleanTr_.at(InIter).CoordY[1] -CleanTr_.at(InIter).CoordY[2] );
        hdY_st2_st3->Fill( CleanTr_.at(InIter).CoordY[2] -CleanTr_.at(InIter).CoordY[3] );
        hdY_st1_st3->Fill( CleanTr_.at(InIter).CoordY[1] -CleanTr_.at(InIter).CoordY[3] );


        if (Y1_0 < 900. && Y2_0 < 900.) hdYst1_0_st2_0->Fill(Y1_0 - Y2_0);
        if (Y1_0 < 900. && Y2_1 < 900.) hdYst1_0_st2_1->Fill(Y1_0 - Y2_1);
        if (Y1_1 < 900. && Y2_0 < 900.) hdYst1_1_st2_0->Fill(Y1_1 - Y2_0);
        if (Y1_1 < 900. && Y2_1 < 900.) hdYst1_1_st2_1->Fill(Y1_1 - Y2_1);
        if (Y1_2 < 900. && Y2_0 < 900.) hdYst1_2_st2_0->Fill(Y1_2 - Y2_0);
        if (Y1_2 < 900. && Y2_1 < 900.) hdYst1_2_st2_1->Fill(Y1_2 - Y2_1);
        if (Y1_3 < 900. && Y2_0 < 900.) hdYst1_3_st2_0->Fill(Y1_3 - Y2_0);
        if (Y1_3 < 900. && Y2_1 < 900.) hdYst1_3_st2_1->Fill(Y1_3 - Y2_1);
        if (Y2_0 < 900. && Y3_1 < 900.) hdYst2_0_st3_1->Fill(Y2_0 - Y3_1);
        if (Y2_0 < 900. && Y3_2 < 900.) hdYst2_0_st3_2->Fill(Y2_0 - Y3_2);
        if (Y2_1 < 900. && Y3_1 < 900.) hdYst2_1_st3_1->Fill(Y2_1 - Y3_1);
        if (Y2_1 < 900. && Y3_2 < 900.) hdYst2_1_st3_2->Fill(Y2_1 - Y3_2);

        if(CleanTr_.at(InIter).Nhits == 6 ){
            Double_t Ax = (CleanTr_.at(InIter).CoordY[1] -CleanTr_.at(InIter).CoordY[3])/(CleanTr_.at(InIter).CoordZ[1] -CleanTr_.at(InIter).CoordZ[3]);
            Double_t Ax23 = (CleanTr_.at(InIter).CoordY[2] -CleanTr_.at(InIter).CoordY[3])/(CleanTr_.at(InIter).CoordZ[2] -CleanTr_.at(InIter).CoordZ[3]);

            if (Y1_0 < 900.) hY1m0_Y23->Fill( Y1_0 - (Ax23*(CleanTr_.at(InIter).CoordZ[1] - CleanTr_.at(InIter).CoordZ[2]) + CleanTr_.at(InIter).CoordY[2]) );
            if (Y1_1 < 900.) hY1m1_Y23->Fill( Y1_1 - (Ax23*(CleanTr_.at(InIter).CoordZ[1] - CleanTr_.at(InIter).CoordZ[2]) + CleanTr_.at(InIter).CoordY[2]) );
            if (Y1_2 < 900.) hY1m2_Y23->Fill( Y1_2 - (Ax23*(CleanTr_.at(InIter).CoordZ[1] - CleanTr_.at(InIter).CoordZ[2]) + CleanTr_.at(InIter).CoordY[2]) );
            if (Y1_3 < 900.) hY1m3_Y23->Fill( Y1_3 - (Ax23*(CleanTr_.at(InIter).CoordZ[1] - CleanTr_.at(InIter).CoordZ[2]) + CleanTr_.at(InIter).CoordY[2]) );

            if (Y2_0 < 900.) hY13_Y2_m0->Fill( Y2_0 - (Ax*(CleanTr_.at(InIter).CoordZ[2] - CleanTr_.at(InIter).CoordZ[1]) + CleanTr_.at(InIter).CoordY[1]) );
            if (Y2_1 < 900.) hY13_Y2_m1->Fill( Y2_1 - (Ax*(CleanTr_.at(InIter).CoordZ[2] - CleanTr_.at(InIter).CoordZ[1]) + CleanTr_.at(InIter).CoordY[1]) );
        }

        double z_cnt = 0;
        if (CleanTr_.at(InIter).CoordX[1] > -900. || CleanTr_.at(InIter).CoordXp[1] > -900.)
            hprofile_beam_z1->Fill(CleanTr_.at(InIter).param[0] * (CleanTr_.at(InIter).CoordZ[1] - z_cnt) + CleanTr_.at(InIter).param[1],CleanTr_.at(InIter).param[2] * (CleanTr_.at(InIter).CoordZ[1]- z_cnt) + CleanTr_.at(InIter).param[3] );
        if (CleanTr_.at(InIter).CoordX[2] > -900. || CleanTr_.at(InIter).CoordXp[2] > -900.)
            hprofile_beam_z2->Fill(CleanTr_.at(InIter).param[0] * (CleanTr_.at(InIter).CoordZ[2] - z_cnt) + CleanTr_.at(InIter).param[1],CleanTr_.at(InIter).param[2] * (CleanTr_.at(InIter).CoordZ[2]- z_cnt) + CleanTr_.at(InIter).param[3] );
        hprofile_beam_z3->Fill(CleanTr_.at(InIter).param[0] * (CleanTr_.at(InIter).CoordZ[3] - z_cnt) + CleanTr_.at(InIter).param[1],CleanTr_.at(InIter).param[2] * (CleanTr_.at(InIter).CoordZ[3]- z_cnt) + CleanTr_.at(InIter).param[3] );


        for (Int_t st = 1; st < fNstations; ++st) {

            if (CleanTr_.at(InIter).Nhits ==  N_min_points) {
                if ( CleanTr_.at(InIter).CoordX[st]  < -900. ) D_eff->Fill(2*st-1);
                if ( CleanTr_.at(InIter).CoordXp[st] < -900. ) D_eff->Fill(2*st);
            }
            if (CleanTr_.at(InIter).Nhits >  N_min_points) {
                D_eff->Fill(2*st-1);
                D_eff->Fill(2*st);
                if ( CleanTr_.at(InIter).CoordX[st]  > -900. ) N_eff->Fill(2*st-1);
                if ( CleanTr_.at(InIter).CoordXp[st] > -900. ) N_eff->Fill(2*st);
            }
        }// st

    }//InIter

    if (CleanTr_.size() > 0) {
        ver_averx = sum_xv/CleanTr_.size();
        ver_avery = sum_yv/CleanTr_.size();
        hvertex_aver_XY->Fill(ver_averx, ver_avery);
    }
    cout<<"-----end find tracks ------  Ntr "<<  CleanTr_.size()<<endl;

}
//----------------------------------------------------------------------



//--------------------Tracking:Case2------------------------------------
void BmnSiliconTrackFinder::Case2( Int_t & Fi_St,Int_t **Nleftoversp_,Double_t ***leftoverXsp_, Double_t ***leftoverXpsp_, Double_t ***leftoverYsp_,
                                   Double_t ***leftoverXsigsp_, Double_t ***leftoverXpsigsp_, Double_t ***leftoverYsigsp_,
                                   Int_t  **NleftoverX_,  Int_t **NleftoverXp_,Double_t ***leftoverX_, Double_t ***leftoverXp_,Double_t ***leftoverXsig_, Double_t ***leftoverXpsig_,
                                   vector<tracksX> & vec_tracks_,
                                   Double_t ***XClust_, Double_t ***XClust_width_, Double_t ***sumQx_, Double_t ***XpClust_, Double_t ***XpClust_width_, Double_t ***sumQxp_,
                                   Double_t ***XspClust_, Double_t ***XspClust_width_, Double_t ***sumQxsp_, Double_t ***XpspClust_, Double_t ***XpspClust_width_, Double_t ***sumQxpsp_){

    if(fDebug) cout<<" case2:"<<Fi_St<<"time "<<endl;

    tracksX tmpTr;

    Double_t xv_candidate = 999., yv_candidate = 999., ax_candidate = 999., ay_candidate = 999.;
    Int_t    iClustXMod_candidate = -1, iModX_candidate = -1;
    Double_t dxV_thr = .2, dyV_thr = .4;
    Int_t    Npoint = 0;
    Int_t    TotalNumberOfHits = 0;
    Double_t ChiSquareNdf      = 0.;

    Bool_t   x5_exist  = 0;
    Bool_t   xp5_exist = 0;
    Int_t    Nmod_Fi   = fNmodules1;
    Int_t    Nmod_Sec  = fNmodules2;
    Int_t    Sec_St    = 2;

    if ( Fi_St == 2) {
        Nmod_Fi  = fNmodules2;
        Nmod_Sec = fNmodules1;
        Sec_St = 1;
    }

    for (Int_t imod1 = 0; imod1 < Nmod_Fi; imod1++) {
        for (Int_t cl1 = 0; cl1 < Nleftoversp_[Fi_St][imod1]; cl1++){

            Double_t ax_target_region = -999., bx_target_region= -999., ay_target_region = -999., by_target_region= -999.;
            Bool_t   was_candidate    = 0; ax_candidate = 999., ay_candidate = 999., xv_candidate = 999., yv_candidate = 999.;
            x5_exist  = 0;
            xp5_exist = 0;
            ChiSquareNdf  = 0.;
            Npoint = 0;

            for (Int_t imod3 = 0; imod3 < fNmodules3; imod3++) {
                for (Int_t cl3 = 0; cl3 < Nleftoversp_[3][imod3]; cl3++){

                    for ( Int_t i = 1; i < fNstations; ++i){
                        iClustXMod[i]= -1;// reset pointers
                        iModX[i]= -1;
                    }

                    ax_target_region = (leftoverXsp_[3][imod3][cl3] - leftoverXsp_[Fi_St][imod1][cl1] ) / (Zstation[3][imod3] - Zstation[Fi_St][imod1] );
                    bx_target_region = -ax_target_region * ( Zstation[3][imod3] - Z0_SRC_target ) + leftoverXsp_[3][imod3][cl3];

                    ay_target_region = (leftoverYsp_[3][imod3][cl3] - leftoverYsp_[Fi_St][imod1][cl1] ) / (Zstation[3][imod3] - Zstation[Fi_St][imod1] );
                    by_target_region = -ay_target_region * ( Zstation[3][imod3] - Z0_SRC_target ) + leftoverYsp_[3][imod3][cl3];

                    if (fDebug) cout<<" bx_target_region "<<bx_target_region<<" by_target_region "<<by_target_region<<endl;

                    if ( fabs(bx_target_region) > half_target_regionX) continue;
                    if ( fabs(by_target_region) > half_target_regionY) continue;

                    if ( was_candidate == 0 ) {

                        xv_candidate         = bx_target_region;
                        yv_candidate         = by_target_region;
                        ax_candidate         = ax_target_region;
                        ay_candidate         = ay_target_region;
                        iModX_candidate      = imod3;
                        iClustXMod_candidate = cl3;
                        was_candidate = 1;
                        // if (fDebug) cout<<" write candidate "<<endl;
                    } else{
                        if ( fabs(bx_target_region) > (fabs(xv_candidate) + dxV_thr) ) continue;//0.2
                        if ( fabs(by_target_region) > (fabs(yv_candidate) + dyV_thr) ) continue;//0.4
                        if ( (fabs(bx_target_region) + 0.5*fabs(by_target_region) ) > ( fabs(xv_candidate) + 0.25*fabs(yv_candidate) ) ) continue;// v2 closed to v1

                        xv_candidate         = bx_target_region;
                        yv_candidate         = by_target_region;
                        ax_candidate         = ax_target_region;
                        ay_candidate         = ay_target_region;

                        iModX_candidate      = imod3;
                        iClustXMod_candidate = cl3;
                        // if (fDebug) cout<<" re - write.  xv_candidate "<<xv_candidate<<endl;
                    }

                    Npoint = 4;

                }//cl3
            }//st3


            if (fDebug) cout<<" aft.st3 cycle:  xv_candidate "<<xv_candidate<<"  by_cand "<<yv_candidate<<" was_candidate "<<was_candidate<<endl;
            if (xv_candidate > 900.) continue;

            for (Int_t ist = 1; ist < fNstations; ++ist) {
                for (Int_t imod = 0; imod < fNmodules; imod++) {
                    Xforglfit[ist][imod]      = -999.;
                    Yforglfit[ist][imod]      = -999.;
                    Xpforglfit[ist][imod]     = -999.;
                    SigmYforglfit[ist][imod]  = -999.;
                    SigmXforglfit[ist][imod]  = -999.;
                    SigmXpforglfit[ist][imod] = -999.;
                }
            }

            if ( iModX_candidate > -1){
                iModX[Fi_St] = imod1;
                iModX[3] = iModX_candidate;
                iClustXMod[Fi_St] = cl1;
                iClustXMod[3] = iClustXMod_candidate;
            }

            for (Int_t ist = 1; ist < fNstations; ++ist) {
                if ( ist == Sec_St ) continue;
                Xforglfit[ist][iModX[ist]]      = leftoverXsp_[ist][iModX[ist]][iClustXMod[ist]];
                SigmXforglfit[ist][iModX[ist]]  = leftoverXsigsp_[ist][iModX[ist]][iClustXMod[ist]];
                Xpforglfit[ist][iModX[ist]]     = leftoverXpsp_[ist][iModX[ist]][iClustXMod[ist]];
                SigmXpforglfit[ist][iModX[ist]] = leftoverXpsigsp_[ist][iModX[ist]][iClustXMod[ist]];
                Yforglfit[ist][iModX[ist]]      = leftoverYsp_[ist][iModX[ist]][iClustXMod[ist]];
                SigmYforglfit[ist][iModX[ist]]  = leftoverYsigsp_[ist][iModX[ist]][iClustXMod[ist]];
            }
            //--search iside 2nd station--
            //--search 5p in X line--
            if ( fDebug) cout<< " try to search 5th p.  Sec_St "<<Sec_St<<" Nmod_Sec "<<Nmod_Sec<<endl;
            for (Int_t imod2 = 0; imod2 < Nmod_Sec; imod2++) {

                // if ( fDebug) cout<< "     imod2 "<<imod2  <<endl;
                if ( x5_exist ) { Npoint = 5; break;}

                for (Int_t clx2 = 0; clx2 <  NleftoverX_[Sec_St][imod2]; clx2++){
                    // if ( fDebug) cout<< "     clx2 "<<clx2  <<endl;

                    iModX[Sec_St]                         = -1;
                    iClustXMod[Sec_St]                    = -1;
                    Xforglfit[Sec_St][iModX[Sec_St]]      = -999.;
                    SigmXforglfit[Sec_St][iModX[Sec_St]]  = -999.;
                    Xpforglfit[Sec_St][iModX[Sec_St]]     = -999.;
                    SigmXpforglfit[Sec_St][iModX[Sec_St]] = -999.;

                    if ( fDebug) cout<< "search 5p in X. tmp x1  "<<leftoverXsp_[Fi_St][imod1][cl1] <<" x2 "<<leftoverX_[Sec_St][imod2][clx2]<<" hrd "<<half_roadX1_X2<<endl;
                    if ( fabs(leftoverXsp_[Fi_St][imod1][cl1]  -  leftoverX_[Sec_St][imod2][clx2]) > half_roadX1_X2 ) continue;
                    if ( fDebug) cout<< "found 5th p in X"<<Sec_St<<"  "<<leftoverX_[Sec_St][imod2][clx2]<<endl;

                    Npoint = 5;
                    // -- special case for neibour points --
                    if ( clx2 < (NleftoverX_[Sec_St][imod2] -1) ) {
                        if ( fabs(leftoverXsp_[Fi_St][imod1][cl1]-leftoverX_[Sec_St][imod2][clx2])  >  fabs(leftoverXsp_[Fi_St][imod1][cl1]-leftoverX_[Sec_St][imod2][clx2+1]) )
                            continue;
                    }

                    iModX[Sec_St]                         = imod2;
                    iClustXMod[Sec_St]                    = clx2;
                    Xforglfit[Sec_St][iModX[Sec_St]]      = leftoverX_[Sec_St][iModX[Sec_St]][clx2];
                    SigmXforglfit[Sec_St][iModX[Sec_St]]  = leftoverXsig_[Sec_St][iModX[Sec_St]][clx2];
                    Xpforglfit[Sec_St][iModX[Sec_St]]     = -999.;
                    SigmXpforglfit[Sec_St][iModX[Sec_St]] = -999.;

                    //---------------------------	GlobalFit---------------------------------
                    TotalNumberOfHits = 0;
                    ChiSquareNdf      = 0.;
                    ChiSquare         = 0.;

                    for (Int_t coeff = 0; coeff < 4; ++coeff) {
                        line[coeff] = 0.;
                    }



                    calculationOfChi2(Xforglfit, Xpforglfit, SigmXforglfit, SigmXpforglfit, iClustXMod, iModX, ChiSquare, line);

                    TotalNumberOfHits = Npoint;
                    if ( Npoint > 4 )
                        ChiSquareNdf = ChiSquare/(TotalNumberOfHits - 4);
                    else ChiSquareNdf = ChiSquare;

                    if ( fDebug) cout<<" before cut ChiSquareNDF "<<ChiSquareNdf<<" TotalNumberOfHits  "<<TotalNumberOfHits <<endl;
                    if (fDebug)  cout<<endl;

                    if ( ChiSquareNdf > Chi2_Globcut ) {
                        Npoint = 4;
                        iModX[Sec_St]                         = -1;
                        iClustXMod[Sec_St]                    = -1;
                        Xforglfit[Sec_St][iModX[Sec_St]]      = -999.;
                        SigmXforglfit[Sec_St][iModX[Sec_St]]  = -999.;
                        Xpforglfit[Sec_St][imod2]             = -999.;
                        SigmXpforglfit[Sec_St][imod2]         = -999.;
                        x5_exist = 0;
                        continue;
                    }

                    x5_exist = 1;
                    if (fDebug) cout<<" case2: x5_exist "<<endl;
                    if (fDebug) cout<<" X "<<Xforglfit[Sec_St][iModX[Sec_St]]<<" strip "<<XClust_[Sec_St][iModX[Sec_St]][iClustXMod[Sec_St]]<<" Q "<<sumQx_[Sec_St][iModX[Sec_St]][iClustXMod[Sec_St]]<<endl;

                    break;   //   goto next iMod of 2-nd st
                }//clx2

                // --try to search 5th xp--
                if ( x5_exist ) continue;

                xp5_exist = 0;
                for (Int_t clxp2 = 0; clxp2 <  NleftoverXp_[Sec_St][imod2]; clxp2++){
                    if ( xp5_exist ) { Npoint = 5; break;}

                    iModX[Sec_St]                         = -1;
                    iClustXMod[Sec_St]                    = -1;
                    Xforglfit[Sec_St][iModX[Sec_St]]      = -999.;
                    SigmXforglfit[Sec_St][iModX[Sec_St]]  = -999.;
                    Xpforglfit[Sec_St][imod2]             = -999.;
                    SigmXpforglfit[Sec_St][imod2]         = -999.;

                    Int_t st1 = Fi_St;  Int_t st2 = Sec_St;
                    Int_t sign1 = Angle(st1,imod1);
                    Int_t sign2 = Angle(st2,imod2);
                    if(fDebug) cout<<" sign1 "<<sign1<<" sign2 "<<sign2<<endl;

                    if ( (sign1 == sign2) && ( fabs(leftoverXpsp_[Fi_St][imod1][cl1]  -  leftoverXp_[Sec_St][imod2][clxp2] )  > half_roadXp1_Xp2) )           continue;
                    if ( (sign1 != sign2) && ( fabs(leftoverXpsp_[Fi_St][imod1][cl1]  -  leftoverXp_[Sec_St][imod2][clxp2] )  > half_roadXp1_Xp2_diff_sign) ) continue;
                    if ( fDebug) cout<<" --Xp1 "<<leftoverXpsp_[Fi_St][imod1][cl1]<<" Xp2 "<<leftoverXp_[Sec_St][imod2][clxp2]<<" dXp "<<leftoverXpsp_[Fi_St][imod1][cl1]  -  leftoverXp_[Sec_St][imod2][clxp2]<<endl;

                    Npoint = 5;

                    iModX[Sec_St]                         = imod2;
                    iClustXMod[Sec_St]                    = clxp2;
                    Xforglfit[Sec_St][iModX[Sec_St]]      = -999.;
                    SigmXforglfit[Sec_St][iModX[Sec_St]]  = -999.;
                    Xpforglfit[Sec_St][imod2]             = leftoverXp_[Sec_St][iModX[Sec_St]][clxp2];
                    SigmXpforglfit[Sec_St][imod2]         = leftoverXpsig_[Sec_St][iModX[Sec_St]][clxp2];

                    //---------------------------GlobalFit---------------------------------
                    TotalNumberOfHits = 0;
                    ChiSquareNdf      = 0.;
                    ChiSquare         = 0.;

                    for (Int_t coeff = 0; coeff < 4; ++coeff) {
                        line[coeff] = 0.;
                    }

                    calculationOfChi2(Xforglfit, Xpforglfit, SigmXforglfit, SigmXpforglfit, iClustXMod, iModX, ChiSquare, line);

                    TotalNumberOfHits = Npoint;
                    if ( Npoint > 4 )
                        ChiSquareNdf = ChiSquare/(TotalNumberOfHits - 4);
                    else ChiSquareNdf = ChiSquare;

                    if ( fDebug) cout<<" before cut ChiSquareNDF "<<ChiSquareNdf<<" TotalNumberOfHits  "<<TotalNumberOfHits <<endl;
                    if (fDebug) cout<<endl;

                    if ( ChiSquareNdf > Chi2_Globcut ){
                        Npoint = 4;
                        iModX[Sec_St]                         = -1;
                        iClustXMod[Sec_St]                    = -1;
                        Xforglfit[Sec_St][iModX[Sec_St]]      = -999.;
                        SigmXforglfit[Sec_St][iModX[Sec_St]]  = -999.;
                        Xpforglfit[Sec_St][imod2]             = -999.;
                        SigmXpforglfit[Sec_St][imod2]         = -999.;
                        xp5_exist = 0;
                        continue;
                    }

                    xp5_exist = 1;
                    if (fDebug) cout<<" case2: xp5_exist "<<xp5_exist<<endl;
                    if (fDebug) cout<<" Xp "<<Xpforglfit[Sec_St][iModX[Sec_St]]<<" strip "<<XpClust_[Sec_St][iModX[Sec_St]][iClustXMod[Sec_St]]<<" Q "<<sumQxp_[Sec_St][iModX[Sec_St]][iClustXMod[Sec_St]]<<endl;

                    break;
                }//clxp2
                if ( xp5_exist ) break;
            }//imod2
            //try find 5th xp.

            if (fDebug) cout<<" case2: end search ------------"<<endl;
            if (fDebug) cout<<" Npoint "<<Npoint<<" ChiSquareNdf "<<ChiSquareNdf<<" x5_exist "<<x5_exist<<" xp5_exist "<<xp5_exist<<endl;

            TotalNumberOfHits = Npoint;
            Int_t  Mod_cand[fNstations];
            Double_t X_cand[fNstations], Xp_cand[fNstations], Y_cand[fNstations],
                    SigX_cand[fNstations], SigXp_cand[fNstations], SigY_cand[fNstations],
                    Chi2_cand = 999., line_cand[4];

            for (Int_t coeff = 0; coeff < 4; ++coeff) {
                line_cand[coeff] = 0.;
            }

            for (Int_t st = 1; st < fNstations; ++st) {
                Mod_cand[st]   = -1;
                X_cand[st]     = -999.;
                Xp_cand[st]    = -999.;
                Y_cand[st]     = -999.;
                SigX_cand[st]  = -999.;
                SigXp_cand[st] = -999.;
                SigY_cand[st]  = -999.;
            }

            if (TotalNumberOfHits > 4 && ChiSquareNdf < Chi2_cand && ChiSquareNdf < Chi2_Globcut ){

                Chi2_cand = ChiSquareNdf;
                line_cand[0] = line[0];
                line_cand[1] = line[1];
                line_cand[2] = line[2];
                line_cand[3] = line[3];

                if (fDebug) cout<<" case2 5p cand  Chi2Ndf "<<Chi2_cand<<endl;

                for (Int_t st = 1; st < fNstations; ++st) {
                    if (iModX[st] < 0) continue;

                    Mod_cand[st]   = iModX[st];
                    X_cand[st]     = Xforglfit[st][iModX[st]];
                    Y_cand[st]     = Yforglfit[st][iModX[st]];
                    Xp_cand[st]    = Xpforglfit[st][iModX[st]];
                    SigX_cand[st]  = SigmXforglfit[st][iModX[st]];
                    SigY_cand[st]  = SigmYforglfit[st][iModX[st]];
                    SigXp_cand[st] = SigmXpforglfit[st][iModX[st]];

                    if (fDebug) cout<<" X_cand["<<st<<"] "<< X_cand[st] <<" Xp_cand["<<st<<"] "<<Xp_cand[st] <<" iModX "<<iModX[st]<<endl;

                }
            }//5 points

            if ( TotalNumberOfHits == 4) {
                if (fDebug) cout<<"TotalNumberOfHits == 4"<<endl;

                Chi2_cand  = 0.;

                line_cand[0] = ax_candidate;
                line_cand[1] = xv_candidate + ax_candidate * (  Zcentr - Z0_SRC_target ); //fix me!
                line_cand[2] = ay_candidate;
                line_cand[3] = yv_candidate + ay_candidate * (  Zcentr - Z0_SRC_target );

                if (fDebug) cout<<" case2 4p cand  Xv "<<xv_candidate<<" Yv "<<yv_candidate<<endl;
                if (fDebug) cout<<" ax "<<line_cand[0]<<" bx "<<line_cand[1]<<" ay "<<line_cand[2]<<" by "<<line_cand[3]<<endl;

                for (Int_t st = 1; st < fNstations; ++st) {
                    if (iModX[st] < 0) continue;

                    Mod_cand[st]   = iModX[st];
                    X_cand[st]     = Xforglfit[st][iModX[st]];
                    Y_cand[st]     = Yforglfit[st][iModX[st]];
                    Xp_cand[st]    = Xpforglfit[st][iModX[st]];
                    SigX_cand[st]  = SigmXforglfit[st][iModX[st]];
                    SigY_cand[st]  = SigmYforglfit[st][iModX[st]];
                    SigXp_cand[st] = SigmXpforglfit[st][iModX[st]];

                    if (fDebug) cout<<" X_cand["<<st<<"] "<< X_cand[st] <<" Xp_cand["<<st<<"] "<<Xp_cand[st] <<" iModX "<<iModX[st]<<endl;

                }//st
            }//4points

            // --write best 4p or 5p tracks--
            if (TotalNumberOfHits >=  4 && Chi2_cand < Chi2_Globcut ){

                if (fDebug) {
                    cout<<"case2: before writing:  "<<endl;
                    for (Int_t st = 1; st < fNstations; ++st) {
                        cout<<"    st "<<st<<"     X_cand  "<< X_cand[st] <<"     Xp_cand  "<< Xp_cand[st] <<endl;
                    }
                }

                if ( fabs(line_cand[2] * ( Z0_SRC_target - Zcentr  ) + line_cand[3]) > half_target_regionY){
                    if (fDebug) cout<<" case2:--------half_target_regionY cut: "<<half_target_regionY<<" Yv "<<fabs(line_cand[2] * ( Z0_SRC_target - Zcentr ) + line_cand[3])<<endl;
                    ChiSquareNdf = 999.;
                    continue;
                }


                for(int itr = 0; itr< vec_tracks_.size(); ++itr){
                    //deleting randomly recorded tracks
                    if (fDebug) {
                        for (Int_t st = 1; st < fNstations; ++st) {
                            cout<<" itr "<<itr<<" st "<<st<<"    X_tr  "<<vec_tracks_.at(itr).CoordX[st]<<"    Xp_tr  "<<vec_tracks_.at(itr).CoordXp[st]<<endl;
                        }
                    }
                    if (
                            ( (vec_tracks_.at(itr).CoordX[1] ==  X_cand[1] && X_cand[1] > -900.)   ||  (vec_tracks_.at(itr).CoordXp[1] == Xp_cand[1] && Xp_cand[1] > -900.) )
                            ||
                            ( (vec_tracks_.at(itr).CoordX[2] ==  X_cand[2] && X_cand[2] > -900.)   ||  (vec_tracks_.at(itr).CoordXp[2] == Xp_cand[2] && Xp_cand[2] > -900.) )
                            ||
                            ( (vec_tracks_.at(itr).CoordX[3] ==  X_cand[3] && X_cand[3] > -900.)   ||  (vec_tracks_.at(itr).CoordXp[3] == Xp_cand[3] && Xp_cand[3] > -900.) )

                            ){
                        if(fDebug ) cout<<" TotalNumberOfHits "<<TotalNumberOfHits<<" Nhits["<<itr<<"] "<<vec_tracks_.at(itr).Nhits<<endl;
                        if ( vec_tracks_.at(itr).Nhits <  TotalNumberOfHits) { vec_tracks_.erase(vec_tracks_.begin()+ itr);  continue;}
                        if ( vec_tracks_.at(itr).Nhits >  TotalNumberOfHits) {
                            if(fDebug ) cout<<" Nhits >  TotalNumberOfHits . Reject cand  "<<endl;
                            Chi2_cand = 999.; continue;}

                        if ( vec_tracks_.at(itr).Nhits ==  TotalNumberOfHits && TotalNumberOfHits == 4 ){
                            if(fDebug ) cout<<" Nhits "<<vec_tracks_.at(itr).Nhits <<" X1 "<< vec_tracks_.at(itr).CoordX[1]<<" X2 "<<vec_tracks_.at(itr).CoordX[2] <<endl;

                            if ( (fabs(vec_tracks_.at(itr).Xv) + 0.25*fabs(vec_tracks_.at(itr).Yv) ) > ( fabs(xv_candidate) + 0.25*fabs(yv_candidate) ) ){ // v2 closed to v1
                                vec_tracks_.erase(vec_tracks_.begin()+ itr);
                                continue;
                            }
                            else {
                                if(fDebug ) cout<<" Nhits == TotalNumberOfHits == 4 . Reject cand  "<<endl;
                                Chi2_cand = 999.;
                                continue;
                            }
                        }

                        if ( vec_tracks_.at(itr).Nhits ==  TotalNumberOfHits && TotalNumberOfHits == 5 ){
                            if ( vec_tracks_.at(itr).Chi2 > Chi2_cand )  {vec_tracks_.erase(vec_tracks_.begin()+ itr);  continue;}
                            else {
                                if(fDebug ) cout<<" Nhits == TotalNumberOfHits == 5 . Reject cand ??? "<<endl;
                                Chi2_cand = 999.;
                                continue;
                            }
                        }
                        if ( vec_tracks_.at(itr).Nhits > TotalNumberOfHits){
                            if(fDebug ) cout<<" Nhits=5 > TotalNumberOfHits=4 . Reject cand ??? "<<endl;
                            Chi2_cand = 999.;
                        }
                    }//if
                }//itr
                
                if(fDebug ) cout<<" Chi2_cand "<<Chi2_cand <<endl;
                if ( Chi2_cand > Chi2_Globcut) continue;

                tmpTr.Chi2     = Chi2_cand ;
                tmpTr.Nhits    = TotalNumberOfHits;
                tmpTr.Xv       = xv_candidate;
                tmpTr.Yv       = yv_candidate;
                tmpTr.param[0] = line_cand[0];
                tmpTr.param[1] = line_cand[1];
                tmpTr.param[2] = line_cand[2];
                tmpTr.param[3] = line_cand[3];

                if (fDebug) cout<<" Xv_candidate  "<<xv_candidate<<" Yv_candidate "<<yv_candidate<<" ChiNdf "<<Chi2_cand<<endl;
                for (Int_t st = 1; st < fNstations; ++st) {
                    if (Mod_cand[st] < 0) continue;

                    tmpTr.ModNum[st]  = iModX[st];
                    tmpTr.CoordX[st]  = X_cand[st];
                    tmpTr.CoordXp[st] = Xp_cand[st];
                    tmpTr.SigmaX[st]  = SigX_cand[st];
                    tmpTr.SigmaXp[st] = SigXp_cand[st];
                    tmpTr.CoordZ[st]  = ZstnCforglfit[st][Mod_cand[st]];
                    tmpTr.CoordY[st]  = Y_cand[st];
                    tmpTr.SigmaY[st]  = SigY_cand[st];

                    if ( st == Sec_St){
                        tmpTr.StripNumX[st] = XClust_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.StripNumXp[st]= XpClust_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.ClSizeX[st]   = XClust_width_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.ClSizeXp[st]  = XpClust_width_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.SumQX[st]     = sumQx_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.SumQXp[st]    = sumQxp_[st][iModX[st]][iClustXMod[st]];

                    }else{
                        tmpTr.StripNumX[st] = XspClust_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.StripNumXp[st]= XpspClust_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.ClSizeX[st]   = XspClust_width_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.ClSizeXp[st]  = XpspClust_width_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.SumQX[st]     = sumQxsp_[st][iModX[st]][iClustXMod[st]];
                        tmpTr.SumQXp[st]    = sumQxpsp_[st][iModX[st]][iClustXMod[st]];
                    }
                    if (fDebug) cout<<" st "<<st<<" Z "<<tmpTr.CoordZ[st]<<endl;
                    if (fDebug) cout<<" tmpTr.CoordX["<<st <<"]  "<<tmpTr.CoordX[st]<<" strip "<<tmpTr.StripNumX[st]<<" Clsize "<<tmpTr.ClSizeX[st]<<" Q "<<tmpTr.SumQX[st]<<endl;
                    if (fDebug) cout<<" tmpTr.CoordXp["<<st <<"] "<<tmpTr.CoordXp[st]<<" strip "<<tmpTr.StripNumXp[st]<<" Clsize "<<tmpTr.ClSizeXp[st]<<" Q "<<tmpTr.SumQXp[st]<<endl;
                }//st

                vec_tracks_.push_back(tmpTr);
                if (fDebug) cout<<" tmpTr.Xv "<<tmpTr.Xv<<" tmpTr.Yv "<<tmpTr.Yv<<endl;
            }//chi2

        }//cl1
    }//st1

}
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void BmnSiliconTrackFinder::Case3( Int_t **Nleftoversp_,Double_t ***leftoverXsp_, Double_t ***leftoverXpsp_, Double_t ***leftoverYsp_,Double_t ***leftoverXsigsp_, Double_t ***leftoverXpsigsp_, Double_t ***leftoverYsigsp_,
                                   Int_t  **NleftoverX_,  Int_t **NleftoverXp_,Double_t ***leftoverX_, Double_t ***leftoverXp_,Double_t ***leftoverXsig_, Double_t ***leftoverXpsig_,
                                   vector<tracksX> & vec_tracks_,
                                   Double_t ***XClust_, Double_t ***XClust_width_, Double_t ***sumQx_, Double_t ***XpClust_, Double_t ***XpClust_width_, Double_t ***sumQxp_,
                                   Double_t ***XspClust_, Double_t ***XspClust_width_, Double_t ***sumQxsp_, Double_t ***XpspClust_, Double_t ***XpspClust_width_, Double_t ***sumQxpsp_){
    tracksX tmpTr;

    if(fDebug) cout<<" case3 "<<endl;

    Bool_t   x5_exist = 0, xp5_exist = 0;
    Int_t    Npoint = 0;
    Double_t TotalNumberOfHits = 0;
    Double_t ChiSquareNdf      = 999.;
    Double_t hrd_case3 = 0.7;//0.5;

    for (Int_t imod1 = 0; imod1 < fNmodules1; imod1++) {
        for (Int_t cl1 = 0; cl1 < Nleftoversp_[1][imod1]; cl1++){

            for (Int_t imod2 = 0; imod2 < fNmodules2; imod2++) {
                for (Int_t cl2 = 0; cl2 < Nleftoversp_[2][imod2]; cl2++){

                    Bool_t   was_candidate     = 0;
                    Int_t    iClustXMod_candidate = -1, iModX_candidate = -1;
                    Double_t ax12_from_target = 999., x_ex3 = 999., ay12_from_target = 999., y_ex3 = 999.;
                    Double_t y1_2_cand = 999,     ay1_2_cand = 999;
                    Double_t bx_candidate = 999., by_candidate = 999.;

                    for ( Int_t i = 1; i < fNstations; ++i){
                        iClustXMod[i]= -1;// reset pointers
                        iModX[i]     = -1;
                    }

                    if ( fabs(leftoverXsp_[1][imod1][cl1]  - leftoverXsp_[2][imod2][cl2] )   > half_roadX1_X2  ) continue;
                    if ( fabs(leftoverXsp_[1][imod1][cl1]  - leftoverXpsp_[1][imod1][cl1])   > half_roadX1_Xp1 ) continue;
                    if ( fabs(leftoverXsp_[2][imod2][cl2]  - leftoverXpsp_[2][imod2][cl2])   > half_roadX2_Xp2 ) continue;
                    if ( fabs(leftoverYsp_[1][imod1][cl1]  - leftoverYsp_[2][imod2][cl2] )   > half_roadY1_Y2)   continue;

                    iClustXMod[1]= cl1;
                    iClustXMod[2]= cl2;
                    iModX[1]     = imod1;
                    iModX[2]     = imod2;

                    if ( fDebug) cout<<"case3: iModX[1] "<<iModX[1]<<" iCl "<<iClustXMod[1]<<" iModX[2] "<<iModX[2]<<" iCl "<<iClustXMod[2]<<endl;

                    Double_t  z1_2 = 0.5 * ( Zstation[1][imod1] + Zstation[2][imod2] );
                    Double_t  y1_2 = 0.5 * ( leftoverYsp_[1][imod1][cl1] +leftoverYsp_[2][imod2][cl2] );
                    ax12_from_target = (Xv_av - leftoverXsp_[1][imod1][cl1] )/( Z0_SRC_target - Zstation[1][imod1] );
                    ay12_from_target = (Yv_av - y1_2 )/( Z0_SRC_target - z1_2 );

                    if ( fDebug) cout<<" ax12_from_target "<<ax12_from_target<<" Xv_av "<<Xv_av<<" Yv_av "<<Yv_av<<endl;

                    for (Int_t ist = 1; ist < fNstations; ++ist) {
                        for (Int_t imod = 0; imod < fNmodules; imod++) {
                            Xforglfit[ist][imod]      = -999.;
                            Yforglfit[ist][imod]      = -999.;
                            Xpforglfit[ist][imod]     = -999.;
                            SigmYforglfit[ist][imod]  = -999.;
                            SigmXforglfit[ist][imod]  = -999.;
                            SigmXpforglfit[ist][imod] = -999.;

                        }
                    }

                    for (Int_t ist = 1; ist < fNstations; ++ist) {
                        if ( ist > 2 ) continue;
                        Xforglfit[ist][iModX[ist]]      = leftoverXsp_[ist][iModX[ist]][iClustXMod[ist]];
                        SigmXforglfit[ist][iModX[ist]]  = leftoverXsigsp_[ist][iModX[ist]][iClustXMod[ist]];
                        Xpforglfit[ist][iModX[ist]]     = leftoverXpsp_[ist][iModX[ist]][iClustXMod[ist]];
                        SigmXpforglfit[ist][iModX[ist]] = leftoverXpsigsp_[ist][iModX[ist]][iClustXMod[ist]];
                        Yforglfit[ist][iModX[ist]]      = leftoverYsp_[ist][iModX[ist]][iClustXMod[ist]];
                        SigmYforglfit[ist][iModX[ist]]  = leftoverYsigsp_[ist][iModX[ist]][iClustXMod[ist]];
                    }
                    // --start look 3st---
                    for (Int_t imod3 = 0; imod3 < fNmodules3; imod3++) {
                        for (Int_t clx3 = 0; clx3 <  NleftoverX_[3][imod3]; clx3++){

                            x_ex3 = ax12_from_target*(Zstation[3][imod3] - Z0_SRC_target ) + Xv_av;
                            y_ex3 = ay12_from_target*(Zstation[3][imod3] - Z0_SRC_target ) + Yv_av;

                            if ( fDebug) cout<<" X1 "<<leftoverXsp_[1][imod1][cl1]<<" X2 "<<leftoverXsp_[2][imod2][cl2]<<" x_ex3 "<<x_ex3<<endl;
                            if ( fDebug) cout<<" Y1 "<<leftoverYsp_[1][imod1][cl1]<<" Y2 "<<leftoverYsp_[2][imod2][cl2]<<" y_ex3 "<<y_ex3<<endl;

                            if ( fabs(x_ex3 - leftoverX_[3][imod3][clx3]) > hrd_case3 ) continue;

                            if ( !was_candidate ) {

                                iModX_candidate      = imod3;
                                iClustXMod_candidate = clx3;
                                was_candidate = 1;

                                y1_2_cand = y1_2;
                                ay1_2_cand = ay12_from_target;
                                if (fDebug) cout<<" write candidate3 x3 = "<<leftoverX_[3][imod3][clx3]<<endl;
                            } else{
                                if ( (x_ex3 - leftoverX_[3][imod3][clx3]) < (x_ex3 - leftoverX_[3][iModX_candidate][iClustXMod_candidate]) ) {
                                    iModX_candidate      = imod3;
                                    iClustXMod_candidate = clx3;
                                    y1_2_cand = y1_2;
                                    ay1_2_cand = ay12_from_target;
                                    if (fDebug) cout<<" re-write cand3 x3 = "<<leftoverX_[3][imod3][clx3]<<endl;
                                }
                            }//else

                            if ( was_candidate ) Npoint = 5;

                        }//clx3
                    }//st3

                    if ( was_candidate ){
                        iModX[3]                             = iModX_candidate;
                        iClustXMod[3]                        = iClustXMod_candidate;
                        Xforglfit[3][iModX_candidate]        = leftoverX_[3][iModX_candidate][iClustXMod_candidate];
                        SigmXforglfit[3][iModX_candidate]    = leftoverXsig[3][iModX_candidate][iClustXMod_candidate];
                        Xpforglfit[3][iModX_candidate]       = -999.;
                        SigmXpforglfit[3][iModX_candidate]   = -999.;
                        Yforglfit[3][iModX_candidate]        = -999.;
                        SigmYforglfit[3][iModX_candidate]    = -999.;

                        //---------------------------	GlobalFit 5p --------------------------------

                        for (Int_t coeff = 0; coeff < 4; ++coeff) {
                            line[coeff] = 0.;
                        }

                        calculationOfChi2(Xforglfit, Xpforglfit, SigmXforglfit, SigmXpforglfit, iClustXMod, iModX, ChiSquare, line);

                        TotalNumberOfHits = Npoint;
                        if ( Npoint > 4 )
                            ChiSquareNdf = ChiSquare/(TotalNumberOfHits - 4);
                        else ChiSquareNdf = ChiSquare;

                        if ( fDebug) cout<<" case3 x: before cut ChiSquareNDF "<<ChiSquareNdf<<" TotalNumberOfHits  "<<TotalNumberOfHits
                                        <<" fitted line par "<<line[0]<<" "<<line[1]<<" "<<line[2]<<" "<<line[3]<<endl;
                        if (fDebug) cout<<endl;

                    }//if ( was_candidate )
                    if ( ChiSquareNdf < Chi2_Globcut ){

                        x5_exist = 1;
                        line[2] = ay1_2_cand;
                        line[3] = Yv_av - line[2] * ( Z0_SRC_target - Zcentr );
                        if ( fDebug) cout<<"              replace to Yv-Y12 line par "<<line[2]<<" "<<line[3]<<endl;

                    }else {
                        Npoint                      =  0;
                        iModX[3]                    = -1;
                        iClustXMod[3]               = -1;
                        Xforglfit[3][iModX[3]]      = -999.;
                        SigmXforglfit[3][iModX[3]]  = -999.;
                        Xpforglfit[3][iModX[3]]     = -999.;
                        SigmXpforglfit[3][iModX[3]] = -999.;
                        iClustXMod_candidate        = -1;
                        x5_exist = 0;
                    }
                    if ( fDebug) cout<<"---x.----"<<endl;
                    if ( fDebug) cout<<"case3: end 3x st: iModX[1] "<<iModX[1]<<" iCl "<<iClustXMod[1]<<" iModX[2] "<<iModX[2]<<" iCl "<<iClustXMod[2]<<endl;
                    // --try to search 5th xp--
                    //---xp----
                    xp5_exist = 0;
                    if ( !x5_exist){

                        was_candidate = 0;
                        for (Int_t imod3 = 0; imod3 < fNmodules3; imod3++) {
                            for (Int_t clxp3 = 0; clxp3 <  NleftoverXp_[3][imod3]; clxp3++){
                                was_candidate = 0;
                                iModX[3]                    = -1;
                                iClustXMod[3]               = -1;
                                Xforglfit[3][iModX[3]]      = -999.;
                                SigmXforglfit[3][iModX[3]]  = -999.;
                                Xpforglfit[3][iModX[3]]     = -999.;
                                SigmXpforglfit[3][iModX[3]] = -999.;

                                x_ex3 = ax12_from_target*(Zstation[3][imod3] - Z0_SRC_target ) + Xv_av;
                                y_ex3 = ay12_from_target*(Zstation[3][imod3] - Z0_SRC_target ) + Yv_av;

                                if ( fDebug) cout<<" X1 "<<leftoverXsp_[1][imod1][cl1]<<" X2 "<<leftoverXsp_[2][imod2][cl2]<<" x_ex3 "<<x_ex3<<endl;
                                if ( fDebug) cout<<" Y1 "<<leftoverYsp_[1][imod1][cl1]<<" Y2 "<<leftoverYsp_[2][imod2][cl2]<<" y_ex3 "<<y_ex3<<endl;

                                Int_t st3 = 3;
                                Double_t  xp_ex3 = y_ex3*(Angle(st3,imod3))*tg2_5 + x_ex3;
                                if ( fabs(xp_ex3 - leftoverXp_[3][imod3][clxp3]) > hrd_case3 ) continue;

                                if ( !was_candidate  ) {
                                    iModX_candidate      = imod3;
                                    iClustXMod_candidate = clxp3;
                                    was_candidate = 1;
                                    if (fDebug) cout<<" write candidate3 xp3 = "<<leftoverXp_[3][imod3][clxp3]<<" xp_ex3 "<<xp_ex3<<endl;
                                }else{
                                    if ( fabs(xp_ex3 - leftoverXp_[3][imod3][clxp3]) < fabs(xp_ex3 - leftoverXp_[3][iModX_candidate][iClustXMod_candidate]) ) {

                                        iModX_candidate      = imod3;
                                        iClustXMod_candidate = clxp3;
                                        if (fDebug) cout<<" re-write cand3 xp3 = "<<leftoverXp_[3][imod3][clxp3]<<" xp_ex3 "<<xp_ex3<<endl;

                                    }
                                }//else

                                if (was_candidate) Npoint = 5;

                            }//clxp3
                        }//st3

                        if ( was_candidate ){
                            iModX[3]                             = iModX_candidate;
                            iClustXMod[3]                        = iClustXMod_candidate;
                            Xforglfit[3][iModX_candidate]        = -999.;
                            SigmXforglfit[3][iModX_candidate]    = -999.;
                            Xpforglfit[3][iModX_candidate]       =  leftoverXp_[3][iModX_candidate][iClustXMod_candidate];
                            SigmXpforglfit[3][iModX_candidate]   =  leftoverXpsig[3][iModX_candidate][iClustXMod_candidate];
                            Yforglfit[3][iModX_candidate]        = -999.;
                            SigmYforglfit[3][iModX_candidate]    = -999.;

                            //---------------------------	GlobalFit---------------------------------
                            TotalNumberOfHits = 0;
                            ChiSquareNdf      = 999.;
                            ChiSquare         = 999.;

                            for (Int_t coeff = 0; coeff < 4; ++coeff) {
                                line[coeff] = 0.;
                            }

                            calculationOfChi2(Xforglfit, Xpforglfit, SigmXforglfit, SigmXpforglfit, iClustXMod, iModX, ChiSquare, line);

                            TotalNumberOfHits = Npoint;
                            if ( Npoint > 4 )
                                ChiSquareNdf = ChiSquare/(TotalNumberOfHits - 4);
                            else ChiSquareNdf = ChiSquare;

                            if ( fDebug) cout<<" case3 xp: before cut ChiSquareNDF "<<ChiSquareNdf<<" TotalNumberOfHits  "<<TotalNumberOfHits <<" fitted line par "<<line[0]<<" "<<line[1]<<" "<<line[2]<<" "<<line[3]<<endl;
                            if (fDebug) cout<<endl;
                            if ( fDebug) cout<<" ChiSquareNdf "<<ChiSquareNdf<<endl;

                        }//if (was_candidate  > 0 ){
                        if ( fDebug) cout<<" ChiSquareNdf "<<ChiSquareNdf<<endl;

                        if ( ChiSquareNdf < Chi2_Globcut ){

                            if ( fabs(line[2] * ( Z0_SRC_target - Zcentr ) + line[3]) > half_target_regionY ){
                                line[2] = ay12_from_target;
                                line[3] = Yv_av - line[2] * ( Z0_SRC_target - Zcentr );
                                if ( fDebug) cout<<"       xp       replace to Yv-Y12 line par "<<line[2]<<" "<<line[3]<<endl;
                            }

                            xp5_exist = 1;
                            if ( fDebug) cout<<"xp5_exist"<<endl;

                        }
                        else{
                            Npoint = 0;
                            iModX[3]                     = -1;
                            iClustXMod[3]                = -1;
                            Xforglfit[3][iModX[3]]       = -999.;
                            SigmXforglfit[3][iModX[3]]   = -999.;
                            Xpforglfit[3][iModX[3]]      = -999.;
                            SigmXpforglfit[3][iModX[3]]  = -999.;
                            xp5_exist = 0;
                        }

                    }//if ( !x5_exist){
                    if (fDebug) cout<<"---xp.----"<<endl;
                    if (fDebug) cout<<" was_candidate "<<was_candidate<<endl;
                    if (fDebug) cout<<"case3: end 3xp st: iModX[1] "<<iModX[1]<<" iCl "<<iClustXMod[1]<<" iModX[2] "<<iModX[2]<<" iCl "<<iClustXMod[2]<<endl;
                    if (fDebug) cout<<" TotalNumberOfHits "<<TotalNumberOfHits<<" ChiSquareNdf "<<ChiSquareNdf<<endl;
                    // Xpforglfit - > Xp_cand

                    Int_t  Mod_cand[fNstations], iCl_cand[fNstations];
                    Double_t X_cand[fNstations], Xp_cand[fNstations], Y_cand[fNstations],
                            SigX_cand[fNstations], SigXp_cand[fNstations], SigY_cand[fNstations],
                            Chi2_cand = 999., line_cand[4];

                    for (Int_t coeff = 0; coeff < 4; ++coeff) {
                        line_cand[coeff] = 0.;
                    }

                    for (Int_t st = 1; st < fNstations; ++st) {
                        Mod_cand[st]   = -1;
                        iCl_cand[st]   = -1;
                        X_cand[st]     = -999.;
                        Xp_cand[st]    = -999.;
                        Y_cand[st]     = -999.;
                        SigX_cand[st]  = -999.;
                        SigXp_cand[st] = -999.;
                        SigY_cand[st]  = -999.;
                    }

                    if (fDebug) cout<<" TotalNumberOfHits "<<TotalNumberOfHits<<" ChiSquareNdf "<<ChiSquareNdf<<" Chi2_Globcut "<<Chi2_Globcut<<endl;


                    if (TotalNumberOfHits >  4){
                        if ( fabs(line[0] * ( Z0_SRC_target - Zcentr ) + line[1]) > half_target_regionX){
                            if (fDebug) cout<<" half_target_regionX cut: "<<half_target_regionX<<" Xv "<<fabs(line[0] * ( Z0_SRC_target - Zcentr ) + line[1])<<endl;
                            ChiSquareNdf = 999.;
                            continue;
                        }
                    }

                    if (TotalNumberOfHits > 4 && ChiSquareNdf < Chi2_cand && ChiSquareNdf < Chi2_Globcut &&  was_candidate ){
                        if (fDebug) cout<<" TotalNumberOfHits "<<TotalNumberOfHits<<" ChiSquareNdf "<<ChiSquareNdf<<endl;
                        Chi2_cand = ChiSquareNdf;
                        line_cand[0] = line[0];
                        line_cand[1] = line[1];
                        line_cand[2] = line[2];
                        line_cand[3] = line[3];

                        for (Int_t st = 1; st < fNstations; ++st) {
                            if (iModX[st] < 0) continue;

                            Mod_cand[st]   = iModX[st];
                            iCl_cand[st]   = iClustXMod[st];
                            X_cand[st]     = Xforglfit[st][iModX[st]];
                            Y_cand[st]     = Yforglfit[st][iModX[st]];
                            Xp_cand[st]    = Xpforglfit[st][iModX[st]];
                            SigX_cand[st]  = SigmXforglfit[st][iModX[st]];
                            SigY_cand[st]  = SigmYforglfit[st][iModX[st]];
                            SigXp_cand[st] = SigmXpforglfit[st][iModX[st]];

                            if (fDebug) cout<<" X_cand["<<st<<"] "<< X_cand[st] <<" Xp_cand["<<st<<"] "<<Xp_cand[st] <<" iMod "<<iModX[st]<<" iCl "<<iClustXMod[st]<<" ClSizeXp "<<XpspClust_width_[st][Mod_cand[st]][iClustXMod[st]]<<endl;

                        }
                    }//5 points

                    if (TotalNumberOfHits >  4 && Chi2_cand < Chi2_Globcut && was_candidate ){//ChiSquareNdf < Chi2_Globcut

                        for(int itr = 0; itr< vec_tracks_.size(); ++itr){
                            //deleting randomly recorded tracks
                            if (
                                    ( (vec_tracks_.at(itr).CoordX[1] ==  X_cand[1] && X_cand[1] > -900.)  ||  (vec_tracks_.at(itr).CoordXp[1] == Xp_cand[1] && Xp_cand[1] > -900.) )
                                    ||
                                    ( (vec_tracks_.at(itr).CoordX[2] ==  X_cand[2] && X_cand[2] > -900.)  ||  (vec_tracks_.at(itr).CoordXp[2] == Xp_cand[2] && Xp_cand[2] > -900.) )
                                    ||
                                    ( (vec_tracks_.at(itr).CoordX[3] ==  X_cand[3] && X_cand[3] > -900.)  ||  (vec_tracks_.at(itr).CoordXp[3] == Xp_cand[3] && Xp_cand[3] > -900.) )

                                    ){
                                if ( vec_tracks_.at(itr).Chi2 > Chi2_cand ){
                                    vec_tracks_.erase(vec_tracks_.begin()+ itr);
                                    continue;
                                } else {
                                    Chi2_cand = 999.;
                                    continue;
                                }
                            }//if
                        }//itr

                        if ( Chi2_cand > Chi2_Globcut) continue;

                        tmpTr.Chi2     = Chi2_cand;
                        tmpTr.Nhits    = TotalNumberOfHits;
                        bx_candidate   = line_cand[0] * ( Z0_SRC_target -Zcentr) + line_cand[1];
                        by_candidate   = line_cand[2] * ( Z0_SRC_target -Zcentr) + line_cand[3];
                        tmpTr.Xv       = bx_candidate;
                        tmpTr.Yv       = by_candidate;
                        tmpTr.param[0] = line_cand[0];
                        tmpTr.param[1] = line_cand[1];
                        tmpTr.param[2] = line_cand[2];
                        tmpTr.param[3] = line_cand[3];

                        if (fDebug) cout<<" Xv_candidate  "<<bx_candidate<<" Yv_candidate "<<by_candidate<<" ChiSquareNdf "<<ChiSquareNdf<<endl;
                        for (Int_t st = 1; st < fNstations; ++st) {
                            if (Mod_cand[st] < 0) continue;

                            tmpTr.ModNum[st]  = iModX[st];
                            tmpTr.CoordX[st]  = X_cand[st];
                            tmpTr.CoordXp[st] = Xp_cand[st];
                            tmpTr.SigmaX[st]  = SigX_cand[st];
                            tmpTr.SigmaXp[st] = SigXp_cand[st];
                            tmpTr.CoordZ[st]  = ZstnCforglfit[st][Mod_cand[st]];
                            tmpTr.CoordY[st]  = Y_cand[st];
                            tmpTr.SigmaY[st]  = SigY_cand[st];

                            if ( st < 3 ){
                                tmpTr.StripNumX[st] =        XspClust_[st][Mod_cand[st]][iCl_cand[st]];
                                tmpTr.StripNumXp[st]=       XpspClust_[st][Mod_cand[st]][iCl_cand[st]];
                                tmpTr.ClSizeX[st]   =  XspClust_width_[st][Mod_cand[st]][iCl_cand[st]];
                                tmpTr.ClSizeXp[st]  = XpspClust_width_[st][Mod_cand[st]][iCl_cand[st]];
                                tmpTr.SumQX[st]     =         sumQxsp_[st][Mod_cand[st]][iCl_cand[st]];
                                tmpTr.SumQXp[st]    =        sumQxpsp_[st][Mod_cand[st]][iCl_cand[st]];
                            }else{
                                tmpTr.StripNumX[st] =        XClust_[st][iModX[st]][iClustXMod_candidate];
                                tmpTr.StripNumXp[st]=       XpClust_[st][iModX[st]][iClustXMod_candidate];
                                tmpTr.ClSizeX[st]   =  XClust_width_[st][iModX[st]][iClustXMod_candidate];
                                tmpTr.ClSizeXp[st]  = XpClust_width_[st][iModX[st]][iClustXMod_candidate];
                                tmpTr.SumQX[st]     =         sumQx_[st][iModX[st]][iClustXMod_candidate];
                                tmpTr.SumQXp[st]    =        sumQxp_[st][iModX[st]][iClustXMod_candidate];
                            }

                            if (fDebug) cout<<" st "<<st<<" Z "<<tmpTr.CoordZ[st]<<endl;
                            if (fDebug) cout<<" tmpTr.CoordX["<<st <<"]  "<<tmpTr.CoordX[st]<<" strip "<<tmpTr.StripNumX[st]<<" Clsize "<<tmpTr.ClSizeX[st]<<" Q "<<tmpTr.SumQX[st]<<endl;
                            if (fDebug) cout<<" tmpTr.CoordXp["<<st <<"] "<<tmpTr.CoordXp[st]<<" strip "<<tmpTr.StripNumXp[st]<<" Clsize "<<tmpTr.ClSizeXp[st]<<" Q "<<tmpTr.SumQXp[st]<<endl;
                        }//st

                        vec_tracks_.push_back(tmpTr);
                    }//chi2

                }//cl2
            }//st2
        }//cl1
    }//st1
    if (fDebug) cout<<endl;
    if (fDebug) cout<<" after second case 3 write cand: "<<vec_tracks_.size()<<endl;
}
//----------------case3.-----------------------------------------------



//----------------------------------------------------------------------
void BmnSiliconTrackFinder::Case4( Int_t & Fi_St,Int_t **Nleftoversp_,Double_t ***leftoverXsp_, Double_t ***leftoverXpsp_, Double_t ***leftoverYsp_,Double_t ***leftoverXsigsp_, Double_t ***leftoverXpsigsp_, Double_t ***leftoverYsigsp_,
                                   Int_t  **NleftoverX_,  Int_t **NleftoverXp_,Double_t ***leftoverX_, Double_t ***leftoverXp_,Double_t ***leftoverXsig_, Double_t ***leftoverXpsig_,
                                   vector<tracksX> & vec_tracks_,
                                   Double_t ***XClust_, Double_t ***XClust_width_, Double_t ***sumQx_, Double_t ***XpClust_, Double_t ***XpClust_width_, Double_t ***sumQxp_,
                                   Double_t ***XspClust_, Double_t ***XspClust_width_, Double_t ***sumQxsp_, Double_t ***XpspClust_, Double_t ***XpspClust_width_, Double_t ***sumQxpsp_){
    
    tracksX tmpTr;

    Int_t Nmod_Fi = fNmodules1;
    Int_t Nmod_Sec = fNmodules2;
    Int_t Sec_St = 2;

    if ( Fi_St == 2) {
        Nmod_Fi = fNmodules2;
        Nmod_Sec = fNmodules1;
        Sec_St = 1;
    }

    Int_t  Mod_cand[fNstations], iCl_cand[fNstations];
    Double_t X_cand[fNstations], Xp_cand[fNstations], Y_cand[fNstations],
            SigX_cand[fNstations], SigXp_cand[fNstations], SigY_cand[fNstations],
            Chi2_cand = 999., line_cand[4];

    for (Int_t coeff = 0; coeff < 4; ++coeff) {
        line_cand[coeff] = 0.;
    }

    for (Int_t st = 1; st < fNstations; ++st) {
        Mod_cand[st]   = -1;
        iCl_cand[st]   = -1;
        X_cand[st]     = -999.;
        Xp_cand[st]    = -999.;
        Y_cand[st]     = -999.;
        SigX_cand[st]  = -999.;
        SigXp_cand[st] = -999.;
        SigY_cand[st]  = -999.;
    }

    Double_t bx_candidate = 999., by_candidate = 999.;
    //st 3 best Xv Yv
    //spatial points in 3 st
    for (Int_t imod3 = 0; imod3 < fNmodules3; imod3++) {
        for (Int_t cl3 = 0; cl3 < Nleftoversp_[3][imod3]; cl3++){

            Double_t Xv_min = 2.5, Yv_min = 2.5;
            Bool_t   was_candidate     = 0;
            // points in 1 or 2 st
            for (Int_t imod1 = 0; imod1 < Nmod_Fi; imod1++) {
                for (Int_t cl1 = 0; cl1 < NleftoverX[Fi_St][imod1]; cl1++){

                    Double_t ax_target_region = -999., ay_target_region = -999.;
                    Double_t ax_candidate = 999., ay_candidate = 999.;

                    for (Int_t imod2 = 0; imod2 < Nmod_Sec; imod2++) {
                        for (Int_t cl2 = 0; cl2 < NleftoverXp_[Sec_St][imod2]; cl2++){

                            if ( fabs(leftoverX_[Fi_St][imod1][cl1]  -  leftoverXp_[Sec_St][imod2][cl2])   > half_roadX1_Xp2  ) continue;

                            Double_t  z1_2 = 0.5 * ( Zstation[1][imod1] + Zstation[2][imod2] );
                            Double_t Y_1_2 = GetXYspatial_station1_2(Fi_St, imod2, leftoverX_[Fi_St][imod1][cl1] , leftoverXp_[Sec_St][imod2][cl2]);

                            if(fDebug) cout<<" Fi_St "<<Fi_St<<" X "<<leftoverX_[Fi_St][imod1][cl1]<<" Xp "<<leftoverXp_[Sec_St][imod2][cl2]<<" new Y "<<Y_1_2<<endl;

                            ax_target_region = (leftoverX_[Fi_St][imod1][cl1] - leftoverXsp_[3][imod3][cl3]) / (Zstation[Fi_St][imod1] - Zstation[3][imod3]);
                            ay_target_region = (Y_1_2 - leftoverYsp_[3][imod3][cl3]) / (z1_2 - Zstation[3][imod3]);

                            Double_t Xv_cand = ax_target_region * (- z1_2 + Z0_SRC_target )+ leftoverX_[Fi_St][imod1][cl1];
                            Double_t Yv_cand = ay_target_region * ( - z1_2 + Z0_SRC_target )+ Y_1_2;

                            if ( fabs(Xv_cand) > half_target_regionX) continue;
                            if ( fabs(Yv_cand) > half_target_regionY) continue;

                            if ( (fabs(Xv_min) + 0.25*fabs(Yv_min) ) < ( fabs(Xv_cand) + 0.25*fabs(Yv_cand) ) ) continue;

                            if(fDebug) cout<<" Xv_cand "<<Xv_cand<<" Yv_cand "<<Yv_cand<<endl;

                            was_candidate = 1;

                            Chi2_cand  = 0.;
                            bx_candidate = Xv_cand;
                            by_candidate = Yv_cand;

                            line_cand[0] = ax_target_region;
                            line_cand[1] = Xv_cand + ax_target_region * (  Zcentr - Z0_SRC_target );
                            line_cand[2] = ay_target_region;
                            line_cand[3] = Yv_cand + ay_target_region * (  Zcentr - Z0_SRC_target );

                            Mod_cand[Fi_St]   = imod1;
                            iCl_cand[Fi_St]   = cl1;
                            X_cand[Fi_St]     = leftoverX_[Fi_St][imod1][cl1];
                            Xp_cand[Fi_St]    = -999.;
                            Y_cand[Fi_St]     = -999.;
                            SigX_cand[Fi_St]  = 1.;
                            SigXp_cand[Fi_St] = 1.;
                            SigY_cand[Fi_St]  = 1.;

                            Mod_cand[Sec_St]   = imod2;
                            iCl_cand[Sec_St]   = cl2;
                            X_cand[Sec_St]     = -999.;
                            Xp_cand[Sec_St]    = leftoverXp_[Sec_St][imod2][cl2];
                            Y_cand[Sec_St]     = -999.;
                            SigX_cand[Sec_St]  = 1.;
                            SigXp_cand[Sec_St] = 1.;
                            SigY_cand[Sec_St]  = 1.;

                            Mod_cand[3]   = imod3;
                            iCl_cand[3]   = cl3;
                            X_cand[3]     = leftoverXsp_[3][imod3][cl3];
                            Xp_cand[3]    = leftoverXpsp_[3][imod3][cl3];
                            Y_cand[3]     = leftoverYsp_[3][imod3][cl3];
                            SigX_cand[3]  = leftoverXsigsp_[3][imod3][cl3];
                            SigXp_cand[3] = leftoverXpsigsp_[3][imod3][cl3];
                            SigY_cand[3]  = leftoverYsigsp_[3][imod3][cl3];
                        }
                    }//2st
                }
            }//1st

            for(int itr = 0; itr< vec_tracks.size(); ++itr){
                //deleting randomly recorded tracks
                if (
                        ( (vec_tracks.at(itr).CoordX[1] ==  X_cand[1] && X_cand[1] > -900.)   ||  (vec_tracks.at(itr).CoordXp[1] == Xp_cand[1] && Xp_cand[1] > -900.) )
                        ||
                        ( (vec_tracks.at(itr).CoordX[2] ==  X_cand[2] && X_cand[2] > -900.)   ||  (vec_tracks.at(itr).CoordXp[2] == Xp_cand[2] && Xp_cand[2] > -900.) )
                        ||
                        ( (vec_tracks.at(itr).CoordX[3] ==  X_cand[3] && X_cand[3] > -900.)   ||  (vec_tracks.at(itr).CoordXp[3] == Xp_cand[3] && Xp_cand[3] > -900.) )
                        ){
                    if ( (fabs(vec_tracks.at(itr).Xv) + 0.25*fabs(vec_tracks.at(itr).Yv) ) > ( fabs(bx_candidate) + 0.25*fabs(by_candidate) ) ){ // v2 closed to v1
                        vec_tracks.erase(vec_tracks.begin()+ itr);
                        continue;
                    }
                    else {
                        Chi2_cand = 999.;
                        continue;
                    }
                }//if
            }//itr

            if ( Chi2_cand > Chi2_Globcut) continue;

            tmpTr.Chi2     = Chi2_cand ;
            tmpTr.Nhits    = 4;
            tmpTr.Xv       = bx_candidate;
            tmpTr.Yv       = by_candidate;
            tmpTr.param[0] = line_cand[0];
            tmpTr.param[1] = line_cand[1];
            tmpTr.param[2] = line_cand[2];
            tmpTr.param[3] = line_cand[3];

            if (fDebug) cout<<endl;
            if (fDebug) cout<<" Xv_candidate  "<<bx_candidate<<" Yv_candidate "<<by_candidate<<" Nhits "<<tmpTr.Nhits<<endl;
            for (Int_t st = 1; st < fNstations; ++st) {
                if (Mod_cand[st] < 0) continue;

                tmpTr.ModNum[st]  = iModX[st];
                tmpTr.CoordX[st]  = X_cand[st];
                tmpTr.CoordXp[st] = Xp_cand[st];
                tmpTr.SigmaX[st]  = SigX_cand[st];
                tmpTr.SigmaXp[st] = SigXp_cand[st];
                tmpTr.CoordZ[st]  = ZstnCforglfit[st][Mod_cand[st]];
                tmpTr.CoordY[st]  = Y_cand[st];
                tmpTr.SigmaY[st]  = SigY_cand[st];

                if ( st > 3 ){
                    tmpTr.StripNumX[st] =        XClust_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.StripNumXp[st]=       XpClust_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.ClSizeX[st]   =  XClust_width_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.ClSizeXp[st]  = XpClust_width_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.SumQX[st]     =         sumQx_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.SumQXp[st]    =        sumQxp_[st][Mod_cand[st]][iCl_cand[st]];
                }else{
                    tmpTr.StripNumX[st] =        XspClust_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.StripNumXp[st]=       XpspClust_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.ClSizeX[st]   =  XspClust_width_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.ClSizeXp[st]  = XpspClust_width_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.SumQX[st]     =         sumQxsp_[st][Mod_cand[st]][iCl_cand[st]];
                    tmpTr.SumQXp[st]    =        sumQxpsp_[st][Mod_cand[st]][iCl_cand[st]];
                }

                if (fDebug) cout<<" st "<<st<<" Z "<<tmpTr.CoordZ[st]<<endl;
                if (fDebug) cout<<" tmpTr.CoordX["<<st <<"]  "<<tmpTr.CoordX[st]<<" strip "<<tmpTr.StripNumX[st]<<" Clsize "<<tmpTr.ClSizeX[st]<<" Q "<<tmpTr.SumQX[st]<<endl;
                if (fDebug) cout<<" tmpTr.CoordXp["<<st <<"] "<<tmpTr.CoordXp[st]<<" strip "<<tmpTr.StripNumXp[st]<<" Clsize "<<tmpTr.ClSizeXp[st]<<" Q "<<tmpTr.SumQXp[st]<<endl;

            }//st

            vec_tracks.push_back(tmpTr);
            if (fDebug) cout<<" tmpTr.Xv "<<tmpTr.Xv<<" tmpTr.Yv "<<tmpTr.Yv<<endl;
            if (fDebug) cout<<endl;
        }
    }//3st sp

}
//----------------------------------------------------------------------



//----------------------------------------------------------------------
Double_t BmnSiliconTrackFinder::GetXYspatial_station1_2(Int_t &Fi_St, Int_t &imod, Double_t & XCoor, Double_t &XpCoor ) {

    Int_t Nmod_Fi = fNmodules1;
    Int_t Nmod_Sec = fNmodules2;
    Int_t Sec_St = 2;
    Double_t Ysp_1_2 = -999.0;
    Double_t YCoor_cand;

    if ( Fi_St == 2) {
        Nmod_Fi = fNmodules2;
        Nmod_Sec = fNmodules1;
        Sec_St = 1;
    }

    if ( XCoor > -900. && XpCoor > -900.){
        YCoor_cand =  ( XpCoor - XCoor  ) / tg2_5;

        if (YCoor_cand >= shiftY[Sec_St][imod] - 0.5  &&  YCoor_cand <= shiftY[Sec_St][imod] + 12. +0.5 ) {
            Ysp_1_2 = YCoor_cand;
        }
    }
    return(Ysp_1_2);
}

//----------------------------------------------------------------------
void BmnSiliconTrackFinder::calculationOfChi2(Double_t **Xforglfit_, Double_t **Xpforglfit_, Double_t **SigmXforglfit_, Double_t **SigmXpforglfit_, 
                                              Int_t *iClustXMod_,  Int_t *iModX_, Double_t & ChiSquare_, Double_t *line_){

    //---------------------------GlobalFit---------------------------------
    ChiSquare_         = 0.;
    if (fDebug) cout<<endl;
    if (fDebug)   cout<<"--GlobalFit--"<<endl;
    //setting global fit matrices to zero

    for (Int_t i = 0; i < 4; ++i) {
        for (Int_t j = 0; j < 4; ++j) {
            Amatr[i][j] = 0.;
        }
    }
    for (Int_t i = 0; i < 4; ++i) {
        rhs[i]   = 0;
        line_[i] = 0;
    }
    for (Int_t i = 0; i < 16; ++i) {
        AmatrArray[i] = 0;
    }
    for (Int_t i = 0; i < 16; ++i) {
        AmatrInverted[i] = 0;
    }

    GlobalFit( Xforglfit_, Xpforglfit_, ZstnCforglfit, SigmXforglfit_, SigmXpforglfit_, Amatr, rhs);

    //here we do smth with Amatr & rhs (solving)
    // Amatr is row-major
    for (Int_t row = 0; row < 4; ++row) {
        for (Int_t col = 0; col < 4; ++col) {
            AmatrArray[4*col + row] = Amatr[row][col];
        }
    }//AmatrArray is col-major

    bool ifSuccess = InvertMatrix(AmatrArray, AmatrInverted);

    if (!ifSuccess) {cout<< "???? InvertMatrix is unsuccessfull ????"<<endl;   }
    if (!ifSuccess) {cout<< "???? InvertMatrix is unsuccessfull ????"<<endl; return;} // continue; }  //FIXME: what does this mean?

    for (Int_t coeff = 0; coeff < 4; ++coeff) {
        for (Int_t iCol = 0; iCol < 4; ++iCol) {
            line_[coeff] += AmatrInverted[4*iCol + coeff]*rhs[iCol];
        }
    }
    //line_ = { ax, bx, ay, by}
    if ( fDebug ) cout<<" ax "<<line_[0]<<" bx "<<line_[1]<<" ay "<<line_[2]<<" by "<<line_[3]<<endl;

    //---------------------------Chi2---------------------------------
    if ( fDebug) cout<<"ChiSquare_ "<<ChiSquare_<<endl;
    for (Int_t ist = 1; ist < fNstations; ++ist) {
        if (Xforglfit_[ist][iModX_[ist]] < -900. ) continue;

        Double_t hit = Xforglfit_[ist][iModX_[ist]] ;
        Double_t fit = line_[0]*ZstnCforglfit[ist][iModX_[ist]] + line_[1];

        Double_t sigmaX_2 = SigmXforglfit_[ist][iModX_[ist]]*SigmXforglfit_[ist][iModX_[ist]];
        ChiSquare_ += ((hit-fit)*(hit-fit))/(sigmaX_2);
        if ( fDebug) cout<<" ist "<<ist<<" hitx  "<<hit<<" dx "<<hit-fit<<" Z "<<ZstnCforglfit[ist][iModX_[ist]]<<" sigma "<<SigmXforglfit_[ist][iModX_[ist]]<<" ChiSquare_ "<<ChiSquare_<<endl;
    }
    for (Int_t ist = 1; ist < fNstations; ++ist) {
        if (Xpforglfit_[ist][iModX_[ist]] < -900. ) continue;

        Double_t hit = Xpforglfit_[ist][iModX_[ist]];
        Double_t fit = (line_[0]*ZstnCforglfit[ist][iModX_[ist]] + line_[1]) + (line_[2]*ZstnCforglfit[ist][iModX_[ist]] + line_[3]) * Angle(ist,iModX_[ist]) * tg2_5 ;// x' = x + y*tg    //x*cos + y*sin

        Double_t sigmaXp_2 = SigmXpforglfit_[ist][iModX_[ist]]*SigmXpforglfit_[ist][iModX_[ist]];
        ChiSquare_ += ((hit-fit)*(hit-fit))/(sigmaXp_2);
        if ( fDebug) cout<<" ist "<<ist<<" hitxp "<<hit<<" dxp "<<hit-fit<<" Z "<<ZstnCforglfit[ist][iModX_[ist]]<<" sigma "<<SigmXpforglfit_[ist][iModX_[ist]]<<" ChiSquare_ "<<ChiSquare_<<endl;
    }//ist

}
//----------------------------------------------------------------------



//----function to calculate angle in modules----------------------------
Int_t BmnSiliconTrackFinder::Angle(Int_t &st, Int_t &Mod) {

    Int_t sign = 0;
    if (st == 1 && Mod == 0) sign = -1;
    if (st == 1 && Mod == 1) sign = 1;
    if (st == 1 && Mod == 2) sign = 1;
    if (st == 1 && Mod == 3) sign = -1;

    if (st == 2 && Mod == 0) sign = -1;
    if (st == 2 && Mod == 1) sign = 1;

    if (st == 3 && Mod == 0) sign = -1;
    if (st == 3 && Mod == 1) sign = -1;
    if (st == 3 && Mod == 2) sign = 1;
    if (st == 3 && Mod == 3) sign = 1;
    if (st == 3 && Mod == 4) sign = 1;
    if (st == 3 && Mod == 5) sign = 1;
    if (st == 3 && Mod == 6) sign = -1;
    if (st == 3 && Mod == 7) sign = -1;

    return (sign);
}
//----------------------------------------------------------------------


//--------------------Matrix      Coefficients Calculation--------------
bool BmnSiliconTrackFinder::InvertMatrix(Double_t *m, Double_t *invOut) {
    Double_t inv[16], det;
    int i;

    inv[0] = m[5] * m[10] * m[15] -m[5] * m[11] * m[14] -m[9] * m[6] * m[15] +m[9] * m[7] * m[14] +m[13] * m[6] * m[11] -m[13] * m[7] * m[10];
    inv[4] = -m[4] * m[10] * m[15] +m[4] * m[11] * m[14] +m[8] * m[6] * m[15] -m[8] * m[7] * m[14] -m[12] * m[6] * m[11] +m[12] * m[7] * m[10];
    inv[8] = m[4] * m[9] * m[15] -m[4] * m[11] * m[13] -m[8] * m[5] * m[15] +m[8] * m[7] * m[13] +m[12] * m[5] * m[11] -m[12] * m[7] * m[9];
    inv[12] = -m[4] * m[9] * m[14] +m[4] * m[10] * m[13] +m[8] * m[5] * m[14] -m[8] * m[6] * m[13] -m[12] * m[5] * m[10] +m[12] * m[6] * m[9];
    inv[1] = -m[1] * m[10] * m[15] +m[1] * m[11] * m[14] +m[9] * m[2] * m[15] -m[9] * m[3] * m[14] -m[13] * m[2] * m[11] +m[13] * m[3] * m[10];
    inv[5] = m[0] * m[10] * m[15] -m[0] * m[11] * m[14] -m[8] * m[2] * m[15] +m[8] * m[3] * m[14] +m[12] * m[2] * m[11] -m[12] * m[3] * m[10];
    inv[9] = -m[0] * m[9] * m[15] +m[0] * m[11] * m[13] +m[8] * m[1] * m[15] -m[8] * m[3] * m[13] -m[12] * m[1] * m[11] +m[12] * m[3] * m[9];
    inv[13] = m[0] * m[9] * m[14] -m[0] * m[10] * m[13] -m[8] * m[1] * m[14] +m[8] * m[2] * m[13] +m[12] * m[1] * m[10] -m[12] * m[2] * m[9];
    inv[2] = m[1] * m[6] * m[15] -m[1] * m[7] * m[14] -m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +m[13] * m[2] * m[7] -m[13] * m[3] * m[6];
    inv[6] = -m[0] * m[6] * m[15] +m[0] * m[7] * m[14] +m[4] * m[2] * m[15] -m[4] * m[3] * m[14] -m[12] * m[2] * m[7] +m[12] * m[3] * m[6];
    inv[10] = m[0] * m[5] * m[15] -m[0] * m[7] * m[13] -m[4] * m[1] * m[15] +m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
    inv[14] = -m[0] * m[5] * m[14] +m[0] * m[6] * m[13] +m[4] * m[1] * m[14] -m[4] * m[2] * m[13] -m[12] * m[1] * m[6] +m[12] * m[2] * m[5];
    inv[3] = -m[1] * m[6] * m[11] +m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -m[5] * m[3] * m[10] - m[9] * m[2] * m[7] +m[9] * m[3] * m[6];
    inv[7] = m[0] * m[6] * m[11] -m[0] * m[7] * m[10] -m[4] * m[2] * m[11] +m[4] * m[3] * m[10] +m[8] * m[2] * m[7] -m[8] * m[3] * m[6];
    inv[11] = -m[0] * m[5] * m[11] +m[0] * m[7] * m[9] +m[4] * m[1] * m[11] -m[4] * m[3] * m[9] -m[8] * m[1] * m[7] +m[8] * m[3] * m[5];
    inv[15] = m[0] * m[5] * m[10] -m[0] * m[6] * m[9] -m[4] * m[1] * m[10] +m[4] * m[2] * m[9] +m[8] * m[1] * m[6] -m[8] * m[2] * m[5];
    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;
    det = 1.0 / det;
    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;
    return true;
}
//----------------------------------------------------------------------


//-------------Function to calculate fit parameters---------------------
void BmnSiliconTrackFinder::GlobalFit(Double_t **XHits, Double_t **XpHits, Double_t **ZHits, Double_t **SigmaXHits, Double_t **SigmaXpHits, Double_t **Amatr_, Double_t *rhs_) {
    //if ( fDebug ) cout<<endl;
    //if ( fDebug ) cout<<"in GlobalFit "<<endl;

    Double_t Z = 0.;
    Double_t X = -999;
    Double_t U = -999.;
    Double_t V = -999.;
    Double_t sigmaX_2 = 0.;
    Double_t sigmaU_2 = 0.;
    Double_t sigmaV_2 = 0.;

    for (Int_t iSt = 1; iSt < fNstations; ++iSt) {
        for (Int_t imod = 0; imod < fNmodules; ++imod) {

            if (iSt == 1 && imod > 3) continue;
            if (iSt == 2 && imod > 1) continue;

            Z = ZHits[iSt][imod];
            //if ( fDebug ) cout<<" Z "<<Z<<endl;

            if (XHits[iSt][imod] > -900.) { //Amatr[0][0] : d/dax, ax; Amatr[0][1] : d/dax, bx ...

                sigmaX_2 = SigmaXHits[iSt][imod] * SigmaXHits[iSt][imod];
                X = XHits[iSt][imod];
                // if ( fDebug ) cout<<" X "<<X<<endl;

                Amatr_[0][0] += 2 * Z * Z / sigmaX_2;
                Amatr_[0][1] += 2 * Z / sigmaX_2;
                rhs_[0] += 2 * Z * X / sigmaX_2;
                Amatr_[1][0] += 2 * Z / sigmaX_2;
                Amatr_[1][1] += 2 / sigmaX_2;
                rhs_[1] += 2 * X / sigmaX_2;
            }

            if (XpHits[iSt][imod] > -900.) {

                if (Angle(iSt, imod) > 0) { // +
                    // if ( fDebug ) cout<< "found x+ hit in station " << iSt << " V = " << XpHitsTry[iSt] << "\n";

                    sigmaU_2 = SigmaXpHits[iSt][imod] * SigmaXpHits[iSt][imod];
                    U = XpHits[iSt][imod];
                    // if ( fDebug ) cout<<" U "<<U<<endl;

                    Amatr_[0][0] += 2 * Z * Z * cos2_5 * cos2_5 / sigmaU_2;
                    Amatr_[0][1] += 2 * Z * cos2_5 * cos2_5 / sigmaU_2;
                    Amatr_[0][2] += 2 * Z * Z * cos2_5 * sin2_5 / sigmaU_2;
                    Amatr_[0][3] += 2 * Z * cos2_5 * sin2_5 / sigmaU_2;
                    rhs_[0] += 2 * Z * cos2_5 * U / sigmaU_2;

                    Amatr_[1][0] += 2 * Z * cos2_5 * cos2_5 / sigmaU_2;
                    Amatr_[1][1] += 2 * cos2_5 * cos2_5 / sigmaU_2;
                    Amatr_[1][2] += 2 * Z * cos2_5 * sin2_5 / sigmaU_2;
                    Amatr_[1][3] += 2 * cos2_5 * sin2_5 / sigmaU_2;
                    rhs_[1] += 2 * cos2_5 * U / sigmaU_2;

                    Amatr_[2][0] += 2 * Z * Z * cos2_5 * sin2_5 / sigmaU_2;
                    Amatr_[2][1] += 2 * Z * cos2_5 * sin2_5 / sigmaU_2;
                    Amatr_[2][2] += 2 * Z * Z * sin2_5 * sin2_5 / sigmaU_2;
                    Amatr_[2][3] += 2 * Z * sin2_5 * sin2_5 / sigmaU_2;
                    rhs_[2] += 2 * Z * sin2_5 * U / sigmaU_2;

                    Amatr_[3][0] += 2 * Z * cos2_5 * sin2_5 / sigmaU_2;
                    Amatr_[3][1] += 2 * cos2_5 * sin2_5 / sigmaU_2;
                    Amatr_[3][2] += 2 * Z * sin2_5 * sin2_5 / sigmaU_2;
                    Amatr_[3][3] += 2 * sin2_5 * sin2_5 / sigmaU_2;
                    rhs_[3] += 2 * sin2_5 * U / sigmaU_2;

                }//+
                else {// -
                    // if ( fDebug ) cout<< "found x- hit in station " << iSt << " V = " << XpHitsTry[iSt] << "\n";

                    sigmaV_2 = SigmaXpHits[iSt][imod] * SigmaXpHits[iSt][imod];

                    V = XpHits[iSt][imod];
                    // if ( fDebug ) cout<<" V "<<V<<endl;

                    Amatr_[0][0] += 2 * Z * Z * cos2_5 * cos2_5 / sigmaV_2;
                    Amatr_[0][1] += 2 * Z * cos2_5 * cos2_5 / sigmaV_2;
                    Amatr_[0][2] += -2 * Z * Z * cos2_5 * sin2_5 / sigmaV_2;
                    Amatr_[0][3] += -2 * Z * cos2_5 * sin2_5 / sigmaV_2;
                    rhs_[0] += 2 * Z * cos2_5 * V / sigmaV_2;

                    Amatr_[1][0] += 2 * Z * cos2_5 * cos2_5 / sigmaV_2;
                    Amatr_[1][1] += 2 * cos2_5 * cos2_5 / sigmaV_2;
                    Amatr_[1][2] += -2 * Z * cos2_5 * sin2_5 / sigmaV_2;
                    Amatr_[1][3] += -2 * cos2_5 * sin2_5 / sigmaV_2;
                    rhs_[1] += 2 * cos2_5 * V / sigmaV_2;

                    Amatr_[2][0] += -2 * Z * Z * cos2_5 * sin2_5 / sigmaV_2;
                    Amatr_[2][1] += -2 * Z * cos2_5 * sin2_5 / sigmaV_2;
                    Amatr_[2][2] += 2 * Z * Z * sin2_5 * sin2_5 / sigmaV_2;
                    Amatr_[2][3] += 2 * Z * sin2_5 * sin2_5 / sigmaV_2;
                    rhs_[2] += -2 * Z * sin2_5 * V / sigmaV_2;

                    Amatr_[3][0] += -2 * Z * cos2_5 * sin2_5 / sigmaV_2;
                    Amatr_[3][1] += -2 * cos2_5 * sin2_5 / sigmaV_2;
                    Amatr_[3][2] += 2 * Z * sin2_5 * sin2_5 / sigmaV_2;
                    Amatr_[3][3] += 2 * sin2_5 * sin2_5 / sigmaV_2;
                    rhs_[3] += -2 * sin2_5 * V / sigmaV_2;
                } // -
            }// xp_exist
            /*
       for (Int_t coeff = 0; coeff < 4; ++coeff) {
        for (Int_t iCol = 0; iCol < 4; ++iCol) {
         if ( fDebug ) cout << "GLOBALFIT: station = " << iSt << " " << "Amatr[" << coeff << "][" << iCol << "] = " << Amatr[coeff][iCol] << "\n";
        }
         if ( fDebug ) cout<<" 	rhs["<<i<<"] "<< rhs[coeff]<<endl;
       }
       */
        }//imod
    }//st

}//global fit
//----------------------------------------------------------------------



//---------------Find Cluster Center------------------------------------
Double_t BmnSiliconTrackFinder::FindClusterCenter(Double_t* Ampl, Int_t nElements, Double_t &SumAmpll) {
    Double_t XQ = 0.;
    Double_t Q = 0.;

    for (Int_t i = 0; i < nElements; ++i) {
        Q += Ampl[i];
        // if (fDebug)  cout<<" Ampl["<<i<<"] ="<<Ampl[i]<<endl;
        XQ += Double_t(i + 0.5) * Ampl[i];
    }
    SumAmpll = Q;
    Double_t CoG = XQ / Q;
    //if (fDebug) cout << "ClasterSize = " << nElements << "; Q = " << Q << "; Q/size = " << Q/nElements << endl;
    return (CoG);
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
InitStatus BmnSiliconTrackFinder::Init()
{
    if (!expData){
        fInputBranchName  = "BmnSiliconHitClean";
        fInputBranchName2 = "BmnSiliconHitSim";
        fRunNumber = 0001;
    }

    if (fRunPeriod == 7){
        fNstations  = 4;
        fNmodules   = 8;
        fNmodules1  = 4;
        fNmodules2  = 2;
        fNmodules3  = 8;
    }
    if (fRunPeriod == 8){
        fNstations  = 5;
        fNmodules   = 2;
        fNmodules1  = 2;
        fNmodules2  = 2;
        fNmodules3  = 2;
    }

    //--some hists--
    if (fDebug) {

        hNpoint          = new TH1D("Npoint","Npoints: X+ X'; Npoints; Events", 7,1,8);fList.Add(hNpoint);
        hChiSquareNdf    = new TH1D("ChiSquareNdf","ChiSquareNdf", 200, 0, Chi2_Globcut);fList.Add(hChiSquareNdf);
        hNtracks         = new TH1D("Ntracks","Ntracks; Ntracks; Events", 10,0,10);fList.Add(hNtracks);
        hAxglob          = new TH1D("Axglob","AngleX; tgX[rad]; Events", 200, -.1,.1);fList.Add(hAxglob);
        hBxglob          = new TH1D("Bxglob","Bx; [cm]; Events", 200, -10,10);fList.Add(hBxglob);
        hAyglob          = new TH1D("Ayglob","AngleY; tgY[rad]; Events", 200, -.1,.1);fList.Add(hAyglob);
        hByglob          = new TH1D("Byglob","By; [cm]; Events", 200, -10,10);fList.Add(hByglob);
        hvertexXY        = new TH2D("vertexXY","vertexXY", 100, -5, 5, 100, -10, 5);fList.Add(hvertexXY);
        hvertex_aver_XY  = new TH2D("vertex_aver_XY","vertex_aver_XY", 100, -5, 5, 100, -10, 5);fList.Add(hvertex_aver_XY);
        hprofile_beam_z1 = new TH2D("profile_beam_z1", "beam_profile_Siz1; [cm];[cm]", 100, -10, 10, 100, -15, 5); fList.Add(hprofile_beam_z1);
        hprofile_beam_z2 = new TH2D("profile_beam_z2", "beam_profile_Siz2; [cm];[cm]", 100, -10, 10, 100, -15, 5);fList.Add(hprofile_beam_z2);
        hprofile_beam_z3 = new TH2D("profile_beam_z3", "beam_profile_Siz3; [cm];[cm]", 100, -15, 15, 100, -20, 5);fList.Add(hprofile_beam_z3);
        hAx_first_tr     = new TH1D("Ax_first_tr","Ax_first_tr; AngleX[mrad]; Events", 200, -.1,.1);fList.Add(hAx_first_tr);
        hAx_more_first_tr= new TH1D("Ax_more_first_tr","Ax_more_first_tr; AngleX[mrad]; Events", 200, -.1,.1); fList.Add(hAx_more_first_tr);
        hYXSi1_run8      = new TH2D("hYXSi1_run8","YXSi1_run8", 100, -15,15, 100, -15,15); fList.Add(hYXSi1_run8);
        hYXSi2_run8      = new TH2D("hYXSi2_run8","YXSi2_run8", 100, -15,15, 100, -15,15); fList.Add(hYXSi2_run8);
        hXSi1_run8       = new TH1D("XSi1_run8","XSi1; [cm]; Events", 200, -10,10);fList.Add(hXSi1_run8);
        hYSi1_run8       = new TH1D("YSi1_run8","YSi1; [cm]; Events", 200, -10,10);fList.Add(hYSi1_run8);
        hXSi2_run8       = new TH1D("XSi2_run8","XSi2; [cm]; Events", 200, -10,10);fList.Add(hXSi2_run8);
        hYSi2_run8       = new TH1D("YSi2_run8","YSi2; [cm]; Events", 200, -10,10);fList.Add(hYSi2_run8);

        hdX_st1_st2 = new TH1D("X_st1_st2","X (st1-st2)",100, -0.1, 0.1);fList.Add(hdX_st1_st2);
        hdX_st2_st3 = new TH1D("X_st2_st3","X (st2-st3)",100, -5., 5.);fList.Add(hdX_st2_st3);
        hdX_st1_st3 = new TH1D("X_st1_st3","X (st1-st3)",100, -5., 5.);fList.Add(hdX_st1_st3);

        hdY_st1_st2 = new TH1D("Y_st1_st2","Y (st1-st2)",100, -1,  1);fList.Add(hdY_st1_st2);
        hdY_st2_st3 = new TH1D("Y_st2_st3","Y (st2-st3)",100, -5., 5.);fList.Add(hdY_st2_st3);
        hdY_st1_st3 = new TH1D("Y_st1_st3","Y (st1-st3)",100, -5., 5.);fList.Add(hdY_st1_st3);


        hX_st_1    = new TH1D("X_st_1", "X_st_1", 100, -6, 6);fList.Add(hX_st_1);
        hX_st_2    = new TH1D("X_st_2", "X_st_2", 100, -6, 6);fList.Add(hX_st_2);
        hX_st_3    = new TH1D("X_st_3", "X_st_3", 100, -6, 6);fList.Add(hX_st_3);

        hdX_st_1    = new TH1D("dX_st_1", "dX_st_1", 100, -0.02, 0.02);fList.Add(hdX_st_1);
        hdX_st_2    = new TH1D("dX_st_2", "dX_st_2", 100, -0.02, 0.02);fList.Add(hdX_st_2);
        hdX_st_3    = new TH1D("dX_st_3", "dX_st_3", 100, -0.002, 0.002);fList.Add(hdX_st_3);

        hdXp_st_1    = new TH1D("dXp_st_1", "dXp_st_1", 100, -0.02, 0.02);fList.Add(hdXp_st_1);
        hdXp_st_2    = new TH1D("dXp_st_2", "dXp_st_2", 100, -0.02, 0.02);fList.Add(hdXp_st_2);
        hdXp_st_3    = new TH1D("dXp_st_3", "dXp_st_3", 100, -0.02, 0.02);fList.Add(hdXp_st_3);

        hdXvsXst_1 = new TH2D("dXvsXst_1","dXvsXst_1;XSi[cm];hit-fit[cm]", 100, -6, 6, 100, -0.03, 0.03);fList.Add(hdXvsXst_1);
        hdXvsXst_2 = new TH2D("dXvsXst_2","dXvsXst_2;XSi[cm];hit-fit[cm]", 100, -6, 6, 100, -0.03, 0.03);fList.Add(hdXvsXst_2);
        hdXvsXst_3 = new TH2D("dXvsXst_3","dXvsXst_3;XSi[cm];hit-fit[cm]", 100, -6, 6, 100, -0.0005, 0.0005);fList.Add(hdXvsXst_3);

        hX13_X2_m0 = new TH1D("X13_X2_m0","X2_X13_m0", 200, -0.05, 0.05);fList.Add(hX13_X2_m0);
        hX13_X2_m1 = new TH1D("X13_X2_m1","X2_X13_m1", 200, -0.05, 0.05);fList.Add(hX13_X2_m1);

        hXp13_Xp2_m0 = new TH1D("Xp13_Xp2_m0","Xp2_Xp13__m0", 200, -0.4, 0.4);fList.Add(hXp13_Xp2_m0);
        hXp13_Xp2_m1 = new TH1D("Xp13_Xp2_m1","Xp2_Xp13__m1", 200, -0.4, 0.4);fList.Add(hXp13_Xp2_m1);

        hY13_Y2_m0 = new TH1D("Y13_Y2_m0","Y2_Y13_m0", 200, -1, 1);fList.Add(hY13_Y2_m0);
        hY13_Y2_m1 = new TH1D("Y13_Y2_m1","Y2_Y13_m1", 200, -1, 1);fList.Add(hY13_Y2_m1);

        hY1m0_Y23 = new TH1D("Y1m0_Y23","Y1m0_Y23",  200, -1, 1);fList.Add(hY1m0_Y23);
        hY1m1_Y23 = new TH1D("Y1m1_Y23","Y1m1_Y23",  200, -1, 1);fList.Add(hY1m1_Y23);
        hY1m2_Y23 = new TH1D("Y1m2_Y23","Y1m2_Y23",  200, -1, 1);fList.Add(hY1m2_Y23);
        hY1m3_Y23 = new TH1D("Y1m3_Y23","Y1m3_Y23",  200, -1, 1);fList.Add(hY1m3_Y23);

        hY_st_1    = new TH1D("Y_st_1", "Y_st_1", 100, -6, 6);fList.Add(hY_st_1);
        hY_st_2    = new TH1D("Y_st_2", "Y_st_2", 100, -6, 6);fList.Add(hY_st_2);
        hY_st_3    = new TH1D("Y_st_3", "Y_st_3", 100, -6, 6);fList.Add(hY_st_3);

        hdY_st_1    = new TH1D("dY_st_1", "dY_st_1", 100, -0.6, 0.6);fList.Add(hdY_st_1);
        hdY_st_2    = new TH1D("dY_st_2", "dY_st_2", 100, -0.6, 0.6);fList.Add(hdY_st_2);
        hdY_st_3    = new TH1D("dY_st_3", "dY_st_3", 100, -0.6, 0.6);fList.Add(hdY_st_3);

        hdYvsYst_1 = new TH2D("dYvsYst_1","dYvsYst_1;YSi[cm];hit-fit[cm]", 100, -6, 6, 100, -0.6, 0.6);fList.Add(hdYvsYst_1);
        hdYvsYst_2 = new TH2D("dYvsYst_2","dYvsYst_2;YSi[cm];hit-fit[cm]", 100, -6, 6, 100, -0.6, 0.6);fList.Add(hdYvsYst_2);
        hdYvsYst_3 = new TH2D("dYvsYst_3","dYvsYst_3;YSi[cm];hit-fit[cm]", 100, -6, 6, 100, -0.4, 0.4);fList.Add(hdYvsYst_3);

        hdXst1_0_st2_0= new TH1D("dXst1_0_st2_0","dXst1_0_st2_0",100, -0.1, 0.1);
        hdXst1_0_st2_1= new TH1D("dXst1_0_st2_1","dXst1_0_st2_1",100, -0.1, 0.1);
        hdXst1_1_st2_0= new TH1D("dXst1_1_st2_0","dXst1_1_st2_0",100, -0.1, 0.1);
        hdXst1_1_st2_1= new TH1D("dXst1_1_st2_1","dXst1_1_st2_1",100, -0.1, 0.1);
        hdXst1_2_st2_0= new TH1D("dXst1_2_st2_0","dXst1_2_st2_0",100, -0.1, 0.1);
        hdXst1_2_st2_1= new TH1D("dXst1_2_st2_1","dXst1_2_st2_1",100, -0.1, 0.1);
        hdXst1_3_st2_0= new TH1D("dXst1_3_st2_0","dXst1_3_st2_0",100, -0.1, 0.1);
        hdXst1_3_st2_1= new TH1D("dXst1_3_st2_1","dXst1_3_st2_1",100, -0.1, 0.1);
        hdXst2_0_st3_1= new TH1D("dXst2_0_st3_1","dXst2_0_st3_1",100, -3, 3);
        hdXst2_0_st3_2= new TH1D("dXst2_0_st3_2","dXst2_0_st3_2",100, -3, 3);
        hdXst2_1_st3_1= new TH1D("dXst2_1_st3_1","dXst2_1_st3_1",100, -3, 3);
        hdXst2_1_st3_2= new TH1D("dXst2_1_st3_2","dXst2_1_st3_2",100, -3, 3);
        fList.Add(hdXst1_0_st2_0);
        fList.Add(hdXst1_1_st2_0);
        fList.Add(hdXst1_2_st2_0);
        fList.Add(hdXst1_3_st2_0);
        fList.Add(hdXst1_0_st2_1);
        fList.Add(hdXst1_1_st2_1);
        fList.Add(hdXst1_2_st2_1);
        fList.Add(hdXst1_3_st2_1);
        fList.Add(hdXst2_0_st3_1);
        fList.Add(hdXst2_0_st3_2);
        fList.Add(hdXst2_1_st3_1);
        fList.Add(hdXst2_1_st3_2);

        hdXp3_mod1 = new TH1D("hdXp3_mod1","hdXp3_mod1",100, -0.02, 0.02);fList.Add(hdXp3_mod1);
        hdXp3_mod2 = new TH1D("hdXp3_mod2","hdXp3_mod2",100, -0.02, 0.02);fList.Add(hdXp3_mod2);
        hdXXp3_mod1 = new TH1D("hdXXp3_mod1","hdXXp3_mod1",100, -0.5, 0.5);fList.Add(hdXXp3_mod1);
        hdXXp3_mod2 = new TH1D("hdXXp3_mod2","hdXXp3_mod2",100, -0.5, 0.5);fList.Add(hdXXp3_mod2);
        hXXp12CheckLeftover   = new TH1D("hXXp12CheckLeftover","hXXp12CheckLeftover",200, -1, 1);fList.Add(hXXp12CheckLeftover);
        hXXp12CheckLeftover03 = new TH1D("hXXp12CheckLeftover03","hXXp12CheckLeftover03",900, -0.15, 0.15);fList.Add(hXXp12CheckLeftover03);

        hdXvsXst1_0 = new TH2D("dXvsXst1_0","dXvsXst1_0", 100, -6, 6, 100, -0.05, 0.05);fList.Add(hdXvsXst1_0);
        hdXvsXst1_1 = new TH2D("dXvsXst1_1","dXvsXst1_1", 100, -6, 6, 100, -0.05, 0.05);fList.Add(hdXvsXst1_1);
        hdXvsXst1_2 = new TH2D("dXvsXst1_2","dXvsXst1_2", 100, -6, 6, 100, -0.05, 0.05);fList.Add(hdXvsXst1_2);
        hdXvsXst1_3 = new TH2D("dXvsXst1_3","dXvsXst1_3", 100, -6, 6, 100, -0.05, 0.05);fList.Add(hdXvsXst1_3);
        hdXvsXst2_0 = new TH2D("dXvsXst2_0","dXvsXst2_0", 100, -6, 6, 100, -0.05, 0.05);fList.Add(hdXvsXst2_0);
        hdXvsXst2_1 = new TH2D("dXvsXst2_1","dXvsXst2_1", 100, -6, 6, 100, -0.05, 0.05);fList.Add(hdXvsXst2_1);
        hdXvsXst3_1 = new TH2D("dXvsXst3_1","dXvsXst3_1", 100, -6, 6, 100, -0.001, 0.001);fList.Add(hdXvsXst3_1);
        hdXvsXst3_2 = new TH2D("dXvsXst3_2","dXvsXst3_2", 100, -6, 6, 100, -0.001, 0.001);fList.Add(hdXvsXst3_2);

        hdYvsYst1_mod0 = new TH2D("dYvsYst1_mod0","dYvsYst1_mod0", 100, -6, 6, 100, -0.6, 0.6);fList.Add(hdYvsYst1_mod0);
        hdYvsYst1_mod1 = new TH2D("dYvsYst1_mod1","dYvsYst1_mod1", 100, -6, 6, 100, -0.6, 0.6);fList.Add(hdYvsYst1_mod1);
        hdYvsYst1_mod2 = new TH2D("dYvsYst1_mod2","dYvsYst1_mod2", 100, -6, 6, 100, -0.6, 0.6);fList.Add(hdYvsYst1_mod2);
        hdYvsYst1_mod3 = new TH2D("dYvsYst1_mod3","dYvsYst1_mod3", 100, -6, 6, 100, -0.6, 0.6);fList.Add(hdYvsYst1_mod3);
        hdYvsYst2_mod0 = new TH2D("dYvsYst2_mod0","dYvsYst2_mod0", 100, -6, 6, 100, -0.6, 0.6);fList.Add(hdYvsYst2_mod0);
        hdYvsYst2_mod1 = new TH2D("dYvsYst2_mod1","dYvsYst2_mod1", 100, -6, 6, 100, -0.6, 0.6);fList.Add(hdYvsYst2_mod1);
        hdYvsYst3_mod1 = new TH2D("dYvsYst3_mod1","dYvsYst3_mod1", 100, -6, 6, 100, -0.2, 0.2);fList.Add(hdYvsYst3_mod1);
        hdYvsYst3_mod2 = new TH2D("dYvsYst3_mod2","dYvsYst3_mod2", 100, -6, 6, 100, -0.2, 0.2);fList.Add(hdYvsYst3_mod2);

        hdYst1_0_st2_0= new TH1D("dYst1_0_st2_0","dYst1_0_st2_0",100, -1, 1);
        hdYst1_0_st2_1= new TH1D("dYst1_0_st2_1","dYst1_0_st2_1",100, -1, 1);
        hdYst1_1_st2_0= new TH1D("dYst1_1_st2_0","dYst1_1_st2_0",100, -1, 1);
        hdYst1_1_st2_1= new TH1D("dYst1_1_st2_1","dYst1_1_st2_1",100, -1, 1);
        hdYst1_2_st2_0= new TH1D("dYst1_2_st2_0","dYst1_2_st2_0",100, -1, 1);
        hdYst1_2_st2_1= new TH1D("dYst1_2_st2_1","dYst1_2_st2_1",100, -1, 1);
        hdYst1_3_st2_0= new TH1D("dYst1_3_st2_0","dYst1_3_st2_0",100, -1, 1);
        hdYst1_3_st2_1= new TH1D("dYst1_3_st2_1","dYst1_3_st2_1",100, -1, 1);
        hdYst2_0_st3_1= new TH1D("dYst2_0_st3_1","dYst2_0_st3_1",100, -3, 3);
        hdYst2_0_st3_2= new TH1D("dYst2_0_st3_2","dYst2_0_st3_2",100, -3, 3);
        hdYst2_1_st3_1= new TH1D("dYst2_1_st3_1","dYst2_1_st3_1",100, -3, 3);
        hdYst2_1_st3_2= new TH1D("dYst2_1_st3_2","dYst2_1_st3_2",100, -3, 3);
        fList.Add(hdYst1_0_st2_0);
        fList.Add(hdYst1_1_st2_0);
        fList.Add(hdYst1_2_st2_0);
        fList.Add(hdYst1_3_st2_0);
        fList.Add(hdYst1_0_st2_1);
        fList.Add(hdYst1_1_st2_1);
        fList.Add(hdYst1_2_st2_1);
        fList.Add(hdYst1_3_st2_1);
        fList.Add(hdYst2_0_st3_1);
        fList.Add(hdYst2_0_st3_2);
        fList.Add(hdYst2_1_st3_1);
        fList.Add(hdYst2_1_st3_2);

        hY_XSisp1=  new TH2D("hY_XSisp1","hY_XSisp1", 100, -10, 10, 100, -10, 10);fList.Add(hY_XSisp1);
        hY_XSisp2=  new TH2D("hY_XSisp2","hY_XSisp2", 100, -10, 10, 100, -10, 10);fList.Add(hY_XSisp2);
        hY_XSisp3=  new TH2D("hY_XSisp3","hY_XSisp3", 100, -10, 10, 100, -20,  0);fList.Add(hY_XSisp3);

        hY_XSisp1bef=  new TH2D("hY_XSisp1bef","hY_XSisp1bef", 100, -10, 10, 100, -10, 10);fList.Add(hY_XSisp1bef);
        hY_XSisp2bef=  new TH2D("hY_XSisp2bef","hY_XSisp2bef", 100, -10, 10, 100, -10, 10);fList.Add(hY_XSisp2bef);

        N_eff = new TH1D("N_eff","N_eff", 6,1,7);fList.Add(N_eff);
        D_eff = new TH1D("D_eff","D_eff", 6,1,7);fList.Add(D_eff);
        E_eff = new TH1D("E_eff","Hit Efficiency per Layer on Track", 6,1,7);fList.Add(E_eff);
        E_eff->Sumw2();

        hdAx_MC_tr = new TH1D("dAx_MC_tr","dAx_MC_tr;[rad]",200, -.001,.001);fList.Add(hdAx_MC_tr);
        hdAy_MC_tr = new TH1D("dAy_MC_tr","dAy_MC_tr;[rad]",200, -.01,.01);fList.Add(hdAy_MC_tr);
        hdX_MC_tr  = new TH1D("dX_MC_tr","dX_MC_tr;[cm]",200, -2,2);fList.Add(hdX_MC_tr);
        hdY_MC_tr  = new TH1D("dY_MC_tr","dY_MC_tr;[cm]",200, -2,2);fList.Add(hdY_MC_tr);

        hAxsi_mctrue  = new TH1D("Axsi_mctrue","Ax_mctrueX; tgX[rad]; Events", 200, -.1,.1);fList.Add(hAxsi_mctrue);
        hBxsi_mctrue  = new TH1D("Bxsi_mctrue","Bx_mctrue; [cm]; Events", 200, -10,10);fList.Add(hBxsi_mctrue);
        hAysi_mctrue  = new TH1D("Aysi_mctrue","Ay_mctrue; tgY[rad]; Events", 200, -.1,.1);fList.Add(hAysi_mctrue);
        hBysi_mctrue  = new TH1D("Bysi_mctrue","By_mctrue; [cm]; Events", 200, -10,10);fList.Add(hBysi_mctrue);

        hSi_st1mc = new TH2D("Si_st1mc", "beam_profileSi_st1mc smeared; [cm];[cm]", 100, -10, 10, 100, -15, 5); fList.Add(hSi_st1mc);
        hSi_st2mc = new TH2D("Si_st2mc", "beam_profileSi_st2mc smeared; [cm];[cm]", 100, -10, 10, 100, -15, 5); fList.Add(hSi_st2mc);
        hSi_st3mc = new TH2D("Si_st3mc", "beam_profileSi_st3mc smeared; [cm];[cm]", 100, -15, 15, 100, -25, 5); fList.Add(hSi_st3mc);

        hdAx_MC_tr_comb = new TH1D("dAx_MC_tr_comb","dAx_MC_tr_comb;[rad]",200, -.005,.005);fList.Add(hdAx_MC_tr_comb);
        hdAy_MC_tr_comb = new TH1D("dAy_MC_tr_comb","dAy_MC_tr_comb;[rad]",200, -.005,.005);fList.Add(hdAy_MC_tr_comb);
        hdX_MC_tr_comb  = new TH1D("dX_MC_tr_comb","dX_MC_tr_comb;[cm]",200, -2,2);fList.Add(hdX_MC_tr_comb);
        hdY_MC_tr_comb  = new TH1D("dY_MC_tr_comb","dY_MC_tr_comb;[cm]",200, -2,2);fList.Add(hdY_MC_tr_comb);

        hDen_mctrSi = new TH1D("hDen_mctrSi", "Den_mctrSi", 1, 0, 1);fList.Add(hDen_mctrSi);
        hNum_mctrSi = new TH1D("hNum_mctrSi", "Num_mctrSi", 1, 0, 1);fList.Add(hNum_mctrSi);
        hEff_mctrSi = new TH1D("hEff_mctrSi", "Eff_mctrSi", 1, 0, 1);fList.Add(hEff_mctrSi);

        hDen_mcreaction= new TH1D("hDen_mcreaction", "hDen_mcreaction", 1, 0, 1);fList.Add(hDen_mcreaction);
        hNum_mcreaction= new TH1D("hNum_mcreaction", "hNum_mcreaction", 1, 0, 1);fList.Add(hNum_mcreaction);
        hEff_mcreaction= new TH1D("hEff_mcreaction", "hEff_mcreaction", 1, 0, 1);fList.Add(hEff_mcreaction);

        hNtrsi_mc = new TH1D("Ntrsi_mc",";N of Si-tr mc", 10, 0,10); fList.Add(hNtrsi_mc);
        hNtrsi_reco = new TH1D("Ntrsi_reco",";N of Si-tr reco", 10, 0,10); fList.Add(hNtrsi_reco);
        hNtrsi_mc_vs_reco= new TH2D("hNtr_mc_vs_reco","hNtr_mc_vs_reco", 10, 0,10, 10, 0,10); fList.Add(hNtrsi_mc_vs_reco);


        TH1D *h;
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (imod > 3) continue;
            h = new TH1D(Form("hoccupancyX1_mod%d",imod), Form("hoccupancyX1_mod%d",imod), fNstrips+1, 0.,fNstrips+1);fList.Add(h); hoccupancyX1.push_back(h);
        }
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (imod > 1) continue;
            h = new TH1D(Form("hoccupancyX2_mod%d",imod), Form("hoccupancyX2_mod%d",imod), fNstrips+1, 0.,fNstrips+1);fList.Add(h); hoccupancyX2.push_back(h);
        }
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            h = new TH1D(Form("hoccupancyX3_mod%d",imod), Form("hoccupancyX3_mod%d",imod), fNstrips+1, 0.,fNstrips+1);fList.Add(h); hoccupancyX3.push_back(h);
        }

        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (imod > 3) continue;
            h = new TH1D(Form("hoccupancyXp1_mod%d",imod), Form("hoccupancyXp1_mod%d",imod), fNstrips+1, 0.,fNstrips+1);fList.Add(h); hoccupancyXp1.push_back(h);
        }
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (imod > 1) continue;
            h = new TH1D(Form("hoccupancyXp2_mod%d",imod), Form("hoccupancyXp2_mod%d",imod), fNstrips+1, 0.,fNstrips+1);fList.Add(h); hoccupancyXp2.push_back(h);
        }
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            h = new TH1D(Form("hoccupancyXp3_mod%d",imod), Form("hoccupancyXp3_mod%d",imod), fNstrips+1, 0.,fNstrips+1);fList.Add(h); hoccupancyXp3.push_back(h);
        }

    }//if (fDebug)

    //--Some Arrays--
    AmatrInverted = new Double_t[16];
    rhs = new Double_t[4];
    AmatrArray = new Double_t[16];
    Amatr = new Double_t*[4];
    line  = new Double_t[4];
    for (Int_t i = 0; i < 4; i++) {
        Amatr[i] = new Double_t[4];
    }
    Ampl_strX      = new Double_t[fNstrips];
    Ampl_strXp     = new Double_t[fNstrips];
    Nsp_st         = new Int_t[fNstations];
    Nleftoversp    = new Int_t*[fNstations];
    NleftoverX     = new Int_t*[fNstations];
    NleftoverXp    = new Int_t*[fNstations];
    iClustXMod     = new Int_t[fNstations];
    iModX          = new Int_t[fNstations];
    shiftStXtoGlob = new Double_t[fNstations];
    shiftStYtoGlob = new Double_t[fNstations];
    shiftX         = new Double_t*[fNstations];
    shiftY         = new Double_t*[fNstations];
    shiftYbelow    = new Double_t*[fNstations];
    Zstation       = new Double_t*[fNstations];
    Zposition      = new Double_t[fNstations];
    NclustX        = new Int_t*[fNstations];
    NclustXp       = new Int_t*[fNstations];
    NhitsXYmod     = new Int_t*[fNstations];
    Xforglfit      = new Double_t*[fNstations];
    Xpforglfit     = new Double_t*[fNstations];
    Yforglfit      = new Double_t*[fNstations];
    SigmXforglfit  = new Double_t*[fNstations];
    SigmXpforglfit = new Double_t*[fNstations];
    SigmYforglfit  = new Double_t*[fNstations];
    ZstnCforglfit  = new Double_t*[fNstations];
    XCoord         = new Double_t**[fNstations];
    XpCoord        = new Double_t**[fNstations];
    XspCoord       = new Double_t**[fNstations];
    XpspCoord      = new Double_t**[fNstations];
    YspCoord       = new Double_t**[fNstations];
    Sp_pdg         = new Int_t**[fNstations];
    SigmaX         = new Double_t**[fNstations];
    SigmaXp        = new Double_t**[fNstations];
    SigmspX        = new Double_t**[fNstations];
    SigmspXp       = new Double_t**[fNstations];
    SigmspY        = new Double_t**[fNstations];
    DigitsArrayX   = new Double_t**[fNstations];
    DigitsArrayXp  = new Double_t**[fNstations];
    XClust         = new Double_t**[fNstations];
    XpClust        = new Double_t**[fNstations];
    XClust_width   = new Double_t**[fNstations];
    XpClust_width  = new Double_t**[fNstations];
    sumQx          = new Double_t**[fNstations];
    sumQxp         = new Double_t**[fNstations];
    XspClust       = new Double_t**[fNstations];
    XpspClust      = new Double_t**[fNstations];
    XspClust_width = new Double_t**[fNstations];
    XpspClust_width= new Double_t**[fNstations];
    sumQxsp        = new Double_t**[fNstations];
    sumQxpsp       = new Double_t**[fNstations];
    leftoverXsp    = new Double_t**[fNstations];
    leftoverXpsp   = new Double_t**[fNstations];
    leftoverYsp    = new Double_t**[fNstations];
    leftoverXsigsp = new Double_t**[fNstations];
    leftoverXpsigsp= new Double_t**[fNstations];
    leftoverYsigsp = new Double_t**[fNstations];
    leftoverX      = new Double_t**[fNstations];
    leftoverXp     = new Double_t**[fNstations];
    leftoverXsig   = new Double_t**[fNstations];
    leftoverXpsig  = new Double_t**[fNstations];

    for (Int_t istat = 0; istat < fNstations; istat++) {
        Nleftoversp[istat]    = new Int_t[fNmodules];
        NleftoverX[istat]     = new Int_t[fNmodules];
        NleftoverXp[istat]    = new Int_t[fNmodules];
        leftoverXsp[istat]    = new Double_t*[fNmodules];
        leftoverXpsp[istat]   = new Double_t*[fNmodules];
        leftoverYsp[istat]    = new Double_t*[fNmodules];
        leftoverXsigsp[istat] = new Double_t*[fNmodules];
        leftoverXpsigsp[istat]= new Double_t*[fNmodules];
        leftoverYsigsp[istat] = new Double_t*[fNmodules];
        leftoverX[istat]      = new Double_t*[fNmodules];
        leftoverXp[istat]     = new Double_t*[fNmodules];
        leftoverXsig[istat]   = new Double_t*[fNmodules];
        leftoverXpsig[istat]  = new Double_t*[fNmodules];
        shiftX[istat]         = new Double_t[fNmodules];
        shiftY[istat]         = new Double_t[fNmodules];
        shiftYbelow[istat]    = new Double_t[fNmodules];
        Zstation[istat]       = new Double_t[fNmodules];
        NclustX[istat]        = new Int_t[fNmodules];
        NclustXp[istat]       = new Int_t[fNmodules];
        NhitsXYmod[istat]     = new Int_t[fNmodules];
        SigmXforglfit[istat]  = new Double_t[fNmodules];
        SigmXpforglfit[istat] = new Double_t[fNmodules];
        SigmYforglfit[istat]  = new Double_t[fNmodules];
        ZstnCforglfit[istat]  = new Double_t[fNmodules];
        Xforglfit[istat]      = new Double_t[fNmodules];
        Xpforglfit[istat]     = new Double_t[fNmodules];
        Yforglfit[istat]      = new Double_t[fNmodules];
        XCoord[istat]         = new Double_t*[fNmodules];
        XpCoord[istat]        = new Double_t*[fNmodules];
        XspCoord[istat]       = new Double_t*[fNmodules];
        XpspCoord[istat]      = new Double_t*[fNmodules];
        YspCoord[istat]       = new Double_t*[fNmodules];
        Sp_pdg[istat]         = new Int_t*[fNmodules];
        SigmaX[istat]         = new Double_t*[fNmodules];
        SigmaXp[istat]        = new Double_t*[fNmodules];
        SigmspX[istat]        = new Double_t*[fNmodules];
        SigmspXp[istat]       = new Double_t*[fNmodules];
        SigmspY[istat]        = new Double_t*[fNmodules];
        DigitsArrayX[istat]   = new Double_t*[fNmodules];
        DigitsArrayXp[istat]  = new Double_t*[fNmodules];
        XClust[istat]         = new Double_t*[fNmodules];
        XpClust[istat]        = new Double_t*[fNmodules];
        XClust_width[istat]   = new Double_t*[fNmodules];
        XpClust_width[istat]  = new Double_t*[fNmodules];
        sumQx[istat]          = new Double_t*[fNmodules];
        sumQxp[istat]         = new Double_t*[fNmodules];
        XspClust[istat]       = new Double_t*[fNmodules];
        XpspClust[istat]      = new Double_t*[fNmodules];
        XspClust_width[istat] = new Double_t*[fNmodules];
        XpspClust_width[istat]= new Double_t*[fNmodules];
        sumQxsp[istat]        = new Double_t*[fNmodules];
        sumQxpsp[istat]       = new Double_t*[fNmodules];

        for (Int_t im = 0; im < fNmodules; im++) {
            leftoverXsp[istat][im]     = new Double_t[kBig];
            leftoverXpsp[istat][im]    = new Double_t[kBig];
            leftoverYsp[istat][im]     = new Double_t[kBig];
            leftoverXsigsp[istat][im]  = new Double_t[kBig];
            leftoverXpsigsp[istat][im] = new Double_t[kBig];
            leftoverYsigsp[istat][im]  = new Double_t[kBig];
            leftoverX[istat][im]       = new Double_t[kBig];
            leftoverXp[istat][im]      = new Double_t[kBig];
            leftoverXsig[istat][im]    = new Double_t[kBig];
            leftoverXpsig[istat][im]   = new Double_t[kBig];
            XCoord[istat][im]          = new Double_t[kBig];
            XpCoord[istat][im]         = new Double_t[kBig];
            XspCoord[istat][im]        = new Double_t[kBig];
            XpspCoord[istat][im]       = new Double_t[kBig];
            YspCoord[istat][im]        = new Double_t[kBig];
            Sp_pdg[istat][im]          = new Int_t[kBig];
            SigmaX[istat][im]          = new Double_t[kBig];
            SigmaXp[istat][im]         = new Double_t[kBig];
            SigmspX[istat][im]         = new Double_t[kBig];
            SigmspXp[istat][im]        = new Double_t[kBig];
            SigmspY[istat][im]         = new Double_t[kBig];
            DigitsArrayX[istat][im]    = new Double_t[fNstrips];
            DigitsArrayXp[istat][im]   = new Double_t[fNstrips];
            XClust[istat][im]          = new Double_t[fNstrips];
            XpClust[istat][im]         = new Double_t[fNstrips];
            XClust_width[istat][im]    = new Double_t[fNstrips];
            XpClust_width[istat][im]   = new Double_t[fNstrips];
            sumQx[istat][im]           = new Double_t[fNstrips];
            sumQxp[istat][im]          = new Double_t[fNstrips];
            XspClust[istat][im]        = new Double_t[fNstrips];
            XpspClust[istat][im]       = new Double_t[fNstrips];
            XspClust_width[istat][im]  = new Double_t[fNstrips];
            XpspClust_width[istat][im] = new Double_t[fNstrips];
            sumQxsp[istat][im]         = new Double_t[fNstrips];
            sumQxpsp[istat][im]        = new Double_t[fNstrips];
        }
    }

    if (fVerbose) cout << "BmnSiliconTrackFinder::Init()" << endl;
    FairRootManager* ioman = FairRootManager::Instance();

    if (expData) {
        if (fDebug) cout<<" expData "<<endl;
        fBmnSiDigitsArray = (TClonesArray*) ioman->GetObject("SILICON");
        if (!fBmnSiDigitsArray) {
            if (fDebug) cout<<"fBmnSiDigitsArray::Init(): branch "<<" not found! Task will be deactivated"<<endl;
            SetActive(kFALSE);
            return kERROR;
        }
    }else{
        if (fDebug) cout<<" !expData BmnSiliconTrackFinder::Init()" << endl;
        fBmnHitsArray = (TClonesArray*)ioman->GetObject(fInputBranchName);
        if (fDebug) cout << "fInputBranchName = " << fInputBranchName << "\n";
        if (!fBmnHitsArray) {
            cout << "BmnSiliconTrackFinder::Init(): branch " <<fInputBranchName<< " not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
        fBmnHitsArray2 = (TClonesArray*)ioman->GetObject(fInputBranchName2);
        if (fDebug) cout << "fInputBranchName2 = " << fInputBranchName2 << "\n";
        if (!fBmnHitsArray2) {
            cout << "BmnSiliconTrackFinder::Init(): branch " <<fInputBranchName2<< " not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
    }

    kX_target = 0;
    kY_target = 0;
    half_target_regionX = 3.;
    half_target_regionY = 5.5; // !

    //--cm--
    if(expData) {
        //cut for cluster charge
        Cut_AmplX       = 120.;
        Cut_AmplXp      = 120.;
        Cut_overflow    = 1800.;
        //cut for strip charge
        Cut_AmplStripX  = 120.;
        Cut_AmplStripXp = 120.;
        Chi2_Globcut    = 80.;//20.;
        N_min_points    = 4;

        if (fRunPeriod == 7){
            kX_target = 0.5;
            kY_target =-4.5;
            Z0_SRC_target = -645.191;
            Zcentr        = -392.524; // Zc = Zi/3
            Shift_toCenterOfMagnetX  =  0.54;
            Shift_toCenterOfMagnetY  = -3.43;
            Shift_toCenterOfMagnetAX =  0.;
            Shift_toCenterOfMagnetAY =  0.0024;
            //--X--
            shiftX[1][0] = -6.4 + 0.1148 + 0.0026; shiftX[2][0] = -5.9 + .1148 + 0.0075; shiftX[3][0] = -12.15 + 0.1148; shiftX[3][4] = -12.15 + 0.1148;
            shiftX[1][1] =  5.9 - 0.1148 + 0.0085; shiftX[2][1] =  6.4 - .1148 + 0.009;  shiftX[3][1] = -6.15  + 0.1148; shiftX[3][5] = -6.15  + 0.1148;
            shiftX[1][2] = -5.9 + 0.1148 + 0.011;                                        shiftX[3][2] =  6.15  - 0.1148; shiftX[3][6] =  6.15  - 0.1148;
            shiftX[1][3] =  6.4 - 0.1148 + 0.019;                                        shiftX[3][3] =  12.15 - 0.1148; shiftX[3][7] =  12.15 - 0.1148;

            //--Y--
            shiftY[1][0] = -0.4 -0.08; shiftY[2][0] = -6.22 -0.02 ; shiftY[3][0] = -0.15;        shiftY[3][4] =  0.15;
            shiftY[1][1] =  0.1 -0.06; shiftY[2][1] = -6.72       ; shiftY[3][1] = -0.15 -0.25;  shiftY[3][5] =  0.15;
            shiftY[1][2] = -0.2 -0.05;                              shiftY[3][2] = -0.15 -0.4;   shiftY[3][6] =  0.15;
            shiftY[1][3] =  0.4 -0.16;                              shiftY[3][3] = -0.15;        shiftY[3][7] =  0.15;

            Zstation[1][0] = 1.44 + 0.5 * 0.003; Zstation[2][0] = 4.71 + 0.5 * 0.003; Zstation[3][0] = 3.51 + 0.5 * 0.003;       Zstation[3][4] = 2.25 + 0.5 * 0.003;
            Zstation[1][1] = 2.17 + 0.5 * 0.003; Zstation[2][1] = 5.44 + 0.5 * 0.003; Zstation[3][1] = 1.98 + 0.5 * 0.003 - .08; Zstation[3][5] = 3.78 + 0.5 * 0.003;
            Zstation[1][2] = 1.44 + 0.5 * 0.003;                                      Zstation[3][2] = 3.29 + 0.5 * 0.003 + .08; Zstation[3][6] = 2.47 + 0.5 * 0.003;
            Zstation[1][3] = 2.17 + 0.5 * 0.003;                                      Zstation[3][3] = 1.76 + 0.5 * 0.003;       Zstation[3][7] = 4.00 + 0.5 * 0.003;

            //--cm-- Shift
            shiftStXtoGlob[0] = 0.;     shiftStYtoGlob[0] =  0.;           Zposition[0] = 0.;
            shiftStXtoGlob[1] = 0.149 ; shiftStYtoGlob[1] =  0.096 + .191; Zposition[1] = 213.999 -6.87;
            shiftStXtoGlob[2] = 0.149 ; shiftStYtoGlob[2] =  0.096 + .151; Zposition[2] = 213.999 -6.87;
            shiftStXtoGlob[3] = 0.047 ; shiftStYtoGlob[3] = -6.274 + .174; Zposition[3] = 327.130 + 0.03;

        }
        if (fRunPeriod == 8){
            kX_target = 0.;//??
            kY_target = 0.;//??
            Z0_SRC_target = -574.91;
            Zcentr        = -423.23; // Zc = Zi/4

            //--X--
            shiftX[1][0] = -6.150 + 0.1148; shiftX[2][0] = -6.130 + 0.1148;  shiftX[3][0] = -6.160 + 0.1148; shiftX[4][0] = -6.158 + 0.1148;
            shiftX[1][1] =  6.470 - 0.1148; shiftX[2][1] =  6.170 - 0.1148;  shiftX[3][1] =  6.122 - 0.1148; shiftX[4][1] =  6.129 - 0.1148;

            //--Y--
            //up parts
            shiftY[1][0] = 6.203;       shiftY[2][0] = 6.218;     shiftY[3][0] = 6.206;      shiftY[4][0] =  6.209;
            shiftY[1][1] = 6.193;       shiftY[2][1] = 6.199;     shiftY[3][1] = 6.197;      shiftY[4][1] =  6.213;
            //below parts
            shiftYbelow[1][0] =-0.098;  shiftYbelow[2][0] =-0.084; shiftYbelow[3][0] =-0.094; shiftYbelow[4][0] =-0.091;
            shiftYbelow[1][1] =-0.104;  shiftYbelow[2][1] =-0.097; shiftYbelow[3][1] =-0.102; shiftYbelow[4][0] =-0.089;

            Zstation[1][0] =-1.14 -1.3;  Zstation[2][0] = 1.145 + 1.3; Zstation[3][0] =-1.095 - 1.3; Zstation[4][0] = 1.140 + 1.3;
            Zstation[1][1] =-1.85 -1.3;  Zstation[2][1] = 1.870 + 1.3; Zstation[3][1] =-1.850 - 1.3; Zstation[4][1] = 1.875 + 1.3;

            //--cm-- Shift
            shiftStXtoGlob[1] = 0.193; shiftStYtoGlob[1] = 0.153; Zposition[1] = 124.779;
            shiftStXtoGlob[2] = 0.193; shiftStYtoGlob[2] = 0.153; Zposition[2] = 126.679;
            shiftStXtoGlob[3] = 0.193; shiftStYtoGlob[3] = 0.153; Zposition[3] = 176.679;
            shiftStXtoGlob[4] = 0.193; shiftStYtoGlob[4] = 0.153; Zposition[4] = 178.679;

            Shift_toCenterOfMagnetX  =  0.;
            Shift_toCenterOfMagnetY  =  0.;
            Shift_toCenterOfMagnetAX =  0.;
            Shift_toCenterOfMagnetAY =  0.;

        }
    }
    if(!expData){

        kX_target = 0.5;
        kY_target =-4.5;
        //cut for cluster charge
        Cut_AmplX       = 0.;
        Cut_AmplXp      = 0.;
        Cut_overflow    = 1800.;
        //cut for strip charge
        Cut_AmplStripX  = 0.;
        Cut_AmplStripXp = 0.;
        Chi2_Globcut    = 80.;//20.;
        N_min_points    = 4;

        Shift_toCenterOfMagnetX  =  0.;
        Shift_toCenterOfMagnetY  =  0.;
        Shift_toCenterOfMagnetAX =  0.;
        Shift_toCenterOfMagnetAY =  0.;

        //--X--
        shiftX[1][0] = -6.4 + 0.1148; shiftX[2][0] = -5.9 + .1148; shiftX[3][0] = -12.15 + 0.1148; shiftX[3][4] = -12.15 + 0.1148;
        shiftX[1][1] =  5.9 - 0.1148; shiftX[2][1] =  6.4 - .1148; shiftX[3][1] = -6.15  + 0.1148; shiftX[3][5] = -6.15  + 0.1148;
        shiftX[1][2] = -5.9 + 0.1148;                              shiftX[3][2] =  6.15  - 0.1148; shiftX[3][6] =  6.15  - 0.1148;
        shiftX[1][3] =  6.4 - 0.1148;                              shiftX[3][3] =  12.15 - 0.1148; shiftX[3][7] =  12.15 - 0.1148;

        //--Y--
        shiftY[1][0] = -0.4 ; shiftY[2][0] = -6.22; shiftY[3][0] = -0.15; shiftY[3][4] =  0.15;
        shiftY[1][1] =  0.1 ; shiftY[2][1] = -6.72; shiftY[3][1] = -0.15; shiftY[3][5] =  0.15;
        shiftY[1][2] = -0.2 ;                       shiftY[3][2] = -0.15; shiftY[3][6] =  0.15;
        shiftY[1][3] =  0.4 ;                       shiftY[3][3] = -0.15; shiftY[3][7] =  0.15;

        shiftStXtoGlob[0] = 0.; shiftStYtoGlob[0] = 0.;
        shiftStXtoGlob[1] = 0.; shiftStYtoGlob[1] = 0.;
        shiftStXtoGlob[2] = 0.; shiftStYtoGlob[2] = 0.;
        shiftStXtoGlob[3] = 0.; shiftStYtoGlob[3] = 0.;

        Zposition[0] = 0.;
        Zposition[1] = 213.999 -6.9 + 0.03;
        Zposition[2] = 213.999 -6.9 + 0.03;
        Zposition[3] = 327.130 + 0.03;

        Zstation[1][0] = 1.44 + 0.5 * 0.003; Zstation[2][0] = 4.71 + 0.5 * 0.003; Zstation[3][0] = 3.51 + 0.5 * 0.003;       Zstation[3][4] = 2.25 + 0.5 * 0.003;
        Zstation[1][1] = 2.17 + 0.5 * 0.003; Zstation[2][1] = 5.44 + 0.5 * 0.003; Zstation[3][1] = 1.98 + 0.5 * 0.003 - .08; Zstation[3][5] = 3.78 + 0.5 * 0.003;
        Zstation[1][2] = 1.44 + 0.5 * 0.003;                                      Zstation[3][2] = 3.29 + 0.5 * 0.003 + .08; Zstation[3][6] = 2.47 + 0.5 * 0.003;
        Zstation[1][3] = 2.17 + 0.5 * 0.003;                                      Zstation[3][3] = 1.76 + 0.5 * 0.003;       Zstation[3][7] = 4.00 + 0.5 * 0.003;

    }//if(!expData)

    for (Int_t is = 1; is < fNstations; is++) {
        for (int im = 0; im < fNmodules; im++) {
            if ( fRunPeriod == 7 && is == 1 && im > 3) continue;
            if ( fRunPeriod == 7 && is == 2 && im > 1) continue;
            Zstation[is][im] += Zposition[is];
            Zstation[is][im] += Z0_SRC_target; //coordinate system reference bm@n
            //if (fDebug) cout<<" is "<<is<<" im "<<im<<" Z "<<Zstation[is][im]<<endl;

            ZstnCforglfit[is][im] = Zstation[is][im] - Zcentr;

        }
    }

    for (Int_t ist = 1; ist < fNstations; ist++) {
        //if ( fDebug ) cout<<" shiftStXtoGlob["<<ist<<"]= "<<shiftStXtoGlob[ist]<<" Y "<< shiftStYtoGlob[ist]<<endl;
        for (Int_t imod = 0; imod < fNmodules; imod++) {
            if (ist == 1 && imod > 3 ) continue;
            if (ist == 2 && imod > 1 ) continue;
            // if ( fDebug ) cout<<" shiftX["<<ist<<"]["<<imod<<"]= "<<shiftX[ist][imod]<<" shiftY["<<ist<<"]["<<imod<<"]= "<<shiftY[ist][imod]<<endl;
        }
    }

    //--------------------------------------------------------------------
    // Create and register hits arrays
    fBmnSiliconHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register("BmnSiliconHits", "SI", fBmnSiliconHitsArray, kTRUE);
    //--XHits--
    fBmnSiliconHitsXArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register("BmnSiliconHitsX", "SI", fBmnSiliconHitsXArray, kTRUE);
    //--XpHits--
    fBmnSiliconHitsXpArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register("BmnSiliconHitsXp", "SI", fBmnSiliconHitsXpArray, kTRUE);
    //--XYHits--
    fBmnSiliconHitsXYArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register("BmnSiliconHitsXY", "SI", fBmnSiliconHitsXYArray, kTRUE);

    // Create and register track arrays
    fSiTracks = new TClonesArray("BmnSiliconTrack");
    ioman->Register("BmnSiliconTrack", "SI", fSiTracks, kTRUE);

    return kSUCCESS;
}
//----------------------------------------------------------------------


//---------------------Initialisation-----------------------------------
void BmnSiliconTrackFinder::PrepareArraysToProcessEvent() {
    fSiTracks->Clear();
    fBmnSiliconHitsArray->Delete();
    fBmnSiliconHitsXArray->Delete();
    fBmnSiliconHitsXpArray->Delete();
    fBmnSiliconHitsXYArray->Delete();
    CleanTr.clear();
    vec_tracks.clear();
    vec_points.clear();

    for (Int_t istat = 0; istat < fNstations; istat++) {
        Nsp_st[istat] = 0;

        for (Int_t im = 0; im < fNmodules; im++) {

            Nleftoversp[istat][im]    = 0;
            NleftoverX[istat][im]     = 0;
            NleftoverXp[istat][im]    = 0;
            NclustX[istat][im]        = 0;
            NclustXp[istat][im]       = 0;
            NhitsXYmod[istat][im]     = 0;
            Xforglfit[istat][im]      = -999.;
            Xpforglfit[istat][im]     = -999.;
            Yforglfit[istat][im]      = -999.;
            SigmXforglfit[istat][im]  = -999.;
            SigmXpforglfit[istat][im] = -999.;
            SigmYforglfit[istat][im]  = -999.;

            for (Int_t ii = 0; ii < fNstrips; ii++) {
                Ampl_strX[ii]  = 0.;
                Ampl_strXp[ii] = 0.;
                DigitsArrayX[istat][im][ii] = 0.;
                DigitsArrayXp[istat][im][ii] = 0.;
            }

            for (Int_t ii = 0; ii < kBig; ii++) {
                leftoverXsp[istat][im][ii]     = -999.;
                leftoverXpsp[istat][im][ii]    = -999.;
                leftoverYsp[istat][im][ii]     = -999.;
                leftoverXsigsp[istat][im][ii]  = -999.;
                leftoverXpsigsp[istat][im][ii] = -999.;
                leftoverYsigsp[istat][im][ii]  = -999.;
                leftoverX[istat][im][ii]       = -999.;
                leftoverXp[istat][im][ii]      = -999.;
                leftoverXsig[istat][im][ii]    = -999.;
                leftoverXpsig[istat][im][ii]   = -999.;
                XCoord[istat][im][ii]          = -999.;
                XpCoord[istat][im][ii]         = -999.;
                XspCoord[istat][im][ii]        = -999.;
                XpspCoord[istat][im][ii]       = -999.;
                YspCoord[istat][im][ii]        = -999.;
                Sp_pdg[istat][im][ii]          = -1;
                SigmaX[istat][im][ii]          = -999.;
                SigmaXp[istat][im][ii]         = -999.;
                SigmspX[istat][im][ii]         = -999.;
                SigmspXp[istat][im][ii]        = -999.;
                SigmspY[istat][im][ii]         = -999.;
                XClust[istat][im][ii]          = 0.;
                XpClust[istat][im][ii]         = 0.;
                XClust_width[istat][im][ii]    = 0.;
                XpClust_width[istat][im][ii]   = 0.;
                sumQx[istat][im][ii]           = 0.;
                sumQxp[istat][im][ii]          = 0.;
                XspClust[istat][im][ii]        = 0.;
                XpspClust[istat][im][ii]       = 0.;
                XspClust_width[istat][im][ii]  = 0.;
                XpspClust_width[istat][im][ii] = 0.;
                sumQxsp[istat][im][ii]         = 0.;
                sumQxpsp[istat][im][ii]        = 0.;
            }
        }
    }

}
//----------------------------------------------------------------------

//-------------------------empty constructor----------------------------
BmnSiliconTrackFinder::BmnSiliconTrackFinder()
{
    fOutputHitsBranchName = "BmnSiliconHit";
}

//-------------------------constructor----------------------------------
BmnSiliconTrackFinder::BmnSiliconTrackFinder(Bool_t isExp, Int_t RunPeriod, Int_t runNumber) {

    fRunNumber = runNumber;
    expData    = isExp;
    fRunPeriod = RunPeriod;
    fOutputHitsBranchName = "BmnSiliconHit";
}
//----------------------------------------------------------------------


//-------------------Destructor-----------------------------------------
BmnSiliconTrackFinder::~BmnSiliconTrackFinder() {

    for (Int_t istat = 0; istat < fNstations; istat++) {

        for (Int_t im = 0; im < fNmodules; im++) {
            delete[] XCoord[istat][im];
            delete[] XpCoord[istat][im];
            delete[] XspCoord[istat][im];
            delete[] XpspCoord[istat][im];
            delete[] YspCoord[istat][im];
            delete[] Sp_pdg[istat][im];
            delete[] SigmaX[istat][im];
            delete[] SigmaXp[istat][im];
            delete[] SigmspX[istat][im];
            delete[] SigmspXp[istat][im];
            delete[] SigmspY[istat][im];
            delete[] leftoverXsp[istat][im];
            delete[] leftoverXpsp[istat][im];
            delete[] leftoverYsp[istat][im];
            delete[] leftoverXsigsp[istat][im];
            delete[] leftoverXpsigsp[istat][im];
            delete[] leftoverYsigsp[istat][im];
            delete[] leftoverX[istat][im];
            delete[] leftoverXp[istat][im];
            delete[] leftoverXsig[istat][im];
            delete[] leftoverXpsig[istat][im];
            delete[] DigitsArrayX[istat][im];
            delete[] DigitsArrayXp[istat][im];
            delete[] XClust[istat][im];
            delete[] XpClust[istat][im];
            delete[] XClust_width[istat][im];
            delete[] XpClust_width[istat][im];
            delete[] sumQx[istat][im];
            delete[] sumQxp[istat][im];
            delete[] XspClust[istat][im];
            delete[] XpspClust[istat][im];
            delete[] XspClust_width[istat][im];
            delete[] XpspClust_width[istat][im];
            delete[] sumQxsp[istat][im];
            delete[] sumQxpsp[istat][im];
        }

        delete[] NclustX[istat];
        delete[] NclustXp[istat];
        delete[] NhitsXYmod[istat];
        delete[] XCoord[istat];
        delete[] XpCoord[istat];
        delete[] XspCoord[istat];
        delete[] XpspCoord[istat];
        delete[] YspCoord[istat];
        delete[] Sp_pdg[istat];
        delete[] SigmaX[istat];
        delete[] SigmaXp[istat];
        delete[] SigmspX[istat];
        delete[] SigmspXp[istat];
        delete[] SigmspY[istat];
        delete[] Xforglfit[istat];
        delete[] Xpforglfit[istat];
        delete[] Yforglfit[istat];
        delete[] SigmXforglfit[istat];
        delete[] SigmXpforglfit[istat];
        delete[] SigmYforglfit[istat];
        delete[] ZstnCforglfit[istat];
        delete[] shiftX[istat];
        delete[] shiftY[istat];
        delete[] shiftYbelow[istat];
        delete[] Zstation[istat];
        delete[] Nleftoversp[istat];
        delete[] NleftoverX[istat];
        delete[] NleftoverXp[istat];
        delete[] leftoverXsp[istat];
        delete[] leftoverXpsp[istat];
        delete[] leftoverYsp[istat];
        delete[] leftoverXsigsp[istat];
        delete[] leftoverXpsigsp[istat];
        delete[] leftoverYsigsp[istat];
        delete[] leftoverX[istat];
        delete[] leftoverXp[istat];
        delete[] leftoverXsig[istat];
        delete[] leftoverXpsig[istat];

        delete[] DigitsArrayX[istat];
        delete[] DigitsArrayXp[istat];
        delete[] XClust[istat];
        delete[] XpClust[istat];
        delete[] XClust_width[istat];
        delete[] XpClust_width[istat];
        delete[] sumQx[istat];
        delete[] sumQxp[istat];
        delete[] XspClust[istat];
        delete[] XpspClust[istat];
        delete[] XspClust_width[istat];
        delete[] XpspClust_width[istat];
        delete[] sumQxsp[istat];
        delete[] sumQxpsp[istat];
    }

    delete[] NclustX;
    delete[] NclustXp;
    delete[] NhitsXYmod;
    delete[] XCoord;
    delete[] XpCoord;
    delete[] XspCoord;
    delete[] XpspCoord;
    delete[] YspCoord;
    delete[] Sp_pdg;
    delete[] SigmaX;
    delete[] SigmaXp;
    delete[] SigmspX;
    delete[] SigmspXp;
    delete[] SigmspY;
    delete[] Xforglfit;
    delete[] Xpforglfit;
    delete[] Yforglfit;
    delete[] SigmXforglfit;
    delete[] SigmXpforglfit;
    delete[] SigmYforglfit;
    delete[] ZstnCforglfit;
    delete[] shiftStXtoGlob;
    delete[] shiftStYtoGlob;
    delete[] shiftX;
    delete[] shiftY;
    delete[] shiftYbelow;
    delete[] Zstation;
    delete[] Zposition;
    delete[] Nleftoversp;
    delete[] NleftoverX;
    delete[] NleftoverXp;
    delete[] leftoverXsp;
    delete[] leftoverXpsp;
    delete[] leftoverYsp;
    delete[] leftoverXsigsp;
    delete[] leftoverXpsigsp;
    delete[] leftoverYsigsp;
    delete[] leftoverX;
    delete[] leftoverXp;
    delete[] leftoverXsig;
    delete[] leftoverXpsig;
    delete[] Nsp_st;
    delete[] iClustXMod;
    delete[] iModX;
    delete[] Ampl_strX;
    delete[] Ampl_strXp;
    delete[] DigitsArrayX;
    delete[] DigitsArrayXp;
    delete[] XClust;
    delete[] XpClust;
    delete[] XClust_width;
    delete[] XpClust_width;
    delete[] sumQx;
    delete[] sumQxp;
    delete[] XspClust;
    delete[] XpspClust;
    delete[] XspClust_width;
    delete[] XpspClust_width;
    delete[] sumQxsp;
    delete[] sumQxpsp;

    for (Int_t i = 0; i < 4; i++) {
        delete[] Amatr[i];
    }
    delete[] AmatrInverted;
    delete[] rhs;
    delete[] AmatrArray;
    delete[] Amatr;
    delete[] line;

}
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void BmnSiliconTrackFinder::Finish() {

    if (fDebug) {

        E_eff->Divide(N_eff,D_eff,1,1);
        hEff_mctrSi->Divide(hNum_mctrSi,hDen_mctrSi,1,1);
        hEff_mcreaction->Divide(hNum_mcreaction,hDen_mcreaction,1,1);

        printf("BmnSiliconTrackFinder: write hists to file... ");
        fOutputFileName = Form("hSiliconTracks_run%d.root", fRunNumber);
        cout<< fOutputFileName <<endl;
        TFile file(fOutputFileName, "RECREATE");
        fList.Write();
        file.Close();
    }

    cout << "Work time of the SI track finder: " << workTime << " s" << endl;
}
//----------------------------------------------------------------------

ClassImp(BmnSiliconTrackFinder)
