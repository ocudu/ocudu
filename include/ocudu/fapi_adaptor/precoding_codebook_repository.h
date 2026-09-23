// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/precoding_beamforming_composite.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief Precoding codebook repository.
///
/// The repository stores precoding configurations. A precoding matrix index selects one of them. A configuration
/// contains a precoding matrix and one beam for each of its ports. The upper physical layer applies the matrix. The
/// beams are applied by the lower physical layer, or by the radio unit when the radio unit does the beamforming.
class precoding_codebook_repository
{
public:
  explicit precoding_codebook_repository(std::vector<precoding_beamforming_composite> repo_) : repo(std::move(repo_))
  {
    ocudu_assert(!repo.empty(), "Empty container");
  }

  /// Iterators.
  std::vector<precoding_beamforming_composite>::const_iterator begin() const { return repo.begin(); }
  std::vector<precoding_beamforming_composite>::const_iterator end() const { return repo.end(); }

  /// \brief Returns the precoding configuration of the given index.
  ///
  /// Index value must be valid, i.e. a precoding configuration must exist in the repository for that index.
  const precoding_beamforming_composite& get_precoding_config(unsigned index) const;

  /// Returns the number of configurations in the repository.
  unsigned size() const { return repo.size(); }

private:
  std::vector<precoding_beamforming_composite> repo;
};

} // namespace fapi_adaptor
} // namespace ocudu
