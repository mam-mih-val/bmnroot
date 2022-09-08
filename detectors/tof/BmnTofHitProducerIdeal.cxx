#include "BmnTofHitProducerIdeal.h"
#include "BmnTOFPoint.h"
#include "BmnTofHit.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "TFile.h"

#include <assert.h>

BmnTofHitProducerIdeal::BmnTofHitProducerIdeal(const char *name, Bool_t useMCdata, Int_t verbose, Bool_t test, Bool_t merge)
: BmnTask(name, verbose),
  fDoTest(test),
  fDoMergeHits(merge),
  fUseMCData(useMCdata),
  fOnlyPrimary(false),
  fTestFlnm("test.BmnTofHitProducerIdeal.root"),
  aMcPoints(nullptr),
  aMcTracks(nullptr),
  aExpDigits(nullptr),
  aTofHits(nullptr)
{
    if (fDoTest)
    {
        h1TestOccup = new TH1D("TOF700_TestOccup", "occupancy per strips;occupancy;Events", 100, -0.5, 99.5);
        fList.Add(h1TestOccup);
        h2TestMergedTimes = new TH2D("TOF700_TestMergedTimes",
                                     "Merged hits on strip times test;faster hit time, ns;slower hit time, ns",
                                     1000, 5., 105., 1000, 5., 105.);
        fList.Add(h2TestMergedTimes);
    }
}

BmnTofHitProducerIdeal::~BmnTofHitProducerIdeal()
{
}

