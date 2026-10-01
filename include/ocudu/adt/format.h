// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/log_channel.h"
#include "ocudu/support/math/bit_ops.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include <complex>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace ocudu {

class bit_buffer;

class byte_buffer;
class byte_buffer_slice;
class byte_buffer_view;

class byte_buffer_chain;

struct cbf16_t;
std::complex<float> to_cf(cbf16_t value);

template <typename Integer, Integer MIN_VALUE, Integer MAX_VALUE>
class bounded_integer;

template <size_t N, bool LowestInfoBitIsMSB, typename Tag>
class bounded_bitset;

template <size_t N, bool LowestInfoBitIsMSB>
class fixed_size_bitset;

namespace detail {

/// Shared implementation of \c to_hex_string for the ocudu bitset types.
template <typename Bitset, typename OutputIt>
OutputIt bitset_to_hex_string(const Bitset& s, OutputIt&& mem_buffer, bool reverse)
{
  const size_t sz = s.size();
  if (sz == 0) {
    return mem_buffer;
  }
  constexpr bool   lowest_info_bit_is_msb = Bitset::bit_order();
  constexpr size_t bits_per_word          = Bitset::bits_per_word;
  const size_t     rem_bits               = sz % bits_per_word;
  const size_t     rem_digits             = (rem_bits + 3U) / 4U;
  const size_t     nwords                 = (sz + bits_per_word - 1) / bits_per_word;

  const uint64_t* words = s.data();

  if (not reverse) {
    if constexpr (lowest_info_bit_is_msb) {
      unsigned i = 0;
      for (; i != nwords - 1; ++i) {
        fmt::format_to(mem_buffer, "{:0>16x}", words[i]);
      }
      uint64_t w = words[i] >> (bits_per_word - rem_bits);
      fmt::format_to(mem_buffer, "{:0>{}x}", w, rem_digits);
    } else {
      int i = nwords - 1;
      fmt::format_to(mem_buffer, "{:0>{}x}", words[i], rem_digits);
      // remaining words will occupy 16 hex digits each (4 bits per hex digit).
      for (--i; i >= 0; --i) {
        fmt::format_to(mem_buffer, "{:0>16x}", words[i]);
      }
    }
  } else {
    if constexpr (lowest_info_bit_is_msb) {
      // first, potentially incomplete, word
      int i = nwords - 1;
      fmt::format_to(mem_buffer, "{:0>{}x}", bit_reverse(words[i]), rem_digits);
      for (--i; i >= 0; --i) {
        fmt::format_to(mem_buffer, "{:0>16x}", bit_reverse(words[i]));
      }
    } else {
      unsigned i = 0;
      for (; i != nwords - 1; ++i) {
        fmt::format_to(mem_buffer, "{:0>16x}", bit_reverse(words[i]));
      }
      uint64_t w = bit_reverse(words[i]) >> (bits_per_word - rem_bits);
      fmt::format_to(mem_buffer, "{:0>{}x}", w, rem_digits);
    }
  }
  return mem_buffer;
}

/// Shared implementation of \c to_bin_string for the ocudu bitset types.
template <typename Bitset, typename OutputIt>
OutputIt bitset_to_bin_string(const Bitset& s, OutputIt&& mem_buffer, bool reverse)
{
  if (s.size() == 0) {
    return mem_buffer;
  }

  reverse = reverse ^ Bitset::bit_order();

  if (!reverse) {
    for (size_t i = s.size(); i != 0; --i) {
      fmt::format_to(mem_buffer, "{}", s.test(i - 1) ? '1' : '0');
    }
  } else {
    for (size_t i = 0; i != s.size(); ++i) {
      fmt::format_to(mem_buffer, "{}", s.test(i) ? '1' : '0');
    }
  }
  return mem_buffer;
}

} // namespace detail

/// \brief Converts the bounded_bitset to a string of hexadecimal digits.
/// \tparam OutputIt Output fmt memory buffer type.
/// \param[in] s Bitset to convert.
/// \param[out] mem_buffer Fmt memory buffer.
/// \param[in] reverse In which bit order to represent this bitset.
/// \return The memory buffer passed as argument.
template <size_t N, bool LowestInfoBitIsMSB, typename Tag, typename OutputIt>
OutputIt to_hex_string(const bounded_bitset<N, LowestInfoBitIsMSB, Tag>& s, OutputIt&& mem_buffer, bool reverse)
{
  return detail::bitset_to_hex_string(s, std::forward<OutputIt>(mem_buffer), reverse);
}

