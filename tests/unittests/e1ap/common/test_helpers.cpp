// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

gnb_cu_cp_ue_e1ap_id_t ocudu::generate_random_gnb_cu_cp_ue_e1ap_id()
{
  return int_to_gnb_cu_cp_ue_e1ap_id(test_rng::uniform_int<uint64_t>(to_underlying(gnb_cu_cp_ue_e1ap_id_t::min),
                                                                     to_underlying(gnb_cu_cp_ue_e1ap_id_t::max) - 1));
}

gnb_cu_up_ue_e1ap_id_t ocudu::generate_random_gnb_cu_up_ue_e1ap_id()
{
  return int_to_gnb_cu_up_ue_e1ap_id(test_rng::uniform_int<uint64_t>(to_underlying(gnb_cu_up_ue_e1ap_id_t::min),
                                                                     to_underlying(gnb_cu_up_ue_e1ap_id_t::max) - 1));
}
