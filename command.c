#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

static char g_csv_path[256] = "students.csv";

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

static ShellResult handle_find(char *args, Student **head);
static ShellResult handle_list(char *args, Student **head);
static ShellResult handle_stats(char *args, Student **head);
static ShellResult handle_help(char *args, Student **head);
static ShellResult handle_clear(char *args, Student **head);
static ShellResult handle_exit(char *args, Student **head);

#ifdef ADMIN_MODE
static const Command commands[] = {
    {"find",  handle_find,  "find <id>", "Find student by ID"},
    {"list",  handle_list,  "list",      "List all students"},
    {"stats", handle_stats, "stats",     "Show statistics"},
    {"help",  handle_help,  "help",      "Show help"},
    {"clear", handle_clear, "clear",     "Clear screen"},
    {"exit",  handle_exit,  "exit",      "Exit program"},
};
#elif defined(CLIENT_MODE)
static const Command commands[] = {
    {"find",  handle_find,  "find <id>", "Find student by ID"},
    {"list",  handle_list,  "list",      "List all students"},
    {"stats", handle_stats, "stats",     "Show statistics"},
    {"help",  handle_help,  "help",      "Show help"},
    {"clear", handle_clear, "clear",     "Clear screen"},
    {"exit",  handle_exit,  "exit",      "Exit program"},
};
#endif
static const int num_commands = (int)(sizeof(commands) / sizeof(commands[0]));

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
