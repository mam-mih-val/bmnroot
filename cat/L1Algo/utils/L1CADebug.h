#ifndef L1CADebug_h
#define L1CADebug_h 1

#define DEBUG_CA

#include "CbmL1.h"

TH1F  
  *h_pick_res_x[20][3][3], *h_pick_res_y[20][3][3],
  *h_pick_pull_x[20][3][3], *h_pick_pull_y[20][3][3],
  *h_nduplets[20][3], 
  *h_res_x[20][3][3], *h_res_y[20][3][3],
  *h_res_tx[20][3][3],*h_res_ty[20][3][3], *h_res_qp[20][3][3],
  *h_pull_x[20][3][3], *h_pull_y[20][3][3], 
  *h_pull_tx[20][3][3], *h_pull_ty[20][3][3], *h_pull_qp[20][3][3]
  ;
//TH2F *h_dyvsy[20], *h_dxvsx[20];

void Pulls( int i, int j, int k, 
	    double *mc, L1TrackPar &T, fvec qp0, L1FieldRegion &fld )
{
  L1TrackPar tmp_T = T;
  fvec z = mc[5];
  L1Extrapolate( tmp_T, z, qp0, fld );
  double dx = mc[0] - tmp_T.x[0];
  double dy = mc[1] - tmp_T.y[0];
  double dtx = mc[2] - tmp_T.tx[0];
  double dty = mc[3] - tmp_T.ty[0];
  double dqp = mc[4] - tmp_T.qp[0];
  double sx = sqrt(tmp_T.C00[0]);
  double sy = sqrt(tmp_T.C11[0]);
  double stx = sqrt(tmp_T.C22[0]);
  double sty = sqrt(tmp_T.C33[0]);
  double sqp = sqrt(tmp_T.C44[0]);
  h_res_x  [i][j][k]->Fill(dx);
  h_res_y  [i][j][k]->Fill(dy);	      
  h_res_tx [i][j][k]->Fill(dtx);
  h_res_ty [i][j][k]->Fill(dty);	      
  h_res_qp [i][j][k]->Fill(dqp/mc[4]);	      
  h_pull_x [i][j][k]->Fill(dx/sx);
  h_pull_y [i][j][k]->Fill(dy/sy);	      
  h_pull_tx[i][j][k]->Fill(dtx/stx);
  h_pull_ty[i][j][k]->Fill(dty/sty);	      
  h_pull_qp[i][j][k]->Fill(dqp/sqp);	      
}

void InitL1Histo(){
  static bool first_call = 1;
  if( first_call ){
    TDirectory *curdir = gDirectory;
    CbmL1::Instance()->histodir->cd();
    L1Algo *algo = CbmL1::Instance()->algo;
    gDirectory->mkdir("L1ALGO");
    gDirectory->cd("L1ALGO");
    for( int ist=0; ist<algo->NStations; ist++ ){      
      char stname[225];//, namedir[225], title[225];
      sprintf(stname,"%i",ist);
      gDirectory->mkdir(stname);  
      gDirectory->cd(stname);
      {
	//h_dyvsy[ist] = new TH2F("h_dyvsy","h_dyvsy",1000,0,100, 1000, 0.,.5);
	//h_dxvsx[ist] = new TH2F("h_dxvsx","h_dxvsx",1000,0,100, 1000, 0.,2.5);
	const char* stepname[3]={"D0","Ref","Sec"};
	for( int istep=0; istep<3; istep++){
	  gDirectory->mkdir(stepname[istep]);  
	  gDirectory->cd(stepname[istep]);

	  h_nduplets[ist][istep] = new TH1F("h_nduplets","h_nduplets",50,0,5000);

	  const char* lmr[3]={"left","middle","right"};
	  for( int ilmr=0; ilmr<3; ilmr++){
	    gDirectory->mkdir(lmr[ilmr]);  
	    gDirectory->cd(lmr[ilmr]);
	    {
	      h_pick_res_x[ist][ilmr][istep] = new TH1F("h_pick_res_x","h_pick_res_x",100,-2,3);
	      h_pick_res_y[ist][ilmr][istep] = new TH1F("h_pick_res_y","h_pick_res_y",100,-1,1);
	      h_pick_pull_x[ist][ilmr][istep] = new TH1F("h_pick_pull_x","h_pick_pull_x",100,-10,10);
	      h_pick_pull_y[ist][ilmr][istep] = new TH1F("h_pick_pull_y","h_pick_pull_y",100,-10,10);
	      h_res_x[ist][ilmr][istep] = new TH1F("h_res_x","h_res_x",100,-.01,.01);
	      h_res_y[ist][ilmr][istep] = new TH1F("h_res_y","h_res_y",100,-.1,.1);
	      h_res_tx[ist][ilmr][istep] = new TH1F("h_res_tx","h_res_tx",100,-.01,.01);
	      h_res_ty[ist][ilmr][istep] = new TH1F("h_res_ty","h_res_ty",100,-.01,.01);
	      h_res_qp[ist][ilmr][istep] = new TH1F("h_res_qp","h_res_qp",100,-.1,.1);
	      h_pull_x[ist][ilmr][istep] = new TH1F("h_pull_x","h_pull_x",100,-10,10);
	      h_pull_y[ist][ilmr][istep] = new TH1F("h_pull_y","h_pull_y",100,-10,10);
	      h_pull_tx[ist][ilmr][istep] = new TH1F("h_pull_tx","h_pull_tx",100,-10,10);
	      h_pull_ty[ist][ilmr][istep] = new TH1F("h_pull_ty","h_pull_ty",100,-10,10);
	      h_pull_qp[ist][ilmr][istep] = new TH1F("h_pull_qp","h_pull_qp",100,-10,10);
	      gDirectory->cd("..");
	    }
	  } gDirectory->cd("..");
	}
      } gDirectory->cd("..");
    } 
    gDirectory->cd("..");    
    curdir->cd();
    first_call = 0;
  }
}


#endif
