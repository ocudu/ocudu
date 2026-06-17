// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/detail/shared_segment_list.h"
#include "ocudu/adt/span.h"
#include "ocudu/support/detail/type_list.h"
#include "ocudu/support/ocudu_assert.h"
#include <array>
#include <cstring>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

namespace ocudu {

namespace detail {

/// \brief Aligns \c val up to the nearest multiple of \c alignment (must be a power of two).
constexpr size_t align_up_v(size_t val, size_t alignment) noexcept
{
  return (val + alignment - 1) & ~(alignment - 1);
}

/// \brief Visits a single stored element given its type index and the buffer position just past the type index.
///
/// Dispatches at runtime using a fold expression over the compile-time index pack. Returns the buffer position of
/// the first byte after the visited element, or \c ~size_t{0} if the type index is out of range.
template <bool IsConst, typename... Types>
struct type_list_dispatch {
  using byte_ptr = std::conditional_t<IsConst, const uint8_t, uint8_t>;

  /// Invoke visitor on the element whose type index equals \p type_idx.
  /// \p after_idx is the buffer offset right after the type-index bytes.
  /// Returns the offset of the first byte after the element (start of the next record).
  template <typename Visitor, size_t... Is>
  static size_t
  run(size_t type_idx, size_t after_idx, byte_ptr* buf, Visitor& v, std::index_sequence<Is...> /*seq*/) noexcept(
      std::conjunction_v<std::is_nothrow_invocable<Visitor&, type_list_helper::type_at_t<Is, type_list<Types...>>&>...>)
  {
    size_t next_pos = 0;
    // Fold over all indices; short-circuits once the matching index is found.
    const bool found = ((type_idx == Is ? (next_pos = invoke<Is>(after_idx, buf, v), true) : false) || ...);
    return found ? next_pos : ~size_t{0};
  }

  /// Relocates a single stored element from \p src_buf to \p dst_buf using move construction.
  /// \p pos is the start of the record (type-index bytes); \p after_idx is \p pos + sizeof(type_idx_t).
  /// Returns the offset of the first byte after the element.
  template <size_t... Is>
  static size_t relocate(size_t   type_idx,
                         size_t   pos,
                         size_t   after_idx,
                         uint8_t* src_buf,
                         uint8_t* dst_buf,
                         std::index_sequence<Is...> /*seq*/) noexcept
  {
    size_t     next_pos = 0;
    const bool found =
        ((type_idx == Is ? (next_pos = relocate_one<Is>(pos, after_idx, src_buf, dst_buf), true) : false) || ...);
    return found ? next_pos : ~size_t{0};
  }

private:
  template <size_t I>
  static size_t relocate_one(size_t pos, size_t after_idx, uint8_t* src_buf, uint8_t* dst_buf) noexcept
  {
    using T              = type_list_helper::type_at_t<I, type_list<Types...>>;
    const size_t obj_pos = align_up_v(after_idx, alignof(T));
    // Copy header bytes (type_idx + zero padding) verbatim.
    std::memcpy(dst_buf + pos, src_buf + pos, obj_pos - pos);
    // Move-construct at destination, destroy at source.
    T* src_obj = std::launder(reinterpret_cast<T*>(src_buf + obj_pos));
    new (dst_buf + obj_pos) T(std::move(*src_obj));
    src_obj->~T();
    return obj_pos + sizeof(T);
  }

