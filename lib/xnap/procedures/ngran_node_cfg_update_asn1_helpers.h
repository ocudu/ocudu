// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../xnap_asn1_converters.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/ran/cu_cp_cell_configuration.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/xnap/xnap_message.h"

namespace ocudu::ocucp {

/// Changes to the NR cells an NG-RAN node serves, as reported in the Served Cells To Update NR IE of TS 38.423
/// section 9.2.2.11.
struct xnap_served_cells_update {
  std::vector<cu_cp_served_cell_info> cells_to_add;
  std::vector<cu_cp_served_cell_info> cells_to_modify;
  std::vector<nr_cell_global_id_t>    cells_to_delete;

  bool empty() const { return cells_to_add.empty() and cells_to_modify.empty() and cells_to_delete.empty(); }
};

/// \brief Generate an NG-RAN Node Configuration Update reporting the cells this node serves.
/// \param[in] update The changes to the cells this node serves.
/// \return The NG-RAN Node Configuration Update message.
inline xnap_message generate_asn1_ngran_node_cfg_update(const xnap_served_cells_update& update)
{
  xnap_message cfg_update;
  cfg_update.pdu.set_init_msg();
  cfg_update.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_N_GRA_NNODE_CFG_UPD);

  asn1::xnap::ngran_node_cfg_upd_s& asn1_ies = cfg_update.pdu.init_msg().value.ngran_node_cfg_upd();

  asn1::xnap::cfg_upd_gnb_container& asn1_gnb             = asn1_ies->cfg_upd_init_node_choice.set_gnb();
  asn1_gnb.served_cells_to_upd_nr_present                 = true;
  asn1::xnap::served_cells_to_upd_nr_s& asn1_cells_to_upd = asn1_gnb.served_cells_to_upd_nr;

  // Fill cells to add.
  for (const auto& cell : update.cells_to_add) {
    asn1::xnap::served_cells_nr_item_s asn1_cell;
    asn1_cell.served_cell_info_nr = served_cell_info_nr_to_asn1(cell);
    asn1_cells_to_upd.served_cells_to_add_nr.push_back(asn1_cell);
  }

  // Fill cells to modify.
  for (const auto& cell : update.cells_to_modify) {
    asn1::xnap::served_cells_to_modify_nr_item_s asn1_cell;
    asn1_cell.old_nr_cgi          = cgi_to_asn1(cell.nr_cgi);
    asn1_cell.served_cell_info_nr = served_cell_info_nr_to_asn1(cell);
    asn1_cells_to_upd.served_cells_to_modify_nr.push_back(asn1_cell);
  }

  // Fill cells to delete.
  for (const auto& cgi : update.cells_to_delete) {
    asn1_cells_to_upd.served_cells_to_delete_nr.push_back(cgi_to_asn1(cgi));
  }

  return cfg_update;
}

/// \brief Generate an NG-RAN Node Configuration Update Acknowledge.
/// \return The NG-RAN Node Configuration Update Acknowledge message.
inline xnap_message generate_asn1_ngran_node_cfg_update_ack()
{
  xnap_message cfg_update_ack;
  cfg_update_ack.pdu.set_successful_outcome();
  cfg_update_ack.pdu.successful_outcome().load_info_obj(ASN1_XNAP_ID_N_GRA_NNODE_CFG_UPD);

  asn1::xnap::ngran_node_cfg_upd_ack_s& asn1_ies =
      cfg_update_ack.pdu.successful_outcome().value.ngran_node_cfg_upd_ack();
  asn1_ies->responding_node_type_cfg_upd_ack.set_gnb();

  return cfg_update_ack;
}

/// \brief Generate an NG-RAN Node Configuration Update Failure.
/// \param[in] cause The cause of the failure.
/// \return The NG-RAN Node Configuration Update Failure message.
inline xnap_message generate_asn1_ngran_node_cfg_update_failure(xnap_cause_t cause)
{
  xnap_message cfg_update_fail;
  cfg_update_fail.pdu.set_unsuccessful_outcome();
  cfg_update_fail.pdu.unsuccessful_outcome().load_info_obj(ASN1_XNAP_ID_N_GRA_NNODE_CFG_UPD);

  asn1::xnap::ngran_node_cfg_upd_fail_s& asn1_ies =
      cfg_update_fail.pdu.unsuccessful_outcome().value.ngran_node_cfg_upd_fail();
  asn1_ies->cause = cause_to_asn1(cause);

  return cfg_update_fail;
}

/// \brief Apply the cell changes an Xn-C peer reported to the cells stored for it.
/// \param[out] peer_cells The cells the Xn-C peer serves.
/// \param[in] asn1_cells_to_upd The Served Cells To Update NR IE received from the peer.
inline void update_peer_served_cells(std::vector<cu_cp_served_cell_info>&        peer_cells,
                                     const asn1::xnap::served_cells_to_upd_nr_s& asn1_cells_to_upd)
{
  auto find_cell = [&peer_cells](const nr_cell_global_id_t& cgi) {
    return std::find_if(peer_cells.begin(), peer_cells.end(), [&cgi](const cu_cp_served_cell_info& cell) {
      return cell.nr_cgi == cgi;
    });
  };

  for (const auto& asn1_cgi : asn1_cells_to_upd.served_cells_to_delete_nr) {
    auto cell_it = find_cell(asn1_to_cgi(asn1_cgi));
    if (cell_it != peer_cells.end()) {
      peer_cells.erase(cell_it);
    }
  }

  for (const auto& asn1_cell : asn1_cells_to_upd.served_cells_to_modify_nr) {
    cu_cp_served_cell_info cell    = asn1_to_served_cell_info(asn1_cell.served_cell_info_nr);
    auto                   cell_it = find_cell(asn1_to_cgi(asn1_cell.old_nr_cgi));
    if (cell_it != peer_cells.end()) {
      *cell_it = cell;
    } else {
      peer_cells.push_back(cell);
    }
  }

  for (const auto& asn1_cell : asn1_cells_to_upd.served_cells_to_add_nr) {
    cu_cp_served_cell_info cell    = asn1_to_served_cell_info(asn1_cell.served_cell_info_nr);
    auto                   cell_it = find_cell(cell.nr_cgi);
    if (cell_it != peer_cells.end()) {
      *cell_it = cell;
    } else {
      peer_cells.push_back(cell);
    }
  }
}

} // namespace ocudu::ocucp
