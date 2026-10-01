// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "csi_report_on_puxch_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/csi_report/csi_report_config_helpers.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;

// \brief Gets the number of <em>indicators of the number of non-zero wideband amplitude coefficients</em> present in
// CSI Part 1.
//
// This is equal to the maximum number of layers according to the RI restriction, as specified in TS38.212
// Table 6.3.2.1.2-3.
//
// \param[in] config CSI report configuration.
// \return the number of indicator fields present in the CSI Part 1 if the CSI report is for precoding codebook Type II
// for quantities containing RI, PMI, and CQI. Zero otherwise.
static unsigned get_nof_amplitude_indicators(const csi_report_configuration& config)
{
  if (!std::holds_alternative<pmi_codebook_typeII>(config.pmi_codebook) ||
      ((config.quantities != csi_report_quantities::cri_ri_pmi_cqi) &&
       (config.quantities != csi_report_quantities::cri_ri_li_pmi_cqi))) {
    return 0;
  }

  return (config.ri_restriction.find_highest() >= 1) ? max_nof_typeII_layers : 1;
}

// Calculates CSI Part 1 size following TS38.212 Table 6.3.2.1.2-3.
static units::bits get_csi_report_part1_size(const csi_report_configuration& config,
                                             const ri_li_cqi_cri_sizes&      field_sizes)
{
  unsigned part1_size = 0;

  // CRI, RI and wideband CQI for the first TB are always present and their size does not depend on the RI.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    part1_size += field_sizes.cri;
    part1_size += field_sizes.ri;
    part1_size += field_sizes.wideband_cqi_first_tb;
  }

  // Indicators of the number of non-zero wideband amplitude coefficients, one per layer for which the PMI can be
  // reported.
  part1_size += get_nof_amplitude_indicators(config) * field_sizes.nof_wideband_amplitudes;

  return units::bits(part1_size);
}

// Calculates CSI Part 2 size following TS38.212 Table 6.3.2.1.2-4.
static units::bits get_csi_report_part2_size(const csi_report_configuration& config,
                                             csi_report_data::ri_type        ri,
                                             const typeII_nof_amplitudes&    nof_amplitudes = {})
{
  // Get CSI Part 2 field sizes, which depend on the number of layers.
  ri_li_cqi_cri_sizes part2_sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, ri, config.nof_csi_rs_resources);

  unsigned part2_size = 0;

  // Wideband CQI for the second TB.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    part2_size += part2_sizes.wideband_cqi_second_tb;
  }

  // LI.
  if (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
    part2_size += part2_sizes.li;
  }

  // PMI.
  if ((config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_pmi_cqi)) {
    part2_size += csi_report_get_size_pmi(config.pmi_codebook, ri, nof_amplitudes);
  }

  return units::bits(part2_size);
}

// \brief Unpacks the indicators of the number of non-zero wideband amplitude coefficients from CSI Part 1.
//
// The returned list contains one entry per reported layer. The indicator of the second layer is set to all zeros when
// the reported rank is one, in which case it is discarded.
//
// \return The number of non-zero wideband amplitude coefficients of each layer, or \c std::nullopt if any of the
// indicators is out of range.
static std::optional<typeII_nof_amplitudes> unpack_nof_amplitudes(const csi_report_packed&        csi1_packed,
                                                                  const csi_report_configuration& config,
                                                                  const ri_li_cqi_cri_sizes&      sizes,
                                                                  csi_report_data::ri_type        ri,
                                                                  unsigned                        offset)
{
  typeII_nof_amplitudes result;

  unsigned nof_indicators = get_nof_amplitude_indicators(config);
  if (nof_indicators == 0) {
    return result;
  }

  unsigned nof_coefficients = 2 * std::get<pmi_codebook_typeII>(config.pmi_codebook).nof_beams.value();

  for (unsigned i_layer = 0; i_layer != nof_indicators; ++i_layer) {
    // The field reports the number of non-zero wideband amplitude coefficients minus one.
    unsigned nof_amplitudes = csi1_packed.extract(offset, sizes.nof_wideband_amplitudes) + 1;
    offset += sizes.nof_wideband_amplitudes;

    // Skip the indicators of the layers that are not reported.
    if (i_layer >= ri.value()) {
      continue;
    }

    if (nof_amplitudes > nof_coefficients) {
      return std::nullopt;
    }

    result.push_back(nof_amplitudes);
  }

  return result;
}

