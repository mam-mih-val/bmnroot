#ifndef _L1Timer_H
#define _L1Timer_H

/*
 *=====================================================
 *
 *  Authors: I.Kulakov
 *
 */

#include "TStopwatch.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::ios;
using std::setw;
using std::string;
using std::vector;

class TimerInfo {
 public:
  TimerInfo():fName(""),fReal(0.),fCpu(0.){ };
  TimerInfo( const string& name ):fName(name),fReal(0.),fCpu(0.){ };
  
  void operator =( TStopwatch& sw ) { fReal  = sw.RealTime(); fCpu  = sw.CpuTime(); };
  void operator+=( TStopwatch& sw ) { fReal += sw.RealTime(); fCpu += sw.CpuTime(); };
  void operator+=( const TimerInfo& t ){ fReal += t.fReal; fCpu += t.fCpu; }
  TimerInfo operator/( const float f ) const { TimerInfo r; r.fName = fName; r.fReal = fReal/f; r.fCpu = fCpu/f; return r; }
  
  // void Print(){ cout << fReal << "/" << fCpu; };
  void PrintReal(){ cout << fReal ; };
  float Real(){ return fReal; };
  string& Name(){ return fName; };
 private:
  string fName;
  float fReal, fCpu;
};

class L1CATFIterTimerInfo {
 public:
 L1CATFIterTimerInfo() : fNameToI(), fTIs() {}
  void Add( string name ) { fNameToI[name] = fTIs.size(); fTIs.push_back(TimerInfo(name)); };
  TimerInfo& operator[]( string name ) { return fTIs[fNameToI[name]]; };
  TimerInfo& operator[]( int i ) { return fTIs[i]; };
  void operator+=( L1CATFIterTimerInfo& t ){ for( unsigned int i = 0; i < fTIs.size(); ++i ) fTIs[i] += t[i]; }
  L1CATFIterTimerInfo operator/( float f ){
    L1CATFIterTimerInfo r;
    r.fNameToI = fNameToI;
    r.fTIs.resize(fTIs.size());
    for( unsigned int i = 0; i < fTIs.size(); ++i ) {
      r.fTIs[i] = fTIs[i]/f;
    }
    return r;
  }
  
  void PrintReal( int f = 0 ){
    if (f) { PrintNames(); cout << endl; }
    fTIs[0].PrintReal(); for( unsigned int i = 1; i < fTIs.size(); ++i ) { cout << " | " << setw(fTIs[i].Name().size()); fTIs[i].PrintReal(); }
    if (f) cout << endl;
  };
  void PrintNames(){ cout << fTIs[0].Name(); for( unsigned int i = 1; i < fTIs.size(); ++i ) { cout << " | " << fTIs[i].Name(); } };
 private:
  map< string, int > fNameToI;
  vector<TimerInfo> fTIs;
};

class L1CATFTimerInfo {
 public:
 L1CATFTimerInfo() : fTIIs(), fTIAll() {};
  void SetNIter( int n ) { fTIIs.resize(n); };

  void Add( string name ) {
    for( unsigned int i = 0; i < fTIIs.size(); ++i )
      fTIIs[i].Add(name);
    fTIAll.Add(name);
  }; // use after setniter
  L1CATFIterTimerInfo& GetTimerAll() { return fTIAll; };
  L1CATFIterTimerInfo& operator[]( int i ) { return fTIIs[i]; };
  void operator+=( L1CATFTimerInfo& t ){ for( unsigned int i = 0; i < fTIIs.size(); ++i ) fTIIs[i] += t[i]; fTIAll += t.GetAllInfo(); }
  L1CATFTimerInfo operator/( float f ){
    L1CATFTimerInfo r;
    r.fTIAll = fTIAll/f;
    r.SetNIter( fTIIs.size() );
    for( unsigned int i = 0; i < fTIIs.size(); ++i ) {
      r.fTIIs[i] = fTIIs[i]/f;
    }
    return r;
  }
  
  void Calc() {
    fTIAll = fTIIs[0];
    for( unsigned int i = 1; i < fTIIs.size(); ++i )
      fTIAll += fTIIs[i];
  }
  
  L1CATFIterTimerInfo& GetAllInfo() { return fTIAll; };
  void PrintReal() {
    cout.precision(1);
    cout.setf(ios::fixed);
    cout << " stage "<< " : "; fTIAll.PrintNames(); cout << endl;
    for( unsigned int i = 0; i < fTIIs.size(); ++i ) {
      cout << " iter " << i << " : "; fTIIs[i].PrintReal(); cout << endl;
    }
    cout << " all   "<< " : "; fTIAll.PrintReal(); cout << endl;
    
  };
 private:
  vector<L1CATFIterTimerInfo> fTIIs;
  L1CATFIterTimerInfo fTIAll;
};

#endif
  
