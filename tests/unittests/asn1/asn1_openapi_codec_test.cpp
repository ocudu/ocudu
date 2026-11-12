// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/asn1_openapi_codec.h"
#include "ocudu/asn1/e2sm/e2sm_common_ies.h"
#include "ocudu/asn1/e2sm/e2sm_kpm_ies.h"
#include <gtest/gtest.h>
#include <limits>
#include <new>

using namespace asn1;
using namespace asn1::e2sm;

namespace {

/// OpenAPI table of S-NSSAI, whose two fields are octet strings that write themselves as strings.
const openapi::field_desc s_nssai_fields[] = {{&openapi::string_value<fixed_octstring<1, true>>::codec},
                                              {&openapi::string_value<fixed_octstring<3, true>>::codec}};
const openapi::seq_desc   s_nssai_table    = {&s_nssai_s::desc, s_nssai_fields};

/// OpenAPI table of MeasurementLabel. Only the octet string and the nested SEQUENCE need an entry; the engine reads
/// and writes the integers and the enumerated values from the shape of the SEQUENCE alone.
/// The entries follow the fields of the SEQUENCE in order: noLabel, plmnID, sliceID, fiveQI, qFI, qCI, qCImax,
/// qCImin, aRPmax, aRPmin, bitrateRange, layerMU-MIMO, sUM, distBinX, distBinY, distBinZ, preLabelOverride,
/// startEndInd, min, max and avg.
const openapi::field_desc meas_label_fields[] = {{},
                                                 {&openapi::string_value<fixed_octstring<3, true>>::codec},
                                                 {nullptr, &s_nssai_table},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {},
                                                 {}};
const openapi::seq_desc   meas_label_table    = {&meas_label_s::desc, meas_label_fields};

} // namespace

TEST(asn1_openapi_codec_test, sequence_writes_only_the_fields_that_are_present)
{
  meas_label_s label;
  label.plmn_id_present = true;
  label.plmn_id.from_string("00f110");
  label.five_qi_present  = true;
  label.five_qi          = 9;
  label.no_label_present = true;
  label.no_label         = meas_label_s::no_label_opts::true_value;

  nlohmann::ordered_json j;
  openapi::to_json(j, &label, meas_label_table);

  ASSERT_EQ(j.dump(), R"({"noLabel":"true","plmnID":"00f110","fiveQI":9})");
}

TEST(asn1_openapi_codec_test, sequence_round_trips_through_its_shape_table)
{
  meas_label_s label;
  label.plmn_id_present = true;
  label.plmn_id.from_string("00f110");
  label.slice_id_present = true;
  label.slice_id.sst.from_string("01");
  label.slice_id.sd_present = true;
  label.slice_id.sd.from_string("000017");
  label.qc_imax_present = true;
  label.qc_imax         = 15;

  nlohmann::ordered_json j;
  openapi::to_json(j, &label, meas_label_table);

  meas_label_s decoded;
  ASSERT_TRUE(openapi::from_json(j, &decoded, meas_label_table));
  ASSERT_TRUE(decoded.plmn_id_present);
  ASSERT_EQ(decoded.plmn_id.to_string(), "00f110");
  ASSERT_TRUE(decoded.slice_id_present);
  ASSERT_EQ(decoded.slice_id.sst.to_string(), "01");
  ASSERT_TRUE(decoded.slice_id.sd_present);
  ASSERT_EQ(decoded.slice_id.sd.to_string(), "000017");
  ASSERT_TRUE(decoded.qc_imax_present);
  ASSERT_EQ(decoded.qc_imax, 15);
  ASSERT_FALSE(decoded.five_qi_present);
  ASSERT_FALSE(decoded.no_label_present);

  nlohmann::ordered_json round_tripped;
  openapi::to_json(round_tripped, &decoded, meas_label_table);
  ASSERT_EQ(round_tripped.dump(), j.dump());
}

TEST(asn1_openapi_codec_test, a_member_the_json_does_not_carry_is_absent_in_a_destination_that_held_one)
{
  // A second read into the same value must not leave what the first one put there.
  meas_label_s decoded;
  ASSERT_TRUE(openapi::from_json(
      nlohmann::ordered_json::parse(R"({"plmnID": "00f110", "fiveQI": 9})"), &decoded, meas_label_table));
  ASSERT_TRUE(decoded.plmn_id_present);
  ASSERT_TRUE(decoded.five_qi_present);

  ASSERT_TRUE(openapi::from_json(nlohmann::ordered_json::parse(R"({"fiveQI": 7})"), &decoded, meas_label_table));
  ASSERT_FALSE(decoded.plmn_id_present);
  ASSERT_EQ(decoded.five_qi, 7);

  nlohmann::ordered_json written;
  openapi::to_json(written, &decoded, meas_label_table);
  ASSERT_EQ(written.dump(), R"({"fiveQI":7})");
}

TEST(asn1_openapi_codec_test, absent_optional_fields_are_left_absent)
{
  // The destination is marked as holding both fields beforehand, so that reading an object that carries neither is
  // what clears them, rather than the value never having held one.
  meas_label_s decoded;
  decoded.plmn_id_present  = true;
  decoded.slice_id_present = true;

  ASSERT_TRUE(openapi::from_json(nlohmann::ordered_json::object(), &decoded, meas_label_table));
  ASSERT_FALSE(decoded.plmn_id_present);
  ASSERT_FALSE(decoded.slice_id_present);
}

TEST(asn1_openapi_codec_test, a_value_outside_the_bounds_of_its_field_fails_to_read)
{
  // aRPmax is an INTEGER (1..15), so 16 falls outside it.
  auto j = nlohmann::ordered_json::parse(R"({"aRPmax": 16})");

  meas_label_s decoded;
  ASSERT_FALSE(openapi::from_json(j, &decoded, meas_label_table));
}

TEST(asn1_openapi_codec_test, a_string_that_names_no_enumerator_fails_to_read)
{
  auto j = nlohmann::ordered_json::parse(R"({"noLabel": "maybe"})");

  meas_label_s decoded;
  ASSERT_FALSE(openapi::from_json(j, &decoded, meas_label_table));
}

TEST(asn1_openapi_codec_test, a_value_of_the_wrong_JSON_type_fails_to_read)
{
  auto j = nlohmann::ordered_json::parse(R"({"fiveQI": "nine"})");

  meas_label_s decoded;
  ASSERT_FALSE(openapi::from_json(j, &decoded, meas_label_table));
}

/*********************************************************
  A CHOICE whose alternatives a oneOf of a schema selects
*********************************************************/

