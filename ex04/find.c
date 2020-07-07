#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <getopt.h>

#include "find-argv.h"

int parseCmdArgs(int argc, char* argv[], findArgs_t* fa);
void find(char* name, char* base_path, unsigned char type, int opt_bit);
unsigned char getTypeOpt(char* opt);
char* getName(char* name);

char* current_dir = ".";
char* up_dir = "..";

struct option longopts[] = {
    {"name", required_argument, NULL, 'n'},
    {"type", required_argument, NULL, 't'},
    {"print", no_argument, NULL, 'p'},
    {0, 0, 0, 0},
};

int find_main(int argc, char* argv[]) {
  findArgs_t fa;
  if (1 < argc) {
    if (parseCmdArgs(argc, argv, &fa)) {
      printf("error parsed args.\n");
      return -1;
    }
  } else {
    printf("Usage: $find [option] [starting path..] [expression]\n");
    return -1;
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

/**
 * @brief コマンドライン引数からオプションと引数に分解。
 * 
 * @param argc コマンドライン引数の数 
 * @param argv コマンドライン引数
 * @param fa 構造体findArgs_tのポインタ
 * @return int 成功時：0、失敗時：-1
 */
int parseCmdArgs(int argc, char* argv[], findArgs_t* fa) {
  opterr = 0;

  fa->option_flag = 0;
  fa->file_type = 0;
  fa->path_num = 0;

  char* tparam = NULL;
  int opt;
  int longindex;
  while ((opt = getopt_long_only(argc, argv, "n:pt:", longopts, &longindex)) !=
         -1) {
    printf("%d\n", opt);
    switch (opt) {
      case 'n':
        fa->option_flag |= OPT_NAME;
        fa->file_name = getName(optarg);
        break;
      case 't':
        fa->option_flag |= OPT_TYPE;
        fa->file_type |= getTypeOpt(optarg);
        tparam = optarg;
        break;
      case 'p':
        fa->option_flag |= OPT_PRINT;
        break;

      default:
        if (optopt == 't')
          printf("missing argument to '-type'\n");

        if(optopt == 'n')
          printf("no argument to '-name'\n");
        return -1;
    }
  }

  fa->path_num = argc - optind;
  fa->path = (char**)malloc(sizeof(char*) * fa->path_num);
  char** p = fa->path;
  for (int i = optind; i < argc; i++) {
    int length = strlen(argv[i]) + 1;
    *p = (char*)malloc(sizeof(char) * length);
    snprintf(*p, length, "%s", argv[i]);
    p++;
  }
  return 0;
}

/**
 * @brief 構造体のメンバ変数file_nameに適切なメモリを取り、保存する
 * 
 * @param arg 引数に取った名前 
 * @return char* 保存した名前
 */
char* getName(char* arg)
{
  int length = strlen(arg) + 1;
  char* file_name = (char*)malloc(sizeof(char) * length);
  snprintf(file_name, length, "%s", arg);
  return file_name;
}

/**
 * @brief オプションの文字列を分解し、どのオプションが有効なのかを判定し構造体に保存
 * 各オプションのフラグの位
 * 1: normal file
 * 2: directory
 * 4: block device
 * 8: character device
 * 16: named pipe
 * 32: symbolic link
 * 64: socket
 * 
 * @param opt 引数に取ったオプションの文字列 
 * @return unsigned char 各オプションのフラグを立てた値
 */
unsigned char getTypeOpt(char* opt) {
  unsigned char type_opt = 0;
  char* p = opt;
  while (*p != '\0') {
    switch (*p) {
      case 'f':
        type_opt |= T_ISREG;
        break;

      case 'd':
        type_opt |= T_ISDIR;
        break;

      case 'b':
        type_opt |= T_ISBLK;
        break;

      case 'c':
        type_opt |= T_ISCHR;
        break;

      case 'p':
        type_opt |= T_ISFIFO;
        break;

      case 'l':
        type_opt |= T_ISLNK;
        break;

      case 's':
        type_opt |= T_ISSOCK;
        break;

      default:
        printf("Unknown argument to -type: %c\n", *p);
        exit(EXIT_FAILURE);
        break;
    }
    p++;
  }
  return type_opt;
}