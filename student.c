#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

Student *student_create(int id, const char *name, int score) {
    Student *s = (Student *)malloc(sizeof(Student));
    if (s == NULL) {
        return NULL;
    }
    s->id = id;
    strncpy(s->name, name, sizeof(s->name) - 1);
    s->name[sizeof(s->name) - 1] = '\0';
    s->score = score;
    s->next = NULL;
    return s;
}

void list_append(Student **head, Student *node) {
    if (head == NULL || node == NULL) {
        return;
    }
    if (*head == NULL) {
        *head = node;
        return;
    }
    Student *cur = *head;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = node;
}

int list_count(const Student *head) {
    int n = 0;
    for (const Student *cur = head; cur != NULL; cur = cur->next) {
        n++;
    }
    return n;
}

Student *list_find(Student *head, int id) {
    for (Student *cur = head; cur != NULL; cur = cur->next) {
        if (cur->id == id) {
            return cur;
        }
    }
    return NULL;
}

int list_delete(Student **head, int id) {
    if (head == NULL) {
        return 0;
    }
    Student *cur = *head;
    Student *prev = NULL;
    while (cur != NULL) {
        if (cur->id == id) {
            if (prev == NULL) {
                *head = cur->next;
            } else {
                prev->next = cur->next;
            }
            free(cur);
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

void list_free(Student **head) {
    if (head == NULL) {
        return;
    }
    Student *cur = *head;
    while (cur != NULL) {
        Student *next = cur->next;
        free(cur);
        cur = next;
    }
    *head = NULL;
}

static void swap_records(Student *a, Student *b) {
    int tmp_id = a->id;
    int tmp_score = a->score;
    char tmp_name[32];
    strcpy(tmp_name, a->name);

    a->id = b->id;
    a->score = b->score;
    strcpy(a->name, b->name);

    b->id = tmp_id;
    b->score = tmp_score;
    strcpy(b->name, tmp_name);
}

void list_sort_by_name(Student **head) {
    if (head == NULL || *head == NULL) {
        return;
    }
    int swapped;
    do {
        swapped = 0;
        for (Student *cur = *head; cur->next != NULL; cur = cur->next) {
            if (strcmp(cur->name, cur->next->name) > 0) {
                swap_records(cur, cur->next);
                swapped = 1;
            }
        }
    } while (swapped);
}

void list_sort_by_score(Student **head) {
    if (head == NULL || *head == NULL) {
        return;
    }
    int swapped;
    do {
        swapped = 0;
        for (Student *cur = *head; cur->next != NULL; cur = cur->next) {
            if (cur->score > cur->next->score) {
                swap_records(cur, cur->next);
                swapped = 1;
            }
        }
    } while (swapped);
}
