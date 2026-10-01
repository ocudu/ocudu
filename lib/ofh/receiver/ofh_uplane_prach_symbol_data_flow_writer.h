// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "../support/prach_context_repository.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace ofh {

struct uplane_message_decoder_results;

/// \brief Open Fronthaul User-Plane PRACH symbol data flow writer.
///
/// Writes IQ data received in an Open Fronthaul message to the corresponding PRACH buffer.
class uplane_prach_symbol_data_flow_writer
{
public:
  uplane_prach_symbol_data_flow_writer(span<const unsigned>                      prach_eaxc_,
                                       unsigned                                  sector_id_,
                                       ocudulog::basic_logger&                   logger_,
                                       std::shared_ptr<prach_context_repository> prach_context_repo_) :
    prach_eaxc(prach_eaxc_.begin(), prach_eaxc_.end()),
    sector_id(sector_id_),
    logger(logger_),
    prach_context_repo(std::move(prach_context_repo_))
  {
    ocudu_assert(prach_context_repo, "Invalid PRACH context repository");
  }

  /// Writes the given decoder results in the corresponding PRACH buffer. Returns true on success, otherwise false.
  bool write_to_prach_buffer(unsigned eaxc, const uplane_message_decoder_results& results);

private:
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  const unsigned                                        sector_id;
  ocudulog::basic_logger&                               logger;
  std::shared_ptr<prach_context_repository>             prach_context_repo;
};

} // namespace ofh
} // namespace ocudu