// \brief Fills the CSI Part 2 correspondence of a Type II CSI report.
//
// The Type II CSI Part 2 payload size depends on the reported rank and on the number of non-zero wideband amplitude
// coefficients \f$M_l\f$ of each layer for which the PMI can be reported, as per TS38.212 Table 6.3.2.1.2-3.
static void fill_typeII_part2_correspondence(csi_report_size&                result,
                                             const csi_report_configuration& config,
                                             const ri_li_cqi_cri_sizes&      part1_sizes)
{
  const auto& codebook = std::get<pmi_codebook_typeII>(config.pmi_codebook);

  ocudu_assert(config.ri_restriction.find_highest() < static_cast<int>(max_nof_typeII_layers),
               "The RI restriction set (i.e., {}) allows higher rank values than the Type II maximum number of layers "
               "(i.e., {}).",
               config.ri_restriction,
               max_nof_typeII_layers);

  unsigned nof_coefficients = 2 * codebook.nof_beams.value();
  unsigned nof_indicators   = get_nof_amplitude_indicators(config);
  unsigned indicator_width  = part1_sizes.nof_wideband_amplitudes;

  // Bit offset of the first indicator of the number of non-zero wideband amplitude coefficients within CSI Part 1.
  unsigned indicator_offset = part1_sizes.cri + part1_sizes.ri + part1_sizes.wideband_cqi_first_tb;

  uci_part2_size_description::entry& entry = result.part2_correspondence.entries.emplace_back();

  // Set RI parameter.
  uci_part2_size_description::parameter& ri_param = entry.parameters.emplace_back();
  ri_param.offset                                 = part1_sizes.cri;
  ri_param.width                                  = part1_sizes.ri;

  // Set one indicator parameter per allowed layer.
  for (unsigned i_layer = 0; i_layer != nof_indicators; ++i_layer) {
    uci_part2_size_description::parameter& indicator_param = entry.parameters.emplace_back();
    indicator_param.offset                                 = indicator_offset + i_layer * indicator_width;
    indicator_param.width                                  = indicator_width;
  }

  // Number of values of each indicator field.
  unsigned nof_m0_values = 1U << indicator_width;
  unsigned nof_m1_values = (nof_indicators > 1) ? nof_m0_values : 1;

  for (unsigned i_rank = 1, nof_ranks = config.ri_restriction.size(); i_rank <= nof_ranks; ++i_rank) {
    if (!config.ri_restriction.test(i_rank - 1)) {
      continue;
    }

    for (unsigned m0_value = 0; m0_value != nof_m0_values; ++m0_value) {
      for (unsigned m1_value = 0; m1_value != nof_m1_values; ++m1_value) {
        // The indicator fields report the number of non-zero wideband amplitude coefficients minus one.
        typeII_nof_amplitudes nof_amplitudes;
        nof_amplitudes.push_back(std::min(m0_value + 1, nof_coefficients));
        if (i_rank == 2) {
          nof_amplitudes.push_back(std::min(m1_value + 1, nof_coefficients));
        }

        // Calculate CSI Part 2 size following TS38.212 Table 6.3.2.1.2-4.
        units::bits part2_size = get_csi_report_part2_size(config, i_rank, nof_amplitudes);

        entry.map.emplace_back(part2_size.value());
      }
    }
  }

  result.part2_min_size = units::bits(*std::min_element(entry.map.begin(), entry.map.end()));
  result.part2_max_size = units::bits(*std::max_element(entry.map.begin(), entry.map.end()));
}

