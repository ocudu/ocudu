// Formatting library for C++ - dynamic argument lists
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_ARGS_H_
#define FMT_ARGS_H_

#ifndef FMT_MODULE
#  include <functional>  // std::reference_wrapper
#  include <memory>      // std::unique_ptr
#  include <new>
#  include <vector>
#endif

#include "format.h"  // std_string_view

FMT_BEGIN_NAMESPACE
namespace detail {

template <typename T> struct is_reference_wrapper : std::false_type {};
template <typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};

template <typename T> auto unwrap(const T& v) -> const T& { return v; }
template <typename T>
auto unwrap(const std::reference_wrapper<T>& v) -> const T& {
  return static_cast<const T&>(v);
}

// node is defined outside dynamic_arg_list to workaround a C2504 bug in MSVC
// 2022 (v17.10.0).
//
// Workaround for clang's -Wweak-vtables. Unlike for regular classes, for
// templates it doesn't complain about inability to deduce single translation
// unit for placing vtable. So node is made a fake template.
template <typename = void> struct node {
  virtual ~node() = default;
  std::unique_ptr<node<>> next;
};

class dynamic_arg_list {
  template <typename T> struct typed_node : node<> {
    T value;

    template <typename Arg>
    FMT_CONSTEXPR typed_node(const Arg& arg) : value(arg) {}

    template <typename Char>
    FMT_CONSTEXPR typed_node(const basic_string_view<Char>& arg)
        : value(arg.data(), arg.size()) {}
  };

  std::unique_ptr<node<>> head_;

 public:
  template <typename T, typename Arg> auto push(const Arg& arg) -> const T& {
    auto new_node = std::unique_ptr<typed_node<T>>(new typed_node<T>(arg));
    auto& value = new_node->value;
    new_node->next = std::move(head_);
    head_ = std::move(new_node);
    return value;
  }
};
}  // namespace detail

/**
 * A dynamic list of formatting arguments with storage.
 *
 * It can be implicitly converted into `fmt::basic_format_args` for passing
 * into type-erased formatting functions such as `fmt::vformat`.
 */