  template <size_t I, typename Visitor>
  static size_t invoke(size_t after_idx, byte_ptr* buf, Visitor& v)
  {
    using T              = type_list_helper::type_at_t<I, type_list<Types...>>;
    using obj_ref        = std::conditional_t<IsConst, const T&, T&>;
    const size_t obj_pos = align_up_v(after_idx, alignof(T));
    v(static_cast<obj_ref>(*std::launder(reinterpret_cast<std::conditional_t<IsConst, const T, T>*>(buf + obj_pos))));
    return obj_pos + sizeof(T);
  }
};

// ---------------------------------------------------------------------------
// CRTP base
// ---------------------------------------------------------------------------

/// \brief CRTP base that implements all shared logic for heterogeneous type-list buffers.
///
/// Derived classes supply a small set of private hooks (accessed via CRTP) and must call
/// \c destroy_all() in their own destructor before any member is destroyed.
///
/// \tparam Derived  The concrete derived class.
/// \tparam Types    List of types that may be stored. All types must be destructible.
template <typename Derived, typename... Types>
class type_list_buffer_base
{
  static_assert(sizeof...(Types) > 0, "type_list_buffer requires at least one type");
  static_assert(sizeof...(Types) <= static_cast<size_t>(std::numeric_limits<uint16_t>::max()),
                "type_list_buffer: too many types (max 65535)");

protected:
  // Use the smallest unsigned integer type that can represent all type indices.
  using type_idx_t = std::
      conditional_t<(sizeof...(Types) <= static_cast<size_t>(std::numeric_limits<uint8_t>::max())), uint8_t, uint16_t>;

  template <typename T>
  static constexpr type_idx_t type_index_of_v = static_cast<type_idx_t>(type_list_helper::type_index_v<T, Types...>);

  using idx_seq        = std::make_index_sequence<sizeof...(Types)>;
  using const_dispatch = detail::type_list_dispatch<true, Types...>;
  using mut_dispatch   = detail::type_list_dispatch<false, Types...>;

public:
  ~type_list_buffer_base() = default;

  /// Returns the number of stored elements.
  size_t size() const noexcept { return nof_elements; }

  /// Returns \c true if the buffer holds no elements.
  bool empty() const noexcept { return nof_elements == 0; }

  /// Returns the number of bytes currently used by the backing buffer (includes type-index and padding bytes).
  size_t byte_size() const noexcept { return self().buf_size_impl(); }

  /// Calls \p v once for every stored element in insertion order (const overload).
  /// \p v must be callable with each type in \c Types (e.g. a generic lambda or a struct with overloads).
  template <typename Visitor>
  void for_each(Visitor&& visitor) const
  {
    const uint8_t* data = self().buf_data_impl();
    const size_t   sz   = self().buf_size_impl();
    size_t         pos  = 0;
    while (pos < sz) {
      type_idx_t idx;
      std::memcpy(&idx, data + pos, sizeof(type_idx_t));
      const size_t after_idx = pos + sizeof(type_idx_t);

      const size_t next = const_dispatch::run(static_cast<size_t>(idx), after_idx, data, visitor, idx_seq{});
      ocudu_assert(next != ~size_t{0}, "Corrupt type_list_buffer: invalid type index {}", static_cast<unsigned>(idx));
      pos = next;
    }
  }

  /// Calls \p v once for every stored element in insertion order (mutable overload).
  template <typename Visitor>
  void for_each(Visitor&& visitor)
  {
    uint8_t*     data = self().buf_data_impl();
    const size_t sz   = self().buf_size_impl();
    size_t       pos  = 0;
    while (pos < sz) {
      type_idx_t idx;
      std::memcpy(&idx, data + pos, sizeof(type_idx_t));
      const size_t after_idx = pos + sizeof(type_idx_t);

      const size_t next = mut_dispatch::run(static_cast<size_t>(idx), after_idx, data, visitor, idx_seq{});
      ocudu_assert(next != ~size_t{0}, "Corrupt type_list_buffer: invalid type index {}", static_cast<unsigned>(idx));
      pos = next;
    }
  }

  /// Destroys all stored elements and resets the buffer to an empty state.
  void clear() noexcept
  {
    destroy_all();
    self().reset_buf_impl();
    nof_elements = 0;
  }

protected:
  // CRTP access to derived class.
  Derived&       self() noexcept { return static_cast<Derived&>(*this); }
  const Derived& self() const noexcept { return static_cast<const Derived&>(*this); }

