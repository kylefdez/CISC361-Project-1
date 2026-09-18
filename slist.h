#ifndef SLIST_H
#define SLIST_H
#include <stddef.h>

#define BUFFERSIZE 128         

typedef struct student {
    char *lastname;       /*Last name that is dynamically allocated*/
    char *firstname;      /*First name that is dynamically allocated*/
    long  id;
    char *year;           /*Graduation year that is dynamically allocated*/   
    int   grad_year;            
    struct student *next; /*Next node toward the tail*/
    struct student *prev; /*Prev node toward the head*/
} student_t;

typedef struct slist {
    student_t *head;
    student_t *tail;
    size_t     count;
} slist_t;

student_t *student_create(const char *lastname, const char *firstname, long id, const char *year, int grad_year);
void       student_free(student_t *student);
void       student_print(const student_t *student);
slist_t *slist_create(void);
void     slist_destroy(slist_t *list);
int      slist_add(slist_t *list, student_t *student); /*Adds a student node to the current tail*/
size_t   slist_delete_by_lastname(slist_t *list, const char *lastname); /*Removes student nodes that match the last name given*/
void     slist_print_forward(const slist_t *list); /*Prints the whole list, starting from the head and moving down*/
void     slist_print_backward(const slist_t *list); /*Prints the whole list, starting from the tail and moving up*/

#endif