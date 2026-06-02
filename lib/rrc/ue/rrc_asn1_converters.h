// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg_ies.h"
#include "ocudu/asn1/rrc_nr/radio_bearer_cfg.h"
#include "ocudu/asn1/rrc_nr/serving_cell.h"
#include "ocudu/asn1/rrc_nr/ul_ccch_msg_ies.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/ran/cause/common.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/five_g_s_tmsi.h"
#include "ocudu/rrc/rrc_setup.h"
#include "ocudu/rrc/rrc_types.h"
#include "ocudu/security/security.h"

namespace ocudu::ocucp {

/// \brief Converts type \c pdcp_discard_timer to an RRC NR ASN.1 type.
/// \param[in] discard_timer discard timer object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::pdcp_cfg_s::drb_s_::discard_timer_e_ discard_timer_to_asn1(pdcp_discard_timer discard_timer);

/// \brief Converts type \c pdcp_t_reordering to an RRC NR ASN.1 type.
/// \param[in] t_reordering t-reordering object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::pdcp_cfg_s::t_reordering_e_ t_reordering_to_asn1(pdcp_t_reordering t_reordering);

/// \brief Converts type \c pdcp_config_t to an RRC NR ASN.1 type.
/// \param[in] pdcp_cfg pdcp config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
///
/// Conditional presence | Explanation
/// DRB                  | This field is mandatory present when the corresponding DRB is being set up, absent for SRBs.
///                      | Otherwise this field is optionally present, need M.
/// MoreThanOneRLC       | This field is mandatory present upon RRC reconfiguration with setup of a PDCP entity for a
///                      | radio bearer with more than one associated logical channel and upon RRC reconfiguration with
///                      | the association of an additional logical channel to the PDCP entity. Upon RRC reconfiguration
///                      | when a PDCP entity is associated with multiple logical channels, this field is optionally
///                      | present need M. Otherwise, this field is absent. Need R.
/// Rlc-AM               | For RLC AM, the field is optionally present, need R. Otherwise, the field is absent.
/// Setup                | The field is mandatory present in case of radio bearer setup. Otherwise the field is
///                      | optionally present, need M.
/// SplitBearer          | The field is absent for SRBs. Otherwise, the field is optional present, need M, in case of
///                      | radio bearer with more than one associated RLC mapped to different cell groups.
/// ConnectedTo5GC       | The field is optionally present, need R, if the UE is connected to 5GC. Otherwise the field
///                      | is absent.
/// ConnectedTo5GC1      | The field is optionally present, need R, if the UE is connected to NR/5GC. Otherwise the
///                      | field is absent.
/// Setup2               | This field is mandatory present in case for radio bearer setup for RLC-AM and RLC-UM.
///                      | Otherwise, this field is absent, Need M.
asn1::rrc_nr::pdcp_cfg_s pdcp_config_to_rrc_nr_asn1(const pdcp_config& pdcp_cfg);

/// \brief Converts type \c sdap_hdr_ul_cfg to an RRC NR ASN.1 type.
/// \param[in] hdr_cfg sdap ul header config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::sdap_cfg_s::sdap_hdr_ul_opts::options sdap_hdr_ul_cfg_to_rrc_asn1(sdap_hdr_ul_cfg hdr_cfg);

/// \brief Converts type \c sdap_hdr_dl_cfg to an RRC NR ASN.1 type.
/// \param[in] hdr_cfg sdap dl header config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::sdap_cfg_s::sdap_hdr_dl_opts::options sdap_hdr_dl_cfg_to_rrc_asn1(sdap_hdr_dl_cfg hdr_cfg);

/// \brief Converts type \c sdap_config_t to an RRC NR ASN.1 type.
/// \param[in] sdap_cfg sdap config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::sdap_cfg_s sdap_config_to_rrc_asn1(const sdap_config_t& sdap_cfg);

/// \brief Converts type \c security::ciphering_algorithm to an RRC NR ASN.1 type.
/// \param[in] ciphering_algo ciphering algorithm object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::ciphering_algorithm_e
ciphering_algorithm_to_rrc_asn1(const security::ciphering_algorithm& ciphering_algo);

/// \brief Converts type \c security::integrity_algorithm to an RRC NR ASN.1 type.
/// \param[in] integrity_prot_algo intergrity protection algorithm object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::integrity_prot_algorithm_e
integrity_prot_algorithm_to_rrc_asn1(const security::integrity_algorithm& integrity_prot_algo);

/// \brief Converts type \c asn1::fixed_bitstring<48> to an RRC NR ASN.1 type.
/// \param[in] asn1_five_g_s_tmsi five g s tmsi object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
five_g_s_tmsi_t asn1_to_five_g_s_tmsi(const asn1::fixed_bitstring<48>& asn1_five_g_s_tmsi);

/// \brief Converts type \c asn1::fixed_bitstring<39> and \c asn1::fixed_bitstring<9> to an RRC NR ASN.1 type.
/// \param[in] asn1_five_g_s_tmsi_part1 five g s tmsi part 1 object.
/// \param[in] asn1_five_g_s_tmsi_part2 five g s tmsi part 2 object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
five_g_s_tmsi_t asn1_to_five_g_s_tmsi(const asn1::fixed_bitstring<39>& asn1_five_g_s_tmsi_part1,
                                      const asn1::fixed_bitstring<9>&  asn1_five_g_s_tmsi_part2);

/// \brief Converts type \c asn1::fixed_bitstring<24> to an RRC NR ASN.1 type.
/// \param[in] asn1_amf_id amf id object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
amf_identifier_t asn1_to_amf_identifier(const asn1::fixed_bitstring<24>& asn1_amf_id);

/// \brief Converts type ASN.1 establishment cause to common type.
/// \param[in] asn1_cause ASN.1 establishment cause object.
/// \return The common establishment cause object where the result of the conversion is stored.
establishment_cause_t asn1_to_establishment_cause(const asn1::rrc_nr::establishment_cause_opts::options& asn1_cause);

/// \brief Converts type ASN.1 resume cause to common type.
/// \param[in] asn1_cause ASN.1 resume cause object.
/// \return The common resume cause object where the result of the conversion is stored.
resume_cause_t asn1_to_resume_cause(const asn1::rrc_nr::resume_cause_opts::options& asn1_cause);

/// \brief Converts type ASN.1 resume cause to common type.
/// \param[in] asn1_cause ASN.1 resume cause object.
/// \return The common establishment cause object where the result of the conversion is stored.
establishment_cause_t asn1_resume_cause_to_establishment_cause(const asn1::rrc_nr::resume_cause_e& asn1_cause);

/// \brief Converts type \c ran_paging_cycle to an RRC NR ASN.1 type.
/// \param[in] ran_paging_cycle ran paging cycle object.
/// \return asn1_ran_paging_cycle The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::enumerated<asn1::rrc_nr::paging_cycle_opts> ran_paging_cycle_to_asn1(uint8_t ran_paging_cycle);

/// \brief Converts type \c plmn_identity to an RRC NR ASN.1 type.
/// \param[in] plmn plmn identity object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored
asn1::rrc_nr::plmn_id_s plmn_to_asn1(const plmn_identity& plmn);

/// \brief Converts type \c rrc_ran_notification_area_info_t to an RRC NR ASN.1 type.
/// \param[in] ran_notif_area_info ran notification area info object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored
asn1::rrc_nr::ran_notif_area_info_c
ran_notification_area_info_to_asn1(const rrc_ran_notification_area_info_t& ran_notif_area_info);

/// \brief Converts type \c rrc_radio_bearer_config to an RRC NR ASN.1 type.
/// \param[in] radio_bearer_cfg radio bearer config object.
/// \param[out] asn1_radio_bearer_cfg The RRC NR ASN.
void radio_bearer_config_to_asn1(const rrc_radio_bearer_config&    radio_bearer_cfg,
                                 asn1::rrc_nr::radio_bearer_cfg_s& asn1_radio_bearer_cfg);

} // namespace ocudu::ocucp
