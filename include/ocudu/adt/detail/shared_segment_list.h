// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/detail/byte_buffer_memory_resource.h"
#include "ocudu/adt/detail/intrusive_ptr.h"
#include "ocudu/adt/expected.h"
#include "ocudu/adt/intrusive_list.h"
#include "ocudu/adt/span.h"
#include "ocudu/support/memory_pool/linear_memory_allocator.h"
#include <cstdlib>
#include <iterator>

namespace ocudu::detail {

/// \brief Ref-counted, owning list of segments backed by a \c byte_buffer_memory_resource. Each segment contains
/// an object T to which the payload of the remaining of the segment is passed.
///
/// Copyable (shallow, ref-counted) and movable.
template <typename T>
class shared_segment_list
{
  /// \brief Intrusive singly-linked list node carrying a raw memory payload with an object T new-emplaced in it.
  ///
  /// The payload span covers the user-accessible bytes of the pool allocation belonging to this node.
  /// For the node co-located with the control block, the payload starts after the control block;
  /// for all other nodes it starts immediately after the node header.
  struct node_t : intrusive_forward_list_element<> {
    T obj;

    node_t(span<uint8_t> payload_) noexcept : obj(payload_) {}
  };

  /// Non-owning, intrusive singly-linked list of \c node_t objects.
  using list_t = intrusive_forward_list<node_t>;

  struct control_block {
    /// Intrusive linked-list of segments (including the cb_node as the first element).
    list_t segments;
    /// Reference count for this control block.
    intrusive_ptr_atomic_ref_counter ref_count;
    /// Pool used to allocate/deallocate segments.
    byte_buffer_memory_resource* pool = nullptr;

    ~control_block() = default;

    /// Returns the segment node that is co-located with this control block.
    /// Layout is [ node | control_block ], so the node is sizeof(node_t) before this.
    node_t* node_in_same_block() const
    {
      return reinterpret_cast<node_t*>(reinterpret_cast<char*>(const_cast<control_block*>(this)) - sizeof(node_t));
    }

    void destroy_cb()
    {
      byte_buffer_memory_resource* p       = pool;
      node_t*                      cb_node = node_in_same_block();

      // Free all segment nodes except the CB node (it shares a pool block with the CB itself).
      node_t* node = segments.empty() ? nullptr : &segments.front();
      while (node != nullptr) {
        auto* next = static_cast<node_t*>(node->next_node);
        if (node != cb_node) {
          node->~node_t();
          p->deallocate(node);
        }
        node = next;
      }

      // Destroy the control block in-place, then release the pool block that holds it.
      cb_node->~node_t();
      this->~control_block();
      p->deallocate(cb_node);
    }

  private:
    friend void intrusive_ptr_inc_ref(control_block* p) { p->ref_count.inc_ref(); }
    friend void intrusive_ptr_dec_ref(control_block* p)
    {
      if (p->ref_count.dec_ref()) {
        p->destroy_cb();
      }
    }
    friend bool intrusive_ptr_is_unique(control_block* p) { return p->ref_count.unique(); }
  };

  /// Malloc-backed memory resource used when no pool is provided.
  struct heap_byte_buffer_memory_resource final : public byte_buffer_memory_resource {
  private:
    span<uint8_t> do_allocate(size_t size_hint, size_t /*alignment*/) override
    {
      void* p = std::malloc(size_hint);
      if (p == nullptr) {
        return {};
      }
      return {static_cast<uint8_t*>(p), size_hint};
    }

    void do_deallocate(void* p) override { std::free(p); }

    bool do_is_equal(const byte_buffer_memory_resource& other) const noexcept override { return this == &other; }
  };

  /// Heap-based memory pool.
  inline static heap_byte_buffer_memory_resource heap_pool;

  template <bool IsConst>
  struct iter_impl {
    using node_ptr  = std::conditional_t<IsConst, const node_t*, node_t*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;

    using iterator_category = std::forward_iterator_tag;
    using value_type        = std::remove_reference_t<reference>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void;

    node_ptr current;

    reference  operator*() const { return current->obj; }
    iter_impl& operator++()
    {
      current = static_cast<node_ptr>(current->next_node);
      return *this;
    }
    bool operator==(const iter_impl& other) const { return current == other.current; }
    bool operator!=(const iter_impl& other) const { return current != other.current; }
  };

public:
  using iterator       = iter_impl<false>;
  using const_iterator = iter_impl<true>;

  /// Creates a segment list that uses a malloc-backed pool on first append_segment() call.
  shared_segment_list() noexcept = default;

  /// Create a shared_segment_list with the specified pool and the provided segment size hint.
  static expected<shared_segment_list> make(size_t seg_size_hint, byte_buffer_memory_resource& p) noexcept
  {
    expected<shared_segment_list> result;
    result.emplace();
    if (OCUDU_UNLIKELY(not result->init_ctrl_block(seg_size_hint, p))) {
      result->clear();
    }
    return result;
  }

