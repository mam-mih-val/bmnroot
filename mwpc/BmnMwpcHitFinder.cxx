// @(#)bmnroot/mwpc:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-02-10

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcHitFinder                                                           //
//                                                                            //
// Implementation of an algorithm developed by                                //
// S. Merts and P. Batyuk                                                     //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for hits                                //
// in the Multi Wire Prop. Chambers of the BM@N experiment                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <Rtypes.h>
#include <climits>

#include "BmnMwpcHitFinder.h"
static Float_t workTime = 0.0;

using namespace std;
using namespace TMath;

BmnMwpcHitFinder::BmnMwpcHitFinder(Bool_t isExp) :
  fEventNo(0),
  fUseDigitsInTimeBin(kTRUE),
  expData(isExp) {
  fInputBranchName = "MWPC";
  fOutputBranchName = "BmnMwpcHit";
  thDist = 1.;
  nInputDigits = 3;
  nTimeSamples = 3;
  kBig = 100;
  }

BmnMwpcHitFinder::~BmnMwpcHitFinder() {

}

InitStatus BmnMwpcHitFinder::Init() {
    if (!expData)
        return kERROR;
    if (fVerbose) cout << " BmnMwpcHitFinder::Init() " << endl;

    FairRootManager* ioman = FairRootManager::Instance();
    fBmnMwpcDigitArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    if (!fBmnMwpcDigitArray)
    {
      cout<<"BmnMwpcHitFinder::Init(): branch "<<fInputBranchName<<" not found! Task will be deactivated"<<endl;
      SetActive(kFALSE);
      return kERROR;
    }

    fBmnMwpcHitArray = new TClonesArray(fOutputBranchName);
    ioman->Register(fOutputBranchName.Data(), "MWPC", fBmnMwpcHitArray, kTRUE);

    fMwpcGeometry = new BmnMwpcGeometry();
    kNChambers = fMwpcGeometry->GetNChambers();
    kNPlanes = fMwpcGeometry->GetNPlanes(); // 6
    kNWires = fMwpcGeometry->GetNWires();

    kMinHits = 4;
    kChi2_Max = 20.;
    
    kX1_sh = 1.775;
    kY1_sh = 8.25;
    kX1_slope_sh = 0;
    kY1_slope_sh = 0.01;

    kX2_sh = 1.63;
    kY2_sh = 7.57;
    kX2_slope_sh = 0;
    kY2_slope_sh = -.008;

    dw = fMwpcGeometry->GetWireStep();//0.25; // [cm] // wires step
    dw_half = 0.5*dw;
    sq3 = sqrt(3.);
    sq12 = sqrt(12.);
    sigma = dw/sq12;

    sigm2 = new Float_t[kNPlanes];
    h = new Int_t[kNPlanes];
    h6 = new Int_t[kNPlanes];
    ipl = new Int_t[kNPlanes];
    XVU = new Float_t[kNPlanes];
    XVU_cl = new Float_t[kNPlanes];
    
    kZ1_loc = new Float_t[kNPlanes];
    kZ2_loc = new Float_t[kNPlanes];

    dX_i = new Float_t[kNPlanes];
    z2 = new Float_t[kNPlanes];

    kPln = new Int_t[kNPlanes * kNChambers];
    iw = new Int_t[kNPlanes * kNChambers];
    iw_Ch1 = new Int_t[kNPlanes];
    iw_Ch2 = new Int_t[kNPlanes];
    
    wire_Ch1 = new Int_t*[kNWires];
    wire_Ch2 = new Int_t*[kNWires];
    xuv_Ch1 = new Float_t*[kNWires];
    xuv_Ch2 = new Float_t*[kNWires];

    for(Int_t iWire=0; iWire < kNWires; iWire++){
      wire_Ch1[iWire] = new Int_t[kNPlanes];
      wire_Ch2[iWire] = new Int_t[kNPlanes];
      xuv_Ch1[iWire] = new Float_t[kNPlanes];
      xuv_Ch2[iWire] = new Float_t[kNPlanes];
    }
    
    ind_best_Ch1 = new Int_t[5];
    ind_best_Ch2 = new Int_t[5];    
    par_ab_Ch1 = new Double_t*[4];
    par_ab_Ch2 = new Double_t*[4];
    A = new Double_t*[4];
    b = new Double_t*[4];
    
    for(Int_t ii=0; ii<4; ii++){
      par_ab_Ch2[ii] = new Double_t[100];
      par_ab_Ch1[ii] = new Double_t[100];
      A[ii] = new Double_t[4];
      b[ii] = new Double_t[4];
    }

    Wires_Ch1 = new Int_t*[kNPlanes];
    Wires_Ch2 = new Int_t*[kNPlanes];    
    clust_Ch1 = new Int_t*[kNPlanes];
    clust_Ch2 = new Int_t*[kNPlanes];    
    XVU_Ch1 = new Float_t*[kNPlanes];
    XVU_Ch2 = new Float_t*[kNPlanes];
    Nhits_Ch1 = new Int_t[kBig];
    Nhits_Ch2 = new Int_t[kBig];

    for(Int_t iPlane=0; iPlane<kNPlanes; iPlane++){
      Wires_Ch1[iPlane] = new Int_t[kBig];
      Wires_Ch2[iPlane] = new Int_t[kBig];      
      clust_Ch1[iPlane] = new Int_t[kBig];
      clust_Ch2[iPlane] = new Int_t[kBig];      
      XVU_Ch1[iPlane] = new Float_t[kBig];
      XVU_Ch2[iPlane] = new Float_t[kBig];      
    }

    ind_best_Ch1 = new Int_t[5];
    ind_best_Ch2 = new Int_t[5];
    Chi2_ndf_Ch1 = new Double_t[kBig];
    Chi2_ndf_Ch2 = new Double_t[kBig];
    Chi2_ndf_best_Ch1 = new Double_t[5];
    Chi2_ndf_best_Ch2 = new Double_t[5];
    
    //  Ch :                     1                                    2
    //                      v+  u-  x-  v-  u+  x+         v+  u-  x-   v-  u+  x+
    //    kPln[12] =      {  4,  5,  0,  1,  2,  3,         7, 11,  6,  10,  8,  9 };  //run6-II   r.1397-last
    kPln[0] = 4;
    kPln[1] = 5;
    kPln[2] = 0;
    kPln[3] = 1;
    kPln[4] = 2;
    kPln[5] = 3;

    kPln[6] = 7;
    kPln[7] = 11;
    kPln[8] = 6;
    kPln[9] = 10;
    kPln[10] = 8;
    kPln[11] = 9;
    
    //                           x-    v-    u+    x+    v+    u-   // canonical order
    //    kZ1_loc[6] = {-0.5,  0.5,  1.5,  2.5, -2.5, -1.5}; //cm   run5
    kZ1_loc[0] = -0.5;
    kZ1_loc[1] = 0.5;
    kZ1_loc[2] = 1.5;
    kZ1_loc[3] = 2.5;
    kZ1_loc[4] = -2.5;
    kZ1_loc[5] = -1.5;
    //    kZ2_loc[6] = {-0.5,  0.5,  1.5,  2.5, -2.5, -1.5}; //cm   run5, run6
    kZ2_loc[0] = -0.5;
    kZ2_loc[1] = 0.5;
    kZ2_loc[2] = 1.5;
    kZ2_loc[3] = 2.5;
    kZ2_loc[4] = -2.5;
    kZ2_loc[5] = -1.5;
    
    
    return kSUCCESS;
}

void BmnMwpcHitFinder::PrepareArraysToProcessEvent(){

      fBmnMwpcHitArray->Clear();

      // Clean and initialize arrays:
      for(Int_t iPl=0; iPl<kNPlanes; iPl++){
	iw_Ch1[iPl] = 0;
	iw_Ch2[iPl] = 0;
	sigm2[iPl] = sigma*sigma;
	h[iPl] = 0;
	h6[iPl] = 1;
	ipl[iPl] = 6;
	XVU[iPl] = 0;
	XVU_cl[iPl] = 0;
	dX_i[iPl] = 0;
	z2[iPl] = 0;
      }

      for(Int_t iPl2=0; iPl2<kNPlanes*kNChambers; iPl2++){
	iw[iPl2] = 0;
      }

      for(Int_t iWire=0; iWire<kNWires; iWire++){
	for(Int_t iPlane=0; iPlane<kNPlanes; iPlane++){
	  wire_Ch1[iWire][iPlane] = 0;
	  wire_Ch2[iWire][iPlane] = 0;
	  xuv_Ch1[iWire][iPlane] = 0;
	  xuv_Ch2[iWire][iPlane] = 0;
	}
      }

      Nbest_Ch1 = 0;
      Nseg_Ch1 = 0;
      Nbest_Ch2 = 0;
      Nseg_Ch2 = 0;

      for(Int_t i=0; i<5; i++){
	ind_best_Ch1[i] = 0;
	ind_best_Ch2[i] = 0;
	Chi2_ndf_best_Ch1[i] = -999.;
	Chi2_ndf_best_Ch2[i] = -999.;	
      }

      for(Int_t ii=0; ii<4; ii++){
	for(Int_t jj=0; jj<100; jj++){
	  par_ab_Ch1[ii][jj] = 999.;
	  par_ab_Ch2[ii][jj] = 999.;
	}
      }

      for(Int_t ii=0; ii<4; ii++){
	for(Int_t jj=0; jj<4; jj++){
	  A[ii][jj] = 0.;
	  b[ii][jj] = 0.;
	}
      }
      
      for(Int_t iPlane=0; iPlane<kNPlanes; iPlane++){
	for(Int_t iBig=0; iBig<kBig; iBig++){
	  Wires_Ch1[iPlane][iBig] = -1;
	  Wires_Ch2[iPlane][iBig] = -1;
	  clust_Ch1[iPlane][iBig] = -1;
	  clust_Ch2[iPlane][iBig] = -1;	  
	  XVU_Ch1[iPlane][iBig] = -999.;	  
	  XVU_Ch2[iPlane][iBig] = -999.;
	}
      }
      
      for(Int_t iBig=0; iBig < kBig; iBig++){
	Chi2_ndf_Ch1[iBig] = 0;
	Chi2_ndf_Ch2[iBig] = 0;
      }
}

