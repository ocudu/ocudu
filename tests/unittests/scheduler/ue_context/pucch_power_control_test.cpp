// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/scheduler/test_utils/sched_custom_test_bench.h"
#include "ocudu/ran/power_control/tpc_mapping.h"
#include "ocudu/ran/pucch/pucch_info.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

// This is taken from the pucch_power_controller.cpp file.
static float compute_delta_tf_format_2_3_4(unsigned nof_uci_bits, unsigned payload_plus_crc_bits, unsigned n_res)
{
  static constexpr unsigned max_uci_payload_no_crc = 11U;
  // Number of bits per RE.
  const float bpre = static_cast<float>(payload_plus_crc_bits) / static_cast<float>(n_res);

  return nof_uci_bits <= max_uci_payload_no_crc ? convert_power_to_dB(6.0f * bpre)
                                                : convert_power_to_dB(std::pow(2.0f, 2.4f * bpre) - 1.0);
}

// This is taken from the pucch_power_controller.cpp file.
static float compute_delta_tf(pucch_format   format,
                              unsigned       nof_prbs,
                              unsigned       nof_symb,
                              pucch_uci_bits uci_bits,
                              bool           pi_2_bpsk,
                              bool           additional_dmrs,
                              bool           intraslot_freq_hopping)
{
  float delta_tf = 0.0f;
  // Compute the delta TF value for the PUCCH power control as per Section 7.2.1, TS 38.213.
  // TODO: introduce a bias for the delta TF value based on the SINR targes given in Section 8.3, TS 38.104, where  each
  //       SINR target is computed for specific number or RBs, symbols and UCI bits.
  switch (format) {
    case pucch_format::FORMAT_0:
      delta_tf = convert_power_to_dB(2.0f / static_cast<float>(nof_symb));
      break;
    case pucch_format::FORMAT_1: {
      // NOTE: To handle the case of PUCCH format 1 with only SR, when the UCI only contains the SR, we count 1 UCI bit.
      // This is because PUCCH format 1 with only SR uses BPSK modulation, the same as when PUCCH F1 transmits 1
      // HARQ bit (ref. to TS 38.211, Section 6.3.2.4.1).
      const unsigned uci_bits_pw_ctrl_f1 = uci_bits.harq_ack_nof_bits != 0 ? uci_bits.harq_ack_nof_bits : 1U;
      delta_tf                           = convert_power_to_dB(14.0f / static_cast<float>(nof_symb)) +
                 convert_power_to_dB(static_cast<float>(uci_bits_pw_ctrl_f1));
      break;
    }
    case pucch_format::FORMAT_2: {
      const unsigned nof_uci_bits          = uci_bits.get_total_bits();
      const unsigned e_uci                 = get_pucch_format2_E_total(nof_prbs, nof_symb);
      const unsigned payload_plus_crc_bits = nof_uci_bits + get_uci_nof_crc_bits(nof_uci_bits, e_uci);

      // Number of resource elements.
      const unsigned n_re = nof_prbs * nof_symb * pucch_constants::f2::NOF_DATA_SUBC_PER_RB;

      // For the following computation, refer to the relevant section for the used formula.
      delta_tf = compute_delta_tf_format_2_3_4(nof_uci_bits, payload_plus_crc_bits, n_re);
      break;
    }
    case pucch_format::FORMAT_3:
    case pucch_format::FORMAT_4: {
      const unsigned nof_uci_bits          = uci_bits.get_total_bits();
      const unsigned e_uci                 = format == ocudu::pucch_format::FORMAT_3
                                                 ? get_pucch_format3_E_total(nof_prbs, nof_symb, pi_2_bpsk)
                                                 : get_pucch_format4_E_total(nof_prbs, nof_symb, pi_2_bpsk);
      const unsigned payload_plus_crc_bits = nof_uci_bits + get_uci_nof_crc_bits(nof_uci_bits, e_uci);

      // Number of resource elements.
      const unsigned n_re =
          nof_prbs *
          (nof_symb - get_pucch_format3_4_nof_dmrs_symbols(nof_symb, intraslot_freq_hopping, additional_dmrs)) *
          NOF_SUBCARRIERS_PER_RB;

      delta_tf = compute_delta_tf_format_2_3_4(nof_uci_bits, payload_plus_crc_bits, n_re);
      break;
    }
    default:
      break;
  }

  return delta_tf;
}

