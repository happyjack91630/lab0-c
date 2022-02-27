#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new) {
        return NULL;  // malloc fail
    }
    INIT_LIST_HEAD(new);
    return new;
}


/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l == NULL) {
        return;
    }
    struct list_head *tmp = l->next;
    while (tmp != l) {
        element_t *del;
        del = container_of(tmp, element_t, list);
        tmp = tmp->next;
        free(del->value);
        free(del);
    }
    free(tmp);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL) {
        return false;
    }
    element_t *new = malloc(sizeof(element_t));
    if (new == NULL) {
        return false;
    }
    int char_size = strlen(s);
    new->value = malloc(char_size + 1);
    if (new->value == NULL) {
        free(new);
        return false;
    }
    memcpy(new->value, s, char_size);
    new->value[char_size] = '\0';
    list_add(&new->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL) {
        return false;
    }
    element_t *new = malloc(sizeof(element_t));
    if (new == NULL) {
        return false;
    }
    int char_size = strlen(s);
    new->value = malloc(char_size + 1);
    if (new->value == NULL) {
        free(new);
        return false;
    }
    memcpy(new->value, s, char_size);
    new->value[char_size] = '\0';
    list_add_tail(&new->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head)) {
        return NULL;
    }
    element_t *remove_head =
        list_first_entry(head, element_t, list);  // for head entry
    if (remove_head == NULL) {
        return NULL;
    }
    list_del(&remove_head->list);
    if (sp) {
        memcpy(sp, remove_head->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove_head;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head)) {
        return NULL;
    }
    element_t *remove_tail =
        list_last_entry(head, element_t, list);  // for tail entry
    if (remove_tail == NULL) {
        return NULL;
    }
    list_del(&remove_tail->list);
    if (sp) {
        memcpy(sp, remove_tail->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove_tail;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (head == NULL) {
        return 0;
    }
    int length = 0;
    struct list_head *node;
    list_for_each (node, head) {
        length++;
    }
    return length;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head)) {
        return false;
    }
    struct list_head *slow_ptr = head;
    struct list_head *fast_ptr = head;
    do {
        slow_ptr = slow_ptr->next;
        fast_ptr = fast_ptr->next->next;
    } while (fast_ptr->next != head && fast_ptr != head);
    element_t *mid_node =
        list_entry(slow_ptr, element_t, list);  // slow_ptr will be the middle
    list_del(slow_ptr);
    free(mid_node->value);
    free(mid_node);
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    if (head == NULL || list_empty(head)) {
        return false;
    }
    struct list_head *cur = head->next;
    element_t *cur_e;
    bool flag = false;  // record is cur duplicate ?
    while (cur->next != head) {
        cur_e = list_entry(cur, element_t, list);
        element_t *cur_next_e = list_entry(cur->next, element_t, list);

        if (strcmp(cur_e->value, cur_next_e->value) == 0) {
            struct list_head *cur_next_next = cur->next->next;
            cur->next = cur_next_next;
            cur_next_next->prev = cur;
            free(cur_next_e->value);
            free(cur_next_e);
            flag =
                true;  // cur_e->value == cur_next_e->value, so cur is duplicate
        } else {
            if (flag) {  // cur is duplicate so need to be delete
                cur->next->prev = cur->prev;
                cur->prev->next = cur->next;
                cur = cur->next;
                free(cur_e->value);
                free(cur_e);
            } else {
                cur = cur->next;
                flag = false;
            }
        }
    }
    if (flag) {  // queue only left one item(cur) and this item also duplicate
        cur->next->prev = cur->prev;
        cur->prev->next = cur->next;
        // cur = cur->next;
        free(cur_e->value);
        free(cur_e);
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (head == NULL) {
        return;
    }
    struct list_head *cur = head->next;
    struct list_head *prev = head;
    struct list_head *next = cur->next;
    int n = 0;
    while (cur->next != head && cur != head) {
        cur->next = next->next;
        cur->prev = next;
        next->next = cur;
        next->prev = prev;
        prev->next = next;
        // prev->prev =cur;
        prev = cur;
        cur = cur->next;
        next = cur->next;
        n++;
    }
    if (n % 2 == 0) {
        head->prev = cur;
    } else {
        cur->prev = prev;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head)) {
        return;
    }
    struct list_head *cur = head;
    struct list_head *tmp;
    do {
        tmp = cur->prev;
        cur->prev = cur->next;
        cur->next = tmp;
        cur = cur->prev;
    } while (cur != head);
}


struct list_head *mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **cur;
    for (cur = NULL; L1 && L2; *cur = (*cur)->next) {
        // compare accending pair by pair
        element_t *node1 = container_of(L1, element_t, list);
        element_t *node2 = container_of(L2, element_t, list);
        cur = (strcmp(node1->value, node2->value) <= 0) ? &L1 : &L2;
        *ptr = *cur;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    return head;
}


static struct list_head *mergesort_list(struct list_head *head)
{
    if (!head || !head->next) {
        return head;
    }
    struct list_head *slow = head;
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next) {
        slow = slow->next;
    }
    struct list_head *mid = slow->next;
    slow->next = NULL;
    struct list_head *left = mergesort_list(head);
    struct list_head *right = mergesort_list(mid);

    return mergeTwoLists(left, right);
}
/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (head == NULL || list_empty(head)) {
        return;
    }
    head->prev->next = NULL;  // circular to non-circular
    head->next = mergesort_list(head->next);


    struct list_head *ptr = head;
    while (ptr->next) {  // reconstruct circular link list (prev)
        ptr->next->prev = ptr;
        ptr = ptr->next;
    }
    ptr->next = head;
    head->prev = ptr;
}