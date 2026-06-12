// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/string_parsing_utils.h"
#include "CLI/CLI11.hpp"
#include <optional>

namespace ocudu {

using cli11_cell = std::vector<std::string>;

/// \brief Extracts the first option name from a comma-separated list of option names.
///
/// CLI11 allows specifying multiple names for an option using commas (e.g., "--addrs,--addr").
/// This function returns only the first name, which is needed for get_option_no_throw lookup.
///
/// \param option_name Single option name or list of comma-separated option name aliases (e.g. "--addrs,--addr").
/// \return The first option name to be used for option lookup.
inline std::string get_first_option_name(const std::string& option_name)
{
  auto pos = option_name.find(',');
  if (pos != std::string::npos) {
    return option_name.substr(0, pos);
  }
  return option_name;
}

/// \brief Adds a subcommand to the given application using the given subcommand name and description.
///
/// If the subcommand already exists in the application, returns a pointer to it.
///
/// \param app Application where the subcommand will be added.
/// \param name Subcommand name.
/// \param desc Human readable description of the subcommand.
/// \return A pointer to the subcommand added to the application.
inline CLI::App* add_subcommand(CLI::App& app, const std::string& name, const std::string& desc)
{
  if (CLI::App* subcommand = app.get_subcommand_no_throw(name)) {
    return subcommand;
  }

  return app.add_subcommand(name, desc)->configurable();
}

/// \brief Adds an option to the given application.
///
/// This function adds an option to the given application using the given parameters. If the option is already present
/// in the application, it is removed and a new option is added that will call the callback of the deleted callback
/// and the conversion of the result for the given parameter. By doing this, it allows to add multiple parameters for
/// one option, so one option will be present in the configuration but the result will be written in all the
/// parameters registered for that option.
///
/// \param app Application where the option will be added.
/// \param option_name Option name.
/// \param param Parameter where the option value will be stored after parsing.
/// \param desc Human readable description of the option.
/// \return A pointer to the option added to the application.
template <typename T>
CLI::Option* add_option(CLI::App& app, const std::string& option_name, T& param, const std::string& desc)
{
  auto* opt = app.get_option_no_throw(get_first_option_name(option_name));
  if (!opt) {
    return app.add_option(option_name, param, desc);
  }

  // Option was found. Get the callback and create new option.
  auto callbck = opt->get_callback();
  app.remove_option(opt);

  return app
      .add_option(
          option_name,
          [&param, callback = std::move(callbck)](const CLI::results_t& res) {
            callback(res);
            return CLI::detail::lexical_conversion<T, T>(res, param);
          },
          desc,
          false,
          [&param]() -> std::string { return CLI::detail::checked_to_string<T, T>(param); })
      ->run_callback_for_default();
}

/// Specialization for bools than changes the default function for capture the default string.
template <>
inline CLI::Option* add_option(CLI::App& app, const std::string& option_name, bool& param, const std::string& desc)
{
  auto* opt = app.get_option_no_throw(get_first_option_name(option_name));
  if (!opt) {
    return app.add_option(option_name, param, desc)->default_function([&param]() -> std::string {
      return param ? "true" : "false";
    });
  }

  // Option was found. Get the callback and create new option.
  auto callbck = opt->get_callback();
  app.remove_option(opt);

  return app
      .add_option(
          option_name,
          [&param, callback = std::move(callbck)](const CLI::results_t& res) {
            callback(res);
            return CLI::detail::lexical_conversion<bool, bool>(res, param);
          },
          desc,
          false,
          [&param]() -> std::string { return param ? "true" : "false"; })
      ->run_callback_for_default();
}

/// \brief Adds an option function to the given application.
///
/// This function adds an option function to the given application using the given parameters. If the option is
/// already present in the application, it is removed and a new option is added that will contain the given function
/// and deleted callback as function. By doing this, it allows to add multiple parameters for one option, so one
/// option will be present in the configuration and the all the functions registered for that option will be called.
///
/// \param app Application where the option will be added.
/// \param option_name Option name.
/// \param func Function to execute during parsing.
/// \param desc Human readable description of the option.
/// \return A pointer to the option added to the application.
template <typename T>
CLI::Option* add_option_function(CLI::App&                            app,
                                 const std::string&                   option_name,
                                 const std::function<void(const T&)>& func,
                                 const std::string&                   desc)
{
  auto* opt = app.get_option_no_throw(get_first_option_name(option_name));
  if (!opt) {
    return app.add_option_function<T>(option_name, func, desc)->run_callback_for_default();
  }

  // Option was found. Get the callback and create new option.
  auto callbck = opt->get_callback();
  app.remove_option(opt);

  return app
      .add_option_function<T>(
          option_name,
          [func, callback = std::move(callbck)](const std::string& value) {
            func(value);
            callback({value});
          },
          desc)
      ->run_callback_for_default();
}

/// \brief Adds an option of type cell to the given application.
///
/// \param app Application where the option will be added.
/// \param option_name Option name.
/// \param func Function to execute during parsing.
/// \param desc Human readable description of the option.
/// \return A pointer to the option added to the application.
inline CLI::Option* add_option_cell(CLI::App&                                     app,
                                    const std::string&                            option_name,
                                    const std::function<void(const cli11_cell&)>& func,
                                    const std::string&                            desc)
{
  auto* opt = app.get_option_no_throw(get_first_option_name(option_name));
  if (!opt) {
    return app.add_option_function<std::vector<std::string>>(option_name, func, desc);
  }

  // Option was found. Get the callback and create new option.
  auto callbck = opt->get_callback();
  app.remove_option(opt);

  return app
      .add_option_function<cli11_cell>(
          option_name,
          [func, callback = std::move(callbck)](const cli11_cell& value) {
            func(value);
            callback(value);
          },
          desc)
      ->run_callback_for_default();
}

/// Parse string into optional type.
template <typename T>
bool lexical_cast(const std::string& in, std::optional<T>& output)
{
  expected<T, std::string> result;

  if constexpr (std::is_integral_v<T>) {
    result = parse_int<T>(in);
  } else if constexpr (std::is_same_v<T, float>) {
    result = parse_float(in);
  } else {
    result = parse_double(in);
  }

  if (!result.has_value()) {
    return false;
  }

  output = result.value();
  return true;
}

/// Parsing an integer with additional option "auto" into an optional of an enum type.
template <typename Param>
void add_auto_enum_option(CLI::App&             app,
                          const std::string&    option_name,
                          std::optional<Param>& param,
                          const std::string&    desc)
{
  add_option_function<std::string>(
      app,
      option_name,
      [&param](const std::string& in) -> void {
        if (in.empty() or in == "auto") {
          return;
        }
        std::stringstream             ss(in);
        std::underlying_type_t<Param> val;
        ss >> val;
        param = (Param)val;
      },
      desc)
      ->check([](const std::string& in_str) -> std::string {
        if (in_str == "auto" or in_str.empty()) {
          return "";
        }
        // Check for a valid integer number;
        CLI::TypeValidator<int> IntegerValidator("INTEGER");
        return IntegerValidator(in_str);
      })
      ->default_str("auto");
}

} // namespace ocudu
