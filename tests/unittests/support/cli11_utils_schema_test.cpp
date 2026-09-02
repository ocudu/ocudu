// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/cli11_utils.h"
#include "ocudu/support/config_parsers.h"
#include <array>
#include <gtest/gtest.h>
#include <sstream>

using namespace ocudu;
using namespace ocudu::config;

namespace {

class cli11_utils_schema_test : public ::testing::Test
{
protected:
  void SetUp() override { registry().reset(); }
  void TearDown() override { registry().reset(); }

  static void setup_yaml(CLI::App& app)
  {
    app.config_formatter(create_yaml_config_parser());
    app.allow_config_extras(CLI::config_extras_mode::capture);
  }
};

struct cell_item {
  unsigned pci       = 1;
  uint8_t  sector_id = 127; // uint8_t default must be captured as a number, not 0x7f
};

void configure_cell(CLI::App& app, cell_item& item)
{
  add_option(app, "--pci", item.pci, "physical cell id")->capture_default_str();
  add_option(app, "--sector_id", item.sector_id, "sector id")->capture_default_str();
}

} // namespace

TEST_F(cli11_utils_schema_test, free_functions_capture_when_root_registered)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  int  count = 3;
  bool flag  = false;
  add_option(app, "--count", count, "a count")->capture_default_str();
  add_option(app, "--flag", flag, "a flag")->capture_default_str();

  CLI::App* sub = add_subcommand(app, "section", "a section");
  int       x   = 5;
  add_option(*sub, "--x", x, "x value")->capture_default_str();

  ASSERT_EQ(root.children.size(), 3u);
  EXPECT_EQ(root.children[0]->name, "count");
  EXPECT_EQ(root.children[0]->type, leaf_type::integer);
  EXPECT_EQ(root.children[1]->name, "flag");
  EXPECT_EQ(root.children[1]->type, leaf_type::boolean);

  schema_node* grp = root.children[2].get();
  EXPECT_EQ(grp->kind, node_kind::group);
  EXPECT_EQ(grp->name, "section");
  ASSERT_EQ(grp->children.size(), 1u);
  EXPECT_EQ(grp->children[0]->name, "x");
}

TEST_F(cli11_utils_schema_test, passthrough_leaves_parsing_unchanged_without_root)
{
  // No schema root registered: capture is a no-op, parsing still works.
  CLI::App app;
  setup_yaml(app);
  int count = 0;
  add_option(app, "--count", count, "a count")->capture_default_str();

  std::istringstream ss("count: 7\n");
  app.parse_from_stream(ss);
  EXPECT_EQ(count, 7);
  EXPECT_EQ(registry().lookup(&app), nullptr);
}

TEST_F(cli11_utils_schema_test, check_validators_are_recorded_as_constraints)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  int r = 5;
  add_option(app, "--r", r, "ranged")->check(CLI::Range(-1, 1024));
  int e = 1;
  add_option(app, "--e", e, "enum int")->check(CLI::IsMember({1, 2, 4, 8}));
  std::string s = "a";
  add_option(app, "--s", s, "enum str")->check(CLI::IsMember({"a", "b", "c"}));
  double nn = 0;
  add_option(app, "--nn", nn, "nonneg")->check(CLI::NonNegativeNumber);
  double p = 1;
  add_option(app, "--p", p, "positive")->check(CLI::PositiveNumber);

  ASSERT_EQ(root.children.size(), 5u);
  EXPECT_EQ(std::get<std::int64_t>(*root.children[0]->constraints.minimum), -1);
  EXPECT_EQ(std::get<std::int64_t>(*root.children[0]->constraints.maximum), 1024);
  ASSERT_EQ(root.children[1]->constraints.enums.size(), 4u);
  EXPECT_EQ(std::get<std::int64_t>(root.children[1]->constraints.enums[2]), 4);
  ASSERT_EQ(root.children[2]->constraints.enums.size(), 3u);
  EXPECT_EQ(std::get<std::string>(root.children[2]->constraints.enums[0]), "a");
  EXPECT_TRUE(root.children[3]->constraints.minimum.has_value()); // NonNegativeNumber -> minimum 0
  EXPECT_TRUE(root.children[4]->constraints.exclusive_minimum.has_value());
}

