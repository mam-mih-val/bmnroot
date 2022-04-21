/**
 * \file BmnEnums.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Define enumerations used in tracking.
 **/

#ifndef BMNENUMS_H_
#define BMNENUMS_H_

/**
 * \enum Define type of hit on detector station.
 */
enum BmnHitType {
    kBMNSTRIPHIT = 0, /**< Strip hit. */
    kBMNPIXELHIT = 1, /**< Pixel hit. */
    kBMNMIXHIT = 2 /**< Mixed type of hits, i.e. both strip and pixel hits. **/
};

/**
 * \enum Define status code.
 */
enum BmnStatus {
    kBMNSUCCESS = 0, /**< Success. */
    kBMNERROR = 1, /**< Error. */
    kBMNTIMEOUT = 2,
    kBMNFINISH = 3
};

/**
 * \enum Define reconstructed track quality.
 */
enum BmnTrackQa {
    kBMNGOOD = 0, /**< Good track. */
    kBMNBAD = 1, /**< Bad track. */
    kBMNGOODMERGE = 2 /**< Correctly merged track. */
};

/**
 * \enum Define system identificator.
 */
enum BmnSystemId {
    kBMNGEM = 0, /**< GEM detector. */
    kBMNTOF = 2, /**< TOF detector. */
    kBMNSTS = 4 /**< STS detector. */
};

/**
 * \enum Define status of TOF hit merging
 */
enum BmnTofHitQA {
    kBMNTOFGOOD = 0, /**< merged TOF hit. */
    kBMNTOFBAD = 1, /**< not merged TOF hit. */
};

/**
 * \enum Define type of event
 */
enum BmnTriggerType {
    kBMNBEAM = 6, /**< "beam"  BC1 + BC2 + T0 + VC */
    kBMNMINBIAS = 1, /**< "min.bias" BC1 + BC2 + T0 + VC + FD */
};

/**
 * \enum Define type of event
 */
enum BmnEventType {
    kBMNPEDESTAL = 0, /**< pedestal event. */
    kBMNPAYLOAD = 1, /**< spill event. */
    kBMNEOS = 2, /**< End of spill. */
    kBMNSTAT = 3, /**< Statistics. */
};

/**
 * \enum Define worker state
 */
enum BmnWorkerState {
    kBMNWAIT = 0,
    kBMNWORK = 1,
    kBMNRECON = 2,
};

/**
 * \enum Define experimental setup
 */
enum BmnSetup {
    kBMNSETUP = 0,
    kSRCSETUP = 1,
};

/**
 * \enum Define ADC strip decoding algorithm
 */
enum BmnADCDecoMode {
    kBMNADCMK = 0,
    kBMNADCSM = 1,
};

#endif /* BMNENUMS_H_ */
