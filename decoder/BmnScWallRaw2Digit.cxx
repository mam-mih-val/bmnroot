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

void BmnScWallRaw2Digit::ParseConfig(TString mappingFile) {

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/input/";

  typedef std::vector< std::string > vect_string_t;
  float version;
  std::string comment;
  std::vector<std::string> adc_serials;
  std::vector<std::string> configuration;

  // Setup options.
  po::options_description desc("Options");
  desc.add_options()
    ("VERSION.id", po::value< float >( &version ), "version identificator" )
    ("COMMENT.str", po::value< std::string >( &comment ), "comment" )
    ("ADCSERIALS.serial", po::value< vect_string_t >( &adc_serials )->multitoken(),
                     "adc serials" )
    ("CONFIGURATION.config", po::value< vect_string_t >( &configuration )->multitoken(),
                     "configuration" )
    ;

  // Load config file.
  po::variables_map vm;
  std::ifstream config_file( (path + mappingFile).Data() , std::ifstream::in );
  if (!config_file.is_open()) {
	  printf("BmnScWallRaw2Digit : Loading Config from file: %s - file open error!\n", mappingFile.Data());
	  return;
  }
  printf("BmnScWallRaw2Digit : Loading Config from file: %s\n", mappingFile.Data());
  po::store( po::parse_config_file( config_file , desc ), vm );
  config_file.close();
  po::notify( vm );

  fScWallSerials.clear();
  for ( auto it : adc_serials )
    fScWallSerials.push_back(std::stoul(it, nullptr, 16));
  std::sort(fScWallSerials.begin(), fScWallSerials.end());

  std::string adc_ser;
  int adc_chan;
  std::string zone;
  int x_position;
  int y_position;
  int size;

  fUniqueX.clear();
  fUniqueY.clear();
  fUniqueSize.clear();
  // First read for unique.
  for ( auto it : configuration )
  {
    istringstream ss(it);
    ss >> adc_ser >> adc_chan >> zone >> x_position >> y_position >> size;
    //cout<< adc_ser << " " << adc_chan << " " << zone << endl;
    fUniqueX.insert(x_position);
    fUniqueY.insert(y_position);
    fUniqueSize.insert(size);
  }

  // Second read for mapping.
  for ( auto it : configuration )
  {
    istringstream ss(it);
    ss >> adc_ser >> adc_chan >> zone >> x_position >> y_position >> size;
    //cout<< adc_ser << " " << adc_chan << " " << zone << endl;

    int adc_board_index, xIdx, yIdx, SizeIdx, ZoneIdx = -1;
    {
    auto iter = find(fScWallSerials.begin(), fScWallSerials.end(), std::stoul(adc_ser, nullptr, 16));
    if (iter != fScWallSerials.end())
      adc_board_index = std::distance(fScWallSerials.begin(), iter);
    else printf("BmnScWallRaw2Digit : unknown adc serial\n");
    }
    {
    auto iter = find(fUniqueX.begin(), fUniqueX.end(), x_position);
    xIdx = std::distance(fUniqueX.begin(), iter);
    }
    {
    auto iter = find(fUniqueY.begin(), fUniqueY.end(), y_position);
    yIdx = std::distance(fUniqueY.begin(), iter);
    }
    {
    auto iter = find(fUniqueSize.begin(), fUniqueSize.end(), size);
    SizeIdx = std::distance(fUniqueSize.begin(), iter);
    }

    ZoneIdx = (int)(zone[0] - 'A' + 1);
    if (ZoneIdx > 12) { cout << "MAX zone letter is L" << endl; continue; }

    UInt_t flat_channel = (UInt_t) GetFlatChannelFromAdcChannel( std::stoul(adc_ser, nullptr, 16), adc_chan );
    UInt_t unique_address = BmnScWallAddress::GetAddress( adc_board_index, adc_chan, xIdx, yIdx, SizeIdx, ZoneIdx);
    fChannelMap.insert(std::pair<UInt_t, UInt_t>(flat_channel, unique_address));
  }
  //std::cout << "COMMENT.str: " << comment << std::endl;

}

Int_t BmnScWallRaw2Digit::GetFlatChannelFromAdcChannel(UInt_t adc_board_serial, UInt_t adc_ch) {
    auto it = find(fScWallSerials.begin(), fScWallSerials.end(), adc_board_serial);
    if (it != fScWallSerials.end()) {
      int adc_board_index = std::distance(fScWallSerials.begin(), it);
      return adc_board_index*100 + adc_ch;
    }
    return -1;
}

BmnScWallRaw2Digit::BmnScWallRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString CalibrationFile, TString MaxPositionFile) {
    periodId = period; 
    runId = run;
    n_rec=0;
    TString dummy;

    ParseConfig(mappingFile);
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

