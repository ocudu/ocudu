// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/phy/antenna_ports.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/ssb/ssb_configuration.h"
#include "ocudu/ran/ssb/ssb_properties.h"

namespace ocudu {

class resource_grid_writer;

/// Describes the SSB processor interface.
class ssb_processor
{
public:
  /// Defines the MIB payload size.
  static constexpr unsigned MIB_PAYLOAD_SIZE = 24;

  /// Describes the SS/PBCH Block PDU.
  struct pdu_t {
    /// Current slot context.
    slot_point slot;
    /// Physical Cell identifier.
    pci_t phys_cell_id;
    /// PSS power level allocation in dB, relative to SSS.
    ssb_pss_to_sss_epre beta_pss;
    /// SSB opportunity index in a burst.
    ssb_id_t ssb_idx;
    /// Maximum number of SS/PBCH block candidates in a 5ms burst, described in TS38.213 Section 4.1.
    unsigned L_max;
    /// Higher layer parameter \e subCarrierSpacingCommon as per TS38.331 Section 6.2.2 - MIB.
    subcarrier_spacing common_scs;
    /// \brief Alignment offset between the resource grid and the SS/PBCH block.
    /// \sa ssb_subcarrier_offset for more details.
    ssb_subcarrier_offset subcarrier_offset;
    /// \brief Start of the SS/PBCH block relative to Point A in PRB.
    ///
    /// The lowest subcarrier of the resource grid overlaps with the lowest subcarrier of the configured bandwidth. It
    /// might not coincide with the parameter \e absoluteFrequencyPointA in the IE \e FrequencyInfoDL. If the parameter
    /// \e offsetToCarrier in the IE \e SCS-SpecificCarrier in TS38.331 Section 6.3.2 is non-zero, the offset to Point A
    /// shall be adjusted accordingly with \e offsetToCarrier so that Point A aligns with the start of the resource
    /// grid.
    ///
    /// \sa ssb_offset_to_pointA for more details.
    ssb_offset_to_pointA offset_to_pointA;
    /// SS/PBCH pattern case.
    ssb_pattern_case pattern_case;
    /// \brief Packed MIB payload.
    /// \remark The MIB contents are described by the Information Element \e MIB in TS38.331 Section 6.2.2.
    std::array<uint8_t, MIB_PAYLOAD_SIZE> mib_payload;
    /// Port indexes to map the SS/PBCH transmission.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~ssb_processor() = default;

  /// \brief Processes the SS/PBCH Block PDU and writes the resultant signal in the given grid.
  /// \param[out] grid Destination resource grid.
  /// \param[in]  pdu  Properties to generate the SSB message.
  virtual void process(resource_grid_writer& grid, const pdu_t& pdu) = 0;
};

/// \brief Describes the SSB processor validator interface.
class ssb_pdu_validator
{
public:
  /// Default destructor.
  virtual ~ssb_pdu_validator() = default;

  /// \brief Validates SSB processor configuration parameters.
  /// \param[in] pdu SS/PBCH Block PDU to be validated.
  /// \param[in] cell_bandwith_prbs Carrier bandwidth in PRBs based on common subcarrier spacing.
  /// \return A success if the parameters contained in \c pdu are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const ssb_processor::pdu_t& pdu, unsigned cell_bandwith_prbs) const = 0;
};

} // namespace ocudu
