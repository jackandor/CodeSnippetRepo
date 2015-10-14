#ifndef LINKEDLIST_H
#define LINKEDLIST_H
struct link_list {
    struct link_list *prev, *next;
};

#define offset_of(type, member) \
    (unsigned int)(&(((type *)0)->member))

#define container_of(ptr, type, member) \
    ((type *)((unsigned int)(ptr) - offset_of(type, member)))

inline void init_link_list(link_list &p) {
    p.prev = NULL;
    p.next = NULL;
}

inline void push_front(link_list * &head, link_list &l) {
    l.next = head;
    if (head)
        head->prev = &l;
    head = &l;
}

inline link_list *pop_back(link_list * &tail) {
    link_list *ret = tail;
    tail->prev->next = NULL;
    tail = tail->prev;
    return ret;
}

#endif //LINKEDLIST_H
