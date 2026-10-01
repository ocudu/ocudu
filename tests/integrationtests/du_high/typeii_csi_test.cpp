// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Tests that check the Type II CSI reporting of the DU-high class.

#include "tests/integrationtests/du_high/test_utils/du_high_env_simulator.h"
#include "tests/integrationtests/du_high/test_utils/typeii_csi_report_packer.h"
#include "tests/integrationtests/du_high/test_utils/typeii_ue_capabilities.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/ran/antenna_topology.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

/// Number of CSI-RS ports of the cell. The Type II codebook needs four ports or more.
static constexpr unsigned nof_csi_rs_ports = 8;

/// \brief Size of the CSI Part 1 payload of a Type II report, indexed by the number of beams \f$L\f$.
///
/// The report carries no CRI, as the cell configures a single CSI-RS resource, a one-bit rank indicator, a four-bit
/// wideband CQI and one \f$\lceil\log_2(2L)\rceil\f$-bit indicator of the number of non-zero amplitudes per layer,
/// as per TS38.212 Table 6.3.2.1.2-3. The sizes are stated here so that the test does not depend on the helpers that
/// the packer and the DU share.
static unsigned get_expected_csi_part1_size(unsigned nof_beams)
{
  switch (nof_beams) {
    case 2:
      return 9;
    case 4:
      return 11;
    default:
      report_fatal_error("Unsupported number of Type II beams {}.", nof_beams);
  }
}

/// Builds a DU-high configuration whose only cell offers the Type II codebook with the given number of beams.
static du_high_configuration make_typeii_du_high_config(unsigned nof_beams)
{
  du_high_env_sim_params params;

  // The Type II codebook needs a single panel of four rows and one column, with two polarizations.
  cell_config_builder_params& builder_params = params.builder_params.emplace();
  builder_params.dl_carrier.nof_ant          = nof_csi_rs_ports;
  builder_params.dl_carrier.topology         = antenna_topology::single_panel_four_one;
  builder_params.max_rank                    = max_nof_typeII_layers;

  du_high_configuration cfg = create_du_high_configuration(params);

  // The Type II PMI is carried in CSI Part 2, which only the aperiodic report on PUSCH provides.
  du_csi_params& csi_params          = cfg.ran.cells[0].ran.init_bwp.csi.value();
  csi_params.enable_aperiodic_report = true;
  csi_params.csi_report_slot_offset.reset();

  du_type2_codebook_params& type2 = csi_params.type2_codebook.emplace();
  type2.nof_beams                 = nof_beams;
  type2.phase_alphabet_size       = pmi_codebook_typeII_phase_size::qpsk;

  return cfg;
}

namespace {

/// Test of a cell that offers the Type II codebook. The codebook that each UE gets depends on its capabilities.
class du_high_typeii_csi_tester : public du_high_env_simulator, public testing::TestWithParam<unsigned>
{
protected:
  du_high_typeii_csi_tester() : du_high_env_simulator(make_typeii_du_high_config(GetParam()))
  {
    OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MIMO-16-2");
  }

  /// Number of beams \f$L\f$ that the cell combines.
  unsigned nof_typeii_beams() const { return GetParam(); }

  /// Number of combining coefficients \f$2L\f$ of each layer.
  unsigned nof_typeii_coefficients() const { return 2 * GetParam(); }

  /// \brief Brings a UE up to the point where the scheduler requests aperiodic CSI reports from it.
  ///
  /// The CU-CP runs the UE capability enquiry after the UE Context Setup, so the UE starts without any reported
  /// capability, exactly as it does on an initial attach.
  void setup_ue(rnti_t rnti)
  {
    ASSERT_TRUE(add_ue(rnti));
    ASSERT_TRUE(run_rrc_setup(rnti));
    ASSERT_TRUE(run_ue_context_setup(rnti));
    ASSERT_NO_FATAL_FAILURE(await_csi_report_config(rnti));
  }

  /// Reports a large uplink buffer, which makes the scheduler allocate PUSCH grants for the UE.
  void report_ul_buffer(rnti_t rnti)
  {
    // Short BSR reporting the largest buffer size of LCG 0, as per TS38.321 Section 6.1.3.1.
    mac_rx_data_indication rx_ind{.sl_rx = next_slot.without_hyper_sfn(), .cell_index = to_du_cell_index(0)};
    rx_ind.pdus.push_back(
        mac_rx_pdu{.rnti = rnti, .harq_id = to_harq_id(0), .pdu = byte_buffer::create({0x3d, 0x1f}).value()});
    du_hi->get_pdu_handler().handle_rx_data_indication(rx_ind);
  }

