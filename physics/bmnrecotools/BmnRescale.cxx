#include "BmnRescale.h"

BmnRescale::BmnRescale(
        UInt_t periodId, UInt_t runId,
        Double_t lowThr,
        Double_t ClusterSizeThr,
        Int_t nBins) {
    fPeriodId = periodId;
    fRunId = runId;
    fSetup = (fRunId >= 2041 && fRunId <= 3588) ? kSRCSETUP : kBMNSETUP;
    fLowThr = lowThr;
    fClusterSizeThr = ClusterSizeThr;
    fNBins = nBins;
    CreateStripVectors();
}

BmnRescale::~BmnRescale() {
    FreeVector3D<BmnSigInfo>(fStripGemInfoMC);
    FreeVector3D<BmnSigInfo>(fStripSilInfoMC);
    FreeVector3D<BmnSigInfo>(fStripCSCInfoMC);
    FreeVector3D<BmnSigInfo>(fStripGemInfoEx);
    FreeVector3D<BmnSigInfo>(fStripSilInfoEx);
    FreeVector3D<BmnSigInfo>(fStripCSCInfoEx);
    FreeVector3D<TF1>(fStripGemRescale);
    FreeVector3D<TF1>(fStripSilRescale);
    FreeVector3D<TF1>(fStripCSCRescale);
    delete fGemStationSet;
    delete fSilStationSet;
    delete fCscStationSet;
}

void BmnRescale::CreateStripVectors() {
    TString xmlConfFileNameGEM;
    TString xmlConfFileNameSil;
    TString xmlConfFileNameCSC;
    // Select xml configs
    switch (fPeriodId) {
        case 7:
            if (fSetup == kBMNSETUP) {
                xmlConfFileNameGEM = "GemRunSpring2018.xml";
                xmlConfFileNameSil = "SiliconRunSpring2018.xml";
                xmlConfFileNameCSC = "CSCRunSpring2018.xml";
            } else {
                xmlConfFileNameGEM = "GemRunSRCSpring2018.xml";
                xmlConfFileNameSil = "SiliconRunSRCSpring2018.xml";
                xmlConfFileNameCSC = "CSCRunSRCSpring2018.xml";
            }
            break;
        case 6:
            xmlConfFileNameGEM = "GemRunSpring2017.xml";
            xmlConfFileNameSil = "SiliconRunSpring2017.xml";
            break;
        default:
            printf("Error! Unknown config!\n");
            return;
            break;

    }
    // create 3d vectors for functions for all stations/modules/planes
    if (xmlConfFileNameGEM.Length() > 0) {
        xmlConfFileNameGEM = TString(getenv("VMCWORKDIR")) + "/parameters/gem/XMLConfigs/" + xmlConfFileNameGEM;
        printf("xmlConfFileName %s\n", xmlConfFileNameGEM.Data());
        fGemStationSet = new BmnGemStripStationSet(xmlConfFileNameGEM);
        FillInfoVector<BmnGemStripStationSet, BmnGemStripStation, BmnGemStripModule>(
                fGemStationSet, fStripGemInfoMC);
        FillInfoVector<BmnGemStripStationSet, BmnGemStripStation, BmnGemStripModule>(
                fGemStationSet, fStripGemInfoEx);
    }
    if (xmlConfFileNameSil.Length() > 0) {
        xmlConfFileNameSil = TString(getenv("VMCWORKDIR")) + "/parameters/silicon/XMLConfigs/" + xmlConfFileNameSil;
        printf("xmlConfFileName %s\n", xmlConfFileNameSil.Data());
        fSilStationSet = new BmnSiliconStationSet(xmlConfFileNameSil);
        FillInfoVector<BmnSiliconStationSet, BmnSiliconStation, BmnSiliconModule>(
                fSilStationSet, fStripSilInfoMC);
        FillInfoVector<BmnSiliconStationSet, BmnSiliconStation, BmnSiliconModule>(
                fSilStationSet, fStripSilInfoEx);

    }
    if (xmlConfFileNameCSC.Length() > 0) {
        xmlConfFileNameCSC = TString(getenv("VMCWORKDIR")) + "/parameters/csc/XMLConfigs/" + xmlConfFileNameCSC;
        printf("xmlConfFileName %s\n", xmlConfFileNameCSC.Data());
        fCscStationSet = new BmnCSCStationSet(xmlConfFileNameCSC);
        FillInfoVector<BmnCSCStationSet, BmnCSCStation, BmnCSCModule>(
                fCscStationSet, fStripCSCInfoMC);
        FillInfoVector<BmnCSCStationSet, BmnCSCStation, BmnCSCModule>(
                fCscStationSet, fStripCSCInfoEx);
    }
}

