// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "phy_to_fapi_results_event_fastpath_translator.h"
#include "ocudu/fapi/p7/messages/crc_indication.h"
#include "ocudu/fapi/p7/messages/rach_indication.h"
#include "ocudu/fapi/p7/messages/rx_data_indication.h"
#include "ocudu/fapi/p7/messages/srs_indication.h"
#include "ocudu/fapi/p7/messages/uci_indication.h"
#include "ocudu/fapi/p7/p7_indications_notifier.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/prach/prach_format_type.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;

namespace {

/// Captures the last indication of each type notified by the translator.
class p7_indications_notifier_spy : public fapi::p7_indications_notifier
{
public:
  void on_rx_data_indication(const fapi::rx_data_indication& msg) override {}
  void on_crc_indication(const fapi::crc_indication& msg) override {}
  void on_uci_indication(const fapi::uci_indication& msg) override {}
  void on_srs_indication(const fapi::srs_indication& msg) override { last_srs_ind = msg; }
  void on_rach_indication(const fapi::rach_indication& msg) override { last_rach_ind = msg; }

  std::optional<fapi::rach_indication> last_rach_ind;
  std::optional<fapi::srs_indication>  last_srs_ind;
};

/// Parameters of a PRACH occasion whose reported slot index is under test.
struct prach_slot_index_params {
  /// PUSCH subcarrier spacing, i.e. the numerology the PRACH buffer context slot is expressed in.
  subcarrier_spacing pusch_scs;
  /// PRACH subcarrier spacing, i.e. the numerology t_id is counted in.
  subcarrier_spacing msg1_scs;
  /// Preamble format.
  prach_format_type format;
  /// Slot the preamble is detected in, within the system frame.
  unsigned detection_slot_index;
  /// Expected value of the RACH.indication slotIndex field, i.e. the t_id.
  unsigned expected_slot_index;
};

void PrintTo(const prach_slot_index_params& value, ::std::ostream* os)
{
  *os << fmt::format("pusch_scs={} msg1_scs={} format={} detection_slot={}",
                     to_string(value.pusch_scs),
                     to_string(value.msg1_scs),
                     fmt::underlying(value.format),
                     value.detection_slot_index);
}

class prach_slot_index_test : public ::testing::TestWithParam<prach_slot_index_params>
{
protected:
  prach_slot_index_test() :
    translator(
        phy_to_fapi_results_event_fastpath_translator_config{.sector_id                     = 0,
                                                             .dbfs_to_dbm_conversion_factor = 0.F,
                                                             .db_to_dbfs_conversion_factor  = 0.F,
                                                             .msg1_scs                      = GetParam().msg1_scs},
        phy_to_fapi_results_event_fastpath_translator_dependencies{.logger = ocudulog::fetch_basic_logger("FAPI")})
  {
    translator.set_p7_indications_notifier(notifier);
  }

  /// Builds a PRACH result with a single detected preamble at the parameterized detection slot.
  static ul_prach_results make_prach_results()
  {
    const prach_slot_index_params& params = GetParam();

    ul_prach_results result;
    result.context.slot         = slot_point(params.pusch_scs, 0, params.detection_slot_index);
    result.context.start_symbol = 0;
    result.context.format       = params.format;
    result.result.rssi_dB       = 0.F;

    prach_detection_result::preamble_indication& preamble = result.result.preambles.emplace_back();
    preamble.preamble_index                               = 0;
    // Only preambles with a non-negative TA are reported.
    preamble.time_advance      = phy_time_unit::from_seconds(0);
    preamble.preamble_power_dB = 0.F;

    return result;
  }

  p7_indications_notifier_spy                   notifier;
  phy_to_fapi_results_event_fastpath_translator translator;
};

} // namespace