namespace {

/// Alternative whose keys are a subset of the keys of cell_du_s. A schema leaves every property optional.
struct cell_cu_s {
  bool     cell_local_id_present  = false;
  bool     plmn_info_list_present = false;
  uint32_t cell_local_id          = 0;
  uint32_t plmn_info_list         = 0;
};

struct cell_du_s {
  bool     cell_local_id_present  = false;
  bool     plmn_info_list_present = false;
  bool     bwp_list_present       = false;
  uint32_t cell_local_id          = 0;
  uint32_t plmn_info_list         = 0;
  uint32_t bwp_list               = 0;
};

/// CHOICE of the two alternatives, laid out as the generated CHOICE types are.
struct cfg_structure_c {
  enum options { cell_cu, cell_du, nulltype };

  uint8_t type = nulltype;
  union {
    cell_cu_s cu;
    cell_du_s du;
  };

  cfg_structure_c() : du() {}
};

void set_cfg_structure(void* choice, uint32_t type)
{
  // The setter of a generated CHOICE constructs the alternative it selects, and the engine reads into it, so one
  // that only wrote the type would leave the engine writing to a member whose lifetime has not begun.
  auto* c = static_cast<cfg_structure_c*>(choice);
  c->type = static_cast<uint8_t>(type);
  if (type == cfg_structure_c::cell_cu) {
    new (&c->cu) cell_cu_s{};
  } else {
    new (&c->du) cell_du_s{};
  }
}

const table::seq_field  cell_cu_fields[] = {{offsetof(cell_cu_s, cell_local_id),
                                             (int32_t)offsetof(cell_cu_s, cell_local_id_present),
                                             table::field_op::int_u32,
                                             false,
                                             false,
                                             0,
                                             16383},
                                            {offsetof(cell_cu_s, plmn_info_list),
                                             (int32_t)offsetof(cell_cu_s, plmn_info_list_present),
                                             table::field_op::int_u32,
                                             false,
                                             false,
                                             0,
                                             16383}};
const table::field_name cell_cu_names[]  = {{"cellLocalId", nullptr}, {"plmnInfoList", nullptr}};
const table::seq_desc   cell_cu_shape    = {{cell_cu_fields, UINT64_C(0x0), nullptr, cell_cu_names, 2, 0}, false, true};

const table::seq_field  cell_du_fields[] = {{offsetof(cell_du_s, cell_local_id),
                                             (int32_t)offsetof(cell_du_s, cell_local_id_present),
                                             table::field_op::int_u32,
                                             false,
                                             false,
                                             0,
                                             16383},
                                            {offsetof(cell_du_s, plmn_info_list),
                                             (int32_t)offsetof(cell_du_s, plmn_info_list_present),
                                             table::field_op::int_u32,
                                             false,
                                             false,
                                             0,
                                             16383},
                                            {offsetof(cell_du_s, bwp_list),
                                             (int32_t)offsetof(cell_du_s, bwp_list_present),
                                             table::field_op::int_u32,
                                             false,
                                             false,
                                             0,
                                             16383}};
const table::field_name cell_du_names[]  = {{"cellLocalId", nullptr}, {"plmnInfoList", nullptr}, {"bwpList", nullptr}};
const table::seq_desc   cell_du_shape    = {{cell_du_fields, UINT64_C(0x0), nullptr, cell_du_names, 3, 0}, false, true};

const table::seq_field cfg_structure_alternatives[] = {
    {0, table::seq_field::mandatory, table::field_op::sequence, false, true, 0, 0, nullptr, &cell_cu_shape},
    {0, table::seq_field::mandatory, table::field_op::sequence, false, true, 0, 0, nullptr, &cell_du_shape}};
const table::field_name  cfg_structure_names[] = {{"O-NrCellCu", nullptr}, {"O-NrCellDu", nullptr}};
const table::choice_desc cfg_structure_shape   = {table::int_type::u8,
                                                  2,
                                                  0,
                                                  false,
                                                  true,
                                                  cfg_structure_alternatives,
                                                  &set_cfg_structure,
                                                  cfg_structure_names,
                                                  "cfg_structure_c",
                                                  offsetof(cfg_structure_c, type),
                                                  offsetof(cfg_structure_c, cu)};

const openapi::field_desc cell_cu_openapi_fields[] = {{}, {}};
const openapi::seq_desc   cell_cu_table            = {&cell_cu_shape, cell_cu_openapi_fields};
const openapi::field_desc cell_du_openapi_fields[] = {{}, {}, {}};
const openapi::seq_desc   cell_du_table            = {&cell_du_shape, cell_du_openapi_fields};

const char* const cell_cu_keys[] = {"cellLocalId", "plmnInfoList"};
const char* const cell_du_keys[] = {"cellLocalId", "plmnInfoList", "bwpList"};

const openapi::alternative_desc cfg_structure_openapi_alternatives[] = {{nullptr, &cell_cu_table, cell_cu_keys, 2},
                                                                        {nullptr, &cell_du_table, cell_du_keys, 3}};
const openapi::choice_desc      cfg_structure_table = {&cfg_structure_shape, cfg_structure_openapi_alternatives};

} // namespace

TEST(asn1_openapi_codec_test, a_choice_writes_its_alternative_without_a_name_wrapper)
{
  cfg_structure_c cfg;
  cfg.type                      = cfg_structure_c::cell_du;
  cfg.du.cell_local_id          = 7;
  cfg.du.cell_local_id_present  = true;
  cfg.du.plmn_info_list         = 1;
  cfg.du.plmn_info_list_present = true;
  cfg.du.bwp_list               = 3;
  cfg.du.bwp_list_present       = true;

  nlohmann::ordered_json j;
  openapi::to_json(j, &cfg, cfg_structure_table);

  ASSERT_EQ(j.dump(), R"({"cellLocalId":7,"plmnInfoList":1,"bwpList":3})");
}

TEST(asn1_openapi_codec_test, the_alternative_that_accepts_every_key_wins)
{
  // The keys of O-NrCellCu are a subset of the keys of O-NrCellDu, so probing one key at a time would take the first
  // alternative that matches rather than the one the object holds.
  auto j = nlohmann::ordered_json::parse(R"({"cellLocalId": 7, "plmnInfoList": 1, "bwpList": 3})");

  cfg_structure_c cfg;
  ASSERT_TRUE(openapi::from_json(j, &cfg, cfg_structure_table));
  ASSERT_EQ(cfg.type, cfg_structure_c::cell_du);
  ASSERT_EQ(cfg.du.cell_local_id, 7);
  ASSERT_EQ(cfg.du.bwp_list, 3);
}

TEST(asn1_openapi_codec_test, the_alternative_that_accepts_the_fewest_keys_fits_most_closely)
{
  auto j = nlohmann::ordered_json::parse(R"({"cellLocalId": 7, "plmnInfoList": 1})");

  cfg_structure_c cfg;
  ASSERT_TRUE(openapi::from_json(j, &cfg, cfg_structure_table));
  ASSERT_EQ(cfg.type, cfg_structure_c::cell_cu);
  ASSERT_EQ(cfg.cu.cell_local_id, 7);
}

