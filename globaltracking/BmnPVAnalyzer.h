#ifndef BMNVERTEXFINDER2_H
#define BMNVERTEXFINDER2_H

#include <sys/time.h>
#include <vector>

#include <TH1D.h>
#include <TH2D.h>
#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "BmnGemTrack.h"
#include "BmnGlobalTrack.h"
#include "BmnGemStripHit.h"
#include "../bmndst/DstEventHeader.h"
#include "BmnEventHeader.h"
#include "TMath.h"
#include "TVector3.h"
#include "BmnEnums.h"
#include "FairRunAna.h"
#include "FairField.h"
#include <CbmGeoStsPar.h>
#include <CbmStsDigiScheme.h>
#include <CbmStsSensor.h>
#include <CbmStsHit.h>
#include <CbmStsTrack.h>
#include "CbmVertex.h"
#include "../KF/Interface/CbmKFTrack.h"
#include "../KF/Interface/CbmStsKFTrackFitter.h"
#include "BmnKalmanFilter.h"
#include <TStopwatch.h>
#include <TGeoNode.h>
#include <TGeoMatrix.h>
#include <TGeoManager.h>
#include "BmnMath.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TGraph.h"

#define VTX_LEN 100

 /** Uncommenting this please also change treename in the config/rootmanager.dat */
#define CBM_TRACKS 1

#define NHITS_THR 4


using namespace std;

class BmnPVAnalyzer : public FairTask {
public:

    BmnPVAnalyzer() {
    };
    BmnPVAnalyzer(Int_t period, Int_t run, Bool_t isField);
    virtual ~BmnPVAnalyzer();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void SetField(Bool_t f) {
        fIsField = f;
    }

    void SetVertexApproximation(TVector3 vertex) {
        fRoughVertex3D = vertex;
    }
    

private:


    void ProcessEvent();
    void ApplyAlignment();

    void v5_topol(int& N, int Niter, int& NZ, double& Z0, int iList[], CbmStsTrack *CList[],
            int& jfirst, int& itpl, int ntopol[], int inxtpl[][VTX_LEN],
            double V_XYZ[][VTX_LEN], double V_cda[][VTX_LEN][VTX_LEN], double vxhi2[],
            double& x_avr, double& y_avr, double& z_avr, int& jNtracks, int jList[]);

    void vtx_topol(int& N, int Niter, int& NZ, double& Z0, int iList[], CbmStsTrack *CList[],
            int& jfirst, int& itpl, int ntopol[], int inxtpl[][VTX_LEN],
            double V_XYZ[][VTX_LEN], double V_cda[][VTX_LEN][VTX_LEN], double vxhi2[]);

    //======================================
    //
    //  Generate combinations : 
    //
    // C^k_n = n!/(k!*(n-k)!)
    //
    //======================================
    static bool combi(int ic[], int& nc, int& jc);

    static void VirtualPlanes(int& N, int Niter, int& NZ, double& Z0, int List[], CbmStsTrack *CList[],
            double vert[], double vtx[], double dcda[][300], double& ssq);


    static void Decay_Plane(double track_Fp[], double track_Lp[], double track_Fm[], double track_Lm[], double& CosT);

    //--------------------------------------------------------------
    //  Simple vertex estimator
    //
    //--------------------------------------------------------------
    static void vercda(double dvec[][300], double dww[],
            double dcda[][300], double dvert[],
            int& nv, double& dssq, int& nvhi);

    //  Impact parameter of the track vs Vertex
    //
    static void ddpoint(double& dxm, double& dym, double& dzm,
            double& dxs, double& dys, double& dzs,
            double& dpx, double& dpy, double& dpz,
            double& dptot, double dist[]);

    static void armentero(double& ppx, double& ppy, double& ppz,
            double& pnx, double& pny, double& pnz,
            double& ep, double& en,
            double& alfa, double& ptt,
            double& eppos, double& epmin);

    //----------------------------------------------------------------------
    //
    //     ******************************************************************
    //     *                                                                *
    //     * Calculate invariant mass of NPAR particles with masses         *
    //     *        AMS and momenta PPA                                     *
    //     *      in OMEGA frame, E - the gamma energy                      *
    //     * Output: EFMS - inv. mass (Gev), POUT - momentum of combin.     *
    //     *                                                                *
    //     ******************************************************************
    static void efmass(int &npar, double ams[], double ppa[][300],
            double &efms, double pout[]);

