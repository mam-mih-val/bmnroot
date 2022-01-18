#include "TMath.h"
#include "TSystem.h"
#include "BmnFHCalRaw2Digit.h"

void BmnFHCalRaw2Digit::print()
{
  printf("BmnFHCalRaw2Digit : \n");
}

BmnFHCalRaw2Digit::BmnFHCalRaw2Digit()
{
  fPeriodId = 0;
  fRunId = 0;
}

BmnFHCalRaw2Digit::BmnFHCalRaw2Digit(int period, int run, TString mappingFile, TString CalibrationFile)
{
  fPeriodId = period;
  fRunId = run;
  fmappingFileName = mappingFile;
  ParseConfig(fmappingFileName);
  fcalibrationFileName = CalibrationFile;
  ParseCalibration(fcalibrationFileName);
}

void BmnFHCalRaw2Digit::ParseConfig(TString mappingFile)
{

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/input/";

  typedef std::vector<std::string> vect_string_t;
  float version;
  std::string comment;
  std::vector<std::string> adc_serials;
  std::vector<std::string> configuration;

  // Setup options.
  po::options_description desc("Options");
  desc.add_options()
    ("VERSION.id", po::value<float>(&version), "version identificator")
    ("COMMENT.str", po::value<std::string>(&comment), "comment")
    ("ADCSERIALS.serial", po::value<vect_string_t>(&adc_serials)->multitoken(), "adc serials")
    ("CONFIGURATION.config", po::value<vect_string_t>(&configuration)->multitoken(), "configuration")
    ;

  // Load config file.
  po::variables_map vm;
  std::ifstream config_file((path + mappingFile).Data(), std::ifstream::in);
  if (!config_file.is_open())
  {
    printf("BmnFHCalRaw2Digit : Loading Config from file: %s - file open error!\n", mappingFile.Data());
    return;
  }
  printf("BmnFHCalRaw2Digit : Loading Config from file: %s\n", mappingFile.Data());
  po::store(po::parse_config_file(config_file, desc), vm);
  config_file.close();
  po::notify(vm);

  fFHCalSerials.clear();
  for (auto it : adc_serials)
    fFHCalSerials.push_back(std::stoul(it, nullptr, 16));
  std::sort(fFHCalSerials.begin(), fFHCalSerials.end());

  std::string adc_ser;
  int adc_chan;
  int module_type;
  int module_id;
  int section_id;
  int x_position;
  int y_position;
  int z_position;

  fUniqueX.clear();
  fUniqueY.clear();
  fUniqueZ.clear();
  // First pass for unique.
  for (auto it : configuration)
  {
    istringstream ss(it);
    ss >> adc_ser >> adc_chan >> module_type >> module_id >> section_id >> x_position >> y_position >> z_position;
    assert(module_type <= 2);
    assert(section_id <= 10);
    fUniqueX.insert(x_position);
    fUniqueY.insert(y_position);
    fUniqueZ.insert(z_position);
  }

  fChannelVect.clear();
  fChannelVect.resize(fFHCalSerials.size() * 64);
  // Second pass for mapping.
  for (auto it : configuration)
  {
    istringstream ss(it);
    ss >> adc_ser >> adc_chan >> module_type >> module_id >> section_id >> x_position >> y_position >> z_position;
    int adc_board_index, xIdx, yIdx, zIdx = -1;
    auto iter = find(fFHCalSerials.begin(), fFHCalSerials.end(), std::stoul(adc_ser, nullptr, 16));
    if (iter != fFHCalSerials.end())
      adc_board_index = std::distance(fFHCalSerials.begin(), iter);
    else
      printf("BmnFHCalRaw2Digit : unknown adc serial\n");

    xIdx = std::distance(fUniqueX.begin(), fUniqueX.find(x_position));
    yIdx = std::distance(fUniqueY.begin(), fUniqueY.find(y_position));
    zIdx = std::distance(fUniqueZ.begin(), fUniqueZ.find(z_position));

    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(std::stoul(adc_ser, nullptr, 16), adc_chan);
    unsigned int unique_address = (module_type == 0) ? 0 : BmnFHCalAddress::GetAddress(module_type, module_id, section_id, xIdx, yIdx, zIdx);
    fChannelVect.at(flat_channel) = unique_address;
  }
  //std::LOG(DEBUG) << "COMMENT.str: " << comment << std::endl;
}

void BmnFHCalRaw2Digit::ParseCalibration(TString calibrationFile)
{

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/fhcal/";

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
    printf("BmnFHCalRaw2Digit : Loading Calibration from file: %s - file open error!\n", calibrationFile.Data());
    return;
  }
  printf("BmnFHCalRaw2Digit : Loading Calibration from file: %s\n", calibrationFile.Data());
  po::store(po::parse_config_file(calib_file, desc), vm);
  calib_file.close();
  po::notify(vm);

  int mod_id;
  int sec_id;
  float calibration;
  float calibError;
  int max_mod_id = 0;
  // First pass for max mod id.
  for (auto it : calibrations)
  {
    istringstream ss(it);
    ss >> mod_id >> sec_id >> calibration >> calibError;
    if (mod_id > max_mod_id) max_mod_id = mod_id;
  }
  fCalibVect.clear();
  fCalibVect.resize(GetFlatCaloChannel(max_mod_id, 10) + 1);

  // Second pass for calibrations.
  for (auto it : calibrations)
  {
    istringstream ss(it);
    ss >> mod_id >> sec_id >> calibration >> calibError;
    fCalibVect.at(GetFlatCaloChannel(mod_id, sec_id)) = std::make_pair(calibration, calibError);
  }

}