/// \brief The reported slotIndex is the occasion's t_id (SCF-222 Section 3.4.11), as \ref
/// ra_helper::get_prach_occasion_slot_index derives it, and not the slot the preamble was detected in.
TEST_P(prach_slot_index_test, reported_slot_index_is_the_occasion_t_id)
{
  translator.on_new_prach_results(make_prach_results());

  ASSERT_TRUE(notifier.last_rach_ind.has_value()) << "No RACH.indication was generated";
  ASSERT_EQ(GetParam().expected_slot_index, notifier.last_rach_ind->pdu.slot_index);
  // The message header keeps reporting the detection slot.
  ASSERT_EQ(GetParam().detection_slot_index, notifier.last_rach_ind->slot.slot_index());
}

INSTANTIATE_TEST_SUITE_P(
    prach_slot_index_test,
    prach_slot_index_test,
    // The t_id derivation itself is covered by ra_helper_test; these two cases only check that the translator reports
    // the derived value, for the configured PRACH subcarrier spacing, in the PDU rather than in the message header.
    ::testing::Values(
        // Long format at 30 kHz: t_id is the subframe index.
        prach_slot_index_params{subcarrier_spacing::kHz30, subcarrier_spacing::invalid, prach_format_type::one, 14, 7},
        // Short format whose PRACH SCS is coarser than the PUSCH SCS: t_id counts the coarser slots.
        prach_slot_index_params{subcarrier_spacing::kHz30, subcarrier_spacing::kHz15, prach_format_type::A1, 15, 7}));

/// \brief With NTN k_mac != 0 the gNB DL and UL frames are misaligned, so the PRACH detection PDU is scheduled at the
/// DL-clock slot (UL occasion + k_mac). The reported t_id must name the UE's UL occasion, which is what the UE used to
/// derive the RA-RNTI (TS 38.321 Section 5.1.3).
TEST(prach_slot_index_ntn_test, reported_slot_index_is_rebased_to_the_ue_ul_occasion)
{
  static constexpr unsigned           K_MAC_SLOTS      = 4;
  static constexpr subcarrier_spacing SCS              = subcarrier_spacing::kHz30;
  static constexpr unsigned           UL_OCCASION_SLOT = 10;
  static constexpr unsigned           DETECTION_SLOT   = UL_OCCASION_SLOT + K_MAC_SLOTS;
  // Long format at 30 kHz: t_id is the subframe index of the UL occasion slot.
  static constexpr unsigned EXPECTED_SLOT_INDEX = UL_OCCASION_SLOT / 2;

  p7_indications_notifier_spy                   notifier;
  phy_to_fapi_results_event_fastpath_translator translator(
      phy_to_fapi_results_event_fastpath_translator_config{.sector_id                     = 0,
                                                           .dbfs_to_dbm_conversion_factor = 0.F,
                                                           .db_to_dbfs_conversion_factor  = 0.F,
                                                           .msg1_scs                      = subcarrier_spacing::invalid,
                                                           .ntn_k_mac_slots               = K_MAC_SLOTS},
      phy_to_fapi_results_event_fastpath_translator_dependencies{.logger = ocudulog::fetch_basic_logger("FAPI")});
  translator.set_p7_indications_notifier(notifier);

  ul_prach_results result;
  result.context.slot         = slot_point(SCS, 0, DETECTION_SLOT);
  result.context.start_symbol = 0;
  result.context.format       = prach_format_type::one;
  result.result.rssi_dB       = 0.F;

  prach_detection_result::preamble_indication& preamble = result.result.preambles.emplace_back();
  preamble.preamble_index                               = 0;
  preamble.time_advance                                 = phy_time_unit::from_seconds(0);
  preamble.preamble_power_dB                            = 0.F;

  translator.on_new_prach_results(result);

  ASSERT_TRUE(notifier.last_rach_ind.has_value()) << "No RACH.indication was generated";
  ASSERT_EQ(EXPECTED_SLOT_INDEX, notifier.last_rach_ind->pdu.slot_index)
      << "The reported t_id must be rebased by k_mac to the UE UL occasion";
  // The message header keeps reporting the DL-clock detection slot.
  ASSERT_EQ(DETECTION_SLOT, notifier.last_rach_ind->slot.slot_index());
}

