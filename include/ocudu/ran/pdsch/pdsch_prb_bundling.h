// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <variant>

namespace ocudu {

/// \brief Indicates the PRB bundle type and bundle size(s).
/// \remark See TS 38.214, clause 5.1.2.3.
struct prb_bundling {
  struct static_bundling {
    enum class bundling_size { n4, wideband };

    std::optional<bundling_size> sz;

    bool operator==(const static_bundling& rhs) const { return sz == rhs.sz; }
    bool operator!=(const static_bundling& rhs) const { return !(rhs == *this); }
  };

  struct dynamic_bundling {
    enum class bundling_size_set1 { n4, wideband, n2_wideband, n4_wideband };
    enum class bundling_size_set2 { n4, wideband };

    std::optional<bundling_size_set1> sz_set1;
    std::optional<bundling_size_set2> sz_set2;

    bool operator==(const dynamic_bundling& rhs) const { return sz_set1 == rhs.sz_set1 && sz_set2 == rhs.sz_set2; }
    bool operator!=(const dynamic_bundling& rhs) const { return !(rhs == *this); }
  };

  std::variant<static_bundling, dynamic_bundling> bundling;

  bool operator==(const prb_bundling& rhs) const { return bundling == rhs.bundling; }
  bool operator!=(const prb_bundling& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
