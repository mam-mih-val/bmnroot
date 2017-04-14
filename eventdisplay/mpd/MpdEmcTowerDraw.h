// -------------------------------------------------------------------------
// -----                      EmcTowerDraw header file                 -----
// -------------------------------------------------------------------------

#ifndef EMCTOWERDRAW_H
#define EMCTOWERDRAW_H

#include "FairTask.h"
#include "FairEventManager.h"
#include "TEvePointSet.h"               // for TEvePointSet
#include "MpdEmcGeoPar.h"

#include <TObject.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TMath.h>

using namespace TMath;

class MpdEmcTowerDraw : public FairTask
{
  public:
    /** Default constructor **/
    MpdEmcTowerDraw();

    /** Standard constructor
    *@param name        Name of task
    *@param emcMinEnergyThreshold minimal energy threshold 
    *@param verbose    Verbosity level   
    **/
    MpdEmcTowerDraw(const char* name, Double_t emcMinEnergyThreshold = 0, Int_t verbose = 1);

    /** Destructor **/
    virtual ~MpdEmcTowerDraw();
    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(UInt_t verboselvl) { fVerboselvl = verboselvl; }
    /** Executed task **/
    virtual void Exec(Option_t* option);
    void Reset();

  protected:    
    TClonesArray* fDigitList; //!
    FairEventManager* fEventManager; //! 
    TEvePointSet* fq;    //!
    
    virtual InitStatus Init();
    virtual void Finish(); ///< Action after each event
    TString FindBoxPathZPhiWithGeoManager(Double_t r, Double_t z, Double_t phi); ///< find path to box using Z and Phi with GeoManager
    TString FindBoxPathZPhi(Double_t z, Double_t phi); ///< find path to box using Z and Phi without GeoManager       
    UInt_t FindN3DbinBoxId(TString path); ///< find box id (in eneArr) corresponding to given path
    TString FindPathN3DbinBoxId(UInt_t id); ///< find path corresponding to given box id (in eneArr)  
    Double_t FindBoxesHeights(); ///<finds box height using GeoManager
    void FillEnergyLossArray(); ///<fills the fEneArr with values from fDigitList without normalizing
    void DrawBoxTowers(); ///< adjust box heights
    void ResetBoxTowers(); ///< reset all box sizes if "reco points" checkbox is now unchecked
    
    /** Accessors **/
    MpdEmcGeoPar* GetEmcGeoPar() { return fGeoPar; } ///< pointer to emc geo parameters
    Double_t GetEmcMinEnergyThreshold() const { return fEmcMinEnergyThreshold; }
    Bool_t GetResetRequiredFlag() const { return fResetRequiredFlag; }
    UInt_t GetVerboselvl() const { return fVerboselvl; }
    Double_t GetRMinEmc() const { return fRMinEmc; }
    Double_t GetRMaxEmc() const { return fRMaxEmc; }
    Double_t GetBoxHeight() const { return fBoxHeight; }
    Double_t GetSector1StartAngle() const { return fSector1StartAngle; }
    Double_t GetSector1EndAngle() const { return fSector1EndAngle; }
    UInt_t GetNBoxes() const { return fNBoxes; }
    UInt_t GetNTubes() const { return fNTubes; }
    UInt_t GetNSectors() const { return fNSectors; }
    UInt_t GetN3Dbins() const { return fN3Dbins; }
    UInt_t GetNRowsByPhi() const { return fNRowsByPhi; }
    UInt_t GetNRowsByZ() const { return fNRowsByZ; }
    UInt_t GetNRowInSuperModByPhi() const { return fNRowInSuperModByPhi; }
    UInt_t GetNRowInSuperModByZ() const { return fNRowInSuperModByZ; }
    UInt_t GetNMiddleBoxesInSuperMod() const { return fNMiddleBoxesInSuperMod; }
    Double_t GetLenSuperModule() const { return fLenSuperModule; }
    Double_t GetLenSector() const { return fLenSector; }
    Double_t GetEneArrValue(UInt_t i) const { return fEneArr[i]; }
    Double_t GetMaxE() const { return fMaxE; }  
    
