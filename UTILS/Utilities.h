#ifndef UTILITIES
#define UTILITIES

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

// Codes of the actions
#define O_ERROR -1
#define O_INFO 1
#define O_FIND 2
#define O_DELETE 3

// Funciones generales
char *EXTRA_read_line(int fd, char last_char);
char *EXTRA_lower_string(char *string);
int EXTRA_equals_ignore_case(char *first, char *second);
char *EXTRA_copy_string(char *string);
char *EXTRA_substring(char *string, int first, int last);

#endif