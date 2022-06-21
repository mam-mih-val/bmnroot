// ----------------------------------------------------------------------
//                    ElogDbBeam header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogDbBeam.h
 ** Class for the table: beam_ 
 **/ 

#ifndef ELOGDBBEAM_H 
#define ELOGDBBEAM_H 1 

#include "TString.h"
#include "TDatime.h"

#include "ElogConnection.h"

class ElogDbBeam
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
        ElogConnection* connectionDb;

	/// beam
	TString str_beam;

	//Constructor
    ElogDbBeam(ElogConnection* db_connect, TString beam);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~ElogDbBeam(); // Destructor

	// static class functions
	/// add new beam to the database
	static ElogDbBeam* CreateBeam(TString beam);
	/// get beam from the database
	static ElogDbBeam* GetBeam(TString beam);
    /// check beam exists in the database: 1 - true, 0 - false, <0 - database operation errors
    static int CheckBeamExists(TString beam);
	/// delete beam from the database
	static int DeleteBeam(TString beam);
	/// print all beams
	static int PrintAll();

	// Getters
	/// get beam of the current beam
	TString GetBeam() {return str_beam;}

	// Setters
	/// set beam of the current beam
	int SetBeam(TString beam);

	/// print information about current beam
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(ElogDbBeam,1);
};

#endif
