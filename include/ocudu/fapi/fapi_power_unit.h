// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/strong_type.h"

namespace ocudu {
namespace fapi {

namespace detail {
/// Tag struct used to uniquely identify the fapi_power units type.
struct fapi_power_tag {
  /// Text representation for the units.
  static const char* str() { return "dBphy"; }
};

} // namespace detail

/// \brief FAPI power measurement unit.
///
/// Stores a power value as a floating-point number in normalized dBs allowing to convert it to dBFS and dBm.
class fapi_power_unit : public strong_type<float,
                                           detail::fapi_power_tag,
                                           strong_arithmetic,
                                           strong_increment_decrement,
                                           strong_arithmetic_with_underlying_type>
{
public:
  /// Creates a FAPI power unit with a normalized dB.
  ///
  /// \param[in] value_dB Value in normalized dB.
  /// \param[in] dbfs_to_dbm_conversion_factor Value in dBm at the antenna connector equivalent to 0 dBFS for a
  /// configured \c rx_gain_dB of 0 dB. For split-8 configurations configured radio receive gain must be subtracted.
  /// \param[in] db_to_dbfs_conversion_factor  Value in dB relative to Full Scale (dBFS) equivalent to 0 dB in
  /// normalized units, i.e., as coming from the physical layer.
  constexpr fapi_power_unit(value_type value_dB,
                            float      dbfs_to_dbm_conversion_factor_,
                            float      db_to_dbfs_conversion_factor_) :
    strong_type(value_dB),
    dbfs_to_dbm_conversion_factor(dbfs_to_dbm_conversion_factor_),
    db_to_dbfs_conversion_factor(db_to_dbfs_conversion_factor_)
  {
  }

  /// Returns the stored value in dB (raw floating-point representation).
  constexpr value_type to_dB() const { return this->value(); }

  /// Converts the stored normalized dB value to dBFS.
  constexpr value_type to_dBFS() const { return this->value() + db_to_dbfs_conversion_factor; }

  /// Converts the stored normalized dB value to dBm.
  constexpr value_type to_dBm() const { return to_dBFS() + dbfs_to_dbm_conversion_factor; }

private:
  const float dbfs_to_dbm_conversion_factor;
  const float db_to_dbfs_conversion_factor;
};

} // namespace fapi
} // namespace ocudu
