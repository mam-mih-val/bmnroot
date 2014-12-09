/** BmnDefaultSettings.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * @since 2009-2014
 * @version 1.0
 **
 ** Header defines constants which are used in the track reconstruction.
 **/

#ifndef BMNDEFAULTSETTINGS_H_
#define BMNDEFAULTSETTINGS_H_
class BmnDefaultSettings
{
public:
	// Track propagation settings
	static const Float_t LINE_EXTRAPOLATION_START_Z; // z coordinate [cm] after which linear track extrapolation starts
	static const Float_t ENERGY_LOSS_CONST; // energy lost constant [GeV/c] used in the simple energy loss calculation
	static const Float_t MINIMUM_PROPAGATION_DISTANCE; // minimum propagation distance [cm]
	static const Float_t MAXIMUM_TGEO_NAVIGATION_DISTANCE; // maximum distance used in the TGeo navigation
	// Not const because it is changed to 1 cm for track length calculation!
	//static Double_t MAXIMUM_PROPAGATION_STEP_SIZE; // maximum step size in the TGeo track propagation
};
                                
#endif /*BMNDEFAULTSETTINGS_H_*/