  /// Inserts a \c T constructed from \p args at the end of the buffer.
  /// Returns a pointer to the new element, or \c nullptr if the buffer has no space.
  template <typename T, typename... Args>
  T* base_emplace_impl(Args&&... args)
  {
    constexpr type_idx_t idx      = type_index_of_v<T>;
    const size_t         start    = self().buf_size_impl();
    const size_t         obj_pos  = detail::align_up_v(start + sizeof(type_idx_t), alignof(T));
    const size_t         new_size = obj_pos + sizeof(T);

    if (!self().ensure_space_impl(new_size)) {
      return nullptr;
    }

    uint8_t* data = self().buf_data_impl();
    std::memcpy(data + start, &idx, sizeof(type_idx_t));

    // Zero-fill alignment-padding bytes so the buffer has no uninitialized reads.
    if (const size_t pad = obj_pos - start - sizeof(type_idx_t); pad > 0) {
      std::memset(data + start + sizeof(type_idx_t), 0, pad);
    }

    T* obj = new (data + obj_pos) T(std::forward<Args>(args)...);
    ++nof_elements;
    return obj;
  }

  /// Calls the destructor of every stored object (no-op if all types are trivially destructible).
  void destroy_all() noexcept
  {
    if constexpr (!std::conjunction_v<std::is_trivially_destructible<Types>...>) {
      for_each([](auto& obj) {
        using T = std::decay_t<decltype(obj)>;
        obj.~T();
      });
    }
  }

  /// Number of objects currently stored in the buffer.
  size_t nof_elements = 0;
};

} // namespace detail

// ---------------------------------------------------------------------------
// dyn_type_list_buffer
// ---------------------------------------------------------------------------

/// \brief Heterogeneous container that stores a sequence of objects of a fixed type list in a packed memory buffer.
///
/// The backing buffer grows on demand. \c push / \c emplace return a reference to the newly stored element.
///
/// Unlike a \c std::vector<std::variant<Types...>>, each stored element occupies exactly
/// \c sizeof(type_index) + \c sizeof(T) bytes (plus at most \c alignof(T)-1 alignment-padding bytes), so smaller
/// types take up less space.
///
/// Layout of each record in the backing buffer:
/// \code
///   [ type_idx_t  idx ][ 0..N padding bytes ][ T  object ]
///                       <---alignof(T)------->
/// \endcode
///
/// \tparam Types List of types that may be stored. All types must be destructible.
template <typename... Types>
class dyn_type_list_buffer : private detail::type_list_buffer_base<dyn_type_list_buffer<Types...>, Types...>
{
  using base = detail::type_list_buffer_base<dyn_type_list_buffer<Types...>, Types...>;

public:
  // Inherited methods from the private base.
  using base::byte_size;
  using base::clear;
  using base::empty;
  using base::for_each;
  using base::size;

  dyn_type_list_buffer() = default;

  ~dyn_type_list_buffer() { this->destroy_all(); }

  dyn_type_list_buffer(const dyn_type_list_buffer& other)
  {
    if constexpr (std::conjunction_v<std::is_trivially_copyable<Types>...>) {
      buf                = other.buf;
      this->nof_elements = other.nof_elements;
    } else {
      buf.reserve(other.buf.size());
      other.for_each([this](const auto& obj) { this->push(obj); });
    }
  }

  dyn_type_list_buffer& operator=(const dyn_type_list_buffer& other)
  {
    if (this != &other) {
      this->clear();
      if constexpr (std::conjunction_v<std::is_trivially_copyable<Types>...>) {
        buf                = other.buf;
        this->nof_elements = other.nof_elements;
      } else {
        buf.reserve(other.buf.size());
        other.for_each([this](const auto& obj) { this->push(obj); });
      }
    }
    return *this;
  }

  dyn_type_list_buffer(dyn_type_list_buffer&& other) noexcept : buf(std::move(other.buf))
  {
    this->nof_elements = std::exchange(other.nof_elements, 0);
  }

  dyn_type_list_buffer& operator=(dyn_type_list_buffer&& other) noexcept
  {
    if (this != &other) {
      this->destroy_all();
      buf                = std::move(other.buf);
      this->nof_elements = std::exchange(other.nof_elements, 0);
    }
    return *this;
  }

