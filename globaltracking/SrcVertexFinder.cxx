#include "SrcVertexFinder.h"

#include "BmnMath.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "vector"

using namespace std;
using namespace TMath;

SrcVertexFinder::SrcVertexFinder(Int_t period, Bool_t isField) {
    fPeriodId = period;
    fEventNo = 0;
    fGlobalTracksArray = NULL;
    fGemHitsArray = NULL;
    fTof400HitsArray = NULL;
    fVertexArray = NULL;
    fArmTracksArray = NULL;
    fTime = 0.0;
    fIsField = isField;
    fKalman = new BmnKalmanFilter();
}

SrcVertexFinder::~SrcVertexFinder() { delete fKalman; }

InitStatus SrcVertexFinder::Init() {
    if (fVerbose > 1)
        cout << "=========================== Vertex finder init started ====================" << endl;

    // Get ROOT Manager
    FairRootManager *ioman = FairRootManager::Instance();
    if (NULL == ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    fGlobalTracksArray = (TClonesArray *)ioman->GetObject("BmnGlobalTrack");  // in
    if (!fGlobalTracksArray) {
        cout << "SrcVertexFinder::Init(): branch BmnGlobalTrack not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fGemHitsArray = (TClonesArray *)ioman->GetObject("BmnGemStripHit");  // in
    if (!fGemHitsArray) {
        cout << "SrcVertexFinder::Init(): branch BmnGemStripHit not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fTof400HitsArray = (TClonesArray *)ioman->GetObject("BmnTof400Hit");  // in
    if (!fTof400HitsArray) {
        cout << "SrcVertexFinder::Init(): branch BmnTof400Hit not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fVertexArray = new TClonesArray("BmnVertex", 1);  // out
    ioman->Register("SrcVertex", "VERTEX", fVertexArray, kTRUE);
    fArmTracksArray = new TClonesArray("BmnTrack", 1);  // out
    ioman->Register("SrcArmTrack", "ARM", fArmTracksArray, kTRUE);

    if (fVerbose > 1) cout << "=========================== Vertex finder init finished ===================" << endl;

    return kSUCCESS;
}

void SrcVertexFinder::Exec(Option_t *opt) {
    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    if (fVerbose > 1) {
        cout << "======================== Vertex finder exec started  ======================" << endl;
        cout << "Event number: " << fEventNo++ << endl;
    }

    fVertexArray->Delete();
    fArmTracksArray->Delete();

    vector<BmnTrack> lTracks;
    vector<BmnTrack> rTracks;
    CreateArmCandidates(lTracks, rTracks);

    Int_t nTrWithUpstream = 0;
    for (Int_t iTrack = 0; iTrack < fGlobalTracksArray->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack *track = (BmnGlobalTrack *)fGlobalTracksArray->UncheckedAt(iTrack);
        if (track->GetUpstreamTrackIndex() == -1)
            track->SetFlag(13);
        else
            nTrWithUpstream++;
    }

    if (lTracks.size() > 100 || rTracks.size() > 100 || nTrWithUpstream > 10) {  //Check this condition!
        new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) BmnVertex();
        if (fVerbose > 0) cout << "SrcVertexFinder: Vertex NOT found" << endl;
    } else {
        FindVertexByVirtualPlanes(lTracks, rTracks);
        if (fVerbose > 0) {
            BmnVertex *vert = (BmnVertex *)fVertexArray->At(0);
            vert->Print();
        }
    }
    lTracks.clear();
    rTracks.clear();

    if (fVerbose > 1)
        cout << "\n======================== Vertex finder exec finished ======================" << endl;

    sw.Stop();
    fTime += sw.RealTime();
}

void SrcVertexFinder::FindVertexByVirtualPlanes(vector<BmnTrack> &lTracks, vector<BmnTrack> &rTracks) {
    Float_t minDist = DBL_MAX;  //minimal distance beetween tracks in point of vertex
    Float_t minVZ = DBL_MAX;    // VZ for tracks with minimal distance
    vector<BmnTrack> trackCombination;
    vector<BmnTrack> bestCombination;
    Int_t type = -1;
    if (lTracks.size() > 0 && rTracks.size() > 0) {
        for (auto lTr : lTracks) {
            for (auto rTr : rTracks) {
                for (Int_t iGl = 0; iGl < fGlobalTracksArray->GetEntriesFast(); ++iGl) {
                    BmnGlobalTrack gl = *((BmnGlobalTrack *)fGlobalTracksArray->At(iGl));
                    if (gl.GetUpstreamTrackIndex() != -1)
                        trackCombination.push_back(*((BmnTrack *)fGlobalTracksArray->At(iGl)));
                }
                trackCombination.push_back(lTr);
                trackCombination.push_back(rTr);
                Float_t dist;
                Float_t vz = FindVZByVirtualPlanes(-647, 100, trackCombination, dist);
                if (vz < -999) {
                    trackCombination.clear();
                    continue;
                }
                if (dist < minDist) {
                    minDist = dist;
                    minVZ = vz;
                    bestCombination = trackCombination;
                    type = (bestCombination.size() > 2) ? 10 : 11;
                }
                trackCombination.clear();
            }
        }
    } else if (lTracks.size() == 0 && rTracks.size() > 0) {
        for (auto rTr : rTracks) {
            for (Int_t iGl = 0; iGl < fGlobalTracksArray->GetEntriesFast(); ++iGl) {
                BmnGlobalTrack gl = *((BmnGlobalTrack *)fGlobalTracksArray->At(iGl));
                if (gl.GetUpstreamTrackIndex() != -1)
                    trackCombination.push_back(*((BmnTrack *)fGlobalTracksArray->At(iGl)));
            }
            trackCombination.push_back(rTr);
            Float_t dist;
            Float_t vz = FindVZByVirtualPlanes(-647, 100, trackCombination, dist);
            if (vz < -999) {
                trackCombination.clear();
                continue;
            }
            if (dist < minDist) {
                minDist = dist;
                minVZ = vz;
                bestCombination = trackCombination;
                type = 13;
            }
            trackCombination.clear();
        }
    } else if (lTracks.size() > 0 && rTracks.size() == 0) {
        for (auto lTr : lTracks) {
            for (Int_t iGl = 0; iGl < fGlobalTracksArray->GetEntriesFast(); ++iGl) {
                BmnGlobalTrack gl = *((BmnGlobalTrack *)fGlobalTracksArray->At(iGl));
                if (gl.GetUpstreamTrackIndex() != -1)
                    trackCombination.push_back(*((BmnTrack *)fGlobalTracksArray->At(iGl)));
            }
            trackCombination.push_back(lTr);
            Float_t dist;
            Float_t vz = FindVZByVirtualPlanes(-647, 100, trackCombination, dist);
            if (vz < -999) {
                trackCombination.clear();
                continue;
            }
            if (dist < minDist) {
                minDist = dist;
                minVZ = vz;
                bestCombination = trackCombination;
                type = 12;
            }
            trackCombination.clear();
        }
    }
    if (minDist > 10.0) {
        new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) BmnVertex();
    } else {
        vector<Double_t> xHits;
        vector<Double_t> yHits;
        for (Int_t iTr = 0; iTr < bestCombination.size(); ++iTr) {
            BmnTrack track = bestCombination[iTr];
            FairTrackParam par0 = *(track.GetParamLast());
            Double_t len = 0.0;
            if (fKalman->TGeoTrackPropagate(&par0, minVZ, 2212, NULL, &len, kFALSE) == kBMNERROR) {
                continue;
            }
            xHits.push_back(par0.GetX());
            yHits.push_back(par0.GetY());
            if (track.GetParamFirst()->GetZ() < -400) {
                track.SetLength(len);
                track.SetNHits(2);
                new ((*fArmTracksArray)[fArmTracksArray->GetEntriesFast()]) BmnTrack(track);
            }
        }
        Double_t vz = minVZ;
        Double_t vx = Mean(xHits.begin(), xHits.end());
        Double_t vy = Mean(yHits.begin(), yHits.end());

        vector<Int_t> idx;
        for (Int_t iGl = 0; iGl < fGlobalTracksArray->GetEntriesFast(); ++iGl) {
            BmnGlobalTrack gl = *((BmnGlobalTrack *)fGlobalTracksArray->At(iGl));
            if (gl.GetUpstreamTrackIndex() != -1)
                idx.push_back(iGl);
        }

        new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) BmnVertex(vx, vy, vz, minDist, 0, xHits.size(), TMatrixFSym(3), type, idx);
    }
}

void SrcVertexFinder::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt", ofstream::app);
    outFile << "Vertex Finder Time: " << fTime;
    if (fVerbose == 0)
        cout << "Work time of the GEM vertex finder: " << fTime << endl;
}

