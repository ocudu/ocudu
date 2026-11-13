// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/adt/strong_type.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/support/ocudu_assert.h"
#include <optional>
#include <vector>

namespace ocudu {

struct pucch_resource_set_config_id_tag;
/// ID of a configuration of the HARQ-ACK PUCCH resource sets of a cell. Range: 0 to the number of configurations - 1.
using pucch_resource_set_config_id =
    strong_type<uint8_t, struct pucch_resource_set_config_id_tag, strong_equality, strong_increment_decrement>;
struct pucch_sr_resource_id_tag;
/// ID of a PUCCH resource of a cell used for SR. Range: 0 to the number of SR resources - 1.
using pucch_sr_resource_id =
    strong_type<uint8_t, struct pucch_sr_resource_id_tag, strong_equality, strong_increment_decrement>;
struct pucch_csi_resource_id_tag;
/// ID of a PUCCH resource of a cell used for CSI. Range: 0 to the number of CSI resources - 1.
using pucch_csi_resource_id =
    strong_type<uint8_t, struct pucch_csi_resource_id_tag, strong_equality, strong_increment_decrement>;

/// \brief UE-dedicated PUCCH resources of a cell, and a mapping from the IDs allocated by the RRM to a UE to their
/// corresponding resources.
///
/// In the F0+F2 case, each SR (F0) and CSI (F2) resource has an alternative resource of the other format with the same
/// symbols (SR_F2, CSI_F0). The HARQ-ACK uses it, so that the UE multiplexes the UCI types on a single PUCCH. See
/// \c make_ue_pucch_res_layout.
struct cell_pucch_resources {
  /// Cell resources of the two HARQ-ACK PUCCH resource sets of a resource set configuration.
  struct harq_resource_set_config {
    static_vector<pucch_res_id_t, MAX_NOF_PUCCH_RESOURCES_PER_PUCCH_RESOURCE_SET> set_0;
    static_vector<pucch_res_id_t, MAX_NOF_PUCCH_RESOURCES_PER_PUCCH_RESOURCE_SET> set_1;
  };
  /// A cell resource for SR or CSI, with its alternative resource in the F0+F2 case (SR_F2 or CSI_F0, respectively).
  struct res_with_alt {
    pucch_res_id_t                res_id;
    std::optional<pucch_res_id_t> alt_res_id;
  };

  /// UE-dedicated PUCCH resources of the cell.
  std::vector<pucch_resource> resources;
  /// HARQ-ACK resource sets, indexed by the ID of the resource set configuration.
  std::vector<harq_resource_set_config> harq;
  /// SR resources, indexed by the ID of the SR resource.
  std::vector<res_with_alt> sr;
  /// CSI resources, indexed by the ID of the CSI resource.
  std::vector<res_with_alt> csi;

  /// Gets a UE-dedicated PUCCH resource by its resource ID.
  const pucch_resource& get(pucch_res_id_t res_id) const
  {
    ocudu_assert(res_id.ded_id() < resources.size(),
                 "PUCCH cell resource ID {} exceeds the size of the dedicated cell resource list ({})",
                 res_id.ded_id(),
                 resources.size());
    return resources[res_id.ded_id()];
  }

  /// Gets the HARQ-ACK resource sets of a resource set configuration.
  const harq_resource_set_config& get_harq(pucch_resource_set_config_id id) const
  {
    ocudu_assert(id.value() < harq.size(), "Invalid resource set configuration ID={}", id.value());
    return harq[id.value()];
  }

  /// Gets the cell resources of an SR resource.
  const res_with_alt& get_sr(pucch_sr_resource_id id) const
  {
    ocudu_assert(id.value() < sr.size(), "Invalid SR resource ID={}", id.value());
    return sr[id.value()];
  }

  /// Gets the cell resources of a CSI resource.
  const res_with_alt& get_csi(pucch_csi_resource_id id) const
  {
    ocudu_assert(id.value() < csi.size(), "Invalid CSI resource ID={}", id.value());
    return csi[id.value()];
  }
};

} // namespace ocudu
