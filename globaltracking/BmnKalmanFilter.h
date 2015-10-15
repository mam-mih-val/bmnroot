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

};

#endif //BmnKalmanFilter