  /// Hint the backing buffer to reserve at least \p bytes of storage.
  void reserve(size_t bytes)
  {
    if (bytes > buf.capacity()) {
      grow_buffer(bytes);
    }
  }

  /// Appends a copy or move of \p val to the buffer.
  /// \tparam T must appear in \c Types.
  /// \returns A reference to the newly stored element.
  template <typename T>
  std::decay_t<T>& push(T&& val)
  {
    using U = std::decay_t<T>;
    static_assert(type_list_helper::contains_v<U, Types...>,
                  "push(): T is not present in this dyn_type_list_buffer's type list");
    return emplace<U>(std::forward<T>(val));
  }

  /// Constructs a \c T in-place at the end of the buffer.
  /// \tparam T must appear in \c Types.
  /// \returns A reference to the newly constructed element.
  template <typename T, typename... Args>
  T& emplace(Args&&... args)
  {
    static_assert(type_list_helper::contains_v<T, Types...>,
                  "emplace(): T is not present in this dyn_type_list_buffer's type list");
    T* obj = this->template base_emplace_impl<T>(std::forward<Args>(args)...);
    ocudu_assert(obj != nullptr, "dyn_type_list_buffer::emplace returned nullptr");
    return *obj;
  }

private:
  friend base;

  // CRTP hooks used by base class.
  uint8_t*       buf_data_impl() noexcept { return buf.data(); }
  const uint8_t* buf_data_impl() const noexcept { return buf.data(); }
  size_t         buf_size_impl() const noexcept { return buf.size(); }
  void           reset_buf_impl() noexcept { buf.clear(); }

  // Resize buffer if needed.
  bool ensure_space_impl(size_t new_size)
  {
    if (new_size > buf.capacity()) {
      grow_buffer(new_size);
    }
    buf.resize(new_size);
    return true;
  }

  /// Grows the backing buffer to at least \p required_capacity bytes, relocating live objects
  /// via their move constructors when any stored type is not trivially copyable.
  void grow_buffer(size_t required_capacity)
  {
    const size_t new_cap = std::max(required_capacity, buf.capacity() == 0 ? required_capacity : buf.capacity() * 2);

    if constexpr (std::conjunction_v<std::is_trivially_copyable<Types>...>) {
      // In case of trivially copyable types, relocate is trivial as well. We can just rely on the byte-wise copy
      // of the std::vector when its buffer grows.
      buf.reserve(new_cap);
      return;
    }

    // Create a new buffer with the required capacity and size of the current buffer.
    std::vector<uint8_t> new_buf;
    new_buf.reserve(new_cap);
    new_buf.resize(buf.size());

    // Objects will be relocated from the old to the new buffer at the same offsets.
    size_t pos = 0;
    while (pos < buf.size()) {
      // Fetch type index.
      typename base::type_idx_t idx;
      std::memcpy(&idx, buf.data() + pos, sizeof(typename base::type_idx_t));
      const size_t after_idx = pos + sizeof(typename base::type_idx_t);

      // Relocate.
      pos = base::mut_dispatch::relocate(
          static_cast<size_t>(idx), pos, after_idx, buf.data(), new_buf.data(), typename base::idx_seq{});
      ocudu_assert(pos != ~size_t{0}, "Corrupt dyn_type_list_buffer during grow_buffer");
    }

    // Old buffer is freed here.
    buf = std::move(new_buf);
  }

  /// Memory buffer for stored objects.
  std::vector<uint8_t> buf;
};

// ---------------------------------------------------------------------------
// static_type_list_buffer
// ---------------------------------------------------------------------------

/// \brief Heterogeneous container that stores a sequence of objects of a fixed type list in a compile-time-sized
/// inline buffer.
///
/// The backing buffer is a \c std::array<uint8_t, N> held directly inside the object (no heap allocation).
/// \c push / \c emplace return a pointer to the newly stored element, or \c nullptr if the buffer is full.
///
/// \tparam N     Capacity of the backing buffer in bytes.
/// \tparam Types List of types that may be stored. All types must be destructible.
template <size_t N, typename... Types>
class static_type_list_buffer : private detail::type_list_buffer_base<static_type_list_buffer<N, Types...>, Types...>
{
  using base = detail::type_list_buffer_base<static_type_list_buffer<N, Types...>, Types...>;

public:
  // Inherited methods from the private base.
  using base::byte_size;
  using base::clear;
  using base::empty;
  using base::for_each;
  using base::size;

