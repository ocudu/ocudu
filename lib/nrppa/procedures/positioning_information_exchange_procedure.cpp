// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "positioning_information_exchange_procedure.h"
#include "../nrppa_asn1_converters.h"
#include "../nrppa_helper.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/nrppa/common.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/asn1/nrppa/nrppa_pdu_contents.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/cause/nrppa_cause.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

positioning_information_exchange_procedure::positioning_information_exchange_procedure(
    const positioning_information_request_t& request_,
    uint16_t                                 transaction_id_,
    cu_cp_du_index_t                         du_index_,
    nrppa_du_context_list&                   du_ctxt_list_,
    nrppa_cu_cp_notifier&                    cu_cp_notifier_,
    ocudulog::basic_logger&                  logger_) :
  pos_info_request(request_),
  transaction_id(transaction_id_),
  du_index(du_index_),
  du_ctxt_list(du_ctxt_list_),
  cu_cp_notifier(cu_cp_notifier_),
  logger(logger_)
{
}

void positioning_information_exchange_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("ue={}: \"{}\" initialized", pos_info_request.ue_index, name());

  // Sanity check.
  if (!du_ctxt_list.contains(du_index)) {
    logger.error("DU context not found for UE {}", pos_info_request.ue_index);
    logger.info("ue={}: \"{}\" failed", pos_info_request.ue_index, name());
    send_ul_nrppa_pdu(
        create_positioning_information_failure(nrppa_cause_protocol_t::msg_not_compatible_with_receiver_state));
    CORO_EARLY_RETURN();
  }

  CORO_AWAIT_VALUE(procedure_outcome,
                   du_ctxt_list[du_index].f1ap->on_positioning_information_request(pos_info_request));

  // Pack positioning information outcome and forward to CU-CP.
  handle_procedure_outcome();

  CORO_RETURN();
}

void positioning_information_exchange_procedure::handle_procedure_outcome()
{
  if (!procedure_outcome.has_value()) {
    asn1_pos_info_outcome = create_positioning_information_failure(procedure_outcome.error().cause);
    logger.info("ue={}: \"{}\" failed", pos_info_request.ue_index, name());
  } else {
    asn1_pos_info_outcome = create_positioning_information_response();
    logger.info("ue={}: \"{}\" finished successfully", pos_info_request.ue_index, name());
  }

  // Send response to CU-CP.
  send_ul_nrppa_pdu(asn1_pos_info_outcome);
}

void positioning_information_exchange_procedure::send_ul_nrppa_pdu(const asn1::nrppa::nr_ppa_pdu_c& outcome)
{
  // Pack into PDU.
  ul_nrppa_pdu = pack_into_pdu(outcome,
                               outcome.type().value == asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome
                                   ? "PositioningInformationResponse"
                                   : "PositioningInformationFailure");

  // Log Tx message.
  log_nrppa_message(ocudulog::fetch_basic_logger("NRPPA"), Tx, ul_nrppa_pdu, outcome);

  // Send response to CU-CP.
  cu_cp_notifier.on_ul_nrppa_pdu(ul_nrppa_pdu, pos_info_request.ue_index);
}

asn1::nrppa::nr_ppa_pdu_c
positioning_information_exchange_procedure::create_positioning_information_failure(nrppa_cause_t cause) const
{
  asn1::nrppa::nr_ppa_pdu_c asn1_fail;

  asn1_fail.set_unsuccessful_outcome().load_info_obj(ASN1_NRPPA_ID_POSITIONING_INFO_EXCHANGE);
  asn1_fail.unsuccessful_outcome().nrppatransaction_id = transaction_id;
  asn1::nrppa::positioning_info_fail_s& pos_info_fail  = asn1_fail.unsuccessful_outcome().value.positioning_info_fail();
  pos_info_fail->cause                                 = cause_to_asn1(cause);

  return asn1_fail;
}

