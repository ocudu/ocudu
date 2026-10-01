// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "ocudu/phy/upper/channel_processors/pucch/pucch_detector.h"
#include "ocudu/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "ocudu/phy/upper/channel_processors/uci/uci_decoder.h"
#include "ocudu/phy/upper/signal_processors/pucch/dmrs_pucch_estimator.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"

namespace ocudu {

/// Implements a parameter validator for \ref pucch_processor_impl.
class pucch_pdu_validator_impl : public pucch_pdu_validator
{
public:
  /// \brief Constructs a PUCCH processor validator.
  ///
  /// The channel estimate dimensions are used to set the bandwidth, slot duration, number of receive ports and transmit
  /// layers.
  ///
  /// \param[in] ce_dims_ Provides the channel estimates dimensions.
  explicit pucch_pdu_validator_impl(const channel_estimate::channel_estimate_dimensions& ce_dims_) : ce_dims(ce_dims_)
  {
    // Do nothing.
  }

  // See interface for documentation.
  error_type<std::string> is_valid(const pucch_processor::format0_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format1_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format2_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format3_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format4_configuration& config) const override;

private:
  /// Maximum transmit and receive resource grid dimensions handled by the PUCCH processor.
  channel_estimate::channel_estimate_dimensions ce_dims;
};

/// Implementation of the PUCCH processor interface.
class pucch_processor_impl : public pucch_processor
{
public:
  // See pucch_processor interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override;

  // See pucch_processor interface for documentation.
  pucch_format1_map<pucch_processor_result> process(const resource_grid_reader&        grid,
                                                    const format1_batch_configuration& config) override;

  // See pucch_processor interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override;

  // See pucch_processor interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override;

  // See pucch_processor interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override;

  /// PUCCH processor constructor.
  pucch_processor_impl(std::unique_ptr<pucch_pdu_validator>                 pdu_validator_,
                       std::unique_ptr<dmrs_pucch_estimator>                channel_estimator_,
                       std::unique_ptr<pucch_detector>                      detector_,
                       std::unique_ptr<pucch_demodulator>                   demodulator_,
                       std::unique_ptr<uci_decoder>                         decoder_,
                       const channel_estimate::channel_estimate_dimensions& estimates_dimensions) :
    pdu_validator(std::move(pdu_validator_)),
    channel_estimator(std::move(channel_estimator_)),
    detector(std::move(detector_)),
    demodulator(std::move(demodulator_)),
    decoder(std::move(decoder_)),
    estimates(estimates_dimensions)
  {
    ocudu_assert(channel_estimator, "Invalid channel estimator.");
    ocudu_assert(detector, "Invalid detector.");
    ocudu_assert(demodulator, "Invalid PUCCH demodulator.");
    ocudu_assert(decoder, "Invalid UCI decoder.");
  }

private:
  /// PUCCH PDU validator for all formats.
  std::unique_ptr<pucch_pdu_validator> pdu_validator;
  /// Channel estimator.
  std::unique_ptr<dmrs_pucch_estimator> channel_estimator;
  /// PUCCH detector for 1 or 2 bits, using format 0 and 1.
  std::unique_ptr<pucch_detector> detector;
  /// PUCCH demodulator for more than 2 bits, using formats 2, 3 and 4.
  std::unique_ptr<pucch_demodulator> demodulator;
  /// UCI decoder for more than 2 bits, using formats 2, 3 and 4.
  std::unique_ptr<uci_decoder> decoder;
  /// Temporal channel estimates.
  channel_estimate estimates;
  /// Temporal LLR storage.
  std::array<log_likelihood_ratio, pucch_constants::MAX_NOF_LLR> temp_llr;
};

} // namespace ocudu
