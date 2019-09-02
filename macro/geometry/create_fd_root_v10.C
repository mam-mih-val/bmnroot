#include <iomanip>
#include <iostream>
#include "TGeoManager.h"

#include "TGeoCompositeShape.h"
#include "TGeoCone.h"
#include "TGeoPara.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
//---------------------------
//---------------------------

TGeoManager *gGeoMan = NULL;

const Double_t SID_Xpos = 0.0;
const Double_t SID_Ypos = 0.0;
const Double_t SID_Zpos = 6.8;  //cm

const Double_t ZLength_SIDActiveVolume = 0.0525;

const Double_t dphia = 5.6;

const Double_t MaxRadiusOfActiveVolume = 8.6;
//const Double_t MaxRadiusOfActiveVolume = 40.;
const Double_t MinRadiusOfActiveVolume = 3.2;

const Double_t phia1OfActiveVolume = dphia * 16.;
const Double_t phib1OfActiveVolume = dphia * 17.;

const Double_t phia2OfActiveVolume = dphia * 17. + 0.025;
const Double_t phib2OfActiveVolume = dphia * 18. + 0.025;

const Double_t phia3OfActiveVolume = dphia * 18. + 0.025 * 2;
const Double_t phib3OfActiveVolume = dphia * 19. + 0.025 * 2;

const Double_t phia4OfActiveVolume = dphia * 19. + 0.025 * 3;
const Double_t phib4OfActiveVolume = dphia * 20. + 0.025 * 3;

const Double_t phia5OfActiveVolume = dphia * 20. + 0.025 * 4;
const Double_t phib5OfActiveVolume = dphia * 21. + 0.025 * 4;

const Double_t phia6OfActiveVolume = dphia * 21. + 0.025 * 5;
const Double_t phib6OfActiveVolume = dphia * 22. + 0.025 * 5;

const Double_t phia7OfActiveVolume = dphia * 22. + 0.025 * 6;
const Double_t phib7OfActiveVolume = dphia * 23. + 0.025 * 6;

const Double_t phia8OfActiveVolume = dphia * 23. + 0.025 * 7;
const Double_t phib8OfActiveVolume = dphia * 24. + 0.025 * 7;

const Double_t phia9OfActiveVolume = dphia * 24. + 0.025 * 8;
const Double_t phib9OfActiveVolume = dphia * 25. + 0.025 * 8;

const Double_t phia10OfActiveVolume = dphia * 25. + 0.025 * 9;
const Double_t phib10OfActiveVolume = dphia * 26. + 0.025 * 9;

const Double_t phia11OfActiveVolume = dphia * 26. + 0.025 * 10;
const Double_t phib11OfActiveVolume = dphia * 27. + 0.025 * 10;

const Double_t phia12OfActiveVolume = dphia * 27. + 0.025 * 11;
const Double_t phib12OfActiveVolume = dphia * 28. + 0.025 * 11;

const Double_t phia13OfActiveVolume = dphia * 28. + 0.025 * 12;
const Double_t phib13OfActiveVolume = dphia * 29. + 0.025 * 12;

const Double_t phia14OfActiveVolume = dphia * 29. + 0.025 * 13;
const Double_t phib14OfActiveVolume = dphia * 30. + 0.025 * 13;

const Double_t phia15OfActiveVolume = dphia * 30. + 0.025 * 14;
const Double_t phib15OfActiveVolume = dphia * 31. + 0.025 * 14;

const Double_t phia16OfActiveVolume = dphia * 31. + 0.025 * 15;
const Double_t phib16OfActiveVolume = dphia * 32. + 0.025 * 15;

const Double_t phia17OfActiveVolume = dphia * 32. + 0.025 * 16;
const Double_t phib17OfActiveVolume = dphia * 33. + 0.025 * 16;

const Double_t phia18OfActiveVolume = dphia * 33. + 0.025 * 17;
const Double_t phib18OfActiveVolume = dphia * 34. + 0.025 * 17;

const Double_t phia19OfActiveVolume = dphia * 34. + 0.025 * 18;
const Double_t phib19OfActiveVolume = dphia * 35. + 0.025 * 18;

const Double_t phia20OfActiveVolume = dphia * 35. + 0.025 * 19;
const Double_t phib20OfActiveVolume = dphia * 36. + 0.025 * 19;

const Double_t phia21OfActiveVolume = dphia * 36. + 0.025 * 20;
const Double_t phib21OfActiveVolume = dphia * 37. + 0.025 * 20;

