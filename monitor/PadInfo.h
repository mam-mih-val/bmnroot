#ifndef PADINFO_H
#define PADINFO_H

#include <TH1.h>
#include <TString.h>
#include <TVirtualPad.h>

/**
 * \class PadInfo
 * \brief Storage for pad content and it's options
 */
class PadInfo : public TObject {
public:
    PadInfo() {
        current = nullptr;
        ref = nullptr;
        padPtr = nullptr;
    }

    ~PadInfo() {
        if (current) delete current;
        if (ref) delete ref;
        current = NULL;
        ref = NULL;
        for (auto &el : aux)
            if (el) delete el;
    }
    TH1* current;
    TH1* ref;
    std::vector<TH1*> aux;
    TVirtualPad * padPtr;
    TString opt;
    TString name;
    
    
    TClass* classPtr;
private:
    ClassDef(PadInfo, 1)
};
ClassImp(PadInfo)


#endif /* PADINFO_H */