asn1::nrppa::nr_ppa_pdu_c positioning_information_exchange_procedure::create_positioning_information_response()
{
  asn1::nrppa::nr_ppa_pdu_c asn1_resp;

  asn1_resp.set_successful_outcome().load_info_obj(ASN1_NRPPA_ID_POSITIONING_INFO_EXCHANGE);
  asn1_resp.successful_outcome().nrppatransaction_id = transaction_id;

  asn1::nrppa::positioning_info_resp_s& asn1_pos_info_resp =
      asn1_resp.successful_outcome().value.positioning_info_resp();

  const positioning_information_response_t& pos_info_resp = procedure_outcome.value();

  if (pos_info_resp.srs_cfg.has_value()) {
    asn1_pos_info_resp->srs_configuration_present = true;
    for (const auto& srs_carrier_item : pos_info_resp.srs_cfg->srs_carrier_list) {
      asn1::nrppa::srs_carrier_list_item_s asn1_srs_carrier_item;
      // Fill point A.
      asn1_srs_carrier_item.point_a = srs_carrier_item.point_a;
      // Fill UL CH BW per SCS list.
      for (const auto& scs_specific_carrier : srs_carrier_item.ul_ch_bw_per_scs_list) {
        asn1::nrppa::scs_specific_carrier_s asn1_scs_specific_carrier;
        asn1_scs_specific_carrier.offset_to_carrier = scs_specific_carrier.offset_to_carrier;

        if (scs_specific_carrier.scs == ocudu::subcarrier_spacing::kHz15) {
          asn1_scs_specific_carrier.subcarrier_spacing =
              asn1::nrppa::scs_specific_carrier_s::subcarrier_spacing_opts::options::khz15;
        } else if (scs_specific_carrier.scs == ocudu::subcarrier_spacing::kHz30) {
          asn1_scs_specific_carrier.subcarrier_spacing =
              asn1::nrppa::scs_specific_carrier_s::subcarrier_spacing_opts::options::khz30;
        } else if (scs_specific_carrier.scs == ocudu::subcarrier_spacing::kHz60) {
          asn1_scs_specific_carrier.subcarrier_spacing =
              asn1::nrppa::scs_specific_carrier_s::subcarrier_spacing_opts::options::khz60;
        } else {
          asn1_scs_specific_carrier.subcarrier_spacing =
              asn1::nrppa::scs_specific_carrier_s::subcarrier_spacing_opts::options::khz120;
        }

        asn1_scs_specific_carrier.carrier_bw = scs_specific_carrier.carrier_bandwidth;
        asn1_srs_carrier_item.ul_ch_bw_per_scs_list.push_back(asn1_scs_specific_carrier);
      }
      // Fill active UL BWP.
      asn1_srs_carrier_item.active_ul_bwp.location_and_bw = srs_carrier_item.active_ul_bwp.location_and_bw;

      if (srs_carrier_item.active_ul_bwp.scs == ocudu::subcarrier_spacing::kHz15) {
        asn1_srs_carrier_item.active_ul_bwp.subcarrier_spacing =
            asn1::nrppa::active_ul_bwp_s::subcarrier_spacing_opts::options::khz15;
      } else if (srs_carrier_item.active_ul_bwp.scs == ocudu::subcarrier_spacing::kHz30) {
        asn1_srs_carrier_item.active_ul_bwp.subcarrier_spacing =
            asn1::nrppa::active_ul_bwp_s::subcarrier_spacing_opts::options::khz30;
      } else if (srs_carrier_item.active_ul_bwp.scs == ocudu::subcarrier_spacing::kHz60) {
        asn1_srs_carrier_item.active_ul_bwp.subcarrier_spacing =
            asn1::nrppa::active_ul_bwp_s::subcarrier_spacing_opts::options::khz60;
      } else {
        asn1_srs_carrier_item.active_ul_bwp.subcarrier_spacing =
            asn1::nrppa::active_ul_bwp_s::subcarrier_spacing_opts::options::khz120;
      }

      if (srs_carrier_item.active_ul_bwp.cp == ocudu::cyclic_prefix::NORMAL) {
        asn1_srs_carrier_item.active_ul_bwp.cp = asn1::nrppa::active_ul_bwp_s::cp_opts::options::normal;
      } else {
        asn1_srs_carrier_item.active_ul_bwp.cp = asn1::nrppa::active_ul_bwp_s::cp_opts::options::extended;
      }

      asn1_srs_carrier_item.active_ul_bwp.tx_direct_current_location =
          srs_carrier_item.active_ul_bwp.tx_direct_current_location;
      if (srs_carrier_item.active_ul_bwp.shift7dot5k_hz.has_value()) {
        asn1_srs_carrier_item.active_ul_bwp.shift7dot5k_hz_present = true;
        if (srs_carrier_item.active_ul_bwp.shift7dot5k_hz.value()) {
          asn1_srs_carrier_item.active_ul_bwp.shift7dot5k_hz =
              asn1::nrppa::active_ul_bwp_s::shift7dot5k_hz_opts::options::true_value;
        }
      }
      // > Fill SRS configuration.
      // >> Fill SRS res list.
      for (const auto& srs_res : srs_carrier_item.active_ul_bwp.srs_cfg.srs_res_list) {
        asn1::nrppa::srs_res_s asn1_srs_res;
        asn1_srs_res.srs_res_id = (uint8_t)srs_res.id.ue_res_id;
        asn1::number_to_enum(asn1_srs_res.nrof_srs_ports, (uint8_t)srs_res.nof_ports);
        // Fill TX comb.
        if (srs_res.tx_comb.size == ocudu::tx_comb_size::n2) {
          asn1::nrppa::tx_comb_c::n2_s_ asn1_n2;
          asn1_n2.comb_offset_n2        = srs_res.tx_comb.tx_comb_offset;
          asn1_n2.cyclic_shift_n2       = srs_res.tx_comb.tx_comb_cyclic_shift;
          asn1_srs_res.tx_comb.set_n2() = asn1_n2;
        } else {
          asn1::nrppa::tx_comb_c::n4_s_ asn1_n4;
          asn1_n4.comb_offset_n4        = srs_res.tx_comb.tx_comb_offset;
          asn1_n4.cyclic_shift_n4       = srs_res.tx_comb.tx_comb_cyclic_shift;
          asn1_srs_res.tx_comb.set_n4() = asn1_n4;
        }
        asn1_srs_res.start_position = srs_res.res_mapping.start_pos;
        asn1::number_to_enum(asn1_srs_res.nrof_symbols, srs_res.res_mapping.nof_symb);
        asn1::number_to_enum(asn1_srs_res.repeat_factor, srs_res.res_mapping.rept_factor);
        asn1_srs_res.freq_domain_position = srs_res.freq_domain_pos;
        asn1_srs_res.freq_domain_shift    = srs_res.freq_domain_shift;
        asn1_srs_res.c_srs                = srs_res.freq_hop.c_srs;
        asn1_srs_res.b_srs                = srs_res.freq_hop.b_srs;
        asn1_srs_res.b_hop                = srs_res.freq_hop.b_hop;
        if (srs_res.grp_or_seq_hop == srs_group_or_sequence_hopping::neither) {
          asn1_srs_res.group_or_seq_hop = asn1::nrppa::srs_res_s::group_or_seq_hop_opts::options::neither;
        } else if (srs_res.grp_or_seq_hop == srs_group_or_sequence_hopping::group_hopping) {
          asn1_srs_res.group_or_seq_hop = asn1::nrppa::srs_res_s::group_or_seq_hop_opts::options::group_hop;
        } else {
          asn1_srs_res.group_or_seq_hop = asn1::nrppa::srs_res_s::group_or_seq_hop_opts::options::seq_hop;
        }
        // >> Fill res type.
        if (srs_res.res_type == srs_resource_type::periodic) {
          ocudu_assert(srs_res.periodicity_and_offset.has_value(),
                       "Periodicity and offset must be set for periodic resources.");
          asn1::nrppa::res_type_periodic_s asn1_res_type_periodic;
          asn1::number_to_enum(asn1_res_type_periodic.periodicity, (uint16_t)srs_res.periodicity_and_offset->period);
          asn1_res_type_periodic.offset        = srs_res.periodicity_and_offset->offset;
          asn1_srs_res.res_type.set_periodic() = asn1_res_type_periodic;
        } else if (srs_res.res_type == srs_resource_type::semi_persistent) {
          ocudu_assert(srs_res.periodicity_and_offset.has_value(),
                       "Periodicity and offset must be set for semi-persistent resources.");
          asn1::nrppa::res_type_semi_persistent_s asn1_res_type_semi_persistent;
          asn1::number_to_enum(asn1_res_type_semi_persistent.periodicity,
                               (uint16_t)srs_res.periodicity_and_offset->period);
          asn1_res_type_semi_persistent.offset        = srs_res.periodicity_and_offset->offset;
          asn1_srs_res.res_type.set_semi_persistent() = asn1_res_type_semi_persistent;
        } else {
          asn1::nrppa::res_type_aperiodic_s asn1_res_type_aperiodic;
          asn1_res_type_aperiodic.aperiodic_res_type =
              asn1::nrppa::res_type_aperiodic_s::aperiodic_res_type_opts::options::true_value;
          asn1_srs_res.res_type.set_aperiodic() = asn1_res_type_aperiodic;
        }
        asn1_srs_res.seq_id = srs_res.sequence_id;

        asn1_srs_carrier_item.active_ul_bwp.srs_cfg.srs_res_list.push_back(asn1_srs_res);
      }
      // >> Fill pos SRS res list.
      for (const auto& pos_srs_res_item : srs_carrier_item.active_ul_bwp.srs_cfg.pos_srs_res_list) {
        asn1::nrppa::pos_srs_res_item_s asn1_pos_srs_res_item;
        // Fill SRS pos res ID.
        asn1_pos_srs_res_item.srs_pos_res_id = pos_srs_res_item.id.ue_res_id;
        // Fill TX comb.
        if (pos_srs_res_item.tx_comb.size == ocudu::tx_comb_pos_size::n2) {
          asn1::nrppa::tx_comb_pos_c::n2_s_ asn1_n2;
          asn1_n2.comb_offset_n2                     = pos_srs_res_item.tx_comb.tx_comb_pos_offset;
          asn1_n2.cyclic_shift_n2                    = pos_srs_res_item.tx_comb.tx_comb_pos_cyclic_shift;
          asn1_pos_srs_res_item.tx_comb_pos.set_n2() = asn1_n2;
        } else if (pos_srs_res_item.tx_comb.size == ocudu::tx_comb_pos_size::n4) {
          asn1::nrppa::tx_comb_pos_c::n4_s_ asn1_n4;
          asn1_n4.comb_offset_n4                     = pos_srs_res_item.tx_comb.tx_comb_pos_offset;
          asn1_n4.cyclic_shift_n4                    = pos_srs_res_item.tx_comb.tx_comb_pos_cyclic_shift;
          asn1_pos_srs_res_item.tx_comb_pos.set_n4() = asn1_n4;
        } else {
          asn1::nrppa::tx_comb_pos_c::n8_s_ asn1_n8;
          asn1_n8.comb_offset_n8                     = pos_srs_res_item.tx_comb.tx_comb_pos_offset;
          asn1_n8.cyclic_shift_n8                    = pos_srs_res_item.tx_comb.tx_comb_pos_cyclic_shift;
          asn1_pos_srs_res_item.tx_comb_pos.set_n8() = asn1_n8;
        }
        // Fill start position.
        asn1_pos_srs_res_item.start_position = pos_srs_res_item.res_mapping.start_pos;
        // Fill nrof symbols.
        asn1::number_to_enum(asn1_pos_srs_res_item.nrof_symbols, pos_srs_res_item.res_mapping.nof_symb);
        // Fill freq domain shift.
        asn1_pos_srs_res_item.freq_domain_shift = pos_srs_res_item.freq_domain_shift;
        // Fill c SRS.
        asn1_pos_srs_res_item.c_srs = pos_srs_res_item.c_srs;
        // Fill group or seq hop.
        if (pos_srs_res_item.grp_or_seq_hop == srs_group_or_sequence_hopping::neither) {
          asn1_pos_srs_res_item.group_or_seq_hop =
              asn1::nrppa::pos_srs_res_item_s::group_or_seq_hop_opts::options::neither;
        } else if (pos_srs_res_item.grp_or_seq_hop == srs_group_or_sequence_hopping::group_hopping) {
          asn1_pos_srs_res_item.group_or_seq_hop =
              asn1::nrppa::pos_srs_res_item_s::group_or_seq_hop_opts::options::group_hop;
        } else {
          asn1_pos_srs_res_item.group_or_seq_hop =
              asn1::nrppa::pos_srs_res_item_s::group_or_seq_hop_opts::options::seq_hop;
        }
        // Fill res type pos.
        if (pos_srs_res_item.res_type == srs_resource_type::periodic) {
          ocudu_assert(pos_srs_res_item.periodicity_and_offset.has_value(),
                       "Periodicity and offset must be set for periodic resources.");
          asn1::nrppa::res_type_periodic_pos_s asn1_res_type_pos_periodic;
          asn1::number_to_enum(asn1_res_type_pos_periodic.srs_periodicity,
                               (uint16_t)pos_srs_res_item.periodicity_and_offset->period);
          asn1_res_type_pos_periodic.offset                 = pos_srs_res_item.periodicity_and_offset->offset;
          asn1_pos_srs_res_item.res_type_pos.set_periodic() = asn1_res_type_pos_periodic;
        } else if (pos_srs_res_item.res_type == srs_resource_type::semi_persistent) {
          ocudu_assert(pos_srs_res_item.periodicity_and_offset.has_value(),
                       "Periodicity and offset must be set for periodic resources.");
          asn1::nrppa::res_type_semi_persistent_pos_s asn1_res_type_semi_persisten_pos;
          asn1::number_to_enum(asn1_res_type_semi_persisten_pos.srs_periodicity,
                               (uint16_t)pos_srs_res_item.periodicity_and_offset->period);
          asn1_res_type_semi_persisten_pos.offset                  = pos_srs_res_item.periodicity_and_offset->offset;
          asn1_pos_srs_res_item.res_type_pos.set_semi_persistent() = asn1_res_type_semi_persisten_pos;
        } else {
          ocudu_assert(pos_srs_res_item.slot_offset.has_value(), "Slot offset must be set for aperiodic resources.");
          asn1::nrppa::res_type_aperiodic_pos_s asn1_res_type_aperiodic_pos;
          asn1_res_type_aperiodic_pos.slot_offset            = pos_srs_res_item.slot_offset.value();
          asn1_pos_srs_res_item.res_type_pos.set_aperiodic() = asn1_res_type_aperiodic_pos;
        }
        // Fill seq ID.
        asn1_pos_srs_res_item.seq_id = pos_srs_res_item.sequence_id;
        // Fill spatial relation pos.
        if (pos_srs_res_item.spatial_relation_info.has_value()) {
          asn1_pos_srs_res_item.spatial_relation_pos_present = true;
          if (std::holds_alternative<ocudu::srs_config::srs_pos_resource::srs_spatial_relation_pos::ssb>(
                  pos_srs_res_item.spatial_relation_info->reference_signal)) {
            asn1::nrppa::ssb_s asn1_ssb;
            const auto&        ssb = std::get<ocudu::srs_config::srs_pos_resource::srs_spatial_relation_pos::ssb>(
                pos_srs_res_item.spatial_relation_info->reference_signal);
            asn1_ssb.pci_nr = ssb.pci_nr;
            if (ssb.ssb_idx.has_value()) {
              asn1_ssb.ssb_idx_present = true;
              asn1_ssb.ssb_idx         = ssb.ssb_idx->value();
            }
            asn1_pos_srs_res_item.spatial_relation_pos.set_ssb_pos() = asn1_ssb;
          } else {
            asn1::nrppa::pr_si_nformation_pos_s asn1_prs_info;
            const auto& prs_info = std::get<ocudu::srs_config::srs_pos_resource::srs_spatial_relation_pos::prs>(
                pos_srs_res_item.spatial_relation_info->reference_signal);
            asn1_prs_info.prs_id_pos         = prs_info.id;
            asn1_prs_info.prs_res_set_id_pos = prs_info.prs_res_set_id;
            if (prs_info.prs_res_id.has_value()) {
              asn1_prs_info.prs_res_id_pos_present = true;
              asn1_prs_info.prs_res_id_pos         = prs_info.prs_res_id.value();
            }
            asn1_pos_srs_res_item.spatial_relation_pos.set_pr_si_nformation_pos() = asn1_prs_info;
          }
        }

        asn1_srs_carrier_item.active_ul_bwp.srs_cfg.pos_srs_res_list.push_back(asn1_pos_srs_res_item);
      }

      // >> Fill SRS res set list.
      for (const auto& srs_res_set : srs_carrier_item.active_ul_bwp.srs_cfg.srs_res_set_list) {
        asn1::nrppa::srs_res_set_s asn1_srs_res_set;
        asn1_srs_res_set.srs_res_set_id1 = srs_res_set.id;
        for (const auto& srs_res_id : srs_res_set.srs_res_id_list) {
          asn1_srs_res_set.srs_res_id_list.push_back(srs_res_id);
        }
        // Fill res set type.
        if (std::holds_alternative<ocudu::srs_config::srs_resource_set::periodic_resource_type>(srs_res_set.res_type)) {
          asn1::nrppa::res_set_type_periodic_s asn1_res_set_type_periodic;
          asn1_res_set_type_periodic.periodic_set =
              asn1::nrppa::res_set_type_periodic_s::periodic_set_opts::options::true_value;
          asn1_srs_res_set.res_set_type.set_periodic() = asn1_res_set_type_periodic;
        } else if (std::holds_alternative<ocudu::srs_config::srs_resource_set::semi_persistent_resource_type>(
                       srs_res_set.res_type)) {
          asn1::nrppa::res_set_type_semi_persistent_s asn1_res_set_type_semi_persistent;
          asn1_res_set_type_semi_persistent.semi_persistent_set =
              asn1::nrppa::res_set_type_semi_persistent_s::semi_persistent_set_opts::options::true_value;
          asn1_srs_res_set.res_set_type.set_semi_persistent() = asn1_res_set_type_semi_persistent;
        } else {
          asn1::nrppa::res_set_type_aperiodic_s asn1_res_set_type_aperiodic;
          const auto&                           aperiodic =
              std::get<ocudu::srs_config::srs_resource_set::aperiodic_resource_type>(srs_res_set.res_type);
          asn1_res_set_type_aperiodic.srs_res_trigger   = aperiodic.aperiodic_srs_res_trigger;
          asn1_res_set_type_aperiodic.slotoffset        = aperiodic.slot_offset.value();
          asn1_srs_res_set.res_set_type.set_aperiodic() = asn1_res_set_type_aperiodic;
        }

        asn1_srs_carrier_item.active_ul_bwp.srs_cfg.srs_res_set_list.push_back(asn1_srs_res_set);
      }

      // >> Fill pos SRS res set list.
      for (const auto& pos_srs_res_set_item : srs_carrier_item.active_ul_bwp.srs_cfg.pos_srs_res_set_list) {
        asn1::nrppa::pos_srs_res_set_item_s asn1_pos_srs_res_set_item;

        asn1_pos_srs_res_set_item.possrs_res_set_id = pos_srs_res_set_item.id;
        for (const auto& pos_srs_res_id : pos_srs_res_set_item.srs_res_id_list) {
          asn1_pos_srs_res_set_item.poss_rs_res_id_per_set_list.push_back(pos_srs_res_id);
        }
        // Fill res set type.
        if (std::holds_alternative<ocudu::srs_config::srs_resource_set::periodic_resource_type>(
                pos_srs_res_set_item.res_type)) {
          asn1::nrppa::pos_res_set_type_periodic_s asn1_pos_res_set_type_periodic;
          asn1_pos_res_set_type_periodic.posperiodic_set =
              asn1::nrppa::pos_res_set_type_periodic_s::posperiodic_set_opts::options::true_value;
          asn1_pos_srs_res_set_item.posres_set_type.set_periodic() = asn1_pos_res_set_type_periodic;
        } else if (std::holds_alternative<ocudu::srs_config::srs_resource_set::semi_persistent_resource_type>(
                       pos_srs_res_set_item.res_type)) {
          asn1::nrppa::pos_res_set_type_semi_persistent_s asn1_res_set_type_semi_persistent;
          asn1_res_set_type_semi_persistent.possemi_persistent_set =
              asn1::nrppa::pos_res_set_type_semi_persistent_s::possemi_persistent_set_opts::options::true_value;
          asn1_pos_srs_res_set_item.posres_set_type.set_semi_persistent() = asn1_res_set_type_semi_persistent;
        } else {
          asn1::nrppa::pos_res_set_type_aperiodic_s asn1_res_set_type_aperiodic;
          asn1_res_set_type_aperiodic.srs_res_trigger =
              std::get<ocudu::srs_config::srs_resource_set::aperiodic_resource_type>(pos_srs_res_set_item.res_type)
                  .aperiodic_srs_res_trigger;
          asn1_pos_srs_res_set_item.posres_set_type.set_aperiodic() = asn1_res_set_type_aperiodic;
        }

        asn1_srs_carrier_item.active_ul_bwp.srs_cfg.pos_srs_res_set_list.push_back(asn1_pos_srs_res_set_item);
      }

      // Fill PCI.
      if (srs_carrier_item.pci_nr.has_value()) {
        asn1_srs_carrier_item.pci_nr_present = true;
        asn1_srs_carrier_item.pci_nr         = srs_carrier_item.pci_nr.value();
      }

      asn1_pos_info_resp->srs_configuration.srs_carrier_list.push_back(asn1_srs_carrier_item);
    }
  }

  if (pos_info_resp.sfn_initialization_time.has_value()) {
    asn1_pos_info_resp->sfn_initisation_time_present = true;
    asn1_pos_info_resp->sfn_initisation_time.from_number(pos_info_resp.sfn_initialization_time.value());
  }

  // TODO: Fill criticality diagnostics.

  // TODO:: Add missing optional values.

  return asn1_resp;
}
