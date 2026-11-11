// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Octet string, bit string and character string helpers, and enum conversions.

#include "ocudu/asn1/asn1_types.h"
#include "asn1_internal.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <vector>

using ocudu::byte_buffer;
using ocudu::span;

namespace asn1 {

using detail::ceil_log2;

/************************
    enum conversion
************************/

const char* table::enum_to_string(const enum_desc& desc, uint32_t value)
{
  return convert_enum_idx(desc.names, desc.nof_types, value, desc.type_name);
}

const char* convert_enum_idx(const char* array[], uint32_t nof_types, uint32_t enum_val, const char* enum_type)
{
  if (enum_val >= nof_types) {
    if (enum_val == nof_types) {
      log_error("The enum of type {} was not initialized.", enum_type);
    } else {
      log_error("The enum value={} of type {} is not valid.", enum_val, enum_type);
    }
    return "";
  }
  return array[enum_val];
}

template <class ItemType>
ItemType map_enum_number(ItemType* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type)
{
  if (enum_val >= nof_types) {
    if (enum_val == nof_types) {
      log_error("The enum of type {} is not initialized.", enum_type);
    } else {
      log_error("The enum value={} of type {} cannot be converted to a number.", enum_val, enum_type);
    }
    return 0;
  }
  return array[enum_val];
}
template const uint8_t
map_enum_number<const uint8_t>(const uint8_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const uint16_t
map_enum_number<const uint16_t>(const uint16_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const uint32_t
map_enum_number<const uint32_t>(const uint32_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const uint64_t
map_enum_number<const uint64_t>(const uint64_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const int8_t
map_enum_number<const int8_t>(const int8_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const int16_t
map_enum_number<const int16_t>(const int16_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const int32_t
map_enum_number<const int32_t>(const int32_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const int64_t
map_enum_number<const int64_t>(const int64_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const float
map_enum_number<const float>(const float* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);

/************************
    common octstring
************************/

uint64_t octet_string_helper::to_uint(ocudu::span<const uint8_t> buf)
{
  size_t nbytes = buf.size();
  if (nbytes > 8U) {
    log_error("OCTET STRING of size={} does not fit in an uint64_t", nbytes);
    return 0;
  }
  uint64_t val = 0;
  for (unsigned i = 0; i < nbytes; ++i) {
    val += ((uint64_t)buf[nbytes - 1 - i]) << (uint64_t)(i * 8U);
  }
  return val;
}

uint64_t octet_string_helper::to_uint(const byte_buffer& buf)
{
  size_t nbytes = buf.length();
  if (nbytes > 8) {
    log_error("OCTET STRING with size={} does not fit in an uint64_t", nbytes);
    return 0;
  }
  uint64_t val = 0;
  auto     it  = buf.begin();
  for (unsigned i = 0; i < nbytes; ++i) {
    val += static_cast<uint64_t>(*it) << static_cast<uint64_t>((nbytes - 1 - i) * 8U);
    ++it;
  }
  return val;
}

void octet_string_helper::to_octet_string(ocudu::span<uint8_t> buf, uint64_t number)
{
  uint64_t nbytes = buf.size();
  if (nbytes < 8 and (static_cast<uint64_t>(1U) << (8U * nbytes)) <= number) {
    log_error("Integer={} does not fit in an OCTET STRING of size={}", number, nbytes);
    return;
  }
  for (uint32_t i = 0; i < nbytes; ++i) {
    buf[nbytes - 1 - i] = (number >> (uint64_t)(i * 8U)) & 0xffu;
  }
}

void octet_string_helper::to_octet_string(ocudu::byte_buffer& buf, uint64_t number)
{
  buf           = byte_buffer{byte_buffer::fallback_allocation_tag{}};
  size_t nbytes = sizeof(number);
  for (uint32_t i = 0; i < nbytes; ++i) {
    if (not buf.append((number >> (uint64_t)((nbytes - 1 - i) * 8U)) & 0xffu)) {
      log_error("Failed to append octet string byte to buffer");
      return;
    }
  }
}

static void to_hex(char* cstr, uint8_t val)
{
  std::sprintf(cstr, "%02x", val);
}

std::string octet_string_helper::to_hex_string(ocudu::span<const uint8_t> buf)
{
  std::string s;
  s.resize(buf.size() * 2);
  char cstr[3];
  for (uint32_t i = 0; i < buf.size(); i++) {
    to_hex(cstr, buf[i]);
    s.replace(i * 2, 2, cstr);
  }
  return s;
}

std::string octet_string_helper::to_hex_string(const byte_buffer& buf)
{
  std::string s;
  s.resize(buf.length() * 2);
  char     cstr[3];
  unsigned i = 0;
  for (uint8_t b : buf) {
    to_hex(cstr, b);
    s.replace(i * 2, 2, cstr);
    ++i;
  }
  return s;
}

unsigned octet_string_helper::hex_string_to_octets(ocudu::span<uint8_t> buf, const std::string& str)
{
  ocudu_assert(buf.size() >= ceil_frac(str.size(), (size_t)2U), "out-of-bounds access");
  if (str.size() % 2 != 0) {
    log_error("The provided hex string size={} is not a multiple of 2.", str.size());
  }
  char cstr[] = "\0\0\0";
  for (unsigned i = 0; i < str.size(); i += 2) {
    std::memcpy(&cstr[0], &str[i], 2);
    buf[i / 2] = strtoul(cstr, nullptr, 16);
  }
  return (str.size() + 1) / 2;
}

void octet_string_helper::append_hex_string(byte_buffer& buf, const std::string& str)
{
  if (str.size() % 2 != 0) {
    log_error("The provided hex string size={} is not a multiple of 2.", str.size());
  }
  char cstr[] = "\0\0\0";
  for (unsigned i = 0; i < str.size(); i += 2) {
    std::memcpy(&cstr[0], &str[i], 2);
    if (not buf.append(strtoul(cstr, nullptr, 16))) {
      log_error("Failed to append octet string byte to buffer");
    }
  }
}

/************************
     dyn_octstring
************************/

template <bool Al>
unbounded_octstring<Al>::unbounded_octstring(const unbounded_octstring& other) noexcept :
  // Use fallback allocator, because operator= should never fail.
  ocudu::byte_buffer(fallback_allocation_tag{}, other)
{
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::operator=(const unbounded_octstring& other) noexcept
{
  if (this != &other) {
    // Use fallback allocator, because operator= should never fail.
    *this = byte_buffer{fallback_allocation_tag{}, other};
  }
  return *this;
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::operator=(unbounded_octstring&& other) noexcept
{
  *static_cast<byte_buffer*>(this) = std::move(other);
  return *this;
}

template <bool Al>
OCUDUASN_CODE unbounded_octstring<Al>::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_length(bref, length(), aligned));
  return bref.pack_bytes(ocudu::byte_buffer_view{*this});
}

template <bool Al>
OCUDUASN_CODE unbounded_octstring<Al>::unpack(cbit_ref& bref)
{
  unsigned len;
  HANDLE_CODE(unpack_length(len, bref, aligned));
  clear();
  // Chunks on the stack let the buffer grow by chunk rather than by byte.
  std::array<uint8_t, 64> chunk;
  while (len != 0) {
    unsigned n = std::min(len, static_cast<unsigned>(chunk.size()));
    HANDLE_CODE(bref.unpack_bytes({chunk.data(), n}));
    HANDLE_CODE(append(ocudu::span<const uint8_t>{chunk.data(), n}) ? OCUDUASN_SUCCESS : OCUDUASN_ERROR_DECODE_FAIL);
    len -= n;
  }
  return OCUDUASN_SUCCESS;
}

template <bool Al>
std::string unbounded_octstring<Al>::to_string() const
{
  return octet_string_helper::to_hex_string(*this);
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::from_string(const std::string& hexstr)
{
  // clears previous buffer.
  *this = byte_buffer{byte_buffer::fallback_allocation_tag{}};

  // appends hex string to buffer.
  octet_string_helper::append_hex_string(*this, hexstr);

  return *this;
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::from_bytes(span<const uint8_t> bytes)
{
  // clears previous buffer.
  *this = byte_buffer{byte_buffer::fallback_allocation_tag{}};

  // appends bytes to buffer.
  if (not this->append(bytes)) {
    log_error("Failed to append byte to buffer");
  }

  return *this;
}

template <bool Al>
uint64_t unbounded_octstring<Al>::to_number() const
{
  return octet_string_helper::to_uint(*this);
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::from_number(uint64_t val)
{
  octet_string_helper::to_octet_string(*this, val);
  return *this;
}

template class unbounded_octstring<true>;
template class unbounded_octstring<false>;

/*********************
     bitstring
*********************/

namespace bitstring_utils {

/**
 * Pack ASN1 bitstring length prefix. Accommodates for cases: fixed/unbounded/bounded, aligned/unaligned, with/out ext
 */
static OCUDUASN_CODE pack_length_prefix(bit_ref& bref,
                                        uint32_t len,
                                        uint32_t lb         = 0,
                                        uint32_t ub         = std::numeric_limits<uint32_t>::max(),
                                        bool     has_ext    = false,
                                        bool     is_aligned = false)
{
  if (has_ext and ub == std::numeric_limits<uint32_t>::max()) {
    log_error("has extension marker but it is an unbounded prefix size");
    return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  bool within_bounds = len >= lb and len <= ub;
  if (not within_bounds and not has_ext) {
    log_error("bitstring length={} is not within bounds [{}, {}]", len, lb, ub);
    return OCUDUASN_ERROR_ENCODE_FAIL;
  }

  // encode ext bit
  if (has_ext) {
    HANDLE_CODE(bref.pack(not within_bounds, 1));
  }

  // pack as unbounded if unbounded bitstring or ext is active
  if (ub == std::numeric_limits<uint32_t>::max() or not within_bounds) {
    return pack_length(bref, len, is_aligned);
  }

  // do not encode prefix if fixed size
  if (lb == ub) {
    return OCUDUASN_SUCCESS;
  }

  return pack_length(bref, len, lb, ub, is_aligned);
}

static OCUDUASN_CODE
pack_bitfield(bit_ref& bref, const uint8_t* buf, uint32_t nbits, uint32_t lb, uint32_t ub, bool is_aligned)
{
  if (nbits == 0) {
    // Empty bit string, which only the length prefix encodes.
    return OCUDUASN_SUCCESS;
  }
  if (is_aligned and (lb != ub or ub > 16)) {
    bref.align_bytes_zero();
  }
  uint32_t n_octs = ceil_frac(nbits, 8u);
  uint32_t offset = ((nbits - 1) % 8) + 1;
  HANDLE_CODE(bref.pack(buf[n_octs - 1], offset));
  // The other octets go out in the reverse order of buf, a stack chunk at a time.
  std::array<uint8_t, 64> chunk;
  for (uint32_t left = n_octs - 1; left != 0;) {
    uint32_t n = std::min(left, static_cast<uint32_t>(chunk.size()));
    std::reverse_copy(buf + left - n, buf + left, chunk.begin());
    HANDLE_CODE(bref.pack_bytes(ocudu::span<const uint8_t>{chunk.data(), n}));
    left -= n;
  }
  return OCUDUASN_SUCCESS;
}

/**
 * Pack ASN1 bitstring. Accommodates for cases: fixed/unbounded/bounded, aligned/unaligned, with/out ext
 */
OCUDUASN_CODE
pack(bit_ref& bref, const uint8_t* data, uint32_t len, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned)
{
  HANDLE_CODE(bitstring_utils::pack_length_prefix(bref, len, lb, ub, has_ext, is_aligned));
  if (len < lb or len > ub) {
    // Lengths outside the root range are encoded as if unbounded.
    return pack_bitfield(bref, data, len, 0, std::numeric_limits<uint32_t>::max(), is_aligned);
  }
  return pack_bitfield(bref, data, len, lb, ub, is_aligned);
}

// Unpack prefix, excluding ext bit

/**
 * Unpack ASN1 bitstring length prefix. Accommodates for cases: fixed/unbounded/bounded, aligned/unaligned, with/out ext
 */
OCUDUASN_CODE
unpack_length_prefix(uint32_t& len, cbit_ref& bref, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned)
{
  bool ext = false;
  if (has_ext) {
    HANDLE_CODE(bref.unpack(ext, 1));
  }

  // unbounded or ext is set
  if (ub == std::numeric_limits<uint32_t>::max() or ext) {
    return unpack_length(len, bref, is_aligned);
  }

  // fixed length with no ext
  if (lb == ub) {
    len = lb;
    return OCUDUASN_SUCCESS;
  }

  // constrained, simple case
  HANDLE_CODE(unpack_length(len, bref, lb, ub, is_aligned));
  // Lengths with an upper bound of 64K or more are decoded without range check.
  if (len < lb or len > ub) {
    log_error("bitstring length={} is not within bounds [{}, {}]", len, lb, ub);
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

// for both fixed, constrained and unconstrained scenarios
OCUDUASN_CODE unpack_bitfield(uint8_t* buf, cbit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool is_aligned)
{
  if (n > ASN_64K) {
    log_error("bitstrings longer than 64K not supported");
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  if (n == 0) {
    // empty bit string
    return OCUDUASN_SUCCESS;
  }
  if (is_aligned and (lb != ub or ub > 16)) {
    bref.align_bytes();
  }
  // unpack exactly "n" bits
  uint32_t n_octs = ceil_frac(n, 8u);
  uint32_t offset = ((n - 1) % 8) + 1;
  HANDLE_CODE(bref.unpack(buf[n_octs - 1], offset));
  // The other octets arrive in the reverse order of buf.
  HANDLE_CODE(bref.unpack_bytes({buf, n_octs - 1}));
  std::reverse(buf, buf + n_octs - 1);
  return OCUDUASN_SUCCESS;
}

void from_number(uint8_t* ptr, uint64_t number, uint32_t nbits)
{
  if (nbits > 64u) {
    log_error("bitstring of size={} does not fit in an uint64_t", nbits);
    return;
  }
  uint32_t nof_bytes = ceil_frac(nbits, 8u);
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    ptr[i] = (number >> (i * 8u)) & 0xffu;
  }
  uint32_t offset = nbits % 8; // clean up any extra set bit
  if (offset > 0) {
    ptr[nof_bytes - 1] &= (uint8_t)((1u << offset) - 1u);
  }
}

std::string to_string(const uint8_t* ptr, uint32_t nbits)
{
  std::string str(nbits, '\0');
  for (uint32_t i = 0; i < nbits; ++i) {
    str[i] = bitstring_utils::get(ptr, nbits - 1 - i) ? '1' : '0';
  }
  return str;
}

std::string to_hex_string(const uint8_t* ptr, uint32_t nbits)
{
  std::vector<uint8_t> octets(ceil_frac(nbits, 8u), 0);
  for (uint32_t i = 0; i < nbits; ++i) {
    if (bitstring_utils::get(ptr, nbits - 1 - i)) {
      octets[i / 8] |= 0x80u >> (i % 8);
    }
  }
  return octet_string_helper::to_hex_string(octets);
}

uint64_t to_number(const uint8_t* ptr, uint32_t nbits)
{
  if (nbits > 64u) {
    log_error("bitstring of size={} does not fit in an uint64_t", nbits);
    return 0;
  }
  uint64_t val       = 0;
  uint32_t nof_bytes = ceil_frac(nbits, 8u);
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    val += static_cast<uint64_t>(ptr[i]) << static_cast<uint64_t>(i * 8u);
  }
  return val;
}

OCUDUASN_CODE unpack(void* bits,
                     uint8_t* (*resize)(void* bits, uint32_t nof_bits),
                     cbit_ref& bref,
                     uint32_t  lb,
                     uint32_t  ub,
                     bool      has_ext,
                     bool      is_aligned)
{
  // X.691, subclause 15.11
  uint32_t nbits;
  HANDLE_CODE(unpack_length_prefix(nbits, bref, lb, ub, has_ext, is_aligned));
  uint8_t* data = resize(bits, nbits);
  if (nbits < lb or nbits > ub) {
    // Lengths outside the root range are encoded as if unbounded.
    return unpack_bitfield(data, bref, nbits, 0, std::numeric_limits<uint32_t>::max(), is_aligned);
  }
  return unpack_bitfield(data, bref, nbits, lb, ub, is_aligned);
}

void to_json(json_writer& j, const uint8_t* ptr, uint32_t nbits, bool fixed_size)
{
  if (fixed_size) {
    j.write_str(to_hex_string(ptr, nbits));
    return;
  }
  j.start_obj();
  j.write_str("value", to_hex_string(ptr, nbits));
  j.write_uint("length", nbits);
  j.end_obj();
}

} // namespace bitstring_utils

/*********************
   printable string
*********************/

namespace asn_string_utils {

static size_t get_nof_bits_per_char(size_t lb, size_t ub, bool aligned)
{
  size_t N = ub - lb + 1;
  size_t b = ceil_log2(N); // B
  // A single-character alphabet takes no bits, aligned or not.
  if (aligned and b != 0) {
    b = size_t{1} << ceil_log2(b); // B2
  }
  return b;
}

static bool is_octet_aligned(size_t bits_per_char, size_t alb, size_t aub, bool aligned)
{
  size_t max_nof_bits = bits_per_char * aub;
  return aligned and (max_nof_bits > 16 or (alb != aub and max_nof_bits == 16));
}

static constexpr bool is_length_encoded(size_t alb, size_t aub, bool aligned)
{
  return alb != aub or aub >= ASN_64K;
}

OCUDUASN_CODE
pack(bit_ref& bref, const std::string& s, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned)
{
  bool within_limits = s.size() >= alb and s.size() <= aub;
  if (not within_limits and not ext) {
    log_error("The PrintableString size={} is not within the limits [{}, {}]", s.size(), alb, aub);
    return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  if (ext) {
    HANDLE_CODE(bref.pack(not within_limits, 1));
  }
  if (not within_limits) {
    // Sizes outside the root range are encoded as if unbounded.
    alb = 0;
    aub = std::numeric_limits<uint32_t>::max();
  }
  size_t b              = asn_string_utils::get_nof_bits_per_char(lb, ub, aligned);
  bool   octet_aligned  = asn_string_utils::is_octet_aligned(b, alb, aub, aligned);
  bool   length_encoded = asn_string_utils::is_length_encoded(alb, aub, aligned);

  if (length_encoded) {
    HANDLE_CODE(pack_length(bref, s.size(), alb, aub, octet_aligned));
  }
  if (octet_aligned) {
    HANDLE_CODE(bref.align_bytes_zero());
  }
  for (char c : s) {
    HANDLE_CODE(bref.pack(c, b));
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE
unpack(std::string& s, cbit_ref& bref, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned)
{
  bool is_ext = false;
  if (ext) {
    HANDLE_CODE(bref.unpack(is_ext, 1));
  }
  if (is_ext) {
    // Sizes outside the root range are encoded as if unbounded.
    alb = 0;
    aub = std::numeric_limits<uint32_t>::max();
  }
  size_t b              = asn_string_utils::get_nof_bits_per_char(lb, ub, aligned);
  bool   octet_aligned  = asn_string_utils::is_octet_aligned(b, alb, aub, aligned);
  bool   length_encoded = asn_string_utils::is_length_encoded(alb, aub, aligned);
  if (length_encoded) {
    size_t n;
    HANDLE_CODE(unpack_length(n, bref, alb, aub, octet_aligned));
    s.resize(n);
  } else {
    s.resize(alb);
  }
  if (octet_aligned) {
    HANDLE_CODE(bref.align_bytes());
  }
  for (char& c : s) {
    HANDLE_CODE(bref.unpack(c, b));
  }
  return OCUDUASN_SUCCESS;
}

} // namespace asn_string_utils

} // namespace asn1
