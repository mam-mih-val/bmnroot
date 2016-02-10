/* This file was automatically created using gGeoManager->Export("magnet.C"),
followed by some manual adjustments: Preserve existing geometry manager, sup-
plement top assembly volume, add file output in the end.
The original source of this geometry had been written in GDML for GEANT and
transferred to ROOT using VGM. */
void magnet() {
   gSystem->Load("libGeom");
   TGeoManager* OldGeo=NULL;
   if (gROOT->GetListOfGlobals()->FindObject("gGeoManager"))
   { OldGeo=gGeoManager; gGeoManager=NULL;
     printf("Preserving existing gGeoManager.\n");
   }
   new TGeoManager("magnet geometry", "temporary geometry manager for magnet");

   Double_t dx,dy,dz;
   Double_t dx1, dx2, dy1, dy2;
   Double_t vert[20], par[20];
   Double_t theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2;
   Double_t twist;
   Double_t origin[3];
   Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
   Double_t r, rlo, rhi;
   Double_t phi1, phi2;
   Double_t a,b;
   Double_t point[3], norm[3];
   Double_t rin, stin, rout, stout;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t alpha, theta1, theta2, phi1, phi2, dphi;
   Double_t tr[3], rot[9];
   Double_t z, density, radl, absl, w;
   Double_t lx,ly,lz,tx,ty,tz;
   Double_t xvert[50], yvert[50];
   Double_t zsect,x0,y0,scale0;
   Int_t nel, numed, nz, nedges, nvert;
   TGeoBoolNode *pBoolNode = 0;

   // MATERIALS, MIXTURES AND TRACKING MEDIA
// Mixture: air
   nel     = 4;
   density = 0.001205;
   pMat6 = new TGeoMixture("air", nel,density);
      a = 12.010736;   z = 6.000000;   w = 0.000124;  // C
   pMat6->DefineElement(0,a,z,w);
      a = 14.006769;   z = 7.000000;   w = 0.755268;  // N
   pMat6->DefineElement(1,a,z,w);
      a = 15.999390;   z = 8.000000;   w = 0.231781;  // O
   pMat6->DefineElement(2,a,z,w);
      a = 39.947693;   z = 18.000000;   w = 0.012827;  // AR
   pMat6->DefineElement(3,a,z,w);
   pMat6->SetIndex(5);
// Medium: air
   numed   = 5;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed6 = new TGeoMedium("air", numed,pMat6, par);
// Mixture: steel
   nel     = 5;
   density = 8.020000;
   pMat5 = new TGeoMixture("steel", nel,density);
      a = 54.938000;   z = 25.000000;   w = 0.020000;  // MN
   pMat5->DefineElement(0,a,z,w);
      a = 28.085361;   z = 14.000000;   w = 0.010000;  // SI
   pMat5->DefineElement(1,a,z,w);
      a = 51.996130;   z = 24.000000;   w = 0.190000;  // CR
   pMat5->DefineElement(2,a,z,w);
      a = 58.693325;   z = 28.000000;   w = 0.100000;  // NI
   pMat5->DefineElement(3,a,z,w);
      a = 55.845111;   z = 26.000000;   w = 0.680000;  // FE
   pMat5->DefineElement(4,a,z,w);
   pMat5->SetIndex(4);
// Medium: steel
   numed   = 4;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed5 = new TGeoMedium("steel", numed,pMat5, par);
// Material: iron
   a       = 55.845111;
   z       = 26.000000;
   density = 7.874000;
   radl    = 1.756670;
   absl    = 16.958893;
   pMat1 = new TGeoMaterial("iron", a,z,density,radl,absl);
   pMat1->SetIndex(0);
// Medium: iron
   numed   = 0;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed1 = new TGeoMedium("iron", numed,pMat1, par);
// Material: copper
   a       = 63.545645;
   z       = 29.000000;
   density = 8.960000;
   radl    = 1.435156;
   absl    = 15.514134;
   pMat2 = new TGeoMaterial("copper", a,z,density,radl,absl);
   pMat2->SetIndex(1);
// Medium: copper
   numed   = 1;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed2 = new TGeoMedium("copper", numed,pMat2, par);

   // TRANSFORMATION MATRICES
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Combi transformation: 
   dx = 0.000000;
   dy = 85.500000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Combi transformation: 
   dx = 0.000000;
   dy = -85.500000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
   // Combi transformation: 
   dx = 79.600000;
   dy = -85.000000;
   dz = -122.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix47 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix46 = new TGeoCombiTrans("", dx,dy,dz,pMatrix47);
   // Combi transformation: 
   dx = -79.600000;
   dy = -85.000000;
   dz = -122.100000;
   // Rotation: 
   thx = 90.000000;    phx = 180.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix17 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,pMatrix17);
   // Combi transformation: 
   dx = 0.000000;
   dy = -85.000000;
   dz = -202.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix19 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,pMatrix19);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix49 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix48 = new TGeoCombiTrans("", dx,dy,dz,pMatrix49);
   // Combi transformation: 
   dx = -159.600000;
   dy = -85.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,pMatrix21);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix51 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix50 = new TGeoCombiTrans("", dx,dy,dz,pMatrix51);
   // Combi transformation: 
   dx = 79.600000;
   dy = -85.000000;
   dz = 122.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix23 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix22 = new TGeoCombiTrans("", dx,dy,dz,pMatrix23);
   // Combi transformation: 
   dx = -79.600000;
   dy = -85.000000;
   dz = 122.100000;
   // Rotation: 
   thx = 90.000000;    phx = 180.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix25 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix24 = new TGeoCombiTrans("", dx,dy,dz,pMatrix25);
   // Combi transformation: 
   dx = 0.000000;
   dy = -85.000000;
   dz = 202.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix27 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix26 = new TGeoCombiTrans("", dx,dy,dz,pMatrix27);
   // Combi transformation: 
   dx = 159.600000;
   dy = -85.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix29 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix28 = new TGeoCombiTrans("", dx,dy,dz,pMatrix29);
   // Combi transformation: 
   dx = 79.600000;
   dy = 85.000000;
   dz = -122.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix31 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix30 = new TGeoCombiTrans("", dx,dy,dz,pMatrix31);
   // Combi transformation: 
   dx = -79.600000;
   dy = 85.000000;
   dz = -122.100000;
   // Rotation: 
   thx = 90.000000;    phx = 180.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix33 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix32 = new TGeoCombiTrans("", dx,dy,dz,pMatrix33);
   // Combi transformation: 
   dx = 0.000000;
   dy = 85.000000;
   dz = -202.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix35 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix34 = new TGeoCombiTrans("", dx,dy,dz,pMatrix35);
   // Combi transformation: 
   dx = -159.600000;
   dy = 85.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix37 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix36 = new TGeoCombiTrans("", dx,dy,dz,pMatrix37);
   // Combi transformation: 
   dx = 79.600000;
   dy = 85.000000;
   dz = 122.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix39 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix38 = new TGeoCombiTrans("", dx,dy,dz,pMatrix39);
   // Combi transformation: 
   dx = -79.600000;
   dy = 85.000000;
   dz = 122.100000;
   // Rotation: 
   thx = 90.000000;    phx = 180.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix41 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix40 = new TGeoCombiTrans("", dx,dy,dz,pMatrix41);
   // Combi transformation: 
   dx = 0.000000;
   dy = 85.000000;
   dz = 202.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix43 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix42 = new TGeoCombiTrans("", dx,dy,dz,pMatrix43);
   // Combi transformation: 
   dx = 159.600000;
   dy = 85.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix45 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix44 = new TGeoCombiTrans("", dx,dy,dz,pMatrix45);
   // Shape: big_box type: TGeoBBox
   dx = 350.000000;
   dy = 224.000000;
   dz = 257.100000;
   TGeoTranslation *MagPos_trans = new TGeoTranslation("MagPos_trans", 0.0, 0.0,
	124.5);

   TGeoShape *pbig_box_1 = new TGeoBBox("big_box", dx,dy,dz);
   TGeoBBox *EmptyHoleS = new TGeoBBox("EmptyHoleS", 225.0, 120.0, 300.0);
   TGeoCompositeShape *ModifiedTopVolumeS = new
	TGeoCompositeShape("magnet_total", "big_box-EmptyHoleS");

   // Volume: magnet
   TGeoVolumeAssembly* top_assembly=new TGeoVolumeAssembly("top_magnet");
   pmagnet_1f48cf0 = new TGeoVolume("magnet",ModifiedTopVolumeS, pMed6);
   pmagnet_1f48cf0->SetVisLeaves(kTRUE);

   // SET TOP VOLUME OF GEOMETRY
   //gGeoManager->SetTopVolume(pmagnet_1f48cf0);
   gGeoManager->SetTopVolume(top_assembly);
   top_assembly->AddNode(pmagnet_1f48cf0, 0, MagPos_trans);

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: yoke_1 type: TGeoBBox
   dx = 350.000000;
   dy = 224.000000;
   dz = 130.000000;
   TGeoShape *pyoke_1_2 = new TGeoBBox("yoke_1", dx,dy,dz);
   // Shape: yoke_2 type: TGeoPara
   dx    = 433.138439;
   dy    = 375.108891;
   dz    = 130.050000;
   alpha = 30.000000;
   theta = 0.000000;
   phi   = 0.000000;
   TGeoShape *pyoke_2_3 = new TGeoPara("yoke_2",dx,dy,dz,alpha,theta,phi);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 60.000000;
   thy = 90.000000;    phy = 150.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   pBoolNode = new TGeoIntersection(pyoke_1_2,pyoke_2_3,0,pMatrix2);
   // Shape: yoke_12 type: TGeoCompositeShape
   TGeoShape *pyoke_12_4 = new TGeoCompositeShape("yoke_12", pBoolNode);
   // Shape: inner_space type: TGeoBBox
   dx = 230.000000;
   dy = 115.000000;
   dz = 130.050000;
   TGeoShape *pinner_space_5 = new TGeoBBox("inner_space", dx,dy,dz);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   pBoolNode = new TGeoSubtraction(pyoke_12_4,pinner_space_5,0,pMatrix4);
   // Shape: yoke_120 type: TGeoCompositeShape
   TGeoShape *pyoke_120_6 = new TGeoCompositeShape("yoke_120", pBoolNode);
   // Volume: iron_yoke
   piron_yoke_1f98e60 = new TGeoVolume("iron_yoke",pyoke_120_6, pMed5);
   piron_yoke_1f98e60->SetVisLeaves(kTRUE);
   pmagnet_1f48cf0->AddNode(piron_yoke_1f98e60, 0, pMatrix8);
   // Shape: pole_1 type: TGeoBBox
   dx = 84.000000;
   dy = 29.500000;
   dz = 124.500000;
   TGeoShape *ppole_1_7 = new TGeoBBox("pole_1", dx,dy,dz);
   // Shape: pole_2 type: TGeoBBox
   dx = 126.218560;
   dy = 29.550000;
   dz = 126.218560;
   TGeoShape *ppole_2_8 = new TGeoBBox("pole_2", dx,dy,dz);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 135.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 45.000000;    phz = 0.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
   pBoolNode = new TGeoIntersection(ppole_1_7,ppole_2_8,0,pMatrix6);
   // Shape: pole_12 type: TGeoCompositeShape
   TGeoShape *ppole_12_9 = new TGeoCompositeShape("pole_12", pBoolNode);
   // Volume: pole
   ppole_1fa6ca0 = new TGeoVolume("pole",ppole_12_9, pMed1);
   ppole_1fa6ca0->SetVisLeaves(kTRUE);
   pmagnet_1f48cf0->AddNode(ppole_1fa6ca0, 0, pMatrix10);
   pmagnet_1f48cf0->AddNode(ppole_1fa6ca0, 0, pMatrix12);
   // Shape: coil_container_curve type: TGeoTubeSeg
   rmin = 25.000000;
   rmax = 135.000000;
   dz   = 25.000000;
   phi1 = 0.000000;
   phi2 = 90.000000;
   TGeoShape *pcoil_container_curve_10 = new TGeoTubeSeg("coil_container_curve",rmin,rmax,dz,phi1,phi2);
   // Volume: coil_container_corner
   pcoil_container_corner_1fa6fb0 = new TGeoVolume("coil_container_corner",pcoil_container_curve_10, pMed5);
   pcoil_container_corner_1fa6fb0->SetVisLeaves(kTRUE);
   pmagnet_1f48cf0->AddNode(pcoil_container_corner_1fa6fb0, 0, pMatrix14);
   pmagnet_1f48cf0->AddNode(pcoil_container_corner_1fa6fb0, 0, pMatrix16);
   // Shape: coil_container_side_1 type: TGeoBBox
   dx = 79.600000;
   dy = 25.000000;
   dz = 55.000000;
   TGeoShape *pcoil_container_side_1_12 = new TGeoBBox("coil_container_side_1", dx,dy,dz);
   // Volume: coil_container_short
   pcoil_container_short_1fac210 = new TGeoVolume("coil_container_short",pcoil_container_side_1_12, pMed5);
   pcoil_container_short_1fac210->SetVisLeaves(kTRUE);
   pmagnet_1f48cf0->AddNode(pcoil_container_short_1fac210, 0, pMatrix18);
   // Shape: coil_container_side_2 type: TGeoBBox
   dx = 55.000000;
   dy = 25.000000;
   dz = 122.100000;
   TGeoShape *pcoil_container_side_2_14 = new TGeoBBox("coil_container_side_2", dx,dy,dz);
   // Volume: coil_container_long
   pcoil_container_long_1fac770 = new TGeoVolume("coil_container_long",pcoil_container_side_2_14, pMed5);
   pcoil_container_long_1fac770->SetVisLeaves(kTRUE);
   pmagnet_1f48cf0->AddNode(pcoil_container_long_1fac770, 0, pMatrix20);
   pmagnet_1f48cf0->AddNode(pcoil_container_corner_1fa6fb0, 0, pMatrix22);
   pmagnet_1f48cf0->AddNode(pcoil_container_corner_1fa6fb0, 0, pMatrix24);
   pmagnet_1f48cf0->AddNode(pcoil_container_short_1fac210, 0, pMatrix26);
   pmagnet_1f48cf0->AddNode(pcoil_container_long_1fac770, 0, pMatrix28);
   pmagnet_1f48cf0->AddNode(pcoil_container_corner_1fa6fb0, 0, pMatrix30);
   pmagnet_1f48cf0->AddNode(pcoil_container_corner_1fa6fb0, 0, pMatrix32);
   pmagnet_1f48cf0->AddNode(pcoil_container_short_1fac210, 0, pMatrix34);
   pmagnet_1f48cf0->AddNode(pcoil_container_long_1fac770, 0, pMatrix36);
   pmagnet_1f48cf0->AddNode(pcoil_container_corner_1fa6fb0, 0, pMatrix38);
   pmagnet_1f48cf0->AddNode(pcoil_container_corner_1fa6fb0, 0, pMatrix40);
   pmagnet_1f48cf0->AddNode(pcoil_container_short_1fac210, 0, pMatrix42);
   pmagnet_1f48cf0->AddNode(pcoil_container_long_1fac770, 0, pMatrix44);
   // Shape: coil_curve type: TGeoTubeSeg
   rmin = 25.200000;
   rmax = 134.800000;
   dz   = 24.800000;
   phi1 = 0.000000;
   phi2 = 90.000000;
   TGeoShape *pcoil_curve_11 = new TGeoTubeSeg("coil_curve",rmin,rmax,dz,phi1,phi2);
   // Volume: coil_corner
   pcoil_corner_1fa71c0 = new TGeoVolume("coil_corner",pcoil_curve_11, pMed2);
   pcoil_corner_1fa71c0->SetVisLeaves(kTRUE);
   pcoil_container_corner_1fa6fb0->AddNode(pcoil_corner_1fa71c0, 0, pMatrix46);
   // Shape: coil_side_1 type: TGeoBBox
   dx = 79.600000;
   dy = 24.800000;
   dz = 54.800000;
   TGeoShape *pcoil_side_1_13 = new TGeoBBox("coil_side_1", dx,dy,dz);
   // Volume: coil_short
   pcoil_short_1fac4a0 = new TGeoVolume("coil_short",pcoil_side_1_13, pMed2);
   pcoil_short_1fac4a0->SetVisLeaves(kTRUE);
   pcoil_container_short_1fac210->AddNode(pcoil_short_1fac4a0, 0, pMatrix48);
   // Shape: coil_side_2 type: TGeoBBox
   dx = 54.800000;
   dy = 24.800000;
   dz = 122.100000;
   TGeoShape *pcoil_side_2_15 = new TGeoBBox("coil_side_2", dx,dy,dz);
   // Volume: coil_long
   pcoil_long_1faca00 = new TGeoVolume("coil_long",pcoil_side_2_15, pMed2);
   pcoil_long_1faca00->SetVisLeaves(kTRUE);
   pcoil_container_long_1fac770->AddNode(pcoil_long_1faca00, 0, pMatrix50);

   // CLOSE GEOMETRY
   gGeoManager->CloseGeometry();
   TFile f("magnet.root","RECREATE");
   if (!f.IsZombie()) { top_assembly->Write();
		      f.Close();
		    }
   if (OldGeo)	{ delete gGeoManager;
		  printf("Restoring old gGeoManager.\n");
		  gGeoManager=OldGeo;
		}
}
