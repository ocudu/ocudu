// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/nrppa/nrppa_asn1_converters.h"
#include "ocudu/adt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

namespace {

asn1::nrppa::srs_res_s valid_asn1_srs_res()
{
  asn1::nrppa::srs_res_s res;
  res.nrof_srs_ports   = asn1::nrppa::srs_res_s::nrof_srs_ports_opts::port1;
  res.nrof_symbols     = asn1::nrppa::srs_res_s::nrof_symbols_opts::n1;
  res.repeat_factor    = asn1::nrppa::srs_res_s::repeat_factor_opts::n1;
  res.group_or_seq_hop = asn1::nrppa::srs_res_s::group_or_seq_hop_opts::neither;
  res.tx_comb.set_n2();
  res.res_type.set_aperiodic();
  return res;
}

asn1::nrppa::pos_srs_res_item_s valid_asn1_pos_srs_res()
{
  asn1::nrppa::pos_srs_res_item_s res;
  res.nrof_symbols     = asn1::nrppa::pos_srs_res_item_s::nrof_symbols_opts::n1;
  res.group_or_seq_hop = asn1::nrppa::pos_srs_res_item_s::group_or_seq_hop_opts::neither;
  res.tx_comb_pos.set_n2();
  res.res_type_pos.set_aperiodic();
  return res;
}

asn1::nrppa::srs_res_set_s valid_asn1_srs_res_set()
{
  asn1::nrppa::srs_res_set_s res_set;
  res_set.res_set_type.set_aperiodic();
  return res_set;
}

asn1::nrppa::pos_srs_res_set_item_s valid_asn1_pos_srs_res_set()
{
  asn1::nrppa::pos_srs_res_set_item_s res_set;
  res_set.posres_set_type.set_aperiodic();
  return res_set;
}

asn1::nrppa::srs_carrier_list_item_s valid_asn1_srs_carrier_list_item()
{
  asn1::nrppa::srs_carrier_list_item_s item;
  item.active_ul_bwp.subcarrier_spacing = asn1::nrppa::active_ul_bwp_s::subcarrier_spacing_opts::khz15;
  item.active_ul_bwp.cp.value           = asn1::nrppa::active_ul_bwp_s::cp_opts::normal;
  return item;
}

prs_cfg_t valid_prs_cfg()
{
  prs_res_item_t res;
  res.prs_res_id        = 0;
  res.seq_id            = 1;
  res.re_offset         = 0;
  res.res_slot_offset   = 0;
  res.res_symbol_offset = 0;

  prs_resource_set_item_t res_set;
  res_set.prs_res_set_id      = 0;
  res_set.scs                 = subcarrier_spacing::kHz30;
  res_set.prs_bw              = 5;
  res_set.start_prb           = 0;
  res_set.point_a             = 620000;
  res_set.comb_size           = 2;
  res_set.cp_type             = cyclic_prefix::NORMAL;
  res_set.res_set_periodicity = 160;
  res_set.res_set_slot_offset = 2;
  res_set.res_repeat_factor   = 1;
  res_set.res_time_gap        = 1;
  res_set.res_numof_symbols   = 12;
  res_set.prs_res_tx_pwr      = 0;
  res_set.prs_res_list.push_back(res);

  prs_cfg_t prs_cfg;
  prs_cfg.prs_res_set_list.push_back(res_set);
  return prs_cfg;
}

trp_information_list_trp_response_item_t make_trp_response_item(trp_information_type_response_item_t info)
{
  trp_information_list_trp_response_item_t item;
  item.trp_info.trp_id = trp_id_t{1};
  item.trp_info.trp_info_type_resp_list.push_back(std::move(info));
  return item;
}

bool packs(const asn1::nrppa::trp_info_list_trp_resp_item_s_& asn1_item)
{
  byte_buffer   buffer;
  asn1::bit_ref bref(buffer);
  return asn1_item.pack(bref) == asn1::OCUDUASN_SUCCESS;
}

} // namespace

TEST(nrppa_asn1_converters_test, when_bandwidth_srs_choice_is_unsupported_then_request_is_dropped)
{
  asn1::nrppa::requested_srs_tx_characteristics_s asn1_req;
  asn1_req.bw.set(asn1::nrppa::bw_srs_c::types_opts::choice_ext);

  ASSERT_FALSE(asn1_to_requested_srs_tx_characteristics(asn1_req).has_value());
}

TEST(nrppa_asn1_converters_test, when_tx_comb_choice_is_unsupported_then_srs_resource_is_dropped)
{
  asn1::nrppa::srs_configuration_s     asn1_cfg;
  asn1::nrppa::srs_carrier_list_item_s carrier = valid_asn1_srs_carrier_list_item();
  asn1::nrppa::srs_res_s               res     = valid_asn1_srs_res();
  res.tx_comb.set_choice_ext();
  carrier.active_ul_bwp.srs_cfg.srs_res_list.push_back(res);
  asn1_cfg.srs_carrier_list.push_back(carrier);

  srs_configuration_t cfg = asn1_to_srs_configuration(asn1_cfg);

  ASSERT_TRUE(cfg.srs_carrier_list[0].active_ul_bwp.srs_cfg.srs_res_list.empty());
}

TEST(nrppa_asn1_converters_test, when_resource_type_choice_is_unsupported_then_srs_resource_is_dropped)
{
  asn1::nrppa::srs_configuration_s     asn1_cfg;
  asn1::nrppa::srs_carrier_list_item_s carrier = valid_asn1_srs_carrier_list_item();
  asn1::nrppa::srs_res_s               res     = valid_asn1_srs_res();
  res.res_type.set_choice_ext();
  carrier.active_ul_bwp.srs_cfg.srs_res_list.push_back(res);
  asn1_cfg.srs_carrier_list.push_back(carrier);

  srs_configuration_t cfg = asn1_to_srs_configuration(asn1_cfg);

  ASSERT_TRUE(cfg.srs_carrier_list[0].active_ul_bwp.srs_cfg.srs_res_list.empty());
}

TEST(nrppa_asn1_converters_test, when_tx_comb_pos_choice_is_unsupported_then_pos_srs_resource_is_dropped)
{
  asn1::nrppa::srs_configuration_s     asn1_cfg;
  asn1::nrppa::srs_carrier_list_item_s carrier = valid_asn1_srs_carrier_list_item();
  asn1::nrppa::pos_srs_res_item_s      res     = valid_asn1_pos_srs_res();
  res.tx_comb_pos.set_choice_ext();
  carrier.active_ul_bwp.srs_cfg.pos_srs_res_list.push_back(res);
  asn1_cfg.srs_carrier_list.push_back(carrier);

  srs_configuration_t cfg = asn1_to_srs_configuration(asn1_cfg);

  ASSERT_TRUE(cfg.srs_carrier_list[0].active_ul_bwp.srs_cfg.pos_srs_res_list.empty());
}

TEST(nrppa_asn1_converters_test, when_resource_type_pos_choice_is_unsupported_then_pos_srs_resource_is_dropped)
{
  asn1::nrppa::srs_configuration_s     asn1_cfg;
  asn1::nrppa::srs_carrier_list_item_s carrier = valid_asn1_srs_carrier_list_item();
  asn1::nrppa::pos_srs_res_item_s      res     = valid_asn1_pos_srs_res();
  res.res_type_pos.set_choice_ext();
  carrier.active_ul_bwp.srs_cfg.pos_srs_res_list.push_back(res);
  asn1_cfg.srs_carrier_list.push_back(carrier);

  srs_configuration_t cfg = asn1_to_srs_configuration(asn1_cfg);

  ASSERT_TRUE(cfg.srs_carrier_list[0].active_ul_bwp.srs_cfg.pos_srs_res_list.empty());
}

TEST(nrppa_asn1_converters_test, when_resource_set_type_choice_is_unsupported_then_srs_resource_set_is_dropped)
{
  asn1::nrppa::srs_configuration_s     asn1_cfg;
  asn1::nrppa::srs_carrier_list_item_s carrier = valid_asn1_srs_carrier_list_item();
  asn1::nrppa::srs_res_set_s           res_set = valid_asn1_srs_res_set();
  res_set.res_set_type.set_choice_ext();
  carrier.active_ul_bwp.srs_cfg.srs_res_set_list.push_back(res_set);
  asn1_cfg.srs_carrier_list.push_back(carrier);

  srs_configuration_t cfg = asn1_to_srs_configuration(asn1_cfg);

  ASSERT_TRUE(cfg.srs_carrier_list[0].active_ul_bwp.srs_cfg.srs_res_set_list.empty());
}

TEST(nrppa_asn1_converters_test, when_pos_resource_set_type_choice_is_unsupported_then_pos_srs_resource_set_is_dropped)
{
  asn1::nrppa::srs_configuration_s     asn1_cfg;
  asn1::nrppa::srs_carrier_list_item_s carrier = valid_asn1_srs_carrier_list_item();
  asn1::nrppa::pos_srs_res_set_item_s  res_set = valid_asn1_pos_srs_res_set();
  res_set.posres_set_type.set_choice_ext();
  carrier.active_ul_bwp.srs_cfg.pos_srs_res_set_list.push_back(res_set);
  asn1_cfg.srs_carrier_list.push_back(carrier);

  srs_configuration_t cfg = asn1_to_srs_configuration(asn1_cfg);

  ASSERT_TRUE(cfg.srs_carrier_list[0].active_ul_bwp.srs_cfg.pos_srs_res_set_list.empty());
}

TEST(nrppa_asn1_converters_test,
     when_spatial_relation_pos_choice_is_unsupported_then_pos_srs_resource_is_kept_without_spatial_relation_info)
{
  asn1::nrppa::srs_configuration_s     asn1_cfg;
  asn1::nrppa::srs_carrier_list_item_s carrier = valid_asn1_srs_carrier_list_item();
  asn1::nrppa::pos_srs_res_item_s      res     = valid_asn1_pos_srs_res();
  res.spatial_relation_pos_present             = true;
  res.spatial_relation_pos.set_choice_ext();
  carrier.active_ul_bwp.srs_cfg.pos_srs_res_list.push_back(res);
  asn1_cfg.srs_carrier_list.push_back(carrier);

  srs_configuration_t cfg = asn1_to_srs_configuration(asn1_cfg);

  ASSERT_EQ(cfg.srs_carrier_list[0].active_ul_bwp.srs_cfg.pos_srs_res_list.size(), 1);
  ASSERT_FALSE(cfg.srs_carrier_list[0].active_ul_bwp.srs_cfg.pos_srs_res_list[0].spatial_relation_info.has_value());
}

TEST(nrppa_asn1_converters_test, when_trp_info_type_item_is_unsupported_then_it_is_dropped)
{
  asn1::nrppa::trp_info_type_item_e asn1_item;

  ASSERT_FALSE(asn1_to_trp_info_type_item(asn1_item).has_value());
}

TEST(nrppa_asn1_converters_test, when_trp_meas_quantities_item_is_unsupported_then_it_is_dropped)
{
  asn1::nrppa::trp_meas_quantities_list_item_s asn1_item;

  ASSERT_FALSE(asn1_to_trp_meas_quantities_list_item(asn1_item).has_value());
}

TEST(nrppa_asn1_converters_test, when_trp_information_has_prs_configuration_then_prs_resource_sets_are_encoded)
{
  asn1::nrppa::trp_info_list_trp_resp_item_s_ asn1_item =
      trp_information_list_trp_response_item_to_asn1(make_trp_response_item(valid_prs_cfg()));

  const asn1::nrppa::trp_info_type_resp_item_c& asn1_info = asn1_item.trp_info.trp_info_type_resp_list[0];
  ASSERT_EQ(asn1_info.type(), asn1::nrppa::trp_info_type_resp_item_c::types_opts::prs_cfg);
  ASSERT_EQ(asn1_info.prs_cfg().prs_res_set_list.size(), 1);
  ASSERT_EQ(asn1_info.prs_cfg().prs_res_set_list[0].subcarrier_spacing.to_number(), 30);
  ASSERT_EQ(asn1_info.prs_cfg().prs_res_set_list[0].prs_res_list.size(), 1);
  ASSERT_TRUE(packs(asn1_item));
}

TEST(nrppa_asn1_converters_test, when_trp_position_is_normal_accuracy_then_trp_information_is_encoded)
{
  geographical_coordinates_t geo_coords;
  geo_coords.trp_position_definition_type = trp_position_direct_t{ng_ran_access_point_position_t{}};

  asn1::nrppa::trp_info_list_trp_resp_item_s_ asn1_item =
      trp_information_list_trp_response_item_to_asn1(make_trp_response_item(geo_coords));

  const asn1::nrppa::trp_info_type_resp_item_c& asn1_info = asn1_item.trp_info.trp_info_type_resp_list[0];
  ASSERT_EQ(asn1_info.geographical_coordinates().trp_position_definition_type.direct().accuracy.type(),
            asn1::nrppa::trp_position_direct_accuracy_c::types_opts::trp_position);
  ASSERT_TRUE(packs(asn1_item));
}

TEST(nrppa_asn1_converters_test, when_trp_position_is_high_accuracy_then_trp_information_is_encoded)
{
  geographical_coordinates_t geo_coords;
  geo_coords.trp_position_definition_type = trp_position_direct_t{ng_ran_high_accuracy_access_point_position_t{}};

  asn1::nrppa::trp_info_list_trp_resp_item_s_ asn1_item =
      trp_information_list_trp_response_item_to_asn1(make_trp_response_item(geo_coords));

  const asn1::nrppa::trp_info_type_resp_item_c& asn1_info = asn1_item.trp_info.trp_info_type_resp_list[0];
  ASSERT_EQ(asn1_info.geographical_coordinates().trp_position_definition_type.direct().accuracy.type(),
            asn1::nrppa::trp_position_direct_accuracy_c::types_opts::trph_aposition);
  ASSERT_TRUE(packs(asn1_item));
}
