/** BmnTGeoNavigator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 **
 ** The class performs geometry navigation, which is based on the ROOT TGeoManager.
 **/

#ifndef BMNGEONAVIGATOR_H_
#define BMNGEONAVIGATOR_H_

#include "TObject.h"
#include "BmnMaterialInfo.h"
#include "FairTrackParam.h"
#include "BmnEnums.h"

#include <vector>

using namespace std;

class BmnGeoNavigator{
public:
    /* Constructor */
    BmnGeoNavigator();

    /* Destructor */
    virtual ~BmnGeoNavigator();

    BmnStatus FindIntersections(const FairTrackParam* par, Float_t zOut, vector<BmnMaterialInfo>& inter);

private:
    /* Sets the current position and direction for the TGeoManager
     * @param par Track parameter from which the current position and direction will be initialized.
     */
    void InitTrack(const FairTrackParam* par, Bool_t downstream) const;

    /* Make a step: if step=0 than to the next crossing boundary, else
     * make a step of length step.
     * @param step Step size [cm].
     * @return Crossed material information
     */
    BmnMaterialInfo MakeStep(Float_t step = 0.) const;

    /* Calculates length between current position and the point on the plane at zOut.
     * @param zOut Output Z position [cm]
     * @return Length.
     */
    Float_t CalcLength(Float_t zOut) const;
};

#endif /*BMNGEONAVIGATOR_H_*/
