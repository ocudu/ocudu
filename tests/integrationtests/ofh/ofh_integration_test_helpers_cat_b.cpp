// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_integration_test_helpers_cat_b.h"
#include "compression/beamforming_weights_compressor.h"
#include "ocudu/ofh/compression/compression_properties.h"
#include "ocudu/ran/beamforming/beam_weights_codebook_generator.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/support/math/math_utils.h"
#include "fmt/format.h"
#include <algorithm>
#include <numeric>

using namespace ocudu;
using namespace ofh;
using namespace test;

std::optional<decoded_cplane_section_type_1> test::decode_cplane_section_type_1(span<const uint8_t> message,
                                                                                unsigned            nof_trx)
{
  // Offset of the radio application header: Ethernet header with VLAN tag and eCPRI header including RTC_ID/SEQ_ID.
  static constexpr unsigned radio_header_offset = 26;
  // Size of the section type 1 common header.
  static constexpr unsigned common_header_size = 8;
  // Size of the section type 1 fields, without extensions.
  static constexpr unsigned section_size = 8;
  // Size of the section extension 1 header: ef/extType, extLen and bfwCompHdr.
  static constexpr unsigned extension_1_header_size = 3;
  // Section extension type 1.
  static constexpr unsigned extension_type_1 = 1;
  // SE1 extLen field specifies the length in 4-Byte words.
  static constexpr unsigned ext_length_unit = 4;

  unsigned msg_size = message.size();

  if (msg_size < radio_header_offset + common_header_size + section_size) {
    return std::nullopt;
  }

  span<const uint8_t>           hdr = message.subspan(radio_header_offset, common_header_size);
  decoded_cplane_section_type_1 decoded;
  decoded.start_symbol = hdr[3] & 0x3f;
  decoded.nof_sections = hdr[4];
  decoded.section_type = hdr[5];

  span<const uint8_t> section = message.subspan(radio_header_offset + common_header_size, section_size);
  decoded.start_prb           = (unsigned(section[1] & 0x03) << 8) | section[2];
  decoded.nof_prb             = section[3];
  decoded.nof_symbols         = section[5] & 0x0f;
  decoded.ef                  = (section[6] & 0x80) != 0;
  decoded.beam_id             = (unsigned(section[6] & 0x7f) << 8) | section[7];

  if (!decoded.ef) {
    return decoded;
  }

  span<const uint8_t> extension = message.last(msg_size - (radio_header_offset + common_header_size + section_size));
  // Return nullopt if the message is too small, or it indicates that more extensions follow it, or the message type is
  // not the SE1.
  if (extension.size() < extension_1_header_size || (extension[0] & 0x80) != 0 ||
      (extension[0] & 0x7f) != extension_type_1) {
    return std::nullopt;
  }

  unsigned              iq_width = extension[2] >> 4;
  ru_compression_params compr_params{to_compression_type(extension[2] & 0x0f), (iq_width == 0) ? 16U : iq_width};
  unsigned              weights_size = get_packed_beamforming_weights_size(nof_trx, compr_params).value();
  unsigned              ext_length   = extension[1] * ext_length_unit;

  // Check the extension length takes padding into account.
  unsigned padded_size = divide_ceil(extension_1_header_size + weights_size, ext_length_unit) * ext_length_unit;
  if (ext_length != padded_size || extension.size() < ext_length) {
    return std::nullopt;
  }

  decoded.extension_1.emplace(decoded_cplane_section_ext_1{
      .compr_params   = compr_params,
      .packed_weights = extension.subspan(extension_1_header_size, weights_size),
  });
  return decoded;
}

namespace {

/// Beam-port patterns of the Category B resource grids. The grids after the last pattern are random.
enum class beam_pattern : unsigned {
  /// Beam 0, transmitted with the wire beam identifier 1.
  first_beam,
  /// Highest beam of the topology.
  last_beam,
  /// Region A beams only, i.e., port-selection beams.
  port_selection_beams,
  /// Region B beams only.
  region_b_beams,
  /// No beam, nothing is transmitted.
  empty,
  /// Beams of both regions.
  mixed_beams,
  /// Random beams.
  random,
};

} // namespace

