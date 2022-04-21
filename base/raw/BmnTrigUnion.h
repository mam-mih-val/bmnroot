#ifndef BMNTRIGUNION_H
#define BMNTRIGUNION_H

/**
 * Struct BmnTrigStructPeriod7SetupBMN
 * Struct for trigger logic state
 * 
 * for example:
 *  BC1 and BC2 and not VETO and BD>=2 and SI >=3 :
 *  BmnTrigStruct s;
 *  s.BC1 = true;
 *  s.BC2 = true;
 *  s.VETO = true;
 *  s.ThrBD = 2;
 *  s.ThrSI = 3;
 * 
 * By default all fields are zero/false.
 * Each zero field means independence on this trigger.
 */
struct __attribute__ ((packed)) BmnTrigStructPeriod7SetupBMN
{
    bool BC1:1;
    bool BC2:1;
    bool BC3:1;
    bool VETO:1; ///<- means not VETO
    UShort_t :7; // not used
    bool IsSpecial:1;
    UShort_t ThrBD:5;
    UShort_t ThrSI:5;
    UShort_t :10; // not used
};

struct __attribute__ ((packed)) BmnTrigStructPeriod7SetupSRC
{
    bool BC1:1;
    bool BC2:1;
    bool AntiBC3Hi:1;
    bool BC3Hi:1;
    bool VETO:1; ///<- means not VETO
    bool X1:1;
    bool Y1:1;
    bool X1Y1_and_X2Y2:1; ///<- 1 - and, 0 - or
    bool X2:1;
    bool Y2:1;
    UShort_t :1; // not used
    bool IsSpecial:1;
    UInt_t :20; // not used
};

struct __attribute__ ((packed)) BmnTrigStructPeriod6
{
    bool BC1:1;
    bool BC2:1;
    bool T0:1;
    bool VETO:1; ///<- means not VETO
    UShort_t :7; // not used
    bool IsSpecial:1;
    UShort_t ThrBD:5;
    UShort_t :15; // not used
};
/** union containing trigger state depending on current experimental setup */
union BmnTrigUnion {
    BmnTrigStructPeriod7SetupBMN Period7BMN;
    BmnTrigStructPeriod7SetupSRC Period7SRC;
    BmnTrigStructPeriod6 Period6;
    UInt_t storage; 
    BmnTrigUnion(){memset(this, 0, sizeof(BmnTrigUnion));}
    BmnTrigUnion(const BmnTrigUnion &v){this->storage = v.storage;}
    Bool_t isSpecial(){  return (storage & (1<<11)) > 0; }
};

#endif /* BMNTRIGUNION_H */

