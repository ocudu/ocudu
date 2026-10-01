// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/strong_type.h"
#include "fmt/format.h"
#include <chrono>
#include <climits>
#include <cstdint>

namespace ocudu {

namespace units {

namespace detail {
/// Tag struct used to uniquely identify the bit units type.
struct bit_tag {
  /// Text representation for the units.
  static const char* str() { return "bits"; }
};

/// Tag struct used to uniquely identify the byte units type.
struct byte_tag {
  /// Text representation for the units.
  static const char* str() { return "bytes"; }
};

/// Tag struct used to uniquely identify the bitrate type.
struct bitrate_tag {
  /// Text representation for the units.
  static const char* str() { return "bps"; }
};

/// Tag struct used to uniquely identify the byterate type.
struct byterate_tag {
  /// Text representation for the units.
  static const char* str() { return "Bps"; }
};
} // namespace detail

class bytes;

/// \brief Abstraction of bit as a unit of digital information.
///
/// An object of this type will represent an amount of digital information expressed in bits.
class bits : public strong_type<unsigned,
                                detail::bit_tag,
                                strong_arithmetic,
                                strong_increment_decrement,
                                strong_multiplication_with<unsigned>>
{
  /// Type alias for the base class of the bits units class.
  using bits_base = strong_type<unsigned,
                                detail::bit_tag,
                                strong_arithmetic,
                                strong_increment_decrement,
                                strong_multiplication_with<unsigned>>;

public:
  using bits_base::bits_base;

  constexpr bits(const bits_base& other) : bits_base(other) {}

  /// Returns true if the amount of digital information expressed as bits is a multiple of a byte.
  constexpr bool is_byte_exact() const { return ((value() % CHAR_BIT) == 0); }

  /// Returns the amount of digital information expressed as an integer number of bytes, rounded down.
  constexpr bytes truncate_to_bytes() const;

  /// Returns the amount of digital information expressed as an integer number of bytes, rounded up.
  constexpr bytes round_up_to_bytes() const;
};

/// \brief Abstraction of byte as a unit of digital information.
///
/// An object of this class will represent an amount of digital information expressed in bytes. The class also provides
/// a method to convert such amount into a number of information bits.
class bytes : public strong_type<unsigned,
                                 detail::byte_tag,
                                 strong_arithmetic,
                                 strong_increment_decrement,
                                 strong_multiplication_with<unsigned>>
{
  /// Type alias for the base class of the byte units class.
  using bytes_base = strong_type<unsigned,
                                 detail::byte_tag,
                                 strong_arithmetic,
                                 strong_increment_decrement,
                                 strong_multiplication_with<unsigned>>;

public:
  using bytes_base::bytes_base;

  constexpr bytes(const bytes_base& other) : bytes_base(other) {}

  explicit constexpr operator bits() const { return to_bits(); }

  /// Returns the amount of digital information as a number of bits.
  constexpr bits to_bits() const { return bits(value() * CHAR_BIT); }
};

constexpr bytes bits::truncate_to_bytes() const
{
  return bytes(value() / CHAR_BIT);
}

constexpr bytes bits::round_up_to_bytes() const
{
  return bytes((value() + CHAR_BIT - 1) / CHAR_BIT);
}

/// \brief Abstraction of a bitrate, i.e., an amount of digital information transferred per unit of time.
///
/// The value is stored normalized in bits per second. Decimal prefixes are used, i.e., 1 kbps == 1000 bps.
class bitrate : public strong_type<double, detail::bitrate_tag, strong_arithmetic, strong_multiplication_with<double>>
{
  /// Type alias for the base class of the bitrate class.
  using bitrate_base = strong_type<double, detail::bitrate_tag, strong_arithmetic, strong_multiplication_with<double>>;

public:
  using bitrate_base::bitrate_base;

  constexpr bitrate(const bitrate_base& other) : bitrate_base(other) {}

  /// Returns the bitrate expressed in kilobits per second.
  constexpr double to_kbps() const { return value() / 1e3; }

  /// Returns the bitrate expressed in megabits per second.
  constexpr double to_Mbps() const { return value() / 1e6; }

