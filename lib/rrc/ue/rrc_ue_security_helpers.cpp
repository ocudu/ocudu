// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_security_helpers.h"
#include "ocudu/asn1/rrc_nr/nr_ue_variables.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/security/integrity.h"

using namespace ocudu;
using namespace ocucp;

namespace {

/// Verifies a MAC-I computed over one of the VarShortMAC-Input / VarResumeMAC-Input variables, which carry the same
/// three fields and differ only in the ASN.1 type they pack as.
template <typename VarMacInput>
bool verify_mac_i(const char*                       mac_i_type,
                  const security::sec_short_mac_i&  mac_i,
                  pci_t                             source_pci,
                  rnti_t                            source_c_rnti,
                  nr_cell_identity                  target_nci,
                  const security::security_context& sec_context,
                  rrc_ue_logger&                    logger)
{
  if (!sec_context.sel_algos.algos_selected) {
    logger.log_warning("Cannot verify {}. Cause: no security algorithms selected", mac_i_type);
    return false;
  }

  VarMacInput var_mac_input = {};
  var_mac_input.source_pci  = source_pci;
  var_mac_input.target_cell_id.from_number(target_nci.value());
  var_mac_input.source_c_rnti = to_underlying(source_c_rnti);

  byte_buffer   var_mac_input_packed = {};
  asn1::bit_ref bref(var_mac_input_packed);
  if (var_mac_input.pack(bref) != asn1::OCUDUASN_SUCCESS) {
    logger.log_warning("Cannot verify {}. Cause: failed to pack the MAC-I input variable", mac_i_type);
    return false;
  }

  logger.log_debug(var_mac_input_packed.begin(),
                   var_mac_input_packed.end(),
                   "Packed {} input. source_pci={} target_cell_id={} source_c_rnti={}",
                   mac_i_type,
                   source_pci,
                   target_nci,
                   source_c_rnti);

  const security::sec_as_config source_as_config = sec_context.get_as_config(security::sec_domain::rrc);
  const bool                    valid = security::verify_short_mac(mac_i, var_mac_input_packed, source_as_config);
  logger.log_debug("Verified {}. valid={}", mac_i_type, valid);

  return valid;
}

} // namespace

bool ocudu::ocucp::verify_short_mac_i(const security::sec_short_mac_i&  short_mac_i,
                                      pci_t                             source_pci,
                                      rnti_t                            source_c_rnti,
                                      nr_cell_identity                  target_nci,
                                      const security::security_context& sec_context,
                                      rrc_ue_logger&                    logger)
{
  return verify_mac_i<asn1::rrc_nr::var_short_mac_input_s>(
      "ShortMAC-I", short_mac_i, source_pci, source_c_rnti, target_nci, sec_context, logger);
}

bool ocudu::ocucp::verify_resume_mac_i(const security::sec_short_mac_i&  resume_mac_i,
                                       pci_t                             source_pci,
                                       rnti_t                            source_c_rnti,
                                       nr_cell_identity                  target_nci,
                                       const security::security_context& sec_context,
                                       rrc_ue_logger&                    logger)
{
  return verify_mac_i<asn1::rrc_nr::var_resume_mac_input_s>(
      "ResumeMAC-I", resume_mac_i, source_pci, source_c_rnti, target_nci, sec_context, logger);
}

std::optional<security::sec_selected_algos> ocudu::ocucp::get_as_security_algorithms(const byte_buffer& rrc_context,
                                                                                     rrc_ue_logger&     logger)
{
  if (rrc_context.empty()) {
    return std::nullopt;
  }

  asn1::rrc_nr::ho_prep_info_s ho_prep_info;
  asn1::cbit_ref               bref({rrc_context.begin(), rrc_context.end()});
  if (ho_prep_info.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
    logger.log_warning("Couldn't unpack the HandoverPreparationInformation to read the AS security algorithms");
    return std::nullopt;
  }
  if (ho_prep_info.crit_exts.type() != asn1::rrc_nr::ho_prep_info_s::crit_exts_c_::types::c1 or
      ho_prep_info.crit_exts.c1().type() != asn1::rrc_nr::ho_prep_info_s::crit_exts_c_::c1_c_::types::ho_prep_info) {
    logger.log_warning("Unsupported HandoverPreparationInformation critical extension");
    return std::nullopt;
  }

  const auto& ies = ho_prep_info.crit_exts.c1().ho_prep_info();
  if (!ies.source_cfg_present) {
    logger.log_debug("No AS-Config in the HandoverPreparationInformation");
    return std::nullopt;
  }

  asn1::rrc_nr::rrc_recfg_s recfg;
  asn1::cbit_ref            recfg_bref({ies.source_cfg.rrc_recfg.begin(), ies.source_cfg.rrc_recfg.end()});
  if (recfg.unpack(recfg_bref) != asn1::OCUDUASN_SUCCESS) {
    logger.log_warning("Couldn't unpack the RRCReconfiguration in the AS-Config");
    return std::nullopt;
  }
  if (recfg.crit_exts.type() != asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types::rrc_recfg) {
    logger.log_warning("Unsupported RRCReconfiguration critical extension in the AS-Config");
    return std::nullopt;
  }

  const auto& recfg_ies = recfg.crit_exts.rrc_recfg();
  if (!recfg_ies.radio_bearer_cfg_present or !recfg_ies.radio_bearer_cfg.security_cfg_present or
      !recfg_ies.radio_bearer_cfg.security_cfg.security_algorithm_cfg_present) {
    logger.log_debug("No security algorithm configuration in the AS-Config");
    return std::nullopt;
  }

  const auto& asn1_algos = recfg_ies.radio_bearer_cfg.security_cfg.security_algorithm_cfg;
  if (asn1_algos.ciphering_algorithm.value > asn1::rrc_nr::ciphering_algorithm_opts::nea3 or
      !asn1_algos.integrity_prot_algorithm_present or
      asn1_algos.integrity_prot_algorithm.value > asn1::rrc_nr::integrity_prot_algorithm_opts::nia3) {
    logger.log_warning("Unsupported security algorithms in the AS-Config");
    return std::nullopt;
  }

  security::sec_selected_algos algos;
  algos.algos_selected = true;
  algos.cipher_algo    = security::ciphering_algorithm_from_number(asn1_algos.ciphering_algorithm.to_number());
  algos.integ_algo     = security::integrity_algorithm_from_number(asn1_algos.integrity_prot_algorithm.to_number());

  return algos;
}
