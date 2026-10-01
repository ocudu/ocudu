// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/support/format/fmt_to_c_str.h"

namespace ocudu {
namespace odu {

class du_procedure_logger
{
public:
  du_procedure_logger(ocudulog::basic_logger& logger_, const char* proc_name_) : logger(logger_), proc_name(proc_name_)
  {
  }

  void log_proc_started() { log_impl(logger.debug, "Procedure started"); }

  template <typename... Args>
  void log_progress(const char* fmt_str, Args&&... args)
  {
    if (not logger.debug.enabled()) {
      return;
    }
    fmt::memory_buffer fmtbuf;
    fmt::format_to(std ::back_inserter(fmtbuf), fmt_str, std::forward<Args>(args)...);
    log_impl(logger.debug, to_c_str(fmtbuf));
  }

  void log_proc_completed() { log_impl(logger.debug, "Procedure finished successfully"); }

  template <typename... Args>
  void log_proc_failure(const char* reason_fmt, Args&&... args)
  {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(std ::back_inserter(fmtbuf), "Procedure failed. Cause: ");
    fmt::format_to(std ::back_inserter(fmtbuf), reason_fmt, std::forward<Args>(args)...);
    log_impl(logger.warning, to_c_str(fmtbuf));
  }

private:
  void log_impl(ocudulog::log_channel& log_ch, const char* result_str)
  {
    log_ch("proc=\"{}\": {}.", proc_name, result_str);
  }

  ocudulog::basic_logger& logger;
  const char*             proc_name;
};

class ue_procedure_logger
{
public:
  ue_procedure_logger(ocudulog::basic_logger& logger_,
                      const char*             proc_name_,
                      du_ue_index_t           ue_index_,
                      rnti_t                  rnti_ = rnti_t::INVALID_RNTI) :
    logger(logger_), proc_name(proc_name_), ue_index(ue_index_), rnti(rnti_)
  {
  }

  void log_proc_started() { log_impl(logger.info, "Procedure started..."); }

  void log_proc_completed() { log_impl(logger.info, "Procedure finished successfully"); }

  template <typename... Args>
  void log_proc_failure(const char* reason_fmt, Args&&... args)
  {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(std ::back_inserter(fmtbuf), "Procedure failed. Cause: ");
    fmt::format_to(std ::back_inserter(fmtbuf), reason_fmt, std::forward<Args>(args)...);
    log_impl(logger.warning, to_c_str(fmtbuf));
  }

  template <typename... Args>
  void log_proc_warning(const char* event_fmt, Args&&... args) const
  {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(std ::back_inserter(fmtbuf), event_fmt, std::forward<Args>(args)...);
    log_impl(logger.warning, to_c_str(fmtbuf));
  }

private:
  void log_impl(ocudulog::log_channel& log_ch, const char* result_str) const
  {
    if (rnti == rnti_t::INVALID_RNTI) {
      log_ch("ue={} proc=\"{}\": {}.", ue_index, proc_name, result_str);
    } else {
      log_ch("ue={} rnti={} proc=\"{}\": {}.", ue_index, rnti, proc_name, result_str);
    }
  }

  ocudulog::basic_logger& logger;
  const char*             proc_name;
  du_ue_index_t           ue_index;
  rnti_t                  rnti;
};

} // namespace odu
} // namespace ocudu
