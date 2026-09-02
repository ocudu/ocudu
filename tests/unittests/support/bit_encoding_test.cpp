// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/span.h"
#include "ocudu/support/bit_encoding.h"
#include "fmt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(bit_encoding_test, bit_encoder)
{
  byte_buffer bytes;
  bit_encoder enc(bytes);

  // TEST: Empty buffer.

  ASSERT_EQ(0, enc.nof_bytes());
  ASSERT_EQ(0, enc.nof_bits());
  ASSERT_EQ(0, enc.next_bit_offset());

  enc.align_bytes_zero();
  ASSERT_EQ(0, enc.nof_bytes());
  ASSERT_EQ(0, enc.nof_bits());
  ASSERT_EQ(0, enc.next_bit_offset());

  enc.pack(0, 0);
  ASSERT_EQ(0, enc.nof_bytes());
  ASSERT_EQ(0, enc.nof_bits());
  ASSERT_EQ(0, enc.next_bit_offset());

  enc.pack_bytes(byte_buffer{});
  ASSERT_EQ(0, enc.nof_bytes());
  ASSERT_EQ(0, enc.nof_bits());
  ASSERT_EQ(0, enc.next_bit_offset());

  // TEST: bit packing.

  // byte_buffer:  [101_____]
  // Written bits: [101]
  enc.pack(0b101, 3);
  ASSERT_EQ(1, enc.nof_bytes());
  ASSERT_EQ(3, enc.nof_bits());
  ASSERT_EQ(0b10100000, *bytes.begin());
  ASSERT_EQ(3, enc.next_bit_offset());

  // byte_buffer:  [10101___]
  // Written bits:    [01]
  enc.pack(0b1, 2);
  ASSERT_EQ(1, enc.nof_bytes());
  ASSERT_EQ(5, enc.nof_bits());
  ASSERT_EQ(0b10101000, *bytes.begin());
  ASSERT_EQ(5, enc.next_bit_offset());

  // TEST: byte packing.

  // byte_buffer:  [10101000][00001  000][00010  000][00011___]
  // Written bits:      [000  00001][000  00010][000  00011]
  byte_buffer vec = byte_buffer::create({0b1, 0b10, 0b11}).value();
  enc.pack_bytes(vec);
  ASSERT_EQ(4, enc.nof_bytes());
  ASSERT_EQ(5 + 3 * 8, enc.nof_bits());
  ASSERT_EQ(5, enc.next_bit_offset());
  byte_buffer vec2 = byte_buffer::create({0b10101000, 0b00001000, 0b00010000, 0b00011000}).value();
  ASSERT_TRUE(bytes == vec2);

  // TEST: alignment padding.
  // byte_buffer:  [10101000][00001000][00010000][00011000]
  // Written bits:                                    [000]
  enc.align_bytes_zero();
  ASSERT_EQ(4, enc.nof_bytes());
  ASSERT_EQ(4 * 8, enc.nof_bits());
  ASSERT_EQ(0, enc.next_bit_offset());
  ASSERT_TRUE(bytes == vec2);

  // byte_buffer:  [10101000][00001000][00010000][00011000][00000000]
  // Written bits:                                         [00000000]
  enc.pack(0, 8);
  // No bits written.
  enc.align_bytes_zero();
  ASSERT_EQ(5, enc.nof_bytes());
  ASSERT_EQ(5 * 8, enc.nof_bits());
  ASSERT_EQ(0, enc.next_bit_offset());

  // TEST: fmt formatting of aligned bits
  fmt::print("encoded bits: {}\n", enc);
  std::string s            = fmt::format("{}", enc);
  std::string expected_str = "10101000 00001000 00010000 00011000 00000000";
  ASSERT_EQ(expected_str, s);

  // TEST: fmt formatting of unaligned bits
  // byte_buffer:  [10101000][00001000][00010000][00011000][00000000][10______]
  // Written bits:                                                   [10]
  enc.pack(0b10, 2);
  fmt::print("encoded bits: {}\n", enc);
  s            = fmt::format("{}", enc);
  expected_str = "10101000 00001000 00010000 00011000 00000000 10";
  ASSERT_EQ(expected_str, s);

  // TEST: encode full all 64 bit of a uint64_t
  uint64_t large = 0xc000000000000001;
  enc.pack(large, 64);

  const uint8_t packed_vec[] = {
      0b10101000,
      0b00001000,
      0b00010000,
      0b00011000,
      0b00000000,
      0b10110000, // combined byte having MSBs of 0xc0
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x40, // two MSBs contain LSBs of 0x01
  };

  ASSERT_TRUE(bytes == byte_buffer::create(packed_vec).value());
}

TEST(bit_encoding_test, bit_encoder_bool)
{
  byte_buffer bytes;
  bit_encoder enc(bytes);
  uint8_t     dummy          = 0;
  bool        bit1           = true;
  bool        bit0           = false;
  byte_buffer expected_bytes = byte_buffer::create({0x02}).value();

  enc.pack(dummy, 6);
  enc.pack(bit1, 1);
  enc.pack(bit0, 1);

  ASSERT_TRUE(expected_bytes == bytes);
}

