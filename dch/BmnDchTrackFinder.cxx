// @(#)bmnroot/dch:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-01-21

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnDchTrackFinder                                                          //
//                                                                            //
// Implementation of an algorithm developed by                                //
// N.Voytishin and V.Paltchik (LIT)                                           //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for track segments                      //
// in the Drift Chambers of the BM@N experiment                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include <Rtypes.h>
#include <TMath.h>

#include "TH1F.h"
#include "TH2F.h"
#include "FairLogger.h"

#include "BmnDchTrackFinder.h"
#include "FairMCPoint.h"
#include "FitWLSQ.h"
static Double_t workTime = 0.0;
TString 		  fhTestFlnm;
TList			  fhList;
TH1D             *dxu1a;
TH1D             *dxuv1a;
TH1D             *dxyuv1a;
TH1D             *dxv1a;
TH1D             *dyu1a;
TH1D             *dyuv1a;
TH1D             *dyxuv1a;
TH1D             *dyv1a;
TH1D             *duv1a;
TH1D             *dxu2a;
TH1D             *dxv2a;
TH1D             *dyu2a;
TH1D             *dyv2a;
TH1D             *duv2a;
TH1D             *dxu1b;
TH1D             *dxuv1b;
TH1D             *dxyuv1b;
TH1D             *dxv1b;
TH1D             *dyu1b;
TH1D             *dyuv1b;
TH1D             *dyxuv1b;
TH1D             *dyv1b;
TH1D             *duv1b;
TH1D             *dxu2b;
TH1D             *dxv2b;
TH1D             *dyu2b;
TH1D             *dyv2b;
TH1D             *duv2b;
TH1D             *GDist;
TH1D             *dtime[16];
TH1D             *occup[16];
TH1D             *xwired[16];
TH1D             *ywired[16];
TH1D             *xywired[16];
TH1D             *yxwired[16];
TH1D             *XResid;
TH1D             *YResid;
TH1D             *FITXResid;
TH1D             *FITYResid;
TH1D             *FITXMCResid;
TH1D             *FITYMCResid;
TH1D             *FITXRMS;
TH1D             *FITYRMS;
TH1D             *XResid1;
TH1D             *YResid1;
TH1D             *FITXResid1;
TH1D             *FITYResid1;
TH1D             *FITXMCResid1;
TH1D             *FITYMCResid1;
TH1D             *FITXRMS1;
TH1D             *FITYRMS1;
TH1D             *Problems;
TH1D             *Ntrack;
TH1D             *Ntrack1;
TH1D             *Ntrack2;
TH1D             *Ncluster1;
TH1D             *Ncluster2;
TH1D             *lrh;
TH1D             *lrhg;
TH1D             *lrhi;
TH1D             *XResid0;
TH1D             *YResid0;
TH1D             *XResid90;
TH1D             *YResid90;
TH1D             *XResidu;
TH1D             *YResidu;
TH1D             *XResidv;
TH1D             *YResidv;
TH1D             *XResid0g;
TH1D             *YResid0g;
TH1D             *XResid90g;
TH1D             *YResid90g;
TH1D             *XResidug;
TH1D             *YResidug;
TH1D             *XResidvg;
TH1D             *YResidvg;
TH2D             *rtI;
TH2D             *rt;
TH1D             *rtr[115];
TH2D             *rtg;
TH1D             *rtrg[115];
TH1D             *XResid2;
TH1D             *YResid2;
TH1D             *FITXResid2;
TH1D             *FITYResid2;
TH1D             *FITYResidy;
TH1D             *FITYResidv;
TH1D             *FITYResidu;
TH1D             *FITXMCResid2;
TH1D             *FITYMCResid2;
TH1D             *FITXRMS2;
TH1D             *FITYRMS2;
TH1D             *XResid3;
TH1D             *YResid3;
TH1D             *XResid4;
TH1D             *YResid4;

const Int_t xai[2]={0,8};
const Int_t xbi[2]={1,9};
const Int_t yai[2]={2,10};
const Int_t ybi[2]={3,11};
const Int_t uai[2]={4,12};
const Int_t ubi[2]={5,13};
const Int_t vai[2]={6,14};
const Int_t vbi[2]={7,15};
Float_t zl_glob[16] = {9.3, 8.1, 3.5, 2.3, -2.3, -3.5, -8.1, -9.3, 9.3, 8.1, 3.5, 2.3, -2.3, -3.5, -8.1, -9.3};
Float_t angle[16] = {0, 0, 90, 90, -45, -45, 45, 45, 0, 0, 90, 90, -45, -45, 45, 45};
Float_t startX[16] = {119, 118.5, 119, 118.5, 119, 118.5, 119, 118.5, 119, 118.5, 119, 118.5, 119, 118.5, 119, 118.5};

const Double_t tanpipeangle = 0.3/5.7 ; // tangent of pipe angle
const Float_t Z_dch_mid = 637.7; //z coord between the two chambers, this z is considered the global z for the matched dch segment
const Double_t DCH1_Ypos = 0.0;
const Double_t DCH1_Zpos = 538.2; //cm
const Double_t DCH1_Xpos = DCH1_Zpos*tanpipeangle;
const Double_t DCH2_Ypos = 0.0;
const Double_t DCH2_Zpos = 737.2; //cm
const Double_t DCH2_Xpos = DCH2_Zpos*tanpipeangle;
const Double_t HoleSize_DCH = 12.0;
const Double_t SideLengthOfOctagon = 120.0;
const Double_t sin45 = 0.707106781;
const Double_t cos45 = 0.707106781;
const Double_t sin_45 = -0.707106781;
const Double_t cos_45 = 0.707106781;
const Double_t maxDistance = 3.6;//1.2;
const Double_t distCutCluster = 10;
const Double_t driftVelocity=4.5;
const Double_t DCHResolution=0.5; //0.015 если включено время дрейфа!!!???
const Double_t  DEG2RAD=3.14159/180;
const Double_t matchCut=12.5;
const Int_t  trackNHitsCut=8;
const Bool_t useDriftDistance=true;
const Int_t  hitsArraySize=12*4;
const Bool_t histoOutput=false;

BmnDchTrackFinder::BmnDchTrackFinder(Bool_t isExp) :
expData(isExp) {
	if(histoOutput){
		fhTestFlnm = "test.BmnDCHTracking.root";	
		dxu1a = new TH1D("dxu1a","dxu1a",8000,-30,30);
		dxuv1a = new TH1D("dxuv1a","dxuv1a",8000,-30,30);
		dxyuv1a = new TH1D("dxyuv1a","dxyuv1a",8000,-30,30);
		dxv1a = new TH1D("dxv1a","dxv1a",8000,-30,30);
		dyu1a = new TH1D("dyu1a","dyu1a",8000,-30,30);
		dyuv1a = new TH1D("dyuv1a","dyuv1a",8000,-30,30);
		dyxuv1a = new TH1D("dyxuv1a","dyxuv1a",8000,-30,30);
		dyv1a = new TH1D("dyv1a","dyv1a",8000,-30,30);
		duv1a = new TH1D("duv1a","duv1a",8000,-30,30);
		dxu2a = new TH1D("dxu2a","dxu2a",8000,-30,30);
		dxv2a = new TH1D("dxv2a","dxv2a",8000,-30,30);
		dyu2a = new TH1D("dyu2a","dyu2a",8000,-30,30);
		dyv2a = new TH1D("dyv2a","dyv2a",8000,-30,30);
		duv2a = new TH1D("duv2a","duv2a",8000,-30,30);
		dxu1b = new TH1D("dxu1b","dxu1b",8000,-30,30);
		dxuv1b = new TH1D("dxuv1b","dxuv1b",8000,-30,30);
		dxyuv1b = new TH1D("dxyuv1b","dxyuv1b",8000,-30,30);
		dxv1b = new TH1D("dxv1b","dxv1b",8000,-30,30);
		dyu1b = new TH1D("dyu1b","dyu1b",8000,-30,30);
		dyuv1b = new TH1D("dyuv1b","dyuv1b",8000,-30,30);
		dyxuv1b = new TH1D("dyxuv1b","dyxuv1b",8000,-30,30);
		dyv1b = new TH1D("dyv1b","dyv1b",8000,-30,30);
		duv1b = new TH1D("duv1b","duv1b",8000,-30,30);	
		dxu2b = new TH1D("dxu2b","dxu2b",8000,-30,30);
		dxv2b = new TH1D("dxv2b","dxv2b",8000,-30,30);
		dyu2b = new TH1D("dyu2b","dyu2b",8000,-30,30);
		dyv2b = new TH1D("dyv2b","dyv2b",8000,-30,30);
		duv2b = new TH1D("duv2b","duv2b",8000,-30,30);
		GDist = new TH1D("GDist","DCH track include distances",2000,-10,10);
		XResid = new TH1D("XResid","DCH xoz residuals",2000,-10,10);
		YResid = new TH1D("YResid","DCH yoz residuals",2000,-10,10);
		FITXResid = new TH1D("FITXResid","DCH xoz fit residuals",2000,-10,10);
		FITYResid = new TH1D("FITYResid","DCH yoz fit residuals",2000,-10,10);
		FITXMCResid = new TH1D("FITXMCResid","DCH xoz fit-MC residuals",2000,-10,10);
		FITYMCResid = new TH1D("FITYMCResid","DCH yoz fit-MC residuals",2000,-10,10);
		FITXRMS = new TH1D("FITXRMS","DCH xoz fit chi2",2000,-10,10);
		FITYRMS = new TH1D("FITYRMS","DCH yoz fit chi2",2000,-10,10);
		XResid1 = new TH1D("XResid1","DCH xoz residuals(z wire)",2000,-2,2);
		YResid1 = new TH1D("YResid1","DCH yoz residuals(z wire)",2000,-2,2);
		FITXResid1 = new TH1D("FITXResid1","DCH xoz fit residuals(z wire)",2000,-2,2);
		FITYResid1 = new TH1D("FITYResid1","DCH yoz fit residuals(z wire)",2000,-2,2);
		FITXMCResid1 = new TH1D("FITXMCResid1","DCH xoz fit-MC residuals(z wire)",2000,-2,2);
		FITYMCResid1 = new TH1D("FITYMCResid1","DCH yoz fit-MC residuals(z wire)",2000,-2,2);
		FITXRMS1 = new TH1D("FITXRMS1","DCH xoz fit chi2(z wire)",2000,-2,2);
		FITYRMS1 = new TH1D("FITYRMS1","DCH yoz fit chi2(z wire)",2000,-2,2);
		Problems = new TH1D("Problems","Problems",20,0,20);
		Ntrack = new TH1D("Ntrack","Ntrack",30,0,30);
		Ntrack1 = new TH1D("Ntrack1","Ntrack1",20,0,20);
		Ntrack2 = new TH1D("Ntrack2","Ntrack2",20,0,20);
		Ncluster1 = new TH1D("Ncluster1","Ncluster1",20,0,20);
		Ncluster2 = new TH1D("Ncluster2","Ncluster2",20,0,20);
		lrh = new TH1D("lr","lr",4000,-5,5);
		lrhg = new TH1D("lrG","lrG",4000,-5,5);
		lrhi = new TH1D("lrI","lrI",4000,-5,5);
		XResid0 = new TH1D("XResid0","DCH xoz residuals(z wire)",2000,-10,10);
		YResid0 = new TH1D("YResid0","DCH yoz residuals(z wire)",2000,-10,10);
		XResid90 = new TH1D("XResid90","DCH xoz residuals(z wire)",2000,-10,10);	
		YResid90 = new TH1D("YResid90","DCH yoz residuals(z wire)",2000,-10,10);
		XResidu = new TH1D("XResidu","DCH xoz residuals(z wire)",2000,-10,10);
		YResidu = new TH1D("YResidu","DCH yoz residuals(z wire)",2000,-10,10);
		XResidv = new TH1D("XResidv","DCH xoz residuals(z wire)",2000,-10,10);
		YResidv = new TH1D("YResidv","DCH yoz residuals(z wire)",2000,-10,10);
		XResid0g = new TH1D("XResid0g","DCH xoz residuals(z wire)",2000,-10,10);
		YResid0g = new TH1D("YResid0g","DCH yoz residuals(z wire)",2000,-10,10);
		XResid90g = new TH1D("XResid90g","DCH xoz residuals(z wire)",2000,-10,10);
		YResid90g = new TH1D("YResid90g","DCH yoz residuals(z wire)",2000,-10,10);
		XResidug = new TH1D("XResidug","DCH xoz residuals(z wire)",2000,-10,10);
		YResidug = new TH1D("YResidug","DCH yoz residuals(z wire)",2000,-10,10);
		XResidvg = new TH1D("XResidvg","DCH xoz residuals(z wire)",2000,-10,10);
		YResidvg = new TH1D("YResidvg","DCH yoz residuals(z wire)",2000,-10,10);
		rtI = new TH2D("rtI","rtI",2500,-1250,1250,1000,0,1);
		rt = new TH2D("rt","rt",2500,-1250,1250,1000,0,1);
		char str[5];
		for(Int_t i=0;i<115;i++){
			sprintf(str,"rt%d",i);
			rtr[i] = new TH1D(str,str,1000,0,1);
		}
		rtg = new TH2D("rtG","rtG",2500,-1250,1250,1000,0,1);
		char strG[6];
		for(Int_t i=0;i<115;i++){
			sprintf(strG,"rtG%d",i);
			rtrg[i] = new TH1D(strG,strG,1000,0,1);
		}
		char strdt[10];
		char strdt2[30];
		for(Int_t i=0;i<16;i++){
			sprintf(strdt,"dtime%d",i);
			sprintf(strdt2,"drift time layer %d",i);
			dtime[i] = new TH1D(strdt,strdt2,4000,-2000,2000);
		}
		for(Int_t i=0;i<16;i++){
			sprintf(strdt,"occup%d",i);
			sprintf(strdt2,"occup%d",i);
			occup[i] = new TH1D(strdt,strdt2,257,0,256);
		}
		for(Int_t i=0;i<16;i++){
			sprintf(strdt,"ywired%d",i);
			sprintf(strdt2,"ywired%d",i);
			ywired[i] = new TH1D(strdt,strdt2,1300,-130,130);
		}
		for(Int_t i=0;i<16;i++){
			sprintf(strdt,"xwired%d",i);
			sprintf(strdt2,"xwired%d",i);
			xwired[i] = new TH1D(strdt,strdt2,1300,-130,130);
		}
		for(Int_t i=0;i<16;i++){
			sprintf(strdt,"xywired%d",i);
			sprintf(strdt2,"xywired%d",i);
			xywired[i] = new TH1D(strdt,strdt2,1300,-130,130);
		}
		for(Int_t i=0;i<16;i++){
			sprintf(strdt,"yxwired%d",i);
			sprintf(strdt2,"yxwired%d",i);
			yxwired[i] = new TH1D(strdt,strdt2,1300,-130,130);
		}
		XResid3 = new TH1D("XResid3","DCH match xoz residuals(z wire)",2000,-10,10);
		YResid3 = new TH1D("YResid3","DCH math yoz residuals(z wire)",2000,-10,10);
		XResid4 = new TH1D("XResid4","DCH match xoz residuals(z wire)",2000,-10,10);
		YResid4 = new TH1D("YResid4","DCH math yoz residuals(z wire)",2000,-10,10);
		XResid2 = new TH1D("XResid2","DCH xoz residuals(z wire)",2000,-2,2);
		YResid2 = new TH1D("YResid2","DCH yoz residuals(z wire)",2000,-2,2);
		FITXResid2 = new TH1D("FITXResid2","DCH xoz fit residuals(z wire)",2000,-2,2);
		FITYResid2 = new TH1D("FITYResid2","DCH yoz fit residuals(z wire)",2000,-2,2);
		FITYResidy = new TH1D("FITYResidy","DCH yoz fit residuals(z wire)",2000,-2,2);
		FITYResidu = new TH1D("FITYResidu","DCH yoz fit residuals(z wire)",2000,-2,2);
		FITYResidv = new TH1D("FITYResidv","DCH yoz fit residuals(z wire)",2000,-2,2);
		FITXMCResid2 = new TH1D("FITXMCResid2","DCH xoz fit-MC residuals(z wire)",2000,-2,2);
		FITYMCResid2 = new TH1D("FITYMCResid2","DCH yoz fit-MC residuals(z wire)",2000,-2,2);
		FITXRMS2 = new TH1D("FITXRMS2","DCH xoz fit chi2(z wire)",2000,-2,2);
		FITYRMS2 = new TH1D("FITYRMS2","DCH yoz fit chi2(z wire)",2000,-2,2);

		fhList.Add(dxu1a);
		fhList.Add(dxuv1a);
		fhList.Add(dxyuv1a);
		fhList.Add(dxv1a);
		fhList.Add(dyu1a);
		fhList.Add(dyuv1a);
		fhList.Add(dyxuv1a);
		fhList.Add(dyv1a);
		fhList.Add(duv1a);
		fhList.Add(dxu2a);
		fhList.Add(dxv2a);
		fhList.Add(dyu2a);
		fhList.Add(dyv2a);
		fhList.Add(duv2a);
		fhList.Add(dxu1b);
		fhList.Add(dxuv1b);
		fhList.Add(dxyuv1b);
		fhList.Add(dxv1b);
		fhList.Add(dyu1b);
		fhList.Add(dyuv1b);
		fhList.Add(dyxuv1b);
		fhList.Add(dyv1b);
		fhList.Add(duv1b);
		fhList.Add(dxu2b);
		fhList.Add(dxv2b);
		fhList.Add(dyu2b);
		fhList.Add(dyv2b);
		fhList.Add(duv2b);
		fhList.Add(GDist);
		fhList.Add(XResid);
		fhList.Add(YResid);
		fhList.Add(FITXResid);
		fhList.Add(FITYResid);
		fhList.Add(FITXMCResid);
		fhList.Add(FITYMCResid);
		fhList.Add(FITXRMS);
		fhList.Add(FITYRMS);
		fhList.Add(XResid1);
		fhList.Add(YResid1);
		fhList.Add(FITXResid1);
		fhList.Add(FITYResid1);
		fhList.Add(FITXMCResid1);
		fhList.Add(FITYMCResid1);
		fhList.Add(FITXRMS1);
		fhList.Add(FITYRMS1);
		fhList.Add(XResid2);
		fhList.Add(YResid2);
		fhList.Add(FITXResid2);
		fhList.Add(FITYResid2);
		fhList.Add(FITYResidy);	
		fhList.Add(FITYResidu);
		fhList.Add(FITYResidv);
		fhList.Add(FITXMCResid2);
		fhList.Add(FITYMCResid2);
		fhList.Add(FITXRMS2);
		fhList.Add(FITYRMS2);
		fhList.Add(XResid3);
		fhList.Add(YResid3);
		fhList.Add(XResid4);
		fhList.Add(YResid4);
		fhList.Add(Problems);
		fhList.Add(Ntrack);
		fhList.Add(Ntrack1);
		fhList.Add(Ntrack2);
		fhList.Add(Ncluster1);
		fhList.Add(Ncluster2);
		fhList.Add(lrh);
		fhList.Add(lrhg);
		fhList.Add(lrhi);
		fhList.Add(XResid0);
		fhList.Add(YResid0);
		fhList.Add(XResid90);
		fhList.Add(YResid90);
		fhList.Add(XResidu);
		fhList.Add(YResidu);
		fhList.Add(XResidv);
		fhList.Add(YResidv);
		fhList.Add(XResid0g);
		fhList.Add(YResid0g);
		fhList.Add(XResid90g);
		fhList.Add(YResid90g);
		fhList.Add(XResidug);
		fhList.Add(YResidug);
		fhList.Add(XResidvg);
		fhList.Add(YResidvg);
		fhList.Add(rtI);
		fhList.Add(rt);
		for(Int_t i=0;i<16;i++)
			fhList.Add(dtime[i]);
		for(Int_t i=0;i<16;i++)
			fhList.Add(occup[i]);
		for(Int_t i=0;i<16;i++)
			fhList.Add(xwired[i]);
		for(Int_t i=0;i<16;i++)
			fhList.Add(ywired[i]);
		for(Int_t i=0;i<16;i++)
			fhList.Add(xywired[i]);
		for(Int_t i=0;i<16;i++)
			fhList.Add(yxwired[i]);
		for(Int_t i=0;i<110;i++)
			fhList.Add(rtr[i]);
		fhList.Add(rtg);
		for(Int_t i=0;i<110;i++)
			fhList.Add(rtrg[i]);
	}
    fEventNo = 0;
    N = 2;
    tracksDch = "BmnDchTrack";
    InputDigitsBranchName = "DCH";
    fTransferFunctionName = "transfer_func.txt";

    prev_wire = -1;
    prev_time = -1;

    nChambers = 2;
    nWires = 4;
    nLayers = 2;
    nSegmentsMax = 100;

    Z_dch1 = 523.45;
    Z_dch2 = 722.45;
    Z_dch_mid = (Z_dch1 + Z_dch2) / 2.;
    dZ_dch_mid = Z_dch2 - Z_dch_mid; // > 0
    dZ_dch = Z_dch2 - Z_dch1;

    // Some alignment corrections
    x1_sh = 12.80;
    x2_sh = 6.24;
    y1_sh = 0.00;
    y2_sh = 0.50;

    x1_slope_sh = -0.087;
    y1_slope_sh = 0.051;
    x2_slope_sh = -0.085;
    y2_slope_sh = 0.058;

    scale = 0.5;
}

BmnDchTrackFinder::~BmnDchTrackFinder() {

}

void BmnDchTrackFinder::RotatePoint(Double_t xc, Double_t yc, Double_t x1, Double_t y1, Double_t* xW, Double_t* yW, Int_t angleP) {
	Double_t xt;
	Double_t yt;
	if(angleP==-45){
		xt=xc+(x1-xc)*cos45-(y1-yc)*sin45;
		yt=yc+(x1-xc)*sin45+(y1-yc)*cos45;
	}
	if(angleP==45){
		xt=xc+(x1-xc)*cos_45-(y1-yc)*sin_45;
		yt=yc+(x1-xc)*sin_45+(y1-yc)*cos_45;
	}
  	*xW=xt;
  	*yW=yt;
}
UChar_t  BmnDchTrackFinder::GetPlane(Double_t z){
  Double_t zp=0;
  Double_t min=10000000000;
  UChar_t plane=100;
  for(Int_t iPlane=0;iPlane<16;iPlane++){
    if(iPlane<8)
      zp = zl_glob[iPlane] +DCH1_Zpos;
    else
      zp= zl_glob[iPlane] +DCH2_Zpos;
    if(Abs(z-zp)<min){
      min = Abs(z-zp);
      plane = iPlane;
    }
  }
  return plane;
}
Double_t BmnDchTrackFinder::Distance(Double_t x1,Double_t y1,Double_t x2,Double_t y2,Double_t x0,Double_t y0)
{
	return Abs((y2-y1)*x0 - (x2-x1)*y0 + x2*y1 - y2*x1)/Sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
}

void BmnDchTrackFinder::GetCoordinatesWirePoint(UChar_t plane, Double_t xT, Double_t yT, Int_t iWire, Double_t* xW, Double_t* yW, Double_t ddist)
{
	if(angle[plane]==0){  
	  Double_t x1 = DCH1_Xpos + startX[plane] - iWire;
	  if(plane>7)
		  x1 = DCH2_Xpos + startX[plane] - iWire;
	  if(useDriftDistance){
		  if(xT<x1) //left
			  x1-=ddist;
		  else //right
			  x1+=ddist;
	  }
	  *xW=x1;
	  *yW=yT;
  }
  if(angle[plane]==90){  
	  Double_t y1 = startX[plane] - iWire;
	  if(plane>7)
		  y1 = startX[plane] - iWire;
	  if(useDriftDistance){
		  if(yT<y1) //left
			  y1-=ddist;
		  else //right
			  y1+=ddist;
	  }
	  *yW=y1;
	  *xW=xT;
  }
  if(angle[plane]==45){  

	  Double_t x1 = DCH1_Xpos + startX[plane] - iWire;
	  if(plane>7)
		  x1 = DCH2_Xpos + startX[plane] - iWire;
  	
	  Double_t xT1,yT1;
	  if(plane>7)
		  RotatePoint(DCH2_Xpos, 0, xT, yT, &xT1, &yT1, -45);
	  else
		  RotatePoint(DCH1_Xpos, 0, xT, yT, &xT1, &yT1, -45);

	  
	  Double_t y1 = yT1; 

	  if(useDriftDistance){
		  if(xT1<x1) //left
			  x1-=ddist;
		  else //right
			  x1+=ddist;
	  }
  	
	Double_t xW1,yW1;
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x1, y1, &xW1, &yW1, 45);
	else
		RotatePoint(DCH1_Xpos, 0, x1, y1, &xW1, &yW1, 45);

  	*xW=xW1;
  	*yW=yW1;
  }
  if(angle[plane]==-45){
	  Double_t x1 = DCH1_Xpos + startX[plane] - iWire;
	  if(plane>7)
		  x1 = DCH2_Xpos + startX[plane] - iWire;

	  Double_t xT1,yT1;
	  if(plane>7)
		  RotatePoint(DCH2_Xpos, 0, xT, yT, &xT1, &yT1, 45);
	  else
		  RotatePoint(DCH1_Xpos, 0, xT, yT, &xT1, &yT1, 45);

	  Double_t y1 = yT1; 
  	
	  if(useDriftDistance)
	  {
		  if(xT1<x1) //left
			  x1-=ddist;
		  else //right
			  x1+=ddist;
	  }
  	
	  Double_t xW1,yW1;
	  if(plane>7)
		  RotatePoint(DCH2_Xpos, 0, x1, y1, &xW1, &yW1, -45);
	  else
		  RotatePoint(DCH1_Xpos, 0, x1, y1, &xW1, &yW1, -45);

	  *xW=xW1;
	  *yW=yW1;
  }
}
	
