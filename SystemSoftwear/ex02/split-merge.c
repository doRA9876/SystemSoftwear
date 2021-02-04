#include<stdio.h>
#include<stdlib.h>
#include"split.h"

extern int split_bar(int chunkNum, int chunkLen, char** dar, char* src);
extern int split_shell(word_t** h, char* src);

/**
 * @brief :  シェルのルールに従って、スペース、縦棒で分割。
 * 分割後、その文字列が入力・出力・引数などの情報を持つ構造体を生成し、そのメタデータ配列のポインタを格納。
 * 
 * @param head_list : 線形リストの先頭ポインタword_t*のリスト格納用変数word_t**のポインタ 
 * @param src : シェルの入力文字列
 * @return int : 成功時：分割数、失敗時：0
 */
int merge_split_shell(word_t*** head_list, char* src) {
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

  int ik = split_bar(NF, BUFSIZ, chunk, src);
  *head_list = (word_t**)malloc(sizeof(word_t*) * ik);
  if(!*head_list)
  {
    exit(EXIT_FAILURE);
  }
  
  for (int i = 0; i < ik; i++) {
    word_t* h;
    if (0 < split_shell(&h, chunk[i])) {
      *(*head_list + i) = h;
    }else{
      return -1;
    }
  }
  return ik;
}