InitStatus BmnTofHitProducerIdeal::Init()
{
    LOG(info) << "[BmnTofHitProducerIdeal::Init] Begin initialization.";

    FairRootManager *ioman = FairRootManager::Instance();
    assert(ioman);

    if (fUseMCData)
    {
        aMcPoints = (TClonesArray *)FairRootManager::Instance()->GetObject("TOF700Point");
        if (!aMcPoints) {
            cout << "BmnTofHitProducerIdeal::Init(): branch TOFPoint not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
        aMcTracks = (TClonesArray *)FairRootManager::Instance()->GetObject("MCTrack");
        if (!aMcTracks) {
            cout << "BmnTofHitProducerIdeal::Init(): branch MCTrack not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
    } else {
        aExpDigits = (TClonesArray *)FairRootManager::Instance()->GetObject("bmn_tof700");
        if (!aExpDigits) {
            cout << "BmnTofHitProducerIdeal::Init(): branch bmn_tof700 not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
    }

    // Create and register output array
    aTofHits = new TClonesArray("BmnTofHit");
    ioman->Register("TOF700Hit", "TOF", aTofHits, kTRUE);

    LOG(info) << "[BmnTofHitProducerIdeal::Init] Initialization finished succesfully.";

    return kSUCCESS;
}

void BmnTofHitProducerIdeal::Exec(Option_t *opt)
{
    if (!IsActive())
        return;

    static const TVector3 XYZ_err(0., 0., 0.);  // FIXME:

    aTofHits->Clear();

    Int_t nSingleHits = 0;
    TVector3 pos;

    if (fUseMCData)
    {
        BmnTOFPoint *pPoint;
        Int_t nTofPoint = aMcPoints->GetEntriesFast();

        for (Int_t pointIndex = 0; pointIndex < nTofPoint; pointIndex++)  // cycle by TOF points
        {
            pPoint = (BmnTOFPoint *)aMcPoints->UncheckedAt(pointIndex);
            pPoint->Position(pos);

            AddHit(pPoint->GetDetectorID(), pos, XYZ_err, pointIndex, pPoint->GetTrackID(), pPoint->GetTime());
            nSingleHits++;
        }
    }

    int nFinally;
    if (fDoMergeHits)
    {
        MergeHitsOnStrip();         // save only the fastest hit in the strip
        nFinally = CompressHits();  // remove blank slotes
    }
    else nFinally = aTofHits->GetEntriesFast();

    cout << " -I- [BmnTofHitProducerIdeal::Exec]  single hits= " << nSingleHits << ", final hits= " << nFinally << endl;
}

void BmnTofHitProducerIdeal::Finish()
{
    if (fDoTest)
    {
        LOG(info) << " [BmnTofHitProducerIdeal::Finish] Update " << fTestFlnm.Data() << " file. ";
        TFile *ptr = gFile;
        TFile file(fTestFlnm.Data(), "RECREATE");
        fList.Write();
        file.Close();
        gFile = ptr;
    }
}

Int_t BmnTofHitProducerIdeal::MergeHitsOnStrip(void)
{
    typedef map<Int_t, BmnTofHit *> hitsMapType;
    hitsMapType fHits;  // pair<detectorUID, BmnTofHit*> fastest hits map
    hitsMapType::iterator it;
    Int_t mergedNmb = 0;

    typedef multiset<Int_t> msUIDsType;  // detectorUID for Hits
    msUIDsType UIDs;

    BmnTofHit *fastHit, *slowHit;
    for (Int_t hitIndex = 0, nHits = aTofHits->GetEntriesFast(); hitIndex < nHits; hitIndex++)  // cycle by hits
    {
        BmnTofHit *pHit = (BmnTofHit *)aTofHits->UncheckedAt(hitIndex);
        assert(nullptr != pHit);

        Int_t UID = pHit->GetDetectorID();

        if (fDoTest) UIDs.insert(UID);

        it = fHits.find(UID);
        if (it != fHits.end())  // hit for this detectorUID already exist
        {
            mergedNmb++;

            if (pHit->GetTimeStamp() < it->second->GetTimeStamp())  //  faster hit  found
            {
                fastHit = pHit;
                slowHit = it->second;
            } else {
                fastHit = it->second;
                slowHit = pHit;
            }
            if (fDoTest) h2TestMergedTimes->Fill(fastHit->GetTimeStamp(), slowHit->GetTimeStamp());

            fastHit->AddLinks(slowHit->GetLinks());  // copy links
            aTofHits->Remove(slowHit);               // remove old hit   --> make blank slote !!
            it->second = fastHit;                    // change pair value to current UID
        } else
            fHits.insert(make_pair(UID, pHit));  // insert new detectorUID pair

    }  // cycle by hits

    // cycle by detector UIDs list
    if (fDoTest)
        for (msUIDsType::const_iterator it1 = UIDs.begin(), itEnd = UIDs.end(); it1 != itEnd;
             it1 = UIDs.upper_bound(*it1))
            h1TestOccup->Fill(UIDs.count(*it1));

    return mergedNmb;
}

void BmnTofHitProducerIdeal::AddHit(Int_t detUID, const TVector3 &posHit, const TVector3 &posHitErr, Int_t pointIndex,
                                    Int_t trackIndex, Double_t time)
{
    BmnTofHit *pHit = new ((*aTofHits)[aTofHits->GetEntriesFast()]) BmnTofHit(detUID, posHit, posHitErr, pointIndex);

    //FairRootManager::Instance()->SetUseFairLinks(kTRUE);
    pHit->SetTimeStamp(time);
    pHit->AddLink(FairLink(0x1, pointIndex));
    pHit->AddLink(FairLink(0x2, trackIndex));
    pHit->AddLink(FairLink(0x4, detUID));
    //FairRootManager::Instance()->SetUseFairLinks(kFALSE);
}

Int_t BmnTofHitProducerIdeal::CompressHits()
{
    aTofHits->Compress();
    return aTofHits->GetEntriesFast();
}

void BmnTofHitProducerIdeal::Dump(const char *title, ostream &out) const
{
    out << "\n [BmnTofHitProducerIdeal::Dump]   ";
    if (title) out << title;
    out << ", size= " << aTofHits->GetEntriesFast();

    BmnTOFPoint *point;
    BmnTofHit *pHit;
    TVector3 hitPos, pointPos;

    TIterator *iter = aTofHits->MakeIterator();
    while ((pHit = (BmnTofHit *)iter->Next()))
    {
        pHit->Position(hitPos);
        out << "\n    hit detUID = " << pHit->GetDetectorID() << ", hit pos(" << hitPos.X() << "," << hitPos.Y() << ","
            << hitPos.Z() << "), ";
        if (aMcPoints) {
            point = (BmnTOFPoint *)aMcPoints->UncheckedAt(pHit->GetRefIndex());
            point->Position(pointPos);
            out << "\n point detUID = " << point->GetDetectorID() << ", point pos(" << pointPos.X() << ","
                << pointPos.Y() << "," << pointPos.Z() << "), dev=" << (hitPos - pointPos).Mag();
        }
    }

    delete iter;
}

ClassImp(BmnTofHitProducerIdeal)