Double_t BmnDchTrackFinder::GetDistance(UChar_t plane, Double_t xD, Double_t yD, Int_t iWire)
{
  Double_t distance;
  if(angle[plane]==0){
    Double_t xPos = DCH1_Xpos + startX[plane] - iWire;
    if(plane>7)
      xPos = DCH2_Xpos + startX[plane] - iWire;
    distance=Abs(xD-xPos);
  }
  if(angle[plane]==90){
    Double_t yPos = startX[plane] - iWire;
    distance=Abs(yD-yPos);
  }
  if(angle[plane]==45){
  	Double_t x1 = DCH1_Xpos + startX[plane] - iWire;
  	if(plane>7)
  		x1 = DCH2_Xpos + startX[plane] - iWire;
  	Double_t x2=x1;
  	Double_t y1 = -startX[plane];
  	Double_t y2 = startX[plane];
  	
	Double_t xW,yW;
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x1, y1, &xW, &yW, 45);
	else
		RotatePoint(DCH1_Xpos, 0, x1, y1, &xW, &yW, 45);
  	x1 = xW;
  	y1 = yW; 
	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x2, y2, &xW, &yW, 45);
	else
		RotatePoint(DCH1_Xpos, 0, x2, y2, &xW, &yW, 45);
  	x2 = xW;
  	y2 = yW;
  	
    distance = Distance(x1,y1,x2,y2,xD,yD);
  }
  if(angle[plane]==-45){
  	Double_t x1 = DCH1_Xpos + startX[plane] - iWire;
  	if(plane>7)
  		x1 = DCH2_Xpos + startX[plane] - iWire;
  	Double_t x2=x1;
  	Double_t y1 = -startX[plane];
  	Double_t y2 = startX[plane];
  	
	Double_t xW,yW;
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x1, y1, &xW, &yW, -45);
	else
		RotatePoint(DCH1_Xpos, 0, x1, y1, &xW, &yW, -45);
  	x1 = xW;
  	y1 = yW; 
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x2, y2, &xW, &yW, -45);
	else
		RotatePoint(DCH1_Xpos, 0, x2, y2, &xW, &yW, -45);
  	x2 = xW;
  	y2 = yW;
  	
    distance = Distance(x1,y1,x2,y2,xD,yD);
  }
  return distance;
}
Short_t  BmnDchTrackFinder::GetClosestWireNumber(UChar_t plane, Double_t xD, Double_t yD, Double_t *dist) 
{
  Double_t xmin=10000000000;
  Double_t ymin=10000000000;
  Double_t dmin=10000000000;
  Short_t rWire=-1;
  
  for(Int_t iWire=0;iWire<240;iWire++){
    if(angle[plane]==0){
    	Double_t xPos = DCH1_Xpos + startX[plane] - iWire;
    	if(plane>7)
    		xPos = DCH2_Xpos + startX[plane] - iWire;
    	if(Abs(xD-xPos)< xmin){
    		xmin=Abs(xD-xPos);
    		rWire=iWire;
    		dmin=xmin;
      }	
    }	
    if(angle[plane]==90){
    	Double_t yPos = startX[plane] - iWire;
    	if(Abs(yD-yPos)< ymin){
    		ymin=Abs(yD-yPos);
    		rWire=iWire;
    		dmin=ymin;
      }
    }	
    if(angle[plane]==45){
    	Double_t x1 = DCH1_Xpos + startX[plane] - iWire;
    	if(plane>7)
    		x1 = DCH2_Xpos + startX[plane] - iWire;
    	Double_t x2=x1;
    	Double_t y1 = -startX[plane];
    	Double_t y2 = startX[plane];
    	
    	Double_t xW;
    	Double_t yW;
    	if(plane>7)
    		RotatePoint(DCH2_Xpos, 0, x1, y1, &xW, &yW, 45);
    	else
    		RotatePoint(DCH1_Xpos, 0, x1, y1, &xW, &yW, 45);
    	
        x1 = xW;
    	y1 = yW; 

    	if(plane>7)
    		RotatePoint(DCH2_Xpos, 0, x2, y2, &xW, &yW, 45);
    	else
    		RotatePoint(DCH1_Xpos, 0, x2, y2, &xW, &yW, 45);
    	x2 = xW;
    	y2 = yW;
    	
    	
    	Double_t d = Distance(x1,y1,x2,y2,xD,yD);
    	if(d<dmin){
    		dmin=d;
    		rWire=iWire;
    	}
    }
    if(angle[plane]==-45){
    	Double_t x1 = DCH1_Xpos + startX[plane] - iWire;
    	if(plane>7)
    		x1 = DCH2_Xpos + startX[plane] - iWire;
    	Double_t x2=x1;
    	Double_t y1 = -startX[plane];
    	Double_t y2 = startX[plane];
    	
    	Double_t xW;
    	Double_t yW;
    	if(plane>7)
    		RotatePoint(DCH2_Xpos, 0, x1, y1, &xW, &yW, -45);
    	else
    		RotatePoint(DCH1_Xpos, 0, x1, y1, &xW, &yW, -45);
    	
    	x1 = xW;
    	y1 = yW; 

    	if(plane>7)
    		RotatePoint(DCH2_Xpos, 0, x2, y2, &xW, &yW, -45);
    	else
    		RotatePoint(DCH1_Xpos, 0, x2, y2, &xW, &yW, -45);
    	x2 = xW;
    	y2 = yW;
    	
    	Double_t d = Distance(x1,y1,x2,y2,xD,yD);
    	if(d<dmin){
    		dmin=d;
    		rWire=iWire;
    	}
    }	
  }		
  if(Abs(dmin)>0.5)
	  return -1;
  *dist=dmin;
  return rWire;
}
Double_t  BmnDchTrackFinder::GetTime(Double_t* distance)
{
  return ((*distance*10000.0)/driftVelocity);
}
Int_t  BmnDchTrackFinder::GetRTIndex(Double_t time)
{
  if(!expData)
	  return (Int_t) (time/10.);
  else{
	  Double_t fTime = time;
	  if((fTime+400)>0)
		  fTime+=400;
	  else
		  fTime=0;
	  return (Int_t) (fTime/10.);
  }
}
Double_t  BmnDchTrackFinder::GetDDistance(Double_t time, Int_t plane)
{
  if(!expData)
	  return (driftVelocity*time)/10000;
  else
	  return rtRel[plane][(Int_t) ((time+2000)/10)];
}
Bool_t BmnDchTrackFinder::CheckPointGEO(Int_t plane, Double_t xD, Double_t yD, Double_t z)
{
  Double_t x1;
  Double_t x2;
  Double_t xmin;
  Double_t xmax;
  Double_t y1;
  x1 = DCH1_Xpos -90.5;
  xmin = DCH1_Xpos -120;
  if(plane>7){
    x1 = DCH2_Xpos -90.5;
    xmin = DCH2_Xpos -120;
  }
  x2 = DCH1_Xpos + 90.5;
  xmax = DCH1_Xpos + 120;
  if(plane>7){
    x2 = DCH2_Xpos + 90.5;
    xmax = DCH2_Xpos + 120;
  }
	
  if(xD>xmin && xD<xmax){
    if(xD>x1 && xD<x2){
      if(Abs(yD)<120)
	return true;
    }
    
    if(xD<=x1){
      if(yD>0)
	y1=120-(Abs(x1)- Abs(xD));
      else
	y1=-120+(Abs(x1)- Abs(xD));
      if(Abs(yD)<Abs(y1))
	return true;
    }
    
    if(xD>=x2){
      if(yD>0)
	y1=120-(Abs(x2)- Abs(xD));
      else
	y1=-120+(Abs(x2)- Abs(xD));
      if(Abs(yD)<Abs(y1))
	return true;
    }
  }
  cout << "MC Point x: " << xD << " y: " << yD << " out of DCH range " 
       << xmin << " " << xmax << " " << plane << " " << DCH1_Xpos << " " << DCH2_Xpos<<  endl;
  return false;
}


void BmnDchTrackFinder::GetTrWPoints(Int_t idx1, Double_t xTr, Double_t yTr, Double_t* xr, Double_t* yr, Double_t xMC, Double_t yMC){
  Double_t x1,y1;
  Double_t xMCR,yMCR;
  Double_t  ddist = GetDDistance(digisDCH[idx1]->GetTime(),digisDCH[idx1]->GetPlane());
  Int_t plane = digisDCH[idx1]->GetPlane();
  Int_t iWire = digisDCH[idx1]->GetWireNumber();
  
  if(!expData){
	  if(angle[plane]==0){
		  yMCR=yMC;
		  xMCR=xMC;
	  }
	  if(angle[plane]==90){
		  yMCR=yMC;
		  xMCR=xMC;
	  }
	  if(angle[plane]==45){
		  	if(plane>7)
				RotatePoint(DCH2_Xpos, 0, xMC, yMC, &xMCR, &yMCR, -45);
			else
				RotatePoint(DCH1_Xpos, 0, xMC, yMC, &xMCR, &yMCR, -45);
	  }
	  if(angle[plane]==-45){
		  	if(plane>7)
				RotatePoint(DCH2_Xpos, 0, xMC, yMC, &xMCR, &yMCR, 45);
			else
				RotatePoint(DCH1_Xpos, 0, xMC, yMC, &xMCR, &yMCR, 45);
	  }
  }
  
  if(!useDriftDistance)
	  ddist=0;
  if(angle[plane]==0){
    x1 = DCH1_Xpos + startX[plane] - iWire;
    if(plane>7)
      x1 = DCH2_Xpos + startX[plane] - iWire;
    if(x1<xTr)
    	x1+=ddist;
    else
    	x1-=ddist;
    
    y1=yTr;
  }
  if(angle[plane]==90){
    y1 = startX[plane] - iWire;
    if(y1<yTr)
    	y1+=ddist;
    else
    	y1-=ddist;
    x1=xTr;
  }
  if(angle[plane]==45){
  	Double_t xRot,yRot;
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, xTr, yTr, &xRot, &yRot, -45);
	else
		RotatePoint(DCH1_Xpos, 0, xTr, yTr, &xRot, &yRot, -45);
	x1 = DCH1_Xpos + startX[plane] - iWire;
  	if(plane>7)
  		x1 = DCH2_Xpos + startX[plane] - iWire;
    if(x1<xRot)
    	x1+=ddist;
    else
    	x1-=ddist;
  	y1 = yRot;
  	
	Double_t xW,yW;
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x1, y1, &xW, &yW, 45);
	else
		RotatePoint(DCH1_Xpos, 0, x1, y1, &xW, &yW, 45);
  	x1 = xW;
  	y1 = yW; 
   }
  if(angle[plane]==-45){
  	Double_t xRot,yRot;
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, xTr, yTr, &xRot, &yRot, 45);
	else
		RotatePoint(DCH1_Xpos, 0, xTr, yTr, &xRot, &yRot, 45);
  	x1 = DCH1_Xpos + startX[plane] - iWire;
  	if(plane>7)
  		x1 = DCH2_Xpos + startX[plane] - iWire;
    if(x1<xRot)
    	x1+=ddist;
    else
    	x1-=ddist;
  	y1 = yRot;
  	
	Double_t xW,yW;
	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x1, y1, &xW, &yW, -45);
	else
		RotatePoint(DCH1_Xpos, 0, x1, y1, &xW, &yW, -45);
  	x1 = xW;
  	y1 = yW; 
  }
  *xr=x1;
  *yr=y1;
}

Double_t BmnDchTrackFinder::GetCoordinateByTwoPoints(Double_t x1, Double_t x2, Double_t z1, Double_t z2, Double_t zc){
	return x1+(zc-z1)*((x2-x1)/(z2-z1));
}
Bool_t BmnDchTrackFinder::CombInitial(Int_t* clIa, Double_t* zz, Double_t* parx, Double_t *pary){  
	Double_t x1l,y1l,x1r,y1r,x2l,y2l,x2r,y2r,x1,xop1,xop2,yop1,yop2;
	Double_t xRot,yRot;

	for(Int_t ii=0;ii<4;ii++){
		Double_t  ddist = GetDDistance(digisDCH[clIa[ii]]->GetTime(),digisDCH[clIa[ii]]->GetPlane());
		Int_t plane = digisDCH[clIa[ii]]->GetPlane();
		Int_t iWire = digisDCH[clIa[ii]]->GetWireNumber();
	
		if(ii==0 && clIa[ii]!=-1){
			x1l = DCH1_Xpos + startX[plane] - iWire;
			if(plane>7)
				x1l = DCH2_Xpos + startX[plane] - iWire;
			x1r = x1l+ddist;
			x1l -= ddist;
		}

		if(ii==1 && clIa[ii]!=-1){
			x2l = DCH1_Xpos + startX[plane] - iWire;
			if(plane>7)
				x2l = DCH2_Xpos + startX[plane] - iWire;
			x2r = x1l+ddist;
			x2l -= ddist;
		}
		
		if(ii==2 && clIa[ii]!=-1){
			y1l = startX[plane] - iWire;
			y1r = y1l+ddist;
			y1l -= ddist;
		}
		
		if(ii==3 && clIa[ii]!=-1){
			y2l = startX[plane] - iWire;
			y2r = y1l+ddist;
			y2l -= ddist;
		}
	}
	Double_t resids[16];
	for(Int_t icombx=0;icombx<4;icombx++){
		for(Int_t icomby=0;icomby<4;icomby++){
			if(icombx==0){
				xop1=x1l;
				xop2=x2l;
			}
			if(icombx==1){
				xop1=x1l;
				xop2=x2r;
			}
			if(icombx==2){
				xop1=x1r;
				xop2=x2l;
			}
			if(icombx==3){
				xop1=x1r;
				xop2=x2r;
			}
			if(icomby==0){
				yop1=y1l;
				yop2=y2l;
			}
			if(icomby==1){
				yop1=y1l;
				yop2=y2r;
			}
		    if(icomby==2){
				yop1=y1r;
				yop2=y2l;
			}
			if(icomby==3){
				yop1=y1r;
				yop2=y2r;
			}
			for(Int_t ii=4;ii<8;ii++){

				Double_t xTr = GetCoordinateByTwoPoints(xop1, xop2, zz[0], zz[1], zz[ii]);
				Double_t yTr = GetCoordinateByTwoPoints(yop1, yop2, zz[2], zz[3], zz[ii]);
				Double_t  ddist = GetDDistance(digisDCH[clIa[ii]]->GetTime(),digisDCH[clIa[ii]]->GetPlane());
				Int_t plane = digisDCH[clIa[ii]]->GetPlane();
				Int_t iWire = digisDCH[clIa[ii]]->GetWireNumber();
	
				if(angle[plane]==45){
					if(plane>7)
						RotatePoint(DCH2_Xpos, 0, xTr, yTr, &xRot, &yRot, -45);
					else
						RotatePoint(DCH1_Xpos, 0, xTr, yTr, &xRot, &yRot, -45);
					x1 = DCH1_Xpos + startX[plane] - iWire;
					if(plane>7)
						x1 = DCH2_Xpos + startX[plane] - iWire;
					if(x1<xRot)
						x1+=ddist;
					else
						x1-=ddist;
					resids[icombx*4+icomby] +=Abs(xRot-x1);
				}
				if(angle[plane]==-45){
					if(plane>7)
						RotatePoint(DCH2_Xpos, 0, xTr, yTr, &xRot, &yRot, 45);
					else
						RotatePoint(DCH1_Xpos, 0, xTr, yTr, &xRot, &yRot, 45);
					x1 = DCH1_Xpos + startX[plane] - iWire;
					if(plane>7)
						x1 = DCH2_Xpos + startX[plane] - iWire;
					if(x1<xRot)
						x1+=ddist;
					else
						x1-=ddist;
				}
				resids[icombx*4+icomby] +=Abs(xRot-x1);
			}
		}
	}
	Double_t minx=0;
	Double_t miny=0;
	Double_t minv=10000000;
	for(Int_t icombx=0;icombx<4;icombx++){
		for(Int_t icomby=0;icomby<4;icomby++){
			if(resids[icombx*4+icomby]<minv){
				minv=resids[icombx*4+icomby];
				minx = icombx;
				miny = icomby;
			}
		}
	}
	Double_t x2,y1,y2;
	if(minx==0){
		x1=x1l;
		x2=x2l;
	}
	if(minx==1){
		x1=x1l;
		x2=x2r;
	}
	if(minx==2){
		x1=x1r;
		x2=x2l;
	}
	if(minx==3){
		x1=x1r;
		x2=x2r;
	}
	if(miny==0){
		y1=y1l;
		y2=y2l;
	}
	if(miny==1){
		y1=y1l;
		y2=y2r;
	}
	if(miny==2){
		y1=y1r;
		y2=y2l;
	}
	if(miny==3){
		y1=y1r;
		y2=y2r;
	}
	parx[0]=x1-zz[0]*((x2-x1)/(zz[1]-zz[0]));
	parx[1]=(x2-x1)/(zz[1]-zz[0]);
	pary[0]=y1-zz[2]*(y2-y1)/(zz[3]-zz[2]);
	pary[1]=(y2-y1)/(zz[3]-zz[2]);
	return true;
}

void BmnDchTrackFinder::FillLR(Int_t idx1, Double_t xTr, Double_t yTr, Double_t xMC, Double_t yMC, Int_t global){
  Double_t x1,y1;
  Double_t xMCR,yMCR;
  Double_t  ddist = GetDDistance(digisDCH[idx1]->GetTime(),digisDCH[idx1]->GetPlane());
  Int_t plane = digisDCH[idx1]->GetPlane();
  Int_t iWire = digisDCH[idx1]->GetWireNumber();
  
  if(!expData){
	  if(angle[plane]==0){
		  yMCR=yMC;
		  xMCR=xMC;
	  }
	  if(angle[plane]==90){
		  yMCR=yMC;
		  xMCR=xMC;
	  }
	  if(angle[plane]==45){
		  	if(plane>7)
				RotatePoint(DCH2_Xpos, 0, xMC, yMC, &xMCR, &yMCR, -45);
			else
				RotatePoint(DCH1_Xpos, 0, xMC, yMC, &xMCR, &yMCR, -45);
	  }
	  if(angle[plane]==-45){
		  	if(plane>7)
				RotatePoint(DCH2_Xpos, 0, xMC, yMC, &xMCR, &yMCR, 45);
			else
				RotatePoint(DCH1_Xpos, 0, xMC, yMC, &xMCR, &yMCR, 45);
	  }
  }
  
  if(!useDriftDistance)
	  ddist=0;
  if(angle[plane]==0){
    x1 = DCH1_Xpos + startX[plane] - iWire;
    if(plane>7)
      x1 = DCH2_Xpos + startX[plane] - iWire;
    if(!expData){
    	if(x1<xTr && x1<xMCR){
    		if(global==0)
    			lrhi->Fill(0);
    		if(global==1)
    			lrh->Fill(0);
    		if(global==2)
    			lrhg->Fill(0);
    	}
    	if(x1>xTr && x1>xMCR){
    		if(global==0)
    			lrhi->Fill(0);
    		if(global==1)
    			lrh->Fill(0);
    		if(global==2)
    			lrhg->Fill(0);
    	}
    	if(x1>xTr && x1<xMCR){
    		if(global==0)
    			lrhi->Fill(-1);
    		if(global==1){
    			lrh->Fill(-1);
    			XResid0->Fill(x1-ddist-xMCR);
    		}
			if(global==2){
    			lrhg->Fill(-1);
    			XResid0g->Fill(x1-ddist-xMCR);
    		}
    	}
    	if(x1<xTr && x1>xMCR){
    		if(global==0)
    			lrhi->Fill(1);
    		if(global==1){
    			lrh->Fill(1);
    			XResid0->Fill(x1+ddist-xMCR);
    		}
			if(global==2){
    			lrhg->Fill(1);
    			XResid0g->Fill(x1+ddist-xMCR);
    		}
    	}
    }
  }
  if(angle[plane]==90){
    y1 = startX[plane] - iWire;
    if(!expData){
    	if(y1<yTr && y1<yMCR){
    		if(global==0)
    			lrhi->Fill(0);
    		if(global==1)
    			lrh->Fill(0);
       		if(global==2)
    			lrhg->Fill(0);
    	}
    	if(y1>yTr && y1>yMCR){
    		if(global==0)
    			lrhi->Fill(0);
    		if(global==1)
    			lrh->Fill(0);
    		if(global==2)
    			lrhg->Fill(0);
    	}
    	if(y1>yTr && y1<yMCR){
    		if(global==0)
    			lrhi->Fill(-2);
    		if(global==1){
    			lrh->Fill(-2);
    			YResid90->Fill(y1-ddist-yMCR);
    		}
    		if(global==2){
    			lrhg->Fill(-2);
    			YResid90g->Fill(y1-ddist-yMCR);
    		}
    	}
    	if(y1<yTr && y1>yMCR){
    		if(global==0)
    			lrhi->Fill(2);
    		if(global==1){
    			lrh->Fill(2);
    			YResid90->Fill(y1+ddist-yMCR);
    		}
    		if(global==2){
    			lrhg->Fill(2);
    			YResid90g->Fill(y1+ddist-yMCR);
    		}
    	}
    }
  }
  if(angle[plane]==45){
  	Double_t xRot,yRot;
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, xTr, yTr, &xRot, &yRot, -45);
	else
		RotatePoint(DCH1_Xpos, 0, xTr, yTr, &xRot, &yRot, -45);
	x1 = DCH1_Xpos + startX[plane] - iWire;
  	if(plane>7)
  		x1 = DCH2_Xpos + startX[plane] - iWire;
    if(!expData){
    	if(x1<xRot && x1<xMCR){
    		if(global==0)
    			lrhi->Fill(0);
    		if(global==1)
    			lrh->Fill(0);
    		if(global==2)
    			lrhg->Fill(0);
    	}
    	if(x1>xRot && x1>xMCR){
    		if(global==0)
    			lrhi->Fill(0);
    		if(global==1)
    			lrh->Fill(0);
    		if(global==2)
    			lrhg->Fill(0);
    	}
    	if(x1>xRot && x1<xMCR){
    		if(global==0)
    			lrhi->Fill(-3);
    		if(global==1){
    			lrh->Fill(-3);
    			XResidu->Fill(x1-ddist-xMCR);
    		}
    		if(global==2){
    			lrhg->Fill(-3);
    			XResidug->Fill(x1-ddist-xMCR);   			
    		}
    	}
    	if(x1<xRot && x1>xMCR){
    		if(global==0)
    			lrhi->Fill(3);
    		if(global==1){
    			lrh->Fill(3);
    			XResidu->Fill(x1+ddist-xMCR);
    		}
    		if(global==2){
    			lrhg->Fill(3);
    			XResidug->Fill(x1+ddist-xMCR);    			
    		}
    	}
    }
  }
  if(angle[plane]==-45){
  	Double_t xRot,yRot;
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, xTr, yTr, &xRot, &yRot, 45);
	else
		RotatePoint(DCH1_Xpos, 0, xTr, yTr, &xRot, &yRot, 45);
  	x1 = DCH1_Xpos + startX[plane] - iWire;
  	if(plane>7)
  		x1 = DCH2_Xpos + startX[plane] - iWire;
    if(!expData){
    	if(x1<xRot && x1<xMCR){
    		if(global==0)
    			lrhi->Fill(0);
    		if(global==1)
    			lrh->Fill(0);
    		if(global==2)
    			lrhg->Fill(0);
    	}
    	if(x1>xRot && x1>xMCR){
    		if(global==0)
    			lrhi->Fill(0);
    		if(global==1)
    			lrh->Fill(0);
    		if(global==2)
    			lrhg->Fill(0);
    	}
    	if(x1>xRot && x1<xMCR){
    		if(global==0)
    			lrhi->Fill(-4);
    		if(global==1){
    			lrh->Fill(-4);
    			XResidv->Fill(x1-ddist-xMCR);
    		}
    		if(global==2){
    			lrhg->Fill(-4);
    			XResidvg->Fill(x1-ddist-xMCR);
    		}
    	}
    	if(x1<xRot && x1>xMCR){
    		if(global==0)
    			lrhi->Fill(4);
    		if(global==1){
    			lrh->Fill(4);
    			XResidv->Fill(x1+ddist-xMCR);
    		}
    		if(global==2){
    			lrhg->Fill(4);
    			XResidvg->Fill(x1+ddist-xMCR);
    		}
    	}
    }
  }
}

