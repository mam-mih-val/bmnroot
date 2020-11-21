#ifndef BMNPADGENERATOR_H
#define BMNPADGENERATOR_H

#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <cstdio>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <TClass.h>
#include <TH2F.h>
#include <TH3S.h>
#include <TCanvas.h>

#include <BmnHistManager.h>

#include <BmnPadBranch.h>

using namespace std;
namespace pt = boost::property_tree;

/**
 * \class BmnPadGenerator
 * \brief Generates pad tree from the scheme
 */
class BmnPadGenerator {
public:

    BmnPadGenerator() {
        _HM = new BmnHistManager();
        _pad = nullptr;
    }

    virtual ~BmnPadGenerator() {
        if (_pad) delete _pad;
        if (_HM) delete _HM;
    }
    
    void LoadPTFrom(string FileName);
    BmnPadBranch* GeneratePadBranch(pt::ptree& PropTree);
    PadInfo* GeneratePadNode(pt::ptree& PropTree);
    TClass* GetClassByName(string ClassName);
    static void PadTree2Canvas(BmnPadBranch* br, TVirtualPad *pad);

    BmnPadBranch* GetPadBranch(){
        return _pad;
    }

private:
    BmnPadBranch* _pad;
    pt::ptree _TreeScheme;
    BmnHistManager* _HM;
//    map<string, >

};

#endif /* BMNPADGENERATOR_H */

