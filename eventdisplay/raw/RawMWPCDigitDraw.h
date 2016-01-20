// -------------------------------------------------------------------------
// -----                      RawMWPCDigitDraw header file             -----
// -----                                                               -----
// -------------------------------------------------------------------------


#ifndef RAWMWPCDIGITDRAW_H
#define RAWMWPCDIGITDRAW_H

#include "RawDataConverter.h"

#include "FairTask.h"
#include "FairEventManager.h"

#include "TEvePointSet.h"
#include "TObject.h"
#include "TVector3.h"

#include <vector>

class RawMWPCDigitDraw : public FairTask
{
  public:
    /** Default constructor **/
    RawMWPCDigitDraw();


    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    RawMWPCDigitDraw(const char* name, Color_t color ,Style_t mstyle, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~RawMWPCDigitDraw();

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }
    /** Executed task **/
    virtual void Exec(Option_t* option);
    void Reset();

    char* source_file_name;  //!

  protected:
    //TVector3 GetVector(TObject* obj);
    TObject* GetValue(TObject* obj,Int_t i);

    Int_t   fVerbose;       //  Verbosity level
    virtual void SetParContainers() ;
    virtual InitStatus Init();
    /** Action after each event**/
    virtual void Finish();

    FairEventManager* fEventManager;   //!
    TEvePointSet* fq;    //!
    Color_t fColor; //!
    Style_t fStyle; //!
    vector<EventData*>* pEventData; //!

  private:
    RawMWPCDigitDraw(const RawMWPCDigitDraw&);
    RawMWPCDigitDraw& operator=(const RawMWPCDigitDraw&);

    ClassDef(RawMWPCDigitDraw,1);
};

#endif
