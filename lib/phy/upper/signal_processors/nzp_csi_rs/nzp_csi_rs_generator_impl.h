// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/support/re_pattern.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/phy/upper/signal_processors/nzp_csi_rs/nzp_csi_rs_generator.h"

namespace ocudu {

/// Implements a parameter validator for \ref nzp_csi_rs_generator_impl.
class nzp_csi_rs_configuration_validator_impl : public nzp_csi_rs_configuration_validator
{
public:
  // See interface for documentation.
  error_type<std::string> is_valid(const nzp_csi_rs_generator::config_t& config) const override;
};

/// Defines a NZP-CSI-RS signal generator.
class nzp_csi_rs_generator_impl : public nzp_csi_rs_generator
{
private:
  /// Maximum number of PRB subcarriers that the CSI-RS can occupy, deduced from TS38.211 Table 7.4.1.5.3-1.
  static constexpr unsigned MAX_SUBCS_PRB = 4;
  /// Maximum currently supported ports for the CSI-RS generation.
  static constexpr unsigned CSI_RS_MAX_PORTS = 16;
  /// Maximum sequence length for one OFDM symbol.
  static constexpr unsigned MAX_SEQ_LEN = MAX_SUBCS_PRB * MAX_NOF_PRBS;

  /// \brief CDM sequence element struct.
  /// \remark This struct is used to build the CDM sequence tables.
  struct cdm_sequence {
    /// Frequency code sequences \f$w_f\f$.
    static_vector<float, 2> w_f;
    /// Time code sequences \f$w_t\f$.
    static_vector<float, 4> w_t;
  };

  /// Temporary RE storage.
  static_re_buffer<CSI_RS_MAX_PORTS, MAX_SEQ_LEN * MAX_NSYMB_PER_SLOT> data;

  /// FD-CDM2 sequence table, as defined in TS38.211 Table 7.4.1.5.3-3.
  static const std::array<const cdm_sequence, 2> fd_cdm2_table;
  /// CDM4-FD2-TD2 sequence table, as defined in TS38.211 Table 7.4.1.5.3-4.
  static const std::array<const cdm_sequence, 4> cdm4_fd2_td2_table;
  /// CDM8-FD2-TD4 sequence table, as defined in TS38.211 Table 7.4.1.5.3-5.
  static const std::array<const cdm_sequence, 8> cdm8_fd2_td4_table;

  /// Pseudo-random sequece generator for the NZP-CSI-RS signal.
  std::unique_ptr<pseudo_random_generator> prg;
  /// Resource grid mapper.
  std::unique_ptr<resource_grid_mapper> mapper;

  /// \brief Generates the NZP-CSI-RS sequence.
  ///
  /// This method implements the NZP-CSI-RS sequence generation, as described in TS38.211 Section 7.4.1.5.2.
  /// The generated sequence does not include the CDM processing.
  ///
  /// \param[out] sequence The returned NZP-CSI-RS sequence.
  /// \param[in] symbol The symbol index within the slot.
  /// \param[in] config Provides the higher layer parameters necessary to generate the sequence.
  ///
  /// \note The generated sequence length is determined by the size of the provided \c sequence.
  void generate_sequence(span<cf_t> sequence, unsigned symbol, const config_t& config) const;

  /// \brief Applies the CDM codes to the NZP-CSI-RS sequence.
  ///
  /// This helper method takes an NZP-CSI-RS sequence pertaining to a single OFDM symbol, and applies the corresponding
  /// CDM code, as defined in TS38.211 Section 7.4.1.5.3.
  ///
  /// \param[out] seq_out The returned NZP-CSI-RS sequence with CDM.
  /// \param[in] seq_in The input sequence.
  /// \param[in] cdm The CDM type.
  /// \param[in] cdm_idx The applied CDM sequence.
  /// \param[in] l_idx The \f$l'\f$ symbol index within the \f$w_t\f$ TD-CDM sequence.
  void apply_cdm(span<cf_t>            seq_out,
                 span<const cf_t>      seq_in,
                 const csi_rs_cdm_type cdm,
                 const unsigned        cdm_idx,
                 const unsigned        l_idx);

public:
  nzp_csi_rs_generator_impl(std::unique_ptr<pseudo_random_generator> prg_,
                            std::unique_ptr<resource_grid_mapper>    mapper_) :
    prg(std::move(prg_)), mapper(std::move(mapper_))
  {
    ocudu_assert(prg, "Invalid pseudo random generator.");
    ocudu_assert(mapper, "Invalid resource grid mapper.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace ocudu
