// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/rrc_nr/dl_ccch_msg.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg_ies.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/ran/rb_id.h"
#include <optional>

namespace ocudu {
namespace test_helpers {

/// \brief Check if DL-CCCH message is a valid RRC Setup message.
bool is_valid_rrc_setup(const asn1::rrc_nr::dl_ccch_msg_s& msg);
bool is_valid_rrc_setup(const byte_buffer& dl_ccch_msg);

/// \brief Check if DL-DCCH message is a valid RRC Reestablishment message.
bool is_valid_rrc_reestablishment(const asn1::rrc_nr::dl_dcch_msg_s& msg);
bool is_valid_rrc_reestablishment(const byte_buffer& dl_dcch_msg);

/// \brief Check if DL-DCCH message is a valid RRC Security Mode Command message.
bool is_valid_rrc_security_mode_command(const asn1::rrc_nr::dl_dcch_msg_s& msg);
bool is_valid_rrc_security_mode_command(const byte_buffer& dl_dcch_msg);

/// \brief Check if DL-DCCH message is a valid RRC UE Capability Enquiry message.
bool is_valid_rrc_ue_capability_enquiry(const asn1::rrc_nr::dl_dcch_msg_s& msg);
bool is_valid_rrc_ue_capability_enquiry(const byte_buffer& dl_dcch_msg);

/// \brief Check if DL-DCCH message is a valid RRC Reconfiguration message.
bool is_valid_rrc_reconfiguration(const asn1::rrc_nr::dl_dcch_msg_s&          msg,
                                  bool                                        contains_nas_pdu,
                                  const std::optional<std::vector<srb_id_t>>& expected_srbs_to_add_mod,
                                  const std::optional<std::vector<drb_id_t>>& expected_drbs_to_add_mod,
                                  const std::optional<std::vector<drb_id_t>>& expected_drbs_to_release,
                                  std::optional<uint8_t>                      serving_cell_mo);
bool is_valid_rrc_reconfiguration(const byte_buffer&                          dl_dcch_msg,
                                  bool                                        contains_nas_pdu         = true,
                                  const std::optional<std::vector<srb_id_t>>& expected_srbs_to_add_mod = std::nullopt,
                                  const std::optional<std::vector<drb_id_t>>& expected_drbs_to_add_mod = std::nullopt,
                                  const std::optional<std::vector<drb_id_t>>& expected_drbs_to_release = std::nullopt,
                                  std::optional<uint8_t>                      serving_cell_mo          = std::nullopt);

bool is_valid_rrc_handover_preparation_info(const asn1::rrc_nr::ho_prep_info_s& ho_prep_info);
bool is_valid_rrc_handover_preparation_info(const byte_buffer& ho_prep_info);

bool is_valid_rrc_release_with_suspend(const asn1::rrc_nr::dl_dcch_msg_s& msg);
bool is_valid_rrc_release_with_suspend(const byte_buffer& dl_dcch_msg);

bool is_valid_rrc_resume(const asn1::rrc_nr::dl_dcch_msg_s& msg);
bool is_valid_rrc_resume(const byte_buffer& dl_dcch_msg);

bool is_valid_sib1(const byte_buffer& packed_sib1);

bool is_valid_pdcp_security(const byte_buffer& dl_dcch_msg,
                            drb_id_t           drb_id,
                            bool               integrity_enabled,
                            bool               ciphering_enabled);

} // namespace test_helpers
} // namespace ocudu
