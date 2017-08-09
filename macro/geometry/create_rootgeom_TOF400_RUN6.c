//**************************************************************************************

void MakeBMNROOT_TOFv3()
{
    // Load the necessary FairRoot libraries 
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    // -------   Load media from media file   --------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;

    // -------   Geometry file name (output)   -------------------------------
    const TString geoDetectorName = "TOF400";
    const TString geoDetectorVersion = "RUN6";
    const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + ".root";

    // -----------------   Get and create the required media    --------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    // Air
    FairGeoMedium* matAir = geoMedia->getMedium("air");
    if (!matAir) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(matAir);
    TGeoMedium* medAir = gGeoMan->GetMedium("air");
    if (!medAir) Fatal("Main", "Medium air not found");

    // Polypropylene
    FairGeoMedium* matPprop = geoMedia->getMedium("polypropylene");
    if (!matPprop) Fatal("Main", "FairMedium polypropylene not found");
    geoBuild->createMedium(matPprop);
    TGeoMedium* medPprop = gGeoMan->GetMedium("polypropylene");
    if (!medPprop) Fatal("Main", "Medium polypropylene not found");

    // lead
    FairGeoMedium* matPb = geoMedia->getMedium("lead");
    if (!matPb) Fatal("Main", "FairMedium lead not found");
    geoBuild->createMedium(matPb);
    TGeoMedium* medPb = gGeoMan->GetMedium("lead");
    if (!medPb) Fatal("Main", "Medium lead not found");

    // aluminium
    FairGeoMedium* matAl = geoMedia->getMedium("aluminium");
    if (!matAl) Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(matAl);
    TGeoMedium* medAl = gGeoMan->GetMedium("aluminium");
    if (!medAl) Fatal("Main", "Medium aluminium not found");

    // RPCglass
    FairGeoMedium* matGlass = geoMedia->getMedium("RPCglass");
    if (!matGlass) Fatal("Main", "FairMedium RPCglass not found");
    geoBuild->createMedium(matGlass);
    TGeoMedium* medGlass = gGeoMan->GetMedium("RPCglass");
    if (!medGlass) Fatal("Main", "Medium RPCglass not found");

    // RPCgas
    FairGeoMedium* matGas = geoMedia->getMedium("RPCgas");
    if (!matGas) Fatal("Main", "FairMedium RPCgas not found");
    geoBuild->createMedium(matGas);
    TGeoMedium* medGas = gGeoMan->GetMedium("RPCgas");
    if (!medGas) Fatal("Main", "Medium RPCgas not found");

    // G10
    FairGeoMedium* matG10 = geoMedia->getMedium("G10");
    if (!matG10) Fatal("Main", "FairMedium G10 not found");
    geoBuild->createMedium(matG10);
    TGeoMedium* medG10 = gGeoMan->GetMedium("G10");
    if (!medG10) Fatal("Main", "Medium G10 not found");

    // -----------------------------------------   Create geometry and top volume  -----------------------------------------------
    gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP"); // TOP VOLUME
    top->SetMedium(medAir);
    gGeoMan->SetTopVolume(top);
    gGeoMan->SetTopVisible(1);
    gGeoMan->SetVisLevel(7);

    // ---------------------------------------  Define TOP Geometry ------------------------------------
    TGeoVolume* topTof = new TGeoVolumeAssembly(geoDetectorName);
    topTof->SetMedium(medAir);

    TGeoVolume* vDetector = new TGeoVolumeAssembly("tof1Detector");
    vDetector->SetMedium(medAir);

    const double DetXsize = 60.; // [cm]		
    const double DetYsize = 30.; // [cm]		
    const double DetZsize = 3.0; // [cm]			
    // --------------------- Create strip ------------------ 	
    const float stripGasX = 1., stripGasY = DetYsize, stripGasZ = 0.3; // [cm]
    TGeoBBox *sStripGas = new TGeoBBox(stripGasX / 2., stripGasY / 2., stripGasZ / 2.);
    sStripGas->SetName("StripGas");
    TGeoVolume *vStripGas = new TGeoVolume("tof1StripActiveGasV", sStripGas, medGas);
    vStripGas->SetLineColor(kRed);


    // --------------------- Create Glass plates ------------------    
    const float detGlassX = DetXsize, detGlassY = DetYsize, detGlassZ = 0.618 / 2.; // [cm]  6.18 mm / 2 plate
    TGeoBBox *sDetGlass = new TGeoBBox(detGlassX / 2., detGlassY / 2., detGlassZ / 2.);
    sDetGlass->SetName("DetectorGlass");
    TGeoVolume *vDetGlass = new TGeoVolume("tof1DetGlassV", sDetGlass, medGlass);
    vDetGlass->SetLineColor(kYellow);

    // --------------------- Create G10 plates ------------------ 
    const float detPlateX = DetXsize, detPlateY = DetYsize, detPlateZ = 0.64 / 2.; // [cm]  6.4 mm / 2 plate	
    TGeoBBox *sDetPlate = new TGeoBBox(detPlateX / 2., detPlateY / 2., detPlateZ / 2.);
    sDetPlate->SetName("DetectorPlate");
    TGeoVolume *vDetPlate = new TGeoVolume("tof1DetPlateV", sDetPlate, medG10);
    vDetPlate->SetLineColor(kBlue);

    // --------------------------------------- Assemble detector

    // Add strips		
    const int NStrips = 48; //	24 strips group	
    const double StripStep = 1.25; // [cm] 
    for (int i = 0; i < NStrips; i++)
    {
        vDetector->AddNode(vStripGas, i + 1, new TGeoTranslation("", -DetXsize / 2. + StripStep / 2. + i * StripStep, 0., 0.));
    }

    // Add  Glass plates
    vDetector->AddNode(vDetGlass, 1, new TGeoTranslation("", 0., 0., -stripGasZ / 2. - detGlassZ / 2.));
    vDetector->AddNode(vDetGlass, 2, new TGeoTranslation("", 0., 0., +stripGasZ / 2. + detGlassZ / 2.));

    // Add  G10 plates
    vDetector->AddNode(vDetPlate, 1, new TGeoTranslation("", 0., 0., -DetZsize / 2. + detPlateZ / 2.));
    vDetector->AddNode(vDetPlate, 2, new TGeoTranslation("", 0., 0., +DetZsize / 2. - detPlateZ / 2.));

    // -------------------------------------- Install Detectors
    const double tofZgap = 0.86; // 8.6 mm between detectors
    const double tofYoverlap = 3.0; // 30 mm detectors overlap at X direction

    const double tofXgap = 0.; // [cm] RUN5, part1
    //const double tofXgap = 2. * 24.4; // [cm] RUN5, part2
    const double tofZ_1box = 454.8.; // [cm] :
    const double tofZ_2box = 442.6.; // [cm] :

    double tofX = tofXgap / 2. + DetXsize / 2.;
    double tofZshift = tofZgap + DetZsize;

    // first box
    topTof->AddNode(vDetector, 1, new TGeoTranslation("t1", +tofX+55.2+27.17, -3.48 + 2 * DetYsize - 2 * tofYoverlap, tofZ_1box + tofZshift)); // right side, x > 0
    topTof->AddNode(vDetector, 2, new TGeoTranslation("t1", +tofX+55.2+27.17, -3.48 + DetYsize - tofYoverlap, tofZ_1box));
    topTof->AddNode(vDetector, 3, new TGeoTranslation("t1", +tofX+55.2+27.17, -3.48, tofZ_1box + tofZshift));
    topTof->AddNode(vDetector, 4, new TGeoTranslation("t1", +tofX+55.2+27.17, -3.48 - DetYsize + tofYoverlap, tofZ_1box));
    topTof->AddNode(vDetector, 5, new TGeoTranslation("t1", +tofX+55.2+27.17, -3.48 - 2 * DetYsize + 2 * tofYoverlap, tofZ_1box + tofZshift));

    //second box
    topTof->AddNode(vDetector, 6, new TGeoTranslation("t1", +tofX+32.96, -3.48 + 2 * DetYsize - 2 * tofYoverlap, tofZ_2box + tofZshift)); // right side, x > 0
    topTof->AddNode(vDetector, 7, new TGeoTranslation("t1", +tofX+32.96, -3.48 + DetYsize - tofYoverlap, tofZ_2box));
    topTof->AddNode(vDetector, 8, new TGeoTranslation("t1", +tofX+32.96, -3.48, tofZ_2box + tofZshift));
    topTof->AddNode(vDetector, 9, new TGeoTranslation("t1", +tofX+32.96, -3.48 - DetYsize + tofYoverlap, tofZ_2box));
    topTof->AddNode(vDetector, 10, new TGeoTranslation("t1", +tofX+32.96, -3.48 - 2 * DetYsize + 2 * tofYoverlap, tofZ_2box + tofZshift));

    //topTof->AddNode(vDetector, 4, new TGeoTranslation("t1",  -tofX,  0., tofZ + tofZshift)); // left side, x < 0
    //topTof->AddNode(vDetector, 5, new TGeoTranslation("t1",  -tofX,  +DetYsize -tofYoverlap, tofZ - tofZshift));  
    //topTof->AddNode(vDetector, 6, new TGeoTranslation("t1",  -tofX,  -DetYsize +tofYoverlap, tofZ - tofZshift));    


    // ------------------  Adding  the TOP Volume --------------------------
    top->AddNode(topTof, 1, new TGeoTranslation("tw", 0., 0., 0.));

    // ---------------   Finish   --------------------------------------------
    top->SetVisContainers(kTRUE);

    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    top->Draw("ogl");
    gGeoMan->Browse(new TBrowser);
}
