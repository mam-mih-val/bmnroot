//3456789012345678901234567890123456789012345678901234567890123456789012
/** Implementation of the CbmKFParticle class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   06.02.06
 *
 */

#include "CbmKFParticle.h"
#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmKFTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "TMath.h"
//#include "TDatabasePDG.h"

#include "CbmKFParticleDatabase.h"

#include <cmath>
#include <vector>

using namespace std;

ClassImp(CbmKFParticle)

CbmKFParticle::CbmKFParticle( CbmKFTrackInterface* Track, Double_t *z0, Int_t  *qHypo, Int_t *pdg):
  fId(-1),
  fDaughtersIds(),
  fPDG(-1),
  NDF(0),
  Chi2(0), Q(0),
  AtProductionVertex(0)
{

  fDaughtersIds.push_back( Track->Id() );

  CbmKFTrack Tr( *Track );
  
  Double_t *m = Tr.GetTrack();
  Double_t *V = Tr.GetCovMatrix();

  //* Fit of vertex track slopes and momentum (a,b,qp) to xyz0 vertex

//  if( z0 ) Tr.Extrapolate( *z0 );

  Double_t a = m[2], b = m[3], qp = m[4];
  
  //* convert the track to (x,y,px,py,pz,e,t/p) parameterization

  double Mass = Tr.GetMass();
  if(pdg)
  {
    Mass=CbmKFParticleDatabase::Instance()->GetMass(*pdg);
//    Mass=TDatabasePDG::Instance()->GetParticle(*pdg)->Mass();
  }


  double c2 = 1./(1. + a*a + b*b);
  double pq = 1./qp;
  if(qHypo) pq *= *qHypo;
  double p2 = pq*pq;
  double pz = sqrt(p2*c2);
  double px = a*pz;
  double py = b*pz;
  double E = sqrt( Mass*Mass + p2 );
    
  double H[3] = { -px*c2, -py*c2, -pz*pq };
  double HE = -pq*p2/E;
	  
  r[0] = m[0];
  r[1] = m[1];
  r[2] = m[5];
  r[3] = px;
  r[4] = py;
  r[5] = pz;
  r[6] = E;
  r[7] = 0;

  double cxpz = H[0]*V[ 3] + H[1]*V[ 6] + H[2]*V[10];
  double cypz = H[0]*V[ 4] + H[1]*V[ 7] + H[2]*V[11];
  double capz = H[0]*V[ 5] + H[1]*V[ 8] + H[2]*V[12];
  double cbpz = H[0]*V[ 8] + H[1]*V[ 9] + H[2]*V[13];
  double cqpz = H[0]*V[12] + H[1]*V[13] + H[2]*V[14];
  double cpzpz = H[0]*H[0]*V[5] +H[1]*H[1]*V[9] + H[2]*H[2]*V[14] 
    + 2*( H[0]*H[1]*V[8] +H[0]*H[2]*V[12] +H[1]*H[2]*V[13]);

  C[ 0] = V[0];
  C[ 1] = V[1];
  C[ 2] = V[2];
  C[ 3] = 0;
  C[ 4] = 0;
  C[ 5] = 0;
  C[ 6] = V[3]*pz + a*cxpz; 
  C[ 7] = V[4]*pz + a*cypz; 
  C[ 8] = 0;
  C[ 9] = V[5]*pz*pz + 2*a*pz*capz + a*a*cpzpz;
  C[10] = V[6]*pz+b*cxpz; 
  C[11] = V[7]*pz+b*cypz; 
  C[12] = 0;
  C[13] = V[8]*pz*pz + a*pz*cbpz + b*pz*capz + a*b*cpzpz;
  C[14] = V[9]*pz*pz + 2*b*pz*cbpz + b*b*cpzpz;
  C[15] = cxpz; 
  C[16] = cypz; 
  C[17] = 0;
  C[18] = capz*pz + a*cpzpz;
  C[19] = cbpz*pz + b*cpzpz;
  C[20] = cpzpz;
  C[21] = HE*V[10];
  C[22] = HE*V[11];
  C[23] = 0;
  C[24] = HE*( V[12]*pz + a*cqpz );
  C[25] = HE*( V[13]*pz + b*cqpz );
  C[26] = HE*cqpz;
  C[27] = HE*HE*V[14];	    
  C[28] = C[29] = C[30] = C[31] = C[32] = C[33] = C[34] = 0;
  C[35] = 1.;

  NDF = Tr.GetRefNDF();
  Chi2 = Tr.GetRefChi2();
  Q = (qp>0.) ?1 :( (qp<0) ?-1 :0);
  if(qHypo) Q *= *qHypo;
  AtProductionVertex = 1;
}

void drawcov(double C[] ){
  double s0= sqrt(C[0]);
  double s1= sqrt(C[2]);
  double s2= sqrt(C[5]);
  double s3= sqrt(C[9]);
  double s4= sqrt(C[14]);
  cout<<s0<<" ";
  cout<<C[1]/s0/s1<<" "<<s1<<" ";
  cout<<C[3]/s0/s2<<" "<<C[4]/s1/s2<<" "<<s2<<" ";
  cout<<C[6]/s0/s3<<" "<<C[7]/s1/s3<<" "<<C[8]/s2/s3<<" "<<s3<<" ";
  cout<<C[10]/s0/s4<<" "<<C[11]/s1/s4<<" "<<C[12]/s2/s4<<" "<<C[13]/s3/s4<<" "<<s4<<endl;
}

