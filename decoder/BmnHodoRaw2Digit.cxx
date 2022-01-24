#include "TMath.h"
#include "TSystem.h"
#include "BmnHodoRaw2Digit.h"

void BmnHodoRaw2Digit::print()
{
  printf("BmnHodoRaw2Digit : \n");
}

BmnHodoRaw2Digit::BmnHodoRaw2Digit()
{
  fPeriodId = 0;
  fRunId = 0;
}

BmnHodoRaw2Digit::BmnHodoRaw2Digit(int period, int run, TString mappingFile, TString CalibrationFile)
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

  fHodoSerials.clear();
  for (auto it : tqdc_serials)
    fHodoSerials.push_back(std::stoul(it, nullptr, 16));
  std::sort(fHodoSerials.begin(), fHodoSerials.end());

  std::string tqdc_ser;
  int tqdc_chan;
  std::string material;
  int strip_id;
  int strip_side;
  int gain;

  fChannelVect.clear();
  fChannelVect.resize(fHodoSerials.size() * 16);
  for (auto it : configuration)
  {
    istringstream ss(it);
    ss >> tqdc_ser >> tqdc_chan >> material >> strip_id >> strip_side >> gain;
    int board_index, xIdx, yIdx, SizeIdx, ZoneIdx = -1;
    auto iter = find(fHodoSerials.begin(), fHodoSerials.end(), std::stoul(tqdc_ser, nullptr, 16));
    if (iter != fHodoSerials.end())
      board_index = std::distance(fHodoSerials.begin(), iter);
    else
      printf("BmnHodoRaw2Digit : unknown adc serial\n");

    int mater = -1; 
    if(material[0] == 'S') mater = 0;
    if(material[0] == 'Q') mater = 1;
    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(std::stoul(tqdc_ser, nullptr, 16), tqdc_chan);
    unsigned int unique_address = (mater == -1) ? 0 : BmnHodoAddress::GetAddress(mater, strip_id, strip_side, gain);
    fChannelVect.at(flat_channel) = unique_address;
  }
  //std::LOG(DEBUG) << "COMMENT.str: " << comment << std::endl;
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
  fCalibVect.resize(BmnHodoAddress::GetMaxFlatAddress()+1);
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
    uint8_t flat_channel = BmnHodoAddress::GetFlatAddress(unique_address);
    fCalibVect.at(flat_channel) = std::make_pair(calibration, calibError);
  }
}

int BmnHodoRaw2Digit::GetFlatChannelFromAdcChannel(unsigned int tqdc_board_serial, unsigned int tqdc_ch)
{
  auto it = find(fHodoSerials.begin(), fHodoSerials.end(), tqdc_board_serial);
  if (it != fHodoSerials.end())
  {
    int tqdc_board_index = std::distance(fHodoSerials.begin(), it);
    return tqdc_board_index * 16 + tqdc_ch;
  }

  printf("BmnHodoRaw2Digit :: Serial 0x%08x Not found in map %s.\n", tqdc_board_serial, fmappingFileName.Data());
  return -1;
}

