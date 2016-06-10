/**
 * \file BmnHistManager.h
 * \brief Histogram manager.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#ifndef BMNHISTMANAGER_H_
#define BMNHISTMANAGER_H_

#include "TObject.h"
#include <iostream>
#include <map>
#include "TString.h"
#include <vector>
#include <cassert>
#include <utility>
#include <functional>

class TFile;
class TNamed;
class TH1;
class TH2;
class TGraph;
class TGraph2D;
class TProfile;
class TProfile2D;

using std::map;
using std::make_pair;
using std::string;
using std::vector;

/**
 * \class BmnHistManager
 * \brief Histogram manager.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class BmnHistManager : public TObject
{
public:

   /**
    * \brief Constructor.
    */
   BmnHistManager();

   /**
    * \brief Destructor.
    */
   virtual ~BmnHistManager();

   /**
    * \brief Add new named object to manager.
    * \param[in] name Name of the object.
    * \param[in] object Pointer to object.
    */
   void Add(const TString& name, TNamed* object) {
         std::pair<TString, TNamed*> newpair = std::make_pair(name, object);
      fMap.insert(newpair);
   }

   /**
    * \brief Helper function for creation of 1-dimensional histograms and profiles.
    * Template argument is a real object type that has to be created, for example,
    * Create1<TH1F>("name", "title", 100, 0, 100);
    * \param[in] name Object name.
    * \param[in] title Object title.
    * \param[in] nofBins Number of bins.
    * \param[in] minBin Low axis limit.
    * \param[in] maxBin Upper axis limit.
    */
   template<class T> void Create1(
         const TString& name,
         const TString& title,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin) {
		T* h = new T(name.Data(), title.Data(), nofBins, minBin, maxBin);
		Add(name, h);
	}

   /**
    * \brief Helper function for creation of 2-dimensional histograms and profiles.
    * Template argument is a real object type that has to be created, for example,
    * Create2<TH2F>("name", "title", 100, 0, 100, 200, 0, 200);
    * \param[in] name Object name.
    * \param[in] title Object title.
    * \param[in] nofBinsX Number of bins for X axis.
    * \param[in] minBinX Low X axis limit.
    * \param[in] maxBinX Upper X axis limit.
    * \param[in] nofBinsY Number of bins for Y axis.
    * \param[in] minBinY Low Y axis limit.
    * \param[in] maxBinY Upper Y axis limit.
    */
   template<class T> void Create2(
         const TString& name,
         const TString& title,
         Int_t nofBinsX,
         Double_t minBinX,
         Double_t maxBinX,
         Int_t nofBinsY,
         Double_t minBinY,
         Double_t maxBinY) {
   	T* h = new T(name.Data(), title.Data(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
   	Add(name, h);
   }

   /**
    * \brief Return pointer to TH1 histogram.
    * \param[in] name Name of histogram.
    * \return pointer to TH1 histogram.
    */
   TH1* H1(const TString& name) const {
      if (fMap.count(name) == 0) { // Temporarily used for debugging
    	  std::cout << "Error: BmnHistManager::H1(name): name=" << name << std::endl;
      }
      assert(fMap.count(name) != 0);
      return (TH1*) fMap.find(name)->second;
   }

   /**
    * \brief Return pointer to TH2 histogram.
    * \param[in] name Name of histogram.
    * \return pointer to TH1 histogram.
    */
   TH2* H2(const TString& name) const {
      if (fMap.count(name) == 0) { // Temporarily used for debugging
    	  std::cout << "Error: BmnHistManager::H2(name): name=" << name << std::endl;
      }
      assert(fMap.count(name) != 0);
      return (TH2*) fMap.find(name)->second;
   }

   /**
    * \brief Check existence of histogram in manager.
    * \param[in] name Name of histogram.
    * \return True if histogram exists in manager.
    */
   Bool_t Exists(const TString& name) const {
      return (fMap.count(name) == 0) ? false : true;
   }

   /**
    * \brief Write all histograms to current opened file.
    */
   void WriteToFile();

   /**
    * \brief Read histograms from file.
    * \param[in] file Pointer to file with histograms.
    */
   void ReadFromFile(
         TFile* file);

   /**
    * \brief Clear memory. Remove all histograms.
    */
   void Clear();

   /**
    * \brief Shrink empty bins in H1.
    * \param[in] histName Name of histogram.
    */
   void ShrinkEmptyBinsH1(const TString& histName);


   /**
    * \brief Shrink empty bins in H2.
    * \param[in] histName Name of histogram.
    */
   void ShrinkEmptyBinsH2(const TString& histName);

   /**
    * \brief Scale histogram.
    * \param[in] histName Name of histogram.
    * \param[in] scale Scaling factor.
    */
   void Scale(const TString& histName, Double_t scale);
   
   /**
    * \brief Normalize histogram to integral.
    * \param[in] histName Name of histogram.
    */
   void NormalizeToIntegral(const TString& histName);

      /**
    * \brief Rebin histogram.
    * \param[in] histName Name of histogram.
    * \param[in] ngroup Rebining factor.
    */
   void Rebin(const TString& histName, Int_t ngroup);

   /**
    * \brief Return string representation of class.
    * \return string representation of class.
    */
   TString ToString() const;

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const BmnHistManager& histManager) {
      strm << histManager.ToString();
      return strm;
   }

private:

   // Map of histogram (graph) name to its pointer
   map<TString, TNamed*> fMap;

   ClassDef(BmnHistManager, 1)
};

#endif /* BmnHISTMANAGER_H_ */
