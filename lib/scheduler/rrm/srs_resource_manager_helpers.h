// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <optional>

namespace ocudu {
namespace srs_res_mng_details {

/// \brief Helper that computes the SRS bandwidth parameter \f$C_{SRS}\f$ that maximizes the SRS bandwidth
/// \f$m_{SRS,0}\f$ subject to it fitting within \c nof_avail_rbs.
/// \param [in] nof_avail_rbs Number of RBs available for the SRS; this may be the whole UL BWP, or a subset of it
///             (e.g. excluding the RBs used by the common PUCCH resources).
std::optional<unsigned> compute_c_srs(unsigned nof_avail_rbs);

/// \brief Helper that returns the PRB start value for the SRS, relative to the start of the RB interval available
/// for the SRS.
/// \param[in] c_srs \f$C_{SRS}\f$ value, as per Section 6.4.1.4, TS 38.211.
/// \param[in] nof_avail_rbs Number of RBs available for the SRS (see \c compute_c_srs).
/// \return The PRB start value for the SRS, relative to the start of the RB interval available for the SRS; this
///         value is computed in such a way that the SRS resources are placed at the center of that RB interval.
unsigned compute_srs_rb_start(unsigned c_srs, unsigned nof_avail_rbs);

} // namespace srs_res_mng_details
} // namespace ocudu
