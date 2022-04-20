#include "disk.h"
#include "fs.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)    /* Don't do anything in release builds */
#endif

// Debug file system -----------------------------------------------------------

void fs_debug(Disk *disk)
{
    if (disk == 0)
        return;

    Block block;

    // Read Superblock
    disk_read(disk, 0, block.Data);

    uint32_t magic_num = block.Super.MagicNumber;
    uint32_t num_blocks = block.Super.Blocks;
    uint32_t num_inodeBlocks = block.Super.InodeBlocks;
    uint32_t num_inodes = block.Super.Inodes;

    if (magic_num != MAGIC_NUMBER)
    {
        printf("Magic number is valid: %c\n", magic_num);
        return;
    }

    printf("SuperBlock:\n");
    printf("    magic number is valid\n");
    printf("    %u blocks\n", num_blocks);
    printf("    %u inode blocks\n", num_inodeBlocks);
    printf("    %u inodes\n", num_inodes);

    uint32_t expected_num_inodeBlocks = round((float)num_blocks / 10);

    if (expected_num_inodeBlocks != num_inodeBlocks)
    {
        printf("SuperBlock declairs %u InodeBlocks but expect %u InodeBlocks!\n", num_inodeBlocks, expected_num_inodeBlocks);
    }

    uint32_t expect_num_inodes = num_inodeBlocks * INODES_PER_BLOCK;
    if (expect_num_inodes != num_inodes)
    {
        printf("SuperBlock declairs %u Inodes but expect %u Inodes!\n", num_inodes, expect_num_inodes);
    }

    // FIXME: Read Inode blocks
    Block inodeBlock, indirectInodeBlock;
    for (int i = 0; i < num_inodeBlocks; i++) {
        disk_read(disk, i+1, inodeBlock.Data);
        for (int j = 0; j < INODES_PER_BLOCK; j++) {
            if (inodeBlock.Inodes[j].Valid == 1) {
                printf("Inode %d:\n", j + i * num_inodeBlocks);
                printf("    size: %d bytes\n", inodeBlock.Inodes[j].Size);
                printf("    direct blocks:");

                for (int k = 0; k < POINTERS_PER_INODE; k++) {
            				if (inodeBlock.Inodes[j].Direct[k]) {
                      printf(" %d", inodeBlock.Inodes[j].Direct[k]);
                    }
            		}
                printf("\n");
                if (inodeBlock.Inodes[j].Indirect) {
                  printf("    indirect block: %d\n", inodeBlock.Inodes[j].Indirect);
                	printf("    indirect data blocks:");
                	disk_read(disk, inodeBlock.Inodes[j].Indirect, indirectInodeBlock.Data);
                	for(int k = 0; k < POINTERS_PER_BLOCK; k++) {
                	   if(indirectInodeBlock.Pointers[k] > 0 && indirectInodeBlock.Pointers[k] < num_blocks) {
                		     printf(" %d", indirectInodeBlock.Pointers[k]);
                			}
                	}
                	printf("\n");
              	}
            }
        }
    }
}


// Format file system ----------------------------------------------------------

bool fs_format(Disk *disk)
{
    if(!disk_mounted(disk)) {
        // Write superblock
        int num_blocks = disk_size(disk);
      	int num_inodeBlocks = (num_blocks + 9) / 10;

      	Block inodeBlock;
      	inodeBlock.Super.MagicNumber = MAGIC_NUMBER;
      	inodeBlock.Super.Blocks = num_blocks;
      	inodeBlock.Super.InodeBlocks = num_inodeBlocks;
      	inodeBlock.Super.Inodes = num_inodeBlocks * INODES_PER_BLOCK;
      	disk_write(disk, 0, inodeBlock.Data);

        // Clear all other blocks
      	for (int i = 1; i <= num_inodeBlocks; i++) {
      		for (int j = 0; j < INODES_PER_BLOCK; j++) {
      			inodeBlock.Inodes[j].Valid = 0;
      			inodeBlock.Inodes[j].Size = 0;
      			for(int k = 0; k < POINTERS_PER_INODE; k++) {
              inodeBlock.Inodes[j].Direct[k] = 0;
            }
            inodeBlock.Inodes[j].Indirect = 0;
      		}
      		disk_write(disk, i, inodeBlock.Data);
      	}
      	return true;
    }
    return false;
}

// FileSystem constructor
FileSystem *new_fs()
{
    FileSystem *fs = malloc(sizeof(FileSystem));
    return fs;
}

// FileSystem destructor
void free_fs(FileSystem *fs)
{
    // FIXME: free resources and allocated memory in FileSystem

    free(fs);
}

// Mount file system -----------------------------------------------------------

bool fs_mount(FileSystem *fs, Disk *disk)
{
    // Read superblock

    // Set device and mount

    // Copy metadata

    // Allocate free block bitmap

    return false;
}

// Create inode ----------------------------------------------------------------

ssize_t fs_create(FileSystem *fs)
{
    // Locate free inode in inode table

    // Record inode if found

    return -1;
}

// Optional: the following two helper functions may be useful.

// bool find_inode(FileSystem *fs, size_t inumber, Inode *inode)
// {
//     return true;
// }

// bool store_inode(FileSystem *fs, size_t inumber, Inode *inode)
// {
//     return true;
// }

// Remove inode ----------------------------------------------------------------

bool fs_remove(FileSystem *fs, size_t inumber)
{
    // Load inode information

    // Free direct blocks

    // Free indirect blocks

    // Clear inode in inode table

    return false;
}

// Inode stat ------------------------------------------------------------------

ssize_t fs_stat(FileSystem *fs, size_t inumber)
{
    // Load inode information
    return 0;
}

// Read from inode -------------------------------------------------------------

ssize_t fs_read(FileSystem *fs, size_t inumber, char *data, size_t length, size_t offset)
{
    // Load inode information

    // Adjust length

    // Read block and copy to data

    return 0;
}

// Optional: the following helper function may be useful.

// ssize_t fs_allocate_block(FileSystem *fs)
// {
//     return -1;
// }

// Write to inode --------------------------------------------------------------

ssize_t fs_write(FileSystem *fs, size_t inumber, char *data, size_t length, size_t offset)
{
    // Load inode

    // Write block and copy to data

    return 0;
}
