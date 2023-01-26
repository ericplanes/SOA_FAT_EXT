#include "Utilities.h"

// Llegeix una cadena de caracters
char *EXTRA_read_line(int fd, char last_char)
{
    char *string;
    char a_char;
    int i = 0;

    string = malloc(sizeof(char));

    // Bucle per llegir tota la linia
    while (read(fd, &a_char, sizeof(char)) != 0 && a_char != last_char)
    {
        string[i] = a_char;
        string = realloc(string, (i + 2) * sizeof(char));
        i++;
    }

    // Tanquem el string amb un \0
    string[i] = '\0';

    // Retornem el string que hem guardat
    return string;
}

// Compares 2 strings, returns 1 if they are the same, 0 if not (ignorecase)
int EXTRA_equals_ignore_case(char *first, char *second)
{
    if (strlen(first) < 1)
    {
        return 0;
    }

    if (strlen(second) < 1)
    {
        return 0;
    }

    int return_value;
    char *lowered_first = EXTRA_lower_string(first);
    char *lowered_second = EXTRA_lower_string(second);

    return_value = strcmp(lowered_first, lowered_second);

    free(lowered_first);
    free(lowered_second);

    if (return_value == 0)
    {
        return 1;
    }

    return 0;
}

// Lowers all the string line
char *EXTRA_lower_string(char *string)
{
    int i;
    char *lowered = malloc(((int)strlen(string) + 1) * sizeof(char));

    for (i = 0; i < (int)strlen(string); i++)
    {
        lowered[i] = tolower(string[i]);
    }

    lowered[i] = '\0';

    return lowered;
}

// Returns a copy of the provided string
char *EXTRA_copy_string(char *string)
{
    int i;
    char *copy;
    copy = malloc(sizeof(char));
    for (i = 0; i < (int)strlen(string); i++)
    {
        copy[i] = string[i];
        copy = realloc(copy, (i + 2) * sizeof(char));
    }
    copy[i] = '\0';
    return copy;
}

// Substrings a string from first to last
char *EXTRA_substring(char *string, int first, int last)
{

    // Function
    int i, j;
    char *substr;
    substr = malloc(sizeof(char));

    // Check
    if (strlen(string) < 1)
    {
        substr[0] = '\0';
        return substr;
    }

    i = first;
    for (j = 0; i <= last && i < strlen(string); i++, j++)
    {
        substr[j] = string[i];
        substr = realloc(substr, (j + 2) * sizeof(char));
    }
    substr[j] = '\0';
    return substr;
}
