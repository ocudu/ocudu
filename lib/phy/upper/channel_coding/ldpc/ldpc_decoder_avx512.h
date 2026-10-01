// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC decoder - declaration of the AVX512-optimized implementation.

#pragma once

#include "ldpc_decoder_impl.h"

namespace ocudu {

/// LDPC decoder implementation based on AVX512 intrinsics.
class ldpc_decoder_avx512 : public ldpc_decoder_impl
{
public:
  /// Constructor: configures the \c force_decoding and \c early_stop_syndrome flags.
  ldpc_decoder_avx512(bool cfg_force_decoding, bool early_stop_syndrome);

private:
  // See ldpc_decoder_impl for the documentation.
  void specific_init() override;

  void compute_soft_bits(span<log_likelihood_ratio>       this_soft_bits,
                         span<const log_likelihood_ratio> this_var_to_check,
                         span<const log_likelihood_ratio> this_check_to_var) override;

  void compute_var_to_check_msgs(span<log_likelihood_ratio>       this_var_to_check,
                                 span<const log_likelihood_ratio> this_soft_bits,
                                 span<const log_likelihood_ratio> this_check_to_var) override;

  void analyze_var_to_check_msgs(span<log_likelihood_ratio>       min_var_to_check,
                                 span<log_likelihood_ratio>       second_min_var_to_check,
                                 span<uint8_t>                    min_var_to_check_index,
                                 span<uint8_t>                    sign_prod_var_to_check,
                                 span<const log_likelihood_ratio> rotated_node,
                                 unsigned                         var_node) override;

  void scale(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) override;

  void compute_check_to_var_msgs(span<log_likelihood_ratio>       this_check_to_var,
                                 span<const log_likelihood_ratio> this_var_to_check,
                                 span<const log_likelihood_ratio> rotated_node,
                                 span<const log_likelihood_ratio> min_var_to_check,
                                 span<const log_likelihood_ratio> second_min_var_to_check,
                                 span<const uint8_t>              min_var_to_check_index,
                                 span<const uint8_t>              sign_prod_var_to_check,
                                 unsigned                         shift,
                                 unsigned                         var_node) override;

  /// Auxiliary buffer to store the rotated check-to-variable messages.
  std::vector<log_likelihood_ratio> help_check_to_var;

  /// Number of AVX512 vectors needed to cover a lifted node.
  unsigned node_size_avx512 = 0;
};

} // namespace ocudu