TEST_F(cli11_utils_schema_test, first_class_constraint_methods_record_and_enforce)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  int pci = 1;
  add_option(app, "--pci", pci, "pci")->range(0, 1007);
  int band = 1;
  add_option(app, "--band", band, "band")->enum_values({1, 3, 7, 78});

  ASSERT_EQ(root.children.size(), 2u);
  EXPECT_EQ(std::get<std::int64_t>(*root.children[0]->constraints.minimum), 0);
  EXPECT_EQ(std::get<std::int64_t>(*root.children[0]->constraints.maximum), 1007);
  ASSERT_EQ(root.children[1]->constraints.enums.size(), 4u);
  EXPECT_EQ(std::get<std::int64_t>(root.children[1]->constraints.enums[3]), 78);

  // The .range() constraint is also enforced by CLI11: an out-of-range value is rejected.
  setup_yaml(app);
  std::istringstream ss("pci: 5000\n");
  EXPECT_THROW(app.parse_from_stream(ss), CLI::ParseError);
}

TEST_F(cli11_utils_schema_test, multiple_of_records_and_enforces)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  unsigned prbs = 24;
  add_option(app, "--prbs", prbs, "PRS bandwidth in PRBs")->range(24, 272)->multiple_of(4);

  ASSERT_EQ(root.children.size(), 1u);
  EXPECT_EQ(std::get<std::int64_t>(*root.children[0]->constraints.minimum), 24);
  EXPECT_EQ(std::get<std::int64_t>(*root.children[0]->constraints.maximum), 272);
  EXPECT_EQ(std::get<std::int64_t>(*root.children[0]->constraints.multiple_of), 4);
  // An arithmetic sequence is recorded as a step, not as an enum of every accepted value.
  EXPECT_TRUE(root.children[0]->constraints.enums.empty());

  setup_yaml(app);
  // A value in range but off the step is rejected.
  std::istringstream bad("prbs: 26\n");
  EXPECT_THROW(app.parse_from_stream(bad), CLI::ParseError);

  // A value on the step is accepted.
  CLI::App    app2;
  schema_node root2;
  register_schema_root(app2, root2);
  unsigned prbs2 = 24;
  add_option(app2, "--prbs", prbs2, "PRS bandwidth in PRBs")->range(24, 272)->multiple_of(4);
  setup_yaml(app2);
  std::istringstream good("prbs: 28\n");
  EXPECT_NO_THROW(app2.parse_from_stream(good));
  EXPECT_EQ(prbs2, 28u);
}

TEST_F(cli11_utils_schema_test, multiple_of_supports_a_fractional_step)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  // As in JSON Schema, the step is any positive number, not only an integer.
  double gain = 0.0;
  add_option(app, "--gain", gain, "gain in dB")->range(0.0, 10.0)->multiple_of(0.5);

  ASSERT_EQ(root.children.size(), 1u);
  EXPECT_DOUBLE_EQ(std::get<double>(*root.children[0]->constraints.multiple_of), 0.5);

  setup_yaml(app);
  // A value in range but off the step is rejected.
  std::istringstream bad("gain: 0.7\n");
  EXPECT_THROW(app.parse_from_stream(bad), CLI::ParseError);

  // A value on the step is accepted.
  CLI::App    app2;
  schema_node root2;
  register_schema_root(app2, root2);
  double gain2 = 0.0;
  add_option(app2, "--gain", gain2, "gain in dB")->range(0.0, 10.0)->multiple_of(0.5);
  setup_yaml(app2);
  std::istringstream good("gain: 2.5\n");
  EXPECT_NO_THROW(app2.parse_from_stream(good));
  EXPECT_DOUBLE_EQ(gain2, 2.5);
}