  /// Returns the bitrate expressed in gigabits per second.
  constexpr double to_Gbps() const { return value() / 1e9; }

  /// Returns the bitrate expressed in bits per second, rounded to the nearest integer. It prevents approximation
  /// errors in the double to integer conversion.
  /// \note std::round is not constexpr in C++17, hence the manual rounding.
  /// \remark The caller must ensure the stored value is non-negative and represents an integer quantity (possibly
  /// affected by a floating-point representation/arithmetic error < 0.5). Otherwise the result is meaningless (and
  /// undefined behavior for negative values).
  constexpr std::uint64_t to_uint() const { return static_cast<std::uint64_t>(value() + 0.5); }
};

/// Returns the bitrate resulting from transferring an amount of digital information over a period of time.
constexpr bitrate operator/(bits b, std::chrono::duration<double> d)
{
  return bitrate(static_cast<double>(b.value()) / d.count());
}

/// Returns the amount of digital information transferred at a given bitrate over a period of time, rounded up.
/// \note std::ceil is not constexpr in C++17, hence the manual ceiling implementation.
constexpr bits operator*(bitrate r, std::chrono::duration<double> d)
{
  const double prod      = r.value() * d.count();
  const auto   truncated = static_cast<bits::value_type>(prod);
  return bits(static_cast<double>(truncated) < prod ? truncated + 1 : truncated);
}

/// \brief Abstraction of a byterate, i.e., an amount of digital information transferred per unit of time, expressed in
/// bytes.
///
/// The value is stored normalized in bytes per second. Decimal prefixes are used, i.e., 1 kBps == 1000 Bps. The class
/// also provides a method to convert the byterate into a bitrate.
class byterate : public strong_type<double, detail::byterate_tag, strong_arithmetic, strong_multiplication_with<double>>
{
  /// Type alias for the base class of the byterate class.
  using byterate_base =
      strong_type<double, detail::byterate_tag, strong_arithmetic, strong_multiplication_with<double>>;

public:
  using byterate_base::byterate_base;

  constexpr byterate(const byterate_base& other) : byterate_base(other) {}

  explicit constexpr operator bitrate() const { return to_bitrate(); }

  /// Returns the byterate expressed as a bitrate, in bits per second.
  constexpr bitrate to_bitrate() const { return bitrate(value() * CHAR_BIT); }

  /// Returns the byterate expressed in kilobytes per second.
  constexpr double to_kBps() const { return value() / 1e3; }

  /// Returns the byterate expressed in megabytes per second.
  constexpr double to_MBps() const { return value() / 1e6; }

  /// Returns the byterate expressed in gigabytes per second.
  constexpr double to_GBps() const { return value() / 1e9; }