static csi_report_data unpack_pusch_csi_cri_ri_li_pmi_cqi(const csi_report_packed&        csi1_packed,
                                                          const csi_report_packed&        csi2_packed,
                                                          const csi_report_configuration& config)
{
  csi_report_data data{.valid = true};

  // Extract field sizes. Given that the PMI field sizes depend on the RI, use a placeholder for the RI (one layer) and
  // get its correct size after extracting the RI.
  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, 1U, config.nof_csi_rs_resources);

  // Verify the CSI Part 1 size.
  units::bits csi_part1_size = get_csi_report_part1_size(config, sizes);
  ocudu_assert(csi1_packed.size() == csi_part1_size.value(),
               "The number of packed bits for CSI Part 1 (i.e., {}) is not equal to the CSI Part 1 size (i.e., {}).",
               units::bits(csi1_packed.size()),
               csi_part1_size);

  // Unpack CSI Part 1 and Part 2 bits following the order in TS38.212 Tables 6.3.2.1.2-3 and 6.3.2.1.2-4.
  unsigned csi1_count = 0;
  unsigned csi2_count = 0;

  // Extract CRI.
  unsigned cri = 0;
  if (sizes.cri > 0) {
    cri = csi1_packed.extract(csi1_count, sizes.cri);
  }
  data.cri.push_back(cri);
  csi1_count += sizes.cri;

  // Extract RI.
  csi_report_data::ri_type ri =
      csi_report_unpack_ri(csi1_packed.slice(csi1_count, csi1_count + sizes.ri), config.ri_restriction);
  data.ri.emplace(ri.value());
  csi1_count += sizes.ri;

  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    // Wideband CQI for the first TB.
    data.first_tb_wideband_cqi.emplace(
        csi_report_unpack_wideband_cqi(csi1_packed.slice(csi1_count, csi1_count + sizes.wideband_cqi_first_tb)));
    csi1_count += sizes.wideband_cqi_first_tb;
  }

  // Extract the number of non-zero wideband amplitude coefficients of each layer, exclusive to the Type II codebook.
  std::optional<typeII_nof_amplitudes> nof_amplitudes =
      unpack_nof_amplitudes(csi1_packed, config, sizes, ri, csi1_count);
  ocudu_assert(nof_amplitudes.has_value(),
               "The reported number of non-zero wideband amplitude coefficients is out of range.");
  csi1_count += get_nof_amplitude_indicators(config) * sizes.nof_wideband_amplitudes;

  ocudu_assert(csi1_count == csi1_packed.size(),
               "The number of read bits (i.e., {}) is not equal to the CSI Part 1 report size (i.e., {}).",
               units::bits(csi1_count),
               csi1_packed.size());

  // Verify the CSI Part 2 size.
  units::bits csi_part2_size = get_csi_report_part2_size(config, ri, *nof_amplitudes);
  ocudu_assert(csi2_packed.size() == csi_part2_size.value(),
               "The number of packed bits for CSI Part 2 (i.e., {}) is not equal to the CSI Part 2 size (i.e., {}).",
               units::bits(csi2_packed.size()),
               csi_part2_size);

  // Skip CSI Part 2 unpacking if the payload is empty.
  if (csi2_packed.empty()) {
    return data;
  }

  // Get field sizes for the unpacked RI.
  ri_li_cqi_cri_sizes sizes_ri =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, ri, config.nof_csi_rs_resources);

  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    // Wideband CQI for the second TB (only present for RI > 4).
    if (sizes_ri.wideband_cqi_second_tb != 0) {
      data.second_tb_wideband_cqi.emplace(
          csi_report_unpack_wideband_cqi(csi2_packed.slice(csi2_count, csi2_count + sizes_ri.wideband_cqi_second_tb)));
      csi2_count += sizes_ri.wideband_cqi_second_tb;
    }
  }

  // Extract LI.
  if (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
    unsigned li = 0;
    if (sizes_ri.li != 0) {
      li = csi2_packed.extract(csi2_count, sizes_ri.li);
    }

    data.li.emplace(li);
    csi2_count += sizes_ri.li;
  }

  // PMI wideband information fields X1 and X2, or codebook index for 2 antenna ports.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    unsigned pmi_size = csi_report_get_size_pmi(config.pmi_codebook, ri, *nof_amplitudes);

    if (pmi_size != 0) {
      data.pmi.emplace(csi_report_unpack_pmi(
          csi2_packed.slice(csi2_count, csi2_count + pmi_size), config.pmi_codebook, ri, *nof_amplitudes));
      csi2_count += pmi_size;
    }
  }

  ocudu_assert(csi2_count == csi2_packed.size(),
               "The number of read bits (i.e., {}) is not equal to the CSI Part 2 report size (i.e., {}).",
               units::bits(csi2_count),
               csi2_packed.size());

  return data;
}