  /// Reports that the UE supports the Type II codebook, as the CU-CP does once it has run the capability enquiry.
  void report_typeii_capability(rnti_t rnti)
  {
    ASSERT_TRUE(run_ue_context_modification(
        rnti,
        test_helpers::create_typeii_ue_capability_container(
            du_high_cfg.ran.cells[0].ran.dl_carrier.band, nof_typeii_beams(), nof_csi_rs_ports)));
    ASSERT_NO_FATAL_FAILURE(await_csi_report_config(rnti));
  }

  /// \brief Awaits the next aperiodic CSI report that the scheduler requests, and stores its configuration.
  ///
  /// A UE reconfiguration drops the uplink buffer state of the UE, so the buffer is reported again.
  void await_csi_report_config(rnti_t rnti)
  {
    report_ul_buffer(rnti);
    last_csi_rep_cfg.reset();
    tracked_rnti = rnti;
    ASSERT_TRUE(run_until([this]() { return last_csi_rep_cfg.has_value(); }))
        << "The scheduler did not request an aperiodic CSI report on PUSCH.";
  }

  /// Reports the Type II CSI that \ref reported_values holds, instead of the all-ones payload of the base simulator.
  std::optional<mac_uci_indication_message> create_pusch_uci_indication(slot_point                sl_rx,
                                                                        span<const ul_sched_info> puschs) override
  {
    // The base simulator reports the payload of any grant whose report configuration is not a Type II one.
    std::optional<mac_uci_indication_message> uci_ind =
        du_high_env_simulator::create_pusch_uci_indication(sl_rx, puschs);

    for (const ul_sched_info& pusch : puschs) {
      if (not pusch.uci.has_value() or not pusch.uci->csi.has_value()) {
        continue;
      }
      const csi_report_configuration& csi_rep_cfg = pusch.uci->csi->csi_rep_cfg;

      if (pusch.pusch_cfg.rnti == tracked_rnti) {
        last_csi_rep_cfg = csi_rep_cfg;
      }
      if (not std::holds_alternative<pmi_codebook_typeII>(csi_rep_cfg.pmi_codebook)) {
        continue;
      }

      // Replace the payload of this PUSCH alone, so that the UEs that are still on the Type I codebook keep the one
      // that the base simulator reports.
      mac_uci_pdu* pdu = nullptr;
      if (uci_ind.has_value()) {
        for (mac_uci_pdu& candidate : uci_ind->ucis) {
          if (candidate.rnti == pusch.pusch_cfg.rnti) {
            pdu = &candidate;
            break;
          }
        }
      }
      if (pdu == nullptr) {
        continue;
      }

      auto* pusch_ind = std::get_if<mac_uci_pdu::pusch_type>(&pdu->pdu);
      if (pusch_ind == nullptr) {
        continue;
      }

      auto [part1, part2] = test_helpers::pack_typeii_csi_report_pusch(csi_rep_cfg, reported_values);
      EXPECT_EQ(part1.size(), get_expected_csi_part1_size(nof_typeii_beams()))
          << "The packed CSI Part 1 does not have the expected size.";
      EXPECT_EQ(part1.size(), pusch.uci->csi->csi_part1_nof_bits)
          << "The packed CSI Part 1 does not match the size that the scheduler expects.";

      pusch_ind->csi_part1_info.emplace();
      pusch_ind->csi_part1_info->is_valid = true;
      pusch_ind->csi_part1_info->payload  = part1;

      pusch_ind->csi_part2_info.emplace();
      pusch_ind->csi_part2_info->is_valid = true;
      pusch_ind->csi_part2_info->payload  = part2;

      ++nof_typeii_reports;
    }

    return uci_ind;
  }

  /// Sends downlink data, so that the scheduler allocates a PDSCH that carries the reported precoding.
  void send_dl_data(rnti_t rnti)
  {
    const ue_sim_context& u = ues.at(rnti);
    du_hi->get_f1ap_pdu_handler().handle_message(test_helpers::generate_dl_rrc_message_transfer(
        *u.du_ue_id, *u.cu_ue_id, srb_id_t::srb1, byte_buffer::create(std::vector<uint8_t>(128, 0x5a)).value()));
  }