  static_type_list_buffer() = default;

  ~static_type_list_buffer() { this->destroy_all(); }

  // Move-only: copying a fixed buffer with potentially non-trivial objects is not supported.
  static_type_list_buffer(const static_type_list_buffer&)            = delete;
  static_type_list_buffer& operator=(const static_type_list_buffer&) = delete;

  static_type_list_buffer(static_type_list_buffer&& other) noexcept : used(std::exchange(other.used, 0))
  {
    this->nof_elements = std::exchange(other.nof_elements, 0);
    relocate_from(other.storage.data(), storage.data(), used);
  }

  static_type_list_buffer& operator=(static_type_list_buffer&& other) noexcept
  {
    if (this != &other) {
      this->destroy_all();
      used               = std::exchange(other.used, 0);
      this->nof_elements = std::exchange(other.nof_elements, 0);
      relocate_from(other.storage.data(), storage.data(), used);
    }
    return *this;
  }

  /// Returns the capacity in bytes of the backing buffer.
  static constexpr size_t capacity_bytes() noexcept { return N; }

  /// Appends a copy or move of \p val to the buffer.
  /// \tparam T must appear in \c Types.
  /// \returns A pointer to the newly stored element, or \c nullptr if the buffer is full.
  template <typename T>
  [[nodiscard]] std::decay_t<T>* push(T&& val)
  {
    using U = std::decay_t<T>;
    static_assert(type_list_helper::contains_v<U, Types...>,
                  "push(): T is not present in this static_type_list_buffer's type list");
    return emplace<U>(std::forward<T>(val));
  }

  /// Constructs a \c T in-place at the end of the buffer.
  /// \tparam T must appear in \c Types.
  /// \returns A pointer to the newly constructed element, or \c nullptr if the buffer is full.
  template <typename T, typename... Args>
  [[nodiscard]] T* emplace(Args&&... args)
  {
    static_assert(type_list_helper::contains_v<T, Types...>,
                  "emplace(): T is not present in this static_type_list_buffer's type list");
    return this->template base_emplace_impl<T>(std::forward<Args>(args)...);
  }

private:
  friend base;

  // CRTP hooks.
  uint8_t*       buf_data_impl() noexcept { return storage.data(); }
  const uint8_t* buf_data_impl() const noexcept { return storage.data(); }
  size_t         buf_size_impl() const noexcept { return used; }
  void           reset_buf_impl() noexcept { used = 0; }

  bool ensure_space_impl(size_t new_size) noexcept
  {
    if (new_size > N) {
      return false;
    }
    used = new_size;
    return true;
  }

  /// Move-constructs each live object from \p src into \p dst at the same offsets, then destroys
  /// the source objects. For trivially copyable types this is a plain byte copy.
  static void relocate_from(uint8_t* src, uint8_t* dst, size_t nbytes) noexcept
  {
    if constexpr (std::conjunction_v<std::is_trivially_copyable<Types>...>) {
      std::memcpy(dst, src, nbytes);
    } else {
      size_t pos = 0;
      while (pos < nbytes) {
        typename base::type_idx_t idx;
        std::memcpy(&idx, src + pos, sizeof(typename base::type_idx_t));
        const size_t after_idx = pos + sizeof(typename base::type_idx_t);
        pos =
            base::mut_dispatch::relocate(static_cast<size_t>(idx), pos, after_idx, src, dst, typename base::idx_seq{});
        ocudu_assert(pos != ~size_t{0}, "Corrupt static_type_list_buffer during move");
      }
    }
  }