void CbmKFParticle::Construct( vector<CbmKFTrackInterface*> &vDaughters,
			       CbmKFVertexInterface *Parent, 
			       Double_t Mass, Double_t CutChi2 )
{
  const Int_t MaxIter=3;

  if( CbmKF::Instance()->vTargets.empty() ){
    r[0] = r[1] = r[2] = 0.;
    C[0] = C[2] = C[5] = 1.;
    C[1] = C[3] = C[4] = 0;
  }else{
    CbmKFTube &t = CbmKF::Instance()->vTargets[0];
    r[0] = r[1] = 0.;
    r[2] = t.z;
    C[0] = C[2] = t.R*t.R/9.;
    C[5] = t.dz/6.;
    C[1] = C[3] = C[4] = 0;
  }
  
  r[3] = 0;
  r[4] = 0;
  r[5] = 0;
  r[6] = 0;
  r[7] = 0;
  
  for ( Int_t iteration =0; iteration<MaxIter;++iteration ){
    
    Double_t r0[8], C0[6];

    for( int i=0; i<8; i++) r0[i] = r[i];
    for( int i=0; i<6; i++) C0[i] = C[i];

    //simple approximation for the vertex position. TODO find better solution
    if(iteration == 0)
    {
      Double_t VertexGuess[3];

      Double_t fTDaughter[(int) vDaughters.size()][6];
      Double_t fCDaughter[(int) vDaughters.size()][15];

      vector<CbmKFTrack*> TrV;

      Int_t nvect = 0;
      for( vector<CbmKFTrackInterface*>::iterator tr=vDaughters.begin(); tr!=vDaughters.end(); ++tr )
      {
        CbmKFTrack *Tr = new CbmKFTrack(**tr);
        fTDaughter[nvect][0] = Tr->GetTrack()[0];
        fTDaughter[nvect][1] = Tr->GetTrack()[1];
        fTDaughter[nvect][2] = Tr->GetTrack()[2];
        fTDaughter[nvect][3] = Tr->GetTrack()[3];
        fTDaughter[nvect][4] = Tr->GetTrack()[4];
        fTDaughter[nvect][5] = Tr->GetTrack()[5];
        
        fCDaughter[nvect][0] = Tr->GetCovMatrix()[0];
        fCDaughter[nvect][2] = Tr->GetCovMatrix()[2];
        fCDaughter[nvect][5] = Tr->GetCovMatrix()[5];
        fCDaughter[nvect][9] = Tr->GetCovMatrix()[9];
        fCDaughter[nvect][14] = Tr->GetCovMatrix()[14];
        
        delete Tr;
        nvect++;
      }

      Double_t z = (fTDaughter[0][3]*fTDaughter[0][5] - fTDaughter[1][3]*fTDaughter[1][5] + fTDaughter[1][1] - fTDaughter[0][1])/(fTDaughter[0][3] - fTDaughter[1][3]);

      CbmKFTrack *Tr = new CbmKFTrack(*vDaughters[1]);
      Tr->Extrapolate( z );

      VertexGuess[0] = Tr->GetTrack()[0];
      VertexGuess[1] = Tr->GetTrack()[1];
      VertexGuess[2] = Tr->GetTrack()[5];

      Double_t dr0[2];
      dr0[0] = sqrt(Tr->GetCovMatrix()[0]);
      dr0[1] = sqrt(Tr->GetCovMatrix()[2]);
      delete Tr;
      r0[0] = VertexGuess[0];
      r0[1] = VertexGuess[1];
      r0[2] = VertexGuess[2];
    }

    r[3] = 0;
    r[4] = 0;
    r[5] = 0;
    r[6] = 0;
   
    for(Int_t i=0;i<36;++i) C[i]=0.;

    C[0] = C[2] = C[5] = 100.;
    C[35] = 1.;

    Double_t B[3]; 
    {
      FairField *MF = CbmKF::Instance()->GetMagneticField();
      MF->GetFieldValue( r0, B );
      const Double_t c_light =  0.000299792458;
      B[0]*=c_light; B[1]*=c_light; B[2]*=c_light;
    }

    NDF  = -3;
    Chi2 =  0.;
    Q = 0;
    bool first = 1;
    for( vector<CbmKFTrackInterface*>::iterator tr=vDaughters.begin(); tr!=vDaughters.end(); ++tr )
    {
      CbmKFParticle Daughter( *tr, &(r0[2]) );

      Daughter.Extrapolate(Daughter.r , Daughter.GetDStoPoint(r0));

      Double_t *m = Daughter.r;
      Double_t *Cd = Daughter.C;

      Double_t d[3] = { r0[0]-m[0], r0[1]-m[1], r0[2]-m[2] };
      Double_t SigmaS = .1+10.*sqrt( (d[0]*d[0]+d[1]*d[1]+d[2]*d[2])/(m[3]*m[3]+m[4]*m[4]+m[5]*m[5])  );

      Double_t h[6];

      h[0] = m[3]*SigmaS;
      h[1] = m[4]*SigmaS;
      h[2] = m[5]*SigmaS; 
      h[3] = ( h[1]*B[2]-h[2]*B[1] )*Daughter.Q;
      h[4] = ( h[2]*B[0]-h[0]*B[2] )*Daughter.Q;
      h[5] = ( h[0]*B[1]-h[1]*B[0] )*Daughter.Q;

	//* Fit of daughter momentum (x,y,z) to r0[0,1,2] vertex
	{
	  Double_t zeta[3] = { r0[0]-m[0], r0[1]-m[1], r0[2]-m[2] };

	  Double_t Vv[6] = 
	    { Cd[ 0] + h[0]*h[0], 
	      Cd[ 1] + h[1]*h[0], Cd[ 2] + h[1]*h[1],			     
	      Cd[ 3] + h[2]*h[0], Cd[ 4] + h[2]*h[1], Cd[ 5] + h[2]*h[2] };
	  
	  Double_t Vvp[9]=
	    { Cd[ 6] + h[0]*h[3], Cd[ 7] + h[1]*h[3], Cd[ 8] + h[2]*h[3],
	      Cd[10] + h[0]*h[4], Cd[11] + h[1]*h[4], Cd[12] + h[2]*h[4],
	      Cd[15] + h[0]*h[5], Cd[16] + h[1]*h[5], Cd[17] + h[2]*h[5] };

	  if( CutChi2 >0. ){

	    Double_t Si[6]={ Vv[0]+C0[0],
			     Vv[1]+C0[1], Vv[2]+C0[2],
			     Vv[3]+C0[3], Vv[4]+C0[4], Vv[5]+C0[5] };
	    Double_t S[6] = { Si[2]*Si[5] - Si[4]*Si[4],
			      Si[3]*Si[4] - Si[1]*Si[5],
			      Si[0]*Si[5] - Si[3]*Si[3],
			      Si[1]*Si[4] - Si[2]*Si[3],
			      Si[1]*Si[3] - Si[0]*Si[4],
			      Si[0]*Si[2] - Si[1]*Si[1] };		 	
	    Double_t det = ( Si[0]*S[0] + Si[1]*S[1] + Si[3]*S[3] );
	    Double_t chi2 = 
	      ( +(S[0]*zeta[0] + S[1]*zeta[1] + S[3]*zeta[2])*zeta[0]
		+(S[1]*zeta[0] + S[2]*zeta[1] + S[4]*zeta[2])*zeta[1]
		+(S[3]*zeta[0] + S[4]*zeta[1] + S[5]*zeta[2])*zeta[2]);
	    if( chi2 > 2*CutChi2*det ) continue;
	  }

	  double S[6] = 
	    { Vv[2]*Vv[5] - Vv[4]*Vv[4],
	      Vv[3]*Vv[4] - Vv[1]*Vv[5], Vv[0]*Vv[5] - Vv[3]*Vv[3],
	      Vv[1]*Vv[4] - Vv[2]*Vv[3], Vv[1]*Vv[3] - Vv[0]*Vv[4], 
	      Vv[0]*Vv[2] - Vv[1]*Vv[1] };		 
	
	  double s = ( Vv[0]*S[0] + Vv[1]*S[1] + Vv[3]*S[3] );
	  s = ( s > 1.E-20 )  ?1./s :0;

	  S[0]*=s; S[1]*=s; S[2]*=s; S[3]*=s; S[4]*=s; S[5]*=s;

	  Double_t Sz[3] = { (S[0]*zeta[0]+S[1]*zeta[1]+S[3]*zeta[2]),
			     (S[1]*zeta[0]+S[2]*zeta[1]+S[4]*zeta[2]),
			     (S[3]*zeta[0]+S[4]*zeta[1]+S[5]*zeta[2]) };
	  
	  Double_t x = m[3] + Vvp[0]*Sz[0] + Vvp[1]*Sz[1] + Vvp[2]*Sz[2];	  
	  Double_t y = m[4] + Vvp[3]*Sz[0] + Vvp[4]*Sz[1] + Vvp[5]*Sz[2];	  
	  Double_t z = m[5] + Vvp[6]*Sz[0] + Vvp[7]*Sz[1] + Vvp[8]*Sz[2];	  

	  h[0] = x*SigmaS;
	  h[1] = y*SigmaS;
	  h[2] = z*SigmaS; 
	  h[3] = ( h[1]*B[2]-h[2]*B[1] )*Daughter.Q;
	  h[4] = ( h[2]*B[0]-h[0]*B[2] )*Daughter.Q;
	  h[5] = ( h[0]*B[1]-h[1]*B[0] )*Daughter.Q;
	}
	
	Double_t V[28];

	V[ 0] = Cd[ 0] + h[0]*h[0];
	V[ 1] = Cd[ 1] + h[1]*h[0];
	V[ 2] = Cd[ 2] + h[1]*h[1];
	V[ 3] = Cd[ 3] + h[2]*h[0];
	V[ 4] = Cd[ 4] + h[2]*h[1];
	V[ 5] = Cd[ 5] + h[2]*h[2];
	
	V[ 6] = Cd[ 6] + h[3]*h[0];
	V[ 7] = Cd[ 7] + h[3]*h[1];
	V[ 8] = Cd[ 8] + h[3]*h[2];
	V[ 9] = Cd[ 9] + h[3]*h[3];

	V[10] = Cd[10] + h[4]*h[0];
	V[11] = Cd[11] + h[4]*h[1];
	V[12] = Cd[12] + h[4]*h[2];
	V[13] = Cd[13] + h[4]*h[3];
	V[14] = Cd[14] + h[4]*h[4];

	V[15] = Cd[15] + h[5]*h[0];
	V[16] = Cd[16] + h[5]*h[1];
	V[17] = Cd[17] + h[5]*h[2];
	V[18] = Cd[18] + h[5]*h[3];
	V[19] = Cd[19] + h[5]*h[4];
	V[20] = Cd[20] + h[5]*h[5];	  
      
	V[21] = Cd[21];
	V[22] = Cd[22];
	V[23] = Cd[23];
	V[24] = Cd[24];
	V[25] = Cd[25];
	V[26] = Cd[26];
	V[27] = Cd[27];

	//* 
	
	NDF  += 2;
	Q    +=  Daughter.Q;

	if( 0&&first ){
	  first = 0;
	  for( int i=0; i<7; i++) r[i] = m[i];
	  for( int i=0; i<28; i++) C[i] = V[i];
	  continue;
	}

	//*

	double S[6];
	{
	  double Si[6] = { C[0]+V[0], 
			   C[1]+V[1], C[2]+V[2], 
			   C[3]+V[3], C[4]+V[4], C[5]+V[5] };
	  
	  S[0] = Si[2]*Si[5] - Si[4]*Si[4];
	  S[1] = Si[3]*Si[4] - Si[1]*Si[5];
	  S[2] = Si[0]*Si[5] - Si[3]*Si[3];
	  S[3] = Si[1]*Si[4] - Si[2]*Si[3];
	  S[4] = Si[1]*Si[3] - Si[0]*Si[4];
	  S[5] = Si[0]*Si[2] - Si[1]*Si[1];	 
	
	  double s = ( Si[0]*S[0] + Si[1]*S[1] + Si[3]*S[3] );
	  s = ( s > 1.E-20 )  ?1./s :0;	  
	  S[0]*=s;
	  S[1]*=s;
	  S[2]*=s;
	  S[3]*=s;
	  S[4]*=s;
	  S[5]*=s;
	}
	
	//* Residual (measured - estimated)

	Double_t zeta[3] = { m[0]-r[0], m[1]-r[1], m[2]-r[2] };
	
	//* Add the daughter momentum to the particle momentum

	r[ 3] += m[ 3];
	r[ 4] += m[ 4];
	r[ 5] += m[ 5];
	r[ 6] += m[ 6];

	C[ 9] += V[ 9];
	C[13] += V[13];
	C[14] += V[14];
	C[18] += V[18];
	C[19] += V[19];
	C[20] += V[20];
	C[24] += V[24];
	C[25] += V[25];
	C[26] += V[26];
	C[27] += V[27];

	//* CHt = CH' - D'
	
	Double_t CHt0[7], CHt1[7], CHt2[7];

	CHt0[0]=C[ 0] ;      CHt1[0]=C[ 1] ;      CHt2[0]=C[ 3] ;
	CHt0[1]=C[ 1] ;      CHt1[1]=C[ 2] ;      CHt2[1]=C[ 4] ;
	CHt0[2]=C[ 3] ;      CHt1[2]=C[ 4] ;      CHt2[2]=C[ 5] ;
	CHt0[3]=C[ 6]-V[ 6]; CHt1[3]=C[ 7]-V[ 7]; CHt2[3]=C[ 8]-V[ 8];
	CHt0[4]=C[10]-V[10]; CHt1[4]=C[11]-V[11]; CHt2[4]=C[12]-V[12];
	CHt0[5]=C[15]-V[15]; CHt1[5]=C[16]-V[16]; CHt2[5]=C[17]-V[17];
	CHt0[6]=C[21]-V[21]; CHt1[6]=C[22]-V[22]; CHt2[6]=C[23]-V[23];

	//* Kalman gain K = CH'*S
	
	Double_t K0[7], K1[7], K2[7];

	for(Int_t i=0;i<7;++i){
	  K0[i] = CHt0[i]*S[0] + CHt1[i]*S[1] + CHt2[i]*S[3];
	  K1[i] = CHt0[i]*S[1] + CHt1[i]*S[2] + CHt2[i]*S[4];
	  K2[i] = CHt0[i]*S[3] + CHt1[i]*S[4] + CHt2[i]*S[5];
	}

	//* New estimation of the vertex position r += K*zeta

	for(Int_t i=0;i<7;++i) 
	  r[i] += K0[i]*zeta[0] + K1[i]*zeta[1] + K2[i]*zeta[2];
      
	//* New covariance matrix C -= K*(CH')'
      
	for(Int_t i=0, k=0;i<7;++i){
	  for(Int_t j=0;j<=i;++j,++k) 
	    C[k] -= K0[i]*CHt0[j] + K1[i]*CHt1[j] + K2[i]*CHt2[j];
	}

	//* Calculate Chi^2 & NDF

	Chi2 += (S[0]*zeta[0] + S[1]*zeta[1] + S[3]*zeta[2])*zeta[0]
	  +     (S[1]*zeta[0] + S[2]*zeta[1] + S[4]*zeta[2])*zeta[1]
	  +     (S[3]*zeta[0] + S[4]*zeta[1] + S[5]*zeta[2])*zeta[2];
	
      } // add tracks 
    
    if( iteration==0 ){
      r0[3] = r[3];
      r0[4] = r[4];
      r0[5] = r[5];
      r0[6] = r[6];
      if( Parent ){
	double dx = Parent->GetRefX()-r[0];
 	double dy = Parent->GetRefY()-r[1];
  	double dz = Parent->GetRefZ()-r[2];
        r0[7] = r[7] = sqrt( (dx*dx+dy*dy+dz*dz)
			     /(r[3]*r[3]+r[4]*r[4]+r[5]*r[5]) );
      }
    }

    if( Mass>=0 ) MeasureMass( r0, Mass );
    if( Parent ) MeasureProductionVertex( r0, Parent );
    
  }// iterations

  AtProductionVertex = 0;
}