  /// Returns the byterate expressed in bytes per second, rounded to the nearest integer. It prevents approximation
  /// errors in the double to integer conversion.
  /// \note std::round is not constexpr in C++17, hence the manual rounding.
  /// \remark The caller must ensure the stored value is non-negative and represents an integer quantity (possibly
  /// affected by a floating-point representation/arithmetic error < 0.5). Otherwise the result is meaningless (and
  /// undefined behavior for negative values).
  constexpr std::uint64_t to_uint() const { return static_cast<std::uint64_t>(value() + 0.5); }
};

/// Returns the byterate resulting from transferring an amount of digital information over a period of time.
constexpr byterate operator/(bytes b, std::chrono::duration<double> d)
{
  return byterate(static_cast<double>(b.value()) / d.count());
}

/// Returns the amount of digital information transferred at a given byterate over a period of time, rounded up.
/// \note std::ceil is not constexpr in C++17, hence the manual ceiling implementation.
constexpr bytes operator*(byterate r, std::chrono::duration<double> d)
{
  const double prod      = r.value() * d.count();
  const auto   truncated = static_cast<bytes::value_type>(prod);
  return bytes(static_cast<double>(truncated) < prod ? truncated + 1 : truncated);
}

namespace literals {

/// User defined literal for byte units.
constexpr bytes operator""_bytes(unsigned long long n)
{
  return bytes(n);
}

/// User defined literal for bit units.
constexpr bits operator""_bits(unsigned long long n)
{
  return bits(n);
}

/// User defined literals for bitrate in bits per second.
constexpr bitrate operator""_bps(long double v)
{
  return bitrate(static_cast<double>(v));
}
constexpr bitrate operator""_bps(unsigned long long v)
{
  return bitrate(static_cast<double>(v));
}

/// User defined literals for bitrate in kilobits per second.
constexpr bitrate operator""_kbps(long double v)
{
  return bitrate(static_cast<double>(v) * 1e3);
}
constexpr bitrate operator""_kbps(unsigned long long v)
{
  return bitrate(static_cast<double>(v) * 1e3);
}

/// User defined literals for bitrate in megabits per second.
constexpr bitrate operator""_Mbps(long double v)
{
  return bitrate(static_cast<double>(v) * 1e6);
}
constexpr bitrate operator""_Mbps(unsigned long long v)
{
  return bitrate(static_cast<double>(v) * 1e6);
}

/// User defined literals for bitrate in gigabits per second.
constexpr bitrate operator""_Gbps(long double v)
{
  return bitrate(static_cast<double>(v) * 1e9);
}
constexpr bitrate operator""_Gbps(unsigned long long v)
{
  return bitrate(static_cast<double>(v) * 1e9);
}

/// User defined literals for byterate in bytes per second.
constexpr byterate operator""_Bps(long double v)
{
  return byterate(static_cast<double>(v));
}
constexpr byterate operator""_Bps(unsigned long long v)
{
  return byterate(static_cast<double>(v));
}

/// User defined literals for byterate in kilobytes per second.
constexpr byterate operator""_kBps(long double v)
{
  return byterate(static_cast<double>(v) * 1e3);
}
constexpr byterate operator""_kBps(unsigned long long v)
{
  return byterate(static_cast<double>(v) * 1e3);
}

/// User defined literals for byterate in megabytes per second.
constexpr byterate operator""_MBps(long double v)
{
  return byterate(static_cast<double>(v) * 1e6);
}
constexpr byterate operator""_MBps(unsigned long long v)
{
  return byterate(static_cast<double>(v) * 1e6);
}

/// User defined literals for byterate in gigabytes per second.
constexpr byterate operator""_GBps(long double v)
{
  return byterate(static_cast<double>(v) * 1e9);
}
constexpr byterate operator""_GBps(unsigned long long v)
{
  return byterate(static_cast<double>(v) * 1e9);
}

} // namespace literals
} // namespace units
} // namespace ocudu

namespace fmt {

/// Formatter for bit units.
template <>
struct formatter<ocudu::units::bits> : public formatter<ocudu::units::bits::value_type> {
  template <typename FormatContext>
  auto format(ocudu::units::bits s, FormatContext& ctx) const
  {
    return fmt::format_to(ctx.out(), "{}{}", s.value(), ocudu::units::bits::tag_type::str());
  }
};

/// Formatter for byte units.
template <>
struct formatter<ocudu::units::bytes> : public formatter<ocudu::units::bytes::value_type> {
  bool print_units = false;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'B') {
        print_units = true;
      }
      ++it;
    }
    return it;
  }

  template <typename FormatContext>
  auto format(ocudu::units::bytes s, FormatContext& ctx) const
  {
    return fmt::format_to(ctx.out(), "{}{}", s.value(), print_units ? ocudu::units::bytes::tag_type::str() : "");
  }
};

/// Formatter for bitrate.
template <>
struct formatter<ocudu::units::bitrate> : public formatter<ocudu::units::bitrate::value_type> {
  template <typename FormatContext>
  auto format(ocudu::units::bitrate rate, FormatContext& ctx) const
  {
    return fmt::format_to(ctx.out(), "{}{}", rate.value(), ocudu::units::bitrate::tag_type::str());
  }
};

/// Formatter for byterate.
template <>
struct formatter<ocudu::units::byterate> : public formatter<ocudu::units::byterate::value_type> {
  template <typename FormatContext>
  auto format(ocudu::units::byterate rate, FormatContext& ctx) const
  {
    return fmt::format_to(ctx.out(), "{}{}", rate.value(), ocudu::units::byterate::tag_type::str());
  }
};

} // namespace fmt