namespace pucch_pw_ctrl_test {

struct pucch_pw_ctrl_params {
  float        pusch_sinr_f0_db;
  float        pusch_sinr_f2_f3_db;
  pucch_format format_set_0;
  pucch_format format_set_1;
};

//// Dummy operator to avoid Valgrind warnings.
std::ostream& operator<<(std::ostream& os, const pucch_pw_ctrl_params& params)
{
  return os;
}

class pucch_power_control_test_bench : public ::testing::TestWithParam<pucch_pw_ctrl_params>,
                                       public sched_basic_custom_test_bench
{
  static cell_config_builder_params make_cell_config_params()
  {
    return cell_config_builder_params{
        .scs_common = subcarrier_spacing::kHz30,
        .dl_carrier = {.carrier_bw = bs_channel_bandwidth::MHz20, .arfcn_f_ref = 520000U}};
  }

  static pucch_resource_builder_params make_pucch_builder_params()
  {
    pucch_resource_builder_params pucch_params;

    if (GetParam().format_set_0 == pucch_format::FORMAT_0) {
      pucch_params.f0_or_f1_params.emplace<pucch_f0_params>(pucch_f0_params{});
    } else if (GetParam().format_set_0 == pucch_format::FORMAT_1) {
      pucch_params.f0_or_f1_params.emplace<pucch_f1_params>(pucch_f1_params{});
    }

    if (GetParam().format_set_1 == pucch_format::FORMAT_2) {
      pucch_params.f2_or_f3_or_f4_params.emplace<pucch_f2_params>(pucch_f2_params{});
    } else if (GetParam().format_set_1 == pucch_format::FORMAT_3) {
      pucch_params.f2_or_f3_or_f4_params.emplace<pucch_f3_params>(pucch_f3_params{});
    } else if (GetParam().format_set_1 == pucch_format::FORMAT_4) {
      pucch_params.f2_or_f3_or_f4_params.emplace<pucch_f4_params>(pucch_f4_params{});
    }
    return pucch_params;
  }

  static sched_cell_configuration_request_message make_cell_config_request(const pucch_pw_ctrl_params& tparams)
  {
    auto req = sched_config_helper::make_default_sched_cell_configuration_request(make_cell_config_params());

    // Overwrite the default list of dedicated PUCCH resources.
    req.ran.init_bwp.pucch.resources = make_pucch_builder_params();
    if (tparams.format_set_0 == pucch_format::FORMAT_0) {
      req.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = 0;
    } else if (tparams.format_set_0 == pucch_format::FORMAT_1) {
      req.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = 11;
    }

    return req;
  }

protected:
  pucch_power_control_test_bench() :
    sched_basic_custom_test_bench(
        [target_sinr_f0 = GetParam().pusch_sinr_f0_db, target_sinr_f2_3 = GetParam().pusch_sinr_f2_f3_db]() {
          scheduler_expert_config exp_cfg                     = config_helpers::make_default_scheduler_expert_config();
          exp_cfg.ue.ul_power_ctrl.enable_pucch_cl_pw_control = true;
          exp_cfg.ue.ul_power_ctrl.pucch_f0_sinr_target_dB    = target_sinr_f0;
          exp_cfg.ue.ul_power_ctrl.pucch_f2_sinr_target_dB    = target_sinr_f2_3;
          exp_cfg.ue.ul_power_ctrl.pucch_f3_sinr_target_dB    = target_sinr_f2_3;
          return exp_cfg;
        }(),
        make_cell_config_params(),
        make_cell_config_request(GetParam())),
    format_set_0(GetParam().format_set_0),
    format_set_1(GetParam().format_set_1)
  {
    // The scope of the following configuration is solely to build the test_bench with a UE. The PUCCH configuration
    // is not used at all for the test, apart from construction and Setup. However, the PUCCH power controller requires
    // the UE configuration to retrieve the formats used by the PUCCH resources; therefore, we need to build a UE
    // configuration that is valid, otherwise the configuration validator triggers an error.
    // The number of PUCCH symbols, PRBs and payload used for the test are generated randomly instead of being taken
    // from the given config; otherwise, it would be extremely complex to build a test in which the symbols, PRBs
    // payload bits change frequently as needed by this test.

    ocudu_assert(format_set_0 == pucch_format::FORMAT_0 or format_set_0 == pucch_format::FORMAT_1,
                 "For PUCCH resources set 0, Format 2, 3 and 4 are not valid");
    ocudu_assert(format_set_1 == pucch_format::FORMAT_2 or format_set_1 == pucch_format::FORMAT_3 or
                     format_set_1 == pucch_format::FORMAT_4,
                 "For PUCCH resources set 1, Format 0 and are not valid");

    sched_ue_creation_request_message ue_req = cfg_mng.get_default_ue_config_request();
    pucch_res_builder_test_helper     pucch_builder(cell_cfg.expert_cfg.ue.max_pucchs_per_slot);
    pucch_builder.setup(cell_cfg.params);
    pucch_builder.add_build_new_ue_pucch_cfg(ue_req.cfg.cells.value().front());
    add_ue(ue_req);
  }

  ~pucch_power_control_test_bench() override { ocudulog::flush(); }

  // Basic constants for the test.
  static constexpr unsigned TDD_PATTERN_LENGTH = 10U;

  void SetUp() override
  {
    // Code here will be called immediately after the constructor (right
    // before each test).
    auto* test_ue = ues.find(to_du_ue_index(0));
    ocudu_assert(test_ue != nullptr, "UE not found in the UE repository");
    pucch_pw_ctrl_manager = &(test_ue->get_pcell().get_pucch_power_controller());
    ocudulog::init();
    mac_logger.set_level(ocudulog::basic_levels::warning);
    test_logger.set_level(ocudulog::basic_levels::info);
  }

  struct pucch_pw_control {
    slot_point     slot_rx;
    pucch_format   format;
    unsigned       nof_prbs;
    unsigned       nof_symb;
    pucch_uci_bits uci_bits               = {};
    bool           pi_2_bpsk              = false;
    bool           additional_dmrs        = false;
    bool           intraslot_freq_hopping = false;
  };

  pucch_pw_control generate_pucch(slot_point sl) const
  {
    pucch_pw_control pucch;
    pucch.format  = test_rng::bernoulli(0.5) ? format_set_0 : format_set_1;
    pucch.slot_rx = sl;
    switch (pucch.format) {
        // NOTE: the PRBs should be set based on the symbols and UCI bits, but this is not meaningful for the PUCCH
        // power control.
      case pucch_format::FORMAT_0:
        pucch.nof_prbs                   = 1U;
        pucch.nof_symb                   = 2U;
        pucch.uci_bits.harq_ack_nof_bits = test_rng::uniform_int(1U, 2U);
        break;
      case pucch_format::FORMAT_1:
        pucch.nof_prbs                   = 1U;
        pucch.nof_symb                   = 14U;
        pucch.uci_bits.harq_ack_nof_bits = test_rng::uniform_int(1U, 2U);
        break;
      case pucch_format::FORMAT_2:
        pucch.nof_prbs                    = test_rng::uniform_int(1U, 4U);
        pucch.nof_symb                    = 2U;
        pucch.uci_bits.harq_ack_nof_bits  = test_rng::uniform_int(1U, 7U);
        pucch.uci_bits.csi_part1_nof_bits = test_rng::bernoulli(0.25f) ? 11U : 0U;
        pucch.uci_bits.sr_bits = test_rng::bernoulli(0.25f) ? ocudu::sr_nof_bits::one : ocudu::sr_nof_bits::no_sr;
        break;
      case pucch_format::FORMAT_3:
        pucch.nof_prbs                    = test_rng::uniform_int(1U, 4U);
        pucch.nof_symb                    = 4U;
        pucch.nof_prbs                    = test_rng::uniform_int(1U, 4U);
        pucch.nof_symb                    = 2U;
        pucch.uci_bits.harq_ack_nof_bits  = test_rng::uniform_int(1U, 7U);
        pucch.uci_bits.csi_part1_nof_bits = test_rng::bernoulli(0.25f) ? 11U : 0U;
        pucch.uci_bits.sr_bits = test_rng::bernoulli(0.25f) ? ocudu::sr_nof_bits::one : ocudu::sr_nof_bits::no_sr;
        break;
      case pucch_format::FORMAT_4:
        pucch.nof_prbs                    = 1U;
        pucch.nof_symb                    = 4U;
        pucch.uci_bits.harq_ack_nof_bits  = test_rng::uniform_int(1U, 7U);
        pucch.uci_bits.csi_part1_nof_bits = test_rng::bernoulli(0.25f) ? 11U : 0U;
        pucch.uci_bits.sr_bits = test_rng::bernoulli(0.25f) ? ocudu::sr_nof_bits::one : ocudu::sr_nof_bits::no_sr;
        break;
      default:
        ocudu_assertion_failure("Only PUCCH format 0, 1, 2, 3 and 4 are supported");
    }

    return pucch;
  }

  friend struct fmt::formatter<pucch_power_control_test_bench::pucch_pw_control>;

  // Initial SINR value before closed-loop power control.
  float        initial_sinr = 2.0f;
  pucch_format format_set_0;
  pucch_format format_set_1;

  pucch_power_controller* pucch_pw_ctrl_manager = nullptr;

  ocudulog::basic_logger& mac_logger  = ocudulog::fetch_basic_logger("SCHED", false);
  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST", true);
};

} // namespace pucch_pw_ctrl_test

