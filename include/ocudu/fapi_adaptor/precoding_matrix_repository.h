// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/precoding_beamforming_composite.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief Precoding matrix repository.
///
/// The repository stores composite precoding configurations. Each configuration has a precoding matrix index. A
/// configuration contains a precoding matrix and one beam for each of its ports. The lower physical layer or a
/// Category B O-RU applies the beamforming.
class precoding_matrix_repository
{
public:
  explicit precoding_matrix_repository(std::vector<precoding_beamforming_composite> repo_) : repo(std::move(repo_))
  {
    ocudu_assert(!repo.empty(), "Empty container");
  }

  /// Iterators.
  std::vector<precoding_beamforming_composite>::const_iterator begin() const { return repo.begin(); }
  std::vector<precoding_beamforming_composite>::const_iterator end() const { return repo.end(); }

  /// \brief Returns the composite precoding of the given index.
  ///
  /// Index value must be valid, i.e. a precoding configuration must exist in the repository for that index.
  const precoding_beamforming_composite& get_precoding(unsigned index) const;

  /// Returns the number of configurations in the repository.
  unsigned size() const { return repo.size(); }

private:
  std::vector<precoding_beamforming_composite> repo;
};

} // namespace fapi_adaptor
} // namespace ocudu