template <typename Context> class dynamic_format_arg_store {
 private:
  static constexpr unsigned MAX_SSO_BUFFER_SIZE = 64;

  using char_type = typename Context::char_type;

  // A custom type small and modestly-aligned enough to live in the inline slot
  // storage. Destructibility is handled by a per-slot thunk, so a non-trivial
  // destructor is no longer disqualifying.
  template <typename T> struct fits_sso {
    enum {
      value = detail::mapped_type_constant<T, char_type>::value ==
                  detail::type::custom_type &&
              sizeof(T) <= MAX_SSO_BUFFER_SIZE &&
              alignof(T) <= alignof(std::max_align_t)
    };
  };

  template <typename T> struct need_copy {
    static constexpr detail::type mapped_type =
        detail::mapped_type_constant<T, char_type>::value;

    enum {
      value = !(detail::is_reference_wrapper<T>::value ||
                std::is_same<T, basic_string_view<char_type>>::value ||
                std::is_same<T, detail::std_string_view<char_type>>::value ||
                fits_sso<T>::value ||
                (mapped_type != detail::type::cstring_type &&
                 mapped_type != detail::type::string_type &&
                 mapped_type != detail::type::custom_type))
    };
  };

  template <typename T>
  using stored_t = conditional_t<
      std::is_convertible<T, std::basic_string<char_type>>::value &&
          !detail::is_reference_wrapper<T>::value,
      std::basic_string<char_type>, T>;

  // Non-relocating inline storage for small custom-type arguments. Each slot
  // carries an optional destructor thunk, so stored objects need not be
  // trivially destructible. Slots are never relocated: capacity is fixed by
  // reserve() and push_back() falls back to dynamic_args_ once it is reached,
  // which is what keeps the pointers held in data_ valid.
  class sso_storage {
    using dtor_fn = void (*)(void*);
    using slot_t = std::aligned_storage_t<MAX_SSO_BUFFER_SIZE>;

    std::vector<slot_t> slots_;
    std::vector<dtor_fn> dtors_;

    void run_dtors() {
      for (size_t i = dtors_.size(); i-- != 0;)
        if (dtors_[i]) dtors_[i](static_cast<void*>(&slots_[i]));
    }

   public:
    sso_storage() = default;
    sso_storage(const sso_storage&) = delete;
    auto operator=(const sso_storage&) -> sso_storage& = delete;

    sso_storage(sso_storage&& other) noexcept
        : slots_(std::move(other.slots_)), dtors_(std::move(other.dtors_)) {
      other.slots_.clear();
      other.dtors_.clear();
    }

    auto operator=(sso_storage&& other) noexcept -> sso_storage& {
      if (this != &other) {
        run_dtors();
        slots_ = std::move(other.slots_);
        dtors_ = std::move(other.dtors_);
        other.slots_.clear();
        other.dtors_.clear();
      }
      return *this;
    }

    ~sso_storage() { run_dtors(); }

    auto size() const -> size_t { return dtors_.size(); }
    auto full() const -> bool { return dtors_.size() >= slots_.capacity(); }

    void reserve(size_t n) {
      slots_.reserve(n);
      dtors_.reserve(n);
    }

    void clear() {
      run_dtors();
      dtors_.clear();
      slots_.clear();
    }

    // Copy-constructs arg into a stable slot and returns a reference to it.
    // Caller must have checked !full().
    template <typename T> auto emplace(const T& arg) -> const T& {
      // Null thunk goes in first: a throwing constructor must not leave a live
      // thunk pointing at uninitialized bytes.
      dtors_.push_back(nullptr);
      slots_.emplace_back();
      T* obj = ::new (static_cast<void*>(&slots_.back())) T(arg);
      if (!std::is_trivially_destructible<T>::value)
        dtors_.back() = +[](void* p) { static_cast<T*>(p)->~T(); };
      return *obj;
    }
  };

  // Storage of basic_format_arg must be contiguous.
  std::vector<basic_format_arg<Context>> data_;
  std::vector<detail::named_arg_info<char_type>> named_info_;
  sso_storage sso_buffer;
  static constexpr unsigned MAX_POOL_STRING_SIZE = 64;
  unsigned free_string_pool_pos = 0;
  std::vector<std::string> string_pool;

  // Storage of arguments not fitting into basic_format_arg must grow
  // without relocation because items in data_ refer to it.
  detail::dynamic_arg_list dynamic_args_;

  friend class basic_format_args<Context>;

  auto data() const -> const basic_format_arg<Context>* {
    return named_info_.empty() ? data_.data() : data_.data() + 1;
  }

  template <typename T> void emplace_arg(const T& arg) {
    data_.emplace_back(arg);
  }

  template <typename T> void emplace_arg_sso(const T& arg) {
    data_.emplace_back(sso_buffer.emplace(arg));
  }

  template <typename T> void emplace_short_string(const T& arg) {
    auto& str = string_pool[free_string_pool_pos++];
    str = arg;
    data_.emplace_back(str.c_str());
  }

  template <typename T>
  void emplace_arg(const detail::named_arg<char_type, T>& arg) {
    if (named_info_.empty())
      data_.insert(data_.begin(), basic_format_arg<Context>(nullptr, 0));
    data_.emplace_back(detail::unwrap(arg.value));
    auto pop_one = [](std::vector<basic_format_arg<Context>>* data) {
      data->pop_back();
    };
    std::unique_ptr<std::vector<basic_format_arg<Context>>, decltype(pop_one)>
        guard{&data_, pop_one};
    named_info_.push_back({arg.name, static_cast<int>(data_.size() - 2u)});
    data_[0] = {named_info_.data(), named_info_.size()};
    guard.release();
  }

 public:
  constexpr dynamic_format_arg_store() = default;

  operator basic_format_args<Context>() const {
    return basic_format_args<Context>(data(), static_cast<int>(data_.size()),
                                      !named_info_.empty());
  }

  /**
   * Adds an argument into the dynamic store for later passing to a formatting
   * function.
   *
   * Note that custom types and string types (but not string views) are copied
   * into the store dynamically allocating memory if necessary.
   *
   * **Example**:
   *
   *     fmt::dynamic_format_arg_store<fmt::format_context> store;
   *     store.push_back(42);
   *     store.push_back("abc");
   *     store.push_back(1.5f);
   *     std::string result = fmt::vformat("{} and {} and {}", store);
   */
  template <typename T> void push_back(const T& arg) {
    if constexpr (detail::const_check(need_copy<T>::value)) {
      if constexpr (detail::mapped_type_constant<T, char_type>::value == detail::type::cstring_type) {
        if (free_string_pool_pos < string_pool.size() && std::strlen(arg) <= MAX_POOL_STRING_SIZE)
          emplace_short_string(detail::unwrap(arg));
        else
          emplace_arg(dynamic_args_.push<stored_t<T>>(arg));
      }
      else if constexpr (detail::mapped_type_constant<T, char_type>::value == detail::type::string_type) {
        if (free_string_pool_pos < string_pool.size() && arg.size() <= MAX_POOL_STRING_SIZE)
          emplace_short_string(detail::unwrap(arg));
        else
          emplace_arg(dynamic_args_.push<stored_t<T>>(arg));
      }
      else
        emplace_arg(dynamic_args_.push<stored_t<T>>(arg));
    }
    else if constexpr (detail::const_check(fits_sso<T>::value)) {
      if (!sso_buffer.full())
        emplace_arg_sso(detail::unwrap(arg));
      else
        emplace_arg(dynamic_args_.push<stored_t<T>>(arg));
    }
    else
      emplace_arg(detail::unwrap(arg));
  }

  /**
   * Adds a reference to the argument into the dynamic store for later passing
   * to a formatting function.
   *
   * **Example**:
   *
   *     fmt::dynamic_format_arg_store<fmt::format_context> store;
   *     char band[] = "Rolling Stones";
   *     store.push_back(std::cref(band));
   *     band[9] = 'c'; // Changing str affects the output.
   *     std::string result = fmt::vformat("{}", store);
   *     // result == "Rolling Scones"
   */
  template <typename T> void push_back(std::reference_wrapper<T> arg) {
    static_assert(
        need_copy<T>::value || fits_sso<T>::value,
        "objects of built-in types and string views are always copied");
    emplace_arg(arg.get());
  }

  /**
   * Adds named argument into the dynamic store for later passing to a
   * formatting function. `std::reference_wrapper` is supported to avoid
   * copying of the argument. The name is always copied into the store.
   */
  template <typename T>
  void push_back(const detail::named_arg<char_type, T>& arg) {
    const char_type* arg_name =
        dynamic_args_.push<std::basic_string<char_type>>(arg.name).c_str();
    if (detail::const_check(need_copy<T>::value)) {
      emplace_arg(
          fmt::arg(arg_name, dynamic_args_.push<stored_t<T>>(arg.value)));
    } else {
      emplace_arg(fmt::arg(arg_name, arg.value));
    }
  }

  /// Erase all elements from the store.
  void clear() {
    data_.clear();
    sso_buffer.clear();
    free_string_pool_pos = 0;
    named_info_.clear();
    dynamic_args_ = {};
  }

  /// Reserves space to store at least `new_cap` arguments including
  /// `new_cap_named` named arguments.
  void reserve(size_t new_cap, size_t new_cap_named) {
    FMT_ASSERT(new_cap >= new_cap_named,
               "set of arguments includes set of named arguments");
    data_.reserve(new_cap);
    sso_buffer.reserve(new_cap);
    named_info_.reserve(new_cap_named);
    string_pool.resize(new_cap);
    for (auto &elem: string_pool) {
      elem.reserve(MAX_POOL_STRING_SIZE);
    }
  }

  /// Returns the number of elements in the store.
  size_t size() const noexcept { return data_.size(); }
};

FMT_END_NAMESPACE

#endif  // FMT_ARGS_H_
