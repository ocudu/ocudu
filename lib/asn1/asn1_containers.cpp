// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Type-erased storage operations of dyn_array.

#include "ocudu/asn1/asn1_containers.h"
#include <cstring>

namespace asn1 {

/************************
       dyn_array
************************/

namespace {

void construct_elems(const detail::dyn_array_ops& ops, void* dst, uint32_t n)
{
  if (n == 0) {
    return;
  }
  if (ops.construct != nullptr) {
    ops.construct(dst, n);
  } else {
    std::memset(dst, 0, size_t(n) * ops.elem_size);
  }
}

void copy_elems(const detail::dyn_array_ops& ops, void* dst, const void* src, uint32_t n)
{
  if (n == 0) {
    return;
  }
  if (ops.copy != nullptr) {
    ops.copy(dst, src, n);
  } else {
    std::memcpy(dst, src, size_t(n) * ops.elem_size);
  }
}

void relocate_elems(const detail::dyn_array_ops& ops, void* dst, void* src, uint32_t n)
{
  if (n == 0) {
    return;
  }
  if (ops.relocate != nullptr) {
    ops.relocate(dst, src, n);
  } else {
    std::memmove(dst, src, size_t(n) * ops.elem_size);
  }
}

void destroy_elems(const detail::dyn_array_ops& ops, void* ptr, uint32_t n)
{
  if (n != 0 and ops.destroy != nullptr) {
    ops.destroy(ptr, n);
  }
}

void* elem_at(const detail::dyn_array_ops& ops, void* data, uint32_t idx)
{
  return static_cast<uint8_t*>(data) + size_t(idx) * ops.elem_size;
}

} // namespace

void detail::dyn_array_base::reallocate(const dyn_array_ops& ops, uint32_t new_cap)
{
  void* new_data = new_cap > 0 ? ::operator new(size_t(new_cap) * ops.elem_size) : nullptr;
  relocate_elems(ops, new_data, data_, size_);
  ::operator delete(data_);
  data_ = new_data;
  cap_  = new_cap;
}

void detail::dyn_array_base::assign_impl(const dyn_array_ops& ops, const void* src, uint32_t nof_items)
{
  destroy_elems(ops, data_, size_);
  size_ = 0;
  if (nof_items > cap_) {
    reallocate(ops, nof_items);
  }
  copy_elems(ops, data_, src, nof_items);
  size_ = nof_items;
}

void detail::dyn_array_base::move_impl(const dyn_array_ops& ops, dyn_array_base& other)
{
  release_impl(ops);
  data_       = other.data_;
  size_       = other.size_;
  cap_        = other.cap_;
  other.data_ = nullptr;
  other.size_ = 0;
  other.cap_  = 0;
}

void detail::dyn_array_base::reserve_impl(const dyn_array_ops& ops, uint32_t new_cap)
{
  if (new_cap > cap_) {
    reallocate(ops, new_cap);
  }
}

void detail::dyn_array_base::resize_impl(const dyn_array_ops& ops, uint32_t new_size, uint32_t new_cap)
{
  if (new_size <= size_) {
    destroy_elems(ops, elem_at(ops, data_, new_size), size_ - new_size);
    size_ = new_size;
    return;
  }
  if (new_size > cap_) {
    reallocate(ops, std::max(new_size, new_cap));
  }
  construct_elems(ops, elem_at(ops, data_, size_), new_size - size_);
  size_ = new_size;
}

void detail::dyn_array_base::erase_impl(const dyn_array_ops& ops, uint32_t idx)
{
  destroy_elems(ops, elem_at(ops, data_, idx), 1);
  relocate_elems(ops, elem_at(ops, data_, idx), elem_at(ops, data_, idx + 1), size_ - idx - 1);
  size_--;
}

void detail::dyn_array_base::release_impl(const dyn_array_ops& ops)
{
  destroy_elems(ops, data_, size_);
  ::operator delete(data_);
  data_ = nullptr;
  size_ = 0;
  cap_  = 0;
}

} // namespace asn1
