#include "Fat.h"

void FAT16_handle(int action, int argc, char *argv[])
{
    switch (action)
    {
    case O_INFO:
        argc == 3 ? fat16_info(argv[2]) : printf(ERROR_NUM_PARAM_MSG);
        break;
    case O_FIND:
        argc == 4 ? fat16_find(argv[2], argv[3]) : printf(ERROR_NUM_PARAM_MSG);
        break;
    case O_DELETE:
        argc == 4 ? fat16_delete(argv[2], argv[3]) : printf(ERROR_NUM_PARAM_MSG);
        break;
    default:
        break;
    }
}

void fat16_info(char *file_fat16)
{
    BootFAT16 boot_fat16;
    boot_fat16 = fat16_get_boot(file_fat16);
    fat16_info_print(boot_fat16);
}

void fat16_info_print(BootFAT16 boot_fat16)
{
    printf("\n\n------ Filesystem Information ------\n\n");
    printf("Filesystem: %s\n", "FAT16");
    printf("System Name: %s\n", boot_fat16.BS_OEMName);
    printf("Sector size: %d\n", boot_fat16.BPB_BytsPerSec);
    printf("Sectors Per Cluster: %d\n", boot_fat16.BPB_SecPerClus);
    printf("Reserved Sectors: %d\n", boot_fat16.BPB_ResvdSecCnt);
    printf("Number of FATs: %d\n", boot_fat16.BPB_NumFATs);
    printf("MaxRootEntries: %d\n", boot_fat16.BPB_RootEntCnt);
    printf("Sectors per FAT: %d\n", boot_fat16.BPB_FATSz16);
    printf("Label: %s\n", boot_fat16.BS_VolLab);
}

BootFAT16 fat16_get_boot(char *file_fat16)
{
    BootFAT16 boot_fat16;
    FILE *fd;
    fd = fopen(file_fat16, "r");

    // Llegeixo la info del fitxer
    fread(&boot_fat16, sizeof(BootFAT16), 1, fd);
    boot_fat16.BS_VolLab[10] = '\0';
    boot_fat16.BS_FilSysType[7] = '\0';

    // Tanco
    fclose(fd);

    return boot_fat16;
}

int get_fat_off(BootFAT16 boot_sector)
{
    return boot_sector.BPB_ResvdSecCnt * boot_sector.BPB_BytsPerSec;
}

int get_root_dir_off(BootFAT16 boot_sector)
{
    return get_fat_off(boot_sector) +
           (boot_sector.BPB_NumFATs * boot_sector.BPB_FATSz16) *
               boot_sector.BPB_BytsPerSec;
}

int get_first_cluster(BootFAT16 boot_sector)
{
    return get_root_dir_off(boot_sector) + boot_sector.BPB_RootEntCnt * sizeof(DirectorEntry);
}

void print_directory_entry(DirectorEntry entry)
{
    if (entry.attribute != ATR_DELETE)
    {
        printf("\nName: %s\n", entry.filename);
        printf("Extension: %s\n", entry.extension);
        printf("Startnig cluster: %d\n", entry.first_cluster);
        printf("File size %d\n", entry.size);
        printf("Type: 0x%x\n\n", entry.attribute);
    }
}

void fat16_find(char *file_fat16, char *file_to_search)
{
    BootFAT16 boot_sector;
    DirectorEntry entry;
    FILE *fd;

    boot_sector = fat16_get_boot(file_fat16);
    fd = fopen(file_fat16, "r");

    entry = fat16_function(fd, boot_sector, file_to_search, get_root_dir_off(boot_sector), FIND_FUNC);

    if (entry.attribute == ATR_DELETE)
    {
        printf("File %s not found.\n", file_to_search);
    }
    else
    {
        printf("File %s found. It is %d bytes of size.\n", file_to_search, entry.size);
    }

    fclose(fd);
}

void fat16_delete(char *file_fat16, char *file_to_search)
{
    BootFAT16 boot_sector;
    DirectorEntry entry;
    FILE *fd;

    boot_sector = fat16_get_boot(file_fat16);
    fd = fopen(file_fat16, "r+");

    entry = fat16_function(fd, boot_sector, file_to_search, get_root_dir_off(boot_sector), DEL_FUNC);

    if (entry.attribute == ATR_DELETE)
    {
        printf("File %s not found.\n", file_to_search);
    }
    else
    {
        printf("File %s found and deleted successfuly.\n", file_to_search);
    }

    fclose(fd);
}

