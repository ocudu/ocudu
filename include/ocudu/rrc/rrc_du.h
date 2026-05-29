// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/common.h"
#include "ocudu/ran/cu_cp_cell_configuration.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/rrc/rrc_cell_context.h"
#include "ocudu/rrc/rrc_metrics.h"
#include "ocudu/rrc/rrc_ue.h"

namespace ocudu::ocucp {

/// RRC DU cell information extracted from the SIB1 message.
struct rrc_cell_info {
  nr_band                      band;
  pci_t                        nr_pci;
  std::vector<rrc_meas_timing> meas_timings;
  rrc_timers_t                 timers;
  // > PLMN identities broadcasted in SIB1. Max 12 as per TS 38.331 section 6.4.
  std::vector<plmn_identity> plmn_identity_list;
};

class rrc_du_cell_manager
{
public:
  rrc_du_cell_manager()          = default;
  virtual ~rrc_du_cell_manager() = default;

  /// \brief Get information about the cells served by this DU.
  /// \param[in] served_cell_list The list of served cells received from the DU in the F1 Setup Request.
  /// \return A map of cell id to cell information for all served cells or an empty map on error.
  virtual std::map<nr_cell_global_id_t, rrc_cell_info>
  get_cell_info(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) const = 0;

  virtual std::vector<rrc_plmn_ran_area_cell_t> get_ran_area_cells() = 0;

  virtual void store_cell_info_db(const std::map<nr_cell_global_id_t, rrc_cell_info>& cell_infos) = 0;
};

struct rrc_resume_context_t {
  bool                                                       is_resume;
  std::optional<std::variant<short_i_rnti_t, full_i_rnti_t>> rrc_resume_id = std::nullopt;
  std::optional<resume_cause_t>                              resume_cause  = std::nullopt;
};

struct rrc_ue_creation_message {
  cu_cp_ue_index_t                       ue_index;
  rnti_t                                 c_rnti;
  rrc_cell_context                       cell;
  rrc_pdu_f1ap_notifier*                 f1ap_pdu_notifier;
  rrc_ue_ngap_notifier*                  ngap_notifier;
  rrc_ue_context_update_notifier*        rrc_ue_cu_cp_notifier;
  rrc_ue_measurement_notifier*           measurement_notifier;
  rrc_ue_cu_cp_ue_notifier*              cu_cp_ue_notifier;
  byte_buffer                            du_to_cu_container;
  std::optional<rrc_ue_transfer_context> rrc_context;
};

/// \brief Interface class to the main RRC DU object to manage RRC UEs.
/// This interface provides functions to add, remove and release UEs.
class rrc_du_ue_repository
{
public:
  rrc_du_ue_repository()          = default;
  virtual ~rrc_du_ue_repository() = default;

  /// \brief Get the RRC Reject message to send to the UE.
  virtual byte_buffer get_rrc_reject() = 0;

  /// \brief Pack the RRC MeasConfig into ASN.1 PDU forwarded to the DU via F1AP CUtoDURRCInformation.
  ///
  /// Used on the target node of an inter-CU handover, where the MeasConfig must be delivered before the RRC UE
  /// context exists. Returns an empty buffer on packing failure.
  virtual byte_buffer pack_meas_config(const rrc_meas_cfg& meas_cfg) = 0;

  /// \brief Get the RRC Resume context containing the resume ID and resume cause from a RRC container.
  /// \param[in] rrc_container The RRC container from the DU.
  /// \param[in] nof_i_rnti_ue_bits Number of bits used for the I-RNTI UE.
  /// \returns The RRC Resume context if the container contains a valid UL CCCH Message, std::nullopt if an error
  /// occured e.g. during unpacking.
  virtual std::optional<rrc_resume_context_t> get_rrc_resume_context(byte_buffer rrc_container,
                                                                     uint8_t     nof_i_rnti_ue_bits) = 0;

  /// Creates a new RRC UE object and returns a handle to it.
  virtual rrc_ue_interface* add_ue(const rrc_ue_creation_message& msg) = 0;

  /// Send RRC Release to all UEs connected to this DU.
  virtual void release_ues() = 0;