void BmnMwpcHitFinder::Exec(Option_t* opt) {
    if (!IsActive())
        return;
    clock_t tStart = clock();
    PrepareArraysToProcessEvent();
    if (fVerbose) cout << "\n======================== MWPC hit finder exec started =====================\n" << endl;
    if (fVerbose) cout << "Event number: " << fEventNo++ << endl;
    cout<<"NWires = "<<kNWires<<", NPlanes = "<<kNPlanes<<endl;
    
    Short_t wn, pn, ts, pl;

    
    for (Int_t iDigit = 0; iDigit < fBmnMwpcDigitArray->GetEntriesFast(); iDigit++) {
      //        BmnMwpcDigit* digit = (BmnMwpcDigit*) fBmnMwpcDigitArray->UncheckedAt(iDigit);
      //digit->SetUsing(kFALSE);
      BmnMwpcDigit* digit = (BmnMwpcDigit*) fBmnMwpcDigitArray->At(iDigit);
      wn = digit->GetWireNumber();
      pl = digit->GetPlane();
      ts = digit->GetTime();
      cout<<"++++++++"<<endl;
      cout<<"wn = "<<wn<<", pl = "<<pl<<", ts = "<<ts<<endl;
	// digits[digit->GetPlane() / kNPlanes][digit->GetPlane() % kNPlanes].push_back(digit);
      pn = kPln[pl];

      Bool_t repeat = 0;
      Int_t pn0 = 6;
      if (pn < 6) pn0 = 0;

      if (iw[pn] > 0) {
	for (Int_t ix = 0; ix < iw[pn]; ix++) {
	  if (wn == wire_Ch1[ix][pn - pn0] && pn0 == 0 ) {
	    repeat = 1;
	    break;
	  }
	  if (wn == wire_Ch2[ix][pn - pn0] && pn0 == 6) {
	    repeat = 1;
	    break;
	  }
	}//ix
      }

      if (repeat) continue;

      cout<<"pl = "<<pl<<", pn = "<<pn<<", wn = "<<wn<<endl;
      
      if ( pn < 6) {
	wire_Ch1[iw[pn]][pn - pn0] = wn;
	xuv_Ch1[iw[pn]][pn - pn0] = (wn - 47.25) * dw;
	//	cout<<" pn "<<pn<<" wire_Ch1 "<<wire_Ch1[iw[pn]][pn - pn0]<<" xuv_Ch1 "<<xuv_Ch1[iw[pn]][pn - pn0]<<endl;
      }

      if ( pn > 5){
	wire_Ch2[iw[pn]][pn - pn0] = wn;
	xuv_Ch2[iw[pn]][pn - pn0] = (wn - 47.25) * dw;//46.75
	//cout<<" pn "<<pn<<" wire_Ch2 "<<wire_Ch2[iw[pn]][pn - pn0]<<" xuv_Ch2 "<<xuv_Ch2[iw[pn]][pn - pn0]<<endl; 
      }
      if (pn == 0 || pn == 1 || pn == 5 ) xuv_Ch1[iw[pn]][pn - pn0] = -xuv_Ch1[iw[pn]][pn - pn0]; //for run 5
      if (pn == 6 || pn == 7 || pn == 11) xuv_Ch2[iw[pn]][pn - pn0] = -xuv_Ch2[iw[pn]][pn - pn0];
      
      iw[pn]++;

      if (pn0 < 6) iw_Ch1[pn - pn0] = iw[pn];
      if (pn0 > 5) iw_Ch2[pn - pn0] = iw[pn];
    }// iDigit
    
    //    for (Int_t iChamber = 0; iChamber < kNChambers; iChamber++) {

          for(Int_t iCase=1; iCase < 9; iCase ++){
	    SegmentFinder(1, Wires_Ch1, clust_Ch1, XVU_Ch1, kMinHits);
	    //SegmentFinder(1, Wires_Ch1, clust_Ch1, XVU_Ch1, Nhits_Ch1, iw_Ch1, Nseg_Ch1, wire_Ch1, xuv_Ch1, kMinHits, iCase, kBig);
	//      	SegmentFinder(2, Wires_Ch2, clust_Ch2, XVU_Ch2, Nhits_Ch2, iw_Ch2, Nseg_Ch2, wire_Ch2, xuv_Ch2, kMinHits, iCase, kBig);
      }
	  /*	  
      if(Nseg_Ch1 > 0) ProcessSegments(1, sigma, dw_half, kZ1_loc, kMinHits, Nseg_Ch1, Nhits_Ch1, Wires_Ch1, clust_Ch1, XVU_Ch1, Nbest_Ch1, ind_best_Ch1, Chi2_ndf_Ch1, Chi2_ndf_best_Ch1, par_ab_Ch1, A, kNPlanes, ipl, XVU, XVU_cl, kChi2_Max);
	  */
      // create a track and put in into TClonesArray:
      
      
      /*        for (Int_t iPlane = 0; iPlane < kNPlanes; iPlane++)
            digits_filtered[iChamber][iPlane] = CheckDigits(digits[iChamber][iPlane]);

        // Z-coordinate of created hit is considered as known Z-position of planes 1 and 4 respecting to the considering chamber (1 or 2) 
        CreateMwpcHits(CreateHitsBy3Planes(digits_filtered[iChamber][0], digits_filtered[iChamber][1], digits_filtered[iChamber][2],
                fMwpcGeometry->GetZPlanePos(iChamber, 1)), fBmnMwpcHitArray, iChamber);
        CreateMwpcHits(CreateHitsBy3Planes(digits_filtered[iChamber][3], digits_filtered[iChamber][4], digits_filtered[iChamber][5],
	fMwpcGeometry->GetZPlanePos(iChamber, 4)), fBmnMwpcHitArray, iChamber);*/


      
      //    }
    if (fVerbose) cout << "\n======================== MWPC hit finder exec finished ====================" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}
