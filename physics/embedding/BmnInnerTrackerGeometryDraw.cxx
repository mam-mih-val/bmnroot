#include "BmnInnerTrackerGeometryDraw.h"

BmnInnerTrackerGeometryDraw::BmnInnerTrackerGeometryDraw() {
    // 1. Initialize GEM and SILICON geometries to be used ...
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    fGEM = new BmnGemStripStationSet(gPathConfig + "/parameters/gem/XMLConfigs/GemRunSpring2018.xml");
    fSILICON = new BmnSiliconStationSet(gPathConfig + "/parameters/silicon/XMLConfigs/SiliconRunSpring2018.xml");

    // 2. Create box, layer and dead zone borders ...
    CreateBordersGem();
    CreateBordersSilicon();
}

BmnInnerTrackerGeometryDraw::~BmnInnerTrackerGeometryDraw() {
    if (fGEM)
        delete fGEM;
    
    if (fSILICON)
        delete fSILICON;
}

void BmnInnerTrackerGeometryDraw::CreateBordersGem() {
    const Int_t nMods = 2;
    const Int_t nLayers = 4;

    gemModBoxes = new TBox**[fGEM->GetNStations()];
    gemLayBoxes = new TBox***[fGEM->GetNStations()];
    gemDeadZones = new TPolyLine***[fGEM->GetNStations()];

    for (Int_t iStat = 0; iStat < fGEM->GetNStations(); iStat++) {
        gemModBoxes[iStat] = new TBox*[nMods];
        gemLayBoxes[iStat] = new TBox**[nMods];
        gemDeadZones[iStat] = new TPolyLine**[nMods];

        for (Int_t iMod = 0; iMod < fGEM->GetStation(iStat)->GetNModules(); iMod++) {
            BmnGemStripModule* mod = fGEM->GetStation(iStat)->GetModule(iMod);

            Double_t xmin = -mod->GetXMinModule();
            Double_t xmax = -mod->GetXMaxModule();
            Double_t ymin = mod->GetYMinModule();
            Double_t ymax = mod->GetYMaxModule();

            gemModBoxes[iStat][iMod] = new TBox(xmin, ymin, xmax, ymax);
            gemLayBoxes[iStat][iMod] = new TBox*[nLayers];
            gemDeadZones[iStat][iMod] = new TPolyLine*[nLayers];

            gemModBoxes[iStat][iMod]->SetLineColorAlpha(TColor::GetColor("#009900"), 0.3);
            gemModBoxes[iStat][iMod]->SetLineStyle(1);
            gemModBoxes[iStat][iMod]->SetLineWidth(1);

            gemModBoxes[iStat][iMod]->SetFillColorAlpha(TColor::GetColor("#cccccc"), 0.3);
            gemModBoxes[iStat][iMod]->SetFillStyle(0); // 0 - not filled, 1001 - solid

            for (Int_t iLayer = 0; iLayer < fGEM->GetStation(iStat)->GetModule(iMod)->GetNStripLayers(); iLayer++) {
                BmnGemStripLayer layer = fGEM->GetStation(iStat)->GetModule(iMod)->GetStripLayer(iLayer);

                // Get dead zones per each layer ...
                vector <DeadZoneOfStripLayer> DeadZones = layer.GetDeadZones();
                Int_t NDeadZones = DeadZones.size();

                gemDeadZones[iStat][iMod][iLayer] = new TPolyLine[NDeadZones];
                gemDeadZones[iStat][iMod][iLayer]->SetFillStyle(0); // 0 - not filled, 1001 - solid

                for (Int_t iZone = 0; iZone < NDeadZones; ++iZone) {
                    DeadZoneOfStripLayer dead_zone = DeadZones[iZone];

                    for (Int_t ipoint = 0; ipoint < dead_zone.GetNPoints(); ++ipoint) {
                        Double_t x = -dead_zone.GetXPoint(ipoint);
                        Double_t y = dead_zone.GetYPoint(ipoint);
                        gemDeadZones[iStat][iMod][iLayer][iZone].SetNextPoint(x, y);
                        if (ipoint == dead_zone.GetNPoints() - 1) {
                            gemDeadZones[iStat][iMod][iLayer][iZone].SetNextPoint(dead_zone.GetXPoint(0), dead_zone.GetYPoint(0));
                        }
                    }
                }

                Double_t xminL = -layer.GetXMinLayer();
                Double_t xmaxL = -layer.GetXMaxLayer();
                Double_t yminL = layer.GetYMinLayer();
                Double_t ymaxL = layer.GetYMaxLayer();

                gemLayBoxes[iStat][iMod][iLayer] = new TBox(xminL, yminL, xmaxL, ymaxL);
                gemLayBoxes[iStat][iMod][iLayer]->SetLineColorAlpha(TColor::GetColor("#009900"), 0.3);
                gemLayBoxes[iStat][iMod][iLayer]->SetLineStyle(1);
                gemLayBoxes[iStat][iMod][iLayer]->SetLineWidth(1);

                gemLayBoxes[iStat][iMod][iLayer]->SetFillColorAlpha(TColor::GetColor("#cccccc"), 0.3);
                gemLayBoxes[iStat][iMod][iLayer]->SetFillStyle(0); // 0 - not filled, 1001 - solid
            }
        }
    }
}

