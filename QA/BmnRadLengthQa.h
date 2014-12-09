/**
 * \file CbmLitRadLengthQa.h
 * \brief FairTask for estimation of radiation length in the detector.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#ifndef CBMLITRADLENGTHQA_H_
#define CBMLITRADLENGTHQA_H_

#include "FairTask.h"
#include "base/CbmLitDetectorSetup.h"
#include <string>
#include <map>
using std::string;
using std::map;
using std::pair;
class CbmHistManager;
class TClonesArray;

class CbmLitRadLengthQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitRadLengthQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitRadLengthQa();

   /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
       Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

    /* Setters */
    void SetOutputDir(const string& outputDir) { fOutputDir = outputDir; }

private:
    /**
     * \brief Read data branches.
     */
    void ReadDataBranches();

    /**
     * \brief Create histograms.
     */
    void CreateHistograms();

    /**
     * \brief Execute total radiation length for a particular detector.
     * \param[in] pathPattern Path pattern for this detector in TGeomanager.
     * \param[in] detName Name of the detector, like in histograms.
     */
    void ExecDetector(
          const string& pathPattern,
          const string& detName);

    /**
     * \brief Exec additional radiation thickness calculation for tracking detectors.
     * \param[in] detName Name of the detector (Mvd, Sts, Much, Trd).
     * \param[in] getStationId Pointer to the function which return station ID by full path to the node.
     */
    void ExecDetector(
          const string& detName,
          Int_t (*getStationId)(const TString&));

    void FillHistosDetector(
          const map<Int_t, map<Int_t, Double_t> >& parMap,
          const string& histName,
          map<Int_t, map<Int_t, pair<Double_t, Double_t> > >& xyOnTrack);

    /**
     * \brief Return MVD station ID by path to the node or -1 in case node does not exists.
     * \param[in] nodePath Full path to the node.
     * \return MVD station ID by path to the node or -1 in case node does not exists.
     */
    static Int_t GetMvdStationId(
          const TString& nodePath);

    /**
     * \brief Return STS station ID by path to the node or -1 in case node does not exists.
     * \param[in] nodePath Full path to the node.
     * \return STS station ID by path to the node or -1 in case node does not exists.
     */
    static Int_t GetStsStationId(
          const TString& nodePath);

    /**
     * \brief Return TRD station ID by path to the node or -1 in case node does not exists.
     * \param[in] nodePath Full path to the node.
     * \return TRD station ID by path to the node or -1 in case node does not exists.
     */
    static Int_t GetTrdStationId(
          const TString& nodePath);

    /**
     * \brief Return MUCH station ID by path to the node or -1 in case node does not exists.
     * \param[in] nodePath Full path to the node.
     * \return MUCH station ID by path to the node or -1 in case node does not exists.
     */
    static Int_t GetMuchStationId(
          const TString& nodePath);

    /**
     * \brief Return MUCH absorber ID by path to the node or -1 in case node does not exists.
     * \param[in] nodePath Full path to the node.
     * \return MUCH absorber ID by path to the node or -1 in case node does not exists.
     */
    static Int_t GetMuchAbsorberId(
          const TString& nodePath);

    /**
     * \brief Save silicon equivalent histograms to a separate files for each detector.
     */
    void SaveMaterialBudgetToFile();

    void SaveDetectorMaterialBudgetToFile(
          const string& detName);

    // Material properties of silicon
    //static const Double_t SILICON_DENSITY = 2.33; // g*cm^-3
    //static const Double_t SILICON_A = 28.08855; // silicon atomic weight
    //static const Double_t SILICON_Z = 14.0; // silicon atomic number
    static const Double_t SILICON_RAD_LENGTH; // cm

    CbmHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results

    // Pointers to data arrays
    TClonesArray* fRadLen; // RadLen array

    CbmLitDetectorSetup fDet; // Detector setup

    CbmLitRadLengthQa(const CbmLitRadLengthQa&);
    CbmLitRadLengthQa& operator=(const CbmLitRadLengthQa&);
    
    ClassDef(CbmLitRadLengthQa, 1);
};

#endif /* CBMLITRADLENGTHQA_H_ */