void CbmKFParticle::ConstructFromKFParticle( vector<CbmKFParticle*> &vDaughters, CbmKFVertexInterface *Parent, Double_t Mass, Double_t CutChi2 )
{
  const Int_t MaxIter=3;

  if( CbmKF::Instance()->vTargets.empty() ){
    r[0] = r[1] = r[2] = 0.;
    C[0] = C[2] = C[5] = 1.;
    C[1] = C[3] = C[4] = 0;
  }else{
    CbmKFTube &t = CbmKF::Instance()->vTargets[0];
    r[0] = r[1] = 0.;
    r[2] = t.z;
    C[0] = C[2] = t.R*t.R/9.;
    C[5] = t.dz/6.;
    C[1] = C[3] = C[4] = 0;
  }
  
  r[3] = 0;
  r[4] = 0;
  r[5] = 0;
  r[6] = 0;
  r[7] = 0;
  
  for ( Int_t iteration =0; iteration<MaxIter;++iteration ){
    
    Double_t r0[8], C0[6];

    for( int i=0; i<8; i++) r0[i] = r[i];
    for( int i=0; i<6; i++) C0[i] = C[i];

    if(iteration == 0)
    {
      Double_t VertexGuess[3];

      Double_t fTDaughter[(int) vDaughters.size()][6];
      Double_t fCDaughter[(int) vDaughters.size()][15];

      vector<CbmKFTrack*> TrV;

      Int_t nvect = 0;
      for( vector<CbmKFParticle*>::iterator tr=vDaughters.begin(); tr!=vDaughters.end(); ++tr )
      {
        CbmKFParticle *Part = *tr;
        CbmKFTrackInterface *TrInt = new CbmKFTrackInterface();
        Part->GetKFTrack(TrInt);
        CbmKFTrack *Tr = new CbmKFTrack(*TrInt);

        fTDaughter[nvect][0] = Tr->GetTrack()[0];
        fTDaughter[nvect][1] = Tr->GetTrack()[1];
        fTDaughter[nvect][2] = Tr->GetTrack()[2];
        fTDaughter[nvect][3] = Tr->GetTrack()[3];
        fTDaughter[nvect][4] = Tr->GetTrack()[4];
        fTDaughter[nvect][5] = Tr->GetTrack()[5];
        
        fCDaughter[nvect][0] = Tr->GetCovMatrix()[0];
        fCDaughter[nvect][2] = Tr->GetCovMatrix()[2];
        fCDaughter[nvect][5] = Tr->GetCovMatrix()[5];
        fCDaughter[nvect][9] = Tr->GetCovMatrix()[9];
        fCDaughter[nvect][14] = Tr->GetCovMatrix()[14];
        
        delete Tr;
        delete TrInt;
        nvect++;
      }

      Double_t z = (fTDaughter[0][3]*fTDaughter[0][5] - fTDaughter[1][3]*fTDaughter[1][5] + fTDaughter[1][1] - fTDaughter[0][1])/(fTDaughter[0][3] - fTDaughter[1][3]);

      CbmKFParticle *Part = vDaughters[0];
      CbmKFTrackInterface *TrInt = new CbmKFTrackInterface();
      Part->GetKFTrack(TrInt);
      CbmKFTrack *Tr = new CbmKFTrack(*TrInt);

      Tr->Extrapolate( z );

      VertexGuess[0] = Tr->GetTrack()[0];
      VertexGuess[1] = Tr->GetTrack()[1];
      VertexGuess[2] = Tr->GetTrack()[5];
      delete Tr;
      r0[0] = VertexGuess[0];
      r0[1] = VertexGuess[1];
      r0[2] = VertexGuess[2];
    }

    r[3] = 0;
    r[4] = 0;
    r[5] = 0;
    r[6] = 0;
   
    for(Int_t i=0;i<36;++i) C[i]=0.;

    C[0] = C[2] = C[5] = 100.;
    C[35] = 1.;

    Double_t B[3]; 
    {
      FairField *MF = CbmKF::Instance()->GetMagneticField();
      MF->GetFieldValue( r0, B );
      const Double_t c_light =  0.000299792458;
      B[0]*=c_light; B[1]*=c_light; B[2]*=c_light;
    }

    NDF  = -3;
    Chi2 =  0.;
    Q = 0;
    bool first = 1;
    for( vector<CbmKFParticle*>::iterator tr=vDaughters.begin(); tr!=vDaughters.end(); ++tr )
    {
      CbmKFParticle Daughter = *(*tr);

      Double_t dx_aprox = Daughter.r[0] - r0[0];
      Double_t dy_aprox = Daughter.r[1] - r0[1];
      Double_t dz_aprox = Daughter.r[2] - r0[2];
      Double_t dS_aprox = sqrt((dx_aprox*dx_aprox + dy_aprox*dy_aprox + dz_aprox*dz_aprox)
				/(Daughter.r[3]*Daughter.r[3]+Daughter.r[4]*Daughter.r[4]+Daughter.r[5]*Daughter.r[5]));
	
      Daughter.Extrapolate(Daughter.r ,-dS_aprox);

      Double_t *m = Daughter.r;
      Double_t *Cd = Daughter.C;	

      Double_t d[3] = { r0[0]-m[0], r0[1]-m[1], r0[2]-m[2] };
      Double_t SigmaS = .1+10.*sqrt( (d[0]*d[0]+d[1]*d[1]+d[2]*d[2])/
				       (m[3]*m[3]+m[4]*m[4]+m[5]*m[5])  );

      Double_t h[6];

      h[0] = m[3]*SigmaS;
      h[1] = m[4]*SigmaS;
      h[2] = m[5]*SigmaS; 
      h[3] = ( h[1]*B[2]-h[2]*B[1] )*Daughter.Q;
      h[4] = ( h[2]*B[0]-h[0]*B[2] )*Daughter.Q;
      h[5] = ( h[0]*B[1]-h[1]*B[0] )*Daughter.Q;

	//* Fit of daughter momentum (x,y,z) to r0[0,1,2] vertex
	{
	  Double_t zeta[3] = { r0[0]-m[0], r0[1]-m[1], r0[2]-m[2] };

	  Double_t Vv[6] = 
	    { Cd[ 0] + h[0]*h[0], 
	      Cd[ 1] + h[1]*h[0], Cd[ 2] + h[1]*h[1],			     
	      Cd[ 3] + h[2]*h[0], Cd[ 4] + h[2]*h[1], Cd[ 5] + h[2]*h[2] };
	  
	  Double_t Vvp[9]=
	    { Cd[ 6] + h[0]*h[3], Cd[ 7] + h[1]*h[3], Cd[ 8] + h[2]*h[3],
	      Cd[10] + h[0]*h[4], Cd[11] + h[1]*h[4], Cd[12] + h[2]*h[4],
	      Cd[15] + h[0]*h[5], Cd[16] + h[1]*h[5], Cd[17] + h[2]*h[5] };

	  if( CutChi2 >0. ){

	    Double_t Si[6]={ Vv[0]+C0[0],
			     Vv[1]+C0[1], Vv[2]+C0[2],
			     Vv[3]+C0[3], Vv[4]+C0[4], Vv[5]+C0[5] };
	    Double_t S[6] = { Si[2]*Si[5] - Si[4]*Si[4],
			      Si[3]*Si[4] - Si[1]*Si[5],
			      Si[0]*Si[5] - Si[3]*Si[3],
			      Si[1]*Si[4] - Si[2]*Si[3],
			      Si[1]*Si[3] - Si[0]*Si[4],
			      Si[0]*Si[2] - Si[1]*Si[1] };		 	
	    Double_t det = ( Si[0]*S[0] + Si[1]*S[1] + Si[3]*S[3] );
	    Double_t chi2 = 
	      ( +(S[0]*zeta[0] + S[1]*zeta[1] + S[3]*zeta[2])*zeta[0]
		+(S[1]*zeta[0] + S[2]*zeta[1] + S[4]*zeta[2])*zeta[1]
		+(S[3]*zeta[0] + S[4]*zeta[1] + S[5]*zeta[2])*zeta[2]);
	    if( chi2 > 2*CutChi2*det ) continue;
	  }

	  double S[6] = 
	    { Vv[2]*Vv[5] - Vv[4]*Vv[4],
	      Vv[3]*Vv[4] - Vv[1]*Vv[5], Vv[0]*Vv[5] - Vv[3]*Vv[3],
	      Vv[1]*Vv[4] - Vv[2]*Vv[3], Vv[1]*Vv[3] - Vv[0]*Vv[4], 
	      Vv[0]*Vv[2] - Vv[1]*Vv[1] };		 
	
	  double s = ( Vv[0]*S[0] + Vv[1]*S[1] + Vv[3]*S[3] );
	  s = ( s > 1.E-20 )  ?1./s :0;

	  S[0]*=s; S[1]*=s; S[2]*=s; S[3]*=s; S[4]*=s; S[5]*=s;

	  Double_t Sz[3] = { (S[0]*zeta[0]+S[1]*zeta[1]+S[3]*zeta[2]),
			     (S[1]*zeta[0]+S[2]*zeta[1]+S[4]*zeta[2]),
			     (S[3]*zeta[0]+S[4]*zeta[1]+S[5]*zeta[2]) };
	  
	  Double_t x = m[3] + Vvp[0]*Sz[0] + Vvp[1]*Sz[1] + Vvp[2]*Sz[2];	  
	  Double_t y = m[4] + Vvp[3]*Sz[0] + Vvp[4]*Sz[1] + Vvp[5]*Sz[2];	  
	  Double_t z = m[5] + Vvp[6]*Sz[0] + Vvp[7]*Sz[1] + Vvp[8]*Sz[2];	  

	  h[0] = x*SigmaS;
	  h[1] = y*SigmaS;
	  h[2] = z*SigmaS; 
	  h[3] = ( h[1]*B[2]-h[2]*B[1] )*Daughter.Q;
	  h[4] = ( h[2]*B[0]-h[0]*B[2] )*Daughter.Q;
	  h[5] = ( h[0]*B[1]-h[1]*B[0] )*Daughter.Q;
	}
	
	Double_t V[28];

	V[ 0] = Cd[ 0] + h[0]*h[0];
	V[ 1] = Cd[ 1] + h[1]*h[0];
	V[ 2] = Cd[ 2] + h[1]*h[1];
	V[ 3] = Cd[ 3] + h[2]*h[0];
	V[ 4] = Cd[ 4] + h[2]*h[1];
	V[ 5] = Cd[ 5] + h[2]*h[2];
	
	V[ 6] = Cd[ 6] + h[3]*h[0];
	V[ 7] = Cd[ 7] + h[3]*h[1];
	V[ 8] = Cd[ 8] + h[3]*h[2];
	V[ 9] = Cd[ 9] + h[3]*h[3];

	V[10] = Cd[10] + h[4]*h[0];
	V[11] = Cd[11] + h[4]*h[1];
	V[12] = Cd[12] + h[4]*h[2];
	V[13] = Cd[13] + h[4]*h[3];
	V[14] = Cd[14] + h[4]*h[4];

	V[15] = Cd[15] + h[5]*h[0];
	V[16] = Cd[16] + h[5]*h[1];
	V[17] = Cd[17] + h[5]*h[2];
	V[18] = Cd[18] + h[5]*h[3];
	V[19] = Cd[19] + h[5]*h[4];
	V[20] = Cd[20] + h[5]*h[5];	  
      
	V[21] = Cd[21];
	V[22] = Cd[22];
	V[23] = Cd[23];
	V[24] = Cd[24];
	V[25] = Cd[25];
	V[26] = Cd[26];
	V[27] = Cd[27];

	//* 
	
	NDF  += 2;
	Q    +=  Daughter.Q;

	if( 0&&first ){
	  first = 0;
	  for( int i=0; i<7; i++) r[i] = m[i];
	  for( int i=0; i<28; i++) C[i] = V[i];
	  continue;
	}

	//*

	double S[6];
	{
	  double Si[6] = { C[0]+V[0], 
			   C[1]+V[1], C[2]+V[2], 
			   C[3]+V[3], C[4]+V[4], C[5]+V[5] };
	  
	  S[0] = Si[2]*Si[5] - Si[4]*Si[4];
	  S[1] = Si[3]*Si[4] - Si[1]*Si[5];
	  S[2] = Si[0]*Si[5] - Si[3]*Si[3];
	  S[3] = Si[1]*Si[4] - Si[2]*Si[3];
	  S[4] = Si[1]*Si[3] - Si[0]*Si[4];
	  S[5] = Si[0]*Si[2] - Si[1]*Si[1];	 
	
	  double s = ( Si[0]*S[0] + Si[1]*S[1] + Si[3]*S[3] );
	  s = ( s > 1.E-20 )  ?1./s :0;	  
	  S[0]*=s;
	  S[1]*=s;
	  S[2]*=s;
	  S[3]*=s;
	  S[4]*=s;
	  S[5]*=s;
	}
	
	//* Residual (measured - estimated)

	Double_t zeta[3] = { m[0]-r[0], m[1]-r[1], m[2]-r[2] };
	
	//* Add the daughter momentum to the particle momentum

	r[ 3] += m[ 3];
	r[ 4] += m[ 4];
	r[ 5] += m[ 5];
	r[ 6] += m[ 6];

	C[ 9] += V[ 9];
	C[13] += V[13];
	C[14] += V[14];
	C[18] += V[18];
	C[19] += V[19];
	C[20] += V[20];
	C[24] += V[24];
	C[25] += V[25];
	C[26] += V[26];
	C[27] += V[27];

	//* CHt = CH' - D'
	
	Double_t CHt0[7], CHt1[7], CHt2[7];

	CHt0[0]=C[ 0] ;      CHt1[0]=C[ 1] ;      CHt2[0]=C[ 3] ;
	CHt0[1]=C[ 1] ;      CHt1[1]=C[ 2] ;      CHt2[1]=C[ 4] ;
	CHt0[2]=C[ 3] ;      CHt1[2]=C[ 4] ;      CHt2[2]=C[ 5] ;
	CHt0[3]=C[ 6]-V[ 6]; CHt1[3]=C[ 7]-V[ 7]; CHt2[3]=C[ 8]-V[ 8];
	CHt0[4]=C[10]-V[10]; CHt1[4]=C[11]-V[11]; CHt2[4]=C[12]-V[12];
	CHt0[5]=C[15]-V[15]; CHt1[5]=C[16]-V[16]; CHt2[5]=C[17]-V[17];
	CHt0[6]=C[21]-V[21]; CHt1[6]=C[22]-V[22]; CHt2[6]=C[23]-V[23];

	//* Kalman gain K = CH'*S
	
	Double_t K0[7], K1[7], K2[7];

	for(Int_t i=0;i<7;++i){
	  K0[i] = CHt0[i]*S[0] + CHt1[i]*S[1] + CHt2[i]*S[3];
	  K1[i] = CHt0[i]*S[1] + CHt1[i]*S[2] + CHt2[i]*S[4];
	  K2[i] = CHt0[i]*S[3] + CHt1[i]*S[4] + CHt2[i]*S[5];
	}

	//* New estimation of the vertex position r += K*zeta

	for(Int_t i=0;i<7;++i) 
	  r[i] += K0[i]*zeta[0] + K1[i]*zeta[1] + K2[i]*zeta[2];
      
	//* New covariance matrix C -= K*(CH')'
      
	for(Int_t i=0, k=0;i<7;++i){
	  for(Int_t j=0;j<=i;++j,++k) 
	    C[k] -= K0[i]*CHt0[j] + K1[i]*CHt1[j] + K2[i]*CHt2[j];
	}

	//* Calculate Chi^2 & NDF

	Chi2 += (S[0]*zeta[0] + S[1]*zeta[1] + S[3]*zeta[2])*zeta[0]
	  +     (S[1]*zeta[0] + S[2]*zeta[1] + S[4]*zeta[2])*zeta[1]
	  +     (S[3]*zeta[0] + S[4]*zeta[1] + S[5]*zeta[2])*zeta[2];
	
      } // add tracks 
    
    if( iteration==0 ){
      r0[3] = r[3];
      r0[4] = r[4];
      r0[5] = r[5];
      r0[6] = r[6];
      if( Parent ){
	double dx = Parent->GetRefX()-r[0];
 	double dy = Parent->GetRefY()-r[1];
  	double dz = Parent->GetRefZ()-r[2];
        r0[7] = r[7] = sqrt( (dx*dx+dy*dy+dz*dz)
			     /(r[3]*r[3]+r[4]*r[4]+r[5]*r[5]) );
      }
    }

    if( Mass>=0 ) MeasureMass( r0, Mass );
    if( Parent ) MeasureProductionVertex( r0, Parent );
    
  }// iterations

  AtProductionVertex = 0;
}

