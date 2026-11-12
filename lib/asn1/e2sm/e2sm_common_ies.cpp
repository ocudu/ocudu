// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2sm/e2sm_common_ies.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::e2sm;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// Beam-ID ::= CHOICE
static const asn1::table::seq_field  choice_table_beam_id_c_alternatives[] = {{0,
                                                                               asn1::table::seq_field::mandatory,
                                                                               asn1::table::field_op::int_u8,
                                                                               false,
                                                                               true,
                                                                               int64_t{0},
                                                                               uint64_t{63},
                                                                               nullptr}};
static const asn1::table::field_name choice_table_beam_id_c_names[]        = {{"nR-Beam-ID", nullptr}};
const asn1::table::choice_desc       beam_id_c::desc = {asn1::table::enum_int_type<beam_id_c::types>(),
                                                        beam_id_c::types::nof_types,
                                                        beam_id_c::types::nof_exts,
                                                        beam_id_c::types::has_ext,
                                                        true,
                                                        choice_table_beam_id_c_alternatives,
                                                        nullptr,
                                                        choice_table_beam_id_c_names,
                                                        "beam_id_c",
                                                        asn1::table::choice_desc::no_member,
                                                        offsetof(beam_id_c, c),
                                                        false};

static const char*           enum_names_beam_id_c_types_values[] = {"nR-Beam-ID"};
const asn1::table::enum_desc beam_id_c::types_opts::desc = {enum_names_beam_id_c_types_values, 1, "beam_id_c::types"};