const Double_t phia22OfActiveVolume = dphia * 37. + 0.025 * 21;
const Double_t phib22OfActiveVolume = dphia * 38. + 0.025 * 21;

const Double_t phia23OfActiveVolume = dphia * 38. + 0.025 * 22;
const Double_t phib23OfActiveVolume = dphia * 39. + 0.025 * 22;

const Double_t phia24OfActiveVolume = dphia * 39. + 0.025 * 23;
const Double_t phib24OfActiveVolume = dphia * 40. + 0.025 * 23;

const Double_t phia25OfActiveVolume = dphia * 40. + 0.025 * 24;
const Double_t phib25OfActiveVolume = dphia * 41. + 0.025 * 24;

const Double_t phia26OfActiveVolume = dphia * 41. + 0.025 * 25;
const Double_t phib26OfActiveVolume = dphia * 42. + 0.025 * 25;

const Double_t phia27OfActiveVolume = dphia * 42. + 0.025 * 26;
const Double_t phib27OfActiveVolume = dphia * 43. + 0.025 * 26;

const Double_t phia28OfActiveVolume = dphia * 43. + 0.025 * 27;
const Double_t phib28OfActiveVolume = dphia * 44. + 0.025 * 27;

const Double_t phia29OfActiveVolume = dphia * 44. + 0.025 * 28;
const Double_t phib29OfActiveVolume = dphia * 45. + 0.025 * 28;

const Double_t phia30OfActiveVolume = dphia * 45. + 0.025 * 29;
const Double_t phib30OfActiveVolume = dphia * 46. + 0.025 * 29;

const Double_t phia31OfActiveVolume = dphia * 46. + 0.025 * 30;
const Double_t phib31OfActiveVolume = dphia * 47. + 0.025 * 30;

const Double_t phia32OfActiveVolume = dphia * 47. + 0.025 * 31;
const Double_t phib32OfActiveVolume = dphia * 48. + 0.025 * 31;

const Double_t phia33OfActiveVolume = dphia * 48. + 0.025 * 32;
const Double_t phib33OfActiveVolume = dphia * 49. + 0.025 * 32;

const Double_t phia34OfActiveVolume = dphia * 49. + 0.025 * 33;
const Double_t phib34OfActiveVolume = dphia * 50. + 0.025 * 33;

const Double_t phia35OfActiveVolume = dphia * 50. + 0.025 * 34;
const Double_t phib35OfActiveVolume = dphia * 51. + 0.025 * 34;

const Double_t phia36OfActiveVolume = dphia * 51. + 0.025 * 35;
const Double_t phib36OfActiveVolume = dphia * 52. + 0.025 * 35;

const Double_t phia37OfActiveVolume = dphia * 52. + 0.025 * 36;
const Double_t phib37OfActiveVolume = dphia * 53. + 0.025 * 36;

const Double_t phia38OfActiveVolume = dphia * 53. + 0.025 * 37;
const Double_t phib38OfActiveVolume = dphia * 54. + 0.025 * 37;

const Double_t phia39OfActiveVolume = dphia * 54. + 0.025 * 38;
const Double_t phib39OfActiveVolume = dphia * 55. + 0.025 * 38;

const Double_t phia40OfActiveVolume = dphia * 55. + 0.025 * 39;
const Double_t phib40OfActiveVolume = dphia * 56. + 0.025 * 39;

const Double_t phia41OfActiveVolume = dphia * 56. + 0.025 * 40;
const Double_t phib41OfActiveVolume = dphia * 57. + 0.025 * 40;

const Double_t phia42OfActiveVolume = dphia * 57. + 0.025 * 41;
const Double_t phib42OfActiveVolume = dphia * 58. + 0.025 * 41;

const Double_t phia43OfActiveVolume = dphia * 58. + 0.025 * 42;
const Double_t phib43OfActiveVolume = dphia * 59. + 0.025 * 42;

const Double_t phia44OfActiveVolume = dphia * 59. + 0.025 * 43;
const Double_t phib44OfActiveVolume = dphia * 60. + 0.025 * 43;

const Double_t phia45OfActiveVolume = dphia * 60. + 0.025 * 44;
const Double_t phib45OfActiveVolume = dphia * 61. + 0.025 * 44;

const Double_t phia46OfActiveVolume = dphia * 61. + 0.025 * 45;
const Double_t phib46OfActiveVolume = dphia * 62. + 0.025 * 45;