void CbmKFParticle::MeasureMass( Double_t r0[], Double_t Mass )
{
  double H[8];
  H[0] = H[1] = H[2] = 0.;
  H[3] = -2*r0[3]; 
  H[4] = -2*r0[4]; 
  H[5] = -2*r0[5]; 
  H[6] =  2*r0[6];
  H[7] = 0; 
  double m2 = r0[6]*r0[6] - r0[3]*r0[3] - r0[4]*r0[4] - r0[5]*r0[5]; 

  double zeta = Mass*Mass - m2;
  for(Int_t i=0;i<8;++i) zeta -= H[i]*(r[i]-r0[i]);
  
  Double_t S = 0.;
  Double_t CHt[8];
  for (Int_t i=0;i<8;++i ){
    CHt[i] = 0.0;
    for (Int_t j=0;j<8;++j) CHt[i] += Cij(i,j)*H[j];
    S += H[i]*CHt[i];
  }
    
  if( S<1.e-20 ) return;
  S = 1./S;
  Chi2 += zeta*zeta*S;
  NDF  += 1;
  for( Int_t i=0, ii=0; i<8; ++i ){
    Double_t Ki = CHt[i]*S;
    r[i]+= Ki*zeta;
    for(Int_t j=0;j<=i;++j) C[ii++] -= Ki*CHt[j];    
  }
}