  /// \brief Get the number of UEs registered at the RRC DU.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Handle RRC UE removal
class rrc_ue_handler
{
public:
  virtual ~rrc_ue_handler() = default;

  virtual rrc_ue_interface* find_ue(cu_cp_ue_index_t ue_index) = 0;

  /// Remove a RRC UE object.
  /// \param[in] ue_index The index of the UE object to remove.
  virtual void remove_ue(cu_cp_ue_index_t ue_index) = 0;
};

class rrc_du_connection_event_handler
{
public:
  virtual ~rrc_du_connection_event_handler() = default;

  /// \brief Add the successful RRC setup to the metrics.
  /// \param[in] cause The establishment cause of the RRC connection. If this is given the connection establishment
  /// metrics are increased. Otherwise the connection metrics are increased.
  virtual void handle_successful_rrc_setup(std::optional<establishment_cause_t> cause = std::nullopt) = 0;

  /// \brief Add the successful RRC release to the metrics.
  /// \param[in] is_inactive True if the released RRC connection was in inactive state, false otherwise.
  virtual void handle_successful_rrc_release(bool is_inactive = false) = 0;

  /// \brief Add the RRC inactive transition to the metrics.
  virtual void handle_rrc_inactive() = 0;

  /// \brief Add the attempted RRC connection establishment to the metrics.
  /// \param[in] cause The establishment cause of the RRC connection.
  virtual void handle_attempted_rrc_setup(establishment_cause_t cause) = 0;

  /// \brief Add the attempted RRC connection re-establishment to the metrics.
  virtual void handle_attempted_rrc_reestablishment() = 0;

  /// \brief Add the failed RRC connection establishment to the metrics.
  virtual void handle_failed_rrc_connection_establishment(establishment_fail_cause_t cause) = 0;

  /// \brief Add the successful RRC connection re-establishment to the metrics.
  virtual void handle_successful_rrc_reestablishment() = 0;

  /// \brief Add the successful RRC connection re-establishment fallback to the metrics.
  virtual void handle_successful_rrc_reestablishment_fallback() = 0;

  /// \brief Add the attempted RRC connection resume to the metrics.
  virtual void handle_attempted_rrc_resume(resume_cause_t cause) = 0;

  /// \brief Add the RRC resume to the metrics.
  /// \param[in] cause The resume cause of the RRC connection resume.
  virtual void handle_successful_rrc_resume(resume_cause_t cause) = 0;

  /// \brief Add the successful RRC connection resume with fallback to the metrics.
  virtual void handle_successful_rrc_resume_with_fallback(resume_cause_t cause) = 0;

  /// \brief Add the RRC connection resume followed by network release to the metrics.
  virtual void handle_rrc_resume_followed_by_network_release(resume_cause_t cause) = 0;

  /// \brief Add the attempted RRC connection resume followed by RRC setup to the metrics.
  virtual void handle_attempted_rrc_resume_followed_by_rrc_setup(resume_cause_t cause) = 0;
};

class rrc_du_metrics_collector
{
public:
  virtual ~rrc_du_metrics_collector() = default;

  /// \brief Collect the metrics of this RRC DU.
  /// \param[out] metrics The metrics to collect.
  virtual void collect_metrics(rrc_du_metrics& metrics) = 0;
};

/// Combined entry point for the RRC DU handling.
class rrc_du : public rrc_du_cell_manager,
               public rrc_du_ue_repository,
               public rrc_ue_handler,
               public rrc_du_connection_event_handler
{
public:
  virtual ~rrc_du() = default;

  virtual rrc_du_cell_manager&             get_rrc_du_cell_manager()             = 0;
  virtual rrc_du_ue_repository&            get_rrc_du_ue_repository()            = 0;
  virtual rrc_ue_handler&                  get_rrc_ue_handler()                  = 0;
  virtual rrc_du_connection_event_handler& get_rrc_du_connection_event_handler() = 0;
  virtual rrc_du_metrics_collector&        get_rrc_du_metrics_collector()        = 0;
};

} // namespace ocudu::ocucp
