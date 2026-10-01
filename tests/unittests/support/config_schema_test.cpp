// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/support/config_schema.h"
#include <gtest/gtest.h>

using namespace ocudu::config;

namespace {

// Fixture that clears the process-global registry before each test.
class config_schema_test : public ::testing::Test
{
protected:
  void SetUp() override { registry().reset(); }
  void TearDown() override { registry().reset(); }
};

enum class colour { red = 0, green = 1, blue = 2 };

} // namespace

// ---------------------------------------------------------------------------
// Typed capture
// ---------------------------------------------------------------------------

TEST_F(config_schema_test, capture_scalar_types)
{
  {
    int       v    = -7;
    leaf_info info = capture_leaf(v);
    EXPECT_EQ(info.type, leaf_type::integer);
    EXPECT_FALSE(info.is_scalar_array);
    ASSERT_TRUE(info.dflt.present);
    ASSERT_EQ(info.dflt.values.size(), 1u);
    EXPECT_EQ(std::get<std::int64_t>(info.dflt.values[0]), -7);
  }
  {
    double    v    = 0.5;
    leaf_info info = capture_leaf(v);
    EXPECT_EQ(info.type, leaf_type::number);
    EXPECT_DOUBLE_EQ(std::get<double>(info.dflt.values[0]), 0.5);
  }
  {
    bool      v    = true;
    leaf_info info = capture_leaf(v);
    EXPECT_EQ(info.type, leaf_type::boolean);
    EXPECT_TRUE(std::get<bool>(info.dflt.values[0]));
  }
  {
    std::string v    = "hello";
    leaf_info   info = capture_leaf(v);
    EXPECT_EQ(info.type, leaf_type::string);
    EXPECT_EQ(std::get<std::string>(info.dflt.values[0]), "hello");
  }
}

TEST_F(config_schema_test, capture_uint8_is_a_number_not_a_byte)
{
  // The bug that motivated typed capture: uint8_t default 127 must be the number 127, never the raw 0x7f byte.
  std::uint8_t v    = 127;
  leaf_info    info = capture_leaf(v);
  EXPECT_EQ(info.type, leaf_type::integer);
  ASSERT_TRUE(info.dflt.present);
  EXPECT_EQ(std::get<std::uint64_t>(info.dflt.values[0]), 127u);
}

TEST_F(config_schema_test, capture_bounded_integer_is_an_integer)
{
  // Strong-typed integers such as arfcn_t are bounded_integer<>, which std::is_integral does not match; capture must
  // still classify them as integers and read the default through value(), not fall back to string.
  ocudu::bounded_integer<std::uint32_t, 0, 1000> v{42};
  leaf_info                                      info = capture_leaf(v);
  EXPECT_EQ(info.type, leaf_type::integer);
  EXPECT_FALSE(info.is_scalar_array);
  ASSERT_TRUE(info.dflt.present);
  EXPECT_EQ(std::get<std::uint64_t>(info.dflt.values[0]), 42u);
}

TEST_F(config_schema_test, capture_enum_and_duration)
{
  {
    colour    v    = colour::blue;
    leaf_info info = capture_leaf(v);
    EXPECT_EQ(info.type, leaf_type::integer);
    EXPECT_EQ(std::get<std::int64_t>(info.dflt.values[0]), 2);
  }
  {
    std::chrono::milliseconds v{40};
    leaf_info                 info = capture_leaf(v);
    EXPECT_EQ(info.type, leaf_type::integer);
    EXPECT_EQ(std::get<std::int64_t>(info.dflt.values[0]), 40);
  }
}

TEST_F(config_schema_test, capture_optional)
{
  {
    std::optional<unsigned> engaged = 9u;
    leaf_info               info    = capture_leaf(engaged);
    EXPECT_EQ(info.type, leaf_type::integer);
    ASSERT_TRUE(info.dflt.present);
    EXPECT_EQ(std::get<std::uint64_t>(info.dflt.values[0]), 9u);
  }
  {
    std::optional<unsigned> empty;
    leaf_info               info = capture_leaf(empty);
    EXPECT_EQ(info.type, leaf_type::integer);
    EXPECT_FALSE(info.dflt.present); // no default for an empty optional
  }
}

