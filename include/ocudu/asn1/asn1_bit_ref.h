// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Bit references, error codes and logging of the ASN.1 runtime.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/compiler.h"
#include <cstdint>
#include <utility>

namespace asn1 {

/************************
        logging
************************/

template <typename... Args>
void log_error(const char* format, Args&&... args)
{
  ocudulog::fetch_basic_logger("ASN1").error(format, std::forward<Args>(args)...);
}

template <typename... Args>
void log_warning(const char* format, Args&&... args)
{
  ocudulog::fetch_basic_logger("ASN1").warning(format, std::forward<Args>(args)...);
}

void warn_assert(bool cond, const char* filename, int lineno);
void log_invalid_choice_id(uint32_t val, const char* choice_type);
void invalid_enum_number(int value, const char* name);

/************************
     error handling
************************/

enum OCUDUASN_CODE {
  OCUDUASN_ERROR             = -3,
  OCUDUASN_ERROR_ENCODE_FAIL = -2,
  OCUDUASN_ERROR_DECODE_FAIL = -1,
  OCUDUASN_SUCCESS           = 0
};

void log_error_code(OCUDUASN_CODE code, const char* filename, int line);

#define HANDLE_CODE(ret)                                                                                               \
  do {                                                                                                                 \
    OCUDUASN_CODE macrocode = ((ret));                                                                                 \
    if (macrocode != OCUDUASN_SUCCESS) {                                                                               \
      log_error_code(macrocode, __FILE__, __LINE__);                                                                   \
      return macrocode;                                                                                                \
    }                                                                                                                  \
  } while (0)

/************************
        bit_ref
************************/

// write+read bit_ref version
struct bit_ref {
  bit_ref(ocudu::byte_buffer_writer byte_writer) : writer(byte_writer) {}

  ocudu::byte_buffer_view data() const { return writer.view(); }

  int distance_bytes() const;
  int distance() const;

  OCUDU_FORCE_INLINE OCUDUASN_CODE pack(uint64_t val, uint32_t n_bits)
  {
    // Bits that fit in the last byte, which most fields are.
    if (offset != 0 and n_bits < 8U - offset) {
      writer.back() += static_cast<uint8_t>((val & ((1UL << n_bits) - 1UL)) << (8U - offset - n_bits));
      offset += n_bits;
      return OCUDUASN_SUCCESS;
    }
    return pack_across_bytes(val, n_bits);
  }
  OCUDUASN_CODE pack_bytes(ocudu::span<const uint8_t> bytes);
  OCUDUASN_CODE pack_bytes(ocudu::byte_buffer_view bytes);
  OCUDUASN_CODE align_bytes_zero();

private:
  OCUDUASN_CODE pack_across_bytes(uint64_t val, uint32_t n_bits);
  template <class Bytes>
  OCUDUASN_CODE pack_unaligned_bytes(const Bytes& bytes);

  ocudu::byte_buffer_writer writer;
  uint8_t                   offset = 0;
};

// read-only bit_ref
struct cbit_ref {
  cbit_ref(ocudu::byte_buffer_view buffer_) : buffer(buffer_), it(buffer.begin()) {}
  cbit_ref(const bit_ref& bref) : buffer(bref.data()), it(bref.data().begin()) {}

  /// Construct a cbit_ref with an offset and max size, starting from the current cbit_ref position.
  cbit_ref subview(uint32_t offset_bytes, uint32_t len_bytes) const;

  ocudu::byte_buffer_view data() const { return buffer; }

  int           distance_bytes() const;
  int           distance() const;
  int           distance(const cbit_ref& other) const;
  OCUDUASN_CODE advance_bits(uint32_t n_bits);
  OCUDUASN_CODE advance_bytes(uint32_t bytes);

  template <class T>
  OCUDU_FORCE_INLINE OCUDUASN_CODE unpack(T& val, uint32_t n_bits)
  {
    // Bits within the current byte, which most fields are.
    uint32_t left = 8U - offset;
    if (n_bits != 0 and n_bits < left and it != buffer.end()) {
      val = static_cast<T>((*it >> (left - n_bits)) & ((1U << n_bits) - 1U));
      offset += n_bits;
      return OCUDUASN_SUCCESS;
    }
    return unpack_across_bytes(val, n_bits);
  }
  OCUDUASN_CODE unpack_bytes(ocudu::span<uint8_t> bytes);
  OCUDUASN_CODE align_bytes();

private:
  template <class T>
  OCUDUASN_CODE unpack_across_bytes(T& val, uint32_t n_bits);

  ocudu::byte_buffer_view            buffer;
  ocudu::byte_buffer::const_iterator it;
  uint8_t                            offset = 0;
};

} // namespace asn1