csi_report_size ocudu::get_csi_report_pusch_size(const csi_report_configuration& config)
{
  // Handle cri-RSRP and ssb-Index-RSRP quantities.
  if ((config.quantities == csi_report_quantities::cri_rsrp) ||
      (config.quantities == csi_report_quantities::ssb_index_rsrp)) {
    // Make sure configurations are valid.
    ocudu_assert(std::holds_alternative<std::monostate>(config.pmi_codebook),
                 "PMI codebook is not compatible with this CSI report quantity.");
    ocudu_assert(config.ri_restriction.empty(), "RI restriction is not compatible with this CSI report quantity.");
    ocudu_assert(!config.subband.has_value(), "Subband is not compatible with this CSI report quantity.");

    // Get the CSI report size.
    return get_csi_report_size_cri_ssbri_rsrp(config.nof_csi_rs_resources, config.nof_reported_rs);
  }

  ocudu_assert(config.nof_reported_rs == 1, "The number of reported Resource Sets must be one.");
  ocudu_assert(!config.subband.has_value(), "Subbands CSI reports are not supported on PUSCH.");

  csi_report_size result                = {};
  unsigned        nof_csi_antenna_ports = get_precoding_codebook_antenna_ports(config.pmi_codebook);
  unsigned        max_rank              = get_precoding_codebook_max_rank(config.pmi_codebook);

  // Get CSI Part 1 field sizes which do not depend on the number of layers.
  ri_li_cqi_cri_sizes part1_sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, 1, config.nof_csi_rs_resources);

  // Calculate CSI Part 1 size following TS38.212 Table 6.3.2.1.2-3.
  result.part1_size = get_csi_report_part1_size(config, part1_sizes);

  // Skip CSI Part 2 if there is one transmit port or no quantity is reported in CSI Part 2. The cri-RI-CQI quantity
  // reports the wideband CQI for the second TB in CSI Part 2 when the codebook allows more than four layers.
  bool has_part2_content = (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) ||
                           (config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
                           ((config.quantities == csi_report_quantities::cri_ri_cqi) && (max_rank > 4));
  if ((nof_csi_antenna_ports == 1) || !has_part2_content) {
    return result;
  }

  // Fill the Part 2 correspondence in case of Type II codebook, where the number of non-zero wideband amplitude
  // coefficients is taken into account.
  if (std::holds_alternative<pmi_codebook_typeII>(config.pmi_codebook)) {
    fill_typeII_part2_correspondence(result, config, part1_sizes);
    return result;
  }

  // Prepare CSI Part 2 entry.
  uci_part2_size_description::entry& entry = result.part2_correspondence.entries.emplace_back();

  // Prepare RI field as parameter.
  uci_part2_size_description::parameter& parameter = entry.parameters.emplace_back();
  parameter.offset                                 = part1_sizes.cri;
  parameter.width                                  = part1_sizes.ri;

  // Fill the entry table in function of the RI.
  for (unsigned i_rank = 1; i_rank <= max_rank; ++i_rank) {
    // As per TS38.214 Section 5.2.2.2.1, the RI can only indicate rank values allowed by the RI restriction bitset. If
    // the RI restriction bit corresponding to the current rank is not set, exclude the corresponding CSI Part 2 size.
    if (!config.ri_restriction.test(i_rank - 1)) {
      continue;
    }

    // Calculate CSI Part 2 size following TS38.212 Table 6.3.2.1.2-4.
    units::bits part2_size = get_csi_report_part2_size(config, i_rank);

    entry.map.emplace_back(part2_size.value());
  }

  result.part2_min_size = units::bits(*std::min_element(entry.map.begin(), entry.map.end()));
  result.part2_max_size = units::bits(*std::max_element(entry.map.begin(), entry.map.end()));

  return result;
}

