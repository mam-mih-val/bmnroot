/** BmnKalmanFilter.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2007-2014
 **
 ** Class implements Kalman Filter update step.
 **/

#ifndef BMNKALMANFILTER_H_
#define BMNKALMANFILTER_H_

#include "BmnEnums.h"
#include "FairTrackParam.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmTofHit.h"
#include "CbmStsHit.h"
#include "BmnGemHit.h"
#include "BmnDchHit.h"
#include "BmnHit.h"

class CbmHit;

class BmnKalmanFilter
{
public:
   /* Constructor */
   BmnKalmanFilter();

   /* Destructor */
   virtual ~BmnKalmanFilter();

   BmnStatus Update(FairTrackParam* par, const BmnHit* hit, Float_t& chiSq);
   BmnStatus Update(FairTrackParam* par, const CbmTofHit* hit, Float_t& chiSq);
   BmnStatus Update(FairTrackParam* par, const CbmStsHit* hit, Float_t& chiSq);
   BmnStatus Update(FairTrackParam* par, const BmnGemHit* hit, Float_t& chiSq);
   BmnStatus Update(FairTrackParam* par, const BmnDchHit* hit, Float_t& chiSq);

private:
   /* Implements KF update step for pixel hits using gain matrix formalism
    *@param par Input/Output track parameter
    *@param hit Pointer to the hit
    *@param chiSq Output calculated chi square value
    *@return Status code */
   BmnStatus Update(FairTrackParam* par, const CbmPixelHit* hit, Float_t& chiSq);

   /* Implements KF update step for pixel hits using weighted means formalism
    *@param par Input/Output track parameter
    *@param hit Pointer to the hit
    *@param chiSq Output calculated chi square value
    *@return Status code */
   BmnStatus UpdateWMF(FairTrackParam* par, const CbmPixelHit* hit, Float_t& chiSq);

   /* Implements KF update step for strip hits using gain matrix formalism
    *@param par Input/Output track parameter
    *@param hit Pointer to the hit
    *@param chiSq Output calculated chi square value
    *@return Status code */
   BmnStatus Update(FairTrackParam* par, const CbmStripHit* hit, Float_t& chiSq);

   /* Implements KF update step for strip hits using weighted means formalism
    *@param par Input/Output track parameter
    *@param hit Pointer to the hit
    *@param chiSq Output calculated chi square value
    *@return Status code */
   BmnStatus UpdateWMF(FairTrackParam* par, const CbmStripHit* hit, Float_t& chiSq);
};

#endif //BmnKalmanFilter

