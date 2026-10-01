// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/stable_id_map.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(stable_id_map_test, empty_table)
{
  stable_id_map<int> table;

  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_EQ(table.capacity(), 0);
  ASSERT_FALSE(table.contains(stable_id_t{0}));
}

TEST(stable_id_map_test, insert_one_object)
{
  stable_id_map<int> table;

  stable_id_t id = table.insert(42);
  ASSERT_EQ(id.value(), 0);
  ASSERT_FALSE(table.empty());
  ASSERT_EQ(table.size(), 1);
  ASSERT_GE(table.capacity(), 1);
  ASSERT_TRUE(table.contains(id));
  ASSERT_EQ(table.at(id), 42);
  ASSERT_EQ(table[id], 42);

  auto unsorted = table.unsorted();
  ASSERT_EQ(unsorted.size(), 1);
  ASSERT_EQ(unsorted[0], 42);
}

TEST(stable_id_map_test, insert_and_rem_one_object)
{
  stable_id_map<int> table;
  auto               id = table.insert(42);
  ASSERT_TRUE(table.erase(id));
  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_FALSE(table.contains(id));
  ASSERT_TRUE(table.unsorted().empty());
}

TEST(stable_id_map_test, insert_two)
{
  stable_id_map<int> table;
  auto               id0 = table.insert(42);
  auto               id1 = table.insert(43);
  ASSERT_NE(id0, id1);

  ASSERT_FALSE(table.empty());
  ASSERT_EQ(table.size(), 2);
  ASSERT_TRUE(table.contains(id0));
  ASSERT_TRUE(table.contains(id1));
  ASSERT_EQ(table[id0], 42);
  ASSERT_EQ(table[id1], 43);
}

TEST(stable_id_map_test, insert_and_rem_two)
{
  stable_id_map<int> table;
  auto               id0 = table.insert(42);
  auto               id1 = table.insert(43);
  ASSERT_TRUE(table.erase(id0));

  ASSERT_FALSE(table.empty());
  ASSERT_EQ(table.size(), 1);
  ASSERT_FALSE(table.contains(id0));
  ASSERT_TRUE(table.contains(id1));
  ASSERT_EQ(table[id1], 43);

  ASSERT_TRUE(table.erase(id1));
  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_FALSE(table.contains(id0));
  ASSERT_FALSE(table.contains(id1));
}

TEST(stable_id_map_test, insert_two_rem_and_insert)
{
  stable_id_map<int> table;
  auto               id0 = table.insert(42);
  auto               id1 = table.insert(43);
  ASSERT_TRUE(table.erase(id0));
  ASSERT_FALSE(table.contains(id0));
  auto id2 = table.insert(44);

  ASSERT_TRUE(table.contains(id2));
  ASSERT_EQ(id2, id0) << "row_id should be reused";
  ASSERT_NE(id1, id2);
  ASSERT_EQ(table.size(), 2);
  ASSERT_EQ(table[id1], 43) << "row_id must stay stable";
  ASSERT_EQ(table[id2], 44);
  ASSERT_NE(id1, id2);
}

TEST(stable_id_map_test, insert_three_rem_two_and_insert)
{
  stable_id_map<int> table;
  auto               id0 = table.insert(42);
  auto               id1 = table.insert(43);
  auto               id2 = table.insert(44);
  ASSERT_TRUE(table.erase(id0));
  ASSERT_TRUE(table.erase(id1));
  ASSERT_FALSE(table.contains(id0));
  ASSERT_FALSE(table.contains(id1));
  auto id3 = table.insert(45);

  ASSERT_TRUE(table.contains(id2));
  ASSERT_TRUE(table.contains(id3));
}

TEST(stable_id_map_test, iterator)
{
  stable_id_map<int> table;
  ASSERT_EQ(table.begin(), table.end());

  table.insert(42);
  auto id1 = table.insert(43);

  auto it = table.begin();
  ASSERT_NE(it, table.end());
  ASSERT_EQ(*it, 42);
  ++it;
  ASSERT_NE(it, table.end());
  ASSERT_EQ(*it, table[id1]);
  ASSERT_EQ(*it, 43);
  ++it;
  ASSERT_EQ(it, table.end());

  table.erase(table.begin());
  ASSERT_EQ(table.size(), 1);
  it = table.begin();
  ASSERT_NE(it, table.end());
  ASSERT_EQ(*it, table[id1]);
  ASSERT_EQ(*it, 43);

  table.erase(it);
  ASSERT_EQ(table.end(), table.begin());
}

