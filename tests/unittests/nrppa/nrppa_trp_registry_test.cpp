// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/nrppa/du_context/nrppa_trp_registry.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class for the NRPPA TRP registry.
class nrppa_trp_registry_test : public ::testing::Test
{
protected:
  const cu_cp_du_index_t du_index  = uint_to_cu_cp_du_index(0);
  const cu_cp_du_index_t du_index2 = uint_to_cu_cp_du_index(1);
  const trp_id_t         trp_id    = uint_to_trp_id(1);
  const trp_id_t         trp_id2   = uint_to_trp_id(2);

  nrppa_trp_registry trp_registry;
};

TEST_F(nrppa_trp_registry_test, when_no_trp_was_added_then_trp_information_is_not_available)
{
  ASSERT_FALSE(trp_registry.has_trp_information());
}

TEST_F(nrppa_trp_registry_test, when_trp_is_added_then_trp_information_is_available)
{
  trp_registry.add_trp(trp_id, du_index);

  ASSERT_TRUE(trp_registry.has_trp_information());
}

TEST_F(nrppa_trp_registry_test, when_trp_is_added_then_find_du_resolves_the_hosting_du)
{
  trp_registry.add_trp(trp_id, du_index);
  trp_registry.add_trp(trp_id2, du_index2);

  ASSERT_EQ(trp_registry.find_du(trp_id), du_index);
  ASSERT_EQ(trp_registry.find_du(trp_id2), du_index2);
}

TEST_F(nrppa_trp_registry_test, when_trp_is_unknown_then_find_du_resolves_nothing)
{
  trp_registry.add_trp(trp_id, du_index);

  ASSERT_FALSE(trp_registry.find_du(trp_id2).has_value());
}

TEST_F(nrppa_trp_registry_test, when_du_is_removed_then_its_trps_are_dropped)
{
  trp_registry.add_trp(trp_id, du_index);
  trp_registry.add_trp(trp_id2, du_index);

  trp_registry.remove_du(du_index);

  ASSERT_FALSE(trp_registry.has_trp_information());
  ASSERT_FALSE(trp_registry.find_du(trp_id).has_value());
  ASSERT_FALSE(trp_registry.find_du(trp_id2).has_value());
}

TEST_F(nrppa_trp_registry_test, when_du_is_removed_then_the_trps_of_the_other_dus_are_kept)
{
  trp_registry.add_trp(trp_id, du_index);
  trp_registry.add_trp(trp_id2, du_index2);

  trp_registry.remove_du(du_index);

  ASSERT_FALSE(trp_registry.find_du(trp_id).has_value());
  ASSERT_EQ(trp_registry.find_du(trp_id2), du_index2);
}

TEST_F(nrppa_trp_registry_test, when_unknown_du_is_removed_then_the_registry_is_unchanged)
{
  trp_registry.add_trp(trp_id, du_index);

  trp_registry.remove_du(du_index2);

  ASSERT_EQ(trp_registry.find_du(trp_id), du_index);
}
