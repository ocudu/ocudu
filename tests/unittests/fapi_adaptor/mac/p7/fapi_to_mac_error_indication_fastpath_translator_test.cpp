// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fapi_to_mac_error_indication_fastpath_translator.h"
#include "ocudu/fapi/common/error_indication.h"
#include "ocudu/mac/mac_cell_slot_handler.h"
#include "ocudu/mac/mac_cell_timing_context.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <gtest/gtest.h>
#include <optional>
#include <vector>

using namespace ocudu;
using namespace fapi_adaptor;

/// Builds an ERROR.indication for the given request and error code. The message has no slot when none is given.
static fapi::error_indication build_error_indication(fapi::message_type_id     message_id,
                                                     fapi::error_code_id       code,
                                                     std::optional<slot_point> slot = std::nullopt)
{
  return fapi::error_indication{
      .slot = slot, .message_id = message_id, .error_code = code, .expected_slot = std::nullopt};
}

/// Slot of the ERROR.indication messages built by the tests.
static const slot_point test_slot(subcarrier_spacing::kHz30, 1, 3);

namespace {

/// MAC slot handler spy that records the error indications it receives.
class mac_cell_slot_handler_spy : public mac_cell_slot_handler
{
public:
  struct error_indication_record {
    slot_point                         slot;
    mac_cell_slot_handler::error_event event;
  };

  void handle_slot_indication(const mac_cell_timing_context& context) override {}

  void handle_error_indication(slot_point sl_tx, error_event event) override
  {
    errors.push_back(error_indication_record{.slot = sl_tx, .event = event});
  }

  void handle_stop_indication() override {}

  std::vector<error_indication_record> errors;
};

/// Fixture that connects the translator to a MAC slot handler spy.
class fapi_to_mac_error_indication_test : public ::testing::Test
{
protected:
  fapi_to_mac_error_indication_test() { translator.set_cell_slot_handler(handler); }

  mac_cell_slot_handler_spy                        handler;
  fapi_to_mac_error_indication_fastpath_translator translator{0, ocudulog::fetch_basic_logger("FAPI")};
};

/// Error code and message identifier of an ERROR.indication, and the effect the MAC must be told about.
struct error_mapping_param {
  fapi::error_code_id   code;
  fapi::message_type_id message_id;
  bool                  pdcch_discarded;
  bool                  pdsch_discarded;
  bool                  pusch_and_pucch_discarded;
};

class fapi_to_mac_error_indication_fixture : public fapi_to_mac_error_indication_test,
                                             public ::testing::WithParamInterface<error_mapping_param>
{};

} // namespace

TEST_P(fapi_to_mac_error_indication_fixture, error_code_is_mapped_to_the_results_discarded_by_the_lower_layers)
{
  translator.on_error_indication(build_error_indication(GetParam().message_id, GetParam().code, test_slot));

  ASSERT_EQ(1U, handler.errors.size());
  const mac_cell_slot_handler_spy::error_indication_record& error = handler.errors.front();
  EXPECT_EQ(test_slot, error.slot);
  EXPECT_EQ(GetParam().pdcch_discarded, error.event.pdcch_discarded);
  EXPECT_EQ(GetParam().pdsch_discarded, error.event.pdsch_discarded);
  EXPECT_EQ(GetParam().pusch_and_pucch_discarded, error.event.pusch_and_pucch_discarded);
}

// The combinations below are the error codes that SCF-222 lists for each request in Tables 3.4.5-1 to 3.4.5-3 and
// 3.4.6.1-1: DL_TTI.request (INVALID_STATE, OUT_OF_SYNC, BCH_MISSING, SLOT_ERR), UL_TTI.request (INVALID_STATE,
// SLOT_ERR), UL_DCI.request (INVALID_STATE, INVALID_SFN, UL_DCI_ERR) and TX_DATA.request (INVALID_STATE, INVALID_SFN,
// TX_ERR).
using error_code    = fapi::error_code_id;
using message_id    = fapi::message_type_id;
using error_mapping = error_mapping_param;