/// \brief Converts the bounded_bitset to a string of bits.
/// \tparam OutputIt Output fmt memory buffer type.
/// \param[in] s Bitset to convert.
/// \param[out] mem_buffer Fmt memory buffer.
/// \return The memory buffer passed as argument.
template <size_t N, bool LowestInfoBitIsMSB, typename Tag, typename OutputIt>
OutputIt to_bin_string(const bounded_bitset<N, LowestInfoBitIsMSB, Tag>& s, OutputIt&& mem_buffer, bool reverse)
{
  return detail::bitset_to_bin_string(s, std::forward<OutputIt>(mem_buffer), reverse);
}

/// \brief Converts the fixed_size_bitset to a string of hexadecimal digits.
/// \tparam OutputIt Output fmt memory buffer type.
/// \param[in] s Bitset to convert.
/// \param[out] mem_buffer Fmt memory buffer.
/// \param[in] reverse In which bit order to represent this bitset.
/// \return The memory buffer passed as argument.
template <size_t N, bool LowestInfoBitIsMSB, typename OutputIt>
OutputIt to_hex_string(const fixed_size_bitset<N, LowestInfoBitIsMSB>& s, OutputIt&& mem_buffer, bool reverse)
{
  return detail::bitset_to_hex_string(s, std::forward<OutputIt>(mem_buffer), reverse);
}

/// \brief Converts the fixed_size_bitset to a string of bits.
/// \tparam OutputIt Output fmt memory buffer type.
/// \param[in] s Bitset to convert.
/// \param[out] mem_buffer Fmt memory buffer.
/// \return The memory buffer passed as argument.
template <size_t N, bool LowestInfoBitIsMSB, typename OutputIt>
OutputIt to_bin_string(const fixed_size_bitset<N, LowestInfoBitIsMSB>& s, OutputIt&& mem_buffer, bool reverse)
{
  return detail::bitset_to_bin_string(s, std::forward<OutputIt>(mem_buffer), reverse);
}

namespace detail {

/// Shared implementation of the fmt formatters of the ocudu bitset types.
template <typename Bitset>
struct bitset_formatter {
  enum { hexadecimal, binary, bit_positions, intervals } mode = binary;
  enum { forward, reverse } order                             = forward;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'x') {
        mode = hexadecimal;
      }
      if (*it == 'r') {
        order = reverse;
      }
      if (*it == 'n') {
        mode = bit_positions;
      }
      if (*it == 'i') {
        mode = intervals;
      }
      ++it;
    }

    return it;
  }

  template <typename FormatContext>
  auto format(const Bitset& s, FormatContext& ctx) const
  {
    if (mode == hexadecimal) {
      return to_hex_string(s, ctx.out(), order == reverse);
    }

    if (mode == intervals) {
      bool first = true;
      fmt::format_to(ctx.out(), "{{");
      for_each_interval(s, [&first, &ctx](size_t start_interval, size_t end_interval) {
        // Append a comma if the interval is not the first.
        if (first) {
          first = false;
        } else {
          fmt::format_to(ctx.out(), ", ");
        }

        // Print interval if it is more than one bit, otherwise a single value.
        if (end_interval - start_interval > 1) {
          fmt::format_to(ctx.out(), "[{}, {})", start_interval, end_interval);
        } else {
          fmt::format_to(ctx.out(), "{}", start_interval);
        }
      });
      fmt::format_to(ctx.out(), "}}");
      return ctx.out();
    }

    if (mode == bit_positions) {
      if (s.empty()) {
        fmt::format_to(ctx.out(), "empty");
      } else if (s.count() == 0) {
        fmt::format_to(ctx.out(), "none");
      } else if (s.is_contiguous()) {
        unsigned lowest  = s.find_lowest();
        unsigned highest = s.find_highest();
        if (lowest == highest) {
          // Single value.
          fmt::format_to(ctx.out(), "{}", lowest);
        } else {
          // Format as a range.
          fmt::format_to(ctx.out(), "[{}, {})", lowest, highest + 1);
        }

      } else {
        // Format as a list of bit positions.
        fmt::format_to(ctx.out(), "{}", s.get_bit_positions());
      }
      return ctx.out();
    }

    return to_bin_string(s, ctx.out(), order == reverse);
  }
};

} // namespace detail

