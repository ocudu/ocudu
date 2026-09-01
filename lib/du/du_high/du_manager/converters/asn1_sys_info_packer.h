// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/mac/cell_configuration.h"
#include <string>
#include <vector>

namespace ocudu {

struct sib19_info;

namespace odu {

/// \brief Set of SI messages that a packed BCCH-DL-SCH payload accounts for.
///
/// A cell lists an SI message carrying SIB6/7/8 in its SIB1 schedulingInfoList only while the warning is on air, which
/// the MAC adds and removes. Hence, the two sets differ in whether they hold those SI messages.
enum class si_message_set {
  /// SI messages of the normal operation, which a cell starts out broadcasting. The ones that only carry a warning are
  /// left out.
  normal_operation,
  /// Every SI message the cell can broadcast, the ones that only carry a warning included.
  every_si_message
};

namespace asn1_packer {

/// \brief Derive and pack cell MIB based on DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return byte buffer with packed cell MIB.
byte_buffer pack_mib(const du_cell_config& du_cfg);

/// \brief Derive and pack cell SIB1 based on DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \param[out] json_string String where the RRC ASN.1 SIB1 is stored in json format. If nullptr, no conversion takes
/// place.
/// \return byte buffer with packed cell SIB1.
byte_buffer pack_sib1(const du_cell_config& du_cfg, si_message_set msg_set, std::string* js_str = nullptr);

/// \brief Derive and pack cell SIB19.
/// \param[in] sib19_params content of SIB19 msg.
///\param[out] json_string String where the RRC ASN.1 SIB1 is stored in json format. If nullptr, no conversion takes
/// place.
/// \return byte buffer with packed cell SIB19.
byte_buffer pack_sib19(const sib19_info& sib19_params, std::string* js_str = nullptr);

/// \brief Pack all cell BCCH-DL-SCH messages (SIB1 + SI messages) from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \param[out] bcch_dl_sch_json_msgs Optional list of BCCH-DL-SCH messages serialized as JSON (one entry per returned
/// message). If nullptr, no conversion takes place.
/// \return A list of buffers with packed cell BCCH-DL-SCH message. First buffer is SIB1, the rest are SI messages.
std::vector<bcch_dl_sch_payload_type>
pack_all_bcch_dl_sch_msgs(const du_cell_config&     du_cfg,
                          si_message_set            msg_set,
                          std::vector<std::string>* bcch_dl_sch_json_msgs = nullptr);

} // namespace asn1_packer
} // namespace odu
} // namespace ocudu