TEST(asn1_openapi_codec_test, an_object_that_holds_a_subset_of_the_keys_of_an_alternative_selects_it)
{
  // The schema leaves every property optional, so an object holds no key that it must.
  auto j = nlohmann::ordered_json::parse(R"({"cellLocalId": 7})");

  cfg_structure_c cfg;
  ASSERT_TRUE(openapi::from_json(j, &cfg, cfg_structure_table));
  ASSERT_EQ(cfg.type, cfg_structure_c::cell_cu);
  ASSERT_EQ(cfg.cu.cell_local_id, 7);
}

TEST(asn1_openapi_codec_test, an_object_with_a_key_no_alternative_accepts_fails_to_read)
{
  auto j = nlohmann::ordered_json::parse(R"({"gnbDuId": 1})");

  cfg_structure_c cfg;
  ASSERT_FALSE(openapi::from_json(j, &cfg, cfg_structure_table));
}

/*********************************************************
  Fields that the schema leaves unbounded
*********************************************************/

namespace {

/// Field whose bounds the generator fills with the whole range of its type, as it does for a schema that gives none.
struct unbounded_s {
  bool     style_type_present = false;
  int64_t  style_type         = 0;
  uint64_t count              = 0;
};

const table::seq_field unbounded_fields[] = {
    {offsetof(unbounded_s, style_type),
     (int32_t)offsetof(unbounded_s, style_type_present),
     table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U}},
    {offsetof(unbounded_s, count), table::seq_field::mandatory, table::field_op::int_u64, false, true, 0, 4294967295}};
const table::field_name unbounded_names[] = {{"ricStyleType", nullptr}, {"count", nullptr}};
const table::seq_desc   unbounded_shape   = {{unbounded_fields, UINT64_C(0x2), nullptr, unbounded_names, 2, 0},
                                             false,
                                             true};

const openapi::field_desc unbounded_openapi_fields[] = {{}, {}};
const openapi::seq_desc   unbounded_table            = {&unbounded_shape, unbounded_openapi_fields};

} // namespace

TEST(asn1_openapi_codec_test, an_unbounded_signed_field_reads_a_non_negative_value)
{
  auto j = nlohmann::ordered_json::parse(R"({"ricStyleType": 2, "count": 0})");

  unbounded_s decoded;
  ASSERT_TRUE(openapi::from_json(j, &decoded, unbounded_table));
  ASSERT_TRUE(decoded.style_type_present);
  ASSERT_EQ(decoded.style_type, 2);
}

TEST(asn1_openapi_codec_test, an_unbounded_signed_field_reads_a_negative_value)
{
  auto j = nlohmann::ordered_json::parse(R"({"ricStyleType": -2, "count": 0})");

  unbounded_s decoded;
  ASSERT_TRUE(openapi::from_json(j, &decoded, unbounded_table));
  ASSERT_EQ(decoded.style_type, -2);
}

TEST(asn1_openapi_codec_test, an_unsigned_field_reads_the_top_of_its_range)
{
  auto j = nlohmann::ordered_json::parse(R"({"count": 4294967295})");

  unbounded_s decoded;
  ASSERT_TRUE(openapi::from_json(j, &decoded, unbounded_table));
  ASSERT_EQ(decoded.count, 4294967295U);
}

TEST(asn1_openapi_codec_test, an_unsigned_field_rejects_a_value_above_its_range)
{
  auto j = nlohmann::ordered_json::parse(R"({"count": 4294967296})");

  unbounded_s decoded;
  ASSERT_FALSE(openapi::from_json(j, &decoded, unbounded_table));
}

TEST(asn1_openapi_codec_test, a_cell_identity_round_trips_as_padded_uppercase_hexadecimal)
{
  // NRCellIdentity carries the pattern ^[A-Fa-f0-9]{9}$, as per O-RAN.WG3.TS.E2SM-CCC clause 9.4.2.
  using codec = openapi::hex_number_value<fixed_bitstring<36, false, true>, 9>;

  fixed_bitstring<36, false, true> cell_id;
  cell_id.from_number(6733824);

  nlohmann::ordered_json j;
  codec::write(j, &cell_id);
  ASSERT_EQ(j.get<std::string>(), "00066C000");

  openapi::read_context            ctx;
  fixed_bitstring<36, false, true> decoded;
  ASSERT_TRUE(codec::read(j, &decoded, ctx));
  ASSERT_EQ(decoded.to_number(), 6733824);

  ASSERT_TRUE(codec::read(nlohmann::ordered_json("00066c000"), &decoded, ctx));
  ASSERT_EQ(decoded.to_number(), 6733824);
  ASSERT_FALSE(codec::read(nlohmann::ordered_json("00066C00"), &decoded, ctx));
  ASSERT_FALSE(codec::read(nlohmann::ordered_json("00066G000"), &decoded, ctx));
}

/*********************************************************
  Strictness and the path of a failure
*********************************************************/

TEST(asn1_openapi_codec_test, a_key_the_sequence_does_not_have_is_accepted_by_default)
{
  auto j = nlohmann::ordered_json::parse(R"({"fiveQI": 9, "sixQI": 10})");

  meas_label_s decoded;
  ASSERT_TRUE(openapi::from_json(j, &decoded, meas_label_table));
  ASSERT_EQ(decoded.five_qi, 9);
}

TEST(asn1_openapi_codec_test, a_key_the_sequence_does_not_have_fails_a_strict_read)
{
  auto j = nlohmann::ordered_json::parse(R"({"fiveQI": 9, "sixQI": 10})");

  openapi::read_context ctx;
  ctx.reject_unknown_keys = true;

  meas_label_s decoded;
  ASSERT_FALSE(openapi::from_json(j, &decoded, meas_label_table, ctx));
}

TEST(asn1_openapi_codec_test, the_path_of_a_failure_names_the_nested_field)
{
  // sD is a string in the schema, so a number fails, and the path names it under the nested SEQUENCE.
  auto j = nlohmann::ordered_json::parse(R"({"sliceID": {"sST": "01", "sD": 7}})");

  openapi::read_context ctx;
  meas_label_s          decoded;
  ASSERT_FALSE(openapi::from_json(j, &decoded, meas_label_table, ctx));
  ASSERT_EQ(ctx.path, "sliceID.sD");
}

namespace {

/// OpenAPI table of LabelInfoItem, whose one field is the MeasurementLabel.
const openapi::field_desc label_info_item_openapi_fields[] = {{nullptr, &meas_label_table}};
const openapi::seq_desc   label_info_item_table            = {&label_info_item_s::desc, label_info_item_openapi_fields};

/// SEQUENCE that holds a list of LabelInfoItem, to reach a failure under a list.
struct label_list_s {
  label_info_list_l labels;
};

const table::seq_field label_list_fields[] = {
    {offsetof(label_list_s, labels),
     table::seq_field::list_presence,
     table::field_op::codec,
     false,
     true,
     1,
     2147483647,
     &table::item_list_codec<label_info_list_l, table::sequence_item_ops, false, true>::value,
     &label_info_item_s::desc}};
const table::field_name label_list_names[] = {{"labelInfoList", nullptr}};
const table::seq_desc   label_list_shape   = {{label_list_fields, UINT64_C(0x0), nullptr, label_list_names, 1, 0},
                                              false,
                                              true};

const openapi::field_desc label_list_openapi_fields[] = {
    {&openapi::seq_list_value<label_info_list_l, &label_info_item_table>::codec}};
const openapi::seq_desc label_list_table = {&label_list_shape, label_list_openapi_fields};

} // namespace

