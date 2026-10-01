// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_configuration_manager.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/plmn_identity.h"
#include <algorithm>

using namespace ocudu;
using namespace ocucp;

static error_type<du_setup_result::rejected> validate_cell_config(const cu_cp_du_served_cells_item& served_cell)
{
  if (not served_cell.served_cell_info.five_gs_tac.has_value()) {
    return make_unexpected(du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                                     fmt::format("Missing TAC for cell")});
  }

  if (not served_cell.gnb_du_sys_info.has_value()) {
    return make_unexpected(du_setup_result::rejected{cause_protocol_t::semantic_error,
                                                     fmt::format("Missing system information for cell")});
  }

  return {};
}

class du_configuration_manager::du_configuration_handler_impl : public du_configuration_handler
{
public:
  du_configuration_handler_impl(du_configuration_manager& parent_) : parent(parent_) {}
  ~du_configuration_handler_impl() override
  {
    if (ctxt != nullptr) {
      parent.rem_du(this->ctxt->id);
    }
  }

  validation_result handle_new_du_config(const du_setup_request&         req,
                                         span<const nr_cell_global_id_t> readable_cells) override
  {
    if (this->ctxt != nullptr) {
      return make_unexpected(
          du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state, "DU already configured"});
    }
    auto ret = parent.add_du_config(req, readable_cells);
    if (ret.has_value()) {
      this->ctxt = ret.value();
      return {};
    }
    return make_unexpected(ret.error());
  }

  error_type<du_config_update_result::rejected>
  handle_du_config_update(const du_config_update_request& req, span<const nr_cell_global_id_t> readable_cells) override
  {
    if (this->ctxt == nullptr) {
      return make_unexpected(du_config_update_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                                               "DU with same gNB-DU-Id was not setup"});
    }

    // Reconfiguration.
    auto ret = parent.handle_du_config_update(*this->ctxt, req, readable_cells);
    if (not ret.has_value()) {
      return make_unexpected(ret.error());
    }
    this->ctxt = ret.value();
    return {};
  }

  void handle_gnb_cu_configuration_update(const f1ap_gnb_cu_configuration_update& req) override
  {
    if (this->ctxt == nullptr) {
      ocudulog::fetch_basic_logger("CU-CP").debug(
          "Can't handle gNB CU Configuration Update. Cause: DU configuration context not found.");
      return;
    }
    parent.handle_gnb_cu_configuration_update(req, this->ctxt->id);
  }

private:
  du_configuration_manager& parent;
};

du_configuration_manager::du_configuration_manager(
    const gnb_id_t&                               gnb_id_,
    const std::vector<plmn_identity>&             plmns_,
    const std::vector<ntn_cell_location_mapping>& ntn_location_mappings_) :
  gnb_id(gnb_id_),
  plmns(plmns_),
  ntn_location_mappings(ntn_location_mappings_),
  logger(ocudulog::fetch_basic_logger("CU-CP"))
{
}

std::unique_ptr<du_configuration_handler> du_configuration_manager::create_du_handler()
{
  return std::make_unique<du_configuration_handler_impl>(*this);
}

/// \brief Reads the broadcast TAC list (trackingAreaList, TS 38.331) from the gNB-DU's SIB1.
///
/// F1AP carries at most one TAC per broadcast PLMN, never several for one PLMN, so SIB1 is the only source.
/// Empty when the cell broadcasts trackingAreaCode.
///
/// TODO: TS 38.413 reports the list for the UE's serving PLMN, while the CU-CP stores one list per cell and reads it
/// from the first PLMN-IdentityInfo. A cell broadcasting a different list per PLMN is not supported.
static tac_list_t
extract_broadcast_tac_list(const nr_cell_global_id_t& cgi, const byte_buffer& packed_sib1, tac_t five_gs_tac)
{
  tac_list_t tac_list;

  asn1::cbit_ref       bref{packed_sib1};
  asn1::rrc_nr::sib1_s sib1;
  if (sib1.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
    ocudulog::fetch_basic_logger("CU-CP").warning(
        "nci={}: Failed to unpack the SIB1 of the served cell. Assuming the cell broadcasts a single TAC", cgi.nci);
    return tac_list;
  }

  const auto& plmn_id_info_list = sib1.cell_access_related_info.plmn_id_info_list;
  if (plmn_id_info_list.size() == 0 or not plmn_id_info_list[0].tracking_area_list_r17.is_present()) {
    return tac_list;
  }

  for (const auto& asn1_tac : *plmn_id_info_list[0].tracking_area_list_r17) {
    tac_list.push_back(asn1_tac.to_number());
  }

  // The gNB-DU reports one TAC per cell over F1AP, so a list without it contradicts the cell configuration.
  if (std::find(tac_list.begin(), tac_list.end(), five_gs_tac) == tac_list.end()) {
    ocudulog::fetch_basic_logger("CU-CP").warning(
        "nci={}: Ignoring the broadcast TAC list [{}] of the served cell. Cause: it does not contain the 5GS TAC {} "
        "that the gNB-DU reports for the cell over F1AP. Assuming the cell broadcasts that TAC alone",
        cgi.nci,
        fmt::join(tac_list, ", "),
        five_gs_tac);
    tac_list.clear();
  }

  return tac_list;
}

