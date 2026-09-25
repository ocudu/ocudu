// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_sched/uci_cell_decoder.h"
#include "mac_test_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// \brief Tests the decoding of aperiodic CSI reports on PUSCH.
///
/// A two-port codebook with PMI reporting is used, so that a CSI Part 2 payload is always expected: 2 bits for rank 1
/// and 1 bit for rank 2, as per TS38.212 Table 6.3.2.1.2-4.
class uci_cell_decoder_aperiodic_csi_test : public ::testing::Test
{
protected:
  static constexpr rnti_t        test_rnti = to_rnti(0x4601);
  static constexpr du_ue_index_t ue_idx    = to_du_ue_index(0);
  static constexpr slot_point    sl_rx{subcarrier_spacing::kHz30, 0, 4};

  /// Builds a CSI report configuration for a two-port codebook reporting CRI, RI, PMI and CQI.
  static csi_report_configuration make_csi_rep_cfg()
  {
    return {.nof_csi_rs_resources = 1,
            .nof_reported_rs      = 1,
            .pmi_codebook         = pmi_codebook_two_port{},
            .ri_restriction       = ri_restriction_type({true, true}),
            .quantities           = csi_report_quantities::cri_ri_pmi_cqi};
  }

  uci_cell_decoder_aperiodic_csi_test() : decoder(make_cell_cfg(), rnti_table, rlf_handler)
  {
    OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MIMO-16-1", "MVP-FUNC-MIMO-16-3");

    ocudulog::init();
    rnti_table.add_ue(test_rnti, ue_idx);
    rlf_handler.add_ue(ue_idx, rlf_notif);

    // Store the expected CSI report configuration for the slot under test.
    ul_sched_info pusch{};
    pusch.pusch_cfg.rnti = test_rnti;
    pusch.uci.emplace().csi.emplace(uci_info::csi_info{.csi_rep_cfg = make_csi_rep_cfg()});
    decoder.store_uci(sl_rx, {}, {&pusch, 1});
  }
  ~uci_cell_decoder_aperiodic_csi_test() override { ocudulog::flush(); }

  /// Builds a cell configuration with aperiodic CSI reporting enabled.
  static sched_cell_configuration_request_message make_cell_cfg()
  {
    auto cfg = sched_config_helper::make_default_sched_cell_configuration_request();
    cfg.ran.init_bwp.csi.emplace().enable_aperiodic_report = true;
    return cfg;
  }

  /// Builds a UCI indication with a valid CSI Part 1 reporting rank 2 and CQI 15, and the given CSI Part 2 contents.
  static mac_uci_indication_message
  make_uci_ind(const std::optional<mac_uci_pdu::pusch_type::csi_information>& csi_part2)
  {
    mac_uci_pdu::pusch_type pusch;
    // CSI Part 1 is 5 bits: RI (1 bit, set to indicate rank 2) followed by the wideband CQI (4 bits, set to 15).
    pusch.csi_part1_info.emplace(
        mac_uci_pdu::pusch_type::csi_information{.is_valid = true, .payload = {true, true, true, true, true}});
    pusch.csi_part2_info = csi_part2;

    mac_uci_indication_message msg;
    msg.sl_rx        = sl_rx;
    mac_uci_pdu& pdu = msg.ucis.emplace_back();
    pdu.rnti         = test_rnti;
    pdu.pdu          = pusch;

    return msg;
  }

  manual_task_worker                  worker{64};
  test_helpers::dummy_ue_rlf_notifier rlf_notif;
  rlf_detector                        rlf_handler{mac_expert_config{.configs = {{100, 100, 100}}}, worker};
  du_rnti_table                       rnti_table;
  uci_cell_decoder                    decoder;
};

TEST_F(uci_cell_decoder_aperiodic_csi_test, when_csi_part2_is_valid_then_report_is_decoded)
{
  // CSI Part 2 for rank 2 with a two-port codebook is 1 bit (the PMI i2 field).
  mac_uci_pdu::pusch_type::csi_information csi2;
  csi2.is_valid = true;
  csi2.payload.resize(1);

  uci_indication ind = decoder.decode_uci(make_uci_ind(csi2));

  ASSERT_EQ(ind.ucis.size(), 1);
  const auto& pusch_pdu = std::get<uci_indication::uci_pdu::uci_pusch_pdu>(ind.ucis[0].pdu);
  ASSERT_TRUE(pusch_pdu.csi.has_value());
  ASSERT_TRUE(pusch_pdu.csi->valid);
  ASSERT_TRUE(pusch_pdu.csi->ri.has_value());
  ASSERT_EQ(pusch_pdu.csi->ri->value(), 2);
  ASSERT_TRUE(pusch_pdu.csi->first_tb_wideband_cqi.has_value());
  ASSERT_EQ(pusch_pdu.csi->first_tb_wideband_cqi->value(), 15);
  ASSERT_TRUE(pusch_pdu.csi->pmi.has_value());
}

TEST_F(uci_cell_decoder_aperiodic_csi_test, when_csi_part2_is_not_detected_then_report_is_discarded)
{
  // The PHY reports the CSI Part 2 field as not detected (e.g. DTX declared by the short block detector). In that case
  // the payload is forwarded empty, even though CSI Part 1 was correctly decoded.
  mac_uci_pdu::pusch_type::csi_information csi2;
  csi2.is_valid = false;

  uci_indication ind = decoder.decode_uci(make_uci_ind(csi2));

  ASSERT_EQ(ind.ucis.size(), 1);
  const auto& pusch_pdu = std::get<uci_indication::uci_pdu::uci_pusch_pdu>(ind.ucis[0].pdu);
  ASSERT_TRUE(pusch_pdu.csi.has_value());
  ASSERT_FALSE(pusch_pdu.csi->valid) << "A partial CSI report must not be taken into account";
}

TEST_F(uci_cell_decoder_aperiodic_csi_test, when_csi_part2_is_absent_then_report_is_discarded)
{
  // The PHY does not report a CSI Part 2 field at all.
  uci_indication ind = decoder.decode_uci(make_uci_ind(std::nullopt));

  ASSERT_EQ(ind.ucis.size(), 1);
  const auto& pusch_pdu = std::get<uci_indication::uci_pdu::uci_pusch_pdu>(ind.ucis[0].pdu);
  ASSERT_TRUE(pusch_pdu.csi.has_value());
  ASSERT_FALSE(pusch_pdu.csi->valid) << "A partial CSI report must not be taken into account";
}