TEST(bit_encoding_test, bit_encoder_uint64_aligned)
{
  byte_buffer bytes;
  bit_encoder enc(bytes);
  uint64_t    val64          = 0xc00f00000000f001;
  byte_buffer expected_bytes = byte_buffer::create({0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x01}).value();

  ASSERT_EQ(0, enc.nof_bytes());
  ASSERT_EQ(0, enc.nof_bits());
  ASSERT_EQ(0, enc.next_bit_offset());

  enc.pack(val64, 64);
  ASSERT_EQ(8, enc.nof_bytes());
  ASSERT_EQ(64, enc.nof_bits());
  ASSERT_EQ(0, enc.next_bit_offset());
  ASSERT_TRUE(expected_bytes == bytes);
}

TEST(bit_encoding_test, bit_encoder_uint64_offset)
{
  byte_buffer bytes;
  bit_encoder enc(bytes);
  uint8_t     val1           = 1;
  uint64_t    val64          = 0xc00f00000000f001;
  byte_buffer expected_bytes = byte_buffer::create({0xe0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x78, 0x00, 0x80}).value();

  ASSERT_EQ(0, enc.nof_bytes());
  ASSERT_EQ(0, enc.nof_bits());
  ASSERT_EQ(0, enc.next_bit_offset());

  enc.pack(val1, 1);
  ASSERT_EQ(1, enc.nof_bytes());
  ASSERT_EQ(1, enc.nof_bits());
  ASSERT_EQ(1, enc.next_bit_offset());

  enc.pack(val64, 64);
  ASSERT_EQ(9, enc.nof_bytes());
  ASSERT_EQ(65, enc.nof_bits());
  ASSERT_EQ(1, enc.next_bit_offset());
  ASSERT_TRUE(expected_bytes == bytes);
}

TEST(bit_encoding_test, bit_decoder_empty_buffer)
{
  byte_buffer          bytes;
  bit_decoder          dec(bytes);
  uint32_t             val;
  std::vector<uint8_t> vec;

  ASSERT_EQ(0, dec.nof_bytes());
  ASSERT_EQ(0, dec.nof_bits());
  ASSERT_EQ(0, dec.data().length());
  ASSERT_EQ(0, dec.next_bit_offset());

  ASSERT_TRUE(dec.advance_bits(0));
  ASSERT_EQ(0, dec.nof_bytes());
  ASSERT_EQ(0, dec.nof_bits());

  dec.align_bytes();
  ASSERT_EQ(0, dec.nof_bytes());
  ASSERT_EQ(0, dec.nof_bits());

  val = 1;
  ASSERT_TRUE(dec.unpack(val, 0));
  ASSERT_EQ(0, dec.nof_bytes());
  ASSERT_EQ(0, dec.nof_bits());
  ASSERT_EQ(val, 0);

  ASSERT_TRUE(dec.unpack_bytes(vec));
  ASSERT_EQ(0, dec.nof_bytes());
  ASSERT_EQ(0, dec.nof_bits());
  ASSERT_EQ(0, vec.size());
  ASSERT_EQ(0, dec.next_bit_offset());
}

TEST(bit_encoding_test, bit_decoder)
{
  byte_buffer          bytes = byte_buffer::create({0b1, 0b10, 0b11, 0b100}).value();
  bit_decoder          dec(bytes);
  uint32_t             val;
  std::vector<uint8_t> vec;

  ASSERT_EQ(0, dec.nof_bytes());
  ASSERT_EQ(0, dec.nof_bits());
  ASSERT_TRUE(bytes == dec.data());

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:   [00]
  ASSERT_TRUE(dec.unpack(val, 2));
  ASSERT_EQ(1, dec.nof_bytes());
  ASSERT_EQ(2, dec.nof_bits());
  ASSERT_EQ(2, dec.next_bit_offset());
  ASSERT_EQ(val, 0);

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:     [000001]
  ASSERT_TRUE(dec.unpack(val, 6));
  ASSERT_EQ(1, dec.nof_bytes());
  ASSERT_EQ(8, dec.nof_bits());
  ASSERT_EQ(0, dec.next_bit_offset());
  ASSERT_EQ(val, 0b1);

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:             [0]
  ASSERT_TRUE(dec.unpack(val, 1));
  ASSERT_EQ(2, dec.nof_bytes());
  ASSERT_EQ(9, dec.nof_bits());
  ASSERT_EQ(1, dec.next_bit_offset());
  ASSERT_EQ(val, 0);

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:              [0000010  0]
  vec.resize(1);
  ASSERT_TRUE(dec.unpack_bytes(vec));
  ASSERT_EQ(3, dec.nof_bytes());
  ASSERT_EQ(9 + 8, dec.nof_bits());
  ASSERT_EQ(1, dec.next_bit_offset());
  ASSERT_EQ(0b100, vec[0]);

  // byte_buffer:   [00000001][00000010][00000011][00000100]
  // Advanced bits:                       ---------^
  dec.align_bytes();
  ASSERT_EQ(3, dec.nof_bytes());
  ASSERT_EQ(3 * 8, dec.nof_bits());
  ASSERT_EQ(0, dec.next_bit_offset());

  // TEST: fmt formatting of aligned bits.
  fmt::print("decoded bits: {}\n", dec);
  std::string s            = fmt::format("{}", dec);
  std::string expected_str = "00000001 00000010 00000011";
  ASSERT_EQ(expected_str, s);

  // TEST: fmt formatting of unaligned bits.
  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:                                 [00]
  ASSERT_TRUE(dec.unpack(val, 2));
  fmt::print("decoded bits: {}\n", dec);
  s            = fmt::format("{}", dec);
  expected_str = "00000001 00000010 00000011 00";
  ASSERT_EQ(expected_str, s);

  // TEST: unpack beyond limits
  ASSERT_EQ(3 * 8 + 2, dec.nof_bits());
  ASSERT_TRUE(not dec.unpack(val, 8));
  ASSERT_EQ(4 * 8, dec.nof_bits());

  ASSERT_TRUE(not dec.unpack_bytes(vec));
  ASSERT_EQ(4 * 8, dec.nof_bits());

  ASSERT_TRUE(not dec.advance_bits(1));
  ASSERT_EQ(4 * 8, dec.nof_bits());
}