namespace fmt {

// pucch_power_control_test_bench::pucch_pw_control formatter.
template <>
struct formatter<pucch_pw_ctrl_test::pucch_power_control_test_bench::pucch_pw_control> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const pucch_pw_ctrl_test::pucch_power_control_test_bench::pucch_pw_control& pucch,
              FormatContext&                                                              ctx) const
  {
    return format_to(ctx.out(),
                     "slot_rx={} Format={} prbs={} sym={}: {}",
                     pucch.slot_rx,
                     to_string(pucch.format),
                     pucch.nof_prbs,
                     pucch.nof_symb,
                     pucch.uci_bits);
  }
};

} // namespace fmt

using namespace pucch_pw_ctrl_test;

TEST_P(pucch_power_control_test_bench, when_phr_is_non_positive_cl_stops_increasing_pw_target)
{
  // In this test, as assume TDD with 10 slots period, and 2 UL slots. We generate a PUCCH with a Bernoulli random
  // process with probability 0.5 in each UL slot. The PUCCH parameters and UCI bits are randomly set, to simulate
  // deterministic SINR variations due to the PUCCH parameters. The test succeeds if the SINR reaches convergence
  // towards the SINR target.

  slot_point sl = slot_point(to_numerology_value(subcarrier_spacing::kHz30), 0U);

  // PUCCH for slot .8 and .18.
  std::optional<pucch_pw_control> pucch_1 = std::nullopt;
  // PUCCH for slot .9 and .19.
  std::optional<pucch_pw_control> pucch_2 = std::nullopt;

  // This is the parameter that is used to determine the convergence of the test.
  exp_average_fast_start<float> average_reported_sinr_f0(0.5f);
  exp_average_fast_start<float> average_reported_sinr_f2_f3(0.5f);

  float g_cl_pw_control = 0;

  // Used to indicate if the test has been successful.
  bool convergence_reached = false;

  for (unsigned sl_cnt = 0, sl_max = 1200; sl_cnt != sl_max; ++sl_cnt, ++sl) {
    const unsigned slot_idx_tdd_slot = sl.to_uint() % TDD_PATTERN_LENGTH;
    const bool     is_dl_slot        = slot_idx_tdd_slot <= 7U;

    // In DL slots, process the PUCCH SINR, which is supposed to be carried by the UCI indication.
    if (is_dl_slot) {
      for (const auto& p : {&pucch_1, &pucch_2}) {
        std::optional<pucch_pw_control>& pucch_pw = (*p);
        if (pucch_pw.has_value()) {
          const auto& pucch          = pucch_pw.value();
          const float pucch_sinr_var = compute_delta_tf(pucch.format,
                                                        pucch.nof_prbs,
                                                        pucch.nof_symb,
                                                        pucch.uci_bits,
                                                        pucch.pi_2_bpsk,
                                                        pucch.additional_dmrs,
                                                        pucch.intraslot_freq_hopping);
          const auto  gaussian_var   = test_rng::normal_dist<float>(0, 0.5f);
          const float sinr_to_report = initial_sinr + pucch_sinr_var + gaussian_var + g_cl_pw_control;
          if (pucch.format == pucch_format::FORMAT_0 or pucch.format == pucch_format::FORMAT_1) {
            average_reported_sinr_f0.push(initial_sinr + gaussian_var + g_cl_pw_control);
            pucch_pw_ctrl_manager->update_pucch_sinr_f0_f1(pucch.slot_rx, sinr_to_report);
            test_logger.debug("Slot={}: Updating PUCCH={} delta_tf={} reported_sinr={}dB av_sinr={}dB",
                              sl,
                              pucch,
                              pucch_sinr_var,
                              sinr_to_report,
                              average_reported_sinr_f0.average());
          } else if (pucch.format == pucch_format::FORMAT_2 or pucch.format == pucch_format::FORMAT_3 or
                     pucch.format == pucch_format::FORMAT_4) {
            average_reported_sinr_f2_f3.push(initial_sinr + gaussian_var + g_cl_pw_control);
            pucch_pw_ctrl_manager->update_pucch_sinr_f2_f3_f4(pucch.slot_rx,
                                                              sinr_to_report,
                                                              pucch.uci_bits.harq_ack_nof_bits != 0,
                                                              pucch.uci_bits.csi_part1_nof_bits != 0);
            test_logger.debug("Slot={}: Updating PUCCH={} delta_tf={} reported_sinr={}dB av_sinr={}dB",
                              sl,
                              pucch,
                              pucch_sinr_var,
                              sinr_to_report,
                              average_reported_sinr_f2_f3.average());
          }
          pucch_pw.reset();
        }
      }
      ASSERT_FALSE(pucch_1.has_value());
      ASSERT_FALSE(pucch_2.has_value());
    }
    // In UL slots, generate a PUCCH, compute the TPC command and update the PUCCH power control state.
    // NOTE: the TPC command would be sent during the DL slots, but we can ignore this for sake of simplifying the test.
    else {
      std::optional<pucch_pw_control>& pucch = slot_idx_tdd_slot == 8U ? pucch_1 : pucch_2;
      if (test_rng::bernoulli(0.5)) {
        pucch             = generate_pucch(sl);
        const uint8_t tpc = pucch_pw_ctrl_manager->compute_tpc_command(sl);
        g_cl_pw_control += tpc_mapping(tpc);
        pucch_pw_ctrl_manager->update_pucch_pw_ctrl_state(sl,
                                                          pucch.value().format,
                                                          pucch.value().nof_prbs,
                                                          pucch.value().nof_symb,
                                                          pucch.value().uci_bits,
                                                          pucch.value().pi_2_bpsk,
                                                          pucch.value().additional_dmrs,
                                                          pucch.value().intraslot_freq_hopping);
        test_logger.debug("Slot={}: TPC={} g_cl={} Saving PUCCH={}", sl, tpc, g_cl_pw_control, pucch.value());
      }
    }

    // > Check if the SINR has converged to the target SINR.
    const float actual_av_sinr_f0_dB   = average_reported_sinr_f0.average();
    const float actual_av_sinr_f2_3_dB = average_reported_sinr_f2_f3.average();

    // Wait at least 160 slots before checking the SINR convergence, to avoid trivial convergence when the initial SINR
    // is set the same as the target.
    static constexpr unsigned min_nof_slots = 160U;
    if (sl_cnt > min_nof_slots) {
      // We consider the SINR has converged when the average SINR is within 0.5 dB of the target SINR. This is to
      // account the precision of the TPC commands.
      if (format_set_0 == ocudu::pucch_format::FORMAT_0 and format_set_1 == ocudu::pucch_format::FORMAT_4) {
        if (actual_av_sinr_f0_dB < GetParam().pusch_sinr_f0_db + 0.5f and
            actual_av_sinr_f0_dB > GetParam().pusch_sinr_f0_db - 0.5f) {
          convergence_reached = true;
        }
      } else if (format_set_0 == ocudu::pucch_format::FORMAT_1 and
                 (format_set_1 == ocudu::pucch_format::FORMAT_2 or format_set_1 == ocudu::pucch_format::FORMAT_3)) {
        if (actual_av_sinr_f2_3_dB < GetParam().pusch_sinr_f2_f3_db + 0.5f and
            actual_av_sinr_f2_3_dB > GetParam().pusch_sinr_f2_f3_db - 0.5f) {
          convergence_reached = true;
        }
      }
      // With Format 0 + Format 2/3, the convergence is reached when the at least one of the SINR reaches convergence,
      // while the other one is above the threshold.
      // NOTE: it is not possible to reach the convergence for both, as there is only 1 TPC command and 2 different
      // targets to achieve.
      else if (format_set_0 == ocudu::pucch_format::FORMAT_0 and
               (format_set_1 == ocudu::pucch_format::FORMAT_2 or format_set_1 == ocudu::pucch_format::FORMAT_3)) {
        if ((actual_av_sinr_f0_dB < GetParam().pusch_sinr_f0_db + 0.5f and
             actual_av_sinr_f0_dB > GetParam().pusch_sinr_f0_db - 0.5f and
             actual_av_sinr_f2_3_dB >= GetParam().pusch_sinr_f2_f3_db - 0.5f) or
            (actual_av_sinr_f2_3_dB < GetParam().pusch_sinr_f2_f3_db + 0.5f and
             actual_av_sinr_f2_3_dB > GetParam().pusch_sinr_f2_f3_db - 0.5f and
             actual_av_sinr_f0_dB >= GetParam().pusch_sinr_f0_db - 0.5f)) {
          convergence_reached = true;
        }
      }
    }

    if (convergence_reached) {
      break;
    }
  }
  ASSERT_TRUE(convergence_reached) << fmt::format(
      "The SINR didn't converge with Format 0/1 SINR={}dB and Format 2/3/4 SINR={}dB, format_set_0={} format_set_1={}",
      GetParam().pusch_sinr_f0_db,
      GetParam().pusch_sinr_f2_f3_db,
      to_string(GetParam().format_set_0),
      to_string(GetParam().format_set_1));
}

