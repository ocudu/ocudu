// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/scheduler/support/cg_pusch_dtx.h"
#include "ocudu/support/ocudu_assert.h"
#include <array>
#include <atomic>
#include <optional>

namespace ocudu {

/// Detector of configured grant PUSCH occasions that the UE did not transmit (DTX).
class cg_dtx_detector
{
public:
  explicit cg_dtx_detector(float sinr_threshold_dB_) : sinr_threshold_dB(sinr_threshold_dB_)
  {
    for (auto& cs_rnti : ue_cs_rntis) {
      cs_rnti.store(rnti_t::INVALID_RNTI, std::memory_order_relaxed);
    }
  }

  /// Sets the CS-RNTI of a UE. \c rnti_t::INVALID_RNTI clears it.
  void set_cs_rnti(du_ue_index_t ue_index, rnti_t cs_rnti)
  {
    ocudu_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);
    // The CS-RNTI is written from the control executor and read from the executors that deliver the CRC indications.
    // Relaxed ordering is enough, as the CS-RNTI is stored before the reconfiguration reaches the scheduler, and the
    // scheduler-to-PHY path orders it before the CRC of any configured grant PUSCH.
    ue_cs_rntis[ue_index].store(cs_rnti, std::memory_order_relaxed);
  }

  /// Returns true if the CRC indication is for a configured grant PUSCH that the UE did not transmit.
  bool is_dtx(du_ue_index_t ue_index, rnti_t rnti, bool tb_crc_success, std::optional<float> ul_sinr_dB) const
  {
    ocudu_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);
    // After its configured grant is removed, a UE keeps its old CS-RNTI here until the next configured grant activation
    // or until a new UE is created with the same ue_index. The RNTI table no longer maps that CS-RNTI to the UE, so its
    // CRCs never reach this check with this UE.
    if (rnti == rnti_t::INVALID_RNTI or rnti != ue_cs_rntis[ue_index].load(std::memory_order_relaxed)) {
      return false;
    }
    return is_cg_pusch_dtx(tb_crc_success, ul_sinr_dB, sinr_threshold_dB);
  }

private:
  /// SINR threshold, in dB, below which a CRC KO of a configured grant PUSCH is a DTX.
  const float sinr_threshold_dB;
  /// CS-RNTI of each UE, or \c rnti_t::INVALID_RNTI if the UE has had no configured grant since its creation.
  std::array<std::atomic<rnti_t>, MAX_NOF_DU_UES> ue_cs_rntis;
};

} // namespace ocudu