TEST(bit_encoding_test, bit_decoder_bytes)
{
  byte_buffer          bytes = byte_buffer::create({0b1, 0b10, 0b11, 0b100}).value();
  bit_decoder          dec(bytes);
  std::vector<uint8_t> vec;

  ASSERT_EQ(0, dec.nof_bytes());
  ASSERT_EQ(0, dec.nof_bits());
  ASSERT_TRUE(bytes == dec.data());

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:   [00000001]
  vec.resize(1);
  ASSERT_TRUE(dec.unpack_bytes(vec));
  ASSERT_EQ(1, dec.nof_bytes());
  ASSERT_EQ(8, dec.nof_bits());
  ASSERT_EQ(0, dec.next_bit_offset());
  ASSERT_EQ(0b1, vec[0]);

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:             [00000010][00000011]
  vec.resize(2);
  ASSERT_TRUE(dec.unpack_bytes(vec));
  ASSERT_EQ(3, dec.nof_bytes());
  ASSERT_EQ(24, dec.nof_bits());
  ASSERT_EQ(0, dec.next_bit_offset());
  ASSERT_EQ(0b10, vec[0]);
  ASSERT_EQ(0b11, vec[1]);

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:                                 [00000100]
  vec.resize(1);
  ASSERT_TRUE(dec.unpack_bytes(vec));
  ASSERT_EQ(4, dec.nof_bytes());
  ASSERT_EQ(32, dec.nof_bits());
  ASSERT_EQ(0, dec.next_bit_offset());
  ASSERT_EQ(0b100, vec[0]);
}

TEST(bit_encoding_test, bit_decoder_bool)
{
  byte_buffer bytes = byte_buffer::create({0x02}).value();
  bit_decoder dec(bytes);
  uint8_t     dummy;
  bool        bit1, bit0;

  ASSERT_TRUE(dec.unpack(dummy, 6));
  ASSERT_TRUE(dec.unpack(bit1, 1));
  ASSERT_TRUE(dec.unpack(bit0, 1));

  ASSERT_EQ(bit1, true);
  ASSERT_EQ(bit0, false);
}

TEST(bit_encoding_test, bit_decoder_uint64_aligned)
{
  byte_buffer bytes = byte_buffer::create({0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x01}).value();
  bit_decoder dec(bytes);
  uint64_t    val;

  ASSERT_EQ(0, dec.nof_bytes());
  ASSERT_EQ(0, dec.nof_bits());
  ASSERT_TRUE(bytes == dec.data());

  ASSERT_TRUE(dec.unpack(val, 64));
  ASSERT_EQ(8, dec.nof_bytes());
  ASSERT_EQ(64, dec.nof_bits());
  ASSERT_EQ(0, dec.next_bit_offset());
  ASSERT_EQ(val, 0xc00f00000000f001);
}

TEST(bit_encoding_test, bit_decoder_uint64_offset)
{
  byte_buffer bytes = byte_buffer::create({0xe0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x78, 0x00, 0x80}).value();
  bit_decoder dec(bytes);
  uint64_t    val;

  ASSERT_EQ(0, dec.nof_bytes());
  ASSERT_EQ(0, dec.nof_bits());
  ASSERT_TRUE(bytes == dec.data());

  ASSERT_TRUE(dec.unpack(val, 1));
  ASSERT_EQ(1, dec.nof_bytes());
  ASSERT_EQ(1, dec.nof_bits());
  ASSERT_EQ(1, dec.next_bit_offset());
  ASSERT_EQ(val, 1);

  ASSERT_TRUE(dec.unpack(val, 64));
  ASSERT_EQ(9, dec.nof_bytes());
  ASSERT_EQ(65, dec.nof_bits());
  ASSERT_EQ(1, dec.next_bit_offset());
  ASSERT_EQ(val, 0xc00f00000000f001);
}
