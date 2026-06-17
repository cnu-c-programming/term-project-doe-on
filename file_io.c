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

    char line[256];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fprintf(stderr, "Error: invalid CSV header.\n");
        fclose(fp);
        return -1;
    }
    line[strcspn(line, "\r\n")] = '\0';
    if (strcmp(line, "id,name,score") != 0) {
        fprintf(stderr, "Error: invalid CSV header.\n");
        fclose(fp);
        return -1;
    }

    list_free(head);

    int count = 0;
    int lineno = 1;
    while (fgets(line, sizeof(line), fp) != NULL) {
        lineno++;
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') {
            continue;
        }

        char *comma1 = strchr(line, ',');
        char *comma2 = (comma1 != NULL) ? strchr(comma1 + 1, ',') : NULL;
        if (comma1 == NULL || comma2 == NULL) {
            fprintf(stderr, "Error: invalid CSV format at line %d.\n", lineno);
            continue;
        }
        *comma1 = '\0';
        *comma2 = '\0';
        const char *id_s = line;
        const char *name = comma1 + 1;
        const char *score_s = comma2 + 1;

        char *end;
        long id = strtol(id_s, &end, 10);
        if (*end != '\0' || id <= 0) {
            fprintf(stderr, "Error: invalid id at line %d.\n", lineno);
            continue;
        }
        long score = strtol(score_s, &end, 10);
        if (*end != '\0' || score < 0 || score > 100) {
            fprintf(stderr, "Error: invalid score at line %d.\n", lineno);
            continue;
        }
        if (name[0] == '\0') {
            fprintf(stderr, "Error: invalid name at line %d.\n", lineno);
            continue;
        }

        Student *s = student_create((int)id, name, (int)score);
        if (s == NULL) {
            fprintf(stderr, "Error: out of memory.\n");
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
        char *comma1 = strchr(line, ',');
        char *comma2 = (comma1 != NULL) ? strchr(comma1 + 1, ',') : NULL;
        if (comma1 == NULL || comma2 == NULL) {
            fprintf(stderr, "Error: invalid CSV format at line %d.\n", lineno);
            continue;
        }
        *comma1 = '\0';
        *comma2 = '\0';
        const char *id_s = line;
        const char *name = comma1 + 1;
        const char *score_s = comma2 + 1;

        char *end;
        long id = strtol(id_s, &end, 10);
        if (*end != '\0' || id <= 0) {
            fprintf(stderr, "Error: invalid id at line %d.\n", lineno);
            continue;
        }
        long score = strtol(score_s, &end, 10);
        if (*end != '\0' || score < 0 || score > 100) {
            fprintf(stderr, "Error: invalid score at line %d.\n", lineno);
            continue;
        }
        if (name[0] == '\0') {
            fprintf(stderr, "Error: invalid name at line %d.\n", lineno);
            continue;
        }

        Student *s = student_create((int)id, name, (int)score);
        if (s == NULL) {
            fprintf(stderr, "Error: out of memory.\n");
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
