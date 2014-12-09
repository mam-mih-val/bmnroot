/**
 * \brief BmnTrackPropagator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for track propagation algorithm.
 */

#ifndef BMNTRACKPROPAGATOR_H_
#define BMNTRACKPROPAGATOR_H_

#include "BmnEnums.h"
#include "FairTrackParam.h"
#include "BmnGeoNavigator.h"
#include "BmnTrackExtrapolator.h"
#include "BmnMaterialEffects.h"
#include "TString.h"

#include <vector>
#include <cstddef>

using namespace std;

/**
 * \brief BmnTrackPropagator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for track propagation algorithm.
 * Propagation algorithm has to take into account material and
 * can use BmnTrackExtrapolator to extrapolate tracks between material layers.
 */
class BmnTrackPropagator
{
public:
   /**
    * \brief Constructor.
    */
   BmnTrackPropagator();

   /**
    * \brief Destructor.
    */
   virtual ~BmnTrackPropagator();
   
   BmnStatus TGeoTrackPropagate(FairTrackParam* parOut, Float_t zOut, Int_t pdg, vector<Double_t>* F, Float_t* length, TString type);
   Bool_t IsParCorrect( const FairTrackParam* par);
   void UpdateF(vector<Double_t>& F, const vector<Double_t>& newF);
   
private:
    BmnGeoNavigator* fNavigator;
    BmnTrackExtrapolator* fExtrapolator;
    BmnMaterialEffects* fMaterial;
};

#endif
