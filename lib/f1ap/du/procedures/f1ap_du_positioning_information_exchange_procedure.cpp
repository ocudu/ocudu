// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_positioning_information_exchange_procedure.h"
#include "../ue_context/f1ap_du_ue.h"
#include "proc_logger.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_ies.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/resource_allocation/sliv.h"
#include "ocudu/ran/resource_block.h"

using namespace ocudu;
using namespace odu;

f1ap_du_positioning_information_exchange_procedure::f1ap_du_positioning_information_exchange_procedure(
    const asn1::f1ap::positioning_info_request_s& msg_,
    f1ap_du_positioning_handler&                  du_mng_,
    f1ap_du_ue&                                   ue_) :
  msg(msg_), du_mng(du_mng_), ue(ue_), logger(ocudulog::fetch_basic_logger("DU-F1"))
{
}

void f1ap_du_positioning_information_exchange_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  CORO_AWAIT_VALUE(result, request_du_for_positioning_info());

  // Respond back to CU-CP.
  send_response();

  CORO_RETURN();
}

async_task<du_positioning_info_response>
f1ap_du_positioning_information_exchange_procedure::request_du_for_positioning_info()
{
  du_positioning_info_request du_req;
  du_req.ue_index = ue.context.ue_index;

  return du_mng.request_positioning_info(du_req);
}

static asn1::f1ap::srs_res_s convert_to_asn1(const srs_config::srs_resource& cfg)
{
  using namespace asn1::f1ap;

  srs_res_s ret;

  ret.srs_res_id = cfg.id.ue_res_id;
  report_fatal_error_if_not(asn1::number_to_enum(ret.nrof_srs_ports, (unsigned)cfg.nof_ports),
                            "Invalid number of SRS ports");
  if (cfg.tx_comb.size == tx_comb_size::n2) {
    auto& n2           = ret.tx_comb.set_n2();
    n2.comb_offset_n2  = cfg.tx_comb.tx_comb_offset;
    n2.cyclic_shift_n2 = cfg.tx_comb.tx_comb_cyclic_shift;
  } else {
    auto& n4           = ret.tx_comb.set_n4();
    n4.comb_offset_n4  = cfg.tx_comb.tx_comb_offset;
    n4.cyclic_shift_n4 = cfg.tx_comb.tx_comb_cyclic_shift;
  }
  ret.start_position = cfg.res_mapping.start_pos;
  report_fatal_error_if_not(asn1::number_to_enum(ret.nrof_symbols, cfg.res_mapping.nof_symb),
                            "Invalid number of symbols");
  report_fatal_error_if_not(asn1::number_to_enum(ret.repeat_factor, cfg.res_mapping.rept_factor),
                            "Invalid repeat factor");
  ret.freq_domain_position = cfg.freq_domain_pos;
  ret.freq_domain_shift    = cfg.freq_domain_shift;
  ret.c_srs                = cfg.freq_hop.c_srs;
  ret.b_srs                = cfg.freq_hop.b_srs;
  ret.b_hop                = cfg.freq_hop.b_hop;
  switch (cfg.grp_or_seq_hop) {
    case srs_group_or_sequence_hopping::neither:
      ret.group_or_seq_hop = srs_res_s::group_or_seq_hop_opts::neither;
      break;
    case srs_group_or_sequence_hopping::group_hopping:
      ret.group_or_seq_hop = srs_res_s::group_or_seq_hop_opts::group_hop;
      break;
    case srs_group_or_sequence_hopping::sequence_hopping:
      ret.group_or_seq_hop = srs_res_s::group_or_seq_hop_opts::seq_hop;
      break;
    default:
      report_fatal_error("Invalid grp or seq hop={}", fmt::underlying(cfg.grp_or_seq_hop));
  }
  switch (cfg.res_type) {
    case srs_resource_type::aperiodic: {
      res_type_aperiodic_s& aper_res = ret.res_type.set_aperiodic();
      aper_res.aperiodic_res_type    = asn1::f1ap::res_type_aperiodic_s::aperiodic_res_type_opts::options::true_value;
    } break;
    case srs_resource_type::semi_persistent: {
      auto& sp_res = ret.res_type.set_semi_persistent();
      ocudu_assert(cfg.periodicity_and_offset.has_value(), "Semi-persistent resource must have periodicity and offset");
      report_fatal_error_if_not(
          asn1::number_to_enum(sp_res.periodicity, (unsigned)cfg.periodicity_and_offset.value().period),
          "Invalid period");
      sp_res.offset = cfg.periodicity_and_offset.value().offset;
    } break;
    case srs_resource_type::periodic: {
      ocudu_assert(cfg.periodicity_and_offset.has_value(), "Periodic resource must have periodicity and offset");
      auto& p_res = ret.res_type.set_periodic();
      report_fatal_error_if_not(
          asn1::number_to_enum(p_res.periodicity, (unsigned)cfg.periodicity_and_offset.value().period),
          "Invalid period");
      p_res.offset = cfg.periodicity_and_offset.value().offset;
    } break;
    default:
      ocudu_assertion_failure("Invalid resource type={}", fmt::underlying(cfg.res_type));
  }
  ret.seq_id = cfg.sequence_id;

  return ret;
}

