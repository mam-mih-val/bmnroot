#ifndef PADINFO_H
#define PADINFO_H

#include <string>

#include <TH1.h>
#include <TVirtualPad.h>

using std::string;
/**
 * \class PadInfo
 * \brief Storage for pad content and it's options
 */
class PadInfo : public TObject {
public:
    PadInfo() {
        temp = nullptr;
        current = nullptr;
        ref = nullptr;
        padPtr = nullptr;
    }

    ~PadInfo() {
        if (temp) delete temp;
        if (current) delete current;
        if (ref) delete ref;
        current = NULL;
        ref = NULL;
        for (auto &el : aux)
            if (el) delete el;
    }
    TH1* temp;
    TH1* current;
    TH1* ref;
    std::vector<TH1*> aux;
    TVirtualPad * padPtr;
    string opt;
    string name;
    string variable;
    string selection;
    
    
    TClass* classPtr;
private:
    ClassDef(PadInfo, 1)
};
ClassImp(PadInfo)


#endif /* PADINFO_H */

