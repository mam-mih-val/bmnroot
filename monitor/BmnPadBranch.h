#ifndef BMNPADBRANCH_H
#define BMNPADBRANCH_H

#include <stdlib.h>
#include <vector>

#include <PadInfo.h>

using namespace std;

/**
 * \class BmnPadBranch
 * \brief Node of pad tree
 */
class BmnPadBranch {
public:

    BmnPadBranch() {
        _info = nullptr;
        _NDivX = 1;
        _NDivY = 1;
    }

        virtual ~BmnPadBranch() {
        if (_info) {
            delete _info;
        }
        for (auto &el : _SubBranches) {
            delete el;
        }
    }

    PadInfo * GetPadInfo() {
        return _info;
    }

    vector < BmnPadBranch* > & GetBranchesRef() {
        return _SubBranches;
    }
    
    Int_t GetDivX(){
        return _NDivX;
    }
    Int_t GetDivY(){
        return _NDivY;
    }
    
    void SetDivXY(Int_t x, Int_t y){
        _NDivX = x;
        _NDivY = y;
    }

    void SetPadInfo(PadInfo *p) {
        _info = p;
    }

    void AddBranch(BmnPadBranch* b) {
        _SubBranches.push_back(b);
    }

private:
    PadInfo * _info;
    Int_t _NDivX;
    Int_t _NDivY;
    vector < BmnPadBranch* > _SubBranches;

};

#endif /* BMNPADBRANCH_H */

