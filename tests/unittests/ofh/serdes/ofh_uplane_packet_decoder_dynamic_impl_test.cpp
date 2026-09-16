// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "../../../../lib/ofh/serdes/ofh_uplane_message_decoder_dynamic_compression_impl.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ofh/compression/compression_factory.h"
#include "ocudu/ofh/compression/compression_properties.h"
#include "ocudu/ofh/compression/iq_decompressor.h"
#include "ocudu/ran/cyclic_prefix.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ofh;

namespace {

/// Dummy IQ decompressor.
class iq_decompressor_dummy : public iq_decompressor
{
public:
  bool
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) override
  {
    return true;
  }
};

/// IQ decompressor that succeeds a given number of times and fails afterwards.
class iq_decompressor_failing : public iq_decompressor
{
public:
  explicit iq_decompressor_failing(unsigned nof_successes_) : nof_successes(nof_successes_) {}

  // See interface for documentation.
  bool
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) override
  {
    return ++nof_calls <= nof_successes;
  }

  unsigned get_nof_calls() const { return nof_calls; }

private:
  const unsigned nof_successes;
  unsigned       nof_calls = 0;
};

} // namespace

/// Builds the decompressors the receiver installs, so a test exercises the real implementations.
// Builds a one-section U-plane message holding nof_prbs PRBs. Byte 7 is the section header numPrbu and byte 8
// the udCompHdr, udIqWidth in the high nibble where zero means 16 and udCompMeth in the low one. For BFP the
// per-PRB udCompParam is the first byte of every PRB, which get_compressed_prb_size() already accounts for.
// The payload bytes only have to be present, no test here reads the samples back.
static std::vector<uint8_t> build_uplane_packet(const ru_compression_params& params, unsigned nof_prbs)
{
  unsigned             width_nibble = (params.data_width == MAX_IQ_WIDTH) ? 0U : params.data_width;
  std::vector<uint8_t> packet       = {0x10,
                                       0x02,
                                       0x90,
                                       0x40,
                                       0x00,
                                       0x70,
                                       0x24,
                                       static_cast<uint8_t>(nof_prbs),
                                       static_cast<uint8_t>((width_nibble << 4U) | static_cast<unsigned>(params.type)),
                                       0x00};
  packet.resize(packet.size() + nof_prbs * get_compressed_prb_size(params).value(), 0x01);

  return packet;
}

