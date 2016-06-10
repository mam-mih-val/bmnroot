/**
 * \file BmnHistManager.cxx
 * \brief Histogram manager.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#include "BmnHistManager.h"
#include "TH1.h"
#include "TH2.h"
#include "TNamed.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TClass.h"
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <limits>

using namespace std;

class CompareTNamedMore:
   public std::binary_function<
   const TNamed*,
   const TNamed*,
   Bool_t>
{
public:
   Bool_t operator()(const TNamed* object1, const TNamed* object2) const {
      return TString(object1->GetName()) > TString(object2->GetName());
   }
};

BmnHistManager::BmnHistManager(): fMap() {}

BmnHistManager::~BmnHistManager() {}

void BmnHistManager::WriteToFile() {
   map<TString, TNamed*>::iterator it;
   for (it = fMap.begin(); it != fMap.end(); it++){
      it->second->Write();
   }
}

void BmnHistManager::ReadFromFile(TFile* file) {
   assert(file != NULL);
   cout << "-I- BmnHistManager::ReadFromFile" << endl;
   TDirectory* dir = gDirectory;
   TIter nextkey(dir->GetListOfKeys());
   TKey *key;
   Int_t c = 0;
   while (key = (TKey*) nextkey()) {
      TObject* obj = key->ReadObj();
      if (obj->IsA()->InheritsFrom (TH1::Class()) || obj->IsA()->InheritsFrom (TGraph::Class()) || obj->IsA()->InheritsFrom (TGraph2D::Class())) {
         TNamed* h = (TNamed*) obj;
         TNamed* h1 = (TNamed*)file->Get(h->GetName());
         Add(TString(h->GetName()), h1);
      }
   }
}

void BmnHistManager::Clear() {
   map<TString, TNamed*>::iterator it;
   for (it = fMap.begin(); it != fMap.end(); it++) {
      delete (*it).second;
   }
   fMap.clear();
}

void BmnHistManager::ShrinkEmptyBinsH1(const TString& histName)
{
   TH1* hist = H1(histName);
   Int_t nofBins = hist->GetNbinsX();
   Int_t minShrinkBin = std::numeric_limits<Int_t>::max();
   Int_t maxShrinkBin = std::numeric_limits<Int_t>::min();
   Bool_t isSet = false;
   for (Int_t iBin = 1; iBin <= nofBins; iBin++) {
      Double_t content = hist->GetBinContent(iBin);
      if (content != 0.) {
         minShrinkBin = std::min(iBin, minShrinkBin);
         maxShrinkBin = std::max(iBin, maxShrinkBin);
         isSet = true;
      }
   }
   if (isSet) {
      hist->GetXaxis()->SetRange(minShrinkBin, maxShrinkBin);
      hist->GetYaxis()->SetRange(minShrinkBin, maxShrinkBin);
   }
}

void BmnHistManager::ShrinkEmptyBinsH2(const TString& histName) {
   TH1* hist = H2(histName);
   Int_t nofBinsX = hist->GetNbinsX();
   Int_t nofBinsY = hist->GetNbinsY();
   Int_t minShrinkBinX = std::numeric_limits<Int_t>::max();
   Int_t maxShrinkBinX = std::numeric_limits<Int_t>::min();
   Int_t minShrinkBinY = std::numeric_limits<Int_t>::max();
   Int_t maxShrinkBinY = std::numeric_limits<Int_t>::min();
   Bool_t isSet = false;
   for (Int_t iBinX = 1; iBinX <= nofBinsX; iBinX++) {
      for (Int_t iBinY = 1; iBinY <= nofBinsY; iBinY++) {
         Double_t content = hist->GetBinContent(iBinX, iBinY);
         if (content != 0.) {
            minShrinkBinX = std::min(iBinX, minShrinkBinX);
            maxShrinkBinX = std::max(iBinX, maxShrinkBinX);
            minShrinkBinY = std::min(iBinY, minShrinkBinY);
            maxShrinkBinY = std::max(iBinY, maxShrinkBinY);
            isSet = true;
         }
      }
   }
   if (isSet) {
      hist->GetXaxis()->SetRange(minShrinkBinX, maxShrinkBinX);
      hist->GetYaxis()->SetRange(minShrinkBinY, maxShrinkBinY);
   }
}

void BmnHistManager::Scale(const TString& histName, Double_t scale) {
	H1(histName)->Scale(scale);
}

void BmnHistManager::NormalizeToIntegral(const TString& histName) {
   TH1* hist = H1(histName);
   hist->Scale(1. / hist->Integral());
}

void BmnHistManager::Rebin(const TString& histName, Int_t ngroup) {
	TH1* hist = H1(histName);
	if (ngroup > 1) {
		hist->Rebin(ngroup);
		hist->Scale(1. / (Double_t)ngroup);
	}
}

TString BmnHistManager::ToString() const
{
	TString str = "BmnHistManager list of histograms:\n";
	map<TString, TNamed*>::const_iterator it;
	for (it = fMap.begin(); it != fMap.end(); it++){
		str += it->first + "\n";
	}
	return str;
}

ClassImp(BmnHistManager)
