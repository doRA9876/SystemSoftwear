#include <dirent.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#include "find-argv.h"
#include "split.h"

extern void chomp(char* line);
extern int merge_split_shell(word_t*** head_list, char* src);
extern void showWordList(word_t* m);

void changeDirectory(int argc, char* argv[1]);
int runBuiltInCmd(char* cmd_name, int argc, char* argv[]);

// ls command
int ls(int argc, char* argv[]);
void getMode(mode_t mode, char* str);

// find command
int find(int argc, char* argv[]);
int parseCmdArgs(int argc, char* argv[], findArgs_t* fa);
void find_main(char* name, char* base_path, unsigned char type, int opt_bit);
unsigned char getTypeOpt(char* opt);
char* getName(char* name);

char cwd[BUFSIZ];

const char* current_dir = ".";
const char* up_dir = "..";

int main(void) {
  pid_t pid;
  int status;
  char buf[BUFSIZ];

  getcwd(cwd, BUFSIZ);
  if (cwd == NULL) {
    perror("cwd");
    exit(EXIT_FAILURE);
  }

  while (1) {
    printf("%s $", cwd);

    if (fgets(buf, sizeof(buf), stdin) == NULL)
      break;
    chomp(buf);

    word_t** h_list;
    int ik = merge_split_shell(&h_list, buf);

    int argc = 0;
    word_t* p = h_list[0];
    while (p != NULL) {
      if (p->token == ARGUMENT)
        argc++;
      p = p->next;
    }

    char** argv = (char**)malloc(sizeof(char*) * argc);
    p = h_list[0];
    for (int i = 0; i < argc; p = p->next) {
      if (p->token == ARGUMENT) {
        argv[i] = (char*)malloc(sizeof(char) * 32);
        strcpy(argv[i], p->str);
        i++;
      }
    }

    runBuiltInCmd(h_list[0]->str, argc, argv);

    /*
    for (int i = 0; i < ik; i++) {
      word_t* p = h_list[i];
      printf("ik:%d\n", i);
      showWordList(p);

      pid = fork();

      if (pid == 0) {
        execlp(p->str, p->str, (char*)NULL);
        perror(p->str);
        exit(1);
      }
    }

    while (wait(&status) != pid)
      ;
    */
  }
  return 0;
}

void changeDirectory(int argc, char* argv[]) {
  // argv[1]:dest
  char tmp_path[PATH_MAX];

  if (argv[1][0] == '/') {
    // absolute path
    strcpy(tmp_path, argv[1]);
  } else {
    // relative path
    char p[PATH_MAX];
    snprintf(p, PATH_MAX, "%s/%s", cwd, argv[1]);
    realpath(p, tmp_path);
  }

  if (opendir(tmp_path) == NULL) {
    perror(tmp_path);
  } else {
    strcpy(cwd, tmp_path);
  }
}

int runBuiltInCmd(char* cmd_name, int argc, char* argv[]) {
  if (strcmp(cmd_name, "cd") == 0) {
    changeDirectory(argc, argv);
    return 0;
  }

  if (strcmp(cmd_name, "find") == 0) {
    find(argc, argv);
    return 0;
  }

  if (strcmp(cmd_name, "ls") == 0) {
    ls(argc, argv);
    return 0;
  }

  return -1;
}

/********** ls ***************/

int ls(int argc, char* argv[]) {
  char base_path[PATH_MAX];
  char path[PATH_MAX + 1];
  DIR* dp;
  struct dirent* dirp;
  int path_len;

  if (argc > 2) {
    printf("Can't use option.\n");
    return -1;
  }

  if (argc > 1) {
    if (argv[1][0] == '/') {
      // absolute path
      strcpy(base_path, argv[1]);
    } else {
      // relative path
      char p[PATH_MAX];
      snprintf(p, PATH_MAX, "%s/%s", cwd, argv[1]);
      realpath(p, base_path);
    }

    if (opendir(base_path) == NULL) {
      perror(base_path);
    }
  } else {
    strcpy(base_path, cwd);
  }

  dp = opendir(base_path);
  if (dp == NULL) {
    perror(base_path);
    return -1;
  }

  path_len = strlen(base_path);
  if (path_len >= PATH_MAX) {
    return -1;
  }
  strncpy(path, base_path, PATH_MAX);
  if (path[path_len - 1] != '/') {
    path[path_len] = '/';
    path_len++;
    path[path_len] = '\0';
  }

  while ((dirp = readdir(dp)) != NULL) {
    char parallel_path[PATH_MAX];
    snprintf(parallel_path, PATH_MAX, "%s%s", path, dirp->d_name);
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

/****************** end ls ********************/

/***************** find *********************/

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
int find(int argc, char* argv[]) {
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

  /*
  printf("name = %d, %s\n", fa.option_flag & 0b001, fa.file_name);
  printf("type = %d, %d\n", fa.option_flag & 0b010, fa.file_type);
  printf("print = %d\n", fa.option_flag & 0b100);
  */

  char base_path[PATH_MAX];
  strcpy(base_path, cwd);

  for (int i = 0; i < fa.path_num; i++) {
    struct stat buf;
    if (fa.path[i][0] == '/') {
      // absolute path
      strcpy(base_path, fa.path[i]);
    } else {
      // relative path
      char p[PATH_MAX];
      snprintf(p, PATH_MAX, "%s/%s", base_path, fa.path[i]);
      realpath(p, base_path);
    }

    if (!stat(fa.path[i], &buf)) {
      find_main(fa.file_name, base_path, fa.file_type, fa.option_flag);
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
void find_main(char* name,
               char* abs_path,
               unsigned char file_type,
               int opt_bit) {
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
          find_main(name, path_plus, file_type, opt_bit);
      }
    }
  }
  closedir(dp);
}

/**
 * @brief 構造体のメンバ変数file_nameに適切なメモリを取り、保存する
 *
 * @param arg 引数に取った名前
 * @return char* 保存した名前
 */
char* getName(char* arg) {
  int length = strlen(arg) + 1;
  char* file_name = (char*)malloc(sizeof(char) * length);
  snprintf(file_name, length, "%s", arg);
  return file_name;
}

/**
 * @brief
 * オプションの文字列を分解し、どのオプションが有効なのかを判定し構造体に保存
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

int parseCmdArgs(int argc, char* argv[], findArgs_t* fa) {
  opterr = 0;

  fa->option_flag = 0;
  fa->file_type = 0;
  fa->path_num = 0;

  char* tparam = NULL;
  int opt = 0;
  int longindex = 0;


  while ((opt = getopt_long_only(argc, argv, "n:pt:", longopts, &longindex)) !=
         -1) {
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

        if (optopt == 'n')
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

  optind = 1;

  return 0;
}