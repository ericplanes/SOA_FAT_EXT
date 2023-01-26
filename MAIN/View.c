#include "View.h"

int main(int argc, char *argv[])
{
    int petition_type;

    printf("\n\n");

    // Mirem que tingui un argument i un parametre minim
    if (argc < 3)
    {
        printf(ERROR_NUM_PARAM_MSG);
        return -1;
    }

    // Mirem el tipus de peticio que ens han fet i la gestionem
    petition_type = get_petition_type(argv[1]);
    if (petition_type == O_ERROR)
    {
        printf(ERROR_PETITION_UNKNNOWN);
        return -1;
    }

    // Mirem el tipus d'arxiu i redirigim la peticio
    switch (get_file_type(argv[2]))
    {
    case FAT16_TYPE:
        FAT16_handle(petition_type, argc, argv);
        break;
    case EXT2_TYPE:
        EXT2_handle(petition_type, argc, argv);
        break;
    case ERROR_TYPE:
        printf(ERROR_TYPE_MSG);
    case ERROR_LECTURE:
        printf(ERROR_LECTURE_MSG);
        break;
    default:
        break;
    }

    // Tanquem
    printf("\n\n");
    logout();

    return 0;
}

// Mira el tipus de petició que ens han fet i retorna el tipus corresponent
int get_petition_type(char *petition)
{
    if (!strcmp(petition, AC_INFO))
    {
        return O_INFO;
    }
    if (!strcmp(petition, AC_FIND))
    {
        return O_FIND;
    }
    if (!strcmp(petition, AC_DELETE))
    {
        return O_DELETE;
    }
    return O_ERROR;
}

// Check the type of the file
int get_file_type(char *file)
{
    // Obrim el fitxer
    FILE *fd;
    int checker;

    fd = fopen(file, "r");
    if (fd == NULL)
    {
        return ERROR_LECTURE;
    }
    checker = check_fat16(fd);
    if (!(checker == ERROR_TYPE))
    {
        return checker;
    }

    fd = fopen(file, "r");
    if (fd == NULL)
    {
        return ERROR_LECTURE;
    }
    return check_ext2(fd);
}

int check_fat16(FILE *fd)
{
    char type_fat16[BS_FilSysType_size];
    int return_value;

    fseek(fd, BS_FilSysType_off, SEEK_SET);

    if (fread(&type_fat16, BS_FilSysType_size, 1, fd) < 1)
    {
        return_value = ERROR_LECTURE;
    }
    else
    {
        if (!strcmp(FAT16_CHECK, type_fat16))
        {
            return_value = FAT16_TYPE;
        }
        else
        {
            return_value = ERROR_TYPE;
        }
    }

    fclose(fd);
    return return_value;
}

int check_ext2(FILE *fd)
{
    int type_ext2 = 0;
    int return_type;

    fseek(fd, s_magic_pos, SEEK_SET);
    if (fread(&type_ext2, s_magic_size, 1, fd) < 1)
    {
        return_type = ERROR_LECTURE;
    }
    else
    {
        return_type = (type_ext2 == EXT2_CHECK) ? EXT2_TYPE : ERROR_TYPE;
    }

    fclose(fd);
    return return_type;
}

// Funcions relatives al log out
void logout()
{
    // Tancar fd oberts
    close(2);
    close(1);
    close(0);

    // Acabem el programa
    raise(SIGINT);
}