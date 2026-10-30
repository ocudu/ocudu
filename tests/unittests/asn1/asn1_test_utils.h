// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/asn1_utils.h"
#include <string>
#include <vector>

/// Converts a hex string into bytes.
inline std::vector<uint8_t> hex_to_bytes(const std::string& hex)
{
  std::vector<uint8_t> bytes;
  bytes.reserve(hex.size() / 2);
  for (std::size_t i = 0; i + 1 < hex.size(); i += 2) {
    bytes.push_back(static_cast<uint8_t>(std::stoul(hex.substr(i, 2), nullptr, 16)));
  }
  return bytes;
}

/// PDU captured from a real network, as a hex string.
struct capture_pdu {
  const char* name;
  const char* hex;
};

/// Packs a message, unpacks it and packs it again, checking that both encodings match.
template <class Msg>
asn1::OCUDUASN_CODE test_pack_roundtrip(const Msg& msg)
{
  ocudu::byte_buffer buf;
  asn1::bit_ref      bref(buf);
  if (msg.pack(bref) != asn1::OCUDUASN_SUCCESS) {
    asn1::log_error_code(asn1::OCUDUASN_ERROR_ENCODE_FAIL, __FILE__, __LINE__);
    return asn1::OCUDUASN_ERROR_ENCODE_FAIL;
  }

  Msg            msg2;
  asn1::cbit_ref cbref(buf);
  if (msg2.unpack(cbref) != asn1::OCUDUASN_SUCCESS) {
    asn1::log_error_code(asn1::OCUDUASN_ERROR_DECODE_FAIL, __FILE__, __LINE__);
    return asn1::OCUDUASN_ERROR_DECODE_FAIL;
  }
  if (cbref.distance() != bref.distance()) {
    asn1::log_error("Unpacked {} bits out of {} packed bits", cbref.distance(), bref.distance());
    return asn1::OCUDUASN_ERROR_DECODE_FAIL;
  }

  ocudu::byte_buffer buf2;
  asn1::bit_ref      bref2(buf2);
  if (msg2.pack(bref2) != asn1::OCUDUASN_SUCCESS) {
    asn1::log_error_code(asn1::OCUDUASN_ERROR_ENCODE_FAIL, __FILE__, __LINE__);
    return asn1::OCUDUASN_ERROR_ENCODE_FAIL;
  }
  if (bref2.distance() != bref.distance() or buf2 != buf) {
    asn1::log_error("Repacked message differs from the first packing");
    return asn1::OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return asn1::OCUDUASN_SUCCESS;
}

/// Unpacks a canonical PER encoding, converts it to JSON and checks that it packs back to the same bytes.
template <class Msg>
asn1::OCUDUASN_CODE test_unpack_roundtrip(const ocudu::byte_buffer& encoded)
{
  Msg            msg;
  asn1::cbit_ref cbref(encoded);
  if (msg.unpack(cbref) != asn1::OCUDUASN_SUCCESS) {
    asn1::log_error_code(asn1::OCUDUASN_ERROR_DECODE_FAIL, __FILE__, __LINE__);
    return asn1::OCUDUASN_ERROR_DECODE_FAIL;
  }

  asn1::json_writer j;
  msg.to_json(j);

  ocudu::byte_buffer repacked;
  asn1::bit_ref      bref(repacked);
  if (msg.pack(bref) != asn1::OCUDUASN_SUCCESS) {
    asn1::log_error_code(asn1::OCUDUASN_ERROR_ENCODE_FAIL, __FILE__, __LINE__);
    return asn1::OCUDUASN_ERROR_ENCODE_FAIL;
  }
  // Non-canonical encodings, e.g. with nonzero padding bits, legitimately repack to different bytes.
  if (repacked != encoded) {
    asn1::log_error("Repacked message differs from the original encoding");
    return asn1::OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return test_pack_roundtrip(msg);
}

/// Unpacks a canonical PER encoding, converts it to JSON and checks that it packs back to the same bytes.
template <class Msg>
asn1::OCUDUASN_CODE test_unpack_roundtrip(const std::vector<uint8_t>& encoded)
{
  return test_unpack_roundtrip<Msg>(ocudu::byte_buffer::create(encoded).value());
}
