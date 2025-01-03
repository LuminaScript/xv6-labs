#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
    return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}

void concat_path(char *result, const char *path, const char *dname) {
    while (*path) {
        *result++ = *path++;
    }
    if (*(result - 1) != '/') {
        *result++ = '/';
    }
    while (*dname && *dname != ' ') { // Stop at null or padding spaces in DIRSIZ
        *result++ = *dname++;
    }
    *result = '\0';
}

void iterate_dir(char *path, char *target_file)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }


    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("ls: path too long\n");
        return ;
    }

    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0 || strcmp(de.name, "..") == 0 || strcmp(de.name, ".") == 0) {
            continue;
        }

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            printf("find: cannot stat %s\n", buf);
            continue;
        }
        char *dname = fmtname(buf);

        if (st.type == T_DIR ) { 
            char child_dir[512];
            concat_path(child_dir, path, dname);
            iterate_dir(child_dir, target_file);
            printf("dir_name: %s\n", child_dir);
        } else if (st.type == T_FILE && strcmp(de.name, target_file) == 0){
                printf("%s/%s\n", path, dname);
        }
    }
  
  close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(2, "Usage: find <directory tree> <file_name>\nExample: find . b");
        exit(1);
    }

    char *path = argv[1];
    char *targed_dname = argv[2];
    iterate_dir(path, targed_dname);

    exit(0);
}