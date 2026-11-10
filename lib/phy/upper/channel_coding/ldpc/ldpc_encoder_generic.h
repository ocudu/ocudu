// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC encoder - Declaration of generic implementation.
#pragma once

#include "ldpc_encoder_impl.h"
#include "ocudu/support/math/math_utils.h"

namespace ocudu {

/// \brief Generic LDPC encoder implementation operating on packed bits.
///
/// Each stored variable node holds two consecutive copies of its lifted bits, which turns a circular shift of the node
/// into a read at a bit offset.
class ldpc_encoder_generic : public ldpc_encoder_impl
{
protected:
  /// Maximum number of bytes of a packed lifted node.
  static constexpr unsigned max_node_nof_bytes = divide_ceil(ldpc::MAX_LIFTING_SIZE, 8);

  /// Returns the number of bytes of a packed lifted node.
  unsigned get_node_nof_bytes() const { return divide_ceil(lifting_size, 8U); }

  /// Computes the sum of the variable nodes connected to a check node, modulo 2.
  /// \param[out] out           Packed sum, with room for \c max_node_nof_bytes bytes. The bits beyond the lifting size
  ///                           are left undefined.
  /// \param[in]  i_check       Check node index.
  /// \param[in]  nof_var_nodes Number of variable nodes considered, starting from the first one.
  virtual void compute_check_node(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const;

  /// \brief Calls a function for each variable node connected to a check node.
  ///
  /// The function receives the doubled storage of the variable node and its circular shift in bits.
  template <typename Func>
  void for_each_connected_node(unsigned i_check, unsigned nof_var_nodes, Func&& func) const
  {
    // The adjacency row is sorted in ascending order and padded with NO_EDGE.
    for (uint16_t i_var : current_graph->get_adjacency_row(i_check)) {
      if (i_var >= nof_var_nodes) {
        break;
      }
      func(nodes[i_var].data(), current_graph->get_lifted_node(i_check, i_var));
    }
  }

private:
  // See ldpc_encoder_impl for documentation.
  void load_input(const bit_buffer& in) override;
  // See ldpc_encoder_impl for documentation.
  void preprocess_systematic_bits() override;
  // See ldpc_encoder_impl for documentation.
  void encode_high_rate() override;
  // See ldpc_encoder_buffer interface for documentation.
  void write_codeblock(span<uint8_t> out, unsigned offset) const override;

  /// Repeats the first lifting size bits of a doubled node storage right behind them.
  void double_node(span<uint8_t> doubled_node) const;

  /// Number of bytes of a doubled node storage, including room for SIMD loads beyond the second copy.
  static constexpr unsigned doubled_node_nof_bytes = 2 * max_node_nof_bytes + 32;
  /// Maximum number of stored variable nodes, that is, systematic nodes and high-rate parity nodes.
  static constexpr unsigned max_nof_stored_nodes = ldpc::BG1_N_FULL - ldpc::BG1_M + bg_hr_parity_nodes;

  /// Systematic and high-rate parity nodes, each stored twice in a row.
  std::array<std::array<uint8_t, doubled_node_nof_bytes>, max_nof_stored_nodes> nodes = {};
  /// Contributions of the systematic nodes to the high-rate check nodes.
  std::array<std::array<uint8_t, max_node_nof_bytes>, bg_hr_parity_nodes> auxiliary = {};
};

} // namespace ocudu