const Double_t phia47OfActiveVolume = dphia * 62. + 0.025 * 46;
const Double_t phib47OfActiveVolume = dphia * 63. + 0.025 * 46;

const Double_t phia48OfActiveVolume = dphia * 63. + 0.025 * 47;
const Double_t phib48OfActiveVolume = dphia * 64. + 0.025 * 47;

const Double_t phia49OfActiveVolume = dphia * 64. + 0.025 * 48;
const Double_t phib49OfActiveVolume = dphia * 1. + 0.025 * 48;

const Double_t phia50OfActiveVolume = dphia * 1. + 0.025 * 49;
const Double_t phib50OfActiveVolume = dphia * 2. + 0.025 * 49;

const Double_t phia51OfActiveVolume = dphia * 2. + 0.025 * 50;
const Double_t phib51OfActiveVolume = dphia * 3. + 0.025 * 50;

const Double_t phia52OfActiveVolume = dphia * 3. + 0.025 * 51;
const Double_t phib52OfActiveVolume = dphia * 4. + 0.025 * 51;

const Double_t phia53OfActiveVolume = dphia * 4. + 0.025 * 52;
const Double_t phib53OfActiveVolume = dphia * 5. + 0.025 * 52;

const Double_t phia54OfActiveVolume = dphia * 5. + 0.025 * 53;
const Double_t phib54OfActiveVolume = dphia * 6. + 0.025 * 53;

const Double_t phia55OfActiveVolume = dphia * 6. + 0.025 * 54;
const Double_t phib55OfActiveVolume = dphia * 7. + 0.025 * 54;

const Double_t phia56OfActiveVolume = dphia * 7. + 0.025 * 55;
const Double_t phib56OfActiveVolume = dphia * 8. + 0.025 * 55;

const Double_t phia57OfActiveVolume = dphia * 8. + 0.025 * 56;
const Double_t phib57OfActiveVolume = dphia * 9. + 0.025 * 56;

const Double_t phia58OfActiveVolume = dphia * 9. + 0.025 * 57;
const Double_t phib58OfActiveVolume = dphia * 10. + 0.025 * 57;

const Double_t phia59OfActiveVolume = dphia * 10. + 0.025 * 58;
const Double_t phib59OfActiveVolume = dphia * 11. + 0.025 * 58;

const Double_t phia60OfActiveVolume = dphia * 11. + 0.025 * 59;
const Double_t phib60OfActiveVolume = dphia * 12. + 0.025 * 59;

const Double_t phia61OfActiveVolume = dphia * 12. + 0.025 * 60;
const Double_t phib61OfActiveVolume = dphia * 13. + 0.025 * 60;

const Double_t phia62OfActiveVolume = dphia * 13. + 0.025 * 61;
const Double_t phib62OfActiveVolume = dphia * 14. + 0.025 * 61;

const Double_t phia63OfActiveVolume = dphia * 14. + 0.025 * 62;
const Double_t phib63OfActiveVolume = dphia * 15. + 0.025 * 62;

const Double_t phia64OfActiveVolume = dphia * 15. + 0.025 * 63;
const Double_t phib64OfActiveVolume = dphia * 16. + 0.025 * 63;

