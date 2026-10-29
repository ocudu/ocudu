// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/cell_configuration.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/pdsch/pdsch_antenna_ports_mapping.h"
#include "ocudu/ran/pdsch/pdsch_dmrs_symbol_mask.h"
#include "ocudu/scheduler/result/dmrs_info.h"
#include "ocudu/support/error_handling.h"

namespace ocudu {

/// Calculates the number of RE used for DMRS per RB in PDSCH and PUSCH transmissions.
inline unsigned calculate_nof_dmrs_per_rb(const dmrs_information& dmrs)
{
  // Calculate number of RE for DMRS per RB, symbol and CDM group.
  unsigned nof_dmrs_re = get_nof_re_per_prb(dmrs.config_type);

  // Calculate the maximum number of CDM groups without data and make sure the number does not exceed the maximum.
  unsigned max_nof_cdm_groups_without_data = get_max_nof_cdm_groups_without_data(dmrs.config_type);
  ocudu_assert(dmrs.num_dmrs_cdm_grps_no_data >= 1 && dmrs.num_dmrs_cdm_grps_no_data <= max_nof_cdm_groups_without_data,
               "Invalid number of CDM groups without data {}.",
               dmrs.num_dmrs_cdm_grps_no_data);

  // Determines the number of RE for DMRS per RB.
  return nof_dmrs_re * dmrs.num_dmrs_cdm_grps_no_data * dmrs.dmrs_symb_pos.count();
}

/// Builds the DM-RS symbol mask of a PDSCH with single front-load DM-RS symbols.
inline dmrs_symbol_mask make_pdsch_dmrs_symbol_mask(const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                                    dmrs_typeA_position                          dmrs_typeA_pos,
                                                    dmrs_additional_positions                    additional_positions)
{
  if (pdsch_td_cfg.map_type == sch_mapping_type::typeA) {
    pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
    dmrscfg.typeA_pos                         = dmrs_typeA_pos;
    dmrscfg.additional_position               = additional_positions;
    dmrscfg.last_symbol                       = pdsch_td_cfg.symbols.stop();
    dmrscfg.lte_crs_match_around              = false;
    dmrscfg.ue_capable_additional_dmrs_dl_alt = false;
    return pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);
  }

  // TS38.214 Table 5.1.2.1-1 allows a mapping type B PDSCH of 2 to 13 symbols (2, 4 or 6 with extended cyclic prefix).
  // The bounds on the duration field below are checked by an assertion, which a release build removes, so an invalid
  // duration is rejected here explicitly.
  const unsigned nof_symbols = pdsch_td_cfg.symbols.length();
  report_fatal_error_if_not(
      nof_symbols >= 2 and nof_symbols <= 13, "Invalid mapping type B PDSCH duration of {} symbols", nof_symbols);

  pdsch_dmrs_symbol_mask_mapping_type_B_single_configuration dmrscfg{};
  dmrscfg.duration            = nof_symbols;
  dmrscfg.start_symbol        = pdsch_td_cfg.symbols.start();
  dmrscfg.additional_position = additional_positions;
  return pdsch_dmrs_symbol_mask_mapping_type_B_single_get(dmrscfg);
}

/// [Implementation-defined] Returns the DM-RS ports of a single-user PDSCH, i.e., antenna ports 1000 to 999 +
/// \c nof_layers.
inline dmrs_port_mask get_su_pdsch_dmrs_ports(unsigned nof_layers)
{
  dmrs_port_mask ports(DMRS_MAX_NPORTS);
  ports.fill(0, nof_layers);
  return ports;
}

/// \brief Finds the value of the Antenna port(s) field of DCI format 1_1 that signals the DM-RS of a PDSCH.
/// \return The row of TS 38.212 Tables 7.3.1.2.2-1/2/3/4, or \c std::nullopt if no row matches the DM-RS.
inline std::optional<unsigned> find_pdsch_antenna_ports_row_index(const dmrs_information&     dmrs,
                                                                  const dmrs_downlink_config& dmrs_dl_cfg_ded)
{
  const dmrs_max_length max_len = dmrs_dl_cfg_ded.is_max_length_len2 ? dmrs_max_length::len2 : dmrs_max_length::len1;
  // Double-symbol DM-RS is not supported, hence the DM-RS always uses a single front-load symbol.
  return find_pdsch_antenna_port_mapping_row_index(
      dmrs.config_type, max_len, {dmrs.num_dmrs_cdm_grps_no_data, dmrs.dmrs_ports, 1});
}