void CbmKFParticle::MeasureProductionVertex( Double_t r0[], 
					      CbmKFVertexInterface *Parent)
{

  double m[3], *V;
  {    
    m[0] = Parent->GetRefX();
    m[1] = Parent->GetRefY();
    m[2] = Parent->GetRefZ();
    V = Parent->GetCovMatrix();
  }
  r[7] = r0[7];
  Extrapolate(r0, -r0[7]);
  Convert(r0, 1);
 
  double Ai[6];
  Ai[0] = C[4]*C[4] - C[2]*C[5];
  Ai[1] = C[1]*C[5] - C[3]*C[4];
  Ai[3] = C[2]*C[3] - C[1]*C[4];
  double det = 1./(C[0]*Ai[0] + C[1]*Ai[1] + C[3]*Ai[3]);
  Ai[0] *= det;
  Ai[1] *= det;
  Ai[3] *= det;
  Ai[2] = ( C[3]*C[3] - C[0]*C[5] )*det;
  Ai[4] = ( C[0]*C[4] - C[1]*C[3] )*det;
  Ai[5] = ( C[1]*C[1] - C[0]*C[2] )*det;

  double B[5][3];

  B[0][0] = C[ 6]*Ai[0] + C[ 7]*Ai[1] + C[ 8]*Ai[3];
  B[0][1] = C[ 6]*Ai[1] + C[ 7]*Ai[2] + C[ 8]*Ai[4];
  B[0][2] = C[ 6]*Ai[3] + C[ 7]*Ai[4] + C[ 8]*Ai[5];

  B[1][0] = C[10]*Ai[0] + C[11]*Ai[1] + C[12]*Ai[3];
  B[1][1] = C[10]*Ai[1] + C[11]*Ai[2] + C[12]*Ai[4];
  B[1][2] = C[10]*Ai[3] + C[11]*Ai[4] + C[12]*Ai[5];

  B[2][0] = C[15]*Ai[0] + C[16]*Ai[1] + C[17]*Ai[3];
  B[2][1] = C[15]*Ai[1] + C[16]*Ai[2] + C[17]*Ai[4];
  B[2][2] = C[15]*Ai[3] + C[16]*Ai[4] + C[17]*Ai[5];

  B[3][0] = C[21]*Ai[0] + C[22]*Ai[1] + C[23]*Ai[3];
  B[3][1] = C[21]*Ai[1] + C[22]*Ai[2] + C[23]*Ai[4];
  B[3][2] = C[21]*Ai[3] + C[22]*Ai[4] + C[23]*Ai[5];

  B[4][0] = C[28]*Ai[0] + C[29]*Ai[1] + C[30]*Ai[3];
  B[4][1] = C[28]*Ai[1] + C[29]*Ai[2] + C[30]*Ai[4];
  B[4][2] = C[28]*Ai[3] + C[29]*Ai[4] + C[30]*Ai[5];

  double z[3] = { m[0]-r[0], m[1]-r[1], m[2]-r[2] };

  {
    double AV[6] = { C[0]-V[0], C[1]-V[1], C[2]-V[2], 
		     C[3]-V[3], C[4]-V[4], C[5]-V[5] };
    double AVi[6];
    AVi[0] = AV[4]*AV[4] - AV[2]*AV[5];
    AVi[1] = AV[1]*AV[5] - AV[3]*AV[4];
    AVi[2] = AV[3]*AV[3] - AV[0]*AV[5] ;
    AVi[3] = AV[2]*AV[3] - AV[1]*AV[4];
    AVi[4] = AV[0]*AV[4] - AV[1]*AV[3] ;
    AVi[5] = AV[1]*AV[1] - AV[0]*AV[2] ;

    det = 1./( AV[0]*AVi[0] + AV[1]*AVi[1] + AV[3]*AVi[3] );

    NDF  += 2;
    Chi2 += 
      ( +(AVi[0]*z[0] + AVi[1]*z[1] + AVi[3]*z[2])*z[0]
	+(AVi[1]*z[0] + AVi[2]*z[1] + AVi[4]*z[2])*z[1]
	+(AVi[3]*z[0] + AVi[4]*z[1] + AVi[5]*z[2])*z[2] )*det;
  }
  
  r[0] = m[0];
  r[1] = m[1];
  r[2] = m[2];
  r[3]+= B[0][0]*z[0] + B[0][1]*z[1] + B[0][2]*z[2];
  r[4]+= B[1][0]*z[0] + B[1][1]*z[1] + B[1][2]*z[2];
  r[5]+= B[2][0]*z[0] + B[2][1]*z[1] + B[2][2]*z[2];
  r[6]+= B[3][0]*z[0] + B[3][1]*z[1] + B[3][2]*z[2];
  r[7]+= B[4][0]*z[0] + B[4][1]*z[1] + B[4][2]*z[2];
  
  double d0, d1, d2;

  C[0] = V[0];
  C[1] = V[1];
  C[2] = V[2];
  C[3] = V[3];
  C[4] = V[4];
  C[5] = V[5];

  d0= B[0][0]*V[0] + B[0][1]*V[1] + B[0][2]*V[3] - C[ 6];
  d1= B[0][0]*V[1] + B[0][1]*V[2] + B[0][2]*V[4] - C[ 7];
  d2= B[0][0]*V[3] + B[0][1]*V[4] + B[0][2]*V[5] - C[ 8];

  C[ 6]+= d0;
  C[ 7]+= d1;
  C[ 8]+= d2;
  C[ 9]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];

  d0= B[1][0]*V[0] + B[1][1]*V[1] + B[1][2]*V[3] - C[10];
  d1= B[1][0]*V[1] + B[1][1]*V[2] + B[1][2]*V[4] - C[11];
  d2= B[1][0]*V[3] + B[1][1]*V[4] + B[1][2]*V[5] - C[12];

  C[10]+= d0;
  C[11]+= d1;
  C[12]+= d2;
  C[13]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[14]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];

  d0= B[2][0]*V[0] + B[2][1]*V[1] + B[2][2]*V[3] - C[15];
  d1= B[2][0]*V[1] + B[2][1]*V[2] + B[2][2]*V[4] - C[16];
  d2= B[2][0]*V[3] + B[2][1]*V[4] + B[2][2]*V[5] - C[17];

  C[15]+= d0;
  C[16]+= d1;
  C[17]+= d2;
  C[18]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[19]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];
  C[20]+= d0*B[2][0] + d1*B[2][1] + d2*B[2][2];

  d0= B[3][0]*V[0] + B[3][1]*V[1] + B[3][2]*V[3] - C[21];
  d1= B[3][0]*V[1] + B[3][1]*V[2] + B[3][2]*V[4] - C[22];
  d2= B[3][0]*V[3] + B[3][1]*V[4] + B[3][2]*V[5] - C[23];

  C[21]+= d0;
  C[22]+= d1;
  C[23]+= d2;
  C[24]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[25]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];
  C[26]+= d0*B[2][0] + d1*B[2][1] + d2*B[2][2];
  C[27]+= d0*B[3][0] + d1*B[3][1] + d2*B[3][2];

  d0= B[4][0]*V[0] + B[4][1]*V[1] + B[4][2]*V[3] - C[28];
  d1= B[4][0]*V[1] + B[4][1]*V[2] + B[4][2]*V[4] - C[29];
  d2= B[4][0]*V[3] + B[4][1]*V[4] + B[4][2]*V[5] - C[30];

  C[28]+= d0;
  C[29]+= d1;
  C[30]+= d2;
  C[31]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[32]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];
  C[33]+= d0*B[2][0] + d1*B[2][1] + d2*B[2][2];
  C[34]+= d0*B[3][0] + d1*B[3][1] + d2*B[3][2];
  C[35]+= d0*B[4][0] + d1*B[4][1] + d2*B[4][2];
  
  Extrapolate(r0, r[7]);
  Convert(r0, 0);
}


