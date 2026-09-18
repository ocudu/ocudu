// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/p7_indications_notifier.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/upper/upper_phy_rx_results_notifier.h"

namespace ocudu {
namespace fapi_adaptor {

/// PHY to FAPI results event fastpath translator configuration.
struct phy_to_fapi_results_event_fastpath_translator_config {
  unsigned sector_id;
  float    dbfs_to_dbm_conversion_factor;
  float    db_to_dbfs_conversion_factor;
  /// Random access subcarrier spacing, parameter \f$\Delta f^{RA}\f$, from \c msg1-SubcarrierSpacing.
  subcarrier_spacing msg1_scs;
  /// NTN k_mac in slots at the cell SCS; zero for terrestrial cells.
  unsigned ntn_k_mac_slots = 0;
};

/// PHY to FAPI results event fastpath translator dependencies.
struct phy_to_fapi_results_event_fastpath_translator_dependencies {
  ocudulog::basic_logger& logger;
};

/// \brief PHY-to-FAPI uplink results events fastpath translator.
///
/// This class listens to upper PHY uplink result events and translates them into FAPI indication messages that are sent
/// through the FAPI data-specific message notifier.
class phy_to_fapi_results_event_fastpath_translator : public upper_phy_rx_results_notifier
{
public:
  phy_to_fapi_results_event_fastpath_translator(
      const phy_to_fapi_results_event_fastpath_translator_config&       cfg,
      const phy_to_fapi_results_event_fastpath_translator_dependencies& dependencies);

  // See interface for documentation.
  void on_new_prach_results(const ul_prach_results& result) override;

  // See interface for documentation.
  void on_new_pusch_results_control(const ul_pusch_results_control& result) override;

  // See interface for documentation.
  void on_new_pusch_results_data(const ul_pusch_results_data& result) override;

  // See interface for documentation.
  void on_new_pucch_results(const ul_pucch_results& result) override;

  // See interface for documentation.
  void on_new_srs_results(const ul_srs_results& result) override;

  /// Configures the FAPI P7 indications notifier to the given one.
  void set_p7_indications_notifier(fapi::p7_indications_notifier& fapi_p7_notifier) { p7_notifier = &fapi_p7_notifier; }

private:
  /// Notifies a new FAPI \e CRC.indication through the data notifier.
  void notify_crc_indication(const ul_pusch_results_data& result);

  /// Notifies a new FAPI \e Rx_Data.indication through the data notifier.
  void notify_rx_data_indication(const ul_pusch_results_data& result);

  ///  Notifies a new FAPI \e UCI.indication through the data notifier that carries a PUSCH PDU.
  void notify_pusch_uci_indication(const ul_pusch_results_control& result);

private:
  /// Radio sector identifier.
  const unsigned sector_id;
  /// Value in dBm at the antenna connector equivalent to 0 dBFS for a receive gain of 0 dB.
  float dbfs_to_dbm_conversion_factor;
  /// Value in dB relative to Full Scale (dBFS) equivalent to 0 dB in normalized units, i.e., as coming from the
  /// physical layer.
  float db_to_dbfs_conversion_factor;
  /// Random access subcarrier spacing, used to report the PRACH occasion slot index.
  const subcarrier_spacing msg1_scs;
  /// \brief NTN k_mac in slots at the cell SCS; zero for terrestrial cells.
  ///
  /// The PRACH detection PDU is scheduled at the gNB DL-clock slot (UL occasion + k_mac), so the detection slot must
  /// be rebased to the UL occasion before deriving the occasion slot index, whose t_id lives in the UE UL frame.
  const unsigned ntn_k_mac_slots;
  /// FAPI logger.
  ocudulog::basic_logger& logger;
  /// FAPI P7 indications notifier.
  fapi::p7_indications_notifier* p7_notifier;
};

} // namespace fapi_adaptor
} // namespace ocudu