du_cell_configuration
du_configuration_manager::create_du_cell_config(du_cell_index_t                   cell_idx,
                                                const cu_cp_du_served_cells_item& f1ap_cell_cfg) const
{
  const auto&           cell_req = f1ap_cell_cfg.served_cell_info;
  du_cell_configuration cell;
  cell.cell_index = cell_idx;
  cell.cgi        = cell_req.nr_cgi;
  if (cell_req.five_gs_tac.has_value()) {
    cell.tac = cell_req.five_gs_tac.value();
  }
  cell.pci               = cell_req.nr_pci;
  cell.served_plmns      = cell_req.served_plmns;
  cell.deactivated_plmns = {};
  cell.nr_mode_info      = cell_req.nr_mode_info;
  cell.meas_timing_cfg   = cell_req.meas_timing_cfg.copy();
  // Add band information.
  if (std::holds_alternative<cu_cp_fdd_info>(cell_req.nr_mode_info)) {
    for (const auto& band : std::get<cu_cp_fdd_info>(cell_req.nr_mode_info).dl_nr_freq_info.freq_band_list_nr) {
      cell.bands.push_back(uint_to_nr_band(band.freq_band_ind_nr));
    }
  } else if (std::holds_alternative<cu_cp_tdd_info>(cell_req.nr_mode_info)) {
    for (const auto& band : std::get<cu_cp_tdd_info>(cell_req.nr_mode_info).nr_freq_info.freq_band_list_nr) {
      cell.bands.push_back(uint_to_nr_band(band.freq_band_ind_nr));
    }
  }
  // Add packed MIB and SIB1
  cell.sys_info.packed_mib  = f1ap_cell_cfg.gnb_du_sys_info->mib_msg.copy();
  cell.sys_info.packed_sib1 = f1ap_cell_cfg.gnb_du_sys_info->sib1_msg.copy();
  cell.tac_list             = extract_broadcast_tac_list(cell.cgi, cell.sys_info.packed_sib1, cell.tac);
  cell.location_mapping     = get_location_mapping(cell);
  return cell;
}

/// \brief Returns the configured coarse-location mapping of a cell, if any.
ntn_location_mapping du_configuration_manager::get_location_mapping(const du_cell_configuration& cell) const
{
  auto mapping_it = std::find_if(ntn_location_mappings.begin(), ntn_location_mappings.end(), [&cell](const auto& item) {
    return item.nci == cell.cgi.nci;
  });
  if (mapping_it == ntn_location_mappings.end()) {
    // Only an NTN cell derives anything from a position, so a terrestrial one without a mapping says nothing.
    if (not ntn_location_mappings.empty() and
        std::any_of(cell.bands.begin(), cell.bands.end(), band_helper::is_ntn_band)) {
      logger.info("Cell={}: No coarse UE location mapping is configured for this cell, so it derives no TAC from a "
                  "reported position",
                  cell.cgi.nci);
    }
    return {};
  }

  // Only an NTN cell is ever asked for a position, so a mapping on any other cell is configuration that can never be
  // reached.
  if (std::none_of(cell.bands.begin(), cell.bands.end(), band_helper::is_ntn_band)) {
    logger.warning("Cell={}: Location mapping is configured, but the cell is not an NTN cell. No TAC will be derived "
                   "for it",
                   cell.cgi.nci);
  }

  logger.info(
      "Cell={}: Configured {} coarse UE location areas", cell.cgi.nci, mapping_it->mapping.location_areas.size());

  return mapping_it->mapping;
}