Double_t SrcVertexFinder::CalcMeanDist(vector<Double_t> x, vector<Double_t> y) {
    Double_t sumDist = 0.0;
    Int_t nPairs = 0;
    for (Int_t i = 0; i < x.size(); ++i) {
        for (Int_t j = i + 1; j < x.size(); ++j) {
            sumDist += Sqrt(Sq(x[i] - x[j]) + Sq(y[i] - y[j]));
            nPairs++;
        }
    }
    return sumDist / nPairs;  // calc. ave. dist value
}

Float_t SrcVertexFinder::FindVZByVirtualPlanes(Float_t z_0, Float_t range, vector<BmnTrack> tracks, Float_t &minDist) {
    const Int_t nPlanes = 3;
    Float_t minZ = z_0;

    while (range >= 0.01) {
        Float_t zMax = minZ + range;
        Float_t zMin = minZ - range;
        Float_t zStep = (zMax - zMin) / (nPlanes - 1);

        vector<Double_t> xHits[nPlanes];
        vector<Double_t> yHits[nPlanes];
        Float_t zPlane[nPlanes];
        Float_t rRMS[nPlanes] = {0};

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane)
            zPlane[iPlane] = zMax - iPlane * zStep;

        Int_t nOkTr = 0;

        for (Int_t iTr = 0; iTr < tracks.size(); ++iTr) {
            BmnTrack track = tracks[iTr];
            FairTrackParam par0 = *(track.GetParamFirst());
            Double_t xTr[nPlanes];
            Double_t yTr[nPlanes];
            Bool_t trOk = kTRUE;
            for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
                if (fKalman->TGeoTrackPropagate(&par0, zPlane[iPlane], 2212, NULL, NULL, kFALSE) == kBMNERROR) {
                    trOk = kFALSE;
                    break;
                }

                //cout << " " << par0.GetX() << " " << par0.GetY() << " " << par0.GetZ() << endl;
                xTr[iPlane] = par0.GetX();
                yTr[iPlane] = par0.GetY();
                //xHits[iPlane].push_back(par0.GetX());
                //yHits[iPlane].push_back(par0.GetY());
            }

            if (trOk) {
                nOkTr++;
                for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
                    xHits[iPlane].push_back(xTr[iPlane]);
                    yHits[iPlane].push_back(yTr[iPlane]);
                }
            }
        }

        if (nOkTr < 2) {
            minDist = DBL_MAX;
            return -1000.0;
        }

        //Calculation minZ as minimum of parabola
        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            rRMS[iPlane] = CalcMeanDist(xHits[iPlane], yHits[iPlane]);
        }
        TGraph *vertex = new TGraph(nPlanes, zPlane, rRMS);
        TFitResultPtr ptr = vertex->Fit("pol2", "QFS");
        Float_t c = ptr->Parameter(0);
        Float_t b = ptr->Parameter(1);
        Float_t a = ptr->Parameter(2);
        minZ = -b / (2 * a);
        minDist = a * minZ * minZ + b * minZ + c;
        delete vertex;

        range /= 2;
    }
    return minZ;
}