TEST(stable_id_map_test, reserve_and_clear_resets_state)
{
  // This test guards the reset logic after bulk cleanup.
  stable_id_map<int> table;
  table.reserve(10);
  ASSERT_GE(table.capacity(), 10);

  auto id0 = table.insert(42);
  table.insert(43);
  ASSERT_EQ(table.size(), 2);

  table.clear();
  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_GE(table.capacity(), 10);

  auto id2 = table.insert(44);
  ASSERT_EQ(id2, id0) << "row_id should be reused";
  ASSERT_EQ(table.size(), 1);
  ASSERT_EQ(table[id2], 44);
  ASSERT_FALSE(false);
}

TEST(stable_id_map_test, erase_invalid_row_id_is_noop)
{
  stable_id_map<int> table;
  auto               id0 = table.insert(42);
  table.insert(43);
  auto id2 = table.insert(44);

  // Erasing invalid row should be no-op.
  ASSERT_EQ(table.size(), 3);
  ASSERT_FALSE(table.erase(stable_id_t{id2.value() + 1}));
  ASSERT_EQ(table.size(), 3);
  ASSERT_TRUE(table.contains(id0));
  ASSERT_TRUE(table.contains(id2));

  // Erasing again should be a noop.
  ASSERT_TRUE(table.erase(id0));
  ASSERT_EQ(table.size(), 2);
  ASSERT_FALSE(table.contains(id0));
  ASSERT_TRUE(table.contains(id2));
  ASSERT_FALSE(table.erase(id0));
  ASSERT_EQ(table.size(), 2);
  ASSERT_TRUE(table.contains(id2));
  ASSERT_FALSE(table.contains(id0));
}

TEST(stable_id_map_test, move_only_columns)
{
  stable_id_map<std::unique_ptr<int>> table;
  auto                                ptr1     = std::make_unique<int>(42);
  auto*                               ptr1_raw = ptr1.get();
  auto                                ptr2     = std::make_unique<int>(43);
  auto*                               ptr2_raw = ptr2.get();
  auto                                id0      = table.insert(std::move(ptr1));
  auto                                id1      = table.insert(std::move(ptr2));
  ASSERT_EQ(table[id0].get(), ptr1_raw);
  ASSERT_EQ(table[id1].get(), ptr2_raw);
  ASSERT_EQ(*table[id0], 42);
  ASSERT_EQ(*table[id1], 43);
  ASSERT_EQ(ptr1, nullptr);
  ASSERT_EQ(ptr2, nullptr);
  table.erase(id0);
  ASSERT_FALSE(table.contains(id0));
  ASSERT_EQ(*table[id1], 43);
}

struct node {
  int         value;
  stable_id_t next;
};

TEST(stable_id_map_intrusive_list, empty_list)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;
  ASSERT_TRUE(list.empty());
  auto r = list.get_list(table);
  ASSERT_EQ(r.begin(), r.end());
}

TEST(stable_id_map_intrusive_list, push_front_one)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{42, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);

  ASSERT_FALSE(r.empty());
  ASSERT_FALSE(list.empty());
  auto it = r.begin();
  ASSERT_NE(it, r.end());
  ASSERT_EQ(it->value, 42);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, push_front_preserves_lifo_order)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto id2 = table.insert(node{30, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);
  r.push_front(id2);

  auto it = r.begin();
  ASSERT_EQ(it->value, 30);
  ++it;
  ASSERT_EQ(it->value, 20);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, pop_front)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);

  auto popped = r.pop_front();
  ASSERT_EQ(popped, id1);
  ASSERT_FALSE(list.empty());

  popped = r.pop_front();
  ASSERT_EQ(popped, id0);
  ASSERT_TRUE(list.empty());
}

TEST(stable_id_map_intrusive_list, push_pop_push)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);

  r.pop_front();

  auto id2 = table.insert(node{30, {}});
  r.push_front(id2);

  auto it = r.begin();
  ASSERT_NE(it, r.end());
  ASSERT_EQ(it->value, 30);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

// stable_id_map_intrusive_list