void BmnDchTrackFinder::GetBEPoints(Int_t idx1, Double_t* x1r, Double_t* y1r, Double_t* x2r, Double_t* y2r, Int_t lr){
  Double_t x1,x2,y1,y2;
  Double_t  ddist = lr*GetDDistance(digisDCH[idx1]->GetTime(),digisDCH[idx1]->GetPlane());
  Int_t plane = digisDCH[idx1]->GetPlane();
  Int_t iWire = digisDCH[idx1]->GetWireNumber();
  if(angle[plane]==0){
    x1 = DCH1_Xpos + startX[plane] - iWire;
    if(plane>7)
      x1 = DCH2_Xpos + startX[plane] - iWire;
    x1+=ddist;
    x2=x1;
    y1=-startX[plane];
    y2=startX[plane];
  }
  if(angle[plane]==90){
    y1 = startX[plane] - iWire;
    y1+=ddist;
    y2=y1;
    x1=-startX[plane];
    x2=startX[plane];
  }
  if(angle[plane]==45){
  	x1 = DCH1_Xpos + startX[plane] - iWire;
  	if(plane>7)
  		x1 = DCH2_Xpos + startX[plane] - iWire;
    x1+=ddist;
 	  x2=x1;
  	y1 = -startX[plane];
  	y2 = startX[plane];
	Double_t xW,yW;
  	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x1, y1, &xW, &yW, 45);
	else
		RotatePoint(DCH1_Xpos, 0, x1, y1, &xW, &yW, 45);
  	x1 = xW;
  	y1 = yW; 
	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x2, y2, &xW, &yW, 45);
	else
		RotatePoint(DCH1_Xpos, 0, x2, y2, &xW, &yW, 45);
  	x2 = xW;
  	y2 = yW;
   }
  if(angle[plane]==-45){
  	x1 = DCH1_Xpos + startX[plane] - iWire;
  	if(plane>7)
  		x1 = DCH2_Xpos + startX[plane] - iWire;
    x1+=ddist;
  	x2=x1;
  	y1 = -startX[plane];
  	y2 = startX[plane];
  	
	Double_t xW,yW;
	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x1, y1, &xW, &yW, -45);
	else
		RotatePoint(DCH1_Xpos, 0, x1, y1, &xW, &yW, -45);
  	x1 = xW;
  	y1 = yW; 
 	if(plane>7)
		RotatePoint(DCH2_Xpos, 0, x2, y2, &xW, &yW, -45);
	else
		RotatePoint(DCH1_Xpos, 0, x2, y2, &xW, &yW, -45);
  	x2 = xW;
  	y2 = yW;
  }
  *x1r=x1;
  *x2r=x2;
  *y1r=y1;
  *y2r=y2;
}
void BmnDchTrackFinder::LRQA(Int_t* clI, Double_t* parx, Double_t* pary, Double_t *zz11x, Double_t *zz11y, FairMCPoint* pnt1, FairMCPoint* pnt2, Int_t global){
	Double_t xT; 
	Double_t yT;
	Double_t xMC; 
	Double_t yMC;
	for(Int_t ii=0;ii<6;ii++){
		if(clI[ii]!=-1 && clI[ii+2]!=-1){
			if(ii<2){
				xT = parx[1]*zz11x[ii] + parx[0];
				yT = pary[1]*zz11x[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11x[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11x[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				if(histoOutput)
					FillLR(clI[ii], xT, yT, xMC, yMC, global);

				xT = parx[1]*zz11y[ii] + parx[0];
				yT = pary[1]*zz11y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				if(histoOutput)
					FillLR(clI[ii+2], xT, yT, xMC, yMC, global);
			}
			else{
				xT = parx[1]*zz11y[ii] + parx[0];
				yT = pary[1]*zz11y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				if(histoOutput)
					FillLR(clI[ii+2], xT, yT, xMC, yMC, global);
			}
		}
	}
}

Bool_t BmnDchTrackFinder::FitIteraGlobal(Int_t* clIa, Int_t* clIb, Double_t* parx, Double_t* pary, Double_t* parx1, Double_t* pary1, Double_t *zz11x, Double_t *zz11y, Double_t *zz21x, Double_t *zz21y, Bool_t fillHisto, FairMCPoint* pnt1, FairMCPoint* pnt2, Int_t sp, Double_t *chi2x, Double_t *chi2y){
	Double_t xT; 
	Double_t yT;
	Double_t xMC=0.; 
	Double_t yMC=0.;
	Double_t xr; 
	Double_t yr;

	vector<Double_t> xf; 
	vector<Double_t> yf; 
	vector<Double_t> zf; 
	vector<Double_t> zyf; 
	
	for(Int_t ii=0;ii<6;ii++){
		if(ii<2){
			if(clIa[ii]!=-1 && clIa[ii+2]!=-1){
				xT = parx[1]*zz11x[ii] + parx[0];
				yT = pary[1]*zz11x[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11x[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11x[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clIa[ii], xT, yT, &xr, &yr, xMC, yMC);
				xf.push_back(xr);
				zf.push_back(zz11x[ii]);

				xT = parx[1]*zz11y[ii] + parx[0];
				yT = pary[1]*zz11y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clIa[ii+2], xT, yT, &xr, &yr, xMC, yMC);
				yf.push_back(yr);
				zyf.push_back(zz11y[ii]);
			}
		}
		else{
			if(clIa[ii+2]!=-1){
				xT = parx[1]*zz11y[ii] + parx[0];
				yT = pary[1]*zz11y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clIa[ii+2], xT, yT, &xr, &yr, xMC, yMC);
				xf.push_back(xr);
				zf.push_back(zz11x[ii]);
				yf.push_back(yr);
				zyf.push_back(zz11y[ii]);
			}
		}
	}

	for(Int_t ii=0;ii<6;ii++){
		if(ii<2){
			if(clIb[ii]!=-1 && clIb[ii+2]!=-1){
				xT = parx[1]*zz21x[ii] + parx[0];
				yT = pary[1]*zz21x[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz21x[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz21x[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clIb[ii], xT, yT, &xr, &yr, xMC, yMC);
				xf.push_back(xr);
				zf.push_back(zz21x[ii]);

				xT = parx[1]*zz21y[ii] + parx[0];
				yT = pary[1]*zz21y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz21y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz21y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clIb[ii+2], xT, yT, &xr, &yr, xMC, yMC);
				yf.push_back(yr);
				zyf.push_back(zz21y[ii]);
			}
		}
		else{
			if(clIb[ii+2]!=-1){
				xT = parx[1]*zz21y[ii] + parx[0];
				yT = pary[1]*zz21y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz21y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz21y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clIb[ii+2], xT, yT, &xr, &yr, xMC, yMC);
				xf.push_back(xr);
				zf.push_back(zz21x[ii]);
				yf.push_back(yr);
				zyf.push_back(zz21y[ii]);
			}
		}
	}
	Double_t xx11[xf.size()];
	Double_t yy11[yf.size()];
	Double_t zzx11[zf.size()];
	Double_t zzy11[zyf.size()];
	std::copy(xf.begin(), xf.end(), xx11);
	std::copy(yf.begin(), yf.end(), yy11);
	std::copy(zf.begin(), zf.end(), zzx11);
	std::copy(zyf.begin(), zyf.end(), zzy11);
	
	Int_t sizeA = zf.size();
	if(zyf.size()<zf.size())
		sizeA=zyf.size();
		
	FitWLSQ *fit11x = new FitWLSQ(zzx11,0.1,10*0.1, 0.9, (int) zf.size(),2,false,false,6);
	FitWLSQ *fit11y = new FitWLSQ(zzy11,0.1,10*0.1, 0.9, (int) zyf.size(),2,false,false,6);
	//FitWLSQ *fit11x = new FitWLSQ(zzx11,0.03,10*0.02, 0.9, (int) zf.size(),2,true,true,3);
	//FitWLSQ *fit11y = new FitWLSQ(zzy11,0.03,10*0.02, 0.9, (int) zyf.size(),2,true,true,3);
	if(fit11x->Fit(xx11) && fit11y->Fit(yy11)){
		*chi2x = fit11x->WLSQRms(xx11);
		*chi2y = fit11y->WLSQRms(yy11);
		parx1[0]=fit11x->param[0];
		parx1[1]=fit11x->param[1];
		pary1[0]=fit11y->param[0];
		pary1[1]=fit11y->param[1];
		if(fillHisto && histoOutput){
			FITXRMS2->Fill(fit11x->WLSQRms(xx11));
			FITYRMS2->Fill(fit11y->WLSQRms(yy11));
			for(Int_t ii=0; ii<sizeA; ii++){
				if(fit11x->wrb[ii]!=0) {
					FITXResid2->Fill(fit11x->param[1]*zzx11[ii] + fit11x->param[0] - xx11[ii]);
				}
				if(fit11y->wrb[ii]!=0) {
					FITYResid2->Fill(fit11y->param[1]*zzy11[ii] + fit11y->param[0] - yy11[ii]);
					if(ii==0 || ii==1 || ii==6 || ii==7)
						FITYResidy->Fill(fit11y->param[1]*zzy11[ii] + fit11y->param[0] - yy11[ii]);
					if(ii==2 || ii==3 || ii==8 || ii==9)
						FITYResidu->Fill(fit11y->param[1]*zzy11[ii] + fit11y->param[0] - yy11[ii]);
					if(ii==4 || ii==5 || ii==10 || ii==11)
						FITYResidv->Fill(fit11y->param[1]*zzy11[ii] + fit11y->param[0] - yy11[ii]);
				}
				if(!expData){
					Double_t xTr;
					Double_t yTr;
					Double_t zTrx;
					Double_t zTry;
				
					zTrx = zzx11[ii];
					zTry = zzy11[ii];
					xTr=pnt1->GetX()+(zTrx-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yTr=pnt1->GetY()+(zTry-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
					if(fit11x->wrb[ii]!=0){
						FITXMCResid2->Fill(fit11x->param[1]*zzx11[ii] + fit11x->param[0] - xTr);
					}
					if(fit11y->wrb[ii]!=0){
						FITYMCResid2->Fill(fit11y->param[1]*zzy11[ii] + fit11y->param[0] - yTr);
					}
					if(fit11x->wrb[ii]!=0)
						XResid2->Fill(xx11[ii] - xTr);
					if(fit11y->wrb[ii]!=0)
						YResid2->Fill(yy11[ii] - yTr);
				}
				Double_t dt=0;
				Double_t mdist=0;
				switch (ii){
					case 0 : 
						dt = digisDCH[clIa[0]]->GetTime();    
						mdist=GetDistance(digisDCH[clIa[0]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
								fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIa[0]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clIa[0]]->GetTime(),digisDCH[clIa[0]]->GetPlane())-mdist)<0.1)
								rtg->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtrg[GetRTIndex(dt)]->Fill(mdist);
						}
						dt = digisDCH[clIa[2]]->GetTime();    
						mdist=GetDistance(digisDCH[clIa[2]]->GetPlane(), fit11x->param[1]*zzy11[ii] + fit11x->param[0], 
								fit11y->param[1]*zzy11[ii] + fit11y->param[0], digisDCH[clIa[2]]->GetWireNumber()); 
						if(fit11y->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clIa[2]]->GetTime(),digisDCH[clIa[2]]->GetPlane())-mdist)<0.1)
								rtg->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtrg[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 1 : 
						dt = digisDCH[clIa[1]]->GetTime();    
						mdist=GetDistance(digisDCH[clIa[1]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
								fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIa[1]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clIa[1]]->GetTime(),digisDCH[clIa[1]]->GetPlane())-mdist)<0.1)
								rtg->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtrg[GetRTIndex(dt)]->Fill(mdist);
						}
						dt = digisDCH[clIa[3]]->GetTime();    
						mdist=GetDistance(digisDCH[clIa[3]]->GetPlane(), fit11x->param[1]*zzy11[ii] + fit11x->param[0], 
								fit11y->param[1]*zzy11[ii] + fit11y->param[0], digisDCH[clIa[3]]->GetWireNumber()); 
						if(fit11y->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clIa[3]]->GetTime(),digisDCH[clIa[3]]->GetPlane())-mdist)<0.1)
								rtg->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtrg[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 2 : 
						dt = digisDCH[clIa[4]]->GetTime();    
						mdist=GetDistance(digisDCH[clIa[4]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
								fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIa[4]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clIa[4]]->GetTime(),digisDCH[clIa[4]]->GetPlane())-mdist)<0.1)
								rtg->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtrg[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 3 : 
						dt = digisDCH[clIa[5]]->GetTime();    
						mdist=GetDistance(digisDCH[clIa[5]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
								fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIa[5]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clIa[5]]->GetTime(),digisDCH[clIa[5]]->GetPlane())-mdist)<0.1)
								rtg->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtrg[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 4 : 
						dt = digisDCH[clIa[6]]->GetTime();    
						mdist=GetDistance(digisDCH[clIa[6]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
								fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIa[6]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clIa[6]]->GetTime(),digisDCH[clIa[6]]->GetPlane())-mdist)<0.1)
								rtg->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtrg[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 5 : 
						dt = digisDCH[clIa[7]]->GetTime();    
						mdist=GetDistance(digisDCH[clIa[7]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
								fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIa[7]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clIa[7]]->GetTime(),digisDCH[clIa[7]]->GetPlane())-mdist)<0.1)
								rtg->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtrg[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 6 : 
						if(clIb[0]!=-1){
							dt = digisDCH[clIb[0]]->GetTime();    
							mdist=GetDistance(digisDCH[clIb[0]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
									fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIb[0]]->GetWireNumber()); 
							if(fit11x->wrb[ii]!=0){
								if(Abs(GetDDistance(digisDCH[clIb[0]]->GetTime(),digisDCH[clIb[0]]->GetPlane())-mdist)<0.1)
									rtg->Fill(dt,mdist);
								if(GetRTIndex(dt)<115)
									rtrg[GetRTIndex(dt)]->Fill(mdist);
							}
						}
						if(clIb[2]!=-1){
							dt = digisDCH[clIb[2]]->GetTime();    
							mdist=GetDistance(digisDCH[clIb[2]]->GetPlane(), fit11x->param[1]*zzy11[ii] + fit11x->param[0], 
									fit11y->param[1]*zzy11[ii] + fit11y->param[0], digisDCH[clIb[2]]->GetWireNumber()); 
							if(fit11y->wrb[ii]!=0){
								if(Abs(GetDDistance(digisDCH[clIb[2]]->GetTime(),digisDCH[clIb[2]]->GetPlane())-mdist)<0.1)
									rtg->Fill(dt,mdist);
								if(GetRTIndex(dt)<115)
									rtrg[GetRTIndex(dt)]->Fill(mdist);
							}
						}
						break;
					case 7 : 
						if(clIb[1]!=-1){
							dt = digisDCH[clIb[1]]->GetTime();    
							mdist=GetDistance(digisDCH[clIb[1]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
									fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIb[1]]->GetWireNumber()); 
							if(fit11x->wrb[ii]!=0){
								if(Abs(GetDDistance(digisDCH[clIb[1]]->GetTime(),digisDCH[clIb[1]]->GetPlane())-mdist)<0.1)
									rtg->Fill(dt,mdist);
								if(GetRTIndex(dt)<115)
									rtrg[GetRTIndex(dt)]->Fill(mdist);
							}
						}
						if(clIb[3]!=-1){
							dt = digisDCH[clIb[3]]->GetTime();    
							mdist=GetDistance(digisDCH[clIb[3]]->GetPlane(), fit11x->param[1]*zzy11[ii] + fit11x->param[0], 
									fit11y->param[1]*zzy11[ii] + fit11y->param[0], digisDCH[clIb[3]]->GetWireNumber()); 
							if(fit11y->wrb[ii]!=0){
								if(Abs(GetDDistance(digisDCH[clIb[3]]->GetTime(),digisDCH[clIb[3]]->GetPlane())-mdist)<0.1)
									rtg->Fill(dt,mdist);
								if(GetRTIndex(dt)<115)
									rtrg[GetRTIndex(dt)]->Fill(mdist);
							}
						}
						break;
					case 8 : 
						if(clIb[4]!=-1){
							dt = digisDCH[clIb[4]]->GetTime();    
							mdist=GetDistance(digisDCH[clIb[4]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
									fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIb[4]]->GetWireNumber()); 
							if(fit11x->wrb[ii]!=0){
								if(Abs(GetDDistance(digisDCH[clIb[4]]->GetTime(),digisDCH[clIb[4]]->GetPlane())-mdist)<0.1)
									rtg->Fill(dt,mdist);
								if(GetRTIndex(dt)<115)
									rtrg[GetRTIndex(dt)]->Fill(mdist);
							}
						}
						break;
					case 9 : 
						if(clIb[5]!=-1){
							dt = digisDCH[clIb[5]]->GetTime();    
							mdist=GetDistance(digisDCH[clIb[5]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
									fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIb[5]]->GetWireNumber()); 
							if(fit11x->wrb[ii]!=0){
								if(Abs(GetDDistance(digisDCH[clIb[5]]->GetTime(),digisDCH[clIb[5]]->GetPlane())-mdist)<0.1)
									rtg->Fill(dt,mdist);
								if(GetRTIndex(dt)<115)
									rtrg[GetRTIndex(dt)]->Fill(mdist);
							}
						}
						break;
					case 10 : 
						if(clIb[6]!=-1){
							dt = digisDCH[clIb[6]]->GetTime();    
							mdist=GetDistance(digisDCH[clIb[6]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
									fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIb[6]]->GetWireNumber()); 
							if(fit11x->wrb[ii]!=0){
								if(Abs(GetDDistance(digisDCH[clIb[6]]->GetTime(),digisDCH[clIb[6]]->GetPlane())-mdist)<0.1)
									rtg->Fill(dt,mdist);
								if(GetRTIndex(dt)<115)
									rtrg[GetRTIndex(dt)]->Fill(mdist);
							}
						}
						break;
					case 11 : 
						if(clIb[7]!=-1){
							dt = digisDCH[clIb[7]]->GetTime();    
							mdist=GetDistance(digisDCH[clIb[7]]->GetPlane(), fit11x->param[1]*zzx11[ii] + fit11x->param[0], 
									fit11y->param[1]*zzx11[ii] + fit11y->param[0], digisDCH[clIb[7]]->GetWireNumber()); 
							if(fit11x->wrb[ii]!=0){
								if(Abs(GetDDistance(digisDCH[clIb[7]]->GetTime(),digisDCH[clIb[7]]->GetPlane())-mdist)<0.1)
									rtg->Fill(dt,mdist);
								if(GetRTIndex(dt)<115)
									rtrg[GetRTIndex(dt)]->Fill(mdist);
							}
						}
						break;
				}					  
			}
		}
		delete fit11x;
		delete fit11y;
		return true;
	}
	if(!fit11x->Status && fillHisto && histoOutput){
		Problems->Fill(sp);
	}
	if(!fit11y->Status && fillHisto && histoOutput){
		Problems->Fill(sp+1);
	}
	delete fit11x;
	delete fit11y;
	return false;
}

void BmnDchTrackFinder::InitialHisto(Int_t* clI, Double_t* parx, Double_t* pary, Double_t *zz11x, Double_t *zz11y, FairMCPoint* pnt1, FairMCPoint* pnt2){
	Double_t xT; 
	Double_t yT;
	Double_t xMC=0.; 
	Double_t yMC=0.;
	Double_t xr; 
	Double_t yr;

	vector<Double_t> xf; 
	vector<Double_t> yf; 
	vector<Double_t> zf; 
	vector<Double_t> zyf; 
	
	for(Int_t ii=0;ii<6;ii++){
		if(clI[ii]!=-1 && clI[ii+2]!=-1){
			if(ii<2){
				xT = parx[1]*zz11x[ii] + parx[0];
				yT = pary[1]*zz11x[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11x[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11x[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clI[ii], xT, yT, &xr, &yr, xMC, yMC);
				xf.push_back(xr);
				zf.push_back(zz11x[ii]);

				xT = parx[1]*zz11y[ii] + parx[0];
				yT = pary[1]*zz11y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clI[ii+2], xT, yT, &xr, &yr, xMC, yMC);
				yf.push_back(yr);
				zyf.push_back(zz11y[ii]);
			}
			else{
				xT = parx[1]*zz11y[ii] + parx[0];
				yT = pary[1]*zz11y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clI[ii+2], xT, yT, &xr, &yr, xMC, yMC);
				xf.push_back(xr);
				zf.push_back(zz11x[ii]);
				yf.push_back(yr);
				zyf.push_back(zz11y[ii]);
			}
		}
	}

	Double_t xx11[xf.size()];
	Double_t yy11[yf.size()];
	Double_t zzx11[zf.size()];
	Double_t zzy11[zyf.size()];
	std::copy(xf.begin(), xf.end(), xx11);
	std::copy(yf.begin(), yf.end(), yy11);
	std::copy(zf.begin(), zf.end(), zzx11);
	std::copy(zyf.begin(), zyf.end(), zzy11);
	
	Int_t sizeA = zf.size();
	if(zyf.size()<zf.size())
		sizeA=zyf.size();
			
	for(Int_t ii=0; ii<sizeA; ii++){
		if(histoOutput){
			FITXResid->Fill(parx[1]*zz11x[ii] + parx[0] - xx11[ii]);
			FITYResid->Fill(pary[1]*zz11y[ii] + pary[0] - yy11[ii]);
		}
		if(!expData){
			Double_t xTr;
			Double_t yTr;
			Double_t zTrx;
			Double_t zTry;
				
			zTrx = zz11x[ii];
			zTry = zz11y[ii];
			xTr=pnt1->GetX()+(zTrx-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
			yTr=pnt1->GetY()+(zTry-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
			if(histoOutput){
				FITXMCResid->Fill(parx[1]*zz11x[ii] + parx[0] - xTr);
				FITYMCResid->Fill(pary[1]*zz11y[ii] + pary[0] - yTr);
				XResid->Fill(xx11[ii] - xTr);
				YResid->Fill(yy11[ii] - yTr);
			}
		}
	}
}

Bool_t BmnDchTrackFinder::FitItera(Int_t* clI, Double_t* parx, Double_t* pary, Double_t* parx1, Double_t* pary1, Double_t *zz11x, Double_t *zz11y, Bool_t fillHisto, FairMCPoint* pnt1, FairMCPoint* pnt2, Int_t sp, Double_t *chi2x, Double_t *chi2y){
	Double_t xT; 
	Double_t yT;
	Double_t xMC=0.; 
	Double_t yMC=0.;
	Double_t xr; 
	Double_t yr;

	vector<Double_t> xf; 
	vector<Double_t> yf; 
	vector<Double_t> zf; 
	vector<Double_t> zyf; 
	
	for(Int_t ii=0;ii<6;ii++){
		if(clI[ii]!=-1 && clI[ii+2]!=-1){
			if(ii<2){
				xT = parx[1]*zz11x[ii] + parx[0];
				yT = pary[1]*zz11x[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11x[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11x[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clI[ii], xT, yT, &xr, &yr, xMC, yMC);
				xf.push_back(xr);
				zf.push_back(zz11x[ii]);

				xT = parx[1]*zz11y[ii] + parx[0];
				yT = pary[1]*zz11y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clI[ii+2], xT, yT, &xr, &yr, xMC, yMC);
				yf.push_back(yr);
				zyf.push_back(zz11y[ii]);
			}
			else{
				xT = parx[1]*zz11y[ii] + parx[0];
				yT = pary[1]*zz11y[ii] + pary[0]; 
				if(!expData) {
					xMC=pnt1->GetX()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yMC=pnt1->GetY()+(zz11y[ii]-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
				}
				GetTrWPoints(clI[ii+2], xT, yT, &xr, &yr, xMC, yMC);
				xf.push_back(xr);
				zf.push_back(zz11x[ii]);
				yf.push_back(yr);
				zyf.push_back(zz11y[ii]);
			}
		}
	}

	Double_t xx11[xf.size()];
	Double_t yy11[yf.size()];
	Double_t zzx11[zf.size()];
	Double_t zzy11[zyf.size()];
	std::copy(xf.begin(), xf.end(), xx11);
	std::copy(yf.begin(), yf.end(), yy11);
	std::copy(zf.begin(), zf.end(), zzx11);
	std::copy(zyf.begin(), zyf.end(), zzy11);
	
	Int_t sizeA = zf.size();
	if(zyf.size()<zf.size())
		sizeA=zyf.size();
	
	FitWLSQ *fit11x = new FitWLSQ(zzx11,0.1,10*0.1, 0.9, (int) zf.size(),2,false,false,6);
	FitWLSQ *fit11y = new FitWLSQ(zzy11,0.1,10*0.1, 0.9, (int) zyf.size(),2,false,false,6);
	//FitWLSQ *fit11x = new FitWLSQ(zzx11,0.03,10*0.03, 0.9, (int) zf.size(),2,true,true,2);
	//FitWLSQ *fit11y = new FitWLSQ(zzy11,0.03,10*0.03, 0.9, (int) zyf.size(),2,true,true,2);
	if(fit11x->Fit(xx11) && fit11y->Fit(yy11)){
		*chi2x = fit11x->WLSQRms(xx11);
		*chi2y = fit11y->WLSQRms(yy11);
		parx1[0]=fit11x->param[0];
		parx1[1]=fit11x->param[1];
		pary1[0]=fit11y->param[0];
		pary1[1]=fit11y->param[1];
		if(fillHisto && histoOutput){
			FITXRMS1->Fill(fit11x->WLSQRms(xx11));
			FITYRMS1->Fill(fit11y->WLSQRms(yy11));
			for(Int_t ii=0; ii<sizeA; ii++){
				if(fit11x->wrb[ii]!=0)
					FITXResid1->Fill(fit11x->param[1]*zz11x[ii] + fit11x->param[0] - xx11[ii]);
				if(fit11y->wrb[ii]!=0)
					FITYResid1->Fill(fit11y->param[1]*zz11y[ii] + fit11y->param[0] - yy11[ii]);
				if(!expData){
					Double_t xTr;
					Double_t yTr;
					Double_t zTrx;
					Double_t zTry;
				
					zTrx = zz11x[ii];
					zTry = zz11y[ii];
					xTr=pnt1->GetX()+(zTrx-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
					yTr=pnt1->GetY()+(zTry-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
					if(fit11x->wrb[ii]!=0)
						FITXMCResid1->Fill(fit11x->param[1]*zz11x[ii] + fit11x->param[0] - xTr);
					if(fit11y->wrb[ii]!=0)
						FITYMCResid1->Fill(fit11y->param[1]*zz11y[ii] + fit11y->param[0] - yTr);
					if(fit11x->wrb[ii]!=0)
						XResid1->Fill(xx11[ii] - xTr);
					if(fit11y->wrb[ii]!=0)
						YResid1->Fill(yy11[ii] - yTr);
				}		  
				Double_t dt=0;
				Double_t mdist=0;
				switch (ii){
					case 0 : 
						dt = digisDCH[clI[0]]->GetTime();    
						mdist=GetDistance(digisDCH[clI[0]]->GetPlane(), fit11x->param[1]*zz11x[ii] + fit11x->param[0], 
								fit11y->param[1]*zz11x[ii] + fit11y->param[0], digisDCH[clI[0]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clI[0]]->GetTime(),digisDCH[clI[0]]->GetPlane())-mdist)<0.1)
								rt->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtr[GetRTIndex(dt)]->Fill(mdist);
						}
						dt = digisDCH[clI[2]]->GetTime();    
						mdist=GetDistance(digisDCH[clI[2]]->GetPlane(), fit11x->param[1]*zz11y[ii] + fit11x->param[0], 
								fit11y->param[1]*zz11y[ii] + fit11y->param[0], digisDCH[clI[2]]->GetWireNumber()); 
						if(fit11y->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clI[2]]->GetTime(),digisDCH[clI[2]]->GetPlane())-mdist)<0.1)
								rt->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtr[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 1 : 
						dt = digisDCH[clI[1]]->GetTime();    
						mdist=GetDistance(digisDCH[clI[1]]->GetPlane(), fit11x->param[1]*zz11x[ii] + fit11x->param[0], 
								fit11y->param[1]*zz11x[ii] + fit11y->param[0], digisDCH[clI[1]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clI[1]]->GetTime(),digisDCH[clI[1]]->GetPlane())-mdist)<0.1)
								rt->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtr[GetRTIndex(dt)]->Fill(mdist);
						}
						dt = digisDCH[clI[3]]->GetTime();    
						mdist=GetDistance(digisDCH[clI[3]]->GetPlane(), fit11x->param[1]*zz11y[ii] + fit11x->param[0], 
								fit11y->param[1]*zz11y[ii] + fit11y->param[0], digisDCH[clI[3]]->GetWireNumber()); 
						if(fit11y->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clI[3]]->GetTime(),digisDCH[clI[3]]->GetPlane())-mdist)<0.1)
								rt->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtr[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 2 : 
						dt = digisDCH[clI[4]]->GetTime();    
						mdist=GetDistance(digisDCH[clI[4]]->GetPlane(), fit11x->param[1]*zz11x[ii] + fit11x->param[0], 
								fit11y->param[1]*zz11x[ii] + fit11y->param[0], digisDCH[clI[4]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clI[4]]->GetTime(),digisDCH[clI[4]]->GetPlane())-mdist)<0.1)
								rt->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtr[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 3 : 
						dt = digisDCH[clI[5]]->GetTime();    
						mdist=GetDistance(digisDCH[clI[5]]->GetPlane(), fit11x->param[1]*zz11x[ii] + fit11x->param[0], 
								fit11y->param[1]*zz11x[ii] + fit11y->param[0], digisDCH[clI[5]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clI[5]]->GetTime(),digisDCH[clI[5]]->GetPlane())-mdist)<0.1)
								rt->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtr[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 4 : 
						dt = digisDCH[clI[6]]->GetTime();    
						mdist=GetDistance(digisDCH[clI[6]]->GetPlane(), fit11x->param[1]*zz11x[ii] + fit11x->param[0], 
								fit11y->param[1]*zz11x[ii] + fit11y->param[0], digisDCH[clI[6]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clI[6]]->GetTime(),digisDCH[clI[6]]->GetPlane())-mdist)<0.1)
								rt->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtr[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
					case 5 : 
						dt = digisDCH[clI[7]]->GetTime();    
						mdist=GetDistance(digisDCH[clI[7]]->GetPlane(), fit11x->param[1]*zz11x[ii] + fit11x->param[0], 
								fit11y->param[1]*zz11x[ii] + fit11y->param[0], digisDCH[clI[7]]->GetWireNumber()); 
						if(fit11x->wrb[ii]!=0){
							if(Abs(GetDDistance(digisDCH[clI[7]]->GetTime(),digisDCH[clI[7]]->GetPlane())-mdist)<0.1)
								rt->Fill(dt,mdist);
							if(GetRTIndex(dt)<115)
								rtr[GetRTIndex(dt)]->Fill(mdist);
						}
						break;
				}					  
			}
		}
		delete fit11x;
		delete fit11y;
		return true;
	}
	if(!fit11x->Status && fillHisto && histoOutput){
		Problems->Fill(sp);
	}
	if(!fit11y->Status && fillHisto && histoOutput){
		Problems->Fill(sp+1);
	}
	delete fit11x;
	delete fit11y;
	return false;
}

