/*
 Supplementary class to be used 
 * when drawing geometry boxes (modules, layer, dead zones)
 * of detectors pertaining to the BM@N inner tracker system
 Pavel Batyuk (pavel.batyuk@jinr.ru)
 28 February 2020
 */

#ifndef BMNINNGEOM_H
#define BMNINNGEOM_H 1

#include <TNamed.h>
#include <TSystem.h>
#include <TBox.h>
#include <TColor.h>
#include <TPolyLine.h>

#include <BmnGemStripStationSet.h>
#include <BmnSiliconStationSet.h>

using namespace std;

class BmnInnerTrackerGeometryDraw : public TNamed {
public:
    BmnInnerTrackerGeometryDraw();
    
    virtual ~BmnInnerTrackerGeometryDraw();
   
    BmnGemStripStationSet* GetGemGeometry() {
        return fGEM;
    }
    
    BmnSiliconStationSet* GetSiliconGeometry() {
        return fSILICON;
    }
    
    void GetGemBorders(TBox***& mods, TBox****& layers, TPolyLine****& deadZones) {
        mods = gemModBoxes;
        layers = gemLayBoxes;
        deadZones = gemDeadZones;
    }
    
    void GetSiliconBorders(TBox***& mods, TBox****& layers, TPolyLine****& deadZones) {
        mods = siliconModBoxes;
        layers = siliconLayBoxes;
        deadZones = siliconDeadZones;
    }
    
    void GetSiliconBorders(TBox***& mods) {
        mods = siliconModBoxes;
    }
       
private: 
    // Geometries 
    BmnGemStripStationSet* fGEM;
    BmnSiliconStationSet* fSILICON;

    // Borders of boxes and layers
    TBox*** gemModBoxes;
    TBox**** gemLayBoxes;
    TPolyLine**** gemDeadZones;
    
    TBox*** siliconModBoxes;
    TBox**** siliconLayBoxes;
    TPolyLine**** siliconDeadZones;
    
private: 
    void CreateBordersGem();
    void CreateBordersSilicon();
    
    ClassDef(BmnInnerTrackerGeometryDraw, 1)
};

#endif