template <typename T>
class span;

template <typename T, std::size_t MAX_N>
class static_vector;

template <typename T, bool RightClosed, typename Tag>
class interval;

} // namespace ocudu

namespace fmt {

template <>
struct is_range<ocudu::byte_buffer_view, char> : std::false_type {};

/// \brief Custom formatter for byte_buffer_view.
template <>
struct formatter<ocudu::byte_buffer_view> {
  enum { hexadecimal, binary } mode = hexadecimal;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'b') {
        mode = binary;
      }
      ++it;
    }
    return it;
  }

  template <typename T, typename FormatContext>
  auto format(const T& buf, FormatContext& ctx) const
  {
    if (mode == hexadecimal) {
      return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
    }
    return format_to(ctx.out(), "{:0>8b}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

template <>
struct is_range<ocudu::byte_buffer, char> : std::false_type {};

/// \brief Custom formatter for byte_buffer.
template <>
struct formatter<ocudu::byte_buffer> {
  enum { hexadecimal, binary } mode = hexadecimal;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'b') {
        mode = binary;
      }
      ++it;
    }
    return it;
  }

  template <typename T, typename FormatContext>
  auto format(const T& buf, FormatContext& ctx) const
  {
    if (mode == hexadecimal) {
      return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
    }
    return format_to(ctx.out(), "{:0>8b}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

template <>
struct is_range<ocudu::byte_buffer_slice, char> : std::false_type {};

/// \brief Custom formatter for byte_buffer_slice.
template <>
struct formatter<ocudu::byte_buffer_slice> : public formatter<ocudu::byte_buffer_view> {
  template <typename T, typename FormatContext>
  auto format(const T& buf, FormatContext& ctx) const
  {
    return formatter<ocudu::byte_buffer_view>::format(buf.view(), ctx);
  }
};

template <>
struct is_range<ocudu::byte_buffer_chain, char> : std::false_type {};

/// \brief Custom formatter for byte_buffer_chain.
template <>
struct formatter<ocudu::byte_buffer_chain> : public formatter<ocudu::byte_buffer_view> {
  template <typename T, typename FormatContext>
  auto format(const T& buf, FormatContext& ctx) const
  {
    if (mode == hexadecimal) {
      return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
    }
    return format_to(ctx.out(), "{:0>8b}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

template <>
struct formatter<ocudu::bit_buffer> {
  enum { hexadecimal, binary } mode = binary;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'x') {
        mode = hexadecimal;
      }
      ++it;
    }

    return it;
  }

  template <typename T, typename FormatContext>
  auto format(const T& s, FormatContext& ctx) const
  {
    if (mode == hexadecimal) {
      return s.template to_hex_string<decltype(std::declval<FormatContext>().out())>(ctx.out());
    }
    return s.template to_bin_string<decltype(std::declval<FormatContext>().out())>(ctx.out());
  }
};

/// \brief Custom formatter for bounded_bitset<N, LowestInfoBitIsMSB, Tag>
template <size_t N, bool LowestInfoBitIsMSB, typename Tag>
struct formatter<ocudu::bounded_bitset<N, LowestInfoBitIsMSB, Tag>>
  : public ocudu::detail::bitset_formatter<ocudu::bounded_bitset<N, LowestInfoBitIsMSB, Tag>> {};

/// \brief Custom formatter for fixed_size_bitset<N, LowestInfoBitIsMSB>
template <size_t N, bool LowestInfoBitIsMSB>
struct formatter<ocudu::fixed_size_bitset<N, LowestInfoBitIsMSB>>
  : public ocudu::detail::bitset_formatter<ocudu::fixed_size_bitset<N, LowestInfoBitIsMSB>> {};

/// Formatter for bounded_integer<...> types.
template <typename Integer, Integer MIN_VALUE, Integer MAX_VALUE>
struct formatter<ocudu::bounded_integer<Integer, MIN_VALUE, MAX_VALUE>> : public formatter<Integer> {
  template <typename FormatContext>
  auto format(const ocudu::bounded_integer<Integer, MIN_VALUE, MAX_VALUE>& s, FormatContext& ctx) const
  {
    if (s.valid()) {
      return fmt::format_to(ctx.out(), "{}", static_cast<Integer>(s));
    }
    return fmt::format_to(ctx.out(), "INVALID");
  }
};

/// Format intervals with the notation [start, stop)
template <typename T, bool RightClosed, typename Tag>
struct formatter<ocudu::interval<T, RightClosed, Tag>> : public formatter<T> {
  template <typename FormatContext>
  auto format(const ocudu::interval<T, RightClosed, Tag>& interv, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "[{}{}{}{}",
                     interv.start(),
                     ocudu::interval<T, RightClosed, Tag>::is_real::value ? ", " : "..",
                     interv.stop(),
                     RightClosed ? ']' : ')');
  }
};

} // namespace fmt

