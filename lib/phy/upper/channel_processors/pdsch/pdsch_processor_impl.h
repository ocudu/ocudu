// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_encoder.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_modulator.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "ocudu/phy/upper/signal_processors/pdsch/dmrs_pdsch_processor.h"
#include "ocudu/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"

namespace ocudu {

/// Implements a generic PDSCH processor.
class pdsch_processor_impl : public pdsch_processor
{
public:
  /// \brief Creates a generic PDSCH processor.
  pdsch_processor_impl(std::unique_ptr<pdsch_encoder>        encoder_,
                       std::unique_ptr<pdsch_modulator>      modulator_,
                       std::unique_ptr<dmrs_pdsch_processor> dmrs_,
                       std::unique_ptr<ptrs_pdsch_generator> ptrs_) :
    encoder(std::move(encoder_)), modulator(std::move(modulator_)), dmrs(std::move(dmrs_)), ptrs(std::move(ptrs_))
  {
    ocudu_assert(encoder != nullptr, "Invalid encoder pointer.");
    ocudu_assert(modulator != nullptr, "Invalid modulator pointer.");
    ocudu_assert(dmrs != nullptr, "Invalid dmrs pointer.");
    ocudu_assert(ptrs != nullptr, "Invalid ptrs pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override;

private:
  /// \brief Encodes a codeword as per TS38.212 Section 7.2.
  ///
  /// \param[in]     data        Provides the data to transmit as packed bits.
  /// \param[in]     codeword_id Indicates the codeword identifier.
  /// \param[in]     nof_layers  Indicates the number of layers the codeword is mapped on to.
  /// \param[in]     Nre         Indicates the number of resource elements used for PDSCH mapping.
  /// \param[in]     pdu         Provides the PDSCH processor PDU.
  /// \return A view of the encoded codeword.
  const bit_buffer&
  encode(span<const uint8_t> data, unsigned codeword_id, unsigned nof_layers, unsigned Nre, const pdu_t& pdu);

  /// \brief Modulates a PDSCH transmission as per TS38.211 Section 7.3.1.
  /// \param[out] grid          Resource grid writer interface.
  /// \param[in] temp_codewords Provides the encoded codewords.
  /// \param[in] pdu            Provides the PDSCH processor PDU.
  void modulate(resource_grid_writer& grid, span<const bit_buffer> temp_codewords, const pdu_t& pdu);

  std::unique_ptr<pdsch_encoder>                                  encoder;
  std::unique_ptr<pdsch_modulator>                                modulator;
  std::unique_ptr<dmrs_pdsch_processor>                           dmrs;
  std::unique_ptr<ptrs_pdsch_generator>                           ptrs;
  std::array<uint8_t, pdsch_constants::CODEWORD_MAX_SIZE.value()> temp_unpacked_codeword;
  std::array<static_bit_buffer<pdsch_constants::CODEWORD_MAX_SIZE.value()>, MAX_NOF_TRANSPORT_BLOCKS>
      temp_packed_codewords;
};

} // namespace ocudu
