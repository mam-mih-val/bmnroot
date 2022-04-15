// @(#)bmnroot/mwpc:$Id$
// Author: Maria Patsyuk <mpatsyuk@mit.ru> 2018-02-10

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcGeometrySRC                                                         //
//                                                                            //
// A class containing geometry params. of                                     //
// the Multi Wire Prop. Chambers of the SRC at BM@N experiment                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include "BmnMwpcGeometrySRC.h"

BmnMwpcGeometrySRC::BmnMwpcGeometrySRC(Int_t periodNum, Int_t runNum) :
fDebug(kFALSE) {
  if(periodNum == 7 && runNum <= 3588){//SRC
    fNChambers = 4; 
    kCh_max    = 4;
    fGlobalZdiff = -647.476; // location of SRC Z=0 relative to BM@N Z=0
  }else if (periodNum == 6 || (periodNum == 7 && runNum > 3588) ){
    fNChambers = 2;
    kCh_max    = 2;
    fGlobalZdiff = -647.476; 
  }else if (periodNum == 8){//SRC
    fNChambers = 4; 
    kCh_max    = 4;
    fGlobalZdiff = -574.91; 
  }

  fNPlanes = 6;

  fX         = new Double_t [fNChambers];
  fY         = new Double_t [fNChambers];
  fZleft     = new Double_t [fNChambers];
  fZright    = new Double_t [fNChambers];
  fAngleX    = new Double_t [fNChambers];
  fAngleY    = new Double_t [fNChambers];
  fZPlanePos = new Double_t*[fNChambers];
  for(Int_t i=0; i<fNChambers; i++){
    fZPlanePos[i] = new Double_t[fNPlanes];
  }
  
  fTimeBin   = 8;
  fNWires    = 96;
  fAngleStep = 60 * DegToRad();
  fWireStep  = 0.25;
  fPlaneStep = 1.0015; // According to schemes provided by M. Rumyantsev

  fPlaneHeight = 43.3;
  fPlaneWidth = fNWires * fWireStep;
  
  fSpaceLeft  = 2.1338; // According to schemes provided by M. Rumyantsev
  fSpaceRight = 2.1328; // According to schemes given by M. Rumyantsev
  
  fChamberWidth = fSpaceLeft + fSpaceLeft + (fNPlanes - 1) * fPlaneStep;
  

  // MWPCs along the beam: 0, 1, 2, 3
  
  /*
    Reference frame is related to left side of the magnetic pole (z = 0, zC = 124.5 cm) 
    -----> Z -----> (MWPC -> GEM -> DCH)
    MWPC1                MWPC2
    
    fZleft[0] (fSpaceLeft | | | | | | fSpaceRight) fZright[0]  fZleft[1] (fSpaceLeft | | | | | | fSpaceRight) fZright[1]
                         1 2 3 4 5 6                                              1 2 3 4 5 6
  */

    if (periodNum == 6){
      fX[0] = 0.271; 
      fY[0] = 6.038; 
      fZright[0] = fGlobalZdiff + 287.858 -.4; 
      fZleft[0] = fZright[2] - fChamberWidth; 
      fX[1] = 0.234; 
      fY[1] = 6.140; 
      fZright[1] = fGlobalZdiff + 437.568 +1.1;  
      fZleft[1] = fZright[3] - fChamberWidth;

    }else if(periodNum == 7 && runNum > 001 && runNum <= 3588 ){//shift//cm //SRC
      
      fX[0] = -0.24  + 0.516;
      fY[0] = -3.342 + 0.44 -4.5;
      fZright[0] = fGlobalZdiff - 206.042;
      fZleft[0] = fZright[0] - fChamberWidth;
      
      fX[1] = -0.109 + 0.516;
      fY[1] = -3.601 + 0.44 -4.5;
      fZright[1] = fGlobalZdiff - 107.363;
      fZleft[1] = fZright[1] - fChamberWidth;
      
      fX[2] = 0.271 + 0.56;
      fY[2] = 6.038 + 0.1 -4.5;
      fZright[2] = fGlobalZdiff + 287.858;
      fZleft[2] = fZright[2] - fChamberWidth;
      
      fX[3] = 0.234 + 0.56;
      fY[3] = 6.140 + 0.1 -4.5;
      fZright[3] = fGlobalZdiff + 437.568;
      fZleft[3] = fZright[3] - fChamberWidth;
      
      }else if(periodNum == 8){//shift//cm
      
      fX[0] = .272;
      fY[0] = .160;
      fZleft[0]  = fGlobalZdiff - 249.222;
      fZright[0] = fZleft[0] - fChamberWidth;
      
      fX[1] = -.113;
      fY[1] =  .060;
      fZleft[1]  = fGlobalZdiff - 175.424;
      fZright[1] = fZleft[1] - fChamberWidth;
      
      fX[2] = .078;
      fY[2] = .564;
      fZright[2] = fGlobalZdiff + 220.431;
      fZleft[2]  = fZright[2] - fChamberWidth;
      
      fX[3] = -.030;
      fY[3] =  .390;
      fZright[3] = fGlobalZdiff + 346.541;
      fZleft[3]  = fZright[3] - fChamberWidth;
      
      }else if(periodNum == 7 && runNum == 0001){//MC //SRC
      
      fX[0] = -0.24  + 0.5;
      fY[0] = -3.342 - 4.5;
      fZright[0] = fGlobalZdiff - 206.042;
      fZleft[0] = fZright[0] - fChamberWidth;
      
      fX[1] = -0.109 + 0.5;
      fY[1] = -3.601 - 4.5;
      fZright[1] = fGlobalZdiff - 107.363;
      fZleft[1] = fZright[1] - fChamberWidth;
      
      fX[2] = 0.271 + 0.5;
      fY[2] = 6.038 - 4.5;
      fZright[2] = fGlobalZdiff + 287.858;
      fZleft[2] = fZright[2] - fChamberWidth;
      
      fX[3] = 0.234 + 0.5;
      fY[3] = 6.140 - 4.5;
      fZright[3] = fGlobalZdiff + 437.568;
      fZleft[3] = fZright[3] - fChamberWidth;

    }else if(periodNum == 7 && runNum > 3588){//BMN

      fX[0] = 0.271;//+ 1.09;
      fY[0] = 6.038;//-.95;
      fZright[0] = fGlobalZdiff + 287.858;
      fZleft[0] = fZright[0] - fChamberWidth;
      
      fX[1] = 0.234;//+ 1.34;
      fY[1] = 6.140;//- .87;
      fZright[1] = fGlobalZdiff + 437.568;
      fZleft[1] = fZright[1] - fChamberWidth; 
    }

 

    if(periodNum == 7 && runNum <= 3588){//SRC
      for(Int_t ichh = 0; ichh < 3; ichh++){
        for(int ii = 0; ii < fNPlanes; ii++){
          
          if ( ichh < 2){
            fZPlanePos[ichh][ii] = -0.5 + ii;
            if(ii == 4) { fZPlanePos[ichh][ii] = -2.5;}
            if(ii == 5) { fZPlanePos[ichh][ii] = -1.5;}
          }
          if( ichh == 2){
            fZPlanePos[ichh][ii] = -1.5 + ii;
            if(ii == 5) { fZPlanePos[ichh][ii] = -2.5;}
          }
        }
      }
      fZPlanePos[3][0] = -1.5;
      fZPlanePos[3][1] = -2.5;
      fZPlanePos[3][2] =  2.5;
      fZPlanePos[3][3] =  1.5;
      fZPlanePos[3][4] =  0.5;
      fZPlanePos[3][5] = -0.5;
    }
    if(periodNum == 6 || (periodNum == 7 && runNum > 3588) || periodNum == 8){
      for(Int_t ichh = 0; ichh < 2; ichh++){
        for(int ii = 0; ii < fNPlanes; ii++){
          
          fZPlanePos[ichh][ii] = -0.5 + ii;
          if(ii == 4) { fZPlanePos[ichh][ii] = -2.5;}
          if(ii == 5) { fZPlanePos[ichh][ii] = -1.5;}
            
        }
      }      
    }
    if(periodNum == 8){//?
      fZPlanePos[2][0] =  1.5;
      fZPlanePos[2][1] =  0.5;
      fZPlanePos[2][2] = -0.5;
      fZPlanePos[2][3] = -1.5;
      fZPlanePos[2][4] = -2.5;
      fZPlanePos[2][5] =  2.5;
        
      fZPlanePos[3][0] =  1.5;
      fZPlanePos[3][1] =  2.5;
      fZPlanePos[3][2] = -2.5;
      fZPlanePos[3][3] = -1.5;
      fZPlanePos[3][4] = -0.5;
      fZPlanePos[3][5] =  0.5;
    }

  // Check built geometry
  if (fDebug)
    for (Int_t iChamber = 0; iChamber < fNChambers; iChamber++) {
      cout << "MWPC" << iChamber + 1 << endl;
      cout << "Zleft = " << fZleft[iChamber] << " Zright = " << fZright[iChamber] << endl;
      cout << "OXprime : x = "<< endl;
      fOXprime[iChamber].Print() ;
      cout <<", y = "<< endl;
      fOYprime[iChamber].Print();
      cout<<", z = "<< endl;
      fOZprime[iChamber].Print();
      for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++)
        cout << "zPlanePos " << iPlane + 1 << " " << fZPlanePos[iChamber][iPlane] << endl;
      cout << endl;
    }
}