namespace ocudulog {

/// Type trait specialization to instruct the logger to use a user defined copy implementation as it is unsafe to
/// directly copy the contents of a span.
template <typename T>
struct copy_loggable_type<ocudu::span<T>> {
  static constexpr bool is_copyable = false;

  static void copy(fmt::dynamic_format_arg_store<fmt::format_context>* store, ocudu::span<T> s)
  {
    static constexpr unsigned MAX_NOF_ELEMENTS = 128;
    if (s.size() < MAX_NOF_ELEMENTS) {
      store->push_back(ocudu::static_vector<typename std::remove_cv_t<T>, MAX_NOF_ELEMENTS>(s.begin(), s.end()));
    } else {
      store->push_back(std::vector<typename std::remove_cv_t<T>>(s.begin(), s.end()));
    }
  }
};

} // namespace ocudulog

namespace fmt {

/// FMT formatter shared by \c std::complex specializations.
template <typename ComplexType>
struct formatter_template {
  // Stores parsed format string.
  memory_buffer format_buffer;

  formatter_template()
  {
    static constexpr std::string_view DEFAULT_FORMAT =
        (std::is_same<ComplexType, std::complex<float>>::value) ? "{:+f}{:+f}j" : "{:+d}{:+d}j";
    format_buffer.append(DEFAULT_FORMAT.begin(), DEFAULT_FORMAT.end());
  }

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    static constexpr std::string_view PREAMBLE_FORMAT = "{:";

    // Skip if context is empty and use default format.
    if (ctx.begin() == ctx.end()) {
      return ctx.end();
    }

    // Store the format string.
    format_buffer.clear();
    format_buffer.append(PREAMBLE_FORMAT.begin(), PREAMBLE_FORMAT.end());
    for (auto& it : ctx) {
      format_buffer.push_back(it);

      // Found the end of the context.
      if (it == '}') {
        // Replicate the format string for the imaginary part.
        format_buffer.append(format_buffer.begin(), format_buffer.end());
        format_buffer.push_back('j');
        return &it;
      }
    }

    // No end of context was found.
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(ComplexType value, FormatContext& ctx) const
  {
    const string_view format_str = string_view(format_buffer.data(), format_buffer.size());
    return format_to(ctx.out(), format_str, value.real(), value.imag());
  }
};

template <>
struct formatter<std::complex<float>> : public formatter_template<std::complex<float>> {};
template <>
struct formatter<std::complex<int8_t>> : public formatter_template<std::complex<int8_t>> {};
template <>
struct formatter<std::complex<int16_t>> : public formatter_template<std::complex<int16_t>> {};

/// FMT formatter of cbf16_t type.
template <>
struct formatter<ocudu::cbf16_t> {
  formatter_template<std::complex<float>> cf_formatter;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return cf_formatter.parse(ctx);
  }

  template <typename T, typename FormatContext>
  auto format(const T& value, FormatContext& ctx) const
  {
    return cf_formatter.format(ocudu::to_cf(value), ctx);
  }
};

} // namespace fmt
