//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNTOFCONTFACT_H
#define __BMNTOFCONTFACT_H 1

#include "FairContFact.h"

class FairContainer;

//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTOFContFact : public FairContFact 
{
private:
	void setAllContainers();

public:
	BmnTOFContFact();
	~BmnTOFContFact() {}

	FairParSet* 	createContainer(FairContainer*);

ClassDef(BmnTOFContFact, 0) 
};

#endif  
//--------------------------------------------------------------------------------------------------------------------------------------

