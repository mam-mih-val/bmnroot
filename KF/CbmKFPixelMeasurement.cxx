#include "CbmKFPixelMeasurement.h"

#include <cmath>

using std::vector;

ClassImp(CbmKFPixelMeasurement);

Int_t CbmKFPixelMeasurement::Filter( CbmKFTrackInterface &track ){
  
  Bool_t err = 0;

  Double_t *T = track.GetTrack();
  Double_t *C = track.GetCovMatrix();  

  Double_t w = (C[0]+V[0])*(C[2]+V[2]) - (C[1]+V[1])*(C[1]+V[1]);
  if( w<1.E-20 || !finite(w)) {
    err = 1;
    return err;
  }
  w = 1./w ;
  if( !finite(w) ) return 1;

  Double_t W[3] = { w*(C[2]+V[2]), -w*(C[1]+V[1]), w*(C[0]+V[0])};
  Double_t zeta[2] = { T[0] - x,  T[1] - y };

  track.GetRefChi2() += zeta[0]*zeta[0]*W[0] + 2*zeta[0]*zeta[1]*W[1] + zeta[1]*zeta[1]*W[2];
  track.GetRefNDF() += 2;

  Double_t K[5][2] = { { C[ 0]*W[ 0] + C[ 1]*W[ 1],  C[ 0]*W[ 1] + C[ 1]*W[ 2] },
		     { C[ 1]*W[ 0] + C[ 2]*W[ 1],  C[ 1]*W[ 1] + C[ 2]*W[ 2] },
		     { C[ 3]*W[ 0] + C[ 4]*W[ 1],  C[ 3]*W[ 1] + C[ 4]*W[ 2] },
		     { C[ 6]*W[ 0] + C[ 7]*W[ 1],  C[ 6]*W[ 1] + C[ 7]*W[ 2] },
		     { C[10]*W[ 0] + C[11]*W[ 1],  C[10]*W[ 1] + C[11]*W[ 2] }, };
  
  T[0]-= K[0][0]*zeta[0] + K[0][1]*zeta[1];
  T[1]-= K[1][0]*zeta[0] + K[1][1]*zeta[1];
  T[2]-= K[2][0]*zeta[0] + K[2][1]*zeta[1];
  T[3]-= K[3][0]*zeta[0] + K[3][1]*zeta[1];
  T[4]-= K[4][0]*zeta[0] + K[4][1]*zeta[1];
  
  Double_t KHC[15] = { C[0]*K[0][0] + C[1]*K[0][1],
		       C[0]*K[1][0] + C[1]*K[1][1], C[1]*K[1][0] + C[2]*K[1][1],
		     
		       C[0]*K[2][0] + C[1]*K[2][1], C[1]*K[2][0] + C[2]*K[2][1], 
		       C[3]*K[2][0] + C[4]*K[2][1],
		     
		       C[0]*K[3][0] + C[1]*K[3][1], C[1]*K[3][0] + C[2]*K[3][1], 
		       C[3]*K[3][0] + C[4]*K[3][1], C[6]*K[3][0] + C[7]*K[3][1],
		     
		       C[0]*K[4][0] + C[1]*K[4][1], C[1]*K[4][0] + C[2]*K[4][1], 
		       C[3]*K[4][0] + C[4]*K[4][1], C[6]*K[4][0] + C[7]*K[4][1],
		       C[10]*K[4][0] + C[11]*K[4][1]                             }; 
  
  C[ 0]-= KHC[ 0];
  C[ 1]-= KHC[ 1];
  C[ 2]-= KHC[ 2];
  C[ 3]-= KHC[ 3];
  C[ 4]-= KHC[ 4];
  C[ 5]-= KHC[ 5];
  C[ 6]-= KHC[ 6];
  C[ 7]-= KHC[ 7];
  C[ 8]-= KHC[ 8];
  C[ 9]-= KHC[ 9];
  C[10]-= KHC[10];
  C[11]-= KHC[11];
  C[12]-= KHC[12];
  C[13]-= KHC[13];
  C[14]-= KHC[14];  
  return 0;
}


////////////////////////////////////////////////////////////////////////////
// 
// mathAddMeasurements: the implementation of the Probabilistic
//                      Data Association Filter for the MAPS
// 
//
// Author : Dmitry Emeliyanov, RAL, dmitry.emeliyanov@cern.ch
//     
/////////////////////////////////////////////////////////////////////////////


//#define _DEBUG_PDAF_

