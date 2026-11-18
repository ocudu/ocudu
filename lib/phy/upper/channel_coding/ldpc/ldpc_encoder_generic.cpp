// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ldpc_encoder_generic.h"
#include "ocudu/ocuduvec/binary.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/support/math/bit_ops.h"
#include <cstring>

using namespace ocudu;
using namespace ocudu::ldpc;

/// Number of punctured systematic nodes at the beginning of the codeblock, as per TS38.212 Section 5.3.2.
static constexpr unsigned nof_punctured_nodes = 2;

/// Adds a variable node, circularly shifted to the left by \c shift bits, to \c out modulo 2.
static void add_shifted_node(span<uint8_t> out, const uint8_t* doubled_node, unsigned shift)
{
  const uint8_t* data      = doubled_node + shift / 8;
  unsigned       bit_shift = shift % 8;
  for (unsigned i_byte = 0, i_byte_end = out.size(); i_byte != i_byte_end; ++i_byte) {
    out[i_byte] ^= static_cast<uint8_t>((data[i_byte] << bit_shift) | (data[i_byte + 1] >> (8 - bit_shift)));
  }
}

/// \brief Reads 64 consecutive bits from an MSB-first packed buffer.
///
/// \param[in] data Packed bits, most significant bit first.
/// \param[in] pos  Position of the first bit to read.
/// \return The 64 bits, with the bit at \c pos in the most significant position.
/// \remark The nine bytes starting at byte <tt>pos / 8</tt> must be readable.
static uint64_t load_bits(const uint8_t* data, unsigned pos)
{
  uint64_t word;
  std::memcpy(&word, data + pos / 8, sizeof(word));
  word           = __builtin_bswap64(word);
  unsigned shift = pos % 8;
  return (shift == 0) ? word : ((word << shift) | (data[pos / 8 + sizeof(word)] >> (8 - shift)));
}

/// Writes the 64 bits at \c node starting at bit \c pos, most significant bit first. The bits before \c pos are
/// preserved, and nine bytes from \c pos / 8 must be writable.
static void store_64bits(uint8_t* data, unsigned pos, const uint8_t* node)
{
  uint64_t word;
  std::memcpy(&word, node, sizeof(word));
  word           = __builtin_bswap64(word);
  unsigned shift = pos % 8;
  uint8_t  head  = data[pos / 8] & mask_msb_ones<uint8_t>(shift);
  uint64_t out   = __builtin_bswap64((word >> shift) | (static_cast<uint64_t>(head) << 56));
  std::memcpy(data + pos / 8, &out, sizeof(out));
  data[pos / 8 + 8] = static_cast<uint8_t>(word << (8 - shift));
}

/// Writes the first \c lifting_size bits of \c node twice in a row, most significant bit first.
static void store_doubled_node(uint8_t* doubled_node, uint64_t node, unsigned lifting_size)
{
  node                          = node & mask_msb_ones<uint64_t>(lifting_size);
  std::array<uint64_t, 2> words = {__builtin_bswap64(node | (node >> lifting_size)),
                                   __builtin_bswap64(node << (64 - lifting_size))};
  std::memcpy(doubled_node, words.data(), sizeof(words));
}