INSTANTIATE_TEST_SUITE_P(error_codes,
                         fapi_to_mac_error_indication_fixture,
                         ::testing::Values(
                             // OUT_OF_SYNC and MSG_SLOT_ERR on a DL_TTI.request lose the downlink.
                             error_mapping{.code                      = error_code::out_of_sync,
                                           .message_id                = message_id::dl_tti_request,
                                           .pdcch_discarded           = true,
                                           .pdsch_discarded           = true,
                                           .pusch_and_pucch_discarded = false},
                             error_mapping{.code                      = error_code::msg_slot_err,
                                           .message_id                = message_id::dl_tti_request,
                                           .pdcch_discarded           = true,
                                           .pdsch_discarded           = true,
                                           .pusch_and_pucch_discarded = false},
                             // MSG_SLOT_ERR on a UL_TTI.request loses the PUSCH and PUCCH.
                             error_mapping{.code                      = error_code::msg_slot_err,
                                           .message_id                = message_id::ul_tti_request,
                                           .pdcch_discarded           = false,
                                           .pdsch_discarded           = false,
                                           .pusch_and_pucch_discarded = true},
                             // MSG_INVALID_SFN on a UL_DCI.request loses the PDCCH, and on a TX_DATA.request the PDSCH.
                             error_mapping{.code                      = error_code::msg_invalid_sfn,
                                           .message_id                = message_id::ul_dci_request,
                                           .pdcch_discarded           = true,
                                           .pdsch_discarded           = false,
                                           .pusch_and_pucch_discarded = false},
                             error_mapping{.code                      = error_code::msg_invalid_sfn,
                                           .message_id                = message_id::tx_data_request,
                                           .pdcch_discarded           = false,
                                           .pdsch_discarded           = true,
                                           .pusch_and_pucch_discarded = false},
                             // Errors specific to one request.
                             error_mapping{.code                      = error_code::msg_ul_dci_err,
                                           .message_id                = message_id::ul_dci_request,
                                           .pdcch_discarded           = true,
                                           .pdsch_discarded           = false,
                                           .pusch_and_pucch_discarded = false},
                             error_mapping{.code                      = error_code::msg_tx_err,
                                           .message_id                = message_id::tx_data_request,
                                           .pdcch_discarded           = false,
                                           .pdsch_discarded           = true,
                                           .pusch_and_pucch_discarded = false},
                             // Errors that the translator does not report as discarded results.
                             error_mapping{.code                      = error_code::msg_invalid_state,
                                           .message_id                = message_id::dl_tti_request,
                                           .pdcch_discarded           = false,
                                           .pdsch_discarded           = false,
                                           .pusch_and_pucch_discarded = false},
                             error_mapping{.code                      = error_code::msg_bch_missing,
                                           .message_id                = message_id::dl_tti_request,
                                           .pdcch_discarded           = false,
                                           .pdsch_discarded           = false,
                                           .pusch_and_pucch_discarded = false}),
                         [](const ::testing::TestParamInfo<error_mapping_param>& param_info) {
                           return fmt::format("code_{}_message_{}",
                                              static_cast<unsigned>(param_info.param.code),
                                              static_cast<unsigned>(param_info.param.message_id));
                         });

TEST_F(fapi_to_mac_error_indication_test, error_indication_without_a_slot_is_not_forwarded)
{
  translator.on_error_indication(
      build_error_indication(fapi::message_type_id::dl_tti_request, fapi::error_code_id::out_of_sync));

  EXPECT_TRUE(handler.errors.empty());
}

TEST(fapi_to_mac_error_indication_without_handler_test, error_indication_is_ignored_when_no_mac_handler_is_set)
{
  fapi_to_mac_error_indication_fastpath_translator translator(0, ocudulog::fetch_basic_logger("FAPI"));

  // The translator falls back to a dummy handler until the cell one is set.
  translator.on_error_indication(
      build_error_indication(fapi::message_type_id::dl_tti_request, fapi::error_code_id::out_of_sync, test_slot));
}
