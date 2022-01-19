R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include <services/json.hpp>

using json = nlohmann::json;
using namespace ROOT;


int mdst_converter(TString inFilePath,
                   TString outFilePath = "",
                   TString configFileName = "mdst_config.json",
                   TString outFormat = "root",
                   Int_t threadsNumber = -1)
{
    TStopwatch timer;
    timer.Start();

    // form output file path if it is empty or a directory
    gSystem->ExpandPathName(inFilePath);
    if (outFilePath == "") {
        outFilePath = inFilePath;
        outFilePath.ReplaceAll("dst", "mdst");
    }
    else gSystem->ExpandPathName(outFilePath);
    if (BmnFunctionSet::isDirectory(outFilePath))
        outFilePath = gSystem->ConcatFileName(outFilePath, ((TString)gSystem->BaseName(inFilePath.Data())).ReplaceAll("dst", "mdst"));
    if (outFilePath == inFilePath) {
        cout << "[ERROR] Output file path is not set correctly: " << outFilePath << endl;
        return -1;
    }

    // read configuration variables
    configFileName = gSystem->ExpandPathName("$VMCWORKDIR/config/" + configFileName);
    ifstream configFile(configFileName.Data());
    if (!configFile.is_open()) {
        cout << "[ERROR] Cannot open config file " << configFileName << endl;
        return -2;
    }
    json config;
    try {
        configFile >> config;
    }
    catch (const json::parse_error& ex) {
        cout << "[ERROR] Wrong config file format: " << ex.what() << endl;
        return -3;
    }
    Int_t minNTracks = config.value("minNTracks", 2);
    Double_t minZ = config.value("minZ", -TMath::Infinity());
    Double_t maxZ = config.value("maxZ", TMath::Infinity());
    vector<string> excludedBranches = config.value("excludedBranches", vector<string>());

    if (outFormat != "root") {
        cout << "[ERROR] Only \"root\" format is supported at the moment" << endl;
        return -4;
    }

    if (threadsNumber >= 0) ROOT::EnableImplicitMT(threadsNumber);
    
    TFile* inFile = TFile::Open(inFilePath.Data());
    if (inFile == nullptr) {
        cout << "[ERROR] Cannot open input DST file " << inFilePath << endl;
        return -5;
    }
    TTree* inTree = (TTree*) inFile->Get("bmndata");  
    if (!inTree) {
        cout << "[ERROR] Input file " << inFilePath << " does not contain \"bmndata\" tree" << endl;
        return -6;
    }

    // exclude branches
    vector<string> branches;
    for (TObject* branch: *inTree->GetListOfBranches()) {
        string branchName = ((TBranch*)branch)->GetName();
        if (find(excludedBranches.begin(), excludedBranches.end(), branchName) == excludedBranches.end())
            branches.push_back(branchName);
    }

    // lamda to filter events
    auto filter = [minNTracks, minZ, maxZ](const TClonesArray& vertices){
        BmnVertex* vtx = static_cast<BmnVertex*>(vertices.UncheckedAt(0));
        return (vtx->GetZ() > minZ && vtx->GetZ() < maxZ && vtx->GetNTracks() >= minNTracks); 
    };

    RDataFrame data(*inTree);
    auto eventsCountBeforeFilter = data.Count();
    auto filteredData = data.Filter(filter, {"BmnVertex"});
    auto eventsCountAfterFilter = filteredData.Count();
    
    if (BmnFunctionSet::CreateDirectoryTree(outFilePath, 1) < 0) {
        cout << "[ERROR] Output file " << outFilePath << " cannot be created" << endl;
        return -7;
    }
    filteredData.Snapshot("bmndata", outFilePath.Data(), branches);

    cout << "Event number before filtering: " << *eventsCountBeforeFilter << endl;
    cout << "Event number after filtering:  " << *eventsCountAfterFilter << endl;
    delete inFile;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    return 0;
}
