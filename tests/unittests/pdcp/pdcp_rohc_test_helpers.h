// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rohc/rohc_config.h"
#include "ocudu/rohc/rohc_compressor.h"
#include "ocudu/rohc/rohc_decompressor.h"
#include "ocudu/rohc/rohc_factory.h"

namespace ocudu::rohc {

class dummy_rohc_compressor : public rohc_compressor
{
public:
  dummy_rohc_compressor()  = default;
  ~dummy_rohc_compressor() = default;

  byte_buffer compress(byte_buffer packet) override { return packet; }
  bool        handle_feedback(byte_buffer feedback) override { return true; }
};

class dummy_rohc_decompressor : public rohc_decompressor
{
public:
  dummy_rohc_decompressor()  = default;
  ~dummy_rohc_decompressor() = default;

  rohc_decromp_result decompress(byte_buffer packet) override
  {
    rohc_decromp_result result = {};
    result.decomp_packet       = std::move(packet);
    // no feedback;
    return result;
  }
};

class dummy_rohc_factory : public rohc_factory
{
private:
  mutable unsigned                   nof_compressors   = 0;
  mutable unsigned                   nof_decompressors = 0;
  mutable std::optional<rohc_config> last_compressor_config;
  mutable std::optional<rohc_config> last_decompressor_config;

public:
  dummy_rohc_factory()  = default;
  ~dummy_rohc_factory() = default;

  std::unique_ptr<rohc_compressor> create_rohc_compressor(const rohc_config& cfg) const override
  {
    nof_compressors++;
    last_compressor_config = cfg;
    return std::make_unique<dummy_rohc_compressor>();
  }

  std::unique_ptr<rohc_decompressor> create_rohc_decompressor(const rohc_config& cfg) const override
  {
    nof_decompressors++;
    last_decompressor_config = cfg;
    return std::make_unique<dummy_rohc_decompressor>();
  }

  unsigned                          get_nof_compressors() { return nof_compressors; }
  unsigned                          get_nof_decompressors() { return nof_decompressors; }
  const std::optional<rohc_config>& get_last_compressor_config() { return last_compressor_config; }
  const std::optional<rohc_config>& get_last_decompressor_config() { return last_decompressor_config; }
};

} // namespace ocudu::rohc
