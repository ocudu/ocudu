// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/support/test_utils.h"
#include <cmath>
#include <gtest/gtest.h>
#include <numeric>
#include <random>

using namespace asn1;
using ocudu::byte_buffer;

std::random_device rd;
std::mt19937       g(rd());

ocudu::log_sink_spy* test_spy = nullptr;

TEST(asn1_bit_ref_test, unpack_empty_buffer)
{
  byte_buffer   pdu;
  cbit_ref      bref(pdu);
  uint8_t       dummy  = 0;
  OCUDUASN_CODE result = bref.unpack(dummy, 1);
  ASSERT_EQ(result, OCUDUASN_ERROR_DECODE_FAIL);

  // Make sure the log backend has already processed the generated log entries.
  ocudulog::flush();
  ASSERT_TRUE(test_spy->get_error_counter() == 1 and test_spy->get_warning_counter() == 0);
  test_spy->reset_counters();
}

TEST(asn1_bit_ref_test, if_no_pack_is_called_buffer_stays_empty)
{
  byte_buffer pdu;
  {
    bit_ref bref(pdu);
    ASSERT_EQ(bref.distance(), 0);
    ASSERT_EQ(bref.distance_bytes(), 0);
  }
  ASSERT_EQ(pdu.length(), 0);
}

TEST(asn1_bit_ref_test, pack_up_to_1_byte_fills_underlying_buffer)
{
  byte_buffer pdu;
  bit_ref     bref(pdu);
  unsigned    nof_bits = std::uniform_int_distribution<unsigned>{1, 8}(g);
  unsigned    value    = std::uniform_int_distribution<unsigned>{0, (1U << nof_bits) - 1U}(g);

  bref.pack(value, nof_bits);
  ASSERT_EQ(bref.distance(), nof_bits);
  ASSERT_EQ(bref.distance_bytes(), 1);
  ASSERT_EQ(pdu.length(), 1);
  ASSERT_EQ(*pdu.begin(), value << (8U - nof_bits));
}

TEST(asn1_array_test, pack_unpack_operators)
{
  /* Test Ext Array */
  ext_array<int> ext_ar;
  const int*     old_ptr = ext_ar.data();
  // Test Resize until goes out of small buffer
  for (uint32_t i = 0; i < ext_ar.small_buffer_size + 1; ++i) {
    ASSERT_TRUE(ext_ar.is_in_small_buffer());
    ASSERT_TRUE(ext_ar.capacity() == ext_ar.small_buffer_size);
    ASSERT_TRUE(ext_ar.size() == i);
    ext_ar.push_back(i);
  }
  ASSERT_TRUE(not ext_ar.is_in_small_buffer());
  ASSERT_TRUE(ext_ar.size() == ext_ar.small_buffer_size + 1);
  ASSERT_TRUE(old_ptr != ext_ar.data());
  ASSERT_TRUE(ext_ar.capacity() > ext_ar.small_buffer_size);
  // Test Copy
  {
    ext_array<int> ext_ar2(ext_ar);
    ASSERT_TRUE(ext_ar2 == ext_ar);
    ASSERT_TRUE(not ext_ar.is_in_small_buffer());
  }
  ASSERT_TRUE(ext_ar.size() == 5);
  // Test Move
  {
    ext_array<int> ext_ar2(std::move(ext_ar));
    ASSERT_TRUE(ext_ar.is_in_small_buffer());
    ASSERT_TRUE(ext_ar2.size() == 5);
    ASSERT_TRUE(ext_ar2[2] == 2);
  }
  ext_ar = ext_array<int>(5);
  ASSERT_TRUE(not ext_ar.is_in_small_buffer());
  ASSERT_TRUE(ext_ar.size() == 5);
  ext_ar.resize(0);
  ASSERT_TRUE(not ext_ar.is_in_small_buffer());
}

