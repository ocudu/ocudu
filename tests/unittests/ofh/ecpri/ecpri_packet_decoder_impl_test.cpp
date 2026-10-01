// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "../../../lib/ofh/ecpri/ecpri_packet_decoder_impl.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ecpri;

TEST(ecpri_packet_decoder_impl_test, decode_valid_control_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x00, 0x14, 0xbe, 0xbe, 0xca, 0xfe, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(ocudulog::fetch_basic_logger("TEST"), 0);

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_FALSE(payload.empty());
  ASSERT_EQ(params.header.payload_size.value() - 2 * sizeof(uint16_t), payload.size());
  ASSERT_TRUE(params.header.is_last_packet);
  ASSERT_EQ(params.header.revision, 1);
  ASSERT_EQ(params.header.msg_type, message_type::rt_control_data);
}

TEST(ecpri_packet_decoder_impl_test, decode_valid_data_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x00, 0x00, 0x14, 0xca, 0xfe, 0xde, 0xba, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(ocudulog::fetch_basic_logger("TEST"), 0);

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_FALSE(payload.empty());
  ASSERT_EQ(params.header.payload_size.value() - 2 * sizeof(uint16_t), payload.size());
  ASSERT_TRUE(params.header.is_last_packet);
  ASSERT_EQ(params.header.revision, 1);
  ASSERT_EQ(params.header.msg_type, message_type::iq_data);
}

TEST(ecpri_packet_decoder_impl_test, decode_invalid_packet_size_should_fail)
{
  std::vector<uint8_t> packet = {0x20, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(ocudulog::fetch_basic_logger("TEST"), 0);

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}

TEST(ecpri_packet_decoder_impl_test, decode_invalid_payload_size_should_fail)
{
  std::vector<uint8_t> packet = {0x20, 0x00, 0x00, 0x14, 0xca, 0xfe, 0xde, 0xba, 0x90, 0x00, 0x00,
                                 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(ocudulog::fetch_basic_logger("TEST"), 0);

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}

TEST(ecpri_packet_decoder_impl_test, decode_invalid_ecpri_protocol_revision_should_fail)
{
  std::vector<uint8_t> packet = {0x20, 0x00, 0x00, 0x14, 0xca, 0xfe, 0xde, 0xba, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(ocudulog::fetch_basic_logger("TEST"), 0);

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}

TEST(ecpri_packet_decoder_impl_test, decode_invalid_ecpri_last_packet_should_fail)
{
  std::vector<uint8_t> packet = {0x11, 0x00, 0x00, 0x14, 0xca, 0xfe, 0xde, 0xba, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(ocudulog::fetch_basic_logger("TEST"), 0);

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}

TEST(ecpri_packet_decoder_impl_test, decode_packet_with_payload_smaller_than_necesary_should_fail)
{
  // This vector contains only 3 of the 4 bytes needed to decode Rtci/Pcid and Seqid.
  std::vector<uint8_t> packet = {0x10, 0x00, 0x00, 0x03, 0xca, 0xfe, 0xde};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(ocudulog::fetch_basic_logger("TEST"), 0);

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}

TEST(ecpri_packet_decoder_impl_test, decode_packet_ignoring_header_with_payload_smaller_than_necesary_should_fail)
{
  // This vector contains only 3 of the 4 bytes needed to decode Rtci/Pcid and Seqid.
  std::vector<uint8_t> packet = {0x10, 0x00, 0x00, 0x03, 0xca, 0xfe, 0xde};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_ignore_header_payload_size decoder(ocudulog::fetch_basic_logger("TEST"), 0);

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}
