#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 1000

typedef struct {
    int is_occupied;
} Block;

typedef struct {
    char name[50];
    int size;
    char mode[10];
    int blocks[MAX_BLOCKS];
    int block_count;
} File;

Block allocation_table[MAX_BLOCKS];
File files[MAX_BLOCKS];
int total_blocks;
int block_size;
int file_count = 0;

void initialize_disk(int total_blocks_input, int block_size_input) {
    total_blocks = total_blocks_input;
    block_size = block_size_input;
    for (int i = 0; i < total_blocks; i++) {
        allocation_table[i].is_occupied = 0;
    }
    file_count = 0;
    printf("Disk initialized.\n");
}

int check_free_space(int blocks_needed) {
    int free_blocks = 0;
    for (int i = 0; i < total_blocks; i++) {
        if (!allocation_table[i].is_occupied) {
            free_blocks++;
        }
        if (free_blocks >= blocks_needed) {
            return 1;
        }
    }
    return 0;
}

void compact() {
    printf("Compacting disk...\n");
    int next_free_index = 0;
    for (int i = 0; i < file_count; i++) {
        File *file = &files[i];
        int new_blocks[file->block_count];

        for (int j = 0; j < file->block_count; j++) {
            while (allocation_table[next_free_index].is_occupied) {
                next_free_index++;
            }
            new_blocks[j] = next_free_index;
            allocation_table[next_free_index].is_occupied = 1;
            next_free_index++;
        }

        for (int j = 0; j < file->block_count; j++) {
            allocation_table[file->blocks[j]].is_occupied = 0;
        }

        memcpy(file->blocks, new_blocks, sizeof(new_blocks));
    }
    printf("Disk compacted.\n");
}

void create_file(const char *file_name, int file_size, const char *mode) {
    int blocks_needed = (file_size + block_size - 1) / block_size;

    if (!check_free_space(blocks_needed)) {
        printf("Not enough space. Attempting to compact...\n");
        compact();
        if (!check_free_space(blocks_needed)) {
            printf("Error: Not enough space even after compacting.\n");
            return;
        }
    }

    File *new_file = &files[file_count];
    strcpy(new_file->name, file_name);
    new_file->size = file_size;
    strcpy(new_file->mode, mode);
    new_file->block_count = blocks_needed;

    if (strcmp(mode, "contiguous") == 0) {
        int start_index = -1, free_blocks = 0;

        for (int i = 0; i < total_blocks; i++) {
            if (!allocation_table[i].is_occupied) {
                free_blocks++;
                if (free_blocks == blocks_needed) {
                    start_index = i - blocks_needed + 1;
                    break;
                }
            } else {
                free_blocks = 0;
            }
        }

        if (start_index == -1) {
            printf("Error: Unable to find contiguous space.\n");
            return;
        }

        for (int i = start_index; i < start_index + blocks_needed; i++) {
            allocation_table[i].is_occupied = 1;
            new_file->blocks[i - start_index] = i;
        }

    } else if (strcmp(mode, "chained") == 0) {
        int allocated = 0;
        for (int i = 0; i < total_blocks && allocated < blocks_needed; i++) {
            if (!allocation_table[i].is_occupied) {
                allocation_table[i].is_occupied = 1;
                new_file->blocks[allocated++] = i;
            }
        }
    }

    file_count++;
    printf("File '%s' created in %s mode.\n", file_name, mode);
}

void delete_file(const char *file_name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, file_name) == 0) {
            for (int j = 0; j < files[i].block_count; j++) {
                allocation_table[files[i].blocks[j]].is_occupied = 0;
            }
            for (int k = i; k < file_count - 1; k++) {
                files[k] = files[k + 1];
            }
            file_count--;
            printf("File '%s' deleted.\n", file_name);
            return;
        }
    }
    printf("Error: File '%s' not found.\n", file_name);
}

void clear_disk() {
    initialize_disk(total_blocks, block_size);
    printf("Disk cleared.\n");
}

void display_allocation_table() {
    printf("Allocation Table:\n");
    for (int i = 0; i < total_blocks; i++) {
        printf("%d ", allocation_table[i].is_occupied);
    }
    printf("\n");
}

void display_files() {
    printf("Files on Disk:\n");
    for (int i = 0; i < file_count; i++) {
        printf("- %s (Size: %d, Mode: %s, Blocks: ", files[i].name, files[i].size, files[i].mode);
        for (int j = 0; j < files[i].block_count; j++) {
            printf("%d ", files[i].blocks[j]);
        }
        printf(")\n");
    }
}

int main() {
    initialize_disk(20, 512);
    create_file("file1", 1000, "contiguous");
    create_file("file2", 1500, "chained");
    display_allocation_table();
    display_files();
    delete_file("file1");
    display_allocation_table();
    compact();
    display_files();
    return 0;
}

