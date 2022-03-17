/* Copyright (C) 2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer], Viktor Klochkov [committer] */

/** @brief run_analysistree_qa
 ** @param filelist    Filefist (text file) of input AnalysisTree files
 ** @param is_single_file  if true, instead of filelist a single ROOT file will be used as input
 **
 ** Macro to run AnalysisTreeQA package (https://github.com/HeavyIonAnalysis/AnalysisTreeQA)
 ** Produces an output ROOT file with specified histograms / TProfiles.
 ** Examples how to add plots could be found here:
 ** https://github.com/HeavyIonAnalysis/AnalysisTreeQA/blob/master/examples/example.cpp
 ** To add event cuts:
 ** task->SetEventCuts(EventCuts);
 ** where EventCuts is AnalysisTree::Cuts* object, for example
 ** GetCbmEventCuts("RecEventHeader") from macro/analysis/common/cuts/cbm_cuts.h
 ** To apply a cut on some branch, for example select only primiry MC-tracks:
 ** task->AddBranchCut(GetCbmMcTracksCuts("SimParticles"));
 ** or apply quality cuts on STS tracks:
 ** task->AddBranchCut(GetCbmTrackCuts("RecTracks"));
 **/

using namespace AnalysisTree;

void KFPFTracksQA(QA::Task& task);
void VertexTracksQA(QA::Task& task);
void TofHitsQA(QA::Task& task);
void SimParticlesQA(QA::Task& task);
void SimEventHeaderQA(QA::Task& task);
void RecEventHeaderQA(QA::Task& task);
void EfficiencyMaps(QA::Task& task);
void TrdTracksQA(QA::Task& task);
void RichRingsQA(QA::Task& task);

void run_analysistree_qa(std::string filelist, bool is_single_file = false);

const std::string sim_event_header = "SimEventHeader";
const std::string rec_event_header = "RecEventHeader";
const std::string sim_particles    = "SimParticles";
const std::string rec_tracks       = "GlobalTracks";
const std::string tof400_hits         = "Tof400Hits";
const std::string tof700_hits         = "Tof400Hits";
const std::string trd_tracks       = "TrdTracks";
const std::string rich_rings       = "RichRings";

void run_analysistree_qa(std::string filelist, bool is_single_file)
{
  if (is_single_file) {
    std::ofstream fl("fl_temp.txt");
    fl << filelist << "\n";
    fl.close();
    filelist = "fl_temp.txt";
  }

  TaskManager* man = TaskManager::GetInstance();

  auto* task = new QA::Task;
  task->SetOutputFileName("cbm_qa.root");

  VertexTracksQA(*task);
  SimParticlesQA(*task);
  SimEventHeaderQA(*task);
  RecEventHeaderQA(*task);
  TofHitsQA(*task);

  man->AddTask(task);

  man->Init({filelist}, {"rTree"});
  man->Run(-1);
  man->Finish();

  if (is_single_file) {
    // -----   Finish   -------------------------------------------------------
    std::cout << " Test passed" << std::endl;
    std::cout << " All ok " << std::endl;
  }
}

void TrdTracksQA(QA::Task& task)
{
  AddTrackQA(&task, trd_tracks);
  AddTracksMatchQA(&task, trd_tracks, rec_tracks);

  task.AddH1({"TRD energy loss in 1st station, keV", {trd_tracks, "energy_loss_0"}, {QA::gNbins, 0, 50}});
  task.AddH1({"TRD energy loss in 2nd station", {trd_tracks, "energy_loss_1"}, {QA::gNbins, 0, 50}});
  task.AddH1({"TRD energy loss in 3rd station", {trd_tracks, "energy_loss_2"}, {QA::gNbins, 0, 50}});
  task.AddH1({"TRD energy loss in 4th station", {trd_tracks, "energy_loss_3"}, {QA::gNbins, 0, 50}});

  task.AddH1({"Number of hits in TRD", {trd_tracks, "n_hits"}, {6, 0, 6}});

  task.AddH1({"PID Likelihood, e^{-}", {trd_tracks, "pid_like_e"}, {QA::gNbins, -1, 1}});
  task.AddH1({"PID Likelihood, #pi", {trd_tracks, "pid_like_pi"}, {QA::gNbins, -1, 1}});
  task.AddH1({"PID Likelihood, K", {trd_tracks, "pid_like_k"}, {QA::gNbins, -1, 1}});
  task.AddH1({"PID Likelihood, p", {trd_tracks, "pid_like_p"}, {QA::gNbins, -1, 1}});

  task.AddH1({"#chi^{2}/NDF", {trd_tracks, "chi2_ov_ndf"}, {QA::gNbins, 0, 30}});
  task.AddH1({"p_{T}^{last} (GeV/c)", {trd_tracks, "pT_out"}, {QA::gNbins, 0, 10}});
  task.AddH1({"p^{last} (GeV/c)", {trd_tracks, "p_out"}, {QA::gNbins, 0, 10}});
}

