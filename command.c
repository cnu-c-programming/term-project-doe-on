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

static ShellResult handle_list(char *args, Student **head);
static ShellResult handle_exit(char *args, Student **head);

#ifdef ADMIN_MODE
static const Command commands[] = {
    {"list", handle_list, "list", "List all students"},
    {"exit", handle_exit, "exit", "Exit program"},
};
#elif defined(CLIENT_MODE)
static const Command commands[] = {
    {"list", handle_list, "list", "List all students"},
    {"exit", handle_exit, "exit", "Exit program"},
};
#endif
static const int num_commands = (int)(sizeof(commands) / sizeof(commands[0]));

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