void BmnDchTrackFinder::GetClusterHitsArray(Cluster* cluster, Int_t* ret){
	for(Int_t iha=0;iha<cluster->hits.size();iha++){
        if(digisDCH[cluster->hits[iha]]->GetPlane()==xai[0] || digisDCH[cluster->hits[iha]]->GetPlane()==xai[1])
        	ret[0]=cluster->hits[iha];
        if(digisDCH[cluster->hits[iha]]->GetPlane()==xbi[0] || digisDCH[cluster->hits[iha]]->GetPlane()==xbi[1])
        	ret[1]=cluster->hits[iha];
        if(digisDCH[cluster->hits[iha]]->GetPlane()==yai[0] || digisDCH[cluster->hits[iha]]->GetPlane()==yai[1])
        	ret[2]=cluster->hits[iha];
        if(digisDCH[cluster->hits[iha]]->GetPlane()==ybi[0] || digisDCH[cluster->hits[iha]]->GetPlane()==ybi[1])
        	ret[3]=cluster->hits[iha];
        if(digisDCH[cluster->hits[iha]]->GetPlane()==uai[0] || digisDCH[cluster->hits[iha]]->GetPlane()==uai[1])
        	ret[4]=cluster->hits[iha];
        if(digisDCH[cluster->hits[iha]]->GetPlane()==ubi[0] || digisDCH[cluster->hits[iha]]->GetPlane()==ubi[1])
        	ret[5]=cluster->hits[iha];
        if(digisDCH[cluster->hits[iha]]->GetPlane()==vai[0] || digisDCH[cluster->hits[iha]]->GetPlane()==vai[1])
        	ret[6]=cluster->hits[iha];
        if(digisDCH[cluster->hits[iha]]->GetPlane()==vbi[0] || digisDCH[cluster->hits[iha]]->GetPlane()==vbi[1])
        	ret[7]=cluster->hits[iha];
	}
}
void BmnDchTrackFinder::RecoDCHCluster(){
  Point3DC p;
  //Combine a and b clusters 
	if(aC.size()==0){
		//cout << "empty A cluster" << endl;
		return;
	}
	if(bC.size()==0){
		//cout << "empty B cluster" << endl;
		return;
	}
	for(Int_t ia = 0; ia< aC.size(); ia++){
		//bubble_sort
		for (Int_t idx_i = 0; idx_i < aC[ia]->hits.size() - 1; idx_i++){
			for (std::size_t idx_j = 0; idx_j < aC[ia]->hits.size() - idx_i - 1; idx_j++){
				if (digisDCH[aC[ia]->hits[idx_j + 1]]->GetPlane() < digisDCH[aC[ia]->hits[idx_j]]->GetPlane()){
					std::swap(aC[ia]->hits[idx_j], aC[ia]->hits[idx_j + 1]);
				}
			}
		}
	}
	for(Int_t ib = 0; ib< bC.size(); ib++){
		//bubble_sort
		for (Int_t idx_i = 0; idx_i < bC[ib]->hits.size() - 1; idx_i++){
			for (std::size_t idx_j = 0; idx_j < bC[ib]->hits.size() - idx_i - 1; idx_j++){
				if (digisDCH[bC[ib]->hits[idx_j + 1]]->GetPlane() < digisDCH[bC[ib]->hits[idx_j]]->GetPlane()){
					std::swap(bC[ib]->hits[idx_j], bC[ib]->hits[idx_j + 1]);
				}
			}
		}
	}
    Double_t xc;
    Double_t yc;
    Double_t zz1[hitsArraySize];
	Double_t zz11[8];
	Double_t zz11x[6];
	Double_t zz11y[6];
	Double_t zz2[hitsArraySize];
	Double_t zz21x[6];
	Double_t zz21y[6];
	Double_t zz21[8];
	
    zz11[0] =  zl_glob[xai[0]] + DCH1_Zpos;
	zz11[1] =  zl_glob[xbi[0]] + DCH1_Zpos;
	zz11[2] =  zl_glob[yai[0]] + DCH1_Zpos;
	zz11[3] =  zl_glob[ybi[0]] + DCH1_Zpos;
	zz11[4] =  zl_glob[uai[0]] + DCH1_Zpos;
	zz11[5] =  zl_glob[ubi[0]] + DCH1_Zpos;
	zz11[6] =  zl_glob[vai[0]] + DCH1_Zpos;
	zz11[7] =  zl_glob[vbi[0]] + DCH1_Zpos;

	zz11x[0] =  zl_glob[xai[0]] + DCH1_Zpos;
	zz11x[1] =  zl_glob[xbi[0]] + DCH1_Zpos;
	zz11x[2] =  zl_glob[uai[0]] + DCH1_Zpos;
	zz11x[3] =  zl_glob[ubi[0]] + DCH1_Zpos;
	zz11x[4] =  zl_glob[vai[0]] + DCH1_Zpos;
	zz11x[5] =  zl_glob[vbi[0]] + DCH1_Zpos;
	zz11y[0] =  zl_glob[yai[0]] + DCH1_Zpos;
	zz11y[1] =  zl_glob[ybi[0]] + DCH1_Zpos;
	zz11y[2] =  zl_glob[uai[0]] + DCH1_Zpos;
	zz11y[3] =  zl_glob[ubi[0]] + DCH1_Zpos;
	zz11y[4] =  zl_glob[vai[0]] + DCH1_Zpos;
	zz11y[5] =  zl_glob[vbi[0]] + DCH1_Zpos;
	  
	zz1[0] =  zl_glob[xai[0]] + DCH1_Zpos + (zl_glob[yai[0]]-zl_glob[xai[0]])/2.;  //axy
	zz1[1] =  zl_glob[xbi[0]] + DCH1_Zpos + (zl_glob[ybi[0]]-zl_glob[xbi[0]])/2.;  //bxy
	zz1[2] =  zl_glob[yai[0]] + DCH1_Zpos + (zl_glob[uai[0]]-zl_glob[yai[0]])/2.;  //ayu
	zz1[3] =  zl_glob[ybi[0]] + DCH1_Zpos + (zl_glob[ubi[0]]-zl_glob[ybi[0]])/2.;  //byu
	zz1[4] =  zl_glob[uai[0]] + DCH1_Zpos + (zl_glob[vai[0]]-zl_glob[uai[0]])/2.;  //auv
	zz1[5] =  zl_glob[ubi[0]] + DCH1_Zpos + (zl_glob[vbi[0]]-zl_glob[ubi[0]])/2.;  //buv

    zz21[0] =  zl_glob[xai[1]] + DCH2_Zpos;
	zz21[1] =  zl_glob[xbi[1]] + DCH2_Zpos;
	zz21[2] =  zl_glob[yai[1]] + DCH2_Zpos;
	zz21[3] =  zl_glob[ybi[1]] + DCH2_Zpos;
	zz21[4] =  zl_glob[uai[1]] + DCH2_Zpos;
	zz21[5] =  zl_glob[ubi[1]] + DCH2_Zpos;
	zz21[6] =  zl_glob[vai[1]] + DCH2_Zpos;
	zz21[7] =  zl_glob[vbi[1]] + DCH2_Zpos;

	zz21x[0] =  zl_glob[xai[1]] + DCH2_Zpos;
	zz21x[1] =  zl_glob[xbi[1]] + DCH2_Zpos;
	zz21x[2] =  zl_glob[uai[1]] + DCH2_Zpos;
	zz21x[3] =  zl_glob[ubi[1]] + DCH2_Zpos;
	zz21x[4] =  zl_glob[vai[1]] + DCH2_Zpos;
	zz21x[5] =  zl_glob[vbi[1]] + DCH2_Zpos;
	zz21y[0] =  zl_glob[yai[1]] + DCH2_Zpos;
	zz21y[1] =  zl_glob[ybi[1]] + DCH2_Zpos;
	zz21y[2] =  zl_glob[uai[1]] + DCH2_Zpos;
	zz21y[3] =  zl_glob[ubi[1]] + DCH2_Zpos;
	zz21y[4] =  zl_glob[vai[1]] + DCH2_Zpos;
	zz21y[5] =  zl_glob[vbi[1]] + DCH2_Zpos;
	  
	zz2[0] =  zl_glob[xai[1]] + DCH2_Zpos + (zl_glob[yai[1]]-zl_glob[xai[1]])/2.;  //axy
	zz2[1] =  zl_glob[xbi[1]] + DCH2_Zpos + (zl_glob[ybi[1]]-zl_glob[xbi[1]])/2.;  //bxy
	zz2[2] =  zl_glob[yai[1]] + DCH2_Zpos + (zl_glob[uai[1]]-zl_glob[yai[1]])/2.;  //ayu
	zz2[3] =  zl_glob[ybi[1]] + DCH2_Zpos + (zl_glob[ubi[1]]-zl_glob[ybi[1]])/2.;  //byu
	zz2[4] =  zl_glob[uai[1]] + DCH2_Zpos + (zl_glob[vai[1]]-zl_glob[uai[1]])/2.; //auv
	zz2[5] =  zl_glob[ubi[1]] + DCH2_Zpos + (zl_glob[vbi[1]]-zl_glob[ubi[1]])/2;  //buv
	  
    fDchTracks->Delete();
	for(Int_t ia = 0; ia< aC.size(); ia++){     
		Int_t clI[8]={-1,};
		
		GetClusterHitsArray(aC[ia], clI);
	  
		FairMCPoint* pnt1;
		FairMCPoint* pnt2;
		Double_t mcmin=1000;
		Double_t mcmax=-1000;
		if(!expData) {
			Int_t iMCTrack = digisDCH[clI[0]]->GetRefId();
			for (Int_t iPoint = 0; iPoint < fBmnDchPointsArray->GetEntriesFast(); iPoint++) {
				FairMCPoint* pnt = (FairMCPoint*) fBmnDchPointsArray->At(iPoint);       
				if(pnt->GetTrackID()==iMCTrack){
					if(pnt->GetZ()<mcmin){
						mcmin=pnt->GetZ();
						pnt1= (FairMCPoint*) fBmnDchPointsArray->At(iPoint);
					}
					if(pnt->GetZ()>mcmax){
						mcmax=pnt->GetZ();
						pnt2= (FairMCPoint*) fBmnDchPointsArray->At(iPoint);
					}
				}
			}
		}
		
		Double_t parx[2],pary[2];
		if(CombInitial(clI, zz11, parx, pary)){
			if(histoOutput){
				LRQA(clI, parx, pary, zz11x, zz11y, pnt1, pnt2, 0);
				InitialHisto(clI, parx, pary, zz11x, zz11y, pnt1, pnt2);
			}
			for(Int_t i=0;i<8;i++){
				if(clI[i]==-1)
					continue;
				Double_t dt = digisDCH[clI[i]]->GetTime();    
				Double_t mdist=GetDistance(digisDCH[clI[i]]->GetPlane(), parx[1]*zz11[i] + parx[0], pary[1]*zz11[i] + pary[0], digisDCH[clI[i]]->GetWireNumber()); 
				if(Abs(GetDDistance(digisDCH[clI[i]]->GetTime(),digisDCH[clI[i]]->GetPlane())-mdist)<0.1 && histoOutput)
					rtI->Fill(dt,mdist);
			}

			Double_t parx1[2],pary1[2];
			Double_t chi2x,chi2y;
			Double_t chi2xc=0,chi2yc=0;
			Int_t iItera=0;
			do{
				iItera++;
				if(FitItera(clI, parx, pary, parx1, pary1, zz11x, zz11y, false, pnt1, pnt2, 7, &chi2x, &chi2y)){
					if((chi2x<0.00001 && chi2y<0.00001) || (Abs(chi2x-chi2xc)/chi2xc < 0.001 &&  Abs(chi2x-chi2xc)/chi2xc < 0.001) ){
						FitItera(clI, parx, pary, parx1, pary1, zz11x, zz11y, true, pnt1, pnt2, 7, &chi2x, &chi2y);
						LRQA(clI, parx, pary, zz11x, zz11y, pnt1, pnt2, 1);
						break;
					}
					if(iItera>50)
						break;
				}
				else{
					break;
				}
				parx[0]=parx1[0];
				parx[1]=parx1[1];
				pary[0]=pary1[0];
				pary[1]=pary1[1];
				chi2xc=chi2x;
				chi2yc=chi2y;
			}while(1);
			//if(iItera>50)
			//	cout << "Bad A Cluster Itera: " << iItera << " chi2x: " << chi2x << " " << chi2xc << " chi2y: " << chi2y << " " << chi2yc << " " << Abs(chi2x-chi2xc)/chi2xc << " " << Abs(chi2x-chi2xc)/chi2xc << endl;
			aC[ia]->parx[0]=parx1[0];
			aC[ia]->parx[1]=parx1[1];
			aC[ia]->pary[0]=pary1[0];
			aC[ia]->pary[1]=pary1[1];
		}
	}
	for(Int_t ib = 0; ib< bC.size(); ib++){
		
		Int_t clI[8]={-1,};	
		GetClusterHitsArray(bC[ib], clI);
		FairMCPoint* pnt1;
		FairMCPoint* pnt2;
		Double_t mcmin=1000;
		Double_t mcmax=-1000;
		if(!expData) {
			Int_t iMCTrack = digisDCH[clI[0]]->GetRefId();
			for (Int_t iPoint = 0; iPoint < fBmnDchPointsArray->GetEntriesFast(); iPoint++) {
				FairMCPoint* pnt = (FairMCPoint*) fBmnDchPointsArray->At(iPoint);       
				if(pnt->GetTrackID()==iMCTrack){
					if(pnt->GetZ()<mcmin){
						mcmin=pnt->GetZ();
						pnt1= (FairMCPoint*) fBmnDchPointsArray->At(iPoint);
					}
					if(pnt->GetZ()>mcmax){
						mcmax=pnt->GetZ();
						pnt2= (FairMCPoint*) fBmnDchPointsArray->At(iPoint);
					}
				}
			}
		}
		Double_t parx[2],pary[2];
		if(CombInitial(clI, zz21, parx, pary)){
		//if(InitialFit(clI, parx, pary, zz2, true, pnt1, pnt2, 5)){
			LRQA(clI, parx, pary, zz21x, zz21y, pnt1, pnt2, 0);
			InitialHisto(clI, parx, pary, zz11x, zz11y, pnt1, pnt2);
			////////////////////////////////////////////
			for(Int_t i=0;i<8;i++){
				if(clI[i]==-1)
					continue;
				Double_t dt = digisDCH[clI[i]]->GetTime();    
				Double_t mdist=GetDistance(digisDCH[clI[i]]->GetPlane(), parx[1]*zz21[i] + parx[0], pary[1]*zz21[i] + pary[0], digisDCH[clI[i]]->GetWireNumber()); 
				if(Abs(GetDDistance(digisDCH[clI[i]]->GetTime(),digisDCH[clI[i]]->GetPlane())-mdist)<0.1 && histoOutput)
					rtI->Fill(dt,mdist);
			}
			////////////////////////////////////////////
			Double_t parx1[2],pary1[2];
			Double_t chi2x,chi2y;
			Double_t chi2xc=0,chi2yc=0;
			Int_t iItera=0;
			do{
				iItera++;
				if(FitItera(clI, parx, pary, parx1, pary1, zz21x, zz21y, false, pnt1, pnt2, 9, &chi2x, &chi2y)){
					if((chi2x<0.00001 && chi2y<0.00001) || (Abs(chi2x-chi2xc)/chi2xc < 0.001 &&  Abs(chi2x-chi2xc)/chi2xc < 0.001) ){
						FitItera(clI, parx, pary, parx1, pary1, zz21x, zz21y, true, pnt1, pnt2, 9, &chi2x, &chi2y);
						LRQA(clI, parx, pary, zz21x, zz21y, pnt1, pnt2, 1);
						break;
					}
					if(iItera>50)
						break;
				}
				else{
					break;
				}
				parx[0]=parx1[0];
				parx[1]=parx1[1];
				pary[0]=pary1[0];
				pary[1]=pary1[1];
				chi2xc=chi2x;
				chi2yc=chi2y;
			}while(1);
			//if(iItera>50)
			//	cout << "Bad B Cluster Itera: " << iItera << " chi2x: " << chi2x << " " << chi2xc << " chi2y: " << chi2y << " " << chi2yc << " " << Abs(chi2x-chi2xc)/chi2xc << " " << Abs(chi2x-chi2xc)/chi2xc << endl;
			bC[ib]->parx[0]=parx1[0];
			bC[ib]->parx[1]=parx1[1];
			bC[ib]->pary[0]=pary1[0];
			bC[ib]->pary[1]=pary1[1];
		}
	}
	//Match segmets
	vector<BmnDchTrack*> vtrk;
	for(Int_t ia = 0; ia< aC.size(); ia++){     
		Double_t xmin=10000;
		Double_t ymin=10000;
		Double_t zm = (DCH2_Zpos - DCH1_Zpos)/2 + DCH1_Zpos;
		Double_t x1 = aC[ia]->parx[1]*zm + aC[ia]->parx[0];
		Double_t y1 = aC[ia]->pary[1]*zm + aC[ia]->pary[0];
		Int_t iBi=-1;
		for(Int_t ib = 0; ib< bC.size(); ib++){
			Double_t x2 = bC[ib]->parx[1]*zm + bC[ib]->parx[0];
			Double_t y2 = bC[ib]->pary[1]*zm + bC[ib]->pary[0];
			if(Abs(x2-x1)<Abs(xmin) && Abs(y2-y1)<Abs(ymin) && Abs(x2-x1)<matchCut && Abs(y2-y1)< matchCut){
				xmin = x2-x1;
				ymin = y2-y1;
				iBi=ib;
			}
		}
		if(histoOutput){
			if(xmin!=10000 && ymin!=10000){
				XResid4->Fill(xmin);
				YResid4->Fill(ymin);
			}
			if(iBi!=-1){
				if(Abs(xmin)<5 && Abs(ymin)<5){
					XResid3->Fill(xmin);
					YResid3->Fill(ymin);
				}
			}	
		}
		Int_t clIa[8]={-1,};	
		Int_t clIb[8]={-1,};	
		GetClusterHitsArray(aC[ia], clIa);
		if(iBi!=-1)
			GetClusterHitsArray(bC[iBi], clIb);		
			
		FairMCPoint* pnt1;
		FairMCPoint* pnt2;
		Double_t mcmin=1000;
		Double_t mcmax=-1000;
		if(!expData) {
			Int_t iMCTrack = digisDCH[clIa[0]]->GetRefId();
			for (Int_t iPoint = 0; iPoint < fBmnDchPointsArray->GetEntriesFast(); iPoint++) {
				FairMCPoint* pnt = (FairMCPoint*) fBmnDchPointsArray->At(iPoint);       
				if(pnt->GetTrackID()==iMCTrack){
					if(pnt->GetZ()<mcmin){
						mcmin=pnt->GetZ();
						pnt1= (FairMCPoint*) fBmnDchPointsArray->At(iPoint);
					}
					if(pnt->GetZ()>mcmax){
						mcmax=pnt->GetZ();
						pnt2= (FairMCPoint*) fBmnDchPointsArray->At(iPoint);
					}
				}
			}
		}
		Double_t parx[2],pary[2];
		Double_t parx1[2],pary1[2];
		Double_t chi2x,chi2y;
		Double_t chi2xc=0,chi2yc=0;
		Int_t iItera=0;
		parx[0]=aC[ia]->parx[0];
		parx[1]=aC[ia]->parx[1];
		pary[0]=aC[ia]->pary[0];
		pary[1]=aC[ia]->pary[1];
		do{
			iItera++;
			if(FitIteraGlobal(clIa, clIb, parx, pary, parx1, pary1, zz11x, zz11y, zz21x, zz21y, false, pnt1, pnt2, 13, &chi2x, &chi2y)){
				if((chi2x<0.00001 && chi2y<0.00001) || (Abs(chi2x-chi2xc)/chi2xc < 0.001 &&  Abs(chi2x-chi2xc)/chi2xc < 0.001) ){
					FitIteraGlobal(clIa, clIb, parx, pary, parx1, pary1, zz11x, zz11y, zz21x, zz21y, true, pnt1, pnt2, 13, &chi2x, &chi2y);
					LRQA(clIa, parx, pary, zz11x, zz11y, pnt1, pnt2, 2);
					LRQA(clIb, parx, pary, zz21x, zz21y, pnt1, pnt2, 2);
					break;
				}
				if(iItera>50)
					break;
			}
			else{
				break;
			}
			parx[0]=parx1[0];
			parx[1]=parx1[1];
			pary[0]=pary1[0];
			pary[1]=pary1[1];
			chi2xc=chi2x;
			chi2yc=chi2y;
		}while(1);
		//if(iItera>50)
		//	cout << "Bad Global Itera: " << iItera << " chi2x: " << chi2x << " " << chi2xc << " chi2y: " << chi2y << " " << chi2yc << " " << Abs(chi2x-chi2xc)/chi2xc << " " << Abs(chi2x-chi2xc)/chi2xc << endl;
		//else{
			BmnDchTrack* track = new BmnDchTrack();
			track->xozParameters.push_back(parx[0]);
			track->xozParameters.push_back(parx[1]);
			track->yozParameters.push_back(pary[0]);
			track->yozParameters.push_back(pary[1]);
			track->SetNHits(0);
			vtrk.push_back(track);
		//}
	}      
    //!!!??? вот это внимательно проверить !!!??? отсев гостов
    for (Int_t iDig = 0; iDig < fBmnDchDigitsArray->GetEntriesFast(); ++iDig) {
	    BmnDchDigit* digit = (BmnDchDigit*) fBmnDchDigitsArray->UncheckedAt(iDig);	    
	    Double_t mdist=1000;
	    Int_t minIdx=-1;
	    for(Int_t trIdx = 0; trIdx < vtrk.size(); ++trIdx) {
	    	BmnDchTrack* dchTr = vtrk[trIdx];
	    	Double_t zh=zl_glob[digit->GetPlane()] + DCH1_Zpos;
			if(digit->GetPlane()>7)
				zh=zl_glob[digit->GetPlane()] + DCH2_Zpos;
	    	Double_t dist=GetDistance(digit->GetPlane(), dchTr->xozParameters[1]*zh + dchTr->xozParameters[0], 
					dchTr->yozParameters[1]*zh + dchTr->yozParameters[0], digit->GetWireNumber());
	    	if(dist<mdist){// && dist<1.5){
	    		mdist=dist;
	    		minIdx=trIdx;
	    	}
	    }
	    if(minIdx!=-1){
	    	digit->SetRefId(minIdx);
	    	vtrk[minIdx]->SetNHits(vtrk[minIdx]->GetNHits()+1);
	    }
	}
	Int_t idTrack=0;
    //!!!??? вот это внимательно проверить !!!???
	for(Int_t trIdx = 0; trIdx < vtrk.size(); ++trIdx) {
    	if(vtrk[trIdx]->GetNHits()>7){
    	    for (Int_t iDig = 0; iDig < fBmnDchDigitsArray->GetEntriesFast(); ++iDig) {
    	    	BmnDchDigit* digit = (BmnDchDigit*) fBmnDchDigitsArray->UncheckedAt(iDig);
    	    	if(digit->GetRefId()==trIdx)
    	    		digit->SetRefId(idTrack);
    	    }
			new((*fDchTracks)[idTrack]) BmnDchTrack();
			BmnDchTrack* track = (BmnDchTrack*) fDchTracks->At(idTrack);
			track->xozParameters.push_back(vtrk[trIdx]->xozParameters[0]);
			track->xozParameters.push_back(vtrk[trIdx]->xozParameters[1]);
			track->yozParameters.push_back(vtrk[trIdx]->yozParameters[0]);
			track->yozParameters.push_back(vtrk[trIdx]->yozParameters[1]);
			track->xrms = vtrk[trIdx]->xrms;
			track->yrms = vtrk[trIdx]->yrms;
			track->SetNHits(vtrk[trIdx]->GetNHits());
    		idTrack++;
    	}
    }
}
Double_t BmnDchTrackFinder::LinePropagation(Double_t y1, Double_t x1, Double_t y2, Double_t x2, Double_t xs){
	return y1+(xs-x1)*((y2-y1)/(x2-x1));
}
void BmnDchTrackFinder::GetWirePairBE(Int_t idx1, Int_t idx2, Double_t* x1b, Double_t* x1e, Double_t* y1b, Double_t* y1e, Double_t* x2b, Double_t* x2e, Double_t* y2b, Double_t* y2e, Int_t lr){
	Int_t lr1,lr2;
	if(lr==0){
		lr1=-1;
		lr2=-1;
	}
    if(lr==1){
    	lr1=-1;
    	lr2=1;
    }
    if(lr==2){
    	lr1=1;
    	lr2=-1;
    }
    if(lr==3){
    	lr1=1;
    	lr2=1;
    }
	GetBEPoints(idx1,x1b,y1b,x1e,y1e, lr1);
	GetBEPoints(idx2,x2b,y2b,x2e,y2e, lr2);
}
void BmnDchTrackFinder::ClustersPreparationRT(){
    aC.clear();
	bC.clear();
	pairxa.clear();
	pairxb.clear();
	pairya.clear();
	pairyb.clear();
	pairua.clear();
	pairub.clear();
	pairva.clear();
	pairvb.clear();
	vector<RecoCluster*> rc1; 
	vector<RecoCluster*> rc2; 
	vector<RecoCluster*> rc1x; 
	vector<RecoCluster*> rc2x; 
	vector<RecoCluster*> rc1y; 
	vector<RecoCluster*> rc2y; 
	for(Int_t plane=0;plane<16;plane+=2){
		for(Int_t iDig=0;iDig<planes[plane].size(); iDig++){
			Int_t wire1 = digisDCH[planes[plane][iDig]]->GetWireNumber();
			for(Int_t iDig1=0;iDig1<planes[plane+1].size(); iDig1++){
				Int_t wire2 = digisDCH[planes[plane+1][iDig1]]->GetWireNumber();
				if(Abs(wire1-wire2)<2){
					if(plane==xai[0])
						pairxa.push_back(new PointPair(iDig,iDig1));
					if(plane==xai[1])
						pairxb.push_back(new PointPair(iDig,iDig1));
					if(plane==yai[0])
						pairya.push_back(new PointPair(iDig,iDig1));
					if(plane==yai[1])
						pairyb.push_back(new PointPair(iDig,iDig1));
					if(plane==uai[0])
						pairua.push_back(new PointPair(iDig,iDig1));
					if(plane==uai[1])
						pairub.push_back(new PointPair(iDig,iDig1));
					if(plane==vai[0])
						pairva.push_back(new PointPair(iDig,iDig1));
					if(plane==vai[1])
						pairvb.push_back(new PointPair(iDig,iDig1));
				}
			}	
		}		
	}	
	Double_t zx1,zx2,zy1,zy2,zu1,zu2,zv1,zv2;
    for(Int_t ix1 = 0; ix1<pairxa.size();ix1++){
    	Double_t x1b,x1e,y1b,y1e;
    	Double_t x2b,x2e,y2b,y2e;
    	Double_t mdiff=1000;
		Double_t x1bbk,x2bbk,y1bbk,y2bbk,zx1bk,zx2bk,xuabk,xubbk,yuabk,yubbk,zu1bk,zu2bk,xvabk,xvbbk,yvabk,yvbbk,zv1bk,zv2bk;
		Int_t lribk,lrubk,lrvbk,idx1x,idx2x,idx1u,idx2u,idx1v,idx2v;
    	for(Int_t lri=0;lri<4;lri++){
    		GetWirePairBE(planes[xai[0]][pairxa[ix1]->idx1], planes[xbi[0]][pairxa[ix1]->idx2], &x1b, &x1e, &y1b, &y1e, &x2b, &x2e, &y2b, &y2e, lri);
    		zx1 = zl_glob[digisDCH[planes[xai[0]][pairxa[ix1]->idx1]]->GetPlane()] + DCH1_Zpos;
    		zx2 = zl_glob[digisDCH[planes[xbi[0]][pairxa[ix1]->idx2]]->GetPlane()] + DCH1_Zpos;
    		for(Int_t iu1 = 0; iu1<pairua.size();iu1++){
    			for(Int_t lru=0;lru<4;lru++){
    	        	Double_t x1bu,x1eu,y1bu,y1eu;
    	        	Double_t x2bu,x2eu,y2bu,y2eu;
    	    		GetWirePairBE(planes[uai[0]][pairua[iu1]->idx1], planes[ubi[0]][pairua[iu1]->idx2], &x1bu, &x1eu, &y1bu, &y1eu, &x2bu, &x2eu, &y2bu, &y2eu, lru);
    	    		Int_t plane1 = digisDCH[planes[uai[0]][pairua[iu1]->idx1]]->GetPlane();
    	    	    Double_t zp1 = zl_glob[plane1] + DCH1_Zpos;
    	    	    zu1=zp1;
    	    	    Double_t xua = LinePropagation(x1b, zl_glob[xai[0]] + DCH1_Zpos, x2b, zl_glob[xbi[0]] + DCH1_Zpos, zp1);
    	    		Double_t yua = LinePropagation(y1bu,x1bu,y1eu,x1eu,xua);
    	    		if(Abs(xua)>150 && Abs(yua)>150)
    	    			continue;
					
					Int_t plane2 = digisDCH[planes[ubi[0]][pairua[iu1]->idx2]]->GetPlane();
    	    	    Double_t zp2 = zl_glob[plane2] + DCH1_Zpos;
    	    	    zu2=zp2;
    	    	    Double_t xub = LinePropagation(x1b, zl_glob[xai[0]] + DCH1_Zpos, x2b, zl_glob[xbi[0]] + DCH1_Zpos, zp2);
    	    	    Double_t yub = LinePropagation(y2bu,x2bu,y2eu,x2eu,xub);
    	    		if(Abs(xub)>150 && Abs(yub)>150)
    	    			continue;
    	    		
					for(Int_t iv1 = 0; iv1<pairva.size();iv1++){
    	    			for(Int_t lrv=0;lrv<4;lrv++){
    	    				Double_t x1bv,x1ev,y1bv,y1ev;
    	    				Double_t x2bv,x2ev,y2bv,y2ev;
    	    				GetWirePairBE(planes[vai[0]][pairva[iv1]->idx1], planes[vbi[0]][pairva[iv1]->idx2], &x1bv, &x1ev, &y1bv, &y1ev, &x2bv, &x2ev, &y2bv, &y2ev, lrv);
    	    				plane1 = digisDCH[planes[vai[0]][pairva[iv1]->idx1]]->GetPlane();
    	    				zp1 = zl_glob[plane1] + DCH1_Zpos;
    	    				zv1=zp1;
    	    				Double_t xva = LinePropagation(x1b, zl_glob[xai[0]] + DCH1_Zpos, x2b, zl_glob[xbi[0]] + DCH1_Zpos, zp1);
    	    				Double_t yva = LinePropagation(y1bv,x1bv,y1ev,x1ev,xva);
    	    	    		if(Abs(xva)>150 && Abs(yva)>150)
    	    	    			continue;

    	    				plane2 = digisDCH[planes[vbi[0]][pairva[iv1]->idx2]]->GetPlane();
    	    				zp2 = zl_glob[plane2] + DCH1_Zpos;
    	    				zv2=zp2;
    	    				Double_t xvb = LinePropagation(x1b, zl_glob[xai[0]] + DCH1_Zpos, x2b, zl_glob[xbi[0]] + DCH1_Zpos, zp2);
    	    				Double_t yvb = LinePropagation(y2bv,x2bv,y2ev,x2ev,xvb);
    	    	    		if(Abs(xvb)>150 && Abs(yvb)>150)
    	    	    			continue;

    	    	    		if(Abs((yva-yua)-(yvb-yub))<mdiff){
    	    	    			mdiff=Abs((yva-yua)-(yvb-yub)); 
    	    	    			
    	    	    			x1bbk=x1b;
    	    	    			x2bbk=x2b;
    	    	    			y1bbk=-1;
    	    	    			y2bbk=-1;
    	    	    			zx1bk=zx1;
    	    	    			zx2bk=zx2;
    	    	    			xuabk=xua;
    	    	    			xubbk=xub;
    	    	    			yuabk=yua;
    	    	    			yubbk=yub;
    	    	    			zu1bk=zu1;
    	    	    			zu2bk=zu2;
    	    	    			xvabk=xva;
    	    	    			xvbbk=xvb;
    	    	    			yvabk=yva;
    	    	    			yvbbk=yvb;
    	    	    			zv1bk=zv1;
    	    	    			zv2bk=zv2;
    	    	    			
    	    	    			lribk=lri;
    	    	    			lrubk=lru;
    	    	    			lrvbk=lrv;
    	    	    			idx1x=planes[xai[0]][pairxa[ix1]->idx1];
    	    	    			idx2x=planes[xbi[0]][pairxa[ix1]->idx2];
    	    	    			idx1u=planes[uai[0]][pairua[iu1]->idx1];
    	    	    			idx2u=planes[ubi[0]][pairua[iu1]->idx2];
    	    	    			idx1v=planes[vai[0]][pairva[iv1]->idx1];
    	    	    			idx2v=planes[vbi[0]][pairva[iv1]->idx2];
    	    	    		}
    	    			}
    	    		}
    	    	}
    		}
    	}
    	if(mdiff!=1000){
			RecoCluster *rc= new RecoCluster();
			rc->hits.push_back(new RecoPoint3D(x1bbk,y1bbk,zx1bk,idx1x,lribk));
			rc->hits.push_back(new RecoPoint3D(x2bbk,y2bbk,zx2bk,idx2x,lribk));
			rc->hits.push_back(new RecoPoint3D(xuabk,yuabk,zu1bk,idx1u,lrubk));
			rc->hits.push_back(new RecoPoint3D(xubbk,yubbk,zu2bk,idx2u,lrubk));
			rc->hits.push_back(new RecoPoint3D(xvabk,yvabk,zv1bk,idx1v,lrvbk));
			rc->hits.push_back(new RecoPoint3D(xvbbk,yvbbk,zv2bk,idx2v,lrvbk));
			rc1x.push_back(rc);
			if(histoOutput){
				dxu1a->Fill(x1bbk-xuabk);
				dxu1b->Fill(x2bbk-xubbk);
				dxv1a->Fill(x1bbk-xvabk);
				duv1a->Fill(xuabk-xvabk);
				dxuv1a->Fill((x1bbk-xuabk) -(xuabk-xvabk));
				dxyuv1a->Fill((y1bbk-yuabk) -(yuabk-yvabk));
				dxv1b->Fill(x2bbk-xvbbk);
				duv1b->Fill(xubbk-xvbbk);
				dxuv1b->Fill((x2bbk-xubbk) -(xubbk-xvbbk));
				dxyuv1b->Fill((y2bbk-yubbk) -(yubbk-yvbbk));
				xwired[0]->Fill(x1bbk);
				xwired[1]->Fill(x2bbk);
				xwired[4]->Fill(xuabk);
				xwired[5]->Fill(xubbk);
				xwired[6]->Fill(xvabk);
				xwired[7]->Fill(xvbbk);
				yxwired[4]->Fill(yuabk);
				yxwired[5]->Fill(yubbk);
				yxwired[6]->Fill(yvabk);
				yxwired[7]->Fill(yvbbk);
			}
    	}
	}

    for(Int_t ix1 = 0; ix1<pairya.size();ix1++){
    	Double_t x1b,x1e,y1b,y1e;
    	Double_t x2b,x2e,y2b,y2e;
    	Double_t mdiff=1000;
		Double_t x1bbk,x2bbk,y1bbk,y2bbk,zx1bk,zx2bk,xuabk,xubbk,yuabk,yubbk,zu1bk,zu2bk,xvabk,xvbbk,yvabk,yvbbk,zv1bk,zv2bk;
		Int_t lribk,lrubk,lrvbk,idx1x,idx2x,idx1u,idx2u,idx1v,idx2v;
    	for(Int_t lri=0;lri<4;lri++){
    		GetWirePairBE(planes[yai[0]][pairya[ix1]->idx1], planes[ybi[0]][pairya[ix1]->idx2], &x1b, &x1e, &y1b, &y1e, &x2b, &x2e, &y2b, &y2e, lri);
    		zy1 = zl_glob[digisDCH[planes[yai[0]][pairya[ix1]->idx1]]->GetPlane()] + DCH1_Zpos;
    		zy2 = zl_glob[digisDCH[planes[ybi[0]][pairya[ix1]->idx2]]->GetPlane()] + DCH1_Zpos;
    		for(Int_t iu1 = 0; iu1<pairua.size();iu1++){
    	    	for(Int_t lru=0;lru<4;lru++){
    	        	Double_t x1bu,x1eu,y1bu,y1eu;
    	        	Double_t x2bu,x2eu,y2bu,y2eu;
    	    		GetWirePairBE(planes[uai[0]][pairua[iu1]->idx1], planes[ubi[0]][pairua[iu1]->idx2], &x1bu, &x1eu, &y1bu, &y1eu, &x2bu, &x2eu, &y2bu, &y2eu, lru);
    	    		Int_t plane1 = digisDCH[planes[uai[0]][pairua[iu1]->idx1]]->GetPlane();
    	    	    Double_t zp1 = zl_glob[plane1] + DCH1_Zpos;
    	    	    zu1=zp1;
    	    	    Double_t yua = LinePropagation(y1b, zl_glob[yai[0]] + DCH1_Zpos, y2b, zl_glob[ybi[0]] + DCH1_Zpos, zp1);
    	    		Double_t xua = LinePropagation(x1bu,y1bu,x1eu,y1eu,yua);
    	    		if(Abs(xua)>150 && Abs(yua)>150)
    	    			continue;
    	    		Int_t plane2 = digisDCH[planes[ubi[0]][pairua[iu1]->idx2]]->GetPlane();
    	    	    Double_t zp2 = zl_glob[plane2] + DCH1_Zpos;
    	    	    zu2=zp2;
    	    	    Double_t yub = LinePropagation(y1b, zl_glob[yai[0]] + DCH1_Zpos, y2b, zl_glob[ybi[0]] + DCH1_Zpos, zp2);
    	    	    Double_t xub = LinePropagation(x2bu,y2bu,x2eu,y2eu,yub);
    	    		if(Abs(xub)>150 && Abs(yub)>150)
    	    			continue;
    	    		for(Int_t iv1 = 0; iv1<pairva.size();iv1++){
    	    			for(Int_t lrv=0;lrv<4;lrv++){
    	    				Double_t x1bv,x1ev,y1bv,y1ev;
    	    				Double_t x2bv,x2ev,y2bv,y2ev;
    	    				GetWirePairBE(planes[vai[0]][pairva[iv1]->idx1], planes[vbi[0]][pairva[iv1]->idx2], &x1bv, &x1ev, &y1bv, &y1ev, &x2bv, &x2ev, &y2bv, &y2ev, lrv);
    	    				plane1 = digisDCH[planes[vai[0]][pairva[iv1]->idx1]]->GetPlane();
    	    				zp1 = zl_glob[plane1] + DCH1_Zpos;
    	    				zv1=zp1;
    	    				Double_t yva = LinePropagation(y1b, zl_glob[yai[0]] + DCH1_Zpos, y2b, zl_glob[ybi[0]] + DCH1_Zpos, zp1);
    	    				Double_t xva = LinePropagation(x1bv,y1bv,x1ev,y1ev,yva);;
    	    	    		if(Abs(xva)>150 && Abs(yva)>150)
    	    	    			continue;
    	    				plane2 = digisDCH[planes[vbi[0]][pairva[iv1]->idx2]]->GetPlane();
    	    				zp2 = zl_glob[plane2] + DCH1_Zpos;
    	    				zv2=zp2;
    	    				Double_t yvb = LinePropagation(y1b, zl_glob[yai[0]] + DCH1_Zpos, y2b, zl_glob[ybi[0]] + DCH1_Zpos, zp2);
    	    				Double_t xvb = LinePropagation(x2bv,y2bv,x2ev,y2ev,yvb);
    	    	    		if(Abs(xvb)>150 && Abs(yvb)>150)
    	    	    			continue;
    	    	    		if(Abs((yva-yua)-(yvb-yub))<mdiff){
    	    	    			mdiff=Abs((yva-yua)-(yvb-yub));
    	    	    			
    	    	    			x1bbk=-1;
    	    	    			x2bbk=-1;
    	    	    			y1bbk=y1b;
    	    	    			y2bbk=y2b;
    	    	    			zx1bk=zx1;
    	    	    			zx2bk=zx2;
    	    	    			xuabk=xua;
    	    	    			xubbk=xub;
    	    	    			yuabk=yua;
    	    	    			yubbk=yub;
    	    	    			zu1bk=zu1;
    	    	    			zu2bk=zu2;
    	    	    			xvabk=xva;
    	    	    			xvbbk=xvb;
    	    	    			yvabk=yva;
    	    	    			yvbbk=yvb;
    	    	    			zv1bk=zv1;
    	    	    			zv2bk=zv2;
    	    	    			
    	    	    			lribk=lri;
    	    	    			lrubk=lru;
    	    	    			lrvbk=lrv;
    	    	    			idx1x=planes[yai[0]][pairya[ix1]->idx1];
    	    	    			idx2x=planes[ybi[0]][pairya[ix1]->idx2];
    	    	    			idx1u=planes[uai[0]][pairua[iu1]->idx1];
    	    	    			idx2u=planes[ubi[0]][pairua[iu1]->idx2];
    	    	    			idx1v=planes[vai[0]][pairva[iv1]->idx1];
    	    	    			idx2v=planes[vbi[0]][pairva[iv1]->idx2];
    	    	    		}
    	    			}
    	    		}
    	    	}
    		}
    	}
    	if(mdiff!=1000){
			RecoCluster *rc= new RecoCluster();
			rc->hits.push_back(new RecoPoint3D(x1bbk,y1bbk,zx1bk,idx1x,lribk));
			rc->hits.push_back(new RecoPoint3D(x2bbk,y2bbk,zx2bk,idx2x,lribk));
			rc->hits.push_back(new RecoPoint3D(xuabk,yuabk,zu1bk,idx1u,lrubk));
			rc->hits.push_back(new RecoPoint3D(xubbk,yubbk,zu2bk,idx2u,lrubk));
			rc->hits.push_back(new RecoPoint3D(xvabk,yvabk,zv1bk,idx1v,lrvbk));
			rc->hits.push_back(new RecoPoint3D(xvbbk,yvbbk,zv2bk,idx2v,lrvbk));
			rc1y.push_back(rc);
			if(histoOutput){
				dyu1a->Fill(y1bbk-yuabk);
				dyu1b->Fill(y2bbk-yubbk);
				dyv1a->Fill(y1bbk-yvabk);
				duv1a->Fill(yuabk-yvabk);
				dyuv1a->Fill((y1bbk-yuabk) - (yuabk-yvabk));
				dyxuv1a->Fill((x1bbk-xuabk) - (xuabk-xvabk));
				dyv1b->Fill(y2bbk-yvbbk);
				duv1b->Fill(yubbk-yvbbk);
				dyuv1b->Fill((y2bbk-yubbk)-(yubbk-yvbbk));
				dyxuv1b->Fill((x2bbk-xubbk)-(xubbk-xvbbk));
				ywired[2]->Fill(y1bbk);
				ywired[3]->Fill(y2bbk);
				ywired[4]->Fill(yuabk);
				ywired[5]->Fill(yubbk);
				ywired[6]->Fill(yvabk);
				ywired[7]->Fill(yvbbk);
				xywired[4]->Fill(xuabk);
				xywired[5]->Fill(xubbk);
				xywired[6]->Fill(xvabk);
				xywired[7]->Fill(xvbbk);   		
			}
    	}
	}
	for(Int_t ix1 = 0; ix1<pairxb.size();ix1++){
    	Double_t x1b,x1e,y1b,y1e;
    	Double_t x2b,x2e,y2b,y2e;
    	Double_t mdiff=1000;
		Double_t x1bbk,x2bbk,y1bbk,y2bbk,zx1bk,zx2bk,xuabk,xubbk,yuabk,yubbk,zu1bk,zu2bk,xvabk,xvbbk,yvabk,yvbbk,zv1bk,zv2bk;
		Int_t lribk,lrubk,lrvbk,idx1x,idx2x,idx1u,idx2u,idx1v,idx2v;
    	for(Int_t lri=0;lri<4;lri++){
    		GetWirePairBE(planes[xai[1]][pairxb[ix1]->idx1], planes[xbi[1]][pairxb[ix1]->idx2], &x1b, &x1e, &y1b, &y1e, &x2b, &x2e, &y2b, &y2e, lri);
    		zx1 = zl_glob[digisDCH[planes[xai[1]][pairxb[ix1]->idx1]]->GetPlane()] + DCH2_Zpos;
    		zx2 = zl_glob[digisDCH[planes[xbi[1]][pairxb[ix1]->idx2]]->GetPlane()] + DCH2_Zpos;
    		for(Int_t iu1 = 0; iu1<pairub.size();iu1++){
    	    	for(Int_t lru=0;lru<4;lru++){
    	        	Double_t x1bu,x1eu,y1bu,y1eu;
    	        	Double_t x2bu,x2eu,y2bu,y2eu;
    	    		GetWirePairBE(planes[uai[1]][pairub[iu1]->idx1], planes[ubi[1]][pairub[iu1]->idx2], &x1bu, &x1eu, &y1bu, &y1eu, &x2bu, &x2eu, &y2bu, &y2eu, lru);
    	    		Int_t plane1 = digisDCH[planes[uai[1]][pairub[iu1]->idx1]]->GetPlane();
    	    	    Double_t zp1 = zl_glob[plane1] + DCH2_Zpos;
    	    	    zu1=zp1;
    	    	    Double_t xua = LinePropagation(x1b, zl_glob[xai[1]] + DCH2_Zpos, x2b, zl_glob[xbi[1]] + DCH2_Zpos, zp1);
    	    	    Double_t yua = LinePropagation(y1bu,x1bu,y1eu,x1eu,xua);
    	    		if(Abs(xua)>150 && Abs(yua)>150)
    	    			continue;
    	    		Int_t plane2 = digisDCH[planes[ubi[1]][pairub[iu1]->idx2]]->GetPlane();
    	    	    Double_t zp2 = zl_glob[plane2] + DCH2_Zpos;
    	    	    zu2=zp2;
    	    	    Double_t xub = LinePropagation(x1b, zl_glob[xai[1]] + DCH2_Zpos, x2b, zl_glob[xbi[1]] + DCH2_Zpos, zp2);
    	    	    Double_t yub = LinePropagation(y2bu,x2bu,y2eu,x2eu,xub);
    	    		if(Abs(xub)>150 && Abs(yub)>150)
    	    			continue;
    	    		for(Int_t iv1 = 0; iv1<pairvb.size();iv1++){
    	    			for(Int_t lrv=0;lrv<4;lrv++){
    	    				Double_t x1bv,x1ev,y1bv,y1ev;
    	    				Double_t x2bv,x2ev,y2bv,y2ev;
    	    				GetWirePairBE(planes[vai[1]][pairvb[iv1]->idx1], planes[vbi[1]][pairvb[iv1]->idx2], &x1bv, &x1ev, &y1bv, &y1ev, &x2bv, &x2ev, &y2bv, &y2ev, lrv);
    	    				plane1 = digisDCH[planes[vai[1]][pairvb[iv1]->idx1]]->GetPlane();
    	    				zp1 = zl_glob[plane1] + DCH2_Zpos;
    	    				zv1=zp1;
    	    				Double_t xva = LinePropagation(x1b, zl_glob[xai[1]] + DCH2_Zpos, x2b, zl_glob[xbi[1]] + DCH2_Zpos, zp1);
    	    				Double_t yva = LinePropagation(y1bv,x1bv,y1ev,x1ev,xva);
    	    	    		if(Abs(xva)>150 && Abs(yva)>150)
    	    	    			continue;
    	    				plane2 = digisDCH[planes[vbi[1]][pairvb[iv1]->idx2]]->GetPlane();
    	    				zp2 = zl_glob[plane2] + DCH2_Zpos;
    	    				zv2=zp2;
    	    				Double_t xvb = LinePropagation(x1b, zl_glob[xai[1]] + DCH2_Zpos, x2b, zl_glob[xbi[1]] + DCH2_Zpos, zp2);
    	    				Double_t yvb = LinePropagation(y2bv,x2bv,y2ev,x2ev,xvb);
    	    	    		if(Abs(xvb)>150 && Abs(yvb)>150)
    	    	    			continue;
    	    	    		if(Abs((yva-yua)-(yvb-yub))<mdiff){
    	    	    			mdiff=Abs((yva-yua)-(yvb-yub));
    	    	    			
    	    	    			x1bbk=x1b;
    	    	    			x2bbk=x2b;
    	    	    			y1bbk=-1;
    	    	    			y2bbk=-1;
    	    	    			zx1bk=zx1;
    	    	    			zx2bk=zx2;
    	    	    			xuabk=xua;
    	    	    			xubbk=xub;
    	    	    			yuabk=yua;
    	    	    			yubbk=yub;
    	    	    			zu1bk=zu1;
    	    	    			zu2bk=zu2;
    	    	    			xvabk=xva;
    	    	    			xvbbk=xvb;
    	    	    			yvabk=yva;
    	    	    			yvbbk=yvb;
    	    	    			zv1bk=zv1;
    	    	    			zv2bk=zv2;
    	    	    			
    	    	    			lribk=lri;
    	    	    			lrubk=lru;
    	    	    			lrvbk=lrv;
    	    	    			idx1x=planes[xai[1]][pairxb[ix1]->idx1];
    	    	    			idx2x=planes[xbi[1]][pairxb[ix1]->idx2];
    	    	    			idx1u=planes[uai[1]][pairub[iu1]->idx1];
    	    	    			idx2u=planes[ubi[1]][pairub[iu1]->idx2];
    	    	    			idx1v=planes[vai[1]][pairvb[iv1]->idx1];
    	    	    			idx2v=planes[vbi[1]][pairvb[iv1]->idx2];
    	    	    		}
    	    			}
    	    		}
    	    	}
    		}
    	}
    	if(mdiff!=1000){
			RecoCluster *rc= new RecoCluster();
			rc->hits.push_back(new RecoPoint3D(x1bbk,y1bbk,zx1bk,idx1x,lribk));
			rc->hits.push_back(new RecoPoint3D(x2bbk,y2bbk,zx2bk,idx2x,lribk));
			rc->hits.push_back(new RecoPoint3D(xuabk,yuabk,zu1bk,idx1u,lrubk));
			rc->hits.push_back(new RecoPoint3D(xubbk,yubbk,zu2bk,idx2u,lrubk));
			rc->hits.push_back(new RecoPoint3D(xvabk,yvabk,zv1bk,idx1v,lrvbk));
			rc->hits.push_back(new RecoPoint3D(xvbbk,yvbbk,zv2bk,idx2v,lrvbk));
			rc2x.push_back(rc);
			if(histoOutput){
				dxu2a->Fill(x1bbk-xuabk);
				dxu2b->Fill(x2bbk-xubbk);
				dxv2a->Fill(x1bbk-xvabk);
				duv2a->Fill(xuabk-xvabk);
				dxv2b->Fill(x2bbk-xvbbk);
				duv2b->Fill(xubbk-xvbbk);
				xwired[8]->Fill(x1bbk);
				xwired[9]->Fill(x2bbk);
				xwired[12]->Fill(xuabk);
				xwired[13]->Fill(xubbk);
				xwired[14]->Fill(xvabk);
				xwired[15]->Fill(xvbbk);
				yxwired[12]->Fill(yuabk);
				yxwired[13]->Fill(yubbk);
				yxwired[14]->Fill(yvabk);
				yxwired[15]->Fill(yvbbk); 
			}
    	}
	}
	for(Int_t ix1 = 0; ix1<pairyb.size();ix1++){
    	Double_t x1b,x1e,y1b,y1e;
    	Double_t x2b,x2e,y2b,y2e;
    	Double_t mdiff=1000;
		Double_t x1bbk,x2bbk,y1bbk,y2bbk,zx1bk,zx2bk,xuabk,xubbk,yuabk,yubbk,zu1bk,zu2bk,xvabk,xvbbk,yvabk,yvbbk,zv1bk,zv2bk;
		Int_t lribk,lrubk,lrvbk,idx1x,idx2x,idx1u,idx2u,idx1v,idx2v;
    	for(Int_t lri=0;lri<4;lri++){
    		GetWirePairBE(planes[yai[1]][pairyb[ix1]->idx1], planes[ybi[1]][pairyb[ix1]->idx2], &x1b, &x1e, &y1b, &y1e, &x2b, &x2e, &y2b, &y2e, lri);
    		zy1 = zl_glob[digisDCH[planes[yai[1]][pairyb[ix1]->idx1]]->GetPlane()] + DCH2_Zpos;
    		zy2 = zl_glob[digisDCH[planes[ybi[1]][pairyb[ix1]->idx2]]->GetPlane()] + DCH2_Zpos;
    		for(Int_t iu1 = 0; iu1<pairub.size();iu1++){
    	    	for(Int_t lru=0;lru<4;lru++){
    	        	Double_t x1bu,x1eu,y1bu,y1eu;
    	        	Double_t x2bu,x2eu,y2bu,y2eu;
    	    		GetWirePairBE(planes[uai[1]][pairub[iu1]->idx1], planes[ubi[1]][pairub[iu1]->idx2], &x1bu, &x1eu, &y1bu, &y1eu, &x2bu, &x2eu, &y2bu, &y2eu, lru);
    	    		Int_t plane1 = digisDCH[planes[uai[1]][pairub[iu1]->idx1]]->GetPlane();
    	    	    Double_t zp1 = zl_glob[plane1] + DCH2_Zpos;
    	    	    zu1=zp1;
    	    	    Double_t yua = LinePropagation(y1b, zl_glob[yai[1]] + DCH2_Zpos, y2b, zl_glob[ybi[1]] + DCH2_Zpos, zp1);
    	    		Double_t xua = LinePropagation(x1bu,y1bu,x1eu,y1eu,yua);
    	    		if(Abs(xua)>150 && Abs(yua)>150)
    	    			continue;
    	    		Int_t plane2 = digisDCH[planes[ubi[1]][pairub[iu1]->idx2]]->GetPlane();
    	    	    Double_t zp2 = zl_glob[plane2] + DCH2_Zpos;
    	    	    zu2=zp2;
    	    	    Double_t yub = LinePropagation(y1b, zl_glob[yai[1]] + DCH2_Zpos, y2b, zl_glob[ybi[1]] + DCH2_Zpos, zp2);
    	    	    Double_t xub = LinePropagation(x2bu,y2bu,x2eu,y2eu,yub);
    	    		if(Abs(xub)>150 && Abs(yub)>150)
    	    			continue;
    	    		for(Int_t iv1 = 0; iv1<pairvb.size();iv1++){
    	    			for(Int_t lrv=0;lrv<4;lrv++){
    	    				Double_t x1bv,x1ev,y1bv,y1ev;
    	    				Double_t x2bv,x2ev,y2bv,y2ev;
    	    				GetWirePairBE(planes[vai[1]][pairvb[iv1]->idx1], planes[vbi[1]][pairvb[iv1]->idx2], &x1bv, &x1ev, &y1bv, &y1ev, &x2bv, &x2ev, &y2bv, &y2ev, lrv);
    	    				plane1 = digisDCH[planes[vai[1]][pairvb[iv1]->idx1]]->GetPlane();
    	    				zp1 = zl_glob[plane1] + DCH2_Zpos;
    	    				zv1=zp1;
    	    				Double_t yva = LinePropagation(y1b, zl_glob[yai[1]] + DCH2_Zpos, y2b, zl_glob[ybi[1]] + DCH2_Zpos, zp1);
    	    				Double_t xva = LinePropagation(x1bv,y1bv,x1ev,y1ev,yva);;
    	    	    		if(Abs(xva)>150 && Abs(yva)>150)
    	    	    			continue;
    	    				plane2 = digisDCH[planes[vbi[1]][pairvb[iv1]->idx2]]->GetPlane();
    	    				zp2 = zl_glob[plane2] + DCH2_Zpos;
    	    				zv2=zp2;
    	    				Double_t yvb = LinePropagation(y1b, zl_glob[yai[1]] + DCH2_Zpos, y2b, zl_glob[ybi[1]] + DCH2_Zpos, zp2);
    	    				Double_t xvb = LinePropagation(x2bv,y2bv,x2ev,y2ev,yvb);
    	    	    		if(Abs(xvb)>150 && Abs(yvb)>150)
    	    	    			continue;
    	    	    		if(Abs((yva-yua)-(yvb-yub))<mdiff){
    	    	    			mdiff=Abs((yva-yua)-(yvb-yub));
    	    	    			
    	    	    			x1bbk=-1;
    	    	    			x2bbk=-1;
    	    	    			y1bbk=y1b;
    	    	    			y2bbk=y2b;
    	    	    			zx1bk=zx1;
    	    	    			zx2bk=zx2;
    	    	    			xuabk=xua;
    	    	    			xubbk=xub;
    	    	    			yuabk=yua;
    	    	    			yubbk=yub;
    	    	    			zu1bk=zu1;
    	    	    			zu2bk=zu2;
    	    	    			xvabk=xva;
    	    	    			xvbbk=xvb;
    	    	    			yvabk=yva;
    	    	    			yvbbk=yvb;
    	    	    			zv1bk=zv1;
    	    	    			zv2bk=zv2;
    	    	    			
    	    	    			lribk=lri;
    	    	    			lrubk=lru;
    	    	    			lrvbk=lrv;
    	    	    			idx1x=planes[yai[1]][pairyb[ix1]->idx1];
    	    	    			idx2x=planes[ybi[1]][pairyb[ix1]->idx2];
    	    	    			idx1u=planes[uai[1]][pairub[iu1]->idx1];
    	    	    			idx2u=planes[ubi[1]][pairub[iu1]->idx2];
    	    	    			idx1v=planes[vai[1]][pairvb[iv1]->idx1];
    	    	    			idx2v=planes[vbi[1]][pairvb[iv1]->idx2];
    	    	    		}
    	    			}
    	    		}
    	    	}
    		}
    	}
    	if(mdiff!=1000){
			RecoCluster *rc= new RecoCluster();
			rc->hits.push_back(new RecoPoint3D(x1bbk,y1bbk,zx1bk,idx1x,lribk));
			rc->hits.push_back(new RecoPoint3D(x2bbk,y2bbk,zx2bk,idx2x,lribk));
			rc->hits.push_back(new RecoPoint3D(xuabk,yuabk,zu1bk,idx1u,lrubk));
			rc->hits.push_back(new RecoPoint3D(xubbk,yubbk,zu2bk,idx2u,lrubk));
			rc->hits.push_back(new RecoPoint3D(xvabk,yvabk,zv1bk,idx1v,lrvbk));
			rc->hits.push_back(new RecoPoint3D(xvbbk,yvbbk,zv2bk,idx2v,lrvbk));
			rc2y.push_back(rc);
			if(histoOutput){
				dyu2a->Fill(y1bbk-yuabk);
				dyu2b->Fill(y2bbk-yubbk);
				dyv2a->Fill(y1bbk-yvabk);
				duv2a->Fill(yuabk-yvabk);
				dyv2b->Fill(y2bbk-yvbbk);
				duv2b->Fill(yubbk-yvbbk);
				ywired[10]->Fill(y1bbk);
				ywired[11]->Fill(y2bbk);
				ywired[12]->Fill(yuabk);
				ywired[13]->Fill(yubbk);
				ywired[14]->Fill(yvabk);
				ywired[15]->Fill(yvbbk);
				xywired[12]->Fill(xuabk);
				xywired[13]->Fill(xubbk);
				xywired[14]->Fill(xvabk);
				xywired[15]->Fill(xvbbk);   		
			}
    	}   	
	}
	for (Int_t iSegmentX = 0; iSegmentX < rc1x.size(); iSegmentX++) {
		RecoCluster* segX = rc1x[iSegmentX];	
		Double_t mdifx=10000;
		Double_t mdify=10000;
		Int_t iy=-1;
		for (Int_t iSegmentY = 0; iSegmentY < rc1y.size(); iSegmentY++) {
			RecoCluster* segY = rc1y[iSegmentY];
			Double_t difx=0;
			Double_t dify=0;
			difx += Abs(segX->hits[2]->x - segY->hits[2]->x);
			difx += Abs(segX->hits[3]->x - segY->hits[3]->x);
			difx += Abs(segX->hits[4]->x - segY->hits[4]->x);
			difx += Abs(segX->hits[5]->x - segY->hits[5]->x);
			dify += Abs(segX->hits[2]->y - segY->hits[2]->y);
			dify += Abs(segX->hits[3]->y - segY->hits[3]->y);
			dify += Abs(segX->hits[4]->y - segY->hits[4]->y);
			dify += Abs(segX->hits[5]->y - segY->hits[5]->y);
			if(mdifx>difx && mdify>dify){
			    mdifx=difx;
				mdify=dify;
				iy=iSegmentY;
			}
		}
	}
}
void BmnDchTrackFinder::ClustersPreparation(){
	Bool_t hasHits=true;
	for(Int_t plane=0;plane<8;plane++)
		if(planes[plane].size()==0)
			hasHits=false;
	if(histoOutput){
		if(hasHits)
			Ntrack1->Fill(1);
		else{
			Ntrack1->Fill(0);
			Problems->Fill(1);
		}	
	}

	Bool_t hasHits2=true;
	for(Int_t plane=8;plane<16;plane++){
		if(planes[plane].size()==0)
			hasHits2=false;
	}
	if(histoOutput){
		if(hasHits2)
			Ntrack2->Fill(1);
		else{
			Ntrack2->Fill(0);
			Problems->Fill(2);
		}
	}

    aC.clear();
	bC.clear();
	for(Int_t plane=0;plane<16;plane+=2){
		for(Int_t iDig=0;iDig<planes[plane].size(); iDig++){
			Int_t wire1 = digisDCH[planes[plane][iDig]]->GetWireNumber();
			for(Int_t iDig1=0;iDig1<planes[plane+1].size(); iDig1++){
				Int_t wire2 = digisDCH[planes[plane+1][iDig1]]->GetWireNumber();
				if(Abs(wire1-wire2)<2){
					if(plane==xai[0])
						pairxa.push_back(new PointPair(iDig,iDig1));
					if(plane==xai[1])
						pairxb.push_back(new PointPair(iDig,iDig1));
					if(plane==yai[0])
						pairya.push_back(new PointPair(iDig,iDig1));
					if(plane==yai[1])
						pairyb.push_back(new PointPair(iDig,iDig1));
					if(plane==uai[0])
						pairua.push_back(new PointPair(iDig,iDig1));
					if(plane==uai[1])
						pairub.push_back(new PointPair(iDig,iDig1));
					if(plane==vai[0])
						pairva.push_back(new PointPair(iDig,iDig1));
					if(plane==vai[1])
						pairvb.push_back(new PointPair(iDig,iDig1));
				}
			}	
		}		
	}	
	std::vector<PointPair*> xa;
	for(Int_t ix1 = 0; ix1<pairxa.size();ix1++){
		for(Int_t iy1 = 0; iy1<pairya.size();iy1++){
			xa.push_back(new PointPair(ix1,iy1));
		}
	}
    std::vector<PointPair*> xb;
	for(Int_t ix1 = 0; ix1<pairxb.size();ix1++){
		for(Int_t iy1 = 0; iy1<pairyb.size();iy1++){
			xb.push_back(new PointPair(ix1,iy1));
		}
	}
	for(Int_t i = 0; i<xa.size();i++){
		
		Cluster *c = new Cluster();
		c->hits.push_back(planes[xai[0]][pairxa[xa[i]->idx1]->idx1]);
		c->hits.push_back(planes[xbi[0]][pairxa[xa[i]->idx1]->idx2]);
		c->hits.push_back(planes[yai[0]][pairya[xa[i]->idx2]->idx1]);
		c->hits.push_back(planes[ybi[0]][pairya[xa[i]->idx2]->idx2]);
		
		UChar_t planeX = digisDCH[planes[xai[0]][pairxa[xa[i]->idx1]->idx1]]->GetPlane();
		Int_t iWireX = digisDCH[planes[xai[0]][pairxa[xa[i]->idx1]->idx1]]->GetWireNumber();
		UChar_t planeY = digisDCH[planes[yai[0]][pairya[xa[i]->idx2]->idx1]]->GetPlane(); //!!!???
		Int_t iWireY = digisDCH[planes[yai[0]][pairya[xa[i]->idx2]->idx1]]->GetWireNumber();
		Double_t yPos = startX[planeY] - iWireY;
	    Double_t xPos = DCH1_Xpos + startX[planeX] - iWireX;
		
	    UChar_t planeX1 = digisDCH[planes[xbi[0]][pairxa[xa[i]->idx1]->idx2]]->GetPlane();
		Int_t iWireX1 = digisDCH[planes[xbi[0]][pairxa[xa[i]->idx1]->idx2]]->GetWireNumber();
		UChar_t planeY1 = digisDCH[planes[ybi[0]][pairya[xa[i]->idx2]->idx2]]->GetPlane();
		Int_t iWireY1 = digisDCH[planes[ybi[0]][pairya[xa[i]->idx2]->idx2]]->GetWireNumber();
		Double_t yPos1 = startX[planeY1] - iWireY1;
	    Double_t xPos1 = DCH1_Xpos + startX[planeX1] - iWireX1;
	    
	    if(xPos<xPos1){
	    	c->xmin=xPos;
	    	c->xmax=xPos1;
	    }
	    else{
	    	c->xmin=xPos1;
	    	c->xmax=xPos;    	
	    }
		if(yPos<yPos1){
			c->ymin=yPos;
			c->ymax=yPos1;
		}
		else{
			c->ymin=yPos1;
			c->ymax=yPos;
		}

	    Double_t mDist=10000;
	    Int_t iM =-1;
	    for(Int_t iu = 0; iu<pairua.size();iu++){
			UChar_t uplane=digisDCH[planes[uai[0]][pairua[iu]->idx1]]->GetPlane();
			Int_t uwire=digisDCH[planes[uai[0]][pairua[iu]->idx1]]->GetWireNumber();
			Double_t dist = Abs(GetDistance(uplane, xPos, yPos, uwire));
			if(dist < mDist && dist<distCutCluster){
				mDist=dist;
				iM=iu;
			}
		}
	    if(iM!=-1 && mDist<3.) {
	    	c->hits.push_back(planes[uai[0]][pairua[iM]->idx1]);
	    	c->hits.push_back(planes[ubi[0]][pairua[iM]->idx2]);
	    }
		
	    mDist=10000;
	    iM =-1;
	    for(Int_t iv = 0; iv<pairva.size();iv++){
			UChar_t vplane=digisDCH[planes[vai[0]][pairva[iv]->idx1]]->GetPlane();
			Int_t vwire=digisDCH[planes[vai[0]][pairva[iv]->idx1]]->GetWireNumber();
			Double_t dist = Abs(GetDistance(vplane, xPos, yPos, vwire));
			if(dist < mDist && dist<distCutCluster){
				mDist=dist;
				iM=iv;
			}
		}
	    if(iM!=-1 && mDist<3.) {
	    	c->hits.push_back(planes[vai[0]][pairva[iM]->idx1]);
	    	c->hits.push_back(planes[vbi[0]][pairva[iM]->idx2]);
	    }
		if(c->hits.size()>=trackNHitsCut)
			aC.push_back(c);
	} 
	for(Int_t i = 0; i<xb.size();i++){
		
		Cluster *c = new Cluster();	
	    
		c->hits.push_back(planes[xai[1]][pairxb[xb[i]->idx1]->idx1]);
		c->hits.push_back(planes[xbi[1]][pairxb[xb[i]->idx1]->idx2]);
		c->hits.push_back(planes[yai[1]][pairyb[xb[i]->idx2]->idx1]);
		c->hits.push_back(planes[ybi[1]][pairyb[xb[i]->idx2]->idx2]);

		UChar_t planeX = digisDCH[planes[xai[1]][pairxb[xb[i]->idx1]->idx1]]->GetPlane();
		Int_t iWireX = digisDCH[planes[xai[1]][pairxb[xb[i]->idx1]->idx1]]->GetWireNumber();
		UChar_t planeY = digisDCH[planes[yai[1]][pairyb[xb[i]->idx2]->idx1]]->GetPlane();
		Int_t iWireY = digisDCH[planes[yai[1]][pairyb[xb[i]->idx2]->idx1]]->GetWireNumber();
		Double_t yPos = startX[planeY] - iWireY;
		Double_t xPos = DCH2_Xpos + startX[planeX] - iWireX;

		UChar_t planeX1 = digisDCH[planes[xbi[1]][pairxb[xb[i]->idx1]->idx2]]->GetPlane();
		Int_t iWireX1 = digisDCH[planes[xbi[1]][pairxb[xb[i]->idx1]->idx2]]->GetWireNumber();
		UChar_t planeY1 = digisDCH[planes[ybi[1]][pairyb[xb[i]->idx2]->idx2]]->GetPlane();
		Int_t iWireY1 = digisDCH[planes[ybi[1]][pairyb[xb[i]->idx2]->idx2]]->GetWireNumber();
		Double_t yPos1 = startX[planeY1] - iWireY1;
		Double_t xPos1 = DCH2_Xpos + startX[planeX1] - iWireX1;

	    if(xPos<xPos1){
	    	c->xmin=xPos;
	    	c->xmax=xPos1;
	    }
	    else{
	    	c->xmin=xPos1;
	    	c->xmax=xPos;    	
	    }
		if(yPos<yPos1){
			c->ymin=yPos;
			c->ymax=yPos1;
		}
		else{
			c->ymin=yPos1;
			c->ymax=yPos;
		}
		
		Double_t mDist=10000;
	    Int_t iM =-1;
	    for(Int_t iu = 0; iu<pairub.size();iu++){
			UChar_t uplane=digisDCH[planes[uai[1]][pairub[iu]->idx1]]->GetPlane();
			Int_t uwire=digisDCH[planes[uai[1]][pairub[iu]->idx1]]->GetWireNumber();
			Double_t dist = Abs(GetDistance(uplane, xPos, yPos, uwire));
			if(dist < mDist && dist<distCutCluster){
				mDist=dist;
				iM=iu;
			}
		}
	    
	    if(iM!=-1 && mDist<3.) {
			c->hits.push_back(planes[uai[1]][pairub[iM]->idx1]);
			c->hits.push_back(planes[ubi[1]][pairub[iM]->idx2]);
	    }
		
	    mDist=10000;
	    iM =-1;
	    for(Int_t iv = 0; iv<pairvb.size();iv++){
			UChar_t vplane=digisDCH[planes[vai[1]][pairvb[iv]->idx1]]->GetPlane();
			Int_t vwire=digisDCH[planes[vai[1]][pairvb[iv]->idx1]]->GetWireNumber();
			Double_t dist = Abs(GetDistance(vplane, xPos, yPos, vwire));
			if(dist < mDist && dist<distCutCluster){
				mDist=dist;
				iM=iv;
			}
		}
	    if(iM!=-1 && mDist<3.) {
			c->hits.push_back(planes[vai[1]][pairvb[iM]->idx1]);
			c->hits.push_back(planes[vbi[1]][pairvb[iM]->idx2]);
	    }
	    if(c->hits.size()>=trackNHitsCut)
			bC.push_back(c);
	}
	if(histoOutput){
		if(hasHits)
			Ncluster1->Fill(aC.size());		
		if(hasHits2)
			Ncluster2->Fill(bC.size());		
	}
	if(aC.size()==0){
		if(hasHits){
			//cout << "cluster A recognition problem" << endl;
			if(histoOutput)
				Problems->Fill(3);
		}
	}
	if(bC.size()==0){
		if(hasHits2){
			//cout << "cluster B recognition problem" << endl;
			if(histoOutput)
				Problems->Fill(4);
		}
	}
}	