/*
void SegmentFinder(Int_t chNum, Int_t** wires_Ch, Int_t **clust_Ch, Float_t **XVU_Ch, Int_t *Nhits_Ch, Int_t *iw_Ch, Int_t &Nseg, Int_t **wires_glob, Float_t **xuv_glob, Int_t minHits, Short_t code, Int_t kBig) {
  //xuv_glob     - coordinates of all hits
  //wires_glob   - wires of all hits
    Float_t delta = (chNum == 3) ? 1.125 : 0.75; //0.5; //condition for the nearest "lighted" wires
   Int_t cntr_sumWW_x =  95 ;//center wires Wx1+Wx2 due to x-slope
   Int_t min_sumWW_x =  (chNum == 3) ? 3 : 2; //min sum of wires to 95 

     Int_t min_sumWW =  (chNum == 3) ? 3 : 2; //min sum of wires to 95  //wide UV

  Int_t cntr_sumWW =  95 ;//center wires W1+W2 due to u,v-slope
  //Int_t min_sumWW =  2 ; //min sum of wires to 95 //narrow UV

  Int_t minHits4_5= minHits;

  // code :
  // 1  {X-, V-, U+}
  // 2  {X-, V+, U+}
  // 3  {X-, V-, U-}
  // 7  {X+, V-, U+}
  // 5  {X+, V+, U+}
  // 6  {X+, V-, U-}
  // 4  {X-, V+, U-}
  // 8  {X+, V+, U-}
  
  Int_t x = 0, v = 1, u = 2 , x1 = 3, v1 = 4, u1 = 5;//MK
  
  switch (code) {
  case 1:
    x = 0;
    v = 1;
    u = 2;
    x1 = 3;
    v1 = 4;
    u1 = 5;
    break;
  case 2:
    x = 0;
    v = 4;
    u = 2;
    x1 = 3;
    v1 = 1;
    u1 = 5;
    break;
  case 3:
    x = 0;
    v = 1;
    u = 5;
    x1 = 3;
    v1 = 4;
    u1 = 2;
    break;
  case 7:
    x = 3;
    v = 1;
    u = 2;
    x1 = 0;
    v1 = 4;
    u1 = 5;
    break;
  case 5:
    x = 3;
    v = 4;
    u = 2;
    x1 = 0;
    v1 = 1;
    u1 = 5;
    break;
  case 6:
    x = 3;
    v = 1;
    u = 5;
    x1 = 0;
    v1 = 4;
    u1 = 2;
    break;
  case 4:
    x = 0;
    v = 4;
    u = 5;
    x1 = 3;
    v1 = 1;
    u1 = 2;
    break;
  case 8:
    x = 3;
    v = 4;
    u = 5;
    x1 = 0;
    v1 = 1;
    u1 = 2;
    break;

  }

  if (Nseg > kBig - 2)return;// MP

    if (iw_Ch[x] > 0) {
    for (Int_t ix = 0; ix < iw_Ch[x]; ix++) {
      if (iw_Ch[v] > 0) {
	for (Int_t iv = 0; iv < iw_Ch[v]; iv++) {
	  if (iw_Ch[u] > 0) {
	    for (Int_t iu = 0; iu < iw_Ch[u]; iu++) {

	      if (Nseg > kBig - 2) return;

	      Bool_t it_was = 0;
		
	   
	      if (Nseg > 0) {
		for (Int_t iseg = 0; iseg < Nseg; iseg++) {
		  Bool_t it_was_x = 0;
		  Bool_t it_was_v = 0;
		  Bool_t it_was_u = 0;
		 
		  //		  ::out1<<" ch "<<chNum<<" case "<<code<<" iseg "<<iseg<<" ix "<<x<<" wr "<<wires_Ch[x][iseg]<<" iu "<<u<<" wr "<<wires_Ch[u][iseg]<<" iv "<<v<<" wr "<<wires_Ch[v][iseg]<<endl;

		  if (wires_Ch[x][iseg] == wires_glob[ix][x]) it_was_x = 1;
		  if ((clust_Ch[x][iseg] == -1 || clust_Ch[x][iseg] == 3) && (wires_Ch[x][iseg] -1 == wires_glob[ix][x])) it_was_x = 1; 
		  if ((clust_Ch[x][iseg] ==  1 || clust_Ch[x][iseg] == 3) && (wires_Ch[x][iseg] +1 == wires_glob[ix][x])) it_was_x = 1;
		  if (wires_Ch[v][iseg] == wires_glob[iv][v]) it_was_v = 1;
		  if ((clust_Ch[v][iseg] == -1 || clust_Ch[v][iseg] == 3) && wires_Ch[v][iseg] -1 == wires_glob[iv][v]) it_was_v = 1; 
		  if ((clust_Ch[v][iseg] ==  1 || clust_Ch[v][iseg] == 3) && wires_Ch[v][iseg] +1 == wires_glob[iv][v]) it_was_v = 1;
		  if (wires_Ch[u][iseg] == wires_glob[iu][u]) it_was_u = 1;
		  if ((clust_Ch[u][iseg] == -1 || clust_Ch[u][iseg] == 3) && wires_Ch[u][iseg] -1 == wires_glob[iu][u]) it_was_u = 1; 
		  if ((clust_Ch[u][iseg] ==  1 || clust_Ch[u][iseg] == 3) && wires_Ch[u][iseg] +1 == wires_glob[iu][u]) it_was_u = 1;				 
		  //   bef.clustering    if (wires_Ch[x][iseg] == wires_glob[ix][x] && wires_Ch[v][iseg] == wires_glob[iv][v] && wires_Ch[u][iseg] == wires_glob[iu][u]) {
		  if (it_was_x * it_was_v * it_was_u)  { it_was = 1;//{ it_was = 1;
		     break;
		  }
		}  // iseg
	      }  //  Nseg > 0

	      if (it_was) continue;

	      if (fabs(xuv_glob[iu][u] + xuv_glob[iv][v] - xuv_glob[ix][x]) < delta) {
		//  3p-candidate new Nseg
		XVU_Ch[x][Nseg] = xuv_glob[ix][x];
		XVU_Ch[v][Nseg] = xuv_glob[iv][v];
		XVU_Ch[u][Nseg] = xuv_glob[iu][u];
		wires_Ch[x1][Nseg] = -1;
		wires_Ch[v1][Nseg] = -1;
		wires_Ch[u1][Nseg] = -1;
		wires_Ch[x][Nseg] = wires_glob[ix][x];
		wires_Ch[v][Nseg] = wires_glob[iv][v];
		wires_Ch[u][Nseg] = wires_glob[iu][u];
		Nhits_Ch[Nseg] = 3;
		//clustering
		clust_Ch[x][Nseg]=0;
		clust_Ch[v][Nseg]=0;
		clust_Ch[u][Nseg]=0;
			
		if ( Nseg < 20 ) {

		  Int_t below = 0;
		  Int_t above = 0;

		  for (Int_t icl = 0; icl < iw_Ch[u]; icl++) {
		    if ( wires_glob[icl][u] ==  wires_Ch[u][Nseg] -1 ) below = 1;
		    if ( wires_glob[icl][u] ==  wires_Ch[u][Nseg] +1 ) above = 1;
		  }
		  if (below == 1 && above == 0) 	clust_Ch[u][Nseg]= -1;
		  if (below == 0 && above == 1) 	clust_Ch[u][Nseg]= +1;
		  if (below == 1 && above == 1) 	clust_Ch[u][Nseg]=  3;

		  below = 0;
		  above = 0;
		  for (Int_t icl = 0; icl < iw_Ch[x]; icl++) {
		    if ( wires_glob[icl][x] ==  wires_Ch[x][Nseg] -1 ) below = 1;
		    if ( wires_glob[icl][x] ==  wires_Ch[x][Nseg] +1 ) above = 1;
		  }
		  if (below == 1 && above == 0) 	clust_Ch[x][Nseg]= -1;
		  if (below == 0 && above == 1) 	clust_Ch[x][Nseg]= +1;
		  if (below == 1 && above == 1) 	clust_Ch[x][Nseg]=  3;
				
		  below = 0;
		  above = 0;
		  for (Int_t icl = 0; icl < iw_Ch[v]; icl++) {
		    if ( wires_glob[icl][v] ==  wires_Ch[v][Nseg] -1 ) below = 1;
		    if ( wires_glob[icl][v] ==  wires_Ch[v][Nseg] +1 ) above = 1;
		  }
		  if (below == 1 && above == 0) 	clust_Ch[v][Nseg]= -1;
		  if (below == 0 && above == 1) 	clust_Ch[v][Nseg]= +1;
		  if (below == 1 && above == 1) 	clust_Ch[v][Nseg]=  3;

		}	//	if ( Nseg < 20 ) 
                               
		Int_t double_u2 = 0;

		if (iw_Ch[u1] > 0) {
		  for (Int_t iu2 = 0; iu2 < iw_Ch[u1]; iu2++) {
				      
		    Bool_t ifl = 0;
				     
		    if (abs( wires_Ch[u][Nseg] + wires_glob[iu2][u1] - cntr_sumWW) <  min_sumWW ) {
		      XVU_Ch[u1][Nseg] = xuv_glob[iu2][u1];
		      wires_Ch[u1][Nseg] = wires_glob[iu2][u1];
		      Nhits_Ch[Nseg] = 4;
		      clust_Ch[u1][Nseg]=0;

		      if ( Nseg < 20 ) {
			Int_t below = 0;
			Int_t above = 0;
			for (Int_t icl = 0; icl < iw_Ch[u1]; icl++) {
			  if ( wires_glob[icl][u1] ==  wires_Ch[u1][Nseg] -1 ) below = 1;
			  if ( wires_glob[icl][u1] ==  wires_Ch[u1][Nseg] +1 ) above = 1;
			  if ( wires_glob[icl][u1] ==  wires_Ch[u1][Nseg] +2 ) {
			    if (abs( wires_Ch[u][Nseg] + wires_glob[icl][u1] - cntr_sumWW) <  min_sumWW ) double_u2 = icl;			    
			  }
			}
			if (below == 1 && above == 0) 	clust_Ch[u1][Nseg]= -1;
			if (below == 0 && above == 1) 	clust_Ch[u1][Nseg]= +1;
			if (below == 1 && above == 1) 	clust_Ch[u1][Nseg]=  3;
		      }	//	if ( Nseg < 20 ) 

		      ifl = 1;
		    }

		    if ( ifl ) break;

		  }//iu2
		}//u1

		Int_t double_v2 = 0;

		if (iw_Ch[v1] > 0) {
		  for (Int_t iv2 = 0; iv2 < iw_Ch[v1]; iv2++) {
				      
		    Bool_t ifl = 0;

		    if (abs( wires_Ch[v][Nseg] + wires_glob[iv2][v1] - cntr_sumWW) <  min_sumWW ) {
		      XVU_Ch[v1][Nseg] = xuv_glob[iv2][v1];
		      wires_Ch[v1][Nseg] = wires_glob[iv2][v1];

		      if ( wires_Ch[v1][Nseg] > -1 ) {
			Nhits_Ch[Nseg] = Nhits_Ch[Nseg] + 1;//5 points
			clust_Ch[v1][Nseg]=0;

			if ( Nseg < 20 ) {
			  Int_t below = 0;
			  Int_t above = 0;
			  for (Int_t icl = 0; icl < iw_Ch[v1]; icl++) {
			    if ( wires_glob[icl][v1] ==  wires_Ch[v1][Nseg] -1 ) below = 1;
			    if ( wires_glob[icl][v1] ==  wires_Ch[v1][Nseg] +1 ) above = 1;
			    if ( wires_glob[icl][v1] ==  wires_Ch[v1][Nseg] +2 ) {
			      if (abs( wires_Ch[v][Nseg] + wires_glob[icl][v1] - cntr_sumWW) <  min_sumWW ) double_v2 = icl;			    
			    }
			  }
			  if (below == 1 && above == 0) 	clust_Ch[v1][Nseg]= -1;
			  if (below == 0 && above == 1) 	clust_Ch[v1][Nseg]= +1;
			  if (below == 1 && above == 1) 	clust_Ch[v1][Nseg]=  3;
			}	//	if ( Nseg < 20 ) 

			ifl = 1;
		      }

		    }//abs( wires_Ch[v][Nseg] +

		    if ( ifl ) break;

		  }//iv2
		}//v1

		Int_t double_x2 = 0;

		if (iw_Ch[x1] > 0) {
		  for (Int_t ix2 = 0; ix2 < iw_Ch[x1]; ix2++) {
				      
		    Bool_t ifl = 0;

		    if(abs( wires_Ch[x][Nseg] + wires_glob[ix2][x1] - cntr_sumWW_x) <  min_sumWW_x ) {
		      XVU_Ch[x1][Nseg] = xuv_glob[ix2][x1];
		      wires_Ch[x1][Nseg] = wires_glob[ix2][x1];
                                        
		      if ( wires_Ch[x1][Nseg] > -1 ) {
			Nhits_Ch[Nseg] = Nhits_Ch[Nseg] + 1;//6 points
			clust_Ch[x1][Nseg]=0;

			if ( Nseg < 20 ) {
			  Int_t below = 0;
			  Int_t above = 0;
			  for (Int_t icl = 0; icl < iw_Ch[x1]; icl++) {
			    if ( wires_glob[icl][x1] ==  wires_Ch[x1][Nseg] -1 ) below = 1;
			    if ( wires_glob[icl][x1] ==  wires_Ch[x1][Nseg] +1 ) above = 1;
			    if ( wires_glob[icl][x1] ==  wires_Ch[x1][Nseg] +2 ) {
			      if (abs( wires_Ch[x][Nseg] + wires_glob[icl][x1] - cntr_sumWW_x) <  min_sumWW_x ) double_x2 = icl;			    
			    }
			  }
			  if (below == 1 && above == 0) 	clust_Ch[x1][Nseg]= -1;
			  if (below == 0 && above == 1) 	clust_Ch[x1][Nseg]= +1;
			  if (below == 1 && above == 1) 	clust_Ch[x1][Nseg]=  3;
			}	//	if ( Nseg < 20 ) 

			ifl = 1;
		      }//wires_Ch[x1][Nseg] > -1
		    }//abs( wires_Ch[x][Nseg] +

		    if ( ifl ) break;

		  }//ix2
		}//x1


		if (Nseg > 15) minHits4_5=5;
		if (Nseg > 30) minHits4_5=6;

		if (Nhits_Ch[Nseg] >= minHits4_5) {
		  //		  ::out1<<"find iseg "<<Nseg<<" wires_Ch "<< wires_Ch[0][Nseg]<<" "<< wires_Ch[1][Nseg]<<" "<<wires_Ch[2][Nseg]<<" "<<wires_Ch[3][Nseg]<<" "<<wires_Ch[4][Nseg]<<" "<<wires_Ch[5][Nseg]<<endl;
		  Nseg++;

		    if ( double_u2 > 0 && Nseg <= 99 ) {
		      //   ::out1<<"   u2 double "<<double_u2<<" Wire "<< wires_glob[double_u2][u1] <<endl;
		      Nhits_Ch[Nseg] = Nhits_Ch[Nseg-1];
		      XVU_Ch[x][Nseg] = XVU_Ch[x][Nseg-1];
		      XVU_Ch[x1][Nseg] = XVU_Ch[x1][Nseg-1];
		      XVU_Ch[u][Nseg] = XVU_Ch[u][Nseg-1];
		      XVU_Ch[v][Nseg] = XVU_Ch[v][Nseg-1];
		      XVU_Ch[v1][Nseg] = XVU_Ch[v1][Nseg-1];
		      XVU_Ch[u1][Nseg] = xuv_glob[double_u2][u1];
		      wires_Ch[x][Nseg] = wires_Ch[x][Nseg-1];
		      wires_Ch[x1][Nseg] = wires_Ch[x1][Nseg-1];
		      wires_Ch[u][Nseg] = wires_Ch[u][Nseg-1];
		      wires_Ch[v][Nseg] = wires_Ch[v][Nseg-1];
		      wires_Ch[v1][Nseg] = wires_Ch[v1][Nseg-1];
		      wires_Ch[u1][Nseg] = wires_glob[double_u2][u1];
		      //clustering
		      clust_Ch[x][Nseg] = clust_Ch[x][Nseg-1];
		      clust_Ch[x1][Nseg] = clust_Ch[x1][Nseg-1];
		      clust_Ch[u][Nseg] = clust_Ch[u][Nseg-1];
		      clust_Ch[v][Nseg] = clust_Ch[v][Nseg-1];
		      clust_Ch[v1][Nseg] = clust_Ch[v1][Nseg-1];
		      clust_Ch[u1][Nseg]=0;
		      //		        ::out1<<"find doub_u2 iseg "<<Nseg<<" wires_Ch "<< wires_Ch[0][Nseg]<<" "<< wires_Ch[1][Nseg]<<" "<<wires_Ch[2][Nseg]<<" "<<wires_Ch[3][Nseg]<<" "<<wires_Ch[4][Nseg]<<" "<<wires_Ch[5][Nseg]<<endl;
		      Nseg++;
		      if (Nseg > kBig - 2) break;
		    }

		    if ( double_v2 > 0 && Nseg <= 99 ) { 
		      // ::out1<<"   v2 double "<<double_v2<<" Wire "<< wires_glob[double_v2][v1] <<endl;
		      Nhits_Ch[Nseg] = Nhits_Ch[Nseg-1];
		      XVU_Ch[x][Nseg] = XVU_Ch[x][Nseg-1];
		      XVU_Ch[x1][Nseg] = XVU_Ch[x1][Nseg-1];
		      XVU_Ch[u][Nseg] = XVU_Ch[u][Nseg-1];
		      XVU_Ch[v][Nseg] = XVU_Ch[v][Nseg-1];
		      XVU_Ch[u1][Nseg] = XVU_Ch[u1][Nseg-1];
		      XVU_Ch[v1][Nseg] = xuv_glob[double_v2][v1];
		      wires_Ch[x][Nseg] = wires_Ch[x][Nseg-1];
		      wires_Ch[x1][Nseg] = wires_Ch[x1][Nseg-1];
		      wires_Ch[u][Nseg] = wires_Ch[u][Nseg-1];
		      wires_Ch[v][Nseg] = wires_Ch[v][Nseg-1];
		      wires_Ch[u1][Nseg] = wires_Ch[u1][Nseg-1];
		      wires_Ch[v1][Nseg] = wires_glob[double_v2][v1];
		      //clustering
		      clust_Ch[x][Nseg] = clust_Ch[x][Nseg-1];
		      clust_Ch[x1][Nseg] = clust_Ch[x1][Nseg-1];
		      clust_Ch[u][Nseg] = clust_Ch[u][Nseg-1];
		      clust_Ch[v][Nseg] = clust_Ch[v][Nseg-1];
		      clust_Ch[u1][Nseg] = clust_Ch[u1][Nseg-1];
		      clust_Ch[v1][Nseg]=0;
		      //		        ::out1<<"find doub_v2 iseg "<<Nseg<<" wires_Ch "<< wires_Ch[0][Nseg]<<" "<< wires_Ch[1][Nseg]<<" "<<wires_Ch[2][Nseg]<<" "<<wires_Ch[3][Nseg]<<" "<<wires_Ch[4][Nseg]<<" "<<wires_Ch[5][Nseg]<<endl;
		      Nseg++;
		      if (Nseg > kBig - 2) break;
		    }

		    if ( double_x2 > 0 && Nseg <= 99 ) { 
		      // ::out1<<"   x2 double "<<double_x2<<" Wire "<< wires_glob[double_x2][x1] <<endl;
		      Nhits_Ch[Nseg] = Nhits_Ch[Nseg-1];
		      XVU_Ch[x][Nseg] = XVU_Ch[x][Nseg-1];
		      XVU_Ch[u1][Nseg] = XVU_Ch[u1][Nseg-1];
		      XVU_Ch[u][Nseg] = XVU_Ch[u][Nseg-1];
		      XVU_Ch[v][Nseg] = XVU_Ch[v][Nseg-1];
		      XVU_Ch[v1][Nseg] = XVU_Ch[v1][Nseg-1];
		      XVU_Ch[x1][Nseg] = xuv_glob[double_x2][x1];
		      wires_Ch[x][Nseg] = wires_Ch[x][Nseg-1];
		      wires_Ch[u1][Nseg] = wires_Ch[u1][Nseg-1];
		      wires_Ch[u][Nseg] = wires_Ch[u][Nseg-1];
		      wires_Ch[v][Nseg] = wires_Ch[v][Nseg-1];
		      wires_Ch[v1][Nseg] = wires_Ch[v1][Nseg-1];
		      wires_Ch[x1][Nseg] = wires_glob[double_x2][x1];
		      //clustering
		      clust_Ch[x][Nseg] = clust_Ch[x][Nseg-1];
		      clust_Ch[u1][Nseg] = clust_Ch[u1][Nseg-1];
		      clust_Ch[u][Nseg] = clust_Ch[u][Nseg-1];
		      clust_Ch[v][Nseg] = clust_Ch[v][Nseg-1];
		      clust_Ch[v1][Nseg] = clust_Ch[v1][Nseg-1];
		      clust_Ch[x1][Nseg]=0;
		      //
		      //		      ::out1<<"find doub_x2 iseg "<<Nseg<<" wires_Ch "<< wires_Ch[0][Nseg]<<" "<< wires_Ch[1][Nseg]<<" "<<wires_Ch[2][Nseg]<<" "<<wires_Ch[3][Nseg]<<" "<<wires_Ch[4][Nseg]<<" "<<wires_Ch[5][Nseg]<<endl;
		      Nseg++;
		      if (Nseg > kBig - 2) break;
		    }

		  //
		  if (Nseg > 15) minHits4_5=5;
		  if (Nseg > 30) minHits4_5=6;
		} else {
		  Nhits_Ch[Nseg] = 0;
		  wires_Ch[x1][Nseg] = -1;
		  wires_Ch[v1][Nseg] = -1;
		  wires_Ch[u1][Nseg] = -1;
		  wires_Ch[x][Nseg] = -1;
		  wires_Ch[v][Nseg] = -1;
		  wires_Ch[u][Nseg] = -1;
		}//else
		if (Nseg > kBig - 2) break;
	      }// x v u < delta
	      if (Nseg > kBig - 2) break;
	    }//iu
	    if (Nseg > kBig - 2) break;
	  }//iw_Ch[u]
	  if (Nseg > kBig - 2) break;
	}//iv
	if (Nseg > kBig - 2) break;
      }//(iw_Ch[v]
      if (Nseg > kBig - 2) break;
    }//ix
    if (Nseg > kBig - 2)return;//MP
  }//iw_Ch[x]

  
}//SegmentFinder
*/

