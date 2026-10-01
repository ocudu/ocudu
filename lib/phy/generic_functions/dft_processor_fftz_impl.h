// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/generic_functions/dft_processor.h"
#include <aoclfftz.h>
#include <array>
#include <cstring>
#include <memory>
#include <mutex>
#include <vector>

namespace ocudu {

/// Configuration parameters specific to the AOCL-FFTZ library.
struct dft_processor_fftz_configuration {
  /// \brief FFTZ optimization level, with range {0, 1, ..., 3}.
  ///
  /// Controls whether SIMD instructions are enabled. According to the FFTZ documentation:
  /// - 0: non-SIMD optimizations
  /// - 1: AVX128 optimizations
  /// - 2: AVX256 optimizations
  /// - 3: AVX512 optimizations
  int opt_level = 0;
  /// Enable bit reproducibility if deterministic results are required. It may decrease performance.
  bool bit_reproducibility = false;
};

/// DFT processor using AOCL-FFTZ.
class dft_processor_fftz_impl : public dft_processor
{
private:
  /// DFT direction.
  direction dir;
  /// DFT input buffer.
  std::vector<cf_t> input;
  /// DFT output buffer.
  std::vector<cf_t> output;
  // FFTZ handle.
  void* handle = nullptr;

public:
  /// \brief Constructs a DFT processor based on the AOCL-FFTZ library.
  /// \param [in] fftz_config Configuration parameters specific to the FFTZ library.
  /// \param [in] dft_config Generic DFT processor configuration parameters.
  dft_processor_fftz_impl(const dft_processor_fftz_configuration& fftz_config, const configuration& dft_config);

  /// Destroys the FFTZ problem handle.
  ~dft_processor_fftz_impl() override;

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
