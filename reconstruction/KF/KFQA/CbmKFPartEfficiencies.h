#ifndef CbmKFPartEfficiencies_H
#define CbmKFPartEfficiencies_H

#include "TNamed.h"
#include "CbmL1Counters.h"

class CbmKFPartEfficiencies: public TNamed
{
 public:

  CbmKFPartEfficiencies():
  partDaughterPdg(),
    names(),
    indices(),
    fPdgToIndex(),
    ratio_reco(),
    mc(),
    reco(),
    ratio_ghost(),
    ratio_bg(),
    ratio_clone(),
    ghost(),
    bg(),
    clone()
  {
          // add total efficiency
    // AddCounter("piPlus"  ,"PiPlus  efficiency");
    // AddCounter("piMinus" ,"PiMinus efficiency");
    int mPartPDG[nParticles] = {310,3122,-3122,3312,-3312,3322,-3322,3334,-3334,3212,-3212,3222,-3222, //strange meson and hyperons
                                313,-313,323,-323, //K* resonances
                                3224,3114,-3114,-3224, 3214, -3214,//sigma resonances
                                3124,-3124, //Lambda resonances
                                3324, -3324, 1003314, -1003314, 3314, -3314, //Xi resonances
                                1003334, -1003334, //Omega resonances
                                3000, //exotics
                                333,113, //vector mesons, hadron chanel
                                100113, 200113, //light vector mesons
                                22, //dielectrons
                                111,221, //pi0, eta
                                443,100443, // J/Psi
                                421,-421,100421,-100421, //D0
                                411,-411, //D+, D-
                                431,-431, //Ds+, Ds-
                                4122,-4122, //Lambdac
                                10421, -10421, 10411, -10411, 20411, -20411,
                                3001 //H->Lambda p pi
                               };
    TString mPartName[nParticles] = {"ks","lambda","lambdab","xi-","xi+","xi0","xi0b","omega-","omega+","#Sigma^0","#Sigma^0b", "#Sigma^+", "#Sigma^+b",
                                     "k*0","k*0b","k*+","k*-",
                                     "sigma*+","sigma*-","sigma*+b","sigma*-b","sigma*0","sigma*0b",
                                     "lambda*","lambda*b",
                                     "xi*0", "xi*0b", "xi*-_{#Lambda,K}", "xi*+_{#Lambda,K}", "xi*-_{#xi-,#pi0}", "xi*+_{#xi+,#pi0}",
                                     "omega*-","omega*+",
                                     "Hdb",
                                     "phi_{KK}", "rho_{#pi#pi}",
                                     "rho_{ee}", "rho_{#mu#mu}",
                                     "gamma",
                                     "#pi^{0}","eta",
                                     "J#Psi_ee","J#Psi_#mu#mu",
                                     "D0","D0b","D0_4","D0b_4",
                                     "D+","D-",
                                     "Ds+","Ds-",
                                     "lambdac", "lambdacb",
                                     "D*0", "D*0b", "D*+", "D*-", "D*+_4", "D*-_4",
                                     "H0"
                                    };
    TString mPartTitle[nParticles] = {"KShort   ", //0
                                      "Lambda   ", //1
                                      "Lambda b ", //2
                                      "Xi-      ", //3
                                      "Xi+      ", //4
                                      "Xi0      ", //5
                                      "Xi0 b    ", //6
                                      "Omega-   ", //7
                                      "Omega+   ", //8
                                      "Sigma0   ", //9
                                      "Sigma0 b ", //10
                                      "Sigma+   ", //11
                                      "Sigma+ b ", //12
                                      "K*0      ", //13
                                      "K*0 b    ", //14
                                      "K*+      ", //15
                                      "K*-      ", //16
                                      "Sigma*+  ", //17
                                      "Sigma*-  ", //18
                                      "Sigma*+ b", //19
                                      "Sigma*- b", //20
                                      "Sigma*0  ", //21
                                      "Sigma*0 b", //22
                                      "Lambda*  ", //23
                                      "Lambda* b", //24
                                      "Xi*0     ", //25
                                      "Xi*0 b   ", //26
                                      "Xi*-_lk  ", //27
                                      "Xi*+_lk  ", //28
                                      "Xi*-_XiPi", //29
                                      "Xi*+_XiPi", //30
                                      "Omega*-  ", //31
                                      "Omega*+  ", //32
                                      "Hdb      ", //33
                                      "phi_kk   ", //34
                                      "rho_pipi ", //35
                                      "rho_ee   ", //36
                                      "rho_mm   ", //37
                                      "gamma    ", //38
                                      "Pi0      ", //39
                                      "eta      ", //40
                                      "JPsi_ee  ", //41
                                      "JPsi_mm  ", //42
                                      "D0       ", //43
                                      "D0b      ", //44
                                      "D0_4     ", //45
                                      "D0b_4    ", //46
                                      "D+       ", //47
                                      "D-       ", //48
                                      "Ds+      ", //49
                                      "Ds-      ", //50
                                      "Lambdac  ", //51
                                      "Lambdac b", //52
                                      "D*0      ", //53
                                      "D*0 b    ", //54
                                      "D*+      ", //55
                                      "D*-      ", //56
                                      "D*+_4    ", //57
                                      "D*-_4    ", //58
                                      "H0       " //59
                                     };

    float mPartMHistoMin[nParticles] = {0.3, 1., 1., 1., 1., 1., 1.,1.,1.,1.,1.,1.,1.,
                                        0.6, 0.6, 0.6, 0.6,
                                        1.,1.,1.,1.,1.,1.,
                                        1.4, 1.4,
                                        1.4, 1.4, 1.4, 1.4, 1.4, 1.4,
                                        1.8,1.8,
                                        1.,
                                        0.6, 0.1,
                                        0.1, 0.1,
                                        0.,
                                        0.,0.,
                                        1.,1.,
                                        1.,1.,1.,1.,
                                        1.,1.,
                                        1.,1.,
                                        1.8,1.8,
                                        1.8,1.8,1.8,1.8,1.8,1.8,
                                        1.};
    float mPartMHistoMax[nParticles] = {1.3, 2., 2., 3., 3., 3., 3., 3., 3.,3.,3.,3.,3.,
                                        2.6, 2.6, 2.6, 2.6,
                                        3., 3., 3., 3.,3.,3.,
                                        3.4, 3.4,
                                        3.4, 3.4, 3.4, 3.4, 3.4, 3.4,
                                        3.8, 3.8,
                                        3.,
                                        1.6, 2.1,
                                        2.1, 2.1,
                                        3.,
                                        3.,3.,
                                        4.,4.,
                                        3.,3.,3.,3.,
                                        3.,3.,
                                        3.,3.,
                                        3.8,3.8,
                                        3.8,3.8,3.8,3.8,3.8,3.8,
                                        3.};
    //set decay mode
    partDaughterPdg.resize(nParticles);

    partDaughterPdg[ 0].push_back(  211); //K0s -> pi+ pi-
    partDaughterPdg[ 0].push_back( -211);

    partDaughterPdg[ 1].push_back( 2212); //Lambda -> p pi-
    partDaughterPdg[ 1].push_back( -211);

    partDaughterPdg[ 2].push_back(-2212); //Lambda_bar -> p- pi+
    partDaughterPdg[ 2].push_back(  211);

    partDaughterPdg[ 3].push_back( 3122); //Xi- -> Lambda pi-
    partDaughterPdg[ 3].push_back( -211);

    partDaughterPdg[ 4].push_back(-3122); //Xi+ -> Lambda_bar pi+
    partDaughterPdg[ 4].push_back(  211);

    partDaughterPdg[ 5].push_back( 3122); //Xi0 -> Lambda pi0
    partDaughterPdg[ 5].push_back(  111);

    partDaughterPdg[ 6].push_back(-3122); //Xi0_bar -> Lambda_bar pi0
    partDaughterPdg[ 6].push_back(  111);

    partDaughterPdg[ 7].push_back( 3122); //Omega- -> Lambda K-
    partDaughterPdg[ 7].push_back( -321);

    partDaughterPdg[ 8].push_back(-3122); //Omega+ -> Lambda_bar K+
    partDaughterPdg[ 8].push_back(  321);

    partDaughterPdg[ 9].push_back(   22); //Sigma0 -> Lambda Gamma
    partDaughterPdg[ 9].push_back( 3122);

    partDaughterPdg[10].push_back(   22); //Sigma0_bar -> Lambda_bar Gamma
    partDaughterPdg[10].push_back(-3122);

    partDaughterPdg[11].push_back(  111); //Sigma+ -> p pi0
    partDaughterPdg[11].push_back( 2212);

    partDaughterPdg[12].push_back(  111); //Sigma+_bar -> p- pi0
    partDaughterPdg[12].push_back(-2212);

    partDaughterPdg[13].push_back(  321); //K*0 -> K+ pi-
    partDaughterPdg[13].push_back( -211);

    partDaughterPdg[14].push_back( -321); //K*0_bar -> K- pi+
    partDaughterPdg[14].push_back(  211);

    partDaughterPdg[15].push_back(  310); //K*+ -> K0s pi+
    partDaughterPdg[15].push_back(  211);

    partDaughterPdg[16].push_back(  310); //K*- -> K0s pi-
    partDaughterPdg[16].push_back( -211);

    partDaughterPdg[17].push_back( 3122); //Sigma+ -> Lambda pi+
    partDaughterPdg[17].push_back(  211);

    partDaughterPdg[18].push_back( 3122); //Sigma- -> Lambda pi-
    partDaughterPdg[18].push_back( -211);

    partDaughterPdg[19].push_back(-3122); //Sigma+_bar -> Lambda_bar pi+
    partDaughterPdg[19].push_back(  211);

    partDaughterPdg[20].push_back(-3122); //Sigma-_bar -> Lambda_bar pi-
    partDaughterPdg[20].push_back( -211);

    partDaughterPdg[21].push_back( 3122); //Sigma*0 -> Lambda pi0
    partDaughterPdg[21].push_back(  111);

    partDaughterPdg[22].push_back(-3122); //Sigma*0_bar -> Lambda_bar pi0
    partDaughterPdg[22].push_back(  111);

    partDaughterPdg[23].push_back( 2212); //Lambda* -> p K-
    partDaughterPdg[23].push_back( -321);

    partDaughterPdg[24].push_back(-2212); //Lambda*_bar -> p- K+
    partDaughterPdg[24].push_back(  321);

    partDaughterPdg[25].push_back( 3312); //Xi*0 -> Xi- pi+
    partDaughterPdg[25].push_back(  211);

    partDaughterPdg[26].push_back(-3312); //Xi*0_bar -> Xi+ pi-
    partDaughterPdg[26].push_back( -211);

    partDaughterPdg[27].push_back( 3122); //Xi*- -> Lambda K-
    partDaughterPdg[27].push_back( -321);

    partDaughterPdg[28].push_back(-3122); //Xi*+ -> Lambda_bar K+
    partDaughterPdg[28].push_back(  321);

    partDaughterPdg[29].push_back( 3312); //Xi*- -> Xi- pi0
    partDaughterPdg[29].push_back(  111);

    partDaughterPdg[30].push_back(-3312); //Xi*+ -> Xi+ pi0
    partDaughterPdg[30].push_back(  111);

    partDaughterPdg[31].push_back( 3312); //Omega*- -> Xi- pi+ K-
    partDaughterPdg[31].push_back(  211);
    partDaughterPdg[31].push_back( -321);

    partDaughterPdg[32].push_back(-3312); //Omega*- -> Xi+ pi- K+
    partDaughterPdg[32].push_back( -211);
    partDaughterPdg[32].push_back(  321);

    partDaughterPdg[33].push_back( 3122); //H-dibar -> Lambda Lambda
    partDaughterPdg[33].push_back( 3122);

    partDaughterPdg[34].push_back(  321); //phi -> K+ K-
    partDaughterPdg[34].push_back( -321);

    partDaughterPdg[35].push_back(  211); //rho, omega, phi -> pi+ pi-
    partDaughterPdg[35].push_back( -211);

    partDaughterPdg[36].push_back(   11); //rho, omega, phi -> e+ e-
    partDaughterPdg[36].push_back(  -11);

    partDaughterPdg[37].push_back(   13); //rho, omega, phi -> mu+ mu-
    partDaughterPdg[37].push_back(  -13);

    partDaughterPdg[38].push_back(   11); //gamma -> e+ e-
    partDaughterPdg[38].push_back(  -11);

    partDaughterPdg[39].push_back(   22); //pi0 -> gamma gamma
    partDaughterPdg[39].push_back(   22);

    partDaughterPdg[40].push_back(  111); //eta -> pi0 pi0
    partDaughterPdg[40].push_back(  111);
    partDaughterPdg[40].push_back(  111);

    partDaughterPdg[41].push_back(   11); //JPsi -> e+ e-
    partDaughterPdg[41].push_back(  -11);

    partDaughterPdg[42].push_back(   13); //JPsi -> mu+ mu-
    partDaughterPdg[42].push_back(  -13);

    partDaughterPdg[43].push_back(  211); //D0 -> pi+ K-
    partDaughterPdg[43].push_back( -321);

    partDaughterPdg[44].push_back( -211); //D0_bar -> K+ pi-
    partDaughterPdg[44].push_back(  321);

    partDaughterPdg[45].push_back(  211); //D0 -> pi+ pi+ pi- K-
    partDaughterPdg[45].push_back(  211);
    partDaughterPdg[45].push_back( -211);
    partDaughterPdg[45].push_back( -321);

    partDaughterPdg[46].push_back( -211); //D0_bar -> pi- pi- pi+ K+
    partDaughterPdg[46].push_back( -211);
    partDaughterPdg[46].push_back(  211);
    partDaughterPdg[46].push_back(  321);

    partDaughterPdg[47].push_back( -321); //D+ -> K- pi+ pi+
    partDaughterPdg[47].push_back(  211);
    partDaughterPdg[47].push_back(  211);

    partDaughterPdg[48].push_back(  321); //D- -> K+ pi- pi-
    partDaughterPdg[48].push_back( -211);
    partDaughterPdg[48].push_back( -211);

    partDaughterPdg[49].push_back( -321); //Ds+ -> K- K+ pi+
    partDaughterPdg[49].push_back(  321);
    partDaughterPdg[49].push_back(  211);

    partDaughterPdg[50].push_back(  321); //Ds- -> K+ K- pi-
    partDaughterPdg[50].push_back( -321);
    partDaughterPdg[50].push_back( -211);

    partDaughterPdg[51].push_back(  211); //Lambdac -> pi+ K- p
    partDaughterPdg[51].push_back( -321);
    partDaughterPdg[51].push_back( 2212);

    partDaughterPdg[52].push_back( -211); //Lambdac_bar -> pi- K+ p-
    partDaughterPdg[52].push_back(  321);
    partDaughterPdg[52].push_back(-2212);

    partDaughterPdg[53].push_back(  411); //D*0 -> D+ pi-
    partDaughterPdg[53].push_back( -211);

    partDaughterPdg[54].push_back( -411); //D*0_bar -> D- pi+
    partDaughterPdg[54].push_back(  211);

    partDaughterPdg[55].push_back(  421); //D*+ -> D0 pi+
    partDaughterPdg[55].push_back(  211);

    partDaughterPdg[56].push_back( -421); //D*- -> D0_bar pi-
    partDaughterPdg[56].push_back( -211);

    partDaughterPdg[57].push_back(  421); //D*+ -> D04 pi+
    partDaughterPdg[57].push_back(  211);

    partDaughterPdg[58].push_back( -421); //D*- -> D04_bar pi-
    partDaughterPdg[58].push_back( -211);

    partDaughterPdg[59].push_back( 3122); //H0-> Lambda pi- p
    partDaughterPdg[59].push_back( -211);
    partDaughterPdg[59].push_back( 2212);

    for(int iP=0; iP<nParticles; iP++)
    {
      partPDG[iP] = mPartPDG[iP];
      partName[iP] = mPartName[iP];
      partTitle[iP] = mPartTitle[iP];
      partMHistoMin[iP] = mPartMHistoMin[iP];
      partMHistoMax[iP] = mPartMHistoMax[iP];
    }

    for(int iP=0; iP<nParticles; iP++)
    {
      AddCounter(Form("%s",partName[iP].Data()), Form("%-*s",14,partTitle[iP].Data()));
      AddCounter(Form("%s_prim",partName[iP].Data()), Form("%s Prim",partTitle[iP].Data()));
      AddCounter(Form("%s_sec",partName[iP].Data()), Form("%s Sec ",partTitle[iP].Data()));
    }

    for(int iP=0; iP<nParticles; iP++)
      fPdgToIndex[mPartPDG[iP]] = iP;
  }