TEST_F(cli11_utils_schema_test, multiple_of_rejects_a_fractional_step_for_an_integer_option)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  // An integer option cannot express a fractional step, which would be recorded truncated (here, as a zero multipleOf).
  unsigned prbs = 0;
  EXPECT_DEATH(add_option(app, "--prbs", prbs, "a number of PRBs")->multiple_of(0.5), ".*");
}

TEST_F(cli11_utils_schema_test, multiple_of_rejects_bounds_off_the_step)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  // A sequence with a non-zero offset (25,29,...) is not expressible as multipleOf, so registering it must fail
  // rather than silently describe a different value set.
  int off_step = 25;
  EXPECT_DEATH(add_option(app, "--off_step", off_step, "off-step")->range(25, 273)->multiple_of(4), ".*");

  // The same check applies when the range is chained after the step.
  int off_step2 = 25;
  EXPECT_DEATH(add_option(app, "--off_step2", off_step2, "off-step")->multiple_of(4)->range(25, 273), ".*");
}

TEST_F(cli11_utils_schema_test, multiple_of_allows_an_upper_bound_off_the_step)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  // An upper bound off the step is not attainable, so it describes the sequence 0,5,...,155 correctly.
  unsigned offset_sf = 0;
  add_option(app, "--offset_sf", offset_sf, "offset in subframes")->range(0U, 159U)->multiple_of(5U);

  ASSERT_EQ(root.children.size(), 1u);
  EXPECT_EQ(std::get<std::int64_t>(*root.children[0]->constraints.multiple_of), 5);

  setup_yaml(app);
  std::istringstream ss("offset_sf: 155\n");
  EXPECT_NO_THROW(app.parse_from_stream(ss));
  EXPECT_EQ(offset_sf, 155u);
}

TEST_F(cli11_utils_schema_test, enum_values_accepts_a_container_of_narrow_integers)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  // A constant listing the accepted values is passed as is, and its narrow element type is recorded (and rendered by
  // CLI11) as a number, not as the character with that code.
  static constexpr std::array<uint8_t, 4> valid_comb_sizes = {2, 4, 6, 12};
  unsigned                                comb_size        = 2;
  add_option(app, "--comb_size", comb_size, "comb size")->capture_default_str()->enum_values(valid_comb_sizes);

  ASSERT_EQ(root.children.size(), 1u);
  ASSERT_EQ(root.children[0]->constraints.enums.size(), 4u);
  EXPECT_EQ(std::get<std::uint64_t>(root.children[0]->constraints.enums[3]), 12u);
  EXPECT_NE(root.children[0]->option->get_type_name().find("{2,4,6,12}"), std::string::npos);

  setup_yaml(app);
  std::istringstream bad("comb_size: 3\n");
  EXPECT_THROW(app.parse_from_stream(bad), CLI::ParseError);

  CLI::App    app2;
  schema_node root2;
  register_schema_root(app2, root2);
  unsigned comb_size2 = 2;
  add_option(app2, "--comb_size", comb_size2, "comb size")->enum_values(valid_comb_sizes);
  setup_yaml(app2);
  std::istringstream good("comb_size: 12\n");
  EXPECT_NO_THROW(app2.parse_from_stream(good));
  EXPECT_EQ(comb_size2, 12u);
}

TEST_F(cli11_utils_schema_test, option_pointer_exposes_required_flag)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  int start = 0;
  add_option(app, "--start", start, "range start")->required();

  ASSERT_EQ(root.children.size(), 1u);
  ASSERT_NE(root.children[0]->option, nullptr);
  EXPECT_TRUE(root.children[0]->option->get_required());
}

TEST_F(cli11_utils_schema_test, option_group_options_recorded_at_parent_level)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  // Options added to an option group share the parent's config namespace, so they must appear as the parent's
  // properties, not be dropped.
  CLI::App* group = add_option_group(app, "display_only");
  int       x     = 7;
  add_option(*group, "--grouped", x, "a grouped option")->capture_default_str();

  ASSERT_EQ(root.children.size(), 1u);
  EXPECT_EQ(root.children[0]->name, "grouped");
  EXPECT_EQ(root.children[0]->type, leaf_type::integer);
}