Bool_t BmnDchTrackFinder::PrepareDigitsFromMC(){
	
  digisDCH.clear();
  
  
  if(!fBmnDchPointsArray->GetEntriesFast() && histoOutput){// no points in DCH
	  Problems->Fill(0);
    return false;
  }
  
   if(histoOutput)
	Ntrack->Fill(fMCTracks->GetEntriesFast());
  
  FairMCPoint* pnt1=NULL; 
  FairMCPoint* pnt2=NULL;
  Double_t min=100000000;
  Double_t max=-100000000;
  
  cout << "++++++++++Number of MC tracks: " << fMCTracks->GetEntriesFast() << endl;
  
  for (Int_t iMCTrack=0; iMCTrack<fMCTracks->GetEntriesFast(); iMCTrack++) {
    min=100000000;
    max=-100000000;
    pnt1=NULL;
    pnt2=NULL;
    for (Int_t iPoint = 0; iPoint < fBmnDchPointsArray->GetEntriesFast(); iPoint++) {
      FairMCPoint* pnt = (FairMCPoint*) fBmnDchPointsArray->At(iPoint);       
      if(pnt->GetTrackID()==iMCTrack){
    	  if(pnt->GetZ()<min){
    		  min=pnt->GetZ();
    		  pnt1= (FairMCPoint*) fBmnDchPointsArray->At(iPoint);
    	  }
    	  if(pnt->GetZ()>max){
    		  max=pnt->GetZ();
    		  pnt2= (FairMCPoint*) fBmnDchPointsArray->At(iPoint);
    	  }
      }
    }

    if(!pnt1 || !pnt2)
      continue;
    
    for(Int_t plane = 0; plane<16;plane++){
      Double_t dist;
      Int_t wire;
      Double_t time;
      Double_t xTr;
      Double_t yTr;
      Double_t zTr;
      
      if(plane<8)
    	  zTr = zl_glob[plane] +DCH1_Zpos;
      else
    	  zTr= zl_glob[plane] + DCH2_Zpos;
      
      xTr=pnt1->GetX()+(zTr-pnt1->GetZ())*((pnt2->GetX()-pnt1->GetX())/(pnt2->GetZ()-pnt1->GetZ()));
      yTr=pnt1->GetY()+(zTr-pnt1->GetZ())*((pnt2->GetY()-pnt1->GetY())/(pnt2->GetZ()-pnt1->GetZ()));
      
      if(!CheckPointGEO(plane,xTr,yTr,zTr))
    	  continue;
      wire = (Int_t) GetClosestWireNumber(plane,xTr,yTr,&dist);
      
      Int_t add = abs(rand()%(300-0))+0;
      Double_t dAdd = (add - 150)/10000.;
      if((dist+dAdd)>0 && (dist+dAdd)<0.5)
    	  dist+=dAdd;
      
      if(wire==-1)
    	  continue;
      time = GetTime(&dist);   	
	  if(histoOutput)
		GDist->Fill(dist);
   	  digisDCH.push_back(new BmnDchDigit(plane,wire,time,iMCTrack)); //iMCTrack->pnt1->GetTrackID()
   	  planes[plane].push_back(digisDCH.size()-1);
      Double_t xT, yT;
      GetCoordinatesWirePoint(digisDCH[digisDCH.size()-1]->GetPlane(), xTr,yTr, digisDCH[digisDCH.size()-1]->GetWireNumber() ,&xT, &yT,GetDDistance(digisDCH[digisDCH.size()-1]->GetTime(),digisDCH[digisDCH.size()-1]->GetPlane()));
    }
  }
  if(digisDCH.size()>0){   	
    for (Int_t iPoint = 0; iPoint < digisDCH.size(); iPoint++) {
      new((*fBmnDchDigitsArray)[iPoint]) BmnDchDigit();
      BmnDchDigit* digit = (BmnDchDigit*) fBmnDchDigitsArray->At(iPoint);
      digit->SetPlane(digisDCH[iPoint]->GetPlane());
      digit->SetWireNumber(digisDCH[iPoint]->GetWireNumber());
      digit->SetTime(digisDCH[iPoint]->GetTime());
      digit->SetRefId(digisDCH[iPoint]->GetRefId());
    }
  }
}
void BmnDchTrackFinder::Exec(Option_t* opt) {
  
    if (!IsActive())
        return;
  for(Int_t i=0; i<16; i++)
	planes[i].clear();
  pairxa.clear();
  pairxb.clear();
  pairya.clear();
  pairyb.clear();
  pairua.clear();
  pairub.clear();
  pairva.clear();
  pairvb.clear();

  if(!expData)
    PrepareDigitsFromMC();
  else{
      if (!fBmnDchDigitsArray)
          return;
    if(fBmnDchDigitsArray->GetEntriesFast()<5){
      return ;
    }
    digisDCH.clear();
    for(Int_t ip=0;ip<16;ip++){
    	planes[ip].clear();
    }
    for (Int_t iDig = 0; iDig < fBmnDchDigitsArray->GetEntriesFast(); ++iDig) {
      BmnDchDigit* digit = (BmnDchDigit*) fBmnDchDigitsArray->UncheckedAt(iDig);
      digisDCH.push_back(digit);
      planes[digit->GetPlane()].push_back(digisDCH.size()-1);
    }
  }
  for (Int_t iDig = 0; iDig < fBmnDchDigitsArray->GetEntriesFast(); ++iDig) {
        BmnDchDigit* digit = (BmnDchDigit*) fBmnDchDigitsArray->UncheckedAt(iDig);
		if(histoOutput){
			dtime[digit->GetPlane()]->Fill(digit->GetTime());
			occup[digit->GetPlane()]->Fill(digit->GetWireNumber());
		}
  }
  //ClustersPreparationRT();
  //return;
  ClustersPreparation();
  RecoDCHCluster();
  return;

    fEventNo++;
    clock_t tStart = clock();
    PrepareArraysToProcessEvent();
    if (fVerbose) cout << "======================== DCH track finder exec started ====================" << endl;
    if (fVerbose) cout << "Event number: " << fEventNo << endl;

    //temporary containers
    // Order used: va1, vb1, ua1, ub1, ya1, yb1, xa1, xb1 (dch1, 0 - 7) - va2, vb2, ua2, ub2, ya2, yb2, xa2, xb2 (dch2, 8 - 15)
    const Int_t nDim = 20;
    const Int_t nPlanes = 16; // Total number of planes in both DCHs (0-7, 8-15)

    Double_t times[nPlanes][nDim];
    Double_t wires[nPlanes][nDim];
    Int_t it[nPlanes];
    Bool_t used[nPlanes][nDim];
    for (Int_t iPlanes = 0; iPlanes < nPlanes; iPlanes++) {
        it[iPlanes] = 0;
        for (Int_t iDim = 0; iDim < nDim; iDim++) {
            used[iPlanes][iDim] = kFALSE;
            times[iPlanes][iDim] = 0.;
            wires[iPlanes][iDim] = 0.;
        }
    }

    Bool_t goodEv = kTRUE; 
    Bool_t written = kFALSE;

    for (Int_t iDig = 0; iDig < fBmnDchDigitsArray->GetEntriesFast(); ++iDig) {
        BmnDchDigit* digit = (BmnDchDigit*) fBmnDchDigitsArray->UncheckedAt(iDig);

        //skip identical events
        if (!written) {
            written = kTRUE;
            if (digit->GetTime() == prev_time && digit->GetWireNumber() == prev_wire) {
                goodEv = kFALSE;
            } else {
                prev_time = Int_t(digit->GetTime());
                prev_wire = Int_t(digit->GetWireNumber());
            }
        }//!written

        if (!goodEv)
            return;

        // Order used: va1(0), vb1(1), ua1(2), ub1(3), ya1(4), yb1(5), xa1(6), xb1(7) ->
        //             va2(8), vb2(9), ua2(10), ub2(11), ya2(12), yb2(13), xa2(14), xb2(15)
        Short_t plane = digit->GetPlane();
        Short_t wire = digit->GetWireNumber();
        Double_t time = digit->GetTime();
        Bool_t secondaries = kFALSE;

        for (Int_t sec = 0; sec < it[plane] - 1; sec++)
            if (wire == wires[plane][sec]) {
                secondaries = kTRUE;
                break;
            }

        if (it[plane] == (nDim - 1) || secondaries)
            continue;

        wires[plane][it[plane]] = wire;
        times[plane][it[plane]] = time;
        it[plane]++;
    }

    const Int_t nWires_ = 4;
    const Int_t nLayers_ = 2;
    TString wireNames[nWires_] = {"v", "u", "y", "x"};
    TString layNames[nLayers_] = {"a", "b"};
    Int_t cntr = 0;

    for (Int_t iDch = 0; iDch < nChambers; iDch++) {
        for (Int_t iWire = 0; iWire < nWires; iWire++) {
            Int_t start = 2 * iWire + (nPlanes / 2) * iDch;
            Int_t finish = start + 1;
            Double_t*** coord = (iWire == 0) ? v : (iWire == 1) ? u : (iWire == 2) ? y : x;
            Double_t*** sigma = (iWire == 0) ? sigm_v : (iWire == 1) ? sigm_u : (iWire == 2) ? sigm_y : sigm_x;

            pairs[iDch][iWire] = Reconstruction(iDch + 1, wireNames[iWire], pairs[iDch][iWire], it[start], it[finish],
                    wires[start], wires[finish], times[start], times[finish], used[start], used[finish],
                    coord[iDch], sigma[iDch]);

            for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
                Double_t*** single_coord = (iWire == 0) ? v_Single : (iWire == 1) ? u_Single : (iWire == 2) ? y_Single : x_Single;
                Double_t*** single_sigma = (iWire == 0) ? Sigm_v_single : (iWire == 1) ? Sigm_u_single : (iWire == 2) ? Sigm_y_single : Sigm_x_single;
                singles[iDch][iWire][iLayer] = ReconstructionSingle(iDch + 1, wireNames[iWire], layNames[iLayer],
                        singles[iDch][iWire][iLayer], it[cntr], wires[cntr], times[cntr], used[cntr], single_coord[iDch], single_sigma[iDch]);
                cntr++;
            }
        }
        nSegments[iDch] = BuildUVSegments(iDch + 1,
                pairs[iDch][1], pairs[iDch][0], pairs[iDch][3], pairs[iDch][2], singles[iDch][1][0], singles[iDch][1][1], singles[iDch][0][0], singles[iDch][0][1],
                x[iDch], y[iDch], u[iDch], v[iDch], sigm_x[iDch], sigm_y[iDch], sigm_u[iDch], sigm_v[iDch], rh_segment[iDch], rh_sigm_segment[iDch],
                u_Single[iDch], v_Single[iDch], Sigm_u_single[iDch], Sigm_v_single[iDch]);

        nSegments[iDch] = BuildXYSegments(iDch + 1,
                pairs[iDch][1], pairs[iDch][0], pairs[iDch][3], pairs[iDch][2], singles[iDch][3][0], singles[iDch][3][1], singles[iDch][2][0], singles[iDch][2][1],
                x[iDch], y[iDch], u[iDch], v[iDch], sigm_x[iDch], sigm_y[iDch], sigm_u[iDch], sigm_v[iDch], rh_segment[iDch], rh_sigm_segment[iDch],
                x_Single[iDch], y_Single[iDch], Sigm_x_single[iDch], Sigm_y_single[iDch]);

        has7DC[iDch] = FitDchSegments(iDch + 1, segment_size[iDch], rh_segment[iDch], rh_sigm_segment[iDch], params[iDch], Chi2[iDch], x_global[iDch], y_global[iDch]); // Fit found segments
        SelectLongestAndBestSegments(iDch + 1, segment_size[iDch], rh_segment[iDch], Chi2[iDch]); // Save only longest and best chi2 segments
        // CreateDchTrack(iDch + 1, Chi2[iDch], params[iDch], segment_size[iDch]); // Fill segment parameters                  
    }

    // Try to match the reconstructed segments from the two chambers
    SegmentsToBeMatched();
    CreateDchTrack();
   
    if (fVerbose) cout << "======================== DCH track finder exec finished ===================" << endl;
    clock_t tFinish = clock();
    workTime += ((Double_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnDchTrackFinder::SegmentsToBeMatched() {
    Int_t match_dc1_seg = -1;
    Double_t ax(0.), ay(0.), xMean(0.), yMean(0.);
    Double_t min_distX = 20;
    Double_t min_distY = 15;
    Double_t min_distSQ = 225;

    Double_t dx = -999;
    Double_t dy = -999;
    Double_t daX = -999;
    Double_t daY = -999;

    for (Int_t segdc2Nr = 0; segdc2Nr < nSegments[1]; segdc2Nr++) {
        if (Chi2[1][segdc2Nr] > 50 || segment_size[1][segdc2Nr] < 7)
            continue;

        Double_t chi2_match = 0;
        for (int segdc1Nr = 0; segdc1Nr < nSegments[0]; segdc1Nr++) {
            if (Chi2[0][segdc1Nr] > 50 || segment_size[0][segdc1Nr] < 7)
                continue;

            Double_t distX = Abs(x_global[0][segdc1Nr] - x_global[1][segdc2Nr]);
            Double_t distY = Abs(y_global[0][segdc1Nr] - y_global[1][segdc2Nr]);

            if (distX < min_distX && distY < min_distY) {
                Double_t distSQ = distX * distX + distY * distY;

                chi2_match = (Abs(x_global[0][segdc1Nr] - x_global[1][segdc2Nr]) * Abs(x_global[0][segdc1Nr] - x_global[1][segdc2Nr]) / 49)
                        + (distY * distY / 43.56) + ((params[1][0][segdc2Nr] - params[0][0][segdc1Nr] * params[1][0][segdc2Nr] - params[0][0][segdc1Nr]) / 0.0144)
                        + (params[1][2][segdc2Nr] - params[0][2][segdc1Nr] * params[1][2][segdc2Nr] - params[0][2][segdc1Nr]) / 0.0225;
                
                if (distSQ < min_distSQ) {
                    dx = x_global[1][segdc2Nr] - x_global[0][segdc1Nr];
                    dy = y_global[1][segdc2Nr] - y_global[0][segdc1Nr];
                    xMean = 0.5 * (x_global[0][segdc1Nr] + x_global[1][segdc2Nr]);
                    yMean = 0.5 * (y_global[0][segdc1Nr] + y_global[1][segdc2Nr]);
                    min_distSQ = distSQ;
                    ax = (params[1][1][segdc2Nr] - params[0][1][segdc1Nr]) / dZ_dch;
                    ay = (params[1][3][segdc2Nr] - params[0][3][segdc1Nr]) / dZ_dch;
                    match_dc1_seg = segdc1Nr;
                    daX = params[1][0][segdc2Nr] - params[0][0][segdc1Nr];
                    daY = params[1][2][segdc2Nr] - params[0][2][segdc1Nr];
                }
            }
        } // segdc1Nr

        if (min_distSQ >= 225)
            continue;

        if (Abs(dx) < 10 && Abs(dy) < 10) {
            if (Abs(daX) < 0.2 && Abs(daY) < 0.2) {
                nSegmentsToBeMatched++;
                leng[nSegmentsToBeMatched] = sqrt(628.65 * 628.65 + (xMean * xMean));
                x_mid[nSegmentsToBeMatched] = xMean;
                y_mid[nSegmentsToBeMatched] = yMean;
                aX[nSegmentsToBeMatched] = ATan(ax);
                aY[nSegmentsToBeMatched] = ATan(ay);
                imp[nSegmentsToBeMatched] = -0.4332 / (ax + 0.006774);
                Chi2_match[nSegmentsToBeMatched] = chi2_match;
                Chi2[1][segdc2Nr] = 999; //mark dc2 seg as used for future iterations
                Chi2[0][match_dc1_seg] = 999; //mark dch1 seg as used for future iterations
            } // dax  day  
        } //  dx  dy
    } // segdc2Nr
}

Int_t BmnDchTrackFinder::BuildXYSegments(Int_t dchID,
        Int_t pairU, Int_t pairV, Int_t pairX, Int_t pairY, Int_t single_xa, Int_t single_xb, Int_t single_ya, Int_t single_yb,
        Double_t** x_ab, Double_t** y_ab, Double_t** u_ab, Double_t** v_ab,
        Double_t** sigm_x_ab, Double_t** sigm_y_ab, Double_t** sigm_u_ab, Double_t** sigm_v_ab,
        Double_t** rh_seg, Double_t** rh_sigm_seg,
        Double_t** x_single, Double_t** y_single, Double_t** sigm_x_single, Double_t** sigm_y_single) {

    Double_t sqrt_2 = sqrt(2.);
    Double_t isqrt_2 = 1. / sqrt_2;

    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    for (Int_t i = 0; i < pairU; i++) {
        if (nDC_segments > 48)
            break;
        Double_t u_coord = (u_ab[0][i] + u_ab[1][i]) / 2;

        for (Int_t j = 0; j < pairV; j++) {
            if (nDC_segments > 48)
                break;
            Double_t v_coord = (v_ab[0][j] + v_ab[1][j]) / 2;
            Bool_t foundX = kFALSE;
            Double_t x_est = isqrt_2 * (v_coord - u_coord);
            Double_t y_est = isqrt_2 * (u_coord + v_coord);
            if (pairX > 0) {
                Double_t dX_thresh = 1.5;
                for (Int_t k = 0; k < pairX; k++) {
                    Double_t x_coord = (x_ab[0][k] + x_ab[1][k]) / 2;
                    if (nDC_segments > 48)
                        break;
                    if (Abs(x_coord - x_est) > dX_thresh)
                        continue;
                    dX_thresh = Abs(x_coord - x_est);

                    rh_seg[0][nDC_segments] = x_ab[0][k];
                    rh_seg[1][nDC_segments] = x_ab[1][k];
                    rh_seg[4][nDC_segments] = u_ab[0][i];
                    rh_seg[5][nDC_segments] = u_ab[1][i];
                    rh_seg[6][nDC_segments] = v_ab[0][j];
                    rh_seg[7][nDC_segments] = v_ab[1][j];
                    rh_sigm_seg[0][nDC_segments] = sigm_x_ab[0][k];
                    rh_sigm_seg[1][nDC_segments] = sigm_x_ab[1][k];
                    rh_sigm_seg[4][nDC_segments] = sigm_u_ab[0][i];
                    rh_sigm_seg[5][nDC_segments] = sigm_u_ab[1][i];
                    rh_sigm_seg[6][nDC_segments] = sigm_v_ab[0][j];
                    rh_sigm_seg[7][nDC_segments] = sigm_v_ab[1][j];

                    foundX = kTRUE;
                    if (nDC_segments > 48)
                        break;
                }//k
            }//(pair_x2>0)

            Bool_t foundY = kFALSE;
            if (pairY > 0) {
                Double_t dY_thresh = 1.0;
                for (Int_t m = 0; m < pairY; m++) {
                    if (nDC_segments > 48)
                        break;
                    Double_t y_coord = (y_ab[0][m] + y_ab[1][m]) / 2;
                    if (Abs(y_coord - y_est) > dY_thresh)
                        continue;
                    dY_thresh = Abs(y_coord - y_est);
                    foundY = kTRUE;
                    rh_seg[2][nDC_segments] = y_ab[0][m];
                    rh_seg[3][nDC_segments] = y_ab[1][m];
                    rh_seg[4][nDC_segments] = u_ab[0][i];
                    rh_seg[5][nDC_segments] = u_ab[1][i];
                    rh_seg[6][nDC_segments] = v_ab[0][j];
                    rh_seg[7][nDC_segments] = v_ab[1][j];
                    rh_sigm_seg[2][nDC_segments] = sigm_y_ab[0][m];
                    rh_sigm_seg[3][nDC_segments] = sigm_y_ab[1][m];
                    rh_sigm_seg[4][nDC_segments] = sigm_u_ab[0][i];
                    rh_sigm_seg[5][nDC_segments] = sigm_u_ab[1][i];
                    rh_sigm_seg[6][nDC_segments] = sigm_v_ab[0][j];
                    rh_sigm_seg[7][nDC_segments] = sigm_v_ab[1][j];
                    if (!foundX) {
                        Double_t min_a = 999;
                        Double_t min_b = 999;
                        for (Int_t kk = 0; kk < single_xa; kk++) {
                            if (Abs(x_single[1][kk] - x_est) > 1.5)
                                continue; //????? 0.5 needs to be reviewed

                            if (Abs(x_single[0][kk] - x_est) < min_a) {
                                min_a = Abs(x_single[0][kk] - x_est);
                                rh_seg[0][nDC_segments] = x_single[0][kk];
                                rh_sigm_seg[0][nDC_segments] = sigm_x_single[0][kk];
                                foundX = kTRUE;
                            }
                        }//for kk
                        for (Int_t kk = 0; kk < single_xb; kk++) {

                            if (Abs(x_single[1][kk] - x_est) > 1.5)
                                continue; //????? 0.5 needs to be reviewed
                            if (Abs(x_single[1][kk] - x_est) < min_b) {
                                min_b = Abs(x_single[1][kk] - x_est);
                                rh_seg[1][nDC_segments] = x_single[1][kk];
                                rh_sigm_seg[1][nDC_segments] = sigm_x_single[1][kk];
                                foundX = kTRUE;
                            }
                        }//for kk
                        if (nDC_segments > 48)
                            break;
                    }//!foundX
                }//m
                if (foundX && !foundY) {
                    Double_t min_a = 999;
                    Double_t min_b = 999;
                    for (Int_t kk = 0; kk < single_ya; kk++) {
                        if (Abs(y_single[0][kk] - y_est) > 1.5)
                            continue; //????? 0.5 needs to be reviewed
                        if (Abs(y_single[0][kk] - y_est) < min_a) {
                            min_a = Abs(y_single[0][kk] - y_est);
                            rh_seg[2][nDC_segments] = y_single[0][kk];
                            rh_sigm_seg[2][nDC_segments] = sigm_y_single[0][kk];
                            foundY = kTRUE;
                        }
                    }//for kk
                    for (Int_t kk = 0; kk < single_yb; kk++) {
                        if (Abs(y_single[1][kk] - y_est) > 1.5)
                            continue; //????? 0.5 needs to be reviewed
                        if (Abs(y_single[1][kk] - y_est) < min_b) {
                            min_b = Abs(y_single[1][kk] - y_est);
                            rh_seg[3][nDC_segments] = y_single[1][kk];
                            rh_sigm_seg[3][nDC_segments] = sigm_y_single[1][kk];
                            foundY = kTRUE;
                        }
                    }//for kk
                }
            }//(pair_y2>0)
            if (foundX || foundY) nDC_segments++;
        }
    }
    return nDC_segments;
}

Int_t BmnDchTrackFinder::BuildUVSegments(Int_t dchID, Int_t pairU, Int_t pairV, Int_t pairX, Int_t pairY, Int_t single_ua, Int_t single_ub, Int_t single_va, Int_t single_vb,
        Double_t** x_ab, Double_t** y_ab, Double_t** u_ab, Double_t** v_ab,
        Double_t** sigm_x_ab, Double_t** sigm_y_ab, Double_t** sigm_u_ab, Double_t** sigm_v_ab,
        Double_t** rh_seg, Double_t** rh_sigm_seg,
        Double_t** u_single, Double_t** v_single, Double_t** sigm_u_single, Double_t** sigm_v_single) {

    Double_t sqrt_2 = sqrt(2.);
    Double_t isqrt_2 = 1. / sqrt_2;

    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];

    for (Int_t i = 0; i < pairX; i++) {
        if (nDC_segments > 48)
            break;
        Double_t x_coord = (x_ab[0][i] + x_ab[1][i]) / 2;
        Double_t XU = x_coord;
        Double_t XV = x_coord;

        for (Int_t j = 0; j < pairY; j++) {
            Double_t y_coord = (y_ab[0][j] + y_ab[1][j]) / 2;
            Double_t YU = y_coord;
            Double_t YV = y_coord;
            Bool_t foundU = kFALSE;
            Double_t u_est = isqrt_2 * (YU - XU);
            Double_t v_est = isqrt_2 * (YV + XV);

            Double_t dU_thresh = 1.3;
            for (Int_t k = 0; k < pairU; k++) {
                Double_t u_coord = (u_ab[0][k] + u_ab[1][k]) / 2;

                if (Abs(u_coord - u_est) > dU_thresh)
                    continue;
                dU_thresh = Abs(u_coord - u_est);

                rh_seg[0][nDC_segments] = x_ab[0][i];
                rh_seg[1][nDC_segments] = x_ab[1][i];
                rh_seg[2][nDC_segments] = y_ab[0][j];
                rh_seg[3][nDC_segments] = y_ab[1][j];
                rh_seg[4][nDC_segments] = u_ab[0][k];
                rh_seg[5][nDC_segments] = u_ab[1][k];
                rh_sigm_seg[0][nDC_segments] = sigm_x_ab[0][i];
                rh_sigm_seg[1][nDC_segments] = sigm_x_ab[1][i];
                rh_sigm_seg[2][nDC_segments] = sigm_y_ab[0][j];
                rh_sigm_seg[3][nDC_segments] = sigm_y_ab[1][j];
                rh_sigm_seg[4][nDC_segments] = sigm_u_ab[0][k];
                rh_sigm_seg[5][nDC_segments] = sigm_u_ab[1][k];

                foundU = kTRUE;
                if (nDC_segments > 48)
                    break;
            }

            Bool_t foundV = kFALSE;

            Double_t dV_thresh = 1.3;
            for (Int_t m = 0; m < pairV; m++) {
                if (nDC_segments > 48)
                    break;
                Double_t v_coord = (v_ab[0][m] + v_ab[1][m]) / 2;

                if (Abs(v_coord - v_est) > dV_thresh)
                    continue;
                dV_thresh = Abs(v_coord - v_est);

                foundV = kTRUE;
                rh_seg[0][nDC_segments] = x_ab[0][i];
                rh_seg[1][nDC_segments] = x_ab[1][i];
                rh_seg[2][nDC_segments] = y_ab[0][j];
                rh_seg[3][nDC_segments] = y_ab[1][j];
                rh_seg[6][nDC_segments] = v_ab[0][m];
                rh_seg[7][nDC_segments] = v_ab[1][m];
                rh_sigm_seg[0][nDC_segments] = sigm_x_ab[0][i];
                rh_sigm_seg[1][nDC_segments] = sigm_x_ab[1][i];
                rh_sigm_seg[2][nDC_segments] = sigm_y_ab[0][j];
                rh_sigm_seg[3][nDC_segments] = sigm_y_ab[1][j];
                rh_sigm_seg[6][nDC_segments] = sigm_v_ab[0][m];
                rh_sigm_seg[7][nDC_segments] = sigm_v_ab[1][m];

                if (!foundU) {
                    Double_t min_a = 999;
                    Double_t min_b = 999;
                    for (Int_t kk = 0; kk < single_ua; kk++) {
                        if (Abs(u_single[0][kk] - u_est) > 1.5)
                            continue; //????? 0.5 needs to be reviewed
                        if (Abs(u_single[0][kk] - u_est) < min_a) {
                            min_a = Abs(u_single[0][kk] - u_est);
                            rh_seg[4][nDC_segments] = u_single[0][kk];
                            rh_sigm_seg[4][nDC_segments] = sigm_u_single[0][kk];
                            foundU = kTRUE;
                        }
                    }//for kk
                    for (Int_t kk = 0; kk < single_ub; kk++) {
                        if (Abs(u_single[1][kk] - u_est) > 1.5)
                            continue; //????? 0.5 needs to be reviewed
                        if (Abs(u_single[1][kk] - u_est) < min_b) {
                            min_b = Abs(u_single[1][kk] - u_est);
                            rh_seg[5][nDC_segments] = u_single[1][kk];
                            rh_sigm_seg[5][nDC_segments] = sigm_u_single[1][kk];
                            foundU = kTRUE;
                        }
                    }//for kk
                    if (nDC_segments > 48)
                        break;
                }//!foundU

                if (nDC_segments > 48)
                    break;
            }//m
            //            }//(pair_v2>0)
            if (!foundV && foundU) {
                Double_t min_a = 999;
                Double_t min_b = 999;
                for (Int_t kk = 0; kk < single_va; kk++) {
                    if (Abs(v_single[0][kk] - v_est) > 1.5)
                        continue; //????? 0.5 needs to be reviewed
                    if (Abs(v_single[0][kk] - v_est) < min_a) {
                        min_a = Abs(v_single[0][kk] - v_est);
                        rh_seg[6][nDC_segments] = v_single[0][kk];
                        rh_sigm_seg[6][nDC_segments] = sigm_v_single[0][kk];
                        foundV = kTRUE;
                    }
                }//for kk
                for (Int_t kk = 0; kk < single_vb; kk++) {
                    if (Abs(v_single[1][kk] - v_est) > 1.5)
                        continue; //????? 0.5 needs to be reviewed
                    if (Abs(v_single[1][kk] - v_est) < min_b) {
                        min_b = Abs(v_single[1][kk] - v_est);
                        rh_seg[7][nDC_segments] = v_single[1][kk];
                        rh_sigm_seg[7][nDC_segments] = sigm_v_single[1][kk];
                        foundV = kTRUE;
                    }
                }
            }
            if (foundV || foundU) nDC_segments++;
        }
    }
    return nDC_segments;
}

Bool_t BmnDchTrackFinder::FitDchSegments(Int_t dchID, Int_t* size_seg, Double_t** rh_seg, Double_t** rh_sigm_seg, Double_t** par_ab, Double_t* chi2, Double_t* x_glob, Double_t* y_glob) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    Bool_t hasSuffNumberOfSegments = kFALSE;
    for (Int_t j = 0; j < nDC_segments; j++) {
        Int_t worst_hit = -1;
        Double_t max_resid = 0;

        Double_t _rh_seg[8];
        Double_t _rh_sigm_seg[8];
        Double_t _par_ab[4];

        for (Int_t i = 0; i < 8; i++)
            if (Abs(rh_seg[i][j] + 999.) > FLT_EPSILON)
                size_seg[j]++;

        for (Int_t rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6
            for (Int_t i = 0; i < 8; i++) {
                _rh_seg[i] = rh_seg[i][j];
                _rh_sigm_seg[i] = rh_sigm_seg[i][j];
            }

            fit_seg(z_loc, _rh_seg, _rh_sigm_seg, _par_ab, -1, -1); //usual fit without skipping any plane
            for (Int_t i = 0; i < 4; i++)
                par_ab[i][j] = _par_ab[i];

            chi2[j] = 0;

            Double_t resid(LDBL_MAX);
            for (Int_t i = 0; i < 8; i++) {
                if (Abs(rh_seg[i][j] + 999.) < FLT_EPSILON)
                    continue;

                resid = CalculateResidual(i, j, rh_seg, par_ab);
                chi2[j] += (resid * resid) / rh_sigm_seg[i][j];
                if (Abs(resid) > max_resid) {
                    worst_hit = i;
                    max_resid = Abs(resid);
                }
            }

            chi2[j] /= (size_seg[j] - 4);

            //if chi2 is big and seg_size = min erase this seg
            if (chi2[j] > 30.)
                if (size_seg[j] == 6) {
                    chi2[j] = 999.;
                    break;
                } else {
                    rh_seg[worst_hit][j] = -999.; //erase worst hit and refit
                    size_seg[j]--;
                    max_resid = 0;
                    continue;
                }
        }

        // Add shifts to slopes and coords
        Double_t x_slope_sh = (dchID == 1) ? x1_slope_sh : x2_slope_sh;
        Double_t y_slope_sh = (dchID == 1) ? y1_slope_sh : y2_slope_sh;
        Double_t x_sh = (dchID == 1) ? x1_sh : x2_sh;
        Double_t y_sh = (dchID == 1) ? y1_sh : y2_sh;

        par_ab[0][j] += x_slope_sh + x_slope_sh * par_ab[0][j] * par_ab[0][j];
        par_ab[2][j] += y_slope_sh + y_slope_sh * par_ab[2][j] * par_ab[2][j];
        par_ab[1][j] += x_sh;
        par_ab[3][j] += y_sh;

        Int_t coeff = (dchID == 1) ? 1 : -1;
        x_glob[j] = coeff * par_ab[0][j] * dZ_dch_mid + par_ab[1][j];
        y_glob[j] = coeff * par_ab[2][j] * dZ_dch_mid + par_ab[3][j];

        if (size_seg[j] > 6)
            hasSuffNumberOfSegments = kTRUE;
    }
    return hasSuffNumberOfSegments;
}