TEST(asn1_openapi_codec_test, the_path_of_a_failure_names_the_item_of_a_list)
{
  auto j = nlohmann::ordered_json::parse(
      R"({"labelInfoList": [{"measLabel": {"plmnID": "00f110"}}, {"measLabel": {"plmnID": 7}}]})");

  openapi::read_context ctx;
  label_list_s          decoded;
  ASSERT_FALSE(openapi::from_json(j, &decoded, label_list_table, ctx));
  ASSERT_EQ(ctx.path, "labelInfoList[1].measLabel.plmnID");
}

TEST(asn1_openapi_codec_test, a_list_of_sequences_round_trips)
{
  auto j = nlohmann::ordered_json::parse(
      R"({"labelInfoList":[{"measLabel":{"plmnID":"00f110"}},{"measLabel":{"fiveQI":9}}]})");

  label_list_s decoded;
  ASSERT_TRUE(openapi::from_json(j, &decoded, label_list_table));
  ASSERT_EQ(decoded.labels.size(), 2);
  ASSERT_TRUE(decoded.labels[0].meas_label.plmn_id_present);
  ASSERT_EQ(decoded.labels[1].meas_label.five_qi, 9);

  nlohmann::ordered_json round_tripped;
  openapi::to_json(round_tripped, &decoded, label_list_table);
  ASSERT_EQ(round_tripped.dump(), j.dump());
}

TEST(asn1_openapi_codec_test, a_list_the_json_does_not_carry_is_emptied_in_a_destination_that_held_one)
{
  label_list_s decoded;
  ASSERT_TRUE(
      openapi::from_json(nlohmann::ordered_json::parse(R"({"labelInfoList":[{"measLabel":{"plmnID":"00f110"}}]})"),
                         &decoded,
                         label_list_table));
  ASSERT_EQ(decoded.labels.size(), 1);

  ASSERT_TRUE(openapi::from_json(nlohmann::ordered_json::parse("{}"), &decoded, label_list_table));
  ASSERT_EQ(decoded.labels.size(), 0);
}

namespace {

/// SEQUENCE whose list the schema bounds to SIZE (1..2), to reach the bounds of a list field.
struct short_list_s {
  label_info_list_l labels;
};

const table::seq_field short_list_fields[] = {
    {offsetof(short_list_s, labels),
     table::seq_field::list_presence,
     table::field_op::codec,
     false,
     true,
     1,
     1,
     &table::item_list_codec<label_info_list_l, table::sequence_item_ops, false, true>::value,
     &label_info_item_s::desc}};
const table::field_name short_list_names[] = {{"labelInfoList", nullptr}};
const table::seq_desc   short_list_shape   = {{short_list_fields, UINT64_C(0x0), nullptr, short_list_names, 1, 0},
                                              false,
                                              true};

const openapi::field_desc short_list_openapi_fields[] = {
    {&openapi::seq_list_value<label_info_list_l, &label_info_item_table>::codec}};
const openapi::seq_desc short_list_table = {&short_list_shape, short_list_openapi_fields};

/// A table that holds fewer entries than the shape it describes, which the codec must not walk.
const openapi::field_desc short_of_its_shape_fields[] = {{}};
const openapi::seq_desc   short_of_its_shape_table    = {&meas_label_s::desc, short_of_its_shape_fields};

} // namespace

TEST(asn1_openapi_codec_test, an_array_of_more_items_than_its_field_holds_fails_to_read)
{
  auto j = nlohmann::ordered_json::parse(
      R"({"labelInfoList": [{"measLabel": {"fiveQI": 1}}, {"measLabel": {"fiveQI": 2}}, {"measLabel": {"fiveQI": 3}}]})");

  openapi::read_context ctx;
  short_list_s          decoded;
  ASSERT_FALSE(openapi::from_json(j, &decoded, short_list_table, ctx));
  // The count is checked before the destination is sized from it, so nothing was taken for the items.
  ASSERT_EQ(decoded.labels.size(), 0);
}

TEST(asn1_openapi_codec_test, an_empty_array_fails_a_field_whose_size_starts_at_one)
{
  auto j = nlohmann::ordered_json::parse(R"({"labelInfoList": []})");

  openapi::read_context ctx;
  short_list_s          decoded;
  ASSERT_FALSE(openapi::from_json(j, &decoded, short_list_table, ctx));
}

TEST(asn1_openapi_codec_test, an_array_within_the_bounds_of_its_field_reads)
{
  auto j =
      nlohmann::ordered_json::parse(R"({"labelInfoList":[{"measLabel":{"fiveQI":1}},{"measLabel":{"fiveQI":2}}]})");

  short_list_s decoded;
  ASSERT_TRUE(openapi::from_json(j, &decoded, short_list_table));
  ASSERT_EQ(decoded.labels.size(), 2);
}

TEST(asn1_openapi_codec_test, an_array_of_more_items_than_its_destination_holds_fails_to_read)
{
  // A list of a fixed capacity holds what it has room for, so a longer array must be turned away rather than
  // written past the end of it.
  using codec = openapi::int_list_value<bounded_array<uint16_t, 4>, uint16_t>;

  openapi::read_context      ctx;
  bounded_array<uint16_t, 4> decoded;
  ASSERT_TRUE(codec::read(nlohmann::ordered_json::parse("[1, 2, 3, 4]"), &decoded, ctx));
  ASSERT_EQ(decoded.size(), 4);
  ASSERT_FALSE(codec::read(nlohmann::ordered_json::parse("[1, 2, 3, 4, 5]"), &decoded, ctx));
}

TEST(asn1_openapi_codec_test, an_item_outside_the_bounds_its_schema_gives_fails_to_read)
{
  // The item fits the type that stores it and falls outside what the schema allows, which only the bounds of the
  // codec carry.
  using codec = openapi::int_list_value<bounded_array<uint16_t, 8>, uint16_t, 0, 255>;

  openapi::read_context      ctx;
  bounded_array<uint16_t, 8> decoded;
  ASSERT_TRUE(codec::read(nlohmann::ordered_json::parse("[255]"), &decoded, ctx));
  ASSERT_FALSE(codec::read(nlohmann::ordered_json::parse("[256]"), &decoded, ctx));
  ASSERT_FALSE(codec::read(nlohmann::ordered_json::parse("[-1]"), &decoded, ctx));
}