void CbmKFParticle::Convert( Double_t r0[], bool ToProduction )
{

  Double_t B[3]; 
  {
    FairField *MF = CbmKF::Instance()->GetMagneticField();
    MF->GetFieldValue( r0, B );
    const Double_t c_light =  Q*0.000299792458;
    B[0]*=c_light; B[1]*=c_light; B[2]*=c_light;
  }

  Double_t h[6];
  
  h[0] = r0[3];
  h[1] = r0[4];
  h[2] = r0[5];
  if( ToProduction ){ h[0]=-h[0]; h[1]=-h[1]; h[2]=-h[2]; } 
  h[3] = h[1]*B[2]-h[2]*B[1];
  h[4] = h[2]*B[0]-h[0]*B[2];
  h[5] = h[0]*B[1]-h[1]*B[0];
  
  Double_t c;

  c = C[28]+h[0]*C[35];
  C[ 0]+= h[0]*(c+C[28]);
  C[28] = c;

  C[ 1]+= h[1]*C[28] + h[0]*C[29];
  c = C[29]+h[1]*C[35];
  C[ 2]+= h[1]*(c+C[29]);
  C[29] = c;

  C[ 3]+= h[2]*C[28] + h[0]*C[30];
  C[ 4]+= h[2]*C[29] + h[1]*C[30];
  c = C[30]+h[2]*C[35];
  C[ 5]+= h[2]*(c+C[30]);
  C[30] = c;

  C[ 6]+= h[3]*C[28] + h[0]*C[31];
  C[ 7]+= h[3]*C[29] + h[1]*C[31];
  C[ 8]+= h[3]*C[30] + h[2]*C[31];
  c = C[31]+h[3]*C[35];
  C[ 9]+= h[3]*(c+C[31]);
  C[31] = c;
  
  C[10]+= h[4]*C[28] + h[0]*C[32];
  C[11]+= h[4]*C[29] + h[1]*C[32];
  C[12]+= h[4]*C[30] + h[2]*C[32];
  C[13]+= h[4]*C[31] + h[3]*C[32];
  c = C[32]+h[4]*C[35];
  C[14]+= h[4]*(c+C[32]);
  C[32] = c;
  
  C[15]+= h[5]*C[28] + h[0]*C[33];
  C[16]+= h[5]*C[29] + h[1]*C[33];
  C[17]+= h[5]*C[30] + h[2]*C[33];
  C[18]+= h[5]*C[31] + h[3]*C[33];
  C[19]+= h[5]*C[32] + h[4]*C[33];
  c = C[33]+h[5]*C[35];
  C[20]+= h[5]*(c+C[33]);
  C[33] = c;

  C[21]+= h[0]*C[34];
  C[22]+= h[1]*C[34];
  C[23]+= h[2]*C[34];
  C[24]+= h[3]*C[34];
  C[25]+= h[4]*C[34];
  C[26]+= h[5]*C[34];
}

