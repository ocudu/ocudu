// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Logging, error codes and the bit_ref and cbit_ref bit cursors.

#include "ocudu/asn1/asn1_bit_ref.h"
#include "ocudu/asn1/asn1_containers.h"

namespace asn1 {

/************************
        logging
************************/

void warn_assert(bool cond, const char* filename, int lineno)
{
  if (cond) {
    log_warning("Assertion in [{}][{}] failed.", filename, lineno);
  }
}

void invalid_enum_number(int value, const char* name)
{
  log_error("The provided enum value={} of type {} cannot be translated into a number", value, name);
}

/************************
     error handling
************************/

void log_error_code(OCUDUASN_CODE code, const char* filename, int line)
{
  switch (code) {
    case OCUDUASN_ERROR_ENCODE_FAIL:
      log_error("[{}][{}] Encoding failure.", filename, line);
      break;
    case OCUDUASN_ERROR_DECODE_FAIL:
      log_error("[{}][{}] Decoding failure.", filename, line);
      break;
    default:
      log_warning("[{}][{}] OCUDUASN_CODE={} not recognized.", filename, line, (int)code);
  }
}

/*********************
     choice utils
*********************/

void log_invalid_choice_id(uint32_t val, const char* choice_type)
{
  log_error("Invalid choice id={} for choice type {}", (size_t)val, choice_type);
}

/*********************
       bit_ref
*********************/

int bit_ref::distance_bytes() const
{
  return static_cast<int>(writer.length());
}

int bit_ref::distance() const
{
  return 8 * (static_cast<int>(writer.length()) - (offset == 0 ? 0 : 1)) + offset;
}

OCUDUASN_CODE bit_ref::pack_across_bytes(uint64_t val, uint32_t n_bits)
{
  ocudu_assert(n_bits <= 64, "Invalid number of bits passed to pack()");
  if (n_bits == 0) {
    return OCUDUASN_SUCCESS;
  }
  // Avoids the undefined 64-bit shift.
  if (n_bits < 64) {
    val &= (1UL << n_bits) - 1UL;
  }
  if (offset != 0) {
    // Fills the free bits of the last byte first.
    uint32_t left = 8U - offset;
    if (n_bits < left) {
      writer.back() += static_cast<uint8_t>(val << (left - n_bits));
      offset += n_bits;
      return OCUDUASN_SUCCESS;
    }
    n_bits -= left;
    writer.back() += static_cast<uint8_t>(val >> n_bits);
    offset = 0;
  }
  while (n_bits >= 8) {
    n_bits -= 8;
    if (not writer.append(static_cast<uint8_t>(val >> n_bits))) {
      return OCUDUASN_ERROR;
    }
  }
  if (n_bits > 0) {
    if (not writer.append(static_cast<uint8_t>(val << (8U - n_bits)))) {
      return OCUDUASN_ERROR;
    }
    offset = n_bits;
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE bit_ref::pack_bytes(ocudu::span<const uint8_t> bytes)
{
  if (bytes.empty()) {
    return OCUDUASN_SUCCESS;
  }
  if (offset == 0) {
    // Aligned case
    HANDLE_CODE(writer.append(bytes) ? OCUDUASN_SUCCESS : OCUDUASN_ERROR_ENCODE_FAIL);
    return OCUDUASN_SUCCESS;
  }
  return pack_unaligned_bytes(bytes);
}

OCUDUASN_CODE bit_ref::pack_bytes(ocudu::byte_buffer_view bytes)
{
  if (bytes.empty()) {
    return OCUDUASN_SUCCESS;
  }
  if (offset == 0) {
    // Aligned case.
    HANDLE_CODE(writer.append(bytes) ? OCUDUASN_SUCCESS : OCUDUASN_ERROR_ENCODE_FAIL);
    return OCUDUASN_SUCCESS;
  }
  return pack_unaligned_bytes(bytes);
}

template <class Bytes>
OCUDUASN_CODE bit_ref::pack_unaligned_bytes(const Bytes& bytes)
{
  // Each byte ends the last octet and starts the next one, so the offset stays.
  for (uint8_t byte : bytes) {
    writer.back() |= static_cast<uint8_t>(byte >> offset);
    HANDLE_CODE(writer.append(static_cast<uint8_t>(byte << (8U - offset))) ? OCUDUASN_SUCCESS
                                                                           : OCUDUASN_ERROR_ENCODE_FAIL);
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE bit_ref::align_bytes_zero()
{
  offset = 0;
  return OCUDUASN_SUCCESS;
}

cbit_ref cbit_ref::subview(uint32_t offset_bytes, uint32_t len_bytes) const
{
  const uint32_t buffer_rem_bytes = buffer.end() - it;
  const uint32_t nof_bytes_needed = len_bytes + ((offset != 0) ? 1 : 0); // account for the last bits in the last byte.
  if (offset_bytes + nof_bytes_needed > buffer_rem_bytes) {
    log_error("subview: Buffer size limit was achieved");
    return cbit_ref(ocudu::byte_buffer_view{});
  }

  auto     start_it = it + offset_bytes;
  cbit_ref v{ocudu::byte_buffer_view(start_it, start_it + nof_bytes_needed)};
  v.offset = offset;
  return v;
}

int cbit_ref::distance_bytes() const
{
  return static_cast<int>(it - buffer.begin()) + (offset != 0 ? 1 : 0);
}

int cbit_ref::distance() const
{
  return (int)offset + 8 * static_cast<int>(it - buffer.begin());
}

int cbit_ref::distance(const cbit_ref& other) const
{
  return static_cast<int>(it - other.it) * 8 + (static_cast<int>(offset) - static_cast<int>(other.offset));
}

template <class T>
OCUDUASN_CODE cbit_ref::unpack_across_bytes(T& val, uint32_t n_bits)
{
  if (n_bits > sizeof(T) * 8) {
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  val = 0;
  if (n_bits == 0) {
    return OCUDUASN_SUCCESS;
  }
  if (it == buffer.end()) {
    log_error("{}: Buffer size limit {} was reached.", __FUNCTION__, buffer.length());
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  // Starts with the bits left in the current byte.
  uint32_t left = 8U - offset;
  uint64_t acc  = *it & ((1U << left) - 1U);
  if (n_bits < left) {
    val = static_cast<T>(acc >> (left - n_bits));
    offset += n_bits;
    return OCUDUASN_SUCCESS;
  }
  n_bits -= left;
  offset = 0;
  ++it;
  while (n_bits >= 8) {
    if (it == buffer.end()) {
      log_error("{}: Buffer size limit {} was reached.", __FUNCTION__, buffer.length());
      return OCUDUASN_ERROR_DECODE_FAIL;
    }
    acc = (acc << 8U) | *it;
    ++it;
    n_bits -= 8;
  }
  if (n_bits > 0) {
    if (it == buffer.end()) {
      log_error("{}: Buffer size limit {} was reached.", __FUNCTION__, buffer.length());
      return OCUDUASN_ERROR_DECODE_FAIL;
    }
    acc    = (acc << n_bits) | (*it >> (8U - n_bits));
    offset = n_bits;
  }
  val = static_cast<T>(acc);
  return OCUDUASN_SUCCESS;
}
template OCUDUASN_CODE cbit_ref::unpack_across_bytes<bool>(bool&, uint32_t n_bits);
template OCUDUASN_CODE cbit_ref::unpack_across_bytes<char>(char&, uint32_t n_bits);
template OCUDUASN_CODE cbit_ref::unpack_across_bytes<uint8_t>(uint8_t&, uint32_t n_bits);
template OCUDUASN_CODE cbit_ref::unpack_across_bytes<uint16_t>(uint16_t&, uint32_t n_bits);
template OCUDUASN_CODE cbit_ref::unpack_across_bytes<uint32_t>(uint32_t&, uint32_t n_bits);
template OCUDUASN_CODE cbit_ref::unpack_across_bytes<uint64_t>(uint64_t&, uint32_t n_bits);

OCUDUASN_CODE cbit_ref::unpack_bytes(ocudu::span<uint8_t> bytes)
{
  if (bytes.empty()) {
    return OCUDUASN_SUCCESS;
  }
  // Unaligned bytes end in the octet after the last one.
  if (static_cast<std::ptrdiff_t>(bytes.size() + (offset != 0 ? 1 : 0)) > buffer.end() - it) {
    log_error("unpack_bytes: Buffer size limit was achieved");
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  if (offset == 0) {
    // Aligned case
    for (uint8_t& byte : bytes) {
      byte = *it;
      ++it;
    }
    return OCUDUASN_SUCCESS;
  }
  uint8_t cur = *it;
  for (uint8_t& byte : bytes) {
    ++it;
    uint8_t next = *it;
    byte         = static_cast<uint8_t>((cur << offset) | (next >> (8U - offset)));
    cur          = next;
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE cbit_ref::align_bytes()
{
  if (offset != 0) {
    if (it == buffer.end()) {
      log_error("{}: Buffer size limit {} was achieved.", __FUNCTION__, buffer.length());
      return OCUDUASN_ERROR_DECODE_FAIL;
    }
    ++it;
    offset = 0;
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE cbit_ref::advance_bits(uint32_t n_bits)
{
  uint32_t extra_bits     = (offset + n_bits) % 8;
  uint32_t bytes_required = ceil_frac(offset + n_bits, 8U);
  uint32_t bytes_offset   = (offset + n_bits) / 8U;

  if (bytes_required > buffer.end() - it) {
    log_error("advance_bytes: Buffer size limit was achieved");
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  it += bytes_offset;
  offset = extra_bits;
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE cbit_ref::advance_bytes(uint32_t bytes)
{
  if (bytes + (offset != 0 ? 1 : 0) > buffer.end() - it) {
    log_error("advance_bytes: Buffer size limit was achieved");
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  it += bytes;
  return OCUDUASN_SUCCESS;
}

} // namespace asn1
