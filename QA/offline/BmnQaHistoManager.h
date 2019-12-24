//#include <TObject.h>
#include <BmnHistManager.h>

#ifndef BMNQAHISTOMANAGER_H
#define BMNQAHISTOMANAGER_H 1

using namespace std;

class BmnQaHistoManager : public BmnHistManager {
public:

    /** Default constructor **/
    BmnQaHistoManager();

    /** Destructor **/
    virtual ~BmnQaHistoManager();

    // Getters
    TString GetListOfHistos() {
        return ToString();
    }
    
    // Setters

private:


    ClassDef(BmnQaHistoManager, 1);

};

#endif