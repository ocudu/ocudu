// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rohc_lib_engine.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <cstdlib>

using namespace ocudu;
using namespace ocudu::rohc;

rohc_lib_engine::rohc_lib_engine() : logger(ocudulog::fetch_basic_logger("ROHC"))
{
  logger.info("Created ROHC engine with ROHC library version {}", rohc_version());
}

// Compressor functions.

static int random_number_generator(const rohc_comp* compressor, void* context);

rohc_lib_compressor::rohc_lib_compressor(rohc_config cfg_) : logger(ocudulog::fetch_basic_logger("ROHC")), cfg(cfg_)
{
  rohc_cid_type_t cid_type = rohc_lib_get_cid_type(cfg.max_cid);
  // TODO: Inject an initialized RNG
  compressor = rohc_comp_new2(cid_type, cfg.max_cid, random_number_generator, nullptr);
  report_error_if_not(compressor != nullptr, "Failed to allocate ROHC compressor. {}", cfg);

  // Disable ROHC segmentation. See TS 38.323 Sec. 5.7.3.
  bool ok = rohc_comp_set_mrru(compressor, 0);
  report_error_if_not(ok, "Failed to disable ROHC segmentation. {}", cfg);

  // Enable uncompressed profile (always on).
  ok = rohc_comp_enable_profile(compressor, ROHC_PROFILE_UNCOMPRESSED);
  report_error_if_not(ok, "Failed to enable ROHC profile=uncompressed. {}", cfg);

  // Enable configured profiles.
  for (rohc_profile profile : rohc::all_rohc_profiles) {
    if (cfg.profiles.is_profile_enabled(profile)) {
      ok = rohc_comp_enable_profile(compressor, to_rohc_profile_t(profile));
      report_error_if_not(ok, "Failed to enable ROHC profile={}. {}", profile, cfg);
    }
  }

  input_packet_buf.resize(rohc_max_packet_size);
  output_packet_buf.resize(rohc_max_packet_size);

  logger.info("Created ROHC compressor with ROHC library version {}. {}", rohc_version(), cfg);
}

rohc_lib_compressor::~rohc_lib_compressor()
{
  rohc_comp_free(compressor);
}

byte_buffer rohc_lib_compressor::compress(byte_buffer packet)
{
  rohc_buf input_packet;
  if (packet.is_contiguous()) {
    input_packet = rohc_buf_init_empty(packet.segments().begin()->begin(), packet.length());
  } else {
    input_packet = rohc_buf_init_empty(input_packet_buf.data(), input_packet_buf.size());
    std::copy(packet.cbegin(), packet.cend(), input_packet_buf.begin());
  }
  input_packet.len = packet.length();

  rohc_buf    output_packet = rohc_buf_init_empty(output_packet_buf.data(), output_packet_buf.size());
  byte_buffer result{};

  rohc_status_t ret = rohc_compress4(compressor, input_packet, &output_packet);
  if (ret != rohc_status_t::ROHC_STATUS_OK) {
    logger.warning("ROHC compressor failed. error={} packet_len={}", rohc_strerror(ret), packet.length());
    return result;
  }

  if (!rohc_buf_is_empty(output_packet)) {
    if (!result.append(span{output_packet.data + output_packet.offset, output_packet.len})) {
      logger.warning("Failed to copy compressed packet. output_packet_len={}", output_packet.len);
    } else {
      logger.debug("Compressed packet. output_packet_len={}", output_packet.len);
    }
  }

  if (result.empty()) {
    logger.warning("Compression did not yield any output");
  }

  return result;
}

static int random_number_generator(const rohc_comp* const compressor, void* const context)
{
  return rand();
}

// Decompressor functions.

rohc_lib_decompressor::rohc_lib_decompressor(rohc_config cfg_) : logger(ocudulog::fetch_basic_logger("ROHC")), cfg(cfg_)
{
  rohc_cid_type_t cid_type = rohc_lib_get_cid_type(cfg.max_cid);
  // TODO: Check MODE.
  decompressor = rohc_decomp_new2(cid_type, cfg.max_cid, ROHC_U_MODE);
  report_error_if_not(decompressor != nullptr, "Failed to allocate ROHC decompressor. {}", cfg);

  // Enable uncompressed profile (always on).
  bool ok = rohc_decomp_enable_profile(decompressor, ROHC_PROFILE_UNCOMPRESSED);
  report_error_if_not(ok, "Failed to enable ROHC profile=uncompressed. {}", cfg);

  // Enable configured profiles.
  for (rohc_profile profile : rohc::all_rohc_profiles) {
    if (cfg.profiles.is_profile_enabled(profile)) {
      ok = rohc_decomp_enable_profile(decompressor, to_rohc_profile_t(profile));
      report_error_if_not(ok, "Failed to enable ROHC profile={}. {}", profile, cfg);
    }
  }

  input_packet_buf.resize(rohc_max_packet_size);
  output_packet_buf.resize(rohc_max_packet_size);
  output_feedback_buf.resize(rohc_max_packet_size);

  logger.info("Created ROHC decompressor with ROHC library version {}. {}", rohc_version(), cfg);
}

rohc_lib_decompressor::~rohc_lib_decompressor()
{
  rohc_decomp_free(decompressor);
}

rohc_decromp_result rohc_lib_decompressor::decompress(byte_buffer packet)
{
  rohc_buf input_packet;
  if (packet.is_contiguous()) {
    input_packet = rohc_buf_init_empty(packet.segments().begin()->begin(), packet.length());
  } else {
    input_packet = rohc_buf_init_empty(input_packet_buf.data(), input_packet_buf.size());
    std::copy(packet.cbegin(), packet.cend(), input_packet_buf.begin());
  }
  input_packet.len = packet.length();

  rohc_buf            output_packet   = rohc_buf_init_empty(output_packet_buf.data(), output_packet_buf.size());
  rohc_buf            output_feedback = rohc_buf_init_empty(output_feedback_buf.data(), output_feedback_buf.size());
  rohc_buf*           input_feedback  = nullptr;
  rohc_decromp_result result{};

  rohc_status_t ret = rohc_decompress3(decompressor, input_packet, &output_packet, input_feedback, &output_feedback);
  if (ret != rohc_status_t::ROHC_STATUS_OK) {
    logger.warning("ROHC decompressor failed. error={} packet_len={}", rohc_strerror(ret), packet.length());
    return result;
  }

  if (!rohc_buf_is_empty(output_packet)) {
    if (!result.decomp_packet.append(span{output_packet.data + output_packet.offset, output_packet.len})) {
      logger.warning("Failed to copy decompressed packet. output_packet_len={}", output_packet.len);
    } else {
      logger.debug("Decompressed packet. output_packet_len={}", output_packet.len);
    }
  }

  if (!rohc_buf_is_empty(output_feedback)) {
    if (!result.feedback_packet.append(span{output_feedback.data + output_feedback.offset, output_feedback.len})) {
      logger.warning("Failed to copy feedback packet. output_feedback_len={}", output_feedback.len);
    } else {
      logger.debug("Decompressed feedback. output_feedback_len={}", output_feedback.len);
    }
  }

  if (result.decomp_packet.empty() && result.feedback_packet.empty()) {
    logger.warning("Decompression did not yield any output or feedback");
  }

  return result;
}
