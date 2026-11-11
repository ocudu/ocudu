// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/f1ap/f1ap_pdu_items.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::f1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// Activated-Cells-Mapping-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_activ_cells_map_list_item_ies_o[]    = {759};
static const uint8_t  objset_crit_activ_cells_map_list_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_activ_cells_map_list_item_ies_o[] = {
    activ_cells_map_list_item_ies_o::value_c::types::activ_cells_map_list_item};
static const uint8_t                    objset_presence_activ_cells_map_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_activ_cells_map_list_item_ies_o[]  = {
    {objset_crit_activ_cells_map_list_item_ies_o, crit_e::nulltype},
    {objset_value_activ_cells_map_list_item_ies_o, activ_cells_map_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_activ_cells_map_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc activ_cells_map_list_item_ies_o::desc = {objset_id_activ_cells_map_list_item_ies_o,
                                                                        1,
                                                                        "id",
                                                                        objset_columns_activ_cells_map_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_activ_cells_map_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &activ_cells_map_list_item_s::desc}};
static const asn1::table::field_name choice_table_activ_cells_map_list_item_ies_o_value_c_names[] = {
    {"Activated-Cells-Mapping-List-Item", nullptr}};
const asn1::table::choice_desc activ_cells_map_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<activ_cells_map_list_item_ies_o::value_c::types>(),
    activ_cells_map_list_item_ies_o::value_c::types::nof_types,
    activ_cells_map_list_item_ies_o::value_c::types::nof_exts,
    activ_cells_map_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_activ_cells_map_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_activ_cells_map_list_item_ies_o_value_c_names,
    "activ_cells_map_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(activ_cells_map_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_activ_cells_map_list_item_ies_o_value_c_types_values[] = {
    "Activated-Cells-Mapping-List-Item"};
const asn1::table::enum_desc activ_cells_map_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_activ_cells_map_list_item_ies_o_value_c_types_values,
    1,
    "activ_cells_map_list_item_ies_o::value_c::types"};

// Associated-SCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_associated_scell_item_ies_o[]    = {200};
static const uint8_t  objset_crit_associated_scell_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_associated_scell_item_ies_o[] = {
    associated_scell_item_ies_o::value_c::types::associated_scell_item};
static const uint8_t                    objset_presence_associated_scell_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_associated_scell_item_ies_o[]  = {
    {objset_crit_associated_scell_item_ies_o, crit_e::nulltype},
    {objset_value_associated_scell_item_ies_o, associated_scell_item_ies_o::value_c::types::nulltype},
    {objset_presence_associated_scell_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc associated_scell_item_ies_o::desc = {objset_id_associated_scell_item_ies_o,
                                                                    1,
                                                                    "id",
                                                                    objset_columns_associated_scell_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_associated_scell_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &associated_scell_item_s::desc}};
static const asn1::table::field_name choice_table_associated_scell_item_ies_o_value_c_names[] = {
    {"Associated-SCell-Item", nullptr}};
const asn1::table::choice_desc associated_scell_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<associated_scell_item_ies_o::value_c::types>(),
    associated_scell_item_ies_o::value_c::types::nof_types,
    associated_scell_item_ies_o::value_c::types::nof_exts,
    associated_scell_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_associated_scell_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_associated_scell_item_ies_o_value_c_names,
    "associated_scell_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(associated_scell_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_associated_scell_item_ies_o_value_c_types_values[] = {"Associated-SCell-Item"};
const asn1::table::enum_desc associated_scell_item_ies_o::value_c::types_opts::desc        = {
    enum_names_associated_scell_item_ies_o_value_c_types_values,
    1,
    "associated_scell_item_ies_o::value_c::types"};

// BAP-Header-Rewriting-Added-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bap_hdr_rewriting_added_list_item_ies_o[]    = {515};
static const uint8_t  objset_crit_bap_hdr_rewriting_added_list_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bap_hdr_rewriting_added_list_item_ies_o[] = {
    bap_hdr_rewriting_added_list_item_ies_o::value_c::types::bap_hdr_rewriting_added_list_item};
static const uint8_t objset_presence_bap_hdr_rewriting_added_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bap_hdr_rewriting_added_list_item_ies_o[] = {
    {objset_crit_bap_hdr_rewriting_added_list_item_ies_o, crit_e::nulltype},
    {objset_value_bap_hdr_rewriting_added_list_item_ies_o,
     bap_hdr_rewriting_added_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_bap_hdr_rewriting_added_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bap_hdr_rewriting_added_list_item_ies_o::desc = {
    objset_id_bap_hdr_rewriting_added_list_item_ies_o,
    1,
    "id",
    objset_columns_bap_hdr_rewriting_added_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bap_hdr_rewriting_added_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bap_hdr_rewriting_added_list_item_s::desc}};
static const asn1::table::field_name choice_table_bap_hdr_rewriting_added_list_item_ies_o_value_c_names[] = {
    {"BAP-Header-Rewriting-Added-List-Item", nullptr}};
const asn1::table::choice_desc bap_hdr_rewriting_added_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bap_hdr_rewriting_added_list_item_ies_o::value_c::types>(),
    bap_hdr_rewriting_added_list_item_ies_o::value_c::types::nof_types,
    bap_hdr_rewriting_added_list_item_ies_o::value_c::types::nof_exts,
    bap_hdr_rewriting_added_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bap_hdr_rewriting_added_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bap_hdr_rewriting_added_list_item_ies_o_value_c_names,
    "bap_hdr_rewriting_added_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bap_hdr_rewriting_added_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bap_hdr_rewriting_added_list_item_ies_o_value_c_types_values[] = {
    "BAP-Header-Rewriting-Added-List-Item"};
const asn1::table::enum_desc bap_hdr_rewriting_added_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_bap_hdr_rewriting_added_list_item_ies_o_value_c_types_values,
    1,
    "bap_hdr_rewriting_added_list_item_ies_o::value_c::types"};

// BAP-Header-Rewriting-Removed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bap_hdr_rewriting_remd_list_item_ies_o[]    = {642};
static const uint8_t  objset_crit_bap_hdr_rewriting_remd_list_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bap_hdr_rewriting_remd_list_item_ies_o[] = {
    bap_hdr_rewriting_remd_list_item_ies_o::value_c::types::bap_hdr_rewriting_remd_list_item};
static const uint8_t objset_presence_bap_hdr_rewriting_remd_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bap_hdr_rewriting_remd_list_item_ies_o[] = {
    {objset_crit_bap_hdr_rewriting_remd_list_item_ies_o, crit_e::nulltype},
    {objset_value_bap_hdr_rewriting_remd_list_item_ies_o,
     bap_hdr_rewriting_remd_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_bap_hdr_rewriting_remd_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bap_hdr_rewriting_remd_list_item_ies_o::desc =
    {objset_id_bap_hdr_rewriting_remd_list_item_ies_o, 1, "id", objset_columns_bap_hdr_rewriting_remd_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bap_hdr_rewriting_remd_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bap_hdr_rewriting_remd_list_item_s::desc}};
static const asn1::table::field_name choice_table_bap_hdr_rewriting_remd_list_item_ies_o_value_c_names[] = {
    {"BAP-Header-Rewriting-Removed-List-Item", nullptr}};
const asn1::table::choice_desc bap_hdr_rewriting_remd_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bap_hdr_rewriting_remd_list_item_ies_o::value_c::types>(),
    bap_hdr_rewriting_remd_list_item_ies_o::value_c::types::nof_types,
    bap_hdr_rewriting_remd_list_item_ies_o::value_c::types::nof_exts,
    bap_hdr_rewriting_remd_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bap_hdr_rewriting_remd_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bap_hdr_rewriting_remd_list_item_ies_o_value_c_names,
    "bap_hdr_rewriting_remd_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bap_hdr_rewriting_remd_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bap_hdr_rewriting_remd_list_item_ies_o_value_c_types_values[] = {
    "BAP-Header-Rewriting-Removed-List-Item"};
const asn1::table::enum_desc bap_hdr_rewriting_remd_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_bap_hdr_rewriting_remd_list_item_ies_o_value_c_types_values,
    1,
    "bap_hdr_rewriting_remd_list_item_ies_o::value_c::types"};

// BH-Routing-Information-Added-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_routing_info_added_list_item_ies_o[]    = {284};
static const uint8_t  objset_crit_bh_routing_info_added_list_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bh_routing_info_added_list_item_ies_o[] = {
    bh_routing_info_added_list_item_ies_o::value_c::types::bh_routing_info_added_list_item};
static const uint8_t objset_presence_bh_routing_info_added_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_routing_info_added_list_item_ies_o[] = {
    {objset_crit_bh_routing_info_added_list_item_ies_o, crit_e::nulltype},
    {objset_value_bh_routing_info_added_list_item_ies_o,
     bh_routing_info_added_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_routing_info_added_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_routing_info_added_list_item_ies_o::desc =
    {objset_id_bh_routing_info_added_list_item_ies_o, 1, "id", objset_columns_bh_routing_info_added_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_routing_info_added_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_routing_info_added_list_item_s::desc}};
static const asn1::table::field_name choice_table_bh_routing_info_added_list_item_ies_o_value_c_names[] = {
    {"BH-Routing-Information-Added-List-Item", nullptr}};
const asn1::table::choice_desc bh_routing_info_added_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_routing_info_added_list_item_ies_o::value_c::types>(),
    bh_routing_info_added_list_item_ies_o::value_c::types::nof_types,
    bh_routing_info_added_list_item_ies_o::value_c::types::nof_exts,
    bh_routing_info_added_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_routing_info_added_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_routing_info_added_list_item_ies_o_value_c_names,
    "bh_routing_info_added_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_routing_info_added_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_routing_info_added_list_item_ies_o_value_c_types_values[] = {
    "BH-Routing-Information-Added-List-Item"};
const asn1::table::enum_desc bh_routing_info_added_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_routing_info_added_list_item_ies_o_value_c_types_values,
    1,
    "bh_routing_info_added_list_item_ies_o::value_c::types"};

// BH-Routing-Information-Removed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_routing_info_remd_list_item_ies_o[]    = {286};
static const uint8_t  objset_crit_bh_routing_info_remd_list_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bh_routing_info_remd_list_item_ies_o[] = {
    bh_routing_info_remd_list_item_ies_o::value_c::types::bh_routing_info_remd_list_item};
static const uint8_t objset_presence_bh_routing_info_remd_list_item_ies_o[]                   = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_routing_info_remd_list_item_ies_o[] = {
    {objset_crit_bh_routing_info_remd_list_item_ies_o, crit_e::nulltype},
    {objset_value_bh_routing_info_remd_list_item_ies_o, bh_routing_info_remd_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_routing_info_remd_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_routing_info_remd_list_item_ies_o::desc =
    {objset_id_bh_routing_info_remd_list_item_ies_o, 1, "id", objset_columns_bh_routing_info_remd_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_routing_info_remd_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_routing_info_remd_list_item_s::desc}};
static const asn1::table::field_name choice_table_bh_routing_info_remd_list_item_ies_o_value_c_names[] = {
    {"BH-Routing-Information-Removed-List-Item", nullptr}};
const asn1::table::choice_desc bh_routing_info_remd_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_routing_info_remd_list_item_ies_o::value_c::types>(),
    bh_routing_info_remd_list_item_ies_o::value_c::types::nof_types,
    bh_routing_info_remd_list_item_ies_o::value_c::types::nof_exts,
    bh_routing_info_remd_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_routing_info_remd_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_routing_info_remd_list_item_ies_o_value_c_names,
    "bh_routing_info_remd_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_routing_info_remd_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_routing_info_remd_list_item_ies_o_value_c_types_values[] = {
    "BH-Routing-Information-Removed-List-Item"};
const asn1::table::enum_desc bh_routing_info_remd_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_routing_info_remd_list_item_ies_o_value_c_types_values,
    1,
    "bh_routing_info_remd_list_item_ies_o::value_c::types"};

// BHChannels-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_failed_to_be_modified_item_ies_o[]    = {268};
static const uint8_t  objset_crit_bh_chs_failed_to_be_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bh_chs_failed_to_be_modified_item_ies_o[] = {
    bh_chs_failed_to_be_modified_item_ies_o::value_c::types::bh_chs_failed_to_be_modified_item};
