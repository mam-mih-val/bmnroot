/**
 * \file BmnPidQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2007-2021
 */

#include "BmnPidQa.h"
#include <fstream>
#include <iostream>
#include "BmnAcceptanceFunction.h"
#include "BmnDchHit.h"
#include "BmnEnums.h"
#include "BmnGemStripHit.h"
#include "BmnGemTrack.h"
#include "BmnMCPoint.h"
#include "BmnMatch.h"
#include "BmnMath.h"
#include "BmnSiliconHit.h"
#include "BmnTrackMatch.h"
#include "BmnPidQaReport.h"
#include "BmnTrackingQaStudyReport.h"
#include "BmnUtils.h"
#include "CbmBaseHit.h"
#include "CbmGlobalTrack.h"
#include "BmnGlobalTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmTofHit.h"
#include "BmnVertex.h"
#include "FairMCEventHeader.h"
#include "FairMCPoint.h"
#include "FairRunAna.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TH1.h"
#include "TH2F.h"
#include "BmnHistManager.h"

using namespace std;
using namespace TMath;
using lit::FindAndReplace;
using lit::Split;

BmnPidQa::BmnPidQa(TString name, TString storageName) :     FairTask("BmnPidQA", 1),
                                                            fHM(NULL),
                                                            fOutputDir("./"),
                                                            fPRangeMin(0.),
                                                            fPRangeMax(6.),
                                                            fPRangeBins(500),
                                                            fBetaRangeMin(0.),
                                                            fBetaRangeMax(1.1),
                                                            fBetaRangeBins(500),
                                                            fTimeRangeMin(0.),
                                                            fTimeRangeMax(50),
                                                            fTimeRangeBins(500),
                                                            fMassRangeMin(0.),
                                                            fMassRangeMax(1),
                                                            fMassRangeBins(100),
                                                            fVelocRangeMin(1.5e+8),
                                                            fVelocRangeMax(3.2e+8),
                                                            fVelocRangeBins(500),
                                                            fNHitsRangeMin(0),
                                                            fNHitsRangeMax(15),
                                                            fNHitsRangeBins(500),
                                                            fOutName(name),
                                                            fStorageName(storageName),
                                                            fMCTracks(NULL),
                                                            fPrimes(kFALSE),
                                                            fGlobalTracks(NULL) {

    db = TDatabasePDG::Instance();  
    //Add several types of particles to the database.
    // p, pi, K, e are already in it
    db->AddParticle("D","D", 1.876123928, true, 0, 3, "Core", 1000010020, 1000010020, 1000010020);
    db->AddParticle("T","T", 2.809432115, true, 0, 3, "Core", 1000010030, 1000010030, 1000010030);
    db->AddParticle("He3","He3", 2.809413523, true, 0, 6, "Core", 1000020030, 1000020030, 1000020030);
    db->AddParticle("He4","He4", 3.728401326, true, 0, 6, "Core", 1000020040, 1000020040, 1000020040);

    for(PidParticles iSort=(PidParticles)0; iSort!=EndPidEnum; iSort=(PidParticles)(iSort+1)){
            Int_t pdg = EnumToPdg(iSort);
            TParticlePDG* iParticle = db->GetParticle(pdg);
            fParticles.push_back(iParticle);
            Double_t mass = iParticle->Mass();
            if(fMassTable.count(mass) == 0){
                fMassTable.insert(pair<Double_t, string>(mass, iParticle->GetName()));
                fPidStatistics400.insert(pair<string, vector<Int_t>>(iParticle->GetName(),vector<Int_t>(4,0)));
            } 
    }
    fPidStatistics700 = fPidStatistics400;
}


BmnPidQa::~BmnPidQa() {
    if(fHM) delete fHM;
}

InitStatus BmnPidQa::Init() {
    fHM = new BmnHistManager();
    CreateHistograms();
    ReadDataBranches();
    return kSUCCESS;
}

void BmnPidQa::Exec(Option_t* opt) {
    ProcessGlobal();
}

