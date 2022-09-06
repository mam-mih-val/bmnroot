#ifndef BMN_ONLINE_TASK_H
#define BMN_ONLINE_TASK_H

#include <FairField.h>
#include <FairTask.h>

#include <TTree.h>

/**
 * @brief BmnTask
 *
 */
class BmnTask : public FairTask {
  public:
    /** Default constructor **/
    BmnTask();

    /** Standard constructor
     *@param name Name of task
     *@param iVerbose Verbosity level
     **/
    BmnTask(const char *name, Int_t iVerbose = 1);

    /**
     * @brief Destroy the BmnTask object
     *
     */
    virtual ~BmnTask();

    /**
     * @brief Online task initialization
     *
     * @return InitStatus
     */
    virtual InitStatus OnlineInit() { return kSUCCESS; }

    /**
     * @brief Read data from tree
     *
     * @param dataTree tree with input data
     * @param resultTree tree with the result of previously executed tasks
     * @return InitStatus
     */
    virtual InitStatus OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree) {
        return kSUCCESS;
    }

    /**
     * @brief Write task resul to tree
     *
     * @param dataTree
     */
    virtual void OnlineWrite(const std::unique_ptr<TTree> &dataTree) {}

    /**
     * @brief Set the Field object
     *
     * @param magneticField
     */
    virtual void SetField(const std::unique_ptr<FairField> &magneticField) {}

    /**
     * @brief Get the Online Active status
     *
     * @return Bool_t
     */
    Bool_t IsOnlineActive() { return fOnlineActive; }

    /**
     * @brief Set the Online Active status
     *
     * @param onlineActive
     */
    void SetOnlineActive(Bool_t onlineActive = kTRUE) { fOnlineActive = onlineActive; }

    BmnTask(const BmnTask &) = delete;
    BmnTask &operator=(const BmnTask &) = delete;

  private:
    Bool_t fOnlineActive;

  ClassDef(BmnTask, 1);
};

#endif  // BMN_ONLINE_TASK_H