TEST_F(config_schema_test, capture_vector_is_scalar_array)
{
  std::vector<int> v    = {1, 2, 3};
  leaf_info        info = capture_leaf(v);
  EXPECT_EQ(info.type, leaf_type::integer);
  EXPECT_TRUE(info.is_scalar_array);
  ASSERT_TRUE(info.dflt.present);
  EXPECT_TRUE(info.dflt.is_sequence);
  ASSERT_EQ(info.dflt.values.size(), 3u);
  EXPECT_EQ(std::get<std::int64_t>(info.dflt.values[1]), 2);
}

TEST_F(config_schema_test, capture_std_array_is_scalar_array)
{
  // A fixed-size std::array (e.g. ss1_n_candidates) must be captured as a scalar array, not fall through to a string;
  // uint8_t elements are numbers, not raw bytes.
  std::array<std::uint8_t, 5> v    = {0, 0, 2, 0, 0};
  leaf_info                   info = capture_leaf(v);
  EXPECT_EQ(info.type, leaf_type::integer);
  EXPECT_TRUE(info.is_scalar_array);
  ASSERT_TRUE(info.dflt.present);
  EXPECT_TRUE(info.dflt.is_sequence);
  ASSERT_EQ(info.dflt.values.size(), 5u);
  EXPECT_EQ(std::get<std::uint64_t>(info.dflt.values[2]), 2u);
}

// ---------------------------------------------------------------------------
// Registry + recording helpers
// ---------------------------------------------------------------------------

TEST_F(config_schema_test, passthrough_when_app_not_registered)
{
  CLI::App app;
  int      x = 5;
  // No register_schema_root: recording must be a no-op and must not crash.
  record_option(app, "--x", x, "desc", nullptr);
  EXPECT_EQ(registry().lookup(&app), nullptr);
}

TEST_F(config_schema_test, record_options_under_root)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  int         i = 3;
  std::string s = "abc";
  record_option(app, "--count,--n", i, "an integer", nullptr);
  record_option(app, "--name", s, "a name", nullptr);
  // Repeated name: no duplicate child (last declaration wins, see the merge test below).
  int other = 99;
  record_option(app, "--count", other, "dup", nullptr);

  ASSERT_EQ(root.children.size(), 2u);
  EXPECT_EQ(root.children[0]->name, "count"); // first alias, dashes stripped
  EXPECT_EQ(root.children[0]->type, leaf_type::integer);
  EXPECT_EQ(root.children[1]->name, "name");
}

// Mirrors the gNB/standalone-CU shared-tree merge: several units register options against one CLI::App, so the same
// schema node is populated more than once. A repeated leaf must be updated in place (last declaration wins, matching
// CLI11's merge-aware add_option), while an array node must survive a later leaf recording of the same name.
TEST_F(config_schema_test, shared_node_repeated_leaf_is_last_wins)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  std::string first = "abc";
  record_option(app, "--val", first, "first description", nullptr);
  // Second unit re-declares the same option with a different C++ type and default.
  int second = 7;
  record_option(app, "--val", second, "second description", nullptr);

  ASSERT_EQ(root.children.size(), 1u); // no duplicate child
  EXPECT_EQ(root.children[0]->name, "val");
  EXPECT_EQ(root.children[0]->type, leaf_type::integer);          // last declaration's type
  EXPECT_EQ(root.children[0]->description, "second description"); // last declaration's description
}

