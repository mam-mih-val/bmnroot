#include "SrcVertexFinder.h"

#include "BmnMath.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "vector"

using namespace std;
using namespace TMath;

SrcVertexFinder::SrcVertexFinder(Int_t period, Bool_t isExp) {
    fPeriodId = period;
    fEventNo = 0;
    fGlobalTracksArray = nullptr;
    fGemHitsArray = nullptr;
    fMwpcTracksArray = nullptr;
    fTof400HitsArray = nullptr;
    fVertexArray = nullptr;
    fArmTracksArray = nullptr;
    fTime = 0.0;
    fisExp = isExp;
}

SrcVertexFinder::~SrcVertexFinder() { delete fKalman; }

InitStatus SrcVertexFinder::Init() {
    if (fVerbose > 1)
        cout << "=========================== Vertex finder init started ====================" << endl;

    fKalman = new BmnKalmanFilter();

    // Get ROOT Manager
    FairRootManager *ioman = FairRootManager::Instance();
    if (nullptr == ioman)
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
    fMwpcTracksArray = (TClonesArray *)ioman->GetObject("BmnMwpcTrack");  // in
    if (!fMwpcTracksArray) {
        cout << "SrcVertexFinder::Init(): branch BmnMwpcTrack not found! Task will be deactivated" << endl;
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
        //FindVertexByVirtualPlanes(lTracks, rTracks);
        FindVertexAnalitically(lTracks, rTracks);
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

void SrcVertexFinder::FindVertexAnalitically(vector<BmnTrack> &lTracks, vector<BmnTrack> &rTracks) {
    Double_t minVZ = 1000;
    Double_t vy = 1000;
    Double_t vx = 1000;
    Double_t minDist = 1000;
    vector<BmnTrack> trackCombination;
    vector<BmnTrack> bestCombination;
    if (lTracks.size() != 0 && rTracks.size() != 0) {
        minDist = 1000;
        minVZ = 1000;
        for (auto lTr : lTracks) {
            for (auto rTr : rTracks) {
                trackCombination.push_back(lTr);
                trackCombination.push_back(rTr);
                Double_t dist;
                Double_t vz = GetVzByVectorStraightTracks(trackCombination, dist);
                if (dist < minDist) {
                    minDist = dist;
                    minVZ = vz;
                    bestCombination = trackCombination;
                }
                trackCombination.clear();
            }
        }
    } else if (lTracks.size() != 0 && rTracks.size() == 0) {
        for (auto lTr : lTracks) {
            for (Int_t iGl = 0; iGl < fGlobalTracksArray->GetEntriesFast(); ++iGl) {
                BmnGlobalTrack *gl = (BmnGlobalTrack *)fGlobalTracksArray->At(iGl);
                if (gl->GetUpstreamTrackIndex() == -1) continue;
                trackCombination.push_back(*((BmnTrack *)fGlobalTracksArray->At(iGl)));
            }
            for (Int_t iM = 0; iM < fMwpcTracksArray->GetEntriesFast(); ++iM) {
                BmnTrack *mTr = (BmnTrack *)fMwpcTracksArray->At(iM);
                if (mTr->GetParamFirst()->GetZ() < -700) trackCombination.push_back(*mTr);
            }
            trackCombination.push_back(lTr);
            Double_t dist;
            Double_t vz = GetVzByVectorStraightTracks(trackCombination, dist);
            if (dist < minDist) {
                minDist = dist;
                minVZ = vz;
                bestCombination = trackCombination;
            }
            trackCombination.clear();
        }
    } else if (rTracks.size() != 0 && lTracks.size() == 0) {
        for (auto rTr : rTracks) {
            for (Int_t iGl = 0; iGl < fGlobalTracksArray->GetEntriesFast(); ++iGl) {
                BmnGlobalTrack *gl = (BmnGlobalTrack *)fGlobalTracksArray->At(iGl);
                if (gl->GetUpstreamTrackIndex() == -1) continue;
                trackCombination.push_back(*((BmnTrack *)fGlobalTracksArray->At(iGl)));
            }
            for (Int_t iM = 0; iM < fMwpcTracksArray->GetEntriesFast(); ++iM) {
                BmnTrack *mTr = (BmnTrack *)fMwpcTracksArray->At(iM);
                if (mTr->GetParamFirst()->GetZ() < -700) trackCombination.push_back(*mTr);
            }
            trackCombination.push_back(rTr);
            Double_t dist;
            Double_t vz = GetVzByVectorStraightTracks(trackCombination, dist);
            if (dist < minDist) {
                minDist = dist;
                minVZ = vz;
                bestCombination = trackCombination;
            }
            trackCombination.clear();
        }
    } else if (rTracks.size() == 0 && lTracks.size() == 0) {
        for (Int_t iGl = 0; iGl < fGlobalTracksArray->GetEntriesFast(); ++iGl) {
            BmnGlobalTrack *gl = (BmnGlobalTrack *)fGlobalTracksArray->At(iGl);
            if (gl->GetUpstreamTrackIndex() == -1) continue;
            trackCombination.push_back(*((BmnTrack *)fGlobalTracksArray->At(iGl)));
        }
        for (Int_t iM = 0; iM < fMwpcTracksArray->GetEntriesFast(); ++iM) {
            BmnTrack *mTr = (BmnTrack *)fMwpcTracksArray->At(iM);
            if (mTr->GetParamFirst()->GetZ() < -700) trackCombination.push_back(*mTr);
        }
        Double_t dist;
        Double_t vz = GetVzByVectorStraightTracks(trackCombination, dist);
        if (dist < minDist) {
            minDist = dist;
            minVZ = vz;
            bestCombination = trackCombination;
        }
        trackCombination.clear();
    } else
        new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) BmnVertex();

    if (minDist > 10.0) {
        new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) BmnVertex();
    } else {
        for (Int_t iM = 0; iM < fMwpcTracksArray->GetEntriesFast(); ++iM) {
            BmnTrack *mTr = (BmnTrack *)fMwpcTracksArray->At(iM);
            if (mTr->GetParamFirst()->GetZ() > -700) continue;  //we need incoming beam
            Double_t ax = mTr->GetParamFirst()->GetTx();
            Double_t ay = mTr->GetParamFirst()->GetTy();
            Double_t x = mTr->GetParamFirst()->GetX();
            Double_t y = mTr->GetParamFirst()->GetY();
            Double_t z = mTr->GetParamFirst()->GetZ();
            Double_t bx = x - ax * z;
            Double_t by = y - ay * z;
            vx = ax * minVZ + bx;
            vy = ay * minVZ + by;
        }

        Bool_t isLeft = kFALSE;
        Bool_t isRight = kFALSE;
        Bool_t isFragm = kFALSE;
        Bool_t isBeam = kFALSE;
        for (Int_t iTr = 0; iTr < bestCombination.size(); ++iTr) {
            BmnTrack track = bestCombination[iTr];
            if (track.GetFlag() == 13) continue;
            if (track.GetParamFirst()->GetZ() > -700 && track.GetParamFirst()->GetZ() < -400 && track.GetParamFirst()->GetX() < 0) {  //right arm track
                track.SetNHits(2);
                new ((*fArmTracksArray)[fArmTracksArray->GetEntriesFast()]) BmnTrack(track);
                isRight = kTRUE;
            }
            if (track.GetParamFirst()->GetZ() > -700 && track.GetParamFirst()->GetZ() < -400 && track.GetParamFirst()->GetX() > 0) {  //left arm track
                track.SetNHits(2);
                new ((*fArmTracksArray)[fArmTracksArray->GetEntriesFast()]) BmnTrack(track);
                isLeft = kTRUE;
            }
            if (track.GetParamFirst()->GetZ() > -400) {  //fragment track
                isFragm = kTRUE;
            }
            if (track.GetParamFirst()->GetZ() < -700) {  //fragment track
                isBeam = kTRUE;
            }
        }

        Int_t type = (isRight && isLeft) ? 10 : (isLeft && isFragm && isBeam) ? 11 : (isRight && isFragm && isBeam) ? 12 : (isBeam && isFragm) ? 13 : -1;

        vector<Int_t> idx;
        for (Int_t iGl = 0; iGl < fGlobalTracksArray->GetEntriesFast(); ++iGl) {
            BmnTrack gl = *((BmnTrack *)fGlobalTracksArray->At(iGl));
            if (gl.GetFlag() != 13) idx.push_back(iGl);
        }

        new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) BmnVertex(vx, vy, minVZ, minDist, 0, bestCombination.size(), TMatrixFSym(3), type, idx);
    }
}

