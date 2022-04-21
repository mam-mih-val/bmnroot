// -------------------------------------------------------------------------
// -----                    BmnFieldContFact header file               -----
// -----                   Created 20/02/06  by V. Friese              -----
// -------------------------------------------------------------------------


/** BmnFieldContFact.h
 ** @author V.Friese <v.friese@gsi.de>
 ** @since 20.02.2006
 ** @version 1.0
 **
 ** Parameter container factory for field parameter container
 **/


#ifndef CBMFIELDCONTFACT_H
#define CBMFIELDCONTFACT_H 1


#include "FairContFact.h"


class FairContainer;
class FairParSet;


class BmnFieldContFact : public FairContFact
{

 public:

  /** Constructor **/
  BmnFieldContFact();


  /** Destructor **/
  ~BmnFieldContFact();


  /** Create containers
   ** Creates the requested parameter sets (BmnFieldPar) 
   **/
  FairParSet* createContainer(FairContainer* container);


 private:

  /** Set all containers  
   ** Creates container objects with all accepted contexts and adds them
   ** to the list of containers for the field library. 
   **/
  void SetAllContainers();


  ClassDef(BmnFieldContFact,1);

};

#endif
