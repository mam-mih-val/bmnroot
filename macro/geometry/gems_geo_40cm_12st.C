#if !defined(__CINT__) || defined(__MAKECINT__)
// MPD includes
// Fair includes
#include "FairGeoRotation.h"

// ROOT includes
#include <TROOT.h>
#include <TMath.h>
#include <TSystem.h>
#include <TVector3.h>

#include <Riostream.h>
#include <sstream>
#endif

#include "mpdshape.class.C"

void gems_geo_40cm_12st()
{
  // 14-may-2014 VV - GEM tracker geometry for BM@N (detector size 40 cm, 12 large stations )

  // Units are mm
  const Int_t nStations = 12; // Number of stations
  Double_t size = 2600.0; //820.0; // GEM station transverse size
  Double_t margin = 0.0; //150.0; // extra size to accomodate rotated stations
  Double_t hole = 13.5; // GEM station inner hole
  Double_t lengSts = 3850.0; //2000.0; // GEM system mother volume length along Z
  Double_t zMag = 1245.0; // magnet center Z
  Double_t zShift = -500.0; // station shift w.r.t. GEM system center
  Double_t thickStat = 130.0; // station thickness
  Double_t distStat = 100.0; // distance betwen stations
  Double_t dPhi = 0.0; //10.0; // rotation between stations
  Double_t sizeLad = 400.0; // module (ladder) transverse size (ladder)
  Double_t thickLad = 13.0; // module (ladder) thickness
  //Double_t thickLad = 1.0; // module (ladder) thickness - test !!!
  Double_t widthFrame = 12.0; // G10 frame width
  Double_t thickSens = 0.2; // sensor thickness
  Double_t dx = sizeLad / 2 + widthFrame; // module (ladder) X displacement
  Double_t dy = sizeLad / 2 + widthFrame; // module (ladder) Y displacement
  Double_t dz = 10.0; // module (ladder) Z displacement
  //Double_t dz = 1.0; // module (ladder) Z displacement - test !!!
  //Double_t sizeSensX[4] = {100.0, 200.0, 200.0, 100.0};
  //Double_t sizeSensY[4] = {100.0, 100.0, 200.0, 200.0};
  //Double_t sizeSensX[4] = {80.0, 220.0, 220.0,  80.0};
  //Double_t sizeSensY[4] = {80.0,  80.0, 220.0, 220.0};
  Double_t sizeSensX[4] = {80.0, 320.0, 320.0,  80.0};
  Double_t sizeSensY[4] = {80.0,  80.0, 320.0, 320.0};
  Double_t sizeSensX2[4] = {200.0, 200.0, 200.0, 200.0};
  Double_t sizeSensY2[4] = {200.0, 200.0, 200.0, 200.0};


  //   !---!------!
  //   !   !      !
  //   ! 4 !  3   !  Ladder
  //   !___!______!
  //   ! 1 !  2   !
  //   !-.-!--..--!

  //====== end declaration =======================================

  // output file
  const char* filename = "gem_bmn_40_VV.geo";
  ofstream* f = new ofstream(filename, ios::out | ios::trunc);
  // helper streams
  std::ostringstream points, position, rotation;
  points.setf(ios::showpoint); points.setf(ios::fixed);
  points << setprecision(6);
  position.setf(ios::showpoint); position.setf(ios::fixed);
  position << setprecision(6);
  rotation.setf(ios::showpoint); rotation.setf(ios::fixed);
  rotation << setprecision(6);

  points.str(""); position.str(""); rotation.str("");
 
  // GEM system mother volume
  zShift = zMag - lengSts/2 - 100; // to avoid overlapping with the target
  Mpdshape* gem = new Mpdshape(f, "STS1", "cave", "PGON", "air", points.str()); 
  //gem->Fill_PGON(-lengSts/2, lengSts/2, hole-0.1, size/2+0.1, 45.0, 4);
  gem->Fill_PGON(-lengSts/2, lengSts/2, hole-0.1, size/2+0.1+margin, 45.0, 4);  //+-130cm
  gem->SetPosition(0., 0., zMag - zShift);
  gem->SetSegment(0);
  gem->DumpWithIncrement();

  // Station 1
  Mpdshape* station = new Mpdshape(f, "STSStation01", "STS1", "PGON", "air", points.str());
  //station->Fill_PGON(-thickStat/2, thickStat/2, hole, size/2, 45.0, 4);
  station->Fill_PGON(-thickStat/2, thickStat/2, hole,3*sizeLad+2*widthFrame+hole, 45.0, 4);
  //Double_t zSt = -(nStations/2 + 0.5) * distStat + zShift;
  Double_t zSt = 300.0 - zMag + zShift; // 30 cm from the target
  station->SetPosition(0., 0., zSt);
  station->SetSegment(1);
  station->DumpWithIncrement();

  Double_t dxy[12][2] = {{hole, -hole-2*widthFrame}, {hole+2*widthFrame, hole},           //0,1
			 {-hole, hole+2*widthFrame}, {-hole-2*widthFrame, -hole},         //2,3
			 {hole+widthFrame+1.5*sizeLad, -hole-widthFrame+0.5*sizeLad},     //4
			 {-hole-widthFrame+1.5*sizeLad, -hole-widthFrame-0.5*sizeLad},    //5
			 {-hole-widthFrame+2.5*sizeLad, -hole-widthFrame-0.5*sizeLad},    //6
			 {hole+widthFrame+2.5*sizeLad, -hole-widthFrame+0.5*sizeLad},     //7
			 {-hole-widthFrame-1.5*sizeLad, hole+widthFrame-0.5*sizeLad},     //8
			 {hole+widthFrame-1.5*sizeLad, hole+widthFrame+0.5*sizeLad},      //9
			 {hole+widthFrame-2.5*sizeLad, hole+widthFrame+0.5*sizeLad},      //10		
			 {-hole-widthFrame-2.5*sizeLad, hole+widthFrame-0.5*sizeLad}};    //11
			
  //position of ladders: view from the target
  //  7  4  0  1  9  10
  //  6  5  3  2  8  11

  // Describe ladder
  Mpdshape* ladder = new Mpdshape(f, "STSStation01Ladder", "STSStation01", "BOX", "air", points.str());
  ladder->Fill_BOX(sizeLad/2+widthFrame, sizeLad/2+widthFrame, thickLad/2);  		
  ladder->SetPosition(dx+dxy[0][0], dy+dxy[0][1], dz);
  ladder->SetSegment(1);
  ladder->SetMotherSegment(1);
  ladder->DumpWithIncrement();

  //Mpdshape* ladder5 = new Mpdshape(f, "STSStation01Ladder", "STSStation01", "BOX", "air", points.str());
  Mpdshape* ladder5 = new Mpdshape(f, "STSStation01Ladder1", "STSStation01", "BOX", "air", points.str());
  ladder5->Fill_BOX(sizeLad/2+widthFrame, sizeLad/2+widthFrame, thickLad/2);  		
  //  ladder->SetPosition(dx+dxy[0][0], dy+dxy[0][1], dz);
  ladder5->SetSegment(1);
  ladder5->SetMotherSegment(1);
  //ladder->DumpWithIncrement();

  // Describe half-ladder (== ladder: auxiliary volume to be consistent with CBM)
  Mpdshape* hladder = new Mpdshape(f, "STSStation01Hladd", "STSStation01Ladder", "BOX", "air", points.str());
  hladder->Fill_BOX(sizeLad/2+widthFrame, sizeLad/2+widthFrame, thickLad/2);
  hladder->SetPosition(0.0, 0.0, 0.0);
  hladder->SetSegment(0);
  hladder->SetMotherSegment(1);
  hladder->DumpWithIncrement();
 
  //Mpdshape* hladder5 = new Mpdshape(f, "STSStation01Hladd", "STSStation01Ladder", "BOX", "air", points.str());
  Mpdshape* hladder5 = new Mpdshape(f, "STSStation01Hladd", "STSStation01Ladder1", "BOX", "air", points.str());
  hladder5->Fill_BOX(sizeLad/2+widthFrame, sizeLad/2+widthFrame, thickLad/2);
  hladder5->SetPosition(0.0, 0.0, 0.0);
  hladder5->SetSegment(1);
  hladder5->SetMotherSegment(1);

  // Ladder frame
  /*
  Mpdshape* frame = new Mpdshape(f, "STSStation01Frame", "STSStation01Hladd", "PGON", "G10", points.str());
  frame->Fill_PGON(-thickLad/2, thickLad/2, sizeLad/2, sizeLad/2+widthFrame, 45.0, 4);
  frame->SetPosition(0.0, 0.0, 0.0);
  frame->SetSegment(0);
  frame->SetMotherSegment(0);
  frame->DumpWithIncrement();   

  Mpdshape* frame5 = new Mpdshape(f, "STSStation01Frame", "STSStation01Hladd", "PGON", "G10", points.str());
  frame5->Fill_PGON(-thickLad/2, thickLad/2, sizeLad/2, sizeLad/2+widthFrame, 45.0, 4);
  frame5->SetPosition(0.0, 0.0, 0.0);
  frame5->SetSegment(0);
  frame5->SetMotherSegment(1);
  */

  // Describe modules and put them inside the half-ladder
  Mpdshape* modules[4];
  for (Int_t i = 0; i < 4; ++i) {
    TString name = "STSStation01Module";
    name += (i+1);
    modules[i] = new Mpdshape(f, name.Data(), "STSStation01Hladd", "BOX", "air", points.str());
    modules[i]->Fill_BOX(sizeSensX[i]/2-0.0, sizeSensY[i]/2-0.0, thickSens/2);
    Double_t x0 = sizeSensX[0] / 2, y0 = sizeSensY[0] / 2;
    if (i && i < 3) x0 = sizeSensX[1] / 2 + sizeSensX[0];
    if (i > 1) y0 = sizeSensY[2] / 2 + sizeSensY[0];
    modules[i]->SetPosition(x0-sizeLad/2, y0-sizeLad/2, 0.0);
    modules[i]->SetSegment(0);
    modules[i]->SetMotherSegment(0);
    modules[i]->DumpWithIncrement();
  }

  Mpdshape* modules5[4];
  for (Int_t i = 0; i < 4; ++i) {
    TString name = "STSStation01Module";
    name += (i+5);
    modules5[i] = new Mpdshape(f, name.Data(), "STSStation01Hladd", "BOX", "air", points.str());
    modules5[i]->Fill_BOX(sizeSensX2[i]/2-0.0, sizeSensY2[i]/2-0.0, thickSens/2);
    Double_t x0 = sizeSensX2[0] / 2, y0 = sizeSensY2[0] / 2;
    if (i && i < 3) x0 = sizeSensX2[1] / 2 + sizeSensX2[0];
    if (i > 1) y0 = sizeSensY2[2] / 2 + sizeSensY2[0];
    modules5[i]->SetPosition(x0-sizeLad/2, y0-sizeLad/2, 0.0);
    modules5[i]->SetSegment(0);
    modules5[i]->SetMotherSegment(1);
    //modules5[i]->DumpWithIncrement();
  }

  // Describe sensors and put them inside modules
  Mpdshape* sensors[4];
  for (Int_t i = 0; i < 4; ++i) {
    TString name = "STSStation01Sensor";
    name += (i+1);
    TString mname = "STSStation01Module";
    mname += (i+1);
    sensors[i] = new Mpdshape(f, name.Data(), mname.Data(), "BOX", "titanium", points.str());
    sensors[i]->Fill_BOX(sizeSensX[i]/2-0.2, sizeSensY[i]/2-0.2, thickSens/2);
    sensors[i]->SetPosition(0.0, 0.0, 0.0);
    sensors[i]->SetSegment(1);
    sensors[i]->SetMotherSegment(0);
    sensors[i]->DumpWithIncrement();
  }

  Mpdshape* sensors5[4];
  for (Int_t i = 0; i < 4; ++i) {
    TString name = "STSStation01Sensor";
    name += (i+5);
    TString mname = "STSStation01Module";
    mname += (i+5);
    sensors5[i] = new Mpdshape(f, name.Data(), mname.Data(), "BOX", "titanium", points.str());
    sensors5[i]->Fill_BOX(sizeSensX2[i]/2-0.2, sizeSensY2[i]/2-0.2, thickSens/2);
    sensors5[i]->SetPosition(0.0, 0.0, 0.0);
    sensors5[i]->SetSegment(1);
    sensors5[i]->SetMotherSegment(0);
    //sensors5[i]->DumpWithIncrement();
  }

  // Position ladder copies
 
  Double_t ddx = dx, ddy = dy, ddz = dz, dzExtra[4] = {-dz, -3*dz, -5*dz, -3*dz};

  for (Int_t lad = 1; lad < 12; ++lad) {
  //for (Int_t lad = 1; lad < 4; ++lad) {
    if (lad % 2) ddx *= -1;
    if (!(lad % 2)) ddy *= -1; 
    ddz *= -1;

    if (lad < 4) {
      ladder->SetPosition(ddx+dxy[lad][0], ddy+dxy[lad][1], ddz);
      ladder->SetRotation(90.0*lad, 0.0, 0.0);
    } else if (lad == 4) {
      ladder = ladder5;
      ladder->SetPosition(dxy[lad][0], dxy[lad][1], dzExtra[lad%4]);
      ladder->SetRotation(0.0, 0.0, 0.0);
      ladder->DumpWithIncrement();  
      hladder5->DumpWithIncrement();
      //frame5->DumpWithIncrement();
      for (Int_t i = 0; i < 4; ++i) modules5[i]->DumpWithIncrement();
      for (Int_t i = 0; i < 4; ++i) sensors5[i]->DumpWithIncrement();
      continue;
    } else {
      ladder->SetPosition(dxy[lad][0], dxy[lad][1], dzExtra[lad%4]);
    }
    ladder->DumpWithIncrement();  
  }
  /*
    for (Int_t lad = 5; lad < 6; ++lad) {     
      ddz *= -1;
      
      ladder->SetPosition(dxy[lad][0], dxy[lad][1], ddz);
      ladder->DumpWithIncrement();   
  }
  */

  // Position station copies
  //Double_t zPosSt[nStations] = {30, 45, 60, 80, 100, 130, 160, 200, 240, 285, 330, 375};
  Double_t zPosSt[nStations] = {30, 45, 60, 80, 100, 130, 160, 200, 240, 280, 320, 360};
  for (Int_t ist = 1; ist < nStations; ++ist) {
  //for (Int_t ist = 1; ist < nStations-2; ++ist) {
    distStat = (zPosSt[ist] - zPosSt[ist-1]) * 10.0;
    zSt += distStat;
    station->SetPosition(0., 0., zSt);
    station->SetRotation(dPhi*ist, 0.0, 0.0);
    station->DumpWithIncrement();
  }

   f->close(); 
}

  

