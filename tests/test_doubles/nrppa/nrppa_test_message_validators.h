// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace asn1 {
namespace nrppa {
struct nr_ppa_pdu_c;
} // namespace nrppa
} // namespace asn1

namespace ocudu::test_helpers {

bool is_valid_e_cid_meas_initiation_failure(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_e_cid_meas_initiation_response(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_e_cid_meas_report(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_nrppa_trp_information_response(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_nrppa_trp_information_failure(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_nrppa_positioning_information_response(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_nrppa_positioning_information_failure(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_nrppa_positioning_activation_response(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_nrppa_positioning_activation_failure(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_nrppa_measurement_response(const asn1::nrppa::nr_ppa_pdu_c& msg);

bool is_valid_nrppa_measurement_failure(const asn1::nrppa::nr_ppa_pdu_c& msg);

} // namespace ocudu::test_helpers