void BmnInnerTrackerGeometryDraw::CreateBordersSilicon() {
    const Int_t nMods = 8;
    const Int_t nLayers = 4;

    siliconModBoxes = new TBox**[fSILICON->GetNStations()];
    siliconLayBoxes = new TBox***[fSILICON->GetNStations()];
    siliconDeadZones = new TPolyLine***[fSILICON->GetNStations()];

    for (Int_t iStat = 0; iStat < fSILICON->GetNStations(); iStat++) {
        siliconModBoxes[iStat] = new TBox*[nMods];
        siliconLayBoxes[iStat] = new TBox**[nMods];
        siliconDeadZones[iStat] = new TPolyLine**[nMods];

        for (Int_t iMod = 0; iMod < fSILICON->GetStation(iStat)->GetNModules(); iMod++) {
            BmnSiliconModule* mod = fSILICON->GetStation(iStat)->GetModule(iMod);

            Double_t xmin = -mod->GetXMinModule();
            Double_t xmax = -mod->GetXMaxModule();
            Double_t ymin = mod->GetYMinModule();
            Double_t ymax = mod->GetYMaxModule();

            siliconModBoxes[iStat][iMod] = new TBox(xmin, ymin, xmax, ymax);
            siliconLayBoxes[iStat][iMod] = new TBox*[nLayers];
            siliconDeadZones[iStat][iMod] = new TPolyLine*[nLayers];

            siliconModBoxes[iStat][iMod] = new TBox(xmin, ymin, xmax, ymax);
            siliconModBoxes[iStat][iMod]->SetLineColorAlpha(TColor::GetColor("#009900"), 0.3);
            siliconModBoxes[iStat][iMod]->SetLineStyle(1);
            siliconModBoxes[iStat][iMod]->SetLineWidth(1);

            siliconModBoxes[iStat][iMod]->SetFillColorAlpha(TColor::GetColor("#cccccc"), 0.3);
            siliconModBoxes[iStat][iMod]->SetFillStyle(0); // 0 - not filled, 1001 - solid

            for (Int_t iLayer = 0; iLayer < fSILICON->GetStation(iStat)->GetModule(iMod)->GetNStripLayers(); iLayer++) {
                BmnSiliconLayer layer = fSILICON->GetStation(iStat)->GetModule(iMod)->GetStripLayer(iLayer);

                // Get dead zones per each layer ...
                vector <DeadZoneOfStripLayer> DeadZones = layer.GetDeadZones();
                Int_t NDeadZones = DeadZones.size();

                siliconDeadZones[iStat][iMod][iLayer] = new TPolyLine[NDeadZones];
                if (iStat == 2 && iMod == 7 && (iLayer == 0 || iLayer == 2))
                    siliconDeadZones[iStat][iMod][iLayer]->SetFillStyle(0); // 0 - not filled, 1001 - solid

                for (Int_t iZone = 0; iZone < NDeadZones; ++iZone) {
                    DeadZoneOfStripLayer dead_zone = DeadZones[iZone];

                    for (Int_t ipoint = 0; ipoint < dead_zone.GetNPoints(); ++ipoint) {
                        Double_t x = -dead_zone.GetXPoint(ipoint);
                        Double_t y = dead_zone.GetYPoint(ipoint);
                        siliconDeadZones[iStat][iMod][iLayer][iZone].SetNextPoint(x, y);
                        if (ipoint == dead_zone.GetNPoints() - 1) {
                            siliconDeadZones[iStat][iMod][iLayer][iZone].SetNextPoint(dead_zone.GetXPoint(0), dead_zone.GetYPoint(0));
                        }
                    }
                }

                Double_t xminL = -layer.GetXMinLayer();
                Double_t xmaxL = -layer.GetXMaxLayer();
                Double_t yminL = layer.GetYMinLayer();
                Double_t ymaxL = layer.GetYMaxLayer();

                siliconLayBoxes[iStat][iMod][iLayer] = new TBox(xminL, yminL, xmaxL, ymaxL);
                siliconLayBoxes[iStat][iMod][iLayer]->SetLineColorAlpha(TColor::GetColor("#009900"), 0.3);
                siliconLayBoxes[iStat][iMod][iLayer]->SetLineStyle(1);
                siliconLayBoxes[iStat][iMod][iLayer]->SetLineWidth(1);
                siliconLayBoxes[iStat][iMod][iLayer]->SetFillColorAlpha(TColor::GetColor("#cccccc"), 0.3);
                siliconLayBoxes[iStat][iMod][iLayer]->SetFillStyle(0); // 0 - not filled, 1001 - solid
            }
        }
    }
}