void SrcVertexFinder::FindVertexByVirtualPlanes(vector<BmnTrack> &lTracks, vector<BmnTrack> &rTracks) {
    Float_t minDist = DBL_MAX;  //minimal distance beetween tracks in point of vertex
    Float_t minVZ = DBL_MAX;    // VZ for tracks with minimal distance
    vector<BmnTrack> trackCombination;
    vector<BmnTrack> bestCombination;
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
            BmnTrack *track = &bestCombination[iTr];
            FairTrackParam par0 = (track->GetParamFirst()->GetZ() < -400) ? (*(track->GetParamLast())) : (*(track->GetParamFirst()));
            Double_t len = track->GetLength();
            if (fKalman->TGeoTrackPropagate(&par0, minVZ, 2212, nullptr, &len) == kBMNERROR) {
                track->SetFlag(13);
                continue;
            }
            track->SetLength(len);
            track->SetB(len / track->GetB() / (TMath::C() * 1e-7));  //for arm tracks B contains beta. It is stupid, but simple way to store it...
            xHits.push_back(par0.GetX());
            yHits.push_back(par0.GetY());
        }
        if (xHits.size() < 2) {
            new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) BmnVertex();
            return;
        }

        Bool_t isLeft = kFALSE;
        Bool_t isRight = kFALSE;
        Bool_t isFragm = kFALSE;
        for (Int_t iTr = 0; iTr < bestCombination.size(); ++iTr) {
            BmnTrack track = bestCombination[iTr];
            if (track.GetFlag() == 13) continue;
            if (track.GetParamFirst()->GetZ() < -400 && track.GetParamFirst()->GetX() < 0) {  //right arm track
                track.SetNHits(2);
                new ((*fArmTracksArray)[fArmTracksArray->GetEntriesFast()]) BmnTrack(track);
                isRight = kTRUE;
            }
            if (track.GetParamFirst()->GetZ() < -400 && track.GetParamFirst()->GetX() > 0) {  //left arm track
                track.SetNHits(2);
                new ((*fArmTracksArray)[fArmTracksArray->GetEntriesFast()]) BmnTrack(track);
                isLeft = kTRUE;
            }
            if (track.GetParamFirst()->GetZ() > -400) {  //fragment track

                isFragm = kTRUE;
            }
        }

        Int_t type = (isRight && isLeft && isFragm) ? 10 : (isRight && isLeft) ? 11 : (isLeft && isFragm) ? 12 : 13;

        Double_t vz = minVZ;
        Double_t vx = Mean(xHits.begin(), xHits.end());
        Double_t vy = Mean(yHits.begin(), yHits.end());

        vector<Int_t> idx;
        for (Int_t iGl = 0; iGl < fGlobalTracksArray->GetEntriesFast(); ++iGl) {
            BmnTrack gl = *((BmnTrack *)fGlobalTracksArray->At(iGl));
            if (gl.GetFlag() != 13) idx.push_back(iGl);
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
    const Int_t nPlanes = 5;
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
                if (fKalman->TGeoTrackPropagate(&par0, zPlane[iPlane], 2212, nullptr, nullptr) == kBMNERROR) {
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

    //Coeficients for arms alignment
    Double_t dxall, dyall;
    Double_t dy_tl, dy_tr;
    Double_t gx, gy, gz, tx, ty, tz;
    if (fisExp) {
        dxall = -2.113;
        dyall = 2.504;
        dy_tl = -1.72;
        dy_tr = 1.3;
    } else {
        dxall = 0;
        dyall = 0;
        dy_tl = 0;
        dy_tr = 0;
    }

    for (auto gIdx : lGemHitIdx) {
        for (auto tIdx : lTofHitIdx) {
            tHit = *((BmnHit *)fTof400HitsArray->At(tIdx));
            gHit = *((BmnHit *)fGemHitsArray->At(gIdx));
            BmnTrack lTr;
            gx = gHit.GetX() + dxall;
            gy = gHit.GetY() + dyall;
            gz = gHit.GetZ();
            tx = tHit.GetX() + dxall;
            ty = tHit.GetY() + dyall + dy_tl;
            tz = tHit.GetZ();

            lTr.GetParamFirst()->SetX(gx);
            lTr.GetParamFirst()->SetY(gy);
            lTr.GetParamFirst()->SetZ(gz);
            lTr.GetParamFirst()->SetTx((tx - gx) / (tz - gz));
            lTr.GetParamFirst()->SetTy((ty - gy) / (tz - gz));
            lTr.GetParamLast()->SetX(tx);
            lTr.GetParamLast()->SetY(ty);
            lTr.GetParamLast()->SetZ(tz);
            lTr.GetParamLast()->SetTx((tx - gx) / (tz - gz));
            lTr.GetParamLast()->SetTy((ty - gy) / (tz - gz));
            lTr.SetB(tHit.GetTimeStamp());  //temporary
            lTracks.push_back(lTr);
        }
    }
    for (auto gIdx : rGemHitIdx) {
        for (auto tIdx : rTofHitIdx) {
            tHit = *((BmnHit *)fTof400HitsArray->At(tIdx));
            gHit = *((BmnHit *)fGemHitsArray->At(gIdx));
            BmnTrack rTr;
            gx = gHit.GetX() + dxall;
            gy = gHit.GetY() + dyall;
            gz = gHit.GetZ();
            tx = tHit.GetX() + dxall;
            ty = tHit.GetY() + dyall + dy_tr;
            tz = tHit.GetZ();

            rTr.GetParamFirst()->SetX(gx);
            rTr.GetParamFirst()->SetY(gy);
            rTr.GetParamFirst()->SetZ(gz);
            rTr.GetParamFirst()->SetTx((tx - gx) / (tz - gz));
            rTr.GetParamFirst()->SetTy((ty - gy) / (tz - gz));
            rTr.GetParamLast()->SetX(tx);
            rTr.GetParamLast()->SetY(ty);
            rTr.GetParamLast()->SetZ(tz);
            rTr.GetParamLast()->SetTx((tx - gx) / (tz - gz));
            rTr.GetParamLast()->SetTy((ty - gy) / (tz - gz));
            rTr.SetB(tHit.GetTimeStamp());  //temporary
            rTracks.push_back(rTr);
        }
    }
}

ClassImp(SrcVertexFinder);