TEST_F(config_schema_test, shared_node_array_survives_later_leaf_recording)
{
  CLI::App    root_app;
  schema_node root;
  register_schema_root(root_app, root);

  // First unit declares a list-of-struct option (as declare_object_list_schema does).
  auto exemplar = std::make_shared<CLI::App>();
  record_array(root_app, "--areas", "tracking areas", exemplar);
  int tac = 1;
  record_option(*exemplar, "--tac", tac, "tracking area code", nullptr);

  // A later scalar recording of the same option name (e.g. the string-array parser behind the same option) must not
  // demote the array node to a leaf.
  std::vector<std::string> raw;
  record_option(root_app, "--areas", raw, "raw blobs", nullptr);

  ASSERT_EQ(root.children.size(), 1u);
  schema_node* arr = root.children[0].get();
  EXPECT_EQ(arr->kind, node_kind::array); // still an array, element shape preserved
  ASSERT_EQ(arr->children.size(), 1u);
  EXPECT_EQ(arr->children[0]->name, "tac");
}

TEST_F(config_schema_test, option_name_strips_whitespace_and_dashes)
{
  EXPECT_EQ(schema_option_name("--pci"), "pci");
  EXPECT_EQ(schema_option_name("--addrs,--addr"), "addrs");
  EXPECT_EQ(schema_option_name(" --e2ap_level"), "e2ap_level"); // stray leading space tolerated
  EXPECT_EQ(schema_option_name("-c"), "c");
}

TEST_F(config_schema_test, record_nested_subcommands)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  CLI::App* sub = app.add_subcommand("f1ap", "F1AP config");
  record_subcommand(app, *sub, "f1ap", "F1AP config");
  int port = 38472;
  record_option(*sub, "--bind_port", port, "bind port", nullptr);

  ASSERT_EQ(root.children.size(), 1u);
  schema_node* grp = root.children[0].get();
  EXPECT_EQ(grp->kind, node_kind::group);
  EXPECT_EQ(grp->name, "f1ap");
  ASSERT_EQ(grp->children.size(), 1u);
  EXPECT_EQ(grp->children[0]->name, "bind_port");
  EXPECT_EQ(grp->children[0]->type, leaf_type::integer);
}

TEST_F(config_schema_test, record_array_captures_item_shape_via_exemplar)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  // Simulate what add_option_cell does: create an exemplar app bound to the array node, then run the element
  // configurator against it, which records the element's fields as leaves under the array node.
  auto exemplar = std::make_shared<CLI::App>();
  record_array(app, "--cells", "per-cell config", exemplar);

  int          pci = 1;
  std::uint8_t sd  = 127;
  record_option(*exemplar, "--pci", pci, "physical cell id", nullptr);
  record_option(*exemplar, "--sector_id", sd, "sector id", nullptr);

  ASSERT_EQ(root.children.size(), 1u);
  schema_node* arr = root.children[0].get();
  EXPECT_EQ(arr->kind, node_kind::array);
  EXPECT_EQ(arr->name, "cells");
  ASSERT_EQ(arr->children.size(), 2u);
  EXPECT_EQ(arr->children[0]->name, "pci");
  EXPECT_EQ(arr->children[1]->name, "sector_id");
  // exemplar kept alive by the array node.
  ASSERT_EQ(arr->exemplars.size(), 1u);
}

TEST_F(config_schema_test, record_array_repeated_declaration_unions_fields)
{
  CLI::App    app;
  schema_node root;
  register_schema_root(app, root);

  auto ex1 = std::make_shared<CLI::App>();
  record_array(app, "--cell_affinities", "affinities", ex1);
  int a = 0;
  record_option(*ex1, "--ru_cpus", a, "ru cpus", nullptr);

  // Second declaration of the same option on the shared node with a different element shape.
  auto ex2 = std::make_shared<CLI::App>();
  record_array(app, "--cell_affinities", "affinities", ex2);
  int b = 0;
  record_option(*ex2, "--extra", b, "extra", nullptr);

  ASSERT_EQ(root.children.size(), 1u); // still one array node
  schema_node* arr = root.children[0].get();
  ASSERT_EQ(arr->children.size(), 2u); // union of both shapes
  EXPECT_EQ(arr->children[0]->name, "ru_cpus");
  EXPECT_EQ(arr->children[1]->name, "extra");
  EXPECT_EQ(arr->exemplars.size(), 2u);
}
