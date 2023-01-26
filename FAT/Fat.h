#ifndef FAT
#define FAT

// Llibreries publiques
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// Includes propis
#include "../UTILS/Utilities.h"
#include "../UTILS/Error.h"

#define END_OF_FILE 0xFF80
#define DIR_TYPE 0X10
#define FIND_FUNC 0
#define DEL_FUNC 1
#define ATR_DELETE 0xf
#define VAL_DELETE 229

// Struct
#pragma pack(1)
typedef struct
{
    char BS_jmpBoot[3];
    char BS_OEMName[8];
    unsigned short BPB_BytsPerSec;
    unsigned short BPB_SecPerClus : 8;
    unsigned short BPB_ResvdSecCnt;
    unsigned short BPB_NumFATs : 8;
    unsigned short BPB_RootEntCnt;
    unsigned short BPB_TotSec16;
    char BPB_Media;
    unsigned short BPB_FATSz16;
    unsigned short BPB_SecPerTrk;
    unsigned short BPB_NumHeads;
    unsigned int BPB_HiddSec;
    unsigned int BPB_TotSec32;
    char BS_DrvNum;
    unsigned short BS_Reserved1 : 8;
    char BS_BootSig;
    unsigned int BS_VolID;
    char BS_VolLab[11];
    char BS_FilSysType[8];

} BootFAT16;

#pragma pack(1)
typedef struct
{
    char filename[8];
    char extension[3];
    char attribute;
    char reserved_windowsNT;
    char creation;
    unsigned short t_creat;
    unsigned short d_creat;
    unsigned short d_last_a;
    unsigned short reserved_fat32;
    unsigned short t_last_w;
    unsigned short d_last_w;
    unsigned short first_cluster;
    unsigned int size;
} DirectorEntry;

// Funcions
void FAT16_handle(int action, int argc, char *argv[]);
void fat16_info(char *file_fat16);
void fat16_find(char *file_fat16, char *file_to_search);
DirectorEntry fat16_function(FILE *fd, BootFAT16 boot_sector, char *file_to_search, int offset, int delete);
BootFAT16 fat16_get_boot(char *file_fat16);
void fat16_info_print(BootFAT16 file);
DirectorEntry null_entry_fat();
int isDirectory_fat(DirectorEntry entry);
char *get_full_name(DirectorEntry entry);
int get_fat_off(BootFAT16 boot_sector);
int get_root_dir_off(BootFAT16 boot_sector);
int get_first_cluster(BootFAT16 boot_sector);
void print_directory_entry(DirectorEntry entry);
DirectorEntry read_directory_entry(FILE *fd, int offset);
int check_entry_found(DirectorEntry entry, char *file_to_search);
void fat16_delete_entry(FILE *fd, int offset);
void fat16_delete(char *file_fat16, char *file_to_search);

#endif