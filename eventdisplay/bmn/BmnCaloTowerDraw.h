/**
 * @file BmnCaloTowerDraw.h
 * @author 
 * @brief BmnCaloTowerDraw header file
 * @date 2021-09-09
 * 
 */

#ifndef __BMNCALOTOWERDRAW_H__
#define __BMNCALOTOWERDRAW_H__

#include "FairTask.h"
#include "MpdEventManager.h"
#include "TEvePointSet.h"
#include "TClonesArray.h"

class BmnCaloTowerDraw  : public FairTask
{
public:
    /** Default constructor */
    BmnCaloTowerDraw();

    /**
     * @brief Construct a new BmnCaloTowerDraw object
     * 
     * @param name Name of task
     * @param caloType Type of calorimeter (0 - ZDC, 1 - ECAL)
     * @param caloMinEnergyThreshold Minimum energy threshold
     * @param iVerbose Verbosity level
     */
    BmnCaloTowerDraw(const char *name, Int_t caloType, Float_t caloMinEnergyThreshold = 0, Int_t iVerbose = 0);

    /** Destructor */
    virtual ~BmnCaloTowerDraw();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option);
    virtual void Finish();
    void Reset();

protected:
    MpdEventManager *fEventManager;     //!
    TClonesArray *fDigitList;           //!
    TEvePointSet *fq;                   //!

    void DrawTowers();

private:
    TString fCaloName;                  //!
    Float_t fCaloMinEnergyThreshold;    // Minimum energy threshold

    Float_t *fEneArr;                   //! array of energies in each module of calorimeter, GeV
    Float_t fMaxE;                      // maximum energy loss in the current event

    UInt_t fNumModules;                 // number of modules in one calorimeter detector
    Double_t *fModuleZLen;              //! z length of calorimeter modules, cm
    Double_t fMaxModuleZLen;
    string fGeoPath;                    //! path to calorimeter geometry

    Bool_t fResetRequiredFlag;          // flag true is module sizes are adjusted
    Bool_t fInitDrawFlag;               // flag false if unique TGeoVolume are created for each module

    BmnCaloTowerDraw(const BmnCaloTowerDraw&) = delete;
    BmnCaloTowerDraw& operator=(const BmnCaloTowerDraw&) = delete;

    ClassDef(BmnCaloTowerDraw, 1);
};

#endif // __BMNCALOTOWERDRAW_H__
