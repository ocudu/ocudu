// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/srs/srs_properties.h"
#include <optional>

namespace ocudu {

/// \brief Parameters required to receive the Sounding Reference Signals described in 3GPP TS38.211 Section 6.4.1.4.
///
/// Higher layer parameters mentioned in this structure are contained in the 3GPP TS38.331 Section 6.3.2 Information
/// Element \e SRS-Resource.
struct srs_resource_configuration {
  /// Enumeration for attributes that accept only one, two, or four.
  enum class one_two_four_enum : uint8_t { one = 1, two = 2, four = 4 };

  /// Collects parameters related to the SRS transmission periodicity.
  struct periodicity_and_offset {
    /// Periodicity in slots, parameter \f$T_{SRS}\f$.
    uint16_t periodicity;
    /// Offset within the periodicity, parameter \f$T_{offset}\f$.
    uint16_t offset;
  };

  /// Number of transmit antenna ports, parameter \f$N^{SRS}_{ap}\f$.
  one_two_four_enum nof_antenna_ports;
  /// \brief Consecutive number of OFDM symbols used for the transmission, parameter \f$N^{SRS}_{symb}\f$.
  ///
  /// It is given by the higher layer parameter \e nrofSymbols contained in the higher layer parameter
  /// \e resourceMapping.
  srs_nof_symbols nof_symbols;
  /// \brief Starting position in the time domain, parameter \f$l_0\f$.
  ///
  /// The parameter is given by \f$l_0=N^{slot}_{symb}-1-l_{offset}\f$ where the offset \f$l_0\in\{0,1,...,5\}\f$
  /// counts symbols backwards from the end of the slot and is given by the field \e startPosition contained in the
  /// higher layer parameter \e resourceMapping.
  ///
  /// The valid range is {0...13}.
  ///
  /// The number of symbols plus the start symbol must not exceed the number of symbols in one slot.
  bounded_integer<uint8_t, 0, 13> start_symbol;
  /// \brief Bandwidth configuration index, parameter \f$C_{SRS}\f$.
  ///
  /// It is given by the field \e c-SRS contained in the higher-layer parameter \e freqHopping.
  ///
  /// The valid range is {0...63}.
  bounded_integer<uint8_t, 0, 63> configuration_index;
  /// \brief Sequence identifier to initialize the pseudo-random group and sequence hopping, parameter
  /// \f$n^{SRS}_{ID}\f$.
  ///
  /// It is given by the higher-layer parameter \e sequenceId.
  ///
  /// The valid range is {0...1023}.
  bounded_integer<uint16_t, 0, 1023> sequence_id;
  /// \brief Bandwidth index, parameter \f$B_{SRS}\f$.
  ///
  /// It is given by the field \e b-SRS contained in the higher-layer parameter \e freqHopping.
  ///
  /// The valid range is {0...3}.
  bounded_integer<uint8_t, 0, 3> bandwidth_index;
  /// \brief Comb size, parameter \f$K_{TC}\f$.
  ///
  /// It is given by the higher-layer field \e transmissionComb.
  ///
  /// Valid values are 2 and 4.
  tx_comb_size comb_size;
  /// \brief Comb offset, parameter \f$\bar{k}_{TC}\f$.
  ///
  /// It is given by the higher-layer field \e combOffset-n2 or \e combOffset-n4 contained in the parameter
  /// \e transmissionComb.
  ///
  /// The valid range is {0...\f$K_{TC}-1\f$}.
  bounded_integer<uint8_t, 0, 3> comb_offset;
  /// \brief Cyclic shift, parameter \f$n^{CS}_{SRS}\f$.
  ///
  /// It is given by the field \e cyclicShift contained in the higher-layer parameter \e transmissionComb.
  ///
  /// The valid range is:
  /// - {0...7} if \f$K_{TC} = 2\f$; and
  /// - {0...11} if \f$K_{TC} = 4\f$.
  bounded_integer<uint8_t, 0, 11> cyclic_shift;
  /// \brief Frequency domain position, parameter \f$n_{RRC}\f$.
  ///
  /// It is given by the higher layer parameter \e freqDomainPosition.
  ///
  /// The valid range is {0...67}.
  bounded_integer<uint8_t, 0, 67> freq_position;
  /// \brief Frequency domain shift, parameter \f$n_{shift}\f$.
  ///
  /// It is given by the higher layer parameter \e freqDomainShift.
  ///
  /// The valid range is {0...268}.
  bounded_integer<uint16_t, 0, 268> freq_shift;
  /// \brief Frequency hopping, parameter \f$b_{hop}\f$.
  ///
  /// It is given by the field \e b-hop contained in the higher layer parameter \e freqHopping.
  ///
  /// The valid range is {0...3}.
  bounded_integer<uint8_t, 0, 3> freq_hopping;
  /// \brief Group or sequence hopping configuration.
  ///
  /// It is given by the higher layer parameter \e groupOrSequenceHopping.
  srs_group_or_sequence_hopping hopping;
  /// \brief Set if frequency hopping is enabled and the SRS resource is either periodic or semi-persistent.
  ///
  /// The periodicity and offset are given by the higher layer parameter \e resourceType.
  std::optional<periodicity_and_offset> periodicity;

  /// \brief Determines if frequency hopping is enabled.
  ///
  /// Frequency hopping is enabled if \f$b_{hop}<B_{SRS}\f$. Otherwise, it is disabled.
  bool has_frequency_hopping() const { return freq_hopping < bandwidth_index; }

  /// \brief Checks the resource parameter combination is valid.
  bool is_valid() const
  {
    // Make sure the comb offset is according to the comb size.
    if (comb_offset >= static_cast<uint8_t>(comb_size)) {
      return false;
    }

    // The cyclic shift must not exceed 7 when the comb size is 2.
    if ((comb_size == tx_comb_size::n2) && (cyclic_shift > 7)) {
      return false;
    }

    return true;
  }
};

} // namespace ocudu