TEST(asn1_openapi_codec_test, a_table_shorter_than_its_shape_fails_rather_than_reading_past_it)
{
  meas_label_s           value;
  nlohmann::ordered_json out;
  ASSERT_FALSE(openapi::to_json(out, &value, short_of_its_shape_table));
  ASSERT_FALSE(openapi::from_json(nlohmann::ordered_json::object(), &value, short_of_its_shape_table));
}

TEST(asn1_openapi_codec_test, a_string_outside_the_bounds_of_its_schema_fails_to_read)
{
  using codec = openapi::bounded_string_value<printable_string<1, 150, true, true>, 1, 4>;

  openapi::read_context                ctx;
  printable_string<1, 150, true, true> decoded;
  ASSERT_TRUE(codec::read(nlohmann::ordered_json("abcd"), &decoded, ctx));
  ASSERT_EQ(decoded.to_string(), "abcd");
  ASSERT_FALSE(codec::read(nlohmann::ordered_json("abcde"), &decoded, ctx));
  ASSERT_FALSE(codec::read(nlohmann::ordered_json(""), &decoded, ctx));
}

TEST(asn1_openapi_codec_test, a_character_string_longer_than_its_type_holds_fails_to_read)
{
  // Every character string of a generated table reads through this codec, and the type stores whatever it is given
  // without shortening it, so the bounds the type declares are what the read checks.
  using codec = openapi::string_value<printable_string<1, 4, true, true>>;

  openapi::read_context              ctx;
  printable_string<1, 4, true, true> decoded;
  ASSERT_TRUE(codec::read(nlohmann::ordered_json("abcd"), &decoded, ctx));
  ASSERT_EQ(decoded.to_string(), "abcd");
  ASSERT_FALSE(codec::read(nlohmann::ordered_json("abcde"), &decoded, ctx));
  ASSERT_FALSE(codec::read(nlohmann::ordered_json(""), &decoded, ctx));
}

TEST(asn1_openapi_codec_test, a_character_string_outside_the_alphabet_of_its_type_fails_to_read)
{
  using codec = openapi::string_value<printable_string<1, 8, true, true>>;

  openapi::read_context              ctx;
  printable_string<1, 8, true, true> decoded;
  ASSERT_TRUE(codec::read(nlohmann::ordered_json("abc"), &decoded, ctx));
  // A PrintableString carries the characters 32 to 122, so a tilde falls above its alphabet and a tab below it.
  ASSERT_FALSE(codec::read(nlohmann::ordered_json("ab~"), &decoded, ctx));
  ASSERT_FALSE(codec::read(nlohmann::ordered_json("ab\t"), &decoded, ctx));
}

TEST(asn1_openapi_codec_test, a_real_outside_the_range_of_its_type_fails_to_read)
{
  openapi::read_context ctx;
  real_s                decoded;
  ASSERT_TRUE(openapi::real_value::read(nlohmann::ordered_json(1.5), &decoded, ctx));
  ASSERT_FLOAT_EQ(decoded.value, 1.5F);
  // The JSON carries a number of twice the range of the REAL that holds it, and narrowing this one is undefined.
  ASSERT_FALSE(openapi::real_value::read(nlohmann::ordered_json(1e308), &decoded, ctx));
}

TEST(asn1_openapi_codec_test, a_hexadecimal_value_that_its_type_cannot_hold_fails_to_read)
{
  using codec = openapi::hex_number_value<fixed_bitstring<8, false, true>, 2>;
  using wide  = openapi::hex_number_value<fixed_bitstring<8, false, true>, 4>;

  openapi::read_context           ctx;
  fixed_bitstring<8, false, true> decoded;
  ASSERT_TRUE(codec::read(nlohmann::ordered_json("FF"), &decoded, ctx));
  ASSERT_EQ(decoded.to_number(), 255);
  // from_number() leaves the value as it was when it needs more bits than the type holds, so the read compares it.
  ASSERT_FALSE(wide::read(nlohmann::ordered_json("FFFF"), &decoded, ctx));
}

/*********************************************************
  Alternatives of a oneOf that are values rather than objects
*********************************************************/

namespace {

/// CHOICE whose alternatives are values, as a oneOf of scalars in a schema is. The kind of the JSON selects one,
/// since an alternative of a oneOf carries no name and a value holds no key to match.
struct scalar_choice_c {
  enum types { count, flag, state };

  uint8_t type = count;
  union {
    uint32_t count_value;
    bool     flag_value;
    uint8_t  state_value;
  };

  scalar_choice_c() : count_value(0) {}
};

void set_scalar_choice(void* choice, uint32_t type)
{
  auto* c = static_cast<scalar_choice_c*>(choice);
  c->type = static_cast<uint8_t>(type);
  if (type == scalar_choice_c::count) {
    new (&c->count_value) uint32_t{};
  } else if (type == scalar_choice_c::flag) {
    new (&c->flag_value) bool{};
  } else {
    new (&c->state_value) uint8_t{};
  }
}

const char*            scalar_state_names[] = {"ACTIVE", "INACTIVE"};
const table::enum_desc scalar_state_shape   = {scalar_state_names, 2, "state_e"};

const table::seq_field scalar_choice_alternatives[] = {
    {0, table::seq_field::mandatory, table::field_op::int_u32, false, true, 0, 16383},
    {0, table::seq_field::mandatory, table::field_op::boolean, false, true, 0, 0},
    {0, table::seq_field::mandatory, table::field_op::enum_u8, false, true, 2, 0, nullptr, &scalar_state_shape}};
const table::field_name  scalar_choice_names[] = {{"count", nullptr}, {"flag", nullptr}, {"state", nullptr}};
const table::choice_desc scalar_choice_shape   = {table::int_type::u8,
                                                  3,
                                                  0,
                                                  false,
                                                  true,
                                                  scalar_choice_alternatives,
                                                  &set_scalar_choice,
                                                  scalar_choice_names,
                                                  "scalar_choice_c",
                                                  offsetof(scalar_choice_c, type),
                                                  offsetof(scalar_choice_c, count_value)};

/// No alternative carries a key, since none of them is an object.
const openapi::alternative_desc scalar_choice_openapi_alternatives[] = {{}, {}, {}};
const openapi::choice_desc      scalar_choice_table = {&scalar_choice_shape, scalar_choice_openapi_alternatives};

} // namespace

TEST(asn1_openapi_codec_test, a_number_selects_the_integer_alternative_of_a_choice_of_values)
{
  scalar_choice_c decoded;
  // The destination starts on the other alternative, so selecting this one is what the read proves.
  decoded.type = scalar_choice_c::state;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(nlohmann::ordered_json::parse("7"), &decoded, scalar_choice_table, ctx));
  ASSERT_EQ(decoded.type, scalar_choice_c::count);
  ASSERT_EQ(decoded.count_value, 7);
}