  virtual ~CbmKFPartEfficiencies(){};

  int GetParticleIndex(int pdg)
  {
    std::map<int, int>::iterator it;
    it=fPdgToIndex.find(pdg);
    if(it != fPdgToIndex.end()) return it->second;
    else return -1;
  }

  virtual void AddCounter(TString shortname, TString name){
    indices[shortname] = names.size();
    names.push_back(name);

    ratio_reco.AddCounter();
    mc.AddCounter();
    reco.AddCounter();

    ratio_ghost.AddCounter();
    ratio_bg.AddCounter();
    ratio_clone.AddCounter();
    ghost.AddCounter();
    bg.AddCounter();
    clone.AddCounter();
  };

  CbmKFPartEfficiencies& operator+=(CbmKFPartEfficiencies& a){
    mc += a.mc; reco += a.reco;
    ghost += a.ghost; bg += a.bg; clone += a.clone;
    return *this;
  };
  
  void CalcEff(){
    ratio_reco = reco/mc;

    TL1TracksCatCounters<int> allReco = reco + ghost + bg;
    ratio_ghost = ghost/allReco;
    ratio_bg  = bg/allReco;
    ratio_clone  = clone/allReco;
  };
  

  void Inc(bool isReco, int nClones, TString name)
  {
    const int index = indices[name];
    
    mc.counters[index]++;
    if (isReco) reco.counters[index]++;
    if(nClones > 0)
      clone.counters[index] += nClones;
  };

