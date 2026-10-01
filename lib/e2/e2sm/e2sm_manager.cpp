// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/e2/e2sm/e2sm_manager.h"
#include "ocudu/e2/e2sm/e2sm.h"

using namespace asn1::e2ap;
using namespace ocudu;

e2sm_manager::e2sm_manager(ocudulog::basic_logger& logger_) : logger(logger_) {}

void e2sm_manager::add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface)
{
  e2sm_iface_list.emplace(oid, std::move(e2sm_iface));
}

e2sm_interface* e2sm_manager::get_e2sm_interface(std::string oid)
{
  auto it = e2sm_iface_list.find(oid);
  if (it == e2sm_iface_list.end()) {
    logger.error("OID not supported");
    return nullptr;
  }
  return &(*(it->second));
}

e2sm_interface* e2sm_manager::get_e2sm_interface(uint16_t ran_func_id)
{
  auto it = supported_ran_functions.find(ran_func_id);
  if (it == supported_ran_functions.end()) {
    logger.error("RAN function ID not supported");
    return nullptr;
  }
  return get_e2sm_interface(it->second);
}

void e2sm_manager::add_supported_ran_function(uint16_t ran_func_id, std::string oid)
{
  if (e2sm_iface_list.count(oid)) {
    supported_ran_functions.emplace(ran_func_id, oid);
  } else {
    logger.error("OID/RAN function ID not supported");
  }
}
