#include "TMath.h"
#include "TSystem.h"
#include "BmnScWallRaw2Digit.h"

void BmnScWallRaw2Digit::print() {
     printf("SCWALLLL\n");
}

BmnScWallRaw2Digit::BmnScWallRaw2Digit(){
  n_rec=0;
  runId = 0;
  periodId = 0;
  for (int i = 0; i < 6; i++) digiPar[i] = 0;
}

BmnScWallRaw2Digit::BmnScWallRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString CalibrationFile, TString MaxPositionFile) {
    periodId = period; 
    runId = run;
    n_rec=0;
    TString dummy;
    ifstream in;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";
    in.open((path + mappingFile).Data());
    if (!in.is_open())
    {
	printf("Loading ScWall Map from file: %s - file open error!\n", mappingFile.Data());
	return;
    }
    printf("Loading ScWall Map from file: %s\n", mappingFile.Data());
    
}



void BmnScWallRaw2Digit::fillEvent(TClonesArray *data, TClonesArray *ScWalldigit) {

	printf("ScWall fillEvent\n");
    for (int i=0; i<64; i++) ScWall_amp[i] = -1.;
    for (int i=0; i<64; i++) log_amp[i] = -1.;
    Float_t amp = 0., ped = 0.;
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
		printf("ScWall fillEvent %i\n", i);
       int ind;
       TClonesArray &ar_ScWall = *ScWalldigit;

       float signalMin = 0.;
       float signalMax = 0.;
       float signalPed = 0.;
       float signalInt = 0.;

       if ((amp = wave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped, &signalMin, &signalMax, &signalPed, &signalInt)) >= 0.)
       {
		   printf("ped %f\n", ped);

           new(ar_ScWall[ScWalldigit->GetEntriesFast()]) BmnScWallDigi(0, signalMin, 1, 
                signalMax, signalPed, signalInt, 99,
                99, 99, 99, 99, 99);


       }
    }
}

float BmnScWallRaw2Digit::wave2amp(UChar_t ns, UShort_t *s, Float_t *pedestal, Float_t *sigMin, Float_t *sigMax, Float_t *sigPed, Float_t *sigInt)
{
	    float pedest = 0., ampl = 0., signal = 0., signal_max = 0.;
	    int nsignal = 0, nsignal_max = 0, ismax = -1, ismin = -1, m1 = 0;

	    float ampl_max = -32768.;
            float ampl_min = 32768.;


            int pedBegin = digiPar[0];
            int pedEnd = digiPar[1];
            int gateBegin = digiPar[2];
            int gateEnd = digiPar[3];

            int signalThresh = digiPar[4];
            int signalType = digiPar[5]; // 0 - max, 1 - integral

            float scaleSignal = -1.0 / 16.; //invert + shift >>4

            float integral = 0.;


	    if (ns > 0) {
	      for (int m = 0; m < ns; m++) {

                float value = scaleSignal * (Short_t)s[m];

                if (m >= pedBegin && m <= pedEnd) {
		  pedest += value;
		  if (m == pedEnd) pedest /= (pedEnd - pedBegin + 1);
		  continue;
		}
	        else if (m >= gateBegin && m <= gateEnd) {
                  ampl = value - pedest;
                  integral += ampl;
                  if (ampl > ampl_max) {
                    ampl_max = ampl;
		    ismax = m;
		  }
                  if (ampl < ampl_min) {
                    ampl_min = ampl;
		    ismin = m;
		  }
		}
	      } // loop over samples

	      printf("Amplmax %f Ped %f imax %d\n", ampl_max, pedest, ismax);

              if (signalType == 0) signal_max = ampl_max;
              else signal_max = integral;

              //store pure signal parameters
              *sigMin = ampl_min;
              *sigMax = ampl_max;
              *sigPed = pedest;
              *sigInt = integral;

              if (fabs(ampl_max - ampl_min) < signalThresh) signal_max = 0.;

	    } // if samples exist
	    else {
              signal_max = -1.;
            }

	    *pedestal = pedest;
	    return signal_max;



}

BmnScWallRaw2Digit::~BmnScWallRaw2Digit()
{
}

ClassImp(BmnScWallRaw2Digit)

