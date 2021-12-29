#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;
void printdir(char *dir, int depth) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL)  // is failed
    {
        cout << "Opendir failed!" << endl;
        return;
    }
    if (chdir(dir) == -1) {
        cout << "Chdir failed!" << endl;
        return;
    }

    while ((entry = readdir(dp)) != NULL)  // is dir
    {
        lstat(entry->d_name, &statbuf) == -1;  // Get info
        if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0 ||
                strcmp(entry->d_name, ".git") == 0)
                continue;
            printf(
                "DEPTH: %d\tDIRNAME: %15s\tINODE: %ld\tUID: %d\tGID: "
                "%d\tSIZE: %ld\tBLOCKSIZE: %ld\tATIME: %s\n",
                depth, entry->d_name, statbuf.st_ino, statbuf.st_uid,
                statbuf.st_gid, statbuf.st_size, statbuf.st_blksize,
                strtok(ctime(&statbuf.st_atime), "\n"));
            printdir((entry->d_name), depth + 4);
        }

        else  // is file
            printf(
                "DEPTH: %d\tDIRNAME: %15s\tINODE: %ld\tUID: %d\tGID: "
                "%d\tSIZE: %ld\tBLOCKSIZE: %ld\tATIME: %s\n",
                depth, entry->d_name, statbuf.st_ino, statbuf.st_uid,
                statbuf.st_gid, statbuf.st_size, statbuf.st_blksize,
                strtok(ctime(&statbuf.st_atime), "\n"));
    }
    chdir("..");
    closedir(dp);
    return;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage:<path>\n");
        return 0;
    }
    printdir(argv[1], 0);
    return 0;
}