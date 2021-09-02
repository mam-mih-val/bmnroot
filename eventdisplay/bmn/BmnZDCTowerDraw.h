/**
 * @file BmnZDCTowerDraw.h
 * @author 
 * @brief BmnZDCTowerDraw header file
 * @version 1.0
 * @date 2021-08-13
 * 
 * @copyright 
 * 
 */

#ifndef __BMNZDCTOWERDRAW_H__
#define __BMNZDCTOWERDRAW_H__

#include "FairTask.h"
#include "MpdEventManager.h"
#include "TEvePointSet.h"
#include "TClonesArray.h"

class BmnZDCTowerDraw  : public FairTask
{
public:
    /** Default constructor */
    BmnZDCTowerDraw();

    /**
     * @brief Construct a new BmnZDCTowerDraw object
     * 
     * @param name Name of task
     * @param zdcMinEnergyThreshold Minimum energy threshold
     * @param iVerbose Verbosity level
     */
    BmnZDCTowerDraw(const char* name, Float_t zdcMinEnergyThreshold = 0, Int_t iVerbose = 0);

    /** Destructor */
    virtual ~BmnZDCTowerDraw();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option);
    virtual void Finish();
    void Reset();

protected:
    MpdEventManager* fEventManager; //!
    TClonesArray* fDigitList;       //!
    TEvePointSet* fq;               //!

    void DrawTowers();

private:
    Float_t fZDCMinEnergyThreshold; // Minimum energy threshold

    Float_t* fEneArr;               //! array of energies in each module of ZDC, GeV
    Float_t fMaxE;                  // maximum energy loss in the current event

    UInt_t fNumModules;             // number of modules in one ZDC detector
    Double_t fModuleZLen;           // z lenght of ZDC module, cm

    Bool_t fResetRequiredFlag;      // flag true is module sizes are adjusted

    BmnZDCTowerDraw(const BmnZDCTowerDraw&) = delete;
    BmnZDCTowerDraw& operator=(const BmnZDCTowerDraw&) = delete;

    ClassDef(BmnZDCTowerDraw, 1);
};


#endif // __BMNZDCTOWERDRAW_H__