void BmnMwpcHitFinder::SegmentFinder(Int_t chNum, Int_t **wires_Ch, Int_t**clust_Ch, Float_t** XVU_Ch, Int_t minHits){

  //xuv_glob     - coordinates of all hits
  //wires_glob   - wires of all hits
  Float_t delta = (chNum == 3) ? 1.125 : 0.75; //0.5; //condition for the nearest "lighted" wires
   Int_t cntr_sumWW_x =  95 ;//center wires Wx1+Wx2 due to x-slope
   Int_t min_sumWW_x =  (chNum == 3) ? 3 : 2; //min sum of wires to 95 

     Int_t min_sumWW =  (chNum == 3) ? 3 : 2; //min sum of wires to 95  //wide UV

  Int_t cntr_sumWW =  95 ;//center wires W1+W2 due to u,v-slope
  //Int_t min_sumWW =  2 ; //min sum of wires to 95 //narrow UV

  Int_t minHits4_5= minHits;

}


void BmnMwpcHitFinder::ProcessSegments(
		     Int_t chNum,
		     Double_t sigma_, 
		     Float_t dw_half_,
		     Float_t *z_loc,
		     Int_t Min_hits,
		     Int_t Nseg,
		     Int_t *Nhits_Ch,
		     Int_t **Wires_Ch,
		     Int_t **clust_Ch,
		     Float_t **XVU_Ch,
		     Int_t Nbest_Ch,
		     Int_t *ind_best_Ch,
		     Double_t *Chi2_ndf_Ch,
		     Double_t *Chi2_ndf_best_Ch,
		     Double_t **par_ab_Ch,
		     Double_t **A_,
		     Int_t nPlanes,
		     Int_t* ipl_,
		     Float_t* XVU_,
		     Float_t* XVU_cl_,
		     Double_t kChi2_Max_,
		     Float_t *dX_i_) {

  Float_t delta = (chNum == 3) ? 1.125 : 0.75; //0.5;

  Float_t sigma2 = sigma_ * sigma_;
  //  Float_t sigm2[nPlanes] = {sigma2, sigma2, sigma2, sigma2, sigma2, sigma2};

  //  Int_t h6[nPlanes] = {1, 1, 1, 1, 1, 1};   
  Int_t Min_hits6 = Min_hits;
  for (Int_t Nhitm = nPlanes; Nhitm > Min_hits - 1; Nhitm--) {
    Int_t ifNhitm = 0;

    if (Nhitm < Min_hits6) break;

    for (Int_t iseg = 0; iseg < Nseg; iseg++) {

      if (Nhits_Ch[iseg] != Nhitm) continue;
      ifNhitm = 1;
  
      // Int_t h[nPlanes] = {0, 0, 0, 0, 0, 0};
      Int_t max_i[7] = {0, 0, 0, 0, 0, 0,   0};
      Int_t min_i[7] = {0, 0, 0, 0, 0, 0,   0};
      //      Int_t ipl_[nPlanes] = {6, 6, 6, 6, 6, 6};
      Int_t ifirst = -1;
      Int_t ilast =  6;
      if ( Nhits_Ch[iseg] > 4) {
	for (Int_t i = 0; i < nPlanes; i++){
	  if (Wires_Ch[i][iseg] > -1) { h[i] = 1;
	    if ( clust_Ch[i][iseg] ==  3) {min_i[i]= -2; 	max_i[i]= 2; ilast--; ipl_[ilast] = i;}
	    if ( clust_Ch[i][iseg] ==  0) {min_i[i]=  0; 	max_i[i]= 0; ifirst++; ipl_[ifirst] = i;}
	    if (i>1 && i<5 ){
	      if ( clust_Ch[i][iseg] == -1) {min_i[i]= -2;   max_i[i]= 0;  }
	      if ( clust_Ch[i][iseg] ==  1) {min_i[i]=  0;   max_i[i]= 2;  }
	    } else {
	      if ( clust_Ch[i][iseg] ==  1) {min_i[i]= -2;   max_i[i]= 0;  }
	      if ( clust_Ch[i][iseg] == -1) {min_i[i]=  0;   max_i[i]= 2;  }
	    }
	  }
	}
      
	for (Int_t i = 0; i < nPlanes; i++){
	  if (Wires_Ch[i][iseg] < 0 ) continue;
	  if ( abs(clust_Ch[i][iseg]) == 1) { ifirst++; ipl_[ifirst] = i;}
	}
      }// Nhits_Ch[iseg] > 4 
      else { for (Int_t i = 0; i < nPlanes; i++){ 
	  if (Wires_Ch[i][iseg] > -1)  h[i] = 1;
	  ipl_[i]=i;
	}
      }
      /*
        ::out1<<" iseg "<<iseg<<" clust_Ch "<<clust_Ch[0][iseg] <<" "<<clust_Ch[1][iseg] <<" "<<clust_Ch[2][iseg] <<" "<<clust_Ch[3][iseg] <<" "<<clust_Ch[4][iseg]<<" "<<clust_Ch[5][iseg]
	    <<" Wires_Ch "<<Wires_Ch[0][iseg]<<" "<<Wires_Ch[1][iseg]<<" "<<Wires_Ch[2][iseg]<<" "<<Wires_Ch[3][iseg]<<" "<<Wires_Ch[4][iseg]<<" "<<Wires_Ch[5][iseg]
	   <<" XVU_Ch "<<XVU_Ch[0][iseg]<<" "<<XVU_Ch[1][iseg]<<" "<<XVU_Ch[2][iseg]<<" "<<XVU_Ch[3][iseg]<<" "<<XVU_Ch[4][iseg]<<" "<<XVU_Ch[5][iseg]
	    <<" ipl "<< ipl_[0] <<" "<< ipl_[1] <<" "<< ipl_[2] <<" "<< ipl_[3] <<" "<< ipl_[4] <<" "<< ipl_[5] <<endl;
     //  */

      //linear fit
      //      Double_t A_[4][4]; //coef matrix
      Double_t F[4]; //free coef 
      
      Float_t dX[nPlanes];
      //      Float_t XVU[nPlanes];
      //      Float_t XVU_cl[nPlanes];
      Double_t Chi2_min_iseg = 999;
      Double_t par_ab_curr[4];
      Double_t par_ab_cl[4];
      
      for (Int_t i10 = min_i[ipl_[0]]; i10 <= max_i[ipl_[0]] ; i10++){ 
	//-?	if ( ipl_[0] == 6 ) continue;
	if ( h[ipl_[0]] == 1){
	  XVU_[ipl_[0]] = XVU_Ch[ipl_[0]][iseg] + dw_half_*i10 ;
	  sigm2[ipl_[0]] = sigma2 ;
	  if ( i10 == -1 || i10 == 1 )  sigm2[ipl_[0]] = 0.5*sigma2 ;
	}
	for (Int_t i2 = min_i[ipl_[1]]; i2 <= max_i[ipl_[1]] ; i2++){ 
	  //-?	 if ( ipl_[1] == 6 ) continue;
	  //-?	 XVU_[ipl_[1]] = XVU_Ch[ipl_[1]][iseg];
	 if ( h[ipl_[1]] == 1){
	    XVU_[ipl_[1]] = XVU_Ch[ipl_[1]][iseg] + dw_half_*i2 ;
	    if ( ( (ipl_[0] < 3 && ipl_[1] > 2) || (ipl_[0] > 2 && ipl_[1] < 3) ) &&  abs(ipl_[0] - ipl_[1]) == 3 ){//conjugated coord
	      if ( ipl_[0] + ipl_[1] >3   ) {
		//			::out1<<" ipl_[0] "<<ipl_[0]<<" ipl_[1] "<<ipl_[1]<<endl;
		if ( fabs(XVU_[ipl_[0]] - XVU_[ipl_[1]]) > 3*dw_half_ ) continue;
		//			::out1<<"     i2 "<<i2<<" XVU_-i2 "<<XVU_[ipl_[1]]<<" XVU_-conj "<<XVU_Ch[ipl_[0]]<<endl;
	      }
	    }//conjugated coord
	    sigm2[ipl_[1]] = sigma2 ;
	    if ( i2 == -1 || i2 == 1 )  sigm2[ipl_[1]] = 0.5*sigma2 ;
	 }
	  for (Int_t i3 = min_i[ipl_[2]]; i3 <= max_i[ipl_[2]] ; i3++){ 
	    //-?	    if ( ipl_[2] == 6 ) continue;
	    //-?	    XVU_[ipl_[2]] = XVU_Ch[ipl_[2]][iseg];
	    if ( h[ipl_[2]] == 1){
	      XVU_[ipl_[2]] = XVU_Ch[ipl_[2]][iseg] + dw_half_*i3 ;
	      Bool_t conj_bad = 0;//conjugated coord
	      for (Int_t ic=0; ic<2 ; ic++){
		if ( ( (ipl_[ic] < 3 && ipl_[2] > 2) || (ipl_[ic] > 2 && ipl_[2] < 3) ) &&  abs(ipl_[ic] - ipl_[2]) == 3 ){
		  if ( ipl_[ic] + ipl_[2] >3  ) {
		    //		      ::out1<<" ipl_[ic] "<<ipl_[ic]<<" ipl_[2] "<<ipl_[2]<<endl;
		    if ( fabs(XVU_[ipl_[ic]] - XVU_[ipl_[2]]) > 3*dw_half_ ) { conj_bad = 1; break;
		      //		        ::out1<<"     i3 "<<i3<<" XVU_-i3 "<<XVU_[ipl_[2]]<<" XVU_-conj "<<XVU_Ch[ipl_[ic]]<<endl;
		    }
		  }
		 
		}
	      }
	      if (  conj_bad ) continue; //conjugated coord
	      sigm2[ipl_[2]] = sigma2 ;
	      if ( i3 == -1 || i3 == 1 )  sigm2[ipl_[2]] = 0.5*sigma2 ;
	    }
	    for (Int_t i4 = min_i[ipl_[3]]; i4 <= max_i[ipl_[3]] ; i4++){ 
	      //-?	      if ( ipl_[3] == 6 ) continue;
	      //-?	      XVU_[ipl_[3]] = XVU_Ch[ipl_[3]][iseg];
	      if ( h[ipl_[3]] == 1){
		XVU_[ipl_[3]] = XVU_Ch[ipl_[3]][iseg] + dw_half_*i4 ;
		Bool_t conj_bad = 0;//conjugated coord
		for (Int_t ic=0; ic < 3 ; ic++){
		  if ( ( (ipl_[ic] < 3 && ipl_[3] > 2) || (ipl_[ic] > 2 && ipl_[3] < 3) ) &&  abs(ipl_[ic] - ipl_[3]) == 3 ){
		    if ( ipl_[ic] + ipl_[3] >3  ) {
		      //		         ::out1<<" ipl_[ic] "<<ipl_[ic]<<" ipl_[3] "<<ipl_[3]<<endl;
		      if ( fabs(XVU_[ipl_[ic]] - XVU_[ipl_[3]]) > 3*dw_half_ )  { conj_bad = 1; break;
			//			  ::out1<<"     i4 "<<i4<<" XVU_-i4 "<<XVU_[ipl_[3]]<<" XVU_-conj "<<XVU_Ch[ipl_[ic]]<<endl;
		      }
		    }
		    
		  }
		}
		if (  conj_bad ) continue; //conjugated coord
		sigm2[ipl_[3]] = sigma2 ;
		if ( i4 == -1 || i4 == 1 )  sigm2[ipl_[3]] = 0.5*sigma2 ;
	      }
	      for (Int_t i5 = min_i[ipl_[4]]; i5 <= max_i[ipl_[4]] ; i5++){ 
		//-?		if ( ipl_[4] == 6 ) continue;
		//-?		XVU_[ipl_[4]] = XVU_Ch[ipl_[4]][iseg];
		if ( h[ipl_[4]] == 1){
		  XVU_[ipl_[4]] = XVU_Ch[ipl_[4]][iseg] + dw_half_*i5 ;
		  Bool_t conj_bad = 0;//conjugated coord
		  for (Int_t ic=0; ic < 4 ; ic++){
		    if ( ( (ipl_[ic] < 3 && ipl_[4] > 2) || (ipl_[ic] > 2 && ipl_[4] < 3) ) &&  abs(ipl_[ic] - ipl_[4]) == 3 ){
		      if ( ipl_[ic] + ipl_[4] >3  ) {
			//				::out1<<" ipl_[ic] "<<ipl_[ic]<<" ipl_[4] "<<ipl_[4]<<endl;
			if ( fabs(XVU_[ipl_[ic]] - XVU_[ipl_[4]]) > 3*dw_half_ )  { conj_bad = 1; break;
			  //			  	  ::out1<<"     i5 "<<i5<<" XVU_-i5 "<<XVU_[ipl_[4]]<<" XVU_-conj "<<XVU_Ch[ipl_[ic]]<<endl;
			}
		      }
		     
		    }
		  }
		  if (  conj_bad ) continue; //conjugated coord
		  sigm2[ipl_[4]] = sigma2 ;
		  if ( i5 == -1 || i5 == 1 )  sigm2[ipl_[4]] = 0.5*sigma2 ;
		}
		//??	Double_t Chi2_i6 = 999;
		for (Int_t i6 = min_i[ipl_[5]]; i6 <= max_i[ipl_[5]] ; i6++){ 
		  //-?		  if ( ipl_[5] == 6 ) continue;
		  //-?		  XVU_[ipl_[5]] = XVU_Ch[ipl_[5]][iseg];
		  if ( h[ipl_[5]] == 1){
		    XVU_[ipl_[5]] = XVU_Ch[ipl_[5]][iseg] + dw_half_*i6 ;
		    Bool_t conj_bad = 0;//conjugated coord
		    Float_t conj =0;
		    for (Int_t ic=0; ic < 5 ; ic++){
		      if ( ( (ipl_[ic] < 3 && ipl_[5] > 2) || (ipl_[ic] > 2 && ipl_[5] < 3) ) &&  abs(ipl_[ic] - ipl_[5]) == 3 ){
			if ( ipl_[ic] + ipl_[5] >3  ) {
			  //			  	  ::out1<<" ipl_[ic] "<<ipl_[ic]<<" ipl_[5] "<<ipl_[5]<<endl;
			  if ( fabs(XVU_[ipl_[ic]] - XVU_[ipl_[5]]) > 3*dw_half_ ) { conj=(XVU_[ipl_[ic]] - XVU_[ipl_[5]]); conj_bad = 1; break;
			    //			    	     ::out1<<"     i6 "<<i6<<" XVU_-i6 "<<XVU_[ipl_[5]]<<" XVU_-conj "<<XVU_Ch[ipl_[ic]]<<endl;
			  }
			}
		
		      }
		    }
		    if (  conj_bad ) continue; //conjugated coord
		    sigm2[ipl_[5]] = sigma2 ;
		    if ( i6 == -1 || i6 == 1 )  sigm2[ipl_[5]] = 0.5*sigma2 ;
		  }
		
		  //		   ::out1<<" XVU_ "<<XVU_[0]<< " "<<XVU_[1]<<" "<<XVU_[2]<<" "<<XVU_[3]<<" "<<XVU_[4]<<" "<<XVU_[5]<<" sigm2 "<<sigm2[0]<<" "<<sigm2[1]<<" "<<sigm2[2]<<" "<<sigm2[3]<<" "<<sigm2[4]<<" "<<sigm2[5]<<" ii "<<i10<<i2<<i3<<i4<<i5<<i6<<endl;

		  Float_t xx = 0;
		  Int_t ii = 0;
		  if ( h[0] == 1 ){ ii = ii +1; xx = xx + XVU_[0]; }
		  if ( h[3] == 1 ){ ii = ii +1; xx = xx + XVU_[3]; xx = xx / ii; }
		  Float_t uu = 0;
		  ii = 0;
		  if ( h[2] == 1 ){ ii = ii +1; uu = uu + XVU_[2]; }
		  if ( h[5] == 1 ){ ii = ii +1; uu = uu + XVU_[5]; uu = uu / ii; }
		  Float_t vv = 0;
		  ii = 0;
		  if ( h[1] == 1 ){ ii = ii +1; vv = vv + XVU_[1]; }
		  if ( h[4] == 1 ){ ii = ii +1; vv = vv + XVU_[4]; vv = vv / ii; }
		 
		  if ( fabs (xx -uu -vv) > delta ) continue;//??

		  //       		  Double_t A_[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};//coef matrix
		  Double_t F[4] = {0,0,0,0};//free coef 

		  if (Nhits_Ch[iseg] == nPlanes) //case 6-point segment
		    FillFitMatrix(A_, z_loc, sigm2, h6, kNPlanes, z2);
		  else
		    FillFitMatrix(A_, z_loc, sigm2, h , kNPlanes, z2);

		  FillFreeCoefVector(F, XVU_, z_loc, sigm2, h, kNPlanes);
		  // FillFreeCoefVector(F, XVU_, iseg, z_loc, sigm2, h);

		  Double_t A0[4][4];
		  for (Int_t i1 = 0; i1 < 4; i1++)
		    for (Int_t j1 = 0; j1 < 4; j1++)
		      A0[i1][j1] = A[i1][j1];

		  //		  Double_t b[4][4];
		  InverseMatrix(A_, b);

		  Double_t sum;
		  Double_t A1[4][4];
	  //MK
		  for (Int_t i1 = 0; i1 < 4; ++i1) {
		    for (Int_t j1 = 0; j1 < 4; ++j1) {
		      sum = 0;
		      for (Int_t k1 = 0; k1 < 4; ++k1) {
			Double_t a0 = A0[i1][k1];
			Double_t b0 = b[k1][j1];
			sum += a0*b0;
		      }
		      A1[i1][j1] = sum;
		    }
		  }

		  for (Int_t i1 = 0; i1 < 4; i1++) {
		    par_ab_curr[i1] = 0;
		    for (Int_t j1 = 0; j1 < 4; j1++) {
		      par_ab_curr[i1] += b[i1][j1] * F[j1];
		    }
		  } 

		  //		  Float_t dX_i[nPlanes] = {0,0,0, 0,0,0};
		  Double_t Chi2_curr_iseg = 0;
		 
		  for (Int_t i1 = 0; i1 < nPlanes; i1++) {
		    if (Wires_Ch[i1][iseg]>-1) {
		      if (i1 == 0 || i1 == 3) dX_i_[i1] = XVU_[i1] - par_ab_curr[0] * z_loc[i1] - par_ab_curr[1];
		      if (i1 == 2 || i1 == 5) dX_i_[i1] = XVU_[i1] - 0.5 * (par_ab_curr[0] + sq3 * par_ab_curr[2]) * z_loc[i1] - 0.5 * (par_ab_curr[1] + sq3 * par_ab_curr[3]);
		      if (i1 == 1 || i1 == 4) dX_i_[i1] = XVU_[i1] - 0.5 * (par_ab_curr[0] - sq3 * par_ab_curr[2]) * z_loc[i1] - 0.5 * (par_ab_curr[1] - sq3 * par_ab_curr[3]);
		      Chi2_curr_iseg =  Chi2_curr_iseg + dX_i_[i1] * dX_i_[i1] / sigm2[i1];//??
		    } 
		  }//i1
		  		 
		  //		   ::out1<<"  --- Chi2_curr_iseg "<<Chi2_curr_iseg<<" Chi2_min_iseg "<<Chi2_min_iseg<<endl;

		  //??	  if ( Chi2_curr_iseg > Chi2_i6) break;              // optimization
		  if ( Chi2_curr_iseg > Chi2_min_iseg * 10. ) break; // optimization
		  //??	  Chi2_i6 =  Chi2_curr_iseg;

		  if ( Chi2_curr_iseg < Chi2_min_iseg) {
		    Chi2_min_iseg = Chi2_curr_iseg;
		    for (Int_t i1 = 0; i1 < nPlanes; i1++) {
		      dX[i1] = dX_i_[i1];
		      XVU_cl_[i1] = XVU_[i1];
		       if (i1 > 3) continue;
		      par_ab_cl[i1] =  par_ab_curr[i1];
		    //????
		    }
		  }

		}//i6
	      }
	    }
	  }
	}//i2
      }//i10

      Chi2_ndf_Ch[iseg] = Chi2_min_iseg; 

      if (Nhits_Ch[iseg] > 4)
	Chi2_ndf_Ch[iseg] = Chi2_ndf_Ch[iseg] / (Nhits_Ch[iseg] - 4);

      if (Chi2_ndf_Ch[iseg] > kChi2_Max_) {
	if (Nhits_Ch[iseg] <= Min_hits) { Nhits_Ch[iseg] = 0; continue;}
	else  { //reject most distant point
	  Float_t Max_dev = 0;
	  Int_t irej = -1;
	  for (Int_t i1 = 0; i1 < nPlanes; i1++)
	    if (Wires_Ch[i1][iseg]>-1)
	      if (fabs(dX[i1]) > Max_dev) {
		irej = i1;
		Max_dev = fabs(dX[i1]);
	      }
	  Wires_Ch[irej][iseg] = -1;
	  Nhits_Ch[iseg]--;// reject most distant point
	  continue;}
      }
      else {
	//	::out1<<" Ch= "<<chNum<<" iseg "<<iseg<<" Nhits(aft.i10) "<<Nhits_Ch[iseg]<<" Chi2/n "<<Chi2_ndf_Ch[iseg]<<" XVU_Ch "<<XVU_Ch[0][iseg]<<" "<<XVU_Ch[1][iseg]<<" "<<XVU_Ch[2][iseg]<<" "<<XVU_Ch[3][iseg]<<" "<<XVU_Ch[4][iseg]<<" "<<XVU_Ch[5][iseg]<<endl;
	for (Int_t i1 = 0; i1 < nPlanes; i1++) {
	  XVU_Ch[i1][iseg] = XVU_cl_[i1];
	  if (i1 > 3) continue;
	  par_ab_Ch[i1][iseg] =  par_ab_cl[i1];
	}
      }
   
    } // iseg

    if (!ifNhitm) continue;

    if (Nbest_Ch == 0) {

      Double_t Chi2_best = 9999;
      Int_t iseg_best = -1;
      for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	if (Nhits_Ch[iseg] != Nhitm) continue;
	if (Chi2_ndf_Ch[iseg] >= Chi2_best) continue;
	Chi2_best = Chi2_ndf_Ch[iseg];
	iseg_best = iseg;
      } // iseg

      if (iseg_best == -1) continue;
      /*
        ::out1<<" Ch= "<< chNum <<" iseg_best1 "<< iseg_best <<" Chi2_best "<< Chi2_best
			     <<" Wires_Ch "<< Wires_Ch[0][iseg_best]<<" "<< Wires_Ch[1][iseg_best]<<" "<< Wires_Ch[2][iseg_best]<<" "<< Wires_Ch[3][iseg_best]<<" "<< Wires_Ch[4][iseg_best]<<" "<< Wires_Ch[5][iseg_best]
			     <<" XVU_Ch "<<XVU_Ch[0][iseg_best]<<" "<<XVU_Ch[1][iseg_best]<<" "<<XVU_Ch[2][iseg_best]<<" "<<XVU_Ch[3][iseg_best]<<" "<<XVU_Ch[4][iseg_best]<<" "<<XVU_Ch[5][iseg_best]<<endl;
      // */  

      ind_best_Ch[0] = iseg_best;
      Chi2_ndf_best_Ch[0] = Chi2_best;
      Nbest_Ch = 1;
      
      if ( chNum == 1) {
	Min_hits6 = Min_hits;//??
	if ( Chi2_best < 0.6) Min_hits6 = Nhitm;//??
      }

      //    ::out1<<" Ch= "<<chNum<<" Nhits (bef.rej) "; for (int iseg=0; iseg< Nseg; iseg++){ ::out1<<Nhits_Ch[iseg]<<" ";  } ::out1<<endl;

      //reject(common points)
      for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	if (iseg == iseg_best)continue;
	/*
	Int_t u_cross = 0;
	Int_t v_cross = 0;
	Int_t x_cross = 0;
	*/

	for (Int_t i1 = 0; i1 < nPlanes; i1++) {
	  if (Wires_Ch[i1][iseg]>-1) {
	    /*   if (Wires_Ch[i1][iseg] == Wires_Ch[i1][iseg_best]) {
		 if (i1 == 0 || i1 == 3) x_cross = 1;
		 if (i1 == 1 || i1 == 4) v_cross = 1;
		 if (i1 == 2 || i1 == 5) u_cross = 1;
		 }
		 if (x_cross + v_cross + u_cross > 0) Nhits_Ch[iseg] = 0;
	    */

	    if( fabs(XVU_Ch[i1][iseg] - XVU_Ch[i1][iseg_best]) < 3*dw_half_ ) Nhits_Ch[iseg] = 0;
	  }
	}
      }// iseg

    }// Nbest_Ch == 0


    //    ::out1<<" Ch= "<<chNum<<" Nhits_(One best) "; for (int iseg=0; iseg< Nseg; iseg++){ ::out1<<Nhits_Ch[iseg]<<" ";  } ::out1<<endl;
	
    if (Nbest_Ch == 1) {
      Double_t Chi2_best = 9999;
      Int_t iseg_best2 = -1;
      for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	if (iseg == ind_best_Ch[0])continue;
	if (Nhits_Ch[iseg] != Nhitm) continue;
	if (Chi2_ndf_Ch[iseg] > Chi2_best) continue;
	Chi2_best = Chi2_ndf_Ch[iseg];
	iseg_best2 = iseg;
      } // iseg

      if (iseg_best2>-1) {
	ind_best_Ch[Nbest_Ch] = iseg_best2;
	Chi2_ndf_best_Ch[Nbest_Ch] = Chi2_best;
	Nbest_Ch++;

	//	if (chNum!=1)	::out1<<" Ch= "<< chNum <<" iseg_best2 "<< iseg_best2 <<" Chi2_best "<< Chi2_best<<" Wires_Ch "<< Wires_Ch[0][iseg_best2]<<" "
	//		      <<Wires_Ch[1][iseg_best2]<<" "<<Wires_Ch[2][iseg_best2]<<" "<<Wires_Ch[3][iseg_best2]<<" "<<Wires_Ch[4][iseg_best2]<<" "<<Wires_Ch[5][iseg_best2]<<endl;
		
	if (chNum == 2 || chNum == 3) {
	  //reject(common points)
	  for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	    if (iseg == ind_best_Ch[0] || iseg == iseg_best2) continue;
	    for (Int_t i1 = 0; i1 < nPlanes; i1++) {
	      if (Wires_Ch[i1][iseg]>-1) {
		if( fabs(XVU_Ch[i1][iseg] - XVU_Ch[i1][iseg_best2]) < 3*dw_half_ ) Nhits_Ch[iseg] = 0;
	      }
	    }
	  }//iseg
	}
      }

    }//Nbest_Ch == 1
	
    //    ::out1<<"Ch= "<<chNum<<" Nhits_Ch(Two bests) "; for (int iseg=0; iseg< Nseg; iseg++){  ::out1<<Nhits_Ch[iseg]<<" ";  }  ::out1<<endl;
	
    if (chNum == 2 || chNum == 3) {
      if (Nbest_Ch == 2) {

	Double_t Chi2_best = 20;
	Int_t iseg_best3 = -1;
	for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	  if (iseg == ind_best_Ch[0] || iseg == ind_best_Ch[1])continue;
	  if (Nhits_Ch[iseg] != Nhitm) continue;
	  if (Chi2_ndf_Ch[iseg] > Chi2_best) continue;
	  Chi2_best = Chi2_ndf_Ch[iseg];
	  iseg_best3 = iseg;
	} // iseg

	if (iseg_best3 >-1) {

	  ind_best_Ch[Nbest_Ch] = iseg_best3;
	  Chi2_ndf_best_Ch[Nbest_Ch] = Chi2_best;
	  Nbest_Ch++;

	  //reject(common points)
	  for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	   if (iseg == ind_best_Ch[0] || iseg == ind_best_Ch[1] || iseg == iseg_best3)continue;
	    for (Int_t i1 = 0; i1 < nPlanes; i1++) {
	      if (Wires_Ch[i1][iseg]>-1) {
		if( fabs(XVU_Ch[i1][iseg] - XVU_Ch[i1][iseg_best3]) < 3*dw_half_ ) Nhits_Ch[iseg] = 0;
	      }
	    }
	  }
	}
      }
    }// if (chNum == 2 || chNum == 3) 

  } // Nhitm

}// ProcessSegments

