// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/complex.h"
#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/support/time_alignment_estimator/time_alignment_measurement.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {

/// Time alignment estimator interface.
class time_alignment_estimator
{
public:
  /// \brief Maximum number of symbols that can be used for estimating the time alignment.
  ///
  /// It is equal to the maximum number of subcarriers that can be contained in an OFDM symbol.
  static constexpr unsigned max_nof_symbols = MAX_NOF_SUBCARRIERS;

  /// Default destructor.
  virtual ~time_alignment_estimator() = default;

  /// \brief Estimates the time alignment from frequency domain symbols from a single antenna port.
  /// \param[in] symbols Complex frequency domain symbols.
  /// \param[in] mask    Distribution of the complex symbols within an OFDM symbol.
  /// \param[in] scs     Subcarrier spacing.
  /// \param[in] max_ta  Maximum absolute time alignment measurement if it is not zero.
  /// \return The measured time alignment.
  /// \remark An assertion is triggered if the number of symbols is not equal to the number of active elements in the
  /// mask, or if the mask size is larger than the maximum supported number of subcarriers.
  virtual time_alignment_measurement estimate(span<const cf_t>                       symbols,
                                              const bounded_bitset<max_nof_symbols>& mask,
                                              subcarrier_spacing                     scs,
                                              double                                 max_ta = 0.0) = 0;

  /// \brief Estimates the time alignment from frequency domain symbols from one or more antenna ports.
  /// \param[in] symbols Complex frequency domain symbols (one slice for each antenna port).
  /// \param[in] mask    Distribution of the complex symbols within an OFDM symbol.
  /// \param[in] scs     Subcarrier spacing.
  /// \param[in] max_ta  Maximum absolute time alignment measurement if it is not zero.
  /// \return The measured time alignment.
  /// \remark An assertion is triggered if the number of symbols is not equal to the number of active elements in the
  /// mask, or if the mask size is larger than the maximum supported number of subcarriers.
  virtual time_alignment_measurement estimate(const re_buffer_reader<cf_t>&          symbols,
                                              const bounded_bitset<max_nof_symbols>& mask,
                                              subcarrier_spacing                     scs,
                                              double                                 max_ta = 0.0) = 0;

  /// \brief Estimates the time alignment from frequency domain symbols from a single antenna port.
  /// \param[in] symbols Complex frequency domain symbols.
  /// \param[in] stride  Distance between the complex symbols within an OFDM symbol.
  /// \param[in] scs     Subcarrier spacing.
  /// \param[in] max_ta  Maximum absolute time alignment measurement if it is not zero.
  /// \return The measured time alignment.
  /// \remark An assertion is triggered if the number of symbols times the stride exceed the frequency domain buffer.
  virtual time_alignment_measurement
  estimate(span<const cf_t> symbols, unsigned stride, subcarrier_spacing scs, double max_ta = 0.0) = 0;

  /// \brief Estimates the time alignment from frequency domain symbols from one or more antenna ports.
  /// \param[in] symbols Complex frequency domain symbols (one slice for each antenna port).
  /// \param[in] stride  Distance between the complex symbols within an OFDM symbol.
  /// \param[in] scs     Subcarrier spacing.
  /// \param[in] max_ta  Maximum absolute time alignment measurement if it is not zero.
  /// \return The measured time alignment.
  /// \remark An assertion is triggered if the number of symbols times the stride exceed the frequency domain buffer.
  virtual time_alignment_measurement
  estimate(const re_buffer_reader<cf_t>& symbols, unsigned stride, subcarrier_spacing scs, double max_ta = 0.0) = 0;
};

} // namespace ocudu