TEST(asn1_bit_ref, pack_unpack_operators)
{
  for (uint32_t n_bit_stride = 1; n_bit_stride < 32; ++n_bit_stride) {
    ocudu::byte_buffer buf;
    uint32_t           nof_bytes_to_pack = n_bit_stride * 4;

    // Pack in batches of n_bit_stride.
    bit_ref bref(buf);
    for (uint32_t i = 0; i < nof_bytes_to_pack * 8 / n_bit_stride; ++i) {
      ASSERT_EQ(i * n_bit_stride, (unsigned)bref.distance());
      ASSERT_EQ(OCUDUASN_SUCCESS, bref.pack(i, n_bit_stride));
    }
    ASSERT_EQ(nof_bytes_to_pack, (unsigned)bref.distance_bytes());
    ASSERT_EQ(nof_bytes_to_pack, (unsigned)bref.distance() / 8U);
    ASSERT_EQ(0, bref.distance() % 8);
    //    bool is_eq = buf == vec_compare;
    //    ASSERT_TRUE(is_eq);

    // Unpack in batches of n_bit_stride.
    cbit_ref bref2(buf);
    uint64_t bitmask = (1u << n_bit_stride) - 1;
    ASSERT_EQ(0, bref2.distance_bytes());
    for (uint32_t i = 0; i < nof_bytes_to_pack * 8 / n_bit_stride; ++i) {
      uint64_t val;
      ASSERT_EQ(i * n_bit_stride, (unsigned)bref2.distance());
      ASSERT_EQ(OCUDUASN_SUCCESS, bref2.unpack(val, n_bit_stride));
      ASSERT_TRUE((i & bitmask) == val);
    }
    ASSERT_EQ(nof_bytes_to_pack, (unsigned)bref2.distance_bytes());
    ASSERT_EQ(bref.distance(), bref2.distance());
  }

  // pack bytes aligned
  {
    ocudu::byte_buffer             buf;
    constexpr uint32_t             nof_bytes = 512;
    uint32_t                       start     = 1;
    bit_ref                        bref(buf);
    std::array<uint8_t, nof_bytes> buf2;
    for (uint32_t i = 0; i < nof_bytes; ++i) {
      buf2[i] = start + i;
    }

    ASSERT_EQ(OCUDUASN_SUCCESS, bref.pack_bytes(buf2));
    ASSERT_EQ((int)nof_bytes, bref.distance_bytes());
    ASSERT_EQ(nof_bytes, buf.length());
    ASSERT_TRUE(buf == buf2);
    cbit_ref bref2(buf);
    ASSERT_EQ(OCUDUASN_SUCCESS, bref2.unpack_bytes(buf2));
    ASSERT_EQ((int)nof_bytes, bref2.distance_bytes());
    ASSERT_TRUE(buf == buf2);
  }

  // pack bytes unaligned
  {
    constexpr uint32_t             nof_bytes = 128;
    uint32_t                       start     = 1;
    ocudu::byte_buffer             buf;
    bit_ref                        bref(buf);
    std::array<uint8_t, nof_bytes> buf2, buf3;
    for (uint32_t i = 0; i < nof_bytes; ++i) {
      buf2[i] = start + i;
    }

    // pack.
    ASSERT_EQ(OCUDUASN_SUCCESS, bref.pack(0, 1)); // this unaligns.
    ASSERT_EQ(OCUDUASN_SUCCESS, bref.pack_bytes(buf2));
    ASSERT_EQ(bref.distance_bytes(), (int)nof_bytes + 1);
    ASSERT_EQ(bref.distance(), (int)nof_bytes * 8 + 1);
    ASSERT_EQ(bref.distance_bytes(), (int)buf.length());

    // unpack and check original bytes with unpacked ones.
    cbit_ref bref2(buf);
    uint32_t val;
    ASSERT_EQ(OCUDUASN_SUCCESS, bref2.unpack(val, 1));
    ASSERT_TRUE(val == 0);
    ASSERT_EQ(OCUDUASN_SUCCESS, bref2.unpack_bytes(buf3));
    ASSERT_TRUE(bref2.distance_bytes() == (int)nof_bytes + 1);
    ASSERT_TRUE(bref2.distance_bytes() == bref.distance_bytes());
    ASSERT_TRUE(std::equal(buf2.begin(), buf2.end(), buf3.begin(), buf3.end()));
  }

  // test advance bits
  {
    ocudu::byte_buffer buf = byte_buffer::create(std::vector<uint8_t>(256)).value();
    cbit_ref           bref(buf);
    ASSERT_EQ(OCUDUASN_SUCCESS, bref.advance_bits(4));
    ASSERT_EQ(4, bref.distance());
    ASSERT_EQ(OCUDUASN_SUCCESS, bref.advance_bits(4));
    ASSERT_EQ(8, bref.distance());
    ASSERT_EQ(OCUDUASN_SUCCESS, bref.advance_bits(3));
    ASSERT_EQ(11, bref.distance());
    ASSERT_EQ(OCUDUASN_SUCCESS, bref.advance_bits(200));
    ASSERT_EQ(211, bref.distance());
    ASSERT_EQ(OCUDUASN_SUCCESS, bref.advance_bits(5));
    ASSERT_EQ(216, bref.distance());
  }
}

