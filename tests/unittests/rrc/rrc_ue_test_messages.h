// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "tests/test_doubles/security/security_test_keys.h"
#include "ocudu/asn1/rrc_nr/ul_ccch_msg_ies.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/rrc/rrc_types.h"

namespace ocudu {
namespace ocucp {

using test_helpers::make_sec_128_key;
using test_helpers::make_sec_key;

/// \brief Generates a dummy meas config
rrc_meas_cfg generate_dummy_meas_config();

/// \brief Constructs full RRC Reconfig request with radioBearerConfig, measConfig, masterCellGroup and NAS PDU
rrc_reconfiguration_procedure_request generate_rrc_reconfiguration_procedure_request();

/// \brief Generate RRC Container with invalid RRC Reestablishment Request.
byte_buffer generate_invalid_rrc_reestablishment_request_pdu(pci_t pci, rnti_t c_rnti);

/// \brief Generate RRC Container with valid RRC Reestablishment Request.
byte_buffer generate_valid_rrc_reestablishment_request_pdu(
    pci_t                       pci,
    rnti_t                      c_rnti,
    std::string                 short_mac_i = "0111011100001000",
    asn1::rrc_nr::reest_cause_e cause       = asn1::rrc_nr::reest_cause_opts::options::other_fail);

/// \brief Generate RRC Container with RRC Resume Request.
byte_buffer
generate_rrc_resume_request_pdu(std::string                  resume_id    = "000000000000000000000001",
                                std::string                  resume_mac_i = "0111011100001000",
                                asn1::rrc_nr::resume_cause_e cause = asn1::rrc_nr::resume_cause_opts::options::mo_sig);

/// \brief Generate RRC Container with RRC Reestablishment Complete.
byte_buffer generate_rrc_reestablishment_complete_pdu();

/// \brief Generate RRC Container with Measurement Report
byte_buffer generate_measurement_report_pdu();

} // namespace ocucp
} // namespace ocudu
