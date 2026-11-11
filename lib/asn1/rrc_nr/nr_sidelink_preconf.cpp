// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/nr_sidelink_preconf.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// SL-BWP-PoolConfigCommon-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_bwp_pool_cfg_common_r16_s_fields[] = {
    {offsetof(sl_bwp_pool_cfg_common_r16_s, sl_rx_pool_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(sl_bwp_pool_cfg_common_r16_s::sl_rx_pool_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_res_pool_r16_s::desc},
    {offsetof(sl_bwp_pool_cfg_common_r16_s, sl_tx_pool_sel_normal_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sl_bwp_pool_cfg_common_r16_s::sl_tx_pool_sel_normal_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_res_pool_cfg_r16_s::desc},
    {offsetof(sl_bwp_pool_cfg_common_r16_s, sl_tx_pool_exceptional_r16),
     (int32_t)offsetof(sl_bwp_pool_cfg_common_r16_s, sl_tx_pool_exceptional_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_res_pool_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_pool_cfg_common_r16_s_names[] = {
    {"sl-RxPool-r16", nullptr},
    {"sl-TxPoolSelectedNormal-r16", nullptr},
    {"sl-TxPoolExceptional-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_pool_cfg_common_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(sl_bwp_pool_cfg_common_r16_s, sl_tx_pool_exceptional_r16_present)}};
const asn1::table::seq_desc sl_bwp_pool_cfg_common_r16_s::desc = {
    {seq_table_sl_bwp_pool_cfg_common_r16_s_fields, 3, UINT64_C(0x0), seq_table_sl_bwp_pool_cfg_common_r16_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_bwp_pool_cfg_common_r16_s_names};

// SL-BWP-DiscPoolConfigCommon-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_bwp_disc_pool_cfg_common_r17_s_fields[] = {
    {offsetof(sl_bwp_disc_pool_cfg_common_r17_s, sl_disc_rx_pool_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(sl_bwp_disc_pool_cfg_common_r17_s::sl_disc_rx_pool_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_res_pool_r16_s::desc},
    {offsetof(sl_bwp_disc_pool_cfg_common_r17_s, sl_disc_tx_pool_sel_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sl_bwp_disc_pool_cfg_common_r17_s::sl_disc_tx_pool_sel_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_res_pool_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_disc_pool_cfg_common_r17_s_names[] = {
    {"sl-DiscRxPool-r17", nullptr},
    {"sl-DiscTxPoolSelected-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_disc_pool_cfg_common_r17_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_bwp_disc_pool_cfg_common_r17_s::desc = {
    {seq_table_sl_bwp_disc_pool_cfg_common_r17_s_fields,
     2,
     UINT64_C(0x0),
     seq_table_sl_bwp_disc_pool_cfg_common_r17_s_opts,
     2},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_bwp_disc_pool_cfg_common_r17_s_names};

// SL-BWP-ConfigCommon-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_bwp_cfg_common_r16_s_fields[] = {
    {offsetof(sl_bwp_cfg_common_r16_s, sl_bwp_generic_r16),
     (int32_t)offsetof(sl_bwp_cfg_common_r16_s, sl_bwp_generic_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_generic_r16_s::desc},
    {offsetof(sl_bwp_cfg_common_r16_s, sl_bwp_pool_cfg_common_r16),
     (int32_t)offsetof(sl_bwp_cfg_common_r16_s, sl_bwp_pool_cfg_common_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_pool_cfg_common_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_common_r16_s_names[] = {
    {"sl-BWP-Generic-r16", nullptr},
    {"sl-BWP-PoolConfigCommon-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_cfg_common_r16_s_opts[] = {
    {0, (int32_t)offsetof(sl_bwp_cfg_common_r16_s, sl_bwp_generic_r16_present)},
    {1, (int32_t)offsetof(sl_bwp_cfg_common_r16_s, sl_bwp_pool_cfg_common_r16_present)}};
static const asn1::table::seq_field seq_table_sl_bwp_cfg_common_r16_s_g0_fields[] = {
    {offsetof(sl_bwp_cfg_common_r16_s, sl_bwp_pool_cfg_common_ps_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_bwp_cfg_common_r16_s::sl_bwp_pool_cfg_common_ps_r17)>::value,
     &sl_bwp_pool_cfg_common_r16_s::desc},
    {offsetof(sl_bwp_cfg_common_r16_s, sl_bwp_disc_pool_cfg_common_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_bwp_cfg_common_r16_s::sl_bwp_disc_pool_cfg_common_r17)>::value,
     &sl_bwp_disc_pool_cfg_common_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_common_r16_s_g0_names[] = {
    {"sl-BWP-PoolConfigCommonPS-r17", nullptr},
    {"sl-BWP-DiscPoolConfigCommon-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_cfg_common_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sl_bwp_cfg_common_r16_s_g1_fields[] = {
    {offsetof(sl_bwp_cfg_common_r16_s, sl_bwp_pool_cfg_common_a2_x_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_bwp_cfg_common_r16_s::sl_bwp_pool_cfg_common_a2_x_r18)>::value,
     &sl_bwp_pool_cfg_common_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_common_r16_s_g1_names[] = {
    {"sl-BWP-PoolConfigCommonA2X-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_cfg_common_r16_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_bwp_cfg_common_r16_s_groups[] = {
    {{seq_table_sl_bwp_cfg_common_r16_s_g0_fields, 2, UINT64_C(0x0), seq_table_sl_bwp_cfg_common_r16_s_g0_opts, 2},
     false,
     seq_table_sl_bwp_cfg_common_r16_s_g0_names},
    {{seq_table_sl_bwp_cfg_common_r16_s_g1_fields, 1, UINT64_C(0x0), seq_table_sl_bwp_cfg_common_r16_s_g1_opts, 1},
     false,
     seq_table_sl_bwp_cfg_common_r16_s_g1_names}};
const asn1::table::seq_desc sl_bwp_cfg_common_r16_s::desc = {
    {seq_table_sl_bwp_cfg_common_r16_s_fields, 2, UINT64_C(0x0), seq_table_sl_bwp_cfg_common_r16_s_opts, 2},
    true,
    false,
    seq_table_sl_bwp_cfg_common_r16_s_groups,
    2,
    seq_table_sl_bwp_cfg_common_r16_s_names};

// SL-BWP-PRS-PoolConfigCommon-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_bwp_prs_pool_cfg_common_r18_s_fields[] = {
    {offsetof(sl_bwp_prs_pool_cfg_common_r18_s, sl_prs_rx_pool_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(sl_bwp_prs_pool_cfg_common_r18_s::sl_prs_rx_pool_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_prs_res_pool_r18_s::desc},
    {offsetof(sl_bwp_prs_pool_cfg_common_r18_s, sl_prs_tx_pool_sel_normal_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sl_bwp_prs_pool_cfg_common_r18_s::sl_prs_tx_pool_sel_normal_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_prs_res_pool_cfg_r18_s::desc},
    {offsetof(sl_bwp_prs_pool_cfg_common_r18_s, sl_prs_tx_pool_exceptional_r18),
     (int32_t)offsetof(sl_bwp_prs_pool_cfg_common_r18_s, sl_prs_tx_pool_exceptional_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_prs_res_pool_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_prs_pool_cfg_common_r18_s_names[] = {
    {"sl-PRS-RxPool-r18", nullptr},
    {"sl-PRS-TxPoolSelectedNormal-r18", nullptr},
    {"sl-PRS-TxPoolExceptional-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_prs_pool_cfg_common_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(sl_bwp_prs_pool_cfg_common_r18_s, sl_prs_tx_pool_exceptional_r18_present)}};
const asn1::table::seq_desc sl_bwp_prs_pool_cfg_common_r18_s::desc = {
    {seq_table_sl_bwp_prs_pool_cfg_common_r18_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_sl_bwp_prs_pool_cfg_common_r18_s_opts,
     3},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_bwp_prs_pool_cfg_common_r18_s_names};

// SL-PosBWP-ConfigCommon-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pos_bwp_cfg_common_r18_s_fields[] = {
    {offsetof(sl_pos_bwp_cfg_common_r18_s, sl_bwp_generic_r18),
     (int32_t)offsetof(sl_pos_bwp_cfg_common_r18_s, sl_bwp_generic_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_generic_r16_s::desc},
    {offsetof(sl_pos_bwp_cfg_common_r18_s, sl_bwp_prs_pool_cfg_common_r18),
     (int32_t)offsetof(sl_pos_bwp_cfg_common_r18_s, sl_bwp_prs_pool_cfg_common_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_prs_pool_cfg_common_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_pos_bwp_cfg_common_r18_s_names[] = {
    {"sl-BWP-Generic-r18", nullptr},
    {"sl-BWP-PRS-PoolConfigCommon-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pos_bwp_cfg_common_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_pos_bwp_cfg_common_r18_s, sl_bwp_generic_r18_present)},
    {1, (int32_t)offsetof(sl_pos_bwp_cfg_common_r18_s, sl_bwp_prs_pool_cfg_common_r18_present)}};
const asn1::table::seq_desc sl_pos_bwp_cfg_common_r18_s::desc = {
    {seq_table_sl_pos_bwp_cfg_common_r18_s_fields, 2, UINT64_C(0x0), seq_table_sl_pos_bwp_cfg_common_r18_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_pos_bwp_cfg_common_r18_s_names};

// SL-FreqConfigCommon-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__fields[] = {
    {0,
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
                       absence_of_any_other_technology_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_, sl_freq_sel_cfg_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<
         decltype(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_freq_sel_cfg_list_r18),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &sl_freq_sel_cfg_r18_s::desc},
    {0,
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_, sl_sync_tx_disabled_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_, sl_energy_detection_cfg_r18),
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
                       sl_energy_detection_cfg_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::desc},
    {offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_, ue_to_ue_cot_sharing_ed_thres_r18),
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
                       ue_to_ue_cot_sharing_ed_thres_r18_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-85),
     UINT64_C(-52),
     nullptr},
    {offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
              harq_ack_feedback_ratiofor_cw_adjustment_gc_option2_r18),
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
                       harq_ack_feedback_ratiofor_cw_adjustment_gc_option2_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(10),
     UINT64_C(100),
     nullptr}};
static const asn1::table::field_name seq_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__names[] = {
    {"absenceOfAnyOtherTechnology-r18", "true"},
    {"sl-FreqSelectionConfigList-r18", nullptr},
    {"sl-SyncTxDisabled-r18", "true"},
    {"sl-EnergyDetectionConfig-r18", nullptr},
    {"ue-ToUE-COT-SharingED-Threshold-r18", nullptr},
    {"harq-ACK-FeedbackRatioforCW-AdjustmentGC-Option2-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__opts[] = {
    {0,
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
                       absence_of_any_other_technology_r18_present)},
    {1, asn1::table::seq_field::list_presence},
    {2,
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
                       sl_sync_tx_disabled_r18_present)},
    {3,
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
                       sl_energy_detection_cfg_r18_present)},
    {4,
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
                       ue_to_ue_cot_sharing_ed_thres_r18_present)},
    {5,
     (int32_t)offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_,
                       harq_ack_feedback_ratiofor_cw_adjustment_gc_option2_r18_present)}};
static const asn1::table::seq_desc seq_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s_ = {
    {seq_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__fields,
     6,
     UINT64_C(0x0),
     seq_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__opts,
     6},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__names};
static const asn1::table::seq_field seq_table_sl_freq_cfg_common_r16_s_fields[] = {
    {offsetof(sl_freq_cfg_common_r16_s, sl_scs_specific_carrier_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     5,
     &asn1::table::item_list_codec<decltype(sl_freq_cfg_common_r16_s::sl_scs_specific_carrier_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &scs_specific_carrier_s::desc},
    {offsetof(sl_freq_cfg_common_r16_s, sl_absolute_freq_point_a_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(sl_freq_cfg_common_r16_s, sl_absolute_freq_ssb_r16),
     (int32_t)offsetof(sl_freq_cfg_common_r16_s, sl_absolute_freq_ssb_r16_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {0,
     (int32_t)offsetof(sl_freq_cfg_common_r16_s, freq_shift7p5khz_sl_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_freq_cfg_common_r16_s, value_n_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-1),
     UINT64_C(1),
     nullptr},
    {offsetof(sl_freq_cfg_common_r16_s, sl_bwp_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(sl_freq_cfg_common_r16_s::sl_bwp_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_bwp_cfg_common_r16_s::desc},
    {offsetof(sl_freq_cfg_common_r16_s, sl_sync_prio_r16),
     (int32_t)offsetof(sl_freq_cfg_common_r16_s, sl_sync_prio_r16_present),
     asn1::table::enum_op<decltype(sl_freq_cfg_common_r16_s::sl_sync_prio_r16)>(),
     decltype(sl_freq_cfg_common_r16_s::sl_sync_prio_r16)::has_ext,
     false,
     decltype(sl_freq_cfg_common_r16_s::sl_sync_prio_r16)::nof_types,
     decltype(sl_freq_cfg_common_r16_s::sl_sync_prio_r16)::nof_exts,
     nullptr,
     &sl_freq_cfg_common_r16_s::sl_sync_prio_r16_opts::desc},
    {offsetof(sl_freq_cfg_common_r16_s, sl_nb_as_sync_r16),
     (int32_t)offsetof(sl_freq_cfg_common_r16_s, sl_nb_as_sync_r16_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_freq_cfg_common_r16_s, sl_sync_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(sl_freq_cfg_common_r16_s::sl_sync_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_sync_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_freq_cfg_common_r16_s_names[] = {
    {"sl-SCS-SpecificCarrierList-r16", nullptr},
    {"sl-AbsoluteFrequencyPointA-r16", nullptr},
    {"sl-AbsoluteFrequencySSB-r16", nullptr},
    {"frequencyShift7p5khzSL-r16", "true"},
    {"valueN-r16", nullptr},
    {"sl-BWP-List-r16", nullptr},
    {"sl-SyncPriority-r16", nullptr},
    {"sl-NbAsSync-r16", nullptr},
    {"sl-SyncConfigList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_freq_cfg_common_r16_s_opts[] = {
    {2, (int32_t)offsetof(sl_freq_cfg_common_r16_s, sl_absolute_freq_ssb_r16_present)},
    {3, (int32_t)offsetof(sl_freq_cfg_common_r16_s, freq_shift7p5khz_sl_r16_present)},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(sl_freq_cfg_common_r16_s, sl_sync_prio_r16_present)},
    {7, (int32_t)offsetof(sl_freq_cfg_common_r16_s, sl_nb_as_sync_r16_present)},
    {8, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_sl_freq_cfg_common_r16_s_g0_fields[] = {
    {offsetof(sl_freq_cfg_common_r16_s, sl_unlicensed_freq_cfg_common_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18)>::value,
     &seq_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s_},
    {offsetof(sl_freq_cfg_common_r16_s, sl_pos_bwp_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::ptr_item_list_codec<decltype(sl_freq_cfg_common_r16_s::sl_pos_bwp_list_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_pos_bwp_cfg_common_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_freq_cfg_common_r16_s_g0_names[] = {
    {"sl-UnlicensedFreqConfigCommon-r18", nullptr},
    {"sl-PosBWP-List-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_freq_cfg_common_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sl_freq_cfg_common_r16_s_g1_fields[] = {
    {offsetof(sl_freq_cfg_common_r16_s, add_spec_emission_v1860),
     (int32_t)offsetof(sl_freq_cfg_common_r16_s, add_spec_emission_v1860_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(8),
     UINT64_C(39),
     nullptr}};
static const asn1::table::field_name seq_table_sl_freq_cfg_common_r16_s_g1_names[] = {
    {"additionalSpectrumEmission-v1860", nullptr}};
static const asn1::table::seq_opt seq_table_sl_freq_cfg_common_r16_s_g1_opts[] = {
    {0, (int32_t)offsetof(sl_freq_cfg_common_r16_s, add_spec_emission_v1860_present)}};
static const asn1::table::seq_group seq_table_sl_freq_cfg_common_r16_s_groups[] = {
    {{seq_table_sl_freq_cfg_common_r16_s_g0_fields, 2, UINT64_C(0x0), seq_table_sl_freq_cfg_common_r16_s_g0_opts, 2},
     false,
     seq_table_sl_freq_cfg_common_r16_s_g0_names},
    {{seq_table_sl_freq_cfg_common_r16_s_g1_fields, 1, UINT64_C(0x0), seq_table_sl_freq_cfg_common_r16_s_g1_opts, 1},
     false,
     seq_table_sl_freq_cfg_common_r16_s_g1_names}};
const asn1::table::seq_desc sl_freq_cfg_common_r16_s::desc = {
    {seq_table_sl_freq_cfg_common_r16_s_fields, 9, UINT64_C(0x13), seq_table_sl_freq_cfg_common_r16_s_opts, 6},
    true,
    false,
    seq_table_sl_freq_cfg_common_r16_s_groups,
    2,
    seq_table_sl_freq_cfg_common_r16_s_names};

static const char*           enum_names_sl_freq_cfg_common_r16_s_sl_sync_prio_r16_e__values[] = {"gnss", "gnbEnb"};
const asn1::table::enum_desc sl_freq_cfg_common_r16_s::sl_sync_prio_r16_opts::desc            = {
    enum_names_sl_freq_cfg_common_r16_s_sl_sync_prio_r16_e__values,
    2,
    "sl_freq_cfg_common_r16_s::sl_sync_prio_r16_e_"};

static const asn1::table::storage_ops* const
    choice_storage_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__sl_energy_detection_cfg_r18_c__alternatives
        [] = {&asn1::table::alternative_ops<int8_t, true>::value, &asn1::table::alternative_ops<int8_t, true>::value};
const asn1::table::choice_storage_desc
    sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::storage_desc = {
        choice_storage_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__sl_energy_detection_cfg_r18_c__alternatives,
        2,
        "sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_"};

int8_t& sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::
    set_sl_max_energy_detection_thres_r18()
{
  set(types::sl_max_energy_detection_thres_r18);
  return c.get<int8_t>();
}
int8_t& sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::
    set_sl_energy_detection_thres_offset_r18()
{
  set(types::sl_energy_detection_thres_offset_r18);
  return c.get<int8_t>();
}
static const asn1::table::seq_field
    choice_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__sl_energy_detection_cfg_r18_c__alternatives
        [] = {{0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::int_i8,
               false,
               false,
               INT64_C(-85),
               UINT64_C(-52),
               nullptr},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::int_i8,
               false,
               false,
               INT64_C(-13),
               UINT64_C(20),
               nullptr}};
static const asn1::table::field_name
    choice_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__sl_energy_detection_cfg_r18_c__names[] =
        {{"sl-MaxEnergyDetectionThreshold-r18", nullptr}, {"sl-EnergyDetectionThresholdOffset-r18", nullptr}};
const asn1::table::choice_desc
    sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::desc = {
        asn1::table::enum_int_type<
            sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::types>(),
        sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::types::
            nof_types,
        sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::types::nof_exts,
        sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::types::has_ext,
        false,
        choice_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__sl_energy_detection_cfg_r18_c__alternatives,
        nullptr,
        choice_table_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__sl_energy_detection_cfg_r18_c__names,
        "sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_",
        offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_,
                 c.type),
        offsetof(sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_,
                 c.value),
        false,
        &sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::storage_desc};

static const char*
    enum_names_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__sl_energy_detection_cfg_r18_c__types_values
        [] = {"sl-MaxEnergyDetectionThreshold-r18", "sl-EnergyDetectionThresholdOffset-r18"};
const asn1::table::enum_desc
    sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::types_opts::desc = {
        enum_names_sl_freq_cfg_common_r16_s_sl_unlicensed_freq_cfg_common_r18_s__sl_energy_detection_cfg_r18_c__types_values,
        2,
        "sl_freq_cfg_common_r16_s::sl_unlicensed_freq_cfg_common_r18_s_::sl_energy_detection_cfg_r18_c_::types"};

// SL-MeasConfigCommon-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_cfg_common_r16_s_fields[] = {
    {offsetof(sl_meas_cfg_common_r16_s, sl_meas_obj_list_common_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(sl_meas_cfg_common_r16_s::sl_meas_obj_list_common_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_meas_obj_info_r16_s::desc},
    {offsetof(sl_meas_cfg_common_r16_s, sl_report_cfg_list_common_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(sl_meas_cfg_common_r16_s::sl_report_cfg_list_common_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_report_cfg_info_r16_s::desc},
    {offsetof(sl_meas_cfg_common_r16_s, sl_meas_id_list_common_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(sl_meas_cfg_common_r16_s::sl_meas_id_list_common_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_meas_id_info_r16_s::desc},
    {offsetof(sl_meas_cfg_common_r16_s, sl_quant_cfg_common_r16),
     (int32_t)offsetof(sl_meas_cfg_common_r16_s, sl_quant_cfg_common_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_quant_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_meas_cfg_common_r16_s_names[] = {
    {"sl-MeasObjectListCommon-r16", nullptr},
    {"sl-ReportConfigListCommon-r16", nullptr},
    {"sl-MeasIdListCommon-r16", nullptr},
    {"sl-QuantityConfigCommon-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_meas_cfg_common_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(sl_meas_cfg_common_r16_s, sl_quant_cfg_common_r16_present)}};
const asn1::table::seq_desc sl_meas_cfg_common_r16_s::desc = {
    {seq_table_sl_meas_cfg_common_r16_s_fields, 4, UINT64_C(0x0), seq_table_sl_meas_cfg_common_r16_s_opts, 4},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_meas_cfg_common_r16_s_names};

// SL-FreqConfigCommonExt-v16k0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_freq_cfg_common_ext_v16k0_s_fields[] = {
    {offsetof(sl_freq_cfg_common_ext_v16k0_s, add_spec_emission_r16),
     (int32_t)offsetof(sl_freq_cfg_common_ext_v16k0_s, add_spec_emission_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_sl_freq_cfg_common_ext_v16k0_s_names[] = {
    {"additionalSpectrumEmission-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_freq_cfg_common_ext_v16k0_s_opts[] = {
    {0, (int32_t)offsetof(sl_freq_cfg_common_ext_v16k0_s, add_spec_emission_r16_present)}};
const asn1::table::seq_desc sl_freq_cfg_common_ext_v16k0_s::desc = {{seq_table_sl_freq_cfg_common_ext_v16k0_s_fields,
                                                                     1,
                                                                     UINT64_C(0x0),
                                                                     seq_table_sl_freq_cfg_common_ext_v16k0_s_opts,
                                                                     1},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_sl_freq_cfg_common_ext_v16k0_s_names};

// SL-PreconfigDiscConfig-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_precfg_disc_cfg_v1800_s_fields[] = {
    {offsetof(sl_precfg_disc_cfg_v1800_s, sl_relay_ue_precfg_u2_u_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_relay_ue_cfg_u2_u_r18_s::desc},
    {offsetof(sl_precfg_disc_cfg_v1800_s, sl_remote_ue_precfg_u2_u_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_remote_ue_cfg_u2_u_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_precfg_disc_cfg_v1800_s_names[] = {
    {"sl-RelayUE-PreconfigU2U-r18", nullptr},
    {"sl-RemoteUE-PreconfigU2U-r18", nullptr}};
const asn1::table::seq_desc sl_precfg_disc_cfg_v1800_s::desc = {
    {seq_table_sl_precfg_disc_cfg_v1800_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_precfg_disc_cfg_v1800_s_names};

// SL-PreconfigDiscConfig-v1840 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_precfg_disc_cfg_v1840_s_fields[] = {
    {offsetof(sl_precfg_disc_cfg_v1840_s, sl_relay_ue_precfg_u2_u_v1840),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_relay_ue_cfg_u2_u_v1840_s::desc},
    {offsetof(sl_precfg_disc_cfg_v1840_s, sl_remote_ue_precfg_u2_u_v1840),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_remote_ue_cfg_u2_u_v1830_s::desc}};
static const asn1::table::field_name seq_table_sl_precfg_disc_cfg_v1840_s_names[] = {
    {"sl-RelayUE-PreconfigU2U-v1840", nullptr},
    {"sl-RemoteUE-PreconfigU2U-v1840", nullptr}};
const asn1::table::seq_desc sl_precfg_disc_cfg_v1840_s::desc = {
    {seq_table_sl_precfg_disc_cfg_v1840_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_precfg_disc_cfg_v1840_s_names};

// SL-PreconfigGeneral-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_precfg_general_r16_s_fields[] = {
    {offsetof(sl_precfg_general_r16_s, sl_tdd_cfg_r16),
     (int32_t)offsetof(sl_precfg_general_r16_s, sl_tdd_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_cfg_common_s::desc},
    {offsetof(sl_precfg_general_r16_s, reserved_bits_r16),
     (int32_t)offsetof(sl_precfg_general_r16_s, reserved_bits_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_precfg_general_r16_s::reserved_bits_r16),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_sl_precfg_general_r16_s_names[] = {{"sl-TDD-Configuration-r16", nullptr},
                                                                                  {"reservedBits-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_precfg_general_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_precfg_general_r16_s, sl_tdd_cfg_r16_present)},
    {1, (int32_t)offsetof(sl_precfg_general_r16_s, reserved_bits_r16_present)}};
const asn1::table::seq_desc sl_precfg_general_r16_s::desc = {
    {seq_table_sl_precfg_general_r16_s_fields, 2, UINT64_C(0x0), seq_table_sl_precfg_general_r16_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_precfg_general_r16_s_names};

// SL-RoHC-Profiles-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_ro_hc_profiles_r16_s_fields[] = {
    {offsetof(sl_ro_hc_profiles_r16_s, profile0x0001_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_ro_hc_profiles_r16_s, profile0x0002_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_ro_hc_profiles_r16_s, profile0x0003_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_ro_hc_profiles_r16_s, profile0x0004_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_ro_hc_profiles_r16_s, profile0x0006_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_ro_hc_profiles_r16_s, profile0x0101_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_ro_hc_profiles_r16_s, profile0x0102_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_ro_hc_profiles_r16_s, profile0x0103_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_ro_hc_profiles_r16_s, profile0x0104_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_ro_hc_profiles_r16_s_names[] = {{"profile0x0001-r16", nullptr},
                                                                                  {"profile0x0002-r16", nullptr},
                                                                                  {"profile0x0003-r16", nullptr},
                                                                                  {"profile0x0004-r16", nullptr},
                                                                                  {"profile0x0006-r16", nullptr},
                                                                                  {"profile0x0101-r16", nullptr},
                                                                                  {"profile0x0102-r16", nullptr},
                                                                                  {"profile0x0103-r16", nullptr},
                                                                                  {"profile0x0104-r16", nullptr}};
const asn1::table::seq_desc          sl_ro_hc_profiles_r16_s::desc             = {
    {seq_table_sl_ro_hc_profiles_r16_s_fields, 9, UINT64_C(0x1ff), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_ro_hc_profiles_r16_s_names};

// SL-TxProfile-r17 ::= ENUMERATED
static const char* enum_names_sl_tx_profile_r17_e_values[] =
    {"drx-Compatible", "drx-Incompatible", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_tx_profile_r17_opts::desc = {enum_names_sl_tx_profile_r17_e_values,
                                                             8,
                                                             "sl_tx_profile_r17_e"};

// SidelinkPreconfigNR-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sidelink_precfg_nr_r16_s_fields[] = {
    {offsetof(sidelink_precfg_nr_r16_s, sl_precfg_freq_info_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sidelink_precfg_nr_r16_s::sl_precfg_freq_info_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_freq_cfg_common_r16_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_precfg_nr_anchor_carrier_freq_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(sidelink_precfg_nr_r16_s::sl_precfg_nr_anchor_carrier_freq_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(3279165), false, false>>::value},
    {offsetof(sidelink_precfg_nr_r16_s, sl_precfg_eutra_anchor_carrier_freq_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(sidelink_precfg_nr_r16_s::sl_precfg_eutra_anchor_carrier_freq_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(262143), false, false>>::value},
    {offsetof(sidelink_precfg_nr_r16_s, sl_radio_bearer_pre_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(sidelink_precfg_nr_r16_s::sl_radio_bearer_pre_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_radio_bearer_cfg_r16_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_rlc_bearer_pre_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(sidelink_precfg_nr_r16_s::sl_rlc_bearer_pre_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_rlc_bearer_cfg_r16_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_meas_pre_cfg_r16),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_meas_pre_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_cfg_common_r16_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_offset_dfn_r16),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_offset_dfn_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(1000),
     nullptr},
    {offsetof(sidelink_precfg_nr_r16_s, t400_r16),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, t400_r16_present),
     asn1::table::enum_op<decltype(sidelink_precfg_nr_r16_s::t400_r16)>(),
     decltype(sidelink_precfg_nr_r16_s::t400_r16)::has_ext,
     false,
     decltype(sidelink_precfg_nr_r16_s::t400_r16)::nof_types,
     decltype(sidelink_precfg_nr_r16_s::t400_r16)::nof_exts,
     nullptr,
     &sidelink_precfg_nr_r16_s::t400_r16_opts::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_max_num_consecutive_dtx_r16),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_max_num_consecutive_dtx_r16_present),
     asn1::table::enum_op<decltype(sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16)>(),
     decltype(sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16)::has_ext,
     false,
     decltype(sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16)::nof_types,
     decltype(sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16)::nof_exts,
     nullptr,
     &sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_ssb_prio_nr_r16),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_ssb_prio_nr_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(sidelink_precfg_nr_r16_s, sl_precfg_general_r16),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_precfg_general_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_precfg_general_r16_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_ue_sel_pre_cfg_r16),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_ue_sel_pre_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ue_sel_cfg_r16_s::desc},
    {0,
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_csi_acquisition_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sidelink_precfg_nr_r16_s, sl_ro_hc_profiles_r16),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_ro_hc_profiles_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ro_hc_profiles_r16_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_max_c_id_r16),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_max_c_id_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(16383),
     nullptr}};
static const asn1::table::field_name seq_table_sidelink_precfg_nr_r16_s_names[] = {
    {"sl-PreconfigFreqInfoList-r16", nullptr},
    {"sl-PreconfigNR-AnchorCarrierFreqList-r16", nullptr},
    {"sl-PreconfigEUTRA-AnchorCarrierFreqList-r16", nullptr},
    {"sl-RadioBearerPreConfigList-r16", nullptr},
    {"sl-RLC-BearerPreConfigList-r16", nullptr},
    {"sl-MeasPreConfig-r16", nullptr},
    {"sl-OffsetDFN-r16", nullptr},
    {"t400-r16", nullptr},
    {"sl-MaxNumConsecutiveDTX-r16", nullptr},
    {"sl-SSB-PriorityNR-r16", nullptr},
    {"sl-PreconfigGeneral-r16", nullptr},
    {"sl-UE-SelectedPreConfig-r16", nullptr},
    {"sl-CSI-Acquisition-r16", "enabled"},
    {"sl-RoHC-Profiles-r16", nullptr},
    {"sl-MaxCID-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sidelink_precfg_nr_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_meas_pre_cfg_r16_present)},
    {6, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_offset_dfn_r16_present)},
    {7, (int32_t)offsetof(sidelink_precfg_nr_r16_s, t400_r16_present)},
    {8, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_max_num_consecutive_dtx_r16_present)},
    {9, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_ssb_prio_nr_r16_present)},
    {10, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_precfg_general_r16_present)},
    {11, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_ue_sel_pre_cfg_r16_present)},
    {12, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_csi_acquisition_r16_present)},
    {13, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_ro_hc_profiles_r16_present)},
    {14, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_max_c_id_r16_present)}};
static const asn1::table::seq_field seq_table_sidelink_precfg_nr_r16_s_g0_fields[] = {
    {offsetof(sidelink_precfg_nr_r16_s, sl_drx_pre_cfg_gc_bc_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sidelink_precfg_nr_r16_s::sl_drx_pre_cfg_gc_bc_r17)>::value,
     &sl_drx_cfg_gc_bc_r17_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_tx_profile_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     256,
     &asn1::table::ptr_list_codec<decltype(sidelink_precfg_nr_r16_s::sl_tx_profile_list_r17),
                                  asn1::table::string_json>::value},
    {offsetof(sidelink_precfg_nr_r16_s, sl_precfg_disc_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sidelink_precfg_nr_r16_s::sl_precfg_disc_cfg_r17)>::value,
     &sl_remote_ue_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sidelink_precfg_nr_r16_s_g0_names[] = {
    {"sl-DRX-PreConfigGC-BC-r17", nullptr},
    {"sl-TxProfileList-r17", nullptr},
    {"sl-PreconfigDiscConfig-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sidelink_precfg_nr_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sidelink_precfg_nr_r16_s_g1_fields[] = {
    {offsetof(sidelink_precfg_nr_r16_s, sl_precfg_freq_info_list_size_ext_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_item_list_codec<decltype(sidelink_precfg_nr_r16_s::sl_precfg_freq_info_list_size_ext_v1800),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_freq_cfg_common_r16_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_rlc_bearer_cfg_list_size_ext_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::ptr_item_list_codec<decltype(sidelink_precfg_nr_r16_s::sl_rlc_bearer_cfg_list_size_ext_v1800),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_rlc_bearer_cfg_r16_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_sync_freq_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_list_codec<decltype(sidelink_precfg_nr_r16_s::sl_sync_freq_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(1), UINT64_C(8), false, false>>::value},
    {0,
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_sync_tx_multi_freq_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sidelink_precfg_nr_r16_s, sl_precfg_disc_cfg_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sidelink_precfg_nr_r16_s::sl_precfg_disc_cfg_v1800)>::value,
     &sl_precfg_disc_cfg_v1800_s::desc},
    {offsetof(sidelink_precfg_nr_r16_s, sl_pos_precfg_freq_info_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(sidelink_precfg_nr_r16_s::sl_pos_precfg_freq_info_list_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_freq_cfg_common_r16_s::desc}};
static const asn1::table::field_name seq_table_sidelink_precfg_nr_r16_s_g1_names[] = {
    {"sl-PreconfigFreqInfoListSizeExt-v1800", nullptr},
    {"sl-RLC-BearerConfigListSizeExt-v1800", nullptr},
    {"sl-SyncFreqList-r18", nullptr},
    {"sl-SyncTxMultiFreq-r18", "true"},
    {"sl-PreconfigDiscConfig-v1800", nullptr},
    {"sl-PosPreconfigFreqInfoList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sidelink_precfg_nr_r16_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(sidelink_precfg_nr_r16_s, sl_sync_tx_multi_freq_r18_present)},
    {4, asn1::table::seq_field::ptr_presence},
    {5, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sidelink_precfg_nr_r16_s_g2_fields[] = {
    {offsetof(sidelink_precfg_nr_r16_s, t400_u2_u_r18),
     (int32_t)offsetof(sidelink_precfg_nr_r16_s, t400_u2_u_r18_present),
     asn1::table::enum_op<decltype(sidelink_precfg_nr_r16_s::t400_u2_u_r18)>(),
     decltype(sidelink_precfg_nr_r16_s::t400_u2_u_r18)::has_ext,
     false,
     decltype(sidelink_precfg_nr_r16_s::t400_u2_u_r18)::nof_types,
     decltype(sidelink_precfg_nr_r16_s::t400_u2_u_r18)::nof_exts,
     nullptr,
     &sidelink_precfg_nr_r16_s::t400_u2_u_r18_opts::desc}};
static const asn1::table::field_name seq_table_sidelink_precfg_nr_r16_s_g2_names[] = {{"t400-U2U-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sidelink_precfg_nr_r16_s_g2_opts[]  = {
    {0, (int32_t)offsetof(sidelink_precfg_nr_r16_s, t400_u2_u_r18_present)}};
static const asn1::table::seq_field seq_table_sidelink_precfg_nr_r16_s_g3_fields[] = {
    {offsetof(sidelink_precfg_nr_r16_s, sl_precfg_disc_cfg_v1840),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sidelink_precfg_nr_r16_s::sl_precfg_disc_cfg_v1840)>::value,
     &sl_precfg_disc_cfg_v1840_s::desc}};
static const asn1::table::field_name seq_table_sidelink_precfg_nr_r16_s_g3_names[] = {
    {"sl-PreconfigDiscConfig-v1840", nullptr}};
static const asn1::table::seq_opt seq_table_sidelink_precfg_nr_r16_s_g3_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sidelink_precfg_nr_r16_s_g4_fields[] = {
    {offsetof(sidelink_precfg_nr_r16_s, sl_pos_precfg_freq_info_list_ext_v1880),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(sidelink_precfg_nr_r16_s::sl_pos_precfg_freq_info_list_ext_v1880),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_freq_cfg_common_ext_v16k0_s::desc}};
static const asn1::table::field_name seq_table_sidelink_precfg_nr_r16_s_g4_names[] = {
    {"sl-PosPreconfigFreqInfoListExt-v1880", nullptr}};
static const asn1::table::seq_opt seq_table_sidelink_precfg_nr_r16_s_g4_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sidelink_precfg_nr_r16_s_groups[] = {
    {{seq_table_sidelink_precfg_nr_r16_s_g0_fields, 3, UINT64_C(0x0), seq_table_sidelink_precfg_nr_r16_s_g0_opts, 3},
     false,
     seq_table_sidelink_precfg_nr_r16_s_g0_names},
    {{seq_table_sidelink_precfg_nr_r16_s_g1_fields, 6, UINT64_C(0x0), seq_table_sidelink_precfg_nr_r16_s_g1_opts, 6},
     false,
     seq_table_sidelink_precfg_nr_r16_s_g1_names},
    {{seq_table_sidelink_precfg_nr_r16_s_g2_fields, 1, UINT64_C(0x0), seq_table_sidelink_precfg_nr_r16_s_g2_opts, 1},
     false,
     seq_table_sidelink_precfg_nr_r16_s_g2_names},
    {{seq_table_sidelink_precfg_nr_r16_s_g3_fields, 1, UINT64_C(0x0), seq_table_sidelink_precfg_nr_r16_s_g3_opts, 1},
     false,
     seq_table_sidelink_precfg_nr_r16_s_g3_names},
    {{seq_table_sidelink_precfg_nr_r16_s_g4_fields, 1, UINT64_C(0x0), seq_table_sidelink_precfg_nr_r16_s_g4_opts, 1},
     false,
     seq_table_sidelink_precfg_nr_r16_s_g4_names}};
const asn1::table::seq_desc sidelink_precfg_nr_r16_s::desc = {
    {seq_table_sidelink_precfg_nr_r16_s_fields, 15, UINT64_C(0x0), seq_table_sidelink_precfg_nr_r16_s_opts, 15},
    true,
    false,
    seq_table_sidelink_precfg_nr_r16_s_groups,
    5,
    seq_table_sidelink_precfg_nr_r16_s_names};

static const char* enum_names_sidelink_precfg_nr_r16_s_t400_r16_e__values[] =
    {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
const asn1::table::enum_desc sidelink_precfg_nr_r16_s::t400_r16_opts::desc = {
    enum_names_sidelink_precfg_nr_r16_s_t400_r16_e__values,
    8,
    "sidelink_precfg_nr_r16_s::t400_r16_e_"};
uint16_t sidelink_precfg_nr_r16_s::t400_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "sidelink_precfg_nr_r16_s::t400_r16_e_");
}

static const char* enum_names_sidelink_precfg_nr_r16_s_sl_max_num_consecutive_dtx_r16_e__values[] =
    {"n1", "n2", "n3", "n4", "n6", "n8", "n16", "n32"};
const asn1::table::enum_desc sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::desc = {
    enum_names_sidelink_precfg_nr_r16_s_sl_max_num_consecutive_dtx_r16_e__values,
    8,
    "sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_"};
uint8_t sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}

static const char* enum_names_sidelink_precfg_nr_r16_s_t400_u2_u_r18_e__values[] =
    {"ms200", "ms400", "ms600", "ms800", "ms1200", "ms2000", "ms3000", "ms4000"};
const asn1::table::enum_desc sidelink_precfg_nr_r16_s::t400_u2_u_r18_opts::desc = {
    enum_names_sidelink_precfg_nr_r16_s_t400_u2_u_r18_e__values,
    8,
    "sidelink_precfg_nr_r16_s::t400_u2_u_r18_e_"};
uint16_t sidelink_precfg_nr_r16_s::t400_u2_u_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {200, 400, 600, 800, 1200, 2000, 3000, 4000};
  return map_enum_number(numbers, 8, value, "sidelink_precfg_nr_r16_s::t400_u2_u_r18_e_");
}

// SidelinkPreconfigNR-v16k0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sidelink_precfg_nr_v16k0_s_fields[] = {
    {offsetof(sidelink_precfg_nr_v16k0_s, sl_precfg_freq_info_list_ext_v16k0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sidelink_precfg_nr_v16k0_s::sl_precfg_freq_info_list_ext_v16k0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_freq_cfg_common_ext_v16k0_s::desc}};
static const asn1::table::field_name seq_table_sidelink_precfg_nr_v16k0_s_names[] = {
    {"sl-PreconfigFreqInfoListExt-v16k0", nullptr}};
static const asn1::table::seq_opt seq_table_sidelink_precfg_nr_v16k0_s_opts[] = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sidelink_precfg_nr_v16k0_s::desc = {
    {seq_table_sidelink_precfg_nr_v16k0_s_fields, 1, UINT64_C(0x0), seq_table_sidelink_precfg_nr_v16k0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_sidelink_precfg_nr_v16k0_s_names};

// SL-PreconfigurationNR-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_precfg_nr_r16_s_fields[] = {
    {offsetof(sl_precfg_nr_r16_s, sidelink_precfg_nr_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sidelink_precfg_nr_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_precfg_nr_r16_s_names[]     = {{"sidelinkPreconfigNR-r16", nullptr}};
static const asn1::table::seq_field  seq_table_sl_precfg_nr_r16_s_g0_fields[] = {
    {offsetof(sl_precfg_nr_r16_s, sidelink_precfg_nr_v16k0),
      asn1::table::seq_field::ptr_mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sl_precfg_nr_r16_s::sidelink_precfg_nr_v16k0)>::value,
      &sidelink_precfg_nr_v16k0_s::desc}};
static const asn1::table::field_name seq_table_sl_precfg_nr_r16_s_g0_names[] = {{"sidelinkPreconfigNR-v16k0", nullptr}};
static const asn1::table::seq_group  seq_table_sl_precfg_nr_r16_s_groups[]   = {
    {{seq_table_sl_precfg_nr_r16_s_g0_fields, 1, UINT64_C(0x1), nullptr, 0},
        false,
        seq_table_sl_precfg_nr_r16_s_g0_names}};
const asn1::table::seq_desc sl_precfg_nr_r16_s::desc = {
    {seq_table_sl_precfg_nr_r16_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    seq_table_sl_precfg_nr_r16_s_groups,
    1,
    seq_table_sl_precfg_nr_r16_s_names};
