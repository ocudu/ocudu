// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/upper_phy_rx_results_notifier.h"

namespace ocudu {

class upper_phy_rx_results_notifier_spy : public upper_phy_rx_results_notifier
{
public:
  void on_new_prach_results(const ul_prach_results& result) override { ++nof_prach_results; }

  void on_new_pusch_results_control(const ul_pusch_results_control& result) override
  {
    ++nof_pusch_uci_results;
    last_pusch_uci_result = result;
  }

  void on_new_pusch_results_data(const ul_pusch_results_data& result) override { ++nof_pusch_data_results; }

  void on_new_pucch_results(const ul_pucch_results& result) override { ++nof_pucch_results; }

  void on_new_srs_results(const ul_srs_results& result) override { ++nof_srs_results; }

  bool has_prach_result_been_notified() const { return nof_prach_results != 0; }

  bool has_pusch_uci_result_been_notified() const { return nof_pusch_uci_results != 0; }

  bool has_pusch_data_result_been_notified() const { return nof_pusch_data_results != 0; }

  bool has_pucch_result_been_notified() const { return nof_pucch_results != 0; }

  bool has_srs_result_been_notified() const { return nof_srs_results != 0; }

  /// Returns the number of times each result type has been notified.
  unsigned get_nof_prach_results() const { return nof_prach_results; }

  unsigned get_nof_pusch_uci_results() const { return nof_pusch_uci_results; }

  unsigned get_nof_pusch_data_results() const { return nof_pusch_data_results; }

  unsigned get_nof_pucch_results() const { return nof_pucch_results; }

  unsigned get_nof_srs_results() const { return nof_srs_results; }

  /// Returns the last captured PUSCH UCI result (empty if none notified).
  const std::optional<ul_pusch_results_control>& get_last_pusch_uci_result() const { return last_pusch_uci_result; }

  void clear()
  {
    nof_prach_results      = 0;
    nof_pusch_data_results = 0;
    nof_pusch_uci_results  = 0;
    nof_pucch_results      = 0;
    nof_srs_results        = 0;
    last_pusch_uci_result.reset();
  }

private:
  unsigned                                nof_prach_results      = 0;
  unsigned                                nof_pusch_data_results = 0;
  unsigned                                nof_pusch_uci_results  = 0;
  unsigned                                nof_pucch_results      = 0;
  unsigned                                nof_srs_results        = 0;
  std::optional<ul_pusch_results_control> last_pusch_uci_result;
};

} // namespace ocudu
