#include "sort.h"
#include <stddef.h>

struct Node* sortedmerge(struct Node* a, struct Node* b, int (*compare)(void *, void *));
void frontbacksplit(struct Node* source, struct Node** frontRef, struct Node** backRef);

/* sort linked list *list using merge insertion sort. */
/* upon success, the elements in *list will be sorted. */
/* return silently if *list is uninitialised or empty. */
/* the compare argument is a pointer to a function which returns */
/* less than 0, 0, or greater than 0 if first argument is */
/* less than, equal to, or greater than second argument respectively. */
void insertion_sort(LinkedList *list, int (*compare)(void *, void *))
{
    Node* i = list->head->next;
    while (i != NULL)
    {
        Node* key = i;
        Node* j = list->head;

        while (j != i)
        {
            if (compare(key->data, j->data) < 0)
            {
                char* temp = key->data;
                key->data = j->data;
                j->data = temp;
            }
            j=j->next;
        }
        i=i->next;
    }
}

void mergesort(struct Node** headRef, int (*compare)(void *, void *))
{
    struct Node* head = *headRef;
    struct Node* a;
    struct Node* b;
    if ((head == NULL) || (head -> next == NULL))
    {
        return;
    }
    frontbacksplit(head, &a, &b);
    mergesort(&a, compare);
    mergesort(&b, compare);
    *headRef = sortedmerge(a, b, compare);
}
 
struct Node* sortedmerge(struct Node* a, struct Node* b, int (*compare)(void *, void *))
{
    struct Node* result = NULL;
 
    if (a == NULL)
        return(b);
    else if (b == NULL)
        return(a);
 
    if (compare(a->data, b->data) <= 0)
    {
        result = a;
        result -> next = sortedmerge(a -> next, b, compare);
    }
    else
    {
        result = b;
        result -> next = sortedmerge(a, b -> next, compare);
    }
    return(result);
}
 
void frontbacksplit(struct Node* source, struct Node** frontRef, struct Node** backRef)
{
    struct Node* fast;
    struct Node* slow;
    if (source==NULL || source->next==NULL)
    {
        *frontRef = source;
        *backRef = NULL;
    }
    else
    {
        slow = source;
        fast = source -> next;
        while (fast != NULL)
        {
            fast = fast -> next;
            if (fast != NULL)
            {
                slow = slow -> next;
                fast = fast -> next;
            }
        }
 
        *frontRef = source;
        *backRef = slow -> next;
        slow -> next = NULL;
    }
}

/* sort linked list *list using merge sort algorithm. */
/* upon success, the elements in *list will be sorted. */
/* return silently if *list is uninitialised or empty. */
/* the compare argument is a pointer to a function which returns */
/* less than 0, 0, or greater than 0 if first argument is */
/* less than, equal to, or greater than second argument respectively. */
void merge_sort(LinkedList *list, int (*compare)(void *, void *))
{
    mergesort(&list->head, compare);
}
