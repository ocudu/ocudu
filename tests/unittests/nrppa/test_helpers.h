// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "ocudu/ran/cu_cp_types.h"
#include <gtest/gtest.h>

namespace ocudu::ocucp {

class dummy_nrppa_cu_cp_notifier : public nrppa_cu_cp_notifier
{
public:
  dummy_nrppa_cu_cp_notifier(ue_manager& ue_mng_) : ue_mng(ue_mng_), logger(ocudulog::fetch_basic_logger("TEST")) {}

  nrppa_cu_cp_ue_notifier* on_new_nrppa_ue(cu_cp_ue_index_t ue_index) override
  {
    last_ue = ue_index;

    auto* ue = ue_mng.find_ue(ue_index);
    if (ue == nullptr) {
      logger.error("ue={}: Failed to create UE", ue_index);
      return nullptr;
    }

    logger.info("ue={}: NRPPA UE was created", ue_index);
    return &ue->get_nrppa_cu_cp_ue_notifier();
  }

  void on_ul_nrppa_pdu(const byte_buffer&                                nrppa_pdu,
                       std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t> ue_or_amf_index) override
  {
    last_ul_nrppa_pdu = nrppa_pdu.copy();
  }

  cu_cp_ue_index_t last_ue;
  byte_buffer      last_ul_nrppa_pdu;

private:
  ue_manager&             ue_mng;
  ocudulog::basic_logger& logger;
};

class dummy_nrppa_cu_cp_ue_notifier : public nrppa_cu_cp_ue_notifier
{
public:
  ~dummy_nrppa_cu_cp_ue_notifier() = default;

  void set_ue_index(cu_cp_ue_index_t ue_index_) { ue_index = ue_index_; }
  void set_du_index(cu_cp_du_index_t du_index_) { du_index = du_index_; }
  void set_meas_results(std::optional<cell_measurement_positioning_info>& meas_results_)
  {
    meas_results = meas_results_;
  }

  /// \brief Get the UE index of the UE.
  cu_cp_ue_index_t get_ue_index() const override { return ue_index.value(); }

  /// \brief Get the DU index of the UE.
  cu_cp_du_index_t get_du_index() const override { return du_index.value(); }

  std::optional<cell_measurement_positioning_info>& on_measurement_results_required() override { return meas_results; }

  /// \brief Schedule an async task for the UE.
  bool schedule_async_task(async_task<void> task) override { return true; }

  std::optional<cu_cp_ue_index_t>                  ue_index;
  std::optional<cu_cp_du_index_t>                  du_index;
  std::optional<cell_measurement_positioning_info> meas_results;
};

} // namespace ocudu::ocucp
