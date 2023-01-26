#include "Ext2.h"

void EXT2_handle(int action, int argc, char *argv[])
{
    switch (action)
    {
    case O_INFO:
        argc == 3 ? ext2_info(argv[2]) : printf(ERROR_NUM_PARAM_MSG);
        break;
    case O_FIND:
        argc == 4 ? ext2_find(argv[2], argv[3]) : printf(ERROR_NUM_PARAM_MSG);
        break;
    case O_DELETE:
        argc == 4 ? ext2_delete(argv[2], argv[3]) : printf(ERROR_NUM_PARAM_MSG);
    default:
        break;
    }
}

void ext2_info(char *file_name)
{
    Superblock superblock;
    superblock = ext2_read_superblock(file_name);
    ext2_print_superblock(superblock);
}

Superblock ext2_read_superblock(char *file_name)
{
    FILE *fd;
    Superblock superblock;
    fd = fopen(file_name, "r");

    fseek(fd, INITIAL_OFF, SEEK_SET);
    fread(&superblock, sizeof(Superblock), 1, fd);

    // Per determinar la mida del bloc
    superblock.s_log_block_size = 1024 << superblock.s_log_block_size;

    // Per determinar la mida del fragment
    superblock.s_log_frag_size = superblock.s_log_frag_size >= 0 ? 1024 << superblock.s_log_frag_size : 1024 >> superblock.s_log_frag_size;

    fclose(fd);

    return superblock;
}

void ext2_print_superblock(Superblock superblock)
{
    time_t last, edit, write;

    last = superblock.s_lastcheck;
    edit = superblock.s_mtime;
    write = superblock.s_wtime;

    printf("\n\n------ Filesystem Information ------\n\n");
    printf("Filesystem: %s\n", "EXT2");

    printf("\nINFO INODE\n");
    printf("Inode Size: %d\n", superblock.s_inode_size);
    printf("Num Inodes: %d\n", superblock.s_inodes_count);
    printf("First Inode: %d\n", superblock.s_first_ino);
    printf("Inodes Group: %d\n", superblock.s_inodes_per_group);
    printf("Free Inodes: %d\n", superblock.s_free_inodes_count);

    printf("\nINFO BLOCK\n");
    printf("Size Block: %d\n", superblock.s_log_block_size);
    printf("Reserved blocks: %d\n", superblock.s_r_blocks_count);
    printf("Free blocks: %d\n", superblock.s_free_blocks_count);
    printf("Total blocks: %d\n", superblock.s_blocks_count);
    printf("First block: %d\n", superblock.s_first_data_block);
    printf("Group blocks: %d\n", superblock.s_blocks_per_group);

    printf("\nINFO BLOCK\n");
    printf("Volume Name: %s\n", superblock.s_volume_name);
    printf("Last Checked: %s", ctime(&last));
    printf("Edited Last: %s", ctime(&edit));
    printf("Written Last: %s", ctime(&write));
}

void ext2_find(char *file_ext2, char *file_name)
{
    Superblock superblock;
    GroupDescriptor group_desc;
    FILE *fd;

    // Read superblock in order to have the information
    superblock = ext2_read_superblock(file_ext2);

    // Open the file and do the search
    fd = fopen(file_ext2, "r");

    // We search at the second block because the first one is empty and the second one is destined to the superblock
    group_desc = ext2_read_group_descriptor(fd, superblock);

    // Testing
    ext2_function(fd, file_name, superblock, group_desc.bg_inode_table, FLAG_FIND);

    fclose(fd);
}

void ext2_delete(char *file_ext2, char *file_name)
{
    Superblock superblock;
    GroupDescriptor group_desc;
    FILE *fd;

    // Read superblock in order to have the information
    superblock = ext2_read_superblock(file_ext2);

    // Open the file and do the search
    fd = fopen(file_ext2, "r+");

    // We search at the second block because the first one is empty and the second one is destined to the superblock
    group_desc = ext2_read_group_descriptor(fd, superblock);

    // Testing
    ext2_function(fd, file_name, superblock, group_desc.bg_inode_table, FLAG_DELETE);

    fclose(fd);
}

GroupDescriptor ext2_read_group_descriptor(FILE *fd, Superblock superblock)
{
    GroupDescriptor group_desc;

    fseek(fd, superblock.s_log_block_size * 2, SEEK_SET);
    fread(&group_desc, sizeof(GroupDescriptor), 1, fd);

    return group_desc;
}

void ext2_function(FILE *fd, char *file_name, Superblock superblock, int in_table_id, int delete)
{
    DirectoryEntry entry;
    Inode inode;
    int offset;

    offset = compute_inode_offset(superblock, in_table_id, ROOT_INODE);

    fseek(fd, offset, SEEK_SET);
    fread(&inode, sizeof(Inode), 1, fd);

    // Find DirectoryEntry of the volume
    entry = find_in_inode(fd, inode, file_name, superblock, in_table_id, delete);

    if (strcmp(entry.name, FILE_NOT_FOUND) == 0)
    {
        printf("File %s not found.\n", file_name);
    }
    else
    {
        if (delete == FLAG_DELETE)
        {
            printf("File %s has been succesfuly deleted.\n", file_name);
        }
        else
        {
            inode = get_inode_by_id(fd, compute_inode_offset(superblock, in_table_id, entry.i_inode));
            printf("File %s found. It is %d bytes of size.\n", file_name, inode.i_size);
        }
    }

    free(entry.name);
}