expected<const du_configuration_context*, du_setup_result::rejected>
du_configuration_manager::add_du_config(const du_setup_request& req, span<const nr_cell_global_id_t> readable_cells)
{
  // Validate the DU-level configuration.
  auto result = validate_new_du_config(req);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }

  // Admit the served cells one by one. A cell that the CU-CP cannot serve is left out of the configuration, so
  // that the remaining cells of the DU still come up. The first rejection cause is kept: it is reported to the
  // DU if no cell is admitted, which keeps a single-cell DU rejected with the cause of its only cell.
  std::vector<du_cell_configuration>       admitted_cells;
  std::optional<du_setup_result::rejected> first_rejection;
  for (const auto& served_cell : req.gnb_du_served_cells_list) {
    const nr_cell_global_id_t& cgi = served_cell.served_cell_info.nr_cgi;

    std::optional<du_setup_result::rejected> rejection;
    if (std::find(readable_cells.begin(), readable_cells.end(), cgi) == readable_cells.end()) {
      rejection = du_setup_result::rejected{cause_protocol_t::semantic_error,
                                            fmt::format("Could not read the RRC containers of cell nci={}", cgi.nci)};
    } else if (std::any_of(admitted_cells.begin(), admitted_cells.end(), [&cgi](const du_cell_configuration& cell) {
                 return cell.cgi == cgi;
               })) {
      // Keep the first occurrence of a cell the DU reports more than once.
      rejection = du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                            "The DU reports the served cell CGI more than once"};
    } else if (auto cell_result = validate_cell_config_request(served_cell, req.gnb_du_id);
               not cell_result.has_value()) {
      rejection = cell_result.error();
    }

    if (rejection.has_value()) {
      logger.warning("du_id={}: Not serving cell nci={}. Cause: {}",
                     fmt::underlying(req.gnb_du_id),
                     cgi.nci,
                     rejection->cause_str);
      if (not first_rejection.has_value()) {
        first_rejection = std::move(rejection);
      }
      continue;
    }

    admitted_cells.push_back(create_du_cell_config(to_du_cell_index(admitted_cells.size()), served_cell));
  }

  if (admitted_cells.empty()) {
    // A DU setup request carries at least one served cell, so a cause is set here whenever the request has
    // cells. The fallback covers a caller that passes an empty served cell list.
    return make_unexpected(first_rejection.value_or(du_setup_result::rejected{
        cause_protocol_t::semantic_error, "The DU reports no served cell that the CU-CP can serve"}));
  }

  // Create new DU config context.
  auto                      ret  = dus.emplace(req.gnb_du_id, du_configuration_context{});
  du_configuration_context& ctxt = ret.first->second;
  ctxt.id                        = req.gnb_du_id;
  ctxt.name                      = req.gnb_du_name;
  ctxt.rrc_version               = req.gnb_du_rrc_version;
  ctxt.served_cells              = std::move(admitted_cells);
  return &ctxt;
}

/// Finds a cell of a DU in either of its cell lists.
static std::vector<du_cell_configuration>::iterator find_cell(std::vector<du_cell_configuration>& cells,
                                                              const nr_cell_global_id_t&          cgi)
{
  return std::find_if(
      cells.begin(), cells.end(), [&cgi](const du_cell_configuration& item) { return item.cgi == cgi; });
}

/// Removes a cell from the DU configuration. Returns false if the DU does not serve the cell.
static bool remove_du_cell(du_configuration_context& ctxt, const nr_cell_global_id_t& cgi)
{
  if (auto it = find_cell(ctxt.served_cells, cgi); it != ctxt.served_cells.end()) {
    ctxt.served_cells.erase(it);
    return true;
  }
  if (auto it = find_cell(ctxt.deactivated_cells, cgi); it != ctxt.deactivated_cells.end()) {
    ctxt.deactivated_cells.erase(it);
    return true;
  }
  return false;
}

/// Returns the lowest cell index the DU does not use.
static du_cell_index_t find_free_cell_index(const du_configuration_context& ctxt)
{
  for (unsigned i = 0; i != MAX_NOF_DU_CELLS; ++i) {
    const du_cell_index_t cell_idx = to_du_cell_index(i);
    auto in_use = [cell_idx](const du_cell_configuration& item) { return item.cell_index == cell_idx; };
    if (std::none_of(ctxt.served_cells.begin(), ctxt.served_cells.end(), in_use) and
        std::none_of(ctxt.deactivated_cells.begin(), ctxt.deactivated_cells.end(), in_use)) {
      return cell_idx;
    }
  }
  return INVALID_DU_CELL_INDEX;
}