namespace {

/// Parameters of a positioning SRS result whose Angle of Arrival report is under test.
struct srs_aoa_params {
  /// DoA components estimated by the PHY. Empty if no DoA was estimated.
  std::optional<std::vector<doa_estimator_result::doa_component_type>> doa_components;
  /// Expected azimuth Angle of Arrival, in degrees, reported in the SRS.indication.
  std::optional<float> expected_azimuth_aoa_deg;
};

void PrintTo(const srs_aoa_params& value, ::std::ostream* os)
{
  if (!value.doa_components.has_value()) {
    *os << "no DoA";
    return;
  }
  *os << fmt::format("nof_components={}", value.doa_components->size());
  for (const auto& component : *value.doa_components) {
    *os << fmt::format(" {{angle={} strength={}}}", component.broadside_angle_degrees, component.spectrum_strength);
  }
}

class srs_aoa_report_test : public ::testing::TestWithParam<srs_aoa_params>
{
protected:
  srs_aoa_report_test() :
    translator(
        phy_to_fapi_results_event_fastpath_translator_config{.sector_id                     = 0,
                                                             .dbfs_to_dbm_conversion_factor = 0.F,
                                                             .db_to_dbfs_conversion_factor  = 0.F,
                                                             .msg1_scs = subcarrier_spacing::invalid},
        phy_to_fapi_results_event_fastpath_translator_dependencies{.logger = ocudulog::fetch_basic_logger("FAPI")})
  {
    translator.set_p7_indications_notifier(notifier);
  }

  /// Builds a positioning SRS result carrying the parameterized DoA components.
  static ul_srs_results make_srs_results()
  {
    const srs_aoa_params& params = GetParam();

    ul_srs_results result;
    result.context.slot                                             = slot_point(subcarrier_spacing::kHz30, 0, 3);
    result.context.rnti                                             = to_rnti(0x4601);
    result.context.is_normalized_channel_iq_matrix_report_requested = false;
    result.context.is_positioning_report_requested                  = true;
    result.processor_result.time_alignment.time_alignment           = 0.0;

    if (params.doa_components.has_value()) {
      doa_estimator_result& doa = result.processor_result.doa_result.emplace(params.doa_components->size());
      std::copy(params.doa_components->begin(), params.doa_components->end(), doa.doa_components.begin());
    }

    return result;
  }

  p7_indications_notifier_spy                   notifier;
  phy_to_fapi_results_event_fastpath_translator translator;
};

} // namespace

/// The strongest DoA component is reported as the azimuth Angle of Arrival, without a zenith angle.
TEST_P(srs_aoa_report_test, strongest_doa_component_is_reported_as_azimuth_aoa)
{
  translator.on_new_srs_results(make_srs_results());

  ASSERT_TRUE(notifier.last_srs_ind.has_value()) << "No SRS.indication was generated";
  const std::optional<fapi::srs_positioning_report>& positioning = notifier.last_srs_ind->pdu.positioning;
  ASSERT_TRUE(positioning.has_value()) << "No positioning report was generated";

  ASSERT_EQ(GetParam().expected_azimuth_aoa_deg, positioning->azimuth_aoa_deg);
  ASSERT_FALSE(positioning->zenith_aoa_deg.has_value());
}

INSTANTIATE_TEST_SUITE_P(
    srs_aoa_report_test,
    srs_aoa_report_test,
    ::testing::Values(
        // DoA estimation disabled.
        srs_aoa_params{std::nullopt, std::nullopt},
        // DoA estimation enabled, but no component detected.
        srs_aoa_params{std::vector<doa_estimator_result::doa_component_type>{}, std::nullopt},
        // Positive broadside angle.
        srs_aoa_params{std::vector<doa_estimator_result::doa_component_type>{{30.F, 10.F}, {-60.F, 2.F}}, 30.F},
        // Negative broadside angle.
        srs_aoa_params{std::vector<doa_estimator_result::doa_component_type>{{-45.F, 10.F}, {20.F, 2.F}}, -45.F},
        // Broadside direction.
        srs_aoa_params{std::vector<doa_estimator_result::doa_component_type>{{0.F, 10.F}}, 0.F}));
