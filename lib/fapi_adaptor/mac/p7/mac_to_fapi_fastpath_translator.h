// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/precoding_codebook_mapper.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_mapper.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/scheduler/result/pdcch_info.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {

namespace fapi {
class p7_last_request_notifier;
class p7_requests_gateway;
} // namespace fapi

namespace fapi_adaptor {

/// MAC-to-FAPI fastpath translator configuration.
struct mac_to_fapi_fastpath_translator_config {
  /// Cell number of PRBs.
  unsigned cell_nof_prbs;
  /// Sector identifier.
  unsigned sector_id;
};

/// MAC-to-FAPI fastpath translator dependencies.
struct mac_to_fapi_fastpath_translator_dependencies {
  /// FAPI P7 gateway.
  fapi::p7_requests_gateway& p7_gateway;
  /// FAPI slot last message gateway.
  fapi::p7_last_request_notifier& p7_last_req_notifier;
  /// Precoding codebook mapper.
  std::unique_ptr<precoding_codebook_mapper> pm_mapper;
  /// UCI Part 2 correspondence mapper.
  std::unique_ptr<uci_part2_correspondence_mapper> part2_mapper;
  /// FAPI logger.
  ocudulog::basic_logger& fapi_logger;
};

/// \brief MAC-to-FAPI fastpath translator.
///
/// This class listens to cell-specific MAC events carrying scheduling results and translates them into FAPI messages
/// that are sent through the FAPI message gateway.
class mac_to_fapi_fastpath_translator : public mac_cell_result_notifier
{
public:
  mac_to_fapi_fastpath_translator(const mac_to_fapi_fastpath_translator_config& config,
                                  mac_to_fapi_fastpath_translator_dependencies  dependencies);

  /// Starts the MAC to FAPI translator.
  void start();

  /// Stops the MAC to FAPI translator.
  void stop();

  // See interface for documentation.
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override;

  // See interface for documentation.
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override;

  // See interface for documentation.
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override;

  // See interface for documentation.
  void on_cell_results_completion(slot_point slot) override;

private:
  /// Handles the UL_DCI.request message.
  void
  handle_ul_dci_request(span<const pdcch_ul_information> pdcch_info, span<const dci_payload> payloads, slot_point slot);

private:
  /// Cell number of resource blocks.
  const unsigned cell_nof_prbs;
  /// Sector identifier.
  const unsigned sector_id;
  /// FAPI P7 gateway.
  fapi::p7_requests_gateway& p7_gateway;
  /// P7 last requests notifier.
  fapi::p7_last_request_notifier& p7_last_req_notifier;
  /// Precoding codebook mapper.
  std::unique_ptr<precoding_codebook_mapper> pm_mapper;
  /// UCI Part2 correspondence mapper.
  std::unique_ptr<uci_part2_correspondence_mapper> part2_mapper;
  /// FAPI logger.
  ocudulog::basic_logger& fapi_logger;
  /// Stop manager.
  rt_stop_event_source stop_manager;
  /// Stop token. This token is used to track when a slot is being used. It is set on new scheduler results (downlink,
  /// uplink or downlink data) and reset on results completion.
  rt_stop_event_token stop_token;
};

} // namespace fapi_adaptor
} // namespace ocudu
