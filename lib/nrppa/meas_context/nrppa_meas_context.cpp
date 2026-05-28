// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "nrppa_meas_context.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace ocucp;

nrppa_meas_context_list::nrppa_meas_context_list(ocudulog::basic_logger& logger_) : logger(logger_) {}

bool nrppa_meas_context_list::contains(lmf_meas_id_t lmf_meas_id) const
{
  return measurements.find(lmf_meas_id) != measurements.end();
}

nrppa_meas_context& nrppa_meas_context_list::operator[](lmf_meas_id_t lmf_meas_id)
{
  ocudu_assert(measurements.find(lmf_meas_id) != measurements.end(),
               "lmf_meas_id={}: NRPPa measurement context not found",
               fmt::underlying(lmf_meas_id));

  return measurements.at(lmf_meas_id);
}

nrppa_meas_context& nrppa_meas_context_list::add_measurement(cu_cp_amf_index_t            amf_index,
                                                             ran_meas_id_t                ran_meas_id,
                                                             lmf_meas_id_t                lmf_meas_id,
                                                             const std::vector<trp_id_t>& trp_list)
{
  logger.debug("ran_meas_id={}, lmf_meas_id={} : NRPPa measurement context created",
               fmt::underlying(ran_meas_id),
               fmt::underlying(lmf_meas_id));
  measurements.emplace(std::piecewise_construct,
                       std::forward_as_tuple(lmf_meas_id),
                       std::forward_as_tuple(amf_index, ran_meas_id, lmf_meas_id, trp_list));
  return measurements.at(lmf_meas_id);
}

void nrppa_meas_context_list::remove_measurement_context(lmf_meas_id_t lmf_meas_id)
{
  if (measurements.find(lmf_meas_id) == measurements.end()) {
    logger.warning("lmf_meas_id={}: NRPPa measurement context not found", fmt::underlying(lmf_meas_id));
    return;
  }

  measurements.at(lmf_meas_id).logger.log_debug("Removing NRPPa measurement context");
  measurements.erase(lmf_meas_id);
}

size_t nrppa_meas_context_list::size() const
{
  return measurements.size();
}

/// \brief Get the next available RAN meas ID.
expected<ran_meas_id_t, std::string> nrppa_meas_context_list::allocate_ran_meas_id()
{
  // Return false when no RAN meas ID is available.
  if (measurements.size() == (unsigned)ran_meas_id_t::max) {
    return make_unexpected(fmt::format("Maxium number of RAN MEAS IDs reached"));
  }

  // Iterate over all IDs starting with the next_ran_meas_id to find the available ID.
  while (true) {
    // Iterate over measurements.
    auto it = std::find_if(measurements.begin(), measurements.end(), [this](auto& meas) {
      return meas.second.ran_meas_id == next_ran_meas_id;
    });

    // Return the ID if it is not already used.
    if (it == measurements.end()) {
      ran_meas_id_t ret = next_ran_meas_id;
      // Increase the next RAN meas ID.
      increase_next_ran_meas_id();
      return ret;
    }

    // Increase the next RAN meas ID and try again.
    increase_next_ran_meas_id();
  }

  return make_unexpected(fmt::format("Maxium number of RAN MEAS IDs reached"));
}

void nrppa_meas_context_list::increase_next_ran_meas_id()
{
  if (next_ran_meas_id == ran_meas_id_t::max) {
    // Reset RAN meas ID counter.
    next_ran_meas_id = ran_meas_id_t::min;
  } else {
    // Increase RAN meas ID counter.
    next_ran_meas_id = uint_to_ran_meas_id(ran_meas_id_to_uint(next_ran_meas_id) + 1);
  }
}