DirectoryEntry find_in_inode(FILE *fd, Inode inode, char *arch_name, Superblock superblock, int in_table_id, int delete)
{
    DirectoryEntry entry, new_entry;
    int offset, previous_offset, size_readed;
    Inode new_inode;

    for (int i = 0; i < inode.i_blocks && inode.i_block[i] != 0; i++)
    {
        size_readed = 0;
        offset = (inode.i_block[i] - 1) * superblock.s_log_block_size + INITIAL_OFF;
        previous_offset = offset;

        while (size_readed < inode.i_size)
        {
            entry = read_entry(fd, offset);

            if (isDirectory(entry))
            {
                // Guardem els nous valors per seguir buscant en el sistema de fitxers
                new_inode = get_inode_by_id(fd, compute_inode_offset(superblock, in_table_id, entry.i_inode));
                new_entry = find_in_inode(fd, new_inode, arch_name, superblock, in_table_id, delete);

                // Mirem si s'ha trobat el volum en questio
                if (strcmp(new_entry.name, arch_name) == 0)
                {
                    free(entry.name);
                    entry = new_entry;
                }
                else
                {
                    free(new_entry.name);
                }
            }

            if (strcmp(entry.name, arch_name) == 0)
            {
                if (delete == FLAG_DELETE)
                {
                    ext2_delete_entry(fd, previous_offset);
                }
                return entry;
            }

            previous_offset = offset;
            offset += entry.rec_len;
            size_readed += entry.rec_len;

            free(entry.name);
        }
    }

    return null_entry();
}

void ext2_delete_entry(FILE *fd, int prev_offset)
{
    DirectoryEntry prev_entry, del_entry;
    int value = 0, new_rec_len;

    if (prev_offset == 0)
    {
        printf(ERROR_DELETE_OFF_MSG);
        return;
    }

    // Read both dir entry, previous and actual
    prev_entry = read_entry(fd, prev_offset);
    del_entry = read_entry(fd, prev_offset + prev_entry.rec_len);

    // Put inode value to 0 at the dir entry we want to delete
    fseek(fd, prev_offset + prev_entry.rec_len, SEEK_SET);
    fwrite(&value, sizeof(int), 1, fd);

    // Put the previous directory entry rec_len pointing to the next entry
    new_rec_len = prev_entry.rec_len + del_entry.rec_len;
    fseek(fd, prev_offset + REC_LEN_OFF, SEEK_SET);
    fwrite(&new_rec_len, sizeof(short), 1, fd);

    // Free remaining memory
    free(prev_entry.name);
    free(del_entry.name);
}

DirectoryEntry read_entry(FILE *fd, int offset)
{
    DirectoryEntry entry;
    int real_name_len;
    fseek(fd, offset, SEEK_SET);
    fread(&entry, sizeof(DirectoryEntry), 1, fd);

    // Read the name
    if (entry.name_len < 1)
    {
        entry.name = malloc(1);
        entry.name[0] = '\0';
    }
    else
    {
        fseek(fd, offset + STATIC_ENTRY_SIZE, SEEK_SET);

        real_name_len = 4 - ((entry.name_len + STATIC_ENTRY_SIZE) % 4);
        real_name_len += entry.name_len;

        entry.name = (char *)malloc(real_name_len + 1);
        fread(entry.name, 1, real_name_len, fd);
        entry.name[real_name_len] = '\0';
    }

    return entry;
}

Inode get_inode_by_id(FILE *fd, int offset)
{
    Inode inode;
    fseek(fd, offset, SEEK_SET);
    fread(&inode, sizeof(Inode), 1, fd);
    return inode;
}

int compute_inode_offset(Superblock superblock, int i_table_block, int inode_id)
{
    int block_group, inode_index, block_group_offset, local_block_offset, inode_offset;

    // Offset until block group
    block_group = get_block_group(inode_id, superblock.s_inodes_per_group);
    block_group_offset = block_group * superblock.s_blocks_per_group * superblock.s_log_block_size + INITIAL_OFF;

    // Offset until block
    local_block_offset = (i_table_block - 1) * superblock.s_log_block_size;

    // Offset until inode
    inode_index = get_inode_index(inode_id, superblock.s_inodes_per_group);
    inode_offset = inode_index * superblock.s_inode_size;

    return block_group_offset + local_block_offset + inode_offset;
}

DirectoryEntry null_entry()
{
    DirectoryEntry null_entry;

    // Initialize
    null_entry.i_inode = 0;
    null_entry.rec_len = 0;
    null_entry.name_len = 0;
    null_entry.file_type = 0;
    null_entry.name = EXTRA_copy_string(FILE_NOT_FOUND);

    return null_entry;
}

int get_block_group(int inode, int s_inodes_per_group)
{
    return (inode - 1) / s_inodes_per_group;
}

int get_inode_index(int inode, int s_inodes_per_group)
{
    return (inode - 1) % s_inodes_per_group;
}

int isDirectory(DirectoryEntry entry)
{
    return entry.file_type == TYPE_DIR && strcmp(entry.name, ".") != 0 && strcmp(entry.name, "..") != 0;
}
