
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

typedef struct {
    char *name;
    time_t mtime;
} FileInfo;

// Custom bubble sort function
void bubble_sort(FileInfo *files, int count, int sort_by_time) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int swap = 0;
            if (sort_by_time) {
                // Sort by modification time (newest first)
                if (files[j].mtime < files[j + 1].mtime) {
                    swap = 1;
                }
            } else {
                // Sort lexicographically
                if (strcmp(files[j].name, files[j + 1].name) > 0) {
                    swap = 1;
                }
            }

            if (swap) {
                // Swap files[j] and files[j + 1]
                FileInfo temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            }
        }
    }
}

void list_directory(const char *dirpath, int show_hidden, int sort_by_time) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    FileInfo *files = NULL;
    int count = 0;

    dir = opendir(dirpath);
    if (!dir) {
        perror("opendir");
        return;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') {
            continue; // Skip hidden files if -a is not set
        }

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);

        if (lstat(fullpath, &file_stat) == -1) {
            perror("lstat");
            continue;
        }

        // Allocate memory for file info
        files = realloc(files, (count + 1) * sizeof(FileInfo));
        files[count].name = strdup(entry->d_name);
        files[count].mtime = file_stat.st_mtim.tv_sec; // Use st_mtim for nanosecond precision
        count++;
    }
    closedir(dir);

    // Sort files using custom sorting function
    bubble_sort(files, count, sort_by_time);

    // Print sorted files
    for (int i = 0; i < count; i++) {
        printf("%s\n", files[i].name);
        free(files[i].name);
    }
    free(files);
}

int main(int argc, char *argv[]) {
    int show_hidden = 0;
    int sort_by_time = 0;
    int i = 1;

    // Parse options
    while (i < argc && argv[i][0] == '-') {
        if (strchr(argv[i], 'a')) show_hidden = 1;
        if (strchr(argv[i], 't')) sort_by_time = 1;
        i++;
    }

    // If no operands, list current directory
    if (i >= argc) {
        list_directory(".", show_hidden, sort_by_time);
    } else {
        // List files and directories
        for (; i < argc; i++) {
            struct stat path_stat;
            if (stat(argv[i], &path_stat) == -1) {
                perror("stat");
                continue;
            }

            if (S_ISDIR(path_stat.st_mode)) {
                list_directory(argv[i], show_hidden, sort_by_time);
            } else {
                printf("%s\n", argv[i]);
            }
        }
    }

    return 0;
}
