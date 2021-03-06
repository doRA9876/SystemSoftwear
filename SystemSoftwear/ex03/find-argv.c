#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "find-argv.h"

unsigned char getTypeOpt(char* opt);
char* getName(char* name);

struct option longopts[] = {
    {"name", required_argument, NULL, 'n'},
    {"type", required_argument, NULL, 't'},
    {"print", no_argument, NULL, 'p'},
    {0, 0, 0, 0},
};

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