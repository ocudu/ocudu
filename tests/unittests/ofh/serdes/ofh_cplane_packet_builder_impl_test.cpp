// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "../../../../lib/ofh/serdes/ofh_cplane_message_builder_static_compression_impl.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/ofh/compression/compression_properties.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ofh;

TEST(ofh_control_plane_packet_builder_impl_test, build_valid_control_packet_should_pass)
{
  std::vector<uint8_t> packet = {
      0x90, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type1_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(0, 0, 0);
  header.start_symbol                     = 0;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 0;
  section.prb_start                             = 0;
  section.nof_prb                               = 0;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 14;
  packet_params.compr_params                    = {compression_type::none, 16};

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_dl_ul_radio_channel_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}

TEST(ofh_control_plane_packet_builder_impl_test, build_valid_invented_control_packet_should_pass)
{
  std::vector<uint8_t> packet = {
      0x90, 0x02, 0x30, 0x44, 0x01, 0x01, 0x00, 0x00, 0x00, 0x50, 0x06, 0x07, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type1_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(1, 2, 7);
  header.start_symbol                     = 4;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 5;
  section.prb_start                             = 6;
  section.nof_prb                               = 7;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 14;

  packet_params.compr_params = {compression_type::none, 16};

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_dl_ul_radio_channel_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}

#ifdef ASSERTS_ENABLED
TEST(ofh_control_plane_packet_builder_impl_test, build_control_packet_with_beam_id_and_no_weights_should_fail)
{
  OCUDU_TEST_REQUIREMENTS("RU-OFH-CATB-WDBF");

  std::vector<uint8_t> result_packet(64, 0);

  cplane_section_type1_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(0, 0, 0);
  header.start_symbol                     = 0;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 0;
  section.prb_start                             = 0;
  section.nof_prb                               = 0;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 14;
  packet_params.compr_params                    = {compression_type::none, 16};

  // Beam identifier without the section extension 1 that defines its weights.
  packet_params.section_fields.beam_id = 0x1234;

  cplane_message_builder_static_compression_impl builder;

  ASSERT_DEATH(builder.build_dl_ul_radio_channel_message(result_packet, packet_params),
               "A non-zero beam identifier must be accompanied by the section extension 1 beamforming weights");
}

TEST(ofh_control_plane_packet_builder_impl_test, build_control_packet_with_empty_section_extension_1_should_fail)
{
  OCUDU_TEST_REQUIREMENTS("RU-OFH-CATB-WDBF");

  std::vector<uint8_t> result_packet(64, 0);

  cplane_section_type1_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(0, 0, 0);
  header.start_symbol                     = 0;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 0;
  section.prb_start                             = 0;
  section.nof_prb                               = 0;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 14;
  packet_params.compr_params                    = {compression_type::none, 16};

  // Section extension 1 without beamforming weights.
  packet_params.section_fields.beam_id = 0x1234;
  packet_params.section_fields.extensions.emplace_back(cplane_section_extension_1_params{});

  cplane_message_builder_static_compression_impl builder;

  ASSERT_DEATH(builder.build_dl_ul_radio_channel_message(result_packet, packet_params),
               "Section extension 1 must carry the beamforming weights");
}

TEST(ofh_control_plane_packet_builder_impl_test, build_control_packet_with_zero_beam_id_and_weights_should_fail)
{
  OCUDU_TEST_REQUIREMENTS("RU-OFH-CATB-WDBF");

  std::vector<uint8_t> result_packet(64, 0);

  cplane_section_type1_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(0, 0, 0);
  header.start_symbol                     = 0;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 0;
  section.prb_start                             = 0;
  section.nof_prb                               = 0;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 14;
  packet_params.compr_params                    = {compression_type::none, 16};

  std::vector<uint8_t> packed_weights = {
      0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01};

  // Beamforming weights associated with the reserved beam identifier zero.
  packet_params.section_fields.beam_id = 0;
  packet_params.section_fields.extensions.emplace_back(cplane_section_extension_1_params{
      .weights = {.compr_params = {compression_type::none, 16}, .packed_weights = packed_weights}});

  cplane_message_builder_static_compression_impl builder;

  ASSERT_DEATH(builder.build_dl_ul_radio_channel_message(result_packet, packet_params),
               "Beamforming weights must be associated with a non-zero beam identifier");
}
#endif

TEST(ofh_control_plane_packet_builder_impl_test, build_control_packet_with_section_extension_1_should_pass)
{
  OCUDU_TEST_REQUIREMENTS("RU-OFH-CATB-WDBF");

  std::vector<uint8_t> packet = {0x90, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0xff, 0xfe, 0x92, 0x34, 0x01, 0x05, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x00,
                                 0x00, 0x7f, 0xff, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type1_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(0, 0, 0);
  header.start_symbol                     = 0;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 0;
  section.prb_start                             = 0;
  section.nof_prb                               = 0;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 14;
  packet_params.compr_params                    = {compression_type::none, 16};

  // Beamforming weights of 4 TRXs, uncompressed and 16 bits wide.
  std::vector<uint8_t> packed_weights = {
      0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01};

  packet_params.section_fields.beam_id = 0x1234;
  packet_params.section_fields.extensions.emplace_back(cplane_section_extension_1_params{
      .weights = {.compr_params = {compression_type::none, 16}, .packed_weights = packed_weights}});

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_dl_ul_radio_channel_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}

TEST(ofh_control_plane_packet_builder_impl_test, build_control_packet_with_bfp_compressed_section_extension_1)
{
  OCUDU_TEST_REQUIREMENTS("RU-OFH-CATB-WDBF");

  std::vector<uint8_t> packet = {0x90, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0xff, 0xfe, 0x80, 0x07, 0x01, 0x04, 0x91, 0x03, 0x12, 0x34,
                                 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x11, 0x00, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type1_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(0, 0, 0);
  header.start_symbol                     = 0;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 0;
  section.prb_start                             = 0;
  section.nof_prb                               = 0;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 14;
  packet_params.compr_params                    = {compression_type::none, 16};

  // Invented beamforming weights of 4 TRXs, BFP compressed and 9 bits wide. The first Byte is the bfwCompParam field
  // holding the block exponent, followed by 9 Bytes of mantissas.
  const ru_compression_params bfw_compr_params = {compression_type::BFP, 9};
  std::vector<uint8_t>        packed_weights   = {0x03, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x11};
  ASSERT_EQ(get_packed_beamforming_weights_size(4, bfw_compr_params).value(), packed_weights.size());

  packet_params.section_fields.beam_id = 0x0007;
  packet_params.section_fields.extensions.emplace_back(cplane_section_extension_1_params{
      .weights = {.compr_params = bfw_compr_params, .packed_weights = packed_weights}});

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_dl_ul_radio_channel_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}

TEST(ofh_control_plane_packet_builder_impl_test, build_valid_invented_idle_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x90, 0x28, 0x20, 0x4a, 0x01, 0x00, 0x08, 0xa0, 0x01, 0x00,
                                 0x00, 0x00, 0x00, 0x50, 0x06, 0x07, 0xff, 0xf4, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type0_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(1, 40, 5);
  header.start_symbol                     = 10;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 5;
  section.prb_start                             = 6;
  section.nof_prb                               = 7;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 4;

  packet_params.cp          = cyclic_prefix::NORMAL;
  packet_params.scs         = ocudu::subcarrier_spacing::kHz30;
  packet_params.time_offset = 2208;

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_idle_guard_period_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}

TEST(ofh_control_plane_packet_builder_impl_test, build_valid_prach_mixed_num_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x13, 0xf4, 0x90, 0x40, 0x01, 0x03, 0x01, 0xe4, 0xc1, 0x00, 0x00, 0x00,
                                 0x00, 0x10, 0x00, 0x0c, 0xff, 0xfc, 0x00, 0x00, 0xff, 0xf3, 0x34, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type3_parameters packet_params;
  packet_params.compr_params                    = {compression_type::none, 16};
  packet_params.scs                             = cplane_scs::kHz30;
  packet_params.time_offset                     = 484;
  packet_params.cpLength                        = 0;
  packet_params.fft_size                        = cplane_fft_size::fft_4096;
  packet_params.section_fields.frequency_offset = -3276;

  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::uplink;
  header.filter_index                     = filter_index_type::ul_prach_preamble_short;
  header.slot                             = slot_point(to_numerology_value(subcarrier_spacing::kHz30), 244, 19);
  header.start_symbol                     = 0;

  cplane_common_section_0_1_3_5_fields& common_section = packet_params.section_fields.common_fields;
  common_section.section_id                            = 1;
  common_section.prb_start                             = 0;
  common_section.nof_prb                               = 12;
  common_section.re_mask                               = 0xfff;
  common_section.nof_symbols                           = 12;

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_prach_mixed_numerology_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}