  /// Sends downlink data and returns the Type II PMI that precodes the next PDSCH of the UE, if any.
  std::optional<pmi_typeII> await_pdsch_typeii_pmi(rnti_t rnti)
  {
    send_dl_data(rnti);

    const dl_msg_alloc* pdsch = nullptr;
    if (not run_until([&]() { return (pdsch = find_ue_pdsch(rnti)) != nullptr; })) {
      return std::nullopt;
    }

    const auto* pmi = std::get_if<precoding_matrix_indicator>(&pdsch->pdsch_cfg.precoding_and_beamforming);
    if (pmi == nullptr) {
      return std::nullopt;
    }

    const auto* typeii_pmi = std::get_if<pmi_typeII>(pmi);
    return (typeii_pmi != nullptr) ? std::make_optional(*typeii_pmi) : std::nullopt;
  }

  /// Asserts that a Type II PMI carries the values that the UE reported.
  void assert_pmi_matches(const pmi_typeII& pmi, const test_helpers::typeii_csi_report_values& values) const
  {
    // Type II wideband amplitude index that corresponds to a unit amplitude.
    static constexpr uint8_t max_wideband_amplitude_index = 7;

    const unsigned nof_coefficients = nof_typeii_coefficients();

    ASSERT_EQ(pmi.i_1_1, values.i_1_1);
    ASSERT_EQ(pmi.i_1_2, values.i_1_2);
    ASSERT_EQ(pmi.layers.size(), values.ri);

    for (unsigned i_layer = 0; i_layer != values.ri; ++i_layer) {
      const pmi_typeII::layer_coefficients& layer = pmi.layers[i_layer];
      ASSERT_EQ(layer.i_1_3, values.i_1_3[i_layer]) << "Layer " << i_layer;
      ASSERT_EQ(layer.i_1_4.size(), nof_coefficients) << "Layer " << i_layer;
      ASSERT_EQ(layer.i_2_1.size(), nof_coefficients) << "Layer " << i_layer;

      for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
        // Every coefficient is reported with the maximum wideband amplitude, and the one of the strongest coefficient
        // is not reported at all.
        ASSERT_EQ(layer.i_1_4[i_coefficient], max_wideband_amplitude_index)
            << "Layer " << i_layer << ", coefficient " << i_coefficient;

        // The phase of the strongest coefficient is fixed to zero, as per TS38.214 Section 5.2.2.2.3.
        const unsigned expected_phase = (i_coefficient == values.i_1_3[i_layer]) ? 0 : values.phase[i_layer];
        ASSERT_EQ(layer.i_2_1[i_coefficient], expected_phase)
            << "Layer " << i_layer << ", coefficient " << i_coefficient;
      }
    }
  }

  /// Returns the PDSCH grant of the given UE in the last downlink result, or nullptr if it was not scheduled.
  const dl_msg_alloc* find_ue_pdsch(rnti_t rnti) const
  {
    const auto& last_dl_res = phy.cells[0].last_dl_res;
    if (not last_dl_res.has_value() or (last_dl_res->dl_res == nullptr)) {
      return nullptr;
    }

    for (const dl_msg_alloc& grant : last_dl_res->dl_res->ue_grants) {
      if (grant.pdsch_cfg.rnti == rnti) {
        return &grant;
      }
    }

    return nullptr;
  }

  /// \brief Fills the values of the first report with a rank two PMI that is not the default one.
  ///
  /// Each layer reports a different strongest coefficient and a different phase, so that the test catches a decode
  /// that mixes the layers up.
  void set_first_reported_values()
  {
    // The beam group index only has a range when fewer beams than panel ports are selected.
    const unsigned nof_beam_groups = (nof_typeii_beams() == 2) ? 6 : 1;

    reported_values.ri    = 2;
    reported_values.i_1_1 = 3;
    reported_values.i_1_2 = nof_beam_groups - 1;
    reported_values.i_1_3 = {1, nof_typeii_coefficients() - 2};
    reported_values.phase = {2, 1};
  }

  /// Values that the UE reports in its Type II CSI report.
  test_helpers::typeii_csi_report_values reported_values;
  /// Configuration of the last CSI report that the scheduler requested on PUSCH.
  std::optional<csi_report_configuration> last_csi_rep_cfg;
  /// UE whose CSI report configuration \ref last_csi_rep_cfg tracks.
  rnti_t tracked_rnti = rnti_t::INVALID_RNTI;
  /// Number of Type II CSI reports that the UEs have reported.
  unsigned nof_typeii_reports = 0;
};

} // namespace