/// \brief Sets the DM-RS ports of a PDSCH scheduled by DCI f1_1.
///
/// \param dmrs_ports                  DM-RS ports of the transmission, one per layer.
/// \param nof_cdm_groups_without_data Number of DM-RS CDM groups without data. If not set, it is the minimum number
///                                    of CDM groups that the DM-RS ports require.
/// \remark An assertion is triggered if DCI f1_1 cannot signal the DM-RS ports and CDM groups.
inline void set_pdsch_dmrs_ports(dmrs_information&           dmrs,
                                 const dmrs_downlink_config& dmrs_dl_cfg_ded,
                                 const dmrs_port_mask&       dmrs_ports,
                                 std::optional<unsigned>     nof_cdm_groups_without_data = std::nullopt)
{
  ocudu_assert(dmrs_ports.any(), "A PDSCH requires at least one DM-RS port");

  // Number of DMRS CDM groups without data indicates whether PDSCH is multiplexed with DMRS or not.
  // For DMRS configuration type 1:
  //    num_dmrs_cdm_grps_no_data = 1 (multiplexing of DMRS and PDSCH).
  //    num_dmrs_cdm_grps_no_data = 2 (no multiplexing of DMRS and PDSCH).
  dmrs.num_dmrs_cdm_grps_no_data =
      nof_cdm_groups_without_data.value_or(get_min_nof_cdm_groups_without_data(dmrs.config_type, dmrs_ports));
  dmrs.dmrs_ports = dmrs_ports;

  ocudu_assert(find_pdsch_antenna_ports_row_index(dmrs, dmrs_dl_cfg_ded).has_value(),
               "DCI f1_1 cannot signal DM-RS ports {:#x} with {} CDM group(s) without data",
               dmrs.dmrs_ports.to_uint64(),
               dmrs.num_dmrs_cdm_grps_no_data);
}

/// Helper factory of a default DM-RS Information object for DCI f1_0 or for when higher layer configuration parameters
/// are not yet available.
/// \remark See TS 38.214, 5.1.6.2 - DM-RS reception procedure.
inline dmrs_information make_dmrs_info_common(const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                              pci_t                                        pci,
                                              dmrs_typeA_position                          dmrs_typeA_pos)
{
  dmrs_information dmrs{};

  dmrs.dmrs_symb_pos = make_pdsch_dmrs_symbol_mask(pdsch_td_cfg, dmrs_typeA_pos, dmrs_additional_positions::pos2);
  dmrs.config_type   = dmrs_config_type::type1;
  // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.dmrs_scrambling_id = pci;
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  // See TS 38.214, 5.1.6.2.
  // When receiving PDSCH scheduled by DCI format 1_0, the UE shall assume the number of DM-RS CDM groups without data
  // is 1 which corresponds to CDM group 0 for the case of PDSCH with allocation duration of 2 symbols,
  // and the UE shall assume that the number of DM-RS CDM groups without data is 2 which corresponds to CDM group {0,1}
  // for all other cases.
  dmrs.num_dmrs_cdm_grps_no_data = pdsch_td_cfg.symbols.length() == 2 ? 1 : 2;
  // Port 1000.
  dmrs.dmrs_ports.resize(DMRS_MAX_NPORTS);
  dmrs.dmrs_ports.set(0);

  return dmrs;
}