TEST(asn1_openapi_codec_test, a_boolean_selects_the_boolean_alternative_of_a_choice_of_values)
{
  scalar_choice_c       decoded;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(nlohmann::ordered_json::parse("true"), &decoded, scalar_choice_table, ctx));
  ASSERT_EQ(decoded.type, scalar_choice_c::flag);
  ASSERT_TRUE(decoded.flag_value);
}

TEST(asn1_openapi_codec_test, a_string_selects_the_enumerated_alternative_of_a_choice_of_values)
{
  scalar_choice_c       decoded;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(nlohmann::ordered_json::parse(R"("INACTIVE")"), &decoded, scalar_choice_table, ctx));
  ASSERT_EQ(decoded.type, scalar_choice_c::state);
  ASSERT_EQ(decoded.state_value, 1);
}

TEST(asn1_openapi_codec_test, a_value_of_a_kind_no_alternative_takes_fails_to_read)
{
  scalar_choice_c       decoded;
  openapi::read_context ctx;
  // Null is the one kind that no alternative of this CHOICE carries, since a number, a boolean and a string each
  // select one of them, so it is what reaches the failure of a value that selects nothing.
  ASSERT_FALSE(openapi::from_json(nlohmann::ordered_json::parse("null"), &decoded, scalar_choice_table, ctx));
}

TEST(asn1_openapi_codec_test, a_number_that_is_not_an_integer_fails_the_alternative_that_it_selects)
{
  scalar_choice_c       decoded;
  openapi::read_context ctx;
  ASSERT_FALSE(openapi::from_json(nlohmann::ordered_json::parse("1.5"), &decoded, scalar_choice_table, ctx));
}

/*********************************************************
  An alternative that wants a member the object lacks
*********************************************************/

namespace {

/// SEQUENCE whose second member the schema lists under required.
struct strict_s {
  uint32_t a         = 0;
  bool     a_present = false;
  uint32_t b         = 0;
};

/// SEQUENCE whose members the schema all leaves optional, as it does for almost every type.
struct loose_s {
  uint32_t a         = 0;
  uint32_t b         = 0;
  uint32_t c         = 0;
  bool     a_present = false;
  bool     b_present = false;
  bool     c_present = false;
};

struct required_choice_c {
  enum types { strict, loose };

  uint8_t type = strict;
  union {
    strict_s strict_value;
    loose_s  loose_value;
  };

  required_choice_c() : loose_value() {}
};

void set_required_choice(void* choice, uint32_t type)
{
  auto* c = static_cast<required_choice_c*>(choice);
  c->type = static_cast<uint8_t>(type);
  if (type == required_choice_c::strict) {
    new (&c->strict_value) strict_s{};
  } else {
    new (&c->loose_value) loose_s{};
  }
}

const table::seq_field strict_fields[] = {
    {offsetof(strict_s, a), (int32_t)offsetof(strict_s, a_present), table::field_op::int_u32, false, false, 0, 16383},
    {offsetof(strict_s, b), table::seq_field::mandatory, table::field_op::int_u32, false, false, 0, 16383}};
const table::field_name strict_names[] = {{"a", nullptr}, {"b", nullptr}};
const table::seq_desc   strict_shape   = {{strict_fields, UINT64_C(0x2), nullptr, strict_names, 2, 0}, false, true};

const table::seq_field loose_fields[] = {
    {offsetof(loose_s, a), (int32_t)offsetof(loose_s, a_present), table::field_op::int_u32, false, false, 0, 16383},
    {offsetof(loose_s, b), (int32_t)offsetof(loose_s, b_present), table::field_op::int_u32, false, false, 0, 16383},
    {offsetof(loose_s, c), (int32_t)offsetof(loose_s, c_present), table::field_op::int_u32, false, false, 0, 16383}};
const table::field_name loose_names[] = {{"a", nullptr}, {"b", nullptr}, {"c", nullptr}};
const table::seq_desc   loose_shape   = {{loose_fields, UINT64_C(0x0), nullptr, loose_names, 3, 0}, false, true};

const table::seq_field required_choice_alternatives[] = {
    {0, table::seq_field::mandatory, table::field_op::sequence, false, true, 0, 0, nullptr, &strict_shape},
    {0, table::seq_field::mandatory, table::field_op::sequence, false, true, 0, 0, nullptr, &loose_shape}};
const table::field_name  required_choice_names[] = {{"Strict", nullptr}, {"Loose", nullptr}};
const table::choice_desc required_choice_shape   = {table::int_type::u8,
                                                    2,
                                                    0,
                                                    false,
                                                    true,
                                                    required_choice_alternatives,
                                                    &set_required_choice,
                                                    required_choice_names,
                                                    "required_choice_c",
                                                    offsetof(required_choice_c, type),
                                                    offsetof(required_choice_c, strict_value)};

const openapi::field_desc strict_openapi_fields[] = {{}, {}};
const openapi::seq_desc   strict_table            = {&strict_shape, strict_openapi_fields};
const openapi::field_desc loose_openapi_fields[]  = {{}, {}, {}};
const openapi::seq_desc   loose_table             = {&loose_shape, loose_openapi_fields};

const char* const strict_keys[] = {"a", "b"};
const char* const loose_keys[]  = {"a", "b", "c"};

const openapi::alternative_desc required_choice_openapi_alternatives[] = {{nullptr, &strict_table, strict_keys, 2},
                                                                          {nullptr, &loose_table, loose_keys, 3}};
const openapi::choice_desc      required_choice_table = {&required_choice_shape, required_choice_openapi_alternatives};

} // namespace

TEST(asn1_openapi_codec_test, an_alternative_that_wants_a_member_the_object_lacks_is_passed_over)
{
  // Strict accepts the fewest keys, so it fits most closely, but it wants b and the object does not hold it.
  auto j = nlohmann::ordered_json::parse(R"({"a": 7})");

  required_choice_c     decoded;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(j, &decoded, required_choice_table, ctx));
  ASSERT_EQ(decoded.type, required_choice_c::loose);
  ASSERT_TRUE(decoded.loose_value.a_present);
  ASSERT_EQ(decoded.loose_value.a, 7);
}

TEST(asn1_openapi_codec_test, an_alternative_that_holds_every_member_it_wants_is_taken)
{
  auto j = nlohmann::ordered_json::parse(R"({"a": 7, "b": 1})");

  required_choice_c decoded;
  // The destination starts on the other alternative, so selecting this one is what the read proves.
  decoded.type = required_choice_c::loose;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(j, &decoded, required_choice_table, ctx));
  ASSERT_EQ(decoded.type, required_choice_c::strict);
  ASSERT_EQ(decoded.strict_value.a, 7);
  ASSERT_EQ(decoded.strict_value.b, 1);
}

/*********************************************************
  Strictness of a string, and the path a failed CHOICE leaves
*********************************************************/

