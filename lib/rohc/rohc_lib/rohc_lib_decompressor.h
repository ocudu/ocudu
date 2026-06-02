// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/rohc/rohc_config.h"
#include "ocudu/rohc/rohc_decompressor.h"
#include <rohc/rohc.h>
#include <rohc/rohc_comp.h>
#include <rohc/rohc_decomp.h>

namespace ocudu::rohc {

class rohc_lib_decompressor : public rohc_decompressor
{
public:
  rohc_lib_decompressor(rohc_config cfg);
  ~rohc_lib_decompressor();
  virtual rohc_decromp_result decompress(byte_buffer packet) override;

private:
  ocudulog::basic_logger& logger;

  rohc_config  cfg;
  rohc_decomp* decompressor = nullptr;

  std::vector<uint8_t> input_packet_buf;
  std::vector<uint8_t> output_packet_buf;
  std::vector<uint8_t> output_feedback_buf;
};

} // namespace ocudu::rohc
