#ifndef LINKEDLIST_H
#define LINKEDLIST_H
struct linkedlist {
    linkedlist *prev, *next;
};

#define offset_of(type, member) \
    (unsigned int)(&(((type *)0)->member))

#define container_of(ptr, type, member) \
    ((type *)((unsigned int)(ptr) - offset_of(type, member)))

inline void init_linkedlist(linkedlist &p) {
    p.prev = NULL;
    p.next = NULL;
}

inline void push_front(linkedlist * &head, linkedlist &l) {
    l.next = head;
    if (head)
        head->prev = &l;
    head = &l;
}

inline linkedlist *pop_back(linkedlist * &tail) {
    linkedlist *ret = tail;
    tail->prev->next = NULL;
    tail = tail->prev;
    return ret;
}

#endif //LINKEDLIST_H