void BmnMwpcHitFinder::FillFitMatrix(Double_t** AA, Float_t* z, Float_t* sigm2_, Int_t* h_, Int_t nPlanes, Float_t *z2_) {

  //out1<<" in FillFitMatrix "<<endl;

    // AA - matrix to be filled
    // z - local z-positions of planes(layers)
    // sigm2 - square of sigma
    // h_ - array to include/exclude planes (h_[i] = 0 or 1)

  //    Float_t z2_[nPlanes] = {z[0] * z[0], z[1] * z[1], z[2] * z[2], z[3] * z[3], z[4] * z[4], z[5] * z[5]}; //cm
  z2_[0] = z[0]*z[0];
  z2_[1] = z[1]*z[1];
  z2_[2] = z[2]*z[2];
  z2_[3] = z[3]*z[3];
  z2_[4] = z[4]*z[4];
  z2_[5] = z[5]*z[5];


    AA[0][0] += 2 * z2_[0] * h_[0] / sigm2_[0] + z2_[2] * h_[2] / (2 * sigm2_[2]) + z2_[1] * h_[1] / (2 * sigm2_[1]) + 2 * z2_[3] * h_[3] / sigm2_[3] + z2_[5] * h_[5] / (2 * sigm2_[5]) + z2_[4] * h_[4] / (2 * sigm2_[4]); //Ax
    AA[0][1] += 2 * z[0] * h_[0] / sigm2_[0] + z[2] * h_[2] / (2 * sigm2_[2]) + z[1] * h_[1] / (2 * sigm2_[1]) + 2 * z[3] * h_[3] / (sigm2_[3]) + z[5] * h_[5] / (2 * sigm2_[5]) + z[4] * h_[4] / (2 * sigm2_[4]); //Bx
    AA[0][2] += sq3 * (z2_[2] * h_[2] / (2 * sigm2_[2]) - z2_[1] * h_[1] / (2 * sigm2_[1]) + z2_[5] * h_[5] / (2 * sigm2_[5]) - z2_[4] * h_[4] / (2 * sigm2_[4])); //Ay
    AA[0][3] += sq3 * (z[2] * h_[2] / (2 * sigm2_[2]) - z[1] * h_[1] / (2 * sigm2_[1]) + z[5] * h_[5] / (2 * sigm2_[5]) - z[4] * h_[4] / (2 * sigm2_[4])); //By
    AA[1][0] = A[0][1];
    AA[1][1] += 2 * h_[0] / sigm2_[0] + 0.5 * h_[2] / sigm2_[2] + 0.5 * h_[1] / sigm2_[1] + 2 * h_[3] / sigm2_[3] + 0.5 * h_[5] / sigm2_[5] + 0.5 * h_[4] / sigm2_[4];
    AA[1][2] += sq3 * (z[2] * h_[2] / sigm2_[2] - z[1] * h_[1] / sigm2_[1] + z[5] * h_[5] / sigm2_[5] - z[4] * h_[4] / sigm2_[4]) * 0.5;
    AA[1][3] += sq3 * (h_[2] / sigm2_[2] - h_[1] / sigm2_[1] + h_[5] / sigm2_[5] - h_[4] / sigm2_[4]) * 0.5;
    AA[2][0] = A[0][2];
    AA[2][1] = A[1][2];
    AA[2][2] += 3.0 * (z2_[2] * h_[2] / sigm2_[2] + z2_[1] * h_[1] / sigm2_[1] + z2_[5] * h_[5] / sigm2_[5] + z2_[4] * h_[4] / sigm2_[4]) * 0.5;
    AA[2][3] += 3.0 * (z[2] * h_[2] / sigm2_[2] + z[1] * h_[1] / sigm2_[1] + z[5] * h_[5] / sigm2_[5] + z[4] * h_[4] / sigm2_[4]) * 0.5;
    AA[3][0] = A[0][3];
    AA[3][1] = A[1][3];
    AA[3][2] = A[2][3];
    AA[3][3] += 3.0 * (0.5 * h_[2] / sigm2_[2] + 0.5 * h_[1] / sigm2_[1] + 0.5 * h_[5] / sigm2_[5] + 0.5 * h_[4] / sigm2_[4]);
}