void CbmKFParticle::ExtrapolateLine(Double_t r0[], Double_t dS )
{
  if( r0 && r0!=r ){
    r0[0]+= dS*r0[3];
    r0[1]+= dS*r0[4];
    r0[2]+= dS*r0[5];
  }

  r[0]+= dS*r[3];
  r[1]+= dS*r[4];
  r[2]+= dS*r[5];

  double C6  = C[ 6] + dS*C[ 9];
  double C11 = C[11] + dS*C[14];
  double C17 = C[17] + dS*C[20];
  double SC13 = dS*C[13];
  double SC18 = dS*C[18];
  double SC19 = dS*C[19];

  C[ 0] += dS*( C[ 6] + C6  );
  C[ 2] += dS*( C[11] + C11 );
  C[ 5] += dS*( C[17] + C17 );

  C[ 7] += SC13;
  C[ 8] += SC18;
  C[12] += SC19;

  C[ 1] += dS*( C[10] + C[ 7] );
  C[ 3] += dS*( C[15] + C[ 8] );
  C[ 4] += dS*( C[16] + C[12] );
 
  C[ 6]  = C6;
  C[10] += SC13;
  C[11]  = C11;
  C[15] += SC18;
  C[16] += SC19;
  C[17]  = C17;
  
  C[21] += dS*C[24];
  C[22] += dS*C[25];
  C[23] += dS*C[26];
  C[28] += dS*C[31];
  C[29] += dS*C[32];
  C[30] += dS*C[33];
}

void CbmKFParticle::Extrapolate(Double_t r0[], Double_t dS )
{  

  if( Q==0 ){
    ExtrapolateLine( r0, dS );
    return;
  }

  const Double_t c_light = 0.000299792458;
  FairField *MF = CbmKF::Instance()->GetMagneticField();

  Double_t c = Q*c_light;

  // construct coefficients 

  Double_t 
    px   = r[3],
    py   = r[4],
    pz   = r[5];
      
  Double_t sx=0, sy=0, sz=0, syy=0, syz=0, syyy=0, Sx=0, Sy=0, Sz=0, Syy=0, Syz=0, Syyy=0;

  { // get field integrals

    Double_t B[3][3];   
    Double_t p0[3], p1[3], p2[3];

    // line track approximation

    p0[0] = r[0];
    p0[1] = r[1];
    p0[2] = r[2];
  
    p2[0] = r[0] + px*dS;
    p2[1] = r[1] + py*dS;
    p2[2] = r[2] + pz*dS;
  
    p1[0] = 0.5*(p0[0]+p2[0]);
    p1[1] = 0.5*(p0[1]+p2[1]);
    p1[2] = 0.5*(p0[2]+p2[2]);

    // first order track approximation
    {
      MF->GetFieldValue( p0, B[0] );
      MF->GetFieldValue( p1, B[1] );
      MF->GetFieldValue( p2, B[2] );

      Double_t Sy1 = ( 7*B[0][1] + 6*B[1][1]-B[2][1] )*c*dS*dS/96.;
      Double_t Sy2 = (   B[0][1] + 2*B[1][1]         )*c*dS*dS/6.;

      p1[0] -= Sy1*pz;
      p1[2] += Sy1*px;
      p2[0] -= Sy2*pz;
      p2[2] += Sy2*px;
    }

    MF->GetFieldValue( p0, B[0] );
    MF->GetFieldValue( p1, B[1] );
    MF->GetFieldValue( p2, B[2] );
    
    sx = c*( B[0][0] + 4*B[1][0] + B[2][0] )*dS/6.;
    sy = c*( B[0][1] + 4*B[1][1] + B[2][1] )*dS/6.;
    sz = c*( B[0][2] + 4*B[1][2] + B[2][2] )*dS/6.;

    Sx = c*( B[0][0] + 2*B[1][0])*dS*dS/6.;
    Sy = c*( B[0][1] + 2*B[1][1])*dS*dS/6.;
    Sz = c*( B[0][2] + 2*B[1][2])*dS*dS/6.;

    Double_t c2[3][3]    =   { {  5, -4, -1},{  44,  80,  -4},{ 11, 44, 5} }; // /=360.    
    Double_t C2[3][3]    =   { { 38,  8, -4},{ 148, 208, -20},{  3, 36, 3} }; // /=2520.
    for(Int_t n=0; n<3; n++)
      for(Int_t m=0; m<3; m++) 
	{
	  syz += c2[n][m]*B[n][1]*B[m][2];
	  Syz += C2[n][m]*B[n][1]*B[m][2];
	}
 
    syz  *= c*c*dS*dS/360.;
    Syz  *= c*c*dS*dS*dS/2520.;
    
    syy  = c*( B[0][1] + 4*B[1][1] + B[2][1] )*dS;
    syyy = syy*syy*syy / 1296;
    syy  = syy*syy/72;

    Syy = ( B[0][1]*( 38*B[0][1] + 156*B[1][1]  -   B[2][1] )+
	    B[1][1]*(              208*B[1][1]  +16*B[2][1] )+
	    B[2][1]*(                             3*B[2][1] )  
	    )*dS*dS*dS*c*c/2520.;
    Syyy = 
      (
       B[0][1]*( B[0][1]*( 85*B[0][1] + 526*B[1][1]  - 7*B[2][1] )+
		 B[1][1]*(             1376*B[1][1]  +84*B[2][1] )+
		 B[2][1]*(                            19*B[2][1] )  )+
       B[1][1]*( B[1][1]*(             1376*B[1][1] +256*B[2][1] )+
		 B[2][1]*(                            62*B[2][1] )  )+
       B[2][1]*B[2][1]  *(                             3*B[2][1] )       
       )*dS*dS*dS*dS*c*c*c/90720.;    
 
  }

  Double_t J[8][8];
  for( int i=0; i<8; i++ ) for( int j=0; j<8; j++) J[i][j]=0;

  J[0][0]=1; J[0][1]=0; J[0][2]=0; J[0][3]=dS-Syy;  J[0][4]=Sx;  J[0][5]=Syyy-Sy;
  J[1][0]=0; J[1][1]=1; J[1][2]=0; J[1][3]=-Sz;     J[1][4]=dS;  J[1][5]=Sx+Syz;
  J[2][0]=0; J[2][1]=0; J[2][2]=1; J[2][3]=Sy-Syyy; J[2][4]=-Sx; J[2][5]=dS-Syy;
  
  J[3][0]=0; J[3][1]=0; J[3][2]=0; J[3][3]=1-syy;   J[3][4]=sx;  J[3][5]=syyy-sy;
  J[4][0]=0; J[4][1]=0; J[4][2]=0; J[4][3]=-sz;     J[4][4]=1;   J[4][5]=sx+syz;
  J[5][0]=0; J[5][1]=0; J[5][2]=0; J[5][3]=sy-syyy; J[5][4]=-sx; J[5][5]=1-syy;
  J[6][6] = J[7][7] = 1;
  
  r[0]+= J[0][3]*px + J[0][4]*py + J[0][5]*pz;
  r[1]+= J[1][3]*px + J[1][4]*py + J[1][5]*pz;
  r[2]+= J[2][3]*px + J[2][4]*py + J[2][5]*pz;
  r[3] = J[3][3]*px + J[3][4]*py + J[3][5]*pz;
  r[4] = J[4][3]*px + J[4][4]*py + J[4][5]*pz;
  r[5] = J[5][3]*px + J[5][4]*py + J[5][5]*pz;

  if( r0 && r0!=r ){
    px   = r0[3];
    py   = r0[4];
    pz   = r0[5];
    
    r0[0]+= J[0][3]*px + J[0][4]*py + J[0][5]*pz;
    r0[1]+= J[1][3]*px + J[1][4]*py + J[1][5]*pz;
    r0[2]+= J[2][3]*px + J[2][4]*py + J[2][5]*pz;
    r0[3] = J[3][3]*px + J[3][4]*py + J[3][5]*pz;
    r0[4] = J[4][3]*px + J[4][4]*py + J[4][5]*pz;
    r0[5] = J[5][3]*px + J[5][4]*py + J[5][5]*pz;
  }

  CbmKFMath::multQSQt( 8, J[0], C, C);

}


void CbmKFParticle::TransportToProductionVertex()
{
  if( AtProductionVertex ) return;
  Double_t r0[8];
  for( int i=0; i<8; i++ ) r0[i] = r[i];
  Extrapolate(r0, -r[7]);
  Convert(r0, 1);    
  AtProductionVertex = 1;
}

void CbmKFParticle::TransportToDecayVertex()
{
  if( !AtProductionVertex ) return;
  Double_t r0[8];
  for( int i=0; i<8; i++ ) r0[i] = r[i];
  Extrapolate(r0, r[7]);
  Convert(r0, 0);
  AtProductionVertex = 0;
}


