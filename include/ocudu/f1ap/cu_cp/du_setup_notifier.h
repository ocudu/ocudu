// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/f1ap_cause.h"
#include "ocudu/ran/cu_cp_cell_configuration.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace ocudu::ocucp {

struct du_setup_request {
  gnb_du_id_t                             gnb_du_id;
  std::string                             gnb_du_name;
  std::vector<cu_cp_du_served_cells_item> gnb_du_served_cells_list;
  uint8_t                                 gnb_du_rrc_version;
  // TODO: Add optional fields
};

struct f1ap_cells_to_be_activ_list_item {
  nr_cell_global_id_t  nr_cgi;
  std::optional<pci_t> nr_pci;
};

/// Result of a DU setup request operation.
struct du_setup_result {
  struct accepted {
    std::string                                   gnb_cu_name;
    std::vector<f1ap_cells_to_be_activ_list_item> cells_to_be_activ_list;
    uint8_t                                       gnb_cu_rrc_version;
  };
  struct rejected {
    f1ap_cause_t cause;
    std::string  cause_str;
  };

  std::variant<accepted, rejected> result;

  /// Whether the DU setup request was accepted by the CU-CP.
  bool is_accepted() const { return std::holds_alternative<accepted>(result); }
};

/// Served cell whose configuration the gNB-DU changed, as per TS 38.473, Section 9.2.1.7.
struct du_cell_to_modify {
  /// Identity the cell had before the change.
  nr_cell_global_id_t old_cgi;
  /// Configuration the cell takes.
  cu_cp_du_served_cells_item cell;
};

struct du_config_update_request {
  gnb_du_id_t                             gnb_du_id;
  std::vector<cu_cp_du_served_cells_item> served_cells_to_add;
  std::vector<du_cell_to_modify>          served_cells_to_mod;
  std::vector<nr_cell_global_id_t>        served_cells_to_rem;
};

/// Result of a DU configuration update.
struct du_config_update_result {
  struct accepted {
    /// Cells the gNB-DU activates. Empty when the update adds no cell that the CU-CP activates.
    std::vector<f1ap_cells_to_be_activ_list_item> cells_to_be_activ_list;
    /// Cells the gNB-DU deactivates, because the CU-CP stopped serving them.
    std::vector<nr_cell_global_id_t> cells_to_be_deactiv_list;
  };
  using rejected = du_setup_result::rejected;

  std::variant<accepted, rejected> result;

  /// Whether the CU-CP accepted the DU configuration update.
  bool is_accepted() const { return std::holds_alternative<accepted>(result); }
};

/// \brief Interface used to handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class du_setup_notifier
{
public:
  virtual ~du_setup_notifier() = default;

  /// \brief Notifies about the reception of a F1 Setup Request message.
  /// \param[in] msg The received F1 Setup Request message.
  /// \return Error with time-to-wait in case the F1 Setup should not be accepted by the DU.
  virtual du_setup_result on_new_du_setup_request(const du_setup_request& msg) = 0;

  /// \brief Notifies about the reception of a gNB-DU Configuration Update message.
  /// \param[in] msg The received gNB-DU Configuration Update message.
  /// \return The cells the gNB-DU activates, or the cause that rejects the update.
  virtual du_config_update_result on_new_du_config_update(const du_config_update_request& msg) = 0;
};

} // namespace ocudu::ocucp
