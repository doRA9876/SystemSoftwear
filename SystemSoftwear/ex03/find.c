#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "find-argv.h"

extern int parseCmdArgs(int argc, char* argv[], findArgs_t* fa);
void find(char* name, char* base_path, unsigned char type, int opt_bit);

char* current_dir = ".";
char* up_dir = "..";

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

  for (int i = 0; i < fa.path_num; i++) {
    struct stat buf;
    if (stat(fa.path[i], &buf) == 0) {
      char abs_path[PATH_MAX];
      realpath(fa.path[i], abs_path);
      find(fa.file_name, abs_path, fa.file_type, fa.option_flag);
    } else {
      printf("No.%d Path: No such path exist.\n", i);
    }
  }
  return 0;
}

/**
 * @brief 指定パスから名前やオプション条件に適合したファイルを表示
 *
 * @param name 目的のファイル名
 * @param abs_path 絶対パス
 * @param file_type 目的のファイルの種類
 * @param opt_bit オプションフラグ
 */
void find(char* name, char* abs_path, unsigned char file_type, int opt_bit) {
  DIR* dp;
  struct dirent* dirp;
  char path[PATH_MAX];
  int path_len;
  int is_print = (opt_bit & OPT_PRINT);

  if ((abs_path[0] == '/') && (abs_path[1] == '\0')) {
    //ルートディレクトリの例外処理
    path[0] = '/';
    path[1] = '\0';
  } else {
    //通常のディレクトリ
    snprintf(path, PATH_MAX, "%s%s", abs_path, "/");
  }

  dp = opendir(path);
  if (dp == NULL) {
    perror(path);
    return;
  }

  while ((dirp = readdir(dp)) != NULL) {
    char path_plus[PATH_MAX];
    snprintf(path_plus, PATH_MAX, "%s%s", path, dirp->d_name);

    if (strcmp(dirp->d_name, current_dir) && strcmp(dirp->d_name, up_dir)) {
      struct stat buf;
      if (!lstat(path_plus, &buf)) {
        //-print option
        if (opt_bit & OPT_PRINT) {
          printf("%s\n", path_plus);
        } else {
          //-name option
          if (opt_bit & OPT_NAME && strcmp(dirp->d_name, name) == 0) {
            printf("%s\n", path_plus);
          } else if (opt_bit & OPT_TYPE) {
            //-type option
            // directry
            if (file_type & T_ISDIR && S_ISDIR(buf.st_mode))
              printf("%s\n", path_plus);
            // normal file
            else if (file_type & T_ISREG && S_ISREG(buf.st_mode))
              printf("%s\n", path_plus);
            // character device
            else if (file_type & T_ISCHR && S_ISCHR(buf.st_mode))
              printf("%s\n", path_plus);
            // block device
            else if (file_type & T_ISBLK && S_ISBLK(buf.st_mode))
              printf("%s\n", path_plus);
            // named pipe
            else if (file_type & T_ISFIFO && S_ISFIFO(buf.st_mode))
              printf("%s\n", path_plus);
            // symbolic link
            else if (file_type & T_ISLNK && S_ISLNK(buf.st_mode))
              printf("%s\n", path_plus);
            // unix domain socket
            else if (file_type & T_ISSOCK && S_ISSOCK(buf.st_mode))
              printf("%s\n", path_plus);
          }
        }
        if (S_ISDIR(buf.st_mode))
          find(name, path_plus, file_type, opt_bit);
      }
    }
  }
  closedir(dp);
}
