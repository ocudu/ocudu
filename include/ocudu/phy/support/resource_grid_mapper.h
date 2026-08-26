// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/phy/support/rb_allocation.h"
#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

struct re_pattern;
class re_pattern_list;
class precoding_beamforming_configuration;
class precoding_configuration;

/// \brief Resource Element mapping interface.
///
/// Provides a generic interface that DL channel processors use to map Resource Elements into the resource grid.
class resource_grid_mapper
{
public:
  /// Default destructor.
  virtual ~resource_grid_mapper() = default;

  /// Complex symbol buffer.
  class symbol_buffer
  {
  public:
    /// Default destructor.
    virtual ~symbol_buffer() = default;

    /// Gets the maximum block size.
    virtual unsigned get_max_block_size() const = 0;

    /// Gets if the buffer is empty.
    virtual bool empty() const = 0;

    /// Pops \c block_size number of symbols.
    virtual span<const ci8_t> pop_symbols(unsigned block_size) = 0;
  };

  /// Adapts an existing symbol vector to a symbol buffer interface.
  class symbol_buffer_adapter : public symbol_buffer
  {
  public:
    /// Creates an adapter based on the view of a data view.
    symbol_buffer_adapter(span<const ci8_t> symbols_) : symbols(symbols_) {}

    /// Destructor. It triggers an assertion if the buffer is not empty.
    ~symbol_buffer_adapter() { ocudu_assert(symbols.empty(), "{} symbols are still in the buffer.", symbols.size()); }

    // See interface for documentation.
    unsigned get_max_block_size() const override { return symbols.size(); }

    // See interface for documentation.
    bool empty() const override { return symbols.empty(); }

    // See interface for documentation.
    span<const ci8_t> pop_symbols(unsigned block_size) override
    {
      // Make sure the block size does not exceed the number of symbols.
      ocudu_assert(symbols.size() >= block_size,
                   "The block size (i.e., {}) exceeds the number of available symbols (i.e., {}).",
                   block_size,
                   symbols.size());

      // Select view of the symbols to return.
      span<const ci8_t> ret = symbols.first(block_size);

      // Cut symbols.
      symbols = symbols.last(symbols.size() - block_size);

      return ret;
    }

  private:
    span<const ci8_t> symbols;
  };

  /// \brief Maps the input resource elements into the resource grid.
  ///
  /// \param[out] grid       Resource grid writer interface.
  /// \param[in]  input      Input data.
  /// \param[in]  pattern    Data allocation pattern in the resource grid.
  /// \param[in]  precoding  Precoding and beamforming configuration.
  virtual void map(resource_grid_writer&                      grid,
                   const re_buffer_reader<cf_t>&              input,
                   const re_pattern&                          pattern,
                   const precoding_beamforming_configuration& precoding) = 0;

  /// Collects the parameters that describe a physical channel generic resource grid allocation.
  struct allocation_configuration {
    /// \brief Bandwidth part location within the resource grid.
    ///
    /// The BWP start common resource block index is relative to Point A and must be in the range {0, ..., 274}. The BWP
    /// length is expressed as a number of contiguous common resource blocks and must be in the range {1, ..., 275}.
    crb_interval bwp;
    /// Frequency-domain allocation.
    rb_allocation freq_alloc;
    /// \brief Time-domain allocation within a slot.
    ///
    /// The start symbol index and the number of symbols within the slot must be in the range {0, ..., 12} and {1, ...,
    /// 14}, respectively.
    ///
    /// The time allocation must not exceed the maximum number of OFDM symbols in a slot.
    ofdm_symbol_range time_alloc;
  };

  /// \brief Maps complex symbols onto the resource grid.
  ///
  /// \param[out] grid       Resource grid writer interface.
  /// \param[in]  buffer     Buffer containing the complex symbols to map.
  /// \param[in]  allocation Resource allocation parameters.
  /// \param[in]  reserved   Reserved resource elements, to be excluded from the allocation pattern.
  /// \param[in]  precoding  Precoding and beamforming configuration.
  /// \param[in]  re_skip    Number of RE to skip before start mapping the buffer.
  /// \remark Only one PRG is supported, therefore the MIMO precoding matrix of the first PRG is applied to the entire
  /// allocation.
  virtual void map(resource_grid_writer&                      grid,
                   symbol_buffer&                             buffer,
                   const allocation_configuration&            allocation,
                   const re_pattern_list&                     reserved,
                   const precoding_beamforming_configuration& precoding,
                   unsigned                                   re_skip = 0) const = 0;

  /// \brief Maps complex symbols onto the resource grid, without beamforming.
  ///
  /// \param[out] grid       Resource grid writer interface.
  /// \param[in]  buffer     Buffer containing the complex symbols to map.
  /// \param[in]  allocation Resource allocation parameters.
  /// \param[in]  reserved   Reserved resource elements, to be excluded from the allocation pattern.
  /// \param[in]  ports      List of port identifiers onto which the complex symbols are mapped in the resource grid.
  /// \param[in]  precoding  Precoding configuration.
  /// \param[in]  re_skip    Number of RE to skip before start mapping the buffer.
  virtual void map(resource_grid_writer&           grid,
                   symbol_buffer&                  buffer,
                   const allocation_configuration& allocation,
                   const re_pattern_list&          reserved,
                   span<const unsigned>            ports,
                   const precoding_configuration&  precoding,
                   unsigned                        re_skip = 0) const = 0;
};

} // namespace ocudu
