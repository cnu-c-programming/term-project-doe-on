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
