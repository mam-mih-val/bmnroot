// -------------------------------------------------------------------------
// -----                MpdUrqmd23Generator header file                  -----
// -----          Created 11/06/04  by V. Friese / D.Bertini           -----
// -------------------------------------------------------------------------


/** MpdUrqmd23Generator.h
 *@ author V.Friese <v.friese@gsi.de>
 *@author D.Bertini <d.bertini@gsi.de>
 *
 The MpdUrqmd23Generator reads the output file 14 (ftn14) from UrQMD. The UrQMD
 calculation has to be performed in the CM system of the collision; Lorentz
 transformation into the lab is performed by this class.
 Derived from FairGenerator.
**/



#ifndef MPDURQMD23GENERATOR_H
#define MPDURQMD23GENERATOR_H


#include "FairGenerator.h"

#include <fstream>
#include <map>

#include <zlib.h>

class TVirtualMCStack;
class FairPrimaryGenerator;

class MpdUrqmd23Generator : public FairGenerator
{

  public:

    /** Default constructor without arguments should not be used. **/
    MpdUrqmd23Generator();


    /** Standard constructor.
     * @param fileName The input file name
     **/
    MpdUrqmd23Generator(const char* fileName);


    /** Destructor. **/
    ~MpdUrqmd23Generator();


    /** Reads on event from the input file and pushes the tracks onto
     ** the stack. Abstract method in base class.
     ** @param pStack    pointer to the stack
     ** @param ver       not used
     **/
    Bool_t ReadEvent(FairPrimaryGenerator* primGen);

    //Skip some events in file
    Bool_t SkipEvents(Int_t count);

  private:

    gzFile fInputFile;                     //!  Input file

    std::map<Int_t,Int_t> fParticleTable;      //!  Map from UrQMD PID to PDGPID

    const Char_t* fFileName;              //!  Input file name

    /** Private method ReadConversionTable. Reads the conversion table
        from UrQMD particle code to PDG particle code and fills the
        conversion map. Is called from the constructor. **/
    void ReadConversionTable();

    MpdUrqmd23Generator(const MpdUrqmd23Generator&);
    MpdUrqmd23Generator& operator=(const MpdUrqmd23Generator&);

    ClassDef(MpdUrqmd23Generator,1);

};

#endif


