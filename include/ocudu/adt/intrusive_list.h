// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

namespace ocudu {

struct default_intrusive_tag;

/// Base class of each node of an intrusive_forward_stack<T> or intrusive_forward_list<T>.
/// \tparam Tag type used to differentiate separate intrusive lists stored in a single node
template <typename Tag = default_intrusive_tag>
struct intrusive_forward_list_element {
  intrusive_forward_list_element<Tag>* next_node = nullptr;
};

/// Intrusive singly-linked stack (push/pop at the front only).
/// Does not track a tail pointer; supports push_front, pop_front, iteration and clear.
/// \tparam T node type. It must be a subclass of intrusive_forward_list_element<Tag>
/// \tparam Tag useful to differentiate multiple intrusive lists in the same node
template <typename T, typename Tag = default_intrusive_tag>
class intrusive_forward_stack
{
  using node_t = intrusive_forward_list_element<Tag>;

  template <typename U>
  class iterator_impl
  {
    using elem_t = std::conditional_t<std::is_const_v<U>, const node_t, node_t>;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = U;
    using difference_type   = std::ptrdiff_t;
    using pointer           = U*;
    using reference         = U&;

    explicit iterator_impl(elem_t* node_ = nullptr) : node(node_) {}
    iterator_impl<U>& operator++()
    {
      node = node->next_node;
      return *this;
    }
    pointer   operator->() { return static_cast<pointer>(node); }
    reference operator*() { return static_cast<reference>(*node); }

    bool operator==(const iterator_impl<U>& other) const { return node == other.node; }
    bool operator!=(const iterator_impl<U>& other) const { return node != other.node; }

  private:
    elem_t* node;
  };

public:
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  intrusive_forward_stack()
  {
    static_assert(std::is_base_of_v<node_t, T>,
                  "Provided template argument T must have intrusive_forward_list_element<Tag> as base class");
  }
  intrusive_forward_stack(const intrusive_forward_stack&) = default;
  intrusive_forward_stack(intrusive_forward_stack&& other) noexcept : node(other.node) { other.node = nullptr; }
  intrusive_forward_stack& operator=(const intrusive_forward_stack&) = default;
  intrusive_forward_stack& operator=(intrusive_forward_stack&& other) noexcept
  {
    node       = other.node;
    other.node = nullptr;
    return *this;
  }

  T& front() const { return *static_cast<T*>(node); }

  void push_front(T* t)
  {
    node_t* new_head    = static_cast<node_t*>(t);
    new_head->next_node = node;
    node                = new_head;
  }
  T* pop_front()
  {
    node_t* ret = node;
    node        = node->next_node;
    return static_cast<T*>(ret);
  }
  void clear()
  {
    while (node != nullptr) {
      node_t* torem    = node;
      node             = node->next_node;
      torem->next_node = nullptr;
    }
  }

  bool empty() const { return node == nullptr; }

  iterator       begin() { return iterator(node); }
  iterator       end() { return iterator(nullptr); }
  const_iterator begin() const { return const_iterator(node); }
  const_iterator end() const { return const_iterator(nullptr); }

private:
  node_t* node = nullptr;
};

/// Intrusive singly-linked list with O(1) push_back via a tail pointer.
/// Supports push_front, push_back, pop_front, front, back, iteration and clear.
/// \tparam T node type. It must be a subclass of intrusive_forward_list_element<Tag>
/// \tparam Tag useful to differentiate multiple intrusive lists in the same node
template <typename T, typename Tag = default_intrusive_tag>
class intrusive_forward_list
{
  using node_t  = intrusive_forward_list_element<Tag>;
  using stack_t = intrusive_forward_stack<T, Tag>;

public:
  using iterator       = typename stack_t::iterator;
  using const_iterator = typename stack_t::const_iterator;

  intrusive_forward_list()                              = default;
  intrusive_forward_list(const intrusive_forward_list&) = default;
  intrusive_forward_list(intrusive_forward_list&& other) noexcept :
    stack(std::move(other.stack)), tail(std::exchange(other.tail, nullptr))
  {
  }
  intrusive_forward_list& operator=(const intrusive_forward_list&) = default;
  intrusive_forward_list& operator=(intrusive_forward_list&& other) noexcept
  {
    stack = std::move(other.stack);
    tail  = std::exchange(other.tail, nullptr);
    return *this;
  }

  T& front() const { return stack.front(); }
  T& back() const { return *tail; }

  void push_front(T* t)
  {
    stack.push_front(t);
    if (tail == nullptr) {
      tail = t;
    }
  }

  void push_back(T* t)
  {
    static_cast<node_t*>(t)->next_node = nullptr;
    if (tail != nullptr) {
      static_cast<node_t*>(tail)->next_node = static_cast<node_t*>(t);
    } else {
      stack.push_front(t);
    }
    tail = t;
  }

  T* pop_front()
  {
    T* front = stack.pop_front();
    if (stack.empty()) {
      tail = nullptr;
    }
    return front;
  }

  void clear()
  {
    stack.clear();
    tail = nullptr;
  }

