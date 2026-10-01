// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ofh/serdes/ofh_uplane_message_decoder_properties.h"
#include "ocudu/ofh/timing/slot_symbol_point.h"
#include <optional>

namespace ocudu {
namespace ofh {
namespace uplane_peeker {

/// Peeks and returns the filter index of the given message.
///
/// \param[in] message Message to peek.
/// \return Message filter index or nullopt if the value cannot be peeked.
std::optional<filter_index_type> peek_filter_index(span<const uint8_t> message);

/// Peeks and returns the slot symbol point of the given message.
///
/// \param[in] message Message to peek.
/// \param[in] nof_symbols Number of symbols.
/// \param[in] scs Subcarrier spacing.
/// \return Expected slot symbol point or nullopt if the value cannot be peeked
std::optional<slot_symbol_point>
peek_slot_symbol_point(span<const uint8_t> message, unsigned nof_symbols, subcarrier_spacing scs);

} // namespace uplane_peeker

/// Open Fronthaul User-Plane message decoder interface.
class uplane_message_decoder
{
public:
  virtual ~uplane_message_decoder() = default;

  /// Decodes the given message into results and returns true on success, false otherwise.
  ///
  /// \param[out] results Results of decoding the message. On error, results value is undefined.
  /// \param[in] message Message to be decoded.
  /// \return True on success, false otherwise.
  virtual bool decode(uplane_message_decoder_results& results, span<const uint8_t> message) = 0;
};

} // namespace ofh
} // namespace ocudu