  /// Returns true if no segments have been appended.
  bool empty() const noexcept { return ctrl_blk_ptr == nullptr || ctrl_blk_ptr->segments.empty(); }

  iterator begin() noexcept { return {empty() ? nullptr : &ctrl_blk_ptr->segments.front()}; }
  iterator end() noexcept { return {nullptr}; }

  const_iterator begin() const noexcept { return {empty() ? nullptr : &ctrl_blk_ptr->segments.front()}; }
  const_iterator end() const noexcept { return {nullptr}; }

  /// Total number of segments (O(n)).
  size_t length() const noexcept
  {
    size_t count = 0;
    for (auto it = begin(); it != end(); ++it) {
      ++count;
    }
    return count;
  }

  /// Returns true if this is the last owner of the shared segment list.
  bool unique() const noexcept { return ctrl_blk_ptr == nullptr || ctrl_blk_ptr.unique(); }

  /// Allocates a new segment and appends it to the list.
  /// \param size_hint Desired payload size; actual size depends on the pool block size.
  /// \returns True on success, false on allocation failure.
  [[nodiscard]] bool append_segment(size_t size_hint)
  {
    if (ctrl_blk_ptr == nullptr) {
      return init_ctrl_block(size_hint + sizeof(control_block), heap_pool);
    }
    node_t* node = create_segment(size_hint);
    if (node == nullptr) {
      return false;
    }
    ctrl_blk_ptr->segments.push_back(node);
    return true;
  }

  /// Returns a pointer to the T object of the first segment, or nullptr if empty().
  T*       front() noexcept { return empty() ? nullptr : &ctrl_blk_ptr->segments.front().obj; }
  const T* front() const noexcept { return empty() ? nullptr : &ctrl_blk_ptr->segments.front().obj; }

  /// Returns a pointer to the T object of the last segment, or nullptr if empty().
  T*       tail() noexcept { return empty() ? nullptr : &ctrl_blk_ptr->segments.back().obj; }
  const T* tail() const noexcept { return empty() ? nullptr : &ctrl_blk_ptr->segments.back().obj; }

  /// Releases the control block (decrements ref count; frees all nodes when it reaches zero).
  void clear() { ctrl_blk_ptr.reset(); }

private:
  /// Allocates the first segment, co-locating the control block inside its pool block.
  /// The node's payload is set to the bytes that follow the control block.
  [[nodiscard]] bool init_ctrl_block(size_t size_hint, byte_buffer_memory_resource& p)
  {
    span<uint8_t> mem_block = p.allocate(sizeof(node_t) + sizeof(control_block) + size_hint);
    if (mem_block.empty()) {
      return false;
    }
    if (mem_block.size() < sizeof(node_t) + sizeof(control_block)) {
      p.deallocate(mem_block.data());
      return false;
    }

    linear_memory_allocator arena{mem_block.data(), mem_block.size()};

    // Allocate space for node.
    void* node_region = arena.allocate(sizeof(node_t), alignof(node_t));

    // Allocate control block and initialize it.
    void* cb_region    = arena.allocate(sizeof(control_block), alignof(control_block));
    ctrl_blk_ptr       = new (cb_region) control_block{};
    ctrl_blk_ptr->pool = &p;

    // Init node with payload starting after control block.
    auto* node = new (node_region)
        node_t{span<uint8_t>{reinterpret_cast<uint8_t*>(ctrl_blk_ptr.get() + 1), arena.nof_bytes_left()}};

    // Add node to segments list.
    ctrl_blk_ptr->segments.push_back(node);
    return true;
  }

  /// Allocates a new segment from the pool stored in the control block.
  [[nodiscard]] node_t* create_segment(size_t size_hint)
  {
    span<uint8_t> mem_block = ctrl_blk_ptr->pool->allocate(sizeof(node_t) + size_hint);
    if (mem_block.empty()) {
      return nullptr;
    }
    if (mem_block.size() < sizeof(node_t) + 1) {
      ctrl_blk_ptr->pool->deallocate(mem_block.data());
      return nullptr;
    }

    linear_memory_allocator arena{mem_block.data(), mem_block.size()};

    /// Allocate node in place.
    void* node_region = arena.allocate(sizeof(node_t), alignof(node_t));
    auto* node        = new (node_region) node_t{
        span<uint8_t>{reinterpret_cast<uint8_t*>(reinterpret_cast<node_t*>(node_region) + 1), arena.nof_bytes_left()}};
    return node;
  }

  /// Pointer to control block where the linked-list of segments is stored.
  intrusive_ptr<control_block> ctrl_blk_ptr;
};

} // namespace ocudu::detail