void BmnMwpcHitFinder::FillFreeCoefVector(Double_t* F, Float_t* XVU_Ch, Float_t* z, Float_t* sigm2_, Int_t* h_, Int_t nPlanes) {
    // F - vector to be filled
    // XVU_Ch - coordinates of segment in chamber (Is it correct definition?)
    // segIdx - index of current segment
    // z - local z-positions of planes(layers)
    // sigm2_ - square of sigma
    // h_ - array to include/exclude planes (h_[i] = 0 or 1)

    F[0] += 2 * XVU_Ch[0] * z[0] * h_[0] / sigm2_[0] + XVU_Ch[1] * z[1] * h_[1] / sigm2_[1] + XVU_Ch[2] * z[2] * h_[2] / sigm2_[2] + 2 * XVU_Ch[3] * z[3] * h_[3] / sigm2_[3] + XVU_Ch[4] * z[4] * h_[4] / sigm2_[4] + XVU_Ch[5] * z[5] * h_[5] / sigm2_[5];
    F[1] += 2 * XVU_Ch[0] * h_[0] / sigm2_[0] + XVU_Ch[1] * h_[1] / sigm2_[1] + XVU_Ch[2] * h_[2] / sigm2_[2] + 2 * XVU_Ch[3] * h_[3] / sigm2_[3] + XVU_Ch[4] * h_[4] / sigm2_[4] + XVU_Ch[5] * h_[5] / sigm2_[5];
    F[2] += sq3 * (-XVU_Ch[1] * z[1] * h_[1] / sigm2_[1] + XVU_Ch[2] * z[2] * h_[2] / sigm2_[2] - XVU_Ch[4] * z[4] * h_[4] / sigm2_[4] + XVU_Ch[5] * z[5] * h_[5] / sigm2_[5]);
    F[3] += sq3 * (-XVU_Ch[1] * h_[1] / sigm2_[1] + XVU_Ch[2] * h_[2] / sigm2_[2] - XVU_Ch[4] * h_[4] / sigm2_[4] + XVU_Ch[5] * h_[5] / sigm2_[5]);
}

