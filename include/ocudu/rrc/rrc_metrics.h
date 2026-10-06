// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/common.h"
#include <array>

namespace ocudu {

struct rrc_connection_counter_with_cause {
  rrc_connection_counter_with_cause() = default;

  void increase(establishment_cause_t cause)
  {
    if (static_cast<unsigned>(cause) < counters_by_cause.size()) {
      counters_by_cause[static_cast<unsigned>(cause)]++;
    } else {
      counters_by_cause[static_cast<unsigned>(establishment_cause_t::unknown)]++;
    }
  }

  unsigned size() const { return counters_by_cause.size(); }

  unsigned get_count(establishment_cause_t cause) const { return counters_by_cause[static_cast<unsigned>(cause)]; }

  unsigned get_count(unsigned index) const { return counters_by_cause[index]; }

  establishment_cause_t get_cause(unsigned index) const { return establishment_cause_t(index); }

  /// Returns a const iterator to the beginning of the container.
  std::array<unsigned, 11>::const_iterator begin() const { return counters_by_cause.begin(); }
  std::array<unsigned, 11>::const_iterator cbegin() const { return counters_by_cause.cbegin(); }

  /// Returns a const iterator to the end of the container.
  std::array<unsigned, 11>::const_iterator end() const { return counters_by_cause.end(); }
  std::array<unsigned, 11>::const_iterator cend() const { return counters_by_cause.cend(); }

private:
  // The RRC setup request/complete counters indexed by the establishment cause.
  std::array<unsigned, 11> counters_by_cause = {};
};

struct rrc_connection_fail_counter_with_cause {
  rrc_connection_fail_counter_with_cause() = default;

  void increase(establishment_fail_cause_t cause)
  {
    if (static_cast<unsigned>(cause) < counters_by_cause.size()) {
      counters_by_cause[static_cast<unsigned>(cause)]++;
    } else {
      counters_by_cause[static_cast<unsigned>(establishment_fail_cause_t::other)]++;
    }
  }

  unsigned size() const { return counters_by_cause.size(); }

  unsigned get_count(establishment_fail_cause_t cause) const { return counters_by_cause[static_cast<unsigned>(cause)]; }

  unsigned get_count(unsigned index) const { return counters_by_cause[index]; }

  establishment_fail_cause_t get_cause(unsigned index) const { return establishment_fail_cause_t(index); }

  /// Returns a const iterator to the beginning of the container.
  std::array<unsigned, 3>::const_iterator begin() const { return counters_by_cause.begin(); }
  std::array<unsigned, 3>::const_iterator cbegin() const { return counters_by_cause.cbegin(); }

  /// Returns a const iterator to the end of the container.
  std::array<unsigned, 3>::const_iterator end() const { return counters_by_cause.end(); }
  std::array<unsigned, 3>::const_iterator cend() const { return counters_by_cause.cend(); }

private:
  // The failed RRC establishment counters indexed by failure cause.
  std::array<unsigned, 3> counters_by_cause = {};
};

struct rrc_connection_resume_counter_with_cause {
  static constexpr unsigned NOF_RESUME_CAUSES = static_cast<unsigned>(resume_cause_t::unknown) + 1;

  rrc_connection_resume_counter_with_cause() = default;

  void increase(resume_cause_t cause)
  {
    if (static_cast<unsigned>(cause) < counters_by_cause.size()) {
      counters_by_cause[static_cast<unsigned>(cause)]++;
    } else {
      counters_by_cause[static_cast<unsigned>(resume_cause_t::unknown)]++;
    }
  }

  unsigned size() const { return counters_by_cause.size(); }

  unsigned get_count(resume_cause_t cause) const { return counters_by_cause[static_cast<unsigned>(cause)]; }

  unsigned get_count(unsigned index) const { return counters_by_cause[index]; }

  resume_cause_t get_cause(unsigned index) const { return resume_cause_t(index); }

  /// Returns a const iterator to the beginning of the container.
  std::array<unsigned, NOF_RESUME_CAUSES>::const_iterator begin() const { return counters_by_cause.begin(); }
  std::array<unsigned, NOF_RESUME_CAUSES>::const_iterator cbegin() const { return counters_by_cause.cbegin(); }

  /// Returns a const iterator to the end of the container.
  std::array<unsigned, NOF_RESUME_CAUSES>::const_iterator end() const { return counters_by_cause.end(); }
  std::array<unsigned, NOF_RESUME_CAUSES>::const_iterator cend() const { return counters_by_cause.cend(); }

private:
  // The RRC setup request/complete counters indexed by the resume cause.
  std::array<unsigned, NOF_RESUME_CAUSES> counters_by_cause = {};
};

struct rrc_du_metrics {
  /// \brief RRC connection metrics, see TS 28.552 section 5.1.1.4.
  unsigned mean_nof_rrc_connections;
  unsigned max_nof_rrc_connections;
  unsigned mean_nof_inactive_rrc_connections;
  unsigned max_nof_inactive_rrc_connections;
  /// \brief RRC connection establishment metrics, see TS 28.552 section 5.1.1.15.
  rrc_connection_counter_with_cause      attempted_rrc_connection_establishments;
  rrc_connection_counter_with_cause      successful_rrc_connection_establishments;
  rrc_connection_fail_counter_with_cause failed_rrc_connection_establishments;
  /// \brief RRC connection re-establishment metrics, see TS 28.552 section 5.1.1.17.
  unsigned attempted_rrc_connection_reestablishments;
  unsigned successful_rrc_connection_reestablishments_with_ue_context;
  unsigned successful_rrc_connection_reestablishments_without_ue_context;
  /// \brief RRC connection resume metrics, see TS 28.552 section 5.1.1.18.
  rrc_connection_resume_counter_with_cause attempted_rrc_connection_resumes;
  rrc_connection_resume_counter_with_cause successful_rrc_connection_resumes;
  rrc_connection_resume_counter_with_cause successful_rrc_connection_resumes_with_fallback;
  rrc_connection_resume_counter_with_cause rrc_connection_resumes_followed_by_network_release;
  rrc_connection_resume_counter_with_cause attempted_rrc_connection_resumes_followed_by_rrc_setup;
};

} // namespace ocudu
