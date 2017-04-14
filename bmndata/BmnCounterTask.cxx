

#define ANSI_COLOR_RED   "\x1b[91m"
#define ANSI_COLOR_BLUE  "\x1b[94m"
#define ANSI_COLOR_RESET "\x1b[0m"


#include "BmnCounterTask.h"

BmnCounter::BmnCounter() :
fEvHead(NULL),
fIEvent(0),
fNEvents(0) {
}

BmnCounter::BmnCounter(Long64_t n) :
fEvHead(NULL),
fIEvent(0),
fNEvents(n) {
}

BmnCounter::~BmnCounter() {
}

InitStatus BmnCounter::Init() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }
    fEvHead = (TClonesArray*) ioman->GetObject("EventHeader");
    if (!fEvHead)
        cout << "WARNING! No EventHeader array!!!" << endl;

    if (ioman->CheckMaxEventNo(fNEvents) < fNEvents)
        fNEvents = ioman->CheckMaxEventNo(fNEvents);

}

void BmnCounter::Exec(Option_t* opt) {

    printf(ANSI_COLOR_BLUE "RUN-" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "%d" ANSI_COLOR_RESET, ((BmnEventHeader*) fEvHead->At(0))->GetRunId());
    printf(ANSI_COLOR_BLUE ": [");

    Float_t progress = fIEvent * 1.0 / fNEvents;
    Int_t barWidth = 70;

    Int_t pos = barWidth * progress;
    for (Int_t i = 0; i < barWidth; ++i) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }

    printf("] " ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "%d%%\r" ANSI_COLOR_RESET, Int_t(progress * 100.0 + 0.5));
    cout.flush();
    fIEvent++;
}

void BmnCounter::Finish() {
    printf("\n");
}