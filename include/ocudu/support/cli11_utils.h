// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/config_parsers.h"
#include "ocudu/support/config_schema.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/string_parsing_utils.h"
#include "CLI/CLI11.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <variant>

namespace ocudu {

/// \brief Chainable handle returned by the option-adding helpers.
///
/// Behaves like a \c CLI::Option* for chaining (via \c operator-> and an implicit conversion) while also recording
/// value constraints into the configuration schema. The first-class constraint methods (\c range / \c enum_values /
/// \c non_negative / \c positive) both enforce the constraint through CLI11 and record it in the schema. The
/// CLI11-style \c check(Validator) overload additionally recovers the constraint from the validator, so existing
/// \c check(CLI::Range(...)) / \c check(CLI::IsMember(...)) call sites populate the schema without being changed.
/// When the owning option is not part of a registered schema (\c node_ is null) every method is a pure pass-through.
class option_handle
{
public:
  option_handle(CLI::Option* opt, config::schema_node* node) : opt_(opt), node_(node) {}

  option_handle* operator->() { return this; }
                 operator CLI::Option*() const { return opt_; }
  CLI::Option*   cli_option() const { return opt_; }

  // -- CLI11 pass-through: forward to the option and keep chaining. --
  option_handle& capture_default_str()
  {
    opt_->capture_default_str();
    return *this;
  }
  option_handle& always_capture_default()
  {
    opt_->always_capture_default();
    return *this;
  }
  option_handle& required(bool value = true)
  {
    opt_->required(value);
    return *this;
  }
  option_handle& configurable(bool value = true)
  {
    opt_->configurable(value);
    return *this;
  }
  option_handle& expected(int value)
  {
    opt_->expected(value);
    return *this;
  }
  option_handle& expected(int min_count, int max_count)
  {
    opt_->expected(min_count, max_count);
    return *this;
  }
  option_handle& default_str(std::string value)
  {
    opt_->default_str(std::move(value));
    return *this;
  }
  option_handle& default_function(std::function<std::string()> fn)
  {
    opt_->default_function(std::move(fn));
    return *this;
  }
  option_handle& run_callback_for_default(bool value = true)
  {
    opt_->run_callback_for_default(value);
    return *this;
  }
  option_handle& needs(CLI::Option* other)
  {
    opt_->needs(other);
    return *this;
  }
  option_handle& group(std::string name)
  {
    opt_->group(std::move(name));
    return *this;
  }
  template <typename U>
  option_handle& default_val(const U& value)
  {
    opt_->default_val(value);
    return *this;
  }
  option_handle& transform(const CLI::Validator& validator)
  {
    opt_->transform(validator);
    return *this;
  }
  template <typename F, std::enable_if_t<!std::is_base_of<CLI::Validator, std::decay_t<F>>::value, int> = 0>
  option_handle& transform(F&& func)
  {
    opt_->transform(std::forward<F>(func));
    return *this;
  }

  /// Enforces \c validator and records the constraint it represents (Range/IsMember/NonNegative/Positive).
  option_handle& check(const CLI::Validator& validator)
  {
    opt_->check(validator);
    std::string desc = validator.get_description();
    if (desc.empty()) {
      desc = validator.get_name();
    }
    config::record_validator_constraint(node_, desc);
    return *this;
  }

  /// Enforces a custom string validator (a callable); nothing structured is recorded.
  template <typename F, std::enable_if_t<!std::is_base_of<CLI::Validator, std::decay_t<F>>::value, int> = 0>
  option_handle& check(F&& func)
  {
    opt_->check(std::forward<F>(func));
    return *this;
  }

  // -- First-class constraints: enforce through CLI11 and record in the schema. --
  template <typename V>
  option_handle& range(V min_value, V max_value)
  {
    opt_->check(CLI::Range(min_value, max_value));
    if (node_ != nullptr) {
      node_->constraints.minimum = config::to_scalar(node_->type, static_cast<double>(min_value));
      node_->constraints.maximum = config::to_scalar(node_->type, static_cast<double>(max_value));
      assert_bounds_match_step();
    }
    return *this;
  }

