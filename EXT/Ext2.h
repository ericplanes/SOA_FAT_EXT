#ifndef EXT
#define EXT

// Llibreries publiques
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <time.h>

// Llibreries propies
#include "../UTILS/Utilities.h"
#include "../UTILS/Error.h"

#define INITIAL_OFF 1024
#define I_SIZE_OFF 4
#define STATIC_ENTRY_SIZE 8
#define TYPE_DIR 2
#define ROOT_INODE 2
#define FILE_NOT_FOUND "FileNotFound"
#define FLAG_DELETE 4
#define FLAG_FIND 0
#define REC_LEN_OFF 4

// Structs
#pragma pack(1)
typedef struct
{
    unsigned int s_inodes_count;
    unsigned int s_blocks_count;
    unsigned int s_r_blocks_count;
    unsigned int s_free_blocks_count;
    unsigned int s_free_inodes_count;
    unsigned int s_first_data_block;
    unsigned int s_log_block_size;
    unsigned int s_log_frag_size;
    unsigned int s_blocks_per_group;
    unsigned int s_frags_per_group;
    unsigned int s_inodes_per_group;
    unsigned int s_mtime;
    unsigned int s_wtime;
    unsigned short s_mnt_count;
    unsigned short s_max_mnt_count;
    unsigned short s_magic;
    unsigned short s_state;
    unsigned short s_errors;
    unsigned short s_minor_rev_level;
    unsigned int s_lastcheck;
    unsigned int s_checkinterval;
    unsigned int s_creator_os;
    unsigned int s_rev_level;
    unsigned short s_def_resuid;
    unsigned short s_def_resgid;
    unsigned int s_first_ino;
    unsigned short s_inode_size;
    unsigned short s_block_group_nr;
    unsigned int s_feature_compat;
    unsigned int s_feature_incompat;
    unsigned int s_feature_ro_compat;
    char s_uuid[16];
    char s_volume_name[16];
    char s_last_mounted[64];
    unsigned int s_algo_bitmap;
    char s_prealloc_blocks;
    char s_prealloc_dir_blocks;
    unsigned short alignment;
    char s_journal_uuid[16];
    unsigned int s_journal_inum;
    unsigned int s_journal_dev;
    unsigned int s_last_orphan;
    char s_hash_seed[4][4];
    char s_def_hash_version;
    char s_default_mount_options[4];
    unsigned int s_first_meta_bg;
} Superblock;

#pragma pack(1)
typedef struct
{
    unsigned int bg_block_bitmap;
    unsigned int bg_inode_bitmap;
    unsigned int bg_inode_table;
    unsigned short bg_free_blocks_count;
    unsigned short bg_free_inodes_count;
    unsigned short bg_used_dirs_count;
    unsigned short bg_pad;
    char bg_reserved[12];
} GroupDescriptor;

#pragma pack(1)
typedef struct
{
    unsigned short i_mode;
    unsigned short i_uid;
    unsigned int i_size;
    unsigned int i_atime;
    unsigned int i_ctime;
    unsigned int i_mtime;
    unsigned int i_dtime;
    unsigned short i_gid;
    unsigned short i_links_count;
    unsigned int i_blocks;
    unsigned int i_flags;
    unsigned int i_osd1;
    unsigned int i_block[15];
    unsigned int i_generation;
    unsigned int i_file_acl;
    unsigned int i_dir_acl;
    unsigned int i_faddr;
    char i_osd2[12];

} Inode;

#pragma pack(1)
typedef struct
{
    unsigned int i_inode;
    unsigned short rec_len;
    unsigned short name_len : 8;
    unsigned short file_type : 8;
    char *name;
} DirectoryEntry;

// Functions
void EXT2_handle(int action, int argc, char *argv[]);
void ext2_info(char *file_name);
void ext2_find(char *file_ext2, char *file_name);

Superblock ext2_read_superblock(char *file_name);
void ext2_print_superblock(Superblock superblock);
GroupDescriptor ext2_read_group_descriptor(FILE *fd, Superblock superblock);
DirectoryEntry find_in_inode(FILE *fd, Inode inode, char *arch_name, Superblock uperblock, int in_table_id, int delete);
DirectoryEntry read_entry(FILE *fd, int offset);
int compute_inode_offset(Superblock superblock, int i_table_block, int inode_id);
Inode get_inode_by_id(FILE *fd, int offset);
int isDirectory(DirectoryEntry entry);
DirectoryEntry null_entry();
void ext2_function(FILE *fd, char *file_name, Superblock superblock, int in_table_id, int delete);
int get_block_group(int inode, int s_inodes_per_group);
int get_inode_index(int inode, int s_inodes_per_group);
void ext2_delete(char *file_ext2, char *file_name);
void ext2_delete_entry(FILE *fd, int prev_offset);

#endif