TVector3 BmnMwpcGeometrySRC::GetChamberCenter(Int_t chamber) {
  return TVector3(fX[chamber], fY[chamber], (fZright[chamber] + fZleft[chamber]) * 0.5);
}

TVector3 BmnMwpcGeometrySRC::GetAxisPrime(Int_t chamber, Int_t axis){
  if(axis == 0)  return fOXprime[chamber];
  if(axis == 1)  return fOYprime[chamber];
  if(axis == 2)  return fOZprime[chamber];

  return TVector3();
}
//shift tgs
Double_t BmnMwpcGeometrySRC::GetTx(Int_t chamber){
  TVector3 ox(1.,0.,0.);
  Double_t shiftX[4];
  //?
  shiftX[0] = -.033;
  shiftX[1] = -.048;
  shiftX[2] = -.033;
  shiftX[3] = -.035;
  

  return TMath::Tan(fOXprime[chamber].Angle(ox)) + shiftX[chamber];
}

Double_t BmnMwpcGeometrySRC::GetTy(Int_t chamber){
  TVector3 oy(0.,1.,0.);
  Double_t shiftY[4];
  
  shiftY[0] =  .003;
  shiftY[1] =  .008;
  shiftY[2] = -.009;
  shiftY[3] = -.014;
  

  return TMath::Tan(fOYprime[chamber].Angle(oy)) + shiftY[chamber];
}

BmnMwpcGeometrySRC::~BmnMwpcGeometrySRC() {
  delete[] fX;
  delete[] fY;
  delete[] fZleft;
  delete[] fZright;
  delete[] fAngleX;
  delete[] fAngleY;
  for(Int_t i=0; i<fNChambers; i++){
    delete[] fZPlanePos[i];
  }
  delete[] fZPlanePos;

}
