// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_encoder.h"
#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_modulator.h"
#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_processor.h"
#include "ocudu/phy/upper/signal_processors/pdcch/dmrs_pdcch_processor.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"

namespace ocudu {

/// Describes the PDCCH processor generic implementation constructor configuration.
struct pdcch_processor_config_t {
  /// Provides the PDCCH encoder. Ownership is transferred to the PDCCH processor.
  std::unique_ptr<pdcch_encoder> encoder;
  /// Provides the PDCCH modulator. Ownership is transferred to the PDCCH processor.
  std::unique_ptr<pdcch_modulator> modulator;
  /// Provides the DMRS for PDCCH. Ownership is transferred to the PDCCH processor.
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
};

/// Describes a generic PDCCH processor.
class pdcch_processor_impl : public pdcch_processor
{
private:
  /// Provides the PDCCH encoder.
  std::unique_ptr<pdcch_encoder> encoder;
  /// Provides the PDCCH modulator.
  std::unique_ptr<pdcch_modulator> modulator;
  /// Provides the DMRS for PDCCH.
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
  /// Temporary encoded bits.
  std::array<uint8_t, pdcch_constants::MAX_NOF_BITS> temp_encoded;

  /// \brief Calculates the number of encoded bits for a PDCCH transmission.
  /// \param[in] aggregation_level Indicates the PDCCH transmission aggregation level.
  /// \return The number of encoded bits.
  static constexpr unsigned nof_encoded_bits(unsigned aggregation_level)
  {
    return aggregation_level * pdcch_constants::NOF_REG_PER_CCE * pdcch_constants::NOF_RE_PDCCH_PER_RB * 2;
  }

  /// \brief Computes the Common Resource Blocks (CRBs) allocation mask as per TS38.211 Section 7.3.2.2.
  /// \param[in] coreset Provides CORESET parameters.
  /// \param[in] dci Provides DCI parameters.
  /// \return The PRB allocation mask for the PDCCH transmission.
  static crb_bitmap compute_rb_mask(const coreset_description& coreset, const dci_description& dci);

public:
  /// Generic PDSCH modulator instance constructor.
  pdcch_processor_impl(std::unique_ptr<pdcch_encoder>        encoder_,
                       std::unique_ptr<pdcch_modulator>      modulator_,
                       std::unique_ptr<dmrs_pdcch_processor> dmrs_) :
    encoder(std::move(encoder_)), modulator(std::move(modulator_)), dmrs(std::move(dmrs_))
  {
    ocudu_assert(encoder, "Invalid encoder.");
    ocudu_assert(modulator, "Invalid modulator.");
    ocudu_assert(dmrs, "Invalid DMRS generator.");
  }

  // See interface for documentation.
  void process(resource_grid_writer& grid, const pdu_t& pdu) override;
};

} // namespace ocudu
