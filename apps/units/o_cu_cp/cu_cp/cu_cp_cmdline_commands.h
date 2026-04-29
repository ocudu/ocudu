// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/cmdline/cmdline_command.h"
#include "apps/services/cmdline/cmdline_command_dispatcher_utils.h"
#include "cu_cp_unit_config_helpers.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/rnti.h"
#include <chrono>
#include <optional>
#include <vector>

namespace ocudu {

/// Application command to trigger a handover.
class handover_app_command : public app_services::cmdline_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit handover_app_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "ho"; }

  // See interface for documentation.
  std::string_view get_description() const override { return " <serving pci> <rnti> <target pci>: force UE handover"; }

  // See interface for documentation.
  void execute(span<const std::string> args) override
  {
    if (args.size() != 3) {
      fmt::print("Invalid handover command structure. Usage: ho <serving pci> <rnti> <target pci>\n");
      return;
    }

    const auto*                     arg         = args.begin();
    expected<unsigned, std::string> serving_pci = app_services::parse_int<unsigned>(*arg);
    if (not serving_pci.has_value()) {
      fmt::print("Invalid serving PCI.\n");
      return;
    }
    ++arg;
    expected<unsigned, std::string> rnti = app_services::parse_unsigned_hex<unsigned>(*arg);
    if (not rnti.has_value()) {
      fmt::print("Invalid UE RNTI.\n");
      return;
    }
    ++arg;
    expected<unsigned, std::string> target_pci = app_services::parse_int<unsigned>(*arg);
    if (not target_pci.has_value()) {
      fmt::print("Invalid target PCI.\n");
      return;
    }

    cu_cp.get_mobility_command_handler().trigger_handover(static_cast<pci_t>(serving_pci.value()),
                                                          static_cast<rnti_t>(rnti.value()),
                                                          static_cast<pci_t>(target_pci.value()));
    fmt::print("Handover triggered for UE with pci={} rnti={} to pci={}.\n",
               serving_pci.value(),
               static_cast<rnti_t>(rnti.value()),
               target_pci.value());
  }
};

/// Application command to trigger a Conditional Handover (CHO).
class cho_app_command : public app_services::cmdline_command
{
  ocucp::cu_cp_command_handler& cu_cp;
  std::chrono::milliseconds     default_timeout;

public:
  explicit cho_app_command(ocucp::cu_cp_command_handler& cu_cp_,
                           std::chrono::milliseconds     default_timeout_ = std::chrono::milliseconds{10000}) :
    cu_cp(cu_cp_), default_timeout(default_timeout_)
  {
  }

  // See interface for documentation.
  std::string_view get_name() const override { return "cho"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return " <serving pci> <rnti> <target pci> [<target pci2> ...] [timeout <s>] [t1 <datetime>]: trigger conditional "
           "handover";
  }

  // See interface for documentation.
  void execute(span<const std::string> args) override
  {
    if (args.size() < 3) {
      fmt::print("Invalid cho command structure.\n");
      fmt::print("Usage: cho <serving pci> <rnti> <target pci1> [<target pci2> ... <target pci8>] [timeout <s>] [t1 "
                 "<datetime>]\n");
      return;
    }

    const auto* arg = args.begin();

    // Parse serving PCI
    expected<unsigned, std::string> serving_pci = app_services::parse_int<unsigned>(*arg);
    if (not serving_pci.has_value()) {
      fmt::print("Invalid serving PCI.\n");
      return;
    }
    ++arg;

    // Parse RNTI
    expected<unsigned, std::string> rnti = app_services::parse_unsigned_hex<unsigned>(*arg);
    if (not rnti.has_value()) {
      fmt::print("Invalid UE RNTI.\n");
      return;
    }
    ++arg;

    // Parse target PCIs and optional keywords (timeout, t1).
    // Examples: "cho 1 0x4601 100 200" -> 2 PCIs, default timeout from config
    //           "cho 1 0x4601 100 200 timeout 30" -> 2 PCIs, 30s timeout
    //           "cho 1 0x4601 100 t1 2026-03-01T10:00:00" -> 1 PCI, T1 override
    //           "cho 1 0x4601 100 timeout 30 t1 2026-03-01T10:00:00" -> 1 PCI, both
    std::vector<pci_t>                                   target_pcis;
    std::chrono::milliseconds                            timeout = default_timeout;
    std::optional<std::chrono::system_clock::time_point> t1_thres_override;

    while (arg != args.end()) {
      if (*arg == "timeout") {
        ++arg;
        if (arg == args.end()) {
          fmt::print("Missing timeout value after 'timeout' keyword.\n");
          return;
        }
        expected<unsigned, std::string> timeout_s = app_services::parse_int<unsigned>(*arg);
        if (not timeout_s.has_value()) {
          fmt::print("Invalid timeout value: {}.\n", *arg);
          return;
        }
        timeout = std::chrono::seconds{timeout_s.value()};
        ++arg;
        continue;
      }

      if (*arg == "t1") {
        ++arg;
        if (arg == args.end()) {
          fmt::print("Missing datetime value after 't1' keyword.\n");
          return;
        }
        auto t1_result = parse_timestamp_ms(*arg);
        if (not t1_result.has_value()) {
          fmt::print("Invalid t1 datetime '{}': {}.\n", *arg, t1_result.error());
          return;
        }
        t1_thres_override = t1_result.value();
        ++arg;
        continue;
      }

      expected<unsigned, std::string> target_pci = app_services::parse_int<unsigned>(*arg);
      if (not target_pci.has_value()) {
        fmt::print("Invalid target PCI: {}.\n", *arg);
        return;
      }
      target_pcis.push_back(static_cast<pci_t>(target_pci.value()));
      ++arg;
    }

    if (target_pcis.empty()) {
      fmt::print("No target PCIs provided.\n");
      return;
    }
    if (target_pcis.size() > 8) {
      fmt::print("Too many target PCIs: {}. Maximum 8 candidates supported per 3GPP.\n", target_pcis.size());
      return;
    }

    cu_cp.get_mobility_command_handler().trigger_conditional_handover(static_cast<pci_t>(serving_pci.value()),
                                                                      static_cast<rnti_t>(rnti.value()),
                                                                      target_pcis,
                                                                      timeout,
                                                                      t1_thres_override);
    fmt::print("CHO triggered for UE with pci={} rnti={} to {} target(s) with timeout={}s.\n",
               serving_pci.value(),
               static_cast<rnti_t>(rnti.value()),
               target_pcis.size(),
               std::chrono::duration_cast<std::chrono::seconds>(timeout).count());
  }
};