bool ocudu::validate_pusch_csi_payload(const csi_report_packed&        csi1_packed,
                                       const csi_report_packed&        csi2_packed,
                                       const csi_report_configuration& config)
{
  // CSI report configuration is invalid.
  if (!is_valid(config)) {
    return false;
  }

  // If the PMI codebook type is for more than a single CSI-RS port and the CSI report quantities include PMI and/or LI,
  // The CSI Part 2 payload must be present.
  if ((!std::holds_alternative<pmi_codebook_one_port>(config.pmi_codebook) &&
       ((config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) ||
        (config.quantities == csi_report_quantities::cri_ri_pmi_cqi))) &&
      csi2_packed.empty()) {
    return false;
  }

  // Verify the CSI Part 1 payload size.
  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, 1U, config.nof_csi_rs_resources);
  units::bits csi_part1_size = get_csi_report_part1_size(config, sizes);
  if (csi1_packed.size() != csi_part1_size.value()) {
    return false;
  }

  unsigned ri_packed = csi1_packed.extract(sizes.cri, sizes.ri);

  // The RI is out of bounds given the number of allowed rank values.
  if (ri_packed >= config.ri_restriction.count()) {
    return false;
  }

  csi_report_data::ri_type ri =
      csi_report_unpack_ri(csi1_packed.slice(sizes.cri, sizes.cri + sizes.ri), config.ri_restriction);

  // The reported number of non-zero wideband amplitude coefficients is out of range.
  std::optional<typeII_nof_amplitudes> nof_amplitudes =
      unpack_nof_amplitudes(csi1_packed, config, sizes, ri, sizes.cri + sizes.ri + sizes.wideband_cqi_first_tb);
  if (!nof_amplitudes.has_value()) {
    return false;
  }

  // Verify the CSI Part 2 payload size.
  units::bits csi_part2_size = get_csi_report_part2_size(config, ri, *nof_amplitudes);
  if (csi2_packed.size() != csi_part2_size.value()) {
    return false;
  }

  return true;
}

csi_report_data ocudu::csi_report_unpack_pusch(const csi_report_packed&        csi1_packed,
                                               const csi_report_packed&        csi2_packed,
                                               const csi_report_configuration& config)
{
  ocudu_assert(!config.subband.has_value(), "Subbands CSI reports are not supported on PUSCH.");
  ocudu_assert(!std::holds_alternative<std::monostate>(config.pmi_codebook), "Unsupported PMI codebook type.");

  [[maybe_unused]] bool is_pmi_codebook_one_port = std::holds_alternative<pmi_codebook_one_port>(config.pmi_codebook);
  [[maybe_unused]] unsigned ri_restriction_size  = config.ri_restriction.size();
  [[maybe_unused]] unsigned max_rank             = get_precoding_codebook_max_rank(config.pmi_codebook);
  ocudu_assert(is_pmi_codebook_one_port || (ri_restriction_size >= max_rank),
               "The RI restriction set size, i.e., {}, is smaller than the maximum rank, i.e., {}.",
               ri_restriction_size,
               max_rank);

  ocudu_assert(is_pmi_codebook_one_port || (config.ri_restriction.find_highest() < static_cast<int>(max_rank)),
               "The RI restriction set, i.e., {}, allows higher rank values than the maximum rank, i.e., {}.",
               config.ri_restriction,
               max_rank);

  // Assert that CSI Part 2 payload is present if it is required.
  ocudu_assert((is_pmi_codebook_one_port || ((config.quantities != csi_report_quantities::cri_ri_li_pmi_cqi) &&
                                             (config.quantities != csi_report_quantities::cri_ri_pmi_cqi))) ||
                   !csi2_packed.empty(),
               "PUSCH CSI Part 2 is required for more than one CSI-RS port when PMI is reported.");

  // Select unpacking depending on the CSI report quantities.
  switch (config.quantities) {
    case csi_report_quantities::cri_ri_pmi_cqi:
    case csi_report_quantities::cri_ri_cqi:
    case csi_report_quantities::cri_ri_li_pmi_cqi:
      return unpack_pusch_csi_cri_ri_li_pmi_cqi(csi1_packed, csi2_packed, config);
    case csi_report_quantities::cri_rsrp:
    case csi_report_quantities::ssb_index_rsrp:
      ocudu_assert(csi2_packed.empty(), "CSI Part 2 is not expected for this quantity.");
      return csi_report_unpack_cri_ssbri_rsrp(csi1_packed, config.nof_csi_rs_resources, config.nof_reported_rs);
    case csi_report_quantities::other:
    default:
      report_error("Invalid CSI report quantities.");
  }
}

csi_report_data ocudu::csi_report_unpack_pusch(const csi_report_packed&        csi1_packed,
                                               const csi_report_configuration& config)
{
  return csi_report_unpack_pusch(csi1_packed, csi_report_packed(0), config);
}
