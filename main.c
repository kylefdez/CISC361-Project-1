#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"

#define MENU_ADD 1
#define MENU_DELETE 2
#define MENU_PRINT_FORWARD 3
#define MENU_PRINT_BACKWARD 4
#define MENU_EXIT 5

/* Read a whole line and return a string sized to fit it. */
static char *read_line(const char *prompt)
{
    size_t capacity = 32;
    size_t length = 0;
    char *buffer = malloc(capacity);
    char *exact;
    int ch;

    if (buffer == NULL) return NULL;
    fputs(prompt, stdout);
    fflush(stdout);

    /* Grow the temporary buffer when the input is longer than expected. */
    while ((ch = getchar()) != '\n' && ch != EOF) {
        if (length + 1 == capacity) {
            char *larger;
            if (capacity > (size_t)-1 / 2) {
                free(buffer);
                return NULL;
            }
            capacity *= 2;
            larger = realloc(buffer, capacity);
            if (larger == NULL) {
                free(buffer);
                return NULL;
            }
            buffer = larger;
        }
        buffer[length++] = (char)ch;
    }
    if (ch == EOF && length == 0) {
        free(buffer);
        return NULL;
    }
    if (length > 0 && buffer[length - 1] == '\r') --length;
    buffer[length] = '\0';

    /* Keep only the space needed for the text and its null terminator. */
    exact = malloc(length + 1);
    if (exact == NULL) {
        free(buffer);
        return NULL;
    }
    memcpy(exact, buffer, length + 1);
    free(buffer);
    return exact;
}

/* Ask again if the user enters only whitespace. */
static char *read_nonblank(const char *prompt)
{
    for (;;) {
        char *value = read_line(prompt);
        if (value == NULL) return NULL;
        if (strspn(value, " \t\r\v\f") != strlen(value)) return value;
        puts("Please enter a nonempty value.");
        free(value);
    }
}

/* Parse a number and make sure it is within the requested range. */
static int read_number(const char *prompt, long min, long max, long *result)
{
    for (;;) {
        char *line = read_line(prompt);
        char *end;
        long value;
        int has_digits;

        if (line == NULL) return 0;
        errno = 0;
        value = strtol(line, &end, 10);
        has_digits = end != line;
        end += strspn(end, " \t\r\v\f");
        if (has_digits && *end == '\0' && errno != ERANGE &&
            value >= min && value <= max) {
            *result = value;
            free(line);
            return 1;
        }
        puts("Please enter a valid number in the requested range.");
        free(line);
    }
}

/* Gather one student's data and add the new record at the tail. */
static int add_student(slist_t *list)
{
    char *lastname = NULL;
    char *firstname = NULL;
    char *year = NULL;
    long id;
    long grad_year;
    student_t *student;

    lastname = read_nonblank("Last name: ");
    if (lastname == NULL) goto incomplete;
    firstname = read_nonblank("First name: ");
    if (firstname == NULL) goto incomplete;
    if (!read_number("Student ID: ", LONG_MIN, LONG_MAX, &id)) goto incomplete;
    year = read_nonblank("Year (e.g. freshman): ");
    if (year == NULL) goto incomplete;
    if (!read_number("Expected graduation year: ", 1, INT_MAX,
                     &grad_year)) goto incomplete;

    /* student_create copies the strings, so these input strings can be freed. */
    student = student_create(lastname, firstname, id, year, (int)grad_year);
    if (student == NULL) goto incomplete;
    if (!slist_add(list, student)) {
        student_free(student);
        goto incomplete;
    }
    free(lastname);
    free(firstname);
    free(year);
    puts("Student added.");
    return 1;

incomplete:
    /* Also release any fields read before input ended. */
    free(lastname);
    free(firstname);
    free(year);
    return 0;
}

/* Display the five menu choices. */
static void print_menu(void)
{
    puts("\n======== Project 1a: Student List ========\n"
         "  1. Add a student\n"
         "  2. Delete student(s) by last name\n"
         "  3. Print the list from beginning to end\n"
         "  4. Print the list from end to beginning\n"
         "  5. Exit\n"
         "==========================================");
}

int main(void)
{
    slist_t *list = slist_create();
    long choice;

    if (list == NULL) {
        fputs("Could not allocate the student list.\n", stderr);
        return EXIT_FAILURE;
    }
    for (;;) {
        print_menu();
        if (!read_number("Choice: ", MENU_ADD, MENU_EXIT, &choice)) break;

        switch (choice) {
        case MENU_ADD:
            if (!add_student(list)) goto done;
            break;
        case MENU_DELETE: {
            char *lastname = read_nonblank("Last name to delete: ");
            size_t removed;
            if (lastname == NULL) goto done;
            removed = slist_delete_by_lastname(list, lastname);
            printf("Removed %zu student(s).\n", removed);
            free(lastname);
            break;
        }
        case MENU_PRINT_FORWARD:
            slist_print_forward(list);
            break;
        case MENU_PRINT_BACKWARD:
            slist_print_backward(list);
            break;
        case MENU_EXIT:
            goto done;
        }
    }

done:
    /* Free every remaining student before the program exits. */
    slist_destroy(list);
    puts("Goodbye.");
    return EXIT_SUCCESS;
}