  std::array<uint8_t, N> storage;
  size_t                 used = 0;
};

// ---------------------------------------------------------------------------
// type_list_buffer_view
// ---------------------------------------------------------------------------

/// \brief Non-owning heterogeneous container that stores a sequence of objects of a fixed type list in a caller-
/// supplied byte buffer.
///
/// The backing storage is a \c span<uint8_t> provided at construction time. This class does not allocate or free any
/// memory; the caller is responsible for the lifetime of the underlying buffer. \c push / \c emplace return a pointer
/// to the newly stored element, or \c nullptr if there is insufficient space remaining.
///
/// \tparam Types List of types that may be stored. All types must be destructible.
template <typename... Types>
class type_list_buffer_view : private detail::type_list_buffer_base<type_list_buffer_view<Types...>, Types...>
{
  using base = detail::type_list_buffer_base<type_list_buffer_view<Types...>, Types...>;

public:
  // Inherited methods from the private base.
  using base::byte_size;
  using base::clear;
  using base::empty;
  using base::for_each;
  using base::size;

  explicit type_list_buffer_view(span<uint8_t> storage) noexcept : buf(storage) {}

  ~type_list_buffer_view() { this->destroy_all(); }

  // Move-only: copying a view with potentially non-trivial objects is not supported.
  type_list_buffer_view(const type_list_buffer_view&)            = delete;
  type_list_buffer_view& operator=(const type_list_buffer_view&) = delete;

  type_list_buffer_view(type_list_buffer_view&& other) noexcept : buf(other.buf), used(std::exchange(other.used, 0))
  {
    this->nof_elements = std::exchange(other.nof_elements, 0);
  }

  type_list_buffer_view& operator=(type_list_buffer_view&& other) noexcept
  {
    if (this != &other) {
      this->destroy_all();
      buf                = other.buf;
      used               = std::exchange(other.used, 0);
      this->nof_elements = std::exchange(other.nof_elements, 0);
    }
    return *this;
  }

  /// Returns the capacity in bytes of the backing span.
  size_t capacity_bytes() const noexcept { return buf.size(); }

  /// Appends a copy or move of \p val to the buffer.
  /// \tparam T must appear in \c Types.
  /// \returns A pointer to the newly stored element, or \c nullptr if the buffer is full.
  template <typename T>
  [[nodiscard]] std::decay_t<T>* push(T&& val)
  {
    using U = std::decay_t<T>;
    static_assert(type_list_helper::contains_v<U, Types...>,
                  "push(): T is not present in this type_list_buffer_view's type list");
    return emplace<U>(std::forward<T>(val));
  }

  /// Constructs a \c T in-place at the end of the buffer.
  /// \tparam T must appear in \c Types.
  /// \returns A pointer to the newly constructed element, or \c nullptr if the buffer is full.
  template <typename T, typename... Args>
  [[nodiscard]] T* emplace(Args&&... args)
  {
    static_assert(type_list_helper::contains_v<T, Types...>,
                  "emplace(): T is not present in this type_list_buffer_view's type list");
    return this->template base_emplace_impl<T>(std::forward<Args>(args)...);
  }

private:
  friend base;

  // CRTP hooks.
  uint8_t*       buf_data_impl() noexcept { return buf.data(); }
  const uint8_t* buf_data_impl() const noexcept { return buf.data(); }
  size_t         buf_size_impl() const noexcept { return used; }
  void           reset_buf_impl() noexcept { used = 0; }

  bool ensure_space_impl(size_t new_size) noexcept
  {
    if (new_size > buf.size()) {
      return false;
    }
    used = new_size;
    return true;
  }

  span<uint8_t> buf;
  size_t        used = 0;
};

/// \brief Heterogeneous container that stores a sequence of objects of a fixed type list in a
/// \c detail::shared_segment_list<T>.
///
/// The buffer is organized as a linked list of segments. At the start of each segment a \c type_list_buffer_view is
/// placement-constructed; the rest of the segment's payload is the view's backing store. Records are appended through
/// the tail view. When it is full a fresh segment is allocated, a new view is placement-constructed there, and the
/// record goes into that segment instead.
///
/// \note For types with non-trivial destructors, a shallow copy must not outlive the copy that ultimately destroys
///       the buffer, since the destructor walks every segment and calls each view's destructor (which destroys the
///       stored objects in the shared memory).
///
/// \tparam Types List of types that may be stored. All types must be destructible.
template <typename... Types>
class type_list_buffer_stream
{
  using view_t = type_list_buffer_view<Types...>;