  void IncReco(bool isGhost, bool isBg, TString name){
    const int index = indices[name];

    if (isGhost) ghost.     counters[index]++;
    if (isBg)    bg.counters[index]++;
  };

  void PrintEff(){
    std::cout.setf(ios::fixed);
    std::cout.setf(ios::showpoint);
    std::cout.precision(3);
    std::cout << "Particle     : "
         << "   Eff "
         <<" / "<< " Ghost "
         <<" / "<< "BackGr "
         <<" / "<< "N Ghost"
         <<" / "<< "N BackGr"
         <<" / "<< "N Reco "
         <<" / "<< "N Clone "
         <<" | "<< "  N MC "  << std::endl;
    
    int NCounters = mc.NCounters;
    for (int iC = 0; iC < NCounters; iC++){
        std::cout << names[iC]
             << "  : " << setw(6) << ratio_reco.counters[iC]              
             << "  / " << setw(6) << ratio_ghost.counters[iC]  // particles w\o MCParticle
             << "  / " << setw(6) << ratio_bg.counters[iC]     // particles with incorrect MCParticle
             << "  / " << setw(6) << ghost.counters[iC]
             << "  / " << setw(7) << bg.counters[iC]
             << "  / " << setw(6) << reco.counters[iC]
             << "  / " << setw(7) << clone.counters[iC]
             << "  | " << setw(6) << mc.counters[iC]  << std::endl;
    }
  };

