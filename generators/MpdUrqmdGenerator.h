// -------------------------------------------------------------------------
// -----                MpdUrqmdGenerator header file                  -----
// -----          Created 11/06/04  by V. Friese / D.Bertini           -----
// -------------------------------------------------------------------------


/** MpdUrqmdGenerator.h
 *@ author V.Friese <v.friese@gsi.de>
 *@author D.Bertini <d.bertini@gsi.de>
 *
 The MpdUrqmdGenerator reads the output file 14 (ftn14) from UrQMD. The UrQMD
 calculation has to be performed in the CM system of the collision; Lorentz
 transformation into the lab is performed by this class.
 Derived from FairGenerator.
**/
#define GZIP_SUPPORT // version with gz support

#ifndef MPDURQMDGENERATOR_H
#define MPDURQMDGENERATOR_H

#include "FairGenerator.h"

#include <fstream>
#include <map>

#ifdef GZIP_SUPPORT
#ifndef __CINT__
#include <zlib.h>
#endif
#endif

class TVirtualMCStack;
class FairPrimaryGenerator;

class MpdUrqmdGenerator : public FairGenerator
{
  public:

    /** Default constructor without arguments should not be used. **/
    MpdUrqmdGenerator();


    /** Standard constructor.
     * @param fileName The input file name
     **/
    MpdUrqmdGenerator(const char* fileName);


    /** Destructor. **/
    ~MpdUrqmdGenerator();


    /** Reads on event from the input file and pushes the tracks onto
     ** the stack. Abstract method in base class.
     ** @param pStack    pointer to the stack
     ** @param ver       not used
     **/
    Bool_t ReadEvent(FairPrimaryGenerator* primGen);

    //Skip some events in file
    Bool_t SkipEvents(Int_t count);

    void SetEventPlane(Double_t phiMin, Double_t phiMax);

    void SetXYZ (Double32_t x = 0., Double32_t y = 0., Double32_t z = 0.) {
        fX = x;
        fY = y;
        fZ = z;
    }

  private:

#ifdef GZIP_SUPPORT
    #ifndef __CINT__
    gzFile fInputFile;                    //!  Input file
    #endif
#else
    FILE* fInputFile;                     //!  Input file
#endif

    std::map<Int_t,Int_t> fParticleTable;      //!  Map from UrQMD PID to PDGPID

    Double32_t fPhiMin, fPhiMax; // Limits of event plane angle
    Bool_t fEventPlaneSet; // Flag whether event plane angle is used

    const Char_t* fFileName;              //!  Input file name

    Double32_t fX, fY, fZ; // Point vertex coordinates [cm]

    /** Private method ReadConversionTable. Reads the conversion table
        from UrQMD particle code to PDG particle code and fills the
        conversion map. Is called from the constructor. **/
    void ReadConversionTable();

    MpdUrqmdGenerator(const MpdUrqmdGenerator&) = delete;
    MpdUrqmdGenerator& operator=(const MpdUrqmdGenerator&) = delete;

    ClassDef(MpdUrqmdGenerator,1);

};

#endif