TEST_P(du_high_typeii_csi_tester, when_ue_reports_typeii_capability_then_the_codebook_is_updated)
{
  const rnti_t rnti = to_rnti(0x4601);

  ASSERT_NO_FATAL_FAILURE(set_first_reported_values());

  ASSERT_NO_FATAL_FAILURE(setup_ue(rnti));
  ASSERT_TRUE(std::holds_alternative<pmi_codebook_typeI_single_panel>(last_csi_rep_cfg->pmi_codebook))
      << "The DU configured the Type II codebook before the UE capabilities were known.";

  ASSERT_NO_FATAL_FAILURE(report_typeii_capability(rnti));
  ASSERT_TRUE(std::holds_alternative<pmi_codebook_typeII>(last_csi_rep_cfg->pmi_codebook))
      << "The DU did not configure the Type II codebook for a UE that supports it.";

  const std::optional<pmi_typeII> pmi = await_pdsch_typeii_pmi(rnti);
  ASSERT_TRUE(pmi.has_value()) << "The PDSCH of the UE is not precoded with a Type II PMI.";
  ASSERT_NO_FATAL_FAILURE(assert_pmi_matches(*pmi, reported_values));
}

TEST_P(du_high_typeii_csi_tester, when_ue_does_not_report_typeii_capability_then_it_is_kept_on_the_typei_codebook)
{
  const rnti_t rnti = to_rnti(0x4601);

  // The UE never reports its capabilities, so the DU cannot know that it supports the Type II codebook.
  ASSERT_NO_FATAL_FAILURE(setup_ue(rnti));

  // The cell offers the Type II codebook, but the UE did not report support for it.
  ASSERT_TRUE(std::holds_alternative<pmi_codebook_typeI_single_panel>(last_csi_rep_cfg->pmi_codebook))
      << "The DU did not fall back to the Type I codebook.";

  send_dl_data(rnti);

  // No downlink transmission of the UE can be precoded with a codebook that the UE was not configured with.
  static constexpr unsigned nof_slots_to_run = 200;
  unsigned                  nof_pdsch        = 0;
  for (unsigned i_slot = 0; i_slot != nof_slots_to_run; ++i_slot) {
    run_slot();

    const dl_msg_alloc* pdsch = find_ue_pdsch(rnti);
    if (pdsch == nullptr) {
      continue;
    }
    ++nof_pdsch;

    const auto* pmi = std::get_if<precoding_matrix_indicator>(&pdsch->pdsch_cfg.precoding_and_beamforming);
    ASSERT_TRUE((pmi == nullptr) or not std::holds_alternative<pmi_typeII>(*pmi)) << "The PDSCH carries a Type II PMI.";
  }

  ASSERT_GT(nof_pdsch, 0) << "The scheduler did not allocate a PDSCH for the UE.";
}

TEST_P(du_high_typeii_csi_tester, when_ue_reports_a_new_pmi_then_the_pdsch_precoding_is_updated)
{
  const rnti_t rnti = to_rnti(0x4601);

  ASSERT_NO_FATAL_FAILURE(set_first_reported_values());

  ASSERT_NO_FATAL_FAILURE(setup_ue(rnti));
  ASSERT_NO_FATAL_FAILURE(report_typeii_capability(rnti));

  const std::optional<pmi_typeII> first_pmi = await_pdsch_typeii_pmi(rnti);
  ASSERT_TRUE(first_pmi.has_value()) << "The PDSCH of the UE is not precoded with a Type II PMI.";
  ASSERT_NO_FATAL_FAILURE(assert_pmi_matches(*first_pmi, reported_values));

  // The UE now reports a different beam selection and a single layer.
  const unsigned nof_first_reports = nof_typeii_reports;
  report_ul_buffer(rnti);
  reported_values.ri    = 1;
  reported_values.i_1_1 = 1;
  reported_values.i_1_2 = 0;
  reported_values.i_1_3 = {nof_typeii_coefficients() - 1, 0};
  reported_values.phase = {1, 0};

  ASSERT_TRUE(run_until([this, nof_first_reports]() { return nof_typeii_reports > nof_first_reports; }))
      << "The scheduler did not request a second aperiodic CSI report.";

  const std::optional<pmi_typeII> second_pmi = await_pdsch_typeii_pmi(rnti);
  ASSERT_TRUE(second_pmi.has_value()) << "The PDSCH of the UE is not precoded with a Type II PMI.";
  ASSERT_NO_FATAL_FAILURE(assert_pmi_matches(*second_pmi, reported_values));
}

INSTANTIATE_TEST_SUITE_P(du_high_typeii_csi_test_suite,
                         du_high_typeii_csi_tester,
                         ::testing::Values(2U, 4U),
                         [](const ::testing::TestParamInfo<unsigned>& params_item) {
                           return fmt::format("L{}", params_item.param);
                         });
