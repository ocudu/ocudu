// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "nrppa_meas_logger.h"
#include "ocudu/adt/expected.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/positioning/positioning_ids.h"
#include <unordered_map>

namespace ocudu::ocucp {

struct nrppa_meas_context {
  cu_cp_amf_index_t amf_index;
  ran_meas_id_t     ran_meas_id;
  lmf_meas_id_t     lmf_meas_id;

  std::vector<trp_id_t> trp_list;

  nrppa_meas_logger logger;

  nrppa_meas_context(cu_cp_amf_index_t            amf_index_,
                     ran_meas_id_t                ran_meas_id_,
                     lmf_meas_id_t                lmf_meas_id_,
                     const std::vector<trp_id_t>& trp_list_) :
    amf_index(amf_index_),
    ran_meas_id(ran_meas_id_),
    lmf_meas_id(lmf_meas_id_),
    trp_list(trp_list_),
    logger("NRPPA", {ran_meas_id_, lmf_meas_id_})
  {
  }
};

class nrppa_meas_context_list
{
public:
  nrppa_meas_context_list(ocudulog::basic_logger& logger_);

  /// \brief Checks whether a measurement context with the given LMF measurement ID exists.
  /// \param[in] lmf_meas_id The LMF measurement ID used to find the measurement context.
  /// \return True when a measurement context for the given LMF measurement ID exists, false otherwise.
  bool contains(lmf_meas_id_t lmf_meas_id) const;

  nrppa_meas_context& operator[](lmf_meas_id_t lmf_meas_id);

  nrppa_meas_context& add_measurement(cu_cp_amf_index_t            amf_index,
                                      ran_meas_id_t                ran_meas_id,
                                      lmf_meas_id_t                lmf_meas_id,
                                      const std::vector<trp_id_t>& trp_list);

  void remove_measurement_context(lmf_meas_id_t lmf_meas_id);

  size_t size() const;

  /// \brief Get the next available RAN MEAS ID.
  expected<ran_meas_id_t, std::string> allocate_ran_meas_id();

protected:
  ran_meas_id_t next_ran_meas_id = ran_meas_id_t::min;

private:
  void increase_next_ran_meas_id();

  ocudulog::basic_logger& logger;
  // Indexed by lmf_meas_id.
  std::unordered_map<lmf_meas_id_t, nrppa_meas_context> measurements;
};

} // namespace ocudu::ocucp