    /** Modifiers **/
    void SetResetRequiredFlag(Bool_t resetRequiredFlag) { fResetRequiredFlag = resetRequiredFlag; }
    void SetRMinEmc(Double_t rMinEmc) { fRMinEmc = rMinEmc; }
    void SetRMaxEmc(Double_t rMaxEmc) { fRMaxEmc = rMaxEmc; }
    void SetBoxHeight(Double_t boxHeight) { fBoxHeight = boxHeight; }
    void SetSector1StartAngle(Double_t sector1StartAngle) { fSector1StartAngle = sector1StartAngle; }
    void SetSector1EndAngle(Double_t sector1EndAngle) { fSector1EndAngle = sector1EndAngle; }    
    void SetNBoxes(UInt_t nBoxes) { fNBoxes = nBoxes; }
    void SetNTubes(UInt_t nTubes) { fNTubes = nTubes; }
    void SetNSectors(UInt_t nSectors) { fNSectors = nSectors; }  
    void SetN3Dbins(UInt_t n3Dbins) { fN3Dbins = n3Dbins; }
    void SetNRowsByPhi(UInt_t nRowsByPhi) { fNRowsByPhi = nRowsByPhi; }
    void SetNRowsByZ(UInt_t nRowsByZ)  { fNRowsByZ = nRowsByZ; }
    void SetNRowInSuperModByPhi(UInt_t nRowInSuperModByPhi)  { fNRowInSuperModByPhi = nRowInSuperModByPhi; }
    void SetNRowInSuperModByZ(UInt_t nRowInSuperModByZ)  { fNRowInSuperModByZ = nRowInSuperModByZ; }
    void SetNMiddleBoxesInSuperMod(UInt_t nMiddleBoxesInSuperMod)  { fNMiddleBoxesInSuperMod = nMiddleBoxesInSuperMod; }
    void SetLenSuperModule(Double_t lenSuperModule){ fLenSuperModule = lenSuperModule; }
    void SetLenSector(Double_t lenSector){ fLenSector = lenSector; }
    void SetEneArr(UInt_t i, Double_t val) { fEneArr[i] = val; } ///< set energy loss array value
    void SetMaxE(Double_t maxE) { fMaxE = maxE; }

  private:   
    Double_t fEmcMinEnergyThreshold;///< min energy threshold
    Int_t fVerboselvl; ///< Verbosity level    
    Bool_t fResetRequiredFlag; ///< flag true is box sizes are adjusted
    Double_t fRMinEmc; ///< Inner radius, cm
    Double_t fRMaxEmc; ///< Outer radius -> module + front plastic and end plastic, cm
    Double_t fBoxHeight; ///< height of box or bt_box, cm
    Double_t fSector1StartAngle; ///< starting angle PHI1 of the first sector (emc1Sector_1), radians
    Double_t fSector1EndAngle; ///< ending angle PHI2 of the first sector (emc1Sector_1), radians
    UInt_t fNBoxes; ///< number of boxes (box + bt_box) in emc1Module (supermodule)    
    UInt_t fNTubes; ///< number of tubes in emcSector    
    UInt_t fNSectors; ///< number of sectors in emcChH
    UInt_t fN3Dbins; ///< total number of bins (depends on visibility level)
    UInt_t fNRowsByPhi; ///< number of emc1Module (supermodules) rows in sector by phi
    UInt_t fNRowsByZ; ///< number of emc1Module (supermodules) rows in sector along z axis
    UInt_t fNRowInSuperModByPhi; ///< number of box rows in emc1Module (supermodules) in sector by phi
    UInt_t fNRowInSuperModByZ; ///< number of box rows in emc1Module (supermodules) in sector along z axis
    UInt_t fNMiddleBoxesInSuperMod; ///< number of middle boxes (not bt_) in emc1Module
    Double_t fLenSuperModule; ///<length of emc1Module (supermodule) along z or x axis, cm
    Double_t fLenSector; ///<length of emc1Sector along z axis, cm
    Double_t* fEneArr; //!< array of energies in each box of EMC
    ///< boxes array id: (9 * 92 * 28 * (chHId-1)) + (9 * 92 * (sectorId-1)) + (9 * (tubeId-1)) + (boxId-1)
    Double_t fMaxE; ///< energy loss at the bin with maximum energy loss
    
    MpdEmcGeoPar* fGeoPar; ///< pointer to emc geo parameters
    
    MpdEmcTowerDraw(const MpdEmcTowerDraw&);
    MpdEmcTowerDraw& operator=(const MpdEmcTowerDraw&);

    ClassDef(MpdEmcTowerDraw,1);
};

#endif
