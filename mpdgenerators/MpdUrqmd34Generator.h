// -------------------------------------------------------------------------
// -----                MpdUrqmdGenerator header file                  -----
// -----          Created 11/06/04  by V. Friese / D.Bertini           -----
// -------------------------------------------------------------------------


/** MpdUrqmd34Generator.h
 *@ author V.Friese <v.friese@gsi.de>
 *@author D.Bertini <d.bertini@gsi.de>
 *
 The MpdUrqmd34Generator reads the output file 14 (ftn14) from UrQMD. The UrQMD
 calculation has to be performed in the CM system of the collision; Lorentz
 transformation into the lab is performed by this class.
 Derived from FairGenerator.
**/



#ifndef MPDURQMD34GENERATOR_H
#define MPDURQMD34GENERATOR_H


#include "FairGenerator.h"

#include <fstream>
#include <map>

#include <zlib.h>

class TVirtualMCStack;
class FairPrimaryGenerator;

class MpdUrqmd34Generator : public FairGenerator
{

  public:

    /** Default constructor without arguments should not be used. **/
    MpdUrqmd34Generator();


    /** Standard constructor.
     * @param fileName The input file name
     **/
    MpdUrqmd34Generator(const char* fileName);


    /** Destructor. **/
    ~MpdUrqmd34Generator();


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

    MpdUrqmd34Generator(const MpdUrqmd34Generator&);
    MpdUrqmd34Generator& operator=(const MpdUrqmd34Generator&);

    ClassDef(MpdUrqmd34Generator,1);

};

#endif


