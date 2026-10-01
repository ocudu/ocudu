// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/tensor.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"

namespace ocudu {
namespace detail {

enum class baseband_gateway_buffer_dims : unsigned { sample = 0, channel, all };

/// Implements a baseband gateway buffer reader based on a tensor.
class baseband_gateway_buffer_reader_tensor : public baseband_gateway_buffer_reader
{
public:
  using storage_type = tensor<static_cast<unsigned>(detail::baseband_gateway_buffer_dims::all),
                              ci16_t,
                              detail::baseband_gateway_buffer_dims>;

  /// Creates a gateway buffer reader based on a tensor storage type.
  explicit baseband_gateway_buffer_reader_tensor(const storage_type& data_) : data(data_) {}

  // See interface for documentation.
  unsigned get_nof_channels() const override { return data.get_dimension_size(baseband_gateway_buffer_dims::channel); }

  // See interface for documentation.
  unsigned get_nof_samples() const override { return data.get_dimension_size(baseband_gateway_buffer_dims::sample); }

  // See interface for documentation.
  span<const ci16_t> get_channel_buffer(unsigned i_channel) const override { return data.get_view({i_channel}); }

private:
  const storage_type& data;
};

/// Implements a baseband gateway buffer writer based on a tensor.
class baseband_gateway_buffer_writer_tensor : public baseband_gateway_buffer_writer
{
public:
  using storage_type = tensor<static_cast<unsigned>(detail::baseband_gateway_buffer_dims::all),
                              ci16_t,
                              detail::baseband_gateway_buffer_dims>;

  /// Creates a gateway buffer writer based on a tensor storage type.
  explicit baseband_gateway_buffer_writer_tensor(storage_type& data_) : data(data_) {}

  // See interface for documentation.
  unsigned get_nof_channels() const override { return data.get_dimension_size(baseband_gateway_buffer_dims::channel); }

  // See interface for documentation.
  unsigned get_nof_samples() const override { return data.get_dimension_size(baseband_gateway_buffer_dims::sample); }

  // See interface for documentation.
  span<ci16_t> get_channel_buffer(unsigned i_channel) override { return data.get_view({i_channel}); }

private:
  storage_type& data;
};

} // namespace detail

/// \brief Describes a baseband buffer implementation that comprises a fix number of channels that can be dynamically
/// resized.
///
/// It contains a fixed get_nof_channels() number of channels that contain the same number of get_nof_samples() samples.
/// The number of samples can be changed at runtime without re-allocating memory using resize(). The number samples
/// shall never exceed the maximum number of samples indicated in the constructor.
class baseband_gateway_buffer_dynamic
{
public:
  /// Gets the number of channels.
  unsigned get_nof_channels() const { return data.get_dimension_size(detail::baseband_gateway_buffer_dims::channel); }

  /// Gets the current number of samples.
  unsigned get_nof_samples() const { return data.get_dimension_size(detail::baseband_gateway_buffer_dims::sample); }

  /// Gets the reader interface.
  const baseband_gateway_buffer_reader& get_reader() const { return reader; }

  /// Gets the writer interface.
  baseband_gateway_buffer_writer& get_writer() { return writer; }

  /// Gets a data view of a channel.
  span<ci16_t> operator[](unsigned i_channel) { return writer.get_channel_buffer(i_channel); }

  /// \brief Resize buffer.
  /// \param[in] new_nof_samples Indicates the new number of samples per channel.
  /// \note The new number of samples must be greater than 0 and must not exceed the maximum number of samples.
  void resize(unsigned new_nof_samples) { data.resize({new_nof_samples, get_nof_channels()}); }

  /// \brief Default constructor.
  /// \param[in] nof_channels    Indicates the number of channels to create.
  /// \param[in] max_nof_samples Indicates the maximum number of samples.
  baseband_gateway_buffer_dynamic(unsigned nof_channels, unsigned max_nof_samples) :
    data({max_nof_samples, nof_channels}), reader(data), writer(data)
  {
  }

  /// Move constructor.
  baseband_gateway_buffer_dynamic(baseband_gateway_buffer_dynamic&& other) noexcept :
    data(std::move(other.data)), reader(data), writer(data)
  {
  }

private:
  dynamic_tensor<static_cast<unsigned>(detail::baseband_gateway_buffer_dims::all),
                 ci16_t,
                 detail::baseband_gateway_buffer_dims>
                                                data;
  detail::baseband_gateway_buffer_reader_tensor reader;
  detail::baseband_gateway_buffer_writer_tensor writer;
};

} // namespace ocudu
