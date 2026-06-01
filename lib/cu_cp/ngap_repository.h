// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "adapters/ngap_adapters.h"
#include "task_schedulers/ngap_task_scheduler.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ran/plmn_identity.h"

namespace ocudu::ocucp {

/// Interface used to capture the NGAP metrics from all the connected AMFs to the CU-CP.
class ngap_repository_metrics_handler
{
public:
  virtual ~ngap_repository_metrics_handler() = default;

  /// \brief Handle new metrics request for all the AMF nodes connected to the CU-CP.
  virtual std::vector<ngap_info> handle_ngap_metrics_report_request() const = 0;
};

struct cu_cp_configuration;

struct ngap_repository_config {
  const cu_cp_configuration& cu_cp;
  cu_cp_ngap_handler&        cu_cp_notifier;
  paging_message_handler&    paging_handler;
  ocudulog::basic_logger&    logger;
};

class ngap_repository : public ngap_repository_metrics_handler
{
public:
  explicit ngap_repository(ngap_repository_config cfg_);

  /// \brief Adds a NGAP object to the CU-CP.
  /// \return A pointer to the interface of the added NGAP object if it was successfully created, a nullptr otherwise.
  ngap_interface* add_ngap(cu_cp_amf_index_t amf_index, const cu_cp_configuration::ngap_config& config);

  /// \brief Updates the PLMN lookup table with the PLMNs supported by the connected NGAP.
  /// \param[in] amf_index The AMF index to identify the NGAP.
  void update_plmn_lookup(cu_cp_amf_index_t amf_index);

  /// \brief Checks whether a AMF with the specified PLMN is served by any of the connected NGAPs.
  /// \param[in] plmn The PLMN to identify the NGAP.
  /// \return The interface of the NGAP for the given PLMN if it is found, nullptr if no NGAP for the PLMN is found.
  ngap_interface* find_ngap(const plmn_identity& plmn);

  /// \brief Checks whether a AMF with the specified AMF index is in the connected NGAPs.
  /// \param[in] amf_index The AMF index to identify the NGAP.
  /// \return The interface of the NGAP for the given AMF index if it is found, nullptr not.
  ngap_interface* find_ngap(const cu_cp_amf_index_t& amf_index);

  /// \brief Get the all NGAP interfaces.
  std::map<cu_cp_amf_index_t, ngap_interface*> get_ngaps();

  ngap_task_scheduler& get_ngap_task_scheduler() { return amf_task_sched; }

  /// Number of NGAPs managed by the CU-CP.
  size_t get_nof_ngaps() const { return ngap_db.size(); }

  std::vector<ngap_info> handle_ngap_metrics_report_request() const override;

  /// Number of UEs managed by the CU-CP.
  size_t get_nof_ngap_ues();

  /// \brief Get all supported tracking areas across all connected NGAPs.
  std::vector<supported_tracking_area> get_supported_tracking_areas() const;

  /// \brief Get all served GUAMIs across all connected NGAPs.
  std::vector<guami_t> get_served_guamis() const;

private:
  struct ngap_context {
    // CU-CP handler of NGAP events.
    ngap_cu_cp_adapter ngap_to_cu_cp_notifier;

    std::unique_ptr<ngap_interface> ngap;

    /// Notifier used by the CU-CP to push NGAP Tx messages to the respective AMF.
    std::unique_ptr<ngap_message_notifier> ngap_tx_pdu_notifier;
  };

  ngap_repository_config  cfg;
  ocudulog::basic_logger& logger;

  ngap_task_scheduler amf_task_sched;

  std::unordered_map<plmn_identity, cu_cp_amf_index_t> plmn_to_amf_index;
  std::map<cu_cp_amf_index_t, ngap_context>            ngap_db;
};

} // namespace ocudu::ocucp