void ldpc_encoder_generic::compute_check_node(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const
{
  if (lifting_size <= 64) {
    // Nodes of up to 64 bits fit in a single 64-bit register.
    uint64_t sum = 0;
    for_each_connected_node(i_check, nof_var_nodes, [&sum](const uint8_t* doubled_node, unsigned shift) {
      sum ^= load_bits(doubled_node, shift);
    });
    sum = __builtin_bswap64(sum);
    std::memcpy(out.data(), &sum, sizeof(sum));
    return;
  }

  // Rounding up to whole vector registers avoids scalar remainders in the vectorized loop.
  unsigned nof_bytes = divide_ceil(get_node_nof_bytes(), 16U) * 16;
  out                = out.first(nof_bytes);
  ocuduvec::zero(out);
  for_each_connected_node(i_check, nof_var_nodes, [out](const uint8_t* doubled_node, unsigned shift) {
    add_shifted_node(out, doubled_node, shift);
  });
}

void ldpc_encoder_generic::double_node(span<uint8_t> doubled_node) const
{
  if (lifting_size % 8 != 0) {
    store_doubled_node(doubled_node.data(), load_bits(doubled_node.data(), 0), lifting_size);
    return;
  }

  // Copies of constant size are inlined by the compiler, unlike overlapping moves. The bytes beyond the second copy are
  // padding.
  std::array<uint8_t, max_node_nof_bytes> first;
  std::memcpy(first.data(), doubled_node.data(), max_node_nof_bytes);
  std::memcpy(doubled_node.data() + lifting_size / 8, first.data(), max_node_nof_bytes);
}

void ldpc_encoder_generic::load_input(const bit_buffer& in)
{
  // The padding after the message allows reading every node with a constant size.
  std::array<uint8_t, MAX_MESSAGE_SIZE / 8 + max_node_nof_bytes> message;
  ocuduvec::copy(span<uint8_t>(message).first(in.get_buffer().size()), in.get_buffer());
  ocuduvec::zero(span<uint8_t>(message).subspan(in.get_buffer().size(), max_node_nof_bytes));

  if (lifting_size % 8 != 0) {
    for (unsigned i_node = 0; i_node != bg_K; ++i_node) {
      store_doubled_node(nodes[i_node].data(), load_bits(message.data(), i_node * lifting_size), lifting_size);
    }
    return;
  }

  // Each systematic node occupies a whole number of bytes of the message and is stored twice in a row (same layout as
  // double_node when lifting_size is byte-aligned).
  unsigned nof_bytes = get_node_nof_bytes();
  for (unsigned i_node = 0; i_node != bg_K; ++i_node) {
    span<const uint8_t> message_padded_node =
        span<const uint8_t>(message).subspan(i_node * nof_bytes, max_node_nof_bytes);
    span<uint8_t> node = nodes[i_node];
    ocuduvec::copy(node.first(max_node_nof_bytes), message_padded_node);
    ocuduvec::copy(node.subspan(nof_bytes, max_node_nof_bytes), message_padded_node);
  }
}

void ldpc_encoder_generic::preprocess_systematic_bits()
{
  for (unsigned i_check = 0; i_check != bg_hr_parity_nodes; ++i_check) {
    compute_check_node(auxiliary[i_check], i_check, bg_K);
  }
}

void ldpc_encoder_generic::encode_high_rate()
{
  // Processing the padding bytes as well lets the compiler vectorize the loops without scalar remainders.
  static constexpr unsigned nof_bytes = max_node_nof_bytes;

  auto aux    = [this](unsigned i) { return span<const uint8_t>(auxiliary[i]).first(nof_bytes); };
  auto parity = [this](unsigned i) { return span<uint8_t>(nodes[bg_K + i]).first(nof_bytes); };

  // The high-rate region has the same structure for all graphs: the first parity node is the sum of the auxiliary
  // nodes, circularly shifted for BG1 with lifting index 6 and for BG2 with lifting index other than 3 and 7.
  // Otherwise, the first parity node contributes circularly shifted by one to the second and fourth parity nodes.
  // Either way, a single circular shift of the sum is needed.
  bool     is_bg1        = current_graph->get_base_graph() == ldpc_base_graph_type::BG1;
  unsigned ls_index      = current_graph->get_lifting_index();
  bool     is_p0_shifted = is_bg1 ? (ls_index == 6) : ((ls_index != 3) && (ls_index != 7));
  unsigned p0_shift      = is_bg1 ? (lifting_size - 105 % lifting_size) : (lifting_size - 1);
  unsigned sum_shift     = is_p0_shifted ? p0_shift : 1;

  std::array<uint8_t, doubled_node_nof_bytes> sum_buffer = {};
  span<uint8_t>                               sum        = span<uint8_t>(sum_buffer).first(nof_bytes);
  ocuduvec::binary_xor(sum, aux(0), aux(1));
  ocuduvec::binary_xor(sum, sum, aux(2));
  ocuduvec::binary_xor(sum, sum, aux(3));
  double_node(sum_buffer);

  std::array<uint8_t, nof_bytes> p0_contrib = {};
  add_shifted_node(p0_contrib, sum_buffer.data(), sum_shift);
  ocuduvec::copy(parity(0), is_p0_shifted ? span<const uint8_t>(p0_contrib) : sum);

  ocuduvec::binary_xor(parity(1), aux(0), p0_contrib);
  ocuduvec::binary_xor(parity(3), aux(3), p0_contrib);
  if (is_bg1) {
    ocuduvec::binary_xor(parity(2), aux(2), parity(3));
  } else {
    ocuduvec::binary_xor(parity(2), aux(1), parity(1));
  }
  for (unsigned i_parity = 0; i_parity != bg_hr_parity_nodes; ++i_parity) {
    double_node(nodes[bg_K + i_parity]);
  }
}

void ldpc_encoder_generic::write_codeblock(span<uint8_t> out, unsigned offset) const
{
  ocudu_assert(out.size() + offset <= bg_N_short * lifting_size,
               "The output size (i.e., {}) plus the offset (i.e., {}) exceeds the codeblock length (i.e., {}).",
               out.size(),
               offset,
               bg_N_short * lifting_size);

  // Rate matching as per TS38.212 Section 5.4.2.1 always reads from a lifted-node boundary, so node_offset is zero in
  // 3GPP operation, but the ldpc_encoder_buffer interface allows arbitrary bit offsets.
  unsigned first_node  = nof_punctured_nodes + offset / lifting_size;
  unsigned node_offset = offset % lifting_size;
  unsigned nof_nodes =
      divide_ceil(node_offset + static_cast<unsigned>(out.size()), static_cast<unsigned>(lifting_size));

  // Unpacking the whole window at once is considerably faster than unpacking node by node. The padding allows writing
  // whole nodes in place.
  static_bit_buffer<MAX_CODEBLOCK_SIZE> packed((nof_nodes - 1) * lifting_size + 8 * max_node_nof_bytes);
  for (unsigned i_node = first_node, i_node_end = first_node + nof_nodes; i_node != i_node_end; ++i_node) {
    // Nodes starting at a byte boundary are written in place. The bits beyond the lifting size are overwritten by the
    // next node.
    unsigned                                i_packed_bit = (i_node - first_node) * lifting_size;
    bool                                    is_in_place  = (i_packed_bit % 8 == 0);
    std::array<uint8_t, max_node_nof_bytes> buffer;
    span<uint8_t> node = is_in_place ? packed.get_buffer().subspan(i_packed_bit / 8, max_node_nof_bytes) : buffer;

    // The extended region nodes are computed on demand from the stored nodes.
    if (i_node < bg_K + bg_hr_parity_nodes) {
      std::memcpy(node.data(), nodes[i_node].data(), max_node_nof_bytes);
    } else {
      compute_check_node(node, i_node - bg_K, bg_K + bg_hr_parity_nodes);
    }
    if (!is_in_place) {
      // All lifting sizes greater than 64 are multiples of 8, so unaligned nodes fit in 64 bits.
      store_64bits(packed.get_buffer().data(), i_packed_bit, node.data());
    }
  }
  ocuduvec::bit_unpack(out, packed, node_offset);
}
