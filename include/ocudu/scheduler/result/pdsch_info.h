// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/logical_channel/lcid_dl_sch.h"
#include "ocudu/ran/pdcch/dci_format.h"
#include "ocudu/ran/pdsch/pdsch_mcs.h"
#include "ocudu/ran/precoding/precoding_constants.h"
#include "ocudu/ran/resource_allocation/vrb_to_prb.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/ran/time_alignment_config.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/result/dmrs_info.h"
#include "ocudu/scheduler/result/vrb_alloc.h"
#include "ocudu/support/units.h"
#include <optional>

namespace ocudu {

/// The precoding information associated with PDSCH PDUs.
struct pdsch_precoding_info {
  /// Precoding Resource Block Group (PRG) information.
  using prg_info = precoding_matrix_indicator;

  /// \brief Size in RBs of a precoding resource block group (PRG) to which same precoding and digital beamforming gets
  /// applied. Values: {1,...,275}.
  unsigned nof_rbs_per_prg;
  /// PRG list.
  static_vector<prg_info, precoding_constants::MAX_NOF_PRG> prg_infos;
};

/// PDSCH codeword.
struct pdsch_codeword {
  /// Modulation and coding scheme.
  sch_mcs_description mcs_descr;
  /// \brief MCS index, range {0, ..., 31} (See TS 38.214 Section 5.1.3.1).
  /// \note Should match value sent in DCI.
  sch_mcs_index mcs_index;
  /// Redundancy version index (see TS 38.212 Table 5.4.2.1-2, and TS 38.214 Table 5.1.2.1-2).
  uint8_t rv_index;
  /// Transport block size, in bytes (see TS 38.214 Section 5.1.3.2).
  units::bytes tb_size_bytes;
  /// Whether this is the first Tx or retx of the HARQ codeword.
  bool new_data;
};

/// Transmit power information associated with PDSCH PDU.
struct tx_power_pdsch_information {
  /// Ratio of PDSCH EPRE to NZP CSI-RS EPRE when UE derives CSI feedback. See 3GPP TS 38.214, clause 5.2.2.3.1. Values
  /// {-8,...,15} dB with 1 dB step size.
  /// \remark [Implementation-defined] In case UE is not configured with powerControlOffset we assume it to be 0dB.
  int8_t pwr_ctrl_offset{0};
  /// Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE. See 3GPP TS 38.214, clause 5.2.2.3.1. Values {-3, 0, 3, 6} dB.
  /// \remark [Implementation-defined] In case UE is not configured with powerControlOffsetSS we assume it to be 0dB.
  int8_t pwr_ctrl_offset_ss{0};
};

/// \brief Information relative to a PDSCH grant in a given slot.
struct pdsch_information {
  rnti_t                                                 rnti;
  const bwp_configuration*                               bwp_cfg;
  const coreset_configuration*                           coreset_cfg;
  vrb_alloc                                              rbs;
  ofdm_symbol_range                                      symbols;
  pdsch_mcs_table                                        mcs_table;
  static_vector<pdsch_codeword, MAX_CODEWORDS_PER_PDSCH> codewords;
  dmrs_information                                       dmrs;
  /// Parameter n_ID, used for scrambling, as per TS 38.211, Section 7.3.1.1.
  unsigned n_id;
  /// Number of layers as per TS 38.211, Section 7.3.1.3. Values: {1,...,8}.
  unsigned nof_layers;
  /// Whether the PDSCH is interleaved via VRB-to-PRB mapping.
  vrb_to_prb::mapping_type vrb_prb_mapping;
  search_space_set_type    ss_set_type;
  dci_dl_format            dci_fmt;
  /// HARQ process number as per TS 38.212 Section 7.3.1.1. Values: {0,...,15}.
  harq_id_t harq_id;
  /// Precoding information for the PDSCH. This field is empty in case of 1-antenna port setups.
  std::optional<pdsch_precoding_info> precoding;
  /// Transmit power information for the PDSCH.
  tx_power_pdsch_information tx_pwr_info;
};

/// Dummy MAC CE payload.
/// To be replaced by other MAC CE payload when its supported.
using dummy_ce_payload = unsigned;

/// Timing Advance Command CE payload.
struct ta_cmd_ce_payload {
  time_alignment_group::id_t tag_id;
  unsigned                   ta_cmd;
};

struct dl_msg_lc_info {
  /// Values of LCID for DL-SCH. See TS 38.321, Table 6.2.1-1.
  lcid_dl_sch_t lcid;
  /// Number of bytes available to fill in this SDU for the specified logical channel. {0..65535}.
  unsigned sched_bytes;
  /// Holds payload of CE except UE Contention Resolution Identity.
  std::variant<ta_cmd_ce_payload, dummy_ce_payload> ce_payload;
};

/// Information relative to the transport blocks scheduled for a given PDSCH.
struct dl_msg_tb_info {
  /// List of logical channels to schedule in this TB.
  static_vector<dl_msg_lc_info, MAX_LC_PER_TB> lc_chs_to_sched;
};

using dl_msg_tb_info_list = static_vector<dl_msg_tb_info, MAX_CODEWORDS_PER_PDSCH>;

/// Dedicated DL Grant for UEs.
struct dl_msg_alloc {
  pdsch_information   pdsch_cfg;
  dl_msg_tb_info_list tb_list;

