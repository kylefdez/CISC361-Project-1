#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"

/* Make an exact-size copy of a string for a student record. */
static char *dup_string(const char *s)
{
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);

    if (copy != NULL) memcpy(copy, s, len);
    return copy;
}

/* Allocate a student and give it its own copies of the strings. */
student_t *student_create(const char *lastname, const char *firstname,
                          long id, const char *year, int grad_year)
{
    student_t *node = malloc(sizeof(student_t));

    if (node == NULL) return NULL;
    node->id = id;
    node->grad_year = grad_year;
    node->next = node->prev = NULL;
    /* NULL pointers are safe to free if a later allocation fails. */
    node->lastname = node->firstname = node->year = NULL;
    node->lastname  = dup_string(lastname);
    node->firstname = dup_string(firstname);
    node->year      = dup_string(year);
    if (node->lastname == NULL || node->firstname == NULL ||
        node->year == NULL) {
        student_free(node);
        return NULL;
    }
    return node;
}

/* Free the strings before freeing the student that points to them. */
void student_free(student_t *node)
{
    if (node == NULL) return;
    free(node->lastname);
    free(node->firstname);
    free(node->year);
    free(node);
}

void student_print(const student_t *node)
{
    if (node == NULL) return;
    printf("  Last name  : %s\n", node->lastname);
    printf("  First name : %s\n", node->firstname);
    printf("  ID number  : %ld\n", node->id);
    printf("  Year       : %s\n", node->year);
    printf("  Grad year  : %d\n", node->grad_year);
}

/* Start an empty list with no head or tail. */
slist_t *slist_create(void)
{
    slist_t *list = malloc(sizeof(slist_t));

    if (list == NULL) return NULL;
    list->head = list->tail = NULL;
    list->count = 0;
    return list;
}

/* Free every record, followed by the list itself. */
void slist_destroy(slist_t *list)
{
    student_t *node = (list != NULL) ? list->head : NULL;

    while (node != NULL) {
        student_t *next = node->next;   /* Save the link before freeing node. */
        student_free(node);
        node = next;
    }
    free(list);                        
}

/* Connect a new student after the current tail. */
int slist_add(slist_t *list, student_t *node)
{
    if (list == NULL || node == NULL) return 0;
    node->next = NULL;                  
    node->prev = list->tail;
    if (list->tail != NULL) list->tail->next = node;
    else list->head = node;            
    list->tail = node;
    list->count++;
    return 1;
}

/* Remove every student whose last name matches exactly. */
size_t slist_delete_by_lastname(slist_t *list, const char *lastname)
{
    student_t *node;
    size_t removed = 0;

    if (list == NULL || lastname == NULL) return 0;
    node = list->head;
    while (node != NULL) {
        student_t *next = node->next;   /* Needed after a matching node is freed. */

        if (strcmp(node->lastname, lastname) == 0) {
            /* Relink both neighbors, updating the head or tail if needed. */
            if (node->prev != NULL) node->prev->next = node->next;
            else list->head = node->next;
            if (node->next != NULL) node->next->prev = node->prev;
            else list->tail = node->prev;
            student_free(node);
            list->count--;
            removed++;
        }
        node = next;
    }
    return removed;
}

/* Traverse through next or prev, depending on the requested direction. */
static void print_records(const slist_t *list, const student_t *node,
                          int forward, const char *heading)
{
    size_t position = 0;

    printf("\nPrinting from the %s (%zu record%s):\n", heading, list->count,
           list->count == 1 ? "" : "s");
    for (; node != NULL; node = forward ? node->next : node->prev) {
        position++;
        printf("\n  --- Student %zu of %zu ---\n", position, list->count);
        student_print(node);
    }
    printf("\nEnd of the list.\n");
}

void slist_print_forward(const slist_t *list)
{
    if (list == NULL || list->head == NULL) printf("\nThe list is empty.\n");
    else print_records(list, list->head, 1, "beginning to the end");
}

void slist_print_backward(const slist_t *list)
{
    if (list == NULL || list->tail == NULL) printf("\nThe list is empty.\n");
    else print_records(list, list->tail, 0, "end to the beginning");
}
