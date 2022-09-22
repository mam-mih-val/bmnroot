#include "TMath.h"
#include "TSystem.h"
#include "BmnHodoRaw2Digit.h"

void BmnHodoRaw2Digit::print()
{
  printf("BmnHodoRaw2Digit : \n");
}

BmnHodoRaw2Digit::BmnHodoRaw2Digit() : WfmProcessor()
{
  fPeriodId = 0;
  fRunId = 0;
}

BmnHodoRaw2Digit::BmnHodoRaw2Digit(int period, int run, TString mappingFile, TString CalibrationFile) : WfmProcessor()
{
  fPeriodId = period;
  fRunId = run;
  fmappingFileName = mappingFile;
  ParseConfig(fmappingFileName);
  fcalibrationFileName = CalibrationFile;
  ParseCalibration(fcalibrationFileName);
}

void BmnHodoRaw2Digit::ParseConfig(TString mappingFile)
{

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/input/";

  typedef std::vector<std::string> vect_string_t;
  float version;
  std::string comment;
  std::vector<std::string> tqdc_serials;
  std::vector<std::string> configuration;

  // Setup options.
  po::options_description desc("Options");
  desc.add_options()
    ("VERSION.id", po::value<float>(&version), "version identificator")
    ("COMMENT.str", po::value<std::string>(&comment), "comment")
    ("TQDCSERIALS.serial", po::value<vect_string_t>(&tqdc_serials)->multitoken(), "tqdc serials")
    ("CONFIGURATION.config", po::value<vect_string_t>(&configuration)->multitoken(), "configuration")
    ;

  // Load config file.
  po::variables_map vm;
  std::ifstream config_file((path + mappingFile).Data(), std::ifstream::in);
  if (!config_file.is_open())
  {
    printf("BmnHodoRaw2Digit : Loading Config from file: %s - file open error!\n", mappingFile.Data());
    return;
  }
  printf("BmnHodoRaw2Digit : Loading Config from file: %s\n", mappingFile.Data());
  po::store(po::parse_config_file(config_file, desc), vm);
  config_file.close();
  po::notify(vm);

  fSerials.clear();
  for (auto it : tqdc_serials)
    fSerials.push_back(std::stoul(it, nullptr, 16));
  std::sort(fSerials.begin(), fSerials.end());

  std::string tqdc_ser;
  int tqdc_chan;
  std::string material;
  int strip_id;
  int strip_side;
  int gain;

  fChannelVect.clear();
  fChannelVect.resize(fSerials.size() * CHANNELS_PER_BOARD);
  for (auto it : configuration)
  {
    istringstream ss(it);
    ss >> tqdc_ser >> tqdc_chan >> material >> strip_id >> strip_side >> gain;
    int board_index, xIdx, yIdx, SizeIdx, ZoneIdx = -1;
    auto iter = find(fSerials.begin(), fSerials.end(), std::stoul(tqdc_ser, nullptr, 16));
    if (iter != fSerials.end())
      board_index = std::distance(fSerials.begin(), iter);
    else
      printf("BmnHodoRaw2Digit : unknown adc serial\n");

    int mater = -1; 
    if(material[0] == 'S') mater = 0;
    if(material[0] == 'Q') mater = 1;
    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(std::stoul(tqdc_ser, nullptr, 16), tqdc_chan);
    unsigned int unique_address = (mater == -1) ? 0 : BmnHodoAddress::GetAddress(mater, strip_id, strip_side, gain);
    fChannelVect.at(flat_channel) = unique_address;
  }
  //std::LOG(debug) << "COMMENT.str: " << comment << std::endl;
}

