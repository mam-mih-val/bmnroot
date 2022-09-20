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
    /** 
     * @brief Load pad tree from json config
     * @param FileName  path to json config
     *  */  
    void LoadPTFrom(string FileName);
    /**
     * @brief Generate pad branch tree from the boost property tree
     * @param PropTree boost property tree
     * @return pointer to the BmnPadBranch instance - start of the branch tree
     */
    BmnPadBranch* GeneratePadBranch(pt::ptree& PropTree);
    PadInfo* GeneratePadNode(pt::ptree& PropTree);
//    TClass* GetClassByName(string ClassName);
    /**
     * @brief Create pad structure in the canvas from the pad tree
     * and associate pad branch instances with the pad pointers
     * @param br  pad tree
     * @param pad  canvas or any other TVirtualPad instance
     */
    static void PadTree2Canvas(BmnPadBranch* br, TVirtualPad *pad);

    BmnPadBranch* GetPadBranch(){
        return _pad;
    }

private:
    BmnPadBranch* _pad; //!
    pt::ptree _TreeScheme; //!
    BmnHistManager* _HM; //!
//    map<string, >

};

#endif /* BMNPADGENERATOR_H */