TEST_F(cli11_utils_schema_test, add_option_function_records_typed_leaf)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  // A custom-parsed option over a non-string type must compile and record with the type of T (integer), no default.
  unsigned captured = 0;
  add_option_function<unsigned>(
      app, "--freq", [&captured](unsigned v) { captured = v; }, "a frequency")
      ->check(CLI::Range(0u, 100u));

  // A string-parsed option (enum name style) records as a string.
  std::string mode;
  add_option_function<std::string>(app, "--mode", [&mode](const std::string& v) { mode = v; }, "a mode");

  // A custom-parsed list of strings records as a scalar array.
  std::vector<std::string> masks;
  add_option_function<std::vector<std::string>>(
      app, "--masks", [&masks](const std::vector<std::string>& v) { masks = v; }, "a list of masks");

  ASSERT_EQ(root.children.size(), 3u);
  EXPECT_EQ(root.children[0]->name, "freq");
  EXPECT_EQ(root.children[0]->type, leaf_type::integer);
  EXPECT_FALSE(root.children[0]->dflt.present);
  EXPECT_EQ(root.children[1]->name, "mode");
  EXPECT_EQ(root.children[1]->type, leaf_type::string);
  EXPECT_EQ(root.children[2]->name, "masks");
  EXPECT_EQ(root.children[2]->type, leaf_type::string);
  EXPECT_TRUE(root.children[2]->is_scalar_array);
}

TEST_F(cli11_utils_schema_test, add_option_object_list_captures_shape_and_parses)
{
  CLI::App root_app;
  setup_yaml(root_app);
  schema_node root;
  register_schema_root(root_app, root);

  std::vector<cell_item> cells;
  add_option_object_list<cell_item>(root_app, "--cells", cells, configure_cell, "per-cell config");

  // Schema: array node with the element's fields, including the uint8_t captured as a number.
  ASSERT_EQ(root.children.size(), 1u);
  schema_node* arr = root.children[0].get();
  EXPECT_EQ(arr->kind, node_kind::array);
  EXPECT_EQ(arr->name, "cells");
  ASSERT_EQ(arr->children.size(), 2u);
  EXPECT_EQ(arr->children[0]->name, "pci");
  EXPECT_EQ(arr->children[1]->name, "sector_id");
  EXPECT_EQ(arr->children[1]->type, leaf_type::integer);
  ASSERT_TRUE(arr->children[1]->dflt.present);
  EXPECT_EQ(std::get<std::uint64_t>(arr->children[1]->dflt.values[0]), 127u);

  // Parse behaviour: a 2-element list is parsed into the target exactly like the hand-written lambdas.
  std::istringstream ss(R"(
cells:
  - pci: 10
  - pci: 20
    sector_id: 3
)");
  root_app.parse_from_stream(ss);
  ASSERT_EQ(cells.size(), 2u);
  EXPECT_EQ(cells[0].pci, 10u);
  EXPECT_EQ(cells[0].sector_id, 127); // default kept
  EXPECT_EQ(cells[1].pci, 20u);
  EXPECT_EQ(cells[1].sector_id, 3);
}

TEST_F(cli11_utils_schema_test, add_option_object_list_prepare_element_seeds_defaults)
{
  CLI::App root_app;
  setup_yaml(root_app);
  schema_node root;
  register_schema_root(root_app, root);

  std::vector<cell_item> cells;
  add_option_object_list<cell_item>(
      root_app, "--cells", cells, configure_cell, "per-cell", [](cell_item& c) { c.pci = 99; });

  std::istringstream ss(R"(
cells:
  - sector_id: 1
)");
  root_app.parse_from_stream(ss);
  ASSERT_EQ(cells.size(), 1u);
  EXPECT_EQ(cells[0].pci, 99u); // seeded by prepare_element, not overridden by the blob
  EXPECT_EQ(cells[0].sector_id, 1);
}
