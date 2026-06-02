// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/expected.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/rrc/rrc_resume.h"
#include "ocudu/rrc/rrc_types.h"

namespace ocudu::ocucp {

// Helper to create PDU from RRC message.
template <class T>
byte_buffer pack_into_pdu(const T& msg, const char* context_name)
{
  context_name = context_name == nullptr ? __FUNCTION__ : context_name;
  byte_buffer   pdu{};
  asn1::bit_ref bref{pdu};
  if (msg.pack(bref) == asn1::OCUDUASN_ERROR_ENCODE_FAIL) {
    ocudulog::fetch_basic_logger("RRC").error("Failed to pack message in {}. Discarding it.", context_name);
  }
  return pdu;
}

/// \brief Fills ASN.1 RRC Setup struct.
/// \param[out] rrc_setup The RRC Setup ASN.1 struct to fill.
/// \param[in] init_ul_rrc_transfer_msg The Init_UL_RRC_Transfer message received by the CU.
/// \return True on success, otherwise false.
bool fill_asn1_rrc_setup_msg(asn1::rrc_nr::rrc_setup_s& rrc_setup, const byte_buffer& mcg, uint8_t rrc_transaction_id);

/// Extracts transaction id of RRC Setup complete message.
expected<uint8_t> get_transaction_id(const asn1::rrc_nr::rrc_setup_complete_s& msg);

/// Extracts transaction id of UL-DCCH message.
expected<uint8_t> get_transaction_id(const asn1::rrc_nr::ul_dcch_msg_s& msg);

/// \brief Fills ASN.1 RRC Security Mode Command struct.
/// \param[out] rrc_smc The RRC security mode command ASN.1 struct to fill.
/// \param[in] int_algo The selected integrity protection algorithm.
/// \param[in] ciph_algo The selected ciphering algorithm.
/// \param[in] rrc_transaction_id The RRC transaction id.
void fill_asn1_rrc_smc_msg(asn1::rrc_nr::security_mode_cmd_s&   rrc_smc,
                           const security::integrity_algorithm& int_algo,
                           const security::ciphering_algorithm& ciph_algo,
                           uint8_t                              rrc_transaction_id);

/// \brief Fills ASN.1 RRC Setup struct.
/// \param[out] asn1_rrc_reconf The RRC Reconfiguration ASN.1 struct to fill.
/// \param[in] rrc_transaction_id The RRC transaction id.
/// \param[in] rrc_reconf The common type struct.
void fill_asn1_rrc_reconfiguration_msg(asn1::rrc_nr::rrc_recfg_s&                   asn1_rrc_reconf,
                                       uint8_t                                      rrc_transaction_id,
                                       const rrc_reconfiguration_procedure_request& rrc_reconf);

/// \brief Fills ASN.1 RRC Resume struct.
/// \param[out] asn1_rrc_resume The RRC Resume ASN.1 struct to fill.
/// \param[in] rrc_transaction_id The RRC transaction id.
/// \param[in] rrc_resume The common type struct.
void fill_asn1_rrc_resume_msg(asn1::rrc_nr::rrc_resume_s&        asn1_rrc_resume,
                              uint8_t                            rrc_transaction_id,
                              const rrc_resume_request_response& rrc_resume);

} // namespace ocudu::ocucp
