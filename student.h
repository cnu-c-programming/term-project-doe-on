#ifndef STUDENT_H
#define STUDENT_H

typedef struct Student {
    int id;
    char name[32];
    int score;
    struct Student *next;
} Student;

Student *student_create(int id, const char *name, int score);

void list_append(Student **head, Student *node);

int list_count(const Student *head);

Student *list_find(Student *head, int id);

int list_delete(Student **head, int id);

void list_free(Student **head);

void list_sort_by_name(Student **head);

void list_sort_by_score(Student **head);

#endif
