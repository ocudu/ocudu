// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/resource_grid.h"
#include "../config/ue_configuration.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include <cstdint>
#include <optional>

namespace ocudu {

class ue_cell;

/// Contains the results of the UCI allocation.
struct uci_allocation {
  /// Delay in slots of the UE's PUCCH HARQ-ACK report with respect to the PDSCH.
  unsigned k1;
  /// \brief Delay in slots of the last transmission carrying the UE's HARQ-ACK report with respect to the PDSCH.
  ///
  /// It is equal to \c k1, except when the report is carried by a multi-slot PUCCH repetition burst (TS 38.213,
  /// Section 9.2.6), in which case it points at the slot of the burst's last repetition.
  unsigned k1_last_rep{0};
  /// Index of the HARQ-bit in the PUCCH/PUSCH HARQ report.
  uint8_t harq_bit_idx{0};
  /// If UCI is allocated on the PUCCH, contains the PUCCH resource indicator; else, this is to be ignored.
  std::optional<unsigned> pucch_res_indicator;
};

/// \brief UCI allocator interface.
/// Depending on whether there is an existing PUSCH grant, it allocates the UCI either on the PUSCH or PUCCH.
class uci_allocator
{
public:
  virtual ~uci_allocator() = default;

  /// \brief Reset the internal counter of the allocated PDSCHs to be acknowledged per slot.
  virtual void slot_indication(slot_point sl_tx) = 0;

  /// Allocate a dedicated PUCCH resource for HARQ-ACK for a given UE.
  /// \param[out,in] res_alloc struct with scheduling results.
  /// \param[in] ue_cc UE cell for which the UCI is being allocated.
  /// \param[in] k0 k0 value, or delay (in slots) of PDSCH slot vs the corresponding PDCCH slot.
  /// \param[in] k1_list List of k1 candidates configured for UE.
  /// \param[in] max_rep_factor Maximum PUCCH repetition factor to use for this grant. Defaults to n1 (no repetition).
  /// \return Returns the UCI allocation result if successful, std::nullopt otherwise.
  virtual std::optional<uci_allocation>
  alloc_harq_ack(cell_resource_allocator& res_alloc,
                 const ue_cell&           ue_cc,
                 unsigned                 k0,
                 span<const uint8_t>      k1_list,
                 pucch_repetition_factor  max_rep_factor = pucch_repetition_factor::n1) = 0;

  /// Allocates the SR opportunities for a given UE.
  /// \param[out,in] slot_alloc struct with scheduling results.
  /// \param[in] ue_cell_cfg user configuration.
  /// \return Returns true if the SR allocation is successful, false otherwise.
  virtual bool alloc_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                                    const ue_cell_configuration&  ue_cell_cfg) = 0;

  /// Allocates the CSI opportunities for a given UE.
  /// \param[out,in] slot_alloc struct with scheduling results.
  /// \param[in] ue_cell_cfg user configuration.
  /// \return Returns true if the CSI allocation is successful, false otherwise.
  virtual bool alloc_csi_opportunity(cell_slot_resource_allocator& slot_alloc,
                                     const ue_cell_configuration&  ue_cell_cfg) = 0;

  /// Multiplexes the UCI on PUSCH, by removing the UCI on the PUCCH (if present) and adding it to the PUSCH.
  /// \param[out,in] pusch_grant struct with PUSCH PDU where UCI need to be allocated.
  /// \param[out,in] slot_alloc struct with scheduling results.
  /// \param[in] ue_cell_cfg user configuration.
  /// \param[in] include_aperiodic_csi Whether to include an aperiodic CSI report in the PUSCH.
  /// \param[in] configured_grant Whether multiplexing is for a Configured Grants PUSCH.
  virtual void multiplex_uci_on_pusch(ul_sched_info&                pusch_grant,
                                      cell_slot_resource_allocator& slot_alloc,
                                      const ue_cell_configuration&  ue_cell_cfg,
                                      bool                          include_aperiodic_csi,
                                      bool                          configured_grant = false) = 0;

  /// Get the number of PDSCHs currently scheduled for a given UE UCI.
  /// \param[in] uci_slot UCI slot of the respective PDSCHs.
  /// \param[in] crnti C-RNTI of the UE.
  /// \return Returns number of PDSCHs scheduled if UCI allocation if found, 0 otherwise.
  virtual uint8_t get_scheduled_pdsch_counter_in_ue_uci(slot_point uci_slot, rnti_t crnti) = 0;

  /// Returns whether a UCI HARQ-ACK allocated on common PUCCH resource exists at a given slot or not.
  /// \param[in] crnti C-RNTI of the UE.
  /// \param[in] sl_tx UCI slot.
  /// \return Returns true if a UCI HARQ-ACK allocated on common PUCCH resource exists. False, otherwise.
  virtual bool has_harq_ack_on_common_pucch_res(rnti_t crnti, slot_point sl_tx) = 0;

  /// \brief Returns whether a given slot is part of an in-flight multi-slot PUCCH repetition burst of a UE.
  ///
  /// As per TS 38.213, Section 9.2.6, the UCI of a PUCCH with repetitions is never multiplexed on an overlapping PUSCH;
  /// the UE transmits the PUCCH and drops the PUSCH in the overlapping slots instead. No PUSCH must therefore be
  /// scheduled for this UE in such a slot.
  ///
  /// \param[in] crnti C-RNTI of the UE.
  /// \param[in] sl_tx Slot to search PUCCH grants.
  virtual bool has_pucch_repetition(rnti_t crnti, slot_point sl_tx) = 0;
};

} // namespace ocudu
