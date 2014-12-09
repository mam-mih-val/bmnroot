/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *=====================================================
 *
 *  Fit reconstructed tracks and find primary vertex
 *
 */

#include "CbmL1.h"

#include "L1Algo/L1Algo.h"
#include "L1Algo/L1TrackPar.h"
#include "L1Algo/L1Extrapolation.h"
#include "L1Algo/L1Filtration.h"
#include "L1Algo/L1AddMaterial.h"
#include "L1Algo/L1StsHit.h"

#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmKFPrimaryVertexFinder.h"

#include "TStopwatch.h"

void CbmL1::TrackFitter( vector<CbmL1Track> &Tracks, CbmL1Vtx *V )
{
  TStopwatch timer;
  timer.Start();
  /*
  //int nt=0;
  for( vector<CbmL1Track>::iterator i = Tracks.begin(); i!=Tracks.end(); ++i)
    {
      CbmL1Track &T = *i;
      
      int stmin = 1000, stmax = -1000;
      for ( vector<L1StsHit*>::iterator j = T.StsHits.begin(); j != T.StsHits.end(); ++j)
	{      
	  if ((*j)->iStation < stmin ) stmin = (*j)->iStation;
	  if ((*j)->iStation > stmax ) stmax = (*j)->iStation;
	}
      
      T.nStations = stmax - stmin + 1;
      T.is_long = ( T.StsHits.size() >= 4 ) && ( T.nStations >=4 );

      //T.is_long = T.is_long &&( vMCPoints[T.MapsHits.back()->MC_Point].mother_ID==-1 );
      //if ( !T.is_long ) continue;
      
      static L1FieldRegion fld[20];
      {
	vector<L1StsHit*>::iterator ih2 = T.StsHits.begin(), ih1, ih0;
	ih1 = ih2;
	ih1++;
	ih0 = ih1; 
	ih0++;
	int ist0 = (*ih0)->iStation;
	int ist1 = (*ih1)->iStation;
	int ist2 = (*ih2)->iStation;
	L1Station *st0 = &(algo->vStations[ist0]);
	L1Station *st1 = &(algo->vStations[ist1]);
	L1Station *st2 = &(algo->vStations[ist2]);
	L1FieldValue B0, B1, B2;
	fvec Z0= st0->z, Z1= st1->z, Z2= st2->z ; 
	st0->fieldSlice.GetFieldValue( (*ih0)->x, (*ih0)->y, B0 );
	st1->fieldSlice.GetFieldValue( (*ih1)->x, (*ih1)->y, B1 );
	st2->fieldSlice.GetFieldValue( (*ih2)->x, (*ih2)->y, B2 );
	fld[ist0].Set( B0, Z0, B1, Z1, B2, Z2 );
	fld[ist1] = fld[ist0];
	fld[ist2] = fld[ist0];
	if( ih0!=T.StsHits.end() ){
	  while(1){ 
	    ih2 = ih1; B2 = B1; Z2 = Z1; st2 = st1;
	    ih1 = ih0; B1 = B0; Z1 = Z0; st1 = st0;
	    ih0++;
	    if( ih0==T.StsHits.end() ) break;
	    ist0 = (*ih0)->iStation;
	    st0 = &(algo->vStations[ist0]);
	    Z0 = st0->z;
	    st0->fieldSlice.GetFieldValue( (*ih0)->x, (*ih0)->y, B0 );
	    fld[ist0].Set( B0, Z0, B1, Z1, B2, Z2 );
	  }
	}
      }
      fvec vINF = 100.;
      L1TrackPar tp;
      for( int i=0; i<5; i++ ) *((&tp.x)+i) = T.T[i];
      {
	fvec qp0  = tp.qp;
	tp.NDF  = 2;
	tp.chi2 = 0;
	vector<L1StsHit*>::iterator ih = T.StsHits.begin();
	int ist = (*ih)->iStation;
	L1Station *st = &(algo->vStations[ist]);	
	tp.x  = (*ih)->x;
	tp.y  = (*ih)->y;
	tp.z  = st->z;
	tp.C00 = st->XYInfo.C00;
	tp.C10 = st->XYInfo.C10;
	tp.C11 = st->XYInfo.C11;
	for( int i=3; i<15; i++ ) *((&tp.C00)+i) = 0;
	tp.C22 = tp.C33 = tp.C44 = vINF;
	for( ; ih!= T.StsHits.end(); ih++ ){
	  ist = (*ih)->iStation;
	  st = &(algo->vStations[ist]);
	  L1Extrapolate( tp, st->z, qp0, fld[ist] );
	  fvec u_f = (*ih)->u_front, u_b = (*ih)->u_back;
	  L1Filter( tp, st->frontInfo, u_f  );
	  L1Filter( tp, st->backInfo, u_b  );
	  L1AddMaterial( tp, st->materialInfo, qp0 );
	}
      }	
      for( int i=0; i<6;  i++ ) T.TLast[i] = (*((&tp.x)+i))[0] ;
      for( int i=0; i<15; i++ ) T.CLast[i] = (*((&tp.C00)+i))[0];

      {
	fvec qp0  = tp.qp;
	tp.NDF  = 2;
	tp.chi2 = 0;
	vector<L1StsHit*>::reverse_iterator ih = T.StsHits.rbegin();
	int ist = (*ih)->iStation;
	L1Station *st = &(algo->vStations[ist]);	
	tp.x  = (*ih)->x;
	tp.y  = (*ih)->y;
	tp.z  = st->z;
	tp.C00 = st->XYInfo.C00;
	tp.C10 = st->XYInfo.C10;
	tp.C11 = st->XYInfo.C11;
	for( int i=3; i<15; i++ ) *((&tp.C00)+i) = 0;
	tp.C22 = tp.C33 = tp.C44 = vINF;
	for( ; ih!= T.StsHits.rend(); ih++ ){
	  ist = (*ih)->iStation;
	  st = &(algo->vStations[ist]);
	  L1Extrapolate( tp, st->z, qp0, fld[ist] );
	  fvec u_f = (*ih)->u_front, u_b = (*ih)->u_back;
	  L1Filter( tp, st->frontInfo, u_f  );
	  L1Filter( tp, st->backInfo, u_b  );
	  L1AddMaterial( tp, st->materialInfo, qp0 );
	}
      }	
      for( int i=0; i<6;  i++ ) T.T[i] = (*((&tp.x)+i))[0] ;
      for( int i=0; i<15; i++ ) T.C[i] = (*((&tp.C00)+i))[0];
      T.chi2 = tp.chi2[0];
      T.NDF = tp.NDF[0]-5;	  
    }
  timer.Stop();

  stat_vtx_ntracks = 0; 

  if(V){ // primary vertex fit  
    
    vector<CbmKFTrackInterface*> vTracks;
    
   { for( vector<CbmL1Track>::iterator i = Tracks.begin(); i!=Tracks.end(); ++i)
      {
	if( ( i->StsHits.size() < 5 ) || ( i->nStations <5 ) ) continue;
	vTracks.push_back( &*i );    
	stat_vtx_ntracks ++;
      }
    }  
   cout<<"Fit vertex"<<endl;
    if ( vTracks.size()>=3 )
      {
	CbmKFPrimaryVertexFinder PVFinder;
	PVFinder.SetTracks( vTracks );
	PVFinder.Fit(*V);
      }
  }// prim. vtx 
  */
  //c_time.Stop();    
  //stat_fit_time += double(c_time.Time());
  //timer.Stop();
  stat_fit_time += timer.CpuTime();
  stat_fit_time_ntracks += Tracks.size();
}
