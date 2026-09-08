// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/security_manager_config.h"

namespace ocudu::ocucp {

/// UE security manager implementation
class ue_security_manager
{
public:
  ue_security_manager(const security_manager_config& cfg_);
  ~ue_security_manager() = default;

  // up_ue_security_manager
  [[nodiscard]] bool                        is_security_context_initialized() const;
  [[nodiscard]] security::sec_as_config     get_up_as_config() const;
  [[nodiscard]] security::sec_128_as_config get_up_128_as_config() const;
  [[nodiscard]] uint8_t                     get_ncc() const;

  // ngap_ue_security_manager
  [[nodiscard]] bool init_security_context(const security::security_context& sec_ctxt);

  /// \brief Initialize the security context of a UE that accesses this node through an inter-CU handover (TS 38.413
  /// section 8.4.2, TS 38.423 section 8.2.1).
  ///
  /// The AS algorithms selected here are signalled to the UE in the HandoverCommand and the UE applies them before
  /// accessing this node, so the context is enabled without a Security Mode Command (TS 33.501 section 6.11).
  ///
  /// \param[in] sec_ctxt The security context received in the handover request.
  [[nodiscard]] bool init_handover_security_context(const security::security_context& sec_ctxt);

  /// \brief Initialize the security context from one retrieved from a peer NG-RAN node (TS 38.423 section
  /// 8.2.5).
  ///
  /// The peer already derived KgNB* for the cell the UE accessed here (TS 33.501 section 6.11), so the key is taken as
  /// it is and only the AS keys below it are generated. The UE arrives with security already active, so the context is
  /// enabled directly.
  ///
  /// \param[in] sec_ctxt The security context retrieved from the peer.
  /// \param[in] algos The AS algorithms the peer signalled in the RRC Context. When empty, they are selected from the
  /// UE capabilities the peer reported, which leaves the UE unable to decipher what this node sends if the peer had
  /// chosen differently.
  [[nodiscard]] bool init_retrieved_security_context(const security::security_context&                  sec_ctxt,
                                                     const std::optional<security::sec_selected_algos>& algos);
  [[nodiscard]] bool finalize_security_context();
  [[nodiscard]] bool is_security_enabled() const;

  // rrc_ue_security_manager
  [[nodiscard]] security::security_context get_security_context() const;

  /// \brief Returns the security context to hand to a handover target, with K_NG-RAN* already derived for the
  /// target cell (TS 33.501 Section 6.9.2.3.2).
  ///
  /// Derives on a copy, so this UE's own key stays intact. The derivation is horizontal.
  [[nodiscard]] security::security_context   get_handover_security_context(pci_t    target_pci,
                                                                           unsigned target_ssb_arfcn) const;
  [[nodiscard]] security::sec_selected_algos get_security_algos() const;
  [[nodiscard]] security::sec_as_config      get_rrc_as_config() const;
  [[nodiscard]] security::sec_128_as_config  get_rrc_128_as_config() const;
  void                                       update_security_context(const security::security_context& sec_ctxt);
  void perform_horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn);

private:
  security_manager_config    cfg;
  security::security_context sec_context;

  ocudulog::basic_logger& logger;
};

} // namespace ocudu::ocucp
