#ifndef _LIST_H
#define _LIST_H

struct LIST_NODE {
  struct LIST_NODE *next, *prev;
};

#define LIST_HEAD_INIT(name)                                                   \
  { &(name), &(name) }

#define LIST_HEAD(name) struct LIST_NODE name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct LIST_NODE *list) {
  list->next = list;
  list->prev = list;
}

static inline void __list_add(struct LIST_NODE *new, struct LIST_NODE *prev,
                              struct LIST_NODE *next) {
  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

static inline void list_add(struct LIST_NODE *new, struct LIST_NODE *head) {
  __list_add(new, head, head->next);
}

static inline void list_add_tail(struct LIST_NODE *new,
                                 struct LIST_NODE *head) {
  __list_add(new, head->prev, head);
}

static inline void __list_del(struct LIST_NODE *prev, struct LIST_NODE *next) {
  next->prev = prev;
  prev->next = next;
}

static inline void list_del(struct LIST_NODE *entry) {
  __list_del(entry->prev, entry->next);
}

static inline void __list_del_entry(struct LIST_NODE *entry) {
  __list_del(entry->prev, entry->next);
}

static inline void list_del_init(struct LIST_NODE *entry) {
  __list_del_entry(entry);
  INIT_LIST_HEAD(entry);
}

static inline void list_replace(struct LIST_NODE *old, struct LIST_NODE *new) {
  new->next = old->next;
  new->next->prev = new;
  new->prev = old->prev;
  new->prev->next = new;
}

static inline int list_empty(const struct LIST_NODE *head) {
  return head->next == head;
}

#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offsetof(type, member));                         \
  })

#define list_for_each(pos, head)                                               \
  for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head)                                       \
  for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#endif