void RichRingsQA(QA::Task& task)
{
  task.AddH1({"RICh ring x-position (cm)", {rich_rings, "x"}, {QA::gNbins, -100, 100}});
  task.AddH1({"RICh ring y-position (cm)", {rich_rings, "y"}, {QA::gNbins, -250, 250}});
  task.AddH2({"RICh ring x-position (cm)", {rich_rings, "x"}, {QA::gNbins, -100, 100}},
             {"RICh ring y-position (cm)", {rich_rings, "y"}, {QA::gNbins, -250, 250}});

  task.AddH1({"Ring radius", {rich_rings, "radius"}, {QA::gNbins, 0, 10}});

  task.AddH1({"n_hits", {rich_rings, "n_hits"}, {QA::gNbins, 0, 60}});
  task.AddH1({"n_hits_on_ring", {rich_rings, "n_hits_on_ring"}, {QA::gNbins, 0, 100}});
  task.AddH1({"axis_a", {rich_rings, "axis_a"}, {QA::gNbins, 0, 10}});
  task.AddH1({"axis_b", {rich_rings, "axis_b"}, {QA::gNbins, 0, 10}});

  task.AddH1({"chi2_ov_ndf", {rich_rings, "chi2_ov_ndf"}, {QA::gNbins, 0, 1}});
  task.AddH1({"phi_ellipse", {rich_rings, "phi_ellipse"}, {QA::gNbins, -3.2, 3.2}});
}

void VertexTracksQA(QA::Task& task)
{
  AddTrackQA(&task, rec_tracks);
  if (!sim_particles.empty()) { AddTracksMatchQA(&task, rec_tracks, sim_particles); }

  Variable chi2_over_ndf("chi2_ndf", {{rec_tracks, "chi2"}, {rec_tracks, "ndf"}},
                         [](std::vector<double>& var) { return var.at(0) / var.at(1); });

  task.AddH1({"DCA_{x}, cm", {rec_tracks, "dcax"}, {QA::gNbins, -1, 1}});
  task.AddH1({"DCA_{y}, cm", {rec_tracks, "dcay"}, {QA::gNbins, -1, 1}});
  task.AddH1({"DCA_{z}, cm", {rec_tracks, "dcaz"}, {QA::gNbins, -1, 1}});
  task.AddH1({"NDF", {rec_tracks, "ndf"}, {30, 0, 30}});
//  task.AddH1({"Number of hits (STS+MVD)", {rec_tracks, "nhits"}, {15, 0, 15}});
//  task.AddH1({"Number of hits (MVD)", {rec_tracks, "nhits_mvd"}, {5, 0, 5}});
  task.AddH1({"#chi^{2}_{vertex}", {rec_tracks, "vtx_chi2"}, {500, 0, 100}});
  task.AddH1({"#chi^{2}/NDF", chi2_over_ndf, {QA::gNbins, 0, 10}});
  task.AddH2({"DCA_{x}, cm", {rec_tracks, "dcax"}, {QA::gNbins, -1, 1}},
             {"DCA_{y}, cm", {rec_tracks, "dcay"}, {QA::gNbins, -1, 1}});
}