expected<const du_configuration_context*, du_config_update_result::rejected>
du_configuration_manager::handle_du_config_update(const du_configuration_context& current_ctxt,
                                                  const du_config_update_request& req,
                                                  span<const nr_cell_global_id_t> readable_cells)
{
  if (current_ctxt.id != req.gnb_du_id) {
    logger.warning("du_id={}: Failed to update DU. Cause: DU ID mismatch", fmt::underlying(current_ctxt.id));
    return make_unexpected(
        du_config_update_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state, "DU ID mismatch"});
  }
  auto it = dus.find(current_ctxt.id);
  if (it == dus.end()) {
    logger.error("du_id={}: DU config update called for non-existent DU", fmt::underlying(current_ctxt.id));
    return make_unexpected(du_config_update_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                                             "DU with the given gNB-DU-Id was not setup"});
  }

  auto result = validate_du_config_update(current_ctxt, req);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }

  du_configuration_context& du_context = it->second;

  // Whether the CU-CP can serve the cell the DU reports, given the cells it already serves.
  auto admit_cell = [this, &du_context, readable_cells](
                        const cu_cp_du_served_cells_item& cell) -> error_type<du_config_update_result::rejected> {
    const nr_cell_global_id_t& cgi = cell.served_cell_info.nr_cgi;
    if (std::find(readable_cells.begin(), readable_cells.end(), cgi) == readable_cells.end()) {
      return make_unexpected(du_config_update_result::rejected{
          cause_protocol_t::semantic_error, fmt::format("Could not read the RRC containers of cell nci={}", cgi.nci)});
    }
    if (du_context.find_cell_any_state(cgi) != nullptr) {
      return make_unexpected(du_config_update_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                                               "The DU already serves the cell"});
    }
    return validate_cell_config_request(cell, du_context.id);
  };

  // > Remove the cells the DU stopped serving.
  for (const nr_cell_global_id_t& cgi : req.served_cells_to_rem) {
    if (not remove_du_cell(du_context, cgi)) {
      logger.warning("du_id={}: Cannot remove cell nci={}. Cause: The DU does not serve it",
                     fmt::underlying(du_context.id),
                     cgi.nci);
    }
  }

  // > Take over the configuration of the cells the DU changed. A cell the CU-CP can no longer serve is removed,
  //   so that the CU-CP never picks it for a UE.
  for (const du_cell_to_modify& cell_to_mod : req.served_cells_to_mod) {
    const du_cell_configuration* old_cell = du_context.find_cell_any_state(cell_to_mod.old_cgi);
    if (old_cell == nullptr) {
      logger.warning("du_id={}: Cannot modify cell nci={}. Cause: The DU does not serve it",
                     fmt::underlying(du_context.id),
                     cell_to_mod.old_cgi.nci);
      continue;
    }
    const du_cell_index_t cell_idx = old_cell->cell_index;
    const bool            deactivated =
        find_cell(du_context.deactivated_cells, cell_to_mod.old_cgi) != du_context.deactivated_cells.end();

    remove_du_cell(du_context, cell_to_mod.old_cgi);

    auto admission = admit_cell(cell_to_mod.cell);
    if (not admission.has_value()) {
      logger.warning("du_id={}: Not serving cell nci={} any more. Cause: {}",
                     fmt::underlying(du_context.id),
                     cell_to_mod.cell.served_cell_info.nr_cgi.nci,
                     admission.error().cause_str);
      continue;
    }

    du_cell_configuration cell = create_du_cell_config(cell_idx, cell_to_mod.cell);
    if (deactivated) {
      // The cell keeps the state the CU-CP gave it, so it stays deactivated until the CU-CP activates it.
      cell.deactivated_plmns = cell.served_plmns;
      cell.served_plmns.clear();
      du_context.deactivated_cells.push_back(std::move(cell));
    } else {
      du_context.served_cells.push_back(std::move(cell));
    }
  }

  // > Add the cells the DU started serving.
  for (const cu_cp_du_served_cells_item& cell_to_add : req.served_cells_to_add) {
    auto admission = admit_cell(cell_to_add);
    if (not admission.has_value()) {
      logger.warning("du_id={}: Not serving cell nci={}. Cause: {}",
                     fmt::underlying(du_context.id),
                     cell_to_add.served_cell_info.nr_cgi.nci,
                     admission.error().cause_str);
      continue;
    }

    const du_cell_index_t cell_idx = find_free_cell_index(du_context);
    if (cell_idx == INVALID_DU_CELL_INDEX) {
      logger.error("du_id={}: Not serving cell nci={}. Cause: The DU serves the maximum number of cells ({})",
                   fmt::underlying(du_context.id),
                   cell_to_add.served_cell_info.nr_cgi.nci,
                   MAX_NOF_DU_CELLS);
      continue;
    }
    du_context.served_cells.push_back(create_du_cell_config(cell_idx, cell_to_add));
  }

  return &it->second;
}

