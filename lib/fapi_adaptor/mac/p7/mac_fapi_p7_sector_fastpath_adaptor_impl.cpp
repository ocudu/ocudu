// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_fapi_p7_sector_fastpath_adaptor_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;
using namespace fapi_adaptor;

static mac_to_fapi_fastpath_translator_config
generate_translator_config(const mac_fapi_p7_sector_fastpath_adaptor_config& config)
{
  return {.cell_nof_prbs = config.cell_nof_prbs, .sector_id = config.sector_id};
}

static mac_to_fapi_fastpath_translator_dependencies
generate_translator_dependencies(mac_fapi_p7_sector_fastpath_adaptor_dependencies& dependencies)
{
  return {.p7_gateway           = dependencies.p7_gateway,
          .p7_last_req_notifier = dependencies.p7_last_req_notifier,
          .pm_mapper            = std::move(dependencies.pm_mapper),
          .part2_mapper         = std::move(dependencies.part2_mapper),
          .fapi_logger          = dependencies.fapi_logger};
}

mac_fapi_p7_sector_fastpath_adaptor_impl::mac_fapi_p7_sector_fastpath_adaptor_impl(
    const mac_fapi_p7_sector_fastpath_adaptor_config&     config,
    mac_fapi_p7_sector_fastpath_adaptor_impl_dependencies dependencies) :
  fapi_data_translator(config.sector_id, dependencies.base_dependencies.fapi_logger),
  fapi_time_translator(config.sector_id, dependencies.slot_handler, dependencies.base_dependencies.fapi_logger),
  fapi_error_translator(config.sector_id, dependencies.base_dependencies.fapi_logger),
  mac_translator(generate_translator_config(config), generate_translator_dependencies(dependencies.base_dependencies))
{
}

fapi::p7_slot_indication_notifier& mac_fapi_p7_sector_fastpath_adaptor_impl::get_p7_slot_indication_notifier()
{
  return fapi_time_translator;
}

fapi::error_indication_notifier& mac_fapi_p7_sector_fastpath_adaptor_impl::get_error_indication_notifier()
{
  return fapi_error_translator;
}

fapi::p7_indications_notifier& mac_fapi_p7_sector_fastpath_adaptor_impl::get_p7_indications_notifier()
{
  return fapi_data_translator;
}

mac_cell_result_notifier& mac_fapi_p7_sector_fastpath_adaptor_impl::get_cell_result_notifier()
{
  return mac_translator;
}

void mac_fapi_p7_sector_fastpath_adaptor_impl::set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler)
{
  fapi_time_translator.set_cell_slot_handler(mac_slot_handler);
  fapi_error_translator.set_cell_slot_handler(mac_slot_handler);
}

void mac_fapi_p7_sector_fastpath_adaptor_impl::set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler)
{
  fapi_data_translator.set_cell_rach_handler(mac_rach_handler);
}

void mac_fapi_p7_sector_fastpath_adaptor_impl::set_cell_pdu_handler(mac_pdu_handler& handler)
{
  fapi_data_translator.set_cell_pdu_handler(handler);
}

void mac_fapi_p7_sector_fastpath_adaptor_impl::set_cell_crc_handler(mac_cell_control_information_handler& handler)
{
  fapi_data_translator.set_cell_crc_handler(handler);
}
