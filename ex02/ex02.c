#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split.h"

extern int split(int chunkNum, int chunkLen, char** dar, char* src, int del);
extern int split_bar(int chunkNum, int chunkLen, char** dar, char* src);
extern int merge_split_shell(word_t*** h_list, char* src);
extern int split_shell(word_t** h, char* src);
extern void deleteList(word_t* h);
extern void showWordList(word_t* m);
extern void chomp(char* line);

int show(int nf, char* far[]) {
  int i;
  for (i = 0; i < nf; i++) {
    printf("%3d: %3d |%s|\n", i, (int)strlen(far[i]), far[i]);
  }
}

int main() {
#if 0
 srand(time(NULL));
#endif

  char** chunk = (char**)malloc(sizeof(char*) * NF);
  if (!chunk) {
    exit(3);
  }
  for (int i = 0; i < NF; i++) {
    chunk[i] = (char*)malloc(sizeof(char) * BUFSIZ);
    if (!chunk[i]) {
      exit(5);
    }
    int m = rand() % 36 + 3;
    int j;
    for (j = 0; j < m; j++) {
      chunk[i][j] = 'A' + (rand() % 26);
    }
    chunk[i][j] = '\0';
  }

  // show(NF, chunk);

  FILE* fp;
  char str[BUFSIZ];
/************** split-bar ****************
 * 以下のコマンドを実行
 * make bar
 ****************************************/
#if BAR
  fp = fopen("./bar-test.txt", "r");
  if (fp == NULL) {
    printf("FILE OPEN ERROR\n");
    return -1;
  }

  while (fgets(str, sizeof(str), fp) != NULL) {
    chomp(str);
    printf("str = %s\n", str);
    int ik = split_bar(NF, BUFSIZ, chunk, str);
    printf("ik %d\n", ik);
    show(ik, chunk);
    puts("");
  }
  fclose(fp);
#endif

/************ split-shell***************
 * 以下のコマンドを実行
 * make shell
 ***************************************/
#if SHELL
  fp = fopen("./shell-test.txt", "r");
  if (fp == NULL) {
    printf("FILE OPEN ERROR\n");
    return -1;
  }

  while (fgets(str, sizeof(str), fp) != NULL) {
    chomp(str);
    printf("$%s\n", str);
    word_t* h;
    if (0 < split_shell(&h, str)) {
      showWordList(h);
    }
    printf("\n");
    deleteList(h);
  }
#endif

/*********** split-merge *********
 * 以下のコマンドを実行
 * make merge
 **************************************/
#if MERGE
  fp = fopen("./merge-test.txt", "r");
  if (fp == NULL) {
    printf("FILE OPEN ERROR\n");
    return -1;
  }

  while (fgets(str, sizeof(str), fp) != NULL) {
    chomp(str);
    printf("$%s\n", str);
    word_t** h_list;
    int ik = merge_split_shell(&h_list, str);
    for (int i = 0; i < ik; i++) {
      showWordList(h_list[i]);
      deleteList(h_list[i]);
      printf("------\n");
    }
    free(h_list);
    printf("\n");
  }
  fclose(fp);
#endif

  free(chunk);

  return 0;
}