    static void CopyDir(TDirectory *source);

    static Int_t GetNofModules(TGeoNode* station);
    
    void InitHists();
    

    // Private Data Members ------------
    TString fGlobalTracksBranchName;
    TString fDstEHBranchName;
    TString fVertexBranchName;
    TString fVertexAllBranchName;

    Int_t fPeriodId;
    Int_t fRunId;
    Int_t fNTracks; // number of reco tracks in event

    TClonesArray* fGlobalTracksArray;
#ifdef CBM_TRACKS
    TClonesArray* fDstEventHeader;
#else
    DstEventHeader* fDstEventHeader;
    
//    TClonesArray* fGemHitsArray;
//    TClonesArray* fSilHitsArray;
//    TClonesArray* fCbmHitsArray;
//    TString fGemHitsBranchName;
//    TString fSilHitsBranchName;
//    TString fCbmHitsBranchName;
#endif
    FairEventHeader* fFairEventHeader;// for single(non run_reco) use, @TODO remove
    
    
    TClonesArray* fVertexArray;
    TClonesArray* fVertexArrayAll;

    Double_t fTime;

    Bool_t fIsField;
    FairField* fField;
    TVector3 fRoughVertex3D;
    TVector3 fCBMFrameShift3D;


    // MZ
    TFile *hhist = nullptr;
    Int_t Nsize = 0;

    Float_t pv[4] = {0}; //PV
    Float_t FirstPar[6] = {0};
    Float_t LastPar[6] = {0}; //FirstParam
    Long64_t evNo = 0;
    TTree *txyz = nullptr;
    TList *parFileList = nullptr;
    Long64_t events = 0;
    Long64_t iev = -1;
    
    const Double_t M_pi = 0.13956995;
    const Double_t M_p = 0.93827231;

    const Double_t M_Kp = 0.493677;

    const Double_t M_K0 = 0.497672;
    const Double_t M_L0 = 1.115683;

    const Double_t HM_K0 = 0.5;
    const Double_t HM_L0 = 1.12;

    const Double_t M_K0_Cut = 0.10;
    const Double_t M_L0_Cut = 0.07;

    const Double_t M_K0_Cut1 = 0.025;
    const Double_t M_L0_Cut1 = 0.006;
    
    const Int_t ICMB = 4;
    
    
    int icharge[1000] = {0};
    int Ttype[1000] = {0};
    int iNaN[1000] = {0};

    double dptrack_F[5][1000] = {{0}}; // the track momentum Px Py Pz E Ptot
    double dpartra_F[6][1000] = {{0}}; // the track parameters: X Y Z Tx Ty Ptot

    double dptrack_L[5][1000] = {{0}}; // the track momentum Px Py Pz E Ptot
    double dpartra_L[6][1000] = {{0}}; // the track parameters: X Y Z Tx Ty Ptot

    double tvdca[4][1000] = {{0}}; // Min distance to vertex
    double timp_velo[4][1000] = {{0}}; // Impact to Prim

    double Tx, Ty, ptx, pty, ptz, pttot, pttot_F;
    double zzT;

    double ZStation[2][10] = {{0}};


    time_t curtime;
    struct tm* loctime = nullptr;

    struct timeval tv0, tv1, tv2;

    struct timeval tm0, tm1, tm2;

    long long tmu = 0, tsec = 0;
    long long tmu_old = 0, tsec_old = 0;
    long long tsumm_0 = 0, tsumm_1 = 0, dtsumm = 0;
    long long t_min = 0, t_max = 0;

    long long tmu_old1 = 0, tsec_old1 = 0;
    long long tsumm_01 = 0, tsumm_11 = 0;
    

    TDirectory *cdTracks = NULL;

    TH1D * hh1[10000] = {nullptr};
    TH2D * hh2[10000] = {nullptr};

    int nh_bins;


    ClassDef(BmnPVAnalyzer, 1);
};


#endif /* BMNVERTEXFINDER2_H */


