// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/config/validator_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;

enum class id_type { MAX_ID_TYPE_VALUE = 1000 };

struct example_resource {
  id_type id_field;
};

std::vector<example_resource> make_resource_list(const std::initializer_list<unsigned>& list)
{
  std::vector<example_resource> ret;
  for (unsigned i : list) {
    ret.push_back({(id_type)i});
  }
  return ret;
}

TEST(validator_helpers_test, detection_of_unique_id)
{
  const std::vector<example_resource> vec1 = make_resource_list({1, 2, 6, 3, 4});
  const std::vector<example_resource> vec2 = make_resource_list({1, 2, 4, 3, 4});

  ASSERT_TRUE(has_unique_ids(vec1, [](example_resource r) { return r.id_field; }));
  ASSERT_TRUE(has_unique_ids(vec1, &example_resource::id_field));
  ASSERT_FALSE(has_unique_ids(vec2, [](example_resource r) { return r.id_field; }));
  ASSERT_FALSE(has_unique_ids(vec2, &example_resource::id_field));
}
