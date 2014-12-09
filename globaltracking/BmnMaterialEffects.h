/**
 * \file BmnMaterialEffects.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for material effects calculation algorithm.
 */

#ifndef BmnMaterialEffects_H_
#define BmnMaterialEffects_H_

#include "BmnEnums.h"
#include "FairTrackParam.h"

class BmnMaterialInfo;

/**
 * \class BmnMaterialEffects
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for material effects calculation algorithm.
 */
class BmnMaterialEffects {
public:

    /**
     * \brief Constructor.
     */
    BmnMaterialEffects();

    /**
     * \brief Destructor.
     */
    virtual ~BmnMaterialEffects();

    /**
     * \brief Main function to be implemented for concrete material effects calculation algorithm.
     * \param[in,out] par Input/Output track parameters.
     * \param[in] mat Material information.
     * \param[in] pdg PDG code
     * \param[in] downstream Propagation direction (true for downstream, false for upstream).
     * \return Status code.
     */
    virtual BmnStatus Update(FairTrackParam* par, const BmnMaterialInfo* mat, Int_t pdg, Bool_t downstream);
    void AddEnergyLoss(FairTrackParam* par, const BmnMaterialInfo* mat) const;
    void AddThickScatter(FairTrackParam* par, const BmnMaterialInfo* mat) const;
    void AddThinScatter(FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t CalcThetaSq(const FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t EnergyLoss(const FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t dEdx(const FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t BetheBloch(const FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t BetheBlochElectron( const FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t CalcQpAfterEloss(Float_t qp, Float_t eloss) const;
    Float_t CalcSigmaSqQp(const FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t CalcSigmaSqQpElectron(const FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t CalcI(Float_t Z) const;
    Float_t BetheHeitler(const FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t PairProduction(const FairTrackParam* par, const BmnMaterialInfo* mat) const;
    Float_t BetheBlochSimple(const BmnMaterialInfo* mat) const;
    Float_t MPVEnergyLoss(const FairTrackParam* par, const BmnMaterialInfo* mat) const;

private:
    Bool_t fDownstream; // Propagation direction
    Float_t fMass; // Hypothesis on particle mass
    Bool_t fIsElectron; // True if particle is an electron or positron
    Bool_t fIsMuon; // True if particle is muon
};

#endif /*BmnMaterialEffects_H_*/
