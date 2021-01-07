
#include "BmnPadGenerator.h"

void BmnPadGenerator::LoadPTFrom(string FileName) {
    pt::read_json(FileName, _TreeScheme);
    _pad = GeneratePadBranch(_TreeScheme);
    return;
}

//BmnPadGenerator::GeneratePadTree(pt::ptree& propTree){
//    
//}

PadInfo* BmnPadGenerator::GeneratePadNode(pt::ptree& propTree) {
    PadInfo * info = nullptr;
    try {
        string clName = propTree.get<string>("Class");
        TClass* cl = TClass::GetClass(clName.c_str());
        printf("cl name %s\n", cl->GetName());
        ROOT::NewFunc_t histNew = cl->GetNew();
        string name = propTree.get<string>("Name");
        string title = propTree.get<string>("Title");
        printf("Hist name %s\n", name.c_str());
        vector<Int_t> dimVec;
        boost::optional<pt::ptree> dim = propTree.get_child("Dimensions");
        if (dim) {
            for (auto v = propTree.get_child("Dimensions").begin(); v != propTree.get_child("Dimensions").end(); v++) {
                dimVec.push_back(stoi((*v).second.data()));
                printf("%d\n", dimVec.back());
            }
        } else
            dimVec.resize(3, 0); // default 1Dim histogram
        info = new PadInfo();
        info->name = TString(clName.c_str());
        boost::optional<string> opt = propTree.get_optional<string>("Options");
        if (opt) {
            info->opt = TString(opt.get());
            printf("info opt %s\n", info->opt.Data());
        }
        TH1* h = nullptr;
        TH2* h2 = nullptr;
        TH3* h3 = nullptr;
        switch (dimVec.size()) {
            case 3:
                h = static_cast<TH1*> (histNew(0));
                h->SetName(name.c_str());
                h->SetTitle(title.c_str());
                h->SetBins(
                        dimVec[0],
                        dimVec[1],
                        dimVec[2]);
                _HM->Add(name, h);
                info->current = h; //_HM->H1(name);
                break;
            case 6:
                h2 = static_cast<TH2*> (histNew(0));
                h2->SetName(info->name);
                h2->SetBins(
                        dimVec[0],
                        dimVec[1],
                        dimVec[2],
                        dimVec[3],
                        dimVec[4],
                        dimVec[5]);
                h2->SetName(name.c_str());
                h2->SetTitle(title.c_str());
                _HM->Add(name, h2);
                info->current = h2; //_HM->H2(name);
                break;
            case 9:
                h3 = static_cast<TH3*> (histNew(0));
                h3->SetName(info->name);
                h3->SetBins(
                        dimVec[0],
                        dimVec[1],
                        dimVec[2],
                        dimVec[3],
                        dimVec[4],
                        dimVec[5],
                        dimVec[6],
                        dimVec[7],
                        dimVec[8]);
                h3->SetName(name.c_str());
                h3->SetTitle(title.c_str());
                _HM->Add(name, h3);
                info->current = h3;
                break;
            default:
                throw string("Wrong dimensions!");
                break;
        }
        printf("hist class name %s\n", info->current->Class()->GetName());
    } catch (std::exception& ex) {
        printf("Exception for node: %s\n", ex.what());
        if (info) {
            delete info;
            info = nullptr;
        }
    }
    return info;
}

BmnPadBranch* BmnPadGenerator::GeneratePadBranch(pt::ptree& propTree) {
    BmnPadBranch* branch = nullptr;
    try {
        branch = new BmnPadBranch();
        boost::optional<Int_t> optDivX = propTree.get_optional<Int_t>("DivX");
        boost::optional<Int_t> optDivY = propTree.get_optional<Int_t>("DivY");
        Int_t divX = optDivX ? *optDivX : 1;
        Int_t divY = optDivY ? *optDivY : 1;
        branch->SetDivXY(divX, divY);
        printf("%d : %d \n", divX, divY);

        if ((divX == 1) && (divY == 1)) {
            PadInfo *info = GeneratePadNode(propTree);
            branch->SetPadInfo(info);
        } else {
            //            string name = propTree.get<string>("Name");
            //            string title = propTree.get<string>("Title");
            //            printf("title %s\n", title.c_str());

            boost::optional<pt::ptree &> pads = propTree.get_child_optional("Pads");
            //            cout << (*pads) << endl;
            if (pads) {
                for (auto v = pads.get().begin(); v != pads.get().end(); v++) {
                    cout << (*v).second.data() << endl;
                    BmnPadBranch *b = GeneratePadBranch((*v).second);
                    branch->AddBranch(b);
                }

            }
        }

    } catch (std::exception& ex) {
        printf("Exception for branch: %s\n", ex.what());
        if (branch) {
            delete branch;
            branch = nullptr;
        }
    }
    return branch;

}

void BmnPadGenerator::PadTree2Canvas(BmnPadBranch* br, TVirtualPad* pad) {
    if ((!br) || (!pad)) {
        printf("No pad data! Skipping.\n");
        return;
    }
    if (PadInfo * info = br->GetPadInfo()) {
        info->padPtr = pad;
    } else {
        if (br->GetBranchesRef().size() == 0)
            return;
        pad->Divide(br->GetDivX(), br->GetDivY());
        for (Int_t i = 0; i < br->GetDivX() * br->GetDivY(); i++) {
            TVirtualPad* innerPad = pad->cd(i + 1);
            BmnPadBranch* innerBr = br->GetBranchesRef().at(i);
            PadTree2Canvas(innerBr, innerPad);
        }
    }
    return;
}