TEST(asn1_openapi_codec_test, a_string_of_another_length_than_its_type_holds_fails_to_read)
{
  // from_string() of a fixed size type leaves the value as it was rather than saying that it took nothing, so the
  // read compares what it stored with what it was given.
  using codec = openapi::string_value<fixed_octstring<3, true>>;

  openapi::read_context    ctx;
  fixed_octstring<3, true> decoded;
  ASSERT_TRUE(codec::read(nlohmann::ordered_json("00f110"), &decoded, ctx));
  ASSERT_EQ(decoded.to_string(), "00f110");
  ASSERT_FALSE(codec::read(nlohmann::ordered_json("00f1"), &decoded, ctx));
  ASSERT_FALSE(codec::read(nlohmann::ordered_json("00f11000"), &decoded, ctx));
}

TEST(asn1_openapi_codec_test, a_string_of_characters_its_type_cannot_hold_fails_to_read)
{
  // An OCTET STRING takes a character that is not hexadecimal as a zero, and a BIT STRING takes one that is not a
  // bit as a zero, so a string of the right length still holds no value of the type.
  using octets = openapi::string_value<fixed_octstring<3, true>>;
  using bits   = openapi::string_value<fixed_bitstring<28, false, true>>;

  openapi::read_context    ctx;
  fixed_octstring<3, true> octet_value;
  ASSERT_FALSE(octets::read(nlohmann::ordered_json("zzzzzz"), &octet_value, ctx));
  ASSERT_FALSE(octets::read(nlohmann::ordered_json("00f1g0"), &octet_value, ctx));
  // The codec writes hexadecimal in lower case, and an upper case string holds the same value.
  ASSERT_TRUE(octets::read(nlohmann::ordered_json("00F110"), &octet_value, ctx));
  ASSERT_EQ(octet_value.to_string(), "00f110");

  fixed_bitstring<28, false, true> bit_value;
  ASSERT_FALSE(bits::read(nlohmann::ordered_json("2222222222222222222222222222"), &bit_value, ctx));
  ASSERT_TRUE(bits::read(nlohmann::ordered_json("0000000001100110110000000000"), &bit_value, ctx));
}

TEST(asn1_openapi_codec_test, a_value_the_json_holds_as_signed_reads_into_an_unsigned_destination)
{
  // A caller that builds the JSON holds 1 as a signed value, where parsing "1" holds it as an unsigned one. Both
  // are the same value, and both belong in an unsigned field.
  using codec = openapi::int_list_value<bounded_array<uint16_t, 8>, uint16_t>;

  openapi::read_context      ctx;
  bounded_array<uint16_t, 8> list;
  ASSERT_TRUE(codec::read(nlohmann::ordered_json::array({int64_t{1}, int64_t{65535}}), &list, ctx));
  ASSERT_EQ(list[0], 1);
  ASSERT_EQ(list[1], 65535);
  ASSERT_FALSE(codec::read(nlohmann::ordered_json::array({int64_t{65536}}), &list, ctx));
  ASSERT_FALSE(codec::read(nlohmann::ordered_json::array({int64_t{-1}}), &list, ctx));

  unbounded_s decoded;
  ASSERT_TRUE(openapi::from_json(nlohmann::ordered_json{{"count", int64_t{7}}}, &decoded, unbounded_table, ctx));
  ASSERT_EQ(decoded.count, 7);
  ASSERT_FALSE(openapi::from_json(nlohmann::ordered_json{{"count", int64_t{-1}}}, &decoded, unbounded_table, ctx));
}

TEST(asn1_openapi_codec_test, an_item_of_a_list_outside_the_bounds_of_its_type_fails_to_read)
{
  // The JSON converts without checking, so 65536 would become 0 in a list of uint16_t.
  using codec = openapi::int_list_value<bounded_array<uint16_t, 8>, uint16_t>;

  openapi::read_context      ctx;
  bounded_array<uint16_t, 8> decoded;
  ASSERT_TRUE(codec::read(nlohmann::ordered_json::parse("[0, 65535]"), &decoded, ctx));
  ASSERT_EQ(decoded[1], 65535);
  ASSERT_FALSE(codec::read(nlohmann::ordered_json::parse("[65536]"), &decoded, ctx));
  ASSERT_FALSE(codec::read(nlohmann::ordered_json::parse("[-1]"), &decoded, ctx));
}

TEST(asn1_openapi_codec_test, an_unsigned_value_above_the_signed_range_fails_to_read)
{
  // ricStyleType carries the whole range of its type, so a wrapped value would pass its bounds.
  auto j = nlohmann::ordered_json::parse(R"({"ricStyleType": 18446744073709551615, "count": 0})");

  unbounded_s           decoded;
  openapi::read_context ctx;
  ASSERT_FALSE(openapi::from_json(j, &decoded, unbounded_table, ctx));
}

TEST(asn1_openapi_codec_test, the_path_of_a_failed_choice_names_where_its_alternatives_stopped)
{
  // Every alternative takes the key and stops on its value, which is where the read of the CHOICE stopped.
  auto j = nlohmann::ordered_json::parse(R"({"cellLocalId": "seven"})");

  openapi::read_context ctx;
  cfg_structure_c       cfg;
  ASSERT_FALSE(openapi::from_json(j, &cfg, cfg_structure_table, ctx));
  ASSERT_EQ(ctx.path, "cellLocalId");
}

TEST(asn1_openapi_codec_test, the_path_of_a_choice_no_alternative_accepts_names_the_choice)
{
  // No alternative takes the key, so no read runs and the path still names where the CHOICE sits.
  auto j = nlohmann::ordered_json::parse(R"({"gnbDuId": 1})");

  openapi::read_context ctx;
  ctx.path = "configurationStructure";
  cfg_structure_c cfg;
  ASSERT_FALSE(openapi::from_json(j, &cfg, cfg_structure_table, ctx));
  ASSERT_EQ(ctx.path, "configurationStructure");
}

/*********************************************************
  Two alternatives that accept as many keys
*********************************************************/

namespace {

/// SEQUENCE that accepts the keys of strict_s and wants neither of them.
struct pair_s {
  uint32_t a         = 0;
  uint32_t b         = 0;
  bool     a_present = false;
  bool     b_present = false;
};

struct even_choice_c {
  enum types { strict, pair };

  uint8_t type = strict;
  union {
    strict_s strict_value;
    pair_s   pair_value;
  };

