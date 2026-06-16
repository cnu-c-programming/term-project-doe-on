#ifndef FILE_IO_H
#define FILE_IO_H

#include "student.h"

int load_students(const char *path, Student **head);

int save_students(const char *path, Student *head);

#endif
