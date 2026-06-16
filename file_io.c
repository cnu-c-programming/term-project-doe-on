#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_io.h"

int load_students(const char *path, Student **head) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: cannot open file '%s'.\n", path);
        return -1;
    }

    list_free(head);

    char line[256];
    int count = 0;
    int is_header = 1;
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';

        if (is_header) {
            is_header = 0;
            continue;
        }
        if (line[0] == '\0') {
            continue;
        }

        char *comma1 = strchr(line, ',');
        char *comma2 = (comma1 != NULL) ? strchr(comma1 + 1, ',') : NULL;
        if (comma1 == NULL || comma2 == NULL) {
            continue;
        }
        *comma1 = '\0';
        *comma2 = '\0';

        int id = atoi(line);
        const char *name = comma1 + 1;
        int score = atoi(comma2 + 1);

        Student *s = student_create(id, name, score);
        if (s == NULL) {
            fclose(fp);
            return -1;
        }
        list_append(head, s);
        count++;
    }

    fclose(fp);
    return count;
}

int save_students(const char *path, Student *head) {
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: cannot write file '%s'.\n", path);
        return -1;
    }

    fprintf(fp, "id,name,score\n");
    int count = 0;
    for (Student *cur = head; cur != NULL; cur = cur->next) {
        fprintf(fp, "%d,%s,%d\n", cur->id, cur->name, cur->score);
        count++;
    }

    fclose(fp);
    return count;
}
