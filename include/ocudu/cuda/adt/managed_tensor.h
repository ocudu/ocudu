// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/tensor.h"
#include "ocudu/cuda/adt/managed_vector.h"
#include <numeric>

namespace ocudu {
namespace cuda {

/// \brief Tensor whose elements live in memory that the host and the device both address.
///
/// It is \ref dynamic_tensor with the storage exchanged: the elements belong to a
/// \ref managed_vector instead of a \c std::vector. A grid or a buffer built on it is addressed as
/// a tensor by the host and as a flat block by an accelerator, without a second allocation or a
/// separate view class.
///
/// \remark The memory is managed rather than device-only, because the host reads the same elements
/// through the tensor views.
template <unsigned NDIMS, typename Type, typename Index_type = unsigned>
class managed_tensor : public tensor<NDIMS, Type, Index_type>
{
public:
  using dimensions_size_type = typename tensor<NDIMS, Type, Index_type>::dimensions_size_type;

  /// Creates an empty tensor, which obtains no memory until it is resized.
  managed_tensor() { dimensions_size.fill(0); }

  /// Creates a tensor of the given dimensions.
  explicit managed_tensor(const dimensions_size_type& dimensions) { resize(dimensions); }

  /// Returns true if the tensor obtained its managed allocation.
  bool is_valid() const { return !elements.empty() || (size() == 0); }

  // See interface for documentation.
  void resize(const dimensions_size_type& dimensions) override
  {
    dimensions_size = dimensions;

    unsigned total_size = size();
    if (total_size == 0) {
      return;
    }
    if (total_size > elements.size()) {
      cuda_expected<managed_vector<Type>> memory = managed_vector<Type>::create(total_size);
      if (!memory.has_value()) {
        // The caller detects the failure through is_valid(), because an allocation that the driver
        // refuses is not an error this class can act on.
        elements = managed_vector<Type>();
        return;
      }
      elements = std::move(memory.value());
    }
  }

  // See interface for documentation.
  const dimensions_size_type& get_dimensions_size() const override { return dimensions_size; }

  // See interface for documentation.
  span<Type> get_data() override { return elements.empty() ? span<Type>() : span<Type>(elements.data(), size()); }

  // See interface for documentation.
  span<const Type> get_data() const override
  {
    return elements.empty() ? span<const Type>() : span<const Type>(elements.data(), size());
  }

  /// Gives the driver a hint about which side accesses the elements, so that it places them there.
  cuda_result advise_accessed_by(memory_location location, int device_id) const
  {
    return elements.advise_accessed_by(location, device_id);
  }

  /// Moves the elements to one side of the system, queued in a stream.
  cuda_result prefetch(memory_location location, int device_id, const cuda_stream& stream) const
  {
    return elements.prefetch(location, device_id, stream);
  }

private:
  /// Number of elements across all dimensions.
  constexpr unsigned size() const
  {
    return std::accumulate(dimensions_size.begin(), dimensions_size.end(), 1U, std::multiplies<>());
  }

  dimensions_size_type dimensions_size;
  managed_vector<Type> elements;
};

} // namespace cuda
} // namespace ocudu
