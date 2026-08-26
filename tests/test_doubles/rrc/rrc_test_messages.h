// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/asn1/rrc_nr/ul_ccch_msg.h"
#include "ocudu/asn1/rrc_nr/ul_ccch_msg_ies.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/tac.h"

namespace ocudu {
namespace test_helpers {

/// \brief Generates a dummy RRC Setup Request message.
asn1::rrc_nr::ul_ccch_msg_s create_rrc_setup_request();

/// \brief Generates a dummy RRC Reestablishment Request message.
asn1::rrc_nr::ul_ccch_msg_s create_rrc_reestablishment_request(rnti_t             old_crnti,
                                                               pci_t              old_pci,
                                                               const std::string& short_mac_i = "1100011101010100");

/// \brief Generates a dummy RRC Resume Request message.
asn1::rrc_nr::ul_ccch_msg_s
create_rrc_resume_request(uint64_t                     resume_id,
                          const std::string&           resume_mac_i = "1111010001000010",
                          asn1::rrc_nr::resume_cause_e resume_cause = asn1::rrc_nr::resume_cause_opts::mo_data);

/// \brief Generates a dummy RRC Resume Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_resume_complete(uint8_t transaction_id = 0, uint8_t sel_plmn_id = 1);

/// \brief Generates a dummy RRC Setup Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_setup_complete(uint8_t                 sel_plmn_id  = 1,
                                                      std::optional<uint64_t> ng_5g_s_tmsi = std::nullopt);

/// \brief Generates a dummy UE Capability Info message.
asn1::rrc_nr::ul_dcch_msg_s create_ue_capability_info(uint8_t transaction_id = 0, bool rrc_inactive_supported = true);

/// \brief Generates a dummy RRC Reestablishment Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_reestablishment_complete();

/// \brief Generates a dummy RRC Reconfiguration Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_reconfiguration_complete(uint8_t transaction_id = 0);

/// \brief Packs an RRC UL-CCCH message into a byte buffer.
byte_buffer pack_ul_ccch_msg(const asn1::rrc_nr::ul_ccch_msg_s& msg);

/// \brief Packs an RRC UL-DCCH message into a byte buffer.
byte_buffer pack_ul_dcch_msg(const asn1::rrc_nr::ul_dcch_msg_s& msg);

/// \brief Generates a dummy SIB1 message.
/// Creates a SIB1. A non-empty \c tac_list is broadcast in trackingAreaList instead of trackingAreaCode,
/// TS 38.331.
asn1::rrc_nr::sib1_s create_sib1(const plmn_identity& plmn     = plmn_identity::test_value(),
                                 span<const tac_t>    tac_list = {});

} // namespace test_helpers
} // namespace ocudu
