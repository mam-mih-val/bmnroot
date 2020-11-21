
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
        //        const TClass ccl = static_cast<const TClass>(*cl);
        string name = propTree.get<string>("Name");
        const char* nc = name.c_str();
        string title = propTree.get<string>("Title");
        const char* tc = title.c_str();
        printf("Hist name %s\n", name.c_str());
        vector<Int_t> dimVec;
        boost::optional<pt::ptree> dim = propTree.get_child("Dimensions");
        if (dim) {
            for (auto v = propTree.get_child("Dimensions").begin(); v != propTree.get_child("Dimensions").end(); v++) {
                //                cout << (*v).second.data() << endl;
                //                printf("%s\n", string((*v).second.data()).c_str());
                dimVec.push_back(stoi((*v).second.data()));
                printf("%d\n", dimVec.back());
            }
            //            BOOST_FOREACH(pt::ptree::value_type &v, dim) {
            //                dimVec.push_back(stoi((*v).second.data()));
            //            }
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
        switch (dimVec.size()) {
            case 3:
                h = static_cast<TH1*> (histNew(0));
                //                h = static_cast<TH1*>(histNew(nc, tc,
                //                        dimVec[0],
                //                        dimVec[1],
                //                        dimVec[2]));
                //                _HM->Create1<TH1F>(name, title,
                //                        dimVec[0],
                //                        dimVec[1],
                //                        dimVec[2]);
                h->SetName(name.c_str());
                h->SetTitle(title.c_str());
                h->SetBins(
                        dimVec[0],
                        dimVec[1],
                        dimVec[2]);
                _HM->Add(name, h);
                info->current = h; //_HM->H1(name);
                printf("hist class name %s\n", info->current->Class()->GetName());
                break;
            case 6:
                h2 = static_cast<TH2*> (histNew(0)); //nc, tc,
                //                _HM->Create2<TH2F>(name, title,
                //                        dimVec[0],
                //                        dimVec[1],
                //                        dimVec[2],
                //                        dimVec[3],
                //                        dimVec[4],
                //                        dimVec[5]);
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
                printf("hist class name %s\n", info->current->Class()->GetName());
                break;
            default:
                throw string("Wrong dimensions!");
                break;
        }
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
