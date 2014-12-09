/**
 * \file CbmLitFieldApproximationQa.h
 * \brief Field map approximation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/

#ifndef CBMLITFIELDAPPROXIMATIONQA_H_
#define CBMLITFIELDAPPROXIMATIONQA_H_

#include "FairTask.h"
#include <vector>
#include <string>

class FairField;
class CbmLitFieldFitter;
class CbmLitFieldGridCreator;
class CbmHistManager;

using std::vector;
using std::string;

/**
 * \class CbmLitFieldApproximationQa
 * \brief Field map approximation QA.
 *
 * Task checks magnetic field map.
 * Compares field map with its polynomial approximation
 * and with approximation based on grid.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
class CbmLitFieldApproximationQa: public FairTask {
public:
   /**
    * \brief Constructor.
    */
   CbmLitFieldApproximationQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldApproximationQa();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   /* Setters */
   void SetSliceZPosition(const vector<Double_t>& zPos) { fZSlicePosition = zPos; }
   void SetPolynomDegrees(const vector<UInt_t>& degrees) { fPolynomDegrees = degrees; }
   void IsFixedBounds(Bool_t fixedBounds) { fFixedBounds = fixedBounds; }
   void SetAcceptanceAngleX(double xangle) { fAcceptanceAngleX = xangle; }
   void SetAcceptanceAngleY(double yangle) { fAcceptanceAngleY = yangle; }
   void SetNofBinsX(int nofBinsX) { fNofBinsX = nofBinsX; }
   void SetNofBinsY(int nofBinsY) { fNofBinsY = nofBinsY; }
   void SetUseEllipseAcc(bool useEllipseAcc) { fUseEllipseAcc = useEllipseAcc; }
   void SetOutputDir(const string& dir) { fOutputDir = dir; }
   void SetPolynomDegreeIndex(unsigned int degreeIndex) { fPolynomDegreeIndex = degreeIndex; }

private:
   /**
    * \brief Create histograms.
    */
   void CreateHistos();

   /**
    * \brief Create field histograms.
    */
   void CreateFieldHistos();

   /**
    * \brief Create histograms for field approximation.
    */
   void CreateFitterHistos();

   /**
    * \brief Create histograms for grid creator.
    */
   void CreateGridHistos();

   /**
    * \brief Fill graphs and histos for field map for each field component (Bx, By, Bz, |B|).
    */
   void FillBHistos();

   /**
    * \brief Fill histograms for polynomial field approximation.
    */
   void FillFieldApproximationHistos();

   /**
    * \brief fill histograms for grid creator tool.
    */
   void FillGridCreatorHistos();

   // Pointer to the magnetic field map
   FairField* fField;

   // Number of slices along Z for field approximation
   Int_t fNofSlices;
   // Slice is defined as rectangle at a certain Z position
   // with upper left corner coordinate equals to [-X, -Y]
   // and lower right corner coordinate equals to [X, Y].
   // Z, X, Y coordinates for each slice.
   // [slice number]
   vector<Double_t> fZSlicePosition; // Z position of the slice
   vector<Double_t> fXSlicePosition; // X coordinate
   vector<Double_t> fYSlicePosition; // Y coordinate

   // Output directory for images
   string fOutputDir;

   //
   // Field approximation variables
   //
   // Acceptance angle for X
   Double_t fAcceptanceAngleX;
   // Acceptance angle for Y
   Double_t fAcceptanceAngleY;
   // Number of bins for X
   Int_t fNofBinsX;
   // Number of bins for Y
   Int_t fNofBinsY;
   // If true than only values inside an ellipse will be used for field approximation
   Bool_t fUseEllipseAcc;
   // Degree of the polynom
   UInt_t fPolynomDegreeIndex;
   // Number of polynoms for tests
   UInt_t fNofPolynoms;
   // Array with polynom degrees to be analyzed
   vector<UInt_t> fPolynomDegrees;
   // Field fitter tool for each polynom degree
   vector<CbmLitFieldFitter*> fFitter;

   //
   // Field grid
   //
   // Field grid creator tool
   CbmLitFieldGridCreator* fGridCreator;

   CbmHistManager* fHM; // Histogram manager

   Bool_t fFixedBounds; // Fixed bounds for error histograms

   CbmLitFieldApproximationQa(const CbmLitFieldApproximationQa&);
   CbmLitFieldApproximationQa& operator=(const CbmLitFieldApproximationQa&);
   
   ClassDef(CbmLitFieldApproximationQa, 1);
};

#endif /* CBMLITFIELDAPPROXIMATIONQA_H_ */