/// Application command to trigger RRC Release, optionally with NR redirection.
/// Usage: release <serving_pci> <rnti> [<target_arfcn> [scs <15|30|60|120|240>]]
class release_app_command : public app_services::cmdline_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit release_app_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  std::string_view get_name() const override { return "release"; }

  std::string_view get_description() const override
  {
    return " <serving_pci> <rnti> [<target_arfcn> [scs <15|30|60|120|240>]]: release UE to RRC_IDLE, optionally with "
           "NR redirection";
  }

  void execute(span<const std::string> args) override
  {
    if (args.size() < 2) {
      fmt::print(
          "Invalid release command. Usage: release <serving_pci> <rnti> [<target_arfcn> [scs <15|30|60|120|240>]]\n");
      return;
    }

    const auto* arg = args.begin();

    expected<unsigned, std::string> serving_pci = app_services::parse_int<unsigned>(*arg);
    if (not serving_pci.has_value()) {
      fmt::print("Invalid serving PCI.\n");
      return;
    }
    ++arg;

    expected<unsigned, std::string> rnti = app_services::parse_unsigned_hex<unsigned>(*arg);
    if (not rnti.has_value()) {
      fmt::print("Invalid UE RNTI.\n");
      return;
    }
    ++arg;

    std::optional<ocucp::cu_cp_release_redirect_nr_info> redirect_info;

    if (arg != args.end()) {
      expected<unsigned, std::string> arfcn = app_services::parse_int<unsigned>(*arg);
      if (not arfcn.has_value()) {
        fmt::print("Invalid target ARFCN.\n");
        return;
      }
      ++arg;

      subcarrier_spacing ssb_scs = subcarrier_spacing::kHz15;
      if (arg != args.end()) {
        if (*arg == "scs") {
          ++arg;
          if (arg == args.end()) {
            fmt::print("Missing SCS value after 'scs' keyword.\n");
            return;
          }
          expected<unsigned, std::string> scs_khz = app_services::parse_int<unsigned>(*arg);
          if (not scs_khz.has_value()) {
            fmt::print("Invalid SCS value: {}.\n", *arg);
            return;
          }
          ssb_scs = khz_to_scs(scs_khz.value());
          if (ssb_scs == subcarrier_spacing::invalid) {
            fmt::print("Invalid SCS value {}. Supported: 15, 30, 60, 120, 240.\n", scs_khz.value());
            return;
          }
        } else {
          fmt::print(
              "Unknown argument '{}'. Usage: release <serving_pci> <rnti> [<target_arfcn> [scs <15|30|60|120|240>]]\n",
              *arg);
          return;
        }
      }

      redirect_info = ocucp::cu_cp_release_redirect_nr_info{arfcn.value(), ssb_scs};
    }

    cu_cp.get_ue_release_command_handler().trigger_release(
        static_cast<pci_t>(serving_pci.value()), static_cast<rnti_t>(rnti.value()), redirect_info);

    if (redirect_info.has_value()) {
      fmt::print("RRC Release with redirection triggered for UE pci={} rnti={} to NR arfcn={} scs={}kHz.\n",
                 serving_pci.value(),
                 static_cast<rnti_t>(rnti.value()),
                 redirect_info->arfcn,
                 scs_to_khz(redirect_info->ssb_scs));
    } else {
      fmt::print(
          "RRC Release triggered for UE pci={} rnti={}.\n", serving_pci.value(), static_cast<rnti_t>(rnti.value()));
    }
  }
};

} // namespace ocudu