void BmnDchTrackFinder::CompareDaDb(Double_t d, Double_t& ele) {
    ele = (d < 0.02) ? (0.08 * 0.08) :
            (d >= 0.02 && d < 0.1) ? (0.06 * 0.06) :
            (d >= 0.1 && d < 0.4) ? (0.025 * 0.025) :
            (d >= 0.4 && d < 0.41) ? (0.08 * 0.08) :
            (0.10 * 0.10);
}

void BmnDchTrackFinder::CompareDaDb(Double_t d, Double_t& ele1, Double_t& ele2) {
    ele1 = (d < 0.02) ? (0.08 * 0.08) :
            (d >= 0.02 && d < 0.1) ? (0.06 * 0.06) :
            (d >= 0.1 && d < 0.4) ? (0.025 * 0.025) :
            (d >= 0.4 && d < 0.41) ? (0.08 * 0.08) :
            (0.10 * 0.10);
    ele2 = ele1;
}

void BmnDchTrackFinder::SelectLongestAndBestSegments(Int_t dchID, Int_t* size_seg, Double_t** rh_seg, Double_t* chi2) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    for (Int_t max_size = 8; max_size > 5; max_size--)
        for (Int_t it1 = 0; it1 < nDC_segments; it1++) {
            if (size_seg[it1] != max_size)
                continue;
            for (Int_t it2 = 0; it2 < nDC_segments; it2++) {
                if (it2 == it1)
                    continue;
                for (Int_t hit = 0; hit < 4; hit++)
                    if (rh_seg[2 * hit][it1] == rh_seg[2 * hit][it2] &&
                            rh_seg[2 * hit + 1][it1] == rh_seg[2 * hit + 1][it2] &&
                            (chi2[it1] <= chi2[it2] || size_seg[it1] > size_seg[it2])) {
                        chi2[it2] = 999.;
                        break;
                    }
            }
        }
}

