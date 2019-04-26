/********************************************************************************
 *    BmnFillDstTask.h                                                          *
 *    BM@N Fill DST Task class declaration                                      *
 *    Fill Event Headers in the DST file after reconstruction                   *
 *    Konstantin Gertsenberger                                                  *
 *    Created: Apr. 25 2019                                                     *
 *******************************************************************************/

#ifndef BMNFILLDSTTASK_H
#define BMNFILLDSTTASK_H

#include "BmnEventHeader.h"
#include "DstEventHeader.h"

#include "FairTask.h"
#include "FairMCEventHeader.h"

#include "TClonesArray.h"

class BmnFillDstTask : public FairTask
{
  public:
    /** Default constructor **/
    BmnFillDstTask();

    /** Constructor with the given event number to be processed
     ** in order to activate printing only progress bar in terminal **/
    BmnFillDstTask(Long64_t nEvents);

    /** Destructor **/
    ~BmnFillDstTask();


    /** Initiliazation of task at the beginning **/
    virtual InitStatus Init();

    /** ReInitiliazation of task when the runID/file changes **/
    virtual InitStatus ReInit();


    /** Executed for each event **/
    virtual void Exec(Option_t* opt);

    /** Load the parameter container from the runtime database **/
    virtual void SetParContainers();

    /** Finish task called at the end **/
    virtual void Finish();

  private:
    /** Input MCEventHeader from Simulation File **/
    FairMCEventHeader* fMCEventHead;
    /** Input BmnEventHeader from Digit File **/
    BmnEventHeader* fEventHead;
    /** whether input file contains simulation data **/
    Bool_t isSimulationInput;
    /** Output DstEventHeader prepared in FairRunAna **/
    DstEventHeader* fDstHead;

    /** event count to be processed for progress bar **/
    Long64_t fNEvents;
    /** current event being processed for progress bar **/
    Long64_t fIEvent;

    BmnFillDstTask(const BmnFillDstTask&);
    BmnFillDstTask operator=(const BmnFillDstTask&);

    ClassDef(BmnFillDstTask,1);
};

#endif
