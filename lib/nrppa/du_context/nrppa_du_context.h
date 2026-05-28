// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "nrppa_du_logger.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/positioning/measurement_information.h"
#include <unordered_map>

namespace ocudu::ocucp {

struct nrppa_du_context {
  cu_cp_du_index_t     du_index;
  nrppa_f1ap_notifier* f1ap = nullptr;

  std::vector<trp_meas_request_item_t>         trp_meas_request_list;
  std::vector<trp_meas_quantities_list_item_t> trp_meas_quantities;
  std::optional<std::chrono::milliseconds>     meas_periodicity_ms;

  nrppa_du_logger logger;

  nrppa_du_context(cu_cp_du_index_t du_index_, nrppa_f1ap_notifier& f1ap_notifier_) :
    du_index(du_index_), f1ap(&f1ap_notifier_), logger("NRPPA", {du_index_})
  {
  }
};

class nrppa_du_context_list
{
public:
  nrppa_du_context_list(ocudulog::basic_logger& logger_);

  /// \brief Checks whether a DU context with the given DU index exists.
  /// \param[in] du_index The DU index used to find the DU context.
  /// \return True when a DU context for the given DU index exists, false otherwise.
  bool contains(cu_cp_du_index_t du_index) const;

  nrppa_du_context& operator[](cu_cp_du_index_t du_index);

  nrppa_du_context& add_du(cu_cp_du_index_t du_index, nrppa_f1ap_notifier& f1ap_notifier);

  void
  update_du_index(cu_cp_du_index_t new_du_index, cu_cp_du_index_t old_du_index, nrppa_f1ap_notifier& new_f1ap_notifier);

  void remove_du_context(cu_cp_du_index_t du_index);

  size_t size() const;

private:
  ocudulog::basic_logger& logger;
  // Indexed by du_index.
  std::unordered_map<cu_cp_du_index_t, nrppa_du_context> dus;
};

} // namespace ocudu::ocucp