void BmnPidQa::Finish(){
    
    fHM->WriteToFile();
    BmnSimulationReport* report = new BmnPidQaReport(fOutName, fStorageName, fMassTable);
    report->SetOnlyPrimes(fPrimes);
    report->Create(fHM, fOutputDir);
    delete report;

    MassTablePrint();
    cout << endl;
    cout << "Name" << '\t' << "Total" << '\t' << "True" << '\t' << "False" <<  endl;
    cout << "TOF400 stat" << endl;
    PidStatisticsPrint400();
    cout << endl;
    cout << "TOF700 stat" << endl;
    PidStatisticsPrint700();
}

void BmnPidQa::ReadDataBranches(){
   
    FairRootManager* ioman = FairRootManager::Instance();
    cout << "ReadDataBranches()" << endl;
    if(NULL == ioman) Fatal("Init", "BmnRootManager is not instantiated");

    fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");
    if(NULL == fMCTracks) Fatal("Init", "No MCTrack array!");

    fGlobalTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
    fGlobalTrackMatches = (TClonesArray*)ioman->GetObject("BmnGlobalTrackMatch");
    
    fTof400Hits = (TClonesArray*) ioman->GetObject("BmnTof400Hit");
    fTof700Hits = (TClonesArray*) ioman->GetObject("BmnTof700Hit");
}

