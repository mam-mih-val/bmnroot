#ifndef CbmL1Counters_H
#define CbmL1Counters_H

#include "TString.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
using std::ios;
using std::setw;
using std::vector;
using std::map;

  /// counters used for efficiency calculation
template <typename T>
struct TL1TracksCatCounters // counters for different tracks categories
{
  TL1TracksCatCounters():NCounters(0),counters(){ counters.clear(); };
  TL1TracksCatCounters(int nCounters):NCounters(nCounters),counters(){ counters.resize( NCounters, T(0)); };

  void AddCounter(){ NCounters++; counters.push_back(T(0)); };
  void AddCounters(int nCounters){ NCounters += nCounters; counters.resize( NCounters, T(0)); };
  
  TL1TracksCatCounters& operator+=(TL1TracksCatCounters& a){
    if (NCounters != a.NCounters){
      std::cout << " TL1TracksCatCounters: Error. Addition of counters of different sizes: " << NCounters << " " << a.NCounters << std::endl;
    }
    else{
      for (int iC = 0; iC < NCounters; iC++){
        counters[iC] += a.counters[iC];
      }
    }
    return *this;
  };

  TL1TracksCatCounters operator+(TL1TracksCatCounters& a){
    TL1TracksCatCounters res = *this;
    res += a;
    return res;
  };

  template <typename T2>
  TL1TracksCatCounters<double> operator/(TL1TracksCatCounters<T2>& a){
    TL1TracksCatCounters<double> b(NCounters);
    if (NCounters != a.NCounters){
      std::cout << " TL1TracksCatCounters: Error. Addition of counters of different sizes: " << NCounters << " " << a.NCounters << std::endl;
    }
    else{
      for (int iC = 0; iC < NCounters; iC++){
        b.counters[iC] = Div(counters[iC],a.counters[iC]);
      }
    }
    return b;
  };

  template <typename T2>
  TL1TracksCatCounters<T2> operator/(double a){
    TL1TracksCatCounters<T2> b(NCounters);
    for (int iC = 0; iC < NCounters; iC++){
      b.counters[iC] = static_cast<T2>( Div(counters[iC],a) );
    }
    return b;
  };

  friend std::fstream & operator<<(std::fstream &strm, const TL1TracksCatCounters<T> &a ){
    strm << a.NCounters << " " << a.counters.size() << " ";
    for(unsigned int iV=0; iV<a.counters.size(); iV++)
      strm << a.counters[iV] << " ";
    strm << std::endl;
    return strm;
  }

  friend std::ostream & operator<<(std::ostream &strm, const TL1TracksCatCounters<T> &a ){
    strm << a.NCounters << " " << a.counters.size() << " ";
    for(unsigned int iV=0; iV<a.counters.size(); iV++)
      strm << a.counters[iV] << " ";
    strm << std::endl;
    return strm;
  }

  friend std::fstream & operator>>(std::fstream &strm, TL1TracksCatCounters<T> &a ){
    int tmp;
    strm >> tmp;
    a.NCounters = tmp;
    strm >> tmp;
    a.counters.resize(tmp,T(0));
    for(int iV=0; iV<tmp; iV++)
    {
      T tmp1;
      strm >> tmp1;
      a.counters[iV] = tmp1;
    }
    return strm;
  }

 private:
  double Div(double a, double b){return (b > 0) ? a/b : -1.;};

 public:

  int NCounters;
  vector<T> counters;
};

struct TL1Efficiencies
{
  TL1Efficiencies():names(),indices(),ratio_reco(),ratio_ghosts(0),ratio_clones(0),mc(),reco(),ghosts(0),clones(0),nEvents(0){
    // you should add counter with shortname="total" !!
  };

  virtual ~TL1Efficiencies(){};

  virtual void AddCounter(TString shortname, TString name);
  
  TL1Efficiencies& operator+=(TL1Efficiencies& a);
  void CalcEff();
  void Inc(bool isReco, TString name); // increment counters according to parameters
  void IncNEvents(){ nEvents++; };
  
  void PrintEff();

  
  vector<TString> names; // names counters indexed by index of counter
  map<TString, int> indices; // indices of counters indexed by a counter shortname
  
  TL1TracksCatCounters<double> ratio_reco;
  double ratio_ghosts;
  double ratio_clones;
  
  TL1TracksCatCounters<int> mc;
  TL1TracksCatCounters<int> reco;
  int ghosts;
  int clones;
  int nEvents;
};

inline void TL1Efficiencies::AddCounter(TString shortname, TString name)
{
  indices[shortname] = names.size();
  names.push_back(name);
    
  ratio_reco.AddCounter();
  mc.AddCounter();
  reco.AddCounter();
}

inline void TL1Efficiencies::CalcEff()
{
  ratio_reco = reco/mc;
  const double total = reco.counters[indices["total"]] + ghosts + clones;
  if (total > 0){
    ratio_clones = clones/total;
    ratio_ghosts = ghosts/total;
  }
  else{
    ratio_clones = -1;
    ratio_ghosts = -1;
  }
};

inline TL1Efficiencies& TL1Efficiencies::operator+=(TL1Efficiencies& a)
{
  mc += a.mc; reco += a.reco;
  ghosts += a.ghosts; clones += a.clones;
  nEvents += a.nEvents;
    
  return *this;
};

inline void TL1Efficiencies::Inc(bool isReco, TString name)
{
  const int index = indices[name];
    
  mc.counters[index]++;
  if (isReco) reco.counters[index]++;
};

inline void TL1Efficiencies::PrintEff(){
  std::cout.setf(ios::fixed);
  std::cout.setf(ios::showpoint);
  std::cout.precision(3);
  std::cout.setf(ios::right);
  std::cout << "Track category         : " << " Eff "        <<" | "<< "All MC"  << std::endl;

  int NCounters = mc.NCounters;
  for (int iC = 0; iC < NCounters; iC++){
    std::cout << names[iC]  << "   : "
        << ratio_reco.counters[iC]
        << "  | " << mc.counters[iC]  << std::endl;
  }

  std::cout << "Clone     probability  : " << ratio_clones <<" | "<< clones << std::endl;
  std::cout << "Ghost     probability  : " << ratio_ghosts <<" | "<< ghosts << std::endl;
};

#endif