  even_choice_c() : pair_value() {}
};

void set_even_choice(void* choice, uint32_t type)
{
  auto* c = static_cast<even_choice_c*>(choice);
  c->type = static_cast<uint8_t>(type);
  if (type == even_choice_c::strict) {
    new (&c->strict_value) strict_s{};
  } else {
    new (&c->pair_value) pair_s{};
  }
}

const table::seq_field pair_fields[] = {
    {offsetof(pair_s, a), (int32_t)offsetof(pair_s, a_present), table::field_op::int_u32, false, false, 0, 16383},
    {offsetof(pair_s, b), (int32_t)offsetof(pair_s, b_present), table::field_op::int_u32, false, false, 0, 16383}};
const table::field_name pair_names[] = {{"a", nullptr}, {"b", nullptr}};
const table::seq_desc   pair_shape   = {{pair_fields, UINT64_C(0x0), nullptr, pair_names, 2, 0}, false, true};

const table::seq_field even_choice_alternatives[] = {
    {0, table::seq_field::mandatory, table::field_op::sequence, false, true, 0, 0, nullptr, &strict_shape},
    {0, table::seq_field::mandatory, table::field_op::sequence, false, true, 0, 0, nullptr, &pair_shape}};
const table::field_name  even_choice_names[] = {{"Strict", nullptr}, {"Pair", nullptr}};
const table::choice_desc even_choice_shape   = {table::int_type::u8,
                                                2,
                                                0,
                                                false,
                                                true,
                                                even_choice_alternatives,
                                                &set_even_choice,
                                                even_choice_names,
                                                "even_choice_c",
                                                offsetof(even_choice_c, type),
                                                offsetof(even_choice_c, strict_value)};

const openapi::field_desc pair_openapi_fields[] = {{}, {}};
const openapi::seq_desc   pair_table            = {&pair_shape, pair_openapi_fields};

const char* const pair_keys[] = {"a", "b"};

const openapi::alternative_desc even_choice_openapi_alternatives[] = {{nullptr, &strict_table, strict_keys, 2},
                                                                      {nullptr, &pair_table, pair_keys, 2}};
const openapi::choice_desc      even_choice_table = {&even_choice_shape, even_choice_openapi_alternatives};

} // namespace

TEST(asn1_openapi_codec_test, an_alternative_that_accepts_as_many_keys_as_one_that_failed_is_still_tried)
{
  // Both alternatives accept a and b, so neither fits more closely than the other, and the first wants b.
  auto j = nlohmann::ordered_json::parse(R"({"a": 7})");

  even_choice_c         decoded;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(j, &decoded, even_choice_table, ctx));
  ASSERT_EQ(decoded.type, even_choice_c::pair);
  ASSERT_TRUE(decoded.pair_value.a_present);
  ASSERT_EQ(decoded.pair_value.a, 7);
  ASSERT_FALSE(decoded.pair_value.b_present);
}

TEST(asn1_openapi_codec_test, the_first_of_two_alternatives_that_accept_as_many_keys_is_taken_when_it_reads)
{
  auto j = nlohmann::ordered_json::parse(R"({"a": 7, "b": 1})");

  even_choice_c decoded;
  // The destination starts on the other alternative, so selecting this one is what the read proves.
  decoded.type = even_choice_c::pair;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(j, &decoded, even_choice_table, ctx));
  ASSERT_EQ(decoded.type, even_choice_c::strict);
  ASSERT_EQ(decoded.strict_value.b, 1);
}

/*********************************************************
  A member of the object that names the alternative
*********************************************************/

namespace {

/// SEQUENCE that names the alternative of the CHOICE it holds, as ConfigurationStructure does for the RAN
/// configuration structure of its valuesOfAttributes.
struct named_holder_s {
  printable_string<1, 150, true, true> structure_name;
  cfg_structure_c                      structure;
};

const table::seq_field  named_holder_fields[] = {{offsetof(named_holder_s, structure_name),
                                                  table::seq_field::mandatory,
                                                  table::field_op::codec,
                                                  false,
                                                  true,
                                                  1,
                                                  150},
                                                 {offsetof(named_holder_s, structure),
                                                  table::seq_field::mandatory,
                                                  table::field_op::choice,
                                                  false,
                                                  true,
                                                  0,
                                                  0,
                                                  nullptr,
                                                  &cfg_structure_shape}};
const table::field_name named_holder_names[]  = {{"ranConfigurationStructureName", nullptr},
                                                 {"ranConfigurationStructure", nullptr}};
const table::seq_desc   named_holder_shape = {{named_holder_fields, UINT64_C(0x3), nullptr, named_holder_names, 2, 0},
                                              false,
                                              true};

const openapi::field_desc named_holder_openapi_fields[] = {
    {&openapi::string_value<printable_string<1, 150, true, true>>::codec, nullptr, true},
    {nullptr, &cfg_structure_table}};
const openapi::seq_desc named_holder_table = {&named_holder_shape, named_holder_openapi_fields};

} // namespace

TEST(asn1_openapi_codec_test, the_member_that_names_the_alternative_selects_it_over_the_keys)
{
  // The keys of O-NrCellCu are every key of O-NrCellDu, so the keys alone would take the CU cell.
  auto j = nlohmann::ordered_json::parse(
      R"({"ranConfigurationStructureName":"O-NrCellDu","ranConfigurationStructure":{"cellLocalId":7}})");

  named_holder_s        decoded;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(j, &decoded, named_holder_table, ctx));
  ASSERT_EQ(decoded.structure.type, cfg_structure_c::cell_du);
  ASSERT_EQ(decoded.structure.du.cell_local_id, 7);
}

TEST(asn1_openapi_codec_test, the_keys_select_the_alternative_that_the_name_of_the_member_gives)
{
  auto j = nlohmann::ordered_json::parse(
      R"({"ranConfigurationStructureName":"O-NrCellCu","ranConfigurationStructure":{"cellLocalId":7}})");

  named_holder_s        decoded;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(j, &decoded, named_holder_table, ctx));
  ASSERT_EQ(decoded.structure.type, cfg_structure_c::cell_cu);
}

TEST(asn1_openapi_codec_test, an_object_that_holds_a_key_the_alternative_it_names_does_not_have_fails_to_read)
{
  // The object says it holds an O-NrCellCu and carries a key only O-NrCellDu has, so it contradicts its own name
  // and no other alternative stands in for the one it named.
  auto j = nlohmann::ordered_json::parse(
      R"({"ranConfigurationStructureName":"O-NrCellCu","ranConfigurationStructure":{"cellLocalId":7,"bwpList":3}})");

  named_holder_s        decoded;
  openapi::read_context ctx;
  ASSERT_FALSE(openapi::from_json(j, &decoded, named_holder_table, ctx));
}

TEST(asn1_openapi_codec_test, a_name_that_no_alternative_carries_leaves_the_keys_to_select_one)
{
  auto j = nlohmann::ordered_json::parse(
      R"({"ranConfigurationStructureName":"O-GnbDuFunction","ranConfigurationStructure":{"cellLocalId":7}})");

  named_holder_s        decoded;
  openapi::read_context ctx;
  ASSERT_TRUE(openapi::from_json(j, &decoded, named_holder_table, ctx));
  ASSERT_EQ(decoded.structure.type, cfg_structure_c::cell_cu);
}