  option_handle& non_negative()
  {
    opt_->check(CLI::NonNegativeNumber);
    if (node_ != nullptr) {
      node_->constraints.minimum = config::to_scalar(node_->type, 0.0);
    }
    return *this;
  }

  option_handle& positive()
  {
    opt_->check(CLI::PositiveNumber);
    if (node_ != nullptr) {
      node_->constraints.exclusive_minimum = config::to_scalar(node_->type, 0.0);
    }
    return *this;
  }

  /// \brief Enforces that the value is a multiple of \c step, and records it as JSON Schema \c multipleOf.
  ///
  /// Use it together with \ref range for a parameter whose accepted values form an arithmetic sequence (e.g. PRBs in
  /// steps of 4), instead of enumerating every value with \ref enum_values: the generated schema and the error
  /// message state the rule rather than a wall of digits. \c step must be greater than zero.
  ///
  /// \warning \c multipleOf expresses divisibility, so it describes an arithmetic sequence only when the sequence
  /// starts and ends on a multiple of \c step (e.g. 24,28,...,272 with step 4). A sequence with a non-zero offset
  /// (e.g. 25,29,...) is not expressible in JSON Schema and must be enumerated with \ref enum_values instead;
  /// combining such a range with this method is rejected at registration.
  template <typename V>
  option_handle& multiple_of(V step)
  {
    static_assert(std::is_integral_v<V>, "multiple_of() only supports integral steps");
    report_fatal_error_if_not(step > 0, "multiple_of() requires a step greater than zero");
    opt_->check(CLI::Validator(
        [step](const std::string& value) -> std::string {
          auto parsed = parse_int<V>(value);
          // Malformed input is left to CLI11's own type conversion, which reports it against the option's type.
          if (parsed.has_value() && (parsed.value() % step != 0)) {
            return "Value " + value + " is not a multiple of " + std::to_string(step);
          }
          return {};
        },
        "MULTIPLE OF " + std::to_string(step)));
    if (node_ != nullptr) {
      node_->constraints.multiple_of = config::to_scalar(node_->type, static_cast<double>(step));
      assert_bounds_match_step();
    }
    return *this;
  }

  template <typename U>
  option_handle& enum_values(std::initializer_list<U> values)
  {
    opt_->check(CLI::IsMember(values));
    if (node_ != nullptr) {
      for (const U& value : values) {
        node_->constraints.enums.push_back(config::detail::scalar_default<U>(value));
      }
    }
    return *this;
  }

  /// Records a JSON Schema \c pattern (ECMA-262 regex) for the option's string value. Schema-only: it tightens the
  /// generated schema for external validators; runtime value validation stays with the option's own check().
  option_handle& pattern(std::string regex)
  {
    if (node_ != nullptr) {
      node_->constraints.pattern = std::move(regex);
    }
    return *this;
  }

  /// Records a JSON Schema \c minLength for the option's string value (schema-only, see \ref pattern).
  option_handle& min_length(std::uint64_t length)
  {
    if (node_ != nullptr) {
      node_->constraints.min_length = length;
    }
    return *this;
  }

  /// Records a JSON Schema \c maxLength for the option's string value (schema-only, see \ref pattern).
  option_handle& max_length(std::uint64_t length)
  {
    if (node_ != nullptr) {
      node_->constraints.max_length = length;
    }
    return *this;
  }

private:
  /// \brief Rejects a recorded range whose bounds are not multiples of the recorded step.
  ///
  /// \c multipleOf constrains divisibility, so pairing it with bounds that are off the step would describe a
  /// different value set than the intended sequence. Called from both \ref range and \ref multiple_of, so the two
  /// are checked whichever order they are chained in.
  void assert_bounds_match_step() const
  {
    if (node_ == nullptr || !node_->constraints.multiple_of) {
      return;
    }
    const auto* step = std::get_if<std::int64_t>(&*node_->constraints.multiple_of);
    if (step == nullptr || *step == 0) {
      return;
    }
    for (const std::optional<config::schema_scalar>& bound : {node_->constraints.minimum, node_->constraints.maximum}) {
      const auto* value = bound ? std::get_if<std::int64_t>(&*bound) : nullptr;
      report_fatal_error_if_not(value == nullptr || (*value % *step == 0),
                                "Option range bound {} is not a multiple of {}. multipleOf cannot express a sequence "
                                "with a non-zero offset; enumerate its values instead",
                                value != nullptr ? *value : 0,
                                *step);
    }
  }