static std::unique_ptr<iq_decompressor> create_production_decompressors(ocudulog::basic_logger& logger)
{
  std::array<std::unique_ptr<iq_decompressor>, NOF_COMPRESSION_TYPES_SUPPORTED> decompressors;
  for (unsigned i = 0, e = decompressors.size(); i != e; ++i) {
    decompressors[i] = create_iq_decompressor(static_cast<compression_type>(i), logger);
  }

  return create_iq_decompressor_selector(std::move(decompressors));
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, valid_packet_should_decode_correctly)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x03, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86,
      0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8,
      0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0,
      0x01, 0xea, 0x01, 0xea, 0x01, 0xf4, 0x01, 0xf4, 0x01, 0xfe, 0x01, 0xfe, 0x02, 0x08, 0x02, 0x08, 0x02, 0x12,
      0x02, 0x12, 0x02, 0x1c, 0x02, 0x1c, 0x02, 0x26, 0x02, 0x26, 0x02, 0x30, 0x02, 0x30, 0x02, 0x3a, 0x02, 0x3a,
      0x02, 0x44, 0x02, 0x44, 0x02, 0x4e, 0x02, 0x4e, 0x02, 0x58, 0x02, 0x58, 0x02, 0x62, 0x02, 0x62, 0x02, 0x6c,
      0x02, 0x6c, 0x02, 0x76, 0x02, 0x76, 0x02, 0x80, 0x02, 0x80, 0x02, 0x8a, 0x02, 0x8a, 0x02, 0x94, 0x02, 0x94,
      0x02, 0x9e, 0x02, 0x9e, 0x02, 0xa8, 0x02, 0xa8, 0x02, 0xb2, 0x02, 0xb2, 0x02, 0xbc, 0x02, 0xbc, 0x02, 0xc6,
      0x02, 0xc6, 0x02, 0xd0, 0x02, 0xd0, 0x02, 0xda, 0x02, 0xda};

  const ru_compression_params                     compr_params = {compression_type::none, 16};
  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  slot_point slot(1, 2, 4, 1);
  ASSERT_TRUE(decode_result);
  ASSERT_EQ(results.params.direction, data_direction::uplink);
  ASSERT_EQ(results.params.slot, slot);
  ASSERT_EQ(results.params.filter_index, filter_index_type::standard_channel_filter);
  ASSERT_EQ(results.params.symbol_id, 2);

  const uplane_section_params& section = results.sections.front();
  ASSERT_EQ(section.section_id, 7);
  ASSERT_EQ(section.start_prb, 36);
  ASSERT_EQ(section.nof_prbs, 3);
  ASSERT_TRUE(section.is_every_rb_used);
  ASSERT_TRUE(section.use_current_symbol_number);
  ASSERT_EQ(section.ud_comp_hdr.data_width, compr_params.data_width);
  ASSERT_EQ(section.ud_comp_hdr.type, compr_params.type);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, missing_one_iq_sample_must_fail)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x03, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01,
      0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8,
      0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01,
      0xea, 0x01, 0xf4, 0x01, 0xf4, 0x01, 0xfe, 0x01, 0xfe, 0x02, 0x08, 0x02, 0x08, 0x02, 0x12, 0x02, 0x12, 0x02, 0x1c,
      0x02, 0x1c, 0x02, 0x26, 0x02, 0x26, 0x02, 0x30, 0x02, 0x30, 0x02, 0x3a, 0x02, 0x3a, 0x02, 0x44, 0x02, 0x44, 0x02,
      0x4e, 0x02, 0x4e, 0x02, 0x58, 0x02, 0x58, 0x02, 0x62, 0x02, 0x62, 0x02, 0x6c, 0x02, 0x6c, 0x02, 0x76, 0x02, 0x76,
      0x02, 0x80, 0x02, 0x80, 0x02, 0x8a, 0x02, 0x8a, 0x02, 0x94, 0x02, 0x94, 0x02, 0x9e, 0x02, 0x9e, 0x02, 0xa8, 0x02,
      0xa8, 0x02, 0xb2, 0x02, 0xb2, 0x02, 0xbc, 0x02, 0xbc, 0x02, 0xc6, 0x02, 0xc6, 0x02, 0xd0, 0x02, 0xd0};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
  ASSERT_TRUE(results.sections.empty());
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, missing_one_prb_must_fail)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x03, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86,
      0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8,
      0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0,
      0x01, 0xea, 0x01, 0xea, 0x01, 0xf4, 0x01, 0xf4, 0x01, 0xfe, 0x01, 0xfe, 0x02, 0x08, 0x02, 0x08, 0x02, 0x12,
      0x02, 0x12, 0x02, 0x1c, 0x02, 0x1c, 0x02, 0x26, 0x02, 0x26, 0x02, 0x30, 0x02, 0x30, 0x02, 0x3a, 0x02, 0x3a,
      0x02, 0x44, 0x02, 0x44, 0x02, 0x4e, 0x02, 0x4e, 0x02, 0x58, 0x02, 0x58, 0x02};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
  ASSERT_TRUE(results.sections.empty());
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, dynamic_compression_with_no_compression_header_fails)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x03, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01,
      0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8,
      0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01,
      0xe0, 0x01, 0xea, 0x01, 0xea, 0x01, 0xf4, 0x01, 0xf4, 0x01, 0xfe, 0x01, 0xfe, 0x02, 0x08, 0x02, 0x08,
      0x02, 0x12, 0x02, 0x12, 0x02, 0x1c, 0x02, 0x1c, 0x02, 0x26, 0x02, 0x26, 0x02, 0x30, 0x02, 0x30, 0x02,
      0x3a, 0x02, 0x3a, 0x02, 0x44, 0x02, 0x44, 0x02, 0x4e, 0x02, 0x4e, 0x02, 0x58, 0x02, 0x58, 0x02};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
  ASSERT_TRUE(results.sections.empty());
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, decoding_one_section_and_failing_to_decode_another_passes)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x00, 0x00, 0x00, 0x01, 0x7c,
                                 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01,
                                 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8,
                                 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01,
                                 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea, 0x01, 0xea, 0x01, 0xea};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
  ASSERT_EQ(results.sections.size(), 1);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, missing_section_header_must_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, missing_header_must_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, missing_compression_header_must_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x00};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, downlink_packet_should_fail)
{
  std::vector<uint8_t> packet = {0x90, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c,
                                 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a,
                                 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8,
                                 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6,
                                 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea, 0xea, 0x01, 0xea};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, reserved_filter_index_should_fail)
{
  std::vector<uint8_t> packet = {0x18, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c,
                                 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a,
                                 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8,
                                 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6,
                                 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea, 0xea, 0x01, 0xea};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, symbol_index_out_of_range_should_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x4e, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c,
                                 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a,
                                 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8,
                                 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6,
                                 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea, 0xea, 0x01, 0xea};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, invalid_subframe_should_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0xf0, 0x4d, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c,
                                 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a,
                                 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8,
                                 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6,
                                 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea, 0xea, 0x01, 0xea};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  for (unsigned i = 0, e = 10; i != e; ++i) {
    packet[2] += 1U << 4;

    uplane_message_decoder_results results;
    bool                           decode_result = decoder.decode(results, packet);

    ASSERT_TRUE(decode_result);
  }

  for (unsigned i = 10, e = 16; i != e; ++i) {
    packet[2] += 1U << 4;

    uplane_message_decoder_results results;
    bool                           decode_result = decoder.decode(results, packet);

    ASSERT_FALSE(decode_result);
  }
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, invalid_slot_should_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x90, 0x40, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c,
                                 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a,
                                 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8,
                                 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6,
                                 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea, 0xea, 0x01, 0xea};

  subcarrier_spacing                              scs = subcarrier_spacing::kHz30;
  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          scs,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  unsigned nof_slots = slot_point(scs, 0).nof_slots_per_subframe();

  for (uint8_t i = 0, e = nof_slots; i != e; ++i) {
    packet[3] = (i & 0x03) << 6;
    packet[2] |= (i >> 2) & 0x0f;
    uplane_message_decoder_results results;
    bool                           decode_result = decoder.decode(results, packet);

    ASSERT_TRUE(decode_result);
  }

  for (uint8_t i = nof_slots, e = 16; i != e; ++i) {
    packet[3] = (i & 0x03) << 6;
    packet[2] |= (i >> 2) & 0x0f;
    uplane_message_decoder_results results;
    bool                           decode_result = decoder.decode(results, packet);

    ASSERT_FALSE(decode_result);
  }
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, invalid_compression_type_should_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x90, 0x40, 0x00, 0x70, 0x24, 0x01, 0x00, 0x7c, 0x01, 0x7c, 0x01,
                                 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4,
                                 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01,
                                 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0,
                                 0x01, 0xea, 0x01, 0xea, 0xea, 0x01, 0xea, 0x01, 0xea};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  for (uint8_t i = 0, e = static_cast<unsigned>(compression_type::reserved); i != e; ++i) {
    packet[8] = i & 0x0f;
    uplane_message_decoder_results results;
    bool                           decode_result = decoder.decode(results, packet);

    ASSERT_TRUE(decode_result);
  }

  for (uint8_t i = static_cast<unsigned>(compression_type::reserved), e = 16; i != e; ++i) {
    packet[8] = i & 0x0f;
    uplane_message_decoder_results results;
    bool                           decode_result = decoder.decode(results, packet);

    ASSERT_FALSE(decode_result);
  }
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, unsupported_compression_type_should_fail)
{
  ocudulog::basic_logger&                         logger = ocudulog::fetch_basic_logger("TEST");
  uplane_message_decoder_dynamic_compression_impl decoder(logger,
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          create_production_decompressors(logger));

  // Compression types that the specification defines but this build does not implement.
  const std::array<compression_type, 5> unsupported_types = {compression_type::block_scaling,
                                                             compression_type::mu_law,
                                                             compression_type::modulation,
                                                             compression_type::bfp_selective,
                                                             compression_type::mod_selective};

  for (compression_type type : unsupported_types) {
    // Sized for the type under test, so the decode can only fail on the decompressor and not on a short read.
    std::vector<uint8_t>           packet = build_uplane_packet({type, MAX_IQ_WIDTH}, 1);
    uplane_message_decoder_results results;

    ASSERT_FALSE(decoder.decode(results, packet)) << "compression type '" << to_string(type) << "' was decoded";
  }
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, supported_compression_types_should_decode_with_the_real_decompressors)
{
  ocudulog::basic_logger&                         logger = ocudulog::fetch_basic_logger("TEST");
  uplane_message_decoder_dynamic_compression_impl decoder(logger,
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          create_production_decompressors(logger));

  // An implementation that reports a failure while writing correct samples would make the receiver drop a
  // good message, and only a positive case catches that. The SIMD backends pack 9 and 16 bits natively, so
  // the 12-bit cases go through their fallback to the generic one, which has to forward its result.
  const std::array<ru_compression_params, 4> cases = {{{compression_type::none, MAX_IQ_WIDTH},
                                                       {compression_type::BFP, MAX_IQ_WIDTH},
                                                       {compression_type::none, 12},
                                                       {compression_type::BFP, 12}}};

  for (const ru_compression_params& test_case : cases) {
    // Exactly one complete section, so the case does not lean on the decoder accepting a truncated tail.
    std::vector<uint8_t>           case_packet = build_uplane_packet(test_case, 1);
    uplane_message_decoder_results results;

    ASSERT_TRUE(decoder.decode(results, case_packet))
        << to_string(test_case.type) << " with " << test_case.data_width << " bits was dropped";
    ASSERT_EQ(results.sections.size(), 1U);
    ASSERT_EQ(results.sections.back().ud_comp_hdr.type, test_case.type);
    ASSERT_EQ(results.sections.back().ud_comp_hdr.data_width, test_case.data_width);
  }
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, no_compression_below_the_minimum_width_should_fail)
{
  ocudulog::basic_logger&                         logger = ocudulog::fetch_basic_logger("TEST");
  uplane_message_decoder_dynamic_compression_impl decoder(logger,
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          create_production_decompressors(logger));

  // The udIqWidth nibble carries this width, so a peer picks it, not the configuration. Without the check the
  // quantizer divides by a gain of zero and every sample reaching the resource grid is a NaN.
  std::vector<uint8_t>           packet = build_uplane_packet({compression_type::none, MIN_IQ_WIDTH - 1}, 1);
  uplane_message_decoder_results results;

  ASSERT_FALSE(decoder.decode(results, packet));
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, failed_decompression_should_drop_the_message)
{
  // A complete BFP section, so the type is supported and only the decompressor can fail the decode.
  std::vector<uint8_t> packet = build_uplane_packet({compression_type::BFP, MAX_IQ_WIDTH}, 1);

  auto  decompressor = std::make_unique<iq_decompressor_failing>(0);
  auto& spy          = *decompressor;

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::move(decompressor));

  uplane_message_decoder_results results;

  ASSERT_FALSE(decoder.decode(results, packet));
  ASSERT_EQ(spy.get_nof_calls(), 1);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, failed_decompression_after_a_valid_section_should_drop_the_message)
{
  // Two complete sections, so the second one reaches the decompressor instead of running out of bytes.
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x91, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86,
      0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01,
      0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
      0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x91, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01,
      0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a,
      0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const unsigned ru_nof_prbs  = 2;
  auto           decompressor = std::make_unique<iq_decompressor_failing>(1);
  auto&          spy          = *decompressor;

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          ru_nof_prbs,
                                                          0,
                                                          std::move(decompressor));

  uplane_message_decoder_results results;

  // A successful first section must not turn the second failure into a truncated tail.
  ASSERT_FALSE(decoder.decode(results, packet));
  ASSERT_EQ(spy.get_nof_calls(), 2);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, none_compression_with_15_bits_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0xf0, 0x00, 0x01, 0x7c, 0x01, 0x7c,
                                 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4,
                                 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2,
                                 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01};

  const ru_compression_params                     compr_params = {compression_type::none, 15};
  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
  const uplane_section_params& section = results.sections.front();
  ASSERT_EQ(section.ud_comp_hdr.data_width, compr_params.data_width);
  ASSERT_EQ(section.ud_comp_hdr.type, compr_params.type);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, bfp_with_9_bits_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x91, 0x00, 0x00, 0x01, 0x7c, 0x01,
                                 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01,
                                 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01,
                                 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01};

  const ru_compression_params                     compr_params = {compression_type::BFP, 9};
  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
  const uplane_section_params& section = results.sections.front();
  ASSERT_EQ(section.ud_comp_hdr.data_width, compr_params.data_width);
  ASSERT_EQ(section.ud_comp_hdr.type, compr_params.type);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, bfp_with_15_bits_without_ud_comp_param_must_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0xf1, 0x00, 0x01, 0x7c, 0x01, 0x7c,
                                 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4,
                                 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2,
                                 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01};

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          273,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, if_message_num_prbs_equals_zero_decoder_uses_configured_ru_nof_prbs)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x91, 0x00, 0x00, 0x01, 0x7c, 0x01,
                                 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01,
                                 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01,
                                 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01,
                                 0xa4, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

  const unsigned ru_nof_prbs = 2;

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          ru_nof_prbs,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
  ASSERT_EQ(ru_nof_prbs, results.sections.front().nof_prbs);
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, if_message_contains_one_valid_section_and_padding_passes)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x91, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86,
      0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01,
      0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const unsigned ru_nof_prbs = 2;

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          ru_nof_prbs,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
  ASSERT_EQ(ru_nof_prbs, results.sections.front().nof_prbs);
  ASSERT_EQ(1, results.sections.size());
}

TEST(ofh_uplane_packet_decoder_dynamic_impl, message_containing_more_than_one_section_should_fail_to_decode)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x91, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86,
      0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01,
      0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
      0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x91, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01,
      0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a,
      0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const unsigned ru_nof_prbs = 2;

  uplane_message_decoder_dynamic_compression_impl decoder(ocudulog::fetch_basic_logger("TEST"),
                                                          subcarrier_spacing::kHz30,
                                                          get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                          ru_nof_prbs,
                                                          0,
                                                          std::make_unique<iq_decompressor_dummy>());

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}
