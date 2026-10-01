// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/psup/psup_message.h"

namespace ocudu {

/// Packing and unpacking of PDU Session Container
///
/// Ref: TS 38.415
class psup_packing
{
public:
  psup_packing(ocudulog::basic_logger& logger_) : logger(logger_) {}

  bool unpack(psup_dl_pdu_session_information& dl_pdu_session_information, byte_buffer_view container) const;
  bool pack(byte_buffer& out_buf, const psup_dl_pdu_session_information& dl_pdu_session_information) const;

private:
  ocudulog::basic_logger& logger;
};

} // namespace ocudu