void CbmKFParticle::GetKFTrack( CbmKFTrackInterface* Track){

  Double_t *T = Track->GetTrack();
  Double_t *Cov = Track->GetCovMatrix();
  Track->GetRefNDF() = NDF;
  Track->GetRefChi2() = Chi2;

  Double_t px=r[3], py=r[4],  pz=r[5];
  Double_t p = sqrt( px*px + py*py + pz*pz );
  Double_t pzi = 1/pz;
  Double_t qp=1/p;

  if( Q!=0 ) qp = Q*qp;

  T[0] = r[0];   // dX = dx -T[2]*dz
  T[1] = r[1];   // dY = dy -T[3]*dz
  T[2] = px*pzi; // dtx= (dpx - T[2]*dpz)/pz
  T[3] = py*pzi; // dty= (dpy - T[3]*dpz)/pz
  T[4] = qp;     // dq = (px*dpx + py*dpy + pz*dpz)*(-q/p^3)
  T[5] = r[2];
  
  Double_t qp3 = qp*qp*qp;
  Double_t pzi2= pzi*pzi;

  Double_t cXpz= C[15]-T[2]*C[17];
  Double_t cYpz= C[16]-T[3]*C[17];
  Double_t cqpx = -qp3*( px*C[ 9] + py*C[13] + pz*C[18] );
  Double_t cqpy = -qp3*( px*C[13] + py*C[14] + pz*C[19] );
  Double_t cqpz = -qp3*( px*C[18] + py*C[19] + pz*C[20] );

  Cov[ 0]= C[0] + T[2]*( - 2*C[3] + T[2]*C[5] );
  Cov[ 1]= C[1] - T[2]*C[4] + T[3]*( -C[3] + T[2]*C[5] );
  Cov[ 2]= C[2] + T[3]*( - 2*C[4] + T[3]*C[5]);
  Cov[ 3]= pzi*( C[6] - T[2]*(C[8]     + cXpz) );
  Cov[ 4]= pzi*( C[7] - T[3]*C[8] - T[2]*cYpz );
  Cov[ 5]= pzi2*( C[9] + T[2]*( -2*C[18] + T[2]*C[20]) );
  Cov[ 6]= pzi*( C[10]-T[2]*C[12]-T[3]*cXpz );
  Cov[ 7]= pzi*( C[11]-T[3]*(C[12]    +cYpz) );
  Cov[ 8]= pzi2*( C[13] - T[2]*C[19] -T[3]*C[18] + T[2]*T[3]*C[20] );
  Cov[ 9]= pzi2*( C[14] + T[3]*( -2*C[19] + T[3]*C[20]) );
  Cov[10]=-qp3*(px*C[6]+py*C[10]+pz*C[15]) - T[2]*cqpz;
  Cov[11]=-qp3*(px*C[7]+py*C[11]+pz*C[16]) - T[3]*cqpz;
  Cov[12]= pzi*( cqpx - T[2]*cqpz );
  Cov[13]= pzi*( cqpy - T[3]*cqpz );
  Cov[14]=-qp3*( px*cqpx + py*cqpy + pz*cqpz );
}

void CbmKFParticle::GetKFVertex( CbmKFVertexInterface  &vtx )
{
  vtx.GetRefX() = r[0];   
  vtx.GetRefY() = r[1];   
  vtx.GetRefZ() = r[2];
  for( int i=0; i<6; i++) vtx.GetCovMatrix()[i] = C[i];
  vtx.GetRefChi2() = Chi2;
  vtx.GetRefNDF()  = NDF;
}

void CbmKFParticle::GetMomentum( Double_t &P, Double_t &Error_ )
{
  Double_t x = r[3];
  Double_t y = r[4];
  Double_t z = r[5];
  Double_t x2 = x*x;
  Double_t y2 = y*y;
  Double_t z2 = z*z;
  Double_t p2 = x2+y2+z2;
  P = sqrt(p2);
  Error_ = sqrt( (x2*C[9]+y2*C[14]+z2*C[20] 
		 + 2*(x*y*C[13]+x*z*C[18]+y*z*C[19]) )/p2 );
}

void CbmKFParticle::GetMass( Double_t &M, Double_t &Error_ )
{
  // S = sigma^2 of m2/2

  Double_t S = (  r[3]*r[3]*C[9] + r[4]*r[4]*C[14] + r[5]*r[5]*C[20] 
		  + r[6]*r[6]*C[27] 
		+2*( + r[3]*r[4]*C[13] + r[5]*(r[3]*C[18] + r[4]*C[19]) 
		     - r[6]*( r[3]*C[24] + r[4]*C[25] + r[5]*C[26] )   )
		 ); 
  Double_t m2 = r[6]*r[6] - r[3]*r[3] - r[4]*r[4] - r[5]*r[5];
  M     = (         m2>1.e-20 ) ? sqrt(m2)   :0.  ;
  Error_ = ( S>=0 && m2>1.e-20 ) ? sqrt(S/m2) :1.e4;
}


void CbmKFParticle::GetDecayLength( Double_t &L, Double_t &Error_ )
{
  Double_t x = r[3];
  Double_t y = r[4];
  Double_t z = r[5];
  Double_t t = r[7];
  Double_t x2 = x*x;
  Double_t y2 = y*y;
  Double_t z2 = z*z;
  Double_t p2 = x2+y2+z2;
  L = t*sqrt(p2);
  Error_ = sqrt( p2*C[35] + t*t/p2*(x2*C[9]+y2*C[14]+z2*C[20]
				   + 2*(x*y*C[13]+x*z*C[18]+y*z*C[19]) )
		+ 2*t*(x*C[31]+y*C[32]+z*C[33]) 
		);
}

void CbmKFParticle::GetLifeTime( Double_t &TauC, Double_t &Error_ ){
  Double_t m, dm;
  GetMass( m, dm );
  Double_t cTM = (-r[3]*C[31] - r[4]*C[32] - r[5]*C[33] + r[6]*C[34]);
  TauC = r[7]*m;
  Error_ = sqrt( m*m*C[35] + 2*r[7]*cTM + r[7]*r[7]*dm*dm);
}

Double_t CbmKFParticle::GetRapidity() const 
{ 
  return 0.5*TMath::Log((r[6] + r[5])/(r[6] - r[5])); 
}
Double_t CbmKFParticle::GetPt()       const 
{ 
  return TMath::Sqrt(r[3]*r[3] + r[4]*r[4]);
}
Double_t CbmKFParticle::GetTheta()    const 
{ 
  return TMath::ATan2(GetPt(),r[5]);
}
Double_t CbmKFParticle::GetPhi()      const 
{ 
  return TMath::ATan2(r[4],r[5]);
}


Double_t CbmKFParticle::GetDStoPoint( const Double_t xyz[] ) const
{
  //TODO check the method!
  Double_t dx = xyz[0] - r[0];
  Double_t dy = xyz[1] - r[1];
  Double_t dz = xyz[2] - r[2];
  Double_t p2 = r[3]*r[3] + r[4]*r[4]+r[5]*r[5];

  if (Q == 0 && r[2] < xyz[2]) return sqrt((dx*dx+dy*dy+dz*dz)/p2);
  if (Q == 0 && r[2] >= xyz[2]) return -sqrt((dx*dx+dy*dy+dz*dz)/p2);

  const Double_t kCLight = 0.000299792458;
  Double_t B[3];
  FairField *MF = CbmKF::Instance()->GetMagneticField();
  MF->GetFieldValue( r, B );

  Double_t bq1 = B[0]*Q*kCLight;
  Double_t bq2 = B[1]*Q*kCLight;
  Double_t bq3 = B[2]*Q*kCLight;
  Double_t a = dx*r[3]+dy*r[4]+dz*r[5];
  Double_t B2 = B[0]*B[0]+B[1]*B[1]+B[2]*B[2];
  Double_t bq = Q*kCLight*sqrt(B2);
  Double_t pB = r[3]*B[0]+r[4]*B[1]+r[5]*B[2];
  Double_t rB = dx*B[0]+dy*B[1]+dz*B[2];
  Double_t pt2 = p2 - pB*pB/B2;
  a = a - pB*rB/B2;

  Double_t dS;

  if( pt2<1.e-4 ) return 0;

  if( TMath::Abs(bq)<1.e-8 ) dS = a/pt2;
  else dS =  TMath::ATan2( bq*a, pt2 + bq1*(dz*r[4]-dy*r[5]) - bq2*(dz*r[3]-dx*r[5]) + bq3*(dy*r[3]-dx*r[4]) )/bq;

  // Double_t dSm = rB/pB;
//  cout <<"normalnyj S  " <<  dS << endl;
//  cout <<"moj       S  " << dSm << endl;

  return dS;
  //return sqrt(dx*dx+dy*dy+dz*dz);
}

