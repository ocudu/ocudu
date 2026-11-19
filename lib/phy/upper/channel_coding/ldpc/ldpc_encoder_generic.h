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
/// into a read at a bit offset. Little-endian \c uint64_t node paths use \c __builtin_bswap64 after \c memcpy for
/// MSB-first packed bytes.
class ldpc_encoder_generic : public ldpc_encoder_impl
{
protected:
  /// Maximum number of bytes of a packed lifted node.
  static constexpr unsigned max_node_nof_bytes = divide_ceil(ldpc::MAX_LIFTING_SIZE, 8);

  /// Returns the number of bytes of a packed lifted node.
  unsigned get_node_nof_bytes() const { return divide_ceil(lifting_size, 8U); }

  /// \brief Computes the sum of the variable nodes connected to a check node, modulo 2.
  ///
  /// Only the variable nodes with index lower than \c nof_var_nodes are added:
  /// - High-rate check nodes: \c nof_var_nodes is \f$K_{BG}\f$, so only the systematic nodes are added. The result is
  ///   an intermediate sum for solving the high-rate parity nodes.
  /// - Extended region check nodes: \c nof_var_nodes is \f$K_{BG} + 4\f$, so the systematic and the four high-rate
  ///   parity nodes are added. The result is the parity node of the check node.
  ///
  /// where \f$K_{BG}\f$ is the number of systematic nodes of the base graph, that is, 22 for BG1 and 10 for BG2.
  ///
  /// \param[out] out           Packed sum of one lifted node, with room for \c max_node_nof_bytes bytes. The bits
  ///                           beyond the lifting size are left undefined.
  /// \param[in]  i_check       Index of the check node, that is, the row of the base graph.
  /// \param[in]  nof_var_nodes Number of leading variable nodes added to the sum. The variable nodes from this index
  ///                           onward are excluded.
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
