#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_PATH_LEN 1024

char* parseCmdArgs(int argc, char* argv[]);
void getMode(mode_t mode, char* str);

int main(int argc, char* argv[]) {
  char* base_path = parseCmdArgs(argc, argv);
  char path[MAX_PATH_LEN + 1];
  DIR* dp;
  struct dirent* dirp;
  int path_len;

  dp = opendir(base_path);
  if (dp == NULL) {
    perror(base_path);
    return EXIT_FAILURE;
  }

  path_len = strlen(base_path);
  if (path_len >= MAX_PATH_LEN) {
    return EXIT_FAILURE;
  }
  strncpy(path, base_path, MAX_PATH_LEN);
  if (path[path_len - 1] != '/') {
    path[path_len] = '/';
    path_len++;
    path[path_len] = '\0';
  }

  while ((dirp = readdir(dp)) != NULL) {
    char parallel_path[MAX_PATH_LEN];
    snprintf(parallel_path, MAX_PATH_LEN, "%s%s", path, dirp->d_name);
    struct stat buf;
    if (!lstat(parallel_path, &buf)) {
      // I-node
      printf("%9ld ", buf.st_ino);

      // Date
      char date[64];
      strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S",
               localtime(&buf.st_mtim.tv_sec));
      printf("%s ", date);

      // Size
      printf("%5ld ", buf.st_size);

      // Mode
      char mode[11];
      getMode(buf.st_mode, mode);
      printf("%s ", mode);

      // Name
      printf("%s ", dirp->d_name);
      printf("\n");
    }
  }
  closedir(dp);

  return 0;
}

char* parseCmdArgs(int argc, char* argv[]) {
  char* path = "./";
  if (argc > 1) {
    path = argv[1];
  }
  return path;
}

void getMode(mode_t mode, char* str) {
  str[0] = (S_ISBLK(mode))
               ? 'b'
               : (S_ISCHR(mode))
                     ? 'c'
                     : (S_ISDIR(mode))
                           ? 'd'
                           : (S_ISREG(mode))
                                 ? '-'
                                 : (S_ISLNK(mode))
                                       ? 'l'
                                       : (S_ISFIFO(mode))
                                             ? 'p'
                                             : (S_ISSOCK(mode)) ? 's' : '?';
  str[1] = mode & S_IRUSR ? 'r' : '-';
  str[2] = mode & S_IWUSR ? 'w' : '-';
  str[3] = mode & S_ISUID ? (mode & S_IXUSR ? 's' : 'S')
                          : (mode & S_IXUSR ? 'x' : '-');
  str[4] = mode & S_IRGRP ? 'r' : '-';
  str[5] = mode & S_IWGRP ? 'w' : '-';
  str[6] = mode & S_ISGID ? (mode & S_IXGRP ? 's' : 'S')
                          : (mode & S_IXGRP ? 'x' : '-');
  str[7] = mode & S_IROTH ? 'r' : '-';
  str[8] = mode & S_IWOTH ? 'w' : '-';
  str[9] = mode & S_ISVTX ? (mode & S_IXOTH ? 't' : 'T')
                          : (mode & S_IXOTH ? 'x' : '-');
  str[10] = '\0';
}