void create_fd_root_v10() {
    // -------   Load media from media file   -----------------------------------
    FairGeoLoader *geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface *geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "FD";
    const TString geoDetectorVersion = "v10";
    TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia *geoMedia = geoFace->getMedia();
    FairGeoBuilder *geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium *mAir = geoMedia->getMedium("air");
    if (!mAir) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium *pMedAir = gGeoMan->GetMedium("air");
    if (!pMedAir) Fatal("Main", "Medium air not found");

    FairGeoMedium *msilicon = geoMedia->getMedium("silicon");
    if (!msilicon) Fatal("Main", "FairMedium silicon not found");
    geoBuild->createMedium(msilicon);
    TGeoMedium *pMedsilicon = gGeoMan->GetMedium("silicon");
    if (!pMedsilicon) Fatal("Main", "Medium silicon not found");

    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager *)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume *top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume *SIDTop = new TGeoVolumeAssembly(geoDetectorName);
    SIDTop->SetMedium(pMedAir);

    //Transformations (translations, rotations and scales)
    TGeoTranslation *SIDPos_trans = new TGeoTranslation("DetPos_trans", SID_Xpos, SID_Ypos, SID_Zpos);

    TGeoTubeSeg *SIDActiveVolumeS1 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia1OfActiveVolume, phib1OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS2 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia2OfActiveVolume, phib2OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS3 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia3OfActiveVolume, phib3OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS4 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia4OfActiveVolume, phib4OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS5 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia5OfActiveVolume, phib5OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS6 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia6OfActiveVolume, phib6OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS7 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia7OfActiveVolume, phib7OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS8 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia8OfActiveVolume, phib8OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS9 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia9OfActiveVolume, phib9OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS10 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia10OfActiveVolume, phib10OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS11 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia11OfActiveVolume, phib11OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS12 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia12OfActiveVolume, phib12OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS13 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia13OfActiveVolume, phib13OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS14 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia14OfActiveVolume, phib14OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS15 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia15OfActiveVolume, phib15OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS16 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia16OfActiveVolume, phib16OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS17 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia17OfActiveVolume, phib17OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS18 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia18OfActiveVolume, phib18OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS19 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia19OfActiveVolume, phib19OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS20 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia20OfActiveVolume, phib20OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS21 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia21OfActiveVolume, phib21OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS22 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia22OfActiveVolume, phib22OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS23 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia23OfActiveVolume, phib23OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS24 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia24OfActiveVolume, phib24OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS25 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia25OfActiveVolume, phib25OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS26 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia26OfActiveVolume, phib26OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS27 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia27OfActiveVolume, phib27OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS28 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia28OfActiveVolume, phib28OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS29 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia29OfActiveVolume, phib29OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS30 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia30OfActiveVolume, phib30OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS31 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia31OfActiveVolume, phib31OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS32 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia32OfActiveVolume, phib32OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS33 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia33OfActiveVolume, phib33OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS34 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia34OfActiveVolume, phib34OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS35 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia35OfActiveVolume, phib35OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS36 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia36OfActiveVolume, phib36OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS37 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia37OfActiveVolume, phib37OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS38 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia38OfActiveVolume, phib38OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS39 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia39OfActiveVolume, phib39OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS40 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia40OfActiveVolume, phib40OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS41 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia41OfActiveVolume, phib41OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS42 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia42OfActiveVolume, phib42OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS43 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia43OfActiveVolume, phib43OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS44 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia44OfActiveVolume, phib44OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS45 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia45OfActiveVolume, phib45OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS46 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia46OfActiveVolume, phib46OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS47 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia47OfActiveVolume, phib47OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS48 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia48OfActiveVolume, phib48OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS49 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia49OfActiveVolume, phib49OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS50 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia50OfActiveVolume, phib50OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS51 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia51OfActiveVolume, phib51OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS52 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia52OfActiveVolume, phib52OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS53 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia53OfActiveVolume, phib53OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS54 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia54OfActiveVolume, phib54OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS55 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia55OfActiveVolume, phib55OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS56 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia56OfActiveVolume, phib56OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS57 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia57OfActiveVolume, phib57OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS58 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia58OfActiveVolume, phib58OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS59 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia59OfActiveVolume, phib59OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS60 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia60OfActiveVolume, phib60OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS61 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia61OfActiveVolume, phib61OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS62 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia62OfActiveVolume, phib62OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS63 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia63OfActiveVolume, phib63OfActiveVolume);
    TGeoTubeSeg *SIDActiveVolumeS64 = new TGeoTubeSeg(MinRadiusOfActiveVolume / 2, MaxRadiusOfActiveVolume / 2, ZLength_SIDActiveVolume / 2, phia64OfActiveVolume, phib64OfActiveVolume);

    TGeoVolume *SIDActiveVolumeV1 = new TGeoVolume("SIDActiveVolumeV1", SIDActiveVolumeS1);
    SIDActiveVolumeV1->SetMedium(pMedsilicon);
    SIDActiveVolumeV1->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV2 = new TGeoVolume("SIDActiveVolumeV2", SIDActiveVolumeS2);
    SIDActiveVolumeV2->SetMedium(pMedsilicon);
    SIDActiveVolumeV2->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV3 = new TGeoVolume("SIDActiveVolumeV3", SIDActiveVolumeS3);
    SIDActiveVolumeV3->SetMedium(pMedsilicon);
    SIDActiveVolumeV3->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV4 = new TGeoVolume("SIDActiveVolumeV4", SIDActiveVolumeS4);
    SIDActiveVolumeV4->SetMedium(pMedsilicon);
    SIDActiveVolumeV4->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV5 = new TGeoVolume("SIDActiveVolumeV5", SIDActiveVolumeS5);
    SIDActiveVolumeV5->SetMedium(pMedsilicon);
    SIDActiveVolumeV5->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV6 = new TGeoVolume("SIDActiveVolumeV6", SIDActiveVolumeS6);
    SIDActiveVolumeV6->SetMedium(pMedsilicon);
    SIDActiveVolumeV6->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV7 = new TGeoVolume("SIDActiveVolumeV7", SIDActiveVolumeS7);
    SIDActiveVolumeV7->SetMedium(pMedsilicon);
    SIDActiveVolumeV7->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV8 = new TGeoVolume("SIDActiveVolumeV8", SIDActiveVolumeS8);
    SIDActiveVolumeV8->SetMedium(pMedsilicon);
    SIDActiveVolumeV8->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV9 = new TGeoVolume("SIDActiveVolumeV9", SIDActiveVolumeS9);
    SIDActiveVolumeV9->SetMedium(pMedsilicon);
    SIDActiveVolumeV9->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV10 = new TGeoVolume("SIDActiveVolumeV10", SIDActiveVolumeS10);
    SIDActiveVolumeV10->SetMedium(pMedsilicon);
    SIDActiveVolumeV10->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV11 = new TGeoVolume("SIDActiveVolumeV11", SIDActiveVolumeS11);
    SIDActiveVolumeV11->SetMedium(pMedsilicon);
    SIDActiveVolumeV11->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV12 = new TGeoVolume("SIDActiveVolumeV12", SIDActiveVolumeS12);
    SIDActiveVolumeV12->SetMedium(pMedsilicon);
    SIDActiveVolumeV12->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV13 = new TGeoVolume("SIDActiveVolumeV13", SIDActiveVolumeS13);
    SIDActiveVolumeV13->SetMedium(pMedsilicon);
    SIDActiveVolumeV13->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV14 = new TGeoVolume("SIDActiveVolumeV14", SIDActiveVolumeS14);
    SIDActiveVolumeV14->SetMedium(pMedsilicon);
    SIDActiveVolumeV14->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV15 = new TGeoVolume("SIDActiveVolumeV15", SIDActiveVolumeS15);
    SIDActiveVolumeV15->SetMedium(pMedsilicon);
    SIDActiveVolumeV15->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV16 = new TGeoVolume("SIDActiveVolumeV16", SIDActiveVolumeS16);
    SIDActiveVolumeV16->SetMedium(pMedsilicon);
    SIDActiveVolumeV16->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV17 = new TGeoVolume("SIDActiveVolumeV17", SIDActiveVolumeS17);
    SIDActiveVolumeV17->SetMedium(pMedsilicon);
    SIDActiveVolumeV17->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV18 = new TGeoVolume("SIDActiveVolumeV18", SIDActiveVolumeS18);
    SIDActiveVolumeV18->SetMedium(pMedsilicon);
    SIDActiveVolumeV18->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV19 = new TGeoVolume("SIDActiveVolumeV19", SIDActiveVolumeS19);
    SIDActiveVolumeV19->SetMedium(pMedsilicon);
    SIDActiveVolumeV19->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV20 = new TGeoVolume("SIDActiveVolumeV20", SIDActiveVolumeS20);
    SIDActiveVolumeV20->SetMedium(pMedsilicon);
    SIDActiveVolumeV20->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV21 = new TGeoVolume("SIDActiveVolumeV21", SIDActiveVolumeS21);
    SIDActiveVolumeV21->SetMedium(pMedsilicon);
    SIDActiveVolumeV21->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV22 = new TGeoVolume("SIDActiveVolumeV22", SIDActiveVolumeS22);
    SIDActiveVolumeV22->SetMedium(pMedsilicon);
    SIDActiveVolumeV22->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV23 = new TGeoVolume("SIDActiveVolumeV23", SIDActiveVolumeS23);
    SIDActiveVolumeV23->SetMedium(pMedsilicon);
    SIDActiveVolumeV23->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV24 = new TGeoVolume("SIDActiveVolumeV24", SIDActiveVolumeS24);
    SIDActiveVolumeV24->SetMedium(pMedsilicon);
    SIDActiveVolumeV24->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV25 = new TGeoVolume("SIDActiveVolumeV25", SIDActiveVolumeS25);
    SIDActiveVolumeV25->SetMedium(pMedsilicon);
    SIDActiveVolumeV25->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV26 = new TGeoVolume("SIDActiveVolumeV26", SIDActiveVolumeS26);
    SIDActiveVolumeV26->SetMedium(pMedsilicon);
    SIDActiveVolumeV26->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV27 = new TGeoVolume("SIDActiveVolumeV27", SIDActiveVolumeS27);
    SIDActiveVolumeV27->SetMedium(pMedsilicon);
    SIDActiveVolumeV27->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV28 = new TGeoVolume("SIDActiveVolumeV28", SIDActiveVolumeS28);
    SIDActiveVolumeV28->SetMedium(pMedsilicon);
    SIDActiveVolumeV28->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV29 = new TGeoVolume("SIDActiveVolumeV29", SIDActiveVolumeS29);
    SIDActiveVolumeV29->SetMedium(pMedsilicon);
    SIDActiveVolumeV29->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV30 = new TGeoVolume("SIDActiveVolumeV30", SIDActiveVolumeS30);
    SIDActiveVolumeV30->SetMedium(pMedsilicon);
    SIDActiveVolumeV30->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV31 = new TGeoVolume("SIDActiveVolumeV31", SIDActiveVolumeS31);
    SIDActiveVolumeV31->SetMedium(pMedsilicon);
    SIDActiveVolumeV31->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV32 = new TGeoVolume("SIDActiveVolumeV32", SIDActiveVolumeS32);
    SIDActiveVolumeV32->SetMedium(pMedsilicon);
    SIDActiveVolumeV32->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV33 = new TGeoVolume("SIDActiveVolumeV33", SIDActiveVolumeS33);
    SIDActiveVolumeV33->SetMedium(pMedsilicon);
    SIDActiveVolumeV33->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV34 = new TGeoVolume("SIDActiveVolumeV34", SIDActiveVolumeS34);
    SIDActiveVolumeV34->SetMedium(pMedsilicon);
    SIDActiveVolumeV34->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV35 = new TGeoVolume("SIDActiveVolumeV35", SIDActiveVolumeS35);
    SIDActiveVolumeV35->SetMedium(pMedsilicon);
    SIDActiveVolumeV35->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV36 = new TGeoVolume("SIDActiveVolumeV36", SIDActiveVolumeS36);
    SIDActiveVolumeV36->SetMedium(pMedsilicon);
    SIDActiveVolumeV36->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV37 = new TGeoVolume("SIDActiveVolumeV37", SIDActiveVolumeS37);
    SIDActiveVolumeV37->SetMedium(pMedsilicon);
    SIDActiveVolumeV37->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV38 = new TGeoVolume("SIDActiveVolumeV38", SIDActiveVolumeS38);
    SIDActiveVolumeV38->SetMedium(pMedsilicon);
    SIDActiveVolumeV38->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV39 = new TGeoVolume("SIDActiveVolumeV39", SIDActiveVolumeS39);
    SIDActiveVolumeV39->SetMedium(pMedsilicon);
    SIDActiveVolumeV39->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV40 = new TGeoVolume("SIDActiveVolumeV40", SIDActiveVolumeS40);
    SIDActiveVolumeV40->SetMedium(pMedsilicon);
    SIDActiveVolumeV40->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV41 = new TGeoVolume("SIDActiveVolumeV41", SIDActiveVolumeS41);
    SIDActiveVolumeV41->SetMedium(pMedsilicon);
    SIDActiveVolumeV41->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV42 = new TGeoVolume("SIDActiveVolumeV42", SIDActiveVolumeS42);
    SIDActiveVolumeV42->SetMedium(pMedsilicon);
    SIDActiveVolumeV42->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV43 = new TGeoVolume("SIDActiveVolumeV43", SIDActiveVolumeS43);
    SIDActiveVolumeV43->SetMedium(pMedsilicon);
    SIDActiveVolumeV43->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV44 = new TGeoVolume("SIDActiveVolumeV44", SIDActiveVolumeS44);
    SIDActiveVolumeV44->SetMedium(pMedsilicon);
    SIDActiveVolumeV44->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV45 = new TGeoVolume("SIDActiveVolumeV45", SIDActiveVolumeS45);
    SIDActiveVolumeV45->SetMedium(pMedsilicon);
    SIDActiveVolumeV45->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV46 = new TGeoVolume("SIDActiveVolumeV46", SIDActiveVolumeS46);
    SIDActiveVolumeV46->SetMedium(pMedsilicon);
    SIDActiveVolumeV46->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV47 = new TGeoVolume("SIDActiveVolumeV47", SIDActiveVolumeS47);
    SIDActiveVolumeV47->SetMedium(pMedsilicon);
    SIDActiveVolumeV47->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV48 = new TGeoVolume("SIDActiveVolumeV48", SIDActiveVolumeS48);
    SIDActiveVolumeV48->SetMedium(pMedsilicon);
    SIDActiveVolumeV48->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV49 = new TGeoVolume("SIDActiveVolumeV49", SIDActiveVolumeS49);
    SIDActiveVolumeV49->SetMedium(pMedsilicon);
    SIDActiveVolumeV49->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV50 = new TGeoVolume("SIDActiveVolumeV50", SIDActiveVolumeS50);
    SIDActiveVolumeV50->SetMedium(pMedsilicon);
    SIDActiveVolumeV50->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV51 = new TGeoVolume("SIDActiveVolumeV51", SIDActiveVolumeS51);
    SIDActiveVolumeV51->SetMedium(pMedsilicon);
    SIDActiveVolumeV51->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV52 = new TGeoVolume("SIDActiveVolumeV52", SIDActiveVolumeS52);
    SIDActiveVolumeV52->SetMedium(pMedsilicon);
    SIDActiveVolumeV52->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV53 = new TGeoVolume("SIDActiveVolumeV53", SIDActiveVolumeS53);
    SIDActiveVolumeV53->SetMedium(pMedsilicon);
    SIDActiveVolumeV53->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV54 = new TGeoVolume("SIDActiveVolumeV54", SIDActiveVolumeS54);
    SIDActiveVolumeV54->SetMedium(pMedsilicon);
    SIDActiveVolumeV54->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV55 = new TGeoVolume("SIDActiveVolumeV55", SIDActiveVolumeS55);
    SIDActiveVolumeV55->SetMedium(pMedsilicon);
    SIDActiveVolumeV55->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV56 = new TGeoVolume("SIDActiveVolumeV56", SIDActiveVolumeS56);
    SIDActiveVolumeV56->SetMedium(pMedsilicon);
    SIDActiveVolumeV56->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV57 = new TGeoVolume("SIDActiveVolumeV57", SIDActiveVolumeS57);
    SIDActiveVolumeV57->SetMedium(pMedsilicon);
    SIDActiveVolumeV57->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV58 = new TGeoVolume("SIDActiveVolumeV58", SIDActiveVolumeS58);
    SIDActiveVolumeV58->SetMedium(pMedsilicon);
    SIDActiveVolumeV58->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV59 = new TGeoVolume("SIDActiveVolumeV59", SIDActiveVolumeS59);
    SIDActiveVolumeV59->SetMedium(pMedsilicon);
    SIDActiveVolumeV59->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV60 = new TGeoVolume("SIDActiveVolumeV60", SIDActiveVolumeS60);
    SIDActiveVolumeV60->SetMedium(pMedsilicon);
    SIDActiveVolumeV60->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV61 = new TGeoVolume("SIDActiveVolumeV61", SIDActiveVolumeS61);
    SIDActiveVolumeV61->SetMedium(pMedsilicon);
    SIDActiveVolumeV61->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV62 = new TGeoVolume("SIDActiveVolumeV62", SIDActiveVolumeS62);
    SIDActiveVolumeV62->SetMedium(pMedsilicon);
    SIDActiveVolumeV62->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV63 = new TGeoVolume("SIDActiveVolumeV63", SIDActiveVolumeS63);
    SIDActiveVolumeV63->SetMedium(pMedsilicon);
    SIDActiveVolumeV63->SetLineColor(kRed);

    TGeoVolume *SIDActiveVolumeV64 = new TGeoVolume("SIDActiveVolumeV64", SIDActiveVolumeS64);
    SIDActiveVolumeV64->SetMedium(pMedsilicon);
    SIDActiveVolumeV64->SetLineColor(kRed);

    //Adding volumes to the TOP Volume
    top->AddNode(SIDTop, 1, SIDPos_trans);

    SIDTop->AddNode(SIDActiveVolumeV1, 1);
    SIDTop->AddNode(SIDActiveVolumeV2, 2);
    SIDTop->AddNode(SIDActiveVolumeV3, 3);
    SIDTop->AddNode(SIDActiveVolumeV4, 4);
    SIDTop->AddNode(SIDActiveVolumeV5, 5);
    SIDTop->AddNode(SIDActiveVolumeV6, 6);
    SIDTop->AddNode(SIDActiveVolumeV7, 7);
    SIDTop->AddNode(SIDActiveVolumeV8, 8);
    SIDTop->AddNode(SIDActiveVolumeV9, 9);
    SIDTop->AddNode(SIDActiveVolumeV10, 10);
    SIDTop->AddNode(SIDActiveVolumeV11, 11);
    SIDTop->AddNode(SIDActiveVolumeV12, 12);
    SIDTop->AddNode(SIDActiveVolumeV13, 13);
    SIDTop->AddNode(SIDActiveVolumeV14, 14);
    SIDTop->AddNode(SIDActiveVolumeV15, 15);
    SIDTop->AddNode(SIDActiveVolumeV16, 16);
    SIDTop->AddNode(SIDActiveVolumeV17, 17);
    SIDTop->AddNode(SIDActiveVolumeV18, 18);
    SIDTop->AddNode(SIDActiveVolumeV19, 19);
    SIDTop->AddNode(SIDActiveVolumeV20, 20);
    SIDTop->AddNode(SIDActiveVolumeV21, 21);
    SIDTop->AddNode(SIDActiveVolumeV22, 22);
    SIDTop->AddNode(SIDActiveVolumeV23, 23);
    SIDTop->AddNode(SIDActiveVolumeV24, 24);
    SIDTop->AddNode(SIDActiveVolumeV25, 25);
    SIDTop->AddNode(SIDActiveVolumeV26, 26);
    SIDTop->AddNode(SIDActiveVolumeV27, 27);
    SIDTop->AddNode(SIDActiveVolumeV28, 28);
    SIDTop->AddNode(SIDActiveVolumeV29, 29);
    SIDTop->AddNode(SIDActiveVolumeV30, 30);
    SIDTop->AddNode(SIDActiveVolumeV31, 31);
    SIDTop->AddNode(SIDActiveVolumeV32, 32);
    SIDTop->AddNode(SIDActiveVolumeV33, 33);
    SIDTop->AddNode(SIDActiveVolumeV34, 34);
    SIDTop->AddNode(SIDActiveVolumeV35, 35);
    SIDTop->AddNode(SIDActiveVolumeV36, 36);
    SIDTop->AddNode(SIDActiveVolumeV37, 37);
    SIDTop->AddNode(SIDActiveVolumeV38, 38);
    SIDTop->AddNode(SIDActiveVolumeV39, 39);
    SIDTop->AddNode(SIDActiveVolumeV40, 40);
    SIDTop->AddNode(SIDActiveVolumeV41, 41);
    SIDTop->AddNode(SIDActiveVolumeV42, 42);
    SIDTop->AddNode(SIDActiveVolumeV43, 43);
    SIDTop->AddNode(SIDActiveVolumeV44, 44);
    SIDTop->AddNode(SIDActiveVolumeV45, 45);
    SIDTop->AddNode(SIDActiveVolumeV46, 46);
    SIDTop->AddNode(SIDActiveVolumeV47, 47);
    SIDTop->AddNode(SIDActiveVolumeV48, 48);
    SIDTop->AddNode(SIDActiveVolumeV49, 49);
    SIDTop->AddNode(SIDActiveVolumeV50, 50);
    SIDTop->AddNode(SIDActiveVolumeV51, 51);
    SIDTop->AddNode(SIDActiveVolumeV52, 52);
    SIDTop->AddNode(SIDActiveVolumeV53, 53);
    SIDTop->AddNode(SIDActiveVolumeV54, 54);
    SIDTop->AddNode(SIDActiveVolumeV55, 55);
    SIDTop->AddNode(SIDActiveVolumeV56, 56);
    SIDTop->AddNode(SIDActiveVolumeV57, 57);
    SIDTop->AddNode(SIDActiveVolumeV58, 58);
    SIDTop->AddNode(SIDActiveVolumeV59, 59);
    SIDTop->AddNode(SIDActiveVolumeV60, 60);
    SIDTop->AddNode(SIDActiveVolumeV61, 61);
    SIDTop->AddNode(SIDActiveVolumeV62, 62);
    SIDTop->AddNode(SIDActiveVolumeV63, 63);
    SIDTop->AddNode(SIDActiveVolumeV64, 64);

    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile *geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
    // top->Draw("");
}
