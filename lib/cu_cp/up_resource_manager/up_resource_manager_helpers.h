// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "up_resource_manager_impl.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/ran/cu_cp_pdu_session.h"

namespace ocudu::ocucp {

/// \brief Perform sanity check on incoming resource setup/modification/release requests.
bool is_valid(const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items,
              const up_context&                                                            context,
              const up_resource_manager_cfg&                                               cfg,
              const ocudulog::basic_logger&                                                logger);
bool is_valid(const ngap_pdu_session_resource_modify_request& pdu,
              const up_context&                               context,
              const up_resource_manager_cfg&                  cfg,
              const ocudulog::basic_logger&                   logger);
bool is_valid(const ngap_pdu_session_resource_release_command& pdu,
              const up_context&                                context,
              const up_resource_manager_cfg&                   cfg,
              const ocudulog::basic_logger&                    logger);

/// \brief Validates that a given FiveQI has a valid PDCP and SDAP config.
bool is_valid(five_qi_t five_qi, const up_resource_manager_cfg& cfg, const ocudulog::basic_logger& logger);

/// @brief Perform sanity check on requested QoS flow configuration.
five_qi_t get_five_qi(const ngap_qos_flow_add_or_mod_item& qos_flow,
                      const up_resource_manager_cfg&       cfg,
                      const ocudulog::basic_logger&        logger);

/// \brief Functions to calculate a configuration update based on the active config and an incoming PDU session
/// setup/modfication/release request/command. No configuration state is altered.
/// \param pdu The PDU session resource setup/modification request or release command.
/// \param  contest The currently active UP resource allocation.
/// \return The config update struct
up_config_update
calculate_update(const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items,
                 const up_context&                                                            context,
                 const up_resource_manager_cfg&                                               cfg,
                 const ocudulog::basic_logger&                                                logger);
up_config_update calculate_update(const ngap_pdu_session_resource_modify_request& pdu,
                                  const up_context&                               context,
                                  const up_resource_manager_cfg&                  cfg,
                                  const ocudulog::basic_logger&                   logger);
up_config_update calculate_update(const ngap_pdu_session_resource_release_command& pdu,
                                  const up_context&                                context,
                                  const up_resource_manager_cfg&                   cfg,
                                  const ocudulog::basic_logger&                    logger);

// \brief Allocates a new DRB ID and returns it.
drb_id_t allocate_drb_id(const up_pdu_session_context_update& new_session_context,
                         const up_context&                    context,
                         const up_config_update&              config_update,
                         uint8_t                              max_nof_drbs_per_ue,
                         const ocudulog::basic_logger&        logger);

// \brief Returns valid RRC PDCP config for a given FiveQI
pdcp_config   set_rrc_pdcp_config(five_qi_t                           five_qi,
                                  const up_resource_manager_cfg&      cfg,
                                  integrity_protection_result_t       integrity,
                                  confidentiality_protection_result_t ciphering);
sdap_config_t set_rrc_sdap_config(const up_drb_context& context);

unsigned get_used_drb_index(drb_id_t drb_id);

} // namespace ocudu::ocucp