void CbmKFPixelMeasurement::FilterPDAF(  CbmKFTrackInterface &track, 
					 vector<CbmKFPixelMeasurement*> &vm,
					 double gateX, double gateY, 
					 vector<double> &vProb              )
{
  const double Pdetect=0.90;// hit efficiency
  const double gateEff=0.99;// probability of the correct hit falling into the search window
                            // 10 sigma x 10 sigma size

  Double_t *T = track.GetTrack();
  Double_t *C = track.GetCovMatrix();  
  
  double gateArea=gateX*gateY; 
  double chi2,zeta[2];
  int ndf=2,idx,i,j,k;
  vector<double> vExp;
  vector<double> vResidX;
  vector<double> vResidY;
  vector<double> vChi2;

#ifdef _DEBUG_PDAF_

  cout<<"PDAF: "<<vm->size()<<" hits validated"<<endl;  
  cout<<"Initial params: x="<<T[0]<<" y="<<T[1]<<" tx="<<T[2]
      <<" ty="<<T[3]<<" Q="<<T[4]<<endl;
  cout<<"GateX="<<gateX<<" GateY="<<gateY<<endl;

#endif

  vExp.clear();vResidX.clear();vResidY.clear();
  vChi2.clear();
  double Sk,Ck;
  
  const double *V = (*vm.begin())->V;

  // determinant of the residual covariance

  Sk=(C[0]+V[0])*(C[2]+V[2]) - (C[1]+V[1])*(C[1]+V[1]);
  
  // 1. Normalization constant

  double Lambda = 1./1.;

  Ck=2.0*3.1415926*sqrt(Sk)*(1.0-Pdetect*gateEff)/(gateArea*Pdetect*gateEff)*Lambda;
  
  double w = Sk;
  if ( w<1.E-20 ) return;
  w = 1./w;
  
  // 2. Kalman gain - assumed to be the same for all gathered hits in a window

  double W[3] = { w*(C[2]+V[2]), -w*(C[1]+V[1]), w*(C[0]+V[0])};
  double K[5][2] = 
    {
      { C[ 0]*W[ 0] + C[ 1]*W[ 1],  C[ 0]*W[ 1] + C[ 1]*W[ 2] },
      { C[ 1]*W[ 0] + C[ 2]*W[ 1],  C[ 1]*W[ 1] + C[ 2]*W[ 2] },
      { C[ 3]*W[ 0] + C[ 4]*W[ 1],  C[ 3]*W[ 1] + C[ 4]*W[ 2] },
      { C[ 6]*W[ 0] + C[ 7]*W[ 1],  C[ 6]*W[ 1] + C[ 7]*W[ 2] },
      { C[10]*W[ 0] + C[11]*W[ 1],  C[10]*W[ 1] + C[11]*W[ 2] },
    };
  
  // 3. Exponential weights

  for(vector<CbmKFPixelMeasurement*>::iterator pmIt=vm.begin();
      pmIt!=vm.end();++pmIt)
    {
      double resX=(*pmIt)->x-T[0];
      double resY=(*pmIt)->y-T[1];
      chi2=resX*resX*W[0]+2*resY*resX*W[1] + resY*resY*W[2]; 
#ifdef _DEBUG_PDAF_   
      cout<<"resX="<<resX<<" resY="<<resY <<" chi2dist="<<chi2<<endl;
#endif
      vChi2.push_back(chi2);
      vResidX.push_back(resX);
      vResidY.push_back(resY);
      vExp.push_back(exp(-0.5*chi2));
    }

  // 4. Assignment probabilities

  double Sum=Ck;
  vector<double>::iterator eIt;
  for(eIt=vExp.begin();eIt!=vExp.end();++eIt)
    {
      Sum+=(*eIt);
    }
  Sum=1.0/Sum;
  double P0=Ck*Sum;
#ifdef _DEBUG_PDAF_
  cout<<"No-detect probability = "<<P0<<endl;
#endif
  for(eIt=vExp.begin();eIt!=vExp.end();++eIt)
    {
      vProb.push_back(Sum*(*eIt));
    }

  // 5. Merged (i.e. weighted by the assignment probabilities) residuals

  zeta[0]=0.0;zeta[1]=0.0;idx=0;chi2=0.0;
  
  for(eIt=vProb.begin();eIt!=vProb.end();++eIt)
    {
      zeta[0]+=vResidX[idx]*(*eIt);
      zeta[1]+=vResidY[idx]*(*eIt);
      chi2+=vChi2[idx]*(*eIt);
#ifdef _DEBUG_PDAF_
      cout<<"Hit "<<idx<<" Assignment probability = "<<(*eIt)<<endl;
#endif
      idx++;
    }
#ifdef _DEBUG_PDAF_
  cout<<"Merged resX="<<zeta[0]<<" resY="<<zeta[1]<<endl;
#endif

  // 6. Empirical correlation matrix of the residuals

  double CR[2][2];
  CR[0][0]=-zeta[0]*zeta[0];
  CR[0][1]=-zeta[0]*zeta[1];
  CR[1][0]=-zeta[1]*zeta[0];
  CR[1][1]=-zeta[1]*zeta[1];
  idx=0;
  for(eIt=vProb.begin();eIt!=vProb.end();++eIt)
    {
      CR[0][0]+=(*eIt)*vResidX[idx]*vResidX[idx];
      CR[0][1]+=(*eIt)*vResidX[idx]*vResidY[idx];
      CR[1][0]+=(*eIt)*vResidY[idx]*vResidX[idx];
      CR[1][1]+=(*eIt)*vResidY[idx]*vResidY[idx];
      idx++;
    }
  double KD[5][2];
  for(i=0;i<5;i++)
    for(j=0;j<2;j++)
      {
	KD[i][j]=0.0;
	for(k=0;k<2;k++) KD[i][j]+=K[i][k]*CR[k][j];
      }

  // 7. Additional covariance term - reflects influence of the residual spread

  double Ga[5][5];
  for(i=0;i<5;i++)
    for(j=0;j<5;j++)
      {
	Ga[i][j]=0.0;
	for(k=0;k<2;k++) Ga[i][j]+=KD[i][k]*K[j][k];
      }
  
  // 8. Track parameters update - usual Kalman formalizm but with the merged
  // residual 

  T[0]+= K[0][0]*zeta[0] + K[0][1]*zeta[1];
  T[1]+= K[1][0]*zeta[0] + K[1][1]*zeta[1];
  T[2]+= K[2][0]*zeta[0] + K[2][1]*zeta[1];
  T[3]+= K[3][0]*zeta[0] + K[3][1]*zeta[1];
  T[4]+= K[4][0]*zeta[0] + K[4][1]*zeta[1];
	
  double KHC[15] =
    {
      C[0]*K[0][0] + C[1]*K[0][1],
      C[0]*K[1][0] + C[1]*K[1][1], C[1]*K[1][0] + C[2]*K[1][1],
      
      C[0]*K[2][0] + C[1]*K[2][1], C[1]*K[2][0] + C[2]*K[2][1], 
      C[3]*K[2][0] + C[4]*K[2][1],
      
      C[0]*K[3][0] + C[1]*K[3][1], C[1]*K[3][0] + C[2]*K[3][1], 
      C[3]*K[3][0] + C[4]*K[3][1], C[6]*K[3][0] + C[7]*K[3][1],
      
      C[0]*K[4][0] + C[1]*K[4][1], C[1]*K[4][0] + C[2]*K[4][1], 
      C[3]*K[4][0] + C[4]*K[4][1], C[6]*K[4][0] + C[7]*K[4][1],
      C[10]*K[4][0] + C[11]*K[4][1]
    }; 

  double Gs[15];
  for(i=0;i<15;i++) Gs[i]=C[i];

  // 9. Standard Kalman covariance
	
  Gs[ 0]-= KHC[ 0];Gs[ 1]-= KHC[ 1];Gs[ 2]-= KHC[ 2];Gs[ 3]-= KHC[ 3];
  Gs[ 4]-= KHC[ 4];Gs[ 5]-= KHC[ 5];Gs[ 6]-= KHC[ 6];Gs[ 7]-= KHC[ 7];
  Gs[ 8]-= KHC[ 8];Gs[ 9]-= KHC[ 9];Gs[10]-= KHC[10];Gs[11]-= KHC[11];
  Gs[12]-= KHC[12];Gs[13]-= KHC[13];Gs[14]-= KHC[14];
  
  // 10. PDAF covariance: linear combination of the extrapolated covariance and 
  //                      covariance produced by the Kalman filter + additional term

  idx=0;
  for(i=0;i<5;i++)
    for(j=0;j<=i;j++)
      {
	C[idx]=P0*C[idx]+(1-P0)*Gs[idx]+Ga[i][j];
	idx++;
      }
#ifdef _DEBUG_PDAF_
  cout<<"Updated params: x="<<T[0]<<" y="<<T[1]<<" tx="<<T[2]
      <<" ty="<<T[3]<<" Q="<<T[4]<<endl;
  cout<<"chi2 contrib.="<<chi2<<endl;
#endif
  
  track.GetRefChi2() += chi2;
  track.GetRefNDF() += ndf;
}