/// Returns true if the pattern needs region B beams.
static bool uses_region_b_beams(beam_pattern pattern)
{
  return (pattern == beam_pattern::region_b_beams) || (pattern == beam_pattern::mixed_beams);
}

/// Appends up to \c max_num_beams beams, every \c step beams in the range [first, last).
static void append_beams(dl_beam_list& beams, unsigned first, unsigned last, unsigned step, unsigned max_num_beams)
{
  for (unsigned i_beam = first; (i_beam < last) && (beams.size() != max_num_beams); i_beam += step) {
    beams.push_back(i_beam);
  }
}

/// Generates the given number of distinct random beams, sorted as the OFH RU scans the grid ports in increasing order.
static dl_beam_list generate_random_beams(std::mt19937& rgen, unsigned nof_beams, unsigned nof_selected)
{
  std::vector<unsigned> all_beams(nof_beams);
  std::iota(all_beams.begin(), all_beams.end(), 0);

  dl_beam_list beams;
  std::sample(all_beams.begin(), all_beams.end(), std::back_inserter(beams), nof_selected, rgen);
  return beams;
}

dl_beam_list test::generate_beam_pattern(std::mt19937& rgen,
                                         unsigned      grid_index,
                                         unsigned      nof_beams,
                                         unsigned      nof_trx,
                                         unsigned      max_num_beams)
{
  // Spacing between the selected region B beams, so that they are not contiguous.
  static constexpr unsigned region_b_beam_step = 3;

  // Region A holds the port-selection beams [0, nof_trx) and region B the beams of the topology beam grid
  // [nof_trx, nof_beams), see antenna_topology. Topologies with a single antenna port per panel dimension have no
  // region B.
  bool has_region_b_beams = (nof_beams > nof_trx);

  beam_pattern pattern = static_cast<beam_pattern>(std::min(grid_index, static_cast<unsigned>(beam_pattern::random)));
  if (!has_region_b_beams && uses_region_b_beams(pattern)) {
    pattern = beam_pattern::random;
  }

  dl_beam_list beams;
  switch (pattern) {
    case beam_pattern::first_beam:
      beams = {0};
      break;
    case beam_pattern::last_beam:
      beams = {nof_beams - 1};
      break;
    case beam_pattern::port_selection_beams:
      append_beams(beams, 0, nof_trx, 1, max_num_beams);
      break;
    case beam_pattern::region_b_beams:
      append_beams(beams, nof_trx, nof_beams, region_b_beam_step, max_num_beams);
      break;
    case beam_pattern::empty:
      break;
    case beam_pattern::mixed_beams:
      // One region A beam followed by two region B beams, sorted in increasing order.
      beams = {1, nof_trx + 1, nof_beams - 2};
      beams.resize(std::min<unsigned>(beams.size(), max_num_beams));
      break;
    case beam_pattern::random:
      // The number of selected beams cycles from one to the maximum along the pool.
      beams = generate_random_beams(rgen, nof_beams, std::min(1 + (grid_index % max_num_beams), nof_beams));
      break;
  }
  return beams;
}

dl_cplane_checker::dl_cplane_checker(const dl_beam_registry& beam_registry_, const dl_cplane_checker_config& config_) :
  beam_registry(beam_registry_), config(config_)
{
  if (!config.beamforming.has_value()) {
    return;
  }
  // Generate the codebook.
  beam_weights_codebook codebook = generate_beam_weights_codebook(config.beamforming->topology);

  nof_trx                  = codebook.get_nof_antennas();
  const auto& compr_params = config.beamforming->bfw_compr_params;
  unsigned    weights_size = get_packed_beamforming_weights_size(nof_trx, compr_params).value();

  // Compress weights of every beam and store in the internal vector.
  for (unsigned i_beam = 0, i_end = codebook.get_nof_beams(); i_beam != i_end; ++i_beam) {
    auto& weights = expected_weights.emplace_back(weights_size);
    compress_beamforming_weights(
        weights, codebook.get_beam_coefficients<MAX_NOF_BEAMFORMING_WEIGHTS>(to_beam_id(i_beam)), compr_params);
  }
}

