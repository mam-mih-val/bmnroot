/**
 * \file BmnDetectorSetup.h
 * \brief Helper class to access detector presence.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2012-2014
 */
#ifndef BMNDETECTORSETUP_H_
#define BMNDETECTORSETUP_H_

#include "CbmDetectorList.h"
#include <string>
#include <map>

using namespace std;

/**
 * \class BmnDetectorSetup
 * \brief Helper class to access detector presence.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2012-2014
 */
class BmnDetectorSetup
{
public:

   /**
    * \brief Constructor.
    */
   BmnDetectorSetup();

   /**
    * \brief Destructor.
    */
   virtual ~BmnDetectorSetup();

   /**
    * \brief Set detector presence manually.
    * \param[in] detId Detector identificator.
    * \param[in] isDet True if detector is in the setup.
    */
   void SetDet(DetectorId detId,Bool_t isDet);

   /**
    * \brief Return detector presence in setup.
    * \param[in] detId Detector identificator.
    * \return Detector presence in setup.
    */
   Bool_t GetDet(DetectorId detId) const;

   /**
    * \brief Determines detector presence using TGeoManager.
    */
   void DetermineSetup();

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   string ToString() const;

private:
   /**
    * \brief Check detector presence using TGeoManager.
    * \param[in] name Name of the detector to be checked.
    * \return True if detector was found in TGeoManager.
    */
   Bool_t CheckDetectorPresence(
      const string& name) const;

   map<DetectorId, Bool_t> fDet; // Map DetectorId to detector presence
};

#endif /* BMNDETECTORSETUP_H_ */