void BmnDchTrackFinder::CreateDchTrack(Int_t dchID, Double_t* chi2Arr, Double_t** parArr, Int_t* sizeArr) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    for (Int_t iSegment = 0; iSegment < nDC_segments; iSegment++) {
        if (chi2Arr[iSegment] > 50)
            continue;
        FairTrackParam trackParam;
        Double_t z0 = (dchID == 1) ? Z_dch1 : Z_dch2;
        Double_t x0 = parArr[1][iSegment];
        Double_t y0 = parArr[3][iSegment];
        trackParam.SetPosition(TVector3(-x0, y0, z0));
        trackParam.SetTx(-parArr[0][iSegment]);
        trackParam.SetTy(parArr[2][iSegment]);

        BmnDchTrack* track = new((*fDchTracks)[fDchTracks->GetEntriesFast()]) BmnDchTrack();
        track->SetChi2(chi2Arr[iSegment]);
        track->SetNHits(sizeArr[iSegment]);
        track->SetParamFirst(trackParam);
    }
}

void BmnDchTrackFinder::CreateDchTrack() {
    for (Int_t iSeg = 0; iSeg < nSegmentsToBeMatched + 1; iSeg++) {
        FairTrackParam trackParam;
        Double_t z0 = Z_dch_mid;
        Double_t x0 = x_mid[iSeg];
        Double_t y0 = y_mid[iSeg];
        trackParam.SetPosition(TVector3(-x0, y0, z0)); // Go to right reference frame
        trackParam.SetTx(-aX[iSeg]); // Go to right reference frame
        trackParam.SetTy(aY[iSeg]);
        
        BmnDchTrack* track = new((*fDchTracks)[fDchTracks->GetEntriesFast()]) BmnDchTrack();
        track->SetChi2(Chi2_match[iSeg]);
        track->SetParamFirst(trackParam);
    }
}

