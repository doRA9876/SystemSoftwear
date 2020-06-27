#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "find-argv.h"

int parseCmdArgs(int argc, char* argv[], findArgs_t* fa);
int findByName(char* name, char* base_path, unsigned char type, int opt_bit);

int main(int argc, char* argv[]) {
  findArgs_t fa;
  if (1 < argc) {
    if (parseCmdArgs(argc, argv, &fa)) {
      printf("error parsed args.\n");
      exit(EXIT_FAILURE);
    }
  } else {
    printf("Usage: $find [option] [starting path..] [expression]\n");
    exit(EXIT_FAILURE);
  }

  printf("name = %d, %s\n", fa.option_flag & 0b001, fa.file_name);
  printf("type = %d, %d\n", fa.option_flag & 0b010, fa.file_type);
  printf("print = %d\n", fa.option_flag & 0b100);

  char** p = fa.path;
  for (int i = 0; i < fa.path_num; i++) {
    printf("arg: %s\n", fa.path[i]);
  }

  struct stat buf;
  if (stat(fa.path[0], &buf) == 0) {
    char abs_path[PATH_MAX];
    realpath(fa.path[0], abs_path);
    findByName(fa.file_name, abs_path, fa.file_type, fa.option_flag);
  } else {
    printf("No such path exist.\n");
  }

  return 0;
}

int findByName(char* name, char* abs_path, unsigned char file_type, int opt_bit) {
  DIR* dp;
  struct dirent* dirp;
  char path[PATH_MAX];
  int path_len;
  int is_print = (opt_bit & OPT_PRINT);

  if ((abs_path[0] == '/') && (abs_path[1] == '\0')) {
    path[0] = '/';
    path[1] = '\0';
  } else {
    snprintf(path, PATH_MAX, "%s%s", abs_path, "/");
  }

  dp = opendir(path);

  while ((dirp = readdir(dp)) != NULL) {
    char path_plus[PATH_MAX];
    snprintf(path_plus, PATH_MAX, "%s%s", path, dirp->d_name);

    char* current_dir = ".";
    char* up_dir = "..";
    if (strcmp(dirp->d_name, current_dir) && strcmp(dirp->d_name, up_dir)) {
      struct stat buf;
      if (!stat(path_plus, &buf)) {
        // printf("name:%s, bool:%d\n", dirp->d_name, S_ISDIR(buf.st_mode));
        if (opt_bit & OPT_PRINT) {
          printf("%s\n", path_plus);
        } else {
          if (opt_bit & OPT_NAME && strcmp(dirp->d_name, name) == 0) {
            printf("%s\n", path_plus);
          } else if (opt_bit & OPT_TYPE) {
            if(file_type & T_ISDIR && S_ISDIR(buf.st_mode))
              printf("%s\n", path_plus); 
            else if(file_type & T_ISREG && S_ISREG(buf.st_mode))
              printf("%s\n", path_plus);
          }
        }
        if (S_ISDIR(buf.st_mode))
          findByName(name, path_plus, file_type, opt_bit);
      }
    }
  }
}