TF1* BmnRescale::GetRescaleFunc(TString name, TF1 *mc, TF1 *ex) {
    if (!mc || !ex)
        return nullptr;
    TF1 *funcRescale = new TF1(name,
            [mc, ex](Double_t *x, Double_t * p) {
                Double_t r = mc->Eval(x[0]);
                Double_t f = ex->GetX(r); //, ex->GetXmin(), ex->GetXmax(), 1.E-15, 100, kTRUE);
                //                printf("x = %f  mc = %f  y = %f\n", x[0], r, f);
                return f;
            }, mc->GetXmin(), mc->GetXmax(), 0);
    return funcRescale;
}

BmnStatus BmnRescale::LoadDigitDistributionsFromFile(
        TString fileName,
        vector<TString> digiNames,
        vector<vector<vector<vector<BmnSigInfo* > > > > digiVecs) {
    vector<TClonesArray*> ars;
    TChain* chain = new TChain("bmndata");
    chain->Add(fileName.Data());
    Long64_t NEventsMC = chain->GetEntries();
    printf("%s recorded entries = %lld\n", fileName.Data(), NEventsMC);
    for (TString &digiName : digiNames) {
        TClonesArray* arDigi = nullptr;
        chain->SetBranchAddress(digiName.Data(), &arDigi);
        ars.push_back(arDigi);
    }
    for (Int_t i = 0; i < digiVecs.size(); i++)
        CreateDigitDistributions(chain, ars[i], digiVecs[i]);
    delete chain;
    return kBMNSUCCESS;
}

BmnStatus BmnRescale::LoadClusterDistributionsFromFile(
        TString fileName,
        vector<TString> hitNames,
        vector<vector<vector<vector<BmnSigInfo* > > > > clusterVecs) {
    vector<TClonesArray*> hitArr;
    TChain* chain = new TChain("bmndata");
    chain->Add(fileName.Data());
    Long64_t NEvents = chain->GetEntries();
    printf("%s recorded entries = %lld\n", fileName.Data(), NEvents);
    for (TString &hitName : hitNames) {
        TClonesArray* arDigi = nullptr;
        chain->SetBranchAddress(hitName.Data(), &arDigi);
        hitArr.push_back(arDigi);
    }
    //    for (Int_t i = 0; i < fNArs; i++)
    //        CreateClusterDistributions(chain, ars[i], clusterVecs[i]);
    CreateClusterDistributions<
            BmnGemStripHit,
            BmnGemStripStationSet,
            BmnGemStripStation,
            BmnGemStripModule,
            BmnGemStripLayer>(chain, hitArr[0], fGemStationSet, clusterVecs[0]);
    CreateClusterDistributions<
            BmnSiliconHit,
            BmnSiliconStationSet,
            BmnSiliconStation,
            BmnSiliconModule,
            BmnSiliconLayer>(chain, hitArr[1], fSilStationSet, clusterVecs[1]);
    CreateClusterDistributions<
            BmnCSCHit,
            BmnCSCStationSet,
            BmnCSCStation,
            BmnCSCModule,
            BmnCSCLayer>(chain, hitArr[2], fCscStationSet, clusterVecs[2]);
    delete chain;
    return kBMNSUCCESS;
}

BmnStatus BmnRescale::LoadClustersOfTrackDistributionsFromFile(
        TString fileName,
        TString globalTrackName,
        vector<TString> subTrackNames,
        vector<TString> hitNames,
        vector<vector<vector<vector<BmnSigInfo* > > > > clusterVecs) {
    TClonesArray* globalTrackArr;
    vector<TClonesArray*> subTrackArr;
    vector<TClonesArray*> hitArr;
    TChain* chain = new TChain("bmndata");
    chain->Add(fileName.Data());
    Long64_t NEvents = chain->GetEntries();
    printf("%s recorded entries = %lld\n", fileName.Data(), NEvents);

    chain->SetBranchAddress(globalTrackName.Data(), &globalTrackArr);

    for (TString &subTrackName : subTrackNames) {
        TClonesArray* arDigi = nullptr;
        if (subTrackName.Length() > 0)
            chain->SetBranchAddress(subTrackName.Data(), &arDigi);
        subTrackArr.push_back(arDigi);
    }
    for (TString &hitName : hitNames) {
        TClonesArray* arDigi = nullptr;
        chain->SetBranchAddress(hitName.Data(), &arDigi);
        hitArr.push_back(arDigi);
    }
    //    for (Int_t i = 0; i < fNArs; i++)
    //        CreateClusterDistributions(chain, ars[i], clusterVecs[i]);
    CreateClusterDistributions<
            BmnGemStripHit,
            BmnGemStripStationSet,
            BmnGemStripStation,
            BmnGemStripModule,
            BmnGemStripLayer>(chain, globalTrackArr, subTrackArr[0], hitArr[0], fGemStationSet, clusterVecs[0]);
    CreateClusterDistributions<
            BmnSiliconHit,
            BmnSiliconStationSet,
            BmnSiliconStation,
            BmnSiliconModule,
            BmnSiliconLayer>(chain, globalTrackArr, subTrackArr[1], hitArr[1], fSilStationSet, clusterVecs[1]);
    CreateClusterDistributions<
            BmnCSCHit,
            BmnCSCStationSet,
            BmnCSCStation,
            BmnCSCModule,
            BmnCSCLayer>(chain, globalTrackArr, nullptr, hitArr[2], fCscStationSet, clusterVecs[2]);
    delete chain;
    return kBMNSUCCESS;
}

BmnStatus BmnRescale::CreateRescales(TString fileNameMC, TString fileNameEx) {
    if (fileNameMC == "" || fileNameEx == "") {
        cout << "Files need to be specified!" << endl;
        return kBMNERROR;
    }
    TString GlobalTrackName = "BmnGlobalTrack";
    vector<TString> mcDigiNames = {"BmnGemStripDigit", "BmnSiliconDigit", "BmnCSCDigit"};
    auto mcVecs = {fStripGemInfoMC, fStripSilInfoMC, fStripCSCInfoMC};
    LoadDigitDistributionsFromFile(fileNameMC, mcDigiNames, mcVecs);

    vector<TString> subTrackNames = {"BmnGemTrack", "BmnSiliconTrack", ""};
    vector<TString> hitNames = {"BmnGemStripHit", "BmnSiliconHit", "BmnCSCHit"};
    //    vector<TString> expDigiNames = {"SILICON", "GEM", "CSC"};
    //    LoadDigitDistributionsFromFile(fileNameEx, expDigiNames, expVecs);
    auto expVecs = {fStripGemInfoEx, fStripSilInfoEx, fStripCSCInfoEx};
    LoadClustersOfTrackDistributionsFromFile(fileNameEx, GlobalTrackName, subTrackNames, hitNames, expVecs);

    FillRescaleVector<BmnGemStripStationSet, BmnGemStripStation, BmnGemStripModule>(fGemStationSet, fStripGemInfoMC, fStripGemInfoEx, fStripGemRescale);
    FillRescaleVector<BmnSiliconStationSet, BmnSiliconStation, BmnSiliconModule>(fSilStationSet, fStripSilInfoMC, fStripSilInfoEx, fStripSilRescale);
    FillRescaleVector<BmnCSCStationSet, BmnCSCStation, BmnCSCModule>(fCscStationSet, fStripCSCInfoMC, fStripCSCInfoEx, fStripCSCRescale);

    return kBMNSUCCESS;
}

void BmnRescale::CreateDigitDistributions(
        TTree *treeDig, TClonesArray *digits,
        vector<vector<vector<BmnSigInfo* > > > &infoVec) {
    printf("CreateDigitDistributions started for %s\n", digits->GetClass()->GetName());
    // Find bounds
    for (Long64_t i = 0; i < treeDig->GetEntries(); i++) {
        treeDig->GetEntry(i);
        for (Int_t iHit = 0; iHit < digits->GetEntriesFast(); iHit++) {
            BmnStripDigit *dig = static_cast<BmnStripDigit*> (digits->UncheckedAt(iHit));
            Int_t s = dig->GetStation();
            Int_t m = dig->GetModule();
            Int_t l = dig->GetStripLayer();
            BmnSigInfo* info = infoVec[s][m][l];
            Double_t val = dig->GetStripSignal();
            if (val > fLowThr) {
                if (info->maxVal < val)
                    info->maxVal = val;
                if (info->minVal > val)
                    info->minVal = val;
            }
        }
    }
    // Create histograms
    for (Int_t iStation = 0; iStation < infoVec.size(); iStation++) {
        printf("iStation %d\n", iStation);
        for (Int_t iModule = 0; iModule < infoVec[iStation].size(); iModule++) {
            printf("\tiModule %d\n", iModule);
            for (Int_t iLayer = 0; iLayer < infoVec[iStation][iModule].size(); iLayer++) {
                printf("\t\tiLayer %d\n", iLayer);
                BmnSigInfo* info = infoVec[iStation][iModule][iLayer];
                Double_t minVal = info->minVal;
                Double_t maxVal = info->maxVal;
                if (minVal > maxVal)
                    continue;
                printf("\t\t\tMin  = %f max = %f\n", minVal, maxVal);
                TString name = Form("Sig_mc_%s_%d_%d_%d",
                        digits->GetName(), iStation, iModule, iLayer);
                info->hSig = new TH1D(name, name, fNBins,
                        minVal - 0.5 * (maxVal - minVal) / (Double_t) fNBins,
                        maxVal + 0.5 * (maxVal - minVal) / (Double_t) fNBins);
                name = Form("IntSignal_%s_%d_%d_%d",
                        digits->GetName(), iStation, iModule, iLayer);
                info->hIntSig = new TH1D(name, name, fNBins,
                        minVal - 0.5 * (maxVal - minVal) / (Double_t) fNBins,
                        maxVal + 0.5 * (maxVal - minVal) / (Double_t) fNBins);
            }
        }
    }
    // Fill signal hists
    for (Long64_t i = 0; i < treeDig->GetEntries(); i++) {
        treeDig->GetEntry(i);
        for (Int_t iHit = 0; iHit < digits->GetEntriesFast(); iHit++) {
            BmnStripDigit *dig = static_cast<BmnStripDigit*> (digits->UncheckedAt(iHit));
            Int_t s = dig->GetStation();
            Int_t m = dig->GetModule();
            Int_t l = dig->GetStripLayer();
            BmnSigInfo* info = infoVec[s][m][l];
            if (!(info->hSig))
                continue;
            Double_t val = dig->GetStripSignal();
            if (val > fLowThr) {
                info->hSig->Fill(val);
            }
        }
    }
    // Fill integral hists
    for (Int_t iStation = 0; iStation < infoVec.size(); iStation++) {
        for (Int_t iModule = 0; iModule < infoVec[iStation].size(); iModule++) {
            for (Int_t iLayer = 0; iLayer < infoVec[iStation][iModule].size(); iLayer++) {
                BmnSigInfo* info = infoVec[iStation][iModule][iLayer];
                if (!(info->hSig))
                    continue;
                Double_t bc = 0;
                for (Int_t i = 0; i < fNBins; i++) {
                    bc += info->hSig->GetBinContent(i);
                    info->hIntSig->SetBinContent(i, bc);
                }
                if (bc == 0)
                    continue;
                info->hIntSig->Scale(1 / bc);
                info->hIntSig->SetLineColor(kRed);
                info->func = new TF1(
                        Form("%s_%d_%d_%d", digits->GetName(), iStation, iModule, iLayer),
                        [info, bc](Double_t *x, Double_t * p) {
                            //            Double_t val = (bc - x[0])/bc;//log(x[0]);
                            if (x[0] >= info->maxVal) return 1.0;
                            Int_t iBin = info->hIntSig->GetXaxis()->FindBin(x[0]);
                            Int_t jBin = iBin + ((x[0] > info->hIntSig->GetBinCenter(iBin)) ? 1 : -1);
                            Double_t v = info->hIntSig->GetBinContent(iBin) +
                                    (info->hIntSig->GetBinContent(jBin) - info->hIntSig->GetBinContent(iBin))*
                                    2 * abs(x[0] - info->hIntSig->GetBinCenter(iBin)) /
                                    (info->hIntSig->GetBinWidth(iBin) + info->hIntSig->GetBinWidth(jBin));
                            return v;
                        }, info->minVal, info->maxVal, 0);
                info->func->SetNpx(fNBins);
                info->func->SetLineColor(kBlue);
            }
        }
    }
}

TCanvas * BmnRescale::GetCanvasGem() {
    return GetCanvas<
            BmnGemStripStationSet,
            BmnGemStripStation,
            BmnGemStripModule>(fGemStationSet);
}

TCanvas * BmnRescale::GetCanvasSil() {
    return GetCanvas<
            BmnSiliconStationSet,
            BmnSiliconStation,
            BmnSiliconModule>(fSilStationSet);
}

TCanvas * BmnRescale::GetCanvasCSC() {
    return GetCanvas<
            BmnCSCStationSet,
            BmnCSCStation,
            BmnCSCModule>(fCscStationSet);
}

//ClassImp(BmnRescale)