static const uint8_t objset_presence_bh_chs_failed_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_failed_to_be_modified_item_ies_o[] = {
    {objset_crit_bh_chs_failed_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_failed_to_be_modified_item_ies_o,
     bh_chs_failed_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_failed_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_failed_to_be_modified_item_ies_o::desc = {
    objset_id_bh_chs_failed_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_bh_chs_failed_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_failed_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_failed_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_failed_to_be_modified_item_ies_o_value_c_names[] = {
    {"BHChannels-FailedToBeModified-Item", nullptr}};
const asn1::table::choice_desc bh_chs_failed_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_failed_to_be_modified_item_ies_o::value_c::types>(),
    bh_chs_failed_to_be_modified_item_ies_o::value_c::types::nof_types,
    bh_chs_failed_to_be_modified_item_ies_o::value_c::types::nof_exts,
    bh_chs_failed_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_failed_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_failed_to_be_modified_item_ies_o_value_c_names,
    "bh_chs_failed_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_failed_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_failed_to_be_modified_item_ies_o_value_c_types_values[] = {
    "BHChannels-FailedToBeModified-Item"};
const asn1::table::enum_desc bh_chs_failed_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_failed_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "bh_chs_failed_to_be_modified_item_ies_o::value_c::types"};

// BHChannels-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_failed_to_be_setup_item_ies_o[]    = {278};
static const uint8_t  objset_crit_bh_chs_failed_to_be_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bh_chs_failed_to_be_setup_item_ies_o[] = {
    bh_chs_failed_to_be_setup_item_ies_o::value_c::types::bh_chs_failed_to_be_setup_item};
static const uint8_t objset_presence_bh_chs_failed_to_be_setup_item_ies_o[]                   = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_failed_to_be_setup_item_ies_o[] = {
    {objset_crit_bh_chs_failed_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_failed_to_be_setup_item_ies_o, bh_chs_failed_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_failed_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_failed_to_be_setup_item_ies_o::desc =
    {objset_id_bh_chs_failed_to_be_setup_item_ies_o, 1, "id", objset_columns_bh_chs_failed_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_failed_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_failed_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_failed_to_be_setup_item_ies_o_value_c_names[] = {
    {"BHChannels-FailedToBeSetup-Item", nullptr}};
const asn1::table::choice_desc bh_chs_failed_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_failed_to_be_setup_item_ies_o::value_c::types>(),
    bh_chs_failed_to_be_setup_item_ies_o::value_c::types::nof_types,
    bh_chs_failed_to_be_setup_item_ies_o::value_c::types::nof_exts,
    bh_chs_failed_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_failed_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_failed_to_be_setup_item_ies_o_value_c_names,
    "bh_chs_failed_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_failed_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_failed_to_be_setup_item_ies_o_value_c_types_values[] = {
    "BHChannels-FailedToBeSetup-Item"};
const asn1::table::enum_desc bh_chs_failed_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_failed_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "bh_chs_failed_to_be_setup_item_ies_o::value_c::types"};

// BHChannels-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_failed_to_be_setup_mod_item_ies_o[]    = {270};
static const uint8_t  objset_crit_bh_chs_failed_to_be_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bh_chs_failed_to_be_setup_mod_item_ies_o[] = {
    bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types::bh_chs_failed_to_be_setup_mod_item};
static const uint8_t objset_presence_bh_chs_failed_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_failed_to_be_setup_mod_item_ies_o[] = {
    {objset_crit_bh_chs_failed_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_failed_to_be_setup_mod_item_ies_o,
     bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_failed_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_failed_to_be_setup_mod_item_ies_o::desc = {
    objset_id_bh_chs_failed_to_be_setup_mod_item_ies_o,
    1,
    "id",
    objset_columns_bh_chs_failed_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_failed_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_failed_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"BHChannels-FailedToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types>(),
    bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_failed_to_be_setup_mod_item_ies_o_value_c_names,
    "bh_chs_failed_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_failed_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_failed_to_be_setup_mod_item_ies_o_value_c_types_values[] = {
    "BHChannels-FailedToBeSetupMod-Item"};
const asn1::table::enum_desc bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_failed_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types"};

// BHChannels-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_modified_item_ies_o[]    = {272};
static const uint8_t  objset_crit_bh_chs_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bh_chs_modified_item_ies_o[] = {
    bh_chs_modified_item_ies_o::value_c::types::bh_chs_modified_item};
static const uint8_t                    objset_presence_bh_chs_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_modified_item_ies_o[]  = {
    {objset_crit_bh_chs_modified_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_modified_item_ies_o, bh_chs_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_modified_item_ies_o::desc = {objset_id_bh_chs_modified_item_ies_o,
                                                                   1,
                                                                   "id",
                                                                   objset_columns_bh_chs_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_modified_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_modified_item_ies_o_value_c_names[] = {
    {"BHChannels-Modified-Item", nullptr}};
const asn1::table::choice_desc bh_chs_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_modified_item_ies_o::value_c::types>(),
    bh_chs_modified_item_ies_o::value_c::types::nof_types,
    bh_chs_modified_item_ies_o::value_c::types::nof_exts,
    bh_chs_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_modified_item_ies_o_value_c_names,
    "bh_chs_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_modified_item_ies_o_value_c_types_values[]    = {"BHChannels-Modified-Item"};
const asn1::table::enum_desc bh_chs_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_modified_item_ies_o_value_c_types_values,
    1,
    "bh_chs_modified_item_ies_o::value_c::types"};

// BHChannels-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_required_to_be_released_item_ies_o[]    = {276};
static const uint8_t  objset_crit_bh_chs_required_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_bh_chs_required_to_be_released_item_ies_o[] = {
    bh_chs_required_to_be_released_item_ies_o::value_c::types::bh_chs_required_to_be_released_item};
static const uint8_t objset_presence_bh_chs_required_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_required_to_be_released_item_ies_o[] = {
    {objset_crit_bh_chs_required_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_required_to_be_released_item_ies_o,
     bh_chs_required_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_required_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_required_to_be_released_item_ies_o::desc = {
    objset_id_bh_chs_required_to_be_released_item_ies_o,
    1,
    "id",
    objset_columns_bh_chs_required_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_required_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_required_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_required_to_be_released_item_ies_o_value_c_names[] = {
    {"BHChannels-Required-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc bh_chs_required_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_required_to_be_released_item_ies_o::value_c::types>(),
    bh_chs_required_to_be_released_item_ies_o::value_c::types::nof_types,
    bh_chs_required_to_be_released_item_ies_o::value_c::types::nof_exts,
    bh_chs_required_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_required_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_required_to_be_released_item_ies_o_value_c_names,
    "bh_chs_required_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_required_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_required_to_be_released_item_ies_o_value_c_types_values[] = {
    "BHChannels-Required-ToBeReleased-Item"};
const asn1::table::enum_desc bh_chs_required_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_required_to_be_released_item_ies_o_value_c_types_values,
    1,
    "bh_chs_required_to_be_released_item_ies_o::value_c::types"};

// BHChannels-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_setup_item_ies_o[]    = {261};
static const uint8_t  objset_crit_bh_chs_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bh_chs_setup_item_ies_o[] = {
    bh_chs_setup_item_ies_o::value_c::types::bh_chs_setup_item};
static const uint8_t                    objset_presence_bh_chs_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_setup_item_ies_o[]  = {
    {objset_crit_bh_chs_setup_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_setup_item_ies_o, bh_chs_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_setup_item_ies_o::desc = {objset_id_bh_chs_setup_item_ies_o,
                                                                1,
                                                                "id",
                                                                objset_columns_bh_chs_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_setup_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_setup_item_ies_o_value_c_names[] = {
    {"BHChannels-Setup-Item", nullptr}};
const asn1::table::choice_desc bh_chs_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_setup_item_ies_o::value_c::types>(),
    bh_chs_setup_item_ies_o::value_c::types::nof_types,
    bh_chs_setup_item_ies_o::value_c::types::nof_exts,
    bh_chs_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_setup_item_ies_o_value_c_names,
    "bh_chs_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_bh_chs_setup_item_ies_o_value_c_types_values[] = {"BHChannels-Setup-Item"};
const asn1::table::enum_desc bh_chs_setup_item_ies_o::value_c::types_opts::desc        = {
    enum_names_bh_chs_setup_item_ies_o_value_c_types_values,
    1,
    "bh_chs_setup_item_ies_o::value_c::types"};

// BHChannels-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_setup_mod_item_ies_o[]    = {274};
static const uint8_t  objset_crit_bh_chs_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_bh_chs_setup_mod_item_ies_o[] = {
    bh_chs_setup_mod_item_ies_o::value_c::types::bh_chs_setup_mod_item};
static const uint8_t                    objset_presence_bh_chs_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_setup_mod_item_ies_o[]  = {
    {objset_crit_bh_chs_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_setup_mod_item_ies_o, bh_chs_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_setup_mod_item_ies_o::desc = {objset_id_bh_chs_setup_mod_item_ies_o,
                                                                    1,
                                                                    "id",
                                                                    objset_columns_bh_chs_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_setup_mod_item_ies_o_value_c_names[] = {
    {"BHChannels-SetupMod-Item", nullptr}};
const asn1::table::choice_desc bh_chs_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_setup_mod_item_ies_o::value_c::types>(),
    bh_chs_setup_mod_item_ies_o::value_c::types::nof_types,
    bh_chs_setup_mod_item_ies_o::value_c::types::nof_exts,
    bh_chs_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_setup_mod_item_ies_o_value_c_names,
    "bh_chs_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_setup_mod_item_ies_o_value_c_types_values[]    = {"BHChannels-SetupMod-Item"};
const asn1::table::enum_desc bh_chs_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_setup_mod_item_ies_o_value_c_types_values,
    1,
    "bh_chs_setup_mod_item_ies_o::value_c::types"};

// BHChannels-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_to_be_modified_item_ies_o[]    = {262};
static const uint8_t  objset_crit_bh_chs_to_be_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_bh_chs_to_be_modified_item_ies_o[] = {
    bh_chs_to_be_modified_item_ies_o::value_c::types::bh_chs_to_be_modified_item};
static const uint8_t                    objset_presence_bh_chs_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_to_be_modified_item_ies_o[]  = {
    {objset_crit_bh_chs_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_to_be_modified_item_ies_o, bh_chs_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_to_be_modified_item_ies_o::desc =
    {objset_id_bh_chs_to_be_modified_item_ies_o, 1, "id", objset_columns_bh_chs_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_to_be_modified_item_ies_o_value_c_names[] = {
    {"BHChannels-ToBeModified-Item", nullptr}};
const asn1::table::choice_desc bh_chs_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_to_be_modified_item_ies_o::value_c::types>(),
    bh_chs_to_be_modified_item_ies_o::value_c::types::nof_types,
    bh_chs_to_be_modified_item_ies_o::value_c::types::nof_exts,
    bh_chs_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_to_be_modified_item_ies_o_value_c_names,
    "bh_chs_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_to_be_modified_item_ies_o_value_c_types_values[] = {
    "BHChannels-ToBeModified-Item"};
const asn1::table::enum_desc bh_chs_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "bh_chs_to_be_modified_item_ies_o::value_c::types"};

// BHChannels-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_to_be_released_item_ies_o[]    = {264};
static const uint8_t  objset_crit_bh_chs_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_bh_chs_to_be_released_item_ies_o[] = {
    bh_chs_to_be_released_item_ies_o::value_c::types::bh_chs_to_be_released_item};
static const uint8_t                    objset_presence_bh_chs_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_to_be_released_item_ies_o[]  = {
    {objset_crit_bh_chs_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_to_be_released_item_ies_o, bh_chs_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_to_be_released_item_ies_o::desc =
    {objset_id_bh_chs_to_be_released_item_ies_o, 1, "id", objset_columns_bh_chs_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_to_be_released_item_ies_o_value_c_names[] = {
    {"BHChannels-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc bh_chs_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_to_be_released_item_ies_o::value_c::types>(),
    bh_chs_to_be_released_item_ies_o::value_c::types::nof_types,
    bh_chs_to_be_released_item_ies_o::value_c::types::nof_exts,
    bh_chs_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_to_be_released_item_ies_o_value_c_names,
    "bh_chs_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_to_be_released_item_ies_o_value_c_types_values[] = {
    "BHChannels-ToBeReleased-Item"};
const asn1::table::enum_desc bh_chs_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_to_be_released_item_ies_o_value_c_types_values,
    1,
    "bh_chs_to_be_released_item_ies_o::value_c::types"};

// BHChannels-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_to_be_setup_item_ies_o[]    = {259};
static const uint8_t  objset_crit_bh_chs_to_be_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_bh_chs_to_be_setup_item_ies_o[] = {
    bh_chs_to_be_setup_item_ies_o::value_c::types::bh_chs_to_be_setup_item};
static const uint8_t                    objset_presence_bh_chs_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_to_be_setup_item_ies_o[]  = {
    {objset_crit_bh_chs_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_to_be_setup_item_ies_o, bh_chs_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_to_be_setup_item_ies_o::desc = {objset_id_bh_chs_to_be_setup_item_ies_o,
                                                                      1,
                                                                      "id",
                                                                      objset_columns_bh_chs_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_to_be_setup_item_ies_o_value_c_names[] = {
    {"BHChannels-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc bh_chs_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_to_be_setup_item_ies_o::value_c::types>(),
    bh_chs_to_be_setup_item_ies_o::value_c::types::nof_types,
    bh_chs_to_be_setup_item_ies_o::value_c::types::nof_exts,
    bh_chs_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_to_be_setup_item_ies_o_value_c_names,
    "bh_chs_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_to_be_setup_item_ies_o_value_c_types_values[]    = {"BHChannels-ToBeSetup-Item"};
const asn1::table::enum_desc bh_chs_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "bh_chs_to_be_setup_item_ies_o::value_c::types"};

// BHChannels-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_bh_chs_to_be_setup_mod_item_ies_o[]    = {266};
static const uint8_t  objset_crit_bh_chs_to_be_setup_mod_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_bh_chs_to_be_setup_mod_item_ies_o[] = {
    bh_chs_to_be_setup_mod_item_ies_o::value_c::types::bh_chs_to_be_setup_mod_item};
static const uint8_t                    objset_presence_bh_chs_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_bh_chs_to_be_setup_mod_item_ies_o[]  = {
    {objset_crit_bh_chs_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_bh_chs_to_be_setup_mod_item_ies_o, bh_chs_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_bh_chs_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc bh_chs_to_be_setup_mod_item_ies_o::desc =
    {objset_id_bh_chs_to_be_setup_mod_item_ies_o, 1, "id", objset_columns_bh_chs_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_bh_chs_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_chs_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_bh_chs_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"BHChannels-ToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc bh_chs_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<bh_chs_to_be_setup_mod_item_ies_o::value_c::types>(),
    bh_chs_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    bh_chs_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    bh_chs_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_bh_chs_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_bh_chs_to_be_setup_mod_item_ies_o_value_c_names,
    "bh_chs_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(bh_chs_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_bh_chs_to_be_setup_mod_item_ies_o_value_c_types_values[] = {
    "BHChannels-ToBeSetupMod-Item"};
const asn1::table::enum_desc bh_chs_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_bh_chs_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "bh_chs_to_be_setup_mod_item_ies_o::value_c::types"};

// Broadcast-MRBs-Transport-Request-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_transport_request_item_ies_o[]    = {790};
static const uint8_t  objset_crit_broadcast_m_rbs_transport_request_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_broadcast_m_rbs_transport_request_item_ies_o[] = {
    broadcast_m_rbs_transport_request_item_ies_o::value_c::types::broadcast_m_rbs_transport_request_item};
static const uint8_t objset_presence_broadcast_m_rbs_transport_request_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_transport_request_item_ies_o[] = {
    {objset_crit_broadcast_m_rbs_transport_request_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_transport_request_item_ies_o,
     broadcast_m_rbs_transport_request_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_transport_request_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_transport_request_item_ies_o::desc = {
    objset_id_broadcast_m_rbs_transport_request_item_ies_o,
    1,
    "id",
    objset_columns_broadcast_m_rbs_transport_request_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_m_rbs_transport_request_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &broadcast_m_rbs_transport_request_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_transport_request_item_ies_o_value_c_names[] = {
    {"Broadcast-MRBs-Transport-Request-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_transport_request_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_transport_request_item_ies_o::value_c::types>(),
    broadcast_m_rbs_transport_request_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_transport_request_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_transport_request_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_transport_request_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_transport_request_item_ies_o_value_c_names,
    "broadcast_m_rbs_transport_request_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_transport_request_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_transport_request_item_ies_o_value_c_types_values[] = {
    "Broadcast-MRBs-Transport-Request-Item"};
const asn1::table::enum_desc broadcast_m_rbs_transport_request_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_transport_request_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_transport_request_item_ies_o::value_c::types"};

// Broadcast-To-Be-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_to_be_cancelled_list_item_ies_o[]    = {149};
static const uint8_t  objset_crit_broadcast_to_be_cancelled_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_broadcast_to_be_cancelled_list_item_ies_o[] = {
    broadcast_to_be_cancelled_list_item_ies_o::value_c::types::broadcast_to_be_cancelled_item};
static const uint8_t objset_presence_broadcast_to_be_cancelled_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_to_be_cancelled_list_item_ies_o[] = {
    {objset_crit_broadcast_to_be_cancelled_list_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_to_be_cancelled_list_item_ies_o,
     broadcast_to_be_cancelled_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_to_be_cancelled_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_to_be_cancelled_list_item_ies_o::desc = {
    objset_id_broadcast_to_be_cancelled_list_item_ies_o,
    1,
    "id",
    objset_columns_broadcast_to_be_cancelled_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_to_be_cancelled_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &broadcast_to_be_cancelled_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_to_be_cancelled_list_item_ies_o_value_c_names[] = {
    {"Broadcast-To-Be-Cancelled-Item", nullptr}};
const asn1::table::choice_desc broadcast_to_be_cancelled_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_to_be_cancelled_list_item_ies_o::value_c::types>(),
    broadcast_to_be_cancelled_list_item_ies_o::value_c::types::nof_types,
    broadcast_to_be_cancelled_list_item_ies_o::value_c::types::nof_exts,
    broadcast_to_be_cancelled_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_to_be_cancelled_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_to_be_cancelled_list_item_ies_o_value_c_names,
    "broadcast_to_be_cancelled_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_to_be_cancelled_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_to_be_cancelled_list_item_ies_o_value_c_types_values[] = {
    "Broadcast-To-Be-Cancelled-Item"};
const asn1::table::enum_desc broadcast_to_be_cancelled_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_to_be_cancelled_list_item_ies_o_value_c_types_values,
    1,
    "broadcast_to_be_cancelled_list_item_ies_o::value_c::types"};

// BroadcastMRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_to_be_setup_mod_item_ies_o[]    = {477};
static const uint8_t  objset_crit_broadcast_m_rbs_to_be_setup_mod_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_broadcast_m_rbs_to_be_setup_mod_item_ies_o[] = {
    broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::broadcast_m_rbs_to_be_setup_mod_item};
static const uint8_t objset_presence_broadcast_m_rbs_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_to_be_setup_mod_item_ies_o[] = {
    {objset_crit_broadcast_m_rbs_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_to_be_setup_mod_item_ies_o,
     broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_to_be_setup_mod_item_ies_o::desc = {
    objset_id_broadcast_m_rbs_to_be_setup_mod_item_ies_o,
    1,
    "id",
    objset_columns_broadcast_m_rbs_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_m_rbs_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &broadcast_m_rbs_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-ToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types>(),
    broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_to_be_setup_mod_item_ies_o_value_c_names,
    "broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_to_be_setup_mod_item_ies_o_value_c_types_values[] = {
    "BroadcastMRBs-ToBeSetupMod-Item"};
const asn1::table::enum_desc broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types"};

// BroadcastMRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_to_be_modified_item_ies_o[]    = {471};
static const uint8_t  objset_crit_broadcast_m_rbs_to_be_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_broadcast_m_rbs_to_be_modified_item_ies_o[] = {
    broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types::broadcast_m_rbs_to_be_modified_item};
static const uint8_t objset_presence_broadcast_m_rbs_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_to_be_modified_item_ies_o[] = {
    {objset_crit_broadcast_m_rbs_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_to_be_modified_item_ies_o,
     broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_to_be_modified_item_ies_o::desc = {
    objset_id_broadcast_m_rbs_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_broadcast_m_rbs_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_m_rbs_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &broadcast_m_rbs_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_to_be_modified_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-ToBeModified-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types>(),
    broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_to_be_modified_item_ies_o_value_c_names,
    "broadcast_m_rbs_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_to_be_modified_item_ies_o_value_c_types_values[] = {
    "BroadcastMRBs-ToBeModified-Item"};
const asn1::table::enum_desc broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types"};

// BroadcastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_to_be_released_item_ies_o[]    = {473};
static const uint8_t  objset_crit_broadcast_m_rbs_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_broadcast_m_rbs_to_be_released_item_ies_o[] = {
    broadcast_m_rbs_to_be_released_item_ies_o::value_c::types::broadcast_m_rbs_to_be_released_item};
static const uint8_t objset_presence_broadcast_m_rbs_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_to_be_released_item_ies_o[] = {
    {objset_crit_broadcast_m_rbs_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_to_be_released_item_ies_o,
     broadcast_m_rbs_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_to_be_released_item_ies_o::desc = {
    objset_id_broadcast_m_rbs_to_be_released_item_ies_o,
    1,
    "id",
    objset_columns_broadcast_m_rbs_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_m_rbs_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &broadcast_m_rbs_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_to_be_released_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_to_be_released_item_ies_o::value_c::types>(),
    broadcast_m_rbs_to_be_released_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_to_be_released_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_to_be_released_item_ies_o_value_c_names,
    "broadcast_m_rbs_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_to_be_released_item_ies_o_value_c_types_values[] = {
    "BroadcastMRBs-ToBeReleased-Item"};
const asn1::table::enum_desc broadcast_m_rbs_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_to_be_released_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_to_be_released_item_ies_o::value_c::types"};

// BroadcastMRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_setup_mod_item_ies_o[]    = {469};
static const uint8_t  objset_crit_broadcast_m_rbs_setup_mod_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_broadcast_m_rbs_setup_mod_item_ies_o[] = {
    broadcast_m_rbs_setup_mod_item_ies_o::value_c::types::broadcast_m_rbs_setup_mod_item};
static const uint8_t objset_presence_broadcast_m_rbs_setup_mod_item_ies_o[]                   = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_setup_mod_item_ies_o[] = {
    {objset_crit_broadcast_m_rbs_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_setup_mod_item_ies_o, broadcast_m_rbs_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_setup_mod_item_ies_o::desc =
    {objset_id_broadcast_m_rbs_setup_mod_item_ies_o, 1, "id", objset_columns_broadcast_m_rbs_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_m_rbs_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &broadcast_m_rbs_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_setup_mod_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-SetupMod-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_setup_mod_item_ies_o::value_c::types>(),
    broadcast_m_rbs_setup_mod_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_setup_mod_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_setup_mod_item_ies_o_value_c_names,
    "broadcast_m_rbs_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_setup_mod_item_ies_o_value_c_types_values[] = {
    "BroadcastMRBs-SetupMod-Item"};
const asn1::table::enum_desc broadcast_m_rbs_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_setup_mod_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_setup_mod_item_ies_o::value_c::types"};

// BroadcastMRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o[]    = {463};
static const uint8_t  objset_crit_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o[] = {
    broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::broadcast_m_rbs_failed_to_be_setup_mod_item};
static const uint8_t objset_presence_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o[] = {
    {objset_crit_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o,
     broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::desc = {
    objset_id_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o,
    1,
    "id",
    objset_columns_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &broadcast_m_rbs_failed_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-FailedToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types>(),
    broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_names,
    "broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values[] = {
    "BroadcastMRBs-FailedToBeSetupMod-Item"};
const asn1::table::enum_desc broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types"};

// BroadcastMRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_modified_item_ies_o[]    = {465};
static const uint8_t  objset_crit_broadcast_m_rbs_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_broadcast_m_rbs_modified_item_ies_o[] = {
    broadcast_m_rbs_modified_item_ies_o::value_c::types::broadcast_m_rbs_modified_item};
static const uint8_t                    objset_presence_broadcast_m_rbs_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_modified_item_ies_o[]  = {
    {objset_crit_broadcast_m_rbs_modified_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_modified_item_ies_o, broadcast_m_rbs_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_modified_item_ies_o::desc =
    {objset_id_broadcast_m_rbs_modified_item_ies_o, 1, "id", objset_columns_broadcast_m_rbs_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_m_rbs_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &broadcast_m_rbs_modified_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_modified_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-Modified-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_modified_item_ies_o::value_c::types>(),
    broadcast_m_rbs_modified_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_modified_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_modified_item_ies_o_value_c_names,
    "broadcast_m_rbs_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_modified_item_ies_o_value_c_types_values[] = {
    "BroadcastMRBs-Modified-Item"};
const asn1::table::enum_desc broadcast_m_rbs_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_modified_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_modified_item_ies_o::value_c::types"};

// BroadcastMRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_failed_to_be_modified_item_ies_o[]    = {459};
static const uint8_t  objset_crit_broadcast_m_rbs_failed_to_be_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_broadcast_m_rbs_failed_to_be_modified_item_ies_o[] = {
    broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::broadcast_m_rbs_failed_to_be_modified_item};
static const uint8_t objset_presence_broadcast_m_rbs_failed_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_failed_to_be_modified_item_ies_o[] = {
    {objset_crit_broadcast_m_rbs_failed_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_failed_to_be_modified_item_ies_o,
     broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_failed_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_failed_to_be_modified_item_ies_o::desc = {
    objset_id_broadcast_m_rbs_failed_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_broadcast_m_rbs_failed_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_broadcast_m_rbs_failed_to_be_modified_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &broadcast_m_rbs_failed_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_failed_to_be_modified_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-FailedToBeModified-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types>(),
    broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_failed_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_failed_to_be_modified_item_ies_o_value_c_names,
    "broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_failed_to_be_modified_item_ies_o_value_c_types_values[] = {
    "BroadcastMRBs-FailedToBeModified-Item"};
const asn1::table::enum_desc broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_failed_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types"};

// BroadcastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_to_be_setup_item_ies_o[]    = {475};
static const uint8_t  objset_crit_broadcast_m_rbs_to_be_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_broadcast_m_rbs_to_be_setup_item_ies_o[] = {
    broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types::broadcast_m_rbs_to_be_setup_item};
static const uint8_t objset_presence_broadcast_m_rbs_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_to_be_setup_item_ies_o[] = {
    {objset_crit_broadcast_m_rbs_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_to_be_setup_item_ies_o,
     broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_to_be_setup_item_ies_o::desc =
    {objset_id_broadcast_m_rbs_to_be_setup_item_ies_o, 1, "id", objset_columns_broadcast_m_rbs_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_m_rbs_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &broadcast_m_rbs_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_to_be_setup_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types>(),
    broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_to_be_setup_item_ies_o_value_c_names,
    "broadcast_m_rbs_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_to_be_setup_item_ies_o_value_c_types_values[] = {
    "BroadcastMRBs-ToBeSetup-Item"};
const asn1::table::enum_desc broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types"};

// BroadcastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_setup_item_ies_o[]    = {467};
static const uint8_t  objset_crit_broadcast_m_rbs_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_broadcast_m_rbs_setup_item_ies_o[] = {
    broadcast_m_rbs_setup_item_ies_o::value_c::types::broadcast_m_rbs_setup_item};
static const uint8_t                    objset_presence_broadcast_m_rbs_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_setup_item_ies_o[]  = {
    {objset_crit_broadcast_m_rbs_setup_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_setup_item_ies_o, broadcast_m_rbs_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_setup_item_ies_o::desc =
    {objset_id_broadcast_m_rbs_setup_item_ies_o, 1, "id", objset_columns_broadcast_m_rbs_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_m_rbs_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &broadcast_m_rbs_setup_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_setup_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-Setup-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_setup_item_ies_o::value_c::types>(),
    broadcast_m_rbs_setup_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_setup_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_setup_item_ies_o_value_c_names,
    "broadcast_m_rbs_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_setup_item_ies_o_value_c_types_values[]    = {"BroadcastMRBs-Setup-Item"};
const asn1::table::enum_desc broadcast_m_rbs_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_setup_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_setup_item_ies_o::value_c::types"};

// BroadcastMRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_broadcast_m_rbs_failed_to_be_setup_item_ies_o[]    = {461};
static const uint8_t  objset_crit_broadcast_m_rbs_failed_to_be_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_broadcast_m_rbs_failed_to_be_setup_item_ies_o[] = {
    broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::broadcast_m_rbs_failed_to_be_setup_item};
static const uint8_t objset_presence_broadcast_m_rbs_failed_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_broadcast_m_rbs_failed_to_be_setup_item_ies_o[] = {
    {objset_crit_broadcast_m_rbs_failed_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_broadcast_m_rbs_failed_to_be_setup_item_ies_o,
     broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_broadcast_m_rbs_failed_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc broadcast_m_rbs_failed_to_be_setup_item_ies_o::desc = {
    objset_id_broadcast_m_rbs_failed_to_be_setup_item_ies_o,
    1,
    "id",
    objset_columns_broadcast_m_rbs_failed_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_broadcast_m_rbs_failed_to_be_setup_item_ies_o_value_c_alternatives[] =
    {{0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &broadcast_m_rbs_failed_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_broadcast_m_rbs_failed_to_be_setup_item_ies_o_value_c_names[] = {
    {"BroadcastMRBs-FailedToBeSetup-Item", nullptr}};
const asn1::table::choice_desc broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types>(),
    broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::nof_types,
    broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::nof_exts,
    broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_broadcast_m_rbs_failed_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_broadcast_m_rbs_failed_to_be_setup_item_ies_o_value_c_names,
    "broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_broadcast_m_rbs_failed_to_be_setup_item_ies_o_value_c_types_values[] = {
    "BroadcastMRBs-FailedToBeSetup-Item"};
const asn1::table::enum_desc broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_broadcast_m_rbs_failed_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types"};

// Candidate-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_candidate_sp_cell_item_ies_o[]    = {91};
static const uint8_t  objset_crit_candidate_sp_cell_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_candidate_sp_cell_item_ies_o[] = {
    candidate_sp_cell_item_ies_o::value_c::types::candidate_sp_cell_item};
static const uint8_t                    objset_presence_candidate_sp_cell_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_candidate_sp_cell_item_ies_o[]  = {
    {objset_crit_candidate_sp_cell_item_ies_o, crit_e::nulltype},
    {objset_value_candidate_sp_cell_item_ies_o, candidate_sp_cell_item_ies_o::value_c::types::nulltype},
    {objset_presence_candidate_sp_cell_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc candidate_sp_cell_item_ies_o::desc = {objset_id_candidate_sp_cell_item_ies_o,
                                                                     1,
                                                                     "id",
                                                                     objset_columns_candidate_sp_cell_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_candidate_sp_cell_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &candidate_sp_cell_item_s::desc}};
static const asn1::table::field_name choice_table_candidate_sp_cell_item_ies_o_value_c_names[] = {
    {"Candidate-SpCell-Item", nullptr}};
const asn1::table::choice_desc candidate_sp_cell_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<candidate_sp_cell_item_ies_o::value_c::types>(),
    candidate_sp_cell_item_ies_o::value_c::types::nof_types,
    candidate_sp_cell_item_ies_o::value_c::types::nof_exts,
    candidate_sp_cell_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_candidate_sp_cell_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_candidate_sp_cell_item_ies_o_value_c_names,
    "candidate_sp_cell_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(candidate_sp_cell_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_candidate_sp_cell_item_ies_o_value_c_types_values[] = {"Candidate-SpCell-Item"};
const asn1::table::enum_desc candidate_sp_cell_item_ies_o::value_c::types_opts::desc        = {
    enum_names_candidate_sp_cell_item_ies_o_value_c_types_values,
    1,
    "candidate_sp_cell_item_ies_o::value_c::types"};

// Cells-Allowed-to-be-Deactivated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_cells_allowed_to_be_deactiv_list_item_ies_o[]    = {747};
static const uint8_t  objset_crit_cells_allowed_to_be_deactiv_list_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_cells_allowed_to_be_deactiv_list_item_ies_o[] = {
    cells_allowed_to_be_deactiv_list_item_ies_o::value_c::types::cells_allowed_to_be_deactiv_list_item};
static const uint8_t objset_presence_cells_allowed_to_be_deactiv_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cells_allowed_to_be_deactiv_list_item_ies_o[] = {
    {objset_crit_cells_allowed_to_be_deactiv_list_item_ies_o, crit_e::nulltype},
    {objset_value_cells_allowed_to_be_deactiv_list_item_ies_o,
     cells_allowed_to_be_deactiv_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_cells_allowed_to_be_deactiv_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cells_allowed_to_be_deactiv_list_item_ies_o::desc = {
    objset_id_cells_allowed_to_be_deactiv_list_item_ies_o,
    1,
    "id",
    objset_columns_cells_allowed_to_be_deactiv_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cells_allowed_to_be_deactiv_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cells_allowed_to_be_deactiv_list_item_s::desc}};
static const asn1::table::field_name choice_table_cells_allowed_to_be_deactiv_list_item_ies_o_value_c_names[] = {
    {"Cells-Allowed-to-be-Deactivated-List-Item", nullptr}};
const asn1::table::choice_desc cells_allowed_to_be_deactiv_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cells_allowed_to_be_deactiv_list_item_ies_o::value_c::types>(),
    cells_allowed_to_be_deactiv_list_item_ies_o::value_c::types::nof_types,
    cells_allowed_to_be_deactiv_list_item_ies_o::value_c::types::nof_exts,
    cells_allowed_to_be_deactiv_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_cells_allowed_to_be_deactiv_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cells_allowed_to_be_deactiv_list_item_ies_o_value_c_names,
    "cells_allowed_to_be_deactiv_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cells_allowed_to_be_deactiv_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_cells_allowed_to_be_deactiv_list_item_ies_o_value_c_types_values[] = {
    "Cells-Allowed-to-be-Deactivated-List-Item"};
const asn1::table::enum_desc cells_allowed_to_be_deactiv_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_cells_allowed_to_be_deactiv_list_item_ies_o_value_c_types_values,
    1,
    "cells_allowed_to_be_deactiv_list_item_ies_o::value_c::types"};

// Cells-Broadcast-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_cells_broadcast_cancelled_list_item_ies_o[]    = {151};
static const uint8_t  objset_crit_cells_broadcast_cancelled_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_cells_broadcast_cancelled_list_item_ies_o[] = {
    cells_broadcast_cancelled_list_item_ies_o::value_c::types::cells_broadcast_cancelled_item};
static const uint8_t objset_presence_cells_broadcast_cancelled_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cells_broadcast_cancelled_list_item_ies_o[] = {
    {objset_crit_cells_broadcast_cancelled_list_item_ies_o, crit_e::nulltype},
    {objset_value_cells_broadcast_cancelled_list_item_ies_o,
     cells_broadcast_cancelled_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_cells_broadcast_cancelled_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cells_broadcast_cancelled_list_item_ies_o::desc = {
    objset_id_cells_broadcast_cancelled_list_item_ies_o,
    1,
    "id",
    objset_columns_cells_broadcast_cancelled_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cells_broadcast_cancelled_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cells_broadcast_cancelled_item_s::desc}};
static const asn1::table::field_name choice_table_cells_broadcast_cancelled_list_item_ies_o_value_c_names[] = {
    {"Cells-Broadcast-Cancelled-Item", nullptr}};
const asn1::table::choice_desc cells_broadcast_cancelled_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cells_broadcast_cancelled_list_item_ies_o::value_c::types>(),
    cells_broadcast_cancelled_list_item_ies_o::value_c::types::nof_types,
    cells_broadcast_cancelled_list_item_ies_o::value_c::types::nof_exts,
    cells_broadcast_cancelled_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_cells_broadcast_cancelled_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cells_broadcast_cancelled_list_item_ies_o_value_c_names,
    "cells_broadcast_cancelled_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cells_broadcast_cancelled_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_cells_broadcast_cancelled_list_item_ies_o_value_c_types_values[] = {
    "Cells-Broadcast-Cancelled-Item"};
const asn1::table::enum_desc cells_broadcast_cancelled_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_cells_broadcast_cancelled_list_item_ies_o_value_c_types_values,
    1,
    "cells_broadcast_cancelled_list_item_ies_o::value_c::types"};

// Cells-Broadcast-Completed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_cells_broadcast_completed_list_item_ies_o[]    = {147};
static const uint8_t  objset_crit_cells_broadcast_completed_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_cells_broadcast_completed_list_item_ies_o[] = {
    cells_broadcast_completed_list_item_ies_o::value_c::types::cells_broadcast_completed_item};
static const uint8_t objset_presence_cells_broadcast_completed_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cells_broadcast_completed_list_item_ies_o[] = {
    {objset_crit_cells_broadcast_completed_list_item_ies_o, crit_e::nulltype},
    {objset_value_cells_broadcast_completed_list_item_ies_o,
     cells_broadcast_completed_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_cells_broadcast_completed_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cells_broadcast_completed_list_item_ies_o::desc = {
    objset_id_cells_broadcast_completed_list_item_ies_o,
    1,
    "id",
    objset_columns_cells_broadcast_completed_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cells_broadcast_completed_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cells_broadcast_completed_item_s::desc}};
static const asn1::table::field_name choice_table_cells_broadcast_completed_list_item_ies_o_value_c_names[] = {
    {"Cells-Broadcast-Completed-Item", nullptr}};
const asn1::table::choice_desc cells_broadcast_completed_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cells_broadcast_completed_list_item_ies_o::value_c::types>(),
    cells_broadcast_completed_list_item_ies_o::value_c::types::nof_types,
    cells_broadcast_completed_list_item_ies_o::value_c::types::nof_exts,
    cells_broadcast_completed_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_cells_broadcast_completed_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cells_broadcast_completed_list_item_ies_o_value_c_names,
    "cells_broadcast_completed_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cells_broadcast_completed_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_cells_broadcast_completed_list_item_ies_o_value_c_types_values[] = {
    "Cells-Broadcast-Completed-Item"};
const asn1::table::enum_desc cells_broadcast_completed_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_cells_broadcast_completed_list_item_ies_o_value_c_types_values,
    1,
    "cells_broadcast_completed_list_item_ies_o::value_c::types"};

// Cells-Failed-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_cells_failed_to_be_activ_list_item_ies_o[]    = {2};
static const uint8_t  objset_crit_cells_failed_to_be_activ_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_cells_failed_to_be_activ_list_item_ies_o[] = {
    cells_failed_to_be_activ_list_item_ies_o::value_c::types::cells_failed_to_be_activ_list_item};
static const uint8_t objset_presence_cells_failed_to_be_activ_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cells_failed_to_be_activ_list_item_ies_o[] = {
    {objset_crit_cells_failed_to_be_activ_list_item_ies_o, crit_e::nulltype},
    {objset_value_cells_failed_to_be_activ_list_item_ies_o,
     cells_failed_to_be_activ_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_cells_failed_to_be_activ_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cells_failed_to_be_activ_list_item_ies_o::desc = {
    objset_id_cells_failed_to_be_activ_list_item_ies_o,
    1,
    "id",
    objset_columns_cells_failed_to_be_activ_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cells_failed_to_be_activ_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cells_failed_to_be_activ_list_item_s::desc}};
static const asn1::table::field_name choice_table_cells_failed_to_be_activ_list_item_ies_o_value_c_names[] = {
    {"Cells-Failed-to-be-Activated-List-Item", nullptr}};
const asn1::table::choice_desc cells_failed_to_be_activ_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cells_failed_to_be_activ_list_item_ies_o::value_c::types>(),
    cells_failed_to_be_activ_list_item_ies_o::value_c::types::nof_types,
    cells_failed_to_be_activ_list_item_ies_o::value_c::types::nof_exts,
    cells_failed_to_be_activ_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_cells_failed_to_be_activ_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cells_failed_to_be_activ_list_item_ies_o_value_c_names,
    "cells_failed_to_be_activ_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cells_failed_to_be_activ_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_cells_failed_to_be_activ_list_item_ies_o_value_c_types_values[] = {
    "Cells-Failed-to-be-Activated-List-Item"};
const asn1::table::enum_desc cells_failed_to_be_activ_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_cells_failed_to_be_activ_list_item_ies_o_value_c_types_values,
    1,
    "cells_failed_to_be_activ_list_item_ies_o::value_c::types"};

// Cells-Status-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_cells_status_item_ies_o[]    = {88};
static const uint8_t  objset_crit_cells_status_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_cells_status_item_ies_o[] = {
    cells_status_item_ies_o::value_c::types::cells_status_item};
static const uint8_t                    objset_presence_cells_status_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cells_status_item_ies_o[]  = {
    {objset_crit_cells_status_item_ies_o, crit_e::nulltype},
    {objset_value_cells_status_item_ies_o, cells_status_item_ies_o::value_c::types::nulltype},
    {objset_presence_cells_status_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cells_status_item_ies_o::desc = {objset_id_cells_status_item_ies_o,
                                                                1,
                                                                "id",
                                                                objset_columns_cells_status_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cells_status_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cells_status_item_s::desc}};
static const asn1::table::field_name choice_table_cells_status_item_ies_o_value_c_names[] = {
    {"Cells-Status-Item", nullptr}};
const asn1::table::choice_desc cells_status_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cells_status_item_ies_o::value_c::types>(),
    cells_status_item_ies_o::value_c::types::nof_types,
    cells_status_item_ies_o::value_c::types::nof_exts,
    cells_status_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_cells_status_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cells_status_item_ies_o_value_c_names,
    "cells_status_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cells_status_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_cells_status_item_ies_o_value_c_types_values[] = {"Cells-Status-Item"};
const asn1::table::enum_desc cells_status_item_ies_o::value_c::types_opts::desc        = {
    enum_names_cells_status_item_ies_o_value_c_types_values,
    1,
    "cells_status_item_ies_o::value_c::types"};

// Cells-To-Be-Broadcast-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_cells_to_be_broadcast_list_item_ies_o[]    = {145};
static const uint8_t  objset_crit_cells_to_be_broadcast_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_cells_to_be_broadcast_list_item_ies_o[] = {
    cells_to_be_broadcast_list_item_ies_o::value_c::types::cells_to_be_broadcast_item};
static const uint8_t objset_presence_cells_to_be_broadcast_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cells_to_be_broadcast_list_item_ies_o[] = {
    {objset_crit_cells_to_be_broadcast_list_item_ies_o, crit_e::nulltype},
    {objset_value_cells_to_be_broadcast_list_item_ies_o,
     cells_to_be_broadcast_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_cells_to_be_broadcast_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cells_to_be_broadcast_list_item_ies_o::desc =
    {objset_id_cells_to_be_broadcast_list_item_ies_o, 1, "id", objset_columns_cells_to_be_broadcast_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cells_to_be_broadcast_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cells_to_be_broadcast_item_s::desc}};
static const asn1::table::field_name choice_table_cells_to_be_broadcast_list_item_ies_o_value_c_names[] = {
    {"Cells-To-Be-Broadcast-Item", nullptr}};
const asn1::table::choice_desc cells_to_be_broadcast_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cells_to_be_broadcast_list_item_ies_o::value_c::types>(),
    cells_to_be_broadcast_list_item_ies_o::value_c::types::nof_types,
    cells_to_be_broadcast_list_item_ies_o::value_c::types::nof_exts,
    cells_to_be_broadcast_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_cells_to_be_broadcast_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cells_to_be_broadcast_list_item_ies_o_value_c_names,
    "cells_to_be_broadcast_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cells_to_be_broadcast_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_cells_to_be_broadcast_list_item_ies_o_value_c_types_values[] = {
    "Cells-To-Be-Broadcast-Item"};
const asn1::table::enum_desc cells_to_be_broadcast_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_cells_to_be_broadcast_list_item_ies_o_value_c_types_values,
    1,
    "cells_to_be_broadcast_list_item_ies_o::value_c::types"};

// Cells-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_cells_to_be_activ_list_item_ies_o[]    = {4};
static const uint8_t  objset_crit_cells_to_be_activ_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_cells_to_be_activ_list_item_ies_o[] = {
    cells_to_be_activ_list_item_ies_o::value_c::types::cells_to_be_activ_list_item};
static const uint8_t                    objset_presence_cells_to_be_activ_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cells_to_be_activ_list_item_ies_o[]  = {
    {objset_crit_cells_to_be_activ_list_item_ies_o, crit_e::nulltype},
    {objset_value_cells_to_be_activ_list_item_ies_o, cells_to_be_activ_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_cells_to_be_activ_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cells_to_be_activ_list_item_ies_o::desc =
    {objset_id_cells_to_be_activ_list_item_ies_o, 1, "id", objset_columns_cells_to_be_activ_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cells_to_be_activ_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cells_to_be_activ_list_item_s::desc}};
static const asn1::table::field_name choice_table_cells_to_be_activ_list_item_ies_o_value_c_names[] = {
    {"Cells-to-be-Activated-List-Item", nullptr}};
const asn1::table::choice_desc cells_to_be_activ_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cells_to_be_activ_list_item_ies_o::value_c::types>(),
    cells_to_be_activ_list_item_ies_o::value_c::types::nof_types,
    cells_to_be_activ_list_item_ies_o::value_c::types::nof_exts,
    cells_to_be_activ_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_cells_to_be_activ_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cells_to_be_activ_list_item_ies_o_value_c_names,
    "cells_to_be_activ_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cells_to_be_activ_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_cells_to_be_activ_list_item_ies_o_value_c_types_values[] = {
    "Cells-to-be-Activated-List-Item"};
const asn1::table::enum_desc cells_to_be_activ_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_cells_to_be_activ_list_item_ies_o_value_c_types_values,
    1,
    "cells_to_be_activ_list_item_ies_o::value_c::types"};

// Cells-to-be-Barred-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_cells_to_be_barred_item_ies_o[]    = {130};
static const uint8_t  objset_crit_cells_to_be_barred_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_cells_to_be_barred_item_ies_o[] = {
    cells_to_be_barred_item_ies_o::value_c::types::cells_to_be_barred_item};
static const uint8_t                    objset_presence_cells_to_be_barred_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cells_to_be_barred_item_ies_o[]  = {
    {objset_crit_cells_to_be_barred_item_ies_o, crit_e::nulltype},
    {objset_value_cells_to_be_barred_item_ies_o, cells_to_be_barred_item_ies_o::value_c::types::nulltype},
    {objset_presence_cells_to_be_barred_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cells_to_be_barred_item_ies_o::desc = {objset_id_cells_to_be_barred_item_ies_o,
                                                                      1,
                                                                      "id",
                                                                      objset_columns_cells_to_be_barred_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cells_to_be_barred_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cells_to_be_barred_item_s::desc}};
static const asn1::table::field_name choice_table_cells_to_be_barred_item_ies_o_value_c_names[] = {
    {"Cells-to-be-Barred-Item", nullptr}};
const asn1::table::choice_desc cells_to_be_barred_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cells_to_be_barred_item_ies_o::value_c::types>(),
    cells_to_be_barred_item_ies_o::value_c::types::nof_types,
    cells_to_be_barred_item_ies_o::value_c::types::nof_exts,
    cells_to_be_barred_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_cells_to_be_barred_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cells_to_be_barred_item_ies_o_value_c_names,
    "cells_to_be_barred_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cells_to_be_barred_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_cells_to_be_barred_item_ies_o_value_c_types_values[]    = {"Cells-to-be-Barred-Item"};
const asn1::table::enum_desc cells_to_be_barred_item_ies_o::value_c::types_opts::desc = {
    enum_names_cells_to_be_barred_item_ies_o_value_c_types_values,
    1,
    "cells_to_be_barred_item_ies_o::value_c::types"};

// Cells-to-be-Deactivated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_cells_to_be_deactiv_list_item_ies_o[]    = {6};
static const uint8_t  objset_crit_cells_to_be_deactiv_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_cells_to_be_deactiv_list_item_ies_o[] = {
    cells_to_be_deactiv_list_item_ies_o::value_c::types::cells_to_be_deactiv_list_item};
static const uint8_t                    objset_presence_cells_to_be_deactiv_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cells_to_be_deactiv_list_item_ies_o[]  = {
    {objset_crit_cells_to_be_deactiv_list_item_ies_o, crit_e::nulltype},
    {objset_value_cells_to_be_deactiv_list_item_ies_o, cells_to_be_deactiv_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_cells_to_be_deactiv_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cells_to_be_deactiv_list_item_ies_o::desc =
    {objset_id_cells_to_be_deactiv_list_item_ies_o, 1, "id", objset_columns_cells_to_be_deactiv_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cells_to_be_deactiv_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cells_to_be_deactiv_list_item_s::desc}};
static const asn1::table::field_name choice_table_cells_to_be_deactiv_list_item_ies_o_value_c_names[] = {
    {"Cells-to-be-Deactivated-List-Item", nullptr}};
const asn1::table::choice_desc cells_to_be_deactiv_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cells_to_be_deactiv_list_item_ies_o::value_c::types>(),
    cells_to_be_deactiv_list_item_ies_o::value_c::types::nof_types,
    cells_to_be_deactiv_list_item_ies_o::value_c::types::nof_exts,
    cells_to_be_deactiv_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_cells_to_be_deactiv_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cells_to_be_deactiv_list_item_ies_o_value_c_names,
    "cells_to_be_deactiv_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cells_to_be_deactiv_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_cells_to_be_deactiv_list_item_ies_o_value_c_types_values[] = {
    "Cells-to-be-Deactivated-List-Item"};
const asn1::table::enum_desc cells_to_be_deactiv_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_cells_to_be_deactiv_list_item_ies_o_value_c_types_values,
    1,
    "cells_to_be_deactiv_list_item_ies_o::value_c::types"};

// DL-UP-TNL-Address-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_dl_up_tnl_address_to_upd_list_item_ies_o[]    = {305};
static const uint8_t  objset_crit_dl_up_tnl_address_to_upd_list_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_dl_up_tnl_address_to_upd_list_item_ies_o[] = {
    dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types::dl_up_tnl_address_to_upd_list_item};
static const uint8_t objset_presence_dl_up_tnl_address_to_upd_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_dl_up_tnl_address_to_upd_list_item_ies_o[] = {
    {objset_crit_dl_up_tnl_address_to_upd_list_item_ies_o, crit_e::nulltype},
    {objset_value_dl_up_tnl_address_to_upd_list_item_ies_o,
     dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_dl_up_tnl_address_to_upd_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc dl_up_tnl_address_to_upd_list_item_ies_o::desc = {
    objset_id_dl_up_tnl_address_to_upd_list_item_ies_o,
    1,
    "id",
    objset_columns_dl_up_tnl_address_to_upd_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_dl_up_tnl_address_to_upd_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_up_tnl_address_to_upd_list_item_s::desc}};
static const asn1::table::field_name choice_table_dl_up_tnl_address_to_upd_list_item_ies_o_value_c_names[] = {
    {"DL-UP-TNL-Address-to-Update-List-Item", nullptr}};
const asn1::table::choice_desc dl_up_tnl_address_to_upd_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types>(),
    dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types::nof_types,
    dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types::nof_exts,
    dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_dl_up_tnl_address_to_upd_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_dl_up_tnl_address_to_upd_list_item_ies_o_value_c_names,
    "dl_up_tnl_address_to_upd_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(dl_up_tnl_address_to_upd_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_dl_up_tnl_address_to_upd_list_item_ies_o_value_c_types_values[] = {
    "DL-UP-TNL-Address-to-Update-List-Item"};
const asn1::table::enum_desc dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_dl_up_tnl_address_to_upd_list_item_ies_o_value_c_types_values,
    1,
    "dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types"};

// DRB-Activity-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drb_activity_item_ies_o[]    = {99};
static const uint8_t  objset_crit_drb_activity_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_drb_activity_item_ies_o[] = {
    drb_activity_item_ies_o::value_c::types::drb_activity_item};
static const uint8_t                    objset_presence_drb_activity_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drb_activity_item_ies_o[]  = {
    {objset_crit_drb_activity_item_ies_o, crit_e::nulltype},
    {objset_value_drb_activity_item_ies_o, drb_activity_item_ies_o::value_c::types::nulltype},
    {objset_presence_drb_activity_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drb_activity_item_ies_o::desc = {objset_id_drb_activity_item_ies_o,
                                                                1,
                                                                "id",
                                                                objset_columns_drb_activity_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drb_activity_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_activity_item_s::desc}};
static const asn1::table::field_name choice_table_drb_activity_item_ies_o_value_c_names[] = {
    {"DRB-Activity-Item", nullptr}};
const asn1::table::choice_desc drb_activity_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drb_activity_item_ies_o::value_c::types>(),
    drb_activity_item_ies_o::value_c::types::nof_types,
    drb_activity_item_ies_o::value_c::types::nof_exts,
    drb_activity_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drb_activity_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drb_activity_item_ies_o_value_c_names,
    "drb_activity_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drb_activity_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_drb_activity_item_ies_o_value_c_types_values[] = {"DRB-Activity-Item"};
const asn1::table::enum_desc drb_activity_item_ies_o::value_c::types_opts::desc        = {
    enum_names_drb_activity_item_ies_o_value_c_types_values,
    1,
    "drb_activity_item_ies_o::value_c::types"};

// DRB-Notify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drb_notify_item_ies_o[]    = {136};
static const uint8_t  objset_crit_drb_notify_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_drb_notify_item_ies_o[] = {drb_notify_item_ies_o::value_c::types::drb_notify_item};
static const uint8_t  objset_presence_drb_notify_item_ies_o[]                  = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drb_notify_item_ies_o[] = {
    {objset_crit_drb_notify_item_ies_o, crit_e::nulltype},
    {objset_value_drb_notify_item_ies_o, drb_notify_item_ies_o::value_c::types::nulltype},
    {objset_presence_drb_notify_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drb_notify_item_ies_o::desc = {objset_id_drb_notify_item_ies_o,
                                                              1,
                                                              "id",
                                                              objset_columns_drb_notify_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drb_notify_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_notify_item_s::desc}};
static const asn1::table::field_name choice_table_drb_notify_item_ies_o_value_c_names[] = {
    {"DRB-Notify-Item", nullptr}};
const asn1::table::choice_desc drb_notify_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drb_notify_item_ies_o::value_c::types>(),
    drb_notify_item_ies_o::value_c::types::nof_types,
    drb_notify_item_ies_o::value_c::types::nof_exts,
    drb_notify_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drb_notify_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drb_notify_item_ies_o_value_c_names,
    "drb_notify_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drb_notify_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_drb_notify_item_ies_o_value_c_types_values[] = {"DRB-Notify-Item"};
const asn1::table::enum_desc drb_notify_item_ies_o::value_c::types_opts::desc        = {
    enum_names_drb_notify_item_ies_o_value_c_types_values,
    1,
    "drb_notify_item_ies_o::value_c::types"};

// DRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_failed_to_be_modified_item_ies_o[]    = {12};
static const uint8_t  objset_crit_drbs_failed_to_be_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_drbs_failed_to_be_modified_item_ies_o[] = {
    drbs_failed_to_be_modified_item_ies_o::value_c::types::drbs_failed_to_be_modified_item};
static const uint8_t objset_presence_drbs_failed_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_failed_to_be_modified_item_ies_o[] = {
    {objset_crit_drbs_failed_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_failed_to_be_modified_item_ies_o,
     drbs_failed_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_failed_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_failed_to_be_modified_item_ies_o::desc =
    {objset_id_drbs_failed_to_be_modified_item_ies_o, 1, "id", objset_columns_drbs_failed_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_failed_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_failed_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_failed_to_be_modified_item_ies_o_value_c_names[] = {
    {"DRBs-FailedToBeModified-Item", nullptr}};
const asn1::table::choice_desc drbs_failed_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_failed_to_be_modified_item_ies_o::value_c::types>(),
    drbs_failed_to_be_modified_item_ies_o::value_c::types::nof_types,
    drbs_failed_to_be_modified_item_ies_o::value_c::types::nof_exts,
    drbs_failed_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_failed_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_failed_to_be_modified_item_ies_o_value_c_names,
    "drbs_failed_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_failed_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_drbs_failed_to_be_modified_item_ies_o_value_c_types_values[] = {
    "DRBs-FailedToBeModified-Item"};
const asn1::table::enum_desc drbs_failed_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_drbs_failed_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "drbs_failed_to_be_modified_item_ies_o::value_c::types"};

// DRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_failed_to_be_setup_item_ies_o[]    = {14};
static const uint8_t  objset_crit_drbs_failed_to_be_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_drbs_failed_to_be_setup_item_ies_o[] = {
    drbs_failed_to_be_setup_item_ies_o::value_c::types::drbs_failed_to_be_setup_item};
static const uint8_t                    objset_presence_drbs_failed_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_failed_to_be_setup_item_ies_o[]  = {
    {objset_crit_drbs_failed_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_failed_to_be_setup_item_ies_o, drbs_failed_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_failed_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_failed_to_be_setup_item_ies_o::desc =
    {objset_id_drbs_failed_to_be_setup_item_ies_o, 1, "id", objset_columns_drbs_failed_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_failed_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_failed_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_failed_to_be_setup_item_ies_o_value_c_names[] = {
    {"DRBs-FailedToBeSetup-Item", nullptr}};
const asn1::table::choice_desc drbs_failed_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_failed_to_be_setup_item_ies_o::value_c::types>(),
    drbs_failed_to_be_setup_item_ies_o::value_c::types::nof_types,
    drbs_failed_to_be_setup_item_ies_o::value_c::types::nof_exts,
    drbs_failed_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_failed_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_failed_to_be_setup_item_ies_o_value_c_names,
    "drbs_failed_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_failed_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_drbs_failed_to_be_setup_item_ies_o_value_c_types_values[] = {"DRBs-FailedToBeSetup-Item"};
const asn1::table::enum_desc drbs_failed_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_drbs_failed_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "drbs_failed_to_be_setup_item_ies_o::value_c::types"};

// DRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_failed_to_be_setup_mod_item_ies_o[]    = {16};
static const uint8_t  objset_crit_drbs_failed_to_be_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_drbs_failed_to_be_setup_mod_item_ies_o[] = {
    drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::drbs_failed_to_be_setup_mod_item};
static const uint8_t objset_presence_drbs_failed_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_failed_to_be_setup_mod_item_ies_o[] = {
    {objset_crit_drbs_failed_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_failed_to_be_setup_mod_item_ies_o,
     drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_failed_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_failed_to_be_setup_mod_item_ies_o::desc =
    {objset_id_drbs_failed_to_be_setup_mod_item_ies_o, 1, "id", objset_columns_drbs_failed_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_failed_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_failed_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"DRBs-FailedToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc drbs_failed_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_failed_to_be_setup_mod_item_ies_o::value_c::types>(),
    drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_failed_to_be_setup_mod_item_ies_o_value_c_names,
    "drbs_failed_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_failed_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_drbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values[] = {
    "DRBs-FailedToBeSetupMod-Item"};
const asn1::table::enum_desc drbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_drbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "drbs_failed_to_be_setup_mod_item_ies_o::value_c::types"};

// DRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_modified_item_ies_o[]    = {20};
static const uint8_t  objset_crit_drbs_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_drbs_modified_item_ies_o[] = {
    drbs_modified_item_ies_o::value_c::types::drbs_modified_item};
static const uint8_t                    objset_presence_drbs_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_modified_item_ies_o[]  = {
    {objset_crit_drbs_modified_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_modified_item_ies_o, drbs_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_modified_item_ies_o::desc = {objset_id_drbs_modified_item_ies_o,
                                                                 1,
                                                                 "id",
                                                                 objset_columns_drbs_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_modified_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_modified_item_ies_o_value_c_names[] = {
    {"DRBs-Modified-Item", nullptr}};
const asn1::table::choice_desc drbs_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_modified_item_ies_o::value_c::types>(),
    drbs_modified_item_ies_o::value_c::types::nof_types,
    drbs_modified_item_ies_o::value_c::types::nof_exts,
    drbs_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_modified_item_ies_o_value_c_names,
    "drbs_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_drbs_modified_item_ies_o_value_c_types_values[] = {"DRBs-Modified-Item"};
const asn1::table::enum_desc drbs_modified_item_ies_o::value_c::types_opts::desc        = {
    enum_names_drbs_modified_item_ies_o_value_c_types_values,
    1,
    "drbs_modified_item_ies_o::value_c::types"};

// DRBs-ModifiedConf-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_modified_conf_item_ies_o[]    = {18};
static const uint8_t  objset_crit_drbs_modified_conf_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_drbs_modified_conf_item_ies_o[] = {
    drbs_modified_conf_item_ies_o::value_c::types::drbs_modified_conf_item};
static const uint8_t                    objset_presence_drbs_modified_conf_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_modified_conf_item_ies_o[]  = {
    {objset_crit_drbs_modified_conf_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_modified_conf_item_ies_o, drbs_modified_conf_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_modified_conf_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_modified_conf_item_ies_o::desc = {objset_id_drbs_modified_conf_item_ies_o,
                                                                      1,
                                                                      "id",
                                                                      objset_columns_drbs_modified_conf_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_modified_conf_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_modified_conf_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_modified_conf_item_ies_o_value_c_names[] = {
    {"DRBs-ModifiedConf-Item", nullptr}};
const asn1::table::choice_desc drbs_modified_conf_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_modified_conf_item_ies_o::value_c::types>(),
    drbs_modified_conf_item_ies_o::value_c::types::nof_types,
    drbs_modified_conf_item_ies_o::value_c::types::nof_exts,
    drbs_modified_conf_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_modified_conf_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_modified_conf_item_ies_o_value_c_names,
    "drbs_modified_conf_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_modified_conf_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_drbs_modified_conf_item_ies_o_value_c_types_values[]    = {"DRBs-ModifiedConf-Item"};
const asn1::table::enum_desc drbs_modified_conf_item_ies_o::value_c::types_opts::desc = {
    enum_names_drbs_modified_conf_item_ies_o_value_c_types_values,
    1,
    "drbs_modified_conf_item_ies_o::value_c::types"};

// DRBs-Required-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_required_to_be_modified_item_ies_o[]    = {22};
static const uint8_t  objset_crit_drbs_required_to_be_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_drbs_required_to_be_modified_item_ies_o[] = {
    drbs_required_to_be_modified_item_ies_o::value_c::types::drbs_required_to_be_modified_item};
static const uint8_t objset_presence_drbs_required_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_required_to_be_modified_item_ies_o[] = {
    {objset_crit_drbs_required_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_required_to_be_modified_item_ies_o,
     drbs_required_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_required_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_required_to_be_modified_item_ies_o::desc = {
    objset_id_drbs_required_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_drbs_required_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_required_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_required_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_required_to_be_modified_item_ies_o_value_c_names[] = {
    {"DRBs-Required-ToBeModified-Item", nullptr}};
const asn1::table::choice_desc drbs_required_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_required_to_be_modified_item_ies_o::value_c::types>(),
    drbs_required_to_be_modified_item_ies_o::value_c::types::nof_types,
    drbs_required_to_be_modified_item_ies_o::value_c::types::nof_exts,
    drbs_required_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_required_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_required_to_be_modified_item_ies_o_value_c_names,
    "drbs_required_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_required_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_drbs_required_to_be_modified_item_ies_o_value_c_types_values[] = {
    "DRBs-Required-ToBeModified-Item"};
const asn1::table::enum_desc drbs_required_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_drbs_required_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "drbs_required_to_be_modified_item_ies_o::value_c::types"};

// DRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_required_to_be_released_item_ies_o[]    = {24};
static const uint8_t  objset_crit_drbs_required_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_drbs_required_to_be_released_item_ies_o[] = {
    drbs_required_to_be_released_item_ies_o::value_c::types::drbs_required_to_be_released_item};
static const uint8_t objset_presence_drbs_required_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_required_to_be_released_item_ies_o[] = {
    {objset_crit_drbs_required_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_required_to_be_released_item_ies_o,
     drbs_required_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_required_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_required_to_be_released_item_ies_o::desc = {
    objset_id_drbs_required_to_be_released_item_ies_o,
    1,
    "id",
    objset_columns_drbs_required_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_required_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_required_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_required_to_be_released_item_ies_o_value_c_names[] = {
    {"DRBs-Required-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc drbs_required_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_required_to_be_released_item_ies_o::value_c::types>(),
    drbs_required_to_be_released_item_ies_o::value_c::types::nof_types,
    drbs_required_to_be_released_item_ies_o::value_c::types::nof_exts,
    drbs_required_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_required_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_required_to_be_released_item_ies_o_value_c_names,
    "drbs_required_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_required_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_drbs_required_to_be_released_item_ies_o_value_c_types_values[] = {
    "DRBs-Required-ToBeReleased-Item"};
const asn1::table::enum_desc drbs_required_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_drbs_required_to_be_released_item_ies_o_value_c_types_values,
    1,
    "drbs_required_to_be_released_item_ies_o::value_c::types"};

// DRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_setup_item_ies_o[]    = {26};
static const uint8_t  objset_crit_drbs_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_drbs_setup_item_ies_o[] = {drbs_setup_item_ies_o::value_c::types::drbs_setup_item};
static const uint8_t  objset_presence_drbs_setup_item_ies_o[]                  = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_setup_item_ies_o[] = {
    {objset_crit_drbs_setup_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_setup_item_ies_o, drbs_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_setup_item_ies_o::desc = {objset_id_drbs_setup_item_ies_o,
                                                              1,
                                                              "id",
                                                              objset_columns_drbs_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_setup_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_setup_item_ies_o_value_c_names[] = {
    {"DRBs-Setup-Item", nullptr}};
const asn1::table::choice_desc drbs_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_setup_item_ies_o::value_c::types>(),
    drbs_setup_item_ies_o::value_c::types::nof_types,
    drbs_setup_item_ies_o::value_c::types::nof_exts,
    drbs_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_setup_item_ies_o_value_c_names,
    "drbs_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_drbs_setup_item_ies_o_value_c_types_values[] = {"DRBs-Setup-Item"};
const asn1::table::enum_desc drbs_setup_item_ies_o::value_c::types_opts::desc        = {
    enum_names_drbs_setup_item_ies_o_value_c_types_values,
    1,
    "drbs_setup_item_ies_o::value_c::types"};

// DRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_setup_mod_item_ies_o[]    = {28};
static const uint8_t  objset_crit_drbs_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_drbs_setup_mod_item_ies_o[] = {
    drbs_setup_mod_item_ies_o::value_c::types::drbs_setup_mod_item};
static const uint8_t                    objset_presence_drbs_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_setup_mod_item_ies_o[]  = {
    {objset_crit_drbs_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_setup_mod_item_ies_o, drbs_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_setup_mod_item_ies_o::desc = {objset_id_drbs_setup_mod_item_ies_o,
                                                                  1,
                                                                  "id",
                                                                  objset_columns_drbs_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_setup_mod_item_ies_o_value_c_names[] = {
    {"DRBs-SetupMod-Item", nullptr}};
const asn1::table::choice_desc drbs_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_setup_mod_item_ies_o::value_c::types>(),
    drbs_setup_mod_item_ies_o::value_c::types::nof_types,
    drbs_setup_mod_item_ies_o::value_c::types::nof_exts,
    drbs_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_setup_mod_item_ies_o_value_c_names,
    "drbs_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_drbs_setup_mod_item_ies_o_value_c_types_values[] = {"DRBs-SetupMod-Item"};
const asn1::table::enum_desc drbs_setup_mod_item_ies_o::value_c::types_opts::desc        = {
    enum_names_drbs_setup_mod_item_ies_o_value_c_types_values,
    1,
    "drbs_setup_mod_item_ies_o::value_c::types"};

// DRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_to_be_modified_item_ies_o[]    = {30};
static const uint8_t  objset_crit_drbs_to_be_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_drbs_to_be_modified_item_ies_o[] = {
    drbs_to_be_modified_item_ies_o::value_c::types::drbs_to_be_modified_item};
static const uint8_t                    objset_presence_drbs_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_to_be_modified_item_ies_o[]  = {
    {objset_crit_drbs_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_to_be_modified_item_ies_o, drbs_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_to_be_modified_item_ies_o::desc = {objset_id_drbs_to_be_modified_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_drbs_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_to_be_modified_item_ies_o_value_c_names[] = {
    {"DRBs-ToBeModified-Item", nullptr}};
const asn1::table::choice_desc drbs_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_to_be_modified_item_ies_o::value_c::types>(),
    drbs_to_be_modified_item_ies_o::value_c::types::nof_types,
    drbs_to_be_modified_item_ies_o::value_c::types::nof_exts,
    drbs_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_to_be_modified_item_ies_o_value_c_names,
    "drbs_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_drbs_to_be_modified_item_ies_o_value_c_types_values[]    = {"DRBs-ToBeModified-Item"};
const asn1::table::enum_desc drbs_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_drbs_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "drbs_to_be_modified_item_ies_o::value_c::types"};

// DRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_to_be_released_item_ies_o[]    = {32};
static const uint8_t  objset_crit_drbs_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_drbs_to_be_released_item_ies_o[] = {
    drbs_to_be_released_item_ies_o::value_c::types::drbs_to_be_released_item};
static const uint8_t                    objset_presence_drbs_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_to_be_released_item_ies_o[]  = {
    {objset_crit_drbs_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_to_be_released_item_ies_o, drbs_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_to_be_released_item_ies_o::desc = {objset_id_drbs_to_be_released_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_drbs_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_to_be_released_item_ies_o_value_c_names[] = {
    {"DRBs-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc drbs_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_to_be_released_item_ies_o::value_c::types>(),
    drbs_to_be_released_item_ies_o::value_c::types::nof_types,
    drbs_to_be_released_item_ies_o::value_c::types::nof_exts,
    drbs_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_to_be_released_item_ies_o_value_c_names,
    "drbs_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_drbs_to_be_released_item_ies_o_value_c_types_values[]    = {"DRBs-ToBeReleased-Item"};
const asn1::table::enum_desc drbs_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_drbs_to_be_released_item_ies_o_value_c_types_values,
    1,
    "drbs_to_be_released_item_ies_o::value_c::types"};

// DRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_to_be_setup_item_ies_o[]    = {34};
static const uint8_t  objset_crit_drbs_to_be_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_drbs_to_be_setup_item_ies_o[] = {
    drbs_to_be_setup_item_ies_o::value_c::types::drbs_to_be_setup_item};
static const uint8_t                    objset_presence_drbs_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_to_be_setup_item_ies_o[]  = {
    {objset_crit_drbs_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_to_be_setup_item_ies_o, drbs_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_to_be_setup_item_ies_o::desc = {objset_id_drbs_to_be_setup_item_ies_o,
                                                                    1,
                                                                    "id",
                                                                    objset_columns_drbs_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_to_be_setup_item_ies_o_value_c_names[] = {
    {"DRBs-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc drbs_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_to_be_setup_item_ies_o::value_c::types>(),
    drbs_to_be_setup_item_ies_o::value_c::types::nof_types,
    drbs_to_be_setup_item_ies_o::value_c::types::nof_exts,
    drbs_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_to_be_setup_item_ies_o_value_c_names,
    "drbs_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_drbs_to_be_setup_item_ies_o_value_c_types_values[] = {"DRBs-ToBeSetup-Item"};
const asn1::table::enum_desc drbs_to_be_setup_item_ies_o::value_c::types_opts::desc        = {
    enum_names_drbs_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "drbs_to_be_setup_item_ies_o::value_c::types"};

// DRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_drbs_to_be_setup_mod_item_ies_o[]    = {36};
static const uint8_t  objset_crit_drbs_to_be_setup_mod_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_drbs_to_be_setup_mod_item_ies_o[] = {
    drbs_to_be_setup_mod_item_ies_o::value_c::types::drbs_to_be_setup_mod_item};
static const uint8_t                    objset_presence_drbs_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_drbs_to_be_setup_mod_item_ies_o[]  = {
    {objset_crit_drbs_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_drbs_to_be_setup_mod_item_ies_o, drbs_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_drbs_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc drbs_to_be_setup_mod_item_ies_o::desc = {objset_id_drbs_to_be_setup_mod_item_ies_o,
                                                                        1,
                                                                        "id",
                                                                        objset_columns_drbs_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_drbs_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drbs_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_drbs_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"DRBs-ToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc drbs_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<drbs_to_be_setup_mod_item_ies_o::value_c::types>(),
    drbs_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    drbs_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    drbs_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_drbs_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_drbs_to_be_setup_mod_item_ies_o_value_c_names,
    "drbs_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(drbs_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_drbs_to_be_setup_mod_item_ies_o_value_c_types_values[]    = {"DRBs-ToBeSetupMod-Item"};
const asn1::table::enum_desc drbs_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_drbs_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "drbs_to_be_setup_mod_item_ies_o::value_c::types"};

// Dedicated-SIDelivery-NeededUE-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ded_si_delivery_needed_ue_item_ies_o[]    = {190};
static const uint8_t  objset_crit_ded_si_delivery_needed_ue_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ded_si_delivery_needed_ue_item_ies_o[] = {
    ded_si_delivery_needed_ue_item_ies_o::value_c::types::ded_si_delivery_needed_ue_item};
static const uint8_t objset_presence_ded_si_delivery_needed_ue_item_ies_o[]                   = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ded_si_delivery_needed_ue_item_ies_o[] = {
    {objset_crit_ded_si_delivery_needed_ue_item_ies_o, crit_e::nulltype},
    {objset_value_ded_si_delivery_needed_ue_item_ies_o, ded_si_delivery_needed_ue_item_ies_o::value_c::types::nulltype},
    {objset_presence_ded_si_delivery_needed_ue_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ded_si_delivery_needed_ue_item_ies_o::desc =
    {objset_id_ded_si_delivery_needed_ue_item_ies_o, 1, "id", objset_columns_ded_si_delivery_needed_ue_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ded_si_delivery_needed_ue_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ded_si_delivery_needed_ue_item_s::desc}};
static const asn1::table::field_name choice_table_ded_si_delivery_needed_ue_item_ies_o_value_c_names[] = {
    {"Dedicated-SIDelivery-NeededUE-Item", nullptr}};
const asn1::table::choice_desc ded_si_delivery_needed_ue_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ded_si_delivery_needed_ue_item_ies_o::value_c::types>(),
    ded_si_delivery_needed_ue_item_ies_o::value_c::types::nof_types,
    ded_si_delivery_needed_ue_item_ies_o::value_c::types::nof_exts,
    ded_si_delivery_needed_ue_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ded_si_delivery_needed_ue_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ded_si_delivery_needed_ue_item_ies_o_value_c_names,
    "ded_si_delivery_needed_ue_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ded_si_delivery_needed_ue_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ded_si_delivery_needed_ue_item_ies_o_value_c_types_values[] = {
    "Dedicated-SIDelivery-NeededUE-Item"};
const asn1::table::enum_desc ded_si_delivery_needed_ue_item_ies_o::value_c::types_opts::desc = {
    enum_names_ded_si_delivery_needed_ue_item_ies_o_value_c_types_values,
    1,
    "ded_si_delivery_needed_ue_item_ies_o::value_c::types"};

// GNB-DU-Served-Cells-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_gnb_du_served_cells_item_ies_o[]    = {43};
static const uint8_t  objset_crit_gnb_du_served_cells_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_gnb_du_served_cells_item_ies_o[] = {
    gnb_du_served_cells_item_ies_o::value_c::types::gnb_du_served_cells_item};
static const uint8_t                    objset_presence_gnb_du_served_cells_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_gnb_du_served_cells_item_ies_o[]  = {
    {objset_crit_gnb_du_served_cells_item_ies_o, crit_e::nulltype},
    {objset_value_gnb_du_served_cells_item_ies_o, gnb_du_served_cells_item_ies_o::value_c::types::nulltype},
    {objset_presence_gnb_du_served_cells_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc gnb_du_served_cells_item_ies_o::desc = {objset_id_gnb_du_served_cells_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_gnb_du_served_cells_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_gnb_du_served_cells_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_du_served_cells_item_s::desc}};
static const asn1::table::field_name choice_table_gnb_du_served_cells_item_ies_o_value_c_names[] = {
    {"GNB-DU-Served-Cells-Item", nullptr}};
const asn1::table::choice_desc gnb_du_served_cells_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<gnb_du_served_cells_item_ies_o::value_c::types>(),
    gnb_du_served_cells_item_ies_o::value_c::types::nof_types,
    gnb_du_served_cells_item_ies_o::value_c::types::nof_exts,
    gnb_du_served_cells_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_gnb_du_served_cells_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_gnb_du_served_cells_item_ies_o_value_c_names,
    "gnb_du_served_cells_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(gnb_du_served_cells_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_gnb_du_served_cells_item_ies_o_value_c_types_values[]    = {"GNB-DU-Served-Cells-Item"};
const asn1::table::enum_desc gnb_du_served_cells_item_ies_o::value_c::types_opts::desc = {
    enum_names_gnb_du_served_cells_item_ies_o_value_c_types_values,
    1,
    "gnb_du_served_cells_item_ies_o::value_c::types"};

// NCGI-to-be-Updated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ncgi_to_be_upd_list_item_ies_o[]    = {764};
static const uint8_t  objset_crit_ncgi_to_be_upd_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ncgi_to_be_upd_list_item_ies_o[] = {
    ncgi_to_be_upd_list_item_ies_o::value_c::types::ncgi_to_be_upd_list_item};
static const uint8_t                    objset_presence_ncgi_to_be_upd_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ncgi_to_be_upd_list_item_ies_o[]  = {
    {objset_crit_ncgi_to_be_upd_list_item_ies_o, crit_e::nulltype},
    {objset_value_ncgi_to_be_upd_list_item_ies_o, ncgi_to_be_upd_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_ncgi_to_be_upd_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ncgi_to_be_upd_list_item_ies_o::desc = {objset_id_ncgi_to_be_upd_list_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_ncgi_to_be_upd_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ncgi_to_be_upd_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ncgi_to_be_upd_list_item_s::desc}};
static const asn1::table::field_name choice_table_ncgi_to_be_upd_list_item_ies_o_value_c_names[] = {
    {"NCGI-to-be-Updated-List-Item", nullptr}};
const asn1::table::choice_desc ncgi_to_be_upd_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ncgi_to_be_upd_list_item_ies_o::value_c::types>(),
    ncgi_to_be_upd_list_item_ies_o::value_c::types::nof_types,
    ncgi_to_be_upd_list_item_ies_o::value_c::types::nof_exts,
    ncgi_to_be_upd_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ncgi_to_be_upd_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ncgi_to_be_upd_list_item_ies_o_value_c_names,
    "ncgi_to_be_upd_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ncgi_to_be_upd_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ncgi_to_be_upd_list_item_ies_o_value_c_types_values[] = {"NCGI-to-be-Updated-List-Item"};
const asn1::table::enum_desc ncgi_to_be_upd_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_ncgi_to_be_upd_list_item_ies_o_value_c_types_values,
    1,
    "ncgi_to_be_upd_list_item_ies_o::value_c::types"};

// GNB-CU-TNL-Association-Failed-To-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o[]    = {135};
static const uint8_t  objset_crit_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o[] = {
    gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types::gnb_cu_tnl_assoc_failed_to_setup_item};
static const uint8_t objset_presence_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o[] = {
    {objset_crit_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o, crit_e::nulltype},
    {objset_value_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o,
     gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::desc = {
    objset_id_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o,
    1,
    "id",
    objset_columns_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_cu_tnl_assoc_failed_to_setup_item_s::desc}};
static const asn1::table::field_name choice_table_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o_value_c_names[] = {
    {"GNB-CU-TNL-Association-Failed-To-Setup-Item", nullptr}};
const asn1::table::choice_desc gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types>(),
    gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types::nof_types,
    gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types::nof_exts,
    gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o_value_c_names,
    "gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o_value_c_types_values[] = {
    "GNB-CU-TNL-Association-Failed-To-Setup-Item"};
const asn1::table::enum_desc gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_gnb_cu_tnl_assoc_failed_to_setup_item_ies_o_value_c_types_values,
    1,
    "gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types"};

// GNB-CU-TNL-Association-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_gnb_cu_tnl_assoc_setup_item_ies_o[]    = {133};
static const uint8_t  objset_crit_gnb_cu_tnl_assoc_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_gnb_cu_tnl_assoc_setup_item_ies_o[] = {
    gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types::gnb_cu_tnl_assoc_setup_item};
static const uint8_t                    objset_presence_gnb_cu_tnl_assoc_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_gnb_cu_tnl_assoc_setup_item_ies_o[]  = {
    {objset_crit_gnb_cu_tnl_assoc_setup_item_ies_o, crit_e::nulltype},
    {objset_value_gnb_cu_tnl_assoc_setup_item_ies_o, gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_gnb_cu_tnl_assoc_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc gnb_cu_tnl_assoc_setup_item_ies_o::desc =
    {objset_id_gnb_cu_tnl_assoc_setup_item_ies_o, 1, "id", objset_columns_gnb_cu_tnl_assoc_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_gnb_cu_tnl_assoc_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_cu_tnl_assoc_setup_item_s::desc}};
static const asn1::table::field_name choice_table_gnb_cu_tnl_assoc_setup_item_ies_o_value_c_names[] = {
    {"GNB-CU-TNL-Association-Setup-Item", nullptr}};
const asn1::table::choice_desc gnb_cu_tnl_assoc_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types>(),
    gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types::nof_types,
    gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types::nof_exts,
    gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_gnb_cu_tnl_assoc_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_gnb_cu_tnl_assoc_setup_item_ies_o_value_c_names,
    "gnb_cu_tnl_assoc_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(gnb_cu_tnl_assoc_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_gnb_cu_tnl_assoc_setup_item_ies_o_value_c_types_values[] = {
    "GNB-CU-TNL-Association-Setup-Item"};
const asn1::table::enum_desc gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_gnb_cu_tnl_assoc_setup_item_ies_o_value_c_types_values,
    1,
    "gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types"};

// GNB-CU-TNL-Association-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_gnb_cu_tnl_assoc_to_add_item_ies_o[]    = {120};
static const uint8_t  objset_crit_gnb_cu_tnl_assoc_to_add_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_gnb_cu_tnl_assoc_to_add_item_ies_o[] = {
    gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types::gnb_cu_tnl_assoc_to_add_item};
static const uint8_t                    objset_presence_gnb_cu_tnl_assoc_to_add_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_gnb_cu_tnl_assoc_to_add_item_ies_o[]  = {
    {objset_crit_gnb_cu_tnl_assoc_to_add_item_ies_o, crit_e::nulltype},
    {objset_value_gnb_cu_tnl_assoc_to_add_item_ies_o, gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types::nulltype},
    {objset_presence_gnb_cu_tnl_assoc_to_add_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc gnb_cu_tnl_assoc_to_add_item_ies_o::desc =
    {objset_id_gnb_cu_tnl_assoc_to_add_item_ies_o, 1, "id", objset_columns_gnb_cu_tnl_assoc_to_add_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_gnb_cu_tnl_assoc_to_add_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_cu_tnl_assoc_to_add_item_s::desc}};
static const asn1::table::field_name choice_table_gnb_cu_tnl_assoc_to_add_item_ies_o_value_c_names[] = {
    {"GNB-CU-TNL-Association-To-Add-Item", nullptr}};
const asn1::table::choice_desc gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types>(),
    gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types::nof_types,
    gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types::nof_exts,
    gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_gnb_cu_tnl_assoc_to_add_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_gnb_cu_tnl_assoc_to_add_item_ies_o_value_c_names,
    "gnb_cu_tnl_assoc_to_add_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(gnb_cu_tnl_assoc_to_add_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_gnb_cu_tnl_assoc_to_add_item_ies_o_value_c_types_values[] = {
    "GNB-CU-TNL-Association-To-Add-Item"};
const asn1::table::enum_desc gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types_opts::desc = {
    enum_names_gnb_cu_tnl_assoc_to_add_item_ies_o_value_c_types_values,
    1,
    "gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types"};

// GNB-CU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_gnb_cu_tnl_assoc_to_rem_item_ies_o[]    = {122};
static const uint8_t  objset_crit_gnb_cu_tnl_assoc_to_rem_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_gnb_cu_tnl_assoc_to_rem_item_ies_o[] = {
    gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types::gnb_cu_tnl_assoc_to_rem_item};
static const uint8_t                    objset_presence_gnb_cu_tnl_assoc_to_rem_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_gnb_cu_tnl_assoc_to_rem_item_ies_o[]  = {
    {objset_crit_gnb_cu_tnl_assoc_to_rem_item_ies_o, crit_e::nulltype},
    {objset_value_gnb_cu_tnl_assoc_to_rem_item_ies_o, gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types::nulltype},
    {objset_presence_gnb_cu_tnl_assoc_to_rem_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc gnb_cu_tnl_assoc_to_rem_item_ies_o::desc =
    {objset_id_gnb_cu_tnl_assoc_to_rem_item_ies_o, 1, "id", objset_columns_gnb_cu_tnl_assoc_to_rem_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_gnb_cu_tnl_assoc_to_rem_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_cu_tnl_assoc_to_rem_item_s::desc}};
static const asn1::table::field_name choice_table_gnb_cu_tnl_assoc_to_rem_item_ies_o_value_c_names[] = {
    {"GNB-CU-TNL-Association-To-Remove-Item", nullptr}};
const asn1::table::choice_desc gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types>(),
    gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types::nof_types,
    gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types::nof_exts,
    gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_gnb_cu_tnl_assoc_to_rem_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_gnb_cu_tnl_assoc_to_rem_item_ies_o_value_c_names,
    "gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_gnb_cu_tnl_assoc_to_rem_item_ies_o_value_c_types_values[] = {
    "GNB-CU-TNL-Association-To-Remove-Item"};
const asn1::table::enum_desc gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types_opts::desc = {
    enum_names_gnb_cu_tnl_assoc_to_rem_item_ies_o_value_c_types_values,
    1,
    "gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types"};

// GNB-CU-TNL-Association-To-Update-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_gnb_cu_tnl_assoc_to_upd_item_ies_o[]    = {124};
static const uint8_t  objset_crit_gnb_cu_tnl_assoc_to_upd_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_gnb_cu_tnl_assoc_to_upd_item_ies_o[] = {
    gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types::gnb_cu_tnl_assoc_to_upd_item};
static const uint8_t                    objset_presence_gnb_cu_tnl_assoc_to_upd_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_gnb_cu_tnl_assoc_to_upd_item_ies_o[]  = {
    {objset_crit_gnb_cu_tnl_assoc_to_upd_item_ies_o, crit_e::nulltype},
    {objset_value_gnb_cu_tnl_assoc_to_upd_item_ies_o, gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types::nulltype},
    {objset_presence_gnb_cu_tnl_assoc_to_upd_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc gnb_cu_tnl_assoc_to_upd_item_ies_o::desc =
    {objset_id_gnb_cu_tnl_assoc_to_upd_item_ies_o, 1, "id", objset_columns_gnb_cu_tnl_assoc_to_upd_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_gnb_cu_tnl_assoc_to_upd_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_cu_tnl_assoc_to_upd_item_s::desc}};
static const asn1::table::field_name choice_table_gnb_cu_tnl_assoc_to_upd_item_ies_o_value_c_names[] = {
    {"GNB-CU-TNL-Association-To-Update-Item", nullptr}};
const asn1::table::choice_desc gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types>(),
    gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types::nof_types,
    gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types::nof_exts,
    gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_gnb_cu_tnl_assoc_to_upd_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_gnb_cu_tnl_assoc_to_upd_item_ies_o_value_c_names,
    "gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_gnb_cu_tnl_assoc_to_upd_item_ies_o_value_c_types_values[] = {
    "GNB-CU-TNL-Association-To-Update-Item"};
const asn1::table::enum_desc gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types_opts::desc = {
    enum_names_gnb_cu_tnl_assoc_to_upd_item_ies_o_value_c_types_values,
    1,
    "gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types"};

// GNB-DU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_gnb_du_tnl_assoc_to_rem_item_ies_o[]    = {227};
static const uint8_t  objset_crit_gnb_du_tnl_assoc_to_rem_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_gnb_du_tnl_assoc_to_rem_item_ies_o[] = {
    gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types::gnb_du_tnl_assoc_to_rem_item};
static const uint8_t                    objset_presence_gnb_du_tnl_assoc_to_rem_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_gnb_du_tnl_assoc_to_rem_item_ies_o[]  = {
    {objset_crit_gnb_du_tnl_assoc_to_rem_item_ies_o, crit_e::nulltype},
    {objset_value_gnb_du_tnl_assoc_to_rem_item_ies_o, gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types::nulltype},
    {objset_presence_gnb_du_tnl_assoc_to_rem_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc gnb_du_tnl_assoc_to_rem_item_ies_o::desc =
    {objset_id_gnb_du_tnl_assoc_to_rem_item_ies_o, 1, "id", objset_columns_gnb_du_tnl_assoc_to_rem_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_gnb_du_tnl_assoc_to_rem_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_du_tnl_assoc_to_rem_item_s::desc}};
static const asn1::table::field_name choice_table_gnb_du_tnl_assoc_to_rem_item_ies_o_value_c_names[] = {
    {"GNB-DU-TNL-Association-To-Remove-Item", nullptr}};
const asn1::table::choice_desc gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types>(),
    gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types::nof_types,
    gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types::nof_exts,
    gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_gnb_du_tnl_assoc_to_rem_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_gnb_du_tnl_assoc_to_rem_item_ies_o_value_c_names,
    "gnb_du_tnl_assoc_to_rem_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(gnb_du_tnl_assoc_to_rem_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_gnb_du_tnl_assoc_to_rem_item_ies_o_value_c_types_values[] = {
    "GNB-DU-TNL-Association-To-Remove-Item"};
const asn1::table::enum_desc gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types_opts::desc = {
    enum_names_gnb_du_tnl_assoc_to_rem_item_ies_o_value_c_types_values,
    1,
    "gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types"};

// Protected-EUTRA-Resources-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_protected_eutra_res_item_ies_o[]    = {168};
static const uint8_t  objset_crit_protected_eutra_res_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_protected_eutra_res_item_ies_o[] = {
    protected_eutra_res_item_ies_o::value_c::types::protected_eutra_res_item};
static const uint8_t                    objset_presence_protected_eutra_res_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_protected_eutra_res_item_ies_o[]  = {
    {objset_crit_protected_eutra_res_item_ies_o, crit_e::nulltype},
    {objset_value_protected_eutra_res_item_ies_o, protected_eutra_res_item_ies_o::value_c::types::nulltype},
    {objset_presence_protected_eutra_res_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc protected_eutra_res_item_ies_o::desc = {objset_id_protected_eutra_res_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_protected_eutra_res_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_protected_eutra_res_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &protected_eutra_res_item_s::desc}};
static const asn1::table::field_name choice_table_protected_eutra_res_item_ies_o_value_c_names[] = {
    {"Protected-EUTRA-Resources-Item", nullptr}};
const asn1::table::choice_desc protected_eutra_res_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<protected_eutra_res_item_ies_o::value_c::types>(),
    protected_eutra_res_item_ies_o::value_c::types::nof_types,
    protected_eutra_res_item_ies_o::value_c::types::nof_exts,
    protected_eutra_res_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_protected_eutra_res_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_protected_eutra_res_item_ies_o_value_c_names,
    "protected_eutra_res_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(protected_eutra_res_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_protected_eutra_res_item_ies_o_value_c_types_values[] = {
    "Protected-EUTRA-Resources-Item"};
const asn1::table::enum_desc protected_eutra_res_item_ies_o::value_c::types_opts::desc = {
    enum_names_protected_eutra_res_item_ies_o_value_c_types_values,
    1,
    "protected_eutra_res_item_ies_o::value_c::types"};

// Neighbour-Cell-Information-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_neighbour_cell_info_item_ies_o[]    = {255};
static const uint8_t  objset_crit_neighbour_cell_info_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_neighbour_cell_info_item_ies_o[] = {
    neighbour_cell_info_item_ies_o::value_c::types::neighbour_cell_info_item};
static const uint8_t                    objset_presence_neighbour_cell_info_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_neighbour_cell_info_item_ies_o[]  = {
    {objset_crit_neighbour_cell_info_item_ies_o, crit_e::nulltype},
    {objset_value_neighbour_cell_info_item_ies_o, neighbour_cell_info_item_ies_o::value_c::types::nulltype},
    {objset_presence_neighbour_cell_info_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc neighbour_cell_info_item_ies_o::desc = {objset_id_neighbour_cell_info_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_neighbour_cell_info_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_neighbour_cell_info_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &neighbour_cell_info_item_s::desc}};
static const asn1::table::field_name choice_table_neighbour_cell_info_item_ies_o_value_c_names[] = {
    {"Neighbour-Cell-Information-Item", nullptr}};
const asn1::table::choice_desc neighbour_cell_info_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<neighbour_cell_info_item_ies_o::value_c::types>(),
    neighbour_cell_info_item_ies_o::value_c::types::nof_types,
    neighbour_cell_info_item_ies_o::value_c::types::nof_exts,
    neighbour_cell_info_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_neighbour_cell_info_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_neighbour_cell_info_item_ies_o_value_c_names,
    "neighbour_cell_info_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(neighbour_cell_info_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_neighbour_cell_info_item_ies_o_value_c_types_values[] = {
    "Neighbour-Cell-Information-Item"};
const asn1::table::enum_desc neighbour_cell_info_item_ies_o::value_c::types_opts::desc = {
    enum_names_neighbour_cell_info_item_ies_o_value_c_types_values,
    1,
    "neighbour_cell_info_item_ies_o::value_c::types"};

// Served-Cells-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_served_cells_to_add_item_ies_o[]    = {57};
static const uint8_t  objset_crit_served_cells_to_add_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_served_cells_to_add_item_ies_o[] = {
    served_cells_to_add_item_ies_o::value_c::types::served_cells_to_add_item};
static const uint8_t                    objset_presence_served_cells_to_add_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_served_cells_to_add_item_ies_o[]  = {
    {objset_crit_served_cells_to_add_item_ies_o, crit_e::nulltype},
    {objset_value_served_cells_to_add_item_ies_o, served_cells_to_add_item_ies_o::value_c::types::nulltype},
    {objset_presence_served_cells_to_add_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc served_cells_to_add_item_ies_o::desc = {objset_id_served_cells_to_add_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_served_cells_to_add_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_served_cells_to_add_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &served_cells_to_add_item_s::desc}};
static const asn1::table::field_name choice_table_served_cells_to_add_item_ies_o_value_c_names[] = {
    {"Served-Cells-To-Add-Item", nullptr}};
const asn1::table::choice_desc served_cells_to_add_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<served_cells_to_add_item_ies_o::value_c::types>(),
    served_cells_to_add_item_ies_o::value_c::types::nof_types,
    served_cells_to_add_item_ies_o::value_c::types::nof_exts,
    served_cells_to_add_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_served_cells_to_add_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_served_cells_to_add_item_ies_o_value_c_names,
    "served_cells_to_add_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(served_cells_to_add_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_served_cells_to_add_item_ies_o_value_c_types_values[]    = {"Served-Cells-To-Add-Item"};
const asn1::table::enum_desc served_cells_to_add_item_ies_o::value_c::types_opts::desc = {
    enum_names_served_cells_to_add_item_ies_o_value_c_types_values,
    1,
    "served_cells_to_add_item_ies_o::value_c::types"};

// Served-Cells-To-Modify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_served_cells_to_modify_item_ies_o[]    = {61};
static const uint8_t  objset_crit_served_cells_to_modify_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_served_cells_to_modify_item_ies_o[] = {
    served_cells_to_modify_item_ies_o::value_c::types::served_cells_to_modify_item};
static const uint8_t                    objset_presence_served_cells_to_modify_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_served_cells_to_modify_item_ies_o[]  = {
    {objset_crit_served_cells_to_modify_item_ies_o, crit_e::nulltype},
    {objset_value_served_cells_to_modify_item_ies_o, served_cells_to_modify_item_ies_o::value_c::types::nulltype},
    {objset_presence_served_cells_to_modify_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc served_cells_to_modify_item_ies_o::desc =
    {objset_id_served_cells_to_modify_item_ies_o, 1, "id", objset_columns_served_cells_to_modify_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_served_cells_to_modify_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &served_cells_to_modify_item_s::desc}};
static const asn1::table::field_name choice_table_served_cells_to_modify_item_ies_o_value_c_names[] = {
    {"Served-Cells-To-Modify-Item", nullptr}};
const asn1::table::choice_desc served_cells_to_modify_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<served_cells_to_modify_item_ies_o::value_c::types>(),
    served_cells_to_modify_item_ies_o::value_c::types::nof_types,
    served_cells_to_modify_item_ies_o::value_c::types::nof_exts,
    served_cells_to_modify_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_served_cells_to_modify_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_served_cells_to_modify_item_ies_o_value_c_names,
    "served_cells_to_modify_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(served_cells_to_modify_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_served_cells_to_modify_item_ies_o_value_c_types_values[] = {
    "Served-Cells-To-Modify-Item"};
const asn1::table::enum_desc served_cells_to_modify_item_ies_o::value_c::types_opts::desc = {
    enum_names_served_cells_to_modify_item_ies_o_value_c_types_values,
    1,
    "served_cells_to_modify_item_ies_o::value_c::types"};

// Served-Cells-To-Delete-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_served_cells_to_delete_item_ies_o[]    = {59};
static const uint8_t  objset_crit_served_cells_to_delete_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_served_cells_to_delete_item_ies_o[] = {
    served_cells_to_delete_item_ies_o::value_c::types::served_cells_to_delete_item};
static const uint8_t                    objset_presence_served_cells_to_delete_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_served_cells_to_delete_item_ies_o[]  = {
    {objset_crit_served_cells_to_delete_item_ies_o, crit_e::nulltype},
    {objset_value_served_cells_to_delete_item_ies_o, served_cells_to_delete_item_ies_o::value_c::types::nulltype},
    {objset_presence_served_cells_to_delete_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc served_cells_to_delete_item_ies_o::desc =
    {objset_id_served_cells_to_delete_item_ies_o, 1, "id", objset_columns_served_cells_to_delete_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_served_cells_to_delete_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &served_cells_to_delete_item_s::desc}};
static const asn1::table::field_name choice_table_served_cells_to_delete_item_ies_o_value_c_names[] = {
    {"Served-Cells-To-Delete-Item", nullptr}};
const asn1::table::choice_desc served_cells_to_delete_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<served_cells_to_delete_item_ies_o::value_c::types>(),
    served_cells_to_delete_item_ies_o::value_c::types::nof_types,
    served_cells_to_delete_item_ies_o::value_c::types::nof_exts,
    served_cells_to_delete_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_served_cells_to_delete_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_served_cells_to_delete_item_ies_o_value_c_names,
    "served_cells_to_delete_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(served_cells_to_delete_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_served_cells_to_delete_item_ies_o_value_c_types_values[] = {
    "Served-Cells-To-Delete-Item"};
const asn1::table::enum_desc served_cells_to_delete_item_ies_o::value_c::types_opts::desc = {
    enum_names_served_cells_to_delete_item_ies_o_value_c_types_values,
    1,
    "served_cells_to_delete_item_ies_o::value_c::types"};

// IAB-Allocated-TNL-Address-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_iab_allocated_tnl_address_list_item_ies_o[]    = {295};
static const uint8_t  objset_crit_iab_allocated_tnl_address_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_iab_allocated_tnl_address_list_item_ies_o[] = {
    iab_allocated_tnl_address_list_item_ies_o::value_c::types::iab_allocated_tnl_address_item};
static const uint8_t objset_presence_iab_allocated_tnl_address_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_iab_allocated_tnl_address_list_item_ies_o[] = {
    {objset_crit_iab_allocated_tnl_address_list_item_ies_o, crit_e::nulltype},
    {objset_value_iab_allocated_tnl_address_list_item_ies_o,
     iab_allocated_tnl_address_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_iab_allocated_tnl_address_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc iab_allocated_tnl_address_list_item_ies_o::desc = {
    objset_id_iab_allocated_tnl_address_list_item_ies_o,
    1,
    "id",
    objset_columns_iab_allocated_tnl_address_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_iab_allocated_tnl_address_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &iab_allocated_tnl_address_item_s::desc}};
static const asn1::table::field_name choice_table_iab_allocated_tnl_address_list_item_ies_o_value_c_names[] = {
    {"IAB-Allocated-TNL-Address-Item", nullptr}};
const asn1::table::choice_desc iab_allocated_tnl_address_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<iab_allocated_tnl_address_list_item_ies_o::value_c::types>(),
    iab_allocated_tnl_address_list_item_ies_o::value_c::types::nof_types,
    iab_allocated_tnl_address_list_item_ies_o::value_c::types::nof_exts,
    iab_allocated_tnl_address_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_iab_allocated_tnl_address_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_iab_allocated_tnl_address_list_item_ies_o_value_c_names,
    "iab_allocated_tnl_address_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(iab_allocated_tnl_address_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_iab_allocated_tnl_address_list_item_ies_o_value_c_types_values[] = {
    "IAB-Allocated-TNL-Address-Item"};
const asn1::table::enum_desc iab_allocated_tnl_address_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_iab_allocated_tnl_address_list_item_ies_o_value_c_types_values,
    1,
    "iab_allocated_tnl_address_list_item_ies_o::value_c::types"};

// IAB-TNL-Addresses-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_iab_tnl_addresses_to_rem_item_ies_o[]    = {293};
static const uint8_t  objset_crit_iab_tnl_addresses_to_rem_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_iab_tnl_addresses_to_rem_item_ies_o[] = {
    iab_tnl_addresses_to_rem_item_ies_o::value_c::types::iab_tnl_addresses_to_rem_item};
static const uint8_t                    objset_presence_iab_tnl_addresses_to_rem_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_iab_tnl_addresses_to_rem_item_ies_o[]  = {
    {objset_crit_iab_tnl_addresses_to_rem_item_ies_o, crit_e::nulltype},
    {objset_value_iab_tnl_addresses_to_rem_item_ies_o, iab_tnl_addresses_to_rem_item_ies_o::value_c::types::nulltype},
    {objset_presence_iab_tnl_addresses_to_rem_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc iab_tnl_addresses_to_rem_item_ies_o::desc =
    {objset_id_iab_tnl_addresses_to_rem_item_ies_o, 1, "id", objset_columns_iab_tnl_addresses_to_rem_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_iab_tnl_addresses_to_rem_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &iab_tnl_addresses_to_rem_item_s::desc}};
static const asn1::table::field_name choice_table_iab_tnl_addresses_to_rem_item_ies_o_value_c_names[] = {
    {"IAB-TNL-Addresses-To-Remove-Item", nullptr}};
const asn1::table::choice_desc iab_tnl_addresses_to_rem_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<iab_tnl_addresses_to_rem_item_ies_o::value_c::types>(),
    iab_tnl_addresses_to_rem_item_ies_o::value_c::types::nof_types,
    iab_tnl_addresses_to_rem_item_ies_o::value_c::types::nof_exts,
    iab_tnl_addresses_to_rem_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_iab_tnl_addresses_to_rem_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_iab_tnl_addresses_to_rem_item_ies_o_value_c_names,
    "iab_tnl_addresses_to_rem_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(iab_tnl_addresses_to_rem_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_iab_tnl_addresses_to_rem_item_ies_o_value_c_types_values[] = {
    "IAB-TNL-Addresses-To-Remove-Item"};
const asn1::table::enum_desc iab_tnl_addresses_to_rem_item_ies_o::value_c::types_opts::desc = {
    enum_names_iab_tnl_addresses_to_rem_item_ies_o_value_c_types_values,
    1,
    "iab_tnl_addresses_to_rem_item_ies_o::value_c::types"};

// UL-UP-TNL-Information-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ul_up_tnl_info_to_upd_list_item_ies_o[]    = {301};
static const uint8_t  objset_crit_ul_up_tnl_info_to_upd_list_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ul_up_tnl_info_to_upd_list_item_ies_o[] = {
    ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types::ul_up_tnl_info_to_upd_list_item};
static const uint8_t objset_presence_ul_up_tnl_info_to_upd_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ul_up_tnl_info_to_upd_list_item_ies_o[] = {
    {objset_crit_ul_up_tnl_info_to_upd_list_item_ies_o, crit_e::nulltype},
    {objset_value_ul_up_tnl_info_to_upd_list_item_ies_o,
     ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_ul_up_tnl_info_to_upd_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ul_up_tnl_info_to_upd_list_item_ies_o::desc =
    {objset_id_ul_up_tnl_info_to_upd_list_item_ies_o, 1, "id", objset_columns_ul_up_tnl_info_to_upd_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ul_up_tnl_info_to_upd_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_up_tnl_info_to_upd_list_item_s::desc}};
static const asn1::table::field_name choice_table_ul_up_tnl_info_to_upd_list_item_ies_o_value_c_names[] = {
    {"UL-UP-TNL-Information-to-Update-List-Item", nullptr}};
const asn1::table::choice_desc ul_up_tnl_info_to_upd_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types>(),
    ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types::nof_types,
    ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types::nof_exts,
    ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ul_up_tnl_info_to_upd_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ul_up_tnl_info_to_upd_list_item_ies_o_value_c_names,
    "ul_up_tnl_info_to_upd_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ul_up_tnl_info_to_upd_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ul_up_tnl_info_to_upd_list_item_ies_o_value_c_types_values[] = {
    "UL-UP-TNL-Information-to-Update-List-Item"};
const asn1::table::enum_desc ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_ul_up_tnl_info_to_upd_list_item_ies_o_value_c_types_values,
    1,
    "ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types"};

// UL-UP-TNL-Address-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ul_up_tnl_address_to_upd_list_item_ies_o[]    = {303};
static const uint8_t  objset_crit_ul_up_tnl_address_to_upd_list_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ul_up_tnl_address_to_upd_list_item_ies_o[] = {
    ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types::ul_up_tnl_address_to_upd_list_item};
static const uint8_t objset_presence_ul_up_tnl_address_to_upd_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ul_up_tnl_address_to_upd_list_item_ies_o[] = {
    {objset_crit_ul_up_tnl_address_to_upd_list_item_ies_o, crit_e::nulltype},
    {objset_value_ul_up_tnl_address_to_upd_list_item_ies_o,
     ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_ul_up_tnl_address_to_upd_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ul_up_tnl_address_to_upd_list_item_ies_o::desc = {
    objset_id_ul_up_tnl_address_to_upd_list_item_ies_o,
    1,
    "id",
    objset_columns_ul_up_tnl_address_to_upd_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ul_up_tnl_address_to_upd_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_up_tnl_address_to_upd_list_item_s::desc}};
static const asn1::table::field_name choice_table_ul_up_tnl_address_to_upd_list_item_ies_o_value_c_names[] = {
    {"UL-UP-TNL-Address-to-Update-List-Item", nullptr}};
const asn1::table::choice_desc ul_up_tnl_address_to_upd_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types>(),
    ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types::nof_types,
    ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types::nof_exts,
    ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ul_up_tnl_address_to_upd_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ul_up_tnl_address_to_upd_list_item_ies_o_value_c_names,
    "ul_up_tnl_address_to_upd_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ul_up_tnl_address_to_upd_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ul_up_tnl_address_to_upd_list_item_ies_o_value_c_types_values[] = {
    "UL-UP-TNL-Address-to-Update-List-Item"};
const asn1::table::enum_desc ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_ul_up_tnl_address_to_upd_list_item_ies_o_value_c_types_values,
    1,
    "ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types"};

// MC-PagingCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_mc_paging_cell_item_ies_o[]    = {688};
static const uint8_t  objset_crit_mc_paging_cell_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_mc_paging_cell_item_ies_o[] = {
    mc_paging_cell_item_ies_o::value_c::types::mc_paging_cell_item};
static const uint8_t                    objset_presence_mc_paging_cell_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_mc_paging_cell_item_ies_o[]  = {
    {objset_crit_mc_paging_cell_item_ies_o, crit_e::nulltype},
    {objset_value_mc_paging_cell_item_ies_o, mc_paging_cell_item_ies_o::value_c::types::nulltype},
    {objset_presence_mc_paging_cell_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc mc_paging_cell_item_ies_o::desc = {objset_id_mc_paging_cell_item_ies_o,
                                                                  1,
                                                                  "id",
                                                                  objset_columns_mc_paging_cell_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_mc_paging_cell_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_paging_cell_item_s::desc}};
static const asn1::table::field_name choice_table_mc_paging_cell_item_ies_o_value_c_names[] = {
    {"MC-PagingCell-Item", nullptr}};
const asn1::table::choice_desc mc_paging_cell_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<mc_paging_cell_item_ies_o::value_c::types>(),
    mc_paging_cell_item_ies_o::value_c::types::nof_types,
    mc_paging_cell_item_ies_o::value_c::types::nof_exts,
    mc_paging_cell_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_mc_paging_cell_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_mc_paging_cell_item_ies_o_value_c_names,
    "mc_paging_cell_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(mc_paging_cell_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_mc_paging_cell_item_ies_o_value_c_types_values[] = {"MC-PagingCell-Item"};
const asn1::table::enum_desc mc_paging_cell_item_ies_o::value_c::types_opts::desc        = {
    enum_names_mc_paging_cell_item_ies_o_value_c_types_values,
    1,
    "mc_paging_cell_item_ies_o::value_c::types"};

// MulticastMRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_to_be_setup_mod_item_ies_o[]    = {501};
static const uint8_t  objset_crit_multicast_m_rbs_to_be_setup_mod_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_multicast_m_rbs_to_be_setup_mod_item_ies_o[] = {
    multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::multicast_m_rbs_to_be_setup_mod_item};
static const uint8_t objset_presence_multicast_m_rbs_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_to_be_setup_mod_item_ies_o[] = {
    {objset_crit_multicast_m_rbs_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_to_be_setup_mod_item_ies_o,
     multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_to_be_setup_mod_item_ies_o::desc = {
    objset_id_multicast_m_rbs_to_be_setup_mod_item_ies_o,
    1,
    "id",
    objset_columns_multicast_m_rbs_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_m_rbs_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_m_rbs_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-ToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types>(),
    multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_to_be_setup_mod_item_ies_o_value_c_names,
    "multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_to_be_setup_mod_item_ies_o_value_c_types_values[] = {
    "MulticastMRBs-ToBeSetupMod-Item"};
const asn1::table::enum_desc multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types"};

// MulticastMRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_to_be_modified_item_ies_o[]    = {495};
static const uint8_t  objset_crit_multicast_m_rbs_to_be_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_multicast_m_rbs_to_be_modified_item_ies_o[] = {
    multicast_m_rbs_to_be_modified_item_ies_o::value_c::types::multicast_m_rbs_to_be_modified_item};
static const uint8_t objset_presence_multicast_m_rbs_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_to_be_modified_item_ies_o[] = {
    {objset_crit_multicast_m_rbs_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_to_be_modified_item_ies_o,
     multicast_m_rbs_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_to_be_modified_item_ies_o::desc = {
    objset_id_multicast_m_rbs_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_multicast_m_rbs_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_m_rbs_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_m_rbs_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_to_be_modified_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-ToBeModified-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_to_be_modified_item_ies_o::value_c::types>(),
    multicast_m_rbs_to_be_modified_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_to_be_modified_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_to_be_modified_item_ies_o_value_c_names,
    "multicast_m_rbs_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_to_be_modified_item_ies_o_value_c_types_values[] = {
    "MulticastMRBs-ToBeModified-Item"};
const asn1::table::enum_desc multicast_m_rbs_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_to_be_modified_item_ies_o::value_c::types"};

// MulticastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_to_be_released_item_ies_o[]    = {497};
static const uint8_t  objset_crit_multicast_m_rbs_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_multicast_m_rbs_to_be_released_item_ies_o[] = {
    multicast_m_rbs_to_be_released_item_ies_o::value_c::types::multicast_m_rbs_to_be_released_item};
static const uint8_t objset_presence_multicast_m_rbs_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_to_be_released_item_ies_o[] = {
    {objset_crit_multicast_m_rbs_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_to_be_released_item_ies_o,
     multicast_m_rbs_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_to_be_released_item_ies_o::desc = {
    objset_id_multicast_m_rbs_to_be_released_item_ies_o,
    1,
    "id",
    objset_columns_multicast_m_rbs_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_m_rbs_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_m_rbs_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_to_be_released_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_to_be_released_item_ies_o::value_c::types>(),
    multicast_m_rbs_to_be_released_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_to_be_released_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_to_be_released_item_ies_o_value_c_names,
    "multicast_m_rbs_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_to_be_released_item_ies_o_value_c_types_values[] = {
    "MulticastMRBs-ToBeReleased-Item"};
const asn1::table::enum_desc multicast_m_rbs_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_to_be_released_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_to_be_released_item_ies_o::value_c::types"};

// MulticastMRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_setup_mod_item_ies_o[]    = {493};
static const uint8_t  objset_crit_multicast_m_rbs_setup_mod_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_multicast_m_rbs_setup_mod_item_ies_o[] = {
    multicast_m_rbs_setup_mod_item_ies_o::value_c::types::multicast_m_rbs_setup_mod_item};
static const uint8_t objset_presence_multicast_m_rbs_setup_mod_item_ies_o[]                   = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_setup_mod_item_ies_o[] = {
    {objset_crit_multicast_m_rbs_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_setup_mod_item_ies_o, multicast_m_rbs_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_setup_mod_item_ies_o::desc =
    {objset_id_multicast_m_rbs_setup_mod_item_ies_o, 1, "id", objset_columns_multicast_m_rbs_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_m_rbs_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_m_rbs_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_setup_mod_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-SetupMod-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_setup_mod_item_ies_o::value_c::types>(),
    multicast_m_rbs_setup_mod_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_setup_mod_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_setup_mod_item_ies_o_value_c_names,
    "multicast_m_rbs_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_setup_mod_item_ies_o_value_c_types_values[] = {
    "MulticastMRBs-SetupMod-Item"};
const asn1::table::enum_desc multicast_m_rbs_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_setup_mod_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_setup_mod_item_ies_o::value_c::types"};

// MulticastMRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o[]    = {487};
static const uint8_t  objset_crit_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o[] = {
    multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::multicast_m_rbs_failed_to_be_setup_mod_item};
static const uint8_t objset_presence_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o[] = {
    {objset_crit_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o,
     multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::desc = {
    objset_id_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o,
    1,
    "id",
    objset_columns_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &multicast_m_rbs_failed_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-FailedToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types>(),
    multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_names,
    "multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values[] = {
    "MulticastMRBs-FailedToBeSetupMod-Item"};
const asn1::table::enum_desc multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types"};

// MulticastMRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_modified_item_ies_o[]    = {489};
static const uint8_t  objset_crit_multicast_m_rbs_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_multicast_m_rbs_modified_item_ies_o[] = {
    multicast_m_rbs_modified_item_ies_o::value_c::types::multicast_m_rbs_modified_item};
static const uint8_t                    objset_presence_multicast_m_rbs_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_modified_item_ies_o[]  = {
    {objset_crit_multicast_m_rbs_modified_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_modified_item_ies_o, multicast_m_rbs_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_modified_item_ies_o::desc =
    {objset_id_multicast_m_rbs_modified_item_ies_o, 1, "id", objset_columns_multicast_m_rbs_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_m_rbs_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_m_rbs_modified_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_modified_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-Modified-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_modified_item_ies_o::value_c::types>(),
    multicast_m_rbs_modified_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_modified_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_modified_item_ies_o_value_c_names,
    "multicast_m_rbs_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_modified_item_ies_o_value_c_types_values[] = {
    "MulticastMRBs-Modified-Item"};
const asn1::table::enum_desc multicast_m_rbs_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_modified_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_modified_item_ies_o::value_c::types"};

// MulticastMRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_failed_to_be_modified_item_ies_o[]    = {483};
static const uint8_t  objset_crit_multicast_m_rbs_failed_to_be_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_multicast_m_rbs_failed_to_be_modified_item_ies_o[] = {
    multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::multicast_m_rbs_failed_to_be_modified_item};
static const uint8_t objset_presence_multicast_m_rbs_failed_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_failed_to_be_modified_item_ies_o[] = {
    {objset_crit_multicast_m_rbs_failed_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_failed_to_be_modified_item_ies_o,
     multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_failed_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_failed_to_be_modified_item_ies_o::desc = {
    objset_id_multicast_m_rbs_failed_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_multicast_m_rbs_failed_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_multicast_m_rbs_failed_to_be_modified_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &multicast_m_rbs_failed_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_failed_to_be_modified_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-FailedToBeModified-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types>(),
    multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_failed_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_failed_to_be_modified_item_ies_o_value_c_names,
    "multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_failed_to_be_modified_item_ies_o_value_c_types_values[] = {
    "MulticastMRBs-FailedToBeModified-Item"};
const asn1::table::enum_desc multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_failed_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types"};

// MulticastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_to_be_setup_item_ies_o[]    = {499};
static const uint8_t  objset_crit_multicast_m_rbs_to_be_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_multicast_m_rbs_to_be_setup_item_ies_o[] = {
    multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::multicast_m_rbs_to_be_setup_item};
static const uint8_t objset_presence_multicast_m_rbs_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_to_be_setup_item_ies_o[] = {
    {objset_crit_multicast_m_rbs_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_to_be_setup_item_ies_o,
     multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_to_be_setup_item_ies_o::desc =
    {objset_id_multicast_m_rbs_to_be_setup_item_ies_o, 1, "id", objset_columns_multicast_m_rbs_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_m_rbs_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_m_rbs_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_to_be_setup_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_to_be_setup_item_ies_o::value_c::types>(),
    multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_to_be_setup_item_ies_o_value_c_names,
    "multicast_m_rbs_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_to_be_setup_item_ies_o_value_c_types_values[] = {
    "MulticastMRBs-ToBeSetup-Item"};
const asn1::table::enum_desc multicast_m_rbs_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_to_be_setup_item_ies_o::value_c::types"};

// MulticastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_setup_item_ies_o[]    = {491};
static const uint8_t  objset_crit_multicast_m_rbs_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_multicast_m_rbs_setup_item_ies_o[] = {
    multicast_m_rbs_setup_item_ies_o::value_c::types::multicast_m_rbs_setup_item};
static const uint8_t                    objset_presence_multicast_m_rbs_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_setup_item_ies_o[]  = {
    {objset_crit_multicast_m_rbs_setup_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_setup_item_ies_o, multicast_m_rbs_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_setup_item_ies_o::desc =
    {objset_id_multicast_m_rbs_setup_item_ies_o, 1, "id", objset_columns_multicast_m_rbs_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_m_rbs_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_m_rbs_setup_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_setup_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-Setup-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_setup_item_ies_o::value_c::types>(),
    multicast_m_rbs_setup_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_setup_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_setup_item_ies_o_value_c_names,
    "multicast_m_rbs_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_setup_item_ies_o_value_c_types_values[]    = {"MulticastMRBs-Setup-Item"};
const asn1::table::enum_desc multicast_m_rbs_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_setup_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_setup_item_ies_o::value_c::types"};

// MulticastMRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_m_rbs_failed_to_be_setup_item_ies_o[]    = {485};
static const uint8_t  objset_crit_multicast_m_rbs_failed_to_be_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_multicast_m_rbs_failed_to_be_setup_item_ies_o[] = {
    multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::multicast_m_rbs_failed_to_be_setup_item};
static const uint8_t objset_presence_multicast_m_rbs_failed_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_m_rbs_failed_to_be_setup_item_ies_o[] = {
    {objset_crit_multicast_m_rbs_failed_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_m_rbs_failed_to_be_setup_item_ies_o,
     multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_m_rbs_failed_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_m_rbs_failed_to_be_setup_item_ies_o::desc = {
    objset_id_multicast_m_rbs_failed_to_be_setup_item_ies_o,
    1,
    "id",
    objset_columns_multicast_m_rbs_failed_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_m_rbs_failed_to_be_setup_item_ies_o_value_c_alternatives[] =
    {{0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &multicast_m_rbs_failed_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_m_rbs_failed_to_be_setup_item_ies_o_value_c_names[] = {
    {"MulticastMRBs-FailedToBeSetup-Item", nullptr}};
const asn1::table::choice_desc multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types>(),
    multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::nof_types,
    multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::nof_exts,
    multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_m_rbs_failed_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_m_rbs_failed_to_be_setup_item_ies_o_value_c_names,
    "multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_m_rbs_failed_to_be_setup_item_ies_o_value_c_types_values[] = {
    "MulticastMRBs-FailedToBeSetup-Item"};
const asn1::table::enum_desc multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_m_rbs_failed_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types"};

// MulticastF1UContext-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_f1_u_context_to_be_setup_item_ies_o[]    = {504};
static const uint8_t  objset_crit_multicast_f1_u_context_to_be_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_multicast_f1_u_context_to_be_setup_item_ies_o[] = {
    multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types::multicast_f1_u_context_to_be_setup_item};
static const uint8_t objset_presence_multicast_f1_u_context_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_f1_u_context_to_be_setup_item_ies_o[] = {
    {objset_crit_multicast_f1_u_context_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_f1_u_context_to_be_setup_item_ies_o,
     multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_f1_u_context_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_f1_u_context_to_be_setup_item_ies_o::desc = {
    objset_id_multicast_f1_u_context_to_be_setup_item_ies_o,
    1,
    "id",
    objset_columns_multicast_f1_u_context_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_f1_u_context_to_be_setup_item_ies_o_value_c_alternatives[] =
    {{0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &multicast_f1_u_context_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_f1_u_context_to_be_setup_item_ies_o_value_c_names[] = {
    {"MulticastF1UContext-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc multicast_f1_u_context_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types>(),
    multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types::nof_types,
    multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types::nof_exts,
    multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_f1_u_context_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_f1_u_context_to_be_setup_item_ies_o_value_c_names,
    "multicast_f1_u_context_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_f1_u_context_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_f1_u_context_to_be_setup_item_ies_o_value_c_types_values[] = {
    "MulticastF1UContext-ToBeSetup-Item"};
const asn1::table::enum_desc multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_f1_u_context_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types"};
uint8_t multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types");
}

// MulticastF1UContext-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_f1_u_context_setup_item_ies_o[]    = {506};
static const uint8_t  objset_crit_multicast_f1_u_context_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_multicast_f1_u_context_setup_item_ies_o[] = {
    multicast_f1_u_context_setup_item_ies_o::value_c::types::multicast_f1_u_context_setup_item};
static const uint8_t objset_presence_multicast_f1_u_context_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_f1_u_context_setup_item_ies_o[] = {
    {objset_crit_multicast_f1_u_context_setup_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_f1_u_context_setup_item_ies_o,
     multicast_f1_u_context_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_f1_u_context_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_f1_u_context_setup_item_ies_o::desc = {
    objset_id_multicast_f1_u_context_setup_item_ies_o,
    1,
    "id",
    objset_columns_multicast_f1_u_context_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_multicast_f1_u_context_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_f1_u_context_setup_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_f1_u_context_setup_item_ies_o_value_c_names[] = {
    {"MulticastF1UContext-Setup-Item", nullptr}};
const asn1::table::choice_desc multicast_f1_u_context_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_f1_u_context_setup_item_ies_o::value_c::types>(),
    multicast_f1_u_context_setup_item_ies_o::value_c::types::nof_types,
    multicast_f1_u_context_setup_item_ies_o::value_c::types::nof_exts,
    multicast_f1_u_context_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_f1_u_context_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_f1_u_context_setup_item_ies_o_value_c_names,
    "multicast_f1_u_context_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_f1_u_context_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_f1_u_context_setup_item_ies_o_value_c_types_values[] = {
    "MulticastF1UContext-Setup-Item"};
const asn1::table::enum_desc multicast_f1_u_context_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_f1_u_context_setup_item_ies_o_value_c_types_values,
    1,
    "multicast_f1_u_context_setup_item_ies_o::value_c::types"};
uint8_t multicast_f1_u_context_setup_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "multicast_f1_u_context_setup_item_ies_o::value_c::types");
}

// MulticastF1UContext-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_multicast_f1_u_context_failed_to_be_setup_item_ies_o[]    = {508};
static const uint8_t  objset_crit_multicast_f1_u_context_failed_to_be_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_multicast_f1_u_context_failed_to_be_setup_item_ies_o[] = {
    multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types::
        multicast_f1_u_context_failed_to_be_setup_item};
static const uint8_t objset_presence_multicast_f1_u_context_failed_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_multicast_f1_u_context_failed_to_be_setup_item_ies_o[] = {
    {objset_crit_multicast_f1_u_context_failed_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_multicast_f1_u_context_failed_to_be_setup_item_ies_o,
     multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_multicast_f1_u_context_failed_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc multicast_f1_u_context_failed_to_be_setup_item_ies_o::desc = {
    objset_id_multicast_f1_u_context_failed_to_be_setup_item_ies_o,
    1,
    "id",
    objset_columns_multicast_f1_u_context_failed_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_multicast_f1_u_context_failed_to_be_setup_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &multicast_f1_u_context_failed_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_multicast_f1_u_context_failed_to_be_setup_item_ies_o_value_c_names[] =
    {{"MulticastF1UContext-FailedToBeSetup-Item", nullptr}};
const asn1::table::choice_desc multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types>(),
    multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types::nof_types,
    multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types::nof_exts,
    multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_multicast_f1_u_context_failed_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_multicast_f1_u_context_failed_to_be_setup_item_ies_o_value_c_names,
    "multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_multicast_f1_u_context_failed_to_be_setup_item_ies_o_value_c_types_values[] = {
    "MulticastF1UContext-FailedToBeSetup-Item"};
const asn1::table::enum_desc multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_multicast_f1_u_context_failed_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types"};
uint8_t multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types");
}

// UEIdentity-List-For-Paging-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_id_list_for_paging_item_ies_o[]    = {480};
static const uint8_t  objset_crit_ue_id_list_for_paging_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ue_id_list_for_paging_item_ies_o[] = {
    ue_id_list_for_paging_item_ies_o::value_c::types::ue_id_list_for_paging_item};
static const uint8_t                    objset_presence_ue_id_list_for_paging_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_id_list_for_paging_item_ies_o[]  = {
    {objset_crit_ue_id_list_for_paging_item_ies_o, crit_e::nulltype},
    {objset_value_ue_id_list_for_paging_item_ies_o, ue_id_list_for_paging_item_ies_o::value_c::types::nulltype},
    {objset_presence_ue_id_list_for_paging_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_id_list_for_paging_item_ies_o::desc =
    {objset_id_ue_id_list_for_paging_item_ies_o, 1, "id", objset_columns_ue_id_list_for_paging_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ue_id_list_for_paging_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_list_for_paging_item_s::desc}};
static const asn1::table::field_name choice_table_ue_id_list_for_paging_item_ies_o_value_c_names[] = {
    {"UEIdentity-List-For-Paging-Item", nullptr}};
const asn1::table::choice_desc ue_id_list_for_paging_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ue_id_list_for_paging_item_ies_o::value_c::types>(),
    ue_id_list_for_paging_item_ies_o::value_c::types::nof_types,
    ue_id_list_for_paging_item_ies_o::value_c::types::nof_exts,
    ue_id_list_for_paging_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ue_id_list_for_paging_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ue_id_list_for_paging_item_ies_o_value_c_names,
    "ue_id_list_for_paging_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_id_list_for_paging_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_id_list_for_paging_item_ies_o_value_c_types_values[] = {
    "UEIdentity-List-For-Paging-Item"};
const asn1::table::enum_desc ue_id_list_for_paging_item_ies_o::value_c::types_opts::desc = {
    enum_names_ue_id_list_for_paging_item_ies_o_value_c_types_values,
    1,
    "ue_id_list_for_paging_item_ies_o::value_c::types"};

// NR-CGI-List-For-Restart-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_nr_cgi_list_for_restart_list_item_ies_o[]    = {153};
static const uint8_t  objset_crit_nr_cgi_list_for_restart_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_nr_cgi_list_for_restart_list_item_ies_o[] = {
    nr_cgi_list_for_restart_list_item_ies_o::value_c::types::nr_cgi_list_for_restart_item};
static const uint8_t objset_presence_nr_cgi_list_for_restart_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_nr_cgi_list_for_restart_list_item_ies_o[] = {
    {objset_crit_nr_cgi_list_for_restart_list_item_ies_o, crit_e::nulltype},
    {objset_value_nr_cgi_list_for_restart_list_item_ies_o,
     nr_cgi_list_for_restart_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_nr_cgi_list_for_restart_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc nr_cgi_list_for_restart_list_item_ies_o::desc = {
    objset_id_nr_cgi_list_for_restart_list_item_ies_o,
    1,
    "id",
    objset_columns_nr_cgi_list_for_restart_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_nr_cgi_list_for_restart_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_cgi_list_for_restart_item_s::desc}};
static const asn1::table::field_name choice_table_nr_cgi_list_for_restart_list_item_ies_o_value_c_names[] = {
    {"NR-CGI-List-For-Restart-Item", nullptr}};
const asn1::table::choice_desc nr_cgi_list_for_restart_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<nr_cgi_list_for_restart_list_item_ies_o::value_c::types>(),
    nr_cgi_list_for_restart_list_item_ies_o::value_c::types::nof_types,
    nr_cgi_list_for_restart_list_item_ies_o::value_c::types::nof_exts,
    nr_cgi_list_for_restart_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_nr_cgi_list_for_restart_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_nr_cgi_list_for_restart_list_item_ies_o_value_c_names,
    "nr_cgi_list_for_restart_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(nr_cgi_list_for_restart_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_nr_cgi_list_for_restart_list_item_ies_o_value_c_types_values[] = {
    "NR-CGI-List-For-Restart-Item"};
const asn1::table::enum_desc nr_cgi_list_for_restart_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_nr_cgi_list_for_restart_list_item_ies_o_value_c_types_values,
    1,
    "nr_cgi_list_for_restart_list_item_ies_o::value_c::types"};

// PWS-Failed-NR-CGI-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_pws_failed_nr_cgi_list_item_ies_o[]    = {155};
static const uint8_t  objset_crit_pws_failed_nr_cgi_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_pws_failed_nr_cgi_list_item_ies_o[] = {
    pws_failed_nr_cgi_list_item_ies_o::value_c::types::pws_failed_nr_cgi_item};
static const uint8_t                    objset_presence_pws_failed_nr_cgi_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_pws_failed_nr_cgi_list_item_ies_o[]  = {
    {objset_crit_pws_failed_nr_cgi_list_item_ies_o, crit_e::nulltype},
    {objset_value_pws_failed_nr_cgi_list_item_ies_o, pws_failed_nr_cgi_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_pws_failed_nr_cgi_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc pws_failed_nr_cgi_list_item_ies_o::desc =
    {objset_id_pws_failed_nr_cgi_list_item_ies_o, 1, "id", objset_columns_pws_failed_nr_cgi_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_pws_failed_nr_cgi_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pws_failed_nr_cgi_item_s::desc}};
static const asn1::table::field_name choice_table_pws_failed_nr_cgi_list_item_ies_o_value_c_names[] = {
    {"PWS-Failed-NR-CGI-Item", nullptr}};
const asn1::table::choice_desc pws_failed_nr_cgi_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<pws_failed_nr_cgi_list_item_ies_o::value_c::types>(),
    pws_failed_nr_cgi_list_item_ies_o::value_c::types::nof_types,
    pws_failed_nr_cgi_list_item_ies_o::value_c::types::nof_exts,
    pws_failed_nr_cgi_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_pws_failed_nr_cgi_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_pws_failed_nr_cgi_list_item_ies_o_value_c_names,
    "pws_failed_nr_cgi_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(pws_failed_nr_cgi_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_pws_failed_nr_cgi_list_item_ies_o_value_c_types_values[]    = {"PWS-Failed-NR-CGI-Item"};
const asn1::table::enum_desc pws_failed_nr_cgi_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_pws_failed_nr_cgi_list_item_ies_o_value_c_types_values,
    1,
    "pws_failed_nr_cgi_list_item_ies_o::value_c::types"};

// PagingCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_paging_cell_item_ies_o[]   = {112};
static const uint8_t  objset_crit_paging_cell_item_ies_o[] = {crit_e::ignore};
static const uint8_t objset_value_paging_cell_item_ies_o[] = {paging_cell_item_ies_o::value_c::types::paging_cell_item};
static const uint8_t objset_presence_paging_cell_item_ies_o[]                   = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_paging_cell_item_ies_o[] = {
    {objset_crit_paging_cell_item_ies_o, crit_e::nulltype},
    {objset_value_paging_cell_item_ies_o, paging_cell_item_ies_o::value_c::types::nulltype},
    {objset_presence_paging_cell_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc paging_cell_item_ies_o::desc = {objset_id_paging_cell_item_ies_o,
                                                               1,
                                                               "id",
                                                               objset_columns_paging_cell_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_paging_cell_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &paging_cell_item_s::desc}};
static const asn1::table::field_name choice_table_paging_cell_item_ies_o_value_c_names[] = {
    {"PagingCell-Item", nullptr}};
const asn1::table::choice_desc paging_cell_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<paging_cell_item_ies_o::value_c::types>(),
    paging_cell_item_ies_o::value_c::types::nof_types,
    paging_cell_item_ies_o::value_c::types::nof_exts,
    paging_cell_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_paging_cell_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_paging_cell_item_ies_o_value_c_names,
    "paging_cell_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(paging_cell_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_paging_cell_item_ies_o_value_c_types_values[] = {"PagingCell-Item"};
const asn1::table::enum_desc paging_cell_item_ies_o::value_c::types_opts::desc        = {
    enum_names_paging_cell_item_ies_o_value_c_types_values,
    1,
    "paging_cell_item_ies_o::value_c::types"};

// Potential-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_potential_sp_cell_item_ies_o[]    = {93};
static const uint8_t  objset_crit_potential_sp_cell_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_potential_sp_cell_item_ies_o[] = {
    potential_sp_cell_item_ies_o::value_c::types::potential_sp_cell_item};
static const uint8_t                    objset_presence_potential_sp_cell_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_potential_sp_cell_item_ies_o[]  = {
    {objset_crit_potential_sp_cell_item_ies_o, crit_e::nulltype},
    {objset_value_potential_sp_cell_item_ies_o, potential_sp_cell_item_ies_o::value_c::types::nulltype},
    {objset_presence_potential_sp_cell_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc potential_sp_cell_item_ies_o::desc = {objset_id_potential_sp_cell_item_ies_o,
                                                                     1,
                                                                     "id",
                                                                     objset_columns_potential_sp_cell_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_potential_sp_cell_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &potential_sp_cell_item_s::desc}};
static const asn1::table::field_name choice_table_potential_sp_cell_item_ies_o_value_c_names[] = {
    {"Potential-SpCell-Item", nullptr}};
const asn1::table::choice_desc potential_sp_cell_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<potential_sp_cell_item_ies_o::value_c::types>(),
    potential_sp_cell_item_ies_o::value_c::types::nof_types,
    potential_sp_cell_item_ies_o::value_c::types::nof_exts,
    potential_sp_cell_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_potential_sp_cell_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_potential_sp_cell_item_ies_o_value_c_names,
    "potential_sp_cell_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(potential_sp_cell_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_potential_sp_cell_item_ies_o_value_c_types_values[] = {"Potential-SpCell-Item"};
const asn1::table::enum_desc potential_sp_cell_item_ies_o::value_c::types_opts::desc        = {
    enum_names_potential_sp_cell_item_ies_o_value_c_types_values,
    1,
    "potential_sp_cell_item_ies_o::value_c::types"};

// SCell-FailedtoSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_scell_failedto_setup_item_ies_o[]    = {84};
static const uint8_t  objset_crit_scell_failedto_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_scell_failedto_setup_item_ies_o[] = {
    scell_failedto_setup_item_ies_o::value_c::types::scell_failedto_setup_item};
static const uint8_t                    objset_presence_scell_failedto_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_scell_failedto_setup_item_ies_o[]  = {
    {objset_crit_scell_failedto_setup_item_ies_o, crit_e::nulltype},
    {objset_value_scell_failedto_setup_item_ies_o, scell_failedto_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_scell_failedto_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc scell_failedto_setup_item_ies_o::desc = {objset_id_scell_failedto_setup_item_ies_o,
                                                                        1,
                                                                        "id",
                                                                        objset_columns_scell_failedto_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_scell_failedto_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &scell_failedto_setup_item_s::desc}};
static const asn1::table::field_name choice_table_scell_failedto_setup_item_ies_o_value_c_names[] = {
    {"SCell-FailedtoSetup-Item", nullptr}};
const asn1::table::choice_desc scell_failedto_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<scell_failedto_setup_item_ies_o::value_c::types>(),
    scell_failedto_setup_item_ies_o::value_c::types::nof_types,
    scell_failedto_setup_item_ies_o::value_c::types::nof_exts,
    scell_failedto_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_scell_failedto_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_scell_failedto_setup_item_ies_o_value_c_names,
    "scell_failedto_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(scell_failedto_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_scell_failedto_setup_item_ies_o_value_c_types_values[]    = {"SCell-FailedtoSetup-Item"};
const asn1::table::enum_desc scell_failedto_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_scell_failedto_setup_item_ies_o_value_c_types_values,
    1,
    "scell_failedto_setup_item_ies_o::value_c::types"};

// SCell-FailedtoSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_scell_failedto_setup_mod_item_ies_o[]    = {86};
static const uint8_t  objset_crit_scell_failedto_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_scell_failedto_setup_mod_item_ies_o[] = {
    scell_failedto_setup_mod_item_ies_o::value_c::types::scell_failedto_setup_mod_item};
static const uint8_t                    objset_presence_scell_failedto_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_scell_failedto_setup_mod_item_ies_o[]  = {
    {objset_crit_scell_failedto_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_scell_failedto_setup_mod_item_ies_o, scell_failedto_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_scell_failedto_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc scell_failedto_setup_mod_item_ies_o::desc =
    {objset_id_scell_failedto_setup_mod_item_ies_o, 1, "id", objset_columns_scell_failedto_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_scell_failedto_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &scell_failedto_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_scell_failedto_setup_mod_item_ies_o_value_c_names[] = {
    {"SCell-FailedtoSetupMod-Item", nullptr}};
const asn1::table::choice_desc scell_failedto_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<scell_failedto_setup_mod_item_ies_o::value_c::types>(),
    scell_failedto_setup_mod_item_ies_o::value_c::types::nof_types,
    scell_failedto_setup_mod_item_ies_o::value_c::types::nof_exts,
    scell_failedto_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_scell_failedto_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_scell_failedto_setup_mod_item_ies_o_value_c_names,
    "scell_failedto_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(scell_failedto_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_scell_failedto_setup_mod_item_ies_o_value_c_types_values[] = {
    "SCell-FailedtoSetupMod-Item"};
const asn1::table::enum_desc scell_failedto_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_scell_failedto_setup_mod_item_ies_o_value_c_types_values,
    1,
    "scell_failedto_setup_mod_item_ies_o::value_c::types"};

// SCell-ToBeRemoved-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_scell_to_be_remd_item_ies_o[]    = {51};
static const uint8_t  objset_crit_scell_to_be_remd_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_scell_to_be_remd_item_ies_o[] = {
    scell_to_be_remd_item_ies_o::value_c::types::scell_to_be_remd_item};
static const uint8_t                    objset_presence_scell_to_be_remd_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_scell_to_be_remd_item_ies_o[]  = {
    {objset_crit_scell_to_be_remd_item_ies_o, crit_e::nulltype},
    {objset_value_scell_to_be_remd_item_ies_o, scell_to_be_remd_item_ies_o::value_c::types::nulltype},
    {objset_presence_scell_to_be_remd_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc scell_to_be_remd_item_ies_o::desc = {objset_id_scell_to_be_remd_item_ies_o,
                                                                    1,
                                                                    "id",
                                                                    objset_columns_scell_to_be_remd_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_scell_to_be_remd_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &scell_to_be_remd_item_s::desc}};
static const asn1::table::field_name choice_table_scell_to_be_remd_item_ies_o_value_c_names[] = {
    {"SCell-ToBeRemoved-Item", nullptr}};
const asn1::table::choice_desc scell_to_be_remd_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<scell_to_be_remd_item_ies_o::value_c::types>(),
    scell_to_be_remd_item_ies_o::value_c::types::nof_types,
    scell_to_be_remd_item_ies_o::value_c::types::nof_exts,
    scell_to_be_remd_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_scell_to_be_remd_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_scell_to_be_remd_item_ies_o_value_c_names,
    "scell_to_be_remd_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(scell_to_be_remd_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_scell_to_be_remd_item_ies_o_value_c_types_values[] = {"SCell-ToBeRemoved-Item"};
const asn1::table::enum_desc scell_to_be_remd_item_ies_o::value_c::types_opts::desc        = {
    enum_names_scell_to_be_remd_item_ies_o_value_c_types_values,
    1,
    "scell_to_be_remd_item_ies_o::value_c::types"};

// SCell-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_scell_to_be_setup_item_ies_o[]    = {53};
static const uint8_t  objset_crit_scell_to_be_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_scell_to_be_setup_item_ies_o[] = {
    scell_to_be_setup_item_ies_o::value_c::types::scell_to_be_setup_item};
static const uint8_t                    objset_presence_scell_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_scell_to_be_setup_item_ies_o[]  = {
    {objset_crit_scell_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_scell_to_be_setup_item_ies_o, scell_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_scell_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc scell_to_be_setup_item_ies_o::desc = {objset_id_scell_to_be_setup_item_ies_o,
                                                                     1,
                                                                     "id",
                                                                     objset_columns_scell_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_scell_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &scell_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_scell_to_be_setup_item_ies_o_value_c_names[] = {
    {"SCell-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc scell_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<scell_to_be_setup_item_ies_o::value_c::types>(),
    scell_to_be_setup_item_ies_o::value_c::types::nof_types,
    scell_to_be_setup_item_ies_o::value_c::types::nof_exts,
    scell_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_scell_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_scell_to_be_setup_item_ies_o_value_c_names,
    "scell_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(scell_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_scell_to_be_setup_item_ies_o_value_c_types_values[] = {"SCell-ToBeSetup-Item"};
const asn1::table::enum_desc scell_to_be_setup_item_ies_o::value_c::types_opts::desc        = {
    enum_names_scell_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "scell_to_be_setup_item_ies_o::value_c::types"};

// SCell-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_scell_to_be_setup_mod_item_ies_o[]    = {55};
static const uint8_t  objset_crit_scell_to_be_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_scell_to_be_setup_mod_item_ies_o[] = {
    scell_to_be_setup_mod_item_ies_o::value_c::types::scell_to_be_setup_mod_item};
static const uint8_t                    objset_presence_scell_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_scell_to_be_setup_mod_item_ies_o[]  = {
    {objset_crit_scell_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_scell_to_be_setup_mod_item_ies_o, scell_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_scell_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc scell_to_be_setup_mod_item_ies_o::desc =
    {objset_id_scell_to_be_setup_mod_item_ies_o, 1, "id", objset_columns_scell_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_scell_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &scell_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_scell_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"SCell-ToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc scell_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<scell_to_be_setup_mod_item_ies_o::value_c::types>(),
    scell_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    scell_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    scell_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_scell_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_scell_to_be_setup_mod_item_ies_o_value_c_names,
    "scell_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(scell_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_scell_to_be_setup_mod_item_ies_o_value_c_types_values[]    = {"SCell-ToBeSetupMod-Item"};
const asn1::table::enum_desc scell_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_scell_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "scell_to_be_setup_mod_item_ies_o::value_c::types"};

// SLDRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_failed_to_be_modified_item_ies_o[]    = {313};
static const uint8_t  objset_crit_sl_drbs_failed_to_be_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_sl_drbs_failed_to_be_modified_item_ies_o[] = {
    sl_drbs_failed_to_be_modified_item_ies_o::value_c::types::sl_drbs_failed_to_be_modified_item};
static const uint8_t objset_presence_sl_drbs_failed_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_failed_to_be_modified_item_ies_o[] = {
    {objset_crit_sl_drbs_failed_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_failed_to_be_modified_item_ies_o,
     sl_drbs_failed_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_failed_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_failed_to_be_modified_item_ies_o::desc = {
    objset_id_sl_drbs_failed_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_sl_drbs_failed_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_failed_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_failed_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_failed_to_be_modified_item_ies_o_value_c_names[] = {
    {"SLDRBs-FailedToBeModified-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_failed_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_failed_to_be_modified_item_ies_o::value_c::types>(),
    sl_drbs_failed_to_be_modified_item_ies_o::value_c::types::nof_types,
    sl_drbs_failed_to_be_modified_item_ies_o::value_c::types::nof_exts,
    sl_drbs_failed_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_failed_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_failed_to_be_modified_item_ies_o_value_c_names,
    "sl_drbs_failed_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_failed_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_failed_to_be_modified_item_ies_o_value_c_types_values[] = {
    "SLDRBs-FailedToBeModified-Item"};
const asn1::table::enum_desc sl_drbs_failed_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_failed_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_failed_to_be_modified_item_ies_o::value_c::types"};

// SLDRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_failed_to_be_setup_item_ies_o[]    = {315};
static const uint8_t  objset_crit_sl_drbs_failed_to_be_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_sl_drbs_failed_to_be_setup_item_ies_o[] = {
    sl_drbs_failed_to_be_setup_item_ies_o::value_c::types::sl_drbs_failed_to_be_setup_item};
static const uint8_t objset_presence_sl_drbs_failed_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_failed_to_be_setup_item_ies_o[] = {
    {objset_crit_sl_drbs_failed_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_failed_to_be_setup_item_ies_o,
     sl_drbs_failed_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_failed_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_failed_to_be_setup_item_ies_o::desc =
    {objset_id_sl_drbs_failed_to_be_setup_item_ies_o, 1, "id", objset_columns_sl_drbs_failed_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_failed_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_failed_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_failed_to_be_setup_item_ies_o_value_c_names[] = {
    {"SLDRBs-FailedToBeSetup-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_failed_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_failed_to_be_setup_item_ies_o::value_c::types>(),
    sl_drbs_failed_to_be_setup_item_ies_o::value_c::types::nof_types,
    sl_drbs_failed_to_be_setup_item_ies_o::value_c::types::nof_exts,
    sl_drbs_failed_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_failed_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_failed_to_be_setup_item_ies_o_value_c_names,
    "sl_drbs_failed_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_failed_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_failed_to_be_setup_item_ies_o_value_c_types_values[] = {
    "SLDRBs-FailedToBeSetup-Item"};
const asn1::table::enum_desc sl_drbs_failed_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_failed_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_failed_to_be_setup_item_ies_o::value_c::types"};

// SLDRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_failed_to_be_setup_mod_item_ies_o[]    = {336};
static const uint8_t  objset_crit_sl_drbs_failed_to_be_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_sl_drbs_failed_to_be_setup_mod_item_ies_o[] = {
    sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::sl_drbs_failed_to_be_setup_mod_item};
static const uint8_t objset_presence_sl_drbs_failed_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_failed_to_be_setup_mod_item_ies_o[] = {
    {objset_crit_sl_drbs_failed_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_failed_to_be_setup_mod_item_ies_o,
     sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_failed_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_failed_to_be_setup_mod_item_ies_o::desc = {
    objset_id_sl_drbs_failed_to_be_setup_mod_item_ies_o,
    1,
    "id",
    objset_columns_sl_drbs_failed_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_failed_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_failed_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"SLDRBs-FailedToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types>(),
    sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_failed_to_be_setup_mod_item_ies_o_value_c_names,
    "sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values[] = {
    "SLDRBs-FailedToBeSetupMod-Item"};
const asn1::table::enum_desc sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types"};

// SLDRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_modified_item_ies_o[]    = {317};
static const uint8_t  objset_crit_sl_drbs_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_sl_drbs_modified_item_ies_o[] = {
    sl_drbs_modified_item_ies_o::value_c::types::sl_drbs_modified_item};
static const uint8_t                    objset_presence_sl_drbs_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_modified_item_ies_o[]  = {
    {objset_crit_sl_drbs_modified_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_modified_item_ies_o, sl_drbs_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_modified_item_ies_o::desc = {objset_id_sl_drbs_modified_item_ies_o,
                                                                    1,
                                                                    "id",
                                                                    objset_columns_sl_drbs_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_modified_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_modified_item_ies_o_value_c_names[] = {
    {"SLDRBs-Modified-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_modified_item_ies_o::value_c::types>(),
    sl_drbs_modified_item_ies_o::value_c::types::nof_types,
    sl_drbs_modified_item_ies_o::value_c::types::nof_exts,
    sl_drbs_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_modified_item_ies_o_value_c_names,
    "sl_drbs_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_sl_drbs_modified_item_ies_o_value_c_types_values[] = {"SLDRBs-Modified-Item"};
const asn1::table::enum_desc sl_drbs_modified_item_ies_o::value_c::types_opts::desc        = {
    enum_names_sl_drbs_modified_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_modified_item_ies_o::value_c::types"};

// SLDRBs-ModifiedConf-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_modified_conf_item_ies_o[]    = {338};
static const uint8_t  objset_crit_sl_drbs_modified_conf_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_sl_drbs_modified_conf_item_ies_o[] = {
    sl_drbs_modified_conf_item_ies_o::value_c::types::sl_drbs_modified_conf_item};
static const uint8_t                    objset_presence_sl_drbs_modified_conf_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_modified_conf_item_ies_o[]  = {
    {objset_crit_sl_drbs_modified_conf_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_modified_conf_item_ies_o, sl_drbs_modified_conf_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_modified_conf_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_modified_conf_item_ies_o::desc =
    {objset_id_sl_drbs_modified_conf_item_ies_o, 1, "id", objset_columns_sl_drbs_modified_conf_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_modified_conf_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_modified_conf_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_modified_conf_item_ies_o_value_c_names[] = {
    {"SLDRBs-ModifiedConf-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_modified_conf_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_modified_conf_item_ies_o::value_c::types>(),
    sl_drbs_modified_conf_item_ies_o::value_c::types::nof_types,
    sl_drbs_modified_conf_item_ies_o::value_c::types::nof_exts,
    sl_drbs_modified_conf_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_modified_conf_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_modified_conf_item_ies_o_value_c_names,
    "sl_drbs_modified_conf_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_modified_conf_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_modified_conf_item_ies_o_value_c_types_values[]    = {"SLDRBs-ModifiedConf-Item"};
const asn1::table::enum_desc sl_drbs_modified_conf_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_modified_conf_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_modified_conf_item_ies_o::value_c::types"};

// SLDRBs-Required-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_required_to_be_modified_item_ies_o[]    = {319};
static const uint8_t  objset_crit_sl_drbs_required_to_be_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_sl_drbs_required_to_be_modified_item_ies_o[] = {
    sl_drbs_required_to_be_modified_item_ies_o::value_c::types::sl_drbs_required_to_be_modified_item};
static const uint8_t objset_presence_sl_drbs_required_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_required_to_be_modified_item_ies_o[] = {
    {objset_crit_sl_drbs_required_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_required_to_be_modified_item_ies_o,
     sl_drbs_required_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_required_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_required_to_be_modified_item_ies_o::desc = {
    objset_id_sl_drbs_required_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_sl_drbs_required_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_required_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_required_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_required_to_be_modified_item_ies_o_value_c_names[] = {
    {"SLDRBs-Required-ToBeModified-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_required_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_required_to_be_modified_item_ies_o::value_c::types>(),
    sl_drbs_required_to_be_modified_item_ies_o::value_c::types::nof_types,
    sl_drbs_required_to_be_modified_item_ies_o::value_c::types::nof_exts,
    sl_drbs_required_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_required_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_required_to_be_modified_item_ies_o_value_c_names,
    "sl_drbs_required_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_required_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_required_to_be_modified_item_ies_o_value_c_types_values[] = {
    "SLDRBs-Required-ToBeModified-Item"};
const asn1::table::enum_desc sl_drbs_required_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_required_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_required_to_be_modified_item_ies_o::value_c::types"};

// SLDRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_required_to_be_released_item_ies_o[]    = {321};
static const uint8_t  objset_crit_sl_drbs_required_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_sl_drbs_required_to_be_released_item_ies_o[] = {
    sl_drbs_required_to_be_released_item_ies_o::value_c::types::sl_drbs_required_to_be_released_item};
static const uint8_t objset_presence_sl_drbs_required_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_required_to_be_released_item_ies_o[] = {
    {objset_crit_sl_drbs_required_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_required_to_be_released_item_ies_o,
     sl_drbs_required_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_required_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_required_to_be_released_item_ies_o::desc = {
    objset_id_sl_drbs_required_to_be_released_item_ies_o,
    1,
    "id",
    objset_columns_sl_drbs_required_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_required_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_required_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_required_to_be_released_item_ies_o_value_c_names[] = {
    {"SLDRBs-Required-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_required_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_required_to_be_released_item_ies_o::value_c::types>(),
    sl_drbs_required_to_be_released_item_ies_o::value_c::types::nof_types,
    sl_drbs_required_to_be_released_item_ies_o::value_c::types::nof_exts,
    sl_drbs_required_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_required_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_required_to_be_released_item_ies_o_value_c_names,
    "sl_drbs_required_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_required_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_required_to_be_released_item_ies_o_value_c_types_values[] = {
    "SLDRBs-Required-ToBeReleased-Item"};
const asn1::table::enum_desc sl_drbs_required_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_required_to_be_released_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_required_to_be_released_item_ies_o::value_c::types"};

// SLDRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_setup_item_ies_o[]    = {323};
static const uint8_t  objset_crit_sl_drbs_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_sl_drbs_setup_item_ies_o[] = {
    sl_drbs_setup_item_ies_o::value_c::types::sl_drbs_setup_item};
static const uint8_t                    objset_presence_sl_drbs_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_setup_item_ies_o[]  = {
    {objset_crit_sl_drbs_setup_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_setup_item_ies_o, sl_drbs_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_setup_item_ies_o::desc = {objset_id_sl_drbs_setup_item_ies_o,
                                                                 1,
                                                                 "id",
                                                                 objset_columns_sl_drbs_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_setup_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_setup_item_ies_o_value_c_names[] = {
    {"SLDRBs-Setup-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_setup_item_ies_o::value_c::types>(),
    sl_drbs_setup_item_ies_o::value_c::types::nof_types,
    sl_drbs_setup_item_ies_o::value_c::types::nof_exts,
    sl_drbs_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_setup_item_ies_o_value_c_names,
    "sl_drbs_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_sl_drbs_setup_item_ies_o_value_c_types_values[] = {"SLDRBs-Setup-Item"};
const asn1::table::enum_desc sl_drbs_setup_item_ies_o::value_c::types_opts::desc        = {
    enum_names_sl_drbs_setup_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_setup_item_ies_o::value_c::types"};

// SLDRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_setup_mod_item_ies_o[]    = {335};
static const uint8_t  objset_crit_sl_drbs_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_sl_drbs_setup_mod_item_ies_o[] = {
    sl_drbs_setup_mod_item_ies_o::value_c::types::sl_drbs_setup_mod_item};
static const uint8_t                    objset_presence_sl_drbs_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_setup_mod_item_ies_o[]  = {
    {objset_crit_sl_drbs_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_setup_mod_item_ies_o, sl_drbs_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_setup_mod_item_ies_o::desc = {objset_id_sl_drbs_setup_mod_item_ies_o,
                                                                     1,
                                                                     "id",
                                                                     objset_columns_sl_drbs_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_setup_mod_item_ies_o_value_c_names[] = {
    {"SLDRBs-SetupMod-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_setup_mod_item_ies_o::value_c::types>(),
    sl_drbs_setup_mod_item_ies_o::value_c::types::nof_types,
    sl_drbs_setup_mod_item_ies_o::value_c::types::nof_exts,
    sl_drbs_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_setup_mod_item_ies_o_value_c_names,
    "sl_drbs_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_sl_drbs_setup_mod_item_ies_o_value_c_types_values[] = {"SLDRBs-SetupMod-Item"};
const asn1::table::enum_desc sl_drbs_setup_mod_item_ies_o::value_c::types_opts::desc        = {
    enum_names_sl_drbs_setup_mod_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_setup_mod_item_ies_o::value_c::types"};

// SLDRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_to_be_modified_item_ies_o[]    = {325};
static const uint8_t  objset_crit_sl_drbs_to_be_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_sl_drbs_to_be_modified_item_ies_o[] = {
    sl_drbs_to_be_modified_item_ies_o::value_c::types::sl_drbs_to_be_modified_item};
static const uint8_t                    objset_presence_sl_drbs_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_to_be_modified_item_ies_o[]  = {
    {objset_crit_sl_drbs_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_to_be_modified_item_ies_o, sl_drbs_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_to_be_modified_item_ies_o::desc =
    {objset_id_sl_drbs_to_be_modified_item_ies_o, 1, "id", objset_columns_sl_drbs_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_to_be_modified_item_ies_o_value_c_names[] = {
    {"SLDRBs-ToBeModified-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_to_be_modified_item_ies_o::value_c::types>(),
    sl_drbs_to_be_modified_item_ies_o::value_c::types::nof_types,
    sl_drbs_to_be_modified_item_ies_o::value_c::types::nof_exts,
    sl_drbs_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_to_be_modified_item_ies_o_value_c_names,
    "sl_drbs_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_to_be_modified_item_ies_o_value_c_types_values[] = {"SLDRBs-ToBeModified-Item"};
const asn1::table::enum_desc sl_drbs_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_to_be_modified_item_ies_o::value_c::types"};

// SLDRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_to_be_released_item_ies_o[]    = {327};
static const uint8_t  objset_crit_sl_drbs_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_sl_drbs_to_be_released_item_ies_o[] = {
    sl_drbs_to_be_released_item_ies_o::value_c::types::sl_drbs_to_be_released_item};
static const uint8_t                    objset_presence_sl_drbs_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_to_be_released_item_ies_o[]  = {
    {objset_crit_sl_drbs_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_to_be_released_item_ies_o, sl_drbs_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_to_be_released_item_ies_o::desc =
    {objset_id_sl_drbs_to_be_released_item_ies_o, 1, "id", objset_columns_sl_drbs_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_to_be_released_item_ies_o_value_c_names[] = {
    {"SLDRBs-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_to_be_released_item_ies_o::value_c::types>(),
    sl_drbs_to_be_released_item_ies_o::value_c::types::nof_types,
    sl_drbs_to_be_released_item_ies_o::value_c::types::nof_exts,
    sl_drbs_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_to_be_released_item_ies_o_value_c_names,
    "sl_drbs_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_to_be_released_item_ies_o_value_c_types_values[] = {"SLDRBs-ToBeReleased-Item"};
const asn1::table::enum_desc sl_drbs_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_to_be_released_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_to_be_released_item_ies_o::value_c::types"};

// SLDRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_to_be_setup_item_ies_o[]    = {329};
static const uint8_t  objset_crit_sl_drbs_to_be_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_sl_drbs_to_be_setup_item_ies_o[] = {
    sl_drbs_to_be_setup_item_ies_o::value_c::types::sl_drbs_to_be_setup_item};
static const uint8_t                    objset_presence_sl_drbs_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_to_be_setup_item_ies_o[]  = {
    {objset_crit_sl_drbs_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_to_be_setup_item_ies_o, sl_drbs_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_to_be_setup_item_ies_o::desc = {objset_id_sl_drbs_to_be_setup_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_sl_drbs_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_to_be_setup_item_ies_o_value_c_names[] = {
    {"SLDRBs-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_to_be_setup_item_ies_o::value_c::types>(),
    sl_drbs_to_be_setup_item_ies_o::value_c::types::nof_types,
    sl_drbs_to_be_setup_item_ies_o::value_c::types::nof_exts,
    sl_drbs_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_to_be_setup_item_ies_o_value_c_names,
    "sl_drbs_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_to_be_setup_item_ies_o_value_c_types_values[]    = {"SLDRBs-ToBeSetup-Item"};
const asn1::table::enum_desc sl_drbs_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_to_be_setup_item_ies_o::value_c::types"};

// SLDRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_sl_drbs_to_be_setup_mod_item_ies_o[]    = {331};
static const uint8_t  objset_crit_sl_drbs_to_be_setup_mod_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_sl_drbs_to_be_setup_mod_item_ies_o[] = {
    sl_drbs_to_be_setup_mod_item_ies_o::value_c::types::sl_drbs_to_be_setup_mod_item};
static const uint8_t                    objset_presence_sl_drbs_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_sl_drbs_to_be_setup_mod_item_ies_o[]  = {
    {objset_crit_sl_drbs_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_sl_drbs_to_be_setup_mod_item_ies_o, sl_drbs_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_sl_drbs_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc sl_drbs_to_be_setup_mod_item_ies_o::desc =
    {objset_id_sl_drbs_to_be_setup_mod_item_ies_o, 1, "id", objset_columns_sl_drbs_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_sl_drbs_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drbs_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_sl_drbs_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"SLDRBs-ToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc sl_drbs_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<sl_drbs_to_be_setup_mod_item_ies_o::value_c::types>(),
    sl_drbs_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    sl_drbs_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    sl_drbs_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_sl_drbs_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_sl_drbs_to_be_setup_mod_item_ies_o_value_c_names,
    "sl_drbs_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_drbs_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_sl_drbs_to_be_setup_mod_item_ies_o_value_c_types_values[] = {"SLDRBs-ToBeSetupMod-Item"};
const asn1::table::enum_desc sl_drbs_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_sl_drbs_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "sl_drbs_to_be_setup_mod_item_ies_o::value_c::types"};

// SRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_srbs_failed_to_be_setup_item_ies_o[]    = {65};
static const uint8_t  objset_crit_srbs_failed_to_be_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_srbs_failed_to_be_setup_item_ies_o[] = {
    srbs_failed_to_be_setup_item_ies_o::value_c::types::srbs_failed_to_be_setup_item};
static const uint8_t                    objset_presence_srbs_failed_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_srbs_failed_to_be_setup_item_ies_o[]  = {
    {objset_crit_srbs_failed_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_srbs_failed_to_be_setup_item_ies_o, srbs_failed_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_srbs_failed_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc srbs_failed_to_be_setup_item_ies_o::desc =
    {objset_id_srbs_failed_to_be_setup_item_ies_o, 1, "id", objset_columns_srbs_failed_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_srbs_failed_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srbs_failed_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_srbs_failed_to_be_setup_item_ies_o_value_c_names[] = {
    {"SRBs-FailedToBeSetup-Item", nullptr}};
const asn1::table::choice_desc srbs_failed_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<srbs_failed_to_be_setup_item_ies_o::value_c::types>(),
    srbs_failed_to_be_setup_item_ies_o::value_c::types::nof_types,
    srbs_failed_to_be_setup_item_ies_o::value_c::types::nof_exts,
    srbs_failed_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_srbs_failed_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_srbs_failed_to_be_setup_item_ies_o_value_c_names,
    "srbs_failed_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(srbs_failed_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_srbs_failed_to_be_setup_item_ies_o_value_c_types_values[] = {"SRBs-FailedToBeSetup-Item"};
const asn1::table::enum_desc srbs_failed_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_srbs_failed_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "srbs_failed_to_be_setup_item_ies_o::value_c::types"};

// SRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_srbs_failed_to_be_setup_mod_item_ies_o[]    = {67};
static const uint8_t  objset_crit_srbs_failed_to_be_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_srbs_failed_to_be_setup_mod_item_ies_o[] = {
    srbs_failed_to_be_setup_mod_item_ies_o::value_c::types::srbs_failed_to_be_setup_mod_item};
static const uint8_t objset_presence_srbs_failed_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_srbs_failed_to_be_setup_mod_item_ies_o[] = {
    {objset_crit_srbs_failed_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_srbs_failed_to_be_setup_mod_item_ies_o,
     srbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_srbs_failed_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc srbs_failed_to_be_setup_mod_item_ies_o::desc =
    {objset_id_srbs_failed_to_be_setup_mod_item_ies_o, 1, "id", objset_columns_srbs_failed_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_srbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srbs_failed_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_srbs_failed_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"SRBs-FailedToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc srbs_failed_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<srbs_failed_to_be_setup_mod_item_ies_o::value_c::types>(),
    srbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    srbs_failed_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    srbs_failed_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_srbs_failed_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_srbs_failed_to_be_setup_mod_item_ies_o_value_c_names,
    "srbs_failed_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(srbs_failed_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_srbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values[] = {
    "SRBs-FailedToBeSetupMod-Item"};
const asn1::table::enum_desc srbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_srbs_failed_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "srbs_failed_to_be_setup_mod_item_ies_o::value_c::types"};

// SRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_srbs_modified_item_ies_o[]    = {207};
static const uint8_t  objset_crit_srbs_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_srbs_modified_item_ies_o[] = {
    srbs_modified_item_ies_o::value_c::types::srbs_modified_item};
static const uint8_t                    objset_presence_srbs_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_srbs_modified_item_ies_o[]  = {
    {objset_crit_srbs_modified_item_ies_o, crit_e::nulltype},
    {objset_value_srbs_modified_item_ies_o, srbs_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_srbs_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc srbs_modified_item_ies_o::desc = {objset_id_srbs_modified_item_ies_o,
                                                                 1,
                                                                 "id",
                                                                 objset_columns_srbs_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_srbs_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srbs_modified_item_s::desc}};
static const asn1::table::field_name choice_table_srbs_modified_item_ies_o_value_c_names[] = {
    {"SRBs-Modified-Item", nullptr}};
const asn1::table::choice_desc srbs_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<srbs_modified_item_ies_o::value_c::types>(),
    srbs_modified_item_ies_o::value_c::types::nof_types,
    srbs_modified_item_ies_o::value_c::types::nof_exts,
    srbs_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_srbs_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_srbs_modified_item_ies_o_value_c_names,
    "srbs_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(srbs_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_srbs_modified_item_ies_o_value_c_types_values[] = {"SRBs-Modified-Item"};
const asn1::table::enum_desc srbs_modified_item_ies_o::value_c::types_opts::desc        = {
    enum_names_srbs_modified_item_ies_o_value_c_types_values,
    1,
    "srbs_modified_item_ies_o::value_c::types"};

// SRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_srbs_required_to_be_released_item_ies_o[]    = {69};
static const uint8_t  objset_crit_srbs_required_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_srbs_required_to_be_released_item_ies_o[] = {
    srbs_required_to_be_released_item_ies_o::value_c::types::srbs_required_to_be_released_item};
static const uint8_t objset_presence_srbs_required_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_srbs_required_to_be_released_item_ies_o[] = {
    {objset_crit_srbs_required_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_srbs_required_to_be_released_item_ies_o,
     srbs_required_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_srbs_required_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc srbs_required_to_be_released_item_ies_o::desc = {
    objset_id_srbs_required_to_be_released_item_ies_o,
    1,
    "id",
    objset_columns_srbs_required_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_srbs_required_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srbs_required_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_srbs_required_to_be_released_item_ies_o_value_c_names[] = {
    {"SRBs-Required-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc srbs_required_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<srbs_required_to_be_released_item_ies_o::value_c::types>(),
    srbs_required_to_be_released_item_ies_o::value_c::types::nof_types,
    srbs_required_to_be_released_item_ies_o::value_c::types::nof_exts,
    srbs_required_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_srbs_required_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_srbs_required_to_be_released_item_ies_o_value_c_names,
    "srbs_required_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(srbs_required_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_srbs_required_to_be_released_item_ies_o_value_c_types_values[] = {
    "SRBs-Required-ToBeReleased-Item"};
const asn1::table::enum_desc srbs_required_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_srbs_required_to_be_released_item_ies_o_value_c_types_values,
    1,
    "srbs_required_to_be_released_item_ies_o::value_c::types"};

// SRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_srbs_setup_item_ies_o[]    = {203};
static const uint8_t  objset_crit_srbs_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_srbs_setup_item_ies_o[] = {srbs_setup_item_ies_o::value_c::types::srbs_setup_item};
static const uint8_t  objset_presence_srbs_setup_item_ies_o[]                  = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_srbs_setup_item_ies_o[] = {
    {objset_crit_srbs_setup_item_ies_o, crit_e::nulltype},
    {objset_value_srbs_setup_item_ies_o, srbs_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_srbs_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc srbs_setup_item_ies_o::desc = {objset_id_srbs_setup_item_ies_o,
                                                              1,
                                                              "id",
                                                              objset_columns_srbs_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_srbs_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srbs_setup_item_s::desc}};
static const asn1::table::field_name choice_table_srbs_setup_item_ies_o_value_c_names[] = {
    {"SRBs-Setup-Item", nullptr}};
const asn1::table::choice_desc srbs_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<srbs_setup_item_ies_o::value_c::types>(),
    srbs_setup_item_ies_o::value_c::types::nof_types,
    srbs_setup_item_ies_o::value_c::types::nof_exts,
    srbs_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_srbs_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_srbs_setup_item_ies_o_value_c_names,
    "srbs_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(srbs_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_srbs_setup_item_ies_o_value_c_types_values[] = {"SRBs-Setup-Item"};
const asn1::table::enum_desc srbs_setup_item_ies_o::value_c::types_opts::desc        = {
    enum_names_srbs_setup_item_ies_o_value_c_types_values,
    1,
    "srbs_setup_item_ies_o::value_c::types"};

// SRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_srbs_setup_mod_item_ies_o[]    = {205};
static const uint8_t  objset_crit_srbs_setup_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_srbs_setup_mod_item_ies_o[] = {
    srbs_setup_mod_item_ies_o::value_c::types::srbs_setup_mod_item};
static const uint8_t                    objset_presence_srbs_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_srbs_setup_mod_item_ies_o[]  = {
    {objset_crit_srbs_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_srbs_setup_mod_item_ies_o, srbs_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_srbs_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc srbs_setup_mod_item_ies_o::desc = {objset_id_srbs_setup_mod_item_ies_o,
                                                                  1,
                                                                  "id",
                                                                  objset_columns_srbs_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_srbs_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srbs_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_srbs_setup_mod_item_ies_o_value_c_names[] = {
    {"SRBs-SetupMod-Item", nullptr}};
const asn1::table::choice_desc srbs_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<srbs_setup_mod_item_ies_o::value_c::types>(),
    srbs_setup_mod_item_ies_o::value_c::types::nof_types,
    srbs_setup_mod_item_ies_o::value_c::types::nof_exts,
    srbs_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_srbs_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_srbs_setup_mod_item_ies_o_value_c_names,
    "srbs_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(srbs_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_srbs_setup_mod_item_ies_o_value_c_types_values[] = {"SRBs-SetupMod-Item"};
const asn1::table::enum_desc srbs_setup_mod_item_ies_o::value_c::types_opts::desc        = {
    enum_names_srbs_setup_mod_item_ies_o_value_c_types_values,
    1,
    "srbs_setup_mod_item_ies_o::value_c::types"};

// SRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_srbs_to_be_released_item_ies_o[]    = {71};
static const uint8_t  objset_crit_srbs_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_srbs_to_be_released_item_ies_o[] = {
    srbs_to_be_released_item_ies_o::value_c::types::srbs_to_be_released_item};
static const uint8_t                    objset_presence_srbs_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_srbs_to_be_released_item_ies_o[]  = {
    {objset_crit_srbs_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_srbs_to_be_released_item_ies_o, srbs_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_srbs_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc srbs_to_be_released_item_ies_o::desc = {objset_id_srbs_to_be_released_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_srbs_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_srbs_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srbs_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_srbs_to_be_released_item_ies_o_value_c_names[] = {
    {"SRBs-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc srbs_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<srbs_to_be_released_item_ies_o::value_c::types>(),
    srbs_to_be_released_item_ies_o::value_c::types::nof_types,
    srbs_to_be_released_item_ies_o::value_c::types::nof_exts,
    srbs_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_srbs_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_srbs_to_be_released_item_ies_o_value_c_names,
    "srbs_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(srbs_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_srbs_to_be_released_item_ies_o_value_c_types_values[]    = {"SRBs-ToBeReleased-Item"};
const asn1::table::enum_desc srbs_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_srbs_to_be_released_item_ies_o_value_c_types_values,
    1,
    "srbs_to_be_released_item_ies_o::value_c::types"};

// SRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_srbs_to_be_setup_item_ies_o[]    = {73};
static const uint8_t  objset_crit_srbs_to_be_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_srbs_to_be_setup_item_ies_o[] = {
    srbs_to_be_setup_item_ies_o::value_c::types::srbs_to_be_setup_item};
static const uint8_t                    objset_presence_srbs_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_srbs_to_be_setup_item_ies_o[]  = {
    {objset_crit_srbs_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_srbs_to_be_setup_item_ies_o, srbs_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_srbs_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc srbs_to_be_setup_item_ies_o::desc = {objset_id_srbs_to_be_setup_item_ies_o,
                                                                    1,
                                                                    "id",
                                                                    objset_columns_srbs_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_srbs_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srbs_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_srbs_to_be_setup_item_ies_o_value_c_names[] = {
    {"SRBs-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc srbs_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<srbs_to_be_setup_item_ies_o::value_c::types>(),
    srbs_to_be_setup_item_ies_o::value_c::types::nof_types,
    srbs_to_be_setup_item_ies_o::value_c::types::nof_exts,
    srbs_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_srbs_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_srbs_to_be_setup_item_ies_o_value_c_names,
    "srbs_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(srbs_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_srbs_to_be_setup_item_ies_o_value_c_types_values[] = {"SRBs-ToBeSetup-Item"};
const asn1::table::enum_desc srbs_to_be_setup_item_ies_o::value_c::types_opts::desc        = {
    enum_names_srbs_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "srbs_to_be_setup_item_ies_o::value_c::types"};

// SRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_srbs_to_be_setup_mod_item_ies_o[]    = {75};
static const uint8_t  objset_crit_srbs_to_be_setup_mod_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_srbs_to_be_setup_mod_item_ies_o[] = {
    srbs_to_be_setup_mod_item_ies_o::value_c::types::srbs_to_be_setup_mod_item};
static const uint8_t                    objset_presence_srbs_to_be_setup_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_srbs_to_be_setup_mod_item_ies_o[]  = {
    {objset_crit_srbs_to_be_setup_mod_item_ies_o, crit_e::nulltype},
    {objset_value_srbs_to_be_setup_mod_item_ies_o, srbs_to_be_setup_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_srbs_to_be_setup_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc srbs_to_be_setup_mod_item_ies_o::desc = {objset_id_srbs_to_be_setup_mod_item_ies_o,
                                                                        1,
                                                                        "id",
                                                                        objset_columns_srbs_to_be_setup_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_srbs_to_be_setup_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srbs_to_be_setup_mod_item_s::desc}};
static const asn1::table::field_name choice_table_srbs_to_be_setup_mod_item_ies_o_value_c_names[] = {
    {"SRBs-ToBeSetupMod-Item", nullptr}};
const asn1::table::choice_desc srbs_to_be_setup_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<srbs_to_be_setup_mod_item_ies_o::value_c::types>(),
    srbs_to_be_setup_mod_item_ies_o::value_c::types::nof_types,
    srbs_to_be_setup_mod_item_ies_o::value_c::types::nof_exts,
    srbs_to_be_setup_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_srbs_to_be_setup_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_srbs_to_be_setup_mod_item_ies_o_value_c_names,
    "srbs_to_be_setup_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(srbs_to_be_setup_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_srbs_to_be_setup_mod_item_ies_o_value_c_types_values[]    = {"SRBs-ToBeSetupMod-Item"};
const asn1::table::enum_desc srbs_to_be_setup_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_srbs_to_be_setup_mod_item_ies_o_value_c_types_values,
    1,
    "srbs_to_be_setup_mod_item_ies_o::value_c::types"};

// ServingCellMO-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_serving_cell_mo_list_item_ies_o[]    = {696};
static const uint8_t  objset_crit_serving_cell_mo_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_serving_cell_mo_list_item_ies_o[] = {
    serving_cell_mo_list_item_ies_o::value_c::types::serving_cell_mo_list_item};
static const uint8_t                    objset_presence_serving_cell_mo_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_serving_cell_mo_list_item_ies_o[]  = {
    {objset_crit_serving_cell_mo_list_item_ies_o, crit_e::nulltype},
    {objset_value_serving_cell_mo_list_item_ies_o, serving_cell_mo_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_serving_cell_mo_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc serving_cell_mo_list_item_ies_o::desc = {objset_id_serving_cell_mo_list_item_ies_o,
                                                                        1,
                                                                        "id",
                                                                        objset_columns_serving_cell_mo_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_serving_cell_mo_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_mo_list_item_s::desc}};
static const asn1::table::field_name choice_table_serving_cell_mo_list_item_ies_o_value_c_names[] = {
    {"ServingCellMO-List-Item", nullptr}};
const asn1::table::choice_desc serving_cell_mo_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<serving_cell_mo_list_item_ies_o::value_c::types>(),
    serving_cell_mo_list_item_ies_o::value_c::types::nof_types,
    serving_cell_mo_list_item_ies_o::value_c::types::nof_exts,
    serving_cell_mo_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_serving_cell_mo_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_serving_cell_mo_list_item_ies_o_value_c_names,
    "serving_cell_mo_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(serving_cell_mo_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_serving_cell_mo_list_item_ies_o_value_c_types_values[]    = {"ServingCellMO-List-Item"};
const asn1::table::enum_desc serving_cell_mo_list_item_ies_o::value_c::types_opts::desc = {
    enum_names_serving_cell_mo_list_item_ies_o_value_c_types_values,
    1,
    "serving_cell_mo_list_item_ies_o::value_c::types"};

// UE-MulticastMRBs-ConfirmedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o[]    = {654};
static const uint8_t  objset_crit_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o[] = {
    ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types::
        ue_multicast_m_rbs_confirmed_to_be_modified_item};
static const uint8_t objset_presence_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o[] = {
    {objset_crit_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o,
     ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::desc = {
    objset_id_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &ue_multicast_m_rbs_confirmed_to_be_modified_item_s::desc}};
static const asn1::table::field_name
    choice_table_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o_value_c_names[] = {
        {"UE-MulticastMRBs-ConfirmedToBeModified-Item", nullptr}};
const asn1::table::choice_desc ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types>(),
    ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types::nof_types,
    ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types::nof_exts,
    ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o_value_c_names,
    "ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o_value_c_types_values[] = {
    "UE-MulticastMRBs-ConfirmedToBeModified-Item"};
const asn1::table::enum_desc ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types"};

// UE-MulticastMRBs-RequiredToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_multicast_m_rbs_required_to_be_modified_item_ies_o[]    = {656};
static const uint8_t  objset_crit_ue_multicast_m_rbs_required_to_be_modified_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ue_multicast_m_rbs_required_to_be_modified_item_ies_o[] = {
    ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types::
        ue_multicast_m_rbs_required_to_be_modified_item};
static const uint8_t objset_presence_ue_multicast_m_rbs_required_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_multicast_m_rbs_required_to_be_modified_item_ies_o[] = {
    {objset_crit_ue_multicast_m_rbs_required_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_ue_multicast_m_rbs_required_to_be_modified_item_ies_o,
     ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_ue_multicast_m_rbs_required_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_multicast_m_rbs_required_to_be_modified_item_ies_o::desc = {
    objset_id_ue_multicast_m_rbs_required_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_ue_multicast_m_rbs_required_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_ue_multicast_m_rbs_required_to_be_modified_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &ue_multicast_m_rbs_required_to_be_modified_item_s::desc}};
static const asn1::table::field_name
    choice_table_ue_multicast_m_rbs_required_to_be_modified_item_ies_o_value_c_names[] = {
        {"UE-MulticastMRBs-RequiredToBeModified-Item", nullptr}};
const asn1::table::choice_desc ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types>(),
    ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types::nof_types,
    ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types::nof_exts,
    ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ue_multicast_m_rbs_required_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ue_multicast_m_rbs_required_to_be_modified_item_ies_o_value_c_names,
    "ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_multicast_m_rbs_required_to_be_modified_item_ies_o_value_c_types_values[] = {
    "UE-MulticastMRBs-RequiredToBeModified-Item"};
const asn1::table::enum_desc ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_ue_multicast_m_rbs_required_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types"};

// UE-MulticastMRBs-RequiredToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_multicast_m_rbs_required_to_be_released_item_ies_o[]    = {658};
static const uint8_t  objset_crit_ue_multicast_m_rbs_required_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ue_multicast_m_rbs_required_to_be_released_item_ies_o[] = {
    ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types::
        ue_multicast_m_rbs_required_to_be_released_item};
static const uint8_t objset_presence_ue_multicast_m_rbs_required_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_multicast_m_rbs_required_to_be_released_item_ies_o[] = {
    {objset_crit_ue_multicast_m_rbs_required_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_ue_multicast_m_rbs_required_to_be_released_item_ies_o,
     ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_ue_multicast_m_rbs_required_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_multicast_m_rbs_required_to_be_released_item_ies_o::desc = {
    objset_id_ue_multicast_m_rbs_required_to_be_released_item_ies_o,
    1,
    "id",
    objset_columns_ue_multicast_m_rbs_required_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_ue_multicast_m_rbs_required_to_be_released_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &ue_multicast_m_rbs_required_to_be_released_item_s::desc}};
static const asn1::table::field_name
    choice_table_ue_multicast_m_rbs_required_to_be_released_item_ies_o_value_c_names[] = {
        {"UE-MulticastMRBs-RequiredToBeReleased-Item", nullptr}};
const asn1::table::choice_desc ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types>(),
    ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types::nof_types,
    ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types::nof_exts,
    ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ue_multicast_m_rbs_required_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ue_multicast_m_rbs_required_to_be_released_item_ies_o_value_c_names,
    "ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_multicast_m_rbs_required_to_be_released_item_ies_o_value_c_types_values[] = {
    "UE-MulticastMRBs-RequiredToBeReleased-Item"};
const asn1::table::enum_desc ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_ue_multicast_m_rbs_required_to_be_released_item_ies_o_value_c_types_values,
    1,
    "ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types"};

// UE-MulticastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_multicast_m_rbs_setup_item_ies_o[]    = {680};
static const uint8_t  objset_crit_ue_multicast_m_rbs_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ue_multicast_m_rbs_setup_item_ies_o[] = {
    ue_multicast_m_rbs_setup_item_ies_o::value_c::types::ue_multicast_m_rbs_setup_item};
static const uint8_t                    objset_presence_ue_multicast_m_rbs_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_multicast_m_rbs_setup_item_ies_o[]  = {
    {objset_crit_ue_multicast_m_rbs_setup_item_ies_o, crit_e::nulltype},
    {objset_value_ue_multicast_m_rbs_setup_item_ies_o, ue_multicast_m_rbs_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_ue_multicast_m_rbs_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_multicast_m_rbs_setup_item_ies_o::desc =
    {objset_id_ue_multicast_m_rbs_setup_item_ies_o, 1, "id", objset_columns_ue_multicast_m_rbs_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ue_multicast_m_rbs_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_multicast_m_rbs_setup_item_s::desc}};
static const asn1::table::field_name choice_table_ue_multicast_m_rbs_setup_item_ies_o_value_c_names[] = {
    {"UE-MulticastMRBs-Setup-Item", nullptr}};
const asn1::table::choice_desc ue_multicast_m_rbs_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ue_multicast_m_rbs_setup_item_ies_o::value_c::types>(),
    ue_multicast_m_rbs_setup_item_ies_o::value_c::types::nof_types,
    ue_multicast_m_rbs_setup_item_ies_o::value_c::types::nof_exts,
    ue_multicast_m_rbs_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ue_multicast_m_rbs_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ue_multicast_m_rbs_setup_item_ies_o_value_c_names,
    "ue_multicast_m_rbs_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_multicast_m_rbs_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_multicast_m_rbs_setup_item_ies_o_value_c_types_values[] = {
    "UE-MulticastMRBs-Setup-Item"};
const asn1::table::enum_desc ue_multicast_m_rbs_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_ue_multicast_m_rbs_setup_item_ies_o_value_c_types_values,
    1,
    "ue_multicast_m_rbs_setup_item_ies_o::value_c::types"};

// UE-MulticastMRBs-Setupnew-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_multicast_m_rbs_setupnew_item_ies_o[]    = {700};
static const uint8_t  objset_crit_ue_multicast_m_rbs_setupnew_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ue_multicast_m_rbs_setupnew_item_ies_o[] = {
    ue_multicast_m_rbs_setupnew_item_ies_o::value_c::types::ue_multicast_m_rbs_setupnew_item};
static const uint8_t objset_presence_ue_multicast_m_rbs_setupnew_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_multicast_m_rbs_setupnew_item_ies_o[] = {
    {objset_crit_ue_multicast_m_rbs_setupnew_item_ies_o, crit_e::nulltype},
    {objset_value_ue_multicast_m_rbs_setupnew_item_ies_o,
     ue_multicast_m_rbs_setupnew_item_ies_o::value_c::types::nulltype},
    {objset_presence_ue_multicast_m_rbs_setupnew_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_multicast_m_rbs_setupnew_item_ies_o::desc =
    {objset_id_ue_multicast_m_rbs_setupnew_item_ies_o, 1, "id", objset_columns_ue_multicast_m_rbs_setupnew_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ue_multicast_m_rbs_setupnew_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_multicast_m_rbs_setupnew_item_s::desc}};
static const asn1::table::field_name choice_table_ue_multicast_m_rbs_setupnew_item_ies_o_value_c_names[] = {
    {"UE-MulticastMRBs-Setupnew-Item", nullptr}};
const asn1::table::choice_desc ue_multicast_m_rbs_setupnew_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ue_multicast_m_rbs_setupnew_item_ies_o::value_c::types>(),
    ue_multicast_m_rbs_setupnew_item_ies_o::value_c::types::nof_types,
    ue_multicast_m_rbs_setupnew_item_ies_o::value_c::types::nof_exts,
    ue_multicast_m_rbs_setupnew_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ue_multicast_m_rbs_setupnew_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ue_multicast_m_rbs_setupnew_item_ies_o_value_c_names,
    "ue_multicast_m_rbs_setupnew_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_multicast_m_rbs_setupnew_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_multicast_m_rbs_setupnew_item_ies_o_value_c_types_values[] = {
    "UE-MulticastMRBs-Setupnew-Item"};
const asn1::table::enum_desc ue_multicast_m_rbs_setupnew_item_ies_o::value_c::types_opts::desc = {
    enum_names_ue_multicast_m_rbs_setupnew_item_ies_o_value_c_types_values,
    1,
    "ue_multicast_m_rbs_setupnew_item_ies_o::value_c::types"};

// UE-MulticastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_multicast_m_rbs_to_be_released_item_ies_o[]    = {629};
static const uint8_t  objset_crit_ue_multicast_m_rbs_to_be_released_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ue_multicast_m_rbs_to_be_released_item_ies_o[] = {
    ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types::ue_multicast_m_rbs_to_be_released_item};
static const uint8_t objset_presence_ue_multicast_m_rbs_to_be_released_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_multicast_m_rbs_to_be_released_item_ies_o[] = {
    {objset_crit_ue_multicast_m_rbs_to_be_released_item_ies_o, crit_e::nulltype},
    {objset_value_ue_multicast_m_rbs_to_be_released_item_ies_o,
     ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types::nulltype},
    {objset_presence_ue_multicast_m_rbs_to_be_released_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_multicast_m_rbs_to_be_released_item_ies_o::desc = {
    objset_id_ue_multicast_m_rbs_to_be_released_item_ies_o,
    1,
    "id",
    objset_columns_ue_multicast_m_rbs_to_be_released_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ue_multicast_m_rbs_to_be_released_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_multicast_m_rbs_to_be_released_item_s::desc}};
static const asn1::table::field_name choice_table_ue_multicast_m_rbs_to_be_released_item_ies_o_value_c_names[] = {
    {"UE-MulticastMRBs-ToBeReleased-Item", nullptr}};
const asn1::table::choice_desc ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types>(),
    ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types::nof_types,
    ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types::nof_exts,
    ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ue_multicast_m_rbs_to_be_released_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ue_multicast_m_rbs_to_be_released_item_ies_o_value_c_names,
    "ue_multicast_m_rbs_to_be_released_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_multicast_m_rbs_to_be_released_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_multicast_m_rbs_to_be_released_item_ies_o_value_c_types_values[] = {
    "UE-MulticastMRBs-ToBeReleased-Item"};
const asn1::table::enum_desc ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types_opts::desc = {
    enum_names_ue_multicast_m_rbs_to_be_released_item_ies_o_value_c_types_values,
    1,
    "ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types"};

// UE-MulticastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_multicast_m_rbs_to_be_setup_item_ies_o[]    = {631};
static const uint8_t  objset_crit_ue_multicast_m_rbs_to_be_setup_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ue_multicast_m_rbs_to_be_setup_item_ies_o[] = {
    ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::ue_multicast_m_rbs_to_be_setup_item};
static const uint8_t objset_presence_ue_multicast_m_rbs_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_multicast_m_rbs_to_be_setup_item_ies_o[] = {
    {objset_crit_ue_multicast_m_rbs_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_ue_multicast_m_rbs_to_be_setup_item_ies_o,
     ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_ue_multicast_m_rbs_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_multicast_m_rbs_to_be_setup_item_ies_o::desc = {
    objset_id_ue_multicast_m_rbs_to_be_setup_item_ies_o,
    1,
    "id",
    objset_columns_ue_multicast_m_rbs_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ue_multicast_m_rbs_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_multicast_m_rbs_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_ue_multicast_m_rbs_to_be_setup_item_ies_o_value_c_names[] = {
    {"UE-MulticastMRBs-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types>(),
    ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::nof_types,
    ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::nof_exts,
    ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ue_multicast_m_rbs_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ue_multicast_m_rbs_to_be_setup_item_ies_o_value_c_names,
    "ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_multicast_m_rbs_to_be_setup_item_ies_o_value_c_types_values[] = {
    "UE-MulticastMRBs-ToBeSetup-Item"};
const asn1::table::enum_desc ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_ue_multicast_m_rbs_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types"};

// UE-MulticastMRBs-ToBeSetup-atModify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o[]    = {686};
static const uint8_t  objset_crit_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o[] = {
    ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types::ue_multicast_m_rbs_to_be_setup_at_modify_item};
static const uint8_t objset_presence_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o[] = {
    {objset_crit_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o, crit_e::nulltype},
    {objset_value_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o,
     ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types::nulltype},
    {objset_presence_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::desc = {
    objset_id_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o,
    1,
    "id",
    objset_columns_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &ue_multicast_m_rbs_to_be_setup_at_modify_item_s::desc}};
static const asn1::table::field_name choice_table_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o_value_c_names[] =
    {{"UE-MulticastMRBs-ToBeSetup-atModify-Item", nullptr}};
const asn1::table::choice_desc ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types>(),
    ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types::nof_types,
    ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types::nof_exts,
    ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o_value_c_names,
    "ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o_value_c_types_values[] = {
    "UE-MulticastMRBs-ToBeSetup-atModify-Item"};
const asn1::table::enum_desc ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types_opts::desc = {
    enum_names_ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o_value_c_types_values,
    1,
    "ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types"};