DirectorEntry fat16_function(FILE *fd, BootFAT16 boot_sector, char *file_to_search, int offset, int delete)
{
    int new_offset;
    DirectorEntry entry, new_entry;

    for (int i = 0; i < boot_sector.BPB_RootEntCnt; i++)
    {
        // Read Directory entry
        fseek(fd, offset, SEEK_SET);
        entry = read_directory_entry(fd, offset);
        offset += sizeof(DirectorEntry);

        // If we find this attr it means we have already finished
        if (entry.attribute == 0x0)
            break;

        // if the directory entry has been previously deleted, don't look at it
        if (entry.attribute == ATR_DELETE || entry.filename[0] == (char)VAL_DELETE)
            continue;

        //  Check if found
        if (check_entry_found(entry, file_to_search))
        {
            if (delete == DEL_FUNC)
            {
                fat16_delete_entry(fd, offset - sizeof(DirectorEntry));
            }
            return entry;
        }

        // If not found, check if directory
        if (isDirectory_fat(entry))
        {
            // Read Inside Directory
            new_offset = get_first_cluster(boot_sector) + (entry.first_cluster - 2) * boot_sector.BPB_SecPerClus * boot_sector.BPB_BytsPerSec;
            new_entry = fat16_function(fd, boot_sector, file_to_search, new_offset, delete);

            // Check if found
            if (check_entry_found(new_entry, file_to_search))
            {
                return new_entry;
            }
        }
    }

    // If not found, return null entry
    return null_entry_fat();
}

void fat16_delete_entry(FILE *fd, int offset)
{
    fseek(fd, offset, SEEK_SET);
    fwrite(&null_entry_fat, sizeof(DirectorEntry), 1, fd);
}

int check_entry_found(DirectorEntry entry, char *file_to_search)
{
    char *file_name, *file_search, *pivot_file_name;

    // Format the names in order to allow longer name than 8
    pivot_file_name = get_full_name(entry);
    file_name = EXTRA_substring(pivot_file_name, 0, 6);
    file_search = EXTRA_substring(file_to_search, 0, 6);
    free(pivot_file_name);

    // Check if found (first 7 char coincide)
    if (EXTRA_equals_ignore_case(file_name, file_search))
    {
        free(file_name);
        free(file_search);
        return 1;
    }

    free(file_name);
    free(file_search);
    return 0;
}

char *get_full_name(DirectorEntry entry)
{
    int i, extension = 0;
    char *file_name = malloc(1);

    i = 0;
    for (int k = 0; k < strlen(entry.filename); k++)
    {
        if (entry.filename[k] != ' ')
        {
            file_name[i] = entry.filename[k];
            file_name = realloc(file_name, (i + 2));
            i++;
        }
    }

    while (entry.extension[extension] == ' ')
    {
        extension++;
    }

    if (strlen(entry.extension) > extension)
    {
        file_name[i] = '.';
        file_name = realloc(file_name, (i + 2));
        i++;
        for (int k = 0; k < strlen(entry.extension); k++)
        {
            if (entry.extension[k] != ' ')
            {
                file_name[i] = entry.extension[k];
                file_name = realloc(file_name, (i + 2));
                i++;
            }
        }
    }

    file_name[i] = '\0';

    return file_name;
}

int isDirectory_fat(DirectorEntry entry)
{
    if (entry.filename[0] == '.')
    {
        if (entry.filename[1] == '.')
        {
            return 0;
        }
        else
        {
            if (entry.filename[1] == ' ' || entry.filename[1] == '\0')
            {
                return 0;
            }
        }
    }
    return entry.attribute == DIR_TYPE;
}

DirectorEntry null_entry_fat()
{
    DirectorEntry entry;

    memset(entry.filename, '\0', 8);
    memset(entry.extension, '\0', 3);

    entry.filename[0] = (char)VAL_DELETE;
    entry.attribute = ATR_DELETE;
    entry.reserved_windowsNT = 0;
    entry.creation = 0;
    entry.t_creat = 0;
    entry.d_creat = 0;
    entry.d_last_a = 0;
    entry.reserved_fat32 = 0;
    entry.t_last_w = 0;
    entry.d_last_w = 0;
    entry.first_cluster = 0;
    entry.size = 0;

    return entry;
}

DirectorEntry read_directory_entry(FILE *fd, int offset)
{
    int readed;
    DirectorEntry entry;

    fseek(fd, offset, SEEK_SET);
    readed = fread(&entry, sizeof(DirectorEntry), 1, fd);
    if (readed < 1)
    {
        return null_entry_fat();
    }
    entry.filename[7] = '\0';
    entry.extension[2] = '\0';

    return entry;
}