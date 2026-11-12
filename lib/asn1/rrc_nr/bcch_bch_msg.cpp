// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/bcch_bch_msg.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PDCCH-ConfigSIB1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdcch_cfg_sib1_s_fields[] = {
    {offsetof(pdcch_cfg_sib1_s, coreset_zero),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(pdcch_cfg_sib1_s, search_space_zero),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr}};
static const asn1::table::field_name seq_table_pdcch_cfg_sib1_s_names[] = {{"controlResourceSetZero", nullptr},
                                                                           {"searchSpaceZero", nullptr}};
const asn1::table::seq_desc          pdcch_cfg_sib1_s::desc             = {
    {seq_table_pdcch_cfg_sib1_s_fields, uint64_t{0x3}, nullptr, seq_table_pdcch_cfg_sib1_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

// MIB ::= SEQUENCE
static const asn1::table::seq_field seq_table_mib_s_fields[] = {
    {offsetof(mib_s, sys_frame_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mib_s::sys_frame_num), asn1::table::struct_json>::value},
    {offsetof(mib_s, sub_carrier_spacing_common),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mib_s::sub_carrier_spacing_common)>(),
     decltype(mib_s::sub_carrier_spacing_common)::has_ext,
     false,
     decltype(mib_s::sub_carrier_spacing_common)::nof_types,
     decltype(mib_s::sub_carrier_spacing_common)::nof_exts,
     nullptr,
     &mib_s::sub_carrier_spacing_common_opts::desc},
    {offsetof(mib_s, ssb_subcarrier_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(mib_s, dmrs_type_a_position),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mib_s::dmrs_type_a_position)>(),
     decltype(mib_s::dmrs_type_a_position)::has_ext,
     false,
     decltype(mib_s::dmrs_type_a_position)::nof_types,
     decltype(mib_s::dmrs_type_a_position)::nof_exts,
     nullptr,
     &mib_s::dmrs_type_a_position_opts::desc},
    {offsetof(mib_s, pdcch_cfg_sib1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcch_cfg_sib1_s::desc},
    {offsetof(mib_s, cell_barred),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mib_s::cell_barred)>(),
     decltype(mib_s::cell_barred)::has_ext,
     false,
     decltype(mib_s::cell_barred)::nof_types,
     decltype(mib_s::cell_barred)::nof_exts,
     nullptr,
     &mib_s::cell_barred_opts::desc},
    {offsetof(mib_s, intra_freq_resel),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mib_s::intra_freq_resel)>(),
     decltype(mib_s::intra_freq_resel)::has_ext,
     false,
     decltype(mib_s::intra_freq_resel)::nof_types,
     decltype(mib_s::intra_freq_resel)::nof_exts,
     nullptr,
     &mib_s::intra_freq_resel_opts::desc},
    {offsetof(mib_s, spare),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mib_s::spare), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mib_s_names[] = {{"systemFrameNumber", nullptr},
                                                                {"subCarrierSpacingCommon", nullptr},
                                                                {"ssb-SubcarrierOffset", nullptr},
                                                                {"dmrs-TypeA-Position", nullptr},
                                                                {"pdcch-ConfigSIB1", nullptr},
                                                                {"cellBarred", nullptr},
                                                                {"intraFreqReselection", nullptr},
                                                                {"spare", nullptr}};
const asn1::table::seq_desc          mib_s::desc =
    {{seq_table_mib_s_fields, uint64_t{0xff}, nullptr, seq_table_mib_s_names, 8, 0}, false, false, nullptr, 0};

static const char*           enum_names_mib_s_sub_carrier_spacing_common_e__values[] = {"scs15or60", "scs30or120"};
const asn1::table::enum_desc mib_s::sub_carrier_spacing_common_opts::desc            = {
    enum_names_mib_s_sub_carrier_spacing_common_e__values,
    2,
    "mib_s::sub_carrier_spacing_common_e_"};
uint8_t mib_s::sub_carrier_spacing_common_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30};
  return map_enum_number(numbers, 2, value, "mib_s::sub_carrier_spacing_common_e_");
}

static const char*           enum_names_mib_s_dmrs_type_a_position_e__values[] = {"pos2", "pos3"};
const asn1::table::enum_desc mib_s::dmrs_type_a_position_opts::desc = {enum_names_mib_s_dmrs_type_a_position_e__values,
                                                                       2,
                                                                       "mib_s::dmrs_type_a_position_e_"};
uint8_t                      mib_s::dmrs_type_a_position_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "mib_s::dmrs_type_a_position_e_");
}

static const char*           enum_names_mib_s_cell_barred_e__values[] = {"barred", "notBarred"};
const asn1::table::enum_desc mib_s::cell_barred_opts::desc            = {enum_names_mib_s_cell_barred_e__values,
                                                                         2,
                                                                         "mib_s::cell_barred_e_"};

static const char*           enum_names_mib_s_intra_freq_resel_e__values[] = {"allowed", "notAllowed"};
const asn1::table::enum_desc mib_s::intra_freq_resel_opts::desc = {enum_names_mib_s_intra_freq_resel_e__values,
                                                                   2,
                                                                   "mib_s::intra_freq_resel_e_"};

// BCCH-BCH-MessageType ::= CHOICE
void bcch_bch_msg_type_c::set(types::options e)
{
  type_ = e;
}
mib_s& bcch_bch_msg_type_c::set_mib()
{
  set(types::mib);
  return c;
}
void bcch_bch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field choice_table_bcch_bch_msg_type_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &mib_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_bcch_bch_msg_type_c_names[] = {{"mib", nullptr},
                                                                                 {"messageClassExtension", nullptr}};
const asn1::table::choice_desc bcch_bch_msg_type_c::desc = {asn1::table::enum_int_type<bcch_bch_msg_type_c::types>(),
                                                            bcch_bch_msg_type_c::types::nof_types,
                                                            bcch_bch_msg_type_c::types::nof_exts,
                                                            bcch_bch_msg_type_c::types::has_ext,
                                                            false,
                                                            choice_table_bcch_bch_msg_type_c_alternatives,
                                                            &asn1::table::choice_set<bcch_bch_msg_type_c>,
                                                            choice_table_bcch_bch_msg_type_c_names,
                                                            "bcch_bch_msg_type_c",
                                                            offsetof(bcch_bch_msg_type_c, type_),
                                                            offsetof(bcch_bch_msg_type_c, c),
                                                            false};

static const char*           enum_names_bcch_bch_msg_type_c_types_values[] = {"mib", "messageClassExtension"};
const asn1::table::enum_desc bcch_bch_msg_type_c::types_opts::desc = {enum_names_bcch_bch_msg_type_c_types_values,
                                                                      2,
                                                                      "bcch_bch_msg_type_c::types"};

// BCCH-BCH-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_bcch_bch_msg_s_fields[] = {{offsetof(bcch_bch_msg_s, msg),
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::choice,
                                                                           false,
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           nullptr,
                                                                           &bcch_bch_msg_type_c::desc}};
static const asn1::table::field_name seq_table_bcch_bch_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc          bcch_bch_msg_s::desc              = {
    {seq_table_bcch_bch_msg_s_fields, uint64_t{0x1}, nullptr, seq_table_bcch_bch_msg_s_names, 1, 0},
    false,
    false,
    nullptr,
    0,
    "BCCH-BCH-Message"};