void BmnHodoRaw2Digit::fillEvent(TClonesArray *tdc, TClonesArray *adc, TClonesArray *Hododigit)
{

  LOG(DEBUG) << "BmnHodoRaw2Digit::fillEvent" << endl;

  for (int iAdc = 0; iAdc < adc->GetEntriesFast(); iAdc++)
  {
    BmnTQDCADCDigit *adcDig = (BmnTQDCADCDigit*) adc->At(iAdc);
    //    Double_t adcTimestamp = adcDig->GetAdcTimestamp() * ADC_CLOCK_TQDC16VS;
    //    Double_t trgTimestamp = adcDig->GetTrigTimestamp() * ADC_CLOCK_TQDC16VS;

    // check if serial is from Hodo
    // cout<<adcDig->GetSerial() << " " << adcDig->GetChannel() << endl;
    if (std::find(fHodoSerials.begin(), fHodoSerials.end(), adcDig->GetSerial()) == fHodoSerials.end()) {
      LOG(DEBUG) << "BmnHodoRaw2Digit::fillEvent" << std::hex << adcDig->GetSerial() << " Not found in ";
      for (auto it : fHodoSerials)
        LOG(DEBUG) << "BmnHodoRaw2Digit::fHodoSerials " << std::hex << it << endl;
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
    if (abs(ThisDigi.fSignal) < fdigiPars.threshold)
      continue;

    TClonesArray &ar_Hodo = *Hododigit;
    new (ar_Hodo[Hododigit->GetEntriesFast()]) BmnHodoDigi(ThisDigi);
  }
}

void BmnHodoRaw2Digit::ProcessWfm(std::vector<float> wfm, BmnHodoDigi *digi)
{
  assert(fdigiPars.gateBegin > 0 && fdigiPars.gateEnd > 0);
  if(fdigiPars.gateBegin >= wfm.size()) { 
    LOG(ERROR) << "BmnScWallRaw2Digit:: waveform too short: accessing " << 
    fdigiPars.gateBegin << "/" << wfm.size() << ". Check calibration file " << fcalibrationFileName;
    fdigiPars.gateBegin = wfm.size()-1;
  }
  if(fdigiPars.gateEnd >= wfm.size()) { 
    LOG(ERROR) << "BmnScWallRaw2Digit:: waveform too short: accessing " << 
    fdigiPars.gateEnd << "/" << wfm.size() << ". Check calibration file " << fcalibrationFileName;
    fdigiPars.gateEnd = wfm.size()-1;
  }

  // Invert
  if (fdigiPars.doInvert)
  {
    LOG(DEBUG) << "BmnHodoRaw2Digit::ProcessWfm Inverting" << endl;
    float myconstant{-1.0};
    std::transform(wfm.begin(), wfm.end(), wfm.begin(),
                   std::bind1st(std::multiplies<float>(), myconstant));
  }

  //Zero level calculation
  LOG(DEBUG) << "BmnHodoRaw2Digit::ProcessWfm ZL calc" << endl;
  const int n_gates = 3;
  int gate_npoints = (int)floor((fdigiPars.gateBegin - 2.) / n_gates);

  Float_t gates_mean[n_gates], gates_rms[n_gates];
  for (int igate = 0; igate < n_gates; igate++)
    MeanRMScalc(wfm, gates_mean + igate, gates_rms + igate, igate * gate_npoints, (igate + 1) * gate_npoints);

  int best_gate = 0;
  for (int igate = 0; igate < n_gates; igate++)
    if (gates_rms[igate] < gates_rms[best_gate])
      best_gate = igate;
  digi->fZL = (int) gates_mean[best_gate];

  //MAX and Integral calculation including borders
  LOG(DEBUG) << "BmnHodoRaw2Digit::ProcessWfm  MAX & INT search" << endl;
  digi->fIntegral = (int) std::accumulate(wfm.begin() + fdigiPars.gateBegin, wfm.begin() + fdigiPars.gateEnd + 1,
                                    -digi->fZL * (fdigiPars.gateEnd - fdigiPars.gateBegin + 1));
  auto const max_iter = std::max_element(wfm.begin() + fdigiPars.gateBegin, wfm.begin() + fdigiPars.gateEnd + 1);
  digi->fAmpl = (int) *max_iter - digi->fZL;
  digi->fTimeMax = (int) std::distance(wfm.begin(), max_iter);

  //Apply calibration
  LOG(DEBUG) << "BmnHodoRaw2Digit::ProcessWfm  Calibration" << endl;
  uint8_t flat_channel = BmnHodoAddress::GetFlatAddress(digi->GetAddress());
  assert(flat_channel < fCalibVect.size());
  if (fdigiPars.signalType == 0)
    digi->fSignal = (float) digi->fAmpl * fCalibVect.at(flat_channel).first;
  if (fdigiPars.signalType == 1)
    digi->fSignal = (float) digi->fIntegral * fCalibVect.at(flat_channel).first;

  //Prony fitting procedure
  PsdSignalFitting::PronyFitter Pfitter;
  if (fdigiPars.isfit)
  {
    LOG(DEBUG) << "BmnHodoRaw2Digit::ProcessWfm  Fitting" << endl;
    Pfitter.Initialize(fdigiPars.harmonics.size(), fdigiPars.harmonics.size(), fdigiPars.gateBegin, fdigiPars.gateEnd);
    Pfitter.SetDebugMode(0);
    Pfitter.SetWaveform(wfm, digi->fZL);
    int SignalBeg = Pfitter.CalcSignalBeginStraight();
    if (SignalBeg < 1 || SignalBeg > wfm.size())
      return;
    Pfitter.SetExternalHarmonics(fdigiPars.harmonics[0], fdigiPars.harmonics[1]);
    int best_signal_begin = Pfitter.ChooseBestSignalBegin(SignalBeg - 1, SignalBeg + 1);
    Pfitter.SetSignalBegin(best_signal_begin);
    Pfitter.CalculateFitAmplitudes();

    digi->fFitIntegral = Pfitter.GetIntegral(fdigiPars.gateBegin, fdigiPars.gateEnd);
    digi->fFitAmpl = Pfitter.GetMaxAmplitude() - Pfitter.GetZeroLevel();
    float fit_R2 = Pfitter.GetRSquare(fdigiPars.gateBegin, fdigiPars.gateEnd);
    digi->fFitR2 = (fit_R2 > 2.0) ? 2.0 : fit_R2;
    digi->fFitZL = Pfitter.GetZeroLevel();
    digi->fFitTimeMax = Pfitter.GetSignalMaxTime();
  }

  if (fdigiPars.isWriteWfm) {
    digi->fWfm = wfm;
    if (fdigiPars.isfit) 
      digi->fFitWfm = Pfitter.GetFitWfm();
  }
}

void BmnHodoRaw2Digit::MeanRMScalc(std::vector<float> wfm, float *Mean, float *RMS, int begin, int end, int step)
{
  begin = (begin < 0) ? 0 : begin;
  if (begin > end)
  {
    float swap = end;
    end = begin;
    begin = swap;
  };
  step = TMath::Abs(step);
  *Mean = *RMS = 0.;
  int Delta = 0;
  for (int n = begin; n <= end; n += step)
  {
    *Mean += wfm[n];
    Delta++;
  }
  *Mean /= (float)Delta;
  for (int n = begin; n <= end; n += step)
    *RMS += (wfm[n] - *Mean) * (wfm[n] - *Mean);
  *RMS = TMath::Sqrt(*RMS / ((float)Delta));
  //printf("AMPL %.2f, RMS %.2f\n",*Mean,*RMS);
}

BmnHodoRaw2Digit::~BmnHodoRaw2Digit()
{
}

ClassImp(BmnHodoRaw2Digit)
