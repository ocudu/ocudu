// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rohc_lib_compressor.h"
#include "rohc_lib_common.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <cstdlib>

using namespace ocudu;
using namespace ocudu::rohc;

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
  for (rohc_profile profile : all_rohc_profiles) {
    if (cfg.profiles.is_profile_enabled(profile)) {
      ok = rohc_comp_enable_profile(compressor, to_rohc_profile_t(profile));
      report_error_if_not(ok, "Failed to enable ROHC profile={}. {}", profile, cfg);
    }
  }

  input_packet_buf.resize(rohc_max_packet_size);
  input_feedback_buf.resize(rohc_max_packet_size);
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

bool rohc_lib_compressor::handle_feedback(byte_buffer feedback)
{
  bool     result = false;
  rohc_buf feedback_packet;
  if (feedback.is_contiguous()) {
    feedback_packet = rohc_buf_init_empty(feedback.segments().begin()->begin(), feedback.length());
  } else {
    feedback_packet = rohc_buf_init_empty(input_feedback_buf.data(), input_feedback_buf.size());
    std::copy(feedback.cbegin(), feedback.cend(), input_feedback_buf.begin());
  }
  feedback_packet.len = feedback.length();

  result = rohc_comp_deliver_feedback2(compressor, feedback_packet);
  if (!result) {
    logger.warning("Failed to deliver feedback to ROHC compressor. feedback_len={}", feedback.length());
  } else {
    logger.debug("Delivered feedback to ROHC compressor. feedback_len={}", feedback.length());
  }

  return result;
}

static int random_number_generator(const rohc_comp* const compressor, void* const context)
{
  return rand();
}
