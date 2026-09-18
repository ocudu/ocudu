// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "phy_fapi_p7_sector_fastpath_adaptor_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/p7_last_request_notifier.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// Generates and returns a FAPI-to-PHY translator configuration from the given PHY adaptor configuration.
static fapi_to_phy_fastpath_translator_config
generate_fapi_to_phy_translator_config(const phy_fapi_p7_sector_fastpath_adaptor_config& config)
{
  return {.sector_id                     = config.sector_id,
          .nof_slots_request_headroom    = config.nof_slots_request_headroom,
          .allow_request_on_empty_ul_tti = config.allow_request_on_empty_ul_tti,
          .scs_common                    = config.scs_common,
          .prach_cfg                     = config.prach_cfg,
          .carrier_cfg                   = config.carrier_cfg,
          .prach_ports                   = config.prach_ports,
          .ntn_k_mac_slots               = config.ntn_k_mac_slots};
}

/// Generates and returns a FAPI-to-PHY translator dependencies from the given PHY adaptor dependencies.
static fapi_to_phy_fastpath_translator_dependencies
generate_fapi_to_phy_translator_dependencies(phy_fapi_p7_sector_fastpath_adaptor_dependencies dependencies)
{
  return {.logger               = dependencies.logger,
          .dl_processor_pool    = dependencies.dl_processor_pool,
          .dl_rg_pool           = dependencies.dl_rg_pool,
          .dl_pdu_validator     = dependencies.dl_pdu_validator,
          .ul_request_processor = dependencies.ul_request_processor,
          .ul_pdu_repository    = dependencies.ul_pdu_repository,
          .ul_pdu_validator     = dependencies.ul_pdu_validator,
          .pm_repo              = std::move(dependencies.pm_repo),
          .part2_repo           = std::move(dependencies.part2_repo)};
}

phy_fapi_p7_sector_fastpath_adaptor_impl::phy_fapi_p7_sector_fastpath_adaptor_impl(
    const phy_fapi_p7_sector_fastpath_adaptor_config& config,
    phy_fapi_p7_sector_fastpath_adaptor_dependencies  dependencies) :
  results_translator(phy_to_fapi_results_event_fastpath_translator_config{.sector_id = config.sector_id,
                                                                          .dbfs_to_dbm_conversion_factor =
                                                                              config.dbfs_to_dbm_conversion_factor,
                                                                          .db_to_dbfs_conversion_factor =
                                                                              config.db_to_dbfs_conversion_factor,
                                                                          .msg1_scs        = config.prach_cfg.msg1_scs,
                                                                          .ntn_k_mac_slots = config.ntn_k_mac_slots},
                     phy_to_fapi_results_event_fastpath_translator_dependencies{.logger = dependencies.logger}),
  fapi_translator(generate_fapi_to_phy_translator_config(config),
                  generate_fapi_to_phy_translator_dependencies(std::move(dependencies))),
  time_translator(fapi_translator)
{
}

upper_phy_timing_notifier& phy_fapi_p7_sector_fastpath_adaptor_impl::get_timing_notifier()
{
  return time_translator;
}

void phy_fapi_p7_sector_fastpath_adaptor_impl::set_p7_slot_indication_notifier(
    fapi::p7_slot_indication_notifier& notifier)
{
  time_translator.set_p7_slot_indication_notifier(notifier);
}

void phy_fapi_p7_sector_fastpath_adaptor_impl::set_error_indication_notifier(
    fapi::error_indication_notifier& fapi_error_notifier)
{
  fapi_translator.set_error_indication_notifier(fapi_error_notifier);
  error_translator.set_error_indication_notifier(fapi_error_notifier);
}

void phy_fapi_p7_sector_fastpath_adaptor_impl::set_p7_indications_notifier(fapi::p7_indications_notifier& fapi_notifier)
{
  results_translator.set_p7_indications_notifier(fapi_notifier);
}

fapi::p7_last_request_notifier& phy_fapi_p7_sector_fastpath_adaptor_impl::get_p7_last_request_notifier()
{
  return fapi_translator;
}

fapi::p7_requests_gateway& phy_fapi_p7_sector_fastpath_adaptor_impl::get_p7_requests_gateway()
{
  return fapi_translator;
}

upper_phy_rx_results_notifier& phy_fapi_p7_sector_fastpath_adaptor_impl::get_rx_results_notifier()
{
  return results_translator;
}

upper_phy_error_notifier& phy_fapi_p7_sector_fastpath_adaptor_impl::get_error_notifier()
{
  return error_translator;
}