TEST(asn1_octet_string_test, pack_unpack_operators)
{
  std::string          hexstr = "014477aaff";
  std::vector<uint8_t> bytes  = {0x01, 0x44, 0x77, 0xaa, 0xff};

  fixed_octstring<5> statstr;
  dyn_octstring      dynstr;
  dyn_octstring      dynstr_from_bytes;

  statstr.from_string(hexstr);
  dynstr.from_string(hexstr);
  dynstr_from_bytes.from_bytes(bytes);

  ASSERT_TRUE(sizeof(statstr) == statstr.size());
  ASSERT_TRUE(statstr.size() == 5);
  ASSERT_TRUE(dynstr.size() == 5);
  ASSERT_TRUE(dynstr[0] == 1);
  ASSERT_TRUE(statstr[4] == 255);
  ASSERT_TRUE(statstr.to_string() == hexstr);
  ASSERT_TRUE(statstr.to_string() == dynstr.to_string());
  ASSERT_TRUE(statstr.to_number() == dynstr.to_number());
  ASSERT_TRUE(statstr.to_number() == dynstr_from_bytes.to_number());

  // check endianess
  ASSERT_TRUE(statstr.to_number() == 5443660543);
  ASSERT_TRUE(dynstr.to_number() == 5443660543);
  dynstr.from_number(dynstr.to_number());
  ASSERT_TRUE(dynstr.to_number() == 5443660543);

  statstr.from_string(hexstr);
  dynstr.from_string(hexstr);

  ocudu::byte_buffer buf;
  bit_ref            b{buf};
  statstr.pack(b);
  ASSERT_TRUE(std::equal(buf.begin(), buf.end(), statstr.begin(), statstr.end())); // no prefix in static strings.
  ASSERT_TRUE(b.distance() == (int)statstr.size() * 8);
  fixed_octstring<5> statstr2;
  cbit_ref           b2 = cbit_ref(buf);
  ASSERT_EQ(OCUDUASN_SUCCESS, statstr2.unpack(b2));
  ASSERT_TRUE(statstr == statstr2);

  buf.clear();
  b = {buf};
  dynstr.pack(b);
  ASSERT_TRUE(*buf.begin() == dynstr.size()); // true for small strings.
  ASSERT_TRUE(std::equal(buf.begin() + 1, buf.end(), dynstr.begin(), dynstr.end()));
  ASSERT_TRUE(b.distance() == (int)(dynstr.size() * 8 + 8));
  dyn_octstring dynstr2; // unpacker allocates automatically
  b2 = cbit_ref(buf);
  dynstr2.unpack(b2);
  ASSERT_TRUE(dynstr == dynstr2);

  // test copy ctor
  {
    fixed_octstring<5> statcopy(statstr);
    ASSERT_TRUE(statcopy == statstr);
  }
  {
    dyn_octstring scopy(dynstr);
    ASSERT_TRUE(scopy == dynstr);
  }

  b.pack(1, 1);
  ASSERT_EQ(b.distance(), (int)(hexstr.size() * 8 / 2 + 9));
  b.align_bytes_zero();
  ASSERT_TRUE(b.distance() == (int)(hexstr.size() * 8 / 2 + 16));
}