Double_t BmnDchTrackFinder::CalculateResidual(Int_t i, Int_t j, Double_t** rh_seg, Double_t** par_ab) {
    Double_t sqrt_2 = sqrt(2.);
    Double_t isqrt_2 = 1 / sqrt_2;

    return (i < 2) ? rh_seg[i][j] - z_loc[i] * par_ab[0][j] - par_ab[1][j] :
            (i >= 2 && i < 4) ? rh_seg[i][j] - z_loc[i] * par_ab[2][j] - par_ab[3][j] :
            (i >= 4 && i < 6) ? rh_seg[i][j] - isqrt_2 * z_loc[i] * (par_ab[2][j] - par_ab[0][j]) - isqrt_2 * (par_ab[3][j] - par_ab[1][j]) :
            rh_seg[i][j] - isqrt_2 * z_loc[i] * (par_ab[2][j] + par_ab[0][j]) - isqrt_2 * (par_ab[3][j] + par_ab[1][j]);
}

InitStatus BmnDchTrackFinder::Init() {
    /*if (!expData)
    {
      cout<<"BmnDchTrackFinder::Init(): simulation data is not supported! Task will be deactivated"<<endl;
      SetActive(kFALSE);
      return kERROR;
    }*/

    TString dir1 = getenv("VMCWORKDIR");
    dir1 += "/input/";

	Int_t time;
	float dist=0.;
  	for(Int_t i=0;i<16;i++)
  	{
  		char line[50];
  		char inFileName1[10];
  		TString inFileName;
		sprintf(inFileName1,"rt%d.txt",i);
	    inFileName=dir1+inFileName1;
		ifstream input(inFileName);
	    if (!input.is_open()) // если файл не открыт
	        cout << "Файл не может быть открыт!\n"; // сообщить об этом
	    else
	    {
	    	while ( input.getline(line,50) ){
	    		sscanf(line, "%d %f", &time, &dist);
	    		rtRel[i][time]=dist;
	    	}
	    }
	    input.close();
  	}
	FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman)
  		Fatal("Init", "FairRootManager is not instantiated");
  
  if (!expData){
    fBmnDchDigitsArray = new TClonesArray("BmnDchDigit", 10000);
    ioman->Register("BmnDchDigit", "DCH", fBmnDchDigitsArray, kTRUE);
    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    fBmnDchPointsArray = (TClonesArray*) ioman->GetObject("DCHPoint");
    fMCPointArray = (TClonesArray*) ioman->GetObject("StsPoint");
  }
  else{
    fBmnDchDigitsArray = (TClonesArray*) ioman->GetObject(InputDigitsBranchName);
  }
  fDchTracks = new TClonesArray("BmnDchTrack", 10000);
  ioman->Register("BmnDchTrack", "DCH", fDchTracks, kTRUE);
  fDchHits = new TClonesArray("BmnDchHit", 10000);
  ioman->Register("BmnDchHit", "DCH", fDchTracks, kTRUE);
  if (!expData)
    return kSUCCESS;
  if (fVerbose) cout << "BmnDchTrackFinder::Init()" << endl;
  
  return kSUCCESS;

    // Create and register track arrays
    fDchTracks = new TClonesArray(tracksDch.Data());
    ioman->Register(tracksDch.Data(), "DCH", fDchTracks, kTRUE);


    ifstream fin;
    TString dir = getenv("VMCWORKDIR");
    dir += "/input/";
    fin.open((TString(dir + fTransferFunctionName)).Data(), ios::in);
    for (Int_t fi = 0; fi < 16; fi++)
        fin >> t_dc[0][fi] >> t_dc[1][fi] >> t_dc[2][fi] >> t_dc[3][fi] >> t_dc[4][fi] >>
            pol_par_dc[0][0][fi] >> pol_par_dc[0][1][fi] >> pol_par_dc[0][2][fi] >> pol_par_dc[0][3][fi] >> pol_par_dc[0][4][fi] >>
            pol_par_dc[1][0][fi] >> pol_par_dc[1][1][fi] >> pol_par_dc[1][2][fi] >> pol_par_dc[1][3][fi] >> pol_par_dc[1][4][fi] >>
            pol_par_dc[2][0][fi] >> pol_par_dc[2][1][fi] >> pol_par_dc[2][2][fi] >> pol_par_dc[2][3][fi] >> pol_par_dc[2][4][fi] >>
            scaling[fi];
    fin.close();

    // z local xa->vb (cm)
    Double_t arr1[8] = {9.3, 8.1, 3.5, 2.3, -2.3, -3.5, -8.1, -9.3};
    for (Int_t iSize = 0; iSize < 4 * N; iSize++)
        z_loc[iSize] = arr1[iSize];

    // z global dc 1 & dc 2 (cm)
    Double_t arr2[16] = {-45.7, -46.9, -51.5, -52.7, -57.3, -58.5, -63.1, -64.3, 64.3, 63.1, 58.5, 57.3, 52.7, 51.5, 46.9, 45.7};
    for (Int_t iSize = 0; iSize < 8 * N; iSize++)
        z_glob[iSize] = arr2[iSize];

    has7DC = new Bool_t[nChambers];
    x_mid = new Double_t[25 * N];
    y_mid = new Double_t[25 * N];
    aX = new Double_t[25 * N];
    aY = new Double_t[25 * N];
    imp = new Double_t[25 * N];
    leng = new Double_t[25 * N];
    Chi2_match = new Double_t[25 * N];
    v = new Double_t**[nChambers];
    u = new Double_t**[nChambers];
    y = new Double_t**[nChambers];
    x = new Double_t**[nChambers];
    v_Single = new Double_t**[nChambers];
    u_Single = new Double_t**[nChambers];
    y_Single = new Double_t**[nChambers];
    x_Single = new Double_t**[nChambers];
    sigm_v = new Double_t**[nChambers];
    sigm_u = new Double_t**[nChambers];
    sigm_y = new Double_t**[nChambers];
    sigm_x = new Double_t**[nChambers];
    Sigm_v_single = new Double_t**[nChambers];
    Sigm_u_single = new Double_t**[nChambers];
    Sigm_y_single = new Double_t**[nChambers];
    Sigm_x_single = new Double_t**[nChambers];
    segment_size = new Int_t*[nChambers];
    Chi2 = new Double_t*[nChambers];
    x_global = new Double_t*[nChambers];
    y_global = new Double_t*[nChambers];
    params = new Double_t**[nChambers];
    rh_segment = new Double_t**[nChambers];
    rh_sigm_segment = new Double_t**[nChambers];
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        v[iChamber] = new Double_t*[N];
        u[iChamber] = new Double_t*[N];
        y[iChamber] = new Double_t*[N];
        x[iChamber] = new Double_t*[N];
        v_Single[iChamber] = new Double_t*[N];
        u_Single[iChamber] = new Double_t*[N];
        y_Single[iChamber] = new Double_t*[N];
        x_Single[iChamber] = new Double_t*[N];
        sigm_v[iChamber] = new Double_t*[N];
        sigm_u[iChamber] = new Double_t*[N];
        sigm_y[iChamber] = new Double_t*[N];
        sigm_x[iChamber] = new Double_t*[N];
        Sigm_v_single[iChamber] = new Double_t*[N];
        Sigm_u_single[iChamber] = new Double_t*[N];
        Sigm_y_single[iChamber] = new Double_t*[N];
        Sigm_x_single[iChamber] = new Double_t*[N];
        segment_size[iChamber] = new Int_t[75 * N];
        Chi2[iChamber] = new Double_t[75 * N];
        x_global[iChamber] = new Double_t[75 * N];
        y_global[iChamber] = new Double_t[75 * N];
        params[iChamber] = new Double_t*[2 * N];
        rh_segment[iChamber] = new Double_t*[4 * N];
        rh_sigm_segment[iChamber] = new Double_t*[4 * N];
        for (Int_t iDim = 0; iDim < N; iDim++) {
            v[iChamber][iDim] = new Double_t[75 * N];
            u[iChamber][iDim] = new Double_t[75 * N];
            y[iChamber][iDim] = new Double_t[75 * N];
            x[iChamber][iDim] = new Double_t[75 * N];
            v_Single[iChamber][iDim] = new Double_t[20 * N];
            u_Single[iChamber][iDim] = new Double_t[20 * N];
            y_Single[iChamber][iDim] = new Double_t[20 * N];
            x_Single[iChamber][iDim] = new Double_t[20 * N];
            sigm_v[iChamber][iDim] = new Double_t[75 * N];
            sigm_u[iChamber][iDim] = new Double_t[75 * N];
            sigm_y[iChamber][iDim] = new Double_t[75 * N];
            sigm_x[iChamber][iDim] = new Double_t[75 * N];
            Sigm_v_single[iChamber][iDim] = new Double_t[20 * N];
            Sigm_u_single[iChamber][iDim] = new Double_t[20 * N];
            Sigm_y_single[iChamber][iDim] = new Double_t[20 * N];
            Sigm_x_single[iChamber][iDim] = new Double_t[20 * N];
        }
        for (Int_t iDim = 0; iDim < 2 * N; iDim++)
            params[iChamber][iDim] = new Double_t[75 * N];
        for (Int_t iDim = 0; iDim < 4 * N; iDim++) {
            rh_segment[iChamber][iDim] = new Double_t[75 * N];
            rh_sigm_segment[iChamber][iDim] = new Double_t[75 * N];
        }
    }
    pairs = new Int_t*[nChambers];
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++)
        pairs[iChamber] = new Int_t[nWires];
    singles = new Int_t**[nChambers];
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        singles[iChamber] = new Int_t*[nWires];
        for (Int_t iWire = 0; iWire < nWires; iWire++)
            singles[iChamber][iWire] = new Int_t[nLayers];
    }
    nSegments = new Int_t[nSegmentsMax];

    return kSUCCESS;
}

void BmnDchTrackFinder::PrepareArraysToProcessEvent() {
    nSegmentsToBeMatched = -1;
    fDchTracks->Clear();

    // Array cleaning and initializing
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        for (Int_t iWire = 0; iWire < nWires; iWire++) {
            pairs[iChamber][iWire] = 0;
            for (Int_t iLayer = 0; iLayer < nLayers; iLayer++)
                singles[iChamber][iWire][iLayer] = 0;
        }

        has7DC[iChamber] = kFALSE;
 
        for (Int_t iDim1 = 0; iDim1 < 2 * N; iDim1++)
            for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++)
                params[iChamber][iDim1][iDim2] = -999.;
        for (Int_t iDim1 = 0; iDim1 < 75 * N; iDim1++) {
            segment_size[iChamber][iDim1] = 0;
            Chi2[iChamber][iDim1] = 50.;
            x_global[iChamber][iDim1] = -999.;
            y_global[iChamber][iDim1] = -999.;
        }
        for (Int_t iDim1 = 0; iDim1 < N; iDim1++) {
            for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++) {
                v[iChamber][iDim1][iDim2] = -999.;
                u[iChamber][iDim1][iDim2] = -999.;
                y[iChamber][iDim1][iDim2] = -999.;
                x[iChamber][iDim1][iDim2] = -999.;
                sigm_v[iChamber][iDim1][iDim2] = 1.;
                sigm_u[iChamber][iDim1][iDim2] = 1.;
                sigm_y[iChamber][iDim1][iDim2] = 1.;
                sigm_x[iChamber][iDim1][iDim2] = 1.;
            }
            for (Int_t iDim3 = 0; iDim3 < 20 * N; iDim3++) {
                v_Single[iChamber][iDim1][iDim3] = -999.;
                u_Single[iChamber][iDim1][iDim3] = -999.;
                y_Single[iChamber][iDim1][iDim3] = -999.;
                x_Single[iChamber][iDim1][iDim3] = -999.;
                Sigm_v_single[iChamber][iDim1][iDim3] = 1.;
                Sigm_u_single[iChamber][iDim1][iDim3] = 1.;
                Sigm_y_single[iChamber][iDim1][iDim3] = 1.;
                Sigm_x_single[iChamber][iDim1][iDim3] = 1.;
            }
        }
        for (Int_t iDim1 = 0; iDim1 < 4 * N; iDim1++)
            for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++) {
                rh_segment[iChamber][iDim1][iDim2] = -999.;
                rh_sigm_segment[iChamber][iDim1][iDim2] = 1.;
            }
    }
    for (Int_t iSegment = 0; iSegment < nSegmentsMax; iSegment++)
        nSegments[iSegment] = 0;
    for (Int_t iDim = 0; iDim < 25 * N; iDim++) {
        x_mid[iDim] = -999.;
        y_mid[iDim] = -999.;
        aX[iDim] = -999.;
        aY[iDim] = -999.;
        leng[iDim] = -999.;
        imp[iDim] = -999.;
    }
}

void BmnDchTrackFinder::Finish() {
	digisDCH.clear();	
	//===============================================================================================================
	TFile *ptr = gFile;
	if(histoOutput){
		FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[BmnDCHTrackFinder::Finish] Update  %s file. ", fhTestFlnm.Data());
		TFile file(fhTestFlnm.Data(), "RECREATE");
		fhList.Write(); 
		file.Close();
	}
	//===============================================================================================================
	return;
	// Delete 1d-arrays
    
	delete [] nSegments;
    delete [] has7DC;
    delete [] x_mid;
    delete [] y_mid;
    delete [] aX;
    delete [] aY;
    delete [] leng;
    delete [] imp;
    // Delete 2d-arrays and 3d-arrays
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        delete [] x_global[iChamber];
        delete [] y_global[iChamber];
        delete [] Chi2[iChamber];
        delete [] pairs[iChamber];
        delete [] segment_size[iChamber];
        for (Int_t iDim = 0; iDim < N; iDim++) {
            delete [] v[iChamber][iDim];
            delete [] u[iChamber][iDim];
            delete [] y[iChamber][iDim];
            delete [] x[iChamber][iDim];
            delete [] sigm_v[iChamber][iDim];
            delete [] sigm_u[iChamber][iDim];
            delete [] sigm_y[iChamber][iDim];
            delete [] sigm_x[iChamber][iDim];
            delete [] v_Single[iChamber][iDim];
            delete [] u_Single[iChamber][iDim];
            delete [] y_Single[iChamber][iDim];
            delete [] x_Single[iChamber][iDim];
            delete [] Sigm_v_single[iChamber][iDim];
            delete [] Sigm_u_single[iChamber][iDim];
            delete [] Sigm_y_single[iChamber][iDim];
            delete [] Sigm_x_single[iChamber][iDim];
        }
        for (Int_t iDim = 0; iDim < 2 * N; iDim++)
            delete [] params[iChamber][iDim];
        for (Int_t iDim = 0; iDim < 4 * N; iDim++) {
            delete [] rh_segment[iChamber][iDim];
            delete [] rh_sigm_segment[iChamber][iDim];
        }
        delete [] v[iChamber];
        delete [] u[iChamber];
        delete [] y[iChamber];
        delete [] x[iChamber];
        delete [] sigm_v[iChamber];
        delete [] sigm_u[iChamber];
        delete [] sigm_y[iChamber];
        delete [] sigm_x[iChamber];
        delete [] v_Single[iChamber];
        delete [] u_Single[iChamber];
        delete [] y_Single[iChamber];
        delete [] x_Single[iChamber];
        delete [] Sigm_v_single[iChamber];
        delete [] Sigm_u_single[iChamber];
        delete [] Sigm_y_single[iChamber];
        delete [] Sigm_x_single[iChamber];
        delete [] params[iChamber];
        delete [] rh_segment[iChamber];
        delete [] rh_sigm_segment[iChamber];
        for (Int_t iWire = 0; iWire < nWires; iWire++)
            delete [] singles[iChamber][iWire];
        delete [] singles[iChamber];
    }
    delete [] x_global;
    delete [] y_global;
    delete [] Chi2;
    delete [] pairs;
    delete [] segment_size;
    delete [] v;
    delete [] u;
    delete [] y;
    delete [] x;
    delete [] sigm_v;
    delete [] sigm_u;
    delete [] sigm_y;
    delete [] sigm_x;
    delete [] v_Single;
    delete [] u_Single;
    delete [] y_Single;
    delete [] x_Single;
    delete [] Sigm_v_single;
    delete [] Sigm_u_single;
    delete [] Sigm_y_single;
    delete [] Sigm_x_single;
    delete [] params;
    delete [] rh_segment;
    delete [] rh_sigm_segment;
    delete [] singles;
    cout << "Work time of the DCH track finder: " << workTime << " s" << endl;
}

Int_t BmnDchTrackFinder::Reconstruction(Int_t dchID, TString wire, Int_t pair, Int_t it_a, Int_t it_b,
        Double_t* wirenr_a, Double_t* wirenr_b, Double_t* time_a, Double_t* time_b,
        Bool_t* used_a, Bool_t* used_b,
        Double_t** _ab, Double_t** sigm_ab) {

    const Int_t arrIdxShift = (dchID == 2) ? 8 : 0;
    const Int_t arrIdxStart = (wire == "x") ? 0 : (wire == "y") ? 2 : (wire == "u") ? 4 : 6;

    Double_t a_pm[2], b_pm[2];

    for (Int_t i = 0; i < it_a; ++i)
        for (Int_t j = 0; j < it_b; ++j) {
            if (pair > 48)
                break;
            if ((wirenr_a[i] != wirenr_b[j] && wirenr_a[i] != wirenr_b[j] + 1))
                continue;
            Int_t func_nr_a = -1;
            Int_t func_nr_b = -1;
            for (Int_t t_it = 0; t_it < 4; t_it++)
                if (time_a[i] >= t_dc[t_it][0 + arrIdxStart + arrIdxShift] && time_a[i] < t_dc[t_it + 1][0 + arrIdxStart + arrIdxShift]) {
                    func_nr_a = t_it;
                    break;
                }

            Double_t time = time_a[i];
            Double_t d_a = 0;
            Double_t d_b = 0;

            if (func_nr_a == 1 || func_nr_a == 2) d_a = scale * (pol_par_dc[1][0][0 + arrIdxStart + arrIdxShift] + pol_par_dc[1][1][0 + arrIdxStart + arrIdxShift] * time +
                    pol_par_dc[1][2][0 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                    pol_par_dc[1][3][0 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                    pol_par_dc[1][4][0 + arrIdxStart + arrIdxShift] * Power(time, 4));

            else if (func_nr_a == 0) d_a = 0;

            else if (func_nr_a == 3) d_a = scale * (pol_par_dc[2][0][0 + arrIdxStart + arrIdxShift] + pol_par_dc[2][1][0 + arrIdxStart + arrIdxShift] * time +
                    pol_par_dc[2][2][0 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                    pol_par_dc[2][3][0 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                    pol_par_dc[2][4][0 + arrIdxStart + arrIdxShift] * Power(time, 4));

            for (Int_t t_it = 0; t_it < 4; t_it++)
                if (time_b[j] >= t_dc[t_it][1 + arrIdxStart + arrIdxShift] && time_b[j] < t_dc[t_it + 1][1 + arrIdxStart + arrIdxShift]) {
                    func_nr_b = t_it;
                    break;
                }

            time = time_b[j];

            if (func_nr_b == 1 || func_nr_b == 2) d_b = scale * (pol_par_dc[1][0][1 + arrIdxStart + arrIdxShift] + pol_par_dc[1][1][1 + arrIdxStart + arrIdxShift] * time +
                    pol_par_dc[1][2][1 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                    pol_par_dc[1][3][1 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                    pol_par_dc[1][4][1 + arrIdxStart + arrIdxShift] * Power(time, 4));

            else if (func_nr_b == 0) d_b = 0;

            else if (func_nr_b == 3) d_b = scale * (pol_par_dc[2][0][1 + arrIdxStart + arrIdxShift] + pol_par_dc[2][1][1 + arrIdxStart + arrIdxShift] * time +
                    pol_par_dc[2][2][1 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                    pol_par_dc[2][3][1 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                    pol_par_dc[2][4][1 + arrIdxStart + arrIdxShift] * Power(time, 4));

            a_pm[0] = wirenr_a[i] - 119 + d_a;
            a_pm[1] = wirenr_a[i] - 119 - d_a;
            b_pm[0] = wirenr_b[j] - 118.5 + d_b;
            b_pm[1] = wirenr_b[j] - 118.5 - d_b;

            Double_t dmin = LDBL_MAX;
            for (Int_t k = 0; k < 2; k++)
                for (Int_t m = 0; m < 2; m++)
                    if (Abs(a_pm[k] - b_pm[m]) < dmin) {
                        dmin = Abs(a_pm[k] - b_pm[m]);
                        _ab[0][pair] = a_pm[k];
                        _ab[1][pair] = b_pm[m];
                    }

            CompareDaDb(d_a, sigm_ab[0][pair]);
            CompareDaDb(d_b, sigm_ab[1][pair]);

            pair++;

            used_a[i] = kTRUE;
            used_b[j] = kTRUE;
        }
    return pair;
}

Int_t BmnDchTrackFinder::ReconstructionSingle(Int_t dchID, TString wire, TString lay, Int_t single, Int_t it,
        Double_t* wirenr, Double_t* time_, Bool_t* used,
        Double_t** _single, Double_t** sigm_single) {

    const Int_t arrIdxStart = (wire == "x") ? 0 : (wire == "y") ? 2 : (wire == "u") ? 4 : 6;

    const Int_t arrIdx1 = (lay == "a") ? 0 : 1;
    const Int_t arrIdx2 = (dchID == 2) ? 8 : 0;
    const Double_t coeff = (lay == "a") ? 119 : 118.5;

    for (Int_t i = 0; i < it; ++i) {
        if (used[i])
            continue;

        Int_t func_nr = -1;
        for (Int_t t_it = 0; t_it < 4; t_it++) {
            if (time_[i] >= t_dc[t_it][0 + arrIdxStart + arrIdx1 + arrIdx2] && time_[i] < t_dc[t_it + 1][0 + arrIdxStart + arrIdx1 + arrIdx2]) {
                func_nr = t_it;
                break;
            }
        }
        Double_t time = time_[i];
        Double_t d = 0;

        if (func_nr == 1 || func_nr == 2) d = scale * (pol_par_dc[1][0][0 + arrIdx1 + arrIdx2] + pol_par_dc[1][1][0 + arrIdx1 + arrIdx2] * time +
                pol_par_dc[1][2][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 2) +
                pol_par_dc[1][3][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 3) +
                pol_par_dc[1][4][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 4));

        else if (func_nr == 0) d = 0;

        else if (func_nr == 3) d = scale * (pol_par_dc[2][0][0 + arrIdx1 + arrIdx2] + pol_par_dc[2][1][0 + arrIdx1 + arrIdx2] * time +
                pol_par_dc[2][2][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 2) +
                pol_par_dc[2][3][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 3) +
                pol_par_dc[2][4][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 4));

        _single[0 + arrIdx1][single] = wirenr[i] - coeff + d;
        _single[0 + arrIdx1][single + 1] = wirenr[i] - coeff - d;

        CompareDaDb(d, sigm_single[0 + arrIdx1][single], sigm_single[0 + arrIdx1][single + 1]);

        single += 2;
    }
    return single;
}
ClassImp(BmnDchTrackFinder)

