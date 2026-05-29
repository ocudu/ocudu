// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
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

struct du_config_update_request {
  gnb_du_id_t                             gnb_du_id;
  std::vector<cu_cp_du_served_cells_item> served_cells_to_add;
  std::vector<nr_cell_global_id_t>        served_cells_to_rem;
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
};

} // namespace ocudu::ocucp