  /// Default segment size.
  static constexpr size_t segment_size = 2048;

public:
  type_list_buffer_stream() = default;

  /// Creates a type_list_buffer_stream with an eagerly allocated first segment from \p p.
  static expected<type_list_buffer_stream> make(byte_buffer_memory_resource& p) noexcept
  {
    auto seg_list = detail::shared_segment_list<view_t>::make(segment_size, p);
    if (not seg_list) {
      return make_unexpected(default_error_t{});
    }
    type_list_buffer_stream result;
    result.buf = std::move(*seg_list);
    return result;
  }
  type_list_buffer_stream(const type_list_buffer_stream& other) noexcept = default;
  type_list_buffer_stream& operator=(const type_list_buffer_stream& other) noexcept
  {
    if (this != &other) {
      buf = other.buf;
    }
    return *this;
  }

  type_list_buffer_stream(type_list_buffer_stream&& other) noexcept = default;

  type_list_buffer_stream& operator=(type_list_buffer_stream&& other) noexcept
  {
    if (this != &other) {
      buf = std::move(other.buf);
    }
    return *this;
  }

  /// Returns the total number of stored elements (O(number of segments)).
  size_t size() const noexcept
  {
    size_t total = 0;
    for (const view_t& seg : buf) {
      total += seg.size();
    }
    return total;
  }

  bool empty() const noexcept { return buf.empty() or buf.front()->empty(); }

  /// Returns the total number of record bytes used across all segments.
  size_t byte_size() const noexcept
  {
    size_t total = 0;
    for (const view_t& seg : buf) {
      total += seg.byte_size();
    }
    return total;
  }

  /// Calls \p visitor once for every stored element in insertion order (const overload).
  template <typename Visitor>
  void for_each(Visitor&& visitor) const
  {
    for (const view_t& seg : buf) {
      seg.for_each(visitor);
    }
  }

  /// Calls \p visitor once for every stored element in insertion order (mutable overload).
  template <typename Visitor>
  void for_each(Visitor&& visitor)
  {
    for (view_t& seg : buf) {
      seg.for_each(visitor);
    }
  }

  /// Destroys all stored elements and resets the buffer to an empty state.
  void clear() noexcept { buf.clear(); }

  /// Appends a copy or move of \p val to the buffer.
  /// \returns A pointer to the newly stored element, or \c nullptr on allocation failure.
  template <typename T>
  [[nodiscard]] std::decay_t<T>* push(T&& val)
  {
    using U = std::decay_t<T>;
    static_assert(type_list_helper::contains_v<U, Types...>,
                  "push(): T is not present in this type_list_buffer_stream's type list");
    return emplace<U>(std::forward<T>(val));
  }

  /// Constructs a \c T in-place at the end of the buffer.
  /// \returns A pointer to the newly constructed element, or \c nullptr on allocation failure.
  template <typename T, typename... Args>
  [[nodiscard]] T* emplace(Args&&... args)
  {
    static_assert(type_list_helper::contains_v<T, Types...>,
                  "emplace(): T is not present in this type_list_buffer_stream's type list");
    if (buf.empty() && !alloc_new_segment()) {
      return nullptr;
    }
    T* obj = buf.tail()->template emplace<T>(std::forward<Args>(args)...);
    if (obj != nullptr) {
      return obj;
    }
    // Current segment is full; allocate a new one and retry.
    if (!alloc_new_segment()) {
      return nullptr;
    }
    return buf.tail()->template emplace<T>(std::forward<Args>(args)...);
  }

private:
  /// Allocates a fresh segment, placement-constructs a \c view_t at its start, and appends it to \c buf.
  bool alloc_new_segment() noexcept { return buf.append_segment(segment_size); }

  detail::shared_segment_list<view_t> buf;
};

} // namespace ocudu
