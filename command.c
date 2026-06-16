#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "file_io.h"

static char g_csv_path[256] = "students.csv";
#ifdef ADMIN_MODE

static int g_dirty = 0;
#endif

void command_set_csv_path(const char *path) {
    if (path != NULL) {
        strncpy(g_csv_path, path, sizeof(g_csv_path) - 1);
        g_csv_path[sizeof(g_csv_path) - 1] = '\0';
    }
}

static int parse_int(const char *s, int *out) {
    if (s == NULL || *s == '\0') {
        return 0;
    }
    char *end;
    long v = strtol(s, &end, 10);
    if (*end != '\0') {
        return 0;
    }
    *out = (int)v;
    return 1;
}

static void print_error(ShellResult r, int line_number) {
    const char *msg;
    switch (r) {
        case SHELL_ERR_UNKNOWN_COMMAND:   msg = "Unknown command or permission denied."; break;
        case SHELL_ERR_INVALID_ARGUMENT:  msg = "Error: invalid argument."; break;
        case SHELL_ERR_MISSING_ARGUMENT:  msg = "Error: missing argument."; break;
        case SHELL_ERR_FILE_OPEN:         msg = "Error: cannot open file."; break;
        case SHELL_ERR_FILE_WRITE:        msg = "Error: cannot write file."; break;
        case SHELL_ERR_STUDENT_NOT_FOUND: msg = "Error: student not found."; break;
        case SHELL_ERR_DUPLICATE_STUDENT: msg = "Error: duplicate ID."; break;
        case SHELL_ERR_INVALID_SCORE:     msg = "Error: invalid score."; break;
        default: return;
    }
    if (line_number > 0) {
        printf("%s Skipped line %d.\n", msg, line_number);
    } else {
        printf("%s\n", msg);
    }
}

static ShellResult handle_reload(char *args, Student **head);
static ShellResult handle_find(char *args, Student **head);
static ShellResult handle_list(char *args, Student **head);
static ShellResult handle_stats(char *args, Student **head);
static ShellResult handle_help(char *args, Student **head);
static ShellResult handle_clear(char *args, Student **head);
static ShellResult handle_exit(char *args, Student **head);
#ifdef ADMIN_MODE
static ShellResult handle_save(char *args, Student **head);
static ShellResult handle_add(char *args, Student **head);
static ShellResult handle_delete(char *args, Student **head);
static ShellResult handle_update(char *args, Student **head);
#endif

#ifdef ADMIN_MODE
static const Command commands[] = {
    {"save",   handle_save,   "save",                    "Save students to CSV"},
    {"reload", handle_reload, "reload",                  "Reload students from CSV"},
    {"add",    handle_add,    "add <id> <name> <score>", "Add a student"},
    {"delete", handle_delete, "delete <id>",             "Delete a student"},
    {"update", handle_update, "update <id> <score>",     "Update student score"},
    {"find",   handle_find,   "find <id>", "Find student by ID"},
    {"list",   handle_list,   "list",      "List all students"},
    {"stats",  handle_stats,  "stats",     "Show statistics"},
    {"help",   handle_help,   "help",      "Show help"},
    {"clear",  handle_clear,  "clear",     "Clear screen"},
    {"exit",   handle_exit,   "exit",      "Exit program"},
};
#elif defined(CLIENT_MODE)
static const Command commands[] = {
    {"reload", handle_reload, "reload",    "Reload students from CSV"},
    {"find",   handle_find,   "find <id>", "Find student by ID"},
    {"list",   handle_list,   "list",      "List all students"},
    {"stats",  handle_stats,  "stats",     "Show statistics"},
    {"help",   handle_help,   "help",      "Show help"},
    {"clear",  handle_clear,  "clear",     "Clear screen"},
    {"exit",   handle_exit,   "exit",      "Exit program"},
};
#endif
static const int num_commands = (int)(sizeof(commands) / sizeof(commands[0]));

static ShellResult handle_reload(char *args, Student **head) {
    (void)args;
    int n = load_students(g_csv_path, head);
    if (n < 0) {
        return SHELL_ERR_FILE_OPEN;
    }
#ifdef ADMIN_MODE
    g_dirty = 0;
#endif
    printf("Reloaded %d students from %s.\n", n, g_csv_path);
    return SHELL_OK;
}