void BmnPidQa::CreateH1(const string& name, const string& xTitle, const string& yTitle, Int_t nofBins, Double_t minBin, Double_t maxBin) {
    TH1F* h = new TH1F(name.c_str(), string(name + ";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
    fHM->Add(name, h);
}

void BmnPidQa::CreateH2(
    const string& name,
    const string& xTitle,
    const string& yTitle,
    const string& zTitle,
    Int_t nofBinsX,
    Double_t minBinX,
    Double_t maxBinX,
    Int_t nofBinsY,
    Double_t minBinY,
    Double_t maxBinY) {
    TH2F* h = new TH2F(name.c_str(), (name + ";" + xTitle + ";" + yTitle + ";" + zTitle).c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
    fHM->Add(name, h);
}

void BmnPidQa::CreateTrackHitsHistogram(const string& detName) {
    string type[] = {"All", "True", "Fake", "TrueOverAll", "FakeOverAll"};
    Double_t min[] = {0., 0., 0., 0., 0.};
    Double_t max[] = {20, 20, 20, 1., 1.};
    Int_t bins[] = {20, 20, 20, 20, 20};
    for (Int_t i = 0; i < 5; i++) {
        string xTitle = (i == 3 || i == 4) ? "Ratio" : "Number of hits";
        string histName = "hth_" + detName + "_TrackHits_" + type[i];
        CreateH1(histName.c_str(), xTitle, "Yeild", bins[i], min[i], max[i]);
    }
}

void BmnPidQa::CreateHistograms() {
      
    cout << "CreateHistograms()" << endl;
    CreateH2("Banana-plot TOF-400", "TOF-400 P_{rec}/q, GeV/c/e", "Beta, c", "N",
                fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);
    CreateH2("Banana-plot TOF-700", "TOF-700 P_{rec}/q, GeV/c/e", "Beta, c", "N",
                fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);
      
    for (auto iter = fMassTable.begin(); iter != fMassTable.end(); ++iter){
        string nameOfParticle = (*iter).second;
        cout << "Particle name is " << nameOfParticle << endl;
                

        // Momentum histograms
        
        //Velocity hist
        CreateH2("Total velocity from P TOF-400 for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Velocity, m/s", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fVelocRangeBins, fVelocRangeMin, fVelocRangeMax);

        CreateH2("Total velocity from P TOF-700 for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Velocity, m/s", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fVelocRangeBins, fVelocRangeMin, fVelocRangeMax);

        //Time hist
       
        CreateH2("Total time from P TOF-400 for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Time, ns", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fTimeRangeBins, fTimeRangeMin, fTimeRangeMax);

        CreateH2("Total time from P TOF-700 for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Time, ns", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fTimeRangeBins, fTimeRangeMin, fTimeRangeMax);        
               
        //
        CreateH1("TOF-400 total_vs_P for " + nameOfParticle,"True total, P_{rec}/q, GeV/c/e","N",fPRangeBins,fPRangeMin,fPRangeMax);
        CreateH1("TOF-700 total_vs_P for " + nameOfParticle,"True total, P_{rec}/q, GeV/c","N",fPRangeBins,fPRangeMin,fPRangeMax);
        // Histogram stores rate of true-reconstructions
        CreateH1("TOF-400 true_vs_P for " + nameOfParticle,"Pid hits, P_{rec}/q, GeV/c/e","N",fPRangeBins,fPRangeMin,fPRangeMax);
        CreateH1("TOF-700 true_vs_P for " + nameOfParticle,"Pid hits, P_{rec}/q, GeV/c/e","N",fPRangeBins,fPRangeMin,fPRangeMax);
        // Histogram stores rate of wrong-reconstructions
        CreateH1("TOF-400 false_vs_P for " + nameOfParticle,"Pid miss, P_{rec}/q, GeV/c/e","N",fPRangeBins,fPRangeMin,fPRangeMax);
        CreateH1("TOF-700 false_vs_P for " + nameOfParticle,"Pid miss, P_{rec}/q, GeV/c/e","N",fPRangeBins,fPRangeMin,fPRangeMax);
        // True + False hist for contamination of PID
        CreateH1("TOF-400 true-false_vs_P for " + nameOfParticle,"Contamination, P_{rec}/q, GeV/c/e","tof400, %",fPRangeBins,fPRangeMin,fPRangeMax);
        CreateH1("TOF-700 true-false_vs_P for " + nameOfParticle,"Contamination, P_{rec}/q, GeV/c/e","tof700, %",fPRangeBins,fPRangeMin,fPRangeMax);

      
        // Rigidity histograms
        //
        //Velocity  true/false hist
        CreateH2("True velocity from P TOF-400 for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Velocity, m/s", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fVelocRangeBins, fVelocRangeMin, fVelocRangeMax);

        CreateH2("True velocity from P TOF-700 for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Velocity, m/s", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fVelocRangeBins, fVelocRangeMin, fVelocRangeMax);

        CreateH2("False velocity from P TOF-400 for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Velocity, m/s", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fVelocRangeBins, fVelocRangeMin, fVelocRangeMax);

        CreateH2("False velocity from P TOF-700 for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Velocity, m/s", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fVelocRangeBins, fVelocRangeMin, fVelocRangeMax);

       
        //Time true/false hist
       
        CreateH2("True time from P TOF-400 for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Time, ns", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fTimeRangeBins, fTimeRangeMin, fTimeRangeMax);

        CreateH2("True time from P TOF-700 for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Time, ns", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fTimeRangeBins, fTimeRangeMin, fTimeRangeMax);

        CreateH2("False time from P TOF-400 for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Time, ns", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fTimeRangeBins, fTimeRangeMin, fTimeRangeMax);

        CreateH2("False time from P TOF-700 for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Time, ns", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fTimeRangeBins, fTimeRangeMin, fTimeRangeMax);

      
        //Mass^2 hist        

        fMassRangeMax = (*iter).first * (*iter).first * 3;

        CreateH2("Total mass^2 from P TOF-400 for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Mass, GeV^2", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fMassRangeBins, fMassRangeMin, fMassRangeMax);

        CreateH2("Total mass^2 from P TOF-700 for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Mass, GeV^2", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fMassRangeBins, fMassRangeMin, fMassRangeMax);

       
        CreateH2("True mass^2 from P TOF-400 for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Mass, GeV^2", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fMassRangeBins, fMassRangeMin, fMassRangeMax);

        CreateH2("True mass^2 from P TOF-700 for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Mass, GeV^2", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fMassRangeBins, fMassRangeMin, fMassRangeMax);

        CreateH2("False mass^2 from P TOF-400 for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Mass, GeV^2", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fMassRangeBins, fMassRangeMin, fMassRangeMax);

        CreateH2("False mass^2 from P TOF-700 for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Mass, GeV^2", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fMassRangeBins, fMassRangeMin, fMassRangeMax);


        CreateH2("TOF-400 total rigidity-momentum for " + nameOfParticle, "P_{rec}/q, GeV/c/e", "Beta, c", "N_{total}",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);
        CreateH2("TOF-700 total rigidity-momentum for " + nameOfParticle, "P_{rec}/q, GeV/c/e", "Beta, c", "N_{total}",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);
        CreateH2("TOF-400 true rigidity-momentum for " + nameOfParticle, "P_{rec}/q, GeV/c/e", "Beta, c", "N_{true}",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);
        CreateH2("TOF-700 true rigidity-momentum for " + nameOfParticle, "P_{rec}/q, GeV/c/e", "Beta, c", "N_{true}",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);
        CreateH2("TOF-400 false rigidity-momentum for " + nameOfParticle, "P_{rec}/q, GeV/c/e", "Beta, c", "N_{false}",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);
        CreateH2("TOF-700 false rigidity-momentum for " + nameOfParticle, "P_{rec}/q, GeV/c/e", "Beta, c", "N_{false}",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);

        /*      
        //Species undivided in time
        CreateH2("Undivided TOF-400 rigidity-momentum for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Beta, c", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);

        CreateH2("Undivided TOF-400 time from P for " + nameOfParticle, "TOF-400 P_{rec}/q, GeV/c/e", "Time, ns", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fTimeRangeBins, fTimeRangeMin, fTimeRangeMax);

        CreateH2("Undivided TOF-700 rigidity-momentum for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Beta, c", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fBetaRangeBins*2, fBetaRangeMin, fBetaRangeMax);

        CreateH2("Undivided TOF-700 time from P for " + nameOfParticle, "TOF-700 P_{rec}/q, GeV/c/e", "Time, ns", "N",
                 fPRangeBins*3, fPRangeMin, fPRangeMax, fTimeRangeBins, fTimeRangeMin, fTimeRangeMax);
       
        */      
        //Number of hits histograms
        //
        CreateH1("TOF-400 total_vs_NOfHits for " + nameOfParticle,"True total, N_{of hits}","N",fNHitsRangeBins,fNHitsRangeMin,fNHitsRangeMax);
        CreateH1("TOF-700 total_vs_NOfHits for " + nameOfParticle,"True total, N_{of hits}","N",fNHitsRangeBins,fNHitsRangeMin,fNHitsRangeMax);
        // Histogram stores rate of true-reconstructions
        CreateH1("TOF-400 true_vs_NOfHits for " + nameOfParticle,"Pid hits, N_{of hits}","N",fNHitsRangeBins,fNHitsRangeMin,fNHitsRangeMax);
        CreateH1("TOF-700 true_vs_NOfHits for " + nameOfParticle,"Pid hits, N_{of hits}","N",fNHitsRangeBins,fNHitsRangeMin,fNHitsRangeMax);
        // Histogram stores rate of wrong-reconstructions
        CreateH1("TOF-400 false_vs_NOfHits for " + nameOfParticle,"Pid miss, N_{of hits}","N",fNHitsRangeBins,fNHitsRangeMin,fNHitsRangeMax);
        CreateH1("TOF-700 false_vs_NOfHits for " + nameOfParticle,"Pid miss, N_{of hits}","N",fNHitsRangeBins,fNHitsRangeMin,fNHitsRangeMax);
        // True + False hist for contamination of PID
        CreateH1("TOF-400 true-false_vs_NOfHits for " + nameOfParticle,"Contamination, N_{of hits}","tof400, %",fNHitsRangeBins,fNHitsRangeMin,fNHitsRangeMax);
        CreateH1("TOF-700 true-false_vs_NOfHits for " + nameOfParticle,"Contamination, N_{of hits}","tof700, %",fNHitsRangeBins,fNHitsRangeMin,fNHitsRangeMax);

    }
}

void BmnPidQa::ProcessGlobal() {   
    for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); iTrack++){
        //Reciving of reco tracks
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*)(fGlobalTracks->At(iTrack));
        if(!glTrack) continue;

        //Banana-plots prepearing
        if(glTrack->GetBeta(1) > -999.0)
            fHM->H2("Banana-plot TOF-400")->Fill(glTrack->GetP(), glTrack->GetBeta(1));
        
        if(glTrack->GetBeta(2) > -999.0)
            fHM->H2("Banana-plot TOF-700")->Fill(glTrack->GetP(), glTrack->GetBeta(2));
        
        //Reciving of trackmatches
        BmnTrackMatch* globTrackMatch = (BmnTrackMatch*)(fGlobalTrackMatches->At(iTrack));
        if(!globTrackMatch) continue;
        if(globTrackMatch->GetNofLinks() == 0) continue;

        //Getting of most possible MCTrack index corresponding to the reco track 
        Int_t globMCId = globTrackMatch->GetMatchedLink().GetIndex();
       
        //Getting of MCTrack by match index
        CbmMCTrack* mcTrack = (CbmMCTrack*) (fMCTracks->At(globMCId));
        if(!mcTrack) continue;
        if(!(mcTrack->GetMotherId() == -1) && GetOnlyPrimes()){
            continue;
        }
        
        Double_t time;
        Double_t length;
        Double_t velocity;
        Int_t mcPdg = mcTrack->GetPdgCode();
       
        //TOF400:
        //Comparation of particle masses in MC and Reco tracks
        if((glTrack->GetBeta(1) > -999.0)){

            Int_t recoPdg1 = EnumToPdg(glTrack->GetParticleTof400());
            Int_t indexTOF400 = glTrack->GetTof1HitIndex();   
            Double_t mass = glTrack->GetMass2(1);             
            
            if(indexTOF400!=-1){                
                BmnHit *hit = (BmnHit *)fTof400Hits->At(indexTOF400);
                length = (hit->GetLength()); 
                time =  (hit->GetTimeStamp()); 
                velocity = length*1e+7/time; // length in cm, time in ns => m/s
            } 
            //Calcuation of corresponding histogram momentum bin           
            
            Double_t p = glTrack->GetP();
            Int_t nOfHits = glTrack->GetNHits();
            Double_t beta = glTrack->GetBeta(1);
            TParticlePDG* recoParticle = GetParticleExtend(recoPdg1);
            TParticlePDG* mcParticle = GetParticleExtend(mcPdg);
            if((mcParticle==0)||(recoParticle==0)) continue;
            Double_t recoMass = recoParticle->Mass();
            Double_t mcMass = mcParticle->Mass();
            string recoName = GetParticleName(recoMass);
            string mcName = GetParticleName(mcMass);                                 

            //Hist for undivided in time TOF400
            /*         
            if((recoPdg1==0)&&((fMassTable.count(mcMass))!=0)){
                ++fPidStatistics400[mcName][3]; // ++ undivided;
                fHM->H2("Undivided TOF-400 rigidity-momentum for " + mcName)->Fill(p, beta);

                fHM->H2("Undivided TOF-400 time from P for " + mcName)->Fill(p, time);
            }*/

            if(abs(mcMass - recoMass) < 0.000001){
                if(fMassTable.count(mcMass)!=0){
                    ++fPidStatistics400[mcName][1]; // +true pid;
                    fHM->H1("TOF-400 true_vs_P for " + mcName)->Fill(p);
                    fHM->H1("TOF-400 true_vs_NOfHits for " + mcName)->Fill(nOfHits);
                    fHM->H2("TOF-400 true rigidity-momentum for " + mcName)->Fill(p, beta);
                    if(indexTOF400!=-1){
                        fHM->H2("True velocity from P TOF-400 for " + mcName)->Fill(p, velocity);
                        fHM->H2("True time from P TOF-400 for " + mcName)->Fill(p, time);
                        fHM->H2("True mass^2 from P TOF-400 for " + mcName)->Fill(p, mass);
                    }                   
                }
            }else{
                if(fMassTable.count(recoMass)!=0){                    
                    ++fPidStatistics400[recoName][2]; // +false pid;
                    fHM->H1("TOF-400 false_vs_P for " + recoName)->Fill(p);
                    fHM->H1("TOF-400 false_vs_NOfHits for " + recoName)->Fill(nOfHits);
                    fHM->H2("TOF-400 false rigidity-momentum for " + recoName)->Fill(p, beta);
                    if(indexTOF400!=-1){
                        fHM->H2("False velocity from P TOF-400 for " + recoName)->Fill(p, velocity);
                        fHM->H2("False time from P TOF-400 for " + recoName)->Fill(p, time);
                        fHM->H2("False mass^2 from P TOF-400 for " + recoName)->Fill(p, mass);
                    }                    
                }  
            }
            if(fMassTable.count(mcMass)!=0){
                    ++fPidStatistics400[mcName][0]; // +total true particle number (mc);
                    fHM->H1("TOF-400 total_vs_P for " + mcName)->Fill(p);
                    fHM->H1("TOF-400 total_vs_NOfHits for " + mcName)->Fill(nOfHits);
                    fHM->H2("TOF-400 total rigidity-momentum for " + mcName)->Fill(p, beta);
                    if(indexTOF400!=-1){
                           fHM->H2("Total velocity from P TOF-400 for " + mcName)->Fill(p, velocity);
                           fHM->H2("Total time from P TOF-400 for " + mcName)->Fill(p, time);
                           fHM->H2("Total mass^2 from P TOF-400 for " + mcName)->Fill(p, mass);
                    }
            }
        }                
        
        //TOF700:
       
        if((glTrack->GetBeta(2) > -999.0)){

            Int_t recoPdg2 = EnumToPdg(glTrack->GetParticleTof700());
            Double_t mass = glTrack->GetMass2(2);
            Int_t indexTOF700 = glTrack->GetTof2HitIndex();
            
            if(indexTOF700!=-1){                
                BmnHit *hit = (BmnHit *)fTof700Hits->At(indexTOF700);
                length = (hit->GetLength()); 
                time =  (hit->GetTimeStamp()); 
                velocity = length*1e+7/time; // length in cm, time in ns => m/s
            }

            //Calcuation of corresponding historgram momentum bin
            Double_t p = glTrack->GetP();
            Int_t nOfHits = glTrack->GetNHits();
            Double_t beta = glTrack->GetBeta(2);
            TParticlePDG* recoParticle = GetParticleExtend(recoPdg2);
            TParticlePDG* mcParticle = GetParticleExtend(mcPdg);
            if((mcParticle==0)||(recoParticle==0)) continue;
            Double_t recoMass = recoParticle->Mass();         
            Double_t mcMass = mcParticle->Mass();
            string recoName = GetParticleName(recoMass);
            string mcName = GetParticleName(mcMass);
                   

            //Hist for undivided in time TOF700
           /* if((recoPdg2==0)&&((fMassTable.count(mcMass))!=0)) {
                ++fPidStatistics700[mcName][3]; // ++ undivided;
                fHM->H2("Undivided TOF-700 rigidity-momentum for " + mcName)->Fill(p, beta);
                fHM->H2("Undivided TOF-700 time from P for " + mcName)->Fill(p, time);
            }*/

            if(abs(mcMass - recoMass) < 0.000001){
                if(fMassTable.count(mcMass)!=0){
                    ++fPidStatistics700[mcName][1]; // +true pid;
                    fHM->H1("TOF-700 true_vs_P for " + mcName)->Fill(p);
                    fHM->H1("TOF-700 true_vs_NOfHits for " + mcName)->Fill(nOfHits);
                    fHM->H2("TOF-700 true rigidity-momentum for " + mcName)->Fill(p, beta);
                    if(indexTOF700!=-1){
                        fHM->H2("True velocity from P TOF-700 for " + mcName)->Fill(p, velocity);
                        fHM->H2("True time from P TOF-700 for " + mcName)->Fill(p, time);
                        fHM->H2("True mass^2 from P TOF-700 for " + mcName)->Fill(p, mass);
                    }                    
                }
            } else {
                if(fMassTable.count(recoMass)!=0){
                        ++fPidStatistics700[recoName][2]; // +false pid;
                        fHM->H1("TOF-700 false_vs_P for " + recoName)->Fill(p);
                        fHM->H1("TOF-700 false_vs_NOfHits for " + recoName)->Fill(nOfHits);
                        fHM->H2("TOF-700 false rigidity-momentum for " + recoName)->Fill(p, beta);
                        if(indexTOF700!=-1){
                           fHM->H2("False velocity from P TOF-700 for " + recoName)->Fill(p, velocity);
                           fHM->H2("False time from P TOF-700 for " + recoName)->Fill(p, time);
                           fHM->H2("False mass^2 from P TOF-700 for " + recoName)->Fill(p, mass);
                        }                    
                }  
            }
            if(fMassTable.count(mcMass)!=0){
                ++fPidStatistics700[mcName][0]; // +total true particle number (mc);
                fHM->H1("TOF-700 total_vs_P for " + mcName)->Fill(p);
                fHM->H1("TOF-700 total_vs_NOfHits for " + mcName)->Fill(nOfHits);
                fHM->H2("TOF-700 total rigidity-momentum for " + (mcName))->Fill(p, beta);
                if(indexTOF700!=-1){
                        fHM->H2("Total velocity from P TOF-700 for " + mcName)->Fill(p, velocity);
                        fHM->H2("Total time from P TOF-700 for " + mcName)->Fill(p, time);
                        fHM->H2("Total mass^2 from P TOF-700 for " + mcName)->Fill(p, mass);
                }
            }
        }   
    }
}

