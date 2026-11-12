// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/asn1_ap_codec.h"
#include "ocudu/asn1/e2sm_ccc/e2sm_ccc.h"

using namespace asn1;
using namespace asn1::e2sm_ccc;

template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::ran_cfg_structure_for_adf_s::list_of_attributes_l_::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_ran_cfg_structures_for_adf_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cell_cfgs_to_be_reported_for_adf_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::sleep_modes_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_event_trigger_styles_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm_ccc::report_style_s::list_of_supported_event_trigger_styles_for_report_style_l_::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_report_styles_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_ctrl_styles_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_query_styles_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::plmn_info_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::o_nr_cell_du_s::bwp_list_l_::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::partition_flow_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::partition_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::rrm_policy_member_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::perf_objive_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::tx_array_item_s::tpolarization_list_l_::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::tx_array_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::rx_array_item_s::rpolarization_list_l_::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::rx_array_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::supported_trx_ctrl_masks_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::prb_blanking_cfg_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::ran_cfg_structures_accepted_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::ran_cfg_structures_failed_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::list_of_cfg_structures_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_attributes_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_ran_cfg_structures_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_ran_cfg_structures_for_event_trigger_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::list_of_cfgs_reported_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_celllevel_cfg_structures_for_query_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cfg_structures_to_be_reported_for_query_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_ctrl_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_for_ctrl_outcome_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cell_level_cfg_structures_for_event_trigger_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_reported_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_nodelevel_ran_cfg_structures_for_query_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_to_be_queried_for_query_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_to_be_reported_for_query_l::value_type>::ops;
template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_for_ran_function_definition_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::plmn_id_list_l::value_type>::ops;
template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::snssai_list_l::value_type>::ops;