static ShellResult handle_find(char *args, Student **head) {
    char *id_s = strtok(args, " \t");
    if (id_s == NULL) {
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    int id;
    if (!parse_int(id_s, &id) || id <= 0) {
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    Student *s = list_find(*head, id);
    if (s == NULL) {
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    printf("ID: %d\n", s->id);
    printf("Name: %s\n", s->name);
    printf("Score: %d\n", s->score);
    return SHELL_OK;
}

static ShellResult handle_list(char *args, Student **head) {
    (void)args;
    if (*head == NULL) {
        printf("No students found.\n");
        return SHELL_OK;
    }
    printf("ID\tName\tScore\n");
    for (Student *cur = *head; cur != NULL; cur = cur->next) {
        printf("%d\t%s\t%d\n", cur->id, cur->name, cur->score);
    }
    return SHELL_OK;
}

static ShellResult handle_stats(char *args, Student **head) {
    (void)args;
    if (*head == NULL) {
        printf("No student data available.\n");
        return SHELL_OK;
    }
    int count = 0, sum = 0, max = 0, min = 0, first = 1;
    for (Student *cur = *head; cur != NULL; cur = cur->next) {
        count++;
        sum += cur->score;
        if (first || cur->score > max) {
            max = cur->score;
        }
        if (first || cur->score < min) {
            min = cur->score;
        }
        first = 0;
    }
    printf("Count: %d\n", count);
    printf("Average: %.1f\n", (double)sum / count);
    printf("Max: %d\n", max);
    printf("Min: %d\n", min);
    return SHELL_OK;
}

static ShellResult handle_help(char *args, Student **head) {
    (void)args;
    (void)head;
    printf("Commands:\n");
    for (int i = 0; i < num_commands; i++) {
        printf("  %-26s %s\n", commands[i].usage, commands[i].description);
    }
    return SHELL_OK;
}

static ShellResult handle_clear(char *args, Student **head) {
    (void)args;
    (void)head;
    printf("\033[2J\033[H");
    return SHELL_OK;
}

static ShellResult handle_exit(char *args, Student **head) {
    (void)args;
    (void)head;
    printf("Goodbye.\n");
    return SHELL_EXIT;
}

#ifdef ADMIN_MODE
static ShellResult handle_save(char *args, Student **head) {
    (void)args;
    int n = save_students(g_csv_path, *head);
    if (n < 0) {
        return SHELL_ERR_FILE_WRITE;
    }
    g_dirty = 0;
    printf("Saved %d students to %s.\n", n, g_csv_path);
    return SHELL_OK;
}

static ShellResult handle_add(char *args, Student **head) {
    char *id_s = strtok(args, " \t");
    char *name_s = strtok(NULL, " \t");
    char *score_s = strtok(NULL, " \t");
    if (id_s == NULL || name_s == NULL || score_s == NULL) {
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    int id, score;
    if (!parse_int(id_s, &id) || id <= 0) {
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (name_s[0] == '\0' || strchr(name_s, ',') != NULL) {
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (!parse_int(score_s, &score) || score < 0 || score > 100) {
        return SHELL_ERR_INVALID_SCORE;
    }
    if (list_find(*head, id) != NULL) {
        return SHELL_ERR_DUPLICATE_STUDENT;
    }
    Student *s = student_create(id, name_s, score);
    if (s == NULL) {
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    list_append(head, s);
    g_dirty = 1;
    printf("Student added.\n");
    return SHELL_OK;
}

static ShellResult handle_delete(char *args, Student **head) {
    char *id_s = strtok(args, " \t");
    if (id_s == NULL) {
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    int id;
    if (!parse_int(id_s, &id) || id <= 0) {
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (!list_delete(head, id)) {
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    g_dirty = 1;
    printf("Student deleted.\n");
    return SHELL_OK;
}

static ShellResult handle_update(char *args, Student **head) {
    char *id_s = strtok(args, " \t");
    char *score_s = strtok(NULL, " \t");
    if (id_s == NULL || score_s == NULL) {
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    int id, score;
    if (!parse_int(id_s, &id) || id <= 0) {
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (!parse_int(score_s, &score) || score < 0 || score > 100) {
        return SHELL_ERR_INVALID_SCORE;
    }
    Student *s = list_find(*head, id);
    if (s == NULL) {
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    s->score = score;
    g_dirty = 1;
    printf("Student updated.\n");
    return SHELL_OK;
}
#endif

ShellResult command_execute(char *line, Student **head, int line_number) {
    char *cmd = strtok(line, " \t");
    if (cmd == NULL) {
        return SHELL_OK;
    }
    char empty[1];
    empty[0] = '\0';
    char *args = strtok(NULL, "");
    if (args == NULL) {
        args = empty;
    }
    while (*args == ' ' || *args == '\t') {
        args++;
    }

    for (int i = 0; i < num_commands; i++) {
        if (strcmp(cmd, commands[i].name) == 0) {
            ShellResult r = commands[i].handler(args, head);
            if (r != SHELL_OK && r != SHELL_EXIT) {
                print_error(r, line_number);
            }
            return r;
        }
    }
    print_error(SHELL_ERR_UNKNOWN_COMMAND, line_number);
    return SHELL_ERR_UNKNOWN_COMMAND;
}
