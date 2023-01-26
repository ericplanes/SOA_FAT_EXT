#ifndef VIEW
#define VIEW

// Llibreries publiques
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <time.h>

// Includes propis
#include "../UTILS/Utilities.h"
#include "../UTILS/Error.h"
#include "../FAT/Fat.h"
#include "../EXT/Ext2.h"

// Defines
#define NUM_PARAM 3
#define FAT16_TYPE 1
#define EXT2_TYPE 2

// Options
#define AC_INFO "/info"
#define AC_FIND "/find"
#define AC_DELETE "/delete"
// Defines per fer el check del tipus FAT16
#define BS_FilSysType_off 54
#define BS_FilSysType_size 8
#define FAT16_CHECK "FAT16   "

// Defines per fer el check del tipus EXT2
#define s_magic_pos (1024 + 56)
#define s_magic_size 2
#define EXT2_CHECK 0xEF53

// Funcions
int get_petition_type(char *petition);
int get_file_type(char *file);
int check_fat16(FILE *fd);
int check_ext2(FILE *fd);
void logout();

#endif