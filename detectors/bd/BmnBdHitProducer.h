// -------------------------------------------------------------------------
// -----                   BmnBdHitProducer header file                -----
// -----                  Created 12/07/18  by A. Zinchenko            -----
// -------------------------------------------------------------------------

/** BmnBdHitProducer.h
 *@author A.Zinchenko <Alexander.Zinchenko@jinr.ru>
 **
 ** The Barrel Detector (BD) hit producer produces hits by combining 
 ** MCPoints from the same scintillator bars.
 **/

#ifndef BMNBDHITPRODUCER_H
#define BMNBDHITPRODUCER_H 1

#include "BmnBdHit.h"

#include "FairTask.h"

#include <map>

class TClonesArray;

class BmnBdHitProducer : public FairTask
{

 public:

  /** Default constructor **/  
  BmnBdHitProducer();

  /** Destructor **/
  ~BmnBdHitProducer();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

 private:

  /** Input array of BmnBdPoints **/
  TClonesArray* fPointArray;

  /** Output array of BmnBdHits **/
  TClonesArray* fHitArray;  

  std::map<Int_t,BmnBdHit> fHitMap;

  BmnBdHitProducer(const BmnBdHitProducer&) = delete;
  BmnBdHitProducer& operator=(const BmnBdHitProducer&) = delete;

  ClassDef(BmnBdHitProducer,0);

};

#endif