void TofHitsQA(QA::Task& task)
{
  task.AddH1({"TOF hit x-position (cm)", {tof400_hits, "x"}, {QA::gNbins, -600, 600}});
  task.AddH1({"TOF hit y-position (cm)", {tof400_hits, "y"}, {QA::gNbins, -400, 400}});
  task.AddH1({"TOF hit z-position (cm)", {tof400_hits, "z"}, {QA::gNbins, 600, 800}});

  task.AddH2({"TOF hit x-position (cm)", {tof400_hits, "x"}, {QA::gNbins, -600, 600}},
             {"TOF hit y-position (cm)", {tof400_hits, "y"}, {QA::gNbins, -600, 600}});

  task.AddH1({"TOF hit x-position (cm)", {tof700_hits, "x"}, {QA::gNbins, -600, 600}});
  task.AddH1({"TOF hit y-position (cm)", {tof700_hits, "y"}, {QA::gNbins, -400, 400}});
  task.AddH1({"TOF hit z-position (cm)", {tof700_hits, "z"}, {QA::gNbins, 600, 800}});

  task.AddH2({"TOF hit x-position (cm)", {tof700_hits, "x"}, {QA::gNbins, -600, 600}},
             {"TOF hit y-position (cm)", {tof700_hits, "y"}, {QA::gNbins, -600, 600}});

  Variable qp_sts("qp_reco", {{rec_tracks, "q"}, {rec_tracks, "p"}},
                  [](std::vector<double>& qp) { return qp.at(0) * qp.at(1); });

  task.AddH2({"sign(q)*p, GeV/c", qp_sts, {QA::gNbins, -10, 10}},
             {"m^{2}, GeV^{2}/c^{2}", {tof400_hits, "mass2"}, {QA::gNbins, -1, 2}});
  task.AddH2({"sign(q)*p, GeV/c", qp_sts, {QA::gNbins, -10, 10}},
             {"m^{2}, GeV^{2}/c^{2}", {tof700_hits, "mass2"}, {QA::gNbins, -1, 2}});

  task.AddH2({"sign(q)*p, GeV/c", qp_sts, {QA::gNbins, -10, 10}},
             {"m^{2}, GeV^{2}/c^{2}", {tof400_hits, "beta"}, {QA::gNbins, -0.2, 1.2}});
  task.AddH2({"sign(q)*p, GeV/c", qp_sts, {QA::gNbins, -10, 10}},
             {"m^{2}, GeV^{2}/c^{2}", {tof700_hits, "beta"}, {QA::gNbins, -0.2, 1.2}});

}

void SimParticlesQA(QA::Task& task) { AddParticleQA(&task, sim_particles); }

