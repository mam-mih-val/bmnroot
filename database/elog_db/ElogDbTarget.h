// ----------------------------------------------------------------------
//                    ElogDbTarget header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogDbTarget.h
 ** Class for the table: target_ 
 **/ 

#ifndef ELOGDBTARGET_H 
#define ELOGDBTARGET_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniConnection.h"

class ElogDbTarget
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
        UniConnection* connectionUniDb;

	/// target
	TString str_target;

	//Constructor
        ElogDbTarget(UniConnection* connUniDb, TString target);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~ElogDbTarget(); // Destructor

	// static class functions
	/// add new target to the database
	static ElogDbTarget* CreateTarget(TString target);
	/// get target from the database
	static ElogDbTarget* GetTarget(TString target);
	/// check target exists in the database
	static bool CheckTargetExists(TString target);
	/// delete target from the database
	static int DeleteTarget(TString target);
	/// print all targets
	static int PrintAll();

	// Getters
	/// get target of the current target
	TString GetTarget() {return str_target;}

	// Setters
	/// set target of the current target
	int SetTarget(TString target);

	/// print information about current target
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(ElogDbTarget,1);
};

#endif
