/*
 *
 * Name Surname: Beyazit Yalcinkaya
 * Student ID:   2172138
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include "ext2.h"

#define BASE_OFFSET 1024

typedef unsigned char bmap;

#define __NBITS (8 * (int) sizeof (bmap))
#define __BMELT(d) ((d) / __NBITS)
#define __BMMASK(d) ((bmap) 1 << ((d) % __NBITS))
#define BM_SET(d, set) ((set[__BMELT (d)] |= __BMMASK (d)))
#define BM_CLR(d, set) ((set[__BMELT (d)] &= ~__BMMASK (d)))
#define BM_ISSET(d, set) ((set[__BMELT (d)] & __BMMASK (d)) != 0)

unsigned int block_size = 0;
#define BLOCK_OFFSET(block) (block*block_size)

unsigned int N_GROUPS;

int isNumber(char const *inp){
    while (*inp) {
        if (!isdigit(*inp))
            return 0;
        inp++;
    }
    return 1;
}

int isPowerOf(unsigned int n, unsigned int m) {
    if (n == 0)
        return 0;
    while (n % m == 0) {
        n /= m;
    }
    return n == 1;
}

void task1(unsigned int fd, struct ext2_super_block *super_block, struct ext2_group_desc *group_descs, unsigned int inode_number, const char *sourcefile) {
    int i, j, k;
    //Get new available inode.
    unsigned int new_inode_number = 0;
    bmap *bitmap = malloc(block_size);
    lseek(fd, BLOCK_OFFSET(group_descs[2].bg_inode_bitmap), SEEK_SET);
    read(fd, bitmap, block_size);
    for (i = 0; i < N_GROUPS; i++) {
        //Go to bitmap.
        lseek(fd, BLOCK_OFFSET(group_descs[i].bg_inode_bitmap), SEEK_SET);
        read(fd, bitmap, block_size);
        if (group_descs[i].bg_free_inodes_count > 0) {
            for (j = 0; j < super_block -> s_inodes_per_group; j++) {
                if(!BM_ISSET(j, bitmap)) {
                    //Found the new inode.
                    new_inode_number = super_block -> s_inodes_per_group * i + j + 1;
                    if (new_inode_number < 12)
                        continue;
                    BM_SET(j, bitmap);
                    super_block -> s_free_inodes_count--;
                    group_descs[i].bg_free_inodes_count--;
                    lseek(fd, BLOCK_OFFSET(group_descs[i].bg_inode_bitmap), SEEK_SET);
                    write(fd, bitmap, block_size);
                    break;
                }
            }
        }
        if (new_inode_number != 0) break;
    }
    printf("%u ", new_inode_number);
    //Create new inode.
    struct ext2_inode new_inode;
    //Fill new inode.
    struct stat st;
    stat(sourcefile, &st);
    new_inode.i_mode        = st.st_mode;
    new_inode.i_uid         = st.st_uid;
    new_inode.i_size        = st.st_size;
    new_inode.i_atime       = st.st_atime;
    new_inode.i_ctime       = st.st_ctime;
    new_inode.i_mtime       = st.st_mtime;
    new_inode.i_dtime       = 0;
    new_inode.i_gid         = st.st_gid;
    new_inode.i_links_count = 1;
    new_inode.i_blocks      = 0;
    new_inode.i_flags       = 0;
    new_inode.osd1          = 0;
    new_inode.i_block[0]    = 0;
    new_inode.i_block[1]    = 0;
    new_inode.i_block[2]    = 0;
    new_inode.i_block[3]    = 0;
    new_inode.i_block[4]    = 0;
    new_inode.i_block[5]    = 0;
    new_inode.i_block[6]    = 0;
    new_inode.i_block[7]    = 0;
    new_inode.i_block[8]    = 0;
    new_inode.i_block[9]    = 0;
    new_inode.i_block[10]   = 0;
    new_inode.i_block[11]   = 0;
    new_inode.i_block[12]   = 0;
    new_inode.i_block[13]   = 0;
    new_inode.i_block[14]   = 0;
    new_inode.i_generation  = 0;
    new_inode.i_file_acl    = 0;
    new_inode.i_dir_acl     = 0;
    new_inode.i_faddr       = 0;
    new_inode.extra[0]      = 0;
    new_inode.extra[1]      = 0;
    new_inode.extra[2]      = 0;
    //Copy source file to data blocks of the new inode.
    unsigned char *buffer = malloc(block_size);
    //Open source file.
    unsigned int source_fd = open(sourcefile, O_RDONLY);
    unsigned int new_block_address = 0;
    unsigned int number_of_bytes_read;
    unsigned int first_data_block_address;
    for (k = 0; k < 12 && (number_of_bytes_read = read(source_fd, buffer, block_size)) != 0; k++) {
        //Get next available block address.
        for (i = (new_inode_number - 1) / super_block -> s_inodes_per_group; i < N_GROUPS; i++) {
            //Go to bitmap.
            first_data_block_address = group_descs[i].bg_inode_table + (unsigned int)ceil((double)(sizeof(struct ext2_inode) * super_block -> s_inodes_per_group) / (double)block_size);
            lseek(fd, BLOCK_OFFSET(group_descs[i].bg_block_bitmap), SEEK_SET);
            read(fd, bitmap, block_size);
            if (group_descs[i].bg_free_blocks_count > 0) {
                for (j = 0; j < block_size * 8; j++) {
                    if(!BM_ISSET(j, bitmap)) {
                        //Found a block.
                        BM_SET(j, bitmap);
                        new_block_address = first_data_block_address + j - (first_data_block_address % super_block -> s_blocks_per_group) + super_block -> s_first_data_block;
                        super_block -> s_free_blocks_count--;
                        group_descs[i].bg_free_blocks_count--;
                        lseek(fd, BLOCK_OFFSET(group_descs[i].bg_block_bitmap), SEEK_SET);
                        write(fd, bitmap, block_size);
                        break;
                    }
                }
            }
            if (new_block_address) break;
        }
        lseek(fd, BLOCK_OFFSET(new_block_address), SEEK_SET);
        write(fd, buffer, number_of_bytes_read);
        new_inode.i_block[k] = new_block_address;
        new_inode.i_blocks += (block_size / 512);
        if (!k) printf("%u", new_block_address);
        else printf(",%u", new_block_address);
    }
    printf("\n");
    //Write new inode to disk.
    //Calculate block group number of new inode.
    unsigned int new_inode_block_group_number = (new_inode_number - 1) / (super_block -> s_inodes_per_group);
    //Calculate index of the new inode in the inode table.
    unsigned int new_inode_index_in_inode_table = (new_inode_number - 1) % (super_block -> s_inodes_per_group);
    //Read the absolute address of the new inode table.
    unsigned int new_inode_table_abs_address = group_descs[new_inode_block_group_number].bg_inode_table;
    //Go to inode pos.
    lseek(fd, BLOCK_OFFSET(new_inode_table_abs_address) + new_inode_index_in_inode_table * sizeof(struct ext2_inode), SEEK_SET);
    write(fd, &new_inode, sizeof(struct ext2_inode));
    //Create new dir entry.
    unsigned int sourcefile_name_len = strlen(sourcefile);
    //Fill new dir entry.
    unsigned int inode      = new_inode_number;
    unsigned short rec_len  = 0;//Will be updated below.
    unsigned char name_len  = sourcefile_name_len;
    unsigned char file_type = EXT2_FT_REG_FILE;
    char name[EXT2_NAME_LEN];
    strncpy(name, sourcefile, sourcefile_name_len);
    //Find an available dir entry place in the data block of the given inode.
    //Calculate block group number of inode.
    unsigned int inode_block_group_number = (inode_number - 1) / (super_block -> s_inodes_per_group);
    //Calculate index of the inode in the inode table.
    unsigned int inode_index_in_inode_table = (inode_number - 1) % (super_block -> s_inodes_per_group);
    //Read the absolute address of the inode table.
    unsigned int inode_table_abs_address = group_descs[inode_block_group_number].bg_inode_table;
    //Go to inode pos.
    lseek(fd, BLOCK_OFFSET(inode_table_abs_address) + inode_index_in_inode_table * sizeof(struct ext2_inode), SEEK_SET);
    struct ext2_inode given_inode;
    //Read inode.
    read(fd, &given_inode, sizeof(struct ext2_inode));
    unsigned int unexplored_data_block_size;
    unsigned int temp_inode;
    unsigned short temp_rec_len;
    unsigned char temp_name_len;
    unsigned char temp_actual_len;
    char flag = 0;
    for (i = 0; i < 12; i++) {
        if (given_inode.i_block[i] != 0) {
            unexplored_data_block_size = block_size;
            lseek(fd, BLOCK_OFFSET(given_inode.i_block[i]), SEEK_SET);
            read(fd, &temp_inode, sizeof(unsigned int));
            while (temp_inode != 0 && unexplored_data_block_size > 0) {
                read(fd, &temp_rec_len, sizeof(temp_rec_len));
                read(fd, &temp_name_len, sizeof(temp_name_len));
                temp_actual_len = temp_name_len + 8;
                if ((temp_actual_len + (4 - (temp_actual_len % 4)) < temp_rec_len) &&
                   ((name_len + sizeof(unsigned int) + sizeof(unsigned short) + sizeof(unsigned char) + sizeof(unsigned char)) <= (temp_rec_len - (temp_actual_len + (4 - (temp_actual_len % 4)))))) {
                    //Put here.
                    temp_rec_len = (temp_actual_len + (4 - (temp_actual_len % 4)));
                    lseek(fd, -(sizeof(temp_rec_len) + sizeof(temp_name_len)), SEEK_CUR);
                    write(fd, &temp_rec_len, sizeof(temp_rec_len));
                    lseek(fd, temp_rec_len - sizeof(temp_inode) - sizeof(temp_rec_len), SEEK_CUR);
                    //Write new dir entry to the found place on disk.
                    rec_len = unexplored_data_block_size - temp_rec_len;
                    write(fd, &inode, sizeof(unsigned int));
                    write(fd, &rec_len, sizeof(unsigned short));
                    write(fd, &name_len, sizeof(unsigned char));
                    write(fd, &file_type, sizeof(unsigned char));
                    write(fd, name, name_len);
                    flag = 1;
                    break;
                }
                unexplored_data_block_size -= temp_rec_len;
                lseek(fd, temp_rec_len - sizeof(temp_inode) - sizeof(temp_rec_len) - sizeof(temp_name_len), SEEK_CUR);
                read(fd, &temp_inode, sizeof(temp_inode));
            }
            if (flag) break;
        } else {
            //Get next available block address.
            // Store old i in k.
            k = i;
            for (i = (inode_number - 1) / super_block -> s_inodes_per_group; i < N_GROUPS; i++) {
                //Go to bitmap.
                first_data_block_address = group_descs[i].bg_inode_table + (unsigned int)ceil((double)(sizeof(struct ext2_inode) * super_block -> s_inodes_per_group) / (double)block_size);
                lseek(fd, BLOCK_OFFSET(group_descs[i].bg_block_bitmap), SEEK_SET);
                read(fd, bitmap, block_size);
                if (group_descs[i].bg_free_blocks_count > 0) {
                    for (j = 0; j < block_size * 8; j++) {
                        if(!BM_ISSET(j, bitmap)) {
                            //Found a block.
                            BM_SET(j, bitmap);
                            new_block_address = first_data_block_address + j - (first_data_block_address % super_block -> s_blocks_per_group) + super_block -> s_first_data_block;
                            super_block -> s_free_blocks_count--;
                            group_descs[i].bg_free_blocks_count--;
                            lseek(fd, BLOCK_OFFSET(group_descs[i].bg_block_bitmap), SEEK_SET);
                            write(fd, bitmap, block_size);
                            break;
                        }
                    }
                }
                if (new_block_address) break;
            }
            lseek(fd, BLOCK_OFFSET(new_block_address), SEEK_SET);
            rec_len = block_size;
            write(fd, &inode, sizeof(unsigned int));
            write(fd, &rec_len, sizeof(unsigned short));
            write(fd, &name_len, sizeof(unsigned char));
            write(fd, &file_type, sizeof(unsigned char));
            write(fd, name, name_len);
            given_inode.i_block[k] = new_block_address;
    		lseek(fd, BLOCK_OFFSET(inode_table_abs_address) + inode_index_in_inode_table * sizeof(struct ext2_inode), SEEK_SET);
    		write(fd, &given_inode, sizeof(struct ext2_inode));
            break;
        }
    }
    //Free everything allocated in the heap.
    free(bitmap);
    free(buffer);
}

unsigned int findNextInode(unsigned int inode_number, char* dir_name, unsigned int fd, struct ext2_super_block *super_block, struct ext2_group_desc *group_descs) {
    int i;
    unsigned int inode_block_group_number = (inode_number - 1) / (super_block -> s_inodes_per_group);
    unsigned int inode_index_in_inode_table = (inode_number - 1) % (super_block -> s_inodes_per_group);
    unsigned int inode_table_abs_address = group_descs[inode_block_group_number].bg_inode_table;
    lseek(fd, BLOCK_OFFSET(inode_table_abs_address) + inode_index_in_inode_table * sizeof(struct ext2_inode), SEEK_SET);
    struct ext2_inode dir_inode;
    read(fd, &dir_inode, sizeof(struct ext2_inode));
    unsigned int unexplored_data_block_size;
    unsigned int inode;
    unsigned short rec_len;
    unsigned char name_len;
    unsigned char file_type;
    char name[EXT2_NAME_LEN];
    char flag = 0;
    for (i = 0; i < 12; i++) {
        if (dir_inode.i_block[i] != 0) {
            unexplored_data_block_size = block_size;
            lseek(fd, BLOCK_OFFSET(dir_inode.i_block[i]), SEEK_SET);
            while (unexplored_data_block_size > 0) {
                read(fd, &inode, sizeof(unsigned int));
                read(fd, &rec_len, sizeof(unsigned short));
                read(fd, &name_len, sizeof(unsigned char));
                read(fd, &file_type, sizeof(unsigned char));
                read(fd, name, name_len);
                if (strncmp(name, dir_name, name_len) == 0) {
                    return inode;
                }
                unexplored_data_block_size -= rec_len;
                lseek(fd, rec_len - name_len - sizeof(unsigned int) - sizeof(unsigned short) - sizeof(unsigned char) - sizeof(unsigned char), SEEK_CUR);
            }
        }
    }
    return 0;
}

unsigned int findInodeFromPath(const char *sourcefile, unsigned int fd, struct ext2_super_block *super_block, struct ext2_group_desc *group_descs) {
    char *path;
    char *dir_name;
    unsigned int inode = 2;
    strcpy(path, sourcefile);
    for (dir_name = strtok(path, "/"); dir_name != NULL; dir_name = strtok(NULL, "/")) {
        inode = findNextInode(inode, dir_name, fd, super_block, group_descs);
    }
    return inode;
}

int main(int argc, char const *argv[]) {
    int i;
    // ./filecopy imagefile sourcefile targetdirinode
    struct ext2_super_block *super_block = malloc(sizeof(struct ext2_super_block));
    //Open ext2 image.
    unsigned int fd = open(argv[1], O_RDWR);
    //Go to super block.
    lseek(fd, BASE_OFFSET, SEEK_SET);
    //Read super block.
    read(fd, super_block, sizeof(struct ext2_super_block));
    //Calculate block size.
    block_size = pow(2, super_block -> s_log_block_size + 10);
    //Calculate number of block groups.
    N_GROUPS = (unsigned int)ceil((double)super_block -> s_inodes_count / (double)super_block -> s_inodes_per_group);
    struct ext2_group_desc *group_descs = malloc(sizeof(struct ext2_group_desc) * N_GROUPS);
    //Go to group descriptor table.
    lseek(fd, (unsigned int)BLOCK_OFFSET(ceil((double)(BASE_OFFSET + sizeof(struct ext2_super_block)) / (double)block_size)), SEEK_SET);
    //Read group descriptors.
    read(fd, group_descs, sizeof(struct ext2_group_desc) * N_GROUPS);
    unsigned int inode_number = 0;
    if (isNumber(argv[3])) {
        //Convert inode value to unsigned int
        inode_number = strtoul(argv[3], NULL, 0);
    }
    else {
        //Find inode from abs path
        inode_number = findInodeFromPath(argv[3], fd, super_block, group_descs);
    }
    task1(fd, super_block, group_descs, inode_number, argv[2]);
    for (i = 0; i < N_GROUPS; i++) {
        if (i == 0 || isPowerOf(i, 3) || isPowerOf(i, 5) || isPowerOf(i, 7)) {
            lseek(fd, BASE_OFFSET + BLOCK_OFFSET(super_block -> s_blocks_per_group * i), SEEK_SET);
            write(fd, super_block, sizeof(struct ext2_super_block));
            lseek(fd, (unsigned int)BLOCK_OFFSET(ceil((double)(BASE_OFFSET + sizeof(struct ext2_super_block) + BLOCK_OFFSET(super_block -> s_blocks_per_group * i)) / (double)block_size)), SEEK_SET);
            write(fd, group_descs, sizeof(struct ext2_group_desc) * N_GROUPS);
        }   
    }
    free(super_block);
    free(group_descs);
    close(fd);
    return 0;
}