  bool empty() const { return stack.empty(); }

  iterator       begin() { return stack.begin(); }
  iterator       end() { return stack.end(); }
  const_iterator begin() const { return stack.begin(); }
  const_iterator end() const { return stack.end(); }

private:
  stack_t stack;
  T*      tail = nullptr;
};

/// Base class of each node of an intrusive_double_linked_list<T>
/// \tparam Tag type used to differentiate separate intrusive lists stored in a single node
template <typename Tag = default_intrusive_tag>
struct intrusive_double_linked_list_element {
  intrusive_double_linked_list_element<Tag>* next_node = nullptr;
  intrusive_double_linked_list_element<Tag>* prev_node = nullptr;
};

/// Double Linked List of pointers of type "T" that doesn't rely on allocations.
/// Instead, it leverages T's internal pointers to store the next and previous nodes
/// @tparam T node type. Must be a subclass of intrusive_double_linked_list_element<Tag>
/// @tparam Tag tag of nodes. Useful to differentiate separate intrusive lists inside the same T node
template <typename T, typename Tag = default_intrusive_tag>
class intrusive_double_linked_list
{
  using node_t = intrusive_double_linked_list_element<Tag>;

  template <typename U>
  class iterator_impl
  {
    using elem_t = std::conditional_t<std::is_const_v<U>, const node_t, node_t>;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = U;
    using difference_type   = std::ptrdiff_t;
    using pointer           = U*;
    using reference         = U&;

    explicit iterator_impl(elem_t* node_ = nullptr) : node(node_) {}
    iterator_impl<U>& operator++()
    {
      node = node->next_node;
      return *this;
    }
    iterator_impl<U>& operator--()
    {
      node = node->prev_node;
      return *this;
    }
    pointer   operator->() { return static_cast<pointer>(node); }
    reference operator*() { return static_cast<reference>(*node); }

    bool operator==(const iterator_impl<U>& other) const { return node == other.node; }
    bool operator!=(const iterator_impl<U>& other) const { return node != other.node; }

  private:
    elem_t* node;
  };

public:
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  intrusive_double_linked_list()
  {
    static_assert(std::is_base_of_v<node_t, T>,
                  "Provided template argument T must have intrusive_forward_list_element<Tag> as base class");
  }
  intrusive_double_linked_list(const intrusive_double_linked_list&) = default;
  intrusive_double_linked_list(intrusive_double_linked_list&& other) noexcept : node(other.node), tail(other.tail)
  {
    other.node = nullptr;
    other.tail = nullptr;
  }
  intrusive_double_linked_list& operator=(const intrusive_double_linked_list&) = default;
  intrusive_double_linked_list& operator=(intrusive_double_linked_list&& other) noexcept
  {
    node       = other.node;
    tail       = other.tail;
    other.node = nullptr;
    other.tail = nullptr;
    return *this;
  }
  ~intrusive_double_linked_list() { clear(); }

  T& front() const { return *static_cast<T*>(node); }
  T& back() const { return *static_cast<T*>(tail); }

  void push_front(T* t)
  {
    node_t* new_head    = static_cast<node_t*>(t);
    new_head->prev_node = nullptr;
    new_head->next_node = node;
    if (node != nullptr) {
      node->prev_node = new_head;
    } else {
      tail = new_head;
    }
    node = new_head;
  }

  void push_back(T* t)
  {
    node_t* new_tail    = static_cast<node_t*>(t);
    new_tail->prev_node = tail;
    new_tail->next_node = nullptr;
    if (tail != nullptr) {
      tail->next_node = new_tail;
    } else {
      node = new_tail;
    }
    tail = new_tail;
  }

  void pop(T* t)
  {
    node_t* to_rem = static_cast<node_t*>(t);
    if (to_rem == node) {
      node = to_rem->next_node;
      if (node == nullptr) {
        tail = nullptr;
      }
    } else if (to_rem == tail) {
      tail = to_rem->prev_node;
      // tail==head checked in first if condition.
    }
    if (to_rem->prev_node != nullptr) {
      to_rem->prev_node->next_node = to_rem->next_node;
    }
    if (to_rem->next_node != nullptr) {
      to_rem->next_node->prev_node = to_rem->prev_node;
    }
    to_rem->next_node = nullptr;
    to_rem->prev_node = nullptr;
  }

  void pop_front() { pop(static_cast<T*>(node)); }

  void pop_back() { pop(static_cast<T*>(tail)); }

  void clear()
  {
    while (node != nullptr) {
      node_t* torem    = node;
      node             = node->next_node;
      torem->next_node = nullptr;
      torem->prev_node = nullptr;
    }
    tail = nullptr;
  }

  bool empty() const { return node == nullptr; }

  iterator       begin() { return iterator(node); }
  iterator       end() { return iterator(nullptr); }
  const_iterator begin() const { return const_iterator(node); }
  const_iterator end() const { return const_iterator(nullptr); }

private:
  node_t* node = nullptr;
  node_t* tail = nullptr;
};

} // namespace ocudu