// NR-CGI ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_cgi_s_fields[] = {
    {offsetof(nr_cgi_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nr_cgi_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(nr_cgi_s, nr_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nr_cgi_s::nr_cell_id), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_nr_cgi_s_names[] = {{"pLMNIdentity", nullptr},
                                                                   {"nRCellIdentity", nullptr}};
const asn1::table::seq_desc          nr_cgi_s::desc =
    {{seq_table_nr_cgi_s_fields, uint64_t{0x3}, nullptr, seq_table_nr_cgi_s_names, 2, 0}, true, true, nullptr, 0};

// EUTRA-CGI ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutra_cgi_s_fields[] = {
    {offsetof(eutra_cgi_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(eutra_cgi_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(eutra_cgi_s, eutra_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(eutra_cgi_s::eutra_cell_id), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_eutra_cgi_s_names[] = {{"pLMNIdentity", nullptr},
                                                                      {"eUTRACellIdentity", nullptr}};
const asn1::table::seq_desc          eutra_cgi_s::desc =
    {{seq_table_eutra_cgi_s_fields, uint64_t{0x3}, nullptr, seq_table_eutra_cgi_s_names, 2, 0}, true, true, nullptr, 0};

// CGI ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_cgi_c_alternatives[] = {
    &asn1::table::alternative_ops<nr_cgi_s, false>::value,
    &asn1::table::alternative_ops<eutra_cgi_s, false>::value};
const asn1::table::choice_storage_desc cgi_c::storage_desc = {choice_storage_cgi_c_alternatives, 2, "cgi_c"};

nr_cgi_s& cgi_c::set_nr_cgi()
{
  set(types::nr_cgi);
  return c.get<nr_cgi_s>();
}
eutra_cgi_s& cgi_c::set_eutra_cgi()
{
  set(types::eutra_cgi);
  return c.get<eutra_cgi_s>();
}
static const asn1::table::seq_field  choice_table_cgi_c_alternatives[] = {{0,
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::sequence,
                                                                           false,
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           nullptr,
                                                                           &nr_cgi_s::desc},
                                                                          {0,
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::sequence,
                                                                           false,
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           nullptr,
                                                                           &eutra_cgi_s::desc}};
static const asn1::table::field_name choice_table_cgi_c_names[]        = {{"nR-CGI", nullptr}, {"eUTRA-CGI", nullptr}};
const asn1::table::choice_desc       cgi_c::desc                       = {asn1::table::enum_int_type<cgi_c::types>(),
                                                                          cgi_c::types::nof_types,
                                                                          cgi_c::types::nof_exts,
                                                                          cgi_c::types::has_ext,
                                                                          true,
                                                                          choice_table_cgi_c_alternatives,
                                                                          nullptr,
                                                                          choice_table_cgi_c_names,
                                                                          "cgi_c",
                                                                          offsetof(cgi_c, c.type),
                                                                          offsetof(cgi_c, c.value),
                                                                          false,
                                                                          &cgi_c::storage_desc};

static const char*           enum_names_cgi_c_types_values[] = {"nR-CGI", "eUTRA-CGI"};
const asn1::table::enum_desc cgi_c::types_opts::desc         = {enum_names_cgi_c_types_values, 2, "cgi_c::types"};

// Cell-RNTI ::= SEQUENCE
static const asn1::table::seq_field  seq_table_cell_rnti_s_fields[] = {{offsetof(cell_rnti_s, c_rnti),
                                                                        asn1::table::seq_field::mandatory,
                                                                        asn1::table::field_op::int_u32,
                                                                        false,
                                                                        true,
                                                                        int64_t{0},
                                                                        uint64_t{65535},
                                                                        nullptr},
                                                                       {offsetof(cell_rnti_s, cell_global_id),
                                                                        asn1::table::seq_field::mandatory,
                                                                        asn1::table::field_op::choice,
                                                                        false,
                                                                        false,
                                                                        0,
                                                                        0,
                                                                        nullptr,
                                                                        &cgi_c::desc}};
static const asn1::table::field_name seq_table_cell_rnti_s_names[] = {{"c-RNTI", nullptr}, {"cell-Global-ID", nullptr}};
const asn1::table::seq_desc          cell_rnti_s::desc =
    {{seq_table_cell_rnti_s_fields, uint64_t{0x3}, nullptr, seq_table_cell_rnti_s_names, 2, 0}, true, true, nullptr, 0};

// GUAMI ::= SEQUENCE
static const asn1::table::seq_field seq_table_guami_s_fields[] = {
    {offsetof(guami_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(guami_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(guami_s, amf_region_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(guami_s::amf_region_id), asn1::table::struct_json>::value},
    {offsetof(guami_s, amf_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(guami_s::amf_set_id), asn1::table::struct_json>::value},
    {offsetof(guami_s, amf_pointer),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(guami_s::amf_pointer), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_guami_s_names[] = {{"pLMNIdentity", nullptr},
                                                                  {"aMFRegionID", nullptr},
                                                                  {"aMFSetID", nullptr},
                                                                  {"aMFPointer", nullptr}};
const asn1::table::seq_desc          guami_s::desc =
    {{seq_table_guami_s_fields, uint64_t{0xf}, nullptr, seq_table_guami_s_names, 4, 0}, true, true, nullptr, 0};

// GUMMEI ::= SEQUENCE
static const asn1::table::seq_field seq_table_gummei_s_fields[] = {
    {offsetof(gummei_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gummei_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(gummei_s, mme_group_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gummei_s::mme_group_id), asn1::table::string_json>::value},
    {offsetof(gummei_s, mme_code),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gummei_s::mme_code), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_gummei_s_names[] = {{"pLMN-Identity", nullptr},
                                                                   {"mME-Group-ID", nullptr},
                                                                   {"mME-Code", nullptr}};
const asn1::table::seq_desc          gummei_s::desc =
    {{seq_table_gummei_s_fields, uint64_t{0x7}, nullptr, seq_table_gummei_s_names, 3, 0}, true, true, nullptr, 0};

// CoreCPID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_core_cp_id_c_alternatives[] = {
    &asn1::table::alternative_ops<guami_s, false>::value,
    &asn1::table::alternative_ops<gummei_s, false>::value};
const asn1::table::choice_storage_desc core_cp_id_c::storage_desc = {choice_storage_core_cp_id_c_alternatives,
                                                                     2,
                                                                     "core_cp_id_c"};

guami_s& core_cp_id_c::set_five_gc()
{
  set(types::five_gc);
  return c.get<guami_s>();
}
gummei_s& core_cp_id_c::set_epc()
{
  set(types::epc);
  return c.get<gummei_s>();
}
static const asn1::table::seq_field  choice_table_core_cp_id_c_alternatives[] = {{0,
                                                                                  asn1::table::seq_field::mandatory,
                                                                                  asn1::table::field_op::sequence,
                                                                                  false,
                                                                                  false,
                                                                                  0,
                                                                                  0,
                                                                                  nullptr,
                                                                                  &guami_s::desc},
                                                                                 {0,
                                                                                  asn1::table::seq_field::mandatory,
                                                                                  asn1::table::field_op::sequence,
                                                                                  false,
                                                                                  false,
                                                                                  0,
                                                                                  0,
                                                                                  nullptr,
                                                                                  &gummei_s::desc}};
static const asn1::table::field_name choice_table_core_cp_id_c_names[]        = {{"fiveGC", nullptr}, {"ePC", nullptr}};
const asn1::table::choice_desc       core_cp_id_c::desc = {asn1::table::enum_int_type<core_cp_id_c::types>(),
                                                           core_cp_id_c::types::nof_types,
                                                           core_cp_id_c::types::nof_exts,
                                                           core_cp_id_c::types::has_ext,
                                                           true,
                                                           choice_table_core_cp_id_c_alternatives,
                                                           nullptr,
                                                           choice_table_core_cp_id_c_names,
                                                           "core_cp_id_c",
                                                           offsetof(core_cp_id_c, c.type),
                                                           offsetof(core_cp_id_c, c.value),
                                                           false,
                                                           &core_cp_id_c::storage_desc};

static const char*           enum_names_core_cp_id_c_types_values[] = {"fiveGC", "ePC"};
const asn1::table::enum_desc core_cp_id_c::types_opts::desc         = {enum_names_core_cp_id_c_types_values,
                                                                       2,
                                                                       "core_cp_id_c::types"};
uint8_t                      core_cp_id_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5};
  return map_enum_number(numbers, 1, value, "core_cp_id_c::types");
}

// EN-GNB-ID ::= CHOICE
static const asn1::table::seq_field choice_table_en_gnb_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<bounded_bitstring<22, 32, false, true>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_en_gnb_id_c_names[] = {{"en-gNB-ID", nullptr}};
const asn1::table::choice_desc       en_gnb_id_c::desc = {asn1::table::enum_int_type<en_gnb_id_c::types>(),
                                                          en_gnb_id_c::types::nof_types,
                                                          en_gnb_id_c::types::nof_exts,
                                                          en_gnb_id_c::types::has_ext,
                                                          true,
                                                          choice_table_en_gnb_id_c_alternatives,
                                                          nullptr,
                                                          choice_table_en_gnb_id_c_names,
                                                          "en_gnb_id_c",
                                                          asn1::table::choice_desc::no_member,
                                                          offsetof(en_gnb_id_c, c),
                                                          false};

static const char*           enum_names_en_gnb_id_c_types_values[] = {"en-gNB-ID"};
const asn1::table::enum_desc en_gnb_id_c::types_opts::desc         = {enum_names_en_gnb_id_c_types_values,
                                                                      1,
                                                                      "en_gnb_id_c::types"};

// ENB-ID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_enb_id_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<20, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<28, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<18, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<21, false, true>, false>::value};
const asn1::table::choice_storage_desc enb_id_c::storage_desc = {choice_storage_enb_id_c_alternatives, 4, "enb_id_c"};

fixed_bitstring<20, false, true>& enb_id_c::set_macro_enb_id()
{
  set(types::macro_enb_id);
  return c.get<fixed_bitstring<20, false, true>>();
}
fixed_bitstring<28, false, true>& enb_id_c::set_home_enb_id()
{
  set(types::home_enb_id);
  return c.get<fixed_bitstring<28, false, true>>();
}
fixed_bitstring<18, false, true>& enb_id_c::set_short_macro_enb_id()
{
  set(types::short_macro_enb_id);
  return c.get<fixed_bitstring<18, false, true>>();
}
fixed_bitstring<21, false, true>& enb_id_c::set_long_macro_enb_id()
{
  set(types::long_macro_enb_id);
  return c.get<fixed_bitstring<21, false, true>>();
}
static const asn1::table::seq_field choice_table_enb_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<20, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<28, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<18, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<21, false, true>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_enb_id_c_names[] = {{"macro-eNB-ID", nullptr},
                                                                      {"home-eNB-ID", nullptr},
                                                                      {"short-Macro-eNB-ID", nullptr},
                                                                      {"long-Macro-eNB-ID", nullptr}};
const asn1::table::choice_desc       enb_id_c::desc                = {asn1::table::enum_int_type<enb_id_c::types>(),
                                                                      enb_id_c::types::nof_types,
                                                                      enb_id_c::types::nof_exts,
                                                                      enb_id_c::types::has_ext,
                                                                      true,
                                                                      choice_table_enb_id_c_alternatives,
                                                                      nullptr,
                                                                      choice_table_enb_id_c_names,
                                                                      "enb_id_c",
                                                                      offsetof(enb_id_c, c.type),
                                                                      offsetof(enb_id_c, c.value),
                                                                      false,
                                                                      &enb_id_c::storage_desc};

static const char*           enum_names_enb_id_c_types_values[] = {"macro-eNB-ID",
                                                                   "home-eNB-ID",
                                                                   "short-Macro-eNB-ID",
                                                                   "long-Macro-eNB-ID"};
const asn1::table::enum_desc enb_id_c::types_opts::desc = {enum_names_enb_id_c_types_values, 4, "enb_id_c::types"};

// FreqBandNrItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_freq_band_nr_item_s_fields[] = {
    {offsetof(freq_band_nr_item_s, freq_band_ind_nr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{1023},
     nullptr}};
static const asn1::table::field_name seq_table_freq_band_nr_item_s_names[] = {{"freqBandIndicatorNr", nullptr}};
const asn1::table::seq_desc          freq_band_nr_item_s::desc             = {
    {seq_table_freq_band_nr_item_s_fields, uint64_t{0x1}, nullptr, seq_table_freq_band_nr_item_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// GNB-ID ::= CHOICE
static const asn1::table::seq_field choice_table_gnb_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<bounded_bitstring<22, 32, false, true>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_gnb_id_c_names[] = {{"gNB-ID", nullptr}};
const asn1::table::choice_desc       gnb_id_c::desc                = {asn1::table::enum_int_type<gnb_id_c::types>(),
                                                                      gnb_id_c::types::nof_types,
                                                                      gnb_id_c::types::nof_exts,
                                                                      gnb_id_c::types::has_ext,
                                                                      true,
                                                                      choice_table_gnb_id_c_alternatives,
                                                                      nullptr,
                                                                      choice_table_gnb_id_c_names,
                                                                      "gnb_id_c",
                                                                      asn1::table::choice_desc::no_member,
                                                                      offsetof(gnb_id_c, c),
                                                                      false};

static const char*           enum_names_gnb_id_c_types_values[] = {"gNB-ID"};
const asn1::table::enum_desc gnb_id_c::types_opts::desc = {enum_names_gnb_id_c_types_values, 1, "gnb_id_c::types"};

// GlobalENB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_enb_id_s_fields[] = {
    {offsetof(global_enb_id_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_enb_id_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(global_enb_id_s, enb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &enb_id_c::desc}};
static const asn1::table::field_name seq_table_global_enb_id_s_names[] = {{"pLMNIdentity", nullptr},
                                                                          {"eNB-ID", nullptr}};
const asn1::table::seq_desc          global_enb_id_s::desc             = {
    {seq_table_global_enb_id_s_fields, uint64_t{0x3}, nullptr, seq_table_global_enb_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// GlobalGNB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_gnb_id_s_fields[] = {
    {offsetof(global_gnb_id_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_gnb_id_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(global_gnb_id_s, gnb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_id_c::desc}};
static const asn1::table::field_name seq_table_global_gnb_id_s_names[] = {{"pLMNIdentity", nullptr},
                                                                          {"gNB-ID", nullptr}};
const asn1::table::seq_desc          global_gnb_id_s::desc             = {
    {seq_table_global_gnb_id_s_fields, uint64_t{0x3}, nullptr, seq_table_global_gnb_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// NgENB-ID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ng_enb_id_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<20, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<18, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<21, false, true>, false>::value};
const asn1::table::choice_storage_desc ng_enb_id_c::storage_desc = {choice_storage_ng_enb_id_c_alternatives,
                                                                    3,
                                                                    "ng_enb_id_c"};

fixed_bitstring<20, false, true>& ng_enb_id_c::set_macro_ng_enb_id()
{
  set(types::macro_ng_enb_id);
  return c.get<fixed_bitstring<20, false, true>>();
}
fixed_bitstring<18, false, true>& ng_enb_id_c::set_short_macro_ng_enb_id()
{
  set(types::short_macro_ng_enb_id);
  return c.get<fixed_bitstring<18, false, true>>();
}
fixed_bitstring<21, false, true>& ng_enb_id_c::set_long_macro_ng_enb_id()
{
  set(types::long_macro_ng_enb_id);
  return c.get<fixed_bitstring<21, false, true>>();
}
static const asn1::table::seq_field choice_table_ng_enb_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<20, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<18, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<21, false, true>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ng_enb_id_c_names[] = {{"macroNgENB-ID", nullptr},
                                                                         {"shortMacroNgENB-ID", nullptr},
                                                                         {"longMacroNgENB-ID", nullptr}};
const asn1::table::choice_desc       ng_enb_id_c::desc = {asn1::table::enum_int_type<ng_enb_id_c::types>(),
                                                          ng_enb_id_c::types::nof_types,
                                                          ng_enb_id_c::types::nof_exts,
                                                          ng_enb_id_c::types::has_ext,
                                                          true,
                                                          choice_table_ng_enb_id_c_alternatives,
                                                          nullptr,
                                                          choice_table_ng_enb_id_c_names,
                                                          "ng_enb_id_c",
                                                          offsetof(ng_enb_id_c, c.type),
                                                          offsetof(ng_enb_id_c, c.value),
                                                          false,
                                                          &ng_enb_id_c::storage_desc};

static const char* enum_names_ng_enb_id_c_types_values[] = {"macroNgENB-ID", "shortMacroNgENB-ID", "longMacroNgENB-ID"};
const asn1::table::enum_desc ng_enb_id_c::types_opts::desc = {enum_names_ng_enb_id_c_types_values,
                                                              3,
                                                              "ng_enb_id_c::types"};

// GlobalNgENB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_ng_enb_id_s_fields[] = {
    {offsetof(global_ng_enb_id_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_ng_enb_id_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(global_ng_enb_id_s, ng_enb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_enb_id_c::desc}};
static const asn1::table::field_name seq_table_global_ng_enb_id_s_names[] = {{"pLMNIdentity", nullptr},
                                                                             {"ngENB-ID", nullptr}};
const asn1::table::seq_desc          global_ng_enb_id_s::desc             = {
    {seq_table_global_ng_enb_id_s_fields, uint64_t{0x3}, nullptr, seq_table_global_ng_enb_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// GlobalNGRANNodeID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_global_ngran_node_id_c_alternatives[] = {
    &asn1::table::alternative_ops<global_gnb_id_s, false>::value,
    &asn1::table::alternative_ops<global_ng_enb_id_s, false>::value};
const asn1::table::choice_storage_desc global_ngran_node_id_c::storage_desc = {
    choice_storage_global_ngran_node_id_c_alternatives,
    2,
    "global_ngran_node_id_c"};

global_gnb_id_s& global_ngran_node_id_c::set_gnb()
{
  set(types::gnb);
  return c.get<global_gnb_id_s>();
}
global_ng_enb_id_s& global_ngran_node_id_c::set_ng_enb()
{
  set(types::ng_enb);
  return c.get<global_ng_enb_id_s>();
}
static const asn1::table::seq_field choice_table_global_ngran_node_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_gnb_id_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_ng_enb_id_s::desc}};
static const asn1::table::field_name choice_table_global_ngran_node_id_c_names[] = {{"gNB", nullptr},
                                                                                    {"ng-eNB", nullptr}};
const asn1::table::choice_desc       global_ngran_node_id_c::desc                = {
    asn1::table::enum_int_type<global_ngran_node_id_c::types>(),
    global_ngran_node_id_c::types::nof_types,
    global_ngran_node_id_c::types::nof_exts,
    global_ngran_node_id_c::types::has_ext,
    true,
    choice_table_global_ngran_node_id_c_alternatives,
    nullptr,
    choice_table_global_ngran_node_id_c_names,
    "global_ngran_node_id_c",
    offsetof(global_ngran_node_id_c, c.type),
    offsetof(global_ngran_node_id_c, c.value),
    false,
    &global_ngran_node_id_c::storage_desc};

static const char*           enum_names_global_ngran_node_id_c_types_values[] = {"gNB", "ng-eNB"};
const asn1::table::enum_desc global_ngran_node_id_c::types_opts::desc = {enum_names_global_ngran_node_id_c_types_values,
                                                                         2,
                                                                         "global_ngran_node_id_c::types"};

// GlobalenGNB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_globalen_gnb_id_s_fields[] = {
    {offsetof(globalen_gnb_id_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(globalen_gnb_id_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(globalen_gnb_id_s, en_gnb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &en_gnb_id_c::desc}};
static const asn1::table::field_name seq_table_globalen_gnb_id_s_names[] = {{"pLMN-Identity", nullptr},
                                                                            {"en-gNB-ID", nullptr}};
const asn1::table::seq_desc          globalen_gnb_id_s::desc             = {
    {seq_table_globalen_gnb_id_s_fields, uint64_t{0x3}, nullptr, seq_table_globalen_gnb_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// GroupID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_group_id_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc group_id_c::storage_desc = {choice_storage_group_id_c_alternatives,
                                                                   2,
                                                                   "group_id_c"};

uint16_t& group_id_c::set_five_gc()
{
  set(types::five_gc);
  return c.get<uint16_t>();
}
uint16_t& group_id_c::set_epc()
{
  set(types::epc);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field  choice_table_group_id_c_alternatives[] = {{0,
                                                                                asn1::table::seq_field::mandatory,
                                                                                asn1::table::field_op::int_u16,
                                                                                true,
                                                                                true,
                                                                                int64_t{0},
                                                                                uint64_t{255},
                                                                                nullptr},
                                                                               {0,
                                                                                asn1::table::seq_field::mandatory,
                                                                                asn1::table::field_op::int_u16,
                                                                                false,
                                                                                true,
                                                                                int64_t{0},
                                                                                uint64_t{255},
                                                                                nullptr}};
static const asn1::table::field_name choice_table_group_id_c_names[]        = {{"fiveGC", nullptr}, {"ePC", nullptr}};
const asn1::table::choice_desc       group_id_c::desc = {asn1::table::enum_int_type<group_id_c::types>(),
                                                         group_id_c::types::nof_types,
                                                         group_id_c::types::nof_exts,
                                                         group_id_c::types::has_ext,
                                                         true,
                                                         choice_table_group_id_c_alternatives,
                                                         nullptr,
                                                         choice_table_group_id_c_names,
                                                         "group_id_c",
                                                         offsetof(group_id_c, c.type),
                                                         offsetof(group_id_c, c.value),
                                                         false,
                                                         &group_id_c::storage_desc};

static const char*           enum_names_group_id_c_types_values[] = {"fiveGC", "ePC"};
const asn1::table::enum_desc group_id_c::types_opts::desc         = {enum_names_group_id_c_types_values,
                                                                     2,
                                                                     "group_id_c::types"};
uint8_t                      group_id_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5};
  return map_enum_number(numbers, 1, value, "group_id_c::types");
}

// Interface-MessageID ::= SEQUENCE
static const asn1::table::seq_field seq_table_interface_msg_id_s_fields[] = {
    {offsetof(interface_msg_id_s, interface_proc_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(interface_msg_id_s, msg_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(interface_msg_id_s::msg_type)>(),
     decltype(interface_msg_id_s::msg_type)::has_ext,
     false,
     decltype(interface_msg_id_s::msg_type)::nof_types,
     decltype(interface_msg_id_s::msg_type)::nof_exts,
     nullptr,
     &interface_msg_id_s::msg_type_opts::desc}};
static const asn1::table::field_name seq_table_interface_msg_id_s_names[] = {{"interfaceProcedureID", nullptr},
                                                                             {"messageType", nullptr}};
const asn1::table::seq_desc          interface_msg_id_s::desc             = {
    {seq_table_interface_msg_id_s_fields, uint64_t{0x3}, nullptr, seq_table_interface_msg_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

static const char*           enum_names_interface_msg_id_s_msg_type_e__values[] = {"initiatingMessage",
                                                                                   "successfulOutcome",
                                                                                   "unsuccessfulOutcome"};
const asn1::table::enum_desc interface_msg_id_s::msg_type_opts::desc            = {
    enum_names_interface_msg_id_s_msg_type_e__values,
    3,
    "interface_msg_id_s::msg_type_e_"};

// InterfaceID-E1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_interface_id_e1_s_fields[] = {{offsetof(interface_id_e1_s, global_gnb_id),
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &global_gnb_id_s::desc},
                                                                            {offsetof(interface_id_e1_s, gnb_cu_up_id),
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::int_u64,
                                                                             false,
                                                                             true,
                                                                             int64_t{0},
                                                                             uint64_t{68719476735},
                                                                             nullptr}};
static const asn1::table::field_name seq_table_interface_id_e1_s_names[] = {{"globalGNB-ID", nullptr},
                                                                            {"gNB-CU-UP-ID", nullptr}};
const asn1::table::seq_desc          interface_id_e1_s::desc             = {
    {seq_table_interface_id_e1_s_fields, uint64_t{0x3}, nullptr, seq_table_interface_id_e1_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// InterfaceID-F1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_interface_id_f1_s_fields[] = {{offsetof(interface_id_f1_s, global_gnb_id),
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &global_gnb_id_s::desc},
                                                                            {offsetof(interface_id_f1_s, gnb_du_id),
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::int_u64,
                                                                             false,
                                                                             true,
                                                                             int64_t{0},
                                                                             uint64_t{68719476735},
                                                                             nullptr}};
static const asn1::table::field_name seq_table_interface_id_f1_s_names[] = {{"globalGNB-ID", nullptr},
                                                                            {"gNB-DU-ID", nullptr}};
const asn1::table::seq_desc          interface_id_f1_s::desc             = {
    {seq_table_interface_id_f1_s_fields, uint64_t{0x3}, nullptr, seq_table_interface_id_f1_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// InterfaceID-NG ::= SEQUENCE
static const asn1::table::seq_field  seq_table_interface_id_ng_s_fields[] = {{offsetof(interface_id_ng_s, guami),
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::sequence,
                                                                              false,
                                                                              false,
                                                                              0,
                                                                              0,
                                                                              nullptr,
                                                                              &guami_s::desc}};
static const asn1::table::field_name seq_table_interface_id_ng_s_names[]  = {{"guami", nullptr}};
const asn1::table::seq_desc          interface_id_ng_s::desc              = {
    {seq_table_interface_id_ng_s_fields, uint64_t{0x1}, nullptr, seq_table_interface_id_ng_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// InterfaceID-S1 ::= SEQUENCE
static const asn1::table::seq_field  seq_table_interface_id_s1_s_fields[] = {{offsetof(interface_id_s1_s, gummei),
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::sequence,
                                                                              false,
                                                                              false,
                                                                              0,
                                                                              0,
                                                                              nullptr,
                                                                              &gummei_s::desc}};
static const asn1::table::field_name seq_table_interface_id_s1_s_names[]  = {{"gUMMEI", nullptr}};
const asn1::table::seq_desc          interface_id_s1_s::desc              = {
    {seq_table_interface_id_s1_s_fields, uint64_t{0x1}, nullptr, seq_table_interface_id_s1_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// InterfaceID-W1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_interface_id_w1_s_fields[] = {
    {offsetof(interface_id_w1_s, global_ng_enb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_ng_enb_id_s::desc},
    {offsetof(interface_id_w1_s, ng_enb_du_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr}};
static const asn1::table::field_name seq_table_interface_id_w1_s_names[] = {{"global-ng-eNB-ID", nullptr},
                                                                            {"ng-eNB-DU-ID", nullptr}};
const asn1::table::seq_desc          interface_id_w1_s::desc             = {
    {seq_table_interface_id_w1_s_fields, uint64_t{0x3}, nullptr, seq_table_interface_id_w1_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// InterfaceID-X2 ::= SEQUENCE
static const asn1::table::seq_field  seq_table_interface_id_x2_s_fields[] = {{offsetof(interface_id_x2_s, node_type),
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::choice,
                                                                              false,
                                                                              false,
                                                                              0,
                                                                              0,
                                                                              nullptr,
                                                                              &interface_id_x2_s::node_type_c_::desc}};
static const asn1::table::field_name seq_table_interface_id_x2_s_names[]  = {{"nodeType", nullptr}};
const asn1::table::seq_desc          interface_id_x2_s::desc              = {
    {seq_table_interface_id_x2_s_fields, uint64_t{0x1}, nullptr, seq_table_interface_id_x2_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::storage_ops* const choice_storage_interface_id_x2_s_node_type_c__alternatives[] = {
    &asn1::table::alternative_ops<global_enb_id_s, false>::value,
    &asn1::table::alternative_ops<globalen_gnb_id_s, false>::value};
const asn1::table::choice_storage_desc interface_id_x2_s::node_type_c_::storage_desc = {
    choice_storage_interface_id_x2_s_node_type_c__alternatives,
    2,
    "interface_id_x2_s::node_type_c_"};

global_enb_id_s& interface_id_x2_s::node_type_c_::set_global_enb_id()
{
  set(types::global_enb_id);
  return c.get<global_enb_id_s>();
}
globalen_gnb_id_s& interface_id_x2_s::node_type_c_::set_global_en_gnb_id()
{
  set(types::global_en_gnb_id);
  return c.get<globalen_gnb_id_s>();
}
static const asn1::table::seq_field choice_table_interface_id_x2_s_node_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_enb_id_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &globalen_gnb_id_s::desc}};
static const asn1::table::field_name choice_table_interface_id_x2_s_node_type_c__names[] = {
    {"global-eNB-ID", nullptr},
    {"global-en-gNB-ID", nullptr}};
const asn1::table::choice_desc interface_id_x2_s::node_type_c_::desc = {
    asn1::table::enum_int_type<interface_id_x2_s::node_type_c_::types>(),
    interface_id_x2_s::node_type_c_::types::nof_types,
    interface_id_x2_s::node_type_c_::types::nof_exts,
    interface_id_x2_s::node_type_c_::types::has_ext,
    true,
    choice_table_interface_id_x2_s_node_type_c__alternatives,
    nullptr,
    choice_table_interface_id_x2_s_node_type_c__names,
    "interface_id_x2_s::node_type_c_",
    offsetof(interface_id_x2_s::node_type_c_, c.type),
    offsetof(interface_id_x2_s::node_type_c_, c.value),
    false,
    &interface_id_x2_s::node_type_c_::storage_desc};

static const char* enum_names_interface_id_x2_s_node_type_c__types_values[]    = {"global-eNB-ID", "global-en-gNB-ID"};
const asn1::table::enum_desc interface_id_x2_s::node_type_c_::types_opts::desc = {
    enum_names_interface_id_x2_s_node_type_c__types_values,
    2,
    "interface_id_x2_s::node_type_c_::types"};

// InterfaceID-Xn ::= SEQUENCE
static const asn1::table::seq_field seq_table_interface_id_xn_s_fields[] = {
    {offsetof(interface_id_xn_s, global_ng_ran_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &global_ngran_node_id_c::desc}};
static const asn1::table::field_name seq_table_interface_id_xn_s_names[] = {{"global-NG-RAN-ID", nullptr}};
const asn1::table::seq_desc          interface_id_xn_s::desc             = {
    {seq_table_interface_id_xn_s_fields, uint64_t{0x1}, nullptr, seq_table_interface_id_xn_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// InterfaceIdentifier ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_interface_id_c_alternatives[] = {
    &asn1::table::alternative_ops<interface_id_ng_s, false>::value,
    &asn1::table::alternative_ops<interface_id_xn_s, false>::value,
    &asn1::table::alternative_ops<interface_id_f1_s, false>::value,
    &asn1::table::alternative_ops<interface_id_e1_s, false>::value,
    &asn1::table::alternative_ops<interface_id_s1_s, false>::value,
    &asn1::table::alternative_ops<interface_id_x2_s, false>::value,
    &asn1::table::alternative_ops<interface_id_w1_s, false>::value};
const asn1::table::choice_storage_desc interface_id_c::storage_desc = {choice_storage_interface_id_c_alternatives,
                                                                       7,
                                                                       "interface_id_c"};

interface_id_ng_s& interface_id_c::set_ng()
{
  set(types::ng);
  return c.get<interface_id_ng_s>();
}
interface_id_xn_s& interface_id_c::set_xn()
{
  set(types::xn);
  return c.get<interface_id_xn_s>();
}
interface_id_f1_s& interface_id_c::set_f1()
{
  set(types::f1);
  return c.get<interface_id_f1_s>();
}
interface_id_e1_s& interface_id_c::set_e1()
{
  set(types::e1);
  return c.get<interface_id_e1_s>();
}
interface_id_s1_s& interface_id_c::set_s1()
{
  set(types::s1);
  return c.get<interface_id_s1_s>();
}
interface_id_x2_s& interface_id_c::set_x2()
{
  set(types::x2);
  return c.get<interface_id_x2_s>();
}
interface_id_w1_s& interface_id_c::set_w1()
{
  set(types::w1);
  return c.get<interface_id_w1_s>();
}
static const asn1::table::seq_field  choice_table_interface_id_c_alternatives[] = {{0,
                                                                                    asn1::table::seq_field::mandatory,
                                                                                    asn1::table::field_op::sequence,
                                                                                    false,
                                                                                    false,
                                                                                    0,
                                                                                    0,
                                                                                    nullptr,
                                                                                    &interface_id_ng_s::desc},
                                                                                   {0,
                                                                                    asn1::table::seq_field::mandatory,
                                                                                    asn1::table::field_op::sequence,
                                                                                    false,
                                                                                    false,
                                                                                    0,
                                                                                    0,
                                                                                    nullptr,
                                                                                    &interface_id_xn_s::desc},
                                                                                   {0,
                                                                                    asn1::table::seq_field::mandatory,
                                                                                    asn1::table::field_op::sequence,
                                                                                    false,
                                                                                    false,
                                                                                    0,
                                                                                    0,
                                                                                    nullptr,
                                                                                    &interface_id_f1_s::desc},
                                                                                   {0,
                                                                                    asn1::table::seq_field::mandatory,
                                                                                    asn1::table::field_op::sequence,
                                                                                    false,
                                                                                    false,
                                                                                    0,
                                                                                    0,
                                                                                    nullptr,
                                                                                    &interface_id_e1_s::desc},
                                                                                   {0,
                                                                                    asn1::table::seq_field::mandatory,
                                                                                    asn1::table::field_op::sequence,
                                                                                    false,
                                                                                    false,
                                                                                    0,
                                                                                    0,
                                                                                    nullptr,
                                                                                    &interface_id_s1_s::desc},
                                                                                   {0,
                                                                                    asn1::table::seq_field::mandatory,
                                                                                    asn1::table::field_op::sequence,
                                                                                    false,
                                                                                    false,
                                                                                    0,
                                                                                    0,
                                                                                    nullptr,
                                                                                    &interface_id_x2_s::desc},
                                                                                   {0,
                                                                                    asn1::table::seq_field::mandatory,
                                                                                    asn1::table::field_op::sequence,
                                                                                    false,
                                                                                    false,
                                                                                    0,
                                                                                    0,
                                                                                    nullptr,
                                                                                    &interface_id_w1_s::desc}};
static const asn1::table::field_name choice_table_interface_id_c_names[]        = {{"nG", nullptr},
                                                                                   {"xN", nullptr},
                                                                                   {"f1", nullptr},
                                                                                   {"e1", nullptr},
                                                                                   {"s1", nullptr},
                                                                                   {"x2", nullptr},
                                                                                   {"w1", nullptr}};
const asn1::table::choice_desc       interface_id_c::desc = {asn1::table::enum_int_type<interface_id_c::types>(),
                                                             interface_id_c::types::nof_types,
                                                             interface_id_c::types::nof_exts,
                                                             interface_id_c::types::has_ext,
                                                             true,
                                                             choice_table_interface_id_c_alternatives,
                                                             nullptr,
                                                             choice_table_interface_id_c_names,
                                                             "interface_id_c",
                                                             offsetof(interface_id_c, c.type),
                                                             offsetof(interface_id_c, c.value),
                                                             false,
                                                             &interface_id_c::storage_desc};

static const char*           enum_names_interface_id_c_types_values[] = {"nG", "xN", "f1", "e1", "s1", "x2", "w1"};
const asn1::table::enum_desc interface_id_c::types_opts::desc         = {enum_names_interface_id_c_types_values,
                                                                         7,
                                                                         "interface_id_c::types"};

// InterfaceType ::= ENUMERATED
static const char*           enum_names_interface_type_e_values[] = {"nG", "xn", "f1", "e1", "s1", "x2", "w1"};
const asn1::table::enum_desc interface_type_opts::desc = {enum_names_interface_type_e_values, 7, "interface_type_e"};

// NR-ARFCN ::= SEQUENCE
static const asn1::table::seq_field  seq_table_nr_arfcn_s_fields[] = {{offsetof(nr_arfcn_s, nr_arfcn),
                                                                       asn1::table::seq_field::mandatory,
                                                                       asn1::table::field_op::int_u32,
                                                                       false,
                                                                       true,
                                                                       int64_t{0},
                                                                       uint64_t{3279165},
                                                                       nullptr}};
static const asn1::table::field_name seq_table_nr_arfcn_s_names[]  = {{"nRARFCN", nullptr}};
const asn1::table::seq_desc          nr_arfcn_s::desc =
    {{seq_table_nr_arfcn_s_fields, uint64_t{0x1}, nullptr, seq_table_nr_arfcn_s_names, 1, 0}, true, true, nullptr, 0};

// SupportedSULFreqBandItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_supported_sul_freq_band_item_s_fields[] = {
    {offsetof(supported_sul_freq_band_item_s, freq_band_ind_nr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{1023},
     nullptr}};
static const asn1::table::field_name seq_table_supported_sul_freq_band_item_s_names[] = {
    {"freqBandIndicatorNr", nullptr}};
const asn1::table::seq_desc supported_sul_freq_band_item_s::desc = {{seq_table_supported_sul_freq_band_item_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_supported_sul_freq_band_item_s_names,
                                                                     1,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// NRFrequencyBandItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_freq_band_item_s_fields[] = {
    {offsetof(nr_freq_band_item_s, freq_band_ind_nr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{1023},
     nullptr},
    {offsetof(nr_freq_band_item_s, supported_sul_band_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     32,
     &asn1::table::item_list_codec<decltype(nr_freq_band_item_s::supported_sul_band_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &supported_sul_freq_band_item_s::desc}};
static const asn1::table::field_name seq_table_nr_freq_band_item_s_names[] = {{"freqBandIndicatorNr", nullptr},
                                                                              {"supportedSULBandList", nullptr}};
const asn1::table::seq_desc          nr_freq_band_item_s::desc             = {
    {seq_table_nr_freq_band_item_s_fields, uint64_t{0x3}, nullptr, seq_table_nr_freq_band_item_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// NRFrequencyShift7p5khz ::= ENUMERATED
static const char*           enum_names_nr_freq_shift7p5khz_e_values[] = {"false", "true"};
const asn1::table::enum_desc nr_freq_shift7p5khz_opts::desc            = {enum_names_nr_freq_shift7p5khz_e_values,
                                                                          2,
                                                                          "nr_freq_shift7p5khz_e"};

// NRFrequencyInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_freq_info_s_fields[] = {
    {offsetof(nr_freq_info_s, nr_arfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_arfcn_s::desc},
    {offsetof(nr_freq_info_s, freq_band_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::
         item_list_codec<decltype(nr_freq_info_s::freq_band_list), asn1::table::sequence_item_ops, false, true>::value,
     &nr_freq_band_item_s::desc},
    {offsetof(nr_freq_info_s, freq_shift7p5khz),
     (int32_t)offsetof(nr_freq_info_s, freq_shift7p5khz_present),
     asn1::table::enum_op<decltype(nr_freq_info_s::freq_shift7p5khz)>(),
     decltype(nr_freq_info_s::freq_shift7p5khz)::has_ext,
     false,
     decltype(nr_freq_info_s::freq_shift7p5khz)::nof_types,
     decltype(nr_freq_info_s::freq_shift7p5khz)::nof_exts,
     nullptr,
     &nr_freq_shift7p5khz_opts::desc}};
static const asn1::table::field_name seq_table_nr_freq_info_s_names[] = {{"nrARFCN", nullptr},
                                                                         {"frequencyBand-List", nullptr},
                                                                         {"frequencyShift7p5khz", nullptr}};
static const asn1::table::seq_opt    seq_table_nr_freq_info_s_opts[]  = {
    {2, (int32_t)offsetof(nr_freq_info_s, freq_shift7p5khz_present)}};
const asn1::table::seq_desc nr_freq_info_s::desc = {{seq_table_nr_freq_info_s_fields,
                                                     uint64_t{0x3},
                                                     seq_table_nr_freq_info_s_opts,
                                                     seq_table_nr_freq_info_s_names,
                                                     3,
                                                     1},
                                                    true,
                                                    true,
                                                    nullptr,
                                                    0};

// PartialUEID ::= SEQUENCE
static const asn1::table::seq_field seq_table_partial_ue_id_s_fields[] = {
    {offsetof(partial_ue_id_s, amf_ue_ngap_id),
     (int32_t)offsetof(partial_ue_id_s, amf_ue_ngap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{1099511627775},
     nullptr},
    {offsetof(partial_ue_id_s, guami),
     (int32_t)offsetof(partial_ue_id_s, guami_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &guami_s::desc},
    {offsetof(partial_ue_id_s, gnb_cu_ue_f1ap_id),
     (int32_t)offsetof(partial_ue_id_s, gnb_cu_ue_f1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(partial_ue_id_s, gnb_cu_cp_ue_e1ap_id),
     (int32_t)offsetof(partial_ue_id_s, gnb_cu_cp_ue_e1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(partial_ue_id_s, ran_ue_id),
     (int32_t)offsetof(partial_ue_id_s, ran_ue_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(partial_ue_id_s::ran_ue_id), asn1::table::string_json>::value},
    {offsetof(partial_ue_id_s, m_ng_ran_ue_xnap_id),
     (int32_t)offsetof(partial_ue_id_s, m_ng_ran_ue_xnap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(partial_ue_id_s, global_ng_ran_node_id),
     (int32_t)offsetof(partial_ue_id_s, global_ng_ran_node_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &global_ngran_node_id_c::desc},
    {offsetof(partial_ue_id_s, cell_rnti),
     (int32_t)offsetof(partial_ue_id_s, cell_rnti_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_rnti_s::desc},
    {offsetof(partial_ue_id_s, ng_enb_cu_ue_w1_ap_id),
     (int32_t)offsetof(partial_ue_id_s, ng_enb_cu_ue_w1_ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(partial_ue_id_s, m_enb_ue_x2ap_id),
     (int32_t)offsetof(partial_ue_id_s, m_enb_ue_x2ap_id_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(partial_ue_id_s, m_enb_ue_x2ap_id_ext),
     (int32_t)offsetof(partial_ue_id_s, m_enb_ue_x2ap_id_ext_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(partial_ue_id_s, global_enb_id),
     (int32_t)offsetof(partial_ue_id_s, global_enb_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_enb_id_s::desc},
    {offsetof(partial_ue_id_s, mme_ue_s1ap_id),
     (int32_t)offsetof(partial_ue_id_s, mme_ue_s1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(partial_ue_id_s, gummei),
     (int32_t)offsetof(partial_ue_id_s, gummei_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gummei_s::desc}};
static const asn1::table::field_name seq_table_partial_ue_id_s_names[] = {{"amf-UE-NGAP-ID", nullptr},
                                                                          {"guami", nullptr},
                                                                          {"gNB-CU-UE-F1AP-ID", nullptr},
                                                                          {"gNB-CU-CP-UE-E1AP-ID", nullptr},
                                                                          {"ran-UEID", nullptr},
                                                                          {"m-NG-RAN-UE-XnAP-ID", nullptr},
                                                                          {"globalNG-RANNode-ID", nullptr},
                                                                          {"cell-RNTI", nullptr},
                                                                          {"ng-eNB-CU-UE-W1AP-ID", nullptr},
                                                                          {"m-eNB-UE-X2AP-ID", nullptr},
                                                                          {"m-eNB-UE-X2AP-ID-Extension", nullptr},
                                                                          {"globalENB-ID", nullptr},
                                                                          {"mME-UE-S1AP-ID", nullptr},
                                                                          {"gUMMEI", nullptr}};
static const asn1::table::seq_opt    seq_table_partial_ue_id_s_opts[]  = {
    {0, (int32_t)offsetof(partial_ue_id_s, amf_ue_ngap_id_present)},
    {1, (int32_t)offsetof(partial_ue_id_s, guami_present)},
    {2, (int32_t)offsetof(partial_ue_id_s, gnb_cu_ue_f1ap_id_present)},
    {3, (int32_t)offsetof(partial_ue_id_s, gnb_cu_cp_ue_e1ap_id_present)},
    {4, (int32_t)offsetof(partial_ue_id_s, ran_ue_id_present)},
    {5, (int32_t)offsetof(partial_ue_id_s, m_ng_ran_ue_xnap_id_present)},
    {6, (int32_t)offsetof(partial_ue_id_s, global_ng_ran_node_id_present)},
    {7, (int32_t)offsetof(partial_ue_id_s, cell_rnti_present)},
    {8, (int32_t)offsetof(partial_ue_id_s, ng_enb_cu_ue_w1_ap_id_present)},
    {9, (int32_t)offsetof(partial_ue_id_s, m_enb_ue_x2ap_id_present)},
    {10, (int32_t)offsetof(partial_ue_id_s, m_enb_ue_x2ap_id_ext_present)},
    {11, (int32_t)offsetof(partial_ue_id_s, global_enb_id_present)},
    {12, (int32_t)offsetof(partial_ue_id_s, mme_ue_s1ap_id_present)},
    {13, (int32_t)offsetof(partial_ue_id_s, gummei_present)}};
const asn1::table::seq_desc partial_ue_id_s::desc = {{seq_table_partial_ue_id_s_fields,
                                                      uint64_t{0x0},
                                                      seq_table_partial_ue_id_s_opts,
                                                      seq_table_partial_ue_id_s_names,
                                                      14,
                                                      14},
                                                     true,
                                                     true,
                                                     nullptr,
                                                     0};

// QoSID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_qos_id_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc qos_id_c::storage_desc = {choice_storage_qos_id_c_alternatives, 2, "qos_id_c"};

uint16_t& qos_id_c::set_five_gc()
{
  set(types::five_gc);
  return c.get<uint16_t>();
}
uint16_t& qos_id_c::set_epc()
{
  set(types::epc);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field  choice_table_qos_id_c_alternatives[] = {{0,
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::int_u16,
                                                                              true,
                                                                              true,
                                                                              int64_t{0},
                                                                              uint64_t{255},
                                                                              nullptr},
                                                                             {0,
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::int_u16,
                                                                              false,
                                                                              true,
                                                                              int64_t{0},
                                                                              uint64_t{255},
                                                                              nullptr}};
static const asn1::table::field_name choice_table_qos_id_c_names[]        = {{"fiveGC", nullptr}, {"ePC", nullptr}};
const asn1::table::choice_desc       qos_id_c::desc = {asn1::table::enum_int_type<qos_id_c::types>(),
                                                       qos_id_c::types::nof_types,
                                                       qos_id_c::types::nof_exts,
                                                       qos_id_c::types::has_ext,
                                                       true,
                                                       choice_table_qos_id_c_alternatives,
                                                       nullptr,
                                                       choice_table_qos_id_c_names,
                                                       "qos_id_c",
                                                       offsetof(qos_id_c, c.type),
                                                       offsetof(qos_id_c, c.value),
                                                       false,
                                                       &qos_id_c::storage_desc};

static const char*           enum_names_qos_id_c_types_values[] = {"fiveGC", "ePC"};
const asn1::table::enum_desc qos_id_c::types_opts::desc = {enum_names_qos_id_c_types_values, 2, "qos_id_c::types"};
uint8_t                      qos_id_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5};
  return map_enum_number(numbers, 1, value, "qos_id_c::types");
}

// RANfunction-Name ::= SEQUENCE
static const asn1::table::seq_field seq_table_ra_nfunction_name_s_fields[] = {
    {offsetof(ra_nfunction_name_s, ran_function_short_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ra_nfunction_name_s::ran_function_short_name),
                                asn1::table::string_json>::value},
    {offsetof(ra_nfunction_name_s, ran_function_e2sm_o_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ra_nfunction_name_s::ran_function_e2sm_o_id),
                                asn1::table::string_json>::value},
    {offsetof(ra_nfunction_name_s, ran_function_description),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ra_nfunction_name_s::ran_function_description),
                                asn1::table::string_json>::value},
    {offsetof(ra_nfunction_name_s, ran_function_instance),
     (int32_t)offsetof(ra_nfunction_name_s, ran_function_instance_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_ra_nfunction_name_s_names[] = {{"ranFunction-ShortName", nullptr},
                                                                              {"ranFunction-E2SM-OID", nullptr},
                                                                              {"ranFunction-Description", nullptr},
                                                                              {"ranFunction-Instance", nullptr}};
static const asn1::table::seq_opt    seq_table_ra_nfunction_name_s_opts[]  = {
    {3, (int32_t)offsetof(ra_nfunction_name_s, ran_function_instance_present)}};
const asn1::table::seq_desc ra_nfunction_name_s::desc = {{seq_table_ra_nfunction_name_s_fields,
                                                          uint64_t{0x7},
                                                          seq_table_ra_nfunction_name_s_opts,
                                                          seq_table_ra_nfunction_name_s_names,
                                                          4,
                                                          1},
                                                         true,
                                                         true,
                                                         nullptr,
                                                         0};

// RRCclass-LTE ::= ENUMERATED
static const char*           enum_names_rrc_class_lte_e_values[] = {"bCCH-BCH",
                                                                    "bCCH-BCH-MBMS",
                                                                    "bCCH-DL-SCH",
                                                                    "bCCH-DL-SCH-BR",
                                                                    "bCCH-DL-SCH-MBMS",
                                                                    "mCCH",
                                                                    "pCCH",
                                                                    "dL-CCCH",
                                                                    "dL-DCCH",
                                                                    "uL-CCCH",
                                                                    "uL-DCCH",
                                                                    "sC-MCCH"};
const asn1::table::enum_desc rrc_class_lte_opts::desc = {enum_names_rrc_class_lte_e_values, 12, "rrc_class_lte_e"};

// RRCclass-NR ::= ENUMERATED
static const char* enum_names_rrc_class_nr_e_values[] =
    {"bCCH-BCH", "bCCH-DL-SCH", "dL-CCCH", "dL-DCCH", "pCCH", "uL-CCCH", "uL-CCCH1", "uL-DCCH"};
const asn1::table::enum_desc rrc_class_nr_opts::desc = {enum_names_rrc_class_nr_e_values, 8, "rrc_class_nr_e"};
uint8_t                      rrc_class_nr_opts::to_number() const
{
  if (value == ul_ccch1) {
    return 1;
  }
  invalid_enum_number(value, "rrc_class_nr_e");
  return 0;
}

// RRC-MessageID ::= SEQUENCE
static const asn1::table::seq_field  seq_table_rrc_msg_id_s_fields[] = {{offsetof(rrc_msg_id_s, rrc_type),
                                                                         asn1::table::seq_field::mandatory,
                                                                         asn1::table::field_op::choice,
                                                                         false,
                                                                         false,
                                                                         0,
                                                                         0,
                                                                         nullptr,
                                                                         &rrc_msg_id_s::rrc_type_c_::desc},
                                                                        {offsetof(rrc_msg_id_s, msg_id),
                                                                         asn1::table::seq_field::mandatory,
                                                                         asn1::table::field_op::int_i64,
                                                                         false,
                                                                         true,
                                                                         std::numeric_limits<int64_t>::min(),
                                                                         uint64_t{18446744073709551615U},
                                                                         nullptr}};
static const asn1::table::field_name seq_table_rrc_msg_id_s_names[]  = {{"rrcType", nullptr}, {"messageID", nullptr}};
const asn1::table::seq_desc          rrc_msg_id_s::desc              = {
    {seq_table_rrc_msg_id_s_fields, uint64_t{0x3}, nullptr, seq_table_rrc_msg_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::storage_ops* const choice_storage_rrc_msg_id_s_rrc_type_c__alternatives[] = {
    &asn1::table::alternative_ops<rrc_class_lte_e, true>::value,
    &asn1::table::alternative_ops<rrc_class_nr_e, true>::value};
const asn1::table::choice_storage_desc rrc_msg_id_s::rrc_type_c_::storage_desc = {
    choice_storage_rrc_msg_id_s_rrc_type_c__alternatives,
    2,
    "rrc_msg_id_s::rrc_type_c_"};

rrc_class_lte_e& rrc_msg_id_s::rrc_type_c_::set_lte()
{
  set(types::lte);
  return c.get<rrc_class_lte_e>();
}
rrc_class_nr_e& rrc_msg_id_s::rrc_type_c_::set_nr()
{
  set(types::nr);
  return c.get<rrc_class_nr_e>();
}
static const asn1::table::seq_field choice_table_rrc_msg_id_s_rrc_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<rrc_class_lte_e>(),
     rrc_class_lte_e::has_ext,
     false,
     rrc_class_lte_e::nof_types,
     rrc_class_lte_e::nof_exts,
     nullptr,
     &rrc_class_lte_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<rrc_class_nr_e>(),
     rrc_class_nr_e::has_ext,
     false,
     rrc_class_nr_e::nof_types,
     rrc_class_nr_e::nof_exts,
     nullptr,
     &rrc_class_nr_opts::desc}};
static const asn1::table::field_name choice_table_rrc_msg_id_s_rrc_type_c__names[] = {{"lTE", nullptr},
                                                                                      {"nR", nullptr}};
const asn1::table::choice_desc       rrc_msg_id_s::rrc_type_c_::desc               = {
    asn1::table::enum_int_type<rrc_msg_id_s::rrc_type_c_::types>(),
    rrc_msg_id_s::rrc_type_c_::types::nof_types,
    rrc_msg_id_s::rrc_type_c_::types::nof_exts,
    rrc_msg_id_s::rrc_type_c_::types::has_ext,
    true,
    choice_table_rrc_msg_id_s_rrc_type_c__alternatives,
    nullptr,
    choice_table_rrc_msg_id_s_rrc_type_c__names,
    "rrc_msg_id_s::rrc_type_c_",
    offsetof(rrc_msg_id_s::rrc_type_c_, c.type),
    offsetof(rrc_msg_id_s::rrc_type_c_, c.value),
    false,
    &rrc_msg_id_s::rrc_type_c_::storage_desc};

static const char*           enum_names_rrc_msg_id_s_rrc_type_c__types_values[] = {"lTE", "nR"};
const asn1::table::enum_desc rrc_msg_id_s::rrc_type_c_::types_opts::desc        = {
    enum_names_rrc_msg_id_s_rrc_type_c__types_values,
    2,
    "rrc_msg_id_s::rrc_type_c_::types"};

// S-NSSAI ::= SEQUENCE
static const asn1::table::seq_field seq_table_s_nssai_s_fields[] = {
    {offsetof(s_nssai_s, sst),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(s_nssai_s::sst), asn1::table::string_json>::value},
    {offsetof(s_nssai_s, sd),
     (int32_t)offsetof(s_nssai_s, sd_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(s_nssai_s::sd), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_s_nssai_s_names[] = {{"sST", nullptr}, {"sD", nullptr}};
static const asn1::table::seq_opt    seq_table_s_nssai_s_opts[]  = {{1, (int32_t)offsetof(s_nssai_s, sd_present)}};
const asn1::table::seq_desc          s_nssai_s::desc             = {
    {seq_table_s_nssai_s_fields, uint64_t{0x1}, seq_table_s_nssai_s_opts, seq_table_s_nssai_s_names, 2, 1},
    true,
    true,
    nullptr,
    0};

// ServingCell-ARFCN ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_serving_cell_arfcn_c_alternatives[] = {
    &asn1::table::alternative_ops<nr_arfcn_s, false>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value};
const asn1::table::choice_storage_desc serving_cell_arfcn_c::storage_desc = {
    choice_storage_serving_cell_arfcn_c_alternatives,
    2,
    "serving_cell_arfcn_c"};

nr_arfcn_s& serving_cell_arfcn_c::set_nr()
{
  set(types::nr);
  return c.get<nr_arfcn_s>();
}
uint32_t& serving_cell_arfcn_c::set_eutra()
{
  set(types::eutra);
  return c.get<uint32_t>();
}
static const asn1::table::seq_field choice_table_serving_cell_arfcn_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_arfcn_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name choice_table_serving_cell_arfcn_c_names[] = {{"nR", nullptr}, {"eUTRA", nullptr}};
const asn1::table::choice_desc serving_cell_arfcn_c::desc = {asn1::table::enum_int_type<serving_cell_arfcn_c::types>(),
                                                             serving_cell_arfcn_c::types::nof_types,
                                                             serving_cell_arfcn_c::types::nof_exts,
                                                             serving_cell_arfcn_c::types::has_ext,
                                                             true,
                                                             choice_table_serving_cell_arfcn_c_alternatives,
                                                             nullptr,
                                                             choice_table_serving_cell_arfcn_c_names,
                                                             "serving_cell_arfcn_c",
                                                             offsetof(serving_cell_arfcn_c, c.type),
                                                             offsetof(serving_cell_arfcn_c, c.value),
                                                             false,
                                                             &serving_cell_arfcn_c::storage_desc};

static const char*           enum_names_serving_cell_arfcn_c_types_values[] = {"nR", "eUTRA"};
const asn1::table::enum_desc serving_cell_arfcn_c::types_opts::desc = {enum_names_serving_cell_arfcn_c_types_values,
                                                                       2,
                                                                       "serving_cell_arfcn_c::types"};

// ServingCell-PCI ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_serving_cell_pci_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc serving_cell_pci_c::storage_desc = {
    choice_storage_serving_cell_pci_c_alternatives,
    2,
    "serving_cell_pci_c"};

uint16_t& serving_cell_pci_c::set_nr()
{
  set(types::nr);
  return c.get<uint16_t>();
}
uint16_t& serving_cell_pci_c::set_eutra()
{
  set(types::eutra);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_serving_cell_pci_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{1007},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{503},
     nullptr}};
static const asn1::table::field_name choice_table_serving_cell_pci_c_names[] = {{"nR", nullptr}, {"eUTRA", nullptr}};
const asn1::table::choice_desc serving_cell_pci_c::desc = {asn1::table::enum_int_type<serving_cell_pci_c::types>(),
                                                           serving_cell_pci_c::types::nof_types,
                                                           serving_cell_pci_c::types::nof_exts,
                                                           serving_cell_pci_c::types::has_ext,
                                                           true,
                                                           choice_table_serving_cell_pci_c_alternatives,
                                                           nullptr,
                                                           choice_table_serving_cell_pci_c_names,
                                                           "serving_cell_pci_c",
                                                           offsetof(serving_cell_pci_c, c.type),
                                                           offsetof(serving_cell_pci_c, c.value),
                                                           false,
                                                           &serving_cell_pci_c::storage_desc};

static const char*           enum_names_serving_cell_pci_c_types_values[] = {"nR", "eUTRA"};
const asn1::table::enum_desc serving_cell_pci_c::types_opts::desc         = {enum_names_serving_cell_pci_c_types_values,
                                                                             2,
                                                                             "serving_cell_pci_c::types"};

// UEID-GNB-CU-CP-F1AP-ID-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_id_gnb_cu_cp_f1ap_id_item_s_fields[] = {
    {offsetof(ue_id_gnb_cu_cp_f1ap_id_item_s, gnb_cu_ue_f1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr}};
static const asn1::table::field_name seq_table_ue_id_gnb_cu_cp_f1ap_id_item_s_names[] = {
    {"gNB-CU-UE-F1AP-ID", nullptr}};
const asn1::table::seq_desc ue_id_gnb_cu_cp_f1ap_id_item_s::desc = {{seq_table_ue_id_gnb_cu_cp_f1ap_id_item_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_ue_id_gnb_cu_cp_f1ap_id_item_s_names,
                                                                     1,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// UEID-GNB-CU-CP-E1AP-ID-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_id_gnb_cu_cp_e1ap_id_item_s_fields[] = {
    {offsetof(ue_id_gnb_cu_cp_e1ap_id_item_s, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr}};
static const asn1::table::field_name seq_table_ue_id_gnb_cu_cp_e1ap_id_item_s_names[] = {
    {"gNB-CU-CP-UE-E1AP-ID", nullptr}};
const asn1::table::seq_desc ue_id_gnb_cu_cp_e1ap_id_item_s::desc = {{seq_table_ue_id_gnb_cu_cp_e1ap_id_item_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_ue_id_gnb_cu_cp_e1ap_id_item_s_names,
                                                                     1,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// UEID-GNB ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_id_gnb_s_fields[] = {
    {offsetof(ue_id_gnb_s, amf_ue_ngap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{1099511627775},
     nullptr},
    {offsetof(ue_id_gnb_s, guami),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &guami_s::desc},
    {offsetof(ue_id_gnb_s, gnb_cu_ue_f1ap_id_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     3,
     &asn1::table::item_list_codec<decltype(ue_id_gnb_s::gnb_cu_ue_f1ap_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_id_gnb_cu_cp_f1ap_id_item_s::desc},
    {offsetof(ue_id_gnb_s, gnb_cu_cp_ue_e1ap_id_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ue_id_gnb_s::gnb_cu_cp_ue_e1ap_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_id_gnb_cu_cp_e1ap_id_item_s::desc},
    {offsetof(ue_id_gnb_s, ran_ue_id),
     (int32_t)offsetof(ue_id_gnb_s, ran_ue_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ue_id_gnb_s::ran_ue_id), asn1::table::string_json>::value},
    {offsetof(ue_id_gnb_s, m_ng_ran_ue_xnap_id),
     (int32_t)offsetof(ue_id_gnb_s, m_ng_ran_ue_xnap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(ue_id_gnb_s, global_gnb_id),
     (int32_t)offsetof(ue_id_gnb_s, global_gnb_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_gnb_id_s::desc}};
static const asn1::table::field_name seq_table_ue_id_gnb_s_names[] = {{"amf-UE-NGAP-ID", nullptr},
                                                                      {"guami", nullptr},
                                                                      {"gNB-CU-UE-F1AP-ID-List", nullptr},
                                                                      {"gNB-CU-CP-UE-E1AP-ID-List", nullptr},
                                                                      {"ran-UEID", nullptr},
                                                                      {"m-NG-RAN-UE-XnAP-ID", nullptr},
                                                                      {"globalGNB-ID", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_gnb_s_opts[]  = {
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(ue_id_gnb_s, ran_ue_id_present)},
    {5, (int32_t)offsetof(ue_id_gnb_s, m_ng_ran_ue_xnap_id_present)},
    {6, (int32_t)offsetof(ue_id_gnb_s, global_gnb_id_present)}};
static const asn1::table::seq_field seq_table_ue_id_gnb_s_g0_fields[] = {
    {offsetof(ue_id_gnb_s, global_ng_ran_node_id),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(ue_id_gnb_s::global_ng_ran_node_id)>::value,
     &global_ngran_node_id_c::desc}};
static const asn1::table::field_name seq_table_ue_id_gnb_s_g0_names[]  = {{"globalNG-RANNode-ID", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_gnb_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_ue_id_gnb_s_g1_fields[] = {
    {offsetof(ue_id_gnb_s, cell_rnti),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(ue_id_gnb_s::cell_rnti)>::value,
      &cell_rnti_s::desc}};
static const asn1::table::field_name seq_table_ue_id_gnb_s_g1_names[] = {{"cell-RNTI", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_gnb_s_g1_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_ue_id_gnb_s_groups[]   = {{{seq_table_ue_id_gnb_s_g0_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_ue_id_gnb_s_g0_opts,
                                                                           seq_table_ue_id_gnb_s_g0_names,
                                                                           1,
                                                                           1},
                                                                          true},
                                                                         {{seq_table_ue_id_gnb_s_g1_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_ue_id_gnb_s_g1_opts,
                                                                           seq_table_ue_id_gnb_s_g1_names,
                                                                           1,
                                                                           1},
                                                                          true}};
const asn1::table::seq_desc          ue_id_gnb_s::desc                = {
    {seq_table_ue_id_gnb_s_fields, uint64_t{0x3}, seq_table_ue_id_gnb_s_opts, seq_table_ue_id_gnb_s_names, 7, 5},
    true,
    true,
    seq_table_ue_id_gnb_s_groups,
    2};

// UEID-GNB-DU ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_id_gnb_du_s_fields[] = {
    {offsetof(ue_id_gnb_du_s, gnb_cu_ue_f1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(ue_id_gnb_du_s, ran_ue_id),
     (int32_t)offsetof(ue_id_gnb_du_s, ran_ue_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ue_id_gnb_du_s::ran_ue_id), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_ue_id_gnb_du_s_names[] = {{"gNB-CU-UE-F1AP-ID", nullptr},
                                                                         {"ran-UEID", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_gnb_du_s_opts[]  = {
    {1, (int32_t)offsetof(ue_id_gnb_du_s, ran_ue_id_present)}};
static const asn1::table::seq_field seq_table_ue_id_gnb_du_s_g0_fields[] = {
    {offsetof(ue_id_gnb_du_s, cell_rnti),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(ue_id_gnb_du_s::cell_rnti)>::value,
     &cell_rnti_s::desc}};
static const asn1::table::field_name seq_table_ue_id_gnb_du_s_g0_names[] = {{"cell-RNTI", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_gnb_du_s_g0_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_ue_id_gnb_du_s_groups[]   = {{{seq_table_ue_id_gnb_du_s_g0_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_ue_id_gnb_du_s_g0_opts,
                                                                              seq_table_ue_id_gnb_du_s_g0_names,
                                                                              1,
                                                                              1},
                                                                             true}};
const asn1::table::seq_desc          ue_id_gnb_du_s::desc                = {{seq_table_ue_id_gnb_du_s_fields,
                                                                             uint64_t{0x1},
                                                                             seq_table_ue_id_gnb_du_s_opts,
                                                                             seq_table_ue_id_gnb_du_s_names,
                                                                             2,
                                                                             1},
                                                                            true,
                                                                            true,
                                                                            seq_table_ue_id_gnb_du_s_groups,
                                                                            1};

// UEID-GNB-CU-UP ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_id_gnb_cu_up_s_fields[] = {
    {offsetof(ue_id_gnb_cu_up_s, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(ue_id_gnb_cu_up_s, ran_ue_id),
     (int32_t)offsetof(ue_id_gnb_cu_up_s, ran_ue_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ue_id_gnb_cu_up_s::ran_ue_id), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_ue_id_gnb_cu_up_s_names[] = {{"gNB-CU-CP-UE-E1AP-ID", nullptr},
                                                                            {"ran-UEID", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_gnb_cu_up_s_opts[]  = {
    {1, (int32_t)offsetof(ue_id_gnb_cu_up_s, ran_ue_id_present)}};
const asn1::table::seq_desc ue_id_gnb_cu_up_s::desc = {{seq_table_ue_id_gnb_cu_up_s_fields,
                                                        uint64_t{0x1},
                                                        seq_table_ue_id_gnb_cu_up_s_opts,
                                                        seq_table_ue_id_gnb_cu_up_s_names,
                                                        2,
                                                        1},
                                                       true,
                                                       true,
                                                       nullptr,
                                                       0};

// UEID-NG-ENB ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_id_ng_enb_s_fields[] = {
    {offsetof(ue_id_ng_enb_s, amf_ue_ngap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{1099511627775},
     nullptr},
    {offsetof(ue_id_ng_enb_s, guami),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &guami_s::desc},
    {offsetof(ue_id_ng_enb_s, ng_enb_cu_ue_w1_ap_id),
     (int32_t)offsetof(ue_id_ng_enb_s, ng_enb_cu_ue_w1_ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(ue_id_ng_enb_s, m_ng_ran_ue_xnap_id),
     (int32_t)offsetof(ue_id_ng_enb_s, m_ng_ran_ue_xnap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(ue_id_ng_enb_s, global_ng_enb_id),
     (int32_t)offsetof(ue_id_ng_enb_s, global_ng_enb_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_ng_enb_id_s::desc}};
static const asn1::table::field_name seq_table_ue_id_ng_enb_s_names[] = {{"amf-UE-NGAP-ID", nullptr},
                                                                         {"guami", nullptr},
                                                                         {"ng-eNB-CU-UE-W1AP-ID", nullptr},
                                                                         {"m-NG-RAN-UE-XnAP-ID", nullptr},
                                                                         {"globalNgENB-ID", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_ng_enb_s_opts[]  = {
    {2, (int32_t)offsetof(ue_id_ng_enb_s, ng_enb_cu_ue_w1_ap_id_present)},
    {3, (int32_t)offsetof(ue_id_ng_enb_s, m_ng_ran_ue_xnap_id_present)},
    {4, (int32_t)offsetof(ue_id_ng_enb_s, global_ng_enb_id_present)}};
static const asn1::table::seq_field seq_table_ue_id_ng_enb_s_g0_fields[] = {
    {offsetof(ue_id_ng_enb_s, global_ng_ran_node_id),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(ue_id_ng_enb_s::global_ng_ran_node_id)>::value,
     &global_ngran_node_id_c::desc}};
static const asn1::table::field_name seq_table_ue_id_ng_enb_s_g0_names[]  = {{"globalNG-RANNode-ID", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_ng_enb_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_ue_id_ng_enb_s_g1_fields[] = {
    {offsetof(ue_id_ng_enb_s, cell_rnti),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(ue_id_ng_enb_s::cell_rnti)>::value,
      &cell_rnti_s::desc}};
static const asn1::table::field_name seq_table_ue_id_ng_enb_s_g1_names[] = {{"cell-RNTI", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_ng_enb_s_g1_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_ue_id_ng_enb_s_groups[]   = {{{seq_table_ue_id_ng_enb_s_g0_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_ue_id_ng_enb_s_g0_opts,
                                                                              seq_table_ue_id_ng_enb_s_g0_names,
                                                                              1,
                                                                              1},
                                                                             true},
                                                                            {{seq_table_ue_id_ng_enb_s_g1_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_ue_id_ng_enb_s_g1_opts,
                                                                              seq_table_ue_id_ng_enb_s_g1_names,
                                                                              1,
                                                                              1},
                                                                             true}};
const asn1::table::seq_desc          ue_id_ng_enb_s::desc                = {{seq_table_ue_id_ng_enb_s_fields,
                                                                             uint64_t{0x3},
                                                                             seq_table_ue_id_ng_enb_s_opts,
                                                                             seq_table_ue_id_ng_enb_s_names,
                                                                             5,
                                                                             3},
                                                                            true,
                                                                            true,
                                                                            seq_table_ue_id_ng_enb_s_groups,
                                                                            2};

// UEID-NG-ENB-DU ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_id_ng_enb_du_s_fields[] = {
    {offsetof(ue_id_ng_enb_du_s, ng_enb_cu_ue_w1_ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr}};
static const asn1::table::field_name seq_table_ue_id_ng_enb_du_s_names[]     = {{"ng-eNB-CU-UE-W1AP-ID", nullptr}};
static const asn1::table::seq_field  seq_table_ue_id_ng_enb_du_s_g0_fields[] = {
    {offsetof(ue_id_ng_enb_du_s, cell_rnti),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(ue_id_ng_enb_du_s::cell_rnti)>::value,
      &cell_rnti_s::desc}};
static const asn1::table::field_name seq_table_ue_id_ng_enb_du_s_g0_names[] = {{"cell-RNTI", nullptr}};
static const asn1::table::seq_opt   seq_table_ue_id_ng_enb_du_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_ue_id_ng_enb_du_s_groups[]  = {{{seq_table_ue_id_ng_enb_du_s_g0_fields,
                                                                               uint64_t{0x0},
                                                                               seq_table_ue_id_ng_enb_du_s_g0_opts,
                                                                               seq_table_ue_id_ng_enb_du_s_g0_names,
                                                                               1,
                                                                               1},
                                                                              true}};
const asn1::table::seq_desc         ue_id_ng_enb_du_s::desc               = {
    {seq_table_ue_id_ng_enb_du_s_fields, uint64_t{0x1}, nullptr, seq_table_ue_id_ng_enb_du_s_names, 1, 0},
    true,
    true,
    seq_table_ue_id_ng_enb_du_s_groups,
    1};

// UEID-EN-GNB ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_id_en_gnb_s_fields[] = {
    {offsetof(ue_id_en_gnb_s, m_enb_ue_x2ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ue_id_en_gnb_s, m_enb_ue_x2ap_id_ext),
     (int32_t)offsetof(ue_id_en_gnb_s, m_enb_ue_x2ap_id_ext_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ue_id_en_gnb_s, global_enb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_enb_id_s::desc},
    {offsetof(ue_id_en_gnb_s, gnb_cu_ue_f1ap_id),
     (int32_t)offsetof(ue_id_en_gnb_s, gnb_cu_ue_f1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(ue_id_en_gnb_s, gnb_cu_cp_ue_e1ap_id_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ue_id_en_gnb_s::gnb_cu_cp_ue_e1ap_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_id_gnb_cu_cp_e1ap_id_item_s::desc},
    {offsetof(ue_id_en_gnb_s, ran_ue_id),
     (int32_t)offsetof(ue_id_en_gnb_s, ran_ue_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ue_id_en_gnb_s::ran_ue_id), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_ue_id_en_gnb_s_names[] = {{"m-eNB-UE-X2AP-ID", nullptr},
                                                                         {"m-eNB-UE-X2AP-ID-Extension", nullptr},
                                                                         {"globalENB-ID", nullptr},
                                                                         {"gNB-CU-UE-F1AP-ID", nullptr},
                                                                         {"gNB-CU-CP-UE-E1AP-ID-List", nullptr},
                                                                         {"ran-UEID", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_en_gnb_s_opts[]  = {
    {1, (int32_t)offsetof(ue_id_en_gnb_s, m_enb_ue_x2ap_id_ext_present)},
    {3, (int32_t)offsetof(ue_id_en_gnb_s, gnb_cu_ue_f1ap_id_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(ue_id_en_gnb_s, ran_ue_id_present)}};
static const asn1::table::seq_field seq_table_ue_id_en_gnb_s_g0_fields[] = {
    {offsetof(ue_id_en_gnb_s, cell_rnti),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(ue_id_en_gnb_s::cell_rnti)>::value,
     &cell_rnti_s::desc}};
static const asn1::table::field_name seq_table_ue_id_en_gnb_s_g0_names[] = {{"cell-RNTI", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_en_gnb_s_g0_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_ue_id_en_gnb_s_groups[]   = {{{seq_table_ue_id_en_gnb_s_g0_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_ue_id_en_gnb_s_g0_opts,
                                                                              seq_table_ue_id_en_gnb_s_g0_names,
                                                                              1,
                                                                              1},
                                                                             true}};
const asn1::table::seq_desc          ue_id_en_gnb_s::desc                = {{seq_table_ue_id_en_gnb_s_fields,
                                                                             uint64_t{0x5},
                                                                             seq_table_ue_id_en_gnb_s_opts,
                                                                             seq_table_ue_id_en_gnb_s_names,
                                                                             6,
                                                                             4},
                                                                            true,
                                                                            true,
                                                                            seq_table_ue_id_en_gnb_s_groups,
                                                                            1};

// UEID-ENB ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_id_enb_s_fields[] = {
    {offsetof(ue_id_enb_s, mme_ue_s1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(ue_id_enb_s, gummei),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gummei_s::desc},
    {offsetof(ue_id_enb_s, m_enb_ue_x2ap_id),
     (int32_t)offsetof(ue_id_enb_s, m_enb_ue_x2ap_id_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ue_id_enb_s, m_enb_ue_x2ap_id_ext),
     (int32_t)offsetof(ue_id_enb_s, m_enb_ue_x2ap_id_ext_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ue_id_enb_s, global_enb_id),
     (int32_t)offsetof(ue_id_enb_s, global_enb_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_enb_id_s::desc}};
static const asn1::table::field_name seq_table_ue_id_enb_s_names[] = {{"mME-UE-S1AP-ID", nullptr},
                                                                      {"gUMMEI", nullptr},
                                                                      {"m-eNB-UE-X2AP-ID", nullptr},
                                                                      {"m-eNB-UE-X2AP-ID-Extension", nullptr},
                                                                      {"globalENB-ID", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_enb_s_opts[]  = {
    {2, (int32_t)offsetof(ue_id_enb_s, m_enb_ue_x2ap_id_present)},
    {3, (int32_t)offsetof(ue_id_enb_s, m_enb_ue_x2ap_id_ext_present)},
    {4, (int32_t)offsetof(ue_id_enb_s, global_enb_id_present)}};
static const asn1::table::seq_field seq_table_ue_id_enb_s_g0_fields[] = {
    {offsetof(ue_id_enb_s, cell_rnti),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(ue_id_enb_s::cell_rnti)>::value,
     &cell_rnti_s::desc}};
static const asn1::table::field_name seq_table_ue_id_enb_s_g0_names[] = {{"cell-RNTI", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_id_enb_s_g0_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_ue_id_enb_s_groups[]   = {{{seq_table_ue_id_enb_s_g0_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_ue_id_enb_s_g0_opts,
                                                                           seq_table_ue_id_enb_s_g0_names,
                                                                           1,
                                                                           1},
                                                                          true}};
const asn1::table::seq_desc          ue_id_enb_s::desc                = {
    {seq_table_ue_id_enb_s_fields, uint64_t{0x3}, seq_table_ue_id_enb_s_opts, seq_table_ue_id_enb_s_names, 5, 3},
    true,
    true,
    seq_table_ue_id_enb_s_groups,
    1};

// UEID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ue_id_c_alternatives[] = {
    &asn1::table::alternative_ops<ue_id_gnb_s, false>::value,
    &asn1::table::alternative_ops<ue_id_gnb_du_s, false>::value,
    &asn1::table::alternative_ops<ue_id_gnb_cu_up_s, false>::value,
    &asn1::table::alternative_ops<ue_id_ng_enb_s, false>::value,
    &asn1::table::alternative_ops<ue_id_ng_enb_du_s, false>::value,
    &asn1::table::alternative_ops<ue_id_en_gnb_s, false>::value,
    &asn1::table::alternative_ops<ue_id_enb_s, false>::value};
const asn1::table::choice_storage_desc ue_id_c::storage_desc = {choice_storage_ue_id_c_alternatives, 7, "ue_id_c"};

ue_id_gnb_s& ue_id_c::set_gnb_ue_id()
{
  set(types::gnb_ue_id);
  return c.get<ue_id_gnb_s>();
}
ue_id_gnb_du_s& ue_id_c::set_gnb_du_ue_id()
{
  set(types::gnb_du_ue_id);
  return c.get<ue_id_gnb_du_s>();
}
ue_id_gnb_cu_up_s& ue_id_c::set_gnb_cu_up_ue_id()
{
  set(types::gnb_cu_up_ue_id);
  return c.get<ue_id_gnb_cu_up_s>();
}
ue_id_ng_enb_s& ue_id_c::set_ng_enb_ue_id()
{
  set(types::ng_enb_ue_id);
  return c.get<ue_id_ng_enb_s>();
}
ue_id_ng_enb_du_s& ue_id_c::set_ng_enb_du_ue_id()
{
  set(types::ng_enb_du_ue_id);
  return c.get<ue_id_ng_enb_du_s>();
}
ue_id_en_gnb_s& ue_id_c::set_en_gnb_ue_id()
{
  set(types::en_gnb_ue_id);
  return c.get<ue_id_en_gnb_s>();
}
ue_id_enb_s& ue_id_c::set_enb_ue_id()
{
  set(types::enb_ue_id);
  return c.get<ue_id_enb_s>();
}
static const asn1::table::seq_field  choice_table_ue_id_c_alternatives[] = {{0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &ue_id_gnb_s::desc},
                                                                            {0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &ue_id_gnb_du_s::desc},
                                                                            {0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &ue_id_gnb_cu_up_s::desc},
                                                                            {0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &ue_id_ng_enb_s::desc},
                                                                            {0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &ue_id_ng_enb_du_s::desc},
                                                                            {0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &ue_id_en_gnb_s::desc},
                                                                            {0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &ue_id_enb_s::desc}};
static const asn1::table::field_name choice_table_ue_id_c_names[]        = {{"gNB-UEID", nullptr},
                                                                            {"gNB-DU-UEID", nullptr},
                                                                            {"gNB-CU-UP-UEID", nullptr},
                                                                            {"ng-eNB-UEID", nullptr},
                                                                            {"ng-eNB-DU-UEID", nullptr},
                                                                            {"en-gNB-UEID", nullptr},
                                                                            {"eNB-UEID", nullptr}};
const asn1::table::choice_desc       ue_id_c::desc = {asn1::table::enum_int_type<ue_id_c::types>(),
                                                      ue_id_c::types::nof_types,
                                                      ue_id_c::types::nof_exts,
                                                      ue_id_c::types::has_ext,
                                                      true,
                                                      choice_table_ue_id_c_alternatives,
                                                      nullptr,
                                                      choice_table_ue_id_c_names,
                                                      "ue_id_c",
                                                      offsetof(ue_id_c, c.type),
                                                      offsetof(ue_id_c, c.value),
                                                      false,
                                                      &ue_id_c::storage_desc};

static const char* enum_names_ue_id_c_types_values[] =
    {"gNB-UEID", "gNB-DU-UEID", "gNB-CU-UP-UEID", "ng-eNB-UEID", "ng-eNB-DU-UEID", "en-gNB-UEID", "eNB-UEID"};
const asn1::table::enum_desc ue_id_c::types_opts::desc = {enum_names_ue_id_c_types_values, 7, "ue_id_c::types"};
