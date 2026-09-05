// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "e2sm/e2sm_ccc/e2sm_ccc_asn1_packer.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.h"
#include "e2sm/e2sm_rc/e2sm_rc_asn1_packer.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_node_component_config.h"
#include "ocudu/e2/e2ap_config.h"
#include "ocudu/e2/e2sm/e2sm_manager.h"
#include "ocudu/ran/bcd_helper.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/security/security.h"
#include "ocudu/support/enum_utils.h"
#include <string>
#include <vector>

namespace ocudu {

inline void fill_ran_function_item(ocudulog::basic_logger&        logger,
                                   asn1::e2ap::e2setup_request_s& setup,
                                   const std::string&             ran_oid,
                                   const std::uint32_t&           ran_func_id,
                                   e2sm_interface*                e2_iface)
{
  using namespace asn1::e2ap;
  asn1::protocol_ie_single_container_s<ran_function_item_ies_o> ran_func_item;
  ran_func_item.load_info_obj(ASN1_E2AP_ID_RAN_FUNCTION_ITEM);
  auto& ran_function_item = ran_func_item->ran_function_item();

  ran_func_item.value().ran_function_item().ran_function_id       = ran_func_id;
  ran_func_item.value().ran_function_item().ran_function_revision = 0;
  ran_func_item.value().ran_function_item().ran_function_o_id.from_string(ran_oid);

  ran_function_item.ran_function_definition = e2_iface->get_e2sm_packer().pack_ran_function_description();
  if (ran_function_item.ran_function_definition.size()) {
    setup->ran_functions_added.push_back(ran_func_item);
  } else {
    logger.error("Failed to pack RAN function description");
  }
}

/// Map the domain-model interface type to the ASN.1 enum value.
inline asn1::e2ap::e2node_component_interface_type_e map_interface_type(e2_node_component_interface_type t)
{
  using opts = asn1::e2ap::e2node_component_interface_type_opts;
  switch (t) {
    case e2_node_component_interface_type::ng:
      return opts::ng;
    case e2_node_component_interface_type::xn:
      return opts::xn;
    case e2_node_component_interface_type::e1:
      return opts::e1;
    case e2_node_component_interface_type::f1:
      return opts::f1;
    case e2_node_component_interface_type::w1:
      return opts::w1;
    case e2_node_component_interface_type::s1:
      return opts::s1;
    case e2_node_component_interface_type::x2:
      return opts::x2;
    default:
      return opts::ng;
  }
}

inline void fill_asn1_e2ap_setup_request(ocudulog::basic_logger&                      logger,
                                         asn1::e2ap::e2setup_request_s&               setup,
                                         const e2ap_config&                           e2ap_config,
                                         e2sm_manager&                                e2sm_mngr,
                                         const std::vector<e2_node_component_config>& node_cfgs)
{
  using namespace asn1::e2ap;
  e2_message  e2_msg;
  init_msg_s& initmsg = e2_msg.pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_E2SETUP);
  setup = initmsg.value.e2setup_request();

  //  Transaction ID
  setup->transaction_id = 1;

  // Global e2 node ID
  auto& gnb_id = setup->global_e2node_id.set_gnb();
  gnb_id.global_gnb_id.gnb_id.gnb_id().from_number(e2ap_config.gnb_id.id, e2ap_config.gnb_id.bit_length);
  // convert PLMN to BCD
  uint32_t plmn_bcd = bcd_helper::plmn_string_to_bcd(e2ap_config.plmn);
  gnb_id.global_gnb_id.plmn_id.from_number(plmn_bcd);

  if (e2ap_config.gnb_du_id.has_value()) {
    gnb_id.gnb_du_id_present = true;
    gnb_id.gnb_du_id         = to_underlying(e2ap_config.gnb_du_id.value());
  }

  if (e2ap_config.gnb_cu_up_id.has_value()) {
    gnb_id.gnb_cu_up_id_present = true;
    gnb_id.gnb_cu_up_id         = to_underlying(e2ap_config.gnb_cu_up_id.value());
  }