Int_t BmnPidQa::EnumToPdg(PidParticles part){
    switch(part)
    {
        case PidProton:
            return 2212;
        case PidPion:
            return 211;
        case PidKaon:
            return 321;
        case PidElectron:
            return 11;
        case PidDeuteron:
            return 1000010020;
        case PidTriton:
            return 1000010030;
       case PidHelium3:
            return 1000020030;
        case PidHelium4:
            return 1000020040;
        default:
            cout << "No such particle in PidParticles\n";
            return -1;
    }
}

TParticlePDG* BmnPidQa::GetParticleExtend(Int_t pdgCode){
    for (auto sort = fParticles.begin(); sort != fParticles.end(); ++sort)
        if((*sort)->PdgCode() == pdgCode)
            return *sort;
        return db->GetParticle(pdgCode);
}

string BmnPidQa::GetParticleName(Double_t mass){
    for (auto iter = fMassTable.begin(); iter != fMassTable.end(); ++iter){
            Double_t tableMass = iter->first;
            if(abs(tableMass - mass) < 0.000001)
                return iter->second;
        }
    return "NULL";
}

void BmnPidQa::MassTablePrint(){
    for (auto iter = fMassTable.begin(); iter != fMassTable.end(); ++iter){
        cout << iter->first << ": " << iter->second << " " << endl;
    }
}
void BmnPidQa::PidStatisticsPrint400(){
    for (auto iter = fPidStatistics400.begin(); iter != fPidStatistics400.end(); ++iter)
        cout << iter->first << " " << iter->second[0] << " " << iter->second[1] << " " << iter->second[2]<< endl;
}
void BmnPidQa::PidStatisticsPrint700(){
    for (auto iter = fPidStatistics700.begin(); iter != fPidStatistics700.end(); ++iter)
        cout << iter->first << " " << iter->second[0] << " " << iter->second[1] << " " << iter->second[2] << endl;
}
    

ClassImp(BmnPidQa);
