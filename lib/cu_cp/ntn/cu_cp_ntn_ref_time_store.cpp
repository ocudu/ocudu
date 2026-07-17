// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cu_cp/cu_cp_ntn_ref_time_store.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/ntn/ntn_configuration_manager.h"
#include "ocudu/ntn/ntn_configuration_manager_dependencies.h"
#include "ocudu/ntn/ntn_configuration_manager_factory.h"
#include "ocudu/ntn/ntn_meas_info_update_handler.h"

using namespace ocudu;

cu_cp_ntn_ref_time_store::cu_cp_ntn_ref_time_store(span<const nr_cell_identity> cells)
{
  // Pre-populate the map so its keys are fixed; both threads only look up entries afterwards.
  for (const nr_cell_identity& nci : cells) {
    mappings.try_emplace(nci);
  }
}

void cu_cp_ntn_ref_time_store::on_ref_time_info_report(span<const nr_cell_global_id_t>     served_cells,
                                                       const ocucp::cu_cp_ref_time_report& report)
{
  for (const nr_cell_global_id_t& cgi : served_cells) {
    auto it = mappings.find(cgi.nci);
    if (it == mappings.end()) {
      // Not a tracked NTN cell.
      continue;
    }
    it->second.write_and_commit(ocudu_ntn::ntn_time_slot_mapping{report.ref_slot, report.time});
  }
}

std::optional<ocudu_ntn::ntn_time_slot_mapping>
cu_cp_ntn_ref_time_store::get_last_mapping(const nr_cell_global_id_t& nr_cgi, subcarrier_spacing scs) const
{
  auto it = mappings.find(nr_cgi.nci);
  if (it == mappings.end()) {
    return std::nullopt;
  }
  // Return the mapping exactly as reported by the DU: the epoch stays anchored to a real DU-provided SFN/time pair
  // instead of one derived from the CU-CP clock. Freshness comes from the periodic DU reference time reporting.
  return it->second.read();
}

namespace {

/// Adapts the NTN time provider interface to the CU-CP reference time store.
class cu_cp_ntn_time_provider_adapter : public ocudu_ntn::ntn_time_provider
{
public:
  explicit cu_cp_ntn_time_provider_adapter(cu_cp_ntn_ref_time_store& store_) : store(store_) {}

  std::optional<ocudu_ntn::ntn_time_slot_mapping> get_last_mapping(const nr_cell_global_id_t& nr_cgi,
                                                                   subcarrier_spacing         scs) override
  {
    return store.get_last_mapping(nr_cgi, scs);
  }

private:
  cu_cp_ntn_ref_time_store& store;
};

/// Forwards NTN neighbour cell measurement info updates from the NTN configuration manager to the CU-CP.
class cu_cp_ntn_meas_info_adapter : public ocudu_ntn::ntn_meas_info_update_handler
{
public:
  explicit cu_cp_ntn_meas_info_adapter(ocucp::cu_cp_ntn_meas_update_handler& handler_) : handler(handler_) {}

  void handle_ntn_meas_info_update(const ocudu_ntn::ntn_meas_info_update_request& req) override
  {
    std::vector<ocucp::rrc_ntn_neighbour_cell_info_item> items;
    items.reserve(req.ncells.size());
    for (const ocudu_ntn::ntn_neighbour_meas_info& ncell : req.ncells) {
      ocucp::rrc_ntn_neighbour_cell_info_item& item = items.emplace_back();
      item.nci                                      = ncell.nci;
      item.info.epoch_time                          = ncell.epoch_time;
      item.info.ephemeris                           = ncell.ephemeris;
      if (ncell.ref_location) {
        item.info.ref_location = reference_location{ncell.ref_location->latitude, ncell.ref_location->longitude};
      }
      item.polarization = ncell.polarization;
    }
    handler.update_ntn_neighbour_info(req.serving_cgi.nci, std::move(items));
  }

private:
  ocucp::cu_cp_ntn_meas_update_handler& handler;
};

} // namespace

std::unique_ptr<ocudu_ntn::ntn_time_provider> ocudu::create_cu_cp_ntn_time_provider(cu_cp_ntn_ref_time_store& store)
{
  return std::make_unique<cu_cp_ntn_time_provider_adapter>(store);
}

std::unique_ptr<ocudu_ntn::ntn_meas_info_update_handler>
ocudu::create_cu_cp_ntn_meas_info_handler(ocucp::cu_cp_ntn_meas_update_handler& handler)
{
  return std::make_unique<cu_cp_ntn_meas_info_adapter>(handler);
}

std::unique_ptr<ocudu_ntn::ntn_configuration_manager>
ocudu::create_cu_cp_ntn_configuration_manager(const ocudu_ntn::ntn_configuration_manager_config& config,
                                              cu_cp_ntn_ref_time_store&                          ref_time_store,
                                              ocucp::cu_cp_ntn_meas_update_handler&              meas_update_handler,
                                              timer_manager&                                     timers,
                                              task_executor&                                     executor)
{
  ocudu_ntn::ntn_configuration_manager_dependencies dependencies{
      /*sib19_msg_update_handler=*/nullptr,
      create_cu_cp_ntn_time_provider(ref_time_store),
      /*doppler_handler=*/nullptr,
      create_cu_cp_ntn_meas_info_handler(meas_update_handler),
      timers,
      executor};

  return ocudu_ntn::create_ntn_configuration_manager(config, std::move(dependencies));
}