void BmnHodoRaw2Digit::ParseCalibration(TString calibrationFile)
{

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/hodo/";

  typedef std::vector<std::string> vect_string_t;
  typedef std::vector<std::complex<float>> vect_complf_t;
  float version;
  std::string comment;
  std::vector<std::string> calibrations;

  // Setup options.
  po::options_description desc("Options");
  desc.add_options()
    ("VERSION.id", po::value<float>(&version), "version identificator")
    ("COMMENT.str", po::value<std::string>(&comment), "comment")
    ("CHECKER.isWriteWfm", po::value<bool>(&fdigiPars.isWriteWfm), "writing waveforms")
    ("PARAMETERS.gateBegin", po::value<int>(&fdigiPars.gateBegin), "digi parameters")
    ("PARAMETERS.gateEnd", po::value<int>(&fdigiPars.gateEnd), "digi parameters")
    ("PARAMETERS.threshold", po::value<float>(&fdigiPars.threshold), "digi parameters")
    ("PARAMETERS.signalType", po::value<int>(&fdigiPars.signalType), "digi parameters")
    ("PARAMETERS.doInvert", po::value<bool>(&fdigiPars.doInvert), "digi parameters")
    ("FITPARAMETERS.isfit", po::value<bool>(&fdigiPars.isfit), "digi parameters")
    ("FITPARAMETERS.harmonic", po::value<vect_complf_t>(&fdigiPars.harmonics)->multitoken(), "fit harmonics")
    ("CALIBRATION.calib", po::value<vect_string_t>(&calibrations)->multitoken(), "calibrations")
    ;

  // Load config file.
  po::variables_map vm;
  std::ifstream calib_file((path + calibrationFile).Data(), std::ifstream::in);
  if (!calib_file.is_open())
  {
    printf("BmnHodoRaw2Digit : Loading Calibration from file: %s - file open error!\n", calibrationFile.Data());
    return;
  }
  printf("BmnHodoRaw2Digit : Loading Calibration from file: %s\n", calibrationFile.Data());
  po::store(po::parse_config_file(calib_file, desc), vm);
  calib_file.close();
  po::notify(vm);

  fCalibVect.clear();
  fCalibVect.resize(BmnHodoAddress::GetMaxFlatIndex()+1);
  std::string material;
  int strip_id;
  int strip_side;
  int gain;
  float calibration;
  float calibError;
  for (auto it : calibrations)
  {
    istringstream ss(it);
    ss >> material >> strip_id >> strip_side >> gain >> calibration >> calibError;
    int mater = -1; 
    if(material[0] == 'S') mater = 0;
    if(material[0] == 'Q') mater = 1;
    if(mater == -1) continue;
    unsigned int unique_address = BmnHodoAddress::GetAddress(mater, strip_id, strip_side, gain);
    uint8_t flat_channel = BmnHodoAddress::GetFlatIndex(unique_address);
    fCalibVect.at(flat_channel) = std::make_pair(calibration, calibError);
  }

  if(fdigiPars.isfit) {
    int model_order = fdigiPars.harmonics.size() + 1;
    fSignalLength = fdigiPars.gateEnd - fdigiPars.gateBegin + 1;
    fAZik = new std::complex<float> *[model_order];
    for (int i = 0; i < model_order; i++) {
      fAZik[i] = new std::complex<float>[model_order];
      for (int j = 0; j < model_order; j++)
        fAZik[i][j] = {0., 0.};
    }
    PsdSignalFitting::PronyFitter Pfitter;
    Pfitter.Initialize(fdigiPars.harmonics.size(), fdigiPars.harmonics.size(), fdigiPars.gateBegin, fdigiPars.gateEnd);
    Pfitter.SetExternalHarmonics(fdigiPars.harmonics);
    Pfitter.MakeInvHarmoMatrix(fSignalLength, fAZik);
  }

}

std::pair<float,float> BmnHodoRaw2Digit::GetCalibPairFromAddress(unsigned int address)
{
  uint8_t flat_channel = BmnHodoAddress::GetFlatIndex(address);
  return fCalibVect.at(flat_channel);
}

int BmnHodoRaw2Digit::GetFlatChannelFromAdcChannel(unsigned int board_serial, unsigned int channel)
{
  auto it = find(fSerials.begin(), fSerials.end(), board_serial);
  if (it != fSerials.end())
  {
    int board_index = std::distance(fSerials.begin(), it);
    return board_index * CHANNELS_PER_BOARD + channel;
  }

  printf("BmnHodoRaw2Digit :: Serial 0x%08x Not found in map %s.\n", board_serial, fmappingFileName.Data());
  return -1;
}

void BmnHodoRaw2Digit::fillEvent(TClonesArray *tdc, TClonesArray *adc, TClonesArray *Hododigit)
{

  LOG(debug) << "BmnHodoRaw2Digit::fillEvent" << endl;

  for (int iAdc = 0; iAdc < adc->GetEntriesFast(); iAdc++)
  {
    BmnTQDCADCDigit *adcDig = (BmnTQDCADCDigit*) adc->At(iAdc);
    //    Double_t adcTimestamp = adcDig->GetAdcTimestamp() * ADC_CLOCK_TQDC16VS;
    //    Double_t trgTimestamp = adcDig->GetTrigTimestamp() * ADC_CLOCK_TQDC16VS;

    // check if serial is from Hodo
    // cout<<adcDig->GetSerial() << " " << adcDig->GetChannel() << endl;
    if (std::find(fSerials.begin(), fSerials.end(), adcDig->GetSerial()) == fSerials.end()) {
      LOG(debug) << "BmnHodoRaw2Digit::fillEvent" << std::hex << adcDig->GetSerial() << " Not found in ";
      for (auto it : fSerials)
        LOG(debug) << "BmnHodoRaw2Digit::fSerials " << std::hex << it << endl;
      continue;
    }

    std::vector<float> wfm(adcDig->GetShortValue(), adcDig->GetShortValue() + adcDig->GetNSamples());
    BmnHodoDigi ThisDigi;
    ThisDigi.reset();
    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(adcDig->GetSerial(), adcDig->GetChannel());
    assert(flat_channel < fChannelVect.size());
    ThisDigi.fuAddress = fChannelVect.at(flat_channel);
    if (ThisDigi.fuAddress == 0)
      continue; // not connected lines
    ProcessWfm(wfm, &ThisDigi);

    //Apply calibration
    LOG(debug) << "BmnHodoRaw2Digit::ProcessWfm  Calibration" << endl;
    uint8_t flat_index = BmnHodoAddress::GetFlatIndex(ThisDigi.GetAddress());
    assert(flat_index < fCalibVect.size());
    if (fdigiPars.signalType == 0)
      ThisDigi.fSignal = (float) ThisDigi.fAmpl * fCalibVect.at(flat_index).first;
    if (fdigiPars.signalType == 1)
      ThisDigi.fSignal = (float) ThisDigi.fIntegral * fCalibVect.at(flat_index).first;
    if (abs(ThisDigi.fSignal) < fdigiPars.threshold)
      continue;

    TClonesArray &ar_Hodo = *Hododigit;
    new (ar_Hodo[Hododigit->GetEntriesFast()]) BmnHodoDigi(ThisDigi);
  }
}

BmnHodoRaw2Digit::~BmnHodoRaw2Digit()
{
}

ClassImp(BmnHodoRaw2Digit)