static asn1::f1ap::srs_res_set_s convert_to_asn1(const srs_config::srs_resource_set& cfg)
{
  using namespace asn1::f1ap;
  using set_type = srs_config::srs_resource_set;

  srs_res_set_s ret;

  ret.srs_res_set_id = cfg.id;
  for (const auto& res_id : cfg.srs_res_id_list) {
    ret.srs_res_id_list.push_back(res_id);
  }
  if (const auto* cfg_aper_res = std::get_if<set_type::aperiodic_resource_type>(&cfg.res_type)) {
    auto& aper_ret                = ret.res_set_type.set_aperiodic();
    aper_ret.srs_res_trigger_list = cfg_aper_res->aperiodic_srs_res_trigger;
    aper_ret.slotoffset           = cfg_aper_res->slot_offset.value_or(0);
  } else if (std::get_if<srs_config::srs_resource_set::semi_persistent_resource_type>(&cfg.res_type) != nullptr) {
    auto& semi_p_res                     = ret.res_set_type.set_semi_persistent();
    semi_p_res.semi_persistent_set.value = res_set_type_semi_persistent_s::semi_persistent_set_opts::true_value;
  } else if (std::get_if<srs_config::srs_resource_set::periodic_resource_type>(&cfg.res_type) != nullptr) {
    auto& per_ret              = ret.res_set_type.set_periodic();
    per_ret.periodic_set.value = res_set_type_periodic_s::periodic_set_opts::true_value;
  } else {
    report_fatal_error("Invalid resource type");
  }

  return ret;
}