void SrcVertexFinder::CreateArmCandidates(vector<BmnTrack> &lTracks, vector<BmnTrack> &rTracks) {
    vector<Int_t> lTofHitIdx;
    vector<Int_t> rTofHitIdx;
    for (Int_t iTof = 0; iTof < fTof400HitsArray->GetEntriesFast(); ++iTof) {
        BmnHit *tHit = (BmnHit *)fTof400HitsArray->At(iTof);
        if (tHit->GetX() > 0)
            lTofHitIdx.push_back(iTof);
        else
            rTofHitIdx.push_back(iTof);
    }
    //Maybe we don't need next condition? Check it!
    //if (lTofHitIdx.size() == 0 || rTofHitIdx.size() == 0) return;  //one track must be on the left arm, another on the right

    vector<Int_t> lGemHitIdx;
    vector<Int_t> rGemHitIdx;
    for (Int_t iGem = 0; iGem < fGemHitsArray->GetEntriesFast(); ++iGem) {
        BmnHit *gHit = (BmnHit *)fGemHitsArray->At(iGem);
        if (gHit->GetStation() > 3) continue;
        if (gHit->GetX() > 0)
            lGemHitIdx.push_back(iGem);
        else
            rGemHitIdx.push_back(iGem);
    }
    //Maybe we don't need next condition? Check it!
    //if (lGemHitIdx.size() == 0 || rGemHitIdx.size() == 0) return;  //one track must be on the left arm, another on the right

    BmnHit tHit, gHit;
    for (auto gIdx : lGemHitIdx) {
        for (auto tIdx : lTofHitIdx) {
            tHit = *((BmnHit *)fTof400HitsArray->At(tIdx));
            gHit = *((BmnHit *)fGemHitsArray->At(gIdx));
            BmnTrack lTr;
            lTr.GetParamFirst()->SetX(gHit.GetX());
            lTr.GetParamFirst()->SetY(gHit.GetY());
            lTr.GetParamFirst()->SetZ(gHit.GetZ());
            lTr.GetParamFirst()->SetTx((tHit.GetX() - gHit.GetX()) / (tHit.GetZ() - gHit.GetZ()));
            lTr.GetParamFirst()->SetTy((tHit.GetY() - gHit.GetY()) / (tHit.GetZ() - gHit.GetZ()));
            lTr.GetParamLast()->SetX(tHit.GetX());
            lTr.GetParamLast()->SetY(tHit.GetY());
            lTr.GetParamLast()->SetZ(tHit.GetZ());
            lTr.GetParamLast()->SetTx((tHit.GetX() - gHit.GetX()) / (tHit.GetZ() - gHit.GetZ()));
            lTr.GetParamLast()->SetTy((tHit.GetY() - gHit.GetY()) / (tHit.GetZ() - gHit.GetZ()));
            lTracks.push_back(lTr);
        }
    }
    for (auto gIdx : rGemHitIdx) {
        for (auto tIdx : rTofHitIdx) {
            tHit = *((BmnHit *)fTof400HitsArray->At(tIdx));
            gHit = *((BmnHit *)fGemHitsArray->At(gIdx));
            BmnTrack rTr;
            rTr.GetParamFirst()->SetX(gHit.GetX());
            rTr.GetParamFirst()->SetY(gHit.GetY());
            rTr.GetParamFirst()->SetZ(gHit.GetZ());
            rTr.GetParamFirst()->SetTx((tHit.GetX() - gHit.GetX()) / (tHit.GetZ() - gHit.GetZ()));
            rTr.GetParamFirst()->SetTy((tHit.GetY() - gHit.GetY()) / (tHit.GetZ() - gHit.GetZ()));
            rTr.GetParamLast()->SetX(tHit.GetX());
            rTr.GetParamLast()->SetY(tHit.GetY());
            rTr.GetParamLast()->SetZ(tHit.GetZ());
            rTr.GetParamLast()->SetTx((tHit.GetX() - gHit.GetX()) / (tHit.GetZ() - gHit.GetZ()));
            rTr.GetParamLast()->SetTy((tHit.GetY() - gHit.GetY()) / (tHit.GetZ() - gHit.GetZ()));
            rTracks.push_back(rTr);
        }
    }
}

ClassImp(SrcVertexFinder);