void du_configuration_manager::handle_gnb_cu_configuration_update(const f1ap_gnb_cu_configuration_update& req,
                                                                  gnb_du_id_t                             du_id)
{
  if (dus.find(du_id) == dus.end()) {
    logger.warning("du_id={}: Failed to update DU. Cause: DU context not found", fmt::underlying(du_id));
    return;
  }
  auto& du_ctxt = dus.at(du_id);

  for (const auto& cell : req.cells_to_be_activated_list) {
    // Find cell in the served cells.
    auto it = std::find_if(
        du_ctxt.served_cells.begin(), du_ctxt.served_cells.end(), [&cell](const auto& c) { return c.cgi == cell.cgi; });

    // If the cell is already in the served cells, update the PLMNs.
    if (it != du_ctxt.served_cells.end()) {
      // If a PLMN from the update is not in the currently served PLMN list, add it to the served PLMNs.
      for (const auto& updated_plmn : cell.available_plmn_list) {
        if (std::find(it->served_plmns.begin(), it->served_plmns.end(), updated_plmn) == it->served_plmns.end()) {
          it->served_plmns.push_back(updated_plmn);
        }
        // The cell serves the PLMN again, so it is no longer deactivated.
        it->deactivated_plmns.erase(
            std::remove(it->deactivated_plmns.begin(), it->deactivated_plmns.end(), updated_plmn),
            it->deactivated_plmns.end());
      }

      // If a currently served PLMN is not in the update, add the PLMN to the deactivated.
      for (const auto& served_plmn : it->served_plmns) {
        if (std::find(cell.available_plmn_list.begin(), cell.available_plmn_list.end(), served_plmn) ==
            cell.available_plmn_list.end()) {
          it->deactivated_plmns.push_back(served_plmn);
        }
      }

      // Remove deactivated PLMNs from served PLMNs.
      for (const auto& deactivated_plmn : it->deactivated_plmns) {
        it->served_plmns.erase(std::remove(it->served_plmns.begin(), it->served_plmns.end(), deactivated_plmn),
                               it->served_plmns.end());
      }
    }

    // If the cell is deactivated, move it to the served cells.
    auto deactivated_it = std::find_if(du_ctxt.deactivated_cells.begin(),
                                       du_ctxt.deactivated_cells.end(),
                                       [&cell](const auto& c) { return c.cgi == cell.cgi; });
    if (deactivated_it != du_ctxt.deactivated_cells.end()) {
      // Activate all PLMNs from the update.
      for (const auto& plmn_to_activate : cell.available_plmn_list) {
        // Add PLMN to served PLMNs.
        deactivated_it->served_plmns.push_back(plmn_to_activate);
        // Remove PLMN from deactivated PLMNs.
        deactivated_it->deactivated_plmns.erase(std::remove(deactivated_it->deactivated_plmns.begin(),
                                                            deactivated_it->deactivated_plmns.end(),
                                                            plmn_to_activate),
                                                deactivated_it->deactivated_plmns.end());
      }

      // Move cell to served cells.
      du_ctxt.served_cells.push_back(*deactivated_it);
      du_ctxt.deactivated_cells.erase(deactivated_it);
    }
  }

  for (const auto& cell : req.cells_to_be_deactivated_list) {
    // Find cell in the served cells.
    auto it = std::find_if(
        du_ctxt.served_cells.begin(), du_ctxt.served_cells.end(), [&cell](const auto& c) { return c.cgi == cell.cgi; });

    // If the cell in the served cells, deactivate it.
    if (it != du_ctxt.served_cells.end()) {
      // Move all served PLMNs to deactivated PLMNs.
      for (const auto& plmn : it->served_plmns) {
        it->deactivated_plmns.push_back(plmn);
      }
      // Remove deactivated PLMNs from served PLMNs.
      for (const auto& deactivated_plmn : it->deactivated_plmns) {
        it->served_plmns.erase(std::remove(it->served_plmns.begin(), it->served_plmns.end(), deactivated_plmn),
                               it->served_plmns.end());
      }

      // Move cell to deactivated cells.
      du_ctxt.deactivated_cells.push_back(*it);
      du_ctxt.served_cells.erase(it);
    }
  }
}

