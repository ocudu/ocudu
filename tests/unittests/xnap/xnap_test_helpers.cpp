// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_test_helpers.h"
#include "lib/xnap/procedures/xn_setup_procedure_asn1_helpers.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/support/async/async_test_utils.h"

using namespace ocudu;
using namespace ocucp;

xnap_test::xnap_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &ctrl_worker;
    return cucfg;
  }())
{
  // Init test's loggers.
  ocudulog::init();
  logger.set_level(ocudulog::basic_levels::debug);

  ocudulog::fetch_basic_logger("XNAP", false).set_level(ocudulog::basic_levels::debug);
  ocudulog::fetch_basic_logger("XNAP", false).set_hex_dump_max_size(100);

  xnap = std::make_unique<xnap_impl>(xnc_peer_index_t::min, xnap_local_cfg, cu_cp_notifier, timers, ctrl_worker);

  xnap->set_tx_association_notifier(std::make_unique<dummy_xnap_message_notifier>(last_tx_msg));
}

void xnap_test::TearDown()
{
  // Flush logger after each test.
  ocudulog::flush();
}

bool xnap_test::run_xn_setup(const xnap_configuration& peer_cfg)
{
  // Action 1: Launch XN setup procedure
  logger.info("Launch xn setup request procedure...");
  async_task<bool>         t = xnap->handle_xn_setup_request_required();
  lazy_task_launcher<bool> t_launcher(t);

  // Action 2: Send XN setup response from peer.
  xnap_message setup_resp = generate_asn1_xn_setup_response(peer_cfg);
  xnap->handle_message(setup_resp);

  // Check procedure completion.
  if (!t.get()) {
    logger.error("XN Setup procedure failed");
    return false;
  }

  return true;
}

cu_cp_ue_index_t xnap_test::create_ue(rnti_t rnti)
{
  // Create UE in UE manager
  cu_cp_ue_index_t ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
  if (ue_index == cu_cp_ue_index_t::invalid) {
    return cu_cp_ue_index_t::invalid;
  }
  if (ue_mng.ue_admission_limit_reached()) {
    ue_mng.remove_ue(ue_index);
    return cu_cp_ue_index_t::invalid;
  }

  if (not ue_mng.update_ue_context(ue_index, int_to_gnb_du_id(0), MIN_PCI, rnti, cu_cp_du_cell_index_t::min)) {
    logger.error("Failed to create UE with pci={} rnti={} pcell_index={}",
                 MIN_PCI,
                 rnti_t::MIN_CRNTI,
                 cu_cp_du_cell_index_t::min);
    return cu_cp_ue_index_t::invalid;
  }
  if (!ue_mng.set_plmn(ue_index, plmn_identity::test_value())) {
    logger.error("ue={}: Failed to set PLMN", ue_index);
    ue_mng.remove_ue(ue_index);
    return cu_cp_ue_index_t::invalid;
  }

  return ue_index;
}
