// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/tac.h"
#include <cstdint>

namespace ocudu {

namespace ocucp {

/// \brief RRC timers, see TS 38.331 section 7.1.1.
struct rrc_timers_t {
  /// T300: RRC Connection Establishment timer in ms. The timer starts upon transmission of RRCSetupRequest.
  std::chrono::milliseconds t300;
  /// T301: RRC Connection Re-establishment timer in ms. The timer starts upon transmission of
  /// RRCReestablishmentRequest.
  std::chrono::milliseconds t301;
  /// T310: Out-of-sync timer in ms. The timer starts upon detecting physical layer problems for the SpCell i.e. upon
  /// receiving N310 consecutive out-of-sync indications from lower layers.
  std::chrono::milliseconds t310;
  /// T311: RRC Connection Re-establishment procedure timer in ms. The timer starts upon initiating the RRC connection
  /// re-establishment procedure.
  std::chrono::milliseconds t311;
  /// T319: RRC Connection Resume timer in ms. The timer starts upon transmission of RRCResumeRequest.
  std::chrono::milliseconds t319;
  /// T304: RRC Reconfiguration timer in ms. The UE starts this timer upon receiving the RRC Reconfiguration including
  /// reconfigurationWithSync.
  std::optional<std::chrono::milliseconds> t304;
};

// Cell-related configuration used by the RRC UE.
struct rrc_cell_context {
  nr_cell_global_id_t        cgi;
  tac_t                      tac;
  tac_list_t                 tac_list; ///< TACs broadcast in trackingAreaList, TS 38.331. Empty unless several.
  pci_t                      pci;
  arfcn_t                    ssb_arfcn; ///< Absolute SSB position.
  std::vector<nr_band>       bands;     ///< Required for capability band filter.
  rrc_timers_t               timers;
  std::vector<plmn_identity> plmn_identity_list; ///< PLMN identities broadcasted in SIB1.
};

} // namespace ocucp

} // namespace ocudu