TEST(stable_id_map_intrusive_list, const_get_list)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{42, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);

  const stable_id_map<node>&                   cref  = table;
  const stable_id_intrusive_list<&node::next>& clist = list;
  auto                                         cr    = clist.get_list(cref);
  auto                                         it    = cr.begin();
  ASSERT_NE(it, cr.end());
  ASSERT_EQ(it->value, 42);
  ++it;
  ASSERT_EQ(it, cr.end());
}

TEST(stable_id_map_intrusive_list, erase_head_by_id)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto id2 = table.insert(node{30, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);
  r.push_front(id2);

  ASSERT_TRUE(r.erase(id2));

  auto it = r.begin();
  ASSERT_EQ(it->value, 20);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, erase_middle_by_id)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto id2 = table.insert(node{30, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);
  r.push_front(id2);

  ASSERT_TRUE(r.erase(id1));

  auto it = r.begin();
  ASSERT_EQ(it->value, 30);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, erase_tail_by_id)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto id2 = table.insert(node{30, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);
  r.push_front(id2);

  ASSERT_TRUE(r.erase(id0));

  auto it = r.begin();
  ASSERT_EQ(it->value, 30);
  ++it;
  ASSERT_EQ(it->value, 20);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, erase_nonexistent_returns_false)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);

  ASSERT_FALSE(r.erase(stable_id_t{99}));
  ASSERT_FALSE(r.empty());
}

TEST(stable_id_map_intrusive_list, erase_by_iterator)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto id2 = table.insert(node{30, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);
  r.push_front(id2);

  // Erase the middle element (value 20) via iterator.
  auto it = r.begin();
  ++it;
  ASSERT_EQ(it->value, 20);
  ASSERT_TRUE(r.erase(it));

  it = r.begin();
  ASSERT_EQ(it->value, 30);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, erase_after_head)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);

  // Erase head using before_begin().
  auto erased = r.erase_after(r.before_begin());
  ASSERT_EQ(erased, id1);

  auto it = r.begin();
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, erase_after_middle)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto id2 = table.insert(node{30, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);
  r.push_front(id2);

  // Erase element after head (value 20), using head as prev.
  auto erased = r.erase_after(r.begin());
  ASSERT_EQ(erased, id1);

  auto it = r.begin();
  ASSERT_EQ(it->value, 30);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, erase_after_iteration_pattern)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto id2 = table.insert(node{30, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);
  r.push_front(id2);

  // Erase all elements matching value==20 using the before_begin/erase_after pattern.
  for (auto prev = r.before_begin(), it = r.begin(); it != r.end();) {
    if (it->value == 20) {
      r.erase_after(prev);
      break;
    }
    prev = it;
    ++it;
  }

  auto it = r.begin();
  ASSERT_EQ(it->value, 30);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, insert_after_before_begin_inserts_at_head)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);

  // Insert at head using before_begin().
  r.insert_after(r.before_begin(), id1);

  auto it = r.begin();
  ASSERT_EQ(it->value, 20);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, insert_after_tail_appends)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto id2 = table.insert(node{30, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);

  // Append after the tail (id0, which is last).
  auto tail = r.begin();
  ++tail;
  ASSERT_EQ(tail->value, 10);
  r.insert_after(tail, id2);

  auto it = r.begin();
  ASSERT_EQ(it->value, 20);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it->value, 30);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, insert_after_middle)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{10, {}});
  auto id1 = table.insert(node{20, {}});
  auto id2 = table.insert(node{30, {}});
  auto r   = list.get_list(table);
  r.push_front(id0);
  r.push_front(id1);

  // Insert after head (id1).
  r.insert_after(r.begin(), id2);

  auto it = r.begin();
  ASSERT_EQ(it->value, 20);
  ++it;
  ASSERT_EQ(it->value, 30);
  ++it;
  ASSERT_EQ(it->value, 10);
  ++it;
  ASSERT_EQ(it, r.end());
}

TEST(stable_id_map_intrusive_list, insert_after_into_empty_list)
{
  stable_id_map<node>                   table;
  stable_id_intrusive_list<&node::next> list;

  auto id0 = table.insert(node{42, {}});
  auto r   = list.get_list(table);

  // Insert into empty list via before_begin().
  r.insert_after(r.before_begin(), id0);

  ASSERT_FALSE(r.empty());
  auto it = r.begin();
  ASSERT_EQ(it->value, 42);
  ++it;
  ASSERT_EQ(it, r.end());
}