TEST(asn1_octet_string_test, to_uint_and_to_octet_string_preserve_byte_order)
{
  constexpr uint64_t           number = 0x0123456789abcdefULL;
  const std::array<uint8_t, 8> expected_full_octets{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
  constexpr uint64_t           small_number = 0x123456;
  const std::array<uint8_t, 3> expected_small_octets{0x12, 0x34, 0x56};
  std::array<uint8_t, 8>       span_full_octets{};
  std::array<uint8_t, 3>       span_small_octets{};
  ocudu::byte_buffer           byte_buffer_octets;

  octet_string_helper::to_octet_string(span_full_octets, number);
  ASSERT_TRUE(std::equal(
      span_full_octets.begin(), span_full_octets.end(), expected_full_octets.begin(), expected_full_octets.end()));
  ASSERT_TRUE(octet_string_helper::to_uint(span_full_octets) == number);

  octet_string_helper::to_octet_string(span_small_octets, small_number);
  ASSERT_TRUE(std::equal(
      span_small_octets.begin(), span_small_octets.end(), expected_small_octets.begin(), expected_small_octets.end()));
  ASSERT_TRUE(octet_string_helper::to_uint(span_small_octets) == small_number);

  octet_string_helper::to_octet_string(byte_buffer_octets, number);
  ASSERT_TRUE(byte_buffer_octets.length() == expected_full_octets.size());
  ASSERT_TRUE(std::equal(
      byte_buffer_octets.begin(), byte_buffer_octets.end(), expected_full_octets.begin(), expected_full_octets.end()));
  ASSERT_TRUE(octet_string_helper::to_uint(byte_buffer_octets) == number);
}

TEST(asn1_fixed_bit_string, integer_conversion)
{
  fixed_bitstring<48> bitstr;
  ASSERT_EQ(bitstr.from_number(4211117727).to_number(), 4211117727);
}

TEST(asn1_bit_string_test, pack_unpack_operators)
{
  fixed_bitstring<10>      bstr1;
  bounded_bitstring<5, 15> bound_bstr1(10);
  dyn_bitstring            dyn_bstr1(10);
  bstr1.set(7, true);
  bstr1.set(9, true);
  bound_bstr1.set(7, true);
  bound_bstr1.set(9, true);
  dyn_bstr1.set(7, true);
  dyn_bstr1.set(9, true);

  /* Test BitSet and BitGet and Length */
  // fixed
  ASSERT_TRUE(bstr1.length() == 10);
  ASSERT_TRUE(bstr1.get(7));
  ASSERT_TRUE(bstr1.get(9));
  ASSERT_TRUE(not bstr1.get(8));
  ASSERT_TRUE(not bstr1.get(0));
  // bounded
  ASSERT_TRUE(bound_bstr1.length() == 10);
  ASSERT_TRUE(bound_bstr1.get(7));
  ASSERT_TRUE(bound_bstr1.get(9));
  ASSERT_TRUE(not bound_bstr1.get(8));
  ASSERT_TRUE(not bound_bstr1.get(0));
  // dyn
  ASSERT_TRUE(dyn_bstr1.length() == 10);
  ASSERT_TRUE(dyn_bstr1.get(7));
  ASSERT_TRUE(dyn_bstr1.get(9));
  ASSERT_TRUE(not dyn_bstr1.get(8));
  ASSERT_TRUE(not dyn_bstr1.get(0));

  /* Test Copy, Dtor, and Equal Comparison */
  {
    // fixed
    fixed_bitstring<10> bstr2;
    bstr2 = bstr1;
    ASSERT_TRUE(bstr2 == bstr1);
    // bound
    bounded_bitstring<5, 15> bound_bstr2;
    bound_bstr2 = bound_bstr1;
    ASSERT_TRUE(bound_bstr2 == bound_bstr1);
    // dyn
    dyn_bitstring dyn_bstr2(10);
    dyn_bstr2 = dyn_bstr1;
    ASSERT_TRUE(dyn_bstr2 == dyn_bstr1);
    // dtors called
  }

  /* Test Number and String conversion */
  // fixed
  ASSERT_TRUE(bstr1.to_string() == "1010000000");
  ASSERT_TRUE(bstr1.to_number() == 640);
  bstr1.set(1, true);
  ASSERT_TRUE(bstr1.to_string() == "1010000010");
  ASSERT_TRUE(bstr1.to_number() == 642);
  ASSERT_TRUE(bstr1 == fixed_bitstring<10>().from_number(642));
  // bounded
  ASSERT_TRUE(bound_bstr1.to_string() == "1010000000");
  ASSERT_TRUE(bound_bstr1.to_number() == 640);
  bound_bstr1.set(1, true);
  ASSERT_TRUE(bound_bstr1.to_string() == "1010000010");
  ASSERT_TRUE(bound_bstr1.to_number() == 642);
  ASSERT_TRUE((bound_bstr1 == bounded_bitstring<5, 15>().from_number(642)));
  ASSERT_TRUE((bound_bstr1 == bounded_bitstring<5, 15>().from_string("1010000010")));
  // dyn
  ASSERT_TRUE(dyn_bstr1.to_string() == "1010000000");
  ASSERT_TRUE(dyn_bstr1.to_number() == 640);
  dyn_bstr1.set(1, true);
  ASSERT_TRUE(dyn_bstr1.to_string() == "1010000010");
  ASSERT_TRUE(dyn_bstr1.to_number() == 642);
  ASSERT_TRUE(dyn_bstr1 == dyn_bitstring().from_number(642));
  ASSERT_TRUE(dyn_bstr1 == dyn_bitstring().from_string("1010000010"));
  // zero case
  ASSERT_TRUE(bound_bstr1.from_number(0) == "00000");

  /* Test Resize */
  bound_bstr1.resize(11);
  ASSERT_TRUE(not bound_bstr1.get(9)); // resets content
  ASSERT_TRUE(bound_bstr1.length() == 11);
  bound_bstr1.set(9, true);
  dyn_bstr1.resize(11);
  ASSERT_TRUE(not dyn_bstr1.get(9));
  ASSERT_TRUE(dyn_bstr1.length() == 11);
  dyn_bstr1.set(9, true);
  ASSERT_TRUE(dyn_bstr1.get(9));
  ASSERT_TRUE(not dyn_bstr1.get(10));

  /* Test Packing/Unpacking */
  ocudu::byte_buffer buf;
  bit_ref            bref(buf);
  // fixed
  ASSERT_EQ(OCUDUASN_SUCCESS, bstr1.pack(bref));
  fixed_bitstring<10> bstr2;
  ASSERT_TRUE(bstr2.length() == 10);
  cbit_ref bref2(buf);
  ASSERT_EQ(OCUDUASN_SUCCESS, bstr2.unpack(bref2));
  ASSERT_TRUE(bstr2 == bstr1);
  ASSERT_TRUE(bref.distance() == 10 and bref.distance() == bref2.distance());
  // bounded
  buf.clear();
  bref = bit_ref(buf);
  ASSERT_TRUE(bound_bstr1.pack(bref) == OCUDUASN_SUCCESS);
  bounded_bitstring<5, 15> bound_bstr2(dyn_bstr1.length());
  ASSERT_TRUE(bound_bstr2.length() == 11);
  bref2 = cbit_ref(buf);
  ASSERT_TRUE(bound_bstr2.unpack(bref2) == OCUDUASN_SUCCESS);
  ASSERT_TRUE(bound_bstr2 == bound_bstr1);
  ASSERT_TRUE(bref.distance() == (11 + 4) and bref.distance() == bref2.distance());
  // dyn
  buf.clear();
  bref = bit_ref(buf);
  ASSERT_TRUE(dyn_bstr1.pack(bref) == OCUDUASN_SUCCESS);
  dyn_bitstring dyn_bstr2(dyn_bstr1.length());
  ASSERT_TRUE(dyn_bstr2.length() == 11);
  bref2 = cbit_ref(buf);
  ASSERT_TRUE(dyn_bstr2.unpack(bref2) == OCUDUASN_SUCCESS);
  ASSERT_TRUE(dyn_bstr2 == dyn_bstr1);
  //  printf("%s==%s\n", dyn_bstr1.to_string().c_str(), dyn_bstr2.to_string().c_str());

  // disable temporarily the prints to check failures
  //  ocudu::nullsink_log null_log("NULL");
  //  bit_ref bref3(&buffer[0], sizeof(buffer));
  //  ASSERT_TRUE(dyn_bstr1.pack(bref3, false, 5, 10)==OCUDUASN_ERROR_ENCODE_FAIL);

  /* Test Pack/Unpack 2 */
  buf.clear();
  ASSERT_TRUE(buf.append(0));
  ASSERT_TRUE(buf.append(7));
  bref2 = cbit_ref(buf);
  fixed_bitstring<16> bstr3;
  bstr3.unpack(bref2);
  ASSERT_TRUE(bstr3 == "0000000000000111");
  ASSERT_TRUE(bstr3.to_string() == "0000000000000111");
}

TEST(asn1_seq_of_test, pack_unpack_and_operators)
{
  uint32_t                 fixed_list_size = 33;
  std::array<uint32_t, 33> fixed_list;
  for (uint32_t i = 0; i < fixed_list_size; ++i) {
    fixed_list[i] = i;
  }
  {
    // test copy and ==
    std::array<uint32_t, 33> fixed_list2 = fixed_list;
    ASSERT_TRUE(fixed_list == fixed_list2);
  }

  int      lb = 0, ub = 40;
  uint32_t n_bits = std::ceil(std::log2(ub - lb + 1));

  ocudu::byte_buffer buffer;
  bit_ref            b{buffer};
  pack_fixed_seq_of(b, fixed_list, fixed_list.size(), integer_packer<uint32_t>(lb, ub, false));
  ASSERT_TRUE(b.distance() == (int)(fixed_list_size * n_bits));
  cbit_ref                 b2(buffer);
  std::array<uint32_t, 33> fixed_list2;
  unpack_fixed_seq_of(fixed_list2, b2, fixed_list.size(), integer_packer<uint32_t>(lb, ub, false));
  ASSERT_TRUE(fixed_list == fixed_list2);

  // bounded seq_of
  buffer.clear();
  bounded_array<uint32_t, 33> bseq;
  ASSERT_TRUE(bseq.size() == 0);
  bseq.resize(fixed_list_size);
  ASSERT_TRUE(bseq.size() == fixed_list_size);
  std::memcpy(&bseq[0], &fixed_list[0], fixed_list_size * sizeof(uint32_t));
  b = {buffer};
  pack_dyn_seq_of(b, bseq, 0, 33, integer_packer<uint32_t>(lb, ub, false));
  ASSERT_TRUE(b.distance() == (int)((fixed_list_size + 1) * n_bits)); // unaligned
  //  ASSERT_EQ(b.distance(), (int)fixed_list_size * (int)n_bits + 8); // aligned
  bounded_array<uint32_t, 33> bseq2;
  bseq2.resize(fixed_list_size);
  b2 = {buffer};
  unpack_dyn_seq_of(bseq2, b2, 0, 33, integer_packer<uint32_t>(lb, ub, false));
  ASSERT_TRUE(bseq2 == bseq);
  ASSERT_TRUE(std::equal(bseq2.begin(), bseq2.end(), &fixed_list[0]));

  {
    bounded_array<uint32_t, 33> bseq3;
    bseq3 = bseq;
    ASSERT_TRUE(bseq == bseq3);
    // call dtor
  }
  ASSERT_TRUE(bseq2 == bseq);

  // dynamic array
  dyn_array<int> vec, vec2(33);
  std::iota(vec2.begin(), vec2.end(), 0);
  std::shuffle(vec2.begin(), vec2.end(), g);
  ASSERT_TRUE(vec2.size() == 33);
  ASSERT_TRUE(vec.size() == 0);
  vec.resize(32);
  std::iota(vec.begin(), vec.end(), 0);
  std::shuffle(vec.begin(), vec.end(), g);
  ASSERT_TRUE(vec.size() == 32);
  vec[5] = 5;
  vec.push_back(33);
  ASSERT_TRUE(vec.size() == 33);
  ASSERT_TRUE(vec[5] == 5); // resize does not affect content
  int* pos = &vec[5];
  vec.resize(10);
  ASSERT_TRUE(vec.size() == 10); // reduction in size does not cause realloc
  ASSERT_TRUE(pos == &vec[5]);
  {
    dyn_array<int> vec3 = vec2;
    ASSERT_TRUE(vec3.size() == vec2.size());
    ASSERT_TRUE(std::equal(vec3.begin(), vec3.end(), vec2.begin()));
    vec3 = vec;
    ASSERT_TRUE(vec3.size() == vec.size());
    ASSERT_TRUE(std::equal(vec3.begin(), vec3.end(), vec.begin()));
  }
  ASSERT_TRUE(vec[5] == 5);
}

TEST(asn1_copy_ptr_test, memory_safety)
{
  typedef fixed_octstring<10> TestType;
  char                        buffer[1024];
  uint32_t                    N = 10;
  for (uint32_t i = 0; i < N; ++i) {
    buffer[i] = i;
  }

  TestType*          s = new fixed_octstring<10>();
  copy_ptr<TestType> cptr(s);
  copy_ptr<TestType> cptr2;

  std::memcpy(&(*cptr)[0], buffer, N);

  ASSERT_TRUE(cptr2.get() == NULL);
  ASSERT_TRUE(std::memcmp(&(*s)[0], buffer, s->size()) == 0);
  ASSERT_TRUE(*cptr == *s);
  ASSERT_TRUE(cptr.get() == s);
  ASSERT_TRUE(std::memcmp(&(*cptr)[0], buffer, cptr->size()) == 0);
  ASSERT_TRUE(cptr2.get() == NULL);
  ASSERT_TRUE(cptr.get() != NULL);

  {
    copy_ptr<TestType> cptr3 = cptr;
    ASSERT_TRUE(cptr3 == cptr);
    ASSERT_TRUE(*cptr3 == *s);
    ASSERT_TRUE(cptr3.get() != cptr.get()); // different addresses
    ASSERT_TRUE(std::memcmp(&(*cptr3)[0], buffer, cptr3->size()) == 0);
    // call dtor
  }
  ASSERT_TRUE(*cptr == *s);

  // Test make_copy_ptr() function. It has the potential to hide all news from the code.
  TestType           s2(*s);
  copy_ptr<TestType> cptr3 = make_copy_ptr(s2);
  ASSERT_TRUE(cptr3.get() != &s2);
  ASSERT_TRUE(cptr3 == cptr);

  // get raw pointer and delete manually
  TestType* s3 = cptr.release();
  ASSERT_TRUE(s3 == s); // same address
  delete s3;            // it should *not* double free
}

class EnumTest
{
public:
  enum options { test5, test10, test20, nulltype };
  options               value;
  static const uint32_t nof_types = 3, nof_exts = 0;
  static const bool     has_ext = false;
  EnumTest() {}
  EnumTest(options v) : value(v) {}
  EnumTest& operator=(options v)
  {
    value = v;
    return *this;
  }
  operator uint8_t() { return (uint8_t)value; }

  std::string to_string() const
  {
    switch (value) {
      case test5:
        return "test5";
      case test10:
        return "test10";
      case test20:
        return "test20";
      default:
        printf("invalid value\n");
    }
    return "";
  }
  int32_t to_number() const
  {
    switch (value) {
      case test5:
        return 5;
      case test10:
        return 10;
      case test20:
        return 20;
      default:
        printf("invalid value\n");
    }
    return -1;
  }
};

TEST(asn1_enumerated, pack_unpack)
{
  EnumTest e;
  EnumTest e2;
  e = EnumTest::test10;
  ASSERT_TRUE(e.nof_types == 3);
  ASSERT_TRUE(e.to_string() == "test10");
  ASSERT_TRUE(e.to_number() == 10);
  ASSERT_TRUE(string_to_enum<EnumTest>(e2, "test10"));
  ASSERT_TRUE(e2 == e);
  ASSERT_TRUE(number_to_enum<EnumTest>(e2, 10));
  ASSERT_TRUE(e2 == e);

  ocudu::byte_buffer buffer;
  bit_ref            bref(buffer);
  ASSERT_TRUE(pack_enum(bref, e) == OCUDUASN_SUCCESS);
  ASSERT_TRUE(bref.distance() == (int)(std::floor(std::log2(e.nof_types)) + 1));

  cbit_ref bref2(buffer);
  ASSERT_TRUE(unpack_enum(e2, bref2) == OCUDUASN_SUCCESS);
  ASSERT_TRUE(bref2.distance() == (int)bref.distance());
  ASSERT_TRUE(e == e2);

  // Test fail path
  buffer.clear();
  ASSERT_TRUE(test_spy->get_error_counter() == 0 and test_spy->get_warning_counter() == 0);
  bref = bit_ref(buffer);
  e    = EnumTest::nulltype;
  ASSERT_TRUE(pack_enum(bref, e) == OCUDUASN_ERROR_ENCODE_FAIL);
  ASSERT_EQ(0, bref.distance());
  ASSERT_TRUE(buffer.append(255));
  bref2 = cbit_ref(buffer);
  ASSERT_TRUE(unpack_enum(e, bref2) == OCUDUASN_ERROR_DECODE_FAIL);

  // Make sure the log backend has already processed the generated log entries.
  ocudulog::flush();
  ASSERT_TRUE(test_spy->get_error_counter() == 2 and test_spy->get_warning_counter() == 0);
  test_spy->reset_counters();
}

class EnumBoolTest
{
public:
  enum options { true_value, nulltype };
  options               value;
  static const uint32_t nof_types = 1, nof_exts = 0;
  static const bool     has_ext = false;
  EnumBoolTest() {}
  EnumBoolTest(options v) : value(v) {}
  EnumBoolTest& operator=(options v)
  {
    value = v;
    return *this;
  }
  operator uint8_t() { return (uint8_t)value; }

  std::string to_string() const
  {
    switch (value) {
      case true_value:
        return "true";
      default:
        printf("invalid value\n");
    }
    return "";
  }
};

TEST(asn1_enumerated, bool_to_enum_test)
{
  EnumBoolTest e, e2;
  ASSERT_TRUE(e.nof_types == 1);
  bool_to_enum<EnumBoolTest>(e, true);
  ASSERT_TRUE(e.to_string() == "true");
  e2 = EnumBoolTest::options::true_value;
  ASSERT_TRUE(enum_to_bool(e2));
  e2 = EnumBoolTest::options::nulltype;
  ASSERT_TRUE(!enum_to_bool(e2));
}

void test_json_writer()
{
  json_writer writer;

  writer.start_array("");
  writer.start_obj("");
  writer.start_obj("obj1");
  writer.write_str("field1", "01010101");
  writer.write_int("field2", 5);
  writer.write_bool("field3", true);
  writer.start_obj("obj2");
  writer.write_int("field4", 1);
  writer.start_obj("obj3");
  writer.end_obj();
  writer.start_array("array1");
  writer.end_array();
  writer.end_obj();
  writer.end_obj();
  writer.end_obj();
  writer.end_array();

  fmt::print("{}\n", writer.to_string().c_str());
}

TEST(asn1_integer_test, large_integer_pack_unpack)
{
  integer<uint64_t, 0, 4294967295, false, true> big_integer = 3172073535;

  ocudu::byte_buffer buffer;
  bit_ref            bref(buffer);
  ASSERT_TRUE(big_integer.pack(bref) == 0);

  std::array<uint8_t, 5> bytes{0xc0, 0xbd, 0x12, 0x00, 0x3f};
  ASSERT_TRUE(std::equal(buffer.begin(), buffer.end(), bytes.begin(), bytes.end()));

  integer<uint64_t, 0, 4294967295, false, true> big_integer2;
  cbit_ref                                      cbref(buffer);
  ASSERT_TRUE(big_integer2.unpack(cbref) == 0);
  ASSERT_TRUE(big_integer == big_integer2);
}

TEST(asn1_real_test, real_special_number_pack_unpack)
{
  std::vector<float>                input_numbers = {0.0, NAN, INFINITY, -INFINITY};
  std::vector<std::vector<uint8_t>> output_bytes  = {{0x00}, {0x01, 0x42}, {0x01, 0x40}, {0x01, 0x41}};

  for (unsigned i = 0; i < input_numbers.size(); i++) {
    std::vector<uint8_t> out_bytes = output_bytes[i];
    real_s               real_number;
    real_number.value = input_numbers[i];

    ocudu::byte_buffer buffer;
    bit_ref            bref(buffer);
    ASSERT_TRUE(real_number.pack(bref) == 0);
    ASSERT_TRUE(std::equal(buffer.begin(), buffer.end(), out_bytes.begin(), out_bytes.end()));

    real_s   real_number2;
    cbit_ref cbref(buffer);
    ASSERT_TRUE(real_number2.unpack(cbref) == 0);
    if (std::isnan(real_number.value)) {
      ASSERT_TRUE(std::isnan(real_number2.value));
    }
    if (std::isinf(real_number.value) && real_number.value < 0) {
      ASSERT_TRUE(std::isinf(real_number2.value) && (real_number2.value < 0));
    }
    if (std::isinf(real_number.value) && real_number.value > 0) {
      ASSERT_TRUE(std::isinf(real_number2.value) && (real_number2.value > 0));
    }
  }
}

TEST(asn1_real_test, real_positive_number_pack_unpack)
{
  std::vector<float>                input_numbers = {0.15625, 1.0, 2.0, 4.0, 4.00001, 5.0, 5.0001, 1234.1234};
  std::vector<std::vector<uint8_t>> output_bytes  = {{0x03, 0x80, 0xfb, 0x05},
                                                     {0x03, 0x80, 0x00, 0x01},
                                                     {0x03, 0x80, 0x01, 0x01},
                                                     {0x03, 0x80, 0x02, 0x01},
                                                     {0x05, 0x80, 0xeb, 0x80, 0x00, 0x15},
                                                     {0x03, 0x80, 0x00, 0x05},
                                                     {0x05, 0x80, 0xec, 0x50, 0x00, 0x69},
                                                     {0x05, 0x80, 0xf3, 0x9a, 0x43, 0xf3}};

  for (unsigned i = 0; i < input_numbers.size(); i++) {
    std::vector<uint8_t> out_bytes = output_bytes[i];
    real_s               real_number;
    real_number.value = input_numbers[i];

    ocudu::byte_buffer buffer;
    bit_ref            bref(buffer);
    ASSERT_TRUE(real_number.pack(bref) == 0);
    ASSERT_TRUE(std::equal(buffer.begin(), buffer.end(), out_bytes.begin(), out_bytes.end()));

    int8_t   S = ((out_bytes[1] >> 6) & 0x1) ? -1 : 1;
    int8_t   E = static_cast<int8_t>(out_bytes[2]);
    uint32_t N = 0;
    for (size_t j = 3; j < out_bytes.size(); ++j) {
      N = (N << 8) | out_bytes[j];
    }
    float val = static_cast<float>(1.0 * S * N * std::pow(2, E));
    ASSERT_TRUE(real_number.value == val);

    real_s   real_number2;
    cbit_ref cbref(buffer);
    ASSERT_TRUE(real_number2.unpack(cbref) == 0);
    ASSERT_TRUE(real_number.value == real_number2.value);
  }
}

TEST(asn1_real_test, real_negative_number_pack_unpack)
{
  std::vector<float>                input_numbers = {-0.15625, -1.0, -2.0, -4.0, -4.00001, -5.0, -5.0001, -1234.1234};
  std::vector<std::vector<uint8_t>> output_bytes  = {{0x03, 0xc0, 0xfb, 0x05},
                                                     {0x03, 0xc0, 0x00, 0x01},
                                                     {0x03, 0xc0, 0x01, 0x01},
                                                     {0x03, 0xc0, 0x02, 0x01},
                                                     {0x05, 0xc0, 0xeb, 0x80, 0x00, 0x15},
                                                     {0x03, 0xc0, 0x00, 0x05},
                                                     {0x05, 0xc0, 0xec, 0x50, 0x00, 0x69},
                                                     {0x05, 0xc0, 0xf3, 0x9a, 0x43, 0xf3}};

  for (unsigned i = 0; i < input_numbers.size(); i++) {
    std::vector<uint8_t> out_bytes = output_bytes[i];
    real_s               real_number;
    real_number.value = input_numbers[i];

    ocudu::byte_buffer buffer;
    bit_ref            bref(buffer);
    ASSERT_TRUE(real_number.pack(bref) == 0);
    ASSERT_TRUE(std::equal(buffer.begin(), buffer.end(), out_bytes.begin(), out_bytes.end()));

    int8_t   S = ((out_bytes[1] >> 6) & 0x1) ? -1 : 1;
    int8_t   E = static_cast<int8_t>(out_bytes[2]);
    uint32_t N = 0;
    for (size_t j = 3; j < out_bytes.size(); ++j) {
      N = (N << 8) | out_bytes[j];
    }
    float val = static_cast<float>(1.0 * S * N * std::pow(2, E));
    ASSERT_TRUE(real_number.value == val);

    real_s   real_number2;
    cbit_ref cbref(buffer);
    ASSERT_TRUE(real_number2.unpack(cbref) == 0);
    ASSERT_TRUE(real_number.value == real_number2.value);
  }
}

TEST(asn1_real_test, real_oversized_length_rejected)
{
  // Crafted ASN.1 REAL with length=11, exceeding the 10-byte decode buffer — must be rejected.
  std::vector<uint8_t> raw = {0x0b, 0x80, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

  ocudu::byte_buffer buf;
  for (uint8_t b : raw) {
    ASSERT_TRUE(buf.append(b));
  }

  real_s   real_number;
  cbit_ref cbref(buf);
  ASSERT_EQ(OCUDUASN_ERROR_DECODE_FAIL, real_number.unpack(cbref));

  ocudulog::flush();
  ASSERT_EQ(1, test_spy->get_error_counter());
  test_spy->reset_counters();
}

TEST(asn1_varlength_field_test, pack)
{
  ocudu::byte_buffer buffer;
  bit_ref            bref(buffer);
  ASSERT_EQ(OCUDUASN_SUCCESS, bref.pack(0, 1));
  ASSERT_EQ(1, bref.distance());
  {
    varlength_field_pack_guard guard(bref);
    ASSERT_EQ(0, bref.distance());
    bref.pack(0, 8);
    ASSERT_EQ(1, bref.distance_bytes());
  }
  ASSERT_EQ(17, bref.distance()); // accounts for length determinant and 1 byte of data
}

int main(int argc, char** argv)
{
  // Setup the log spy to intercept error and warning log entries.
  if (!ocudulog::install_custom_sink(
          ocudu::log_sink_spy::name(),
          std::unique_ptr<ocudu::log_sink_spy>(new ocudu::log_sink_spy(ocudulog::get_default_log_formatter())))) {
    return OCUDUASN_ERROR;
  }
  test_spy = static_cast<ocudu::log_sink_spy*>(ocudulog::find_sink(ocudu::log_sink_spy::name()));
  if (!test_spy) {
    return OCUDUASN_ERROR;
  }

  auto& asn1_logger = ocudulog::fetch_basic_logger("ASN1", *test_spy, false);
  asn1_logger.set_level(ocudulog::basic_levels::debug);
  asn1_logger.set_hex_dump_max_size(-1);

  // Start the log backend.
  ocudulog::init();

  //  ASSERT_TRUE(test_json_writer()==0);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