  // RAN functions added
  if (e2ap_config.e2sm_kpm_enabled) {
    std::string ran_oid     = e2sm_kpm_asn1_packer::oid;
    uint32_t    ran_func_id = e2sm_kpm_asn1_packer::ran_func_id;
    logger.info("Generate RAN function definition for OID: {}", ran_oid.c_str());
    e2sm_interface* e2_iface = e2sm_mngr.get_e2sm_interface(ran_oid);
    if (e2_iface) {
      fill_ran_function_item(logger, setup, ran_oid, ran_func_id, e2_iface);
    } else {
      logger.error("No E2SM interface found for RAN OID {}", ran_oid.c_str());
    }
  }
  if (e2ap_config.e2sm_rc_enabled) {
    std::string ran_oid     = e2sm_rc_asn1_packer::oid;
    uint32_t    ran_func_id = e2sm_rc_asn1_packer::ran_func_id;
    logger.info("Generate RAN function definition for OID: {}", ran_oid.c_str());
    e2sm_interface* e2_iface = e2sm_mngr.get_e2sm_interface(ran_oid);
    if (e2_iface) {
      fill_ran_function_item(logger, setup, ran_oid, ran_func_id, e2_iface);
    } else {
      logger.error("No E2SM interface found for RAN OID {}", ran_oid.c_str());
    }
  }
  if (e2ap_config.e2sm_ccc_enabled) {
    std::string ran_oid     = e2sm_ccc_asn1_packer::oid;
    uint32_t    ran_func_id = e2sm_ccc_asn1_packer::ran_func_id;
    logger.info("Generate RAN function definition for OID: {}", ran_oid.c_str());
    e2sm_interface* e2_iface = e2sm_mngr.get_e2sm_interface(ran_oid);
    if (e2_iface) {
      fill_ran_function_item(logger, setup, ran_oid, ran_func_id, e2_iface);
    } else {
      logger.error("No E2SM interface found for RAN OID {}", ran_oid.c_str());
    }
  }

  // E2 node component config, one list entry per collected setup exchange.
  auto& list = setup->e2node_component_cfg_addition;
  list.resize(node_cfgs.size());
  for (size_t i = 0, n = node_cfgs.size(); i < n; ++i) {
    const e2_node_component_config& cfg_item = node_cfgs[i];

    list[i].load_info_obj(ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM);
    e2node_component_cfg_addition_item_s& e2node_cfg_item = list[i].value().e2node_component_cfg_addition_item();

    e2node_cfg_item.e2node_component_interface_type = map_interface_type(cfg_item.interface_type);

    // Set the component ID based on interface type.
    switch (cfg_item.interface_type) {
      case e2_node_component_interface_type::f1: {
        auto& f1_id = e2node_cfg_item.e2node_component_id.set_e2node_component_interface_type_f1();
        if (std::holds_alternative<gnb_du_id_t>(cfg_item.component_id)) {
          f1_id.gnb_du_id = to_underlying(std::get<gnb_du_id_t>(cfg_item.component_id));
        }
        break;
      }
      case e2_node_component_interface_type::e1: {
        auto& e1_id = e2node_cfg_item.e2node_component_id.set_e2node_component_interface_type_e1();
        if (std::holds_alternative<gnb_cu_up_id_t>(cfg_item.component_id)) {
          e1_id.gnb_cu_up_id = to_underlying(std::get<gnb_cu_up_id_t>(cfg_item.component_id));
        }
        break;
      }
      case e2_node_component_interface_type::ng: {
        const std::string* amf = std::get_if<std::string>(&cfg_item.component_id);
        e2node_cfg_item.e2node_component_id.set_e2node_component_interface_type_ng().amf_name.from_string(
            (amf != nullptr && !amf->empty()) ? *amf : "unknown");
        break;
      }
      default:
        // TODO: add xn/w1/s1/x2 when available.
        break;
    }

    // Copy request/response bytes.
    byte_buffer req_copy  = cfg_item.request_part.copy();
    byte_buffer resp_copy = cfg_item.response_part.copy();

    logger.debug("E2 Setup: node component config [{}] interface_type={} req_bytes={} resp_bytes={}",
                 i,
                 static_cast<int>(cfg_item.interface_type),
                 req_copy.length(),
                 resp_copy.length());

    if (e2node_cfg_item.e2node_component_cfg.e2node_component_request_part.resize(req_copy.length())) {
      std::copy(
          req_copy.begin(), req_copy.end(), e2node_cfg_item.e2node_component_cfg.e2node_component_request_part.begin());
    }
    if (e2node_cfg_item.e2node_component_cfg.e2node_component_resp_part.resize(resp_copy.length())) {
      std::copy(
          resp_copy.begin(), resp_copy.end(), e2node_cfg_item.e2node_component_cfg.e2node_component_resp_part.begin());
    }
  }
}

} // namespace ocudu