void du_configuration_manager::rem_du(gnb_du_id_t du_id)
{
  auto it = dus.find(du_id);
  if (it == dus.end()) {
    logger.warning("du={}: Failed to remove DU. Cause: DU not found", fmt::underlying(du_id));
    return;
  }
  dus.erase(it);
}

error_type<du_setup_result::rejected>
du_configuration_manager::validate_new_du_config(const du_setup_request& req) const
{
  if (req.gnb_du_served_cells_list.size() > MAX_NOF_DU_CELLS) {
    return make_unexpected(
        du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state, "Too many served cells"});
  }

  // Ensure the DU config does not collide with another DU. A colliding cell is caught per cell in
  // validate_cell_config_request.
  if (dus.find(req.gnb_du_id) != dus.end()) {
    return make_unexpected(
        du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state, "Duplicate DU ID"});
  }

  return {};
}

error_type<du_config_update_result::rejected>
du_configuration_manager::validate_du_config_update(const du_configuration_context& current_ctxt,
                                                    const du_config_update_request& req) const
{
  // Count the cells the DU serves once the update is applied. A cell the DU does not serve cannot be removed,
  // so it keeps counting.
  size_t nof_cells = current_ctxt.served_cells.size() + current_ctxt.deactivated_cells.size();
  for (const nr_cell_global_id_t& cgi : req.served_cells_to_rem) {
    if (current_ctxt.find_cell_any_state(cgi) != nullptr) {
      --nof_cells;
    }
  }
  nof_cells += req.served_cells_to_add.size();

  if (nof_cells > MAX_NOF_DU_CELLS) {
    return make_unexpected(du_config_update_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                                             "Too many served cells"});
  }

  return {};
}

error_type<du_setup_result::rejected>
du_configuration_manager::validate_cell_config_request(const cu_cp_du_served_cells_item& cell_req,
                                                       gnb_du_id_t                       serving_du) const
{
  auto ret = validate_cell_config(cell_req);
  if (not ret.has_value()) {
    return make_unexpected(ret.error());
  }

  // Ensure NCIs match the gNB-Id.
  gnb_id_t served_gnb_id = cell_req.served_cell_info.nr_cgi.nci.gnb_id(gnb_id.bit_length);
  if (served_gnb_id != gnb_id) {
    return make_unexpected(du_setup_result::rejected{
        cause_protocol_t::msg_not_compatible_with_receiver_state,
        fmt::format("NCI {:#x} of the served Cell does not match this gNB-Id ({:#x} != {:#x})",
                    cell_req.served_cell_info.nr_cgi.nci,
                    gnb_id.id,
                    served_gnb_id.id)});
  }

  if (std::find(plmns.begin(), plmns.end(), cell_req.served_cell_info.nr_cgi.plmn_id) == plmns.end()) {
    return make_unexpected(du_setup_result::rejected{f1ap_cause_radio_network_t::plmn_not_served_by_the_gnb_cu,
                                                     "Served Cell CGI PLMN is not supported by the CU-CP"});
  }

  if (std::none_of(
          cell_req.served_cell_info.served_plmns.begin(),
          cell_req.served_cell_info.served_plmns.end(),
          [this](const plmn_identity& plmn) { return std::find(plmns.begin(), plmns.end(), plmn) != plmns.end(); })) {
    return make_unexpected(du_setup_result::rejected{f1ap_cause_radio_network_t::plmn_not_served_by_the_gnb_cu,
                                                     "None of the served cell PLMNs is available in the CU-CP"});
  }

  // Ensure no other DU already serves the cell.
  for (const auto& [du_id, du_cfg] : dus) {
    if (du_id == serving_du) {
      continue;
    }
    if (du_cfg.find_cell_any_state(cell_req.served_cell_info.nr_cgi) != nullptr) {
      return make_unexpected(du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                                       "Duplicate served cell CGI"});
    }
  }

  return {};
}
