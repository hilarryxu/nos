#ifndef _NOS_LIBS_LIST_H
#define _NOS_LIBS_LIST_H

#include <stdbool.h>

#include <nos/macros.h>

struct list_entry {
  struct list_entry *prev, *next;
};

static __always_inline void
__list_del(struct list_entry *prev, struct list_entry *next)
{
  prev->next = next;
  next->prev = prev;
}

static __always_inline void
__list_add(struct list_entry *elem, struct list_entry *prev,
           struct list_entry *next)
{
  prev->next = next->prev = elem;
  elem->next = next;
  elem->prev = prev;
}

static __always_inline void
list_init(struct list_entry *list)
{
  list->prev = list->next = list;
}

static __always_inline bool
list_is_empty(struct list_entry *list)
{
  return list->next == list;
}

static __always_inline struct list_entry *
list_prev(struct list_entry *elem)
{
  return elem->prev;
}

static __always_inline struct list_entry *
list_next(struct list_entry *elem)
{
  return elem->next;
}

static __always_inline void
list_add_before(struct list_entry *elem, struct list_entry *added_elem)
{
  __list_add(added_elem, elem->prev, elem);
}

static __always_inline void
list_add_after(struct list_entry *elem, struct list_entry *added_elem)
{
  __list_add(added_elem, elem, elem->next);
}

static __always_inline void
list_del(struct list_entry *elem)
{
  __list_del(elem->prev, elem->next);
}

#endif  // !_NOS_LIBS_LIST_H