  CLI::Option*         opt_  = nullptr;
  config::schema_node* node_ = nullptr;
};

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
  CLI::App* subcommand = app.get_subcommand_no_throw(name);
  if (!subcommand) {
    subcommand = app.add_subcommand(name, desc)->configurable();
  }
  config::record_subcommand(app, *subcommand, name, desc);
  return subcommand;
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
option_handle add_option(CLI::App& app, const std::string& option_name, T& param, const std::string& desc)
{
  auto*        existing = app.get_option_no_throw(get_first_option_name(option_name));
  CLI::Option* opt      = nullptr;
  if (!existing) {
    opt = app.add_option(option_name, param, desc);
  } else {
    // Option was found. Get the callback and create new option.
    auto callbck = existing->get_callback();
    app.remove_option(existing);

    opt = app.add_option(
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
  return option_handle{opt, config::record_option(app, option_name, param, desc, opt)};
}

/// Specialization for bools than changes the default function for capture the default string.
template <>
inline option_handle add_option(CLI::App& app, const std::string& option_name, bool& param, const std::string& desc)
{
  auto*        existing = app.get_option_no_throw(get_first_option_name(option_name));
  CLI::Option* opt      = nullptr;
  if (!existing) {
    opt = app.add_option(option_name, param, desc)->default_function([&param]() -> std::string {
      return param ? "true" : "false";
    });
  } else {
    // Option was found. Get the callback and create new option.
    auto callbck = existing->get_callback();
    app.remove_option(existing);

    opt = app.add_option(
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
  return option_handle{opt, config::record_option(app, option_name, param, desc, opt)};
}

/// \brief Adds an option group to the application and records it in the configuration schema.
///
/// Thin wrapper over \c CLI::App::add_option_group. Option groups share the parent's configuration namespace, so
/// their options are recorded as properties of the parent app in the schema (rather than being skipped, as a bare
/// CLI11 option group would be).
///
/// \param app Application where the option group will be added.
/// \param name Option group name.
/// \param desc Human readable description of the option group.
/// \return A pointer to the option group added to the application.
inline CLI::App* add_option_group(CLI::App& app, const std::string& name, const std::string& desc = "")
{
  CLI::App* group = app.add_option_group(name, desc);
  config::record_option_group(app, *group);
  return group;
}

/// \brief Adds a boolean flag to the given application and records it in the configuration schema.
///
/// Thin wrapper over \c CLI::App::add_flag that additionally records the flag as a boolean option in the schema
/// (a no-op when the app is not registered against a schema root).
///
/// \param app Application where the flag will be added.
/// \param option_name Flag name.
/// \param param Boolean parameter where the flag value will be stored after parsing.
/// \param desc Human readable description of the flag.
/// \return A pointer to the flag option added to the application.
inline option_handle add_flag(CLI::App& app, const std::string& option_name, bool& param, const std::string& desc)
{
  CLI::Option* opt = app.add_flag(option_name, param, desc);
  return option_handle{opt, config::record_flag(app, option_name, desc, opt)};
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
option_handle add_option_function(CLI::App&                            app,
                                  const std::string&                   option_name,
                                  const std::function<void(const T&)>& func,
                                  const std::string&                   desc)
{
  auto*        existing = app.get_option_no_throw(get_first_option_name(option_name));
  CLI::Option* opt      = nullptr;
  if (!existing) {
    opt = app.add_option_function<T>(option_name, func, desc)->run_callback_for_default();
  } else {
    // Option was found. Chain the previous callback with the new function. Generic over T: the results callback
    // runs the previous option's callback with the raw results, then converts them to T and calls the function.
    auto callbck = existing->get_callback();
    app.remove_option(existing);

    opt = app.add_option(
                 option_name,
                 [func, callback = std::move(callbck)](const CLI::results_t& res) -> bool {
                   callback(res);
                   T value{};
                   if (!CLI::detail::lexical_conversion<T, T>(res, value)) {
                     return false;
                   }
                   func(value);
                   return true;
                 },
                 desc)
              ->run_callback_for_default();
  }
  return option_handle{opt, config::record_function_option<T>(app, option_name, desc, opt)};
}

/// \brief Adds a list-of-struct option to the given application.
///
/// Parses each element blob into \c target[i] using \c configure, resizing \c target beforehand. Additionally captures
/// the element schema: \c configure is run
/// once against a default-constructed exemplar element, on a throwaway CLI::App bound to the array's item shape, so
/// the schema records the element structure through the same recording path as ordinary options.
///
/// \param app Application where the option will be added.
/// \param option_name Option name.
/// \param target Vector where the parsed elements will be stored.
/// \param configure Function that registers the element options on a per-element subapp.
/// \param desc Human readable description of the option.
/// \param prepare_element Optional function run on every element after the resize and before parsing (e.g. to seed
/// elements from a common configuration).
/// \return A pointer to the option added to the application.
/// \brief Records the element schema of a list-of-struct option \c option_name without registering a parser.
///
/// Runs \c configure once against a default-constructed exemplar element, on a throwaway CLI::App bound to the
/// array's item shape, so the schema records the element structure. Use this alongside a hand-written parse lambda
/// \brief Controls how an object-list element treats configuration keys it does not recognise.
enum class object_list_extras {
  /// An unrecognised key in an element is a configuration error. This is the default: it catches typos.
  reject,
  /// Unrecognised keys in an element are ignored. Needed only for a list that several application units populate
  /// from the same configuration (e.g. \c --qos in the gNB, declared by the DU, CU-CP and CU-UP): every unit parses
  /// each element with its own sub-app that knows only that unit's keys, so it has to tolerate the sibling units'
  /// keys. Typos are still caught by validating the configuration against the generated schema, which describes the
  /// union of all units' keys with \c additionalProperties:false.
  tolerate_unknown
};

/// for options that cannot use the \ref add_option_object_list overload (e.g. a map target, or a configurator that
/// does more than resize+configure+parse). A no-op when the app is not registered against a schema root.
///
/// \warning \c configure is invoked an extra time here, against a *default-constructed* element, in addition to the
/// per-element calls at parse time. It must therefore be free of observable side effects and must not fail (e.g.
/// report_error / terminate) on the element's default values - it should only register options. Cross-field
/// validation belongs in the validator files, not the configurator. The same applies to the configurator passed to
/// \ref add_option_object_list.
template <typename T>
void declare_object_list_schema(CLI::App&                                 app,
                                const std::string&                        option_name,
                                const std::function<void(CLI::App&, T&)>& configure,
                                const std::string&                        desc)
{
  auto exemplar_app  = std::make_shared<CLI::App>();
  auto exemplar_elem = std::make_shared<T>();
  config::record_array(app, option_name, desc, exemplar_app);
  configure(*exemplar_app, *exemplar_elem);
  // Anchor the exemplar element's lifetime to the exemplar app (its options bind to the element by reference); the
  // callback is never invoked as the exemplar app is never parsed.
  exemplar_app->parse_complete_callback([exemplar_elem]() {});
}

template <typename T>
CLI::Option* add_option_object_list(CLI::App&                                 app,
                                    const std::string&                        option_name,
                                    std::vector<T>&                           target,
                                    const std::function<void(CLI::App&, T&)>& configure,
                                    const std::string&                        desc,
                                    const std::function<void(T&)>&            prepare_element = nullptr,
                                    object_list_extras                        extras = object_list_extras::reject)
{
  // Capture the element shape once, on a throwaway exemplar app bound to the array's item shape.
  declare_object_list_schema<T>(app, option_name, configure, desc);

  // Parses the received per-element blobs into \c target: resizes the vector, optionally seeds each element, then
  // parses every blob through a throwaway sub-app configured by \c configure.
  auto parse = [&target, configure, prepare_element, desc, extras](const std::vector<std::string>& values) {
    target.resize(values.size());
    if (prepare_element) {
      for (auto& element : target) {
        prepare_element(element);
      }
    }
    for (unsigned i = 0, e = values.size(); i != e; ++i) {
      CLI::App subapp(desc, "item #" + std::to_string(i));
      subapp.config_formatter(create_yaml_config_parser());
      // Note the bool overload is the only one that truly ignores unknown keys: allow_config_extras(capture) just
      // defers them to the extras check, which still raises an error because it leaves allow_extras_ false.
      if (extras == object_list_extras::tolerate_unknown) {
        subapp.allow_config_extras(true);
      } else {
        subapp.allow_config_extras(CLI::config_extras_mode::error);
      }
      configure(subapp, target[i]);
      std::istringstream ss(values[i]);
      subapp.parse_from_stream(ss);
    }
  };

  auto* existing = app.get_option_no_throw(get_first_option_name(option_name));
  if (!existing) {
    return app.add_option_function<std::vector<std::string>>(option_name, parse, desc);
  }
  // Option already present (e.g. a shared section populated by several units): chain the previous callback with the
  // new parser so both run, mirroring the merge behaviour of \ref add_option.
  auto callbck = existing->get_callback();
  app.remove_option(existing);
  return app
      .add_option_function<std::vector<std::string>>(
          option_name,
          [parse, callback = std::move(callbck)](const std::vector<std::string>& values) {
            parse(values);
            callback(values);
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
  option_handle option = add_option_function<std::string>(
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
      desc);
  option
      ->check([](const std::string& in_str) -> std::string {
        if (in_str == "auto" or in_str.empty()) {
          return "";
        }
        // Check for a valid integer number;
        CLI::TypeValidator<int> IntegerValidator("INTEGER");
        return IntegerValidator(in_str);
      })
      ->default_str("auto");

  // The value is parsed as a std::string only to intercept the "auto" sentinel; its logical value is the enum held in
  // \c param. Record the schema leaf from that typed target so the schema advertises `type: integer` instead of the
  // std::string parse type ("auto" simply means "omit the key": an unset optional, hence no default). leaf_slot makes
  // recording last-declaration-wins, so this supersedes the string leaf that add_option_function just recorded.
  config::record_option(app, option_name, param, desc, option.cli_option());
}

/// \brief Adds an option whose configuration value is one of a fixed set of integers, each mapping to an enum value.
///
/// Parsing the value as an integer (rather than a std::string that a callback later interprets) lets the configuration
/// schema advertise \c "type: integer" with an \c enum of the \c allowed values, instead of an opaque string. \c
/// convert maps an accepted value to the stored enum; it only ever runs on values in \c allowed because the membership
/// check runs first.
///
/// \param app Application where the option will be added.
/// \param option_name Option name.
/// \param target Enum parameter where the mapped value will be stored after parsing.
/// \param allowed Accepted integer values, surfaced as the schema \c enum and enforced at parse time.
/// \param convert Maps an accepted integer to the stored enum value.
/// \param desc Human readable description of the option.
/// \return A chainable handle to the option added to the application.
template <typename Enum>
option_handle add_option_enum(CLI::App&                       app,
                              const std::string&              option_name,
                              Enum&                           target,
                              std::initializer_list<int>      allowed,
                              const std::function<Enum(int)>& convert,
                              const std::string&              desc)
{
  option_handle option =
      add_option_function<int>(app, option_name, [&target, convert](int value) { target = convert(value); }, desc);
  return option->enum_values(allowed);
}

} // namespace ocudu