void BmnMwpcHitFinder::InverseMatrix(Double_t** AA, Double_t** b_) {
    /**** Gaussian algorithm for 4x4 matrix inversion ****/


    Double_t factor;
    Double_t temp[4];

    // Set b to I
    for (Int_t i1 = 0; i1 < 4; i1++)
        for (Int_t j1 = 0; j1 < 4; j1++)
            if (i1 == j1) b_[i1][j1] = 1.0;
            else b_[i1][j1] = 0.0;

    for (Int_t i1 = 0; i1 < 4; i1++) {
        for (Int_t j1 = i1 + 1; j1 < 4; j1++) {
            if (fabs(AA[i1][i1]) < fabs(AA[j1][i1])) {
                for (Int_t l1 = 0; l1 < 4; l1++) temp[l1] = AA[i1][l1];
                for (Int_t l1 = 0; l1 < 4; l1++) AA[i1][l1] = AA[j1][l1];
                for (Int_t l1 = 0; l1 < 4; l1++) AA[j1][l1] = temp[l1];
                for (Int_t l1 = 0; l1 < 4; l1++) temp[l1] = b_[i1][l1];
                for (Int_t l1 = 0; l1 < 4; l1++) b_[i1][l1] = b_[j1][l1];
                for (Int_t l1 = 0; l1 < 4; l1++) b_[j1][l1] = temp[l1];
            }
        }
        factor = AA[i1][i1];
        for (Int_t j1 = 4 - 1; j1>-1; j1--) {
            b_[i1][j1] /= factor;
            AA[i1][j1] /= factor;
        }
        for (Int_t j1 = i1 + 1; j1 < 4; j1++) {
            factor = -AA[j1][i1];
            for (Int_t k1 = 0; k1 < 4; k1++) {
                AA[j1][k1] += AA[i1][k1] * factor;
                b_[j1][k1] += b_[i1][k1] * factor;
            }
        }
    } // i1
    for (Int_t i1 = 3; i1 > 0; i1--) {
        for (Int_t j1 = i1 - 1; j1>-1; j1--) {
            factor = -AA[j1][i1];
            for (Int_t k1 = 0; k1 < 4; k1++) {
                AA[j1][k1] += AA[i1][k1] * factor;
                b_[j1][k1] += b_[i1][k1] * factor;
            }
        }
    } // i1
    //end inverse
}