  /// \brief Information relative to a PDSCH allocation decision that is used for the purpose of logging or
  /// tracing, but not passed to the PHY.
  struct decision_context {
    /// UE index of allocated UE.
    du_ue_index_t ue_index;
    /// Chosen k1 delay to receive UCI HARQ-ACK.
    unsigned k1;
    /// Chosen search space id
    search_space_id ss_id;
    /// Number of times the HARQ process has been retransmitted.
    unsigned nof_retxs;
    /// Current UE DL buffer occupancy, after this PDSCH grant.
    unsigned buffer_occupancy;
    /// Offset that the OLLA algorithm applied to the DL MCS candidate to account for channel impairments.
    std::optional<float> olla_offset;
  } context;
};

/// \brief RAR grant composed of subheader as per TS 38.321 6.2.2, payload as per TS 38.321 6.2.3,
/// with UL grant as per TS 38.213, Table 8.2-1.
struct rar_ul_grant {
  // MAC subheader.
  uint16_t rapid;

  // RAR payload.
  uint16_t ta;
  rnti_t   temp_crnti;

  // UL Grant Payload.
  bool          freq_hop_flag;
  uint8_t       time_resource_assignment;
  uint16_t      freq_resource_assignment;
  sch_mcs_index mcs;
  int8_t        tpc;
  bool          csi_req;

  /// Grant type discriminator.
  /// four_step_info — standard 4-step RAR UL grant (MAC RAR subPDU).
  /// two_step_success_info — 2-step MsgB successRAR (LCID=1); MsgA PUSCH was successfully decoded.
  /// two_step_fallback_info — 2-step MsgB fallbackRAR (LCID=0); UE falls back to 4-step RAR (Msg3).
  struct four_step_info {};
  struct two_step_success_info {
    /// HARQ Feedback Timing Indicator for the MsgB HARQ-ACK, as per TS 38.321, 6.2.3a.
    uint8_t harq_feedback_timing_indicator = 0;
    /// PUCCH Resource Indicator for the MsgB HARQ-ACK, as per TS 38.321, 6.2.3a.
    uint8_t pucch_resource_indicator = 0;
  };
  struct two_step_fallback_info {};
  std::variant<four_step_info, two_step_success_info, two_step_fallback_info> type = four_step_info{};
};

/// Stores the information associated with a RAR.
struct rar_information {
  pdsch_information                                  pdsch_cfg;
  static_vector<rar_ul_grant, MAX_RAR_PDUS_PER_SLOT> grants;
  /// \brief Backoff Indicator (BI) value included as the first MAC subPDU of the RAR, as per TS 38.321, Section
  /// 6.2.2 and Table 7.2-1. Unset if no Backoff Indicator subPDU is included.
  std::optional<uint8_t> backoff_indicator;
};

/// Stores the information associated with an SSB.
struct ssb_information {
  unsigned          ssb_index;
  crb_interval      crbs;
  ofdm_symbol_range symbols;
};

/// Stores the information associated with an SIB1 or other SI allocation.
struct sib_information {
  enum si_indicator_type { sib1, other_si } si_indicator;
  std::optional<uint8_t> si_msg_index;
  /// \brief Version of the SIB1/SI message payload. This counter should be incremented every time the content of the
  /// respective SIB1/SI message changes.
  unsigned version;
  /// \brief Number of times the SIB has been transmitted.
  ///
  /// If the SI message is segmented, all transmission events are counted, regardless of the transmitted segment.
  unsigned long nof_txs;
  /// Set to \c true if the SIB allocation is for an SIB retransmission, \c false otherwise.
  bool              is_repetition;
  pdsch_information pdsch_cfg;
};

/// See ORAN WG8, 9.2.3.3.12 - Downlink Broadcast Allocation.
struct dl_broadcast_allocation {
  static_vector<ssb_information, MAX_SSB_PER_SLOT>     ssb_info;
  static_vector<sib_information, MAX_SI_PDUS_PER_SLOT> sibs;
};

struct paging_ue_info {
  /// Type of Paging. RAN initiated or CN initiated.
  enum paging_identity_type { ran_ue_paging_identity, cn_ue_paging_identity } paging_type_indicator;
  /// Paging identity assigned to UE. Possible values are \c I-RNTI-Value (Bit string of size 40) and \c NG-5G-S-TMSI
  /// (Bit string of size 48). See TS 38.331.
  uint64_t paging_identity;
};

/// Stores the information associated with Paging allocation.
struct dl_paging_allocation {
  static_vector<paging_ue_info, MAX_PAGING_RECORDS_PER_PAGING_PDU> paging_ue_list;
  pdsch_information                                                pdsch_cfg;
};

} // namespace ocudu
