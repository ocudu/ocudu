// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/generic_functions/dft_processor.h"
#include <memory>
#include <vector>

namespace ocudu {

/// Generic interface of an N-point DFT calculator.
class generic_dft_N
{
public:
  virtual ~generic_dft_N() = default;
  /// Computes the N-point of \c in and writes the result in \c out.
  virtual void run(cf_t* out, const cf_t* in) const = 0;
};

/// Describes a DFT processor class configuration based on the FFTW library.
class dft_processor_generic_impl : public dft_processor
{
private:
  /// Stores the DFT direction.
  direction dir;
  /// DFT input buffer ownership.
  std::vector<cf_t> input;
  /// DFT output buffer ownership.
  std::vector<cf_t> output;
  /// Generic FFT.
  std::unique_ptr<generic_dft_N> generic_dft;

public:
  /// \brief Constructs a generic DFT processor.
  /// \param [in] dft_config Provides the generic DFT processor parameters.
  dft_processor_generic_impl(const configuration& dft_config);

  /// Determines whether the initialization was successful.
  bool is_valid() const { return generic_dft != nullptr; }

  // See interface for documentation.
  direction get_direction() const override { return dir; }

  // See interface for documentation.
  unsigned get_size() const override { return input.size(); }

  // See interface for documentation.
  span<cf_t> get_input() override { return input; }

  // See interface for documentation.
  span<const cf_t> run() override;
};

} // namespace ocudu