void dl_cplane_checker::check(span<const uint8_t> message, slot_point slot, unsigned eaxc)
{
  std::optional<decoded_cplane_section_type_1> decoded_msg = decode_cplane_section_type_1(message, nof_trx);
  if (!decoded_msg.has_value()) {
    report_error(slot, eaxc, fmt::format("malformed message of {} bytes", message.size()));
    return;
  }

  // Every message carries a single section type 1 covering all the PRBs and symbols of the slot.
  unsigned expected_nof_prb = (config.nof_prb > std::numeric_limits<uint8_t>::max()) ? 0 : config.nof_prb;
  if (decoded_msg->section_type != 1 || decoded_msg->nof_sections != 1 || decoded_msg->start_symbol != 0 ||
      decoded_msg->nof_symbols != get_nsymb_per_slot(cyclic_prefix::NORMAL) || decoded_msg->start_prb != 0 ||
      decoded_msg->nof_prb != expected_nof_prb) {
    report_error(slot,
                 eaxc,
                 fmt::format("expected a single section type 1 covering the slot, got {} section(s) of type {} with "
                             "start_symbol={}, nof_symbols={}, start_prb={} and nof_prb={}",
                             decoded_msg->nof_sections,
                             decoded_msg->section_type,
                             decoded_msg->start_symbol,
                             decoded_msg->nof_symbols,
                             decoded_msg->start_prb,
                             decoded_msg->nof_prb));
    return;
  }

  std::optional<dl_beam_list> beams_opt = beam_registry.read(slot);
  if (!beams_opt.has_value()) {
    report_error(slot, eaxc, "no beam-ports registered for the slot");
    return;
  }
  auto& beams = *beams_opt;

  // The k-th DL eAxC transmits the k-th beam-port of the slot.
  const std::vector<unsigned>& dl_ports = config.dl_eaxc;
  unsigned eaxc_index = std::distance(dl_ports.begin(), std::find(dl_ports.begin(), dl_ports.end(), eaxc));
  if (eaxc_index >= beams.size()) {
    report_error(slot, eaxc, fmt::format("unexpected eAxC, the slot only transmits {} beam-port(s)", beams.size()));
    return;
  }

  if (!config.beamforming.has_value()) {
    if (decoded_msg->ef) {
      report_error(slot, eaxc, "Category A section with section extensions");
    }
    if (decoded_msg->beam_id != 0) {
      report_error(slot, eaxc, fmt::format("Category A section with beam identifier {}", decoded_msg->beam_id));
    }
    return;
  }

  unsigned beam             = beams[eaxc_index];
  unsigned expected_beam_id = beam + beam_id_offset;
  if (decoded_msg->beam_id != expected_beam_id) {
    report_error(slot,
                 eaxc,
                 fmt::format("beam identifier {} does not match the expected {} of beam-port {}",
                             decoded_msg->beam_id,
                             expected_beam_id,
                             beam));
    return;
  }
  if (!decoded_msg->extension_1.has_value()) {
    report_error(slot, eaxc, fmt::format("missing section extension 1 of beam-port {}", beam));
    return;
  }

  const decoded_cplane_section_ext_1& extension = *decoded_msg->extension_1;
  const ru_compression_params&        params    = config.beamforming->bfw_compr_params;
  if (extension.compr_params.type != params.type || extension.compr_params.data_width != params.data_width) {
    report_error(slot,
                 eaxc,
                 fmt::format("beamforming weights compression {}/{} bits does not match the configured {}/{} bits",
                             to_string(extension.compr_params.type),
                             extension.compr_params.data_width,
                             to_string(params.type),
                             params.data_width));
    return;
  }

  span<const uint8_t> weights = expected_weights[beam];
  if (!std::equal(weights.begin(), weights.end(), extension.packed_weights.begin(), extension.packed_weights.end())) {
    report_error(slot, eaxc, fmt::format("beamforming weights do not match beam-port {}", beam));
  }
}

void dl_cplane_checker::report_error(slot_point slot, unsigned eaxc, std::string_view error)
{
  if (nof_errors.fetch_add(1, std::memory_order_relaxed) < max_nof_printed_errors) {
    fmt::println("DL C-Plane error in slot {} and eAxC {}: {}", slot, eaxc, error);
  }
}