void f1ap_du_positioning_information_exchange_procedure::send_response() const
{
  using namespace asn1::f1ap;

  f1ap_message f1ap_msg;

  if (result.srs_carriers.empty()) {
    // Prepare POSITIONING INFORMATION FAILURE message.

    f1ap_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_POSITIONING_INFO_EXCHANGE);
    positioning_info_fail_s& fail         = f1ap_msg.pdu.unsuccessful_outcome().value.positioning_info_fail();
    fail->gnb_cu_ue_f1ap_id               = msg->gnb_cu_ue_f1ap_id;
    fail->gnb_du_ue_f1ap_id               = msg->gnb_du_ue_f1ap_id;
    fail->cause.set_radio_network().value = cause_radio_network_opts::no_radio_res_available;

  } else {
    // Prepare POSITIONING INFORMATION RESPONSE message.

    f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_POSITIONING_INFO_EXCHANGE);
    positioning_info_resp_s& resp = f1ap_msg.pdu.successful_outcome().value.positioning_info_resp();

    resp->gnb_cu_ue_f1ap_id = msg->gnb_cu_ue_f1ap_id;
    resp->gnb_du_ue_f1ap_id = msg->gnb_du_ue_f1ap_id;

    // Fill SRS Configuration.
    resp->srs_configuration_present = true;
    // Fill SRSCarrier-List.
    resp->srs_configuration.srs_carrier_list.resize(result.srs_carriers.size());
    for (unsigned i = 0, e = result.srs_carriers.size(); i != e; ++i) {
      auto&       out = resp->srs_configuration.srs_carrier_list[i];
      const auto& in  = result.srs_carriers[i];

      out.point_a = in.point_a.value();
      // Fill UplinkChannelBW-PerSCS-List.
      out.ul_ch_bw_per_scs_list.resize(in.ul_ch_bw_per_scs_list.size());
      for (unsigned j = 0, e2 = in.ul_ch_bw_per_scs_list.size(); j != e2; ++j) {
        scs_specific_carrier_s&     scs_out = out.ul_ch_bw_per_scs_list[j];
        const scs_specific_carrier& scs_in  = in.ul_ch_bw_per_scs_list[j];

        scs_out.offset_to_carrier = scs_in.offset_to_carrier;
        scs_out.subcarrier_spacing.value =
            (scs_specific_carrier_s::subcarrier_spacing_opts::options)to_numerology_value(scs_in.scs);
        scs_out.carrier_bw = scs_in.carrier_bandwidth;
      }
      // Fill ActiveULBWP.
      out.active_ul_bwp.location_and_bw =
          sliv_from_s_and_l(MAX_NOF_PRBS, in.ul_bwp_cfg.crbs.start(), in.ul_bwp_cfg.crbs.length());
      out.active_ul_bwp.subcarrier_spacing.value =
          (active_ul_bwp_s::subcarrier_spacing_opts::options)to_numerology_value(in.ul_bwp_cfg.scs);
      out.active_ul_bwp.cp.value = in.ul_bwp_cfg.cp == cyclic_prefix::NORMAL ? active_ul_bwp_s::cp_opts::normal
                                                                             : active_ul_bwp_s::cp_opts::extended;
      out.active_ul_bwp.tx_direct_current_location =
          in.ul_ch_bw_per_scs_list[0].tx_direct_current_location.has_value()
              ? in.ul_ch_bw_per_scs_list[0].tx_direct_current_location.value()
              : 3300;
      out.active_ul_bwp.shift7dot5k_hz_present = in.freq_shift_7p5khz_present;
      if (in.freq_shift_7p5khz_present) {
        out.active_ul_bwp.shift7dot5k_hz = asn1::f1ap::active_ul_bwp_s::shift7dot5k_hz_opts::options::true_value;
      }
      // Fill SRSConfig.
      // > Fill SRSResource-List.
      out.active_ul_bwp.srs_cfg.srs_res_list.resize(in.srs_cfg.srs_res_list.size());
      for (unsigned j = 0, e2 = in.srs_cfg.srs_res_list.size(); j != e2; ++j) {
        out.active_ul_bwp.srs_cfg.srs_res_list[j] = convert_to_asn1(in.srs_cfg.srs_res_list[j]);
      }
      // > Fill SRS-ResourceSetList.
      out.active_ul_bwp.srs_cfg.srs_res_set_list.resize(in.srs_cfg.srs_res_set_list.size());
      for (unsigned j = 0, e2 = in.srs_cfg.srs_res_set_list.size(); j != e2; ++j) {
        out.active_ul_bwp.srs_cfg.srs_res_set_list[j] = convert_to_asn1(in.srs_cfg.srs_res_set_list[j]);
      }
      // Fill PCI.
      if (in.pci.has_value()) {
        out.pci_present = true;
        out.pci         = in.pci.value();
      }
    }
  }

  // Send Response to CU-CP.
  ue.f1ap_msg_notifier.on_new_message(f1ap_msg);

  logger.debug("{}: Procedure finished successfully.", f1ap_log_prefix{ue.context, name()});
}