void SimEventHeaderQA(QA::Task& task)
{
  task.AddH1({"x_{vertex}^{MC} (cm)", {sim_event_header, "vtx_x"}, {QA::gNbins, -1, 1}});
  task.AddH1({"y_{vertex}^{MC} (cm)", {sim_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});
  task.AddH1({"z_{vertex}^{MC} (cm)", {sim_event_header, "vtx_z"}, {QA::gNbins, -1, 1}});
  task.AddH1({"b (fm)", {sim_event_header, "b"}, {QA::gNbins, 0, 20}});
  task.AddH1({"#Psi_{RP}", {sim_event_header, "psi_RP"}, {QA::gNbins, 0, 6.5}});

  task.AddH2({"x_{vertex}^{MC} (cm)", {sim_event_header, "vtx_x"}, {QA::gNbins, -1, 1}},
             {"y_{vertex}^{MC} (cm)", {sim_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});
}

void RecEventHeaderQA(QA::Task& task)
{
  task.AddH1({"x_{vertex} (cm)", {rec_event_header, "vtx_x"}, {QA::gNbins, -1, 1}});
  task.AddH1({"y_{vertex} (cm)", {rec_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});
  task.AddH1({"z_{vertex} (cm)", {rec_event_header, "vtx_z"}, {QA::gNbins, -1, 1}});
  task.AddH1({"#chi^{2}_{vertex fit}", {rec_event_header, "vtx_chi2"}, {QA::gNbins, 0, 5}});

  task.AddH1({"E_{PSD} (GeV)", {rec_event_header, "Epsd"}, {QA::gNbins, 0, 60}});
  task.AddH1({"M_{tracks}", {rec_event_header, "M"}, {800, 0, 800}});
  task.AddH1({"Event ID", {rec_event_header, "evt_id"}, {QA::gNbins, 0, 2000}});

  task.AddH2({"x_{vertex} (cm)", {rec_event_header, "vtx_x"}, {QA::gNbins, -1, 1}},
             {"y_{vertex} (cm)", {rec_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});

  task.AddH2({"x_{vertex} (cm)", {rec_event_header, "vtx_x"}, {QA::gNbins, -1, 1}},
             {"x_{vertex} (cm)", {sim_event_header, "vtx_x"}, {QA::gNbins, -1, 1}});
  task.AddH2({"y_{vertex} (cm)", {rec_event_header, "vtx_y"}, {QA::gNbins, -1, 1}},
             {"y_{vertex} (cm)", {sim_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});
  task.AddH2({"z_{vertex} (cm)", {rec_event_header, "vtx_z"}, {QA::gNbins, -1, 1}},
             {"z_{vertex} (cm)", {sim_event_header, "vtx_z"}, {QA::gNbins, -1, 1}});
}

void EfficiencyMaps(QA::Task& task)
{
  const float y_beam  = 1.62179f;  // TODO from DataHeader
  const float p_mass  = 0.938;
  const float pi_mass = 0.14;

  Variable proton_y("y-y_{beam}", {{rec_tracks, "p"}, {rec_tracks, "pz"}}, [y_beam, p_mass](std::vector<double>& var) {
    const float e = sqrt(p_mass * p_mass + var[0] * var[0]);
    return 0.5 * log((e + var[1]) / (e - var[1]));
  });

  Variable pion_y("y-y_{beam}", {{rec_tracks, "p"}, {rec_tracks, "pz"}}, [y_beam, pi_mass](std::vector<double>& var) {
    const float e = sqrt(pi_mass * pi_mass + var[0] * var[0]);
    return 0.5 * log((e + var[1]) / (e - var[1]));
  });

  Cuts* mc_protons   = new Cuts("McProtons", {EqualsCut({sim_particles + ".pid"}, 2212)});
  Cuts* mc_pions_neg = new Cuts("McPionsNeg", {EqualsCut({sim_particles + ".pid"}, -211)});
  Cuts* mc_pions_pos = new Cuts("McPionsPos", {EqualsCut({sim_particles + ".pid"}, -211)});

  task.AddH2({"#it{y}_{Lab}", {sim_particles, "rapidity"}, {QA::gNbins, -1, 5}},
             {"p_{T}, GeV/c", {sim_particles, "pT"}, {QA::gNbins, 0, 2}}, mc_protons);
  task.AddH2({"#it{y}_{Lab}", proton_y, {QA::gNbins, -1, 5}}, {"p_{T}, GeV/c", {rec_tracks, "pT"}, {QA::gNbins, 0, 2}},
             mc_protons);

  task.AddH2({"#it{y}_{Lab}", {sim_particles, "rapidity"}, {QA::gNbins, -1, 5}},
             {"p_{T}, GeV/c", {sim_particles, "pT"}, {QA::gNbins, 0, 2}}, mc_pions_neg);
  task.AddH2({"#it{y}_{Lab}", pion_y, {QA::gNbins, -1, 5}}, {"p_{T}, GeV/c", {rec_tracks, "pT"}, {QA::gNbins, 0, 2}},
             mc_pions_neg);

  task.AddH2({"#it{y}_{Lab}", {sim_particles, "rapidity"}, {QA::gNbins, -1, 5}},
             {"p_{T}, GeV/c", {sim_particles, "pT"}, {QA::gNbins, 0, 2}}, mc_pions_pos);
  task.AddH2({"#it{y}_{Lab}", pion_y, {QA::gNbins, -1, 5}}, {"p_{T}, GeV/c", {rec_tracks, "pT"}, {QA::gNbins, 0, 2}},
             mc_pions_pos);
}

void KFPFTracksQA(QA::Task& task)
{
  const std::vector<std::string> field_par_names_ {"cx0", "cx1", "cx2", "cy1", "cy2", "cz0", "cz1", "cz2"};
  const std::vector<std::string> cov_names_ {"cov1", "cov2",  "cov3",  "cov4",  "cov5",  "cov6",  "cov7", "cov8",
                                             "cov9", "cov10", "cov11", "cov12", "cov13", "cov14", "cov15"};

  task.AddH1({"x, cm", {rec_tracks, "x"}, {QA::gNbins, -10, 10}});
  task.AddH1({"y, cm", {rec_tracks, "y"}, {QA::gNbins, -10, 10}});
  task.AddH1({"z, cm", {rec_tracks, "z"}, {QA::gNbins, 0, 40}});
  task.AddH1({"t_{x}", {rec_tracks, "tx"}, {QA::gNbins, -2, 2}});
  task.AddH1({"t_{y}", {rec_tracks, "ty"}, {QA::gNbins, -2, 2}});
  task.AddH1({"q*p, GeV/c", {rec_tracks, "qp"}, {QA::gNbins, -10, 10}});

  for (const auto& field_par_name : field_par_names_)
    task.AddH1({field_par_name, {rec_tracks, field_par_name}, {QA::gNbins, -1, 1}});

  for (const auto& cov_name : cov_names_)
    task.AddH1({cov_name, {rec_tracks, cov_name}, {1000, -.1, .1}});

  task.AddH1({"cy0", {rec_tracks, "cy0"}, {QA::gNbins, -12, -8}});
  task.AddH1({"z0", {rec_tracks, "z0"}, {QA::gNbins, 0, 40}});
}

int main(int argc, char** argv)
{
  if (argc <= 1) {
    std::cout << "Not enough arguments! Please use:" << std::endl;
    std::cout << "   ./cbm_qa filelist" << std::endl;
    return -1;
  }

  const std::string filelist = argv[1];
  run_analysistree_qa(filelist);
  return 0;
}