/// \brief Helper factory of the DM-RS Information object of a PDSCH scheduled by DCI f1_1.
///
/// \param dmrs_ports                  DM-RS ports of the transmission, one per layer.
/// \param nof_cdm_groups_without_data Number of DM-RS CDM groups without data. If not set, it is the minimum number
///                                    of CDM groups that the DM-RS ports require.
/// \remark An assertion is triggered if DCI f1_1 cannot signal the resulting DM-RS ports and CDM groups.
inline dmrs_information make_dmrs_info_dedicated(const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                                 pci_t                                        pci,
                                                 dmrs_typeA_position                          dmrs_typeA_pos,
                                                 const dmrs_downlink_config&                  dmrs_dl_cfg_ded,
                                                 const dmrs_port_mask&                        dmrs_ports,
                                                 std::optional<unsigned> nof_cdm_groups_without_data = std::nullopt)
{
  dmrs_information dmrs{};

  // See TS 38.211, 7.4.1.1.2.
  if (dmrs_dl_cfg_ded.additional_positions == dmrs_additional_positions::pos3 and
      dmrs_typeA_pos != dmrs_typeA_position::pos2) {
    ocudu_assertion_failure("Invalid PDSCH DMRS configuration. Cause: DMRS Additional Position of pos3 is only "
                            "supported when DMRS TypeA position is equal to pos2.");
  }
  if (pdsch_td_cfg.map_type != sch_mapping_type::typeA) {
    ocudu_terminate("Mapping type B not supported");
  }

  dmrs.dmrs_symb_pos = make_pdsch_dmrs_symbol_mask(pdsch_td_cfg, dmrs_typeA_pos, dmrs_dl_cfg_ded.additional_positions);
  dmrs.config_type   = dmrs_dl_cfg_ded.is_dmrs_type2 ? dmrs_config_type::type2 : dmrs_config_type::type1;
  // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.dmrs_scrambling_id = pci;
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  set_pdsch_dmrs_ports(dmrs, dmrs_dl_cfg_ded, dmrs_ports, nof_cdm_groups_without_data);

  return dmrs;
}

/// Helper factory of a default DM-RS Information object for DCI f1_0 or for when higher layer configuration parameters
/// are not yet available.
/// \remark See TS 38.214, 5.1.6.2 - DM-RS reception procedure.
inline dmrs_information make_dmrs_info_common(span<const pdsch_time_domain_resource_allocation> pdsch_td_list,
                                              uint8_t                                           time_resource,
                                              pci_t                                             pci,
                                              dmrs_typeA_position                               dmrs_typeA_pos)
{
  ocudu_assert(pdsch_td_list.size() > time_resource, "Invalid time resource");
  const pdsch_time_domain_resource_allocation& pdsch_time_res = pdsch_td_list[time_resource];

  return make_dmrs_info_common(pdsch_time_res, pci, dmrs_typeA_pos);
}

/// Helper factory of a default DM-RS Information object for DCI f0_0 or for when higher layer configuration parameters
/// are not yet available.
/// TODO: generalize to other cases (e.g. mapping type B).
/// \remark See TS 38.214 Section 6.2.2.
inline dmrs_information make_dmrs_info_common(const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                              pci_t                                        pci,
                                              dmrs_typeA_position                          dmrs_typeA_pos)
{
  dmrs_information dmrs{};

  if (pusch_td_cfg.map_type == sch_mapping_type::typeA) {
    // Get DM-RS symbols.
    pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
    dmrscfg.typeA_pos                         = dmrs_typeA_pos;
    dmrscfg.additional_position               = dmrs_additional_positions::pos2;
    dmrscfg.last_symbol                       = pusch_td_cfg.symbols.stop();
    dmrscfg.lte_crs_match_around              = false;
    dmrscfg.ue_capable_additional_dmrs_dl_alt = false;
    dmrs.dmrs_symb_pos                        = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);

  } else {
    ocudu_terminate("Mapping type B not supported");
  }

  dmrs.config_type = dmrs_config_type::type1;
  // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.dmrs_scrambling_id = pci;
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  // See TS 38.214, 6.2.2.
  // For PUSCH scheduled by DCI format 0_0 or by activation DCI format 0_0 with CRC scrambled by CS-RNTI, the UE
  // shall assume the number of DM-RS CDM groups without data is 1 which corresponds to CDM group 0 for the case of
  // PUSCH with allocation duration of 2 or less OFDM symbols with transform precoding disabled, the UE shall assume
  // that the number of DM-RS CDM groups without data is 3 which corresponds to CDM group {0,1,2} for the case of
  // PUSCH scheduled by activation DCI format 0_0 and the dmrs-Type in cg-DMRS-Configuration equal to 'type2' and the
  // PUSCH allocation duration being more than 2 OFDM symbols, and the UE shall assume that the number of DM-RS
  // CDM groups without data is 2 which corresponds to CDM group {0,1} for all other cases.
  dmrs.num_dmrs_cdm_grps_no_data = pusch_td_cfg.symbols.length() <= 2 ? 1 : 2;
  // Port 1000.
  dmrs.dmrs_ports.resize(12);
  dmrs.dmrs_ports.set(0);

  return dmrs;
}

