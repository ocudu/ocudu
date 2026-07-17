// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_vector.h"
#include "ocudu/mac/mac_cell_rach_handler.h"
#include "ocudu/mac/ue_con_res_id.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {

class scheduler_rach_handler;
class rnti_manager;
class mac_rach_handler;
struct sched_cell_configuration_request_message;

/// Handler of RACH indications for a given cell in the MAC.
class mac_cell_rach_handler_impl final : public mac_cell_rach_handler
{
public:
  mac_cell_rach_handler_impl(mac_rach_handler& parent_, const sched_cell_configuration_request_message& sched_cfg_);

  /// Handle detected RACH indication.
  void handle_rach_indication(const mac_rach_indication& rach_ind) override;

  /// Add a new mapping entry between a Contention-free RACH preamble and a given UE.
  [[nodiscard]] bool handle_cfra_allocation(uint8_t preamble_idx, du_ue_index_t ue_idx, rnti_t crnti);

  /// Remove any existing CFRA preamble mapping for a given UE.
  void handle_cfra_deallocation(du_ue_index_t ue_idx);

  /// \brief Resolves the TC-RNTI allocated during 2-step RACH, given the RA-RNTI MsgA PUSCH was
  /// scheduled with and the RAPID of the preamble that originated it. If resolved, also registers the UE ConResId
  /// decoded from the same CCCH SDU.
  /// \return The TC-RNTI, if a matching, non-expired entry exists; std::nullopt otherwise.
  std::optional<rnti_t>
  handle_msga_ccch_sdu(rnti_t ra_rnti, uint8_t rapid, slot_point sl_rx, const ue_con_res_id_t& con_res_id) override;

  /// \brief Resolves and consumes the UE ContResId registered for a TC-RNTI.
  /// The entry is cleared upon a successful resolution, since a successRAR is only ever encoded once per preamble
  /// detection.
  /// \return The Contention Resolution Identity, if a matching entry exists; std::nullopt otherwise.
  std::optional<ue_con_res_id_t> resolve_msga_con_res_id(rnti_t tc_rnti) override;

  /// \brief Resolves the TC-RNTI allocated during 2-step RACH, given the RA-RNTI MsgA PUSCH was scheduled with and
  /// the RAPID of the preamble that originated it.
  /// \return The TC-RNTI, if a matching, non-expired entry exists; std::nullopt otherwise.
  std::optional<rnti_t> resolve_msga_tc_rnti(rnti_t ra_rnti, uint8_t rapid, slot_point sl_rx) const;

  /// \brief Registers the UE ConResId (first 48 bits of the CCCH SDU carried in a MsgA PUSCH),
  /// decoded by the MAC UL PDU executor, so it can later be echoed back by the MAC DL RAR PDU assembler in the
  /// successRAR MAC subPDU acknowledging this preamble.
  void add_msga_con_res_id(rnti_t tc_rnti, const ue_con_res_id_t& con_res_id);

private:
  /// \brief Packs (RA-RNTI, TC-RNTI, expiry slot) into a single 64-bit word, so that a MsgA TC-RNTI mapping entry
  /// can be updated and read with a single atomic store/load, without a mutex, on this latency-critical path.
  ///
  /// Layout: bits [0:16) = RA-RNTI, [16:32) = TC-RNTI, [32:61) = expiry slot_point::count(), [61:64) = expiry
  /// slot_point::numerology(). A slot_point is exactly 32 bits (3-bit numerology + 29-bit count), so it round-trips
  /// losslessly.
  class msga_tc_rnti_entry
  {
  public:
    msga_tc_rnti_entry() = default;
    /// Reconstructs an entry from a word previously produced by \c to_word().
    explicit msga_tc_rnti_entry(uint64_t word) : packed(word) {}
    msga_tc_rnti_entry(rnti_t ra_rnti, rnti_t tc_rnti, slot_point expiry) :
      packed(static_cast<uint64_t>(to_value(ra_rnti)) | (static_cast<uint64_t>(to_value(tc_rnti)) << 16) |
             (static_cast<uint64_t>(expiry.count()) << 32) | (static_cast<uint64_t>(expiry.numerology()) << 61))
    {
    }

