// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/nrppa_cause.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/positioning/measurement_information.h"
#include "ocudu/ran/positioning/positioning_activation.h"
#include "ocudu/ran/positioning/positioning_information_exchange.h"
#include "ocudu/ran/positioning/trp_information_exchange.h"
#include <optional>

namespace ocudu::ocucp {

struct trp_information_request_t {
  // TRP list is optional.
  std::vector<trp_id_t>                    trp_list;
  std::vector<trp_information_type_item_t> trp_info_type_list_trp_req;
};

struct trp_information_response_t {
  std::vector<trp_information_list_trp_response_item_t> trp_info_list_trp_resp;
};

struct trp_information_failure_t {
  nrppa_cause_t cause;
};

struct positioning_information_request_t {
  cu_cp_ue_index_t ue_index;

  std::optional<requested_srs_tx_characteristics_t> requested_srs_tx_characteristics;
  std::optional<ue_report_info_t>                   ue_report_info;
  std::optional<ue_teg_info_request_t>              ue_teg_info_request;
  std::optional<uint32_t>                           ue_teg_report_periodicity;
};

struct positioning_information_response_t {
  std::optional<srs_configuration_t>  srs_cfg;
  std::optional<relative_time_1900_t> sfn_initialization_time;
  // Optional list.
  std::vector<ue_tx_teg_assoc_item_t> ue_tx_teg_assoc_list;
};

struct positioning_information_failure_t {
  nrppa_cause_t cause;
};

struct positioning_activation_request_t {
  cu_cp_ue_index_t ue_index;

  srs_type_t                          srs_type;
  std::optional<relative_time_1900_t> activation_time;
};

struct positioning_activation_response_t {
  std::optional<uint16_t> sys_frame_num;
  std::optional<uint8_t>  slot_num;
};

struct positioning_activation_failure_t {
  nrppa_cause_t cause;
};

struct measurement_request_t {
  lmf_meas_id_t                                lmf_meas_id;
  ran_meas_id_t                                ran_meas_id;
  std::vector<trp_meas_request_item_t>         trp_meas_request_list;
  report_characteristics_t                     report_characteristics;
  std::optional<meas_periodicity_t>            meas_periodicity;
  std::vector<trp_meas_quantities_list_item_t> trp_meas_quantities;
  std::optional<uint64_t>                      sfn_initialization_time;

  std::optional<srs_configuration_t> srs_config;
  std::optional<bool>                meas_beam_info_request;
  std::optional<uint16_t>            sys_frame_num;
  std::optional<uint8_t>             slot_num;
  std::optional<uint32_t>            meas_periodicity_extended;
  std::optional<response_time_t>     resp_time;
  std::optional<uint16_t>            meas_characteristics_request_ind;
  std::optional<uint8_t>             meas_time_occasion;
  std::optional<uint8_t>             meas_amount;
};

struct measurement_response_t {
  lmf_meas_id_t                                lmf_meas_id;
  ran_meas_id_t                                ran_meas_id;
  std::vector<trp_measurement_response_item_t> trp_meas_resp_list;
};

struct measurement_failure_t {
  lmf_meas_id_t lmf_meas_id;
  nrppa_cause_t cause;
};

} // namespace ocudu::ocucp