inline dmrs_information make_dmrs_info_dedicated(const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                 pci_t                                        pci,
                                                 dmrs_typeA_position                          dmrs_typeA_pos,
                                                 const dmrs_uplink_config&                    dmrs_ul_cfg,
                                                 unsigned                                     nof_layers,
                                                 unsigned                                     nof_ul_antenna_ports,
                                                 bool                                         are_both_cws_enabled)
{
  dmrs_information dmrs{};

  // See TS 38.211, 6.4.1.1.3.
  if (dmrs_ul_cfg.additional_positions == dmrs_additional_positions::pos3 and
      dmrs_typeA_pos != dmrs_typeA_position::pos2) {
    ocudu_assertion_failure("Invalid PUSCH DMRS configuration. Cause: DMRS Additional Position of pos3 is only "
                            "supported when DMRS TypeA position is equal to pos2.");
  }

  if (pusch_td_cfg.map_type == sch_mapping_type::typeA) {
    // Get DM-RS symbols.
    pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
    dmrscfg.typeA_pos                         = dmrs_typeA_pos;
    dmrscfg.additional_position               = dmrs_ul_cfg.additional_positions;
    dmrscfg.last_symbol                       = pusch_td_cfg.symbols.stop();
    dmrscfg.lte_crs_match_around              = false;
    dmrscfg.ue_capable_additional_dmrs_dl_alt = false;
    dmrs.dmrs_symb_pos                        = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);

  } else {
    ocudu_terminate("Mapping type B not supported");
  }

  dmrs.config_type = dmrs_ul_cfg.is_dmrs_type2 ? dmrs_config_type::type2 : dmrs_config_type::type1;
  // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.dmrs_scrambling_id = pci;
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  // TODO: Derive antenna port mapping based on PUSCH-Config and other DMRS configuration parameters.
  // Since in our configuration transformPrecoder=disabled, dmrs-Type=1, maxLength=1, rank = 1, we refer to
  // Table 7.3.1.1.2-8 in TS 38.212.
  // Number of DMRS CDM groups without data indicates whether PUSCH is multiplexed with DMRS or not.
  // For DMRS configuration type 1:
  //    num_dmrs_cdm_grps_no_data = 1 (multiplexing of DMRS and PUSCH).
  //    num_dmrs_cdm_grps_no_data = 2 (no multiplexing of DMRS and PUSCH).
  dmrs.num_dmrs_cdm_grps_no_data = 2;
  // Port 1000.
  // Since we are setting num_dmrs_cdm_grps_no_data = 2 it corresponds to row 3 in Table 7.3.1.1.2-8 in TS 38.212.
  dmrs.dmrs_ports.resize(12);
  dmrs.dmrs_ports.set(0);

  return dmrs;
}

/// Helper factory of a default DM-RS Information object for DCI f0_0 or for when higher layer configuration parameters
/// are not yet available.
/// TODO: generalize to other cases (e.g. mapping type B).
/// \remark See TS 38.214 Section 6.2.2.
inline dmrs_information make_dmrs_info_common(const pusch_config_common& pusch_cfg,
                                              uint8_t                    time_resource,
                                              pci_t                      pci,
                                              dmrs_typeA_position        dmrs_typeA_pos)
{
  ocudu_assert(pusch_cfg.pusch_td_alloc_list.size() > time_resource, "Invalid time resource");
  const pusch_time_domain_resource_allocation& pusch_time_res = pusch_cfg.pusch_td_alloc_list[time_resource];

  return make_dmrs_info_common(pusch_time_res, pci, dmrs_typeA_pos);
}

} // namespace ocudu
