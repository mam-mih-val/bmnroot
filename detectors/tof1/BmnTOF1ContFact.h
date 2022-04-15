//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNTOF1CONTFACT_H
#define __BMNTOF1CONTFACT_H 1

#include "FairContFact.h"

class FairContainer;

//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTOF1ContFact : public FairContFact 
{
private:
	void setAllContainers();

public:
	BmnTOF1ContFact();
	~BmnTOF1ContFact() {}

	FairParSet* 	createContainer(FairContainer*);

ClassDef(BmnTOF1ContFact, 0) 
};

#endif  
//--------------------------------------------------------------------------------------------------------------------------------------

