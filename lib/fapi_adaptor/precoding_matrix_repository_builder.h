// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi_adaptor/precoding_matrix_repository.h"
#include "ocudu/ran/beamforming/beam_identifier_helpers.h"
#include <memory>

namespace ocudu {
namespace fapi_adaptor {

/// Precoding matrix repository builder.
class precoding_matrix_repository_builder
{
public:
  explicit precoding_matrix_repository_builder(unsigned size) { repository.reserve(size); }

  /// Adds the given composite precoding configuration to the repository with the given index.
  void add(unsigned index, const precoding_beamforming_composite& composite)
  {
    if (index >= repository.size()) {
      repository.resize(index + 1U);
    }

    repository[index] = composite;
  }

  /// \brief Adds the given precoding matrix to the repository with the given index.
  ///
  /// The matrix contains the complete precoding. Its ports select the antenna ports. No beamforming applies.
  void add(unsigned index, const precoding_weight_matrix& precoding)
  {
    add(index, precoding_beamforming_composite{precoding, get_default_beam_list(precoding.get_nof_ports())});
  }

  /// Builds and returns a precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> build()
  {
    return std::make_unique<precoding_matrix_repository>(std::move(repository));
  }

private:
  std::vector<precoding_beamforming_composite> repository;
};

} // namespace fapi_adaptor
} // namespace ocudu
