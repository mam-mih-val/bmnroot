#ifndef BMNTOFCALRAW2DIGIT_H
#define	BMNTOFCALRAW2DIGIT_H 

#include "BmnTofCalDigit.h"
#include "BmnTacquilaDigit.h"
#include "TClonesArray.h"

#define TOFCAL_ARM_N 2
#define TOFCAL_PLANE_N 4
#define TOFCAL_BAR_N 15
#define TOFCAL_SIDE_N 2
#define TACQ_CRATE_N 2
#define TACQ_MODULE_N 10 
#define TACQ_CHANNEL_N 16

class BmnTofCalRaw2Digit{
  public:
    BmnTofCalRaw2Digit(TString, TString, TString, TString, TString);
    void fillEvent(TClonesArray const *, TClonesArray *);
    struct TCal {
      TCal(): tdc(-1), t_ns(-1) {}
      TCal(UShort_t a_tdc, Float_t a_t_ns): tdc(a_tdc), t_ns(a_t_ns) {}
      UShort_t tdc;
      Float_t t_ns;
    };

  private:
    void SetTCal(BmnTacquilaDigit &);
    struct TacquilaRef {
      TacquilaRef(): crate(-1), module(-1), channel(-1) {}
      Char_t crate;
      Char_t module;
      Char_t channel;
    };
    struct DetectorRef {
      DetectorRef(): arm(-1), plane(-1), bar(-1), side(-1) {}
      Char_t arm;
      Char_t plane;
      Char_t bar;
      Char_t side;
    };
    struct Pedestal {
      Pedestal(): ped(0) {}
      Float_t ped;
    };
    struct DiffSync {
      DiffSync(): time_diff(0), time_sync(0), energy_diff0(1), energy_diff1(1),
      energy_sync(1) {}
      Float_t time_diff;
      Float_t time_sync;
      Float_t energy_diff0;
      Float_t energy_diff1;
      Float_t energy_sync;
    };
    struct VScint {
      VScint(): vscint(1) {}
      Float_t vscint;
    };
    DetectorRef m_tacq2det[TACQ_CRATE_N][TACQ_MODULE_N][TACQ_CHANNEL_N];
    TacquilaRef m_det2tacq[TOFCAL_ARM_N][TOFCAL_PLANE_N][TOFCAL_BAR_N][TOFCAL_SIDE_N];
    Pedestal m_ped[TOFCAL_ARM_N][TOFCAL_PLANE_N][TOFCAL_BAR_N][TOFCAL_SIDE_N];
    std::vector<TCal> m_tcal[TACQ_CRATE_N][TACQ_MODULE_N][TACQ_CHANNEL_N + 1];
    DiffSync m_diff_sync[TOFCAL_ARM_N][TOFCAL_PLANE_N][TOFCAL_BAR_N];
    VScint m_vscint[TOFCAL_ARM_N][TOFCAL_PLANE_N][TOFCAL_BAR_N];
    BmnTacquilaDigit const *m_c17[TACQ_CRATE_N][TACQ_MODULE_N];
    BmnTacquilaDigit const *m_builder[TOFCAL_ARM_N][TOFCAL_PLANE_N][TOFCAL_BAR_N][TOFCAL_SIDE_N];

    ClassDef(BmnTofCalRaw2Digit, 1);
};

#endif /* BMNTOFCALRAW2DIGIT_H */
