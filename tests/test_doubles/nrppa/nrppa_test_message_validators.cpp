// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "nrppa_test_message_validators.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/nrppa/common.h"
#include "ocudu/asn1/nrppa/nrppa.h"

using namespace ocudu;

#define TRUE_OR_RETURN(cond)                                                                                           \
  if (not(cond))                                                                                                       \
    return false;

bool ocudu::test_helpers::is_valid_e_cid_meas_initiation_failure(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::unsuccessful_outcome);
  TRUE_OR_RETURN(msg.unsuccessful_outcome().proc_code == ASN1_NRPPA_ID_E_C_ID_MEAS_INITIATION);
  return true;
}

bool ocudu::test_helpers::is_valid_e_cid_meas_initiation_response(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.successful_outcome().proc_code == ASN1_NRPPA_ID_E_C_ID_MEAS_INITIATION);
  return true;
}

bool ocudu::test_helpers::is_valid_e_cid_meas_report(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.init_msg().proc_code == ASN1_NRPPA_ID_E_C_ID_MEAS_REPORT);
  return true;
}

bool ocudu::test_helpers::is_valid_nrppa_trp_information_response(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.successful_outcome().proc_code == ASN1_NRPPA_ID_T_RP_INFO_EXCHANGE);
  return true;
}

bool ocudu::test_helpers::is_valid_nrppa_trp_information_failure(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::unsuccessful_outcome);
  TRUE_OR_RETURN(msg.unsuccessful_outcome().proc_code == ASN1_NRPPA_ID_T_RP_INFO_EXCHANGE);
  return true;
}

bool ocudu::test_helpers::is_valid_nrppa_positioning_information_response(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.successful_outcome().proc_code == ASN1_NRPPA_ID_POSITIONING_INFO_EXCHANGE);
  return true;
}

bool ocudu::test_helpers::is_valid_nrppa_positioning_information_failure(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::unsuccessful_outcome);
  TRUE_OR_RETURN(msg.unsuccessful_outcome().proc_code == ASN1_NRPPA_ID_POSITIONING_INFO_EXCHANGE);
  return true;
}

bool ocudu::test_helpers::is_valid_nrppa_positioning_activation_response(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.successful_outcome().proc_code == ASN1_NRPPA_ID_POSITIONING_ACTIVATION);
  return true;
}

bool ocudu::test_helpers::is_valid_nrppa_positioning_activation_failure(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::unsuccessful_outcome);
  TRUE_OR_RETURN(msg.unsuccessful_outcome().proc_code == ASN1_NRPPA_ID_POSITIONING_ACTIVATION);
  return true;
}

bool ocudu::test_helpers::is_valid_nrppa_measurement_response(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.successful_outcome().proc_code == ASN1_NRPPA_ID_MEAS);
  return true;
}

bool ocudu::test_helpers::is_valid_nrppa_measurement_failure(const asn1::nrppa::nr_ppa_pdu_c& msg)
{
  TRUE_OR_RETURN(msg.type() == asn1::nrppa::nr_ppa_pdu_c::types_opts::unsuccessful_outcome);
  TRUE_OR_RETURN(msg.unsuccessful_outcome().proc_code == ASN1_NRPPA_ID_MEAS);
  return true;
}
