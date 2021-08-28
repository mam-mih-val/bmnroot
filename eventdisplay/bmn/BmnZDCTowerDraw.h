/**
 * @file BmnZDCTowerDraw.h
 * @author 
 * @brief BmnZDCTowerDraw header file
 * @version 0.1
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
     * @brief Construct a new Bmn Zdc Tower Draw object
     * 
     * @param name Name of the task
     * @param zdcMinEnergyThreshold Minimum energy threshold
     * @param shadow 
     */
    BmnZDCTowerDraw(const char* name, Float_t zdcMinEnergyThreshold = 0, Bool_t shadow = kFALSE,  Int_t iVerbose = 0);

    /** Destructor */
    virtual ~BmnZDCTowerDraw();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option);
    virtual void Finish();
    void Reset();


protected:
    MpdEventManager* fEventManager;
    TClonesArray* fDigitList;
    TEvePointSet* fq;

    void DrawTowers();

private:
    Float_t fZDCMinEnergyThreshold; // Minimum energy threshold
    Bool_t fShadow;

    Float_t* fEneArr; //!
    Float_t fMaxE;

    UInt_t fNumModules; // number of modules in one ZDC detector
    Double_t fModuleZLen; // z lenght of ZDC module, cm

    Bool_t fResetRequiredFlag;

    BmnZDCTowerDraw(const BmnZDCTowerDraw&) = delete;
    BmnZDCTowerDraw& operator=(const BmnZDCTowerDraw&) = delete;

    ClassDef(BmnZDCTowerDraw, 1);
};


#endif // __BMNZDCTOWERDRAW_H__