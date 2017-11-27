// ----------------------------------------------------------------------
//                    ElogDbBeam header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** elog_classes/ElogDbBeam.h 
 ** Class for the table: beam_ 
 **/ 

#ifndef ELOGDBBEAM_H 
#define ELOGDBBEAM_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class ElogDbBeam
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// beam
	TString str_beam;

	//Constructor
	ElogDbBeam(UniDbConnection* connUniDb, TString beam);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~ElogDbBeam(); // Destructor

	// static class functions
	/// add new beam to the database
	static ElogDbBeam* CreateBeam(TString beam);
	/// get beam from the database
	static ElogDbBeam* GetBeam(TString beam);
	/// check beam exists in the database
	static bool CheckBeamExists(TString beam);
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
