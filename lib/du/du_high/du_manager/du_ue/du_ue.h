// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ran_resource_management/du_ran_resource_manager.h"
#include "../ran_resource_management/du_ue_resource_config.h"
#include "du_ue_bearer_manager.h"
#include "du_ue_cond_mobility_manager.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {

class mac_ue_radio_link_notifier;

namespace odu {

/// \brief This class holds the context of an UE in the DU.
struct du_ue_context {
  du_ue_context(du_ue_index_t ue_index_, du_cell_index_t pcell_index_, rnti_t rnti_, nr_cell_global_id_t nr_cgi_) :
    ue_index(ue_index_),
    rnti(rnti_),
    f1ap_ue_id(int_to_gnb_du_ue_f1ap_id(ue_index_)),
    pcell_index(pcell_index_),
    nr_cgi(nr_cgi_)
  {
  }

  const du_ue_index_t ue_index;
  rnti_t              rnti = rnti_t::INVALID_RNTI;
  gnb_du_ue_f1ap_id_t f1ap_ue_id;
  du_cell_index_t     pcell_index;
  /// \brief The NR Cell Global Identity of the UE's serving cell.
  nr_cell_global_id_t nr_cgi;
};

/// \brief Handled causes for RLF.
enum class rlf_cause {
  max_harq_nacks_reached,
  max_crc_kos_reached,
  max_csi_dtx_reached,
  max_rlc_retxs_reached,
  rlc_protocol_failure
};

/// \brief Whether the RLF was detected in the MAC (as opposed to the RLC).
inline bool is_mac_rlf_cause(rlf_cause cause)
{
  switch (cause) {
    case rlf_cause::max_harq_nacks_reached:
    case rlf_cause::max_crc_kos_reached:
    case rlf_cause::max_csi_dtx_reached:
      return true;
    case rlf_cause::max_rlc_retxs_reached:
    case rlf_cause::rlc_protocol_failure:
      return false;
  }
  return false;
}

/// The interface exposes the methods to interact with the state of a DU UE.
class du_ue_controller
{
public:
  virtual ~du_ue_controller() = default;

  /// \brief Disable the handling of RLF detections.
  /// This method should be called, for instance, when the UE context is in the process of being removed from the DU or
  /// the UE is performing handover.
  virtual void disable_rlf_detection() = 0;

  /// \brief Stop traffic for the radio bearers of this UE and detection of RLFs.
  /// \param[in] stop_srbs Whether to stop both SRB and DRB traffic or just DRB traffic. This flag should be only set
  /// to true during UE context removal, to ensure traffic is not flowing through its SRBs during the layer-by-layer
  /// UE context removal. In other situations, we still need SRBs for signalling RRC messages.
  virtual async_task<void> handle_activity_stop_request(bool stop_srbs) = 0;

  /// \brief Stop traffic for a subset of DRBs of a UE.
  ///
  /// This method can be called during a UE reconfiguration, when some DRBs are removed.
  virtual async_task<void> handle_drb_stop_request(span<const drb_id_t> drbs_to_stop) = 0;

  /// \brief Schedule task for a given UE.
  virtual void schedule_async_task(async_task<void> task) = 0;

  /// \brief Handle detection of RLF in MAC or RLC.
  virtual void handle_rlf_detection(rlf_cause cause) = 0;

  /// \brief Handle the detection of a C-RNTI MAC CE for this UE.
  virtual void handle_crnti_ce_detection() = 0;

  /// \brief Access to the MAC RLF notifier for this UE.
  virtual mac_ue_radio_link_notifier& get_mac_rlf_notifier() = 0;

  /// \brief Access to the RLC RLF notifier for this UE.
  virtual rlc_tx_upper_layer_control_notifier& get_rlc_rlf_notifier() = 0;
};

class du_ue : public du_ue_context, public du_ue_controller
{
public:
  explicit du_ue(const du_ue_context& ctx_, ue_ran_resource_configurator resources_) :
    du_ue_context(ctx_), resources(std::move(resources_))
  {
  }

  /// \brief List of bearers of the UE.
  du_ue_bearer_manager bearers;

  /// \brief Radio access network resources currently allocated to the UE.
  ue_ran_resource_configurator resources;

  /// \brief Determines whether this UE is running the RRC Reestablishment procedure and which context was retrieved
  /// from the old UE.
  std::unique_ptr<du_ue_resource_config> reestablished_cfg_pending;
  std::unique_ptr<ue_capability_summary> reestablished_ue_caps_summary;

  /// \brief Conditional mobility state for this UE.
  du_ue_cond_mobility_manager cond_mobility;
};

} // namespace odu
} // namespace ocudu
