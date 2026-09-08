// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_security_manager_impl.h"

using namespace ocudu;
using namespace ocucp;

ue_security_manager::ue_security_manager(const security_manager_config& cfg_) :
  cfg(cfg_), logger(ocudulog::fetch_basic_logger("CU-CP"))
{
}

// up_ue_security_manager
bool ue_security_manager::is_security_context_initialized() const
{
  return sec_context.sel_algos.algos_selected;
}

security::sec_as_config ue_security_manager::get_up_as_config() const
{
  return sec_context.get_as_config(security::sec_domain::up);
}

security::sec_128_as_config ue_security_manager::get_up_128_as_config() const
{
  return sec_context.get_128_as_config(security::sec_domain::up);
}

uint8_t ue_security_manager::get_ncc() const
{
  return sec_context.ncc;
}

// ngap_ue_security_manager
bool ue_security_manager::init_security_context(const security::security_context& sec_ctxt)
{
  // Update the security context
  sec_context = sec_ctxt;

  // Select security algorithms
  logger.debug("Integrity protection algorithms preference list: {}", cfg.int_algo_pref_list);
  logger.debug("Ciphering algorithms preference list: {}", cfg.enc_algo_pref_list);
  logger.debug("Integrity protection algorithms supported list: {}", sec_context.supported_int_algos);
  logger.debug("Ciphering algorithms preference list: {}", sec_context.supported_enc_algos);
  if (not sec_context.select_algorithms(cfg.int_algo_pref_list, cfg.enc_algo_pref_list)) {
    logger.error("Could not select security algorithm");
    return false;
  }
  logger.debug("Selected security algorithms integrity=NIA{} ciphering=NEA{}",
               fmt::underlying(sec_context.sel_algos.integ_algo),
               fmt::underlying(sec_context.sel_algos.cipher_algo));

  // Generate K_rrc_enc and K_rrc_int
  sec_context.generate_as_keys();

  // Set security state to partially enabled. It will be set to fully enabled when the UE confirms the security
  // configuration.
  sec_context.state = security::security_state::partially_enabled;

  return true;
}

bool ue_security_manager::init_handover_security_context(const security::security_context& sec_ctxt)
{
  if (not init_security_context(sec_ctxt)) {
    return false;
  }

  return finalize_security_context();
}

bool ue_security_manager::init_retrieved_security_context(const security::security_context&                  sec_ctxt,
                                                          const std::optional<security::sec_selected_algos>& algos)
{
  sec_context = sec_ctxt;

  // The UE keeps ciphering and integrity protecting with the algorithms it was given, so the ones the peer signalled
  // are taken over. Without them, they are selected from the UE capabilities the peer reported.
  if (algos.has_value()) {
    sec_context.sel_algos = algos.value();
  } else if (not sec_context.select_algorithms(cfg.int_algo_pref_list, cfg.enc_algo_pref_list)) {
    logger.error("Could not select security algorithm for the retrieved security context");
    return false;
  }
  logger.debug("Selected security algorithms integrity=NIA{} ciphering=NEA{}",
               fmt::underlying(sec_context.sel_algos.integ_algo),
               fmt::underlying(sec_context.sel_algos.cipher_algo));

  // Derive the AS keys from the KgNB* the peer transferred.
  sec_context.generate_as_keys();

  sec_context.state = security::security_state::fully_enabled;

  return true;
}

bool ue_security_manager::finalize_security_context()
{
  if (sec_context.state != security::security_state::partially_enabled) {
    logger.error("Attempted to finalize security context, but it is not in partially enabled state");
    return false;
  }

  sec_context.state = security::security_state::fully_enabled;
  return true;
}

bool ue_security_manager::is_security_enabled() const
{
  return sec_context.state == security::security_state::fully_enabled;
}

// rrc_ue_security_manager

security::security_context ue_security_manager::get_security_context() const
{
  return sec_context;
}

security::security_context ue_security_manager::get_handover_security_context(pci_t    target_pci,
                                                                              unsigned target_ssb_arfcn) const
{
  security::security_context target_sec_context = sec_context;
  target_sec_context.horizontal_key_derivation(target_pci, target_ssb_arfcn);
  return target_sec_context;
}

security::sec_selected_algos ue_security_manager::get_security_algos() const
{
  return sec_context.sel_algos;
}

security::sec_as_config ue_security_manager::get_rrc_as_config() const
{
  return sec_context.get_as_config(security::sec_domain::rrc);
}

security::sec_128_as_config ue_security_manager::get_rrc_128_as_config() const
{
  return sec_context.get_128_as_config(security::sec_domain::rrc);
}

void ue_security_manager::update_security_context(const security::security_context& sec_ctxt)
{
  sec_context = sec_ctxt;
}

void ue_security_manager::perform_horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn)
{
  sec_context.horizontal_key_derivation(target_pci, target_ssb_arfcn);
}