void BmnMwpcHitFinder::Finish() {
    delete fMwpcGeometry;

    // delete 1d arrays:
    delete [] kPln;
    delete [] kZ1_loc;
    delete [] kZ2_loc;
    delete [] iw;
    delete [] iw_Ch1;
    delete [] iw_Ch2;
    delete [] ind_best_Ch1;
    delete [] ind_best_Ch2;
    delete [] Nhits_Ch1;
    delete [] Nhits_Ch2;
    delete [] Chi2_ndf_Ch1;
    delete [] Chi2_ndf_Ch2;
    delete [] Chi2_ndf_best_Ch1;
    delete [] Chi2_ndf_best_Ch2;
    delete [] sigm2;
    delete [] h;
    delete [] h6;
    delete [] ipl;
    delete [] XVU;
    delete [] XVU_cl;
    delete [] dX_i;
    delete [] z2;

    // delete 2d arrays:
    for(Int_t iWire=0; iWire< kNWires; iWire++){
      delete [] wire_Ch1[iWire];
      delete [] wire_Ch2[iWire];
      delete [] xuv_Ch1[iWire];
      delete [] xuv_Ch2[iWire];
    }

    for(Int_t ii=0; ii<4; ii++){
      delete [] par_ab_Ch1[ii];
      delete [] par_ab_Ch2[ii];
      delete [] A[ii];
      delete [] b[ii];
    }

    for(Int_t iPl=0; iPl<kNPlanes; iPl++){
      delete [] Wires_Ch1[iPl];
      delete [] Wires_Ch2[iPl];
      delete [] clust_Ch1[iPl];
      delete [] clust_Ch2[iPl];
      delete [] XVU_Ch1[iPl];
      delete [] XVU_Ch2[iPl];
    }
    
    cout << "Work time of the MWPC hit finder: " << workTime << " s" << endl;
}

ClassImp(BmnMwpcHitFinder)