  friend std::fstream & operator<<(std::fstream &strm, CbmKFPartEfficiencies &a) {

    strm << a.ratio_reco;
    strm << a.mc;
    strm << a.reco;
    strm << a.ratio_ghost;
    strm << a.ratio_bg;
    strm << a.ratio_clone;
    strm << a.ghost;
    strm << a.bg;
    strm << a.clone;

    return strm;
  }

  friend std::fstream & operator>>(std::fstream &strm, CbmKFPartEfficiencies &a){

    strm >> a.ratio_reco;
    strm >> a.mc;
    strm >> a.reco;
    strm >> a.ratio_ghost;
    strm >> a.ratio_bg;
    strm >> a.ratio_clone;
    strm >> a.ghost;
    strm >> a.bg;
    strm >> a.clone;

    return strm;
  }

  void AddFromFile(TString fileName)
  {
    std::fstream file(fileName.Data(),fstream::in);
    file >> *this;
  }

  static const int nParticles = 60;
  int partPDG[nParticles];
  TString partName[nParticles];
  TString partTitle[nParticles];
  vector<vector<int> > partDaughterPdg;
  float partMHistoMin[nParticles];
  float partMHistoMax[nParticles];

  ClassDef(CbmKFPartEfficiencies,1);

 private:
  vector<TString> names; // names counters indexed by index of counter
  map<TString, int> indices; // indices of counters indexed by a counter shortname

  map<int, int> fPdgToIndex;

  TL1TracksCatCounters<double> ratio_reco;

  TL1TracksCatCounters<int> mc;
  TL1TracksCatCounters<int> reco;

  TL1TracksCatCounters<double> ratio_ghost;
  TL1TracksCatCounters<double> ratio_bg;
  TL1TracksCatCounters<double> ratio_clone;

  TL1TracksCatCounters<int> ghost;
  TL1TracksCatCounters<int> bg; // background
  TL1TracksCatCounters<int> clone; // background
};

#endif