    rnti_t     ra_rnti() const { return to_rnti(static_cast<uint16_t>(packed)); }
    rnti_t     tc_rnti() const { return to_rnti(static_cast<uint16_t>(packed >> 16)); }
    slot_point expiry() const
    {
      return {static_cast<uint32_t>(packed >> 61), static_cast<uint32_t>((packed >> 32) & 0x1fffffffU)};
    }
    uint64_t to_word() const { return packed; }

  private:
    uint64_t packed = 0;
  };

  // Map of preamble ID to entry in \c preambles vector.
  unsigned get_cfra_index(unsigned ra_preamble_id) const;

  /// Registers the TC-RNTI allocated to a MsgA preamble, so it can later be resolved from its (RA-RNTI, RAPID).
  void add_msga_tc_rnti(rnti_t ra_rnti, uint8_t rapid, rnti_t tc_rnti, slot_point sl_rx);

  /// \brief Maps a TC-RNTI to its slot in \c msga_con_res_ids. As the table is much smaller than the TC-RNTI value
  /// range, this is a hash (modulo), not a direct index; \c msga_con_res_ids entries are tagged with their owning
  /// TC-RNTI to detect collisions.
  unsigned get_con_res_id_index(rnti_t tc_rnti) const;

  mac_rach_handler&        parent;
  const du_cell_index_t    cell_index;
  const interval<unsigned> cfra_preambles;
  const interval<unsigned> msga_cb_preambles;
  const bool               prach_format_is_long;
  /// Number of slots a MsgA TC-RNTI mapping entry remains valid for, before being treated as expired.
  const unsigned msga_tc_rnti_ttl_slots;

  std::vector<std::atomic<rnti_t>> preambles;

  /// \brief Pending RAPID -> (RA-RNTI, TC-RNTI) mappings for ongoing 2-step RACH attempts, indexed by
  /// rapid - msga_cb_preambles.start().
  ///
  /// Written from the RACH indication executor and read from the UL PDU executor that decodes the MsgA PUSCH CCCH
  /// payload; each slot is a single atomic word, so no mutex is needed to synchronize the two.
  std::vector<std::atomic<uint64_t>> msga_tc_rntis;

  /// \brief Pending TC-RNTI -> UE ConResId mappings, hash-indexed by TC-RNTI value (see get_con_res_id_index()).
  /// Packed as [0:48) for the ConResId bytes and [48:64) for the owning TC-RNTI, used as a collision-detection tag
  /// (a zero tag, i.e. \c INVALID_RNTI, marks an empty slot, since a TC-RNTI is never 0).
  ///
  /// Sized to the number of MsgA CB preambles rather than the full TC-RNTI value range, since at most that many
  /// 2-step RACH attempts can be pending at once (one per RAPID slot in \c msga_tc_rntis); two concurrently pending
  /// attempts that hash to the same slot will evict each other, causing the older one's resolution to miss.
  ///
  /// Written from the UL PDU executor that decodes the MsgA PUSCH CCCH payload and read (and consumed) from the DL
  /// executor that assembles the successRAR MAC subPDU; each slot is a single atomic word, so no mutex is needed to
  /// synchronize the two.
  std::vector<std::atomic<uint64_t>> msga_con_res_ids;
};

/// Handler of RACH indications for multiple cells.
class mac_rach_handler
{
public:
  mac_rach_handler(scheduler_rach_handler& sched_, rnti_manager& rnti_mng_, ocudulog::basic_logger& logger_);

  /// Create new handler of RACH indications for a cell.
  mac_cell_rach_handler_impl& add_cell(const sched_cell_configuration_request_message& sched_cfg);

  void rem_cell(du_cell_index_t cell_index);

  /// Deallocate the CFRA preamble for a UE if one is still allocated.
  void handle_cfra_deallocation(du_ue_index_t ue_idx);

private:
  friend class mac_cell_rach_handler_impl;

  struct cfra_ue_context {
    uint8_t         preamble_id;
    du_cell_index_t cell_index;
  };

  scheduler_rach_handler& sched;
  rnti_manager&           rnti_mng;
  ocudulog::basic_logger& logger;

  slotted_id_vector<du_cell_index_t, std::unique_ptr<mac_cell_rach_handler_impl>> cell_map;
  std::vector<cfra_ue_context>                                                    ue_map;
};

} // namespace ocudu