INSTANTIATE_TEST_SUITE_P(
    test_ul_power_control_phr,
    pucch_power_control_test_bench,
    testing::Values(pucch_pw_ctrl_params{0, -2, ocudu::pucch_format::FORMAT_1, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{0, 5, ocudu::pucch_format::FORMAT_1, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{0, 8, ocudu::pucch_format::FORMAT_1, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{0, 10, ocudu::pucch_format::FORMAT_1, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{-2, 5, ocudu::pucch_format::FORMAT_0, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{5, -2, ocudu::pucch_format::FORMAT_0, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{5, 8, ocudu::pucch_format::FORMAT_0, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{8, 5, ocudu::pucch_format::FORMAT_0, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{0, 8, ocudu::pucch_format::FORMAT_0, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{8, 0, ocudu::pucch_format::FORMAT_0, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{8, 10, ocudu::pucch_format::FORMAT_0, ocudu::pucch_format::FORMAT_2},
                    pucch_pw_ctrl_params{10, 8, ocudu::pucch_format::FORMAT_0, ocudu::pucch_format::FORMAT_2}),
    [](const testing::TestParamInfo<pucch_power_control_test_bench::ParamType>& info_) {
      return fmt::format(
          "SINR_f0_{}dB_SINR_f2_3_{}dB_Format_{}_{}",
          [](int val) {
            if (val < 0) {
              return fmt::format("minus_{}", static_cast<unsigned>(-val));
            }
            return fmt::format("{}", static_cast<unsigned>(val));
          }(info_.param.pusch_sinr_f0_db),
          [](int val) {
            if (val < 0) {
              return fmt::format("minus_{}", static_cast<unsigned>(-val));
            }
            return fmt::format("{}", static_cast<unsigned>(val));
          }(info_.param.pusch_sinr_f2_f3_db),
          to_string(info_.param.format_set_0),
          to_string(info_.param.format_set_1));
    });
