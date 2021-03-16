/*  list.c

    A doubly linked list is represented by list elements that have pointers to
    the next and previous list element.

    List structs have a head and tail list element that do not store data, and 
    only serve to mark out the begining and end of a list. 

    Linked lists can represent queues, stacks, or regular lists 
    
    Author: Joseph Shimel
    Date: 1/31/21
*/

#include "include.h"

/*  Creates a new list, with no new nodes. */
struct list* list_create() {
    struct list* list = (struct list*)calloc(1, sizeof(struct list));

    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    
    return list;
}

/*  Gives the starting point of the list */
struct listElem* list_begin(struct list* list) {
    return list->head.next;
}

/*  Returns the tail of the linked list, can be used to detect when a loop has
    reached the end of the list during traversal */
struct listElem* list_end(struct list* list) {
    return &list->tail;
}

/*
    Inserts a new list element after a given list element. */
void list_insert(struct list* list, struct listElem* before, void* data) {
    struct listElem* elem = (struct listElem*)malloc(sizeof(struct listElem));
    elem->data = data;
    elem->prev = before->prev;
    elem->next = before;
    before->prev->next = elem;
    before->prev = elem;
}

void* list_get(struct list* list, int i) {
    struct listElem* elem = list_begin(list);
    for(;i > 0 && elem != list_end(list); i--) {
        elem = elem->next;
    }
    return elem->data;
}

/* Removes the first element in the list, returns data pointer */
void* list_pop(struct list* list) {
    struct listElem* elem = list_begin(list);
    struct listElem* next = list_begin(list)->next;
    void* retval = list_begin(list)->data;

    // Swap pointers, free old node
    list->head.next = next;
    next->prev = &(list->head);
    free(elem);

    return retval;
}

/* Returns data pointer of the first element in the list */
void* list_peek(struct list* list) {
    return list_begin(list)->data;
}

/*  Appends a listElem to the end of the queue, with the given data 
    pointer */
void queue_push(struct list* list, void* data) {
    list_insert(list, list_end(list), data);
}

/* Inserts an element at the top of a stack */
void stack_push(struct list* list, void* data) {
    list_insert(list, list_begin(list), data);
}

/* 
    Returns true if the list head points directly to the list tail */
int list_isEmpty(struct list* list) {
    return list_begin(list) == list_end(list);
}