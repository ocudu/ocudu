// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Tests that check the number of PDSCH layers of a UE in the DU-high class.

#include "tests/integrationtests/du_high/test_utils/du_high_env_simulator.h"
#include "tests/integrationtests/du_high/test_utils/typeii_ue_capabilities.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

/// Number of DL antenna ports, and therefore of PDSCH layers, of the cell.
static constexpr unsigned cell_nof_layers = 4;

/// Builds a DU-high configuration with a cell of \ref cell_nof_layers DL antenna ports.
static du_high_configuration make_mimo_du_high_config()
{
  du_high_env_sim_params params;
  params.builder_params.emplace().dl_carrier.nof_ant = cell_nof_layers;

  return create_du_high_configuration(params);
}

namespace {

/// \brief Test of a UE that always reports the largest rank of the cell.
///
/// The test parameter is the number of PDSCH layers of the UE capabilities.
class du_high_pdsch_mimo_layers_tester : public du_high_env_simulator, public testing::TestWithParam<unsigned>
{
protected:
  du_high_pdsch_mimo_layers_tester() : du_high_env_simulator(make_mimo_du_high_config()) {}

  /// Awaits the next periodic CSI report of the UE and stores its size in \c csi_part1_size.
  void await_csi_report(rnti_t rnti, unsigned& csi_part1_size)
  {
    last_csi_part1_size.reset();
    tracked_rnti = rnti;
    ASSERT_TRUE(run_until([this]() { return last_csi_part1_size.has_value(); }))
        << "The scheduler did not allocate a CSI report on PUCCH.";
    csi_part1_size = *last_csi_part1_size;
  }

  void handle_slot_results(du_cell_index_t cell_index) override
  {
    const auto& last_ul_res = phy.cells[cell_index].last_ul_res;
    if (last_ul_res.has_value()) {
      for (const pucch_info& pucch : last_ul_res->ul_res.pucchs().unsorted()) {
        if (pucch.crnti == tracked_rnti and pucch.uci_bits.csi_part1_nof_bits > 0) {
          last_csi_part1_size = pucch.uci_bits.csi_part1_nof_bits;
        }
      }
    }
    // The all-ones payload of the base simulator indicates the largest rank allowed by the RI restriction.
    du_high_env_simulator::handle_slot_results(cell_index);
  }

  /// Sends downlink data and stores the number of layers of the next PDSCH of the UE in \c nof_layers.
  void await_pdsch_nof_layers(rnti_t rnti, unsigned& nof_layers)
  {
    const ue_sim_context& u = ues.at(rnti);
    du_hi->get_f1ap_pdu_handler().handle_message(test_helpers::generate_dl_rrc_message_transfer(
        *u.du_ue_id, *u.cu_ue_id, srb_id_t::srb1, byte_buffer::create(std::vector<uint8_t>(128, 0x5a)).value()));

    const dl_msg_alloc* pdsch = nullptr;
    ASSERT_TRUE(run_until([&]() { return (pdsch = find_ue_pdsch(rnti)) != nullptr; }))
        << "The scheduler did not allocate a PDSCH for the UE.";
    nof_layers = pdsch->pdsch_cfg.nof_layers;
  }

  /// Returns the PDSCH grant of the given UE in the last downlink result, or nullptr if it was not scheduled.
  const dl_msg_alloc* find_ue_pdsch(rnti_t rnti) const
  {
    const auto& last_dl_res = phy.cells[0].last_dl_res;
    if (not last_dl_res.has_value()) {
      return nullptr;
    }

    for (const dl_msg_alloc& grant : last_dl_res->dl_res.ue_grants()) {
      if (grant.pdsch_cfg.rnti == rnti) {
        return &grant;
      }
    }

    return nullptr;
  }

  /// Size of the last CSI report allocated on PUCCH.
  std::optional<unsigned> last_csi_part1_size;
  /// UE whose CSI reports are tracked.
  rnti_t tracked_rnti = rnti_t::INVALID_RNTI;
};

} // namespace

TEST_P(du_high_pdsch_mimo_layers_tester, when_ue_reports_its_capabilities_then_pdsch_layers_follow_them)
{
  const rnti_t rnti = to_rnti(0x4601);

  // The CU-CP runs the UE capability enquiry after the UE Context Setup, so the UE starts without capabilities.
  ASSERT_TRUE(add_ue(rnti));
  ASSERT_TRUE(run_rrc_setup(rnti));
  ASSERT_TRUE(run_ue_context_setup(rnti));

  unsigned csi_size_before_caps = 0;
  ASSERT_NO_FATAL_FAILURE(await_csi_report(rnti, csi_size_before_caps));

  unsigned nof_layers = 0;
  ASSERT_NO_FATAL_FAILURE(await_pdsch_nof_layers(rnti, nof_layers));
  ASSERT_EQ(nof_layers, 1) << "A single layer is the only one that every UE supports.";

  ASSERT_TRUE(run_ue_context_modification(
      rnti,
      test_helpers::create_typeii_ue_capability_container(du_high_cfg.ran.cells[0].ran.dl_carrier.band,
                                                          /* nof_beams */ 4,
                                                          /* nof_tx_ports */ 8,
                                                          /* nof_pdsch_layers */ GetParam())));

  unsigned csi_size_after_caps = 0;
  ASSERT_NO_FATAL_FAILURE(await_csi_report(rnti, csi_size_after_caps));
  ASSERT_EQ(csi_size_after_caps, csi_size_before_caps) << "The CSI report size depends on the UE capabilities.";

  ASSERT_NO_FATAL_FAILURE(await_pdsch_nof_layers(rnti, nof_layers));
  ASSERT_EQ(nof_layers, GetParam()) << "The PDSCH does not use the layers that the UE supports.";
}

INSTANTIATE_TEST_SUITE_P(du_high_pdsch_mimo_layers_test_suite,
                         du_high_pdsch_mimo_layers_tester,
                         ::testing::Values(2U, 4U),
                         [](const ::testing::TestParamInfo<unsigned>& params_item) {
                           return fmt::format("{}_layers", params_item.param);
                         });