int BmnFHCalRaw2Digit::GetFlatChannelFromAdcChannel(unsigned int adc_board_serial, unsigned int adc_ch)
{
  auto it = find(fFHCalSerials.begin(), fFHCalSerials.end(), adc_board_serial);
  if (it != fFHCalSerials.end())
  {
    int adc_board_index = std::distance(fFHCalSerials.begin(), it);
    return adc_board_index * 64 + adc_ch;
  }

  printf("BmnFHCalRaw2Digit :: Serial 0x%08x Not found in map %s.\n", adc_board_serial, fmappingFileName.Data());
  return -1;
}

int BmnFHCalRaw2Digit::GetFlatCaloChannel(int mod_id, int sec_id)
{
  return mod_id*10 + sec_id - 1;
}

void BmnFHCalRaw2Digit::fillEvent(TClonesArray *data, TClonesArray *FHCaldigit)
{
  LOG(DEBUG) << "BmnFHCalRaw2Digit::fillEvent" << endl;

  for (int i = 0; i < data->GetEntriesFast(); i++)
  {
    BmnADCDigit *digit = (BmnADCDigit *)data->At(i);
    // check if serial is from FHCal
    // cout<<digit->GetSerial() << " " << digit->GetChannel() << endl;
    if (std::find(fFHCalSerials.begin(), fFHCalSerials.end(), digit->GetSerial()) == fFHCalSerials.end()) {
      LOG(DEBUG) << "BmnFHCalRaw2Digit::fillEvent" << std::hex << digit->GetSerial() << " Not found in ";
      for (auto it : fFHCalSerials)
        LOG(DEBUG) << "BmnFHCalRaw2Digit::fFHCalSerials " << std::hex << it << endl;
      continue;
    }
    
    std::vector<float> wfm(digit->GetUShortValue(), digit->GetUShortValue() + digit->GetNSamples());
    BmnFHCalDigi ThisDigi;
    ThisDigi.reset();
    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(digit->GetSerial(), digit->GetChannel());
    assert(flat_channel < fChannelVect.size());
    ThisDigi.fuAddress = fChannelVect.at(flat_channel);
    if (ThisDigi.fuAddress == 0)
      continue; // not connected lines
    ProcessWfm(wfm, &ThisDigi);
    if (abs(ThisDigi.fSignal) < fdigiPars.threshold)
      continue;

    TClonesArray &ar_FHCal = *FHCaldigit;
    new (ar_FHCal[FHCaldigit->GetEntriesFast()]) BmnFHCalDigi(ThisDigi);
  }
  
}

void BmnFHCalRaw2Digit::ProcessWfm(std::vector<float> wfm, BmnFHCalDigi *digi)
{
  assert(fdigiPars.gateBegin > 0 && fdigiPars.gateBegin < wfm.size());
  assert(fdigiPars.gateEnd > 0 && fdigiPars.gateEnd < wfm.size());

  // Invert
  if (fdigiPars.doInvert)
  {
    LOG(DEBUG) << "BmnFHCalRaw2Digit::ProcessWfm Inverting" << endl;
    float myconstant{-1.0};
    std::transform(wfm.begin(), wfm.end(), wfm.begin(),
                   std::bind1st(std::multiplies<float>(), myconstant));
  }

  //Zero level calculation
  LOG(DEBUG) << "BmnFHCalRaw2Digit::ProcessWfm ZL calc" << endl;
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
  LOG(DEBUG) << "BmnFHCalRaw2Digit::ProcessWfm  MAX & INT search" << endl;
  digi->fIntegral = (int) std::accumulate(wfm.begin() + fdigiPars.gateBegin, wfm.begin() + fdigiPars.gateEnd + 1,
                                    -digi->fZL * (fdigiPars.gateEnd - fdigiPars.gateBegin + 1));
  auto const max_iter = std::max_element(wfm.begin() + fdigiPars.gateBegin, wfm.begin() + fdigiPars.gateEnd + 1);
  digi->fAmpl = (int) *max_iter - digi->fZL;
  digi->fTimeMax = (int) std::distance(wfm.begin(), max_iter);

  //Apply calibration
  LOG(DEBUG) << "BmnFHCalRaw2Digit::ProcessWfm  Calibration" << endl;
  int mod_id = digi->GetModuleId();
  int sec_id = digi->GetSectionId();
  int flat_calo_ch = GetFlatCaloChannel(mod_id, sec_id);
  assert(flat_calo_ch < fCalibVect.size());
  if (fdigiPars.signalType == 0)
    digi->fSignal = (float) digi->fAmpl * fCalibVect.at(flat_calo_ch).first;
  if (fdigiPars.signalType == 1)
    digi->fSignal = (float) digi->fIntegral * fCalibVect.at(flat_calo_ch).first;

  //Prony fitting procedure
  PsdSignalFitting::PronyFitter Pfitter;
  if (fdigiPars.isfit)
  {
    LOG(DEBUG) << "BmnFHCalRaw2Digit::ProcessWfm  Fitting" << endl;
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

void BmnFHCalRaw2Digit::MeanRMScalc(std::vector<float> wfm, float *Mean, float *RMS, int begin, int end, int step)
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

BmnFHCalRaw2Digit::~BmnFHCalRaw2Digit()
{
}

ClassImp(BmnFHCalRaw2